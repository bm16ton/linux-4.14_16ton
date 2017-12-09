/*
 * VirtualBox Guest Shared Folders support: Regular file inode and file ops.
 *
 * Copyright (C) 2006-2016 Oracle Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*
 * Limitations: only COW memory mapping is supported
 */

#include <linux/sizes.h>
#include <linux/vbox_utils.h>
#include "vfsmod.h"

static void *alloc_bounce_buffer(size_t *tmp_sizep, size_t xfer_size)
{
	size_t tmp_size;
	void *tmp;

	/* try for big first. */
	tmp_size = PAGE_ALIGN(xfer_size);
	if (tmp_size > SZ_16K)
		tmp_size = SZ_16K;
	tmp = kmalloc(tmp_size, GFP_KERNEL);
	if (!tmp) {
		/* fall back on a page sized buffer. */
		tmp = kmalloc(PAGE_SIZE, GFP_KERNEL);
		if (!tmp)
			return NULL;
		tmp_size = PAGE_SIZE;
	}

	*tmp_sizep = tmp_size;
	return tmp;
}

/* fops */
static int sf_reg_read_aux(struct sf_glob_info *sf_g, struct sf_reg_info *sf_r,
			   void *buf, u32 *nread, u64 pos)
{
	int rc;

	rc = vboxsf_read(sf_g->root, sf_r->handle, pos, nread, buf);
	if (rc < 0)
		return -EPROTO;

	return 0;
}

static int sf_reg_write_aux(struct sf_glob_info *sf_g, struct sf_reg_info *sf_r,
			    void *buf, u32 *nwritten, u64 pos)
{
	int rc;

	/** @todo bird: see sf_reg_read_aux */
	rc = vboxsf_write(sf_g->root, sf_r->handle, pos, nwritten, buf);
	if (rc < 0)
		return -EPROTO;

	return 0;
}

/**
 * Read from a regular file.
 *
 * @param file          the file
 * @param buf           the buffer
 * @param size          length of the buffer
 * @param off           offset within the file
 * @returns the number of read bytes on success, Linux error code otherwise
 */
static ssize_t sf_reg_read(struct file *file, char *buf, size_t size,
			   loff_t *off)
{
	int err;
	void *tmp;
	size_t tmp_size;
	size_t left = size;
	ssize_t total_bytes_read = 0;
	struct inode *inode = GET_F_DENTRY(file)->d_inode;
	struct sf_glob_info *sf_g = GET_GLOB_INFO(inode->i_sb);
	struct sf_reg_info *sf_r = file->private_data;
	loff_t pos = *off;

	if (!S_ISREG(inode->i_mode))
		return -EINVAL;

	/** XXX Check read permission according to inode->i_mode! */

	if (!size)
		return 0;

	tmp = alloc_bounce_buffer(&tmp_size, size);
	if (!tmp)
		return -ENOMEM;

	while (left) {
		u32 to_read, nread;

		to_read = tmp_size;
		if (to_read > left)
			to_read = left;

		nread = to_read;

		err = sf_reg_read_aux(sf_g, sf_r, tmp, &nread, pos);
		if (err)
			goto fail;

		if (copy_to_user(buf, tmp, nread)) {
			err = -EFAULT;
			goto fail;
		}

		pos += nread;
		left -= nread;
		buf += nread;
		total_bytes_read += nread;
		if (nread != to_read)
			break;
	}

	*off += total_bytes_read;
	kfree(tmp);
	return total_bytes_read;

fail:
	kfree(tmp);
	return err;
}

/**
 * Write to a regular file.
 *
 * @param file          the file
 * @param buf           the buffer
 * @param size          length of the buffer
 * @param off           offset within the file
 * @returns the number of written bytes on success, Linux error code otherwise
 */
static ssize_t sf_reg_write(struct file *file, const char *buf, size_t size,
			    loff_t *off)
{
	int err;
	void *tmp;
	size_t tmp_size;
	size_t left = size;
	ssize_t total_bytes_written = 0;
	struct inode *inode = GET_F_DENTRY(file)->d_inode;
	struct sf_inode_info *sf_i = GET_INODE_INFO(inode);
	struct sf_glob_info *sf_g = GET_GLOB_INFO(inode->i_sb);
	struct sf_reg_info *sf_r = file->private_data;
	loff_t pos;

	if (!S_ISREG(inode->i_mode))
		return -EINVAL;

	pos = *off;
	if (file->f_flags & O_APPEND) {
		pos = inode->i_size;
		*off = pos;
	}

    /** XXX Check write permission according to inode->i_mode! */

	if (!size)
		return 0;

	tmp = alloc_bounce_buffer(&tmp_size, size);
	if (!tmp)
		return -ENOMEM;

	while (left) {
		u32 to_write, nwritten;

		to_write = tmp_size;
		if (to_write > left)
			to_write = (u32) left;

		nwritten = to_write;

		if (copy_from_user(tmp, buf, to_write)) {
			err = -EFAULT;
			goto fail;
		}

		err = sf_reg_write_aux(sf_g, sf_r, tmp, &nwritten, pos);
		if (err)
			goto fail;

		pos += nwritten;
		left -= nwritten;
		buf += nwritten;
		total_bytes_written += nwritten;
		if (nwritten != to_write)
			break;
	}

	*off += total_bytes_written;
	if (*off > inode->i_size)
		inode->i_size = *off;

	sf_i->force_restat = 1;
	kfree(tmp);
	return total_bytes_written;

fail:
	kfree(tmp);
	return err;
}

/**
 * Open a regular file.
 *
 * @param inode         the inode
 * @param file          the file
 * @returns 0 on success, Linux error code otherwise
 */
static int sf_reg_open(struct inode *inode, struct file *file)
{
	int rc, rc_linux = 0;
	struct sf_glob_info *sf_g = GET_GLOB_INFO(inode->i_sb);
	struct sf_inode_info *sf_i = GET_INODE_INFO(inode);
	struct sf_reg_info *sf_r;
	struct shfl_createparms params = {};

	sf_r = kmalloc(sizeof(*sf_r), GFP_KERNEL);
	if (!sf_r)
		return -ENOMEM;

	/* Already open? */
	if (sf_i->handle != SHFL_HANDLE_NIL) {
		/*
		 * This inode was created with sf_create_aux(). Check the
		 * create_flags: O_CREAT, O_TRUNC: inherent true (file was
		 * just created).
		 * Not sure about the access flags (SHFL_CF_ACCESS_*).
		 */
		sf_i->force_restat = 1;
		sf_r->handle = sf_i->handle;
		sf_i->handle = SHFL_HANDLE_NIL;
		sf_i->file = file;
		file->private_data = sf_r;
		return 0;
	}

	params.handle = SHFL_HANDLE_NIL;

	/*
	 * We check the value of params.handle afterwards to find out if
	 * the call succeeded or failed, as the API does not seem to cleanly
	 * distinguish error and informational messages.
	 *
	 * Furthermore, we must set params.handle to SHFL_HANDLE_NIL to
	 * make the shared folders host service use our mode parameter.
	 */
	if (file->f_flags & O_CREAT) {
		params.create_flags |= SHFL_CF_ACT_CREATE_IF_NEW;
		/*
		 * We ignore O_EXCL, as the Linux kernel seems to call create
		 * beforehand itself, so O_EXCL should always fail.
		 */
		if (file->f_flags & O_TRUNC)
			params.create_flags |= SHFL_CF_ACT_OVERWRITE_IF_EXISTS;
		else
			params.create_flags |= SHFL_CF_ACT_OPEN_IF_EXISTS;
	} else {
		params.create_flags |= SHFL_CF_ACT_FAIL_IF_NEW;
		if (file->f_flags & O_TRUNC)
			params.create_flags |= SHFL_CF_ACT_OVERWRITE_IF_EXISTS;
	}

	switch (file->f_flags & O_ACCMODE) {
	case O_RDONLY:
		params.create_flags |= SHFL_CF_ACCESS_READ;
		break;

	case O_WRONLY:
		params.create_flags |= SHFL_CF_ACCESS_WRITE;
		break;

	case O_RDWR:
		params.create_flags |= SHFL_CF_ACCESS_READWRITE;
		break;

	default:
		WARN_ON(1);
	}

	if (file->f_flags & O_APPEND)
		params.create_flags |= SHFL_CF_ACCESS_APPEND;

	params.info.attr.mode = inode->i_mode;
	rc = vboxsf_create(sf_g->root, sf_i->path, &params);
	if (rc < 0) {
		kfree(sf_r);
		return vbg_status_code_to_errno(rc);
	}

	if (params.handle == SHFL_HANDLE_NIL) {
		switch (params.result) {
		case SHFL_PATH_NOT_FOUND:
		case SHFL_FILE_NOT_FOUND:
			rc_linux = -ENOENT;
			break;
		case SHFL_FILE_EXISTS:
			rc_linux = -EEXIST;
			break;
		default:
			break;
		}
	}

	sf_i->force_restat = 1;
	sf_r->handle = params.handle;
	sf_i->file = file;
	file->private_data = sf_r;
	return rc_linux;
}

/**
 * Close a regular file.
 *
 * @param inode         the inode
 * @param file          the file
 * @returns 0 on success, Linux error code otherwise
 */
static int sf_reg_release(struct inode *inode, struct file *file)
{
	struct sf_reg_info *sf_r;
	struct sf_glob_info *sf_g;
	struct sf_inode_info *sf_i = GET_INODE_INFO(inode);

	sf_g = GET_GLOB_INFO(inode->i_sb);
	sf_r = file->private_data;

	filemap_write_and_wait(inode->i_mapping);

	vboxsf_close(sf_g->root, sf_r->handle);

	kfree(sf_r);
	sf_i->file = NULL;
	sf_i->handle = SHFL_HANDLE_NIL;
	file->private_data = NULL;
	return 0;
}

static int sf_reg_fault(struct vm_fault *vmf)
{
	struct page *page;
	char *buf;
	loff_t off;
	u32 nread = PAGE_SIZE;
	int err;
	struct vm_area_struct *vma = vmf->vma;
	struct file *file = vma->vm_file;
	struct inode *inode = GET_F_DENTRY(file)->d_inode;
	struct sf_glob_info *sf_g = GET_GLOB_INFO(inode->i_sb);
	struct sf_reg_info *sf_r = file->private_data;

	if (vmf->pgoff > vma->vm_end)
		return VM_FAULT_SIGBUS;

	/*
	 * Don't use GFP_HIGHUSER as long as sf_reg_read_aux() calls
	 * vboxsf_read() which works on virtual addresses.
	 */
	page = alloc_page(GFP_USER);
	if (!page)
		return VM_FAULT_OOM;

	buf = kmap(page);
	off = (vmf->pgoff << PAGE_SHIFT);
	err = sf_reg_read_aux(sf_g, sf_r, buf, &nread, off);
	if (err) {
		kunmap(page);
		put_page(page);
		return VM_FAULT_SIGBUS;
	}

	if (!nread)
		clear_user_page(page_address(page), vmf->pgoff, page);
	else
		memset(buf + nread, 0, PAGE_SIZE - nread);

	flush_dcache_page(page);
	kunmap(page);
	vmf->page = page;
	return 0;
}

static const struct vm_operations_struct sf_vma_ops = {
	.fault = sf_reg_fault
};

static int sf_reg_mmap(struct file *file, struct vm_area_struct *vma)
{
	if (vma->vm_flags & VM_SHARED)
		return -EINVAL;

	vma->vm_ops = &sf_vma_ops;
	return 0;
}

const struct file_operations sf_reg_fops = {
	.read = sf_reg_read,
	.open = sf_reg_open,
	.write = sf_reg_write,
	.release = sf_reg_release,
	.mmap = sf_reg_mmap,
	.splice_read = generic_file_splice_read,
	.read_iter = generic_file_read_iter,
	.write_iter = generic_file_write_iter,
	.fsync = noop_fsync,
	.llseek = generic_file_llseek,
};

const struct inode_operations sf_reg_iops = {
	.getattr = sf_getattr,
	.setattr = sf_setattr
};

static int sf_readpage(struct file *file, struct page *page)
{
	struct inode *inode = GET_F_DENTRY(file)->d_inode;
	struct sf_glob_info *sf_g = GET_GLOB_INFO(inode->i_sb);
	struct sf_reg_info *sf_r = file->private_data;
	u32 nread = PAGE_SIZE;
	char *buf;
	loff_t off = ((loff_t) page->index) << PAGE_SHIFT;
	int err;

	buf = kmap(page);
	err = sf_reg_read_aux(sf_g, sf_r, buf, &nread, off);
	if (err) {
		kunmap(page);
		if (PageLocked(page))
			unlock_page(page);
		return err;
	}
	memset(&buf[nread], 0, PAGE_SIZE - nread);
	flush_dcache_page(page);
	kunmap(page);
	SetPageUptodate(page);
	unlock_page(page);
	return 0;
}

static int sf_writepage(struct page *page, struct writeback_control *wbc)
{
	struct address_space *mapping = page->mapping;
	struct inode *inode = mapping->host;
	struct sf_glob_info *sf_g = GET_GLOB_INFO(inode->i_sb);
	struct sf_inode_info *sf_i = GET_INODE_INFO(inode);
	struct file *file = sf_i->file;
	struct sf_reg_info *sf_r = file->private_data;
	char *buf;
	u32 nwritten = PAGE_SIZE;
	int end_index = inode->i_size >> PAGE_SHIFT;
	loff_t off = ((loff_t) page->index) << PAGE_SHIFT;
	int err;

	if (page->index >= end_index)
		nwritten = inode->i_size & (PAGE_SIZE - 1);

	buf = kmap(page);

	err = sf_reg_write_aux(sf_g, sf_r, buf, &nwritten, off);
	if (err < 0) {
		ClearPageUptodate(page);
		goto out;
	}

	if (off > inode->i_size)
		inode->i_size = off;

	if (PageError(page))
		ClearPageError(page);
	err = 0;

out:
	kunmap(page);

	unlock_page(page);
	return err;
}

int sf_write_begin(struct file *file, struct address_space *mapping, loff_t pos,
		   unsigned int len, unsigned int flags, struct page **pagep,
		   void **fsdata)
{
	return simple_write_begin(file, mapping, pos, len, flags, pagep,
				  fsdata);
}

int sf_write_end(struct file *file, struct address_space *mapping, loff_t pos,
		 unsigned int len, unsigned int copied, struct page *page,
		 void *fsdata)
{
	struct inode *inode = mapping->host;
	struct sf_glob_info *sf_g = GET_GLOB_INFO(inode->i_sb);
	struct sf_reg_info *sf_r = file->private_data;
	unsigned int from = pos & (PAGE_SIZE - 1);
	u32 nwritten = len;
	void *buf;
	int err;

	buf = kmap(page);
	err = sf_reg_write_aux(sf_g, sf_r, buf + from, &nwritten, pos);
	kunmap(page);

	if (!PageUptodate(page) && err == PAGE_SIZE)
		SetPageUptodate(page);

	if (err >= 0) {
		pos += nwritten;
		if (pos > inode->i_size)
			inode->i_size = pos;
	}

	unlock_page(page);
	put_page(page);

	return nwritten;
}

const struct address_space_operations sf_reg_aops = {
	.readpage = sf_readpage,
	.writepage = sf_writepage,
	.write_begin = sf_write_begin,
	.write_end = sf_write_end,
};

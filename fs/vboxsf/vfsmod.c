/*
 * VirtualBox Guest Shared Folders support: Virtual File System.
 *
 * Module initialization/finalization
 * File system registration/deregistration
 * Superblock reading
 * Few utility functions
 *
 * Copyright (C) 2006-2016 Oracle Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/**
 * @note Anyone wishing to make changes here might wish to take a look at
 *  http://www.atnf.csiro.au/people/rgooch/linux/vfs.txt
 * which seems to be the closest there is to official documentation on
 * writing filesystem drivers for Linux.
 */

#include <linux/module.h>
#include <linux/nls.h>
#include <linux/vbox_utils.h>
#include <linux/vbsfmount.h>
#include "vfsmod.h"

MODULE_DESCRIPTION("Oracle VM VirtualBox Module for Host File System Access");
MODULE_AUTHOR("Oracle Corporation");
MODULE_LICENSE("GPL");
MODULE_ALIAS_FS("vboxsf");

/* forward declarations */
static struct super_operations sf_super_ops;

/* allocate global info, try to map host share */
static int sf_glob_alloc(struct vbsf_mount_info_new *info,
			 struct sf_glob_info **sf_gp)
{
	int err, rc;
	size_t name_len, str_len;
	struct sf_glob_info *sf_g;
	struct shfl_string *str_name = NULL;
#ifdef CONFIG_NLS_DEFAULT
	const char *nls_name = CONFIG_NLS_DEFAULT;
#else
	const char *nls_name = "";
#endif

	if (info->nullchar != '\0' ||
	    info->signature[0] != VBSF_MOUNT_SIGNATURE_BYTE_0 ||
	    info->signature[1] != VBSF_MOUNT_SIGNATURE_BYTE_1 ||
	    info->signature[2] != VBSF_MOUNT_SIGNATURE_BYTE_2)
		return -EINVAL;

	sf_g = kzalloc(sizeof(*sf_g), GFP_KERNEL);
	if (!sf_g)
		return -ENOMEM;

	info->name[sizeof(info->name) - 1] = 0;
	info->nls_name[sizeof(info->nls_name) - 1] = 0;

	name_len = strlen(info->name);
	str_len = offsetof(struct shfl_string, string.utf8) + name_len + 1;

	str_name = kmalloc(str_len, GFP_KERNEL);
	if (!str_name) {
		err = -ENOMEM;
		goto fail;
	}

	str_name->length = name_len;
	str_name->size = name_len + 1;
	memcpy(str_name->string.utf8, info->name, name_len + 1);

	if (info->nls_name[0])
		nls_name = info->nls_name;

	/* Load nls if not utf8 */
	if (nls_name[0] && strcmp(nls_name, "utf8") != 0) {
		sf_g->nls = load_nls(info->nls_name);
		if (!sf_g->nls) {
			err = -EINVAL;
			goto fail;
		}
	} else {
		sf_g->nls = NULL;
	}

	rc = vboxsf_map_folder(str_name, &sf_g->root);
	if (rc < 0) {
		err = -EPROTO;
		goto fail;
	}

	kfree(str_name);

	sf_g->ttl = info->ttl;
	sf_g->uid = info->uid;
	sf_g->gid = info->gid;

	if ((size_t)info->length >= sizeof(struct vbsf_mount_info_new)) {
		/* new fields */
		sf_g->dmode = info->dmode;
		sf_g->fmode = info->fmode;
		sf_g->dmask = info->dmask;
		sf_g->fmask = info->fmask;
	} else {
		sf_g->dmode = ~0;
		sf_g->fmode = ~0;
	}

	*sf_gp = sf_g;
	return 0;

fail:
	if (sf_g->nls)
		unload_nls(sf_g->nls);

	kfree(str_name);
	kfree(sf_g);

	return err;
}

/* unmap the share and free global info [sf_g] */
static void sf_glob_free(struct sf_glob_info *sf_g)
{
	vboxsf_unmap_folder(sf_g->root);

	if (sf_g->nls)
		unload_nls(sf_g->nls);

	kfree(sf_g);
}

/**
 * This is called when vfs mounts the fs and wants to read the super_block.
 *
 * calls [sf_glob_alloc] to map the folder and allocate global
 * information structure.
 *
 * initializes [sb], initializes root inode and dentry.
 *
 * should respect [flags]
 */
static int sf_read_super(struct super_block *sb, void *data, int flags)
{
	int err;
	struct dentry *droot;
	struct inode *iroot;
	struct sf_inode_info *sf_i;
	struct sf_glob_info *sf_g;
	struct shfl_fsobjinfo fsinfo;
	struct vbsf_mount_info_new *info;

	if (!data)
		return -EINVAL;

	info = data;

	if (flags & MS_REMOUNT)
		return -EINVAL;

	err = sf_glob_alloc(info, &sf_g);
	if (err)
		goto fail0;

	sf_i = kmalloc(sizeof(*sf_i), GFP_KERNEL);
	if (!sf_i) {
		err = -ENOMEM;
		goto fail1;
	}

	sf_i->handle = SHFL_HANDLE_NIL;
	sf_i->path = kmalloc(sizeof(struct shfl_string) + 1, GFP_KERNEL);
	if (!sf_i->path) {
		err = -ENOMEM;
		goto fail2;
	}

	sf_i->path->length = 1;
	sf_i->path->size = 2;
	sf_i->path->string.utf8[0] = '/';
	sf_i->path->string.utf8[1] = 0;
	sf_i->force_reread = 0;

	err = sf_stat(__func__, sf_g, sf_i->path, &fsinfo, 0);
	if (err)
		goto fail3;

	sb->s_magic = 0xface;
	sb->s_blocksize = 1024;
	sb->s_maxbytes = MAX_LFS_FILESIZE;
	sb->s_op = &sf_super_ops;

	iroot = iget_locked(sb, 0);
	if (!iroot) {
		err = -ENOMEM;	/* XXX */
		goto fail3;
	}

	sf_init_inode(sf_g, iroot, &fsinfo);
	SET_INODE_INFO(iroot, sf_i);

	unlock_new_inode(iroot);

	droot = d_make_root(iroot);
	if (!droot) {
		err = -ENOMEM;
		goto fail3;
	}

	sb->s_root = droot;
	SET_GLOB_INFO(sb, sf_g);
	return 0;

fail3:
	kfree(sf_i->path);

fail2:
	kfree(sf_i);

fail1:
	sf_glob_free(sf_g);

fail0:
	return err;
}

/*
 * This is called when vfs is about to destroy the [inode]. all
 * resources associated with this [inode] must be cleared here.
 */
static void sf_evict_inode(struct inode *inode)
{
	struct sf_inode_info *sf_i;

	truncate_inode_pages(&inode->i_data, 0);
	clear_inode(inode);

	sf_i = GET_INODE_INFO(inode);
	if (!sf_i)
		return;

	kfree(sf_i->path);
	kfree(sf_i);
	SET_INODE_INFO(inode, NULL);
}

/*
 * vfs is done with [sb] (umount called) call [sf_glob_free] to unmap
 * the folder and free [sf_g]
 */
static void sf_put_super(struct super_block *sb)
{
	struct sf_glob_info *sf_g;

	sf_g = GET_GLOB_INFO(sb);
	sf_glob_free(sf_g);
}

static int sf_statfs(struct dentry *dentry, struct kstatfs *stat)
{
	struct super_block *sb = dentry->d_inode->i_sb;

	return sf_get_volume_info(sb, stat);
}

static int sf_remount_fs(struct super_block *sb, int *flags, char *data)
{
	struct sf_glob_info *sf_g;
	struct sf_inode_info *sf_i;
	struct inode *iroot;
	struct shfl_fsobjinfo fsinfo;
	int err;

	sf_g = GET_GLOB_INFO(sb);
	if (data && data[0] != 0) {
		struct vbsf_mount_info_new *info =
		    (struct vbsf_mount_info_new *)data;
		if (info->signature[0] == VBSF_MOUNT_SIGNATURE_BYTE_0
		    && info->signature[1] == VBSF_MOUNT_SIGNATURE_BYTE_1
		    && info->signature[2] == VBSF_MOUNT_SIGNATURE_BYTE_2) {
			sf_g->uid = info->uid;
			sf_g->gid = info->gid;
			sf_g->ttl = info->ttl;
			sf_g->dmode = info->dmode;
			sf_g->fmode = info->fmode;
			sf_g->dmask = info->dmask;
			sf_g->fmask = info->fmask;
		}
	}

	iroot = ilookup(sb, 0);
	if (!iroot)
		return -ENOENT;

	sf_i = GET_INODE_INFO(iroot);
	err = sf_stat(__func__, sf_g, sf_i->path, &fsinfo, 0);
	sf_init_inode(sf_g, iroot, &fsinfo);

	return 0;
}

static struct super_operations sf_super_ops = {
	.evict_inode = sf_evict_inode,
	.put_super = sf_put_super,
	.statfs = sf_statfs,
	.remount_fs = sf_remount_fs
};

static struct dentry *sf_mount(struct file_system_type *fs_type, int flags,
			       const char *dev_name, void *data)
{
	return mount_nodev(fs_type, flags, data, sf_read_super);
}

static struct file_system_type vboxsf_fs_type = {
	.owner = THIS_MODULE,
	.name = "vboxsf",
	.mount = sf_mount,
	.kill_sb = kill_anon_super
};

static int follow_symlinks;
module_param(follow_symlinks, int, 0444);
MODULE_PARM_DESC(follow_symlinks,
		 "Let host resolve symlinks rather than showing them");

/* Module initialization/finalization handlers */
static int __init init(void)
{
	int rc, err;

	if (sizeof(struct vbsf_mount_info_new) > PAGE_SIZE) {
		vbg_err("vboxsf: Mount information structure is too large %zd; Must be less than or equal to %ld\n",
			sizeof(struct vbsf_mount_info_new), PAGE_SIZE);
		return -EINVAL;
	}

	err = register_filesystem(&vboxsf_fs_type);
	if (err)
		return err;

	err = vboxsf_connect();
	if (err) {
		vbg_err("vboxsf_connect error %d\n", err);
		goto fail1;
	}

	rc = vboxsf_set_utf8();
	if (rc < 0) {
		vbg_err("vboxsf_setutf8 failed, rc=%d\n", rc);
		err = -EPROTO;
		goto fail2;
	}

	if (!follow_symlinks) {
		rc = vboxsf_set_symlinks();
		if (rc < 0) {
			vbg_warn("vboxsf: Host unable to show symlinks, rc=%d\n",
				 rc);
		}
	}

	return 0;

fail2:
	vboxsf_disconnect();

fail1:
	unregister_filesystem(&vboxsf_fs_type);
	return err;
}

static void __exit fini(void)
{
	vboxsf_disconnect();
	unregister_filesystem(&vboxsf_fs_type);
}

module_init(init);
module_exit(fini);

/*
 * Copyright (C) 2006-2016 Oracle Corporation
 *
 * Wrapper functions for the shfl host calls.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * The contents of this file may alternatively be used under the terms
 * of the Common Development and Distribution License Version 1.0
 * (CDDL) only, in which case the provisions of the CDDL are applicable
 * instead of those of the GPL.
 *
 * You may elect to license modified versions of this file under the
 * terms and conditions of either the GPL or the CDDL or both.
 */

#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/vbox_err.h>
#include <linux/vbox_utils.h>
#include "vboxsf_wrappers.h"

static u32 vboxsf_client_id;

int vboxsf_connect(void)
{
	struct vbg_dev *gdev;
	struct vmmdev_hgcm_service_location loc;
	int err, rc;

	loc.type = VMMDEV_HGCM_LOC_LOCALHOST_EXISTING;
	strcpy(loc.u.localhost.service_name, "VBoxSharedFolders");

	gdev = vbg_get_gdev();
	if (IS_ERR(gdev))
		return VERR_NOT_SUPPORTED;	/* No guest-device */

	err = vbg_hgcm_connect(gdev, &loc, &vboxsf_client_id, &rc);
	vbg_put_gdev(gdev);

	return err ? err : vbg_status_code_to_errno(rc);
}

void vboxsf_disconnect(void)
{
	struct vbg_dev *gdev;
	int rc;

	gdev = vbg_get_gdev();
	if (IS_ERR(gdev))
		return;   /* guest-device is gone, already disconnected */

	vbg_hgcm_disconnect(gdev, vboxsf_client_id, &rc);
	vbg_put_gdev(gdev);
}

static int vboxsf_hgcm_call(u32 function, void *parms, u32 parm_count)
{
	struct vbg_dev *gdev;
	int ret, vbox_status;

	gdev = vbg_get_gdev();
	if (IS_ERR(gdev))
		return VERR_DEV_IO_ERROR; /* guest-dev removed underneath us */

	ret = vbg_hgcm_call(gdev, vboxsf_client_id, function, U32_MAX,
			    parms, parm_count, &vbox_status);
	vbg_put_gdev(gdev);

	if (ret < 0)
		return ret == -ENOMEM ? VERR_NO_MEMORY : VERR_DEV_IO_ERROR;

	return vbox_status;
}

int vboxsf_query_mappings(struct shfl_mapping mappings[], u32 *mappings_len)
{
	int rc;
	struct shfl_query_mappings parms;

	parms.flags.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.flags.u.value32 = SHFL_MF_UCS2;

	parms.number_of_mappings.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.number_of_mappings.u.value32 = *mappings_len;

	parms.mappings.type = VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL;
	parms.mappings.u.pointer.size = sizeof(struct shfl_mapping) *
					*mappings_len;
	parms.mappings.u.pointer.u.linear_addr = (uintptr_t)&mappings[0];

	rc = vboxsf_hgcm_call(SHFL_FN_QUERY_MAPPINGS, &parms,
			      SHFL_CPARMS_QUERY_MAPPINGS);
	if (rc >= 0)
		*mappings_len = parms.number_of_mappings.u.value32;

	return rc;
}

int vboxsf_query_mapname(SHFLROOT root, struct shfl_string *string, u32 size)
{
	struct shfl_query_map_name parms;

	parms.root.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.root.u.value32 = root;

	parms.name.type = VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL;
	parms.name.u.pointer.size = size;
	parms.name.u.pointer.u.linear_addr = (uintptr_t)string;

	return vboxsf_hgcm_call(SHFL_FN_QUERY_MAP_NAME, &parms,
				SHFL_CPARMS_QUERY_MAP_NAME);
}

int vboxsf_map_folder(struct shfl_string *folder_name, SHFLROOT *root)
{
	int rc;
	struct shfl_map_folder parms;

	parms.path.type = VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL;
	parms.path.u.pointer.size = shfl_string_buf_size(folder_name);
	parms.path.u.pointer.u.linear_addr = (uintptr_t)folder_name;

	parms.root.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.root.u.value32 = 0;

	parms.delimiter.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.delimiter.u.value32 = '/';

	parms.case_sensitive.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.case_sensitive.u.value32 = 1;

	rc = vboxsf_hgcm_call(SHFL_FN_MAP_FOLDER, &parms,
			      SHFL_CPARMS_MAP_FOLDER);
	if (rc >= 0)
		*root = parms.root.u.value32;
	else if (rc == VERR_NOT_IMPLEMENTED)
		vbg_err("%s: Error host is too old\n", __func__);

	return rc;
}

int vboxsf_unmap_folder(SHFLROOT root)
{
	struct shfl_unmap_folder parms;

	parms.root.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.root.u.value32 = root;

	return vboxsf_hgcm_call(SHFL_FN_UNMAP_FOLDER, &parms,
				SHFL_CPARMS_UNMAP_FOLDER);
}

int vboxsf_create(SHFLROOT root, struct shfl_string *parsed_path,
		  struct shfl_createparms *create_parms)
{
	/** @todo copy buffers to physical or mapped memory. */
	struct shfl_create parms;

	parms.root.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.root.u.value32 = root;

	parms.path.type = VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL;
	parms.path.u.pointer.size = shfl_string_buf_size(parsed_path);
	parms.path.u.pointer.u.linear_addr = (uintptr_t)parsed_path;

	parms.parms.type = VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL;
	parms.parms.u.pointer.size = sizeof(struct shfl_createparms);
	parms.parms.u.pointer.u.linear_addr = (uintptr_t)create_parms;

	return vboxsf_hgcm_call(SHFL_FN_CREATE, &parms, SHFL_CPARMS_CREATE);
}

int vboxsf_close(SHFLROOT root, SHFLHANDLE file)
{
	struct shfl_close parms;

	parms.root.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.root.u.value32 = root;

	parms.handle.type = VMMDEV_HGCM_PARM_TYPE_64BIT;
	parms.handle.u.value64 = file;

	return vboxsf_hgcm_call(SHFL_FN_CLOSE, &parms, SHFL_CPARMS_CLOSE);
}

int vboxsf_remove(SHFLROOT root, struct shfl_string *parsed_path, u32 flags)
{
	struct shfl_remove parms;

	parms.root.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.root.u.value32 = root;

	parms.path.type = VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL_IN;
	parms.path.u.pointer.size = shfl_string_buf_size(parsed_path);
	parms.path.u.pointer.u.linear_addr = (uintptr_t)parsed_path;

	parms.flags.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.flags.u.value32 = flags;

	return vboxsf_hgcm_call(SHFL_FN_REMOVE, &parms, SHFL_CPARMS_REMOVE);
}

int vboxsf_rename(SHFLROOT root, struct shfl_string *src_path,
		  struct shfl_string *dest_path, u32 flags)
{
	struct shfl_rename parms;

	parms.root.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.root.u.value32 = root;

	parms.src.type = VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL_IN;
	parms.src.u.pointer.size = shfl_string_buf_size(src_path);
	parms.src.u.pointer.u.linear_addr = (uintptr_t)src_path;

	parms.dest.type = VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL_IN;
	parms.dest.u.pointer.size = shfl_string_buf_size(dest_path);
	parms.dest.u.pointer.u.linear_addr = (uintptr_t)dest_path;

	parms.flags.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.flags.u.value32 = flags;

	return vboxsf_hgcm_call(SHFL_FN_RENAME, &parms, SHFL_CPARMS_RENAME);
}

int vboxsf_read(SHFLROOT root, SHFLHANDLE file, u64 offset,
		u32 *buf_len, u8 *buf)
{
	int rc;
	struct shfl_read parms;

	parms.root.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.root.u.value32 = root;

	parms.handle.type = VMMDEV_HGCM_PARM_TYPE_64BIT;
	parms.handle.u.value64 = file;
	parms.offset.type = VMMDEV_HGCM_PARM_TYPE_64BIT;
	parms.offset.u.value64 = offset;
	parms.cb.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.cb.u.value32 = *buf_len;
	parms.buffer.type = VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL_OUT;
	parms.buffer.u.pointer.size = *buf_len;
	parms.buffer.u.pointer.u.linear_addr = (uintptr_t)buf;

	rc = vboxsf_hgcm_call(SHFL_FN_READ, &parms, SHFL_CPARMS_READ);
	if (rc >= 0)
		*buf_len = parms.cb.u.value32;

	return rc;
}

int vboxsf_write(SHFLROOT root, SHFLHANDLE file, u64 offset,
		 u32 *buf_len, u8 *buf)
{
	int rc;
	struct shfl_write parms;

	parms.root.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.root.u.value32 = root;

	parms.handle.type = VMMDEV_HGCM_PARM_TYPE_64BIT;
	parms.handle.u.value64 = file;
	parms.offset.type = VMMDEV_HGCM_PARM_TYPE_64BIT;
	parms.offset.u.value64 = offset;
	parms.cb.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.cb.u.value32 = *buf_len;
	parms.buffer.type = VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL_IN;
	parms.buffer.u.pointer.size = *buf_len;
	parms.buffer.u.pointer.u.linear_addr = (uintptr_t)buf;

	rc = vboxsf_hgcm_call(SHFL_FN_WRITE, &parms, SHFL_CPARMS_WRITE);
	if (rc >= 0)
		*buf_len = parms.cb.u.value32;

	return rc;
}

int vboxsf_flush(SHFLROOT root, SHFLHANDLE file)
{
	struct shfl_flush parms;

	parms.root.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.root.u.value32 = root;

	parms.handle.type = VMMDEV_HGCM_PARM_TYPE_64BIT;
	parms.handle.u.value64 = file;

	return vboxsf_hgcm_call(SHFL_FN_FLUSH, &parms, SHFL_CPARMS_FLUSH);
}

int vboxsf_dirinfo(SHFLROOT root, SHFLHANDLE file,
		   struct shfl_string *parsed_path, u32 flags, u32 index,
		   u32 *buf_len, struct shfl_dirinfo *buf, u32 *file_count)
{
	int rc;
	struct shfl_list parms;

	parms.root.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.root.u.value32 = root;

	parms.handle.type = VMMDEV_HGCM_PARM_TYPE_64BIT;
	parms.handle.u.value64 = file;
	parms.flags.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.flags.u.value32 = flags;
	parms.cb.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.cb.u.value32 = *buf_len;
	parms.path.type = VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL_IN;
	parms.path.u.pointer.size =
	    parsed_path ? shfl_string_buf_size(parsed_path) : 0;
	parms.path.u.pointer.u.linear_addr = (uintptr_t)parsed_path;

	parms.buffer.type = VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL_OUT;
	parms.buffer.u.pointer.size = *buf_len;
	parms.buffer.u.pointer.u.linear_addr = (uintptr_t)buf;

	parms.resume_point.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.resume_point.u.value32 = index;
	parms.file_count.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.file_count.u.value32 = 0;	/* out parameter only */

	rc = vboxsf_hgcm_call(SHFL_FN_LIST, &parms, SHFL_CPARMS_LIST);

	*buf_len = parms.cb.u.value32;
	*file_count = parms.file_count.u.value32;

	return rc;
}

int vboxsf_fsinfo(SHFLROOT root, SHFLHANDLE file, u32 flags,
		  u32 *buf_len, void *buf)
{
	int rc;
	struct shfl_information parms;

	parms.root.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.root.u.value32 = root;

	parms.handle.type = VMMDEV_HGCM_PARM_TYPE_64BIT;
	parms.handle.u.value64 = file;
	parms.flags.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.flags.u.value32 = flags;
	parms.cb.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.cb.u.value32 = *buf_len;
	parms.info.type = VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL;
	parms.info.u.pointer.size = *buf_len;
	parms.info.u.pointer.u.linear_addr = (uintptr_t)buf;

	rc = vboxsf_hgcm_call(SHFL_FN_INFORMATION, &parms,
			      SHFL_CPARMS_INFORMATION);
	if (rc >= 0)
		*buf_len = parms.cb.u.value32;

	return rc;
}

int vboxsf_lock(SHFLROOT root, SHFLHANDLE file, u64 offset,
		u64 size, u32 lock)
{
	struct shfl_lock parms;

	parms.root.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.root.u.value32 = root;

	parms.handle.type = VMMDEV_HGCM_PARM_TYPE_64BIT;
	parms.handle.u.value64 = file;
	parms.offset.type = VMMDEV_HGCM_PARM_TYPE_64BIT;
	parms.offset.u.value64 = offset;
	parms.length.type = VMMDEV_HGCM_PARM_TYPE_64BIT;
	parms.length.u.value64 = size;

	parms.flags.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.flags.u.value32 = lock;

	return vboxsf_hgcm_call(SHFL_FN_LOCK, &parms, SHFL_CPARMS_LOCK);
}

int vboxsf_set_utf8(void)
{
	return vboxsf_hgcm_call(SHFL_FN_SET_UTF8, NULL, 0);
}

int vboxsf_readlink(SHFLROOT root, struct shfl_string *parsed_path,
		    u32 buf_len, u8 *buf)
{
	struct shfl_readLink parms;

	parms.root.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.root.u.value32 = root;

	parms.path.type = VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL_IN;
	parms.path.u.pointer.size = shfl_string_buf_size(parsed_path);
	parms.path.u.pointer.u.linear_addr = (uintptr_t)parsed_path;

	parms.buffer.type = VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL_OUT;
	parms.buffer.u.pointer.size = buf_len;
	parms.buffer.u.pointer.u.linear_addr = (uintptr_t)buf;

	return vboxsf_hgcm_call(SHFL_FN_READLINK, &parms, SHFL_CPARMS_READLINK);
}

int vboxsf_symlink(SHFLROOT root, struct shfl_string *new_path,
		   struct shfl_string *old_path, struct shfl_fsobjinfo *buf)
{
	struct shfl_symlink parms;

	parms.root.type = VMMDEV_HGCM_PARM_TYPE_32BIT;
	parms.root.u.value32 = root;

	parms.new_path.type = VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL_IN;
	parms.new_path.u.pointer.size = shfl_string_buf_size(new_path);
	parms.new_path.u.pointer.u.linear_addr = (uintptr_t)new_path;

	parms.old_path.type = VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL_IN;
	parms.old_path.u.pointer.size = shfl_string_buf_size(old_path);
	parms.old_path.u.pointer.u.linear_addr = (uintptr_t)old_path;

	parms.info.type = VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL_OUT;
	parms.info.u.pointer.size = sizeof(struct shfl_fsobjinfo);
	parms.info.u.pointer.u.linear_addr = (uintptr_t)buf;

	return vboxsf_hgcm_call(SHFL_FN_SYMLINK, &parms, SHFL_CPARMS_SYMLINK);
}

int vboxsf_set_symlinks(void)
{
	return vboxsf_hgcm_call(SHFL_FN_SET_SYMLINKS, NULL, 0);
}

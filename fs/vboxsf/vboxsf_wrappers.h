/*
 * Copyright (C) 2006-2016 Oracle Corporation
 *
 * Protype declarations for the wrapper functions for the shfl host calls.
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

#ifndef VBOXSF_WRAPPERS_H
#define VBOXSF_WRAPPERS_H

#include "shfl_hostintf.h"

/**
 * @addtogroup grp_vboxguest_lib_r0
 *
 * Note all these functions (all functions prefixed with vboxsf_)
 * return a vbox status code rather then a negative errno on error.
 * @{
 */

int vboxsf_connect(void);
void vboxsf_disconnect(void);

int vboxsf_query_mappings(struct shfl_mapping mappings[], u32 *mappings_len);
int vboxsf_query_mapname(SHFLROOT root, struct shfl_string *string, u32 size);

/**
 * Create a new file or folder or open an existing one in a shared folder.
 * Proxies to vbsfCreate in the host shared folder service.
 *
 * @returns VBox status code, but see note below
 * @param   root         Root of the shared folder in which to create the file
 * @param   parsed_path  The path of the file or folder relative to the shared
 *                       folder
 * @param   create_parms Parameters for file/folder creation.  See the
 *                       structure description in shflsvc.h
 * @retval  create_parms See the structure description in shflsvc.h
 *
 * @note This function reports errors as follows.  The return value is always
 *       VINF_SUCCESS unless an exceptional condition occurs - out of
 *       memory, invalid arguments, etc.  If the file or folder could not be
 *       opened or created, create_parms->handle will be set to
 *       SHFL_HANDLE_NIL on return.  In this case the value in
 *       create_parms->result provides information as to why (e.g.
 *       SHFL_FILE_EXISTS).  create_parms->result is also set on success
 *       as additional information.
 */
int vboxsf_create(SHFLROOT root, struct shfl_string *parsed_path,
		  struct shfl_createparms *create_parms);

int vboxsf_close(SHFLROOT root, SHFLHANDLE file);
int vboxsf_remove(SHFLROOT root, struct shfl_string *parsed_path, u32 flags);
int vboxsf_rename(SHFLROOT root, struct shfl_string *src_path,
		  struct shfl_string *dest_path, u32 flags);
int vboxsf_flush(SHFLROOT root, SHFLHANDLE file);

int vboxsf_read(SHFLROOT root, SHFLHANDLE file, u64 offset,
		u32 *buf_len, u8 *buf);
int vboxsf_write(SHFLROOT root, SHFLHANDLE file, u64 offset,
		 u32 *buf_len, u8 *buf);

int vboxsf_lock(SHFLROOT root, SHFLHANDLE file, u64 offset,
		u64 size, u32 lock);

int vboxsf_dirinfo(SHFLROOT root, SHFLHANDLE file,
		   struct shfl_string *parsed_path, u32 flags, u32 index,
		   u32 *buf_len, struct shfl_dirinfo *buf, u32 *file_count);
int vboxsf_fsinfo(SHFLROOT root, SHFLHANDLE file, u32 flags,
		  u32 *buf_len, void *buf);

int vboxsf_map_folder(struct shfl_string *folder_name, SHFLROOT *root);
int vboxsf_unmap_folder(SHFLROOT root);

int vboxsf_readlink(SHFLROOT root, struct shfl_string *parsed_path,
		    u32 buf_len, u8 *buf);
int vboxsf_symlink(SHFLROOT root, struct shfl_string *new_path,
		   struct shfl_string *old_path, struct shfl_fsobjinfo *buf);

int vboxsf_set_utf8(void);
int vboxsf_set_symlinks(void);

/** @} */

#endif

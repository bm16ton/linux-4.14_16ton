/*
 * VirtualBox Guest Shared Folders: Operations for symbolic links.
 *
 * Copyright (C) 2010-2016 Oracle Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "vfsmod.h"

static const char *sf_get_link(struct dentry *dentry, struct inode *inode,
			       struct delayed_call *done)
{
	struct sf_glob_info *sf_g = GET_GLOB_INFO(inode->i_sb);
	struct sf_inode_info *sf_i = GET_INODE_INFO(inode);
	char *path;
	int rc;

	if (!dentry)
		return ERR_PTR(-ECHILD);
	path = kzalloc(PAGE_SIZE, GFP_KERNEL);
	if (!path)
		return ERR_PTR(-ENOMEM);
	rc = vboxsf_readlink(sf_g->root, sf_i->path, PATH_MAX, path);
	if (rc < 0) {
		kfree(path);
		return ERR_PTR(-EPROTO);
	}
	set_delayed_call(done, kfree_link, path);
	return path;
}

const struct inode_operations sf_lnk_iops = {
	.get_link = sf_get_link
};

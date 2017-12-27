/***************************************************************************
 *   Copyright (C) 2004 by Intra2net AG                                    *
 *   opensource@intra2net.com                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License           *
 *   version 2.1 as published by the Free Software Foundation;             *
 *                                                                         *
 ***************************************************************************/

#ifndef _xt_ACCOUNT_cl_H
#define _xt_ACCOUNT_cl_H

#include <xt_ACCOUNT.h>

#define LIBXT_ACCOUNT_VERSION "1.3"

/* Don't set this below the size of struct ipt_account_handle_sockopt */
#define IPT_ACCOUNT_MIN_BUFSIZE 4096

struct ipt_ACCOUNT_context
{
	int sockfd;
	struct ipt_acc_handle_sockopt handle;

	unsigned int data_size;
	void *data;
	unsigned int pos;

	char *error_str;
};

#ifdef __cplusplus
extern "C" {
#endif

int ipt_ACCOUNT_init(struct ipt_ACCOUNT_context *ctx);
void ipt_ACCOUNT_deinit(struct ipt_ACCOUNT_context *ctx);

void ipt_ACCOUNT_free_entries(struct ipt_ACCOUNT_context *ctx);
int ipt_ACCOUNT_read_entries(struct ipt_ACCOUNT_context *ctx,
                             const char *table, char dont_flush);
struct ipt_acc_handle_ip *ipt_ACCOUNT_get_next_entry(
                             struct ipt_ACCOUNT_context *ctx);

/* ipt_ACCOUNT_free_entries is for internal use only function as this library
is constructed to be used in a loop -> Don't allocate memory all the time.
The data buffer is freed on deinit() */

int ipt_ACCOUNT_get_handle_usage(struct ipt_ACCOUNT_context *ctx);
int ipt_ACCOUNT_free_all_handles(struct ipt_ACCOUNT_context *ctx);
int ipt_ACCOUNT_get_table_names(struct ipt_ACCOUNT_context *ctx);
const char *ipt_ACCOUNT_get_next_name(struct ipt_ACCOUNT_context *ctx);

#ifdef __cplusplus
}
#endif


#endif

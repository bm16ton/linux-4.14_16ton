/***************************************************************************
 *   Copyright (C) 2004 by Intra2net AG                                    *
 *   opensource@intra2net.com                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License           *
 *   version 2.1 as published by the Free Software Foundation;             *
 *                                                                         *
 ***************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <netinet/in.h>
#include <linux/if.h>

#include <libxt_ACCOUNT_cl.h>

int ipt_ACCOUNT_init(struct ipt_ACCOUNT_context *ctx)
{
	memset(ctx, 0, sizeof(struct ipt_ACCOUNT_context));
	ctx->handle.handle_nr = -1;

	ctx->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if (ctx->sockfd < 0) {
		ctx->sockfd = -1;
		ctx->error_str = "Can't open socket to kernel. "
		                 "Permission denied or ipt_ACCOUNT module not loaded";
		return -1;
	}

	// 4096 bytes default buffer should save us from reallocations
	// as it fits 200 concurrent active clients
	if ((ctx->data = malloc(IPT_ACCOUNT_MIN_BUFSIZE)) == NULL) {
		close(ctx->sockfd);
		ctx->sockfd = -1;
		ctx->error_str = "Out of memory for data buffer";
		return -1;
	}
	ctx->data_size = IPT_ACCOUNT_MIN_BUFSIZE;

	return 0;
}

void ipt_ACCOUNT_free_entries(struct ipt_ACCOUNT_context *ctx)
{
	if (ctx->handle.handle_nr != -1) {
		setsockopt(ctx->sockfd, IPPROTO_IP, IPT_SO_SET_ACCOUNT_HANDLE_FREE,
		           &ctx->handle, sizeof(struct ipt_acc_handle_sockopt));
		ctx->handle.handle_nr = -1;
	}

	ctx->handle.itemcount = 0;
	ctx->pos = 0;
}

void ipt_ACCOUNT_deinit(struct ipt_ACCOUNT_context *ctx)
{
	free(ctx->data);
	ctx->data = NULL;

	ipt_ACCOUNT_free_entries(ctx);

	close(ctx->sockfd);
	ctx->sockfd = -1;
}

int ipt_ACCOUNT_read_entries(struct ipt_ACCOUNT_context *ctx,
                             const char *table, char dont_flush)
{
	unsigned int s = sizeof(struct ipt_acc_handle_sockopt);
	unsigned int new_size;
	int rtn;

	strncpy(ctx->handle.name, table, ACCOUNT_TABLE_NAME_LEN-1);

	// Get table information
	if (!dont_flush)
		rtn = getsockopt(ctx->sockfd, IPPROTO_IP,
		      IPT_SO_GET_ACCOUNT_PREPARE_READ_FLUSH, &ctx->handle, &s);
	else
		rtn = getsockopt(ctx->sockfd, IPPROTO_IP, IPT_SO_GET_ACCOUNT_PREPARE_READ,
		      &ctx->handle, &s);

	if (rtn < 0) {
		ctx->error_str = "Can't get table information from kernel. "
		                 "Does it exist?";
		return -1;
	}

	// Check data buffer size
	ctx->pos = 0;
	new_size = ctx->handle.itemcount * sizeof(struct ipt_acc_handle_ip);
	// We want to prevent reallocations all the time
	if (new_size < IPT_ACCOUNT_MIN_BUFSIZE)
		new_size = IPT_ACCOUNT_MIN_BUFSIZE;

	// Reallocate if it's too small or twice as big
	if (ctx->data_size < new_size || ctx->data_size > new_size * 2) {
		// Free old buffer
		free(ctx->data);
		ctx->data_size = 0;

		if ((ctx->data = malloc(new_size)) == NULL) {
			ctx->error_str = "Out of memory for data buffer";
			ipt_ACCOUNT_free_entries(ctx);
			return -1;
		}

		ctx->data_size = new_size;
	}

	// Copy data from kernel
	memcpy(ctx->data, &ctx->handle, sizeof(struct ipt_acc_handle_sockopt));
	rtn = getsockopt(ctx->sockfd, IPPROTO_IP, IPT_SO_GET_ACCOUNT_GET_DATA,
	      ctx->data, &ctx->data_size);
	if (rtn < 0) {
		ctx->error_str = "Can't get data from kernel. "
		                 "Check /var/log/messages for details.";
		ipt_ACCOUNT_free_entries(ctx);
		return -1;
	}

	// Free kernel handle but don't reset pos/itemcount
	setsockopt(ctx->sockfd, IPPROTO_IP, IPT_SO_SET_ACCOUNT_HANDLE_FREE,
	           &ctx->handle, sizeof(struct ipt_acc_handle_sockopt));
	ctx->handle.handle_nr = -1;

	return 0;
}

struct ipt_acc_handle_ip *ipt_ACCOUNT_get_next_entry(struct ipt_ACCOUNT_context *ctx)
{
	struct ipt_acc_handle_ip *rtn;

	// Empty or no more items left to return?
	if (!ctx->handle.itemcount || ctx->pos >= ctx->handle.itemcount)
		return NULL;

	// Get next entry
	rtn = (struct ipt_acc_handle_ip *)(ctx->data + ctx->pos
	      * sizeof(struct ipt_acc_handle_ip));
	ctx->pos++;

	return rtn;
}

int ipt_ACCOUNT_get_handle_usage(struct ipt_ACCOUNT_context *ctx)
{
	unsigned int s = sizeof(struct ipt_acc_handle_sockopt);
	if (getsockopt(ctx->sockfd, IPPROTO_IP,
	    IPT_SO_GET_ACCOUNT_GET_HANDLE_USAGE, &ctx->handle, &s) < 0) {
		ctx->error_str = "Can't get handle usage information from kernel";
		return -1;
	}
	ctx->handle.handle_nr = -1;

	return ctx->handle.itemcount;
 }

int ipt_ACCOUNT_free_all_handles(struct ipt_ACCOUNT_context *ctx)
{
	if (setsockopt(ctx->sockfd, IPPROTO_IP,
	    IPT_SO_SET_ACCOUNT_HANDLE_FREE_ALL, NULL, 0) < 0) {
		ctx->error_str = "Can't free all kernel handles";
		return -1;
	}

	return 0;
}

int ipt_ACCOUNT_get_table_names(struct ipt_ACCOUNT_context *ctx)
{
	int rtn = getsockopt(ctx->sockfd, IPPROTO_IP,
	          IPT_SO_GET_ACCOUNT_GET_TABLE_NAMES,
	          ctx->data, &ctx->data_size);
	if (rtn < 0) {
		ctx->error_str = "Can't get table names from kernel. Out of memory, "
		                 "MINBUFISZE too small?";
		return -1;
	}
	ctx->pos = 0;
	return 0;
}

const char *ipt_ACCOUNT_get_next_name(struct ipt_ACCOUNT_context *ctx)
{
	const char *rtn;
	if (((char *)ctx->data)[ctx->pos] == 0)
		return 0;

	rtn = ctx->data + ctx->pos;
	ctx->pos += strlen(ctx->data + ctx->pos) + 1;

	return rtn;
}

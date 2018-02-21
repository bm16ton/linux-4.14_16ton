/***************************************************************************
 *   Copyright (C) 2004-2006 by Intra2net AG                               *
 *   opensource@intra2net.com                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License                  *
 *   version 2 as published by the Free Software Foundation;               *
 *                                                                         *
 ***************************************************************************/

#ifndef _IPT_ACCOUNT_H
#define _IPT_ACCOUNT_H

/*
 * Socket option interface shared between kernel (xt_ACCOUNT) and userspace
 * library (libxt_ACCOUNT_cl). Hopefully we are unique at least within our
 * kernel & xtables-addons space.
 *
 * Turned out often enough we are not.
 * 64-67	used by ip_tables, ip6_tables
 * 96-100	used by arp_tables
 * 128-131	used by ebtables
 */
#define SO_ACCOUNT_BASE_CTL 70

#define IPT_SO_SET_ACCOUNT_HANDLE_FREE (SO_ACCOUNT_BASE_CTL + 1)
#define IPT_SO_SET_ACCOUNT_HANDLE_FREE_ALL (SO_ACCOUNT_BASE_CTL + 2)
#define IPT_SO_SET_ACCOUNT_MAX		 IPT_SO_SET_ACCOUNT_HANDLE_FREE_ALL

#define IPT_SO_GET_ACCOUNT_PREPARE_READ (SO_ACCOUNT_BASE_CTL + 4)
#define IPT_SO_GET_ACCOUNT_PREPARE_READ_FLUSH (SO_ACCOUNT_BASE_CTL + 5)
#define IPT_SO_GET_ACCOUNT_GET_DATA (SO_ACCOUNT_BASE_CTL + 6)
#define IPT_SO_GET_ACCOUNT_GET_HANDLE_USAGE (SO_ACCOUNT_BASE_CTL + 7)
#define IPT_SO_GET_ACCOUNT_GET_TABLE_NAMES (SO_ACCOUNT_BASE_CTL + 8)
#define IPT_SO_GET_ACCOUNT_MAX	  IPT_SO_GET_ACCOUNT_GET_TABLE_NAMES

#define ACCOUNT_MAX_TABLES 128
#define ACCOUNT_TABLE_NAME_LEN 32
#define ACCOUNT_MAX_HANDLES 10

/* Structure for the userspace part of ipt_ACCOUNT */
struct ipt_acc_info {
	__be32 net_ip;
	__be32 net_mask;
	char table_name[ACCOUNT_TABLE_NAME_LEN];
	int32_t table_nr;
};

/* Handle structure for communication with the userspace library */
struct ipt_acc_handle_sockopt {
	uint32_t handle_nr;				   /* Used for HANDLE_FREE */
	char name[ACCOUNT_TABLE_NAME_LEN];	 /* Used for HANDLE_PREPARE_READ/
												 HANDLE_READ_FLUSH */
	uint32_t itemcount;				   /* Used for HANDLE_PREPARE_READ/
												 HANDLE_READ_FLUSH */
};

/*
	Used for every IP when returning data
*/
struct ipt_acc_handle_ip {
	__be32 ip, __dummy;
	uint64_t src_packets;
	uint64_t src_bytes;
	uint64_t dst_packets;
	uint64_t dst_bytes;
};

#endif /* _IPT_ACCOUNT_H */

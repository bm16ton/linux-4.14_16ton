/*
 * Kernel module to implement Port Knocking and SPA matching support.
 *
 * (C) 2006-2008 J. Federico Hernandez <fede.hernandez@gmail.com>
 * (C) 2006 Luis Floreani <luis.floreani@gmail.com>
 *
 * $Id$
 *
 * This program is released under the terms of GNU GPL version 2.
 */
#ifndef _XT_PKNOCK_H
#define _XT_PKNOCK_H

#define PKNOCK "xt_pknock: "

enum {
	XT_PKNOCK_KNOCKPORT   = 1 << 0,
	XT_PKNOCK_TIME        = 1 << 1,
	XT_PKNOCK_NAME        = 1 << 2,
	XT_PKNOCK_STRICT      = 1 << 3,
	XT_PKNOCK_CHECKIP     = 1 << 4,
	XT_PKNOCK_OPENSECRET  = 1 << 5,
	XT_PKNOCK_CLOSESECRET = 1 << 6,
	XT_PKNOCK_AUTOCLOSE   = 1 << 7,

	/* Can never change these, as they are make up the user protocol. */
	XT_PKNOCK_MAX_PORTS      = 15,
	XT_PKNOCK_MAX_BUF_LEN    = 31,
	XT_PKNOCK_MAX_PASSWD_LEN = 31,
};

#define DEBUG 1

struct xt_pknock_mtinfo {
	char rule_name[XT_PKNOCK_MAX_BUF_LEN+1];
	uint32_t			rule_name_len;
	char open_secret[XT_PKNOCK_MAX_PASSWD_LEN+1];
	uint32_t			open_secret_len;
	char close_secret[XT_PKNOCK_MAX_PASSWD_LEN+1];
	uint32_t			close_secret_len;
	uint8_t	option;		/* --time, --knock-port, ... */
	uint8_t	ports_count;	/* number of ports */
	uint16_t	port[XT_PKNOCK_MAX_PORTS]; /* port[,port,port,...] */
	uint32_t	max_time;	/* max matching time between ports */
	uint32_t autoclose_time;
};

struct xt_pknock_nl_msg {
	char rule_name[XT_PKNOCK_MAX_BUF_LEN+1];
	__be32 peer_ip;
};

#endif /* _XT_PKNOCK_H */

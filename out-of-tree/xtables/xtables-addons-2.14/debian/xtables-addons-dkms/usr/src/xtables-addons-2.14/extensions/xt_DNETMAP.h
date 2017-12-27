#ifndef _LINUX_NETFILTER_XT_DNETMAP_H
#define _LINUX_NETFILTER_XT_DNETMAP_H 1

#define DNETMAP_VERSION 2

enum {
	XT_DNETMAP_TTL 					= 1 << 0,
	XT_DNETMAP_REUSE 				= 1 << 1,
	XT_DNETMAP_PREFIX 			= 1 << 2,
	XT_DNETMAP_STATIC 			= 1 << 3,
	XT_DNETMAP_PERSISTENT 	= 1 << 4,
	XT_DNETMAP_FULL				 	= 1 << 5,
};

struct xt_DNETMAP_tginfo {
	struct nf_nat_range prefix;
	__u8 flags;
	__s32 ttl;
};

#endif

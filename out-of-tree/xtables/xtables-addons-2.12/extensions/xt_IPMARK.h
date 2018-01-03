#ifndef _LINUX_NETFILTER_XT_IPMARK_H
#define _LINUX_NETFILTER_XT_IPMARK_H 1

enum {
	XT_IPMARK_SRC,
	XT_IPMARK_DST,
};

struct xt_ipmark_tginfo {
	__u32 andmask;
	__u32 ormask;
	__u8 selector;
	__u8 shift;
};

#endif /* _LINUX_NETFILTER_XT_IPMARK_H */

#ifndef _LINUX_NETFILTER_XT_LOGMARK_TARGET_H
#define _LINUX_NETFILTER_XT_LOGMARK_TARGET_H 1

struct xt_logmark_tginfo {
	char prefix[14];
	u_int8_t level;
};

#endif /* _LINUX_NETFILTER_XT_LOGMARK_TARGET_H */

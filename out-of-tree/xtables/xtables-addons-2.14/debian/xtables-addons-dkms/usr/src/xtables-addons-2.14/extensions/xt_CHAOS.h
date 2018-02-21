#ifndef _LINUX_NETFILTER_XT_CHAOS_H
#define _LINUX_NETFILTER_XT_CHAOS_H 1

enum xt_chaos_target_variant {
	XTCHAOS_NORMAL,
	XTCHAOS_TARPIT,
	XTCHAOS_DELUDE,
};

struct xt_chaos_tginfo {
	uint8_t variant;
};

#endif /* _LINUX_NETFILTER_XT_CHAOS_H */

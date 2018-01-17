#ifndef _LINUX_NETFILTER_XT_LENGTH2_H
#define _LINUX_NETFILTER_XT_LENGTH2_H

enum {
	XT_LENGTH_INVERT = 1 << 0,

	/* IP header plus payload */
	XT_LENGTH_LAYER3 = 1 << 1,
	/* Strip IP header: */
	XT_LENGTH_LAYER4 = 1 << 2,
	/* Strip TCP/UDP/etc. header */
	XT_LENGTH_LAYER5 = 1 << 3,
	/* TCP/UDP/SCTP payload */
	XT_LENGTH_LAYER7 = 1 << 4,
};

struct xt_length_mtinfo2 {
	u_int32_t min, max;
	u_int16_t flags;
};

#endif /* _LINUX_NETFILTER_XT_LENGTH2_H */

#ifndef _LINUX_NETFILTER_XT_FUZZY_H
#define _LINUX_NETFILTER_XT_FUZZY_H 1

enum {
	FUZZY_MIN_RATE = 3,
	FUZZY_MAX_RATE = 10000000,
};

struct xt_fuzzy_mtinfo {
	uint32_t minimum_rate;
	uint32_t maximum_rate;
	uint32_t packets_total;
	uint32_t bytes_total;
	uint32_t previous_time;
	uint32_t present_time;
	uint32_t mean_rate;
	uint8_t acceptance_rate;
};

#endif /* _LINUX_NETFILTER_XT_FUZZY_H */

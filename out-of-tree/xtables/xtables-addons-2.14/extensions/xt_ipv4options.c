/*
 *	xt_ipv4opts - Xtables module to match IPv4 options
 *	Copyright © Jan Engelhardt, 2009
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License; either
 *	version 2 of the License, or any later version, as published by the
 *	Free Software Foundation.
 */
#include <linux/ip.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/netfilter/x_tables.h>
#include <net/ip.h>
#include "xt_ipv4options.h"
#include "compat_xtables.h"

static uint32_t ipv4options_rd(const uint8_t *data, int len)
{
	uint32_t opts = 0;

	while (len >= 2) {
		switch (data[0]) {
		case IPOPT_END:
			return opts;
		case IPOPT_NOOP:
			--len;
			++data;
			continue;
		}

		if (data[1] < 2 || data[1] > len)
			return opts;
		opts |= 1 << (data[0] & 0x1F);
		len  -= data[1];
		data += data[1];
	}

	return opts;
}

static bool ipv4options_mt(const struct sk_buff *skb,
    struct xt_action_param *par)
{
	const struct xt_ipv4options_mtinfo1 *info = par->matchinfo;
	const struct iphdr *iph = ip_hdr(skb);
	uint32_t opts = 0;
	uint16_t len  = ip_hdrlen(skb) - sizeof(struct iphdr);

	if (len > 0)
		opts = ipv4options_rd((const void *)iph +
		       sizeof(struct iphdr), len);

	opts ^= info->invert;
	opts &= info->map;
	return (info->flags & XT_V4OPTS_ANY) ? opts : opts == info->map;
}

static struct xt_match ipv4options_mt_reg __read_mostly = {
	.name      = "ipv4options",
	.revision  = 1,
	.family    = NFPROTO_IPV4,
	.match     = ipv4options_mt,
	.matchsize = sizeof(struct xt_ipv4options_mtinfo1),
	.me        = THIS_MODULE,
};

static int __init ipv4options_mt_init(void)
{
	return xt_register_match(&ipv4options_mt_reg);
}

static void __exit ipv4options_mt_exit(void)
{
	xt_unregister_match(&ipv4options_mt_reg);
}

MODULE_DESCRIPTION("Xatblse: IPv4 option match");
MODULE_AUTHOR("Jan Engelhardt ");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_ipv4options");
module_init(ipv4options_mt_init);
module_exit(ipv4options_mt_exit);

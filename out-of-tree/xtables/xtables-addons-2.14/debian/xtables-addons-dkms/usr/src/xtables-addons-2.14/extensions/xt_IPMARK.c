/*
 *	"IPMARK" target extension for Xtables
 *	Copyright © Grzegorz Janoszka <Grzegorz.Janoszka@pro.onet.pl>, 2003
 *	Jan Engelhardt, 2008
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License; either
 *	version 2 of the License, or any later version, as published by the
 *	Free Software Foundation.
 */
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/version.h>
#include <linux/netfilter/x_tables.h>
#include <net/checksum.h>
#include "xt_IPMARK.h"
#include "compat_xtables.h"

MODULE_AUTHOR("Grzegorz Janoszka <Grzegorz@Janoszka.pl>");
MODULE_DESCRIPTION("Xtables: mark based on IP address");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_IPMARK");
MODULE_ALIAS("ip6t_IPMARK");

static unsigned int
ipmark_tg4(struct sk_buff *skb, const struct xt_action_param *par)
{
	const struct xt_ipmark_tginfo *ipmarkinfo = par->targinfo;
	const struct iphdr *iph = ip_hdr(skb);
	__u32 mark;

	if (ipmarkinfo->selector == XT_IPMARK_SRC)
		mark = ntohl(iph->saddr);
	else
		mark = ntohl(iph->daddr);

	mark >>= ipmarkinfo->shift;
	mark &= ipmarkinfo->andmask;
	mark |= ipmarkinfo->ormask;

	skb_nfmark(skb) = mark;
	return XT_CONTINUE;
}

/* Function is safe for any value of @s */
static __u32 ipmark_from_ip6(const struct in6_addr *a, unsigned int s)
{
	unsigned int q = s % 32;
	__u32 mask;

	if (s >= 128)
		return 0;

	mask = ntohl(a->s6_addr32[3 - s/32]) >> q;
	if (s > 0 && s < 96 && q != 0)
		mask |= ntohl(a->s6_addr32[2 - s/32]) << (32 - q);
	return mask;
}

static unsigned int
ipmark_tg6(struct sk_buff *skb, const struct xt_action_param *par)
{
	const struct xt_ipmark_tginfo *info = par->targinfo;
	const struct ipv6hdr *iph = ipv6_hdr(skb);
	__u32 mark;

	if (info->selector == XT_IPMARK_SRC)
		mark = ipmark_from_ip6(&iph->saddr, info->shift);
	else
		mark = ipmark_from_ip6(&iph->daddr, info->shift);

	mark &= info->andmask;
	mark |= info->ormask;
	skb_nfmark(skb) = mark;
	return XT_CONTINUE;
}

static struct xt_target ipmark_tg_reg[] __read_mostly = {
	{
		.name       = "IPMARK",
		.revision   = 1,
		.family     = NFPROTO_IPV4,
		.table      = "mangle",
		.target     = ipmark_tg4,
		.targetsize = sizeof(struct xt_ipmark_tginfo),
		.me         = THIS_MODULE,
	},
	{
		.name       = "IPMARK",
		.revision   = 1,
		.family     = NFPROTO_IPV6,
		.table      = "mangle",
		.target     = ipmark_tg6,
		.targetsize = sizeof(struct xt_ipmark_tginfo),
		.me         = THIS_MODULE,
	},
};

static int __init ipmark_tg_init(void)
{
	return xt_register_targets(ipmark_tg_reg, ARRAY_SIZE(ipmark_tg_reg));
}

static void __exit ipmark_tg_exit(void)
{
	xt_unregister_targets(ipmark_tg_reg, ARRAY_SIZE(ipmark_tg_reg));
}

module_init(ipmark_tg_init);
module_exit(ipmark_tg_exit);

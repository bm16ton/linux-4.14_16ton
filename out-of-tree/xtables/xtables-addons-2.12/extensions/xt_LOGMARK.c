/*
 *	"LOGMARK" target extension to Xtables
 *	useful for debugging
 *
 *	Copyright © Jan Engelhardt, 2008-2010
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License; either
 *	version 2 of the License, or any later version, as published by the
 *	Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/version.h>
#include <linux/netfilter/nf_conntrack_common.h>
#include <linux/netfilter/x_tables.h>
#include "compat_xtables.h"
#include "xt_LOGMARK.h"

static const char *const hook_names[] = {
	[NF_INET_PRE_ROUTING]  = "PREROUTING",
	[NF_INET_LOCAL_IN]     = "INPUT",
	[NF_INET_FORWARD]      = "FORWARD",
	[NF_INET_LOCAL_OUT]    = "OUTPUT",
	[NF_INET_POST_ROUTING] = "POSTROUTING",
};

static const char *const dir_names[] = {
	"ORIGINAL", "REPLY",
};

static void logmark_ct(const struct nf_conn *ct, enum ip_conntrack_info ctinfo)
{
	bool prev = false;

	printk(" ct=0x%p ctmark=0x%x ctstate=", ct, ct->mark);
	ctinfo %= IP_CT_IS_REPLY;
	if (ctinfo == IP_CT_NEW)
		printk("NEW");
	else if (ctinfo == IP_CT_ESTABLISHED)
		printk("ESTABLISHED");
	else if (ctinfo == IP_CT_RELATED)
		printk("RELATED");
	if (test_bit(IPS_SRC_NAT_BIT, &ct->status))
		printk(",SNAT");
	if (test_bit(IPS_DST_NAT_BIT, &ct->status))
		printk(",DNAT");

	printk(" ctstatus=");
	if (ct->status & IPS_EXPECTED) {
		printk("EXPECTED");
		prev = true;
	}
	if (ct->status & IPS_SEEN_REPLY)
		printk("%s""SEEN_REPLY", prev++ ? "," : "");
	if (ct->status & IPS_ASSURED)
		printk("%s""ASSURED", prev++ ? "," : "");
	if (ct->status & IPS_CONFIRMED)
		printk("%s""CONFIRMED", prev++ ? "," : "");
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0)
	printk(" lifetime=%lus", nf_ct_expires(ct) / HZ);
#else
	printk(" lifetime=%lus",
	       (jiffies - ct->timeout.expires) / HZ);
#endif
}

static unsigned int
logmark_tg(struct sk_buff *skb, const struct xt_action_param *par)
{
	const struct xt_logmark_tginfo *info = par->targinfo;
	const struct nf_conn *ct;
	enum ip_conntrack_info ctinfo;

	printk("<%u>%.*s""iif=%d hook=%s nfmark=0x%x "
	       "secmark=0x%x classify=0x%x",
	       info->level, (unsigned int)sizeof(info->prefix), info->prefix,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,10,0)
	       skb_ifindex(skb), hook_names[par->state->hook],
#else
	       skb_ifindex(skb), hook_names[par->hooknum],
#endif
	       skb_nfmark(skb), skb_secmark(skb), skb->priority);

	ct = nf_ct_get(skb, &ctinfo);
	printk(" ctdir=%s", dir_names[ctinfo >= IP_CT_IS_REPLY]);
	if (ct == NULL)
		printk(" ct=NULL ctmark=NULL ctstate=INVALID ctstatus=NONE");
	else if (nf_ct_is_untracked(ct))
		printk(" ct=UNTRACKED ctmark=NULL ctstate=UNTRACKED ctstatus=NONE");
	else
		logmark_ct(ct, ctinfo);

	printk("\n");
	return XT_CONTINUE;
}

static int
logmark_tg_check(const struct xt_tgchk_param *par)
{
	const struct xt_logmark_tginfo *info = par->targinfo;

	if (info->level >= 8) {
		pr_debug("LOGMARK: level %u >= 8\n", info->level);
		return -EINVAL;
	}

	return 0;
}

static struct xt_target logmark_tg_reg[] __read_mostly = {
	{
		.name       = "LOGMARK",
		.revision   = 0,
		.family     = NFPROTO_IPV4,
		.checkentry = logmark_tg_check,
		.target     = logmark_tg,
		.targetsize = sizeof(struct xt_logmark_tginfo),
		.me         = THIS_MODULE,
	},
	{
		.name       = "LOGMARK",
		.revision   = 0,
		.family     = NFPROTO_IPV6,
		.checkentry = logmark_tg_check,
		.target     = logmark_tg,
		.targetsize = sizeof(struct xt_logmark_tginfo),
		.me         = THIS_MODULE,
	},
};

static int __init logmark_tg_init(void)
{
	return xt_register_targets(logmark_tg_reg, ARRAY_SIZE(logmark_tg_reg));
}

static void __exit logmark_tg_exit(void)
{
	xt_unregister_targets(logmark_tg_reg, ARRAY_SIZE(logmark_tg_reg));
}

module_init(logmark_tg_init);
module_exit(logmark_tg_exit);
MODULE_DESCRIPTION("Xtables: netfilter mark logging to syslog");
MODULE_AUTHOR("Jan Engelhardt ");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_LOGMARK");
MODULE_ALIAS("ip6t_LOGMARK");

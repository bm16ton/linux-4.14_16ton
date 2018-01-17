/*
 *	xt_iface - kernel module to match interface state flags
 *
 *	Original author: Gáspár Lajos <gaspar.lajos@glsys.eu>
 */

#include <linux/if.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/version.h>
#include <linux/netfilter/x_tables.h>
#include "xt_iface.h"
#include "compat_xtables.h"

struct xt_iface_flag_pairs {
	uint16_t iface_flag;
	uint32_t iff_flag;
};

MODULE_AUTHOR("Gáspár Lajos <gaspar.lajos@glsys.eu>");
MODULE_DESCRIPTION("Xtables: iface match module");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_iface");
MODULE_ALIAS("ip6t_iface");
//MODULE_ALIAS("arpt_iface");

static const struct xt_iface_flag_pairs xt_iface_lookup[] =
{
	{.iface_flag = XT_IFACE_UP,		.iff_flag = IFF_UP},
	{.iface_flag = XT_IFACE_BROADCAST,	.iff_flag = IFF_BROADCAST},
	{.iface_flag = XT_IFACE_LOOPBACK,	.iff_flag = IFF_LOOPBACK},
	{.iface_flag = XT_IFACE_POINTOPOINT,	.iff_flag = IFF_POINTOPOINT},
	{.iface_flag = XT_IFACE_RUNNING,	.iff_flag = IFF_RUNNING},
	{.iface_flag = XT_IFACE_NOARP,		.iff_flag = IFF_NOARP},
	{.iface_flag = XT_IFACE_PROMISC,	.iff_flag = IFF_PROMISC},
	{.iface_flag = XT_IFACE_MULTICAST,	.iff_flag = IFF_MULTICAST},
	{.iface_flag = XT_IFACE_DYNAMIC,	.iff_flag = IFF_DYNAMIC},
	{.iface_flag = XT_IFACE_LOWER_UP,	.iff_flag = IFF_LOWER_UP},
	{.iface_flag = XT_IFACE_DORMANT,	.iff_flag = IFF_DORMANT},
};

static const struct net_device *iface_get(const struct xt_iface_mtinfo *info,
    const struct xt_action_param *par, struct net_device **put)
{
	if (info->flags & XT_IFACE_DEV_IN)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,10,0)
		return par->state->in;
#else
		return par->in;
#endif
	else if (info->flags & XT_IFACE_DEV_OUT)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,10,0)
		return par->state->out;
#else
		return par->out;
#endif
	return *put = dev_get_by_name(&init_net, info->ifname);
}

static bool iface_flagtest(unsigned int devflags, unsigned int flags,
    unsigned int invflags)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(xt_iface_lookup); ++i)
		if ((flags & xt_iface_lookup[i].iface_flag) &&
		    !!(devflags & xt_iface_lookup[i].iff_flag) ^
		    !(invflags & xt_iface_lookup[i].iface_flag))
			return false;
	return true;
}

static bool xt_iface_mt(const struct sk_buff *skb,
    struct xt_action_param *par)
{
	const struct xt_iface_mtinfo *info = par->matchinfo;
	struct net_device *put = NULL;
	const struct net_device *dev = iface_get(info, par, &put);
	bool retval;

	if (dev == NULL)
		return false;
	retval = iface_flagtest(dev->flags, info->flags, info->invflags);
	if (put != NULL)
		dev_put(put);
	return retval;
}

static struct xt_match xt_iface_mt_reg[] __read_mostly = {
	{
		.name       = "iface",
		.revision   = 0,
		.family     = NFPROTO_IPV4,
		.matchsize  = sizeof(struct xt_iface_mtinfo),
		.match      = xt_iface_mt,
		.me         = THIS_MODULE,
	},
	{
		.name       = "iface",
		.revision   = 0,
		.family     = NFPROTO_IPV6,
		.matchsize  = sizeof(struct xt_iface_mtinfo),
		.match      = xt_iface_mt,
		.me         = THIS_MODULE,
	},
};

static int __init xt_iface_match_init(void)
{
	return xt_register_matches(xt_iface_mt_reg,
		ARRAY_SIZE(xt_iface_mt_reg));
}

static void __exit xt_iface_match_exit(void)
{
	xt_unregister_matches(xt_iface_mt_reg, ARRAY_SIZE(xt_iface_mt_reg));
}

module_init(xt_iface_match_init);
module_exit(xt_iface_match_exit);

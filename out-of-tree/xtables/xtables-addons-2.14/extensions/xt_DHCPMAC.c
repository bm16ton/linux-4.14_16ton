/*
 *	"DHCPMAC" extensions for Xtables
 *	Copyright © Jan Engelhardt, 2008
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License; either
 *	version 2 of the License, or any later version, as published by the
 *	Free Software Foundation.
 */
#include <linux/ip.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/types.h>
#include <linux/udp.h>
#include <net/ip.h>
#include <linux/netfilter/x_tables.h>
#include "xt_DHCPMAC.h"
#include "compat_xtables.h"

struct dhcp_message {
	uint8_t op, htype, hlen, hops;
	__be32 xid;
	__be16 secs, flags;
	__be32 ciaddr, yiaddr, siaddr, giaddr;
	char chaddr[16];
	/* Omitting all unneeded fields saves runtime memory */
	/* char sname[64], file[128]; */
};

static void ether_set(unsigned char *addr, const unsigned char *op,
    uint8_t mask)
{
	uint8_t lo_mask;
	unsigned int i;

	for (i = 0; i < ETH_ALEN && mask > 0; ++i) {
		lo_mask = (mask >= 8) ? 8 : mask;
		/* FF << 4 >> 4 = 0F */
		lo_mask = (uint8_t)(~0U << lo_mask) >> lo_mask;
		addr[i] &= lo_mask;
		addr[i] |= op[i] & ~lo_mask;
		if (mask >= 8)
			mask -= 8;
		else
			mask = 0;
	}
}

static bool ether_cmp(const unsigned char *lh, const unsigned char *rh,
    uint8_t mask)
{
	uint8_t lo_mask;
	unsigned int i;
#define ZMAC_FMT "%02X:%02X:%02X:%02X:%02X:%02X"
#define ZMACHEX(s) s[0], s[1], s[2], s[3], s[4], s[5]

	for (i = 0; i < ETH_ALEN && mask > 0; ++i) {
		lo_mask = (mask >= 8) ? 8 : mask;
		/* ~(0xFF << 4 >> 4) = ~0x0F = 0xF0 */
		lo_mask = ~((uint8_t)(~0U << lo_mask) >> lo_mask);
		if ((lh[i] ^ rh[i]) & lo_mask)
			return false;
		if (mask >= 8)
			mask -= 8;
		else
			mask = 0;
	}
	return true;
}

static bool
dhcpmac_mt(const struct sk_buff *skb, struct xt_action_param *par)
{
	const struct dhcpmac_info *info = par->matchinfo;
	const struct dhcp_message *dh;
	struct dhcp_message dhcpbuf;

	dh = skb_header_pointer(skb, par->thoff + sizeof(struct udphdr),
	     sizeof(dhcpbuf), &dhcpbuf);
	if (dh == NULL)
		/*
		 * No hotdrop. This packet does not look like DHCP, but other
		 * matches may still have a valid reason to get their chance
		 * to match on this.
		 */
		return false;

	return ether_cmp((const void *)dh->chaddr, info->addr, info->mask);
}

static unsigned int
dhcpmac_tg(struct sk_buff *skb, const struct xt_action_param *par)
{
	const struct dhcpmac_info *info = par->targinfo;
	struct dhcp_message dhcpbuf, *dh;
	struct udphdr udpbuf, *udph;
	unsigned int i;

	if (!skb_make_writable(skb, 0))
		return NF_DROP;

	udph = skb_header_pointer(skb, ip_hdrlen(skb),
	       sizeof(udpbuf), &udpbuf);
	if (udph == NULL)
		return NF_DROP;

	dh = skb_header_pointer(skb, ip_hdrlen(skb) + sizeof(udpbuf),
	     sizeof(dhcpbuf), &dhcpbuf);
	if (dh == NULL)
		return NF_DROP;

	for (i = 0; i < sizeof(dh->chaddr); i += 2)
		csum_replace2(&udph->check, *(const __be16 *)(dh->chaddr + i), 0);

	ether_set(dh->chaddr, info->addr, info->mask);

	for (i = 0; i < sizeof(dh->chaddr); i += 2)
		csum_replace2(&udph->check, 0, *(const __be16 *)(dh->chaddr + i));

	return XT_CONTINUE;
}

static struct xt_target dhcpmac_tg_reg __read_mostly = {
	.name       = "DHCPMAC",
	.revision   = 0,
	.family     = NFPROTO_IPV4,
	.proto      = IPPROTO_UDP,
	.table      = "mangle",
	.target     = dhcpmac_tg,
	.targetsize = XT_ALIGN(sizeof(struct dhcpmac_info)),
	.me         = THIS_MODULE,
};

static struct xt_match dhcpmac_mt_reg __read_mostly = {
	.name       = "dhcpmac",
	.revision   = 0,
	.family     = NFPROTO_IPV4,
	.proto      = IPPROTO_UDP,
	.match      = dhcpmac_mt,
	.matchsize  = sizeof(struct dhcpmac_info),
	.me         = THIS_MODULE,
};

static int __init dhcpmac_init(void)
{
	int ret;

	ret = xt_register_target(&dhcpmac_tg_reg);
	if (ret != 0)
		return ret;
	ret = xt_register_match(&dhcpmac_mt_reg);
	if (ret != 0) {
		xt_unregister_target(&dhcpmac_tg_reg);
		return ret;
	}
	return 0;
}

static void __exit dhcpmac_exit(void)
{
	xt_unregister_target(&dhcpmac_tg_reg);
	xt_unregister_match(&dhcpmac_mt_reg);
}

module_init(dhcpmac_init);
module_exit(dhcpmac_exit);
MODULE_DESCRIPTION("Xtables: Clamp DHCP MAC to packet MAC addresses");
MODULE_AUTHOR("Jan Engelhardt ");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_DHCPMAC");
MODULE_ALIAS("ipt_dhcpmac");

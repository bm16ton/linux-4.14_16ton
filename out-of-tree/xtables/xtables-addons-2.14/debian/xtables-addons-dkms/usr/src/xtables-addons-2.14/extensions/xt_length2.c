/*
 *	xt_length - Xtables module to match packet length
 *	Copyright © Jan Engelhardt , 2007 - 2009
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License; either
 *	version 2 of the License, or any later version, as published by the
 *	Free Software Foundation.
 */
#include <linux/dccp.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/icmp.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/sctp.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <net/ip.h>
#include <net/ipv6.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_ipv6/ip6_tables.h>
#include "xt_length2.h"
#include "compat_xtables.h"
#if defined(CONFIG_IP6_NF_IPTABLES) || defined(CONFIG_IP6_NF_IPTABLES_MODULE)
#	define WITH_IPV6 1
#endif
#ifndef NEXTHDR_IPV4
#	define NEXTHDR_IPV4 4
#endif

MODULE_AUTHOR("Jan Engelhardt ");
MODULE_DESCRIPTION("Xtables: Packet length (Layer3,4,5) match");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_length2");
MODULE_ALIAS("ip6t_length2");

static bool
xtlength_layer5_tcp(unsigned int *length, const struct sk_buff *skb,
                    unsigned int offset)
{
	const struct tcphdr *tcph;
	struct tcphdr buf;

	tcph = skb_header_pointer(skb, offset, sizeof(buf), &buf);
	if (tcph == NULL)
		return false;

	*length = skb->len - offset;
	if (*length >= 4 * tcph->doff)
		*length -= 4 * tcph->doff;
	return true;
}

static bool
xtlength_layer5_dccp(unsigned int *length, const struct sk_buff *skb,
                     unsigned int offset)
{
	const struct dccp_hdr *dh;
	struct dccp_hdr dhbuf;

	dh = skb_header_pointer(skb, offset, sizeof(dhbuf), &dhbuf);
	if (dh == NULL)
		return false;

	*length = skb->len - offset;
	if (*length >= 4 * dh->dccph_doff)
		*length -= 4 * dh->dccph_doff;
	return true;
}

static inline bool
xtlength_layer5(unsigned int *length, const struct sk_buff *skb,
                unsigned int prot, unsigned int offset)
{
	switch (prot) {
	case IPPROTO_TCP:
		return xtlength_layer5_tcp(length, skb, offset);
	case IPPROTO_UDP:
	case IPPROTO_UDPLITE:
		*length = skb->len - offset - sizeof(struct udphdr);
		return true;
	case IPPROTO_SCTP:
		*length = skb->len - offset - sizeof(struct sctphdr);
		return true;
	case IPPROTO_DCCP:
		return xtlength_layer5_dccp(length, skb, offset);
	case IPPROTO_ICMP:
		*length = skb->len - offset - sizeof(struct icmphdr);
		return true;
	case IPPROTO_ICMPV6:
		*length = skb->len - offset -
		          offsetof(struct icmp6hdr, icmp6_dataun);
		return true;
	case IPPROTO_AH:
		*length = skb->len - offset - sizeof(struct ip_auth_hdr);
		return true;
	case IPPROTO_ESP:
		*length = skb->len - offset - sizeof(struct ip_esp_hdr);
		return true;
	}
	return false;
}

static bool
xtlength_layer7_sctp(unsigned int *length, const struct sk_buff *skb,
                     unsigned int offset)
{
	const struct sctp_chunkhdr *ch;
	struct sctp_chunkhdr chbuf;
	unsigned int pos;

	*length = 0;
	for (pos = sizeof(struct sctphdr); pos < skb->len;
	     pos += ntohs(ch->length))
	{
		ch = skb_header_pointer(skb, offset + pos,
		     sizeof(chbuf), &chbuf);
		if (ch == NULL)
			return false;
		if (ch->type != SCTP_CID_DATA)
			continue;
		*length += ntohs(ch->length);
	}
	return true;
}

static bool xtlength_layer7(unsigned int *length, const struct sk_buff *skb,
                            unsigned int proto, unsigned int offset)
{
	switch (proto) {
	case IPPROTO_SCTP:
		return xtlength_layer7_sctp(length, skb, offset);
	default:
		return xtlength_layer5(length, skb, proto, offset);
	}
}

static bool
length2_mt(const struct sk_buff *skb, struct xt_action_param *par)
{
	const struct xt_length_mtinfo2 *info = par->matchinfo;
	const struct iphdr *iph = ip_hdr(skb);
	unsigned int len = 0;
	bool hit = true;

	if (info->flags & XT_LENGTH_LAYER3)
		len = ntohs(iph->tot_len);
	else if (info->flags & XT_LENGTH_LAYER4)
		len = ntohs(iph->tot_len) - par->thoff;
	else if (info->flags & XT_LENGTH_LAYER5)
		hit = xtlength_layer5(&len, skb, iph->protocol, par->thoff);
	else if (info->flags & XT_LENGTH_LAYER7)
		hit = xtlength_layer7(&len, skb, iph->protocol, par->thoff);
	if (!hit)
		return false;

	return (len >= info->min && len <= info->max) ^
	       !!(info->flags & XT_LENGTH_INVERT);
}

#ifdef WITH_IPV6
/**
 * llayer4_proto - figure out the L4 protocol in an IPv6 packet
 * @skb:	skb pointer
 * @offset:	position at which L4 starts (equal to 'protoff' in IPv4 code)
 * @hotdrop:	hotdrop pointer
 *
 * Searches for a recognized L4 header. On success, fills in @offset and
 * returns the protocol number. If not found, %NEXTHDR_MAX is returned.
 * On error, @hotdrop is set.
 */
static unsigned int
llayer4_proto(const struct sk_buff *skb, unsigned int *offset, bool *hotdrop)
{
	/*
	 * Do encapsulation first so that %NEXTHDR_TCP does not hit the TCP
	 * part in an IPv6-in-IPv6 encapsulation.
	 */
	static const unsigned int types[] =
		{IPPROTO_IPV6, IPPROTO_IPIP, IPPROTO_ESP, IPPROTO_AH,
		IPPROTO_ICMP, IPPROTO_TCP, IPPROTO_UDP, IPPROTO_UDPLITE,
		IPPROTO_SCTP, IPPROTO_DCCP};
	unsigned int i;
	int err;

	for (i = 0; i < ARRAY_SIZE(types); ++i) {
		err = ipv6_find_hdr(skb, offset, types[i], NULL, NULL);
		if (err >= 0)
			return types[i];
		if (err != -ENOENT) {
			*hotdrop = true;
			break;
		}
	}

	return NEXTHDR_MAX;
}

static bool
length2_mt6(const struct sk_buff *skb, struct xt_action_param *par)
{
	const struct xt_length_mtinfo2 *info = par->matchinfo;
	const struct ipv6hdr *iph = ipv6_hdr(skb);
	unsigned int len = 0, l4proto;
	/* par->thoff would only set if ip6tables -p was used; so just use 0 */
	unsigned int thoff = 0;
	bool hit = true;

	if (info->flags & XT_LENGTH_LAYER3) {
		if (iph->payload_len == 0)
			/* Jumbogram */
			len = skb->len;
		else
			len = sizeof(struct ipv6hdr) + ntohs(iph->payload_len);
	} else {
		l4proto = llayer4_proto(skb, &thoff, &par->hotdrop);
		if (l4proto == NEXTHDR_MAX)
			return false;
		if (info->flags & XT_LENGTH_LAYER4)
			len = skb->len - thoff;
		else if (info->flags & XT_LENGTH_LAYER5)
			hit = xtlength_layer5(&len, skb, l4proto, thoff);
		else if (info->flags & XT_LENGTH_LAYER7)
			hit = xtlength_layer7(&len, skb, l4proto, thoff);
	}
	if (!hit)
		return false;

	return (len >= info->min && len <= info->max) ^
	       !!(info->flags & XT_LENGTH_INVERT);
}
#endif

static struct xt_match length2_mt_reg[] __read_mostly = {
	{
		.name           = "length2",
		.revision       = 2,
		.family         = NFPROTO_IPV4,
		.match          = length2_mt,
		.matchsize      = sizeof(struct xt_length_mtinfo2),
		.me             = THIS_MODULE,
	},
#ifdef WITH_IPV6
	{
		.name           = "length2",
		.revision       = 2,
		.family         = NFPROTO_IPV6,
		.match          = length2_mt6,
		.matchsize      = sizeof(struct xt_length_mtinfo2),
		.me             = THIS_MODULE,
	},
#endif
};

static int __init length2_mt_init(void)
{
	return xt_register_matches(length2_mt_reg, ARRAY_SIZE(length2_mt_reg));
}

static void __exit length2_mt_exit(void)
{
	xt_unregister_matches(length2_mt_reg, ARRAY_SIZE(length2_mt_reg));
}

module_init(length2_mt_init);
module_exit(length2_mt_exit);

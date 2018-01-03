/*
 *	"DELUDE" target extension for Xtables
 *	Copyright © Jan Engelhardt, 2007 - 2008
 *
 *	Based upon linux-2.6.18.5/net/ipv4/netfilter/ipt_REJECT.c:
 *	(C) 1999-2001 Paul `Rusty' Russell
 *	(C) 2002-2004 Netfilter Core Team <coreteam@netfilter.org>
 *
 *	xt_DELUDE acts like REJECT, but does reply with SYN-ACK on SYN.
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License version 2 as
 *	published by the Free Software Foundation.
 */
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/version.h>
#include <linux/netfilter/x_tables.h>
#ifdef CONFIG_BRIDGE_NETFILTER
#	include <linux/netfilter_bridge.h>
#endif
#include <net/tcp.h>
#include "compat_xtables.h"
#define PFX KBUILD_MODNAME ": "

static void delude_send_reset(struct net *net, struct sk_buff *oldskb,
    unsigned int hook)
{
	struct tcphdr _otcph, *tcph;
	const struct tcphdr *oth;
	const struct iphdr *oiph;
	unsigned int addr_type;
	struct sk_buff *nskb;
	struct iphdr *niph;

	oiph = ip_hdr(oldskb);

	/* IP header checks: fragment. */
	if (oiph->frag_off & htons(IP_OFFSET))
		return;

	oth = skb_header_pointer(oldskb, ip_hdrlen(oldskb),
				 sizeof(_otcph), &_otcph);
	if (oth == NULL)
		return;

	/* No RST for RST. */
	if (oth->rst)
		return;

	/* Check checksum */
	if (nf_ip_checksum(oldskb, hook, ip_hdrlen(oldskb), IPPROTO_TCP))
		return;

	nskb = alloc_skb(sizeof(struct iphdr) + sizeof(struct tcphdr) +
	                 LL_MAX_HEADER, GFP_ATOMIC);
	if (nskb == NULL)
		return;

	skb_reserve(nskb, LL_MAX_HEADER);
	skb_reset_network_header(nskb);
	niph = (struct iphdr *)skb_put(nskb, sizeof(struct iphdr));
	niph->version  = 4;
	niph->ihl      = sizeof(struct iphdr) / 4;
	niph->tos      = 0;
	niph->id       = 0;
	niph->frag_off = htons(IP_DF);
	niph->protocol = IPPROTO_TCP;
	niph->check    = 0;
	niph->saddr    = oiph->daddr;
	niph->daddr    = oiph->saddr;

	tcph = (struct tcphdr *)skb_put(nskb, sizeof(struct tcphdr));
	memset(tcph, 0, sizeof(*tcph));
	tcph->source = oth->dest;
	tcph->dest   = oth->source;
	tcph->doff   = sizeof(struct tcphdr) / 4;

	/* DELUDE essential part */
	if (oth->syn && !oth->ack && !oth->rst && !oth->fin) {
		tcph->syn     = true;
		tcph->seq     = 0;
		tcph->ack     = true;
		tcph->ack_seq = htonl(ntohl(oth->seq) + oth->syn + oth->fin +
		                oldskb->len - ip_hdrlen(oldskb) -
		                (oth->doff << 2));
	} else {
		tcph->rst = true;
		if (!oth->ack) {
			tcph->seq     = 0;
			tcph->ack     = true;
			tcph->ack_seq = htonl(ntohl(oth->seq) + oth->syn +
			                oth->fin + oldskb->len -
			                ip_hdrlen(oldskb) - (oth->doff << 2));
		} else {
			tcph->seq     = oth->ack_seq;
			tcph->ack     = false;
			tcph->ack_seq = 0;
		}
	}

	tcph->check = tcp_v4_check(sizeof(struct tcphdr), niph->saddr,
	              niph->daddr, csum_partial((char *)tcph,
	              sizeof(struct tcphdr), 0));

	addr_type = RTN_UNSPEC;
#ifdef CONFIG_BRIDGE_NETFILTER
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0)
	if (hook != NF_INET_FORWARD || (nskb->nf_bridge != NULL &&
	    nskb->nf_bridge->physoutdev))
#else
	if (hook != NF_INET_FORWARD || (nskb->nf_bridge != NULL &&
	    nskb->nf_bridge->mask & BRNF_BRIDGED))
#endif
#else
	if (hook != NF_INET_FORWARD)
#endif
		addr_type = RTN_LOCAL;

	/* ip_route_me_harder expects skb->dst to be set */
	skb_dst_set(nskb, dst_clone(skb_dst(oldskb)));

	if (ip_route_me_harder(net, nskb, addr_type))
		goto free_nskb;
	else
		niph = ip_hdr(nskb);

	niph->ttl       = dst_metric(skb_dst(nskb), RTAX_HOPLIMIT);
	nskb->ip_summed = CHECKSUM_NONE;

	/* "Never happens" */
	if (nskb->len > dst_mtu(skb_dst(nskb)))
		goto free_nskb;

	nf_ct_attach(nskb, oldskb);

	ip_local_out(net, nskb->sk, nskb);
	return;

 free_nskb:
	kfree_skb(nskb);
}

static unsigned int
delude_tg(struct sk_buff *skb, const struct xt_action_param *par)
{
	/*
	 * Sending the reset causes reentrancy within iptables - and should not pose
	 * a problem, as that is supported since Linux 2.6.35. But since we do not
	 * actually want to have a connection open, we are still going to drop it.
	 */
	delude_send_reset(par_net(par), skb,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,10,0)
	                  par->state->hook
#else
	                  par->hooknum
#endif
	                  );
	return NF_DROP;
}

static struct xt_target delude_tg_reg __read_mostly = {
	.name     = "DELUDE",
	.revision = 0,
	.family   = NFPROTO_IPV4,
	.table    = "filter",
	.hooks    = (1 << NF_INET_LOCAL_IN) | (1 << NF_INET_FORWARD),
	.proto    = IPPROTO_TCP,
	.target   = delude_tg,
	.me       = THIS_MODULE,
};

static int __init delude_tg_init(void)
{
	return xt_register_target(&delude_tg_reg);
}

static void __exit delude_tg_exit(void)
{
	xt_unregister_target(&delude_tg_reg);
}

module_init(delude_tg_init);
module_exit(delude_tg_exit);
MODULE_DESCRIPTION("Xtables: Close TCP connections after handshake");
MODULE_AUTHOR("Jan Engelhardt ");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_DELUDE");

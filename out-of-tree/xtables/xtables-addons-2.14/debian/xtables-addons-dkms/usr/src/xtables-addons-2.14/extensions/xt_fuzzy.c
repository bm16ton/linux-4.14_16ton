/*
 *	This module implements a simple TSK FLC (Takagi-Sugeno-Kang Fuzzy Logic
 *	Controller) that aims to limit, in an adaptive and flexible way, the
 *	packet rate crossing a given stream. It serves as an initial and very
 *	simple (but effective) example of how Fuzzy Logic techniques can be
 *	applied to defeat DoS attacks.
 *
 *	As a matter of fact, Fuzzy Logic can help us to insert any "behavior"
 *	into our code in a precise, adaptive and efficient manner.
 *
 *	The goal is very similar to that of "limit" match, but using techniques
 *	of Fuzzy Control, that allow us to shape the transfer functions
 *	precisely, avoiding over and undershoots - and stuff like that.
 *
 * 2002-08-10  Hime Aguiar e Oliveira Jr. <hime@engineer.com> : Initial version.
 * 2002-08-17  : Changed to eliminate floating point operations .
 * 2002-08-23  : Coding style changes .
 * 2003-04-08  Maciej Soltysiak <solt@dns.toxicilms.tv> : IPv6 Port
 */

#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/random.h>
#include <net/tcp.h>
#include <linux/netfilter/x_tables.h>
#include "xt_fuzzy.h"
#include "compat_xtables.h"

/*
 * Packet Acceptance Rate - LOW and Packet Acceptance Rate - HIGH
 * Expressed in percentage.
 */

#define PAR_LOW		1/100
#define PAR_HIGH	1

MODULE_AUTHOR("Hime Aguiar e Oliveira Junior <hime@engineer.com>");
MODULE_DESCRIPTION("Xtables: Fuzzy Logic Controller match");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_fuzzy");
MODULE_ALIAS("ip6t_fuzzy");

static uint8_t mf_high(uint32_t tx, uint32_t mini, uint32_t maxi)
{
	if (tx >= maxi)
		return 100;
	if (tx <= mini)
		return 0;
	return 100 * (tx - mini) / (maxi - mini);
}

static uint8_t mf_low(uint32_t tx, uint32_t mini, uint32_t maxi)
{
	if (tx <= mini)
		return 100;
	if (tx >= maxi)
		return 0;
	return 100 * (maxi - tx) / (maxi - mini);

}

static bool
fuzzy_mt(const struct sk_buff *skb, struct xt_action_param *par)
{
	struct xt_fuzzy_mtinfo *info = (void *)par->matchinfo;
	unsigned long amount;
	uint8_t howhigh, howlow, random_number;

	info->bytes_total += skb->len;
	++info->packets_total;
	info->present_time = jiffies;

	if (info->present_time >= info->previous_time) {
		amount = info->present_time - info->previous_time;
	} else {
		/*
		 * There was a transition: I choose to re-sample
		 * and keep the old acceptance rate...
	         */
		amount = 0;
		info->previous_time = info->present_time;
		info->bytes_total = info->packets_total = 0;
	}

	if (amount > HZ / 10) {
		/* More than 100 ms elapsed ... */

		info->mean_rate     = HZ * info->packets_total / amount;
		info->previous_time = info->present_time;
		info->bytes_total   = info->packets_total = 0;

		howhigh = mf_high(info->mean_rate, info->minimum_rate,
		          info->maximum_rate);
		howlow  = mf_low(info->mean_rate, info->minimum_rate,
		          info->maximum_rate);

		info->acceptance_rate = howhigh * PAR_LOW + PAR_HIGH * howlow;

		/*
		 * In fact, the above defuzzification would require a
		 * denominator proportional to (howhigh+howlow) but, in this
		 * particular case, that expression is constant.
		 *
		 * An imediate consequence is that it is not necessary to call
		 * both mf_high and mf_low - but to keep things understandable,
		 * I did so.
		 */
	}

	if (info->acceptance_rate < 100) {
		get_random_bytes(&random_number, sizeof(random_number));

		if (random_number <= 255 * info->acceptance_rate / 100)
			/*
			 * If within the acceptance, it can pass
			 * => do not match.
			 */
			return false;
		else
			/* It cannot pass (it matches) */
			return true;
	};

	/* acceptance_rate == 100 % => Everything passes ... */
	return false;
}

static int fuzzy_mt_check(const struct xt_mtchk_param *par)
{
	const struct xt_fuzzy_mtinfo *info = par->matchinfo;

	if (info->minimum_rate < FUZZY_MIN_RATE ||
	    info->maximum_rate > FUZZY_MAX_RATE ||
	    info->minimum_rate >= info->maximum_rate) {
		printk(KERN_INFO KBUILD_MODNAME ": bad values, please check.\n");
		return -EDOM;
	}

	return 0;
}

static struct xt_match fuzzy_mt_reg[] __read_mostly = {
	{
		.name       = "fuzzy",
		.revision   = 1,
		.family     = NFPROTO_IPV4,
		.match      = fuzzy_mt,
		.checkentry = fuzzy_mt_check,
		.matchsize  = sizeof(struct xt_fuzzy_mtinfo),
		.me         = THIS_MODULE,
	},
	{
		.name       = "fuzzy",
		.revision   = 1,
		.family     = NFPROTO_IPV6,
		.match      = fuzzy_mt,
		.checkentry = fuzzy_mt_check,
		.matchsize  = sizeof(struct xt_fuzzy_mtinfo),
		.me         = THIS_MODULE,
	},
};

static int __init fuzzy_mt_init(void)
{
	return xt_register_matches(fuzzy_mt_reg, ARRAY_SIZE(fuzzy_mt_reg));
}

static void __exit fuzzy_mt_exit(void)
{
	xt_unregister_matches(fuzzy_mt_reg, ARRAY_SIZE(fuzzy_mt_reg));
}

module_init(fuzzy_mt_init);
module_exit(fuzzy_mt_exit);

/*
 *	"fuzzy" match extension for iptables
 *	Hime Aguiar e Oliveira Jr. <hime@engineer.com>, 2002 - 2003
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License;
 *	either version 2 of the License, or any later version, as
 *	published by the Free Software Foundation.
 */
#include <getopt.h>
#include <netdb.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xtables.h>
#include "xt_fuzzy.h"
#include "compat_user.h"

static void fuzzy_mt_help(void)
{
	printf(
"fuzzy match options:\n"
"  --lower-limit number (in packets per second)\n"
"  --upper-limit number\n");
};

static const struct option fuzzy_mt_opts[] = {
	{.name = "lower-limit", .has_arg = true, .val = '1'},
	{.name = "upper-limit", .has_arg = true, .val = '2'},
	{NULL},
};

/* Initialize data structures */
static void fuzzy_mt_init(struct xt_entry_match *m)
{
	struct xt_fuzzy_mtinfo *info = (void *)m->data;

	/*
	 * Default rates (I will improve this very soon with something based
	 * on real statistics of the running machine).
	 */
	info->minimum_rate = 1000;
	info->maximum_rate = 2000;
}

#define IPT_FUZZY_OPT_MINIMUM	0x01
#define IPT_FUZZY_OPT_MAXIMUM	0x02

static int fuzzy_mt_parse(int c, char **argv, int invert, unsigned int *flags,
                          const void *entry, struct xt_entry_match **match)
{
	struct xt_fuzzy_mtinfo *info = (void *)(*match)->data;
	uint32_t num;

	switch (c) {
	case '1':
		if (invert)
			xtables_error(PARAMETER_PROBLEM,"Can't specify ! --lower-limit");
		if (*flags & IPT_FUZZY_OPT_MINIMUM)
			xtables_error(PARAMETER_PROBLEM,"Can't specify --lower-limit twice");
		if (!xtables_strtoui(optarg, NULL, &num, 1, FUZZY_MAX_RATE) || num < 1)
			xtables_error(PARAMETER_PROBLEM,"BAD --lower-limit");
		info->minimum_rate = num;
		*flags |= IPT_FUZZY_OPT_MINIMUM;
		return true;

	case '2':
		if (invert)
			xtables_error(PARAMETER_PROBLEM,"Can't specify ! --upper-limit");
		if (*flags & IPT_FUZZY_OPT_MAXIMUM)
			xtables_error(PARAMETER_PROBLEM,"Can't specify --upper-limit twice");
		if (!xtables_strtoui(optarg, NULL, &num, 1, FUZZY_MAX_RATE) || num < 1)
			xtables_error(PARAMETER_PROBLEM,"BAD --upper-limit");
		info->maximum_rate = num;
		*flags |= IPT_FUZZY_OPT_MAXIMUM;
		return true;
	}
	return false;
}

static void fuzzy_mt_check(unsigned int flags)
{
}

static void fuzzy_mt_save(const void *ip, const struct xt_entry_match *match)
{
	const struct xt_fuzzy_mtinfo *info = (const void *)match->data;

	printf(" --lower-limit %u ", info->minimum_rate);
	printf(" --upper-limit %u ", info->maximum_rate);
}

static void fuzzy_mt_print(const void *ip, const struct xt_entry_match *match,
                           int numeric)
{
	printf(" -m fuzzy");
	fuzzy_mt_save(ip, match);
}

static struct xtables_match fuzzy_mt_reg = {
	.name          = "fuzzy",
	.revision      = 1,
	.version       = XTABLES_VERSION,
	.family        = NFPROTO_UNSPEC,
	.size          = XT_ALIGN(sizeof(struct xt_fuzzy_mtinfo)),
	.userspacesize = offsetof(struct xt_fuzzy_mtinfo, packets_total),
	.help          = fuzzy_mt_help,
	.init          = fuzzy_mt_init,
	.parse         = fuzzy_mt_parse,
	.final_check   = fuzzy_mt_check,
	.print         = fuzzy_mt_print,
	.save          = fuzzy_mt_save,
	.extra_opts    = fuzzy_mt_opts,
};

static __attribute__((constructor)) void fuzzy_mt_ldr(void)
{
	xtables_register_match(&fuzzy_mt_reg);
}

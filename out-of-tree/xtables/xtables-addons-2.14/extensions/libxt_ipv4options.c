/*
 *	"ipv4options" match extension for iptables
 *	Copyright © Jan Engelhardt, 2009
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License; either
 *	version 2 of the License, or any later version, as published by the
 *	Free Software Foundation.
 */
#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <xtables.h>
#include "xt_ipv4options.h"
#include "compat_user.h"

/*
 * Overview from http://www.networksorcery.com/enp/protocol/ip.htm
 * Not providing strings for options that seem to be most distant in the past.
 */
static const char *const v4opt_names[32] = {
	[ 1] = "nop",
	[ 2] = "security",     /* RFC 1108 */
	[ 3] = "lsrr",         /* RFC 791 */
	[ 4] = "timestamp",    /* RFC 781, 791 */
	[ 7] = "record-route", /* RFC 791 */
	[ 9] = "ssrr",         /* RFC 791 */
	[11] = "mtu-probe",    /* RFC 1063 */
	[12] = "mtu-reply",    /* RFC 1063 */
	[18] = "traceroute",   /* RFC 1393 */
	[20] = "router-alert", /* RFC 2113 */
};

static void ipv4options_mt_help(void)
{
	printf(
"ipv4options match options:\n"
"--flags [!]symbol[,...]    Match presence/absence (!) of option\n"
"                           (either by name or number)\n"
"--any                      Interpret --flags as OR-combined\n\n");
}

static const struct option ipv4options_mt_opts[] = {
	{.name = "flags", .has_arg = true,  .val = 'f'},
	{.name = "any",   .has_arg = false, .val = 'a'},
	{NULL},
};

static void ipv4options_parse_flagspec(struct xt_ipv4options_mtinfo1 *info,
    char *arg)
{
	unsigned int i, opt;
	bool inv;
	char *p;

	while (true) {
		p = strchr(arg, ',');
		if (p != NULL)
			*p = '\0';

		inv = false;
		opt = 0;
		if (*arg == '!') {
			inv = true;
			++arg;
		}

		for (i = 1; i < 32;++i)
			if (v4opt_names[i] != NULL &&
			    strcmp(v4opt_names[i], arg) == 0) {
				opt = i;
				break;
			}

		if (opt == 0 &&
		    !xtables_strtoui(arg, NULL, &opt, 0, UINT8_MAX))
			xtables_error(PARAMETER_PROBLEM,
				"ipv4options: Bad option value \"%s\"", arg);

		if (opt == 0)
			xtables_error(PARAMETER_PROBLEM,
				"ipv4options: Option value may not be zero");

		info->map |= (1 << opt);
		if (inv)
			info->invert |= (1 << opt);
		if (p == NULL)
			break;
		arg = p + 1;
	}
}

static int ipv4options_mt_parse(int c, char **argv, int invert,
    unsigned int *flags, const void *entry, struct xt_entry_match **match)
{
	struct xt_ipv4options_mtinfo1 *info = (void *)(*match)->data;

	switch (c) {
	case 'a': /* --any */
		xtables_param_act(XTF_NO_INVERT, "ipv4options", "--any", invert);
		info->flags |= XT_V4OPTS_ANY;
		return true;
	case 'f': /* --flags */
		xtables_param_act(XTF_NO_INVERT, "ipv4options", "--flags", invert);
		ipv4options_parse_flagspec(info, optarg);
		return true;
	}

	return false;
}

/* no checking of *flags - no IPv4 options is also valid */

static void ipv4options_print_flags(const struct xt_ipv4options_mtinfo1 *info,
    bool numeric)
{
	uint32_t tmp = info->map;
	unsigned int i;

	for (i = 1; i < 32; ++i)
		if (tmp & (1 << i)) {
			if (info->invert & (1 << i))
				printf("!");
			if (!numeric && v4opt_names[i] != NULL)
				printf("%s", v4opt_names[i]);
			else
				printf("%u", i);
			tmp &= ~(1 << i);
			if (tmp)
				printf(",");
		}
}

static void ipv4options_mt_save(const void *ip,
    const struct xt_entry_match *match)
{
	const struct xt_ipv4options_mtinfo1 *info = (void *)match->data;

	if (info->map != 0) {
		printf(" --flags ");
		ipv4options_print_flags(info, true);
	}
	if (info->flags & XT_V4OPTS_ANY)
		printf(" --any");
	printf(" ");
}

static void ipv4options_mt_print(const void *ip,
    const struct xt_entry_match *match, int numeric)
{
	printf(" -m ipv4options");
	ipv4options_mt_save(ip, match);
}

static struct xtables_match ipv4options_mt_reg = {
	.version       = XTABLES_VERSION,
	.name          = "ipv4options",
	.revision      = 1,
	.family        = NFPROTO_IPV4,
	.size          = XT_ALIGN(sizeof(struct xt_ipv4options_mtinfo1)),
	.userspacesize = XT_ALIGN(sizeof(struct xt_ipv4options_mtinfo1)),
	.help          = ipv4options_mt_help,
	.parse         = ipv4options_mt_parse,
	.print         = ipv4options_mt_print,
	.save          = ipv4options_mt_save,
	.extra_opts    = ipv4options_mt_opts,
};

static __attribute__((constructor)) void ipv4options_mt_ldr(void)
{
	xtables_register_match(&ipv4options_mt_reg);
}

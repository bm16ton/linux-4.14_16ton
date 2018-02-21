/*
 *	"CHAOS" target extension for iptables
 *	Copyright © Jan Engelhardt, 2006 - 2008
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
#include <linux/netfilter/x_tables.h>
#include "xt_CHAOS.h"
#include "compat_user.h"

enum {
	F_DELUDE = 1 << 0,
	F_TARPIT = 1 << 1,
};

static const struct option chaos_tg_opts[] = {
	{.name = "delude", .has_arg = false, .val = 'd'},
	{.name = "tarpit", .has_arg = false, .val = 't'},
	{NULL},
};

static void chaos_tg_help(void)
{
	printf(
		"CHAOS target options:\n"
		"  --delude    Enable DELUDE processing for TCP\n"
		"  --tarpit    Enable TARPIT processing for TCP\n");
}

static int chaos_tg_parse(int c, char **argv, int invert, unsigned int *flags,
    const void *entry, struct xt_entry_target **target)
{
	struct xt_chaos_tginfo *info = (void *)((*target)->data);

	switch (c) {
	case 'd':
		info->variant = XTCHAOS_DELUDE;
		*flags |= F_DELUDE;
		return true;
	case 't':
		info->variant = XTCHAOS_TARPIT;
		*flags |= F_TARPIT;
		return true;
	}
	return false;
}

static void chaos_tg_check(unsigned int flags)
{
	if (flags == (F_DELUDE | F_TARPIT))
		/* If flags == 0x03, both were specified, which should not be. */
		xtables_error(PARAMETER_PROBLEM,
		           "CHAOS: only one of --tarpit or --delude "
		           "may be specified");
}

static void chaos_tg_save(const void *ip, const struct xt_entry_target *target)
{
	const struct xt_chaos_tginfo *info = (const void *)target->data;

	switch (info->variant) {
	case XTCHAOS_DELUDE:
		printf(" --delude ");
		break;
	case XTCHAOS_TARPIT:
		printf(" --tarpit ");
		break;
	}
}

static void chaos_tg_print(const void *ip,
    const struct xt_entry_target *target, int numeric)
{
	printf(" -j CHAOS");
	chaos_tg_save(ip, target);
}

static struct xtables_target chaos_tg_reg = {
	.version       = XTABLES_VERSION,
	.name          = "CHAOS",
	.family        = NFPROTO_IPV4,
	.size          = XT_ALIGN(sizeof(struct xt_chaos_tginfo)),
	.userspacesize = XT_ALIGN(sizeof(struct xt_chaos_tginfo)),
	.help          = chaos_tg_help,
	.parse         = chaos_tg_parse,
	.final_check   = chaos_tg_check,
	.print         = chaos_tg_print,
	.save          = chaos_tg_save,
	.extra_opts    = chaos_tg_opts,
};

static __attribute__((constructor)) void chaos_tg_ldr(void)
{
	xtables_register_target(&chaos_tg_reg);
}

/*
 *	"DELUDE" target extension for iptables
 *	Copyright © Jan Engelhardt, 2006 - 2008
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License; either
 *	version 2 of the License, or any later version, as published by the
 *	Free Software Foundation.
 */
#include <getopt.h>
#include <stdio.h>
#include <string.h>

#include <xtables.h>
#include <linux/netfilter/x_tables.h>
#include "compat_user.h"

static void delude_tg_help(void)
{
	printf("DELUDE takes no options\n");
}

static int delude_tg_parse(int c, char **argv, int invert, unsigned int *flags,
    const void *entry, struct xt_entry_target **target)
{
	return 0;
}

static void delude_tg_check(unsigned int flags)
{
}

static struct xtables_target delude_tg_reg = {
	.version       = XTABLES_VERSION,
	.name          = "DELUDE",
	.revision      = 0,
	.family        = NFPROTO_IPV4,
	.help          = delude_tg_help,
	.parse         = delude_tg_parse,
	.final_check   = delude_tg_check,
};

static __attribute__((constructor)) void delude_tg_ldr(void)
{
	xtables_register_target(&delude_tg_reg);
}

/*
 *	"DHCPMAC" target extension for iptables
 *	Copyright © Jan Engelhardt, 2008
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
#include <stdlib.h>
#include <string.h>
#include <netinet/ether.h>
#include <xtables.h>
#include "xt_DHCPMAC.h"
#include "mac.c"
#include "compat_user.h"

enum {
	F_MAC = 1 << 0,
};

static const struct option dhcpmac_tg_opts[] = {
	{.name = "set-mac", .has_arg = true, .val = 'M'},
	{NULL},
};

static void dhcpmac_tg_help(void)
{
	printf(
"DHCPMAC target options:\n"
"  --set-mac lladdr[/mask]    Set MAC address in DHCP Client Host field\n"
	);
}

static int dhcpmac_tg_parse(int c, char **argv, int invert,
    unsigned int *flags, const void *entry, struct xt_entry_target **target)
{
	struct dhcpmac_info *info = (void *)(*target)->data;

	switch (c) {
	case 'M':
		xtables_param_act(XTF_ONLY_ONCE, "DHCPMAC", "--set-mac", *flags & F_MAC);
		xtables_param_act(XTF_NO_INVERT, "DHCPMAC", "--set-mac", invert);
		if (!mac_parse(optarg, info->addr, &info->mask))
			xtables_param_act(XTF_BAD_VALUE, "DHCPMAC", "--set-mac", optarg);
		*flags |= F_MAC;
		return true;
	}

	return false;
}

static void dhcpmac_tg_check(unsigned int flags)
{
	if (flags == 0)
		xtables_error(PARAMETER_PROBLEM, "DHCPMAC target: "
		           "--set-mac parameter required");
}

static void dhcpmac_tg_save(const void *ip,
    const struct xt_entry_target *target)
{
	const struct dhcpmac_info *info = (const void *)target->data;

	if (info->invert)
		printf(" !");
	printf(" --set-mac " DH_MAC_FMT "/%u ",
	       DH_MAC_HEX(info->addr), info->mask);
}

static void dhcpmac_tg_print(const void *ip,
    const struct xt_entry_target *target, int numeric)
{
	printf(" -j DHCPMAC");
	dhcpmac_tg_save(ip, target);
}

static struct xtables_target dhcpmac_tg_reg = {
	.version       = XTABLES_VERSION,
	.name          = "DHCPMAC",
	.revision      = 0,
	.family        = NFPROTO_IPV4,
	.size          = XT_ALIGN(sizeof(struct dhcpmac_info)),
	.userspacesize = XT_ALIGN(sizeof(struct dhcpmac_info)),
	.help          = dhcpmac_tg_help,
	.parse         = dhcpmac_tg_parse,
	.final_check   = dhcpmac_tg_check,
	.print         = dhcpmac_tg_print,
	.save          = dhcpmac_tg_save,
	.extra_opts    = dhcpmac_tg_opts,
};

static __attribute__((constructor)) void dhcpmac_tg_ldr(void)
{
	xtables_register_target(&dhcpmac_tg_reg);
}

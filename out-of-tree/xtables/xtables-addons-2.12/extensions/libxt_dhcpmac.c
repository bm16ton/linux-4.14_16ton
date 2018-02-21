/*
 *	"dhcpmac" match extension for iptables
 *	Copyright © Jan Engelhardt, 2008
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License; either
 *	version 2 of the License, or any later version, as published by the
 *	Free Software Foundation.
 */
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <net/ethernet.h>
#include <xtables.h>
#include "xt_DHCPMAC.h"
#include "mac.c"
#include "compat_user.h"

enum {
	F_MAC = 1 << 0,
};

static const struct option dhcpmac_mt_opts[] = {
	{.name = "mac", .has_arg = true, .val = 'M'},
	{NULL},
};

static void dhcpmac_mt_help(void)
{
	printf(
"dhcpmac match options:\n"
"[!] --mac lladdr[/mask]    Match on MAC address in DHCP Client Host field\n"
	);
}

static int dhcpmac_mt_parse(int c, char **argv, int invert,
    unsigned int *flags, const void *entry, struct xt_entry_match **match)
{
	struct dhcpmac_info *info = (void *)(*match)->data;

	switch (c) {
	case 'M':
		xtables_param_act(XTF_ONLY_ONCE, "dhcpmac", "--mac", *flags & F_MAC);
		xtables_param_act(XTF_NO_INVERT, "dhcpmac", "--mac", invert);
		if (!mac_parse(optarg, info->addr, &info->mask))
			xtables_param_act(XTF_BAD_VALUE, "dhcpmac", "--mac", optarg);
		if (invert)
			info->invert = true;
		*flags |= F_MAC;
		return true;
	}

	return false;
}

static void dhcpmac_mt_check(unsigned int flags)
{
	if (flags == 0)
		xtables_error(PARAMETER_PROBLEM, "dhcpmac match: "
		           "--mac parameter required");
}

static void dhcpmac_mt_save(const void *ip,
    const struct xt_entry_match *match)
{
	const struct dhcpmac_info *info = (void *)match->data;

	if (info->invert)
		printf(" !");
	printf(" --mac " DH_MAC_FMT "/%u ",
	       DH_MAC_HEX(info->addr), info->mask);
}

static void dhcpmac_mt_print(const void *ip,
    const struct xt_entry_match *match, int numeric)
{
	printf(" -m dhcpmac");
	dhcpmac_mt_save(ip, match);
}

static struct xtables_match dhcpmac_mt_reg = {
	.version       = XTABLES_VERSION,
	.name          = "dhcpmac",
	.revision      = 0,
	.family        = NFPROTO_IPV4,
	.size          = XT_ALIGN(sizeof(struct dhcpmac_info)),
	.userspacesize = XT_ALIGN(sizeof(struct dhcpmac_info)),
	.help          = dhcpmac_mt_help,
	.parse         = dhcpmac_mt_parse,
	.final_check   = dhcpmac_mt_check,
	.print         = dhcpmac_mt_print,
	.save          = dhcpmac_mt_save,
	.extra_opts    = dhcpmac_mt_opts,
};

static __attribute__((constructor)) void dhcpmac_mt_ldr(void)
{
	xtables_register_match(&dhcpmac_mt_reg);
}

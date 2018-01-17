/*
 *	"gradm" match extension for iptables
 *	Zbigniew Krzystolik <zbyniu@destrukcja.pl>, 2010
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
#include "xt_gradm.h"
#include "compat_user.h"

static void gradm_mt_help(void)
{
	printf(
"gradm match options:\n"
" [!] --enabled    is Grsecurity RBAC enabled\n"
" [!] --disabled   is Grsecurity RBAC disabled\n");
};

static const struct option gradm_mt_opts[] = {
	{.name = "enabled",  .has_arg = false, .val = '1'},
	{.name = "disabled", .has_arg = false, .val = '2'},
	{NULL},
};

static void gradm_mt_init(struct xt_entry_match *m)
{
}

static int gradm_mt_parse(int c, char **argv, int invert, unsigned int *flags,
                          const void *entry, struct xt_entry_match **match)
{
	struct xt_gradm_mtinfo *info = (void *)(*match)->data;

	switch (c) {
	case '1':
		if (invert)
			info->invflags |= 1;
		return true;
	case '2':
		if (!invert)
			info->invflags |= 1;
		return true;
	}
	return false;
}

static void gradm_mt_check(unsigned int flags)
{
}

static void gradm_mt_save(const void *ip, const struct xt_entry_match *match)
{
	const struct xt_gradm_mtinfo *info = (const void *)match->data;

	if (info->invflags)
		printf(" --disabled ");
	else
		printf(" --enabled ");
}

static void gradm_mt_print(const void *ip, const struct xt_entry_match *match,
                           int numeric)
{
	printf(" -m gradm");
	gradm_mt_save(ip, match);
}

static struct xtables_match gradm_mt_reg = {
	.family        = NFPROTO_UNSPEC,
	.name          = "gradm",
	.version       = XTABLES_VERSION,
	.size          = XT_ALIGN(sizeof(struct xt_gradm_mtinfo)),
	.userspacesize = XT_ALIGN(sizeof(struct xt_gradm_mtinfo)),
	.help          = gradm_mt_help,
	.init          = gradm_mt_init,
	.parse         = gradm_mt_parse,
	.final_check   = gradm_mt_check,
	.print         = gradm_mt_print,
	.save          = gradm_mt_save,
	.extra_opts    = gradm_mt_opts,
};

static __attribute__((constructor)) void gradm_mt_ldr(void)
{
	xtables_register_match(&gradm_mt_reg);
}

/* Shared library add-on to iptables to add ACCOUNT(ing) support.
   Author: Intra2net AG <opensource@intra2net.com>
*/

#include <stdbool.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include <getopt.h>
#include <stddef.h>
#include <xtables.h>
#include "xt_ACCOUNT.h"
#include "compat_user.h"

static struct option account_tg_opts[] = {
	{.name = "addr",  .has_arg = true, .val = 'a'},
	{.name = "tname", .has_arg = true, .val = 't'},
	{NULL},
};

/* Function which prints out usage message. */
static void account_tg_help(void)
{
	printf(
"ACCOUNT target options:\n"
" --%s ip/netmask\t\tBase network IP and netmask used for this table\n"
" --%s name\t\t\tTable name for the userspace library\n",
account_tg_opts[0].name, account_tg_opts[1].name);
}

/* Initialize the target. */
static void
account_tg_init(struct xt_entry_target *t)
{
	struct ipt_acc_info *accountinfo = (struct ipt_acc_info *)t->data;

	accountinfo->table_nr = -1;
}

#define IPT_ACCOUNT_OPT_ADDR 0x01
#define IPT_ACCOUNT_OPT_TABLE 0x02

/* Function which parses command options; returns true if it
   ate an option */

static int account_tg_parse(int c, char **argv, int invert, unsigned int *flags,
		const void *entry, struct xt_entry_target **target)
{
	struct ipt_acc_info *accountinfo = (struct ipt_acc_info *)(*target)->data;
	struct in_addr *addrs = NULL, mask;
	unsigned int naddrs = 0;

	switch (c) {
	case 'a':
		if (*flags & IPT_ACCOUNT_OPT_ADDR)
			xtables_error(PARAMETER_PROBLEM, "Can't specify --%s twice",
				account_tg_opts[0].name);

		xtables_ipparse_any(optarg, &addrs, &mask, &naddrs);
		if (naddrs > 1)
			xtables_error(PARAMETER_PROBLEM, "multiple IP addresses not allowed");

		accountinfo->net_ip = addrs[0].s_addr;
		accountinfo->net_mask = mask.s_addr;

		*flags |= IPT_ACCOUNT_OPT_ADDR;
		break;

	case 't':
		if (*flags & IPT_ACCOUNT_OPT_TABLE)
			xtables_error(PARAMETER_PROBLEM,
				"Can't specify --%s twice",
				account_tg_opts[1].name);

		if (strlen(optarg) > ACCOUNT_TABLE_NAME_LEN - 1)
			xtables_error(PARAMETER_PROBLEM,
				"Maximum table name length %u for --%s",
				ACCOUNT_TABLE_NAME_LEN - 1,
				account_tg_opts[1].name);

		strcpy(accountinfo->table_name, optarg);
		*flags |= IPT_ACCOUNT_OPT_TABLE;
		break;

	default:
		return 0;
	}
	return 1;
}

static void account_tg_check(unsigned int flags)
{
	if (!(flags & IPT_ACCOUNT_OPT_ADDR) || !(flags & IPT_ACCOUNT_OPT_TABLE))
		xtables_error(PARAMETER_PROBLEM, "ACCOUNT: needs --%s and --%s",
			account_tg_opts[0].name, account_tg_opts[1].name);
}

static void account_tg_print_it(const void *ip,
		const struct xt_entry_target *target, bool do_prefix)
{
	const struct ipt_acc_info *accountinfo
		= (const struct ipt_acc_info *)target->data;
	struct in_addr a;

	if (!do_prefix)
		printf(" ACCOUNT ");

	// Network information
	if (do_prefix)
		printf(" --");
	printf("%s ", account_tg_opts[0].name);

	a.s_addr = accountinfo->net_ip;
	printf("%s", xtables_ipaddr_to_numeric(&a));
	a.s_addr = accountinfo->net_mask;
	printf("%s", xtables_ipmask_to_numeric(&a));

	printf(" ");
	if (do_prefix)
		printf(" --");

	printf("%s %s", account_tg_opts[1].name, accountinfo->table_name);
}


static void
account_tg_print(const void *ip,
	const struct xt_entry_target *target,
	int numeric)
{
	account_tg_print_it(ip, target, false);
}

/* Saves the union ipt_targinfo in parsable form to stdout. */
static void
account_tg_save(const void *ip, const struct xt_entry_target *target)
{
	account_tg_print_it(ip, target, true);
}

static struct xtables_target account_tg_reg = {
	.name          = "ACCOUNT",
	.revision      = 1,
	.family        = NFPROTO_IPV4,
	.version       = XTABLES_VERSION,
	.size          = XT_ALIGN(sizeof(struct ipt_acc_info)),
	.userspacesize = offsetof(struct ipt_acc_info, table_nr),
	.help          = account_tg_help,
	.init          = account_tg_init,
	.parse         = account_tg_parse,
	.final_check   = account_tg_check,
	.print         = account_tg_print,
	.save          = account_tg_save,
	.extra_opts    = account_tg_opts,
};

static __attribute__((constructor)) void account_tg_ldr(void)
{
	xtables_register_target(&account_tg_reg);
}

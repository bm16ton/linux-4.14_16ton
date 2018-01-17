/*
 * Shared library add-on to iptables to add Port Knocking and SPA matching
 * support.
 *
 * (C) 2006-2009 J. Federico Hernandez <fede.hernandez@gmail.com>
 * (C) 2006 Luis Floreani <luis.floreani@gmail.com>
 *
 * This program is released under the terms of GNU GPL version 2.
 */
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <xtables.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include "xt_pknock.h"
#include "compat_user.h"

static const struct option pknock_mt_opts[] = {
	/* .name, .has_arg, .flag, .val */
	{.name = "knockports",  .has_arg = true,  .val = 'k'},
	{.name = "time",        .has_arg = true,  .val = 't'},
	{.name = "autoclose",   .has_arg = true,  .val = 'a'},
	{.name = "name",        .has_arg = true,  .val = 'n'},
	{.name = "opensecret",  .has_arg = true,  .val = 'o'},
	{.name = "closesecret", .has_arg = true,  .val = 'z'},
	{.name = "strict",      .has_arg = false, .val = 'x'},
	{.name = "checkip",     .has_arg = false, .val = 'c'},
	{NULL},
};

static void pknock_mt_help(void)
{
	printf("pknock match options:\n"
		" --knockports port[,port,port,...]	"
			"Matches destination port(s).\n"
		" --time seconds\n"
			"Max allowed time between knocks.\n"
		" --autoclose minutes\n"
			"Time after which to automatically close opened\n"
			"\t\t\t\t\tport(s).\n"
		" --strict				"
			"Knocks sequence must be exact.\n"
		" --name rule_name			"
			"Rule name.\n"
		" --checkip				"
			"Matches if the source ip is in the list.\n"
		);
}

static unsigned int
parse_ports(const char *portstring, uint16_t *ports, const char *proto)
{
	char *buffer, *cp, *next;
	unsigned int i;

	buffer = strdup(portstring);
	if (buffer == NULL)
		xtables_error(OTHER_PROBLEM, "strdup failed");

	for (cp = buffer, i = 0; cp != NULL && i < XT_PKNOCK_MAX_PORTS; cp = next, ++i)
	{
		next=strchr(cp, ',');
		if (next != NULL)
			*next++ = '\0';
		ports[i] = xtables_parse_port(cp, proto);
	}

	if (cp != NULL)
		xtables_error(PARAMETER_PROBLEM, "too many ports specified");

	free(buffer);
	return i;
}

static char *
proto_to_name(uint8_t proto)
{
	switch (proto) {
	case IPPROTO_TCP:
		return "tcp";
	case IPPROTO_UDP:
		return "udp";
	default:
		return NULL;
	}
}

static const char *
check_proto(uint16_t pnum, uint8_t invflags)
{
	char *proto;

	if (invflags & XT_INV_PROTO)
		xtables_error(PARAMETER_PROBLEM, PKNOCK "only works with TCP and UDP.");

	if ((proto = proto_to_name(pnum)) != NULL)
		return proto;
	else if (pnum == 0)
		xtables_error(PARAMETER_PROBLEM, PKNOCK "needs `-p tcp' or `-p udp'");
	else
		xtables_error(PARAMETER_PROBLEM, PKNOCK "only works with TCP and UDP.");
}

static int
__pknock_parse(int c, char **argv, int invert, unsigned int *flags,
		struct xt_entry_match **match, uint16_t pnum,
		uint16_t invflags)
{
	const char *proto;
	struct xt_pknock_mtinfo *info = (void *)(*match)->data;
	unsigned int tmp;

	switch (c) {
	case 'k': /* --knockports */
		if (*flags & XT_PKNOCK_KNOCKPORT)
			xtables_error(PARAMETER_PROBLEM, PKNOCK
				"cannot use --knockports twice.\n");
		proto = check_proto(pnum, invflags);

		info->ports_count = parse_ports(optarg, info->port, proto);
		info->option |= XT_PKNOCK_KNOCKPORT;
		*flags |= XT_PKNOCK_KNOCKPORT;
#if DEBUG
		printf("ports_count: %d\n", info->ports_count);
#endif
		break;

	case 't': /* --time */
		if (*flags & XT_PKNOCK_TIME)
			xtables_error(PARAMETER_PROBLEM, PKNOCK
				"cannot use --time twice.\n");
		info->max_time = atoi(optarg);
		if (info->max_time == 0)
			xtables_error(PARAMETER_PROBLEM, PKNOCK
				"--time number must be > 0.\n");
		info->option |= XT_PKNOCK_TIME;
		*flags |= XT_PKNOCK_TIME;
		break;

        case 'a': /* --autoclose */
		if (*flags & XT_PKNOCK_AUTOCLOSE)
			xtables_error(PARAMETER_PROBLEM, PKNOCK
				"cannot use --autoclose twice.\n");
		if (!xtables_strtoui(optarg, NULL, &tmp, 0, ~0U))
			xtables_param_act(XTF_BAD_VALUE, PKNOCK,
				"--autoclose", optarg);
                info->autoclose_time = tmp;
                info->option |= XT_PKNOCK_AUTOCLOSE;
                *flags |= XT_PKNOCK_AUTOCLOSE;
                break;

	case 'n': /* --name */
		if (*flags & XT_PKNOCK_NAME)
			xtables_error(PARAMETER_PROBLEM, PKNOCK
				"cannot use --name twice.\n");
		memset(info->rule_name, 0, sizeof(info->rule_name));
		strncpy(info->rule_name, optarg, sizeof(info->rule_name) - 1);

		info->rule_name_len = strlen(info->rule_name);
		info->option |= XT_PKNOCK_NAME;
		*flags |= XT_PKNOCK_NAME;
#if DEBUG
		printf("info->rule_name: %s\n", info->rule_name);
#endif
		break;

	case 'o': /* --opensecret */
		if (*flags & XT_PKNOCK_OPENSECRET)
			xtables_error(PARAMETER_PROBLEM, PKNOCK
				"cannot use --opensecret twice.\n");
		memset(info->open_secret, 0, sizeof(info->open_secret));
		strncpy(info->open_secret, optarg, sizeof(info->open_secret) - 1);

		info->open_secret_len = strlen(info->open_secret);
		info->option |= XT_PKNOCK_OPENSECRET;
		*flags |= XT_PKNOCK_OPENSECRET;
		break;

	case 'z': /* --closesecret */
		if (*flags & XT_PKNOCK_CLOSESECRET)
			xtables_error(PARAMETER_PROBLEM, PKNOCK
				"cannot use --closesecret twice.\n");
		memset(info->close_secret, 0, sizeof(info->close_secret));
		strncpy(info->close_secret, optarg, sizeof(info->close_secret) - 1);

		info->close_secret_len = strlen(info->close_secret);
		info->option |= XT_PKNOCK_CLOSESECRET;
		*flags |= XT_PKNOCK_CLOSESECRET;
		break;

	case 'c': /* --checkip */
		if (*flags & XT_PKNOCK_CHECKIP)
			xtables_error(PARAMETER_PROBLEM, PKNOCK
				"cannot use --checkip twice.\n");
		info->option |= XT_PKNOCK_CHECKIP;
		*flags |= XT_PKNOCK_CHECKIP;
		break;

	case 'x': /* --strict */
		if (*flags & XT_PKNOCK_STRICT)
			xtables_error(PARAMETER_PROBLEM, PKNOCK
				"cannot use --strict twice.\n");
		info->option |= XT_PKNOCK_STRICT;
		*flags |= XT_PKNOCK_STRICT;
		break;

	default:
		return 0;
	}

	if (invert)
		xtables_error(PARAMETER_PROBLEM, PKNOCK "does not support invert.");

	return 1;
}

static int pknock_mt_parse(int c, char **argv, int invert, unsigned int *flags,
                		const void *e, struct xt_entry_match **match)
{
	const struct ipt_entry *entry = e;
	return __pknock_parse(c, argv, invert, flags, match,
			entry->ip.proto, entry->ip.invflags);
}

static void pknock_mt_check(unsigned int flags)
{
	if (!(flags & XT_PKNOCK_NAME))
		xtables_error(PARAMETER_PROBLEM, PKNOCK
			"--name option is required.\n");

	if (flags & XT_PKNOCK_KNOCKPORT) {
		if (flags & XT_PKNOCK_CHECKIP)
			xtables_error(PARAMETER_PROBLEM, PKNOCK
				"cannot specify --knockports with --checkip.\n");
		if ((flags & XT_PKNOCK_OPENSECRET)
			&& !(flags & XT_PKNOCK_CLOSESECRET))
			xtables_error(PARAMETER_PROBLEM, PKNOCK
				"--opensecret must go with --closesecret.\n");
		if ((flags & XT_PKNOCK_CLOSESECRET)
			&& !(flags & XT_PKNOCK_OPENSECRET))
			xtables_error(PARAMETER_PROBLEM, PKNOCK
				"--closesecret must go with --opensecret.\n");
	}

	if (flags & XT_PKNOCK_CHECKIP) {
		if (flags & XT_PKNOCK_KNOCKPORT)
			xtables_error(PARAMETER_PROBLEM, PKNOCK
				"cannot specify --checkip with --knockports.\n");
		if ((flags & XT_PKNOCK_OPENSECRET)
			|| (flags & XT_PKNOCK_CLOSESECRET))
			xtables_error(PARAMETER_PROBLEM, PKNOCK
				"cannot specify --opensecret and"
				" --closesecret with --checkip.\n");
		if (flags & XT_PKNOCK_TIME)
			xtables_error(PARAMETER_PROBLEM, PKNOCK
				"cannot specify --time with --checkip.\n");
		if (flags & XT_PKNOCK_AUTOCLOSE)
			xtables_error(PARAMETER_PROBLEM, PKNOCK
				"cannot specify --autoclose with --checkip.\n");
	} else if (!(flags & (XT_PKNOCK_OPENSECRET | XT_PKNOCK_TIME))) {
		xtables_error(PARAMETER_PROBLEM, PKNOCK
			"you must specify --time.\n");
	}
}

static void pknock_mt_print(const void *ip,
						const struct xt_entry_match *match, int numeric)
{
	const struct xt_pknock_mtinfo *info = (void *)match->data;
	int i;

	printf(" pknock ");
	if (info->option & XT_PKNOCK_KNOCKPORT) {
		printf("knockports ");
		for (i = 0; i < info->ports_count; ++i)
			printf("%s%d", i ? "," : "", info->port[i]);
		printf(" ");
	}
	if (info->option & XT_PKNOCK_TIME)
		printf("time %ld ", (long)info->max_time);
	if (info->option & XT_PKNOCK_AUTOCLOSE)
		printf("autoclose %lu ", (unsigned long)info->autoclose_time);
	if (info->option & XT_PKNOCK_NAME)
		printf("name %s ", info->rule_name);
	if (info->option & XT_PKNOCK_OPENSECRET)
		printf("opensecret ");
	if (info->option & XT_PKNOCK_CLOSESECRET)
		printf("closesecret ");
	if (info->option & XT_PKNOCK_STRICT)
		printf("strict ");
	if (info->option & XT_PKNOCK_CHECKIP)
		printf("checkip ");
}

static void pknock_mt_save(const void *ip, const struct xt_entry_match *match)
{
	int i;
	const struct xt_pknock_mtinfo *info = (void *)match->data;

	if (info->option & XT_PKNOCK_KNOCKPORT) {
		printf(" --knockports ");
		for (i = 0; i < info->ports_count; ++i)
			printf("%s%d", i ? "," : "", info->port[i]);
		printf(" ");
	}
	if (info->option & XT_PKNOCK_TIME)
		printf(" --time %ld ", (long)info->max_time);
	if (info->option & XT_PKNOCK_AUTOCLOSE)
		printf(" --autoclose %lu ",
		       (unsigned long)info->autoclose_time);
	if (info->option & XT_PKNOCK_NAME)
		printf(" --name %s ", info->rule_name);
	if (info->option & XT_PKNOCK_OPENSECRET)
		printf(" --opensecret ");
	if (info->option & XT_PKNOCK_CLOSESECRET)
		printf(" --closesecret ");
	if (info->option & XT_PKNOCK_STRICT)
		printf(" --strict ");
	if (info->option & XT_PKNOCK_CHECKIP)
		printf(" --checkip ");
}

static struct xtables_match pknock_mt_reg = {
	.name		= "pknock",
	.version	= XTABLES_VERSION,
	.revision      = 1,
	.family        = NFPROTO_IPV4,
	.size          = XT_ALIGN(sizeof(struct xt_pknock_mtinfo)),
	.userspacesize = XT_ALIGN(sizeof(struct xt_pknock_mtinfo)),
	.help          = pknock_mt_help,
	.parse         = pknock_mt_parse,
	.final_check   = pknock_mt_check,
	.print         = pknock_mt_print,
	.save          = pknock_mt_save,
	.extra_opts    = pknock_mt_opts,
};

static __attribute__((constructor)) void pknock_mt_ldr(void)
{
	xtables_register_match(&pknock_mt_reg);
}

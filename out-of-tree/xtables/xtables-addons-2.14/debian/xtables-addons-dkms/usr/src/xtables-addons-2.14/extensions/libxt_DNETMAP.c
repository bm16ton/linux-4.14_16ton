/* Shared library add-on to iptables to add DNETMAP support.
 * (C) 2010 Marek Kierdelewicz <marek@koba.pl>
 *
 * uses some code from libipt_NETMAP by:
 * Svenning Soerensen <svenning@post5.tele.dk>
 */

#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <xtables.h>
#include <linux/netfilter/nf_nat.h>
#include "xt_DNETMAP.h"

#define MODULENAME "DNETMAP"

static const struct option DNETMAP_opts[] = {
	{"prefix", 1, NULL, 'p'},
	{"reuse", 0, NULL, 'r'},
	{"ttl", 1, NULL, 't'},
	{"static", 0, NULL, 's'},
	{"persistent", 0, NULL, 'e'},
	{.name = NULL}
};

static void DNETMAP_help(void)
{
	printf(MODULENAME " target options:\n"
	       "  --%s address[/mask]\n"
	       "    Network subnet to map to. If not specified, all existing prefixes are used.\n"
	       "  --%s\n"
	       "    Reuse entry for given prenat-ip from any prefix despite bindings ttl < 0.\n"
	       "  --%s seconds\n"
	       "    Regenerate bindings ttl value to seconds. If negative value is specified,\n"
	       "    bindings ttl is kept unchanged. If not specified then default ttl value (600s)\n"
		"    is used\n"
		"  --%s\n"
		"    Match only static entries for this rule. Dynamic entries won't be created.\n"
		"  --%s\n"
		"    Set prefix persistent. It won't be removed after deleting last iptables rule.\n\n",
		DNETMAP_opts[0].name, DNETMAP_opts[1].name,
		DNETMAP_opts[2].name, DNETMAP_opts[3].name,
		DNETMAP_opts[4].name);
}

static u_int32_t bits2netmask(int bits)
{
	u_int32_t netmask, bm;

	if (bits >= 32 || bits < 0)
		return ~0;
	for (netmask = 0, bm = 0x80000000; bits; bits--, bm >>= 1)
		netmask |= bm;
	return htonl(netmask);
}

static int netmask2bits(u_int32_t netmask)
{
	u_int32_t bm;
	int bits;

	netmask = ntohl(netmask);
	for (bits = 0, bm = 0x80000000; netmask & bm; netmask <<= 1)
		bits++;
	if (netmask)
		return -1;	/* holes in netmask */
	return bits;
}

/* Parses network address */
static void parse_prefix(char *arg, struct nf_nat_range *range)
{
	char *slash;
	const struct in_addr *ip;
	u_int32_t netmask;
	unsigned int bits;

	range->flags |= NF_NAT_RANGE_MAP_IPS;
	slash = strchr(arg, '/');
	if (slash)
		*slash = '\0';

	ip = xtables_numeric_to_ipaddr(arg);
	if (ip == NULL)
		xtables_error(PARAMETER_PROBLEM, "Bad IP address \"%s\"\n",
			      arg);
	range->min_addr.in = *ip;
	if (slash) {
		if (strchr(slash + 1, '.')) {
			ip = xtables_numeric_to_ipmask(slash + 1);
			if (ip == NULL)
				xtables_error(PARAMETER_PROBLEM,
					      "Bad netmask \"%s\"\n",
					      slash + 1);
			netmask = ip->s_addr;
		} else {
			if (!xtables_strtoui(slash + 1, NULL, &bits, 0, 32))
				xtables_error(PARAMETER_PROBLEM,
					      "Bad netmask \"%s\"\n",
					      slash + 1);
			netmask = bits2netmask(bits);
		}
		/* Don't allow /0 (/1 is probably insane, too) */
		if (netmask == 0)
			xtables_error(PARAMETER_PROBLEM, "Netmask needed\n");
		/* Mask should be <= then /16 */
		if (bits < 16)
			xtables_error(PARAMETER_PROBLEM,
				      "Max netmask size is /16\n");
	} else
		netmask = ~0;

	if (range->min_addr.ip & ~netmask) {
		if (slash)
			*slash = '/';
		xtables_error(PARAMETER_PROBLEM, "Bad network address \"%s\"\n",
			      arg);
	}
	range->max_addr.ip = range->min_addr.ip | ~netmask;
}

static int DNETMAP_parse(int c, char **argv, int invert, unsigned int *flags,
			 const void *entry, struct xt_entry_target **target)
{
	struct xt_DNETMAP_tginfo *tginfo = (void *)(*target)->data;
	struct nf_nat_range *mr = &tginfo->prefix;
	char *end;

	switch (c) {
	case 'p':
		xtables_param_act(XTF_ONLY_ONCE, MODULENAME, "--prefix",
				  *flags & XT_DNETMAP_PREFIX);
		xtables_param_act(XTF_NO_INVERT, MODULENAME, "--prefix",
				  invert);

		/* TO-DO use xtables_ipparse_any instead? */
		parse_prefix(optarg, mr);
		*flags |= XT_DNETMAP_PREFIX;
		tginfo->flags |= XT_DNETMAP_PREFIX;
		return 1;
	case 'r':
		xtables_param_act(XTF_ONLY_ONCE, MODULENAME, "--reuse",
				  *flags & XT_DNETMAP_REUSE);
		xtables_param_act(XTF_NO_INVERT, MODULENAME, "--reuse", invert);
		*flags |= XT_DNETMAP_REUSE;
		tginfo->flags |= XT_DNETMAP_REUSE;
		return 1;
	case 's':
		xtables_param_act(XTF_ONLY_ONCE, MODULENAME, "--static",
				  *flags & XT_DNETMAP_STATIC);
		xtables_param_act(XTF_NO_INVERT, MODULENAME, "--static", invert);
		*flags |= XT_DNETMAP_STATIC;
		tginfo->flags |= XT_DNETMAP_STATIC;
		return 1;
	case 'e':
		xtables_param_act(XTF_ONLY_ONCE, MODULENAME, "--persistent",
				  *flags & XT_DNETMAP_PERSISTENT);
		xtables_param_act(XTF_NO_INVERT, MODULENAME, "--persistent", invert);
		*flags |= XT_DNETMAP_PERSISTENT;
		tginfo->flags |= XT_DNETMAP_PERSISTENT;
		return 1;
	case 't':
		xtables_param_act(XTF_ONLY_ONCE, MODULENAME, "--ttl",
				  *flags & XT_DNETMAP_TTL);
		xtables_param_act(XTF_NO_INVERT, MODULENAME, "--ttl", invert);
		*flags |= XT_DNETMAP_TTL;
		tginfo->flags |= XT_DNETMAP_TTL;
		tginfo->ttl = strtol(optarg, &end, 10);
		if (*end != '\0')
			return 0;
		return 1;
	default:
		return 0;
	}
}

static void DNETMAP_print_addr(const void *ip,
			       const struct xt_entry_target *target,
			       int numeric)
{
	struct xt_DNETMAP_tginfo *tginfo = (void *)&target->data;
	const struct nf_nat_range *r = &tginfo->prefix;
	struct in_addr a;
	int bits;

	a = r->min_addr.in;
	printf("%s", xtables_ipaddr_to_numeric(&a));
	a.s_addr = ~(r->min_addr.ip ^ r->max_addr.ip);
	bits = netmask2bits(a.s_addr);
	if (bits < 0)
		printf("/%s", xtables_ipaddr_to_numeric(&a));
	else
		printf("/%d", bits);
}

static void DNETMAP_save(const void *ip, const struct xt_entry_target *target)
{
	struct xt_DNETMAP_tginfo *tginfo = (void *)&target->data;
	const __u8 *flags = &tginfo->flags;

	if (*flags & XT_DNETMAP_PREFIX) {
		printf(" --%s ", DNETMAP_opts[0].name);
		DNETMAP_print_addr(ip, target, 0);
	}

	if (*flags & XT_DNETMAP_REUSE)
		printf(" --reuse ");

	if (*flags & XT_DNETMAP_STATIC)
		printf(" --static ");

	if (*flags & XT_DNETMAP_PERSISTENT)
		printf(" --persistent ");

	/* ommited because default value can change as kernel mod param */
	if (*flags & XT_DNETMAP_TTL)
		printf(" --ttl %i ", tginfo->ttl);
}

static void DNETMAP_print(const void *ip, const struct xt_entry_target *target,
			  int numeric)
{
	printf(" -j DNETMAP");
	DNETMAP_save(ip, target);
}

static struct xtables_target dnetmap_tg_reg = {
	.name          = MODULENAME,
	.version       = XTABLES_VERSION,
	.family        = NFPROTO_IPV4,
	.size          = XT_ALIGN(sizeof(struct xt_DNETMAP_tginfo)),
	.userspacesize = XT_ALIGN(sizeof(struct xt_DNETMAP_tginfo)),
	.help          = DNETMAP_help,
	.parse         = DNETMAP_parse,
	.print         = DNETMAP_print,
	.save          = DNETMAP_save,
	.extra_opts    = DNETMAP_opts,
};

static void _init(void)
{
	xtables_register_target(&dnetmap_tg_reg);
}

#include <getopt.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xtables.h>
#include "xt_length2.h"
#include "compat_user.h"

enum {
	F_LAYER  = 1 << 0,
	F_LENGTH = 1 << 1,

	XT_LENGTH_LAYER_MASK = XT_LENGTH_LAYER3 | XT_LENGTH_LAYER4 |
	                       XT_LENGTH_LAYER5 | XT_LENGTH_LAYER7,
};

static void length_mt_help(void)
{
	printf(
"length match options:\n"
"    --layer3          Match against layer3 size (e.g. L4 + IPv6 header)\n"
"    --layer4          Match against layer4 size (e.g. L5 + SCTP header)\n"
"    --layer5          Match against layer5 size (e.g. L7 + chunk headers)\n"
"    --layer7          Match against layer7 payload (e.g. SCTP payload)\n"
"[!] --length n[:n]    Match packet length against value or range\n"
"                      of values (inclusive)\n"
);
}

static const struct option length_mt_opts[] = {
	{.name = "layer3", .has_arg = false, .val = '3'},
	{.name = "layer4", .has_arg = false, .val = '4'},
	{.name = "layer5", .has_arg = false, .val = '5'},
	{.name = "layer7", .has_arg = false, .val = '7'},
	{.name = "length", .has_arg = true,  .val = '='},
	{NULL},
};

static void length_mt_init(struct xt_entry_match *match)
{
	struct xt_length_mtinfo2 *info = (void *)match->data;

	info->flags = XT_LENGTH_LAYER3;
}

static int length_mt_parse(int c, char **argv, int invert, unsigned int *flags,
                           const void *entry, struct xt_entry_match **match)
{
	struct xt_length_mtinfo2 *info = (void *)(*match)->data;
	unsigned int from, to;
	char *end;

	switch (c) {
	case '3': /* --layer3 */
		xtables_param_act(XTF_ONLY_ONCE, "length", "--layer*", *flags & F_LAYER);
		info->flags &= ~XT_LENGTH_LAYER_MASK;
		info->flags |= XT_LENGTH_LAYER3;
		*flags |= F_LAYER;
		return true;
	case '4': /* --layer4 */
		xtables_param_act(XTF_ONLY_ONCE, "length", "--layer*", *flags & F_LAYER);
		info->flags &= ~XT_LENGTH_LAYER_MASK;
		info->flags |= XT_LENGTH_LAYER4;
		*flags |= F_LAYER;
		return true;
	case '5': /* --layer5 */
		xtables_param_act(XTF_ONLY_ONCE, "length", "--layer*", *flags & F_LAYER);
		info->flags &= ~XT_LENGTH_LAYER_MASK;
		info->flags |= XT_LENGTH_LAYER5;
		*flags |= F_LAYER;
		return true;
	case '7': /* --layer7 */
		xtables_param_act(XTF_ONLY_ONCE, "length", "--layer*", *flags & F_LAYER);
		info->flags &= ~XT_LENGTH_LAYER_MASK;
		info->flags |= XT_LENGTH_LAYER7;
		*flags |= F_LAYER;
		return true;
	case '=': /* --length */
		xtables_param_act(XTF_ONLY_ONCE, "length", "--length", *flags & F_LENGTH);
		if (invert)
			info->flags |= XT_LENGTH_INVERT;
		if (!xtables_strtoui(optarg, &end, &from, 0, ~0U))
			xtables_param_act(XTF_BAD_VALUE, "length", "--length", optarg);
		to = from;
		if (*end == ':')
			if (!xtables_strtoui(end + 1, &end, &to, 0, ~0U))
				xtables_param_act(XTF_BAD_VALUE, "length",
				          "--length", optarg);
		if (*end != '\0')
			xtables_param_act(XTF_BAD_VALUE, "length", "--length", optarg);
		info->min = from;
		info->max = to;
		*flags |= F_LENGTH;
		return true;
	}
	return false;
}

static void length_mt_check(unsigned int flags)
{
	if (!(flags & F_LENGTH))
		xtables_error(PARAMETER_PROBLEM,
		           "length: You must specify \"--length\"");
	if (!(flags & F_LAYER))
		fprintf(stderr, "iptables: length match: Defaulting to "
		        "--layer3. Consider specifying it explicitly.\n");
}

static void length_mt_save(const void *ip, const struct xt_entry_match *match)
{
	const struct xt_length_mtinfo2 *info = (const void *)match->data;

	if (info->flags & XT_LENGTH_LAYER3)
		printf(" --layer3 ");
	else if (info->flags & XT_LENGTH_LAYER4)
		printf(" --layer4 ");
	else if (info->flags & XT_LENGTH_LAYER5)
		printf(" --layer5 ");
	else if (info->flags & XT_LENGTH_LAYER7)
		printf(" --layer7 ");
	if (info->flags & XT_LENGTH_INVERT)
		printf(" !");
	printf(" --length ");
	if (info->min == info->max)
		printf("%u ", (unsigned int)info->min);
	else
		printf("%u:%u ", (unsigned int)info->min,
		       (unsigned int)info->max);
}

static void length_mt_print(const void *ip, const struct xt_entry_match *match,
                            int numeric)
{
	printf(" -m length2");
	length_mt_save(ip, match);
}

static struct xtables_match length2_mt_reg = {
	.version        = XTABLES_VERSION,
	.name           = "length2",
	.revision       = 2,
	.family         = NFPROTO_UNSPEC,
	.size           = XT_ALIGN(sizeof(struct xt_length_mtinfo2)),
	.userspacesize  = XT_ALIGN(sizeof(struct xt_length_mtinfo2)),
	.init           = length_mt_init,
	.help           = length_mt_help,
	.parse          = length_mt_parse,
	.final_check    = length_mt_check,
	.print          = length_mt_print,
	.save           = length_mt_save,
	.extra_opts     = length_mt_opts,
};

static void _init(void)
{
	xtables_register_match(&length2_mt_reg);
}

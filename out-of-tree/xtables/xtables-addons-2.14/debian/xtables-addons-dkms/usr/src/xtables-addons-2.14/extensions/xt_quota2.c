/*
 * xt_quota2 - enhanced xt_quota that can count upwards and in packets
 * as a minimal accounting match.
 * by Jan Engelhardt , 2008
 *
 * Originally based on xt_quota.c:
 * 	Xtables module to enforce network quotas
 * 	Sam Johnston <samj@samj.net>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License
 *	version 2, as published by the Free Software Foundation.
 */
#include <linux/list.h>
#include <linux/module.h>
#include <linux/nsproxy.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/uidgid.h>
#include <linux/version.h>
#include <asm/atomic.h>
#include <net/net_namespace.h>
#include <net/netns/generic.h>
#include <net/dst.h>

#include <linux/netfilter/x_tables.h>
#include "xt_quota2.h"
#include "compat_xtables.h"

/**
 * @lock:	lock to protect quota writers from each other
 */
struct xt_quota_counter {
	u_int64_t quota;
	spinlock_t lock;
	struct list_head list;
	atomic_t ref;
	char name[sizeof(((struct xt_quota_mtinfo2 *)NULL)->name)];
	struct proc_dir_entry *procfs_entry;
};

struct quota2_net {
	struct list_head counter_list;
	struct proc_dir_entry *proc_xt_quota;
};

static int quota2_net_id;
static inline struct quota2_net *quota2_pernet(struct net *net)
{
	return net_generic(net, quota2_net_id);
}

static DEFINE_SPINLOCK(counter_list_lock);

static unsigned int quota_list_perms = S_IRUGO | S_IWUSR;
static unsigned int quota_list_uid   = 0;
static unsigned int quota_list_gid   = 0;
module_param_named(perms, quota_list_perms, uint, S_IRUGO | S_IWUSR);
module_param_named(uid, quota_list_uid, uint, S_IRUGO | S_IWUSR);
module_param_named(gid, quota_list_gid, uint, S_IRUGO | S_IWUSR);

static int quota_proc_show(struct seq_file *m, void *data)
{
	struct xt_quota_counter *e = m->private;

	spin_lock_bh(&e->lock);
	seq_printf(m, "%llu\n", e->quota);
	spin_unlock_bh(&e->lock);
	return 0;
}

static int quota_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, quota_proc_show, PDE_DATA(inode));
}

static ssize_t
quota_proc_write(struct file *file, const char __user *input,
                 size_t size, loff_t *loff)
{
	struct xt_quota_counter *e = PDE_DATA(file_inode(file));
	char buf[sizeof("+-18446744073709551616")];

	if (size > sizeof(buf))
		size = sizeof(buf);
	if (copy_from_user(buf, input, size) != 0)
		return -EFAULT;
	buf[sizeof(buf)-1] = '\0';
	if (size < sizeof(buf))
		buf[size] = '\0';

	if (*buf == '+') {
		int64_t temp = simple_strtoll(buf + 1, NULL, 0);
		spin_lock_bh(&e->lock);
		/* Do not let quota become negative if @tmp is very negative */
		if (temp > 0 || -temp < e->quota)
			e->quota += temp;
		else
			e->quota = 0;
		spin_unlock_bh(&e->lock);
	} else if (*buf == '-') {
		int64_t temp = simple_strtoll(buf + 1, NULL, 0);
		spin_lock_bh(&e->lock);
		/* Do not let quota become negative if @tmp is very big */
		if (temp < 0 || temp < e->quota)
			e->quota -= temp;
		else
			e->quota = 0;
		spin_unlock_bh(&e->lock);
	} else {
		spin_lock_bh(&e->lock);
		e->quota = simple_strtoull(buf, NULL, 0);
		spin_unlock_bh(&e->lock);
	}
	return size;
}

static const struct file_operations quota_proc_fops = {
	.open    = quota_proc_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.write   = quota_proc_write,
	.release = single_release,
};

static struct xt_quota_counter *
q2_new_counter(const struct xt_quota_mtinfo2 *q, bool anon)
{
	struct xt_quota_counter *e;
	unsigned int size;

	/* Do not need all the procfs things for anonymous counters. */
	size = anon ? offsetof(typeof(*e), list) : sizeof(*e);
	e = kmalloc(size, GFP_KERNEL);
	if (e == NULL)
		return NULL;

	e->quota = q->quota;
	spin_lock_init(&e->lock);
	if (!anon) {
		INIT_LIST_HEAD(&e->list);
		atomic_set(&e->ref, 1);
		strncpy(e->name, q->name, sizeof(e->name));
	}
	return e;
}

/**
 * q2_get_counter - get ref to counter or create new
 * @name:	name of counter
 */
static struct xt_quota_counter *
q2_get_counter(struct net *net, const struct xt_quota_mtinfo2 *q)
{
	struct proc_dir_entry *p;
	struct xt_quota_counter *e;
	struct quota2_net *quota2_net = quota2_pernet(net);

	if (*q->name == '\0')
		return q2_new_counter(q, true);

	spin_lock_bh(&counter_list_lock);
	list_for_each_entry(e, &quota2_net->counter_list, list)
		if (strcmp(e->name, q->name) == 0) {
			atomic_inc(&e->ref);
			spin_unlock_bh(&counter_list_lock);
			return e;
		}

	e = q2_new_counter(q, false);
	if (e == NULL)
		goto out;

	p = proc_create_data(e->name, quota_list_perms,
	                     quota2_net->proc_xt_quota,
	                     &quota_proc_fops, e);
	if (p == NULL || IS_ERR(p))
		goto out;

	e->procfs_entry = p;
	proc_set_user(p, make_kuid(&init_user_ns, quota_list_uid),
	              make_kgid(&init_user_ns, quota_list_gid));
	list_add_tail(&e->list, &quota2_net->counter_list);
	spin_unlock_bh(&counter_list_lock);
	return e;

 out:
	spin_unlock_bh(&counter_list_lock);
	kfree(e);
	return NULL;
}

static int quota_mt2_check(const struct xt_mtchk_param *par)
{
	struct xt_quota_mtinfo2 *q = par->matchinfo;

	if (q->flags & ~XT_QUOTA_MASK)
		return -EINVAL;

	q->name[sizeof(q->name)-1] = '\0';
	if (*q->name == '.' || strchr(q->name, '/') != NULL) {
		printk(KERN_ERR "xt_quota.3: illegal name\n");
		return -EINVAL;
	}

	q->master = q2_get_counter(par->net, q);
	if (q->master == NULL) {
		printk(KERN_ERR "xt_quota.3: memory alloc failure\n");
		return -ENOMEM;
	}

	return 0;
}

static void quota_mt2_destroy(const struct xt_mtdtor_param *par)
{
	struct xt_quota_mtinfo2 *q = par->matchinfo;
	struct xt_quota_counter *e = q->master;
	struct quota2_net *quota2_net = quota2_pernet(par->net);

	if (*q->name == '\0') {
		kfree(e);
		return;
	}

	spin_lock_bh(&counter_list_lock);
	if (!atomic_dec_and_test(&e->ref)) {
		spin_unlock_bh(&counter_list_lock);
		return;
	}

	list_del(&e->list);
	remove_proc_entry(e->name, quota2_net->proc_xt_quota);
	spin_unlock_bh(&counter_list_lock);
	kfree(e);
}

static bool
quota_mt2(const struct sk_buff *skb, struct xt_action_param *par)
{
	struct xt_quota_mtinfo2 *q = (void *)par->matchinfo;
	struct xt_quota_counter *e = q->master;
	bool ret = q->flags & XT_QUOTA_INVERT;

	spin_lock_bh(&e->lock);
	if (q->flags & XT_QUOTA_GROW) {
		/*
		 * While no_change is pointless in "grow" mode, we will
		 * implement it here simply to have a consistent behavior.
		 */
		if (!(q->flags & XT_QUOTA_NO_CHANGE)) {
			e->quota += (q->flags & XT_QUOTA_PACKET) ? 1 : skb->len;
			q->quota = e->quota;
		}
		ret = true;
	} else {
		if (e->quota >= ((q->flags & XT_QUOTA_PACKET) ? 1 : skb->len)) {
			if (!(q->flags & XT_QUOTA_NO_CHANGE))
				e->quota -= (q->flags & XT_QUOTA_PACKET) ? 1 : skb->len;
			ret = !ret;
		} else {
			/* we do not allow even small packets from now on */
			if (!(q->flags & XT_QUOTA_NO_CHANGE))
				e->quota = 0;
		}
		q->quota = e->quota;
	}
	spin_unlock_bh(&e->lock);
	return ret;
}

static struct xt_match quota_mt2_reg[] __read_mostly = {
	{
		.name       = "quota2",
		.revision   = 3,
		.family     = NFPROTO_IPV4,
		.checkentry = quota_mt2_check,
		.match      = quota_mt2,
		.destroy    = quota_mt2_destroy,
		.matchsize  = sizeof(struct xt_quota_mtinfo2),
		.me         = THIS_MODULE,
	},
	{
		.name       = "quota2",
		.revision   = 3,
		.family     = NFPROTO_IPV6,
		.checkentry = quota_mt2_check,
		.match      = quota_mt2,
		.destroy    = quota_mt2_destroy,
		.matchsize  = sizeof(struct xt_quota_mtinfo2),
		.me         = THIS_MODULE,
	},
};

static int __net_init quota2_net_init(struct net *net)
{
	struct quota2_net *quota2_net = quota2_pernet(net);
	INIT_LIST_HEAD(&quota2_net->counter_list);

	quota2_net->proc_xt_quota = proc_mkdir("xt_quota", net->proc_net);
	if (quota2_net->proc_xt_quota == NULL)
		return -EACCES;
	return 0;
}

static void __net_exit quota2_net_exit(struct net *net)
{
	struct quota2_net *quota2_net = quota2_pernet(net);
	struct xt_quota_counter *e = NULL;
	struct list_head *pos, *q;

	remove_proc_entry("xt_quota", net->proc_net);

	/* destroy counter_list while freeing it's content */
	spin_lock_bh(&counter_list_lock);
	list_for_each_safe(pos, q, &quota2_net->counter_list) {
		e = list_entry(pos, struct xt_quota_counter, list);
		list_del(pos);
		kfree(e);
	}
	spin_unlock_bh(&counter_list_lock);
}

static struct pernet_operations quota2_net_ops = {
	.init   = quota2_net_init,
	.exit   = quota2_net_exit,
	.id     = &quota2_net_id,
	.size   = sizeof(struct quota2_net),
};

static int __init quota_mt2_init(void)
{
	int ret;
	ret = register_pernet_subsys(&quota2_net_ops);
	if (ret < 0)
		return ret;

	ret = xt_register_matches(quota_mt2_reg, ARRAY_SIZE(quota_mt2_reg));
	if (ret < 0)
		unregister_pernet_subsys(&quota2_net_ops);

	return ret;
}

static void __exit quota_mt2_exit(void)
{
	xt_unregister_matches(quota_mt2_reg, ARRAY_SIZE(quota_mt2_reg));
	unregister_pernet_subsys(&quota2_net_ops);
}

module_init(quota_mt2_init);
module_exit(quota_mt2_exit);
MODULE_DESCRIPTION("Xtables: countdown quota match; up counter");
MODULE_AUTHOR("Sam Johnston <samj@samj.net>");
MODULE_AUTHOR("Jan Engelhardt ");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_quota2");
MODULE_ALIAS("ip6t_quota2");

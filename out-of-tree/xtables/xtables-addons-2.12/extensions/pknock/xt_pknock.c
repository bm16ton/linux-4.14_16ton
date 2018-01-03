/*
 * Kernel module to implement Port Knocking and SPA matching support.
 *
 * (C) 2006-2009 J. Federico Hernandez Scarso <fede.hernandez@gmail.com>
 * (C) 2006 Luis A. Floreani <luis.floreani@gmail.com>
 *
 * This program is released under the terms of GNU GPL version 2.
 */
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#include <linux/module.h>
#include <linux/version.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/in.h>
#include <linux/list.h>
#include <linux/proc_fs.h>
#include <linux/spinlock.h>
#include <linux/jhash.h>
#include <linux/random.h>
#include <linux/proc_fs.h>
#include <linux/spinlock.h>
#include <linux/jiffies.h>
#include <linux/timer.h>
#include <linux/seq_file.h>
#include <linux/connector.h>
#include <linux/netfilter/x_tables.h>
#include <crypto/hash.h>
#include "xt_pknock.h"
#include "compat_xtables.h"

enum status {
	ST_INIT = 1,
	ST_MATCHING,
	ST_ALLOWED,
};

/**
 * @timestamp:	seconds, but not since epoch (uses jiffies/HZ)
 * @login_sec: seconds at login since the epoch
 */
struct peer {
	struct list_head head;
	__be32 ip;
	uint32_t accepted_knock_count;
	unsigned long timestamp;
	unsigned long login_sec;
	enum status status;
	uint8_t proto;
};

/**
 * @timer:	garbage collector timer
 * @max_time:	max matching time between ports
 */
struct xt_pknock_rule {
	struct list_head head;
	char rule_name[XT_PKNOCK_MAX_BUF_LEN+1];
	int rule_name_len;
	unsigned int ref_count;
	struct timer_list timer;
	struct list_head *peer_head;
	struct proc_dir_entry *status_proc;
	unsigned long max_time;
	unsigned long autoclose_time;
};

/**
 * @port:	destination port
 */
struct transport_data {
	uint8_t proto;
	uint16_t port;
	int payload_len;
	const unsigned char *payload;
};

MODULE_LICENSE("GPL");
MODULE_AUTHOR("J. Federico Hernandez Scarso, Luis A. Floreani");
MODULE_DESCRIPTION("netfilter match for Port Knocking and SPA");
MODULE_ALIAS("ipt_pknock");

enum {
	DEFAULT_GC_EXPIRATION_TIME = 65000, /* in msecs */
	DEFAULT_RULE_HASH_SIZE  = 8,
	DEFAULT_PEER_HASH_SIZE  = 16,
};

#define hashtable_for_each_safe(pos, n, head, size, i)	\
	for ((i) = 0; (i) < (size); ++(i)) \
		list_for_each_safe((pos), (n), (&head[(i)]))

#define pk_debug(msg, peer) pr_debug( \
			"(S) peer: " NIPQUAD_FMT " - %s.\n", \
			NIPQUAD((peer)->ip), msg)

static uint32_t ipt_pknock_hash_rnd;

static unsigned int rule_hashsize	= DEFAULT_RULE_HASH_SIZE;
static unsigned int peer_hashsize	= DEFAULT_PEER_HASH_SIZE;
static unsigned int gc_expir_time = DEFAULT_GC_EXPIRATION_TIME;
static int nl_multicast_group		= -1;

static struct list_head *rule_hashtable;
static struct proc_dir_entry *pde;

static DEFINE_SPINLOCK(list_lock);

static struct {
	const char *algo;
	struct crypto_shash *tfm;
	unsigned int size;
	struct shash_desc desc;
} crypto = {
	.algo	= "hmac(sha256)",
	.tfm	= NULL,
	.size	= 0
};

module_param(rule_hashsize, int, S_IRUGO);
MODULE_PARM_DESC(rule_hashsize, "Buckets in rule hash table (default: 8)");
module_param(peer_hashsize, int, S_IRUGO);
MODULE_PARM_DESC(peer_hashsize, "Buckets in peer hash table (default: 16)");
module_param(gc_expir_time, int, S_IRUGO);
MODULE_PARM_DESC(gc_expir_time, "Time until garbage collection after valid knock packet (default: 65000 msec)");
module_param(nl_multicast_group, int, S_IRUGO);
MODULE_PARM_DESC(nl_multicast_group, "Netlink multicast group number for pknock messages");

/**
 * Calculates a value from 0 to max from a hash of the arguments.
 *
 * @key
 * @len: length
 * @initval
 * @max
 * @return: a 32 bits index
 */
static inline uint32_t
pknock_hash(const void *key, uint32_t len, uint32_t initval, uint32_t max)
{
	return jhash(key, len, initval) % max;
}

/**
 * Alloc a hashtable with n buckets.
 *
 * @size
 * @return: hashtable
 */
static struct list_head *
alloc_hashtable(unsigned int size)
{
	struct list_head *hash;
	unsigned int i;

	hash = kmalloc(sizeof(*hash) * size, GFP_KERNEL);
	if (hash == NULL)
		return NULL;
	for (i = 0; i < size; ++i)
		INIT_LIST_HEAD(&hash[i]);

	return hash;
}

/**
 * This function converts the status from integer to string.
 *
 * @status
 * @return: status
 */
static inline const char *
status_itoa(enum status status)
{
	switch (status) {
		case ST_INIT: 	  	return "INIT";
		case ST_MATCHING: 	return "MATCHING";
		case ST_ALLOWED:  	return "ALLOWED";
		default: 			return "UNKNOWN";
	}
}

/**
 * @s
 * @pos
 * @return: private value used by the iterator
 */
static void *
pknock_seq_start(struct seq_file *s, loff_t *pos)
{
	const struct xt_pknock_rule *rule = s->private;

	spin_lock_bh(&list_lock);

	if (*pos >= peer_hashsize)
		return NULL;

	return rule->peer_head + *pos;
}

/**
 * @s
 * @v
 * @pos
 * @return: next value for the iterator
 */
static void *
pknock_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	const struct xt_pknock_rule *rule = s->private;

	++*pos;
	if (*pos >= peer_hashsize)
		return NULL;

	return rule->peer_head + *pos;
}

/**
 * @s
 * @v
 */
static void
pknock_seq_stop(struct seq_file *s, void *v)
{
	spin_unlock_bh(&list_lock);
}

/**
 * @s
 * @v
 * @return: 0 if OK
 */
static int
pknock_seq_show(struct seq_file *s, void *v)
{
	const struct list_head *pos, *n;
	const struct peer *peer;
	unsigned long time;
	const struct list_head *peer_head = v;

	const struct xt_pknock_rule *rule = s->private;

	list_for_each_safe(pos, n, peer_head) {
		peer = list_entry(pos, struct peer, head);

		seq_printf(s, "src=" NIPQUAD_FMT " ", NIPQUAD(peer->ip));
		seq_printf(s, "proto=%s ", (peer->proto == IPPROTO_TCP) ?
                                                "TCP" : "UDP");
		seq_printf(s, "status=%s ", status_itoa(peer->status));
		seq_printf(s, "accepted_knock_count=%lu ",
			(unsigned long)peer->accepted_knock_count);
		if (peer->status == ST_MATCHING) {
			time = 0;
			if (time_before(jiffies / HZ, peer->timestamp +
			    rule->max_time))
				time = peer->timestamp + rule->max_time -
				       jiffies / HZ;
			seq_printf(s, "expir_time=%lu [secs] ", time);
		}
		if (peer->status == ST_ALLOWED && rule->autoclose_time != 0) {
			time = 0;
			if (time_before(get_seconds(), peer->login_sec +
			    rule->autoclose_time * 60))
				time = peer->login_sec +
				       rule->autoclose_time * 60 -
				       get_seconds();
			seq_printf(s, "autoclose_time=%lu [secs] ", time);
		}
		seq_printf(s, "\n");
	}

	return 0;
}

static const struct seq_operations pknock_seq_ops = {
	.start = pknock_seq_start,
	.next = pknock_seq_next,
	.stop = pknock_seq_stop,
	.show = pknock_seq_show
};

/**
 * @inode
 * @file
 */
static int
pknock_proc_open(struct inode *inode, struct file *file)
{
	int ret = seq_open(file, &pknock_seq_ops);
	if (ret == 0) {
		struct seq_file *sf = file->private_data;
		sf->private = PDE_DATA(inode);
	}
	return ret;
}

static const struct file_operations pknock_proc_ops = {
	.owner = THIS_MODULE,
	.open = pknock_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release
};

/**
 * It updates the rule timer to execute garbage collector.
 *
 * @rule
 */
static void update_rule_gc_timer(struct xt_pknock_rule *rule)
{
	if (timer_pending(&rule->timer))
		del_timer(&rule->timer);

	rule->timer.expires = jiffies + msecs_to_jiffies(gc_expir_time);
	add_timer(&rule->timer);
}

/**
 * @peer
 * @autoclose_time
 *
 * Returns true if autoclose due, or false if still valid.
 */
static inline bool
autoclose_time_passed(const struct peer *peer, unsigned int autoclose_time)
{
	return peer != NULL && autoclose_time != 0 && time_after(get_seconds(),
	       peer->login_sec + autoclose_time * 60);
}

/**
 * @peer
 * @max_time
 * @return: 1 time exceeded, 0 still valid
 */
static inline bool
is_interknock_time_exceeded(const struct peer *peer, unsigned int max_time)
{
	return peer != NULL && time_after(jiffies / HZ,
	       peer->timestamp + max_time);
}

/**
 * @peer
 * @return: 1 has logged, 0 otherwise
 */
static inline bool
has_logged_during_this_minute(const struct peer *peer)
{
	return peer != NULL && peer->login_sec / 60 == get_seconds() / 60;
}

/**
 * Garbage collector. It removes the old entries after tis timers have expired.
 *
 * @r: rule
 */
static void
peer_gc(unsigned long r)
{
	unsigned int i;
	struct xt_pknock_rule *rule = (struct xt_pknock_rule *)r;
	struct peer *peer;
	struct list_head *pos, *n;

	pr_debug("(S) running %s\n", __func__);
	hashtable_for_each_safe(pos, n, rule->peer_head, peer_hashsize, i) {
		peer = list_entry(pos, struct peer, head);

		/*
		 * Remove any peer whose (inter-knock) max_time
		 * or autoclose_time passed.
		 */
		if ((peer->status != ST_ALLOWED &&
		    is_interknock_time_exceeded(peer, rule->max_time)) ||
		    (peer->status == ST_ALLOWED &&
		    autoclose_time_passed(peer, rule->autoclose_time)))
		{
			pk_debug("GC-DELETED", peer);
			list_del(pos);
			kfree(peer);
		}
	}
}

/**
 * Compares length and name equality for the rules.
 */
static inline bool
rulecmp(const struct xt_pknock_mtinfo *info, const struct xt_pknock_rule *rule)
{
	if (info->rule_name_len != rule->rule_name_len)
		return false;
	if (strncmp(info->rule_name, rule->rule_name, info->rule_name_len) != 0)
		return false;
	return true;
}

/**
 * Search the rule and returns a pointer if it exists.
 *
 * @info
 * @return: rule or NULL
 */
static struct xt_pknock_rule *search_rule(const struct xt_pknock_mtinfo *info)
{
	struct xt_pknock_rule *rule;
	struct list_head *pos, *n;
	unsigned int hash = pknock_hash(info->rule_name, info->rule_name_len,
					ipt_pknock_hash_rnd, rule_hashsize);

	list_for_each_safe(pos, n, &rule_hashtable[hash]) {
		rule = list_entry(pos, struct xt_pknock_rule, head);
		if (rulecmp(info, rule))
			return rule;
	}
	return NULL;
}

/**
 * It adds a rule to list only if it doesn't exist.
 *
 * @info
 * @return: 1 success, 0 failure
 */
static bool
add_rule(struct xt_pknock_mtinfo *info)
{
	struct xt_pknock_rule *rule;
	struct list_head *pos, *n;
	unsigned int hash = pknock_hash(info->rule_name, info->rule_name_len,
                                ipt_pknock_hash_rnd, rule_hashsize);

	list_for_each_safe(pos, n, &rule_hashtable[hash]) {
		rule = list_entry(pos, struct xt_pknock_rule, head);

		if (!rulecmp(info, rule))
			continue;
		++rule->ref_count;

		if (info->option & XT_PKNOCK_OPENSECRET) {
			rule->max_time       = info->max_time;
			rule->autoclose_time = info->autoclose_time;
		}

		if (info->option & XT_PKNOCK_CHECKIP)
			pr_debug("add_rule() (AC) rule found: %s - "
				"ref_count: %d\n",
				rule->rule_name, rule->ref_count);
		return true;
	}

	rule = kmalloc(sizeof(*rule), GFP_KERNEL);
	if (rule == NULL)
		return false;

	INIT_LIST_HEAD(&rule->head);

	memset(rule->rule_name, 0, sizeof(rule->rule_name));
	strncpy(rule->rule_name, info->rule_name, info->rule_name_len);
	rule->rule_name_len = info->rule_name_len;

	rule->ref_count      = 1;
	rule->max_time       = info->max_time;
	rule->autoclose_time = info->autoclose_time;
	rule->peer_head      = alloc_hashtable(peer_hashsize);
	if (rule->peer_head == NULL)
		goto out;

	init_timer(&rule->timer);
	rule->timer.function	= peer_gc;
	rule->timer.data	= (unsigned long)rule;

	rule->status_proc = proc_create_data(info->rule_name, 0, pde,
	                    &pknock_proc_ops, rule);
	if (rule->status_proc == NULL)
		goto out;

	list_add(&rule->head, &rule_hashtable[hash]);
	pr_debug("(A) rule_name: %s - created.\n", rule->rule_name);
	return true;
 out:
	kfree(rule->peer_head);
	kfree(rule);
	return false;
}

/**
 * It removes a rule only if it exists.
 *
 * @info
 */
static void
remove_rule(struct xt_pknock_mtinfo *info)
{
	struct xt_pknock_rule *rule = NULL;
	struct list_head *pos, *n;
	struct peer *peer;
	unsigned int i;
	int found = 0;
	unsigned int hash = pknock_hash(info->rule_name, info->rule_name_len,
                                ipt_pknock_hash_rnd, rule_hashsize);

	if (list_empty(&rule_hashtable[hash]))
		return;

	list_for_each_safe(pos, n, &rule_hashtable[hash]) {
		rule = list_entry(pos, struct xt_pknock_rule, head);

		if (rulecmp(info, rule)) {
			found = 1;
			rule->ref_count--;
			break;
		}
	}
	if (!found) {
		pr_debug("(N) rule not found: %s.\n", info->rule_name);
		return;
	}
	if (rule == NULL || rule->ref_count != 0)
		return;

	hashtable_for_each_safe(pos, n, rule->peer_head, peer_hashsize, i) {
		peer = list_entry(pos, struct peer, head);
		pk_debug("DELETED", peer);
		list_del(pos);
		kfree(peer);
	}

	if (rule->status_proc != NULL)
		remove_proc_entry(info->rule_name, pde);
	pr_debug("(D) rule deleted: %s.\n", rule->rule_name);
	if (timer_pending(&rule->timer))
		del_timer(&rule->timer);

	list_del(&rule->head);
	kfree(rule->peer_head);
	kfree(rule);
}

/**
 * If peer status exist in the list it returns peer status, if not it returns NULL.
 *
 * @rule
 * @ip
 * @return: peer or NULL
 */
static struct peer *get_peer(struct xt_pknock_rule *rule, __be32 ip)
{
	struct peer *peer;
	struct list_head *pos, *n;
	unsigned int hash;

	hash = pknock_hash(&ip, sizeof(ip), ipt_pknock_hash_rnd, peer_hashsize);

	list_for_each_safe(pos, n, &rule->peer_head[hash]) {
		peer = list_entry(pos, struct peer, head);
		if (peer->ip == ip)
			return peer;
	}
	return NULL;
}

/**
 * Reset the knock sequence status of the peer.
 *
 * @peer
 */
static void reset_knock_status(struct peer *peer)
{
	peer->accepted_knock_count = 0;
	peer->status = ST_INIT;
}

/**
 * It creates a new peer matching status.
 *
 * @rule
 * @ip
 * @proto
 * @return: peer or NULL
 */
static struct peer *new_peer(__be32 ip, uint8_t proto)
{
	struct peer *peer = kmalloc(sizeof(*peer), GFP_ATOMIC);

	if (peer == NULL)
		return NULL;

	INIT_LIST_HEAD(&peer->head);
	peer->ip	= ip;
	peer->proto	= proto;
	peer->timestamp = jiffies/HZ;
	peer->login_sec = 0;
	reset_knock_status(peer);

	return peer;
}

/**
 * It adds a new peer matching status to the list.
 *
 * @peer
 * @rule
 */
static void add_peer(struct peer *peer, struct xt_pknock_rule *rule)
{
	unsigned int hash = pknock_hash(&peer->ip, sizeof(peer->ip),
                                ipt_pknock_hash_rnd, peer_hashsize);
	list_add(&peer->head, &rule->peer_head[hash]);
}

/**
 * It removes a peer matching status.
 *
 * @peer
 */
static void remove_peer(struct peer *peer)
{
	list_del(&peer->head);
	if (peer != NULL)
		kfree(peer);
}

/**
 * @peer
 * @info
 * @port
 * @return: 1 success, 0 failure
 */
static inline bool
is_first_knock(const struct peer *peer, const struct xt_pknock_mtinfo *info,
                uint16_t port)
{
	return peer == NULL && info->port[0] == port;
}

/**
 * @peer
 * @info
 * @port
 * @return: 1 success, 0 failure
 */
static inline bool
is_wrong_knock(const struct peer *peer, const struct xt_pknock_mtinfo *info,
		uint16_t port)
{
	return peer != NULL && info->port[peer->accepted_knock_count] != port;
}

/**
 * @peer
 * @info
 * @return: 1 success, 0 failure
 */
static inline bool
is_last_knock(const struct peer *peer, const struct xt_pknock_mtinfo *info)
{
	return peer != NULL && peer->accepted_knock_count == info->ports_count;
}

/**
 * @peer
 * @return: 1 success, 0 failure
 */
static inline bool
is_allowed(const struct peer *peer)
{
	return peer != NULL && peer->status == ST_ALLOWED;
}

/**
 * Sends a message to user space through netlink sockets.
 *
 * @info
 * @peer
 * @return: 1 success, 0 otherwise
 */
static bool
msg_to_userspace_nl(const struct xt_pknock_mtinfo *info,
                const struct peer *peer, int multicast_group)
{
#if defined(CONFIG_CONNECTOR) || defined(CONFIG_CONNECTOR_MODULE)
	struct cn_msg *m;
	struct xt_pknock_nl_msg msg;

	m = kmalloc(sizeof(*m) + sizeof(msg), GFP_ATOMIC);
	if (m == NULL)
		return false;

	memset(m, 0, sizeof(*m) + sizeof(msg));
	m->seq = 0;
	m->len = sizeof(msg);

	msg.peer_ip = peer->ip;
	scnprintf(msg.rule_name, info->rule_name_len + 1, info->rule_name);

	memcpy(m + 1, &msg, m->len);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 15, 0)
	cn_netlink_send(m, 0, multicast_group, GFP_ATOMIC);
#else
	cn_netlink_send(m, multicast_group, GFP_ATOMIC);
#endif

	kfree(m);
#endif
	return true;
}

/**
 * Transforms a sequence of characters to hexadecimal.
 *
 * @out: the hexadecimal result
 * @crypt: the original sequence
 * @size
 */
static void
crypt_to_hex(char *out, const char *crypt, unsigned int size)
{
	unsigned int i;
	for (i = 0; i < size; ++i) {
		unsigned char c = crypt[i];
		*out++ = '0' + ((c&0xf0)>>4) + (c>=0xa0)*('a'-'9'-1);
		*out++ = '0' + (c&0x0f) + ((c&0x0f)>=0x0a)*('a'-'9'-1);
	}
}

/**
 * Checks that the payload has the hmac(secret+ipsrc+epoch_min).
 *
 * @secret
 * @secret_len
 * @ipsrc
 * @payload
 * @payload_len
 * @return: 1 success, 0 failure
 */
static bool
has_secret(const unsigned char *secret, unsigned int secret_len, uint32_t ipsrc,
    const unsigned char *payload, unsigned int payload_len)
{
	char result[64]; // 64 bytes * 8 = 512 bits
	char *hexresult;
	unsigned int hexa_size;
	int ret;
	bool fret = false;
	unsigned int epoch_min;

	if (payload_len == 0)
		return false;

	/*
	 * hexa:  4bits
	 * ascii: 8bits
	 * hexa = ascii * 2
	 */
	hexa_size = crypto.size * 2;

	/* + 1 cause we MUST add NULL in the payload */
	if (payload_len != hexa_size + 1)
		return false;

	hexresult = kmalloc(hexa_size, GFP_ATOMIC);
	if (hexresult == NULL)
		return false;

	memset(result, 0, sizeof(result));
	memset(hexresult, 0, hexa_size);

	epoch_min = get_seconds() / 60;

	ret = crypto_shash_setkey(crypto.tfm, secret, secret_len);
	if (ret != 0) {
		printk("crypto_hash_setkey() failed ret=%d\n", ret);
		goto out;
	}

	/*
	 * The third parameter is the number of bytes INSIDE the sg!
	 * 4 bytes IP (32 bits) +
	 * 4 bytes int epoch_min (32 bits)
	 */
	if ((ret = crypto_shash_update(&crypto.desc, (const void *)&ipsrc, sizeof(ipsrc))) != 0 ||
	    (ret = crypto_shash_update(&crypto.desc, (const void *)&epoch_min, sizeof(epoch_min))) != 0 ||
	    (ret = crypto_shash_final(&crypto.desc, result)) != 0) {
		printk("crypto_shash_update/final() failed ret=%d\n", ret);
		goto out;
	}

	crypt_to_hex(hexresult, result, crypto.size);

	if (memcmp(hexresult, payload, hexa_size) != 0)
		pr_debug("secret match failed\n");
	else
		fret = true;

 out:
	kfree(hexresult);
	return fret;
}

/**
 * If the peer pass the security policy.
 *
 * @peer
 * @info
 * @payload
 * @payload_len
 * @return: 1 if pass security, 0 otherwise
 */
static bool
pass_security(struct peer *peer, const struct xt_pknock_mtinfo *info,
        const unsigned char *payload, unsigned int payload_len)
{
	if (is_allowed(peer))
		return true;

	/* The peer can't log more than once during the same minute. */
	if (has_logged_during_this_minute(peer)) {
		pk_debug("DENIED (anti-spoof protection)", peer);
		return false;
	}
	/* Check for OPEN secret */
	if (has_secret(info->open_secret,
					info->open_secret_len, peer->ip,
					payload, payload_len))
		return true;

	return false;
}

/**
 * Validates the peer and updates the peer status for an initiating or
 * in-sequence knock packet.
 *
 * @peer
 * @info
 * @rule
 * @hdr
 *
 * Returns true if allowed, false otherwise.
 */
static bool
update_peer(struct peer *peer, const struct xt_pknock_mtinfo *info,
		struct xt_pknock_rule *rule,
		const struct transport_data *hdr)
{
	unsigned long time;

	if (is_wrong_knock(peer, info, hdr->port)) {
		pk_debug("DIDN'T MATCH", peer);
		/* Peer must start the sequence from scratch. */
		if (info->option & XT_PKNOCK_STRICT)
			remove_peer(peer);

		return false;
	}

	/* If security is needed. */
	if (info->option & XT_PKNOCK_OPENSECRET ) {
		if (hdr->proto != IPPROTO_UDP && hdr->proto != IPPROTO_UDPLITE)
			return false;

		if (!pass_security(peer, info, hdr->payload, hdr->payload_len))
			return false;
	}

	/* Update the gc timer when there is a state change. */
	update_rule_gc_timer(rule);

	++peer->accepted_knock_count;

	if (is_last_knock(peer, info)) {
		peer->status = ST_ALLOWED;

		pk_debug("ALLOWED", peer);
		peer->login_sec = get_seconds();

		if (nl_multicast_group > 0)
			msg_to_userspace_nl(info, peer, nl_multicast_group);

		return true;
	}

	/* Immediate control over the maximum time between knocks. */
	if (info->option & XT_PKNOCK_TIME) {
		time = jiffies/HZ;

		if (is_interknock_time_exceeded(peer, info->max_time)) {
			pk_debug("ST_MATCHING knock received after interknock "
				"time passed => destroyed", peer);
			pr_debug("max_time: %ld - time: %ld\n",
					peer->timestamp + info->max_time,
					time);
			remove_peer(peer);
			return false;
		}
		peer->timestamp = time;
	}
	pk_debug("MATCHING", peer);
	peer->status = ST_MATCHING;
	return false;
}

/**
 * Make the peer no more ALLOWED sending a payload with a special secret for
 * closure.
 *
 * @peer
 * @info
 * @payload
 * @payload_len
 * @return: 1 if close knock, 0 otherwise
 */
static bool
is_close_knock(const struct peer *peer, const struct xt_pknock_mtinfo *info,
		const unsigned char *payload, unsigned int payload_len)
{
	/* Check for CLOSE secret. */
	if (has_secret(info->close_secret,
				info->close_secret_len, peer->ip,
				payload, payload_len))
	{
		pk_debug("BLOCKED", peer);
		return true;
	}
	return false;
}

static bool pknock_mt(const struct sk_buff *skb,
    struct xt_action_param *par)
{
	const struct xt_pknock_mtinfo *info = par->matchinfo;
	struct xt_pknock_rule *rule;
	struct peer *peer;
	const struct iphdr *iph = ip_hdr(skb);
	unsigned int hdr_len = 0;
	__be16 _ports[2];
	const __be16 *pptr;
	struct transport_data hdr = {0, 0, 0, NULL};
	bool ret = false;

	pptr = skb_header_pointer(skb, par->thoff, sizeof _ports, &_ports);
	if (pptr == NULL) {
		/* We've been asked to examine this packet, and we
		 * can't. Hence, no choice but to drop.
		 */
		par->hotdrop = true;
		return false;
	}

	hdr.port = ntohs(pptr[1]);
	hdr.proto = iph->protocol;

	switch (hdr.proto) {
	case IPPROTO_TCP:
		break;

	case IPPROTO_UDP:
	case IPPROTO_UDPLITE:
		hdr_len = (iph->ihl * 4) + sizeof(struct udphdr);
		break;
	default:
		pr_debug("IP payload protocol is neither tcp nor udp.\n");
		return false;
	}

	spin_lock_bh(&list_lock);

	/* Searches a rule from the list depending on info structure options. */
	rule = search_rule(info);
	if (rule == NULL) {
		printk(KERN_INFO PKNOCK "The rule %s doesn't exist.\n",
						info->rule_name);
		goto out;
	}

	/* Gives the peer matching status added to rule depending on ip src. */
	peer = get_peer(rule, iph->saddr);

	if (info->option & XT_PKNOCK_CHECKIP) {
		ret = is_allowed(peer);
		goto out;
	}

	if (iph->protocol == IPPROTO_UDP || iph->protocol == IPPROTO_UDPLITE) {
		hdr.payload = (void *)iph + hdr_len;
		hdr.payload_len = skb->len - hdr_len;
	}

	/* Sets, updates, removes or checks the peer matching status. */
	if (info->option & XT_PKNOCK_KNOCKPORT) {
		if ((ret = is_allowed(peer))) {
			if (info->option & XT_PKNOCK_CLOSESECRET &&
			    (iph->protocol == IPPROTO_UDP ||
			    iph->protocol == IPPROTO_UDPLITE))
			{
				if (is_close_knock(peer, info, hdr.payload, hdr.payload_len))
				{
					reset_knock_status(peer);
					ret = false;
				}
			}
			goto out;
		}

		if (is_first_knock(peer, info, hdr.port)) {
			peer = new_peer(iph->saddr, iph->protocol);
			add_peer(peer, rule);
		}

		if (peer == NULL)
			goto out;

		update_peer(peer, info, rule, &hdr);
	}

out:
	/* Handle cur.peer matching and deletion after autoclose_time passed */
	if (ret && autoclose_time_passed(peer, rule->autoclose_time)) {
		pk_debug("AUTOCLOSE TIME PASSED => BLOCKED", peer);
		ret = false;
		if (iph->protocol == IPPROTO_TCP ||
		    !has_logged_during_this_minute(peer))
			remove_peer(peer);
	}

	if (ret)
		pk_debug("PASS OK", peer);
	spin_unlock_bh(&list_lock);
	return ret;
}

#define RETURN_ERR(err) do { printk(KERN_ERR PKNOCK err); return -EINVAL; } while (false)

static int pknock_mt_check(const struct xt_mtchk_param *par)
{
	struct xt_pknock_mtinfo *info = par->matchinfo;

	/* Singleton. */
	if (rule_hashtable == NULL) {
		rule_hashtable = alloc_hashtable(rule_hashsize);
		if (rule_hashtable == NULL)
			RETURN_ERR("alloc_hashtable() error in checkentry()\n");

		get_random_bytes(&ipt_pknock_hash_rnd, sizeof (ipt_pknock_hash_rnd));
	}

	if (!(info->option & XT_PKNOCK_NAME))
		RETURN_ERR("You must specify --name option.\n");
	if (info->option & XT_PKNOCK_OPENSECRET && info->ports_count != 1)
		RETURN_ERR("--opensecret must have just one knock port\n");
	if (info->option & XT_PKNOCK_KNOCKPORT) {
		if (info->option & XT_PKNOCK_CHECKIP)
			RETURN_ERR("Can't specify --knockports with --checkip.\n");
		if (info->option & XT_PKNOCK_OPENSECRET &&
				!(info->option & XT_PKNOCK_CLOSESECRET))
			RETURN_ERR("--opensecret must go with --closesecret.\n");
		if (info->option & XT_PKNOCK_CLOSESECRET &&
				!(info->option & XT_PKNOCK_OPENSECRET))
			RETURN_ERR("--closesecret must go with --opensecret.\n");
	}

	if (info->option & XT_PKNOCK_CHECKIP) {
		if (info->option & XT_PKNOCK_KNOCKPORT)
			RETURN_ERR("Can't specify --checkip with --knockports.\n");
		if ((info->option & XT_PKNOCK_OPENSECRET) ||
				(info->option & XT_PKNOCK_CLOSESECRET))
			RETURN_ERR("Can't specify --opensecret and --closesecret"
							" with --checkip.\n");
		if (info->option & XT_PKNOCK_TIME)
			RETURN_ERR("Can't specify --time with --checkip.\n");
		if (info->option & XT_PKNOCK_AUTOCLOSE)
			RETURN_ERR("Can't specify --autoclose with --checkip.\n");
	} else if (!(info->option & (XT_PKNOCK_OPENSECRET | XT_PKNOCK_TIME))) {
		RETURN_ERR("you must specify --time.\n");
	}

	if (info->option & XT_PKNOCK_OPENSECRET &&
	    info->open_secret_len == info->close_secret_len &&
	    memcmp(info->open_secret, info->close_secret,
	    info->open_secret_len) == 0)
		RETURN_ERR("opensecret & closesecret cannot be equal.\n");

	if (!add_rule(info))
		/* should ENOMEM here */
		RETURN_ERR("add_rule() error in checkentry() function.\n");

	return 0;
}

static void pknock_mt_destroy(const struct xt_mtdtor_param *par)
{
	struct xt_pknock_mtinfo *info = par->matchinfo;
	/* Removes a rule only if it exits and ref_count is equal to 0. */
	remove_rule(info);
}

static struct xt_match xt_pknock_mt_reg __read_mostly = {
	.name		= "pknock",
	.revision   = 1,
	.family		= NFPROTO_IPV4,
	.matchsize  = sizeof(struct xt_pknock_mtinfo),
	.match      = pknock_mt,
	.checkentry = pknock_mt_check,
	.destroy    = pknock_mt_destroy,
	.me			= THIS_MODULE
};

static int __init xt_pknock_mt_init(void)
{
#if !defined(CONFIG_CONNECTOR) && !defined(CONFIG_CONNECTOR_MODULE)
	if (nl_multicast_group != -1)
		pr_info("CONFIG_CONNECTOR not present; "
		        "netlink messages disabled\n");
#endif

	if (gc_expir_time < DEFAULT_GC_EXPIRATION_TIME)
		gc_expir_time = DEFAULT_GC_EXPIRATION_TIME;
	if (request_module(crypto.algo) < 0) {
		printk(KERN_ERR PKNOCK "request_module('%s') error.\n",
                        crypto.algo);
		return -ENXIO;
	}

	crypto.tfm = crypto_alloc_shash(crypto.algo, 0, 0);
	if (IS_ERR(crypto.tfm)) {
		printk(KERN_ERR PKNOCK "failed to load transform for %s\n",
						crypto.algo);
		return PTR_ERR(crypto.tfm);
	}

	crypto.size = crypto_shash_digestsize(crypto.tfm);
	crypto.desc.tfm = crypto.tfm;
	crypto.desc.flags = 0;

	pde = proc_mkdir("xt_pknock", init_net.proc_net);
	if (pde == NULL) {
		printk(KERN_ERR PKNOCK "proc_mkdir() error in _init().\n");
		return -ENXIO;
	}
	return xt_register_match(&xt_pknock_mt_reg);
}

static void __exit xt_pknock_mt_exit(void)
{
	remove_proc_entry("xt_pknock", init_net.proc_net);
	xt_unregister_match(&xt_pknock_mt_reg);
	kfree(rule_hashtable);
	if (crypto.tfm != NULL)
		crypto_free_shash(crypto.tfm);
}

module_init(xt_pknock_mt_init);
module_exit(xt_pknock_mt_exit);

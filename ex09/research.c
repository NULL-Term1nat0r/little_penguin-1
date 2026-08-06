// mount.h

struct mnt_namespace {
	struct ns_common	ns;
	struct mount *	root;
	struct {
		struct rb_root	mounts;		 /* Protected by namespace_sem */
		struct rb_node	*mnt_last_node;	 /* last (rightmost) mount in the rbtree */
		struct rb_node	*mnt_first_node; /* first (leftmost) mount in the rbtree */
	};
	struct user_namespace	*user_ns;
	struct ucounts		*ucounts;
	wait_queue_head_t	poll;
	u64			seq_origin; /* Sequence number of origin mount namespace */
	u64 event;
#ifdef CONFIG_FSNOTIFY
	__u32			n_fsnotify_mask;
	struct fsnotify_mark_connector __rcu *n_fsnotify_marks;
#endif
	struct hlist_head	mnt_visible_mounts; /* SB_I_USERNS_VISIBLE mounts */
	unsigned int		nr_mounts; /* # of mounts in the namespace */
	unsigned int		pending_mounts;
	refcount_t		passive; /* number references not pinning @mounts */
	bool			is_anon;
} __randomize_layout;

struct mount {
	struct hlist_node mnt_hash;
	struct mount *mnt_parent;
	struct dentry *mnt_mountpoint;
	struct vfsmount mnt;
	union {
		struct rb_node mnt_node; /* node in the ns->mounts rbtree */
		struct rcu_head mnt_rcu;
		struct llist_node mnt_llist;
	};
#ifdef CONFIG_SMP
	struct mnt_pcp __percpu *mnt_pcp;
#else
	int mnt_count;
	int mnt_writers;
#endif
	struct list_head mnt_mounts;	/* list of children, anchored here */
	struct list_head mnt_child;	/* and going through their mnt_child */
	struct mount *mnt_next_for_sb;	/* the next two fields are hlist_node, */
	struct mount * __aligned(1) *mnt_pprev_for_sb;
					/* except that LSB of pprev is stolen */
#define WRITE_HOLD 1			/* ... for use by mnt_hold_writers() */
	const char *mnt_devname;	/* Name of device e.g. /dev/dsk/hda1 */
	struct list_head mnt_list;
	struct list_head mnt_expire;	/* link in fs-specific expiry list */
	struct list_head mnt_share;	/* circular list of shared mounts */
	struct hlist_head mnt_slave_list;/* list of slave mounts */
	struct hlist_node mnt_slave;	/* slave list entry */
	struct mount *mnt_master;	/* slave is on master->mnt_slave_list */
	/*
	 * Containing namespace (active or deactivating, non-refcounted).
	 * Normally protected by namespace_sem.
	 * Can also be accessed locklessly under RCU. RCU readers can't rely on
	 * the namespace still being active, but implicitly hold a passive
	 * reference (because an RCU delay happens between a namespace being
	 * deactivated and the corresponding passive refcount drop).
	 */
	struct mnt_namespace *mnt_ns;
	struct mountpoint *mnt_mp;	/* where is it mounted */
	union {
		struct hlist_node mnt_mp_list;	/* list mounts with the same mountpoint */
		struct hlist_node mnt_umount;
	};
#ifdef CONFIG_FSNOTIFY
	struct fsnotify_mark_connector __rcu *mnt_fsnotify_marks;
	__u32 mnt_fsnotify_mask;
	struct list_head to_notify;	/* need to queue notification */
	struct mnt_namespace *prev_ns;	/* previous namespace (NULL if none) */
#endif
	int mnt_t_flags;		/* namespace_sem-protected flags */
	int mnt_id;			/* mount identifier, reused */
	u64 mnt_id_unique;		/* mount ID unique until reboot */
	int mnt_group_id;		/* peer group identifier */
	int mnt_expiry_mark;		/* true if marked for expiry */
	struct hlist_head mnt_pins;
	struct hlist_head mnt_stuck_children;
	struct hlist_node mnt_ns_visible; /* link in ns->mnt_visible_mounts */
	struct mount *overmount;	/* mounted on ->mnt_root */
} __randomize_layout;


// ######################### seq_file.c 

// ######################### __seq_open_private

void *__seq_open_private(struct file *f, const struct seq_operations *ops,
		int psize)
{
	int rc;
	void *private;
	struct seq_file *seq;

	private = kzalloc(psize, GFP_KERNEL_ACCOUNT);
	if (private == NULL)
		goto out;

	rc = seq_open(f, ops);
	if (rc < 0)
		goto out_free;

	seq = f->private_data;
	seq->private = private;
	return private;

out_free:
	kfree(private);
out:
	return NULL;
}
EXPORT_SYMBOL(__seq_open_private);

int seq_open_private(struct file *filp, const struct seq_operations *ops,
		int psize)
{
	return __seq_open_private(filp, ops, psize) ? 0 : -ENOMEM;
}
EXPORT_SYMBOL(seq_open_private);

// ########################## __seq_puts

void __seq_puts(struct seq_file *m, const char *s)
{
	seq_write(m, s, strlen(s));
}
EXPORT_SYMBOL(__seq_puts);

// include/linux/seq_file.h

struct seq_file {
        char *buf;
        size_t size;
        size_t from;
        size_t count;
        size_t pad_until;
        loff_t index;
        loff_t read_pos;
        struct mutex lock;
        const struct seq_operations *op;
        int poll_event;
        const struct file *file;
        void *private;
};

struct seq_operations {
        void * (*start) (struct seq_file *m, loff_t *pos);
        void (*stop) (struct seq_file *m, void *v);
        void * (*next) (struct seq_file *m, void *v, loff_t *pos);
        int (*show) (struct seq_file *m, void *v);
};

// include/linux/rbtree.h


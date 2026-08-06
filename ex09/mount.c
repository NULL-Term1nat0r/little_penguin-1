#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/mnt_namespace.h>
#include <linux/nsproxy.h>
#include <linux/sched.h>
#include <linux/fs_struct.h>
#include <linux/path.h>
#include <linux/namei.h>

#include <mount.h>


struct mymounts {
	struct mnt_namespace *ns;
};


static bool mymounts_skip(struct mymounts *p, struct mount *mnt)
{
	return mnt != p->ns->root &&
	       mnt->mnt_parent == p->ns->root &&
	       mnt->mnt_mountpoint == p->ns->root->mnt.mnt_root;
}


static void *mymounts_start(struct seq_file *m, loff_t *pos)
{
	struct mymounts *p = m->private;
	struct rb_node *node;
	struct mount *mnt;
	loff_t i = 0;

	node = p->ns->mnt_first_node;

	while (node) {
		mnt = rb_entry(node, struct mount, mnt_node);

		if (!mymounts_skip(p, mnt)) {
			if (i == *pos)
				return mnt;

			i++;
		}

		node = rb_next(node);
	}

	return NULL;
}


static void *mymounts_next(struct seq_file *m,
			   void *v,
			   loff_t *pos)
{
	struct mymounts *p = m->private;
	struct mount *mnt = v;
	struct rb_node *node;

	node = rb_next(&mnt->mnt_node);

	while (node) {
		mnt = rb_entry(node, struct mount, mnt_node);

		if (!mymounts_skip(p, mnt)) {
			(*pos)++;
			return mnt;
		}

		node = rb_next(node);
	}

	return NULL;
}


static void mymounts_stop(struct seq_file *m, void *v)
{
}


static int mymounts_show(struct seq_file *m, void *v)
{
	struct mymounts *p = m->private;
	struct mount *mnt = v;
	struct path path = {
		.mnt = &mnt->mnt,
		.dentry = mnt->mnt.mnt_root,
	};

	if (mnt == p->ns->root)
		seq_puts(m, "root ");
	else
		seq_printf(m, "%s ",
			   mnt->mnt_mountpoint->d_name.name);

	seq_path(m, &path, " \t\n\\");
	seq_putc(m, '\n');

	return 0;
}


static const struct seq_operations mymounts_seq_ops = {
	.start = mymounts_start,
	.next  = mymounts_next,
	.stop  = mymounts_stop,
	.show  = mymounts_show,
};


static int mymounts_open(struct inode *inode, struct file *file)
{
	struct seq_file *m;
	struct mymounts *p;
	struct mnt_namespace *ns;
	int ret;

	ns = current->nsproxy->mnt_ns;

	ret = seq_open_private(file, &mymounts_seq_ops,
			       sizeof(struct mymounts));
	if (ret)
		return ret;

	m = file->private_data;
	p = m->private;
	p->ns = ns;

	return 0;
}


static int mymounts_release(struct inode *inode, struct file *file)
{
	return seq_release_private(inode, file);
}


static const struct proc_ops mymounts_proc_ops = {
	.proc_open    = mymounts_open,
	.proc_read    = seq_read,
	.proc_lseek   = seq_lseek,
	.proc_release = mymounts_release,
};


static int __init my_mounts_init(void)
{
	proc_create("mymounts", 0444, NULL, &mymounts_proc_ops);

	return 0;
}


static void __exit my_mounts_exit(void)
{
	remove_proc_entry("mymounts", NULL);
}


module_init(my_mounts_init);
module_exit(my_mounts_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("...");
MODULE_DESCRIPTION("List mount points");
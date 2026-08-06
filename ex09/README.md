# Assignment 09
Phew... I don’t know who wrote that last file, but wow.
Anyway, let’s get back to writing something more meaningful.
To Do
• Create a module that can list mount points on your system, with the associated
name.
• Your file must be named /proc/mymounts.
$> cat /proc/mymounts
root /
sys /sys
proc /proc
run /run
dev /dev
Turn In
• The module source code and a Makefile.
This isn’t a hard task, but it can be tricky.
Check out documentation on mount points, directory listing, and linked list traversal in
the kernel.
And above all—have fun :)

## step 1 - Research -> How is the data structure to get info about mounted filesystems ?

- Information about mounted file systems is stored in /proc by default. Kernel developers are able to create virtual files that contain information about the current mount structure

- Why don't we just parse the information for the exercise from the /proc/mounts virtual file ? It contains all info we need in order to solve the task. We could but the subject wants to understand the virtual file structure allocated in RAM.

- Another interesting side node is the mstab link located in /etc/mstab. You might heard from it. It contains the mount table. the link is pointing to a hard disk file called /proc/self/mounts
```bash
lrwxrwxrwx   1 root root              11 Aug  6 17:58 mounts -> self/mounts
```
This file is exposed by the procfs filesystem. It is a textual representation of the rb-tree mount structure

## step 2 - Find out how to traverse the rb-tree mount structure

potential functions in fs/namespace.c
```c
static void *m_start(struct seq_file *m, loff_t *pos)
{
	struct proc_mounts *p = m->private;
	struct mount *mnt;

	down_read(&namespace_sem);

	mnt = mnt_find_id_at(p->ns, *pos);
	if (mnt)
		*pos = mnt->mnt_id_unique;
	return mnt;
}

static void *m_next(struct seq_file *m, void *v, loff_t *pos)
{
	struct mount *mnt = v;
	struct rb_node *node = rb_next(&mnt->mnt_node);

	if (node) {
		struct mount *next = node_to_mount(node);
		*pos = next->mnt_id_unique;
		return next;
	}

	/*
	 * No more mounts. Set pos past current mount's ID so that if
	 * iteration restarts, mnt_find_id_at() returns NULL.
	 */
	*pos = mnt->mnt_id_unique + 1;
	return NULL;
}

static void m_stop(struct seq_file *m, void *v)
{
	up_read(&namespace_sem);
}

static int m_show(struct seq_file *m, void *v)
{
	struct proc_mounts *p = m->private;
	struct mount *r = v;
	return p->show(m, &r->mnt);
}

const struct seq_operations mounts_op = {
	.start	= m_start,
	.next	= m_next,
	.stop	= m_stop,
	.show	= m_show,
};```

struct seq_file from include/linux/seq_file.h
```c
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
```
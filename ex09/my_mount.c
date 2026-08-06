#include <linux/module.h>
#include <linux/mount.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>

static void *m_start(struct seq_file *m, loff_t *pos);
static void *m_next(struct seq_file *m, void *v, loff_t *pos);
static void m_stop(struct seq_file *m, void *v);
static int m_show(struct seq_file *m, void *v);

static const struct seq_operations my_seq_ops = {
    .start = m_start,
    .next  = m_next,
    .stop  = m_stop,
    .show  = m_show,
};

static int mymounts_open(struct inode *inode, struct file *file)
{
    return seq_open(file, &my_seq_ops);
}

static ssize_t my_mounts_read(struct file *file, char __user *buf, size_t size, loff_t *ppos){
        return seq_read(file, buf, size, ppos);
}

static loff_t my_mounts_lseek(struct file *file, loff_t offset, int whence){
        return seq_lseek(file, offset, whence);
}

static int my_mounts_release(struct inode *inode, struct file *file){
        return seq_release(inode, file);
} 


static const struct proc_ops mymounts_fops = {
    .proc_open    = mymounts_open,
    .proc_read    = my_mounts_read,
    .proc_lseek   = my_mounts_lseek,
    .proc_release = my_mounts_release,
};



static int __init init_mount(void){
        struct proc_dir_entry *proc_dir_entry;
        proc_dir_entry = proc_create("mymounts", 0444, NULL, &mymounts_fops);

        return 0;
}

static void __exit exit_mount(void){
}


module_init(init_mount);
module_exit(exit_mount);


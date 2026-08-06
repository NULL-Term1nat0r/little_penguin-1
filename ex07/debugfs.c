#include <linux/debugfs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include<linux/sched.h>
#include <linux/uaccess.h>
#include<linux/slab.h>

static struct dentry *dir;

typedef struct file_info {
	char *name;
	umode_t mode;
	const struct file_operations *fops;
} file_info_t;

static char foo_data[PAGE_SIZE];
static size_t foo_size;


static int create_directory(void)
{
	struct dentry *new_dir;

	new_dir = debugfs_create_dir("fortytwo", NULL);

	if (IS_ERR(new_dir)) {
		printk(KERN_ERR "Couldn't create directory: %ld\n",
		       PTR_ERR(new_dir));
		return PTR_ERR(new_dir);
	}

	dir = new_dir;

	printk(KERN_INFO "Directory has been successfully created\n");

	return 0;
}

static int open(struct inode *inode, struct file *file) { 
	return 0;	
}

static ssize_t id_read(struct file *file,
		       char __user *buf,
		       size_t count,
		       loff_t *offp)
{
	const char *id = "estruckm\n";
	size_t len = strlen(id);
	size_t remaining;
	size_t to_copy;

	if (*offp >= len)
		return 0;

	remaining = len - *offp;
	to_copy = min(count, remaining);

	if (copy_to_user(buf, id + *offp, to_copy))
		return -EFAULT;

	*offp += to_copy;
	

	printk(KERN_INFO "Reading id file -> estruckm\n");

	return to_copy;
}

static ssize_t foo_read(struct file *file,
			char __user *buf,
			size_t count,
			loff_t *offp)
{
	return simple_read_from_buffer(buf, count, offp,
				       foo_data, foo_size);
}

static ssize_t id_write(struct file *file,
			const char __user *buf,
			size_t count,
			loff_t *offp)
{
	char data[100];

	if (count >= sizeof(data))
		return -EINVAL;

	if (copy_from_user(data, buf, count))
		return -EFAULT;

	data[count] = '\0';

	printk(KERN_INFO "evaluating string: %s\n", data);

	if (strcmp(data, "estruckm")) {
		printk(KERN_INFO "string is not matching estruckm student login\n");
		return count;
	}

	printk(KERN_INFO "fortytwo successfully received: %s\n", data);

	return count;
}

static ssize_t foo_write(struct file *file,
			 const char __user *buf,
			 size_t count,
			 loff_t *offp)
{
	if (count > PAGE_SIZE)
		return -EINVAL;

	if (copy_from_user(foo_data, buf, count))
		return -EFAULT;

	foo_size = count;

	return count;
}



static ssize_t jiffies_read(struct file *file,
			    char __user *buf,
			    size_t count,
			    loff_t *ppos)
{
	char data[32];
	int len;

	len = snprintf(data, sizeof(data), "%lu\n", jiffies);

	return simple_read_from_buffer(buf, count, ppos, data, len);
}
static const struct file_operations id_fops = {
	.owner = THIS_MODULE,
	.open = open,
	.read = id_read,
	.write = id_write,
};

static const struct file_operations jiffies_fops = {
	.owner = THIS_MODULE,
	.open = open,
	.read = jiffies_read,
};

static const struct file_operations foo_fops = {
	.owner = THIS_MODULE,
	.open = open,
	.read = foo_read,
	.write = foo_write,
};
void create_files(void){
	struct dentry *return_ptr;
	
	file_info_t file_info[3];
	file_info[0].name = "id";
	file_info[0].mode = 0666;
	file_info[0].fops = &id_fops;
	file_info[1].name = "jiffies";
	file_info[1].mode = 0444;
	file_info[1].fops = &jiffies_fops;
	file_info[2].name = "foo";
	file_info[2].mode = 0644;
	file_info[2].fops = &foo_fops;
	
	for (int i = 0; i < 3; i++){
		return_ptr = debugfs_create_file(file_info[i].name, file_info[i].mode, 
			 	  dir, 
				  NULL, 
				  file_info[i].fops);
	}
}

int __init debugfs_init(void) {
	create_directory();
	create_files();
	return 0;
}

void __exit debugfs_exit(void){
	debugfs_remove(dir);
	// debugfs_remove_recursive(dir);
	printk(KERN_INFO "debugfs is exiting\n");
}

MODULE_LICENSE("GPL");	
module_init( debugfs_init);
module_exit(debugfs_exit);
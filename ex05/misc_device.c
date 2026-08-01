#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include<linux/sched.h>
#include <linux/uaccess.h>
#include<linux/slab.h>
#include <linux/miscdevice.h>


char msg[7];
int len;
size_t size;


void fill_data(void) { 
	sprintf(msg,"hello\n");
	len=strlen(msg);
	size=sizeof(char)*len;
}

static int misc_open(struct inode *inode, struct file *file) { 

	printk(KERN_INFO "Opening misc device\n");
	fill_data();
	return 0;	

}

static ssize_t misc_read(struct file *file, char __user *buf,
			size_t count, loff_t *offp){

	int temp,ret=size;
	if(size) {
		temp=copy_to_user(buf,msg,size);
		size=temp;
		printk(KERN_INFO "Reading device -> estruckm");
	}
	return ret;
}

static ssize_t fortytwo_write(struct file *file,
			      const char __user *buf,
			      size_t count,
			      loff_t *offp)
{
	char data[100];

	if (count >= sizeof(data))
		return -EINVAL;
	if (copy_from_user(data, buf, count))
		return -EFAULT;
	printk(KERN_INFO "evaluating string: %s\n", data);
        if (strcmp(buf, "estruckm")){
                printk(KERN_INFO "string is not matching estruckm student login");
                return count;
        }
	data[count] = '\0';

	printk(KERN_INFO "fortytwo successfully received: %s\n", data);

	return count;
}

int misc_release (struct inode *inode, struct file *file) {
	printk(KERN_INFO "Closing device");

	return 0;
}
struct file_operations  myfops = { 
	.open = misc_open,
	.read = misc_read,
        .write = fortytwo_write,
	.release = misc_release,
	};


struct miscdevice my_misc = {
	 .minor = MISC_DYNAMIC_MINOR,
	  .name="fortytwo",
	 .fops = &myfops,
};

int __init misc_init(void) {
	misc_register(&my_misc);
	printk(KERN_INFO "registered device %d\n",my_misc.minor);
	return 0;
}

void __exit misc_cleanup(void){
	misc_deregister(&my_misc);
	printk(KERN_INFO "misc device deregistered\n");
}

MODULE_LICENSE("GPL");	
module_init(misc_init);
module_exit(misc_cleanup);
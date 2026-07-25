#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eike Struckmeier");
MODULE_DESCRIPTION("My own kernel module");

static int test_hello_init(void)
{
	pr_info(KERN_INFO "%s: In init\n", __func__);
	return 0;
}

static void test_hello_exit(void)
{
    pr_info(KERN_INFO "%s: In exit\n", __func__ );
}

module_init(test_hello_init);
module_exit(test_hello_exit);

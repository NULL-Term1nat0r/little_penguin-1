#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>


// SPDX-License-Identifier: GPL-2.0
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eike Struckmeier");
MODULE_DESCRIPTION("Assignment 03");

static int do_work(int *my_int)
{
	int x;
	int y = *my_int;
	int z;

	for (x = 0; x < *my_int; x++)
		usleep_range(10, 20);

	if (y < 10)
		pr_info("We slept a long time!\n");

	z = x * y;

	return z;
}

static int __init my_init(void)
{
	int x = 10;

	x = do_work(&x);

	return 0;
}

static void __exit my_exit(void)
{
}

module_init(my_init);
module_exit(my_exit);

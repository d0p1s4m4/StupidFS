#include <linux/init.h>
#include <linux/module.h>

static int __init
stpdfs_mod_init(void)
{
	printk("Hello, world!\n");
	return 0;
}

module_init(stpdfs_mod_init);

static void __exit
stpdfs_mod_exit(void)
{
	printk("Goodbye, world!\n");
}

module_exit(stpdfs_mod_exit);

MODULE_LICENSE("BSD3");
MODULE_AUTHOR("d0p1");
MODULE_DESCRIPTION("Stupid File System");
MODULE_VERSION("1.0");
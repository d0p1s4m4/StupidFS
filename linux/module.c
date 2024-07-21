/**
 * \addtogroup driver
 * @{
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include "module.h"

/**
 * \defgroup linux Linux Kernel Module
 * @{
 */

struct dentry *
stpdfs_mount(struct file_system_type *fs_type, int flags,
							const char *devname, void *data)
{
	struct dentry *dentry;

	dentry = mount_bdev(fs_type, flags, devname, data, stpdfs_fill_super);
	if (IS_ERR(dentry))
	{
		pr_err("'%s' mount failure\n", devname);
	}
	else
	{
		pr_info("'%s' mount success\n", devname);
	}
	return (dentry);
}

void
stpdfs_kill_sb(struct super_block *sb)
{
	(void)sb;
}

static struct file_system_type stpdfs_file_system_type = {
	.owner = THIS_MODULE,
	.name = "stupidfs",
	.mount = stpdfs_mount,
	.kill_sb = stpdfs_kill_sb,
	.fs_flags = FS_REQUIRES_DEV,
	.next = NULL,
};

static int __init
stpdfs_mod_init(void)
{
	int ret;

	ret = register_filesystem(&stpdfs_file_system_type);
	if (ret != 0)
	{
		pr_err("Failed to register filesystem\n");
		goto err;
	}

	pr_info("module loaded\n");
	return (0);

err:
	return (ret);
}

module_init(stpdfs_mod_init);

static void __exit
stpdfs_mod_exit(void)
{
	if (unregister_filesystem(&stpdfs_file_system_type) != 0)
	{
		pr_err("Failed to unregister filesystem\n");
	}

	pr_info("module unloaded\n");
}

module_exit(stpdfs_mod_exit);

MODULE_LICENSE("CECILL-B or BSD3");
MODULE_AUTHOR("d0p1");
MODULE_DESCRIPTION("Stupid File System");
MODULE_VERSION("1.0");

/** @} @} */
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/buffer_head.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/statfs.h>

#include <linux/blkdev.h>
#include <linux/jbd2.h>
#include <linux/namei.h>
#include <linux/parser.h>

#include "module.h"

static struct super_operations stpdfs_super_ops = {

};

int
stpdfs_fill_super(struct super_block *sb, void *data, int silent)
{
	struct buffer_head *bh = NULL;
	struct stpdfs_sb_info *sbi = NULL;

	sb->s_magic = STPDFS_SB_MAGIC;
	sb_set_blocksize(sb, STPDFS_BLOCK_SIZE);
	sb->s_maxbytes = (u32)-1;
	sb->s_op = &stpdfs_super_ops;

	bh = sb_bread(sb, 1);
	if (bh == NULL)
	{
		return (-EIO);
	}

	if (((struct stpdfs_sb *)bh->b_data)->magic != sb->s_magic)
	{
		pr_err("Invalid magic number\n");
		brelse(bh);
		return (-EINVAL);
	}

	sbi = kzalloc(sizeof(struct stpdfs_sb), GFP_KERNEL);
	if (sbi == NULL)
	{
		brelse(bh);
		return (-ENOMEM);
	}

	memcpy(&sbi->sb, bh->b_data, sizeof(struct stpdfs_sb));
	sb->s_fs_info = sbi;

	brelse(bh);


	return (0);
}
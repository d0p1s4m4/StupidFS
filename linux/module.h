#ifndef MODULE_H
# define MODULE_H

# include "../include/stupidfs.h"

struct stpdfs_sb_info {
	struct stpdfs_sb sb;

	uint32_t freeblock;
	uint32_t freeinode;
};

int stpdfs_fill_super(struct super_block *sb, void *data, int silent);

#endif /* !MODULE_H */
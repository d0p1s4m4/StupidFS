#ifndef FS_INODE_H
# define FS_INODE_H 1

# include <stdint.h>
# include <stupidfs.h>
# include "super.h"

struct fs_inode {
	int valid;
	uint32_t inum;
	int refcount;

	struct fs_super *super;
	struct stpdfs_inode inode;

	struct fs_inode *next;
};

struct fs_inode *fs_inode_get(struct fs_super *super, uint32_t inum);
int fs_inode_update(struct fs_inode *ip);
void fs_inode_release(struct fs_inode *ip);
struct fs_inode *fs_inode_read(struct fs_inode *ip);
struct fs_inode *fs_inode_alloc(struct fs_super *super);

#endif /* !FS_INODE_H */
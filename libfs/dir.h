#ifndef FS_DIR_H
# define FS_DIR_H 1

# include <stupidfs.h>
# include "inode.h"

struct fs_dir {
	char name[STPDFS_NAME_MAX];
	uint32_t inum;

	
};

int fs_dir_link(struct fs_inode *dp, const char *name, uint32_t inum);

#endif /* !FS_DIR_H */
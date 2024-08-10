#ifndef FS_SUPER_H
# define FS_SUPER_H 1

# include <stdint.h>
# include <stupidfs.h>

struct fs_super {
	int fd;
	struct stpdfs_sb sb;
};


int fs_super_kill(struct fs_super *super);

#endif /* !FS_SUPER_H */
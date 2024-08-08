#ifndef FS_FILE_H
# define FS_FILE_H 1

# include "inode.h"

struct fs_file {
	int refcount;

	struct fs_inode *ip;

	struct fs_file *next;
};

#endif /* !FS_FILE_H */
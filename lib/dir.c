#include "stpdfs.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

int
stpdfs_dir_add_entry(int fd, struct stpdfs_sb *sb, uint32_t inonum, const char *name, uint32_t target)
{
	struct stpdfs_inode inode;

	stpdfs_read_inode(fd, inonum, &inode);
	if (!(inode.mode & STPDFS_S_IFDIR))
	{
		return (-1);
	}

	return (0);
}
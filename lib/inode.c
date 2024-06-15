#include "stpdfs.h"
#include <string.h>
#include <stdint.h>

uint32_t
stpdfs_locate_inode(uint32_t inode)
{
	return (2 + inode / STPDFS_INODES_PER_BLOCK);
}

int
stpdfs_read_inode()
{
	return (0);
}

int
stpdfs_write_inode()
{
	return (0);
}

uint32_t
stpdfd_alloc_inode(int fd, struct stpdfs_sb *sb)
{
	uint32_t start = 2;
	uint32_t blocknum;
	struct stpdfs_inode inodes[STPDFS_INODES_PER_BLOCK];
	int idx;

	blocknum = 2;

	for (blocknum = 2; (blocknum - 2) < sb->isize; blocknum++)
	{
		stpdfs_read(fd, blocknum, inodes, sizeof(struct stpdfs_inode) * STPDFS_INODES_PER_BLOCK);

		for (idx = (start % STPDFS_INODES_PER_BLOCK); idx < STPDFS_INODES_PER_BLOCK; idx++)
		{
			if (!(inodes[start].flags & STPDFS_INO_FLAG_ALOC))
			{
				inodes[start].flags |= STPDFS_INO_FLAG_ALOC;
				return (start);
			}
			start++;
		}
	}
	return (0);
}

int
stpdfs_free_inode(int fd, struct stpdfs_sb *sb, uint32_t ino)
{
	uint32_t blocknum;
	struct stpdfs_inode inodes[STPDFS_INODES_PER_BLOCK];

	blocknum = 2 + ino / STPDFS_INODES_PER_BLOCK;
	if (blocknum >= sb->fsize || (blocknum - 2) >= sb->isize)
	{
		return (-1);
	}

	sb->state = STPDFS_DIRTY;

	stpdfs_read(fd, blocknum, inodes, sizeof(struct stpdfs_inode) * STPDFS_INODES_PER_BLOCK);

	memset(&inodes[ino % STPDFS_INODES_PER_BLOCK], 0, sizeof(struct stpdfs_inode));

	stpdfs_write(fd, blocknum, inodes, sizeof(struct stpdfs_inode) * STPDFS_INODES_PER_BLOCK);

	return (0);
}
#include "stpdfs.h"
#include <time.h>

int
stpdfs_superblock_valid(const struct stpdfs_sb *sb)
{
	return (sb->magic == STPDFS_SB_MAGIC && sb->revision == STPDFS_SB_REV);
}

int
stpdfs_read_superblock(int fd, struct stpdfs_sb *sb)
{
	if (stpdfs_read(fd, 1, sb, sizeof(struct stpdfs_sb)) != sizeof(struct stpdfs_sb))
	{
		return (-1);
	}

	if (stpdfs_superblock_valid(sb))
	{
		return (0);
	}
	return (-1);
}

int
stpdfs_write_superblock(int fd, struct stpdfs_sb *sb)
{
	sb->time = time(NULL);

	if (stpdfs_write(fd, 1, sb, sizeof(struct stpdfs_sb)) != sizeof(struct stpdfs_sb))
	{
		return (-1);
	}

	return (0);
}
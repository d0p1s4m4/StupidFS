#include "stpdfs.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

int
stpdfs_dir_add_entry(int fd, uint8_t inonum, const char *name, uint32_t target)
{
	struct stpdfs_inode inode;
	struct stpdfs_dirent dirent[STPDFS_DIRENT_PER_BLOCK];
	struct stpdfs_sb sb;
	size_t max_zone;
	size_t zone;
	uint32_t blocknum;
	
	stpdfs_read_superblock(fd, &sb);
	stpdfs_read_inode(fd, inonum, &inode);
	if (!(inode.mode & STPDFS_S_IFDIR))
	{
		return (-1);
	}

	max_zone = (inode.size + sizeof(struct stpdfs_dirent)) / STPDFS_DIRENT_PER_BLOCK;
	zone = max_zone;

	if (max_zone > 2113671)
	{

	}
	if (max_zone > 16519)
	{

	}
	else if (max_zone > 135)
	{

	}
	else if (max_zone > 7)
	{
	}
	else
	{
		if (inode.zones[max_zone] == 0)
		{
			inode.zones[max_zone] = stpdfs_alloc_block(fd, &sb);
		}

		blocknum = inode.zones[max_zone];
	}

	stpdfs_read(fd, blocknum, &dirent, sizeof(struct stpdfs_dirent) * STPDFS_DIRENT_PER_BLOCK);
	dirent[inode.size % STPDFS_DIRENT_PER_BLOCK].inode = target;
	memcpy(dirent[inode.size % STPDFS_DIRENT_PER_BLOCK].filename, name, STPDFS_NAME_MAX);
	stpdfs_write(fd, blocknum, &dirent, sizeof(struct stpdfs_dirent) * STPDFS_DIRENT_PER_BLOCK);
	
	inode.size += sizeof(struct stpdfs_dirent);

	stpdfs_write_inode(fd, inonum, &inode);

	stpdfs_write(fd, 1, &sb, sizeof(struct stpdfs_sb));

	return (0);
}
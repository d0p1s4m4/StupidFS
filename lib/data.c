#include "stpdfs.h"

int
stpdfs_add_data(int fd, struct stpdfs_sb *sb, struct stpdfs_inode *inode, uint8_t *buffer, size_t size)
{
	size_t idx;
	size_t max_zone;

	if (size > STPDFS_BLOCK_SIZE)
	{
		for (idx = 0; idx < size / STPDFS_BLOCK_SIZE; idx++)
		{
			stpdfs_add_data(fd, sb, inode, buffer, 512 * idx);
		}
	}
}
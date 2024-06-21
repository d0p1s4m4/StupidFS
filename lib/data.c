#include "stpdfs.h"
#include <string.h>

static void
write_indirect(int fd, struct stpdfs_sb *sb, struct stpdfs_inode *inode, size_t zone, uint8_t *buffer, size_t size)
{

}

static void
write_zone(int fd, struct stpdfs_sb *sb, struct stpdfs_inode *inode, size_t zone, uint8_t *buffer, size_t size)
{
	uint32_t zones[128];

	if (zone < 7)
	{
		if (inode->zones[zone] == 0)
		{
			inode->zones[zone] = stpdfs_alloc_block(fd, sb);
		}
		stpdfs_write(fd, inode->zones[zone], buffer, size);

		return;
	}
	
	if (zone < 135)
	{
		if (inode->zones[7] == 0)
		{
			inode->zones[7] = stpdfs_alloc_block(fd, sb);
			memset(zones, 0, 512);
			stpdfs_write(fd, inode->zones[7], zones, 512);
		}

		stpdfs_read(fd, inode->zones[7], zones, 512);
		if (zones[zone - 7] == 0)
		{
			zones[zone - 7] = stpdfs_alloc_block(fd, sb);
			stpdfs_write(fd, inode->zones[7], zones, 512);
		}

		stpdfs_write(fd, zones[zone - 7], buffer, size);
		
		return;
	}

	if (zone < 16519)
	{
		if (inode->zones[8] == 0)
		{
			inode->zones[8] = stpdfs_alloc_block(fd, sb);
			memset(zones, 0, 512);
			stpdfs_write(fd, inode->zones[7], zones, 512);
		}
		stpdfs_read(fd, inode->zones[8], zones, 512);

		if (zones[(zone - 135) / 128] == 0)
		{
			zones[(zone - 135) / 128] = stpdfs_alloc_block(fd, sb);
		}
	}
}

int
stpdfs_write_data(int fd, struct stpdfs_sb *sb, struct stpdfs_inode *inode, uint8_t *buffer, size_t size)
{
	size_t idx;
	uint32_t zonenum;
	size_t max_zone;

	for (idx = 0; idx < size; idx += STPDFS_BLOCK_SIZE)
	{
		max_zone = idx / STPDFS_BLOCK_SIZE;
		for (zonenum = 0; zonenum < max_zone; zonenum++)
		{
			write_zone(fd, sb, inode, zonenum, buffer + idx, ((idx + 1) * 512) > size ? size % 512 : 512);
		}

	}

	return (0)
}

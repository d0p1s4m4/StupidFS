#include "stpdfs.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


static int
stpdfs_write_indirect(int fd, uint32_t blocknum, uint8_t *buffer, size_t size)
{
	size_t i;
	uint32_t zones[STPDFS_ZONES_PER_BLOCK];
	size_t byte_read;

	stpdfs_read(fd, blocknum, zones, sizeof(zones));

	byte_read = 0;
	for (i = 0; i < STPDFS_ZONES_PER_BLOCK; i++)
	{
		stpdfs_read(fd, zones[i], buffer, STPDFS_BLOCK_SIZE);
		byte_read += STPDFS_BLOCK_SIZE;

		if (byte_read >= size)
		{
			return (byte_read);
		}
	}

	return (byte_read);
}

int
stpdfs_write_data(int fd, struct stpdfs_sb *sb, struct stpdfs_inode *inode, uint8_t *data)
{
	size_t i;
	size_t j;
	size_t byte_write;
	uint32_t zones1[STPDFS_ZONES_PER_BLOCK];
	uint32_t zones2[STPDFS_ZONES_PER_BLOCK];

	byte_write = 0;
	for (i = 0; i < 7; i++)
	{
		if (inode->zones[i] == 0)
		{
			inode->zones[i] = stpdfs_alloc_block(fd, sb);
		}
		stpdfs_write(fd, inode->zones[i], data + byte_write, (inode->size - byte_write) >= STPDFS_BLOCK_SIZE ? STPDFS_BLOCK_SIZE : (inode->size - byte_write));
		byte_write += STPDFS_BLOCK_SIZE;

		if (byte_write >= inode->size)
		{
			return (byte_write);
		}
	}

	/*

	byte_read += stpdfs_write_indirect(fd, inode->zones[7], *buffer + byte_read, inode->size - byte_read);
	if (byte_read >= inode->size)
	{
		return (byte_read);
	}

	stpdfs_read(fd, inode->zones[8], zones1, sizeof(zones1));

	for (i = 0; i < STPDFS_ZONES_PER_BLOCK; i++)
	{
		byte_read += stpdfs_read_indirect(fd, zones1[i], *buffer + byte_read, inode->size - byte_read);
		if (byte_read >= inode->size)
		{
			return (byte_read);
		}
	}
	

	stpdfs_read(fd, inode->zones[9], zones1, sizeof(zones1));

	for (i = 0; i < STPDFS_ZONES_PER_BLOCK; i++)
	{
		stpdfs_read(fd, zones1[i], zones2, sizeof(zones2));

		for (j = 0; j < STPDFS_ZONES_PER_BLOCK; j++)
		{
			byte_read += stpdfs_read_indirect(fd, zones2[i], *buffer + byte_read, inode->size - byte_read);
			if (byte_read >= inode->size)
			{
				return (byte_read);
			}
		}
		
	}*/

	return (byte_write);
}

static int
stpdfs_read_indirect(int fd, uint32_t blocknum, uint8_t *buffer, size_t size)
{
	size_t i;
	uint32_t zones[STPDFS_ZONES_PER_BLOCK];
	size_t byte_read;

	stpdfs_read(fd, blocknum, zones, sizeof(zones));

	byte_read = 0;
	for (i = 0; i < STPDFS_ZONES_PER_BLOCK; i++)
	{
		stpdfs_read(fd, zones[i], buffer, STPDFS_BLOCK_SIZE);
		byte_read += STPDFS_BLOCK_SIZE;

		if (byte_read >= size)
		{
			return (byte_read);
		}
	}

	return (byte_read);
}

int
stpdfs_read_data(int fd, struct stpdfs_sb *sb, struct stpdfs_inode *inode, uint8_t **buffer)
{
	size_t i;
	size_t j;
	size_t byte_read;
	uint32_t zones1[STPDFS_ZONES_PER_BLOCK];
	uint32_t zones2[STPDFS_ZONES_PER_BLOCK];
	

	*buffer = (uint8_t *)malloc(inode->size + STPDFS_BLOCK_SIZE);

	byte_read = 0;
	for (i = 0; i < 7; i++)
	{
		stpdfs_read(fd, inode->zones[i], *buffer, STPDFS_BLOCK_SIZE);
		byte_read += STPDFS_BLOCK_SIZE;

		if (byte_read >= inode->size)
		{
			return (byte_read);
		}
	}

	byte_read += stpdfs_read_indirect(fd, inode->zones[7], *buffer + byte_read, inode->size - byte_read);
	if (byte_read >= inode->size)
	{
		return (byte_read);
	}

	stpdfs_read(fd, inode->zones[8], zones1, sizeof(zones1));

	for (i = 0; i < STPDFS_ZONES_PER_BLOCK; i++)
	{
		byte_read += stpdfs_read_indirect(fd, zones1[i], *buffer + byte_read, inode->size - byte_read);
		if (byte_read >= inode->size)
		{
			return (byte_read);
		}
	}
	

	stpdfs_read(fd, inode->zones[9], zones1, sizeof(zones1));

	for (i = 0; i < STPDFS_ZONES_PER_BLOCK; i++)
	{
		stpdfs_read(fd, zones1[i], zones2, sizeof(zones2));

		for (j = 0; j < STPDFS_ZONES_PER_BLOCK; j++)
		{
			byte_read += stpdfs_read_indirect(fd, zones2[i], *buffer + byte_read, inode->size - byte_read);
			if (byte_read >= inode->size)
			{
				return (byte_read);
			}
		}
		
	}

	return (byte_read);
}
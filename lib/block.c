/**
 * file: block.c
 */

#include "stpdfs.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

size_t
stpdfs_write(int fd, uint32_t blocknum, void *data, size_t size)
{
	uint8_t buffer[STPDFS_BLOCK_SIZE];

	if (size > STPDFS_BLOCK_SIZE) return (-1);

	lseek(fd, blocknum * STPDFS_BLOCK_SIZE, SEEK_SET);
	if (size > 0)
	{
		memcpy(buffer, data, size);
		return (write(fd, buffer, STPDFS_BLOCK_SIZE));
	}

	return (write(fd, data, size));
}

size_t
stpdfs_read(int fd, uint32_t blocknum, void *data, size_t size)
{
	lseek(fd, blocknum * STPDFS_BLOCK_SIZE, SEEK_SET);

	return (read(fd, data, size));
}

uint32_t
stpdfs_alloc_block(int fd, struct stpdfs_sb *sb)
{
	uint32_t blocknum;
	struct stpdfs_free freelist;

	sb->state = STPDFS_DIRTY; /* mark state dirty */
redo:
	sb->nfree--;
	blocknum = sb->free[sb->nfree];
	if (sb->nfree == 0 && blocknum != 0)
	{
		stpdfs_read(fd, blocknum, &freelist, sizeof(struct stpdfs_free));
		memcpy(sb->free, &freelist, sizeof(uint32_t) * 100);
		sb->nfree = freelist.nfree;
		goto redo;
	}

	sb->time = time(NULL);

	return (blocknum);
}

int
stpdfs_free_block(int fd, struct stpdfs_sb *sb, uint32_t blocknum)
{
	struct stpdfs_free copy;

	if (blocknum == 0 || blocknum >= sb->fsize)
	{
		return (-1);
	}

	sb->state = STPDFS_DIRTY; /* mark state dirty */

	if (sb->nfree == 100)
	{
		memcpy(&copy, sb->free, sizeof(uint32_t) * 100);
		copy.nfree = sb->nfree;

		stpdfs_write(fd, blocknum, &copy, sizeof(struct stpdfs_free));

		sb->nfree = 1;
		sb->free[0] = blocknum;
	}
	else
	{
		sb->free[sb->nfree++] = blocknum;
	}

	sb->time = time(NULL);
	return (0);
}
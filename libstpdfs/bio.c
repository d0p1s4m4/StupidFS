#include "stupidfs.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <libstpdfs/stpdfs.h>

static struct stpdfs_buffer *head = NULL;

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

static struct stpdfs_buffer *
bget(int fd, uint32_t blocknum)
{
	struct stpdfs_buffer *buff;

	for (buff = head; buff != NULL; buff = buff->next)
	{
		if (buff->blocknum == blocknum)
		{
			buff->refcount++;
			return (buff);
		}
	}

	buff = (struct stpdfs_buffer *)malloc(sizeof(struct stpdfs_buffer));
	if (buff == NULL)
	{
		return (NULL);
	}

	buff->next = head;
	buff->blocknum = blocknum;
	buff->valid = 0;
	buff->refcount = 1;
	buff->fd = fd;

	head = buff;

	return (buff);
}

struct stpdfs_buffer *
stpdfs_bread(int fd, uint32_t blocknum)
{
	struct stpdfs_buffer *buff;

	buff = bget(fd, blocknum);
	if (buff == NULL)
	{
		return (NULL);
	}

	if (buff->valid == 0)
	{
		stpdfs_read(fd, blocknum, buff->data, STPDFS_BLOCK_SIZE);
		buff->valid = 1;
	}

	return (buff);
}

void
stpdfs_bwrite(struct stpdfs_buffer *buff)
{
	stpdfs_write(buff->fd, buff->blocknum, buff->data, STPDFS_BLOCK_SIZE);
}

void
stpdfs_brelse(struct stpdfs_buffer *buff)
{
	struct stpdfs_buffer *tmp;

	buff->refcount--;
	if (buff->refcount > 0) return;

	if (buff == head)
	{
		head = buff->next;
	}
	else
	{
		for (tmp = head; tmp != NULL; tmp = tmp->next)
		{
			if (tmp->next == buff)
			{
				tmp->next = buff->next;
				break;
			}
		}
		free(buff);
	}
}

void
stpdfs_bpin(struct stpdfs_buffer *buff)
{
	buff->refcount++;
}

void
stpdfs_bunpin(struct stpdfs_buffer *buff)
{
	buff->refcount--;
}

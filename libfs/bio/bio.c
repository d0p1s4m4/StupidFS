#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "bio.h"

static struct fs_buffer *head = NULL;

static struct fs_buffer *
bget(int fd, uint32_t blocknum)
{
	struct fs_buffer *buff;

	for (buff = head; buff != NULL; buff = buff->next)
	{
		if (buff->blocknum == blocknum)
		{
			buff->refcount++;
			return (buff);
		}
	}

	buff = (struct fs_buffer *)malloc(sizeof(struct fs_buffer));
	if (buff == NULL) return (NULL);

	buff->next = head;
	buff->blocknum = blocknum;
	buff->valid = 0;
	buff->refcount = 1;
	buff->fd = fd;

	head = buff;

	return (buff);
}

struct fs_buffer *
fs_bio_bread(int fd, uint32_t blocknum)
{
	struct fs_buffer *buff;

	buff = bget(fd, blocknum);
	if (buff == NULL)
	{
		return (NULL);
	}

	if (buff->valid == 0)
	{
		fs_bio_raw_read(fd, blocknum, buff->data, STPDFS_BLOCK_SIZE);
		buff->valid = 1;
	}

	return (buff);
}

void
fs_bio_bwrite(struct fs_buffer *buff)
{
	fs_bio_raw_read(buff->fd, buff->blocknum, buff->data, STPDFS_BLOCK_SIZE);
}

void
fs_bio_brelse(struct fs_buffer *buff)
{
	struct fs_buffer *tmp;

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
	}

	free(buff);
}

void
stpdfs_bpin(struct fs_buffer *buff)
{
	buff->refcount++;
}

void
stpdfs_bunpin(struct fs_buffer *buff)
{
	buff->refcount--;
}

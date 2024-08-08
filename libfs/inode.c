#include <stdlib.h>
#include <string.h>
#include "inode.h"
#include "bio/bio.h"
#include "stupidfs.h"

#define IBLOCK(x) (x / STPDFS_INODES_PER_BLOCK + 2)

static struct fs_inode *head;

struct fs_inode *
fs_inode_get(struct fs_super *super, uint32_t inum)
{
	struct fs_inode *ip;

	for (ip = head; ip != NULL; ip = ip->next)
	{
		if (ip->inum == inum)
		{
			ip->refcount++;
			return (ip);
		}
	}

	ip = (struct fs_inode *)malloc(sizeof(struct fs_inode));
	if (ip == NULL) return (NULL);

	ip->super = super;
	ip->inum = 0;
	ip->refcount = 1;
	ip->valid = 0;

	return (ip);
}

struct fs_inode *
fs_inode_read(struct fs_inode *ip)
{
	struct fs_buffer *buff;
	struct stpdfs_inode *dinode;

	if (ip == NULL) return (NULL);

	buff = fs_bio_bread(ip->super->fd, IBLOCK(ip->inum));
	if (buff == NULL) return (NULL);

	dinode = (struct stpdfs_inode *)buff->data;
	memcpy(&ip->inode, dinode, sizeof(struct stpdfs_inode));

	fs_bio_brelse(buff);

	ip->valid = 1;

	return (ip);
}

int
fs_inode_update(struct fs_inode *ip)
{
	struct fs_buffer *buff;
	struct stpdfs_inode *dinode;

	buff = fs_bio_bread(ip->super->fd, IBLOCK(ip->inum));
	if (buff == NULL) return (-1);

	dinode = (struct stpdfs_inode *)buff->data + ip->inum % STPDFS_INODES_PER_BLOCK;
	memcpy(dinode, &ip->inode, sizeof(struct stpdfs_inode));

	fs_bio_bwrite(buff);
	fs_bio_brelse(buff);

	return (0);
}

void
fs_inode_release(struct fs_inode *ip)
{
	struct fs_inode *tmp;

	ip->refcount--;
	if (ip->refcount > 0) return;

	if (ip == head)
	{
		head = ip->next;
	}
	else
	{
		for (tmp = head; tmp != NULL; tmp = tmp->next)
		{
			if (tmp->next == tmp)
			{
				tmp->next = ip->next;
				break;
			}
		}
	}

	free(ip);
}
#include "stpdfs.h"
#include "stupidfs.h"
#include <libstpdfs/stpdfs.h>
#include <stdint.h>
#include <string.h>

#ifndef MIN
# define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif /* !MIN */
#define INODE_CACHE 50
#define IBLOCK(x) (x / STPDFS_INODES_PER_BLOCK + 2)

static struct stpdfs_inode_info icache[INODE_CACHE] = {0};

struct stpdfs_inode_info *
stpdfs_inode_get(struct stpdfs_super_info *sbi, uint32_t inum)
{
	struct stpdfs_inode_info *empty;
	struct stpdfs_buffer *buff;
	struct stpdfs_inode *dinode;
	int idx;

	empty = NULL;
	for (idx = 0; idx < INODE_CACHE; idx++)
	{
		if (icache[idx].refcount > 0 && icache[idx].inum == inum)
		{
			icache[idx].refcount++;
			return (&icache[idx]);
		}

		if (empty == NULL && icache[idx].refcount == 0)
		{
			empty = &icache[idx];
		}
	}

	if (empty == NULL) return (NULL);

	empty->sbi = sbi;
	empty->inum = inum;
	empty->refcount = 1;

	empty->valid = 1;
	buff = stpdfs_bread(sbi->fd, IBLOCK(inum));
	dinode = (struct stpdfs_inode *)buff->data + inum % STPDFS_INODES_PER_BLOCK;
	memcpy(&empty->inode, dinode, sizeof(struct stpdfs_inode));
	stpdfs_brelse(buff);

	return (empty);
}

struct stpdfs_inode_info *
stpdfs_inode_alloc(struct stpdfs_super_info *sbi)
{
	uint32_t inum;
	struct stpdfs_buffer *buff;
	struct stpdfs_inode *dinode;

	for (inum = 1; inum < sbi->sb.isize; inum++)
	{
		buff = stpdfs_bread(sbi->fd, IBLOCK(inum));
		dinode = (struct stpdfs_inode *)buff->data + inum % STPDFS_INODES_PER_BLOCK;
		if ((dinode->flags & STPDFS_INO_FLAG_ALOC) == 0)
		{
			dinode->flags = STPDFS_INO_FLAG_ALOC;
			stpdfs_bwrite(buff);
			stpdfs_brelse(buff);

			return (stpdfs_inode_get(sbi, inum));
		}

		stpdfs_brelse(buff);
	}

	return (NULL);
}

void
stpdfs_inode_update(struct stpdfs_inode_info *ip)
{
	struct stpdfs_buffer *buff;
	struct stpdfs_inode *dinode;

	buff = stpdfs_bread(ip->sbi->fd, IBLOCK(ip->inum));
	dinode = (struct stpdfs_inode *)buff->data + ip->inum % STPDFS_INODES_PER_BLOCK;
	memcpy(dinode, &ip->inode, sizeof(struct stpdfs_inode));
	stpdfs_bwrite(buff);
	stpdfs_brelse(buff);
}

int
stpdfs_inode_bmap(struct stpdfs_inode_info *ip, uint32_t blocknum)
{
	uint32_t *addrs;
	uint32_t index;
	struct stpdfs_buffer *buff;

	if (blocknum < STPDFS_NDIR)
	{
		if (ip->inode.zones[blocknum] == 0)
		{
			ip->inode.zones[blocknum] = stpdfs_super_balloc(ip->sbi);
		}
		return (ip->inode.zones[blocknum]);
	}

	index = blocknum - STPDFS_NDIR;
	if (blocknum < (STPDFS_BLOCK_SIZE/sizeof(int32_t) + STPDFS_SIND))
	{
		if (ip->inode.zones[STPDFS_SIND] == 0)
		{
			ip->inode.zones[STPDFS_SIND] = stpdfs_super_balloc(ip->sbi);
		}

		buff = stpdfs_bread(ip->sbi->fd, ip->inode.zones[STPDFS_SIND]);
		addrs = (uint32_t *)buff->data;
		if (addrs[index] == 0)
		{
			addrs[index] = stpdfs_super_balloc(ip->sbi);
		}
		stpdfs_brelse(buff);
		return (addrs[index]);
	}

	/* TODO double and triple ind  */
	return (0);
}

void
stpdfs_inode_trunc(struct stpdfs_inode_info *ip)
{
	(void)ip;
}

int
stpdfs_inode_read(struct stpdfs_inode_info *ip, uint8_t *dest, size_t offset, size_t size)
{
	size_t total;
	size_t rd;
	uint32_t zone;
	struct stpdfs_buffer *buff;

	if (offset > ip->inode.size)
	{
		return (0);
	}

	total = 0;
	while (total < size)
	{
		zone = stpdfs_inode_bmap(ip, offset/STPDFS_BLOCK_SIZE);
		if (zone == 0) return (total);

		buff = stpdfs_bread(ip->sbi->fd, zone);

		rd = MIN(size - total, STPDFS_BLOCK_SIZE - offset % STPDFS_BLOCK_SIZE);
		memcpy(dest, buff->data + (offset % STPDFS_BLOCK_SIZE), rd);
		stpdfs_brelse(buff);

		total += rd;
		offset += rd;
		dest += rd;
	}

	return (total);
}

int
stpdfs_inode_write(struct stpdfs_inode_info *ip, const uint8_t *src, size_t offset, size_t size)
{
	size_t total;
	size_t rd;
	uint32_t zone;
	struct stpdfs_buffer *buff;

	if (offset > ip->inode.size)
	{
		return (0);
	}

	total = 0;
	while (total < size)
	{
		zone = stpdfs_inode_bmap(ip, offset/STPDFS_BLOCK_SIZE);
		if (zone == 0) break;

		buff = stpdfs_bread(ip->sbi->fd, zone);

		rd = MIN(size - total, STPDFS_BLOCK_SIZE - offset % STPDFS_BLOCK_SIZE);
		memcpy(buff->data + (offset % STPDFS_BLOCK_SIZE), src, rd);
		stpdfs_bwrite(buff);
		stpdfs_brelse(buff);

		total += rd;
		offset += rd;
		src += rd;
	}

	if (offset > ip->inode.size)
	{
		ip->inode.size = offset;
	}

	stpdfs_inode_update(ip);

	return (total);
}

struct stpdfs_inode_info *
stpdfs_dirlookup(struct stpdfs_inode_info *dp, const char *name, size_t *offset)
{
	struct stpdfs_dirent dirent;
	size_t idx;

	if (!(dp->inode.flags & STPDFS_S_IFDIR))
	{
		return (NULL);
	}

	for (idx = 0; idx < dp->inode.size; idx += STPDFS_DIRENT_SIZE)
	{
		if (stpdfs_inode_read(dp, (uint8_t *)&dirent, idx, STPDFS_DIRENT_SIZE) != STPDFS_DIRENT_SIZE)
		{
			return (NULL);
		}

		if (strncmp(name, dirent.filename, STPDFS_NAME_MAX))
		{
			*offset = idx;
			return (stpdfs_inode_get(dp->sbi, dirent.inode));
		}
	}

	return (NULL);
}

int
stpdfs_dirlink(struct stpdfs_inode_info *dp, const char *name, uint32_t inum)
{
	return (0);
}


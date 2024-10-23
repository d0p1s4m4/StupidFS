#include <stdint.h>
#include <string.h>
#include <stupidfs.h>
#include "fs.h"
#include "bio/bio.h"

#ifndef MIN
# define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif /* !MIN */

static int
bmap(struct fs_inode *ip, uint32_t blocknum)
{
	uint32_t *addrs;
	uint32_t *dind_addrs;
	uint32_t index;
	uint32_t tmp;
	struct fs_buffer *buff;

	if (blocknum < STPDFS_NDIR)
	{
		if (ip->inode.zones[blocknum] == 0)
		{
			ip->inode.zones[blocknum] = fs_balloc(ip->super);
		}

		return (ip->inode.zones[blocknum]);
	}

	index = blocknum - STPDFS_NDIR;
	if (blocknum < (STPDFS_BLOCK_SIZE/sizeof(int32_t) + STPDFS_SIND))
	{
		if (ip->inode.zones[STPDFS_SIND] == 0)
		{
			ip->inode.zones[STPDFS_SIND] = fs_balloc(ip->super);
		}

		buff = fs_bio_bread(ip->super->fd, ip->inode.zones[STPDFS_SIND]);
		addrs = (uint32_t *)buff->data;
		if (addrs[index] == 0)
		{
			addrs[index] = fs_balloc(ip->super);
			fs_bio_bwrite(buff);
		}
		tmp = addrs[index];
		fs_bio_brelse(buff);
		return (tmp);
	}

	index = blocknum - (STPDFS_NDIR + (STPDFS_BLOCK_SIZE / sizeof(uint32_t)));
	if (blocknum < (STPDFS_NDIR + (STPDFS_BLOCK_SIZE / sizeof(uint32_t))))
	{
		if (ip->inode.zones[STPDFS_DIND] == 0)
		{
			ip->inode.zones[STPDFS_DIND] = fs_balloc(ip->super);
		}

		buff = fs_bio_bread(ip->super->fd, ip->inode.zones[STPDFS_DIND]);
		dind_addrs = (uint32_t *)buff->data;

		if (dind_addrs[index / (STPDFS_BLOCK_SIZE / sizeof(uint32_t))] == 0)
		{
			dind_addrs[index / (STPDFS_BLOCK_SIZE / sizeof(uint32_t))] = fs_balloc(ip->super);
			fs_bio_bwrite(buff);
		}

		tmp = dind_addrs[index / (STPDFS_BLOCK_SIZE / sizeof(uint32_t))];
		fs_bio_brelse(buff);

		buff = fs_bio_bread(ip->super->fd, tmp);
		addrs = (uint32_t *)buff->data;

		if (addrs[index % (STPDFS_BLOCK_SIZE / sizeof(uint32_t))] == 0)
		{
			addrs[index % (STPDFS_BLOCK_SIZE / sizeof(uint32_t))] = fs_balloc(ip->super);
			fs_bio_bwrite(buff);
		}

		tmp = addrs[index % (STPDFS_BLOCK_SIZE / sizeof(uint32_t))];
		fs_bio_brelse(buff);

		return (tmp);
	}
	/* triple ind  */
	return (0);
}

int
fs_read(struct fs_inode *ip, uint8_t *dest, size_t offset, size_t size)
{
	size_t total;
	size_t rd;
	uint32_t zone;
	struct fs_buffer *buff;

	if (offset > ip->inode.size)
	{
		return (0);
	}

	total = 0;
	while (total < size)
	{
		zone = bmap(ip, offset/STPDFS_BLOCK_SIZE);
		if (zone == 0) return (total);

		buff = fs_bio_bread(ip->super->fd, zone);

		rd = MIN(size - total, STPDFS_BLOCK_SIZE - offset % STPDFS_BLOCK_SIZE);
		memcpy(dest, buff->data + (offset % STPDFS_BLOCK_SIZE), rd);
		fs_bio_brelse(buff);

		total += rd;
		offset += rd;
		dest += rd;
	}

	return (total);
}

int
fs_write(struct fs_inode *ip, const uint8_t *src, size_t offset, size_t size)
{
	size_t total;
	size_t rd;
	uint32_t zone;
	struct fs_buffer *buff;

	if (offset > ip->inode.size)
	{
		return (0);
	}

	total = 0;
	while (total < size)
	{
		zone = bmap(ip, offset/STPDFS_BLOCK_SIZE);
		if (zone == 0) break;

		buff = fs_bio_bread(ip->super->fd, zone);

		rd = MIN(size - total, STPDFS_BLOCK_SIZE - offset % STPDFS_BLOCK_SIZE);
		memcpy(buff->data + (offset % STPDFS_BLOCK_SIZE), src, rd);
		fs_bio_bwrite(buff);
		fs_bio_brelse(buff);

		total += rd;
		offset += rd;
		src += rd;
	}

	if (offset > ip->inode.size)
	{
		ip->inode.size = offset;
	}

	fs_inode_update(ip);

	return (total);
}
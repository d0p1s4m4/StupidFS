#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stupidfs.h>
#include "super.h"
#include "bio/bio.h"

uint32_t
fs_balloc(struct fs_super *super)
{
	uint32_t blocknum;
	struct stpdfs_free freelist;
	struct fs_buffer *buff;

	super->sb.state = STPDFS_DIRTY;

redo:
	super->sb.freelist.nfree--;
	blocknum = super->sb.freelist.free[super->sb.freelist.nfree];
	if (super->sb.freelist.nfree == 0 && blocknum != 0)
	{
		buff = fs_bio_bread(super->fd, blocknum);
		memcpy(&super->sb.freelist, buff->data, sizeof(struct stpdfs_free));
		goto redo;
	}

	super->sb.time = time(NULL);

	buff = fs_bio_bread(super->fd, STPDFS_SB_BLOCK);
	memcpy(buff->data, &super->sb, sizeof(struct stpdfs_sb));
	fs_bio_bwrite(buff);
	fs_bio_brelse(buff);

	buff = fs_bio_bread(super->fd, blocknum);
	memset(buff->data, 0, STPDFS_BLOCK_SIZE);
	fs_bio_bwrite(buff);
	fs_bio_brelse(buff);

	return (blocknum);
}

int
fs_bfree(struct fs_super *super, uint32_t blocknum)
{
	struct fs_buffer *buff;

	if (blocknum == 0 || blocknum >= super->sb.fsize)
	{
		return (-1);
	}

	super->sb.state = STPDFS_DIRTY;
	if (super->sb.freelist.nfree == 100)
	{
		buff = fs_bio_bread(super->fd, blocknum);
		memcpy(buff->data, &super->sb.freelist, sizeof(struct stpdfs_free));
		fs_bio_bwrite(buff);
		fs_bio_brelse(buff);

		super->sb.freelist.nfree = 1;
		super->sb.freelist.free[0] = blocknum;
	}
	else
	{
		super->sb.freelist.free[super->sb.freelist.nfree++] = blocknum;
	}

	super->sb.time = time(NULL);

	buff = fs_bio_bread(super->fd, STPDFS_SB_BLOCK);
	memcpy(buff->data, &super->sb, sizeof(struct stpdfs_sb));
	fs_bio_bwrite(buff);
	fs_bio_brelse(buff);

	return (0);	
}
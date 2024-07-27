#include "stupidfs.h"
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <libstpdfs/stpdfs.h>

int
stpdfs_super_validate(struct stpdfs_sb *sb)
{
	if (sb->magic != STPDFS_SB_MAGIC)
	{
		return (-1);
	}

	if (sb->revision != STPDFS_SB_REV)
	{
		return (-1);
	}

	if (sb->fsize == 0 || sb->isize == 0)
	{
		return (-1);
	}

	if (sb->isize > sb->fsize)
	{
		return (-1);
	}

	return (0);
}

int
stpdfs_super_open(struct stpdfs_super_info *sbi, const char *fname)
{
	struct stpdfs_buffer *buff;

	sbi->fd = open(fname, O_RDWR);
	if (sbi->fd < 0)
	{
		perror(fname);
		return (-1);
	}

	buff = stpdfs_bread(sbi->fd, 1);
	if (!buff) goto err;

	if (stpdfs_super_validate((struct stpdfs_sb *)buff->data) != 0) goto err;

	memcpy(&sbi->sb, buff->data, sizeof(struct stpdfs_sb));

	return (0);
err:
	close(sbi->fd);
	return (-1);
}

int
stpdfs_super_kill(struct stpdfs_super_info *sbi)
{
	struct stpdfs_buffer *buff;

	buff = stpdfs_bread(sbi->fd, 1);
	if (buff == NULL) goto end;

	memcpy(buff->data, &sbi->sb, sizeof(struct stpdfs_sb));

	stpdfs_bwrite(buff);

	stpdfs_brelse(buff);
	

end:
	close(sbi->fd);

	return (0);
}

uint32_t
stpdfs_super_balloc(struct stpdfs_super_info *sbi)
{
	uint32_t blocknum;
	struct stpdfs_free freelist;
	struct stpdfs_buffer *buff;

	sbi->sb.state = STPDFS_DIRTY;
redo:
	sbi->sb.freelist.nfree--;
	blocknum = sbi->sb.freelist.free[sbi->sb.freelist.nfree];
	if (sbi->sb.freelist.nfree == 0 && blocknum != 0)
	{
		buff = stpdfs_bread(sbi->fd, blocknum);
		memcpy(&sbi->sb.freelist, buff->data, sizeof(struct stpdfs_free));
		goto redo;
	}

	buff = stpdfs_bread(sbi->fd, 1);
	memcpy(buff->data, &sbi->sb, sizeof(struct stpdfs_sb));
	stpdfs_bwrite(buff);

	buff = stpdfs_bread(sbi->fd, blocknum);
	memset(buff->data, 0, STPDFS_BLOCK_SIZE);
	stpdfs_bwrite(buff);
	stpdfs_brelse(buff);

	sbi->sb.time = time(NULL);

	return (blocknum);
}

int
stpdfs_super_bfreee(struct stpdfs_super_info *sbi, uint32_t blocknum)
{
	struct stpdfs_free copy;
	struct stpdfs_buffer *buff;

	if (blocknum == 0 || blocknum >= sbi->sb.fsize)
	{
		return (-1);
	}

	sbi->sb.state = STPDFS_DIRTY;

	if (sbi->sb.freelist.nfree == 100)
	{
		buff = stpdfs_bread(sbi->fd, blocknum);
		memcpy(buff->data, &sbi->sb.freelist, sizeof(struct stpdfs_free));
		stpdfs_bwrite(buff);
		stpdfs_brelse(buff);
		sbi->sb.freelist.nfree = 1;
		sbi->sb.freelist.free[0] = blocknum;
	}
	else
	{
		sbi->sb.freelist.free[sbi->sb.freelist.nfree++] = blocknum;
	}

	sbi->sb.time = time(NULL);

	buff = stpdfs_bread(sbi->fd, 1);
	memcpy(buff->data, &sbi->sb, sizeof(struct stpdfs_sb));
	stpdfs_bwrite(buff);

	return (0);
}
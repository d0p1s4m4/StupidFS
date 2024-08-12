#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stupidfs.h>
#include "super.h"
#include "bio/bio.h"

int
fs_super_valide(struct stpdfs_sb *sb)
{
	if (sb->magic != STPDFS_SB_MAGIC)
	{
		printf("bad magic\n");
		return (-1);
	}

	if (sb->revision != STPDFS_SB_REV)
	{
		printf("bad rev\n");
		return (-1);
	}

	if (sb->fsize == 0 || sb->isize == 0)
	{
		printf("size null\n");
		return (-1);
	}

	if (sb->isize > sb->fsize)
	{
		printf("invalid size\n");
		return (-1);
	}

	return (0);
}

int
fs_super_open(struct fs_super *super, const char *fname)
{
	struct fs_buffer *buff;

	super->fd = open(fname, O_RDWR | O_BINARY);
	if (super->fd < 0)
	{
		perror(fname);
		return (-1);
	}

	buff = fs_bio_bread(super->fd, STPDFS_SB_BLOCK);
	if (!buff) goto err;

	memcpy(&super->sb, buff->data, sizeof(struct stpdfs_sb));

	fs_bio_brelse(buff);

	if (fs_super_valide(&super->sb) != 0) goto err;

	return (0);

err:
	close(super->fd);
	return (-1);
}

int
fs_super_kill(struct fs_super *super)
{
	struct fs_buffer *buff;
	int ret;

	super->sb.state = STPDFS_CLEANLY_UNMOUNTED;

	ret = 0;
	buff = fs_bio_bread(super->fd, STPDFS_SB_BLOCK);
	if (buff == NULL) 
	{
		ret = 1;
		goto end;
	}

	memcpy(buff->data, &super->sb, sizeof(struct stpdfs_sb));

	fs_bio_bwrite(buff);
	fs_bio_brelse(buff);

end:
	close(super->fd);
	return (ret);
}
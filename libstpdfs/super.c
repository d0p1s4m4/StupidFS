#include <fcntl.h>
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
}

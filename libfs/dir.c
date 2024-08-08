#include "inode.h"
#include "fs.h"
#include "stupidfs.h"
#include <errno.h>
#include <stdint.h>
#include <string.h>

struct fs_inode *
fs_dir_lookup(struct fs_inode *dp, const char *name, size_t *offset)
{
	size_t idx;
	struct stpdfs_dirent dirent;

	if (!(dp->inode.flags & STPDFS_S_IFDIR))
	{
		errno = ENOTDIR;
		return (NULL);
	}

	for (idx = 0; idx < dp->inode.size; idx += STPDFS_DIRENT_SIZE)
	{
		fs_read(dp, (uint8_t *)&dirent, idx, STPDFS_DIRENT_SIZE);

		if (dirent.inode == 0) continue;

		if (strncmp(name, dirent.filename, STPDFS_NAME_MAX))
		{
			if  (offset) *offset = idx;
			return (fs_inode_get(dp->super, dirent.inode));
		}
	}

	return (NULL);
}

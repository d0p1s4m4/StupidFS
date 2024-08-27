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

int
fs_dir_link(struct fs_inode *dp, const char *name, uint32_t inum)
{
	int offset;
	struct fs_inode *ip;
	struct stpdfs_dirent dirent;

	ip = fs_dir_lookup(dp, name, 0);
	if (ip != NULL)
	{
		fs_inode_release(ip);
		return (-EEXIST);
	}

	for (offset = 0; offset < dp->inode.size; offset += STPDFS_DIRENT_SIZE)
	{
		fs_read(dp, (uint8_t *)&dirent, offset, STPDFS_DIRENT_SIZE);
		if (dirent.inode == 0)
		{
			break;
		}
	}

	memset(dirent.filename, 0, STPDFS_NAME_MAX);
	strncpy(dirent.filename, name, STPDFS_NAME_MAX);

	dirent.inode = inum;

	if (fs_write(dp, (uint8_t *)&dirent, offset, STPDFS_DIRENT_SIZE) != STPDFS_DIRENT_SIZE)
	{
		return (-1);
	}

	ip = fs_inode_get(dp->super, inum);
	if (ip->valid == 0)
	  {
	    fs_inode_read(ip);
	  }
	ip->inode.nlink += 1;

	fs_inode_update(ip);
	fs_inode_release(ip);

	return (0);
}

int
fs_dir_unlink(struct fs_inode *dp, const char *name)
{
  int offset;
  struct fs_inode *ip;
  struct stpdfs_dirent dirent;

  ip = fs_dir_lookup(dp, name, 0);
  if (ip == NULL)
    {
      return (-1);
    }

  ip->inode.nlink -= 1;

  fs_inode_update(ip);
  fs_inode_release(ip);

  for (offset = 0; offset < dp->inode.size; offset += STPDFS_DIRENT_SIZE)
    {
      fs_read(dp, (uint8_t *)&dirent, offset, STPDFS_DIRENT_SIZE);
      if (strncmp(dirent.filename, name, STPDFS_NAME_MAX) == 0)
	{
	  memset(dirent.filename, 0, STPDFS_NAME_MAX);
	  dirent.inode = 0;
	  fs_write(dp, (uint8_t *)&dirent, offset, STPDFS_DIRENT_SIZE);
	  break;
	}
    }
}

int
fs_mkdir(struct fs_inode *dp, const char *name)
{
  struct fs_inode *ndp;

  if (!(dp->inode.mode & STPDFS_S_IFDIR))
    {
      return (-1);
    }

  ndp = fs_inode_alloc(dp->super);
  if (ndp == NULL)
    {
      return (-1);
    }

  ndp->inode.mode |= STPDFS_S_IFDIR;

  fs_dir_link(ndp, ".", ndp->inum);
  fs_dir_link(ndp, "..", dp->inum);

  fs_dir_link(dp, name, ndp->inum);

  return (0);
}

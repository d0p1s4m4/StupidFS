#include <libstpdfs/stpdfs.h>

#define INODE_CACHE 50

struct stpdfs_inode_info icache[INODE_CACHE];

struct inode *
stpdfs_inode_get(struct stpdfs_sb *sb, uint32_t ino)
{
	return (NULL);
}

struct stpdfs_dirent *
stpdfs_lookup(struct stpdfs_inode *dir, struct stpdfs_dirent *dentry, int flags)
{
	return (NULL);
}
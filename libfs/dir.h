#ifndef FS_DIR_H
# define FS_DIR_H 1

# include <stupidfs.h>
# include "inode.h"

struct fs_dir {
  struct fs_inode *dp;
  int offset;
};

struct fs_inode *fs_dir_lookup(struct fs_inode *dp, const char *name, size_t *offset);
int fs_dir_link(struct fs_inode *dp, const char *name, uint32_t inum);
int fs_dir_unlink(struct fs_inode *dp, const char *name);
int fs_mkdir(struct fs_inode *dp, const char *name, uint16_t mode);

#endif /* !FS_DIR_H */

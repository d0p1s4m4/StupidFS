#ifndef FS_H
# define FS_H 1

# include <stdint.h>
# include "super.h"
# include "inode.h"
# include "dir.h"

uint32_t fs_balloc(struct fs_super *super);
int fs_bfree(struct fs_super *super, uint32_t blocknum);

int fs_read(struct fs_inode *ip, uint8_t *dest, size_t offset, size_t size);
int fs_write(struct fs_inode *ip, const uint8_t *src, size_t offset, size_t size);

#endif /* !FS_H */
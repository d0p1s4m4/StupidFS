/**
 * \file stpdfs.h
 *
 * StupidFS
 * ```
 * ┌──────────┬───────────┬──────┬───┬──────┬────┬───┬────┐
 * │Boot block│Super block│Inodes│...│Inodes│Data│...│Data│
 * └──────────┴───────────┴──────┴───┴──────┴────┴───┴────┘
 * ```
 */
#ifndef STPDFS_H
# define STPDFS_H 1

# include <stupidfs.h>
# include <stdint.h>
# include <stddef.h>

/*
 * API
 */

size_t stpdfs_write(int fd, uint32_t blocknum, void *data, size_t size);
size_t stpdfs_read(int fd, uint32_t blocknum, void *data, size_t size);


/**
  * mark block as available for new allocation.
  * 
  * \param fd file descriptor.
  * \param sb pointer to \ref stpdfs_sb.
  * \param blocknum block to free
  */
uint32_t stpdfs_alloc_block(int fd, struct stpdfs_sb *sb);

/**
  * find a free block.
  *
  * \param fd file descriptor.
  * \param sb pointer to \ref stpdfs_sb.
  * \return block number or 0 if no block found.
  */
int stpdfs_free_block(int fd, struct stpdfs_sb *sb, uint32_t blocknum);

/* superblock.c */
int stpdfs_superblock_valid(const struct stpdfs_sb *sb);
int stpdfs_read_superblock(int fd, struct stpdfs_sb *sb);

/* inode.c */
uint32_t stpdfd_alloc_inode(int fd, struct stpdfs_sb *sb);
int stpdfs_read_inode(int fd, uint32_t inodenum, struct stpdfs_inode *ino);
int stpdfs_write_inode(int fd, uint32_t inodenum, const struct stpdfs_inode *inode);

#endif /* !STPDFS_H */
/**
 * \file stpdfs.h
 */
#ifndef STPDFS_H
# define STPDFS_H 1

# include <stupidfs.h>

struct stpdfs_super_info {
	struct stpdfs_sb sb;
	int fd;
};

struct stpdfs_inode_info {
	struct stpdfs_inode ino;
	int fd;
	int refcount;
};

struct stpdfs_buffer {
	int valid;
	uint32_t blocknum;
	int refcount;
	int fd;

	struct stpdfs_buffer *next;

	uint8_t data[STPDFS_BLOCK_SIZE];
};

/* bio.c */
size_t stpdfs_write(int fd, uint32_t blocknum, void *data, size_t size);
size_t stpdfs_read(int fd, uint32_t blocknum, void *data, size_t size);
struct stpdfs_buffer *stpdfs_bread(int fd, uint32_t blocknum);
void stpdfs_bwrite(struct stpdfs_buffer *buff);
void stpdfs_brelse(struct stpdfs_buffer *buff);
void stpdfs_bpin(struct stpdfs_buffer *buff);

/* super.c */
int stpdfs_read_super(struct stpdfs_super_info *sbi, int fd);
int stpdfs_super_validate(struct stpdfs_sb *sb);
int stpdfs_super_kill(struct stpdfs_super_info *sbi);

uint32_t stpdfs_alloc_block(int fd, struct stpdfs_sb *sb);
int stpdfs_free_block(int fd, struct stpdfs_sb *sb, uint32_t blocknum);

#endif /* !STPDFS_H */
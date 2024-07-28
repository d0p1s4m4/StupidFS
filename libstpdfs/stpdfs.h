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
	int valid;
	int refcount;
	uint32_t inum;
	struct stpdfs_super_info *sbi;

	struct stpdfs_inode inode;
};

struct stpdfs_file {
	int refcount;

	struct stpdfs_inode_info *ip;
	size_t offset;

	struct stpdfs_file *next;
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
int stpdfs_super_open(struct stpdfs_super_info *sbi, const char *fname);
int stpdfs_read_super(struct stpdfs_super_info *sbi, int fd);
int stpdfs_super_validate(struct stpdfs_sb *sb);
int stpdfs_super_kill(struct stpdfs_super_info *sbi);
uint32_t stpdfs_super_balloc(struct stpdfs_super_info *sbi);
int stpdfs_super_bfreee(struct stpdfs_super_info *sbi, uint32_t blocknum);

uint32_t stpdfs_alloc_block(int fd, struct stpdfs_sb *sb);
int stpdfs_free_block(int fd, struct stpdfs_sb *sb, uint32_t blocknum);

struct stpdfs_inode_info *stpdfs_inode_get(struct stpdfs_super_info *sbi, uint32_t inum);
int stpdfs_inode_read(struct stpdfs_inode_info *ip, uint8_t *dest, size_t offset, size_t size);

#endif /* !STPDFS_H */
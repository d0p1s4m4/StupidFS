#ifndef STPDFS_H
# define STPDFS_H 1

# include <stdint.h>

#define STPDFS_MAGIC 0x44505453

#define STPDFS_BLOCK_SIZE 512

/**
 * \brief StupidFS Superblock
 */
struct stpdfs_sb {
	uint32_t magic;
	uint32_t isize;
	uint32_t fsize;
	uint32_t free[100];
	uint8_t nfree;
	uint8_t flock;
	uint8_t ilock;
	uint8_t fmod;
	uint32_t time[2];
} __attribute__((packed));

#endif /* !STPDFS_H */
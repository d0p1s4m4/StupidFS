#include <cstdint>
#ifndef STPDFS_H
# define STPDFS_H 1

# include <stdint.h>

# define STPDFS_SB_MAGIC 0x44505453

# define STPDFS_BLOCK_SIZE 512

# define STPDFS_LZP_COMPRESSION (1 << 0)
# define STPDFS_FILE_ENCRYPTION (1 << 1)

typedef uint32_t stpdfs_addr_t;
typedef uint32_t stpdfs_off_t;
typedef uint64_t stpdfs_time_t;

struct stpdfs_free {
	uint32_t free[100];
	uint8_t nfree;
} __attribute__((packed));

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

/**
 * \brief StupidFS I-node
 */
struct inode {
	uint16_t mode;
	uint8_t nlink;
	uint8_t uid;
	uint8_t gid;
	uint32_t size;
	uint16_t addr[8];
	uint32_t actime[2];
	uint32_t modtime[2];
} __attribute__((packed));

#define INODE_SIZE sizeof(struct inode)

struct file {
	uint32_t inode;
	char filename[32];
};



#endif /* !STPDFS_H */
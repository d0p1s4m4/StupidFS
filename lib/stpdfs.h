#ifndef STPDFS_H
# define STPDFS_H 1

# include <stdint.h>

# define STPDFS_SB_MAGIC 0x44505453

# define STPDFS_BLOCK_SIZE_BITS 9
# define STPDFS_BLOCK_SIZE (1 << STPDFS_BLOCK_SIZE_BITS)

# define STPDFS_NAME_MAX 255

# define STPDFS_INODES_PER_BLOCK (STPDFS_BLOCK_SIZE / (sizeof(struct stpdfs_inode)))

# define STPDFS_ROOT_INO 1

# define STPDFS_LZP_COMPRESSION (1 << 0)
# define STPDFS_FILE_ENCRYPTION (1 << 1)

struct stpdfs_free {
	uint32_t free[100];
	uint8_t nfree;
} __attribute__((packed));

/**
 * \brief StupidFS Superblock
 */
struct stpdfs_sb {
	uint32_t magic;
	uint32_t isize; /**< size in block of the I list */
	uint32_t fsize; /**< size in block of the entire volume */
	uint32_t free[100];
	uint8_t nfree; /**< number of free block (0-100) */
	uint8_t flock;
	uint8_t ilock;
	uint8_t fmod;
	uint64_t time;
} __attribute__((packed));

/**
 * \brief StupidFS I-node
 */
struct stpdfs_inode {
	uint16_t mode; /**< file mode */
	uint8_t nlink; /**< link count */
	uint8_t uid;   /**< owner user id */
	uint8_t gid;   /**< group id */
	uint32_t size;
	uint32_t zones[10];
	uint32_t actime[2];
	uint32_t modtime[2];
} __attribute__((packed));

#define INODE_SIZE sizeof(struct inode)

struct file {
	uint32_t inode;
	char filename[32];
};

#endif /* !STPDFS_H */
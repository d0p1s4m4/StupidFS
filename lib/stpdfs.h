#ifndef STPDFS_H
# define STPDFS_H 1

# include <stdint.h>
# include <stddef.h>

# define STPDFS_SB_MAGIC 0x44505453
# define STPDFS_SB_REV   STPDFS_SB_REV_1
# define STPDFS_SB_REV_1 1

# define STPDFS_INO_ROOTDIR 1

# define STPDFS_BLOCK_SIZE_BITS 9
# define STPDFS_BLOCK_SIZE (1 << STPDFS_BLOCK_SIZE_BITS)

# define STPDFS_NAME_MAX 255

# define STPDFS_INODES_PER_BLOCK (STPDFS_BLOCK_SIZE / (sizeof(struct stpdfs_inode)))
# define STPDFS_DIRENT_PER_BLOCK (STPDFS_BLOCK_SIZE / (sizeof(struct stpdfs_dirent)))

# define STPDFS_ROOT_INO 1

# define STPDFS_INO_FLAG_ALOC   (1 << 15)
# define STPDFS_INO_FLAG_LARGE  (1 << 0)
# define STPDFS_INO_FLAG_LZP    (1 << 1)
# define STPDFS_INO_FLAG_ENC    (1 << 2)

# define STPDFS_S_IFMT   0xF000
# define STPDFS_S_IFSOCK 0xA000
# define STPDFS_S_IFLNK  0xC000
# define STPDFS_S_IFREG  0x8000
# define STPDFS_S_IFBLK  0x6000
# define STPDFS_S_IFDIR  0x4000

typedef uint32_t zone_t; /**< zone number */
typedef uint32_t block_t; /**< block number */

/**
 * \brief free block list
 */
struct stpdfs_free {
	uint32_t free[100];
	uint8_t nfree;
} __attribute__((packed));

enum stpdfs_state {
	STPDFS_CLEANLY_UNMOUNTED = 0,
	STPDFS_ERROR             = 1,
	STPDFS_DIRTY             = 1,
};

/**
 * \brief StupidFS Superblock
 */
struct stpdfs_sb {
	uint32_t magic; /**< MUST be \ref STPDFS_SB_MAGIC */
	uint32_t isize; /**< size in block of the I list */
	uint32_t fsize; /**< size in block of the entire volume */
	uint32_t free[100];
	uint8_t nfree; /**< number of free block (0-100) */
	uint8_t revision; /**< MUST be \ref STPDFS_SB_REV */
	uint16_t state; /**<  \see stpdfs_state */
	uint64_t time; /**< last time the superblock was modified */
} __attribute__((packed));

#define STPDFS_SB_SIZE sizeof(struct stpdfs_sb)

/**
 * \brief StupidFS I-node
 */
struct stpdfs_inode {
	uint16_t mode; /**< file mode */
	uint16_t nlink; /**< link count */
	uint16_t uid;   /**< owner user id */
	uint16_t gid;   /**< group id */
	uint16_t flags;
	uint32_t size;
	uint32_t zones[10];
	uint64_t actime;
	uint64_t modtime;
} __attribute__((packed));

#define STPDFS_INODE_SIZE sizeof(struct inode)

struct stpdfs_dirent {
	uint32_t inode;
	char filename[STPDFS_NAME_MAX];
};

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
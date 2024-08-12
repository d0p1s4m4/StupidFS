/**
 * \file stupidfs.h
 *
 * StupidFS Filesystem
 *
 * ```
 * ┌──────────┬───────────┬──────┬───┬──────┬────┬───┬────┐
 * │Boot block│Super block│Inodes│...│Inodes│Data│...│Data│
 * └──────────┴───────────┴──────┴───┴──────┴────┴───┴────┘
 * ```
 */
#ifndef STUPIDFS_H
# define STUPIDFS_H 1

# ifndef __KERNEL__
#  include <stdint.h>
#  include <stddef.h>
# endif

# define STPDFS_BLOCK_SIZE 512 /**< StupidFS block size */
# define STPDFS_INODES_PER_BLOCK (STPDFS_BLOCK_SIZE / (sizeof(struct stpdfs_inode)))
# define STPDFS_ZONES_PER_BLOCK (STPDFS_BLOCK_SIZE / sizeof(uint32_t))
# define STPDFS_DIRENT_PER_BLOCK (STPDFS_BLOCK_SIZE / (sizeof(struct stpdfs_dirent)))

# define STPDFS_SB_BLOCK 1

# define STPDFS_SB_MAGIC 0x44505453 /**< Superblock magic number */
# define STPDFS_SB_REV   STPDFS_SB_REV_1 /**< current revision */
# define STPDFS_SB_REV_1 1

# define STPDFS_BADINO  0 /**< StupidFS bad inode number */
# define STPDFS_ROOTINO 1 /**< StupidFS root inode number */

# define STPDFS_NAME_MAX 28 /**< Max filename length */

/**
 * \brief free block list
 * ```
 * ┌──────────┐                                                           
 * │ block 99 │                              
 * ├──────────┤                              
 * │ block 98 │                              
 * ├──────────┤                              
 * │ ...      │                              
 * ├──────────┤                              
 * │ block 2  │                              
 * ├──────────┤                              
 * │ block 1  │                              
 * ├──────────┤    ┌──────────┐              
 * │ block 0  ├───►│ block 99 │              
 * └──────────┘    ├──────────┤              
 *                 │ ...      │              
 *                 ├──────────┤   ┌──────────┐
 *                 │ block 0  ├──►│ block 99 │
 *                 └──────────┘   ├──────────┤
 *                                │ ...      │
 *                                ├──────────┤
 *                                │ block 0  │
 *                                └──────────┘ 
 * ```
 */
struct stpdfs_free {
	uint32_t free[100]; /**< List of free block (0-99), index 0 point to next freelist */
	uint8_t nfree; /**< index  */
};

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
	uint32_t isize; /**< Size in block of the I list */
	uint32_t fsize; /**< Size in block of the entire volume */
	struct stpdfs_free freelist; /**< \see stpdfs_free */
	uint8_t revision; /**< MUST be \ref STPDFS_SB_REV */
	uint16_t state; /**<  \see stpdfs_state */
	uint64_t time; /**< Last time the superblock was modified */
} __attribute__((packed));

#define STPDFS_SB_SIZE sizeof(struct stpdfs_sb)

/**
 * \brief StupidFS I-node
 *
 * ```
 *                         ┌────────┐                                                
 *                         │        │                                                
 *                ┌───────►│        │                                                
 *                │        │        │                                                
 * ┌──────┐ Direct│        └────────┘                                                
 * │zone 0├───────┘                                                                  
 * ├──────┤                                                                          
 * │...   │                                                                          
 * ├──────┤          ┌────────┐      ┌────────┐                                      
 * │zone 6│          │        ├─────►│        │                                      
 * ├──────┤ Indirect │        │      │        │                                      
 * │zone 7├─────────►│        │      │        │                                      
 * ├──────┤          └────────┘      └────────┘                                      
 * │zone 8├───────┐                                                                  
 * ├──────┤       │Double indirect┌────────┐    ┌────────┐    ┌────────┐           
 * │zone 9│       └──────────────►│        ├───►│        ├───►│        │           
 * └──┬───┘                       │        │    │        │    │        │           
 *    │                           │        │    │        │    │        │           
 *    │                           └────────┘    └────────┘    └────────┘           
 *    │ Triple indirect ┌────────┐                                                   
 *    └────────────────►│        │    ┌────────┐      ┌────────┐      ┌────────┐
 *                      │        ├───►│        │      │        │      │        │
 *                      │        │    │        ├─────►│        ├─────►│        │
 *                      └────────┘    │        │      │        │      │        │
 *                                    └────────┘      └────────┘      └────────┘
 * ```
 */
struct stpdfs_inode {
	uint16_t mode; /**< File mode */
	uint16_t nlink; /**< Link count */
	uint16_t uid;   /**< Owner user id */
	uint16_t gid;   /**< Group id */
	uint16_t flags; /** File flags */
	uint32_t size; /** Data size in byte */
	uint32_t zones[10];
	uint64_t actime; /**< Access time */
	uint64_t modtime; /**< Modification time */
} __attribute__((packed));
#define STPDFS_INODE_SIZE sizeof(struct inode)

# define STPDFS_NDIR 7 /**< Number of direct block */
# define STPDFS_SIND 7 /**< Indirect block */
# define STPDFS_DIND 8 /**< Double indirect */
# define STPDFS_TIND 9 /**< Triple indirect */

# define STPDFS_INO_FLAG_ALOC   (1 << 15)
# define STPDFS_INO_FLAG_LZP    (1 << 1)
# define STPDFS_INO_FLAG_ENC    (1 << 2)


# define STPDFS_S_IFMT   0xF000
# define STPDFS_S_IFSOCK 0xA000
# define STPDFS_S_IFLNK  0xC000
# define STPDFS_S_IFREG  0x8000
# define STPDFS_S_IFBLK  0x6000
# define STPDFS_S_IFDIR  0x4000

# define STPDFS_S_ISUID 0x0800
# define STPDFS_S_ISGID 0x0400
# define STPDFS_S_ISVTX 0x0200
  	  	 
# define STPDFS_S_IRWXU 0x01C0 
# define STPDFS_S_IRUSR 0x0100
# define STPDFS_S_IWUSR 0x0080
# define STPDFS_S_IXUSR 0x0040
  	  	 
# define STPDFS_S_IRWXG 0x0038
# define STPDFS_S_IRGRP 0x0020
# define STPDFS_S_IWGRP 0x0010
# define STPDFS_S_IXGRP 0x0008
  	  	 
# define STPDFS_S_IRWXO 0x0007
# define STPDFS_S_IROTH 0x0004
# define STPDFS_S_IWOTH 0x0002
# define STPDFS_S_IXOTH 0x0001

/**
 * \brief StupidFS directory entry
 */
struct stpdfs_dirent {
	uint32_t inode; /**< inode containing file */
	char filename[STPDFS_NAME_MAX]; /** null terminated file name (\see STPDFS_NAME_MAX) */
};
#define STPDFS_DIRENT_SIZE sizeof(struct stpdfs_dirent)

#endif /* !STPIDFS_H */
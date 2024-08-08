
#ifndef FS_BIO_H
# define FS_BIO_H 1

# include <stdint.h>
# include <stupidfs.h>

struct fs_buffer {
	int valid;
	uint32_t blocknum;
	int refcount;
	int fd;

	struct fs_buffer *next;

	uint8_t data[STPDFS_BLOCK_SIZE];
};

size_t fs_bio_raw_write(int fd, uint32_t blocknum, void *data, size_t size);
size_t fs_bio_raw_read(int fd, uint32_t blocknum, void *data, size_t size);
struct fs_buffer *fs_bio_bread(int fd, uint32_t blocknum);
void fs_bio_brelse(struct fs_buffer *buff);
void fs_bio_bwrite(struct fs_buffer *buff);

#endif /* !FS_BIO_H */
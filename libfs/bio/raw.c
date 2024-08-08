#include <stdint.h>
#include <unistd.h>
#include <stupidfs.h>

size_t
fs_bio_raw_write(int fd, uint32_t blocknum, void *data, size_t size)
{
	if (size > STPDFS_BLOCK_SIZE)
	{
		return (-1);
	}

	if (lseek(fd, blocknum * STPDFS_BLOCK_SIZE, SEEK_SET) == -1)
	{
		return (-1);
	}

	return (write(fd, data, size));
}

size_t
fs_bio_raw_read(int fd, uint32_t blocknum, void *data, size_t size)
{
	if (size > STPDFS_BLOCK_SIZE)
	{
		return (-1);
	}

	if (lseek(fd, blocknum * STPDFS_BLOCK_SIZE, SEEK_SET) == -1)
	{
		return (-1);
	}

	return (read(fd, data, size));
}

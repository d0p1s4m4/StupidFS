#include "stpdfs.h"
#include <stdint.h>

size_t
stpdfs_write(int fd, uint32_t blocknum, void *data, size_t size)
{
	return (0);
}

size_t
stpdfs_read(int fd, uint32_t blocknum, void *data, size_t size)
{
	return (0);
}

uint32_t
stpdfs_alloc_block(int fd)
{
	return (0);
}

int
stpdfs_free_block(int fd)
{
	return (0);
}
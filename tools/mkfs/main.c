#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stpdfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */
#ifdef HAVE_LIBGEN_H
# include <libgen.h>
#endif /* HAVE_LIBGEN_H */
#ifdef HAVE_GETOPT_H
# include <getopt.h>
#endif /* HAVE_GETOPT_H */

static const char *prg_name;
static int inodes = -1;
static const char *device;
static int blocks = -1;
static struct stpdfs_sb super;

#ifdef HAVE_STRUCT_OPTION
static struct option long_options[] = {
	{"help", no_argument, 0, 'h'},
	{"version", no_argument, 0, 'V'},
	{"inodes", required_argument, 0, 'i'},
	{0, 0, 0, 0}
};
#endif /* HAVE_STRUCT_OPTION */

static void
usage(int retval)
{
	if (retval == EXIT_FAILURE)
	{
		fprintf(stderr, "Try '%s -h' for more information.\n", prg_name);
	}
	else
	{
		printf("Usage: %s [options] /dev/name [blocks]\n\n", prg_name);
		printf("Options:\n");
		printf(" -i, --inode <num>  number of inodes for the filesystem\n");
		printf(" -h, --help         display this help\n");
		printf(" -V, --version      display version\n\n");

		printf("For more details see mkfs.stpd(8).\n");
	}
	exit(retval);
}

static void
version(void)
{
	printf("%s (%s) %s\n", prg_name, PACKAGE_NAME, PACKAGE_VERSION);
	exit(EXIT_SUCCESS);
}

static int
write_block(int fd, void *data, size_t size)
{
	uint8_t buffer[STPDFS_BLOCK_SIZE];

	memset(buffer, 0, STPDFS_BLOCK_SIZE);
	if (data)
	{
		memcpy(buffer, data, size);
	}

	return (write(fd, buffer, STPDFS_BLOCK_SIZE) == STPDFS_BLOCK_SIZE); 
}


static int
mkfs()
{
	struct stat statbuf;
	struct stpdfs_dirent rootdirent[2];
	int fd;
	int dev_block;
	int idx;
	struct stpdfs_inode inds[STPDFS_INODES_PER_BLOCK];
	struct stpdfs_free freebuff;
	int freeblock;
	int nextfree;

	if (stat(device, &statbuf) < 0)
	{
		perror(device);
		return (EXIT_FAILURE);
	}

	fd = open(device, O_RDWR);
	if (fd < 0)
	{
		perror(device);
		return (EXIT_FAILURE);
	}

	dev_block = statbuf.st_size / STPDFS_BLOCK_SIZE;
	printf("device blocks: %d\n", dev_block);
	if (blocks < 0)
	{
		blocks = dev_block;
	}

	if (inodes <= 0)
	{
		if (512 * 1024 < blocks)
		{
			inodes = blocks / 8;
		}
		else
		{
			inodes = blocks / 3;
		}
	}

	super.magic = STPDFS_SB_MAGIC;
	super.revision = STPDFS_SB_REV;
	super.isize = inodes / STPDFS_INODES_PER_BLOCK;
	super.fsize = blocks;
	super.nfree = 0;

	/* write inodes */
	lseek(fd, 2 * STPDFS_BLOCK_SIZE, SEEK_SET);
	memset(&inds, 0, sizeof(struct stpdfs_inode) * STPDFS_INODES_PER_BLOCK);
	for (idx = 0; idx < super.isize; idx++)
	{
		if (write_block(fd, &inds, sizeof(struct stpdfs_inode) * STPDFS_INODES_PER_BLOCK));
	}

	/* set free blocks */
	freeblock = blocks - (inodes + 2);
	for (nextfree = super.isize + 3; nextfree < freeblock; nextfree++)
	{
		if (stpdfs_free_block(fd, &super, nextfree) != 0)
		{
			fprintf(stderr, "error: %u\n", nextfree);
		}
	}

	/* create root dir */
	stpdfs_read(fd, 2, &inds, sizeof(struct stpdfs_inode) * STPDFS_INODES_PER_BLOCK);
	inds[STPDFS_INO_ROOTDIR].modtime = time(NULL);
	inds[STPDFS_INO_ROOTDIR].actime = time(NULL);
	inds[STPDFS_INO_ROOTDIR].size = sizeof(struct stpdfs_dirent) * 2;
	inds[STPDFS_INO_ROOTDIR].flags = STPDFS_INO_FLAG_ALOC;
	inds[STPDFS_INO_ROOTDIR].mode = STPDFS_S_IFDIR;
	inds[STPDFS_INO_ROOTDIR].zones[0] = stpdfs_alloc_block(fd, &super);
	inds[STPDFS_INO_ROOTDIR].size = sizeof(struct stpdfs_dirent) * 2;
	stpdfs_read(fd, inds[STPDFS_INO_ROOTDIR].zones[0], rootdirent, sizeof(struct stpdfs_dirent) * 2);
	strcpy(rootdirent[0].filename, ".");
	rootdirent[1].inode = 1;
	strcpy(rootdirent[1].filename, "..");
	rootdirent[1].inode = 1;
	stpdfs_write(fd, inds[STPDFS_INO_ROOTDIR].zones[0], rootdirent, sizeof(struct stpdfs_dirent) * 2);
	stpdfs_write(fd, 2, &inds, sizeof(struct stpdfs_inode) * STPDFS_INODES_PER_BLOCK);

	/* write super block */
	super.time = time(NULL);
	super.state = STPDFS_CLEANLY_UNMOUNTED;
	stpdfs_write(fd, 1, &super, sizeof(struct stpdfs_sb));

	/* we finished \o/  */
	printf("StupidFS: %u blocks (%u Inodes)\n", super.fsize, super.isize);

	close(fd);

	return (EXIT_SUCCESS);
}

int
main(int argc, char **argv)
{
	int idx;
	int c;

#ifdef HAVE_LIBGEN_H
	prg_name = basename(argv[0]);
#else
	prg_name = argv[0];
#endif /* HAVE_LIBGEN_H */

#if defined(HAVE_GETOPT_LONG) && defined(HAVE_STRUCT_OPTION) 
	while ((c = getopt_long(argc, argv, "hVi:", long_options, &idx)) != EOF)
#else
	while ((c = getopt(argc, argv, "hVi:")) != EOF)
#endif /* HAVE_GETOPT_LONG && HAVE_STRUCT_OPTION */
	{
		switch (c)
		{
		case 'h':
			usage(EXIT_SUCCESS);
			break;
		case 'V':
			version();
			break;
		case 'i':
			inodes = atoi(optarg);
			break;
		default:
			usage(EXIT_FAILURE);
			break;
		}
	}

	if (optind >= argc)
	{
		usage(EXIT_FAILURE);
	}

	device = argv[optind++];
	if (optind < argc)
	{
		blocks = atoi(argv[optind]);
	}

	return (mkfs());
}

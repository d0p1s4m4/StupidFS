#include "libfs/bio/bio.h"
#include "libfs/inode.h"
#include "libfs/super.h"
#include "stupidfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <libfs/fs.h>
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
static int bootable = 0;

extern const char _binary_mkfs_boot_o_start[];
extern const char _binary_mkfs_boot_o_end[];

#ifdef HAVE_STRUCT_OPTION
static struct option long_options[] = {
	{"help", no_argument, 0, 'h'},
	{"version", no_argument, 0, 'V'},
	{"boot", no_argument, 0, 'b'},
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
		printf(" -b, --boot         make filesystem bootable\n");
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
create_create_super(struct fs_super *super)
{
	struct stat st;
	int dev_block;

	if (stat(device, &st) < 0)
	{
		perror(device);
		return (-1);
	}
#ifdef _WIN32
	super->fd = open(device, O_RDWR | O_BINARY);
#else
	super->fd = open(device, O_RDWR);
#endif 
	if (super->fd < 0)
	{
		perror(device);
		return (-1);
	}

	dev_block = st.st_size / STPDFS_BLOCK_SIZE;
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

	super->sb.magic = STPDFS_SB_MAGIC;
	super->sb.revision = STPDFS_SB_REV;
	super->sb.isize = inodes / STPDFS_INODES_PER_BLOCK;
	super->sb.fsize = blocks;
	super->sb.freelist.nfree = 0;

	return (0);
}

static int
mkfs()
{
	uint32_t idx;
	struct fs_super super;
	struct stpdfs_inode dinodes[STPDFS_INODES_PER_BLOCK];
	struct fs_inode *rootip;

	if (create_create_super(&super) != 0)
	{
		return (EXIT_FAILURE);
	}

	/* write inodes */
	memset(dinodes, 0, sizeof(struct stpdfs_inode) * STPDFS_INODES_PER_BLOCK);
	for (idx = 2; idx < (super.sb.isize + 2); idx++)
	{
		if (fs_bio_raw_write(super.fd, idx, dinodes, sizeof(struct stpdfs_inode) * STPDFS_INODES_PER_BLOCK) != sizeof(struct stpdfs_inode) * STPDFS_INODES_PER_BLOCK)
		{
			fprintf(stderr, "wtf?:");
		}
	}

	/* set free blocks */
	for (idx = super.sb.isize + 3; idx < blocks; idx++)
	{
		if (fs_bfree(&super, idx) != 0)
		{
			fprintf(stderr, "error: %u\n", idx);
		}
	}

	rootip = fs_inode_get(&super, STPDFS_ROOTINO);
	if (rootip == NULL)
	{
		return (EXIT_FAILURE);
	}
	
	rootip->valid = 1;

	rootip->inode.gid = 0;
	rootip->inode.uid = 0;
	rootip->inode.modtime = time(NULL);
	rootip->inode.actime = time(NULL);
	memset(rootip->inode.zones, 0, sizeof(uint32_t) * 10);
	rootip->inode.size = 0;
	rootip->inode.flags = STPDFS_INO_FLAG_ALOC;
	rootip->inode.mode = STPDFS_S_IFDIR | STPDFS_S_IRUSR | STPDFS_S_IWUSR | STPDFS_S_IXUSR | STPDFS_S_IRGRP | STPDFS_S_IXGRP | STPDFS_S_IXOTH;

	fs_inode_update(rootip);

	if (fs_dir_link(rootip, ".", rootip->inum) != 0)
	{
		printf("WTF?");
	}
	fs_dir_link(rootip, "..", rootip->inum);

	rootip->inode.nlink = 2;

	fs_inode_update(rootip);
	fs_inode_release(rootip);

	if (bootable)
	{
		fs_bio_raw_write(super.fd, 0, (void *)_binary_mkfs_boot_o_start, STPDFS_BLOCK_SIZE);
	}

	fs_super_kill(&super);

	/* we finished \o/  */
	printf("StupidFS: %u blocks (%u Inodes)\n", super.sb.fsize, super.sb.isize);

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
	while ((c = getopt_long(argc, argv, "hVbi:", long_options, &idx)) != EOF)
#else
	while ((c = getopt(argc, argv, "hVbi:")) != EOF)
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
		case 'b':
			bootable = 1;
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

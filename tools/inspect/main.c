#include "libfs/inode.h"
#include "libfs/super.h"
#include <stdio.h>
#include <stdlib.h>
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
#include <libfs/fs.h>

static const char *prg_name;
static const char *device;
static int inode = -1;
static int block = -1;
static int super = 0;

#ifdef HAVE_STRUCT_OPTION
static struct option long_options[] = {
	{"help", no_argument, 0, 'h'},
	{"version", no_argument, 0, 'V'},
	{"inode", required_argument, 0, 'i'},
	{"block", required_argument, 0, 'b'},
	{"super", no_argument, 0, 's'},
	{0, 0, 0, 0}
};
#endif /* HAVE_STRUCT_OPTION */


static int
inspect(void)
{
	struct fs_super sb;
	struct fs_inode *ip;
	time_t time;
	int idx;

	if (fs_super_open(&sb, device) != 0)
	{
		fprintf(stderr, "Bad super\n");
		return (EXIT_FAILURE);
	}

	if (super)
	{
		printf("Super:\n");
		printf(" magic: %x\n", sb.sb.magic);
		printf(" isize: %u\n", sb.sb.isize);
		printf(" fsize: %u\n", sb.sb.fsize);
		printf(" freelist:\n");
		for (idx = 0; idx < sb.sb.freelist.nfree; idx++)
		{
			printf("\tblock[%02u]: %x\n", idx, sb.sb.freelist.free[idx]);
		}
		printf(" revision: %u\n", sb.sb.revision);
		printf(" state: %hu\n", sb.sb.state);
		time = sb.sb.time;
		printf(" time: %s", ctime(&time));
	}

	if (inode > 0)
	{
		ip = fs_inode_get(&sb, inode);
		if (ip->valid == 0)
		{
			fs_inode_read(ip);
		}

		printf("Inode %d:\n", inode);
		printf(" mode: %ho\n", ip->inode.mode);
		printf(" nlink: %hu\n", ip->inode.nlink);
		printf(" size: %u\n", ip->inode.size);
		time = ip->inode.actime;
		printf(" actime: %s", ctime(&time));
		time = ip->inode.modtime;
		printf(" modtime: %s", ctime(&time));
	}


	fs_super_kill(&sb);
	return (EXIT_SUCCESS);
}

static void
usage(int retval)
{
	if (retval == EXIT_FAILURE)
	{
		fprintf(stderr, "Try '%s -h' for more information.\n", prg_name);
	}
	else
	{
		printf("Usage %s: [options] /dev/name\n\n", prg_name);
	}

	exit(retval);
}

static void
version(void)
{
	printf("%s (%s) %s\n", prg_name, PACKAGE_NAME, PACKAGE_VERSION);
	exit(EXIT_SUCCESS);
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
	while ((c = getopt_long(argc, argv, "hVi:b:s", long_options, &idx)) != EOF)
#else
	while ((c = getopt(argc, argv, "hVi:b:s")) != EOF)
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
			inode = atoi(optarg);
			break;

		case 'b':
			block = atoi(optarg);
			break;

		case 's':
			super = 1;
			break;

		default:
			usage(EXIT_FAILURE);
		}
	}

	if (optind >= argc)
	{
		usage(EXIT_FAILURE);
	}

	device = argv[optind];

	return (inspect());
}
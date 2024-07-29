
#include "stupidfs.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */
#ifdef HAVE_LIBGEN_H
# include <libgen.h>
#endif /* HAVE_LIBGEN_H */
#ifdef HAVE_GETOPT_H
# include <getopt.h>
#endif /* HAVE_GETOPT_H */
#include <libstpdfs/stpdfs.h>

#ifdef HAVE_STRUCT_OPTION
static struct option long_options[] = {
	{"help", no_argument, 0, 'h'},
	{"image", required_argument, 0, 'i'},
	{0, 0, 0, 0}
};
#endif /* HAVE_STRUCT_OPTION */

static char *path = "/";
static char *image = NULL;

extern char *prg_name;

static void
usage(int retval)
{
	if (retval == EXIT_FAILURE)
	{
		printf("Try '%s ls -h' for more informations.\n", prg_name);
	}
	else
	{
		printf("Usage: %s copy -i /dev/name [path]\n", prg_name);
	}

	exit(retval);
}

static int
do_ls(void)
{
	struct stpdfs_super_info sbi;
	struct stpdfs_inode_info *ip;
	struct stpdfs_dirent dirent;
	size_t idx;

	if (stpdfs_super_open(&sbi, image))
	{
		return (EXIT_FAILURE);
	}

	ip = stpdfs_inode_get(&sbi, STPDFS_ROOTINO);
	for (idx = 0; idx < ip->inode.size; idx += STPDFS_DIRENT_SIZE)
	{
		stpdfs_inode_read(ip, (uint8_t *)&dirent, idx, STPDFS_DIRENT_SIZE);
		printf("%s\n", dirent.filename);
	}


	stpdfs_super_kill(&sbi);
	return (EXIT_SUCCESS);
}

int
ls(int argc, char **argv)
{
	int idx;
	int c;

#if defined(HAVE_GETOPT_LONG) && defined(HAVE_STRUCT_OPTION)
	while ((c = getopt_long(argc, argv, "hi:", long_options, &idx)) != EOF)
#else
	while ((c = getopt(argc, argv, "hi:")) != EOF)
#endif
	{
		switch (c)
		{
		case 'h':
			usage(EXIT_SUCCESS);
			break;

		case 'i':
			image = optarg;
			break;

		default:
			usage(EXIT_FAILURE);
			break;
		}
	}

	if (image == NULL)
	{
		return (EXIT_FAILURE);
	}

	if (optind < argc)
	{
		path = argv[optind];
	}


	return (do_ls());
}
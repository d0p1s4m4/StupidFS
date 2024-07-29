#include "stupidfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
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

static char *dest = NULL;
static char *src = NULL;
static char *image = NULL;

extern char *prg_name;

static void
usage(int retval)
{
	if (retval == EXIT_FAILURE)
	{
		printf("Try '%s copy -h' for more informations.\n", prg_name);
	}
	else
	{
		printf("Usage: %s copy -i /dev/name source [dest]\n", prg_name);
	}
}

int
do_copy(void)
{
	struct stpdfs_super_info sbi;
	struct stpdfs_inode_info *ip;
	struct stpdfs_dirent dirent;
	struct stat st;
	
	stat(src, &st);
	strcpy(dirent.filename, dest);
	if (stpdfs_super_open(&sbi, image))
	{
		return (EXIT_FAILURE);
	}

	ip = stpdfs_inode_get(&sbi, STPDFS_ROOTINO);

	stpdfs_create(ip, &dirent, st.st_mode);

	stpdfs_super_kill(&sbi);

	return (EXIT_SUCCESS);
}

int
copy(int argc, char **argv)
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
		src = argv[optind++];
	}
	else
	{
		usage(EXIT_FAILURE);
	}

	if (optind < argc)
	{
		dest = argv[optind];
	}

	if (dest == NULL)
	{
		dest = src;
	} 

	return (do_copy());
}
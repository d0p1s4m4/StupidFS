#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
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
#include <stupidfs.h>
#include <libfs/fs.h>

#ifdef HAVE_STRUCT_OPTION
static struct option long_options[] = {
	{"help", no_argument, 0, 'h'},
	{"image", required_argument, 0, 'i'},
	{"mode", required_argument, 0, 'm'},
	{"parents", no_argument, 0, 'p'},
	{"verbose", no_argument, 0, 'v'},
	{0, 0, 0, 0}
};
#endif /* HAVE_STRUCT_OPTION */

static char *image = NULL;
static mode_t mode = STPDFS_S_IRUSR | STPDFS_S_IWUSR | STPDFS_S_IXUSR | STPDFS_S_IRGRP | STPDFS_S_IXGRP | STPDFS_S_IXOTH;
static int parents = 0;
static int verbose = 0;

extern char *prg_name;

static void
usage(int retval)
{
	if (retval == EXIT_FAILURE)
	{
		printf("Try '%s mkdir -h' for more informations.\n", prg_name);
	}
	else
	{
		printf("Usage: %s mkdir -i /dev/name [OPTION]... DIRECTORY...\n", prg_name);
		printf("Options:\n");
		printf(" -i, --image <image>  specify disk image.\n");
		printf(" -m, --mode <mode>    set file mode\n");
		printf(" -p, --parents        no error if existing, make parent directories as needed\n");
		printf(" -v, --verbose        print a message for each created directory\n");
		printf(" -h, --help           display this help\n");
	}
}

int
do_mkdir(struct fs_super *super, char *path)
{
	char *dir;
	char *tmp;
	struct fs_inode *dp;
	struct fs_inode *subdp;

	dir = strtok(path, "/");
	dp = fs_inode_get(super, STPDFS_ROOTINO);
	if (dp->valid == 0)
	{
		fs_inode_read(dp);
	}

	while (dir != NULL)
	{
		tmp = strtok(NULL, "/");
		subdp = fs_dir_lookup(dp, dir, NULL);
		if (tmp == NULL)
		{
			if (subdp != NULL)
			{
				fprintf(stderr, "mkdir: cannot create directory '%s': File exists", dir);
				return (EXIT_FAILURE);
			}
			
			fs_mkdir(dp, dir, mode);
			return (EXIT_SUCCESS);
		}
		printf("%s\n", dir);
		dir = tmp;
	}

	return (EXIT_SUCCESS);
}

int
cmd_mkdir(int argc, char **argv)
{
	int idx;
	int c;
	struct fs_super super;
	struct fs_inode *root;
	int status;

#if defined(HAVE_GETOPT_LONG) && defined(HAVE_STRUCT_OPTION)
	while ((c = getopt_long(argc, argv, "hi:m:pv", long_options, &idx)) != EOF)
#else
	while ((c = getopt(argc, argv, "hi:m:pv")) != EOF)
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

		case 'm':
			mode = strtol(optarg, NULL, 8);
			mode |= STPDFS_S_IFDIR;
			break;
		
		case 'p':
			parents = 1;
			break;
		
		case 'v':
			verbose = 1;
			break;

		default:
			usage(EXIT_FAILURE);
			break;
		}
	}

	if (image == NULL)
	{
		usage(EXIT_FAILURE);
	}

	if (optind >= argc)
	{
		usage(EXIT_FAILURE);
	}

	if (fs_super_open(&super, image))
	{
		return (EXIT_FAILURE);
	}

	status = EXIT_SUCCESS;

	while (optind < argc)
	{
		if (do_mkdir(&super, argv[optind++]))
		{
			status = EXIT_FAILURE;
			break;
		}
	}

	fs_super_kill(&super);


	return (status);
}
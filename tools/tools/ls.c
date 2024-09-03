#include "libfs/inode.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
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
#include "libfs/fs.h"

#ifdef HAVE_STRUCT_OPTION
static struct option long_options[] = {
	{"help", no_argument, 0, 'h'},
	{"image", required_argument, 0, 'i'},
	{"all", no_argument, 0, 'a'},
	{0, 0, 0, 0}
};
#endif /* HAVE_STRUCT_OPTION */

static char *path = "/";
static char *image = NULL;
static int all = 0;

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
		printf("Usage: %s ls -i /dev/name [OPTIONS]... FILE...\n", prg_name);
		printf("Options:\n");
		printf(" -i, --image <image>  specify disk image.\n");
		printf(" -a, --all            do not ignore entries starting with .\n");
		printf(" -h, --help           display this menu\n");
	}

	exit(retval);
}

static void
print_mode(struct fs_inode *ip)
{
	char modebuff[] = "----------";

	if (ip->inode.mode & STPDFS_S_IFDIR)
	{
		modebuff[0] = 'd';
	}

	if (ip->inode.mode & STPDFS_S_IRUSR)
	{
		modebuff[1] = 'r';
	}

	if (ip->inode.mode & STPDFS_S_IWUSR)
	{
		modebuff[2] = 'w';
	}

	if (ip->inode.mode & STPDFS_S_IXUSR)
	{
		modebuff[3] = 'x';
	}

	if (ip->inode.mode & STPDFS_S_IRGRP)
	{
		modebuff[4] = 'r';
	}

	if (ip->inode.mode & STPDFS_S_IWGRP)
	{
		modebuff[5] = 'w';
	}

	if (ip->inode.mode & STPDFS_S_IXGRP)
	{
		modebuff[6] = 'x';
	}

	if (ip->inode.mode & STPDFS_S_IROTH)
	{
		modebuff[7] = 'r';
	}

	if (ip->inode.mode & STPDFS_S_IWOTH)
	{
		modebuff[8] = 'w';
	}

	if (ip->inode.mode & STPDFS_S_IXOTH)
	{
		modebuff[9] = 'x';
	}

	printf("%s ", modebuff);
}

static void
print_time(struct fs_inode *ip)
{
	char timebuff[13];
	time_t time;

	time = ip->inode.modtime;

	strftime(timebuff, 13, "%b %y %H:%M", localtime(&time));
	printf("%s ", timebuff);
}

static void 
print_dirent(struct fs_super *super, struct stpdfs_dirent *dirent)
{
	struct fs_inode *ip;

	ip = fs_inode_get(super, dirent->inode);
	if (ip->valid == 0)
	{
		fs_inode_read(ip);
	}
	print_mode(ip);
	printf("%u ", ip->inode.nlink);
	printf("%3u %3u ", ip->inode.uid, ip->inode.gid);
	printf("%5u ", ip->inode.size);

	print_time(ip);

	printf("%s\n", dirent->filename);

	fs_inode_release(ip);
}

static int
do_ls(void)
{
	struct fs_super super;
	struct fs_inode *ip;
	struct stpdfs_dirent dirent;
	size_t idx;

	if (fs_super_open(&super, image))
	{
		return (EXIT_FAILURE);
	}

	ip = fs_inode_get(&super, STPDFS_ROOTINO);
	if (ip->valid == 0)
	{
		fs_inode_read(ip);
	}

	for (idx = 0; idx < ip->inode.size; idx += STPDFS_DIRENT_SIZE)
	{
		fs_read(ip, (uint8_t *)&dirent, idx, STPDFS_DIRENT_SIZE);
		if (dirent.inode == 0)
		{
			continue;
		}

		if (dirent.filename[0] == '.')
		{
			if (all) print_dirent(&super, &dirent);
		}
		else
		{
			print_dirent(&super, &dirent);
		}
	}


	fs_super_kill(&super);
	return (EXIT_SUCCESS);
}

int
cmd_ls(int argc, char **argv)
{
	int idx;
	int c;

#if defined(HAVE_GETOPT_LONG) && defined(HAVE_STRUCT_OPTION)
	while ((c = getopt_long(argc, argv, "hi:a", long_options, &idx)) != EOF)
#else
	while ((c = getopt(argc, argv, "hi:a")) != EOF)
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
		
		case 'a':
			all = 1;
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
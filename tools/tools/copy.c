#include <stdio.h>
#include <stdlib.h>
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
# include <stupidfs.h>
# include "libfs/fs.h"

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
	struct fs_super super;
	struct fs_inode *ip;
	struct fs_inode *dp;
	struct stat st;
	FILE *fp;
	uint8_t buffer[512];
	size_t nread;
	size_t total;
	
	stat(src, &st);

	if (fs_super_open(&super, image))
	{
		return (EXIT_FAILURE);
	}

	dp = fs_inode_get(&super, STPDFS_ROOTINO);
	if (dp->valid == 0)
	{
		fs_inode_read(dp);
	}

	ip = fs_inode_alloc(&super);
	ip->inode.mode = st.st_mode;
	ip->inode.uid = st.st_uid;
	ip->inode.gid = st.st_gid;
	ip->inode.modtime = st.st_mtime;
	ip->inode.actime = st.st_atime;
	ip->inode.flags = STPDFS_INO_FLAG_ALOC;

	fs_inode_update(ip);

	fp = fopen(src, "rb");

	total = 0;
	while ((nread = fread(buffer, 1, 512, fp)) > 0)
	{
		fs_write(ip, buffer, total, nread);
		total += nread;
	}

	fclose(fp);

	if (fs_dir_link(dp, dest, ip->inum))
	{
		printf("WTF?!");
	}

	fs_inode_release(dp);
	fs_inode_release(ip);

	fs_super_kill(&super);

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
#ifdef HAVE_LIBGEN_H
	  dest = basename(src);
#else
	  dest = src;
#endif
	} 

	return (do_copy());
}

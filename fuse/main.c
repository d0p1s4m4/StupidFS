#include <fcntl.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <libfs/fs.h>

#define FUSE_USE_VERSION 31
#include <fuse.h>

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */
#ifdef HAVE_LIBGEN_H
# include <libgen.h>
#endif /* HAVE_LIBGEN_H */

static struct path {
	char *name;

	uint32_t inode;

	size_t child_count;
	struct path *child;

} root_path = {
	"/", 1, 0, NULL
};

static const char *prg_name;
static int fd = -1;

static struct options {
	const char *filename;
	int show_help;
	int show_version;
} options;

static struct fs_super super;

#define OPTION(t, p) { t, offsetof(struct options, p), 1 }

static const struct fuse_opt option_spec[] = {
	OPTION("--image=%s", filename),
	OPTION("-h", show_help),
	OPTION("--help", show_help),
	OPTION("-V", show_version),
	OPTION("--version", show_version),
	FUSE_OPT_END
};


static void *
fuse_stpdfs_init(struct fuse_conn_info *conn,
			struct fuse_config *config)
{
  if (fs_super_open(&super, options.filename) != 0)
    {
      exit(EXIT_FAILURE);
    }
	return (NULL);
}

static void
fuse_stpdfs_destroy(void *data)
{
  fs_super_kill(&super);
}

static int
fuse_stpdfs_getattr(const char *path, struct stat *stbuf,
				struct fuse_file_info *fi)
{
  struct fs_inode *ip;

  ip = fs_inode_get(&super, STPDFS_ROOTINO);
  if (ip == NULL)
    {
      return (-1);
    }

  if (ip->valid == 0)
    {
      fs_inode_read(ip);
    }

	stbuf->st_atim.tv_sec = ip->inode.actime;
	stbuf->st_mtim.tv_sec = ip->inode.modtime;
	stbuf->st_size = ip->inode.size;
	stbuf->st_mode = ip->inode.mode;
	stbuf->st_gid = ip->inode.gid;
	stbuf->st_uid = ip->inode.uid;
	stbuf->st_nlink = ip->inode.nlink;
	return (0);
}


static int
fuse_stpdfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
				off_t offset, struct fuse_file_info *fi,
				enum fuse_readdir_flags flags)
{
	return (0);
}

static int
fuse_stpdfs_open(const char *path, struct fuse_file_info *fi)
{
	return (0);
}

static int
fuse_stpdfs_read(const char *path, char *buf, size_t size, off_t offset,
			struct fuse_file_info *fi)
{
	return (size);
}

static const struct fuse_operations stpdfs_oper = {
	.init  = fuse_stpdfs_init,
	.destroy = fuse_stpdfs_destroy,
	.getattr = fuse_stpdfs_getattr,
	.readdir = fuse_stpdfs_readdir,
	.open = fuse_stpdfs_open,
	.read = fuse_stpdfs_read
};

static void
show_version(void)
{
	printf("%s %s\n", PACKAGE_NAME, PACKAGE_VERSION);

	exit(EXIT_SUCCESS);
}

static void
show_help(void)
{
	printf("Usage: %s --image=<filename> <mountpoint>\n", prg_name);
	exit(EXIT_SUCCESS);
}

int
main(int argc, char *argv[])
{
	int ret;
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

#ifdef HAVE_LIBGEN_H
	prg_name = basename(argv[0]);
#else
	prg_name = argv[0];
#endif /* HAVE_LIBGEN_H */

	if (fuse_opt_parse(&args, &options, option_spec, NULL) == -1)
	{
		return (EXIT_FAILURE);
	}

	if (options.show_version) show_version();
	if (options.show_help) show_help();

	if (options.filename)
	{
		options.filename = realpath(options.filename, NULL);
	}
	else
	{
		show_help();
	}

	ret = fuse_main(args.argc, args.argv, &stpdfs_oper, NULL);
	fuse_opt_free_args(&args);
	return (ret);
}

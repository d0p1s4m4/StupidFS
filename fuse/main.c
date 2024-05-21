#include <stdlib.h>

#define FUSE_USE_VERSION 31
#include <fuse.h>

static void *
stpdfs_init(struct fuse_conn_info *conn,
			struct fuse_config *config)
{
	(void)conn;

	return (NULL);
}

static int
stpdfs_getattr(const char *path, struct stat *stbuf,
				struct fuse_file_info *fi)
{
	return (0);
}


static int
stpdfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
				off_t offset, struct fuse_file_info *fi,
				enum fuse_readdir_flags flags)
{
	return (0);
}

static int
stpdfs_open(const char *path, struct fuse_file_info *fi)
{
	return (0);
}

static int
stpdfs_read(const char *path, char *buf, size_t size, off_t offset,
			struct fuse_file_info *fi)
{
	return (size);
}

static const struct fuse_operations stpdfs_oper = {
	.init  = stpdfs_init,
	.getattr = stpdfs_getattr,
	.readdir = stpdfs_readdir,
	.open = stpdfs_open,
	.read = stpdfs_read
};

int
main(int argc, char *argv[])
{
	int ret;
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

	ret = fuse_main(args.argc, args.argv, &stpdfs_oper, NULL);
	fuse_opt_free_args(&args);
	return (ret);
}

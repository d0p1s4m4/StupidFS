AM_CFLAGS = $(FUSE_CFLAGS) -I$(top_srcdir)/lib -I$(top_srcdir)/include -I$(top_srcdir)
LDADD = $(FUSE_LIBS) ../lib/libstpdfs.a

bin_PROGRAMS = stpdfs-fuse

stpdfs_fuse_SOURCES = main.c

AM_CFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/lib -I$(top_srcdir)
LDADD = ../libstpdfs/libstpdfs.a

bin_PROGRAMS = mkfs.stpd tools.stpd

mkfs_stpd_SOURCES = mkfs/main.c
mkfs_stpd_LDADD = ../libstpdfs/libstpdfs.a mkfs/bootsector.o

tools_stpd_SOURCES = tools/main.c tools/ls.c tools/copy.c

mkfs/boot.o: mkfs/boot.asm
	$(NASM) -fbin -o $@ $<

mkfs/bootsector.o: mkfs/boot.o
	ld -r -b binary -o $@ $<

man_MANS = mkfs/mkfs.stpd.8 tools/tools.stpd.8

CLEANFILES = mkfs/boot.o mkfs/bootsector.o
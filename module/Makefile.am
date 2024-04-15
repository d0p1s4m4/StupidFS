moduledir = @moduledir@
KBUILD_VERBOSE = 1
MOD_DEVDIR = $(PWD)

export module_DATA

EXTRA_PROGRAMS = stpdfs_module
stpdfs_module_SOURCES = module_stpdfs.c 
module_DATA = module_stpdfs.o 

$(module_DATA): $(stpdfs_module_SOURCES) 
	mv Makefile Makefile.automake 
	cp $(srcdir)/../Makefile.kernel Makefile 
	CPPFLAGS="" CFLAGS="" LDFLAGS="" \
	$(MAKE) -C /lib/modules/$(shell uname -r)/build \
        ARCH="x86" CC="gcc" M=$(PWD) modules\
		KBUILD_VERBOSE=$(KBUILD_VERBOSE) 
	mv Makefile.automake Makefile 

CLEANFILES = $(module_DATA) .$(module_DATA).flags $(module_DATA:.o=.mod.c) $(module_DATA:.o=.@kernelext@)
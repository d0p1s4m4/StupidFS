AC_INIT([StupidFS], [0.0])

AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([foreign subdir-objects])

AC_REQUIRE_AUX_FILE([tap-driver.sh])

AC_CANONICAL_HOST

AC_LANG(C)
AC_PROG_CC
AC_PROG_CPP
AM_PROG_AR
AC_PROG_INSTALL
AC_PROG_RANLIB
AM_PROG_CC_C_O
AC_CHECK_PROG([NASM], [nasm], [nasm])
AS_IF([test -z "$NASM"], [
	AC_MSG_ERROR([nasm is required])
])

AC_CONFIG_HEADERS([config.h])

AS_IF([ test "x$host_os" != "xmingw32" ], [
	PKG_CHECK_MODULES([FUSE], [fuse3])
])

AM_CONDITIONAL(BUILD_FUSE, test "x$host_os" != "xmingw32")
AM_CONDITIONAL(BUILD_LINUX, test "x$host_os" = "xlinux-gnu")


AC_CHECK_INCLUDES_DEFAULT
AC_C_CONST
AC_C_INLINE

AC_CHECK_FUNCS_ONCE(strtoull)

AC_CHECK_HEADERS(m4_normalize([
	libgen.h
	getopt.h
]))

AC_CHECK_TYPES([struct option], [], [],
[#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif])

AC_CHECK_FUNCS(m4_normalize([
	getopt_long
]))

AC_CONFIG_FILES([Makefile
				libcrypto/Makefile
				libcrypto/tests/Makefile
				liblzp/Makefile
				liblzp/tests/Makefile
				libfs/Makefile
				libfs/tests/Makefile
				tools/Makefile
				tools/tests/Makefile
				linux/Makefile
				fuse/Makefile])

AC_OUTPUT
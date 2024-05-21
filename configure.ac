AC_INIT([StupidFS], [0.0])

AM_INIT_AUTOMAKE([foreign subdir-objects])

AC_CONFIG_MACRO_DIRS([m4])

AC_CANONICAL_HOST

AC_LANG(C)
AC_PROG_CC
AC_PROG_CPP
AM_PROG_AR
AC_PROG_INSTALL
AC_PROG_RANLIB
AM_PROG_CC_C_O

AS_IF([ test "x$host_os" != "xmingw32" ], [
	PKG_CHECK_MODULES([FUSE], [fuse3])
])

AM_CONDITIONAL(BUILD_FUSE, test "x$host_os" != "xmingw32")
AM_CONDITIONAL(BUILD_LINUX, test "x$host_os" = "xlinux")


AC_CHECK_INCLUDES_DEFAULT
AC_C_CONST
AC_C_INLINE

AC_CONFIG_FILES([Makefile
				lib/Makefile
				linux/Makefile
				fuse/Makefile])

AC_OUTPUT
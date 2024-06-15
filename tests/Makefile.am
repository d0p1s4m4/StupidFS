AM_CPPFLAGS = -I$(top_srcdir)/lib

TESTS = test_lzp test_base64
check_PROGRAMS = test_lzp test_base64

LDADD = -lcmocka ../lib/libstpdfs.a

test_lzp_SOURCES = test_lzp.c

test_base64_SOURCES = test_base64.c

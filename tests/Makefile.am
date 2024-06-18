AM_CPPFLAGS = -I$(top_srcdir)/lib

TESTS = test_lzp test_hchacha test_xchacha
check_PROGRAMS = test_lzp test_hchacha test_xchacha

LDADD = -lcmocka ../lib/libstpdfs.a

test_lzp_SOURCES = test_lzp.c


test_hchacha_SOURCES = test_hchacha.c

test_xchacha_SOURCES = test_xchacha.c
AM_CPPFLAGS = -I$(top_srcdir)/lib

TESTS = test_lzp test_base64 test_hchacha
check_PROGRAMS = test_lzp test_base64 test_hchacha

LDADD = -lcmocka ../lib/libstpdfs.a

test_lzp_SOURCES = test_lzp.c

test_base64_SOURCES = test_base64.c

test_hchacha_SOURCES = test_hchacha.c

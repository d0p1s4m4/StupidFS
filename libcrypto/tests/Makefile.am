TESTS = test_hchacha test_xchacha
check_PROGRAMS = test_hchacha test_xchacha

AM_CFLAGS = -I../
LDADD = -lcmocka ../libcrypto.a

test_hchacha_SOURCES = test_hchacha.c

test_xchacha_SOURCES = test_xchacha.c
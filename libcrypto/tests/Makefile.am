TESTS = test_hchacha test_xchacha test_random
check_PROGRAMS = test_hchacha test_xchacha test_random

AM_CFLAGS = -I../
LDADD = -lcmocka ../libcrypto.a

test_hchacha_SOURCES = test_hchacha.c

test_xchacha_SOURCES = test_xchacha.c

test_random_SOURCES = test_random.c
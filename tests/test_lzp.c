#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

static const char *uncompress_data =  "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
									 "abcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabc"
									 "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccabcabcabcabcabcabcabcabcabcabcabcabcabcabc";

static void
test_lzp(void **state)
{
	(void)state;
}

int
main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_lzp),
	};
 
	return cmocka_run_group_tests(tests, NULL, NULL);
}
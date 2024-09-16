#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

int64_t fs_str2offset(const char *str);

static void
test_default(void **state)
{
	assert_int_equal(0, fs_str2offset(NULL));
	assert_int_equal(512, fs_str2offset("1"));
}

static void
test_valid_units(void **state)
{
	assert_int_equal(1024, fs_str2offset("2S"));
	assert_int_equal(2000, fs_str2offset("2K"));
	assert_int_equal(4000000, fs_str2offset("4M"));
	assert_int_equal(8000000000, fs_str2offset("8G"));
	assert_int_equal(3000000000000, fs_str2offset("3T"));
}

static void
test_invalid_units(void **state)
{
	assert_int_equal(2048, fs_str2offset("4D"));
	assert_int_equal(5120, fs_str2offset("10H"));
}

int
main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_default),
		cmocka_unit_test(test_valid_units),
		cmocka_unit_test(test_invalid_units),
	};
 
	return cmocka_run_group_tests(tests, NULL, NULL);
}
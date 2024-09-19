#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "random.h"

/* test from https://datatracker.ietf.org/doc/html/draft-irtf-cfrg-xchacha-03#section-2.2  */


static void
test_random(void **state)
{
	uint8_t buff[512];

	assert_return_code(crypto_random(buff, 512), 0);
}

int
main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_random),
	};
 
	return cmocka_run_group_tests(tests, NULL, NULL);
}
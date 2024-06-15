#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>

const char *simple_enc = "SGkh";
const char *simple_dec = "Hi!";

const char *with_pad_enc = "bGlnaHQgdw==";
const char *with_pad_dec = "light w";

size_t base64_encode(char *dest, const char *src, size_t srclen);

static void
test_encode(void **state)
{
	size_t len;
	char enc[14];

	len = base64_encode(enc, simple_dec, strlen(simple_dec));

	assert_int_equal(strlen(simple_enc), len);
	assert_string_equal(simple_enc, enc);
}

static void
test_encode_with_padding(void **state)
{
	size_t len;
	char enc[14];

	len = base64_encode(enc, with_pad_dec, strlen(with_pad_dec));

	assert_int_equal(strlen(with_pad_enc), len);
	assert_string_equal(with_pad_enc, enc);
}


static void
test_decode(void **state)
{
	(void)state;
}

int
main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_encode),
		cmocka_unit_test(test_encode_with_padding),
		cmocka_unit_test(test_decode),
	};
 
	return cmocka_run_group_tests(tests, NULL, NULL);
}
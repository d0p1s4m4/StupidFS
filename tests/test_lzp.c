#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

void lzp_compress(uint8_t *out, size_t *outsz, const uint8_t *in, size_t insz);
void lzp_decompress(uint8_t *out, size_t *outsz, const uint8_t *in, size_t insz);

static const char uncompress_data[298] =  "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
									 "abcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabc"
									 "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccabcabcabcabcabcabcabcabcabcabcabcabcabcabc";
static void
test_lzp(void **state)
{
	uint8_t compressed[298];
	char result[298];
	size_t outsz;

	lzp_compress(compressed, &outsz, (uint8_t *)uncompress_data, 298);
	lzp_decompress((uint8_t *)result, &outsz, compressed, outsz);

	assert_int_equal(298, outsz);
	assert_memory_equal(uncompress_data, result, 298);
}

int
main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_lzp),
	};
 
	return cmocka_run_group_tests(tests, NULL, NULL);
}
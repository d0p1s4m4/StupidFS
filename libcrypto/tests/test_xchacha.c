#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>

#include "chacha.h"

void xchacha(uint8_t *out, uint8_t key[CHACHA_KEY_BYTES], uint8_t nonce[24],
		uint8_t *in, size_t inlen, int round);
void chacha_init(struct chacha_ctx *ctx, const uint8_t key[CHACHA_KEY_BYTES],
			const uint32_t ctr, const uint8_t nonce[CHACHA_NONCE_BYTES]);
void chacha_block(struct chacha_ctx *ctx, uint8_t *out, int round);
void xchacha12(uint8_t *out, uint8_t key[CHACHA_KEY_BYTES], uint8_t nonce[24],
		uint8_t *in, size_t inlen);

uint8_t key[CHACHA_KEY_BYTES] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
	0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
	0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
};

uint8_t nonce[CHACHA_NONCE_BYTES] = {
	0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x4a, 0x00, 0x00, 0x00, 0x00,
};

uint8_t nonce2[24] = {
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
	0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x58
};

uint8_t key2[CHACHA_KEY_BYTES] = {
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
	0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f
};

static void
test_chacha_key_init(void **state)
{
	struct chacha_ctx ctx;
	uint32_t expected[] = {
		0x61707865, 0x3320646e, 0x79622d32, 0x6b206574,
		0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c,
		0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c,
		0x00000001, 0x09000000, 0x4a000000, 0x00000000
	};

	chacha_init(&ctx, key, 1, nonce);

	assert_memory_equal(expected, ctx.state, 64);
}

static void
test_chacha20_block(void **state)
{
	struct chacha_ctx ctx;
	uint8_t block[CHACHA_BLOCK_BYTES];
	uint32_t expected[] = {
		0xe4e7f110, 0x15593bd1, 0x1fdd0f50, 0xc47120a3,
		0xc7f4d1c7, 0x0368c033, 0x9aaa2204, 0x4e6cd4c3,
		0x466482d2, 0x09aa9f07, 0x05d7c214, 0xa2028bd9,
		0xd19c12b5, 0xb94e16de, 0xe883d0cb, 0x4e3c50a2
	};

	chacha_init(&ctx, key, 1, nonce);
	chacha_block(&ctx, block, 20);

	assert_memory_equal(expected, block, CHACHA_BLOCK_BYTES);
}

static void
test_xchacha12_enc_dec(void **state)
{
	const char *expected = "Trans Rights are Human Rights";
	uint8_t enc[30];
	uint8_t dec[30];

	xchacha12(enc, key2, nonce2, (uint8_t *)expected, 30);
	xchacha12(dec, key2, nonce2, enc, 30);

	assert_string_equal(expected, dec);
}

static void
test_xchacha20(void **state)
{
	uint8_t in[304] = {
		0x54, 0x68, 0x65, 0x20, 0x64, 0x68, 0x6f, 0x6c,
		0x65, 0x20, 0x28, 0x70, 0x72, 0x6f, 0x6e, 0x6f,
		0x75, 0x6e, 0x63, 0x65, 0x64, 0x20, 0x22, 0x64,
		0x6f, 0x6c, 0x65, 0x22, 0x29, 0x20, 0x69, 0x73,
		0x20, 0x61, 0x6c, 0x73, 0x6f, 0x20, 0x6b, 0x6e,
		0x6f, 0x77, 0x6e, 0x20, 0x61, 0x73, 0x20, 0x74,
		0x68, 0x65, 0x20, 0x41, 0x73, 0x69, 0x61, 0x74,
		0x69, 0x63, 0x20, 0x77, 0x69, 0x6c, 0x64, 0x20,
		0x64, 0x6f, 0x67, 0x2c, 0x20, 0x72, 0x65, 0x64,
		0x20, 0x64, 0x6f, 0x67, 0x2c, 0x20, 0x61, 0x6e,
		0x64, 0x20, 0x77, 0x68, 0x69, 0x73, 0x74, 0x6c,
		0x69, 0x6e, 0x67, 0x20, 0x64, 0x6f, 0x67, 0x2e,
		0x20, 0x49, 0x74, 0x20, 0x69, 0x73, 0x20, 0x61,
		0x62, 0x6f, 0x75, 0x74, 0x20, 0x74, 0x68, 0x65,
		0x20, 0x73, 0x69, 0x7a, 0x65, 0x20, 0x6f, 0x66,
		0x20, 0x61, 0x20, 0x47, 0x65, 0x72, 0x6d, 0x61,
		0x6e, 0x20, 0x73, 0x68, 0x65, 0x70, 0x68, 0x65,
		0x72, 0x64, 0x20, 0x62, 0x75, 0x74, 0x20, 0x6c,
		0x6f, 0x6f, 0x6b, 0x73, 0x20, 0x6d, 0x6f, 0x72,
		0x65, 0x20, 0x6c, 0x69, 0x6b, 0x65, 0x20, 0x61,
		0x20, 0x6c, 0x6f, 0x6e, 0x67, 0x2d, 0x6c, 0x65,
		0x67, 0x67, 0x65, 0x64, 0x20, 0x66, 0x6f, 0x78,
		0x2e, 0x20, 0x54, 0x68, 0x69, 0x73, 0x20, 0x68,
		0x69, 0x67, 0x68, 0x6c, 0x79, 0x20, 0x65, 0x6c,
		0x75, 0x73, 0x69, 0x76, 0x65, 0x20, 0x61, 0x6e,
		0x64, 0x20, 0x73, 0x6b, 0x69, 0x6c, 0x6c, 0x65,
		0x64, 0x20, 0x6a, 0x75, 0x6d, 0x70, 0x65, 0x72,
		0x20, 0x69, 0x73, 0x20, 0x63, 0x6c, 0x61, 0x73,
		0x73, 0x69, 0x66, 0x69, 0x65, 0x64, 0x20, 0x77,
		0x69, 0x74, 0x68, 0x20, 0x77, 0x6f, 0x6c, 0x76,
		0x65, 0x73, 0x2c, 0x20, 0x63, 0x6f, 0x79, 0x6f,
		0x74, 0x65, 0x73, 0x2c, 0x20, 0x6a, 0x61, 0x63,
		0x6b, 0x61, 0x6c, 0x73, 0x2c, 0x20, 0x61, 0x6e,
		0x64, 0x20, 0x66, 0x6f, 0x78, 0x65, 0x73, 0x20,
		0x69, 0x6e, 0x20, 0x74, 0x68, 0x65, 0x20, 0x74,
		0x61, 0x78, 0x6f, 0x6e, 0x6f, 0x6d, 0x69, 0x63,
		0x20, 0x66, 0x61, 0x6d, 0x69, 0x6c, 0x79, 0x20,
		0x43, 0x61, 0x6e, 0x69, 0x64, 0x61, 0x65, 0x2e
	};
	uint8_t expected[304] = {
		0x7d, 0x0a, 0x2e, 0x6b, 0x7f, 0x7c, 0x65, 0xa2,
		0x36, 0x54, 0x26, 0x30, 0x29, 0x4e, 0x06, 0x3b,
		0x7a, 0xb9, 0xb5, 0x55, 0xa5, 0xd5, 0x14, 0x9a,
		0xa2, 0x1e, 0x4a, 0xe1, 0xe4, 0xfb, 0xce, 0x87,
		0xec, 0xc8, 0xe0, 0x8a, 0x8b, 0x5e, 0x35, 0x0a,
		0xbe, 0x62, 0x2b, 0x2f, 0xfa, 0x61, 0x7b, 0x20,
		0x2c, 0xfa, 0xd7, 0x20, 0x32, 0xa3, 0x03, 0x7e,
		0x76, 0xff, 0xdc, 0xdc, 0x43, 0x76, 0xee, 0x05,
		0x3a, 0x19, 0x0d, 0x7e, 0x46, 0xca, 0x1d, 0xe0,
		0x41, 0x44, 0x85, 0x03, 0x81, 0xb9, 0xcb, 0x29,
		0xf0, 0x51, 0x91, 0x53, 0x86, 0xb8, 0xa7, 0x10,
		0xb8, 0xac, 0x4d, 0x02, 0x7b, 0x8b, 0x05, 0x0f,
		0x7c, 0xba, 0x58, 0x54, 0xe0, 0x28, 0xd5, 0x64,
		0xe4, 0x53, 0xb8, 0xa9, 0x68, 0x82, 0x41, 0x73,
		0xfc, 0x16, 0x48, 0x8b, 0x89, 0x70, 0xca, 0xc8,
		0x28, 0xf1, 0x1a, 0xe5, 0x3c, 0xab, 0xd2, 0x01,
		0x12, 0xf8, 0x71, 0x07, 0xdf, 0x24, 0xee, 0x61,
		0x83, 0xd2, 0x27, 0x4f, 0xe4, 0xc8, 0xb1, 0x48,
		0x55, 0x34, 0xef, 0x2c, 0x5f, 0xbc, 0x1e, 0xc2,
		0x4b, 0xfc, 0x36, 0x63, 0xef, 0xaa, 0x08, 0xbc,
		0x04, 0x7d, 0x29, 0xd2, 0x50, 0x43, 0x53, 0x2d,
		0xb8, 0x39, 0x1a, 0x8a, 0x3d, 0x77, 0x6b, 0xf4,
		0x37, 0x2a, 0x69, 0x55, 0x82, 0x7c, 0xcb, 0x0c,
		0xdd, 0x4a, 0xf4, 0x03, 0xa7, 0xce, 0x4c, 0x63,
		0xd5, 0x95, 0xc7, 0x5a, 0x43, 0xe0, 0x45, 0xf0,
		0xcc, 0xe1, 0xf2, 0x9c, 0x8b, 0x93, 0xbd, 0x65,
		0xaf, 0xc5, 0x97, 0x49, 0x22, 0xf2, 0x14, 0xa4,
		0x0b, 0x7c, 0x40, 0x2c, 0xdb, 0x91, 0xae, 0x73,
		0xc0, 0xb6, 0x36, 0x15, 0xcd, 0xad, 0x04, 0x80,
		0x68, 0x0f, 0x16, 0x51, 0x5a, 0x7a, 0xce, 0x9d,
		0x39, 0x23, 0x64, 0x64, 0x32, 0x8a, 0x37, 0x74,
		0x3f, 0xfc, 0x28, 0xf4, 0xdd, 0xb3, 0x24, 0xf4,
		0xd0, 0xf5, 0xbb, 0xdc, 0x27, 0x0c, 0x65, 0xb1,
		0x74, 0x9a, 0x6e, 0xff, 0xf1, 0xfb, 0xaa, 0x09,
		0x53, 0x61, 0x75, 0xcc, 0xd2, 0x9f, 0xb9, 0xe6,
		0x05, 0x7b, 0x30, 0x73, 0x20, 0xd3, 0x16, 0x83,
		0x8a, 0x9c, 0x71, 0xf7, 0x0b, 0x5b, 0x59, 0x07,
		0xa6, 0x6f, 0x7e, 0xa4, 0x9a, 0xad, 0xc4, 0x09
	};
	uint8_t out[304];

	xchacha(out, key2, nonce2, in, 304, 20);

	assert_memory_equal(expected, out, 304);
}

int
main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_chacha_key_init),
		cmocka_unit_test(test_chacha20_block),
		cmocka_unit_test(test_xchacha12_enc_dec),
		cmocka_unit_test(test_xchacha20),
	};
 
	return cmocka_run_group_tests(tests, NULL, NULL);
}
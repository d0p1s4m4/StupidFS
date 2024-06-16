#ifndef STPDFS_CRYPTO_CHACHA_H
# define STPDFS_CRYPTO_CHACHA_H 1

# include <stdint.h>

# define CHACHA_CONST1 "expa"
# define CHACHA_CONST2 "nd 3"
# define CHACHA_CONST3 "2-by"
# define CHACHA_CONST4 "te k"

#define CHACHA_ROT_L(x, n) (((x) << (n))|((x) >> (32 - (n))))
#define CHACHA_QUARTERROUND(a, b, c, d) do { \
			a += b; d ^= a; d = CHACHA_ROT_L(d, 16); \
			c += d; b ^= c; b = CHACHA_ROT_L(b, 12); \
			a += b; d ^= a; d = CHACHA_ROT_L(d, 8); \
			c += d; b ^= c; b = CHACHA_ROT_L(b, 7); \
		} while (0) 

# define HCHACHA_OUT_BYTES   32
# define CHACHA_KEY_BYTES    32
# define CHACHA_NONCE_BYTES  16
# define CHACHA_BLOCK_BYTES  64

# define TO_LE_32(x) ((x)[0]) | ((x)[1] << 8) | ((x)[2] << 16) | ((x)[3] << 24)

struct chacha_ctx {
	uint32_t state[4][4];
	uint32_t ctr;
};

void
hchacha(uint8_t out[HCHACHA_OUT_BYTES],
		const uint8_t key[CHACHA_KEY_BYTES],
		const uint8_t nonce[CHACHA_NONCE_BYTES], int round);

#endif /* STPDFS_CRYPTO_CHACHA_H */
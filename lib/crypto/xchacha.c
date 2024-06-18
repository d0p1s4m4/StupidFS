#ifdef _WIN32
# include <sys/param.h>
# if BYTE_ORDER == LITTLE_ENDIAN
#  define htole32(x) (x)
# else
#  define htole32(x) __builtin_bswap32(x)
# endif
#else
# include <endian.h>
#endif /* _WIN32 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "chacha.h"

void
chacha_init(struct chacha_ctx *ctx, const uint8_t key[CHACHA_KEY_BYTES],
			const uint32_t ctr, const uint8_t nonce[CHACHA_NONCE_BYTES])
{
	ctx->ctr = ctr;
	ctx->state[0][0] = TO_LE_32(CHACHA_CONST1);
	ctx->state[0][1] = TO_LE_32(CHACHA_CONST2);
	ctx->state[0][2] = TO_LE_32(CHACHA_CONST3);
	ctx->state[0][3] = TO_LE_32(CHACHA_CONST4);

	ctx->state[1][0] = TO_LE_32(key);
	ctx->state[1][1] = TO_LE_32(key + 4);
	ctx->state[1][2] = TO_LE_32(key + 4 * 2);
	ctx->state[1][3] = TO_LE_32(key + 4 * 3);

	ctx->state[2][0] = TO_LE_32(key + 16);
	ctx->state[2][1] = TO_LE_32(key + 16 + 4);
	ctx->state[2][2] = TO_LE_32(key + 16 + 4 * 2);
	ctx->state[2][3] = TO_LE_32(key + 16 + 4 * 3);

	ctx->state[3][0] = htole32(ctx->ctr);

	ctx->state[3][1] = TO_LE_32(nonce);
	ctx->state[3][2] = TO_LE_32(nonce + 4);
	ctx->state[3][3] = TO_LE_32(nonce + 4 * 2);
}

void
chacha_block(struct chacha_ctx *ctx, uint8_t *out, int round)
{
	int idx;
	uint32_t tmp[4][4];

	for (idx = 0; idx < 4; idx++)
	{
		tmp[idx][0] = ctx->state[idx][0];
		tmp[idx][1] = ctx->state[idx][1];
		tmp[idx][2] = ctx->state[idx][2];
		tmp[idx][3] = ctx->state[idx][3];
	}

	for (idx = 0; idx < round/2; idx++)
	{
		CHACHA_QUARTERROUND(tmp[0][0], tmp[1][0], tmp[2][0], tmp[3][0]);
		CHACHA_QUARTERROUND(tmp[0][1], tmp[1][1], tmp[2][1], tmp[3][1]);
		CHACHA_QUARTERROUND(tmp[0][2], tmp[1][2], tmp[2][2], tmp[3][2]);
		CHACHA_QUARTERROUND(tmp[0][3], tmp[1][3], tmp[2][3], tmp[3][3]);

		CHACHA_QUARTERROUND(tmp[0][0], tmp[1][1], tmp[2][2], tmp[3][3]);
		CHACHA_QUARTERROUND(tmp[0][1], tmp[1][2], tmp[2][3], tmp[3][0]);
		CHACHA_QUARTERROUND(tmp[0][2], tmp[1][3], tmp[2][0], tmp[3][1]);
		CHACHA_QUARTERROUND(tmp[0][3], tmp[1][0], tmp[2][1], tmp[3][2]);
	}

	*(uint32_t *)(out + 0) = htole32(tmp[0][0] + ctx->state[0][0]);
	*(uint32_t *)(out + 4) = htole32(tmp[0][1] + ctx->state[0][1]);
	*(uint32_t *)(out + 8) = htole32(tmp[0][2] + ctx->state[0][2]);
	*(uint32_t *)(out + 12) = htole32(tmp[0][3] + ctx->state[0][3]);

	*(uint32_t *)(out + 16) = htole32(tmp[1][0] + ctx->state[1][0]);
	*(uint32_t *)(out + 20) = htole32(tmp[1][1] + ctx->state[1][1]);
	*(uint32_t *)(out + 24) = htole32(tmp[1][2] + ctx->state[1][2]);
	*(uint32_t *)(out + 28) = htole32(tmp[1][3] + ctx->state[1][3]);

	*(uint32_t *)(out + 32) = htole32(tmp[2][0] + ctx->state[2][0]);
	*(uint32_t *)(out + 36) = htole32(tmp[2][1] + ctx->state[2][1]);
	*(uint32_t *)(out + 40) = htole32(tmp[2][2] + ctx->state[2][2]);
	*(uint32_t *)(out + 44) = htole32(tmp[2][3] + ctx->state[2][3]);

	*(uint32_t *)(out + 48) = htole32(tmp[3][0] + ctx->state[3][0]);
	*(uint32_t *)(out + 52) = htole32(tmp[3][1] + ctx->state[3][1]);
	*(uint32_t *)(out + 56) = htole32(tmp[3][2] + ctx->state[3][2]);
	*(uint32_t *)(out + 60) = htole32(tmp[3][3] + ctx->state[3][3]);

	ctx->ctr++;
	ctx->state[3][0] = htole32(ctx->ctr);
}

void
chacha_encrypt(struct chacha_ctx *ctx, uint8_t *out, uint8_t *in, size_t inlen, int round)
{
	size_t i;
	size_t j;
	uint8_t block[CHACHA_BLOCK_BYTES];

	chacha_block(ctx, block, round);
	for (i = 0, j = 0; i < inlen; i++, j++)
	{
		if (j >= CHACHA_BLOCK_BYTES)
		{
			j = 0;
			chacha_block(ctx, block, round);
		}

		out[i] = in[i] ^ block[j];
	}
}

void
xchacha(uint8_t *out, uint8_t key[CHACHA_KEY_BYTES], uint8_t nonce[24],
		uint8_t *in, size_t inlen, int round)
{
	uint8_t subkey[CHACHA_KEY_BYTES];
	uint8_t tmp[12];
	struct chacha_ctx ctx;

	hchacha(subkey, key, nonce, round);

	memset(tmp, 0, 12);
	memcpy(tmp + 4, nonce + 16, 8);

	chacha_init(&ctx, subkey, 1, tmp);

	chacha_encrypt(&ctx, out, in, inlen, round);
}

void
xchacha12(uint8_t *out, uint8_t key[CHACHA_KEY_BYTES], uint8_t nonce[24],
		uint8_t *in, size_t inlen)
{
	xchacha(out, key, nonce, in, inlen, 12);
}
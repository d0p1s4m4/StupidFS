#include <endian.h>
#include <stdint.h>
#include "chacha.h"

void
hchacha(uint8_t out[HCHACHA_OUT_BYTES],
		const uint8_t key[CHACHA_KEY_BYTES],
		const uint8_t nonce[CHACHA_NONCE_BYTES],
		int round)
{
	int idx;

	uint32_t state[4][4];
	
	state[0][0] = TO_LE_32(CHACHA_CONST1);
	state[0][1] = TO_LE_32(CHACHA_CONST2);
	state[0][2] = TO_LE_32(CHACHA_CONST3);
	state[0][3] = TO_LE_32(CHACHA_CONST4);

	state[1][0] = TO_LE_32(key);
	state[1][1] = TO_LE_32(key + 4);
	state[1][2] = TO_LE_32(key + 4 * 2);
	state[1][3] = TO_LE_32(key + 4 * 3);

	state[2][0] = TO_LE_32(key + 16);
	state[2][1] = TO_LE_32(key + 16 + 4);
	state[2][2] = TO_LE_32(key + 16 + 4 * 2);
	state[2][3] = TO_LE_32(key + 16 + 4 * 3);

	state[3][0] = TO_LE_32(nonce);
	state[3][1] = TO_LE_32(nonce + 4);
	state[3][2] = TO_LE_32(nonce + 8);
	state[3][3] = TO_LE_32(nonce + 12);

	for (idx = 0; idx < round/2; idx++)
	{
		CHACHA_QUARTERROUND(state[0][0], state[1][0], state[2][0], state[3][0]);
		CHACHA_QUARTERROUND(state[0][1], state[1][1], state[2][1], state[3][1]);
		CHACHA_QUARTERROUND(state[0][2], state[1][2], state[2][2], state[3][2]);
		CHACHA_QUARTERROUND(state[0][3], state[1][3], state[2][3], state[3][3]);

		CHACHA_QUARTERROUND(state[0][0], state[1][1], state[2][2], state[3][3]);
		CHACHA_QUARTERROUND(state[0][1], state[1][2], state[2][3], state[3][0]);
		CHACHA_QUARTERROUND(state[0][2], state[1][3], state[2][0], state[3][1]);
		CHACHA_QUARTERROUND(state[0][3], state[1][0], state[2][1], state[3][2]);
	}

	*(uint32_t *)(out + 0) = htole32(state[0][0]);
	*(uint32_t *)(out + 4) = htole32(state[0][1]);
	*(uint32_t *)(out + 8) = htole32(state[0][2]);
	*(uint32_t *)(out + 12) = htole32(state[0][3]);

	*(uint32_t *)(out + 16) = htole32(state[3][0]);
	*(uint32_t *)(out + 20) = htole32(state[3][1]);
	*(uint32_t *)(out + 24) = htole32(state[3][2]);
	*(uint32_t *)(out + 28) = htole32(state[3][3]);
}

void
hchacha12(uint8_t out[HCHACHA_OUT_BYTES],
		const uint8_t key[CHACHA_KEY_BYTES],
		const uint8_t nonce[CHACHA_NONCE_BYTES])
{
	return (hchacha(out, key, nonce, 12));
}
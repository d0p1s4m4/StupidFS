#include <stdint.h>
#include <string.h>

#define LZP_HASH_ORDER 16
#define LZP_HASH_SIZE  (1 << LZP_HASH_ORDER)

#define HASH(h, x) ((h << 4) ^ x)

void
lzp_compress(uint8_t *out, size_t *outsz, const uint8_t *in, size_t insz)
{
	uint8_t c;
	uint16_t hash = 0;
	uint32_t mask = 0;
	uint8_t table[LZP_HASH_SIZE] = { 0 };
	size_t i, j;
	size_t inpos;
	size_t outpos;
	uint8_t buff[9];

	inpos = 0;
	outpos = 0;
	while (inpos < insz)
	{
		j = 1;
		for (i = 0; i < 8; i++)
		{
			if (inpos >= insz)
			{
				break;
			}

			c = in[inpos++];

			if (table[hash] == c)
			{
				mask |= 1 << i;
			}
			else
			{
				table[hash] = c;
				buff[j++] = c;
			}

			hash = HASH(hash, c);
		}

		if (i > 0)
		{
			buff[0] =  mask;
			if (out != NULL)
			{
				memcpy(out + outpos, buff, j);
			}
			outpos += j;
		}
	}

	*outsz = outpos;
}

void
lzp_decompress(uint8_t *out, size_t *outsz, const uint8_t *in, size_t insz)
{
	uint8_t c;
	uint16_t hash = 0;
	uint32_t mask = 0;
	uint8_t table[LZP_HASH_SIZE] = { 0 };
	size_t i, j;
	size_t inpos;
	size_t outpos;
	uint8_t buff[9];

	inpos = 0;
	outpos = 0;
	while (inpos < insz)
	{
		j = 0;
		if (inpos >= insz)
		{
			break;
		}

		mask = in[inpos++];

		for (i = 0; i < 8; i++)
		{
			if ((mask & (1 << i)) != 0)
			{
				c = table[hash];
			}
			else
			{
				if (inpos >= insz)
				{
					break;
				}
				c = in[inpos++];
				table[hash] = c;
			}
			buff[j++] = c;
			hash = HASH(hash, c);
		}

		if (j > 0)
		{
			if (out != NULL)
			{
				memcpy(out + outpos, buff, j);
			}
			outpos += j;
		}
	}

	*outsz = outpos;
}
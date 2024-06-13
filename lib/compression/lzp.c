/*
        ; hash(h, x) (h = (h << 4) ^ x)

        section '.code' code

        ; xor hash, hash
        ; xor mask, mask
        ; j = 1
        ; while (insz > 0)
        ; {
        ;    if (ecx == 8)
        ;    {
        ;       mov [out], mask
        ;       xor ecx, ecx
        ;       xor mask, mask
        ;       j = 1;
        ;    }
        ;    c = in[inpos++]
        ;    if (c == table[hash])
        ;    {
        ;        mask |= 1 << ecx
        ;    }
        ;    else
        ;    {
        ;        table[hash] = c
        ;        out[j] = c;
        ;    }
        ;    HASH(hash, c)
        ;    ecx++;
        ; }
*/
#include <stdint.h>

#define LZP_HASH_ORDER 16
#define LZP_HASH_SIZE  (1 << LZP_HASH_ORDER)

#define HASH(h, x) ((h << 4) ^ x)

void
lzp_compress()
{
	uint32_t hash = 0;
	uint32_t mask = 0;
	char c;
	uint8_t table[LZP_HASH_SIZE] = { 0 };

	while (1)
	{
		hash = HASH(hash, c);
	}
}

void
lzp_decompress()
{
	
}
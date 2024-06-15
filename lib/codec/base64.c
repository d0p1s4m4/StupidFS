#include <stddef.h>
#include <stdint.h>

static const char ALPHABET[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

size_t
base64_encode(char *dest, const char *src, size_t srclen)
{
	size_t encodedlen;
	size_t i, j;
	uint8_t a, b, c;
	uint32_t triple;

	encodedlen = 4 * ((srclen + 2) / 3);

	if (dest == NULL)
	{
		return (encodedlen);
	}

	for (i = 0, j = 0; i < srclen;)
	{
		a = (i < srclen ? src[i++] : 0);
		b = (i < srclen ? src[i++] : 0);
		c = (i < srclen ? src[i++] : 0);

		triple = (a << 0x10) | (b << 0x08) | c;

		dest[j++] = ALPHABET[(triple >> 3 * 6) & 0x3F];
		dest[j++] = ALPHABET[(triple >> 2 * 6) & 0x3F];
		dest[j++] = ALPHABET[(triple >> 1 * 6) & 0x3F];
		dest[j++] = ALPHABET[(triple >> 0 * 6) & 0x3F];
	}
	if (srclen % 3 != 0)
	{
		dest[encodedlen - 1] = '=';
		if (srclen % 3 == 1)
		{
			dest[encodedlen - 2] = '='; 
		}
	}
	dest[encodedlen] = 0;
	return (encodedlen);
}
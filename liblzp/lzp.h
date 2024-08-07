/**
 * \file lzp.h
 */
#ifndef LZP_H
# define LZP_H 1

# include <stddef.h>
# include <stdint.h>

void lzp_compress(uint8_t *out, size_t *outsz, const uint8_t *in, size_t insz);
void lzp_decompress(uint8_t *out, size_t *outsz, const uint8_t *in, size_t insz);

#endif /* LZP_H */
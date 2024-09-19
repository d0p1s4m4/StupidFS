#ifndef CRYPTO_RANDOM_H
# define CRYPTO_RANDOM_H 1

# include <stdint.h>

int crypto_random(uint8_t *buff, size_t size);

#endif /* CRYPTO_RANDOM_H */
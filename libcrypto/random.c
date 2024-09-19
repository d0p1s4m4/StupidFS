#include "random.h"
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */
#ifdef _WIN32
# include <windows.h>
# include <ntsecapi.h>
#else
# ifdef HAVE_SYS_RANDOM_H
#  include <sys/random.h>
# else
#  include <stdio.h>
# endif /* HAVE_SYS_RANDOM_H */
#endif /* _WIN32 */

int
crypto_random(uint8_t *buff, size_t size)
{
#ifdef _WIN32
	return (RtlGenRandom(buff, size));
#else
# ifdef HAVE_GETRANDOM
	ssize_t ret;

	ret = getrandom(buff, size, 0)
	if (ret < size)
	{
		return (-1);
	}
# else
	FILE *fp;
	size_t total;
	size_t byte_read;

	fp = fopen("/dev/urandom", "rb");
	if (fp == NULL)
	{
		return (-1);
	}

	total = 0;
	do {
		byte_read = fread(buff + total, 1, size - total, fp);
		if (byte_read == 0)
		{
			fclose(fp);
			return (-1);
		}

		total += byte_read;
	} while (total < size);

	fclose(fp);
# endif /* HAVE_GETRANDOM */
#endif /* _WIN32 */
	return (0);
}
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */
#include <stdio.h>

/*
 * S = sectors (512)
 * K = kilobytes
 * M = Megabytes
 * G = Gigabytes
 * T = Terabytes
*/
static const struct {
	char unit;
	uint64_t mult;
} units[] = {
	{'S', 512},
	{'K', 1000},
	{'M', 1000000},
	{'G', 1000000000},
	{'T', 1000000000000},
	{0, 0}
};

uint64_t fs_str2offset(const char *str)
{
	char u;
	uint64_t mult;
	uint64_t off;
	int idx;

	if (str == NULL)
	{
		return (0);
	}

	/* default to sectors */
	mult = units[0].mult;

	u = str[strlen(str) - 1];
	for (idx = 0; units[idx].unit != 0; idx++)
	{
		if (u == units[idx].unit)
		{
			mult = units[idx].mult;
			break;
		}
	}

#ifdef HAVE_STRTOULL
	off = strtoull(str, NULL, 0);
#else
	off = strtoul(str, NULL, 0);
#endif /* HAVE_STRTOULL */

	return (off * mult);
}
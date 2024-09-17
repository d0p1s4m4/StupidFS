#include "colors.h"
#include <stdlib.h>
#include <string.h>

char *ls_colors[] = {
	/* C_NORM    */ "0",
	/* C_FILE    */ "0",
	/* C_DIR     */ "34",
	/* C_LINK    */ "35",
	/* C_SETUID  */ "30;41",
	/* C_SETGID  */ "30;46",
	/* C_STICKY_OTH_WRITABLE */ "30;42",
	/* C_OTHER_WRITABLE */ "30;43",
	/* C_STICKY  */ "0",
	/* C_EXEC    */ "31",
	/* C_LEFT    */ "\033[",
	/* C_RIGHT   */ "m",
	/* C_END     */ "0"
};

static const struct {
	char *abrv;
	enum COLORS type;
} abrv2type[] = {
	{"no=", C_NORM},
	{"fi=", C_FILE},
	{"di=", C_DIR},
	{"ln=", C_LINK},
	{"su=", C_SETUID},
	{"sg=", C_SETGID},
	{"tw=", C_STICKY_OTH_WRITABLE},
	{"ow=", C_OTHER_WRITABLE},
	{"st=", C_STICKY},
	{"ex=", C_EXEC},
	{"lc=", C_LEFT},
	{"rc=", C_RIGHT},
	{"ec=", C_END},
	{NULL, 0}
};

void
ls_parse_colors(void)
{
	char *colors;
	char *line;
	int idx;

	colors = getenv("LS_COLORS");
	if (colors == NULL) return;

	line = strtok(colors, ":");
	while (line != NULL)
	{
		for (idx = 0; abrv2type[idx].abrv != NULL; idx++)
		{
			if (strncmp(abrv2type[idx].abrv, line, 3) == 0)
			{
				ls_colors[abrv2type[idx].type] = line + 3;
				break;
			}
		}
		line = strtok(NULL, ":");
	}
}
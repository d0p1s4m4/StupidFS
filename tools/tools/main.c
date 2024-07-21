#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */
#ifdef HAVE_LIBGEN_H
# include <libgen.h>
#endif /* HAVE_LIBGEN_H */
#ifdef HAVE_GETOPT_H
# include <getopt.h>
#endif /* HAVE_GETOPT_H */
#include "tools.h"

struct command {
	const char *name;
	int (*cmd)(int argc, char **argv);
};

struct command commands[] = {
	{"ls", &ls},
	{"copy", &copy},
	{NULL, NULL}
};

static const char *prg_name;

static void
usage(int retval)
{
	int idx;

	if (retval == EXIT_FAILURE)
	{
		fprintf(stderr, "Try '%s -h' for more information.\n", prg_name);
	}
	else
	{
		printf("Usage: %s COMMAND [OPTIONS]\n\n", prg_name);
		printf("Commands:\n");
		printf("\t");
		for (idx = 0; commands[idx].name != NULL; idx++)
		{
			printf("%s ", commands[idx].name);
		}
		printf("\n");

		printf("For more details see tools.stpd(8).\n");
	}
	exit(retval);
}

static void
version(void)
{
	printf("%s (%s) %s\n", prg_name, PACKAGE_NAME, PACKAGE_VERSION);
	exit(EXIT_SUCCESS);
}
int
main(int argc, char **argv)
{
	int idx;

#ifdef HAVE_LIBGEN_H
	prg_name = basename(argv[0]);
#else
	prg_name = argv[0];
#endif /* HAVE_LIBGEN_H */

	if (argc < 2)
	{
		usage(EXIT_FAILURE);
	}

	if (argv[1][0] == '-')
	{
		switch(argv[1][1])
		{
			case 'h':
				usage(EXIT_SUCCESS);
				break;
			
			case 'V':
				version();
				break;

			default:
				usage(EXIT_FAILURE);
				break;
		}
	}

	for (idx = 0; commands[idx].name != NULL; idx++)
	{
		if (strcmp(argv[1], commands[idx].name) == 0)
		{
			return (commands[idx].cmd(argc-1, argv+1));
		}
	}

	return (EXIT_FAILURE);
}
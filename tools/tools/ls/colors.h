#ifndef TOOLS_LS_COLORS_H
# define TOOLS_LS_COLORS_H 1

enum COLORS {
	C_NORM,
	C_FILE,
	C_DIR,
	C_LINK,
	C_SETUID,
	C_SETGID,
	C_STICKY_OTH_WRITABLE,
	C_OTHER_WRITABLE,
	C_STICKY,
	C_EXEC,
	C_LEFT,
	C_RIGHT,
	C_END
};

extern char *ls_colors[];

void ls_parse_colors(void);

#endif /* !TOOLS_LS_COLORS_H */
#ifndef	__MY__LOGGER__
#define	__MY__LOGGER__

typedef enum exitcode{
	EXIT_FAIL = 1,
	EXIT_SUCC = 2
}EXTC_t;

typedef enum plevel{
	INFO = 0,
	WARN,
	ERR
}PLEVEL_t;

EXTC_t slog(PLEVEL_t, const char *);



#endif

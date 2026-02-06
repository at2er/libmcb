/* Public domain */
#ifndef LIBMCB_ERR_H
#define LIBMCB_ERR_H
#include <stdio.h>

#ifndef DISABLE_EPRINTF_COLOR
#define ERR_COLOR(S) "\x1b[31m" S "\x1b[0m"
#else
#define ERR_COLOR(S) S
#endif

#define ERR_FMT "[libmcb:%s:%s:" ERR_COLOR("error") "]: "
#define ERR_ARG __FILE__, __func__

#define eprint(MSG) \
	fprintf(stderr, ERR_FMT MSG "\n", ERR_ARG)
#define eprintf(MSG_FMT, ...) \
	fprintf(stderr, ERR_FMT MSG_FMT "\n", ERR_ARG, __VA_ARGS__)

#define ereturn(VALUE, MSG) \
	do { \
		eprint(MSG); \
		return VALUE; \
	} while (0);

#define ereturnf(VALUE, MSG_FMT, ...) \
	do { \
		eprintf(MSG_FMT __VA_ARGS__); \
		return VALUE; \
	} while (0);

#endif

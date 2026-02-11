/* Public domain */
#ifndef LIBMCB_ERR_H
#define LIBMCB_ERR_H
#include <stdio.h>
#include <stdlib.h>

#ifndef DISABLE_ERR_COLOR
#define ERR_COLOR(S) "\x1b[31m" S "\x1b[0m"
#else
#define ERR_COLOR(S) S
#endif

#define ERR_ARG __FILE__, __func__, __LINE__
#define ERR_FMT "[libmcb:%s:%s:%d: " ERR_COLOR("error") "]: "

#define eabort(MSG) \
	do { \
		eprint(MSG); \
		abort(); \
	} while (0);

#define eabortf(MSG_FMT, ...) \
	do { \
		eprintf(MSG_FMT, __VA_ARGS__); \
		abort(); \
	} while (0);

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

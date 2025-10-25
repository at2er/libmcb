/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_STR_H
#define LIBMCB_STR_H
#include <stddef.h>

#define MCB_DEF_STR_IN_STACK(NAME, BUF_NAME, LEN) \
	char BUF_NAME[LEN]; \
	struct mcb_str NAME = {.s = BUF_NAME, .len = LEN}

struct mcb_str {
	char *s;
	size_t len;
};

struct mcb_str_const {
	const char *s;
	size_t len;
};

int mcb_str_init(size_t len);

#endif

/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_CONTEXT_H
#define LIBMCB_CONTEXT_H
#include <stddef.h>
#include "mcb/func.h"

struct mcb_context {
	struct mcb_func **fn_arr;
	size_t fn_arr_count;
};

int mcb_define_context(struct mcb_context *ctx);
void mcb_destory_context(struct mcb_context *ctx);

#endif

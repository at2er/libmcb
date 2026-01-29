/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "mcb/context.h"

int mcb_define_context(struct mcb_context *ctx)
{
	assert(ctx);
	memset(ctx, 0, sizeof(*ctx));
	return 0;
}

void mcb_destory_context(struct mcb_context *ctx)
{
	for (size_t i = 0; i < ctx->fn_arr_count; i++)
		mcb_destory_func(ctx->fn_arr[i]);
	free(ctx->fn_arr);
}

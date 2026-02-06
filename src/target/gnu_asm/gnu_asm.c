/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mcb/func.h"
#include "mcb/inst.h"
#include "mcb/target/gnu_asm.h"
#include "mcb/value.h"

#define LIBMCB_STRIP
#include "func.h"
#include "gnu_asm.h"
#include "reg.h"
#include "value.h"

#include "../../str.h"
#include "../../macros.h"

int
mcb_target_gnu_asm(FILE *stream, struct mcb_context *ctx)
{
	struct gnu_asm gnu_asm_ctx = {0};
	int ret = 0;

	if (!stream || !ctx)
		return 1;

	if (!str_from_cstr(&gnu_asm_ctx.text, ".text\n"))
		return 1;
	if (!str_empty(&gnu_asm_ctx.buf))
		return 1;
	if (!str_expand_siz(&gnu_asm_ctx.buf, 8192))
		return 1;
	gnu_asm_ctx.ctx = ctx;
	gnu_asm_ctx.stream = stream;

	for (size_t i = 0; i < ctx->fn_arr_count; i++) {
		ret = define_func(ctx->fn_arr[i], &gnu_asm_ctx);
		if (ret)
			return 1;
	}

	fputs(gnu_asm_ctx.text.s, stream);

	return 0;
}

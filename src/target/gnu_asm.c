/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <string.h>
#include "mcb/target/gnu_asm.h"
#include "../str.h"

struct gnu_asm {
	struct mcb_context *ctx;
	struct str output;
	FILE *stream;
};

static int def_fn(struct mcb_func *fn, struct gnu_asm *ctx);

int def_fn(struct mcb_func *fn, struct gnu_asm *ctx)
{
	const char *globl_temp = ".globl %s\n";

	struct str output;
	assert(fn && ctx);

	if (!str_empty(&output))
		return 1;

	if (fn->export_type == MCB_FUNC_EXPORT) {
		if (!str_expand_siz(&output,
					strlen(globl_temp) - 2 +
					strlen(fn->name)))
			return 1;
		output.len += snprintf(output.s, output.siz,
				globl_temp,
				fn->name);
	}

	if (!str_expand_siz(&output, strlen(fn->name) + 2))
		return 1;
	output.len += sprintf(&output.s[strlen(output.s)], "%s:\n", fn->name);

	if (!str_append_str(&ctx->output, &output))
		return 1;
	str_free(&output);

	return 0;
}

int mcb_target_gnu_asm(FILE *stream, struct mcb_context *ctx)
{
	struct gnu_asm gnu_asm_ctx = {0};
	int ret = 0;

	if (!stream || !ctx)
		return 1;

	if (!str_from_cstr(&gnu_asm_ctx.output, ".text\n"))
		return 1;
	gnu_asm_ctx.ctx = ctx;
	gnu_asm_ctx.stream = stream;

	for (size_t i = 0; i < ctx->fn_arr_count; i++) {
		ret = def_fn(ctx->fn_arr[i], &gnu_asm_ctx);
		if (ret)
			return 1;
	}

	fputs(gnu_asm_ctx.output.s, stream);

	return 0;
}

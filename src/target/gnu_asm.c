/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <string.h>
#include "mcb/target/gnu_asm.h"
#include "../str.h"

struct gnu_asm {
	struct mcb_context *ctx;
	struct str buffer, buffer2;
	FILE *stream;
};

static int def_fn(struct mcb_func *fn, struct gnu_asm *ctx);
static int def_fn_begin(struct mcb_func *fn, struct gnu_asm *ctx);
static int def_label(struct mcb_label *label,
		struct mcb_func *fn,
		struct gnu_asm *ctx);
static int inst(struct mcb_inst *inst,
		struct mcb_func *fn,
		struct gnu_asm *ctx);

int def_fn(struct mcb_func *fn, struct gnu_asm *ctx)
{
	assert(fn && ctx);

	if (def_fn_begin(fn, ctx))
		return 1;
	for (size_t i = 0, label_i = 0; i < fn->inst_arr_count; i++) {
		if (fn->label_arr && label_i < fn->label_arr_count &&
				fn->label_arr[label_i]->beg ==
				fn->inst_arr[i]) {
			if (def_label(fn->label_arr[label_i], fn, ctx))
				return 1;
			label_i++;
		}

		if (inst(fn->inst_arr[i], fn, ctx))
			return 1;
	}
	return 0;
}

int def_fn_begin(struct mcb_func *fn, struct gnu_asm *ctx)
{
	assert(fn && ctx);

	if (!str_clean(&ctx->buffer2))
		return 1;

	if (fn->export_type == MCB_FUNC_EXPORT)
		ctx->buffer2.len = snprintf(
				ctx->buffer2.s,
				ctx->buffer2.siz,
				".globl %s\n", fn->name);

	ctx->buffer2.len += snprintf(
			&ctx->buffer2.s[ctx->buffer2.len],
			ctx->buffer2.siz - ctx->buffer2.len,
			"%s:\n", fn->name);

	if (!str_append_str(&ctx->buffer, &ctx->buffer2))
		return 1;

	return 0;
}

int def_label(struct mcb_label *label,
		struct mcb_func *fn,
		struct gnu_asm *ctx)
{
	assert(label && ctx);

	if (!str_clean(&ctx->buffer2))
		return 1;

	ctx->buffer2.len = snprintf(ctx->buffer2.s, ctx->buffer2.siz,
			".L%s__%s:\n",
			fn->name,
			label->name);

	if (!str_append_str(&ctx->buffer, &ctx->buffer2))
		return 1;

	return 0;
}

int inst(struct mcb_inst *inst,
		struct mcb_func *fn,
		struct gnu_asm *ctx)
{
	assert(inst && fn && ctx);


	return 0;
}

int mcb_target_gnu_asm(FILE *stream, struct mcb_context *ctx)
{
	struct gnu_asm gnu_asm_ctx = {0};
	int ret = 0;

	if (!stream || !ctx)
		return 1;

	if (!str_from_cstr(&gnu_asm_ctx.buffer, ".text\n"))
		return 1;
	if (!str_empty(&gnu_asm_ctx.buffer2))
		return 1;
	if (!str_expand_siz(&gnu_asm_ctx.buffer2, 8192))
		return 1;
	gnu_asm_ctx.ctx = ctx;
	gnu_asm_ctx.stream = stream;

	for (size_t i = 0; i < ctx->fn_arr_count; i++) {
		ret = def_fn(ctx->fn_arr[i], &gnu_asm_ctx);
		if (ret)
			return 1;
	}

	fputs(gnu_asm_ctx.buffer.s, stream);

	return 0;
}

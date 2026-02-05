/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "mcb/func.h"

#include "func.h"
#include "gnu_asm.h"
#include "inst.h"
#include "label.h"

static int define_func_begin(struct mcb_func *fn, struct gnu_asm *ctx);

int
define_func_begin(struct mcb_func *fn, struct gnu_asm *ctx)
{
	struct gnu_asm_func *f;
	assert(fn && ctx);

	f = calloc(1, sizeof(*f));
	if (!f)
		return 1;
	fn->data = f;

	if (!str_clean(&ctx->buf))
		return 1;

	if (fn->export_type == MCB_FUNC_EXPORT)
		ctx->buf.len = snprintf(
				ctx->buf.s,
				ctx->buf.siz,
				".globl %s\n", fn->name);

	ctx->buf.len += snprintf(
			&ctx->buf.s[ctx->buf.len],
			ctx->buf.siz - ctx->buf.len,
			"%s:\n", fn->name);

	if (!str_append_str(&ctx->text, &ctx->buf))
		return 1;

	return 0;
}

int
define_func(struct mcb_func *fn, struct gnu_asm *ctx)
{
	assert(fn && ctx);

	if (define_func_begin(fn, ctx))
		return 1;
	for (size_t i = 0, label_i = 0; i < fn->inst_arr_count; i++) {
		if (fn->label_arr && label_i < fn->label_arr_count &&
				fn->label_arr[label_i]->beg ==
				fn->inst_arr[i]) {
			if (define_label(fn->label_arr[label_i], fn, ctx))
				return 1;
			label_i++;
		}

		if (build_inst(fn->inst_arr[i], fn, ctx))
			return 1;
	}
	return 0;
}

/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "mcb/func.h"

#define LIBMCB_STRIP
#include "func.h"
#include "gnu_asm.h"
#include "inst.h"
#include "label.h"
#include "reg.h"
#include "value.h"
#include "value_kind.h"

#include "../../ealloc.h"
#include "../../err.h"
#include "../../macros.h"

static bool can_define_label(
		const struct mcb_func *fn,
		size_t label_idx,
		size_t inst_idx);
static int define_func_begin(struct mcb_func *fn, struct gnu_asm *ctx);
static void init_func_arg_value(int idx, struct mcb_func *fn);

static const enum GNU_ASM_REG arg_alloc_arr[] = {
	RDI, RSI, RDX, RCX, R8, R9
};

bool
can_define_label(
		const struct mcb_func *fn,
		size_t label_idx,
		size_t inst_idx)
{
	if (!fn->label_arr)
		return false;
	if (label_idx >= fn->label_arr_count)
		return false;
	if (fn->label_arr[label_idx]->beg == fn->inst_arr[inst_idx])
		return true;
	return false;
}

int
define_func_begin(struct mcb_func *fn, struct gnu_asm *ctx)
{
	struct gnu_asm_func *f;
	assert(fn && ctx);

	f = ecalloc(1, sizeof(*f));
	fn->data = f;

	estr_clean(&ctx->buf);

	if (fn->export_type == MCB_EXPORT_FUNC)
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

	/* alloc func->args[*]->val_link->data */
	if (fn->argc && !fn->args)
		eabort("fn->argc > 0 && fn->args == NULL");
	for (int i = 0; i < fn->argc; i++) {
		init_func_arg_value(i, fn);
	}

	return 0;
}

void
init_func_arg_value(int idx, struct mcb_func *fn)
{
	struct gnu_asm_value *gval;
	struct mcb_func_arg *arg;

	assert(fn && fn->args);
	arg = fn->args[idx];
	assert(arg);

	if (!arg->val_link)
		return;

	if (idx > (int)LENGTH(arg_alloc_arr))
		eabort("unsupport, idx > LENGTH(arg_alloc_arr)");

	gval = arg->val_link->data = ecalloc(1, sizeof(*gval));
	gval->kind = map_type_to_value_kind(I8_REG_VALUE, arg->type);
	gval->inner.reg = alloc_reg(arg_alloc_arr[idx], gval, fn);
	if (gval->inner.reg == REG_COUNT)
		eabort("alloc_reg()");
}

int
define_func(struct mcb_func *fn, struct gnu_asm *ctx)
{
	assert(fn && ctx);

	if (define_func_begin(fn, ctx))
		return 1;
	for (size_t i = 0, label_i = 0; i < fn->inst_arr_count; i++) {
		if (can_define_label(fn, label_i, i)) {
			define_label(fn->label_arr[label_i], fn, ctx);
			label_i++;
		}

		if (build_inst(fn->inst_arr[i], fn, ctx))
			return 1;
	}
	return 0;
}

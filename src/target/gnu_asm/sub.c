/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "mcb/func.h"
#include "mcb/inst.h"
#include "mcb/value.h"

#define LIBMCB_STRIP
#include "gen_mov.h"
#include "gnu_asm.h"
#include "inst.h"
#include "value.h"

#include "../../ealloc.h"
#include "../../err.h"
#include "../../str.h"

int
build_sub_inst(struct mcb_inst *inst_outer,
		struct mcb_func *fn,
		struct gnu_asm *ctx)
{
	struct str dst, src;
	struct mcb_sub_inst *inst;
	struct gnu_asm_value *lhs_val, *rhs_val, *result;

	assert(inst_outer && fn && ctx);
	inst = &inst_outer->inner.sub;
	assert(inst->result && inst->result->scope_end);
	if (inst->result->scope_end == inst_outer)
		return 0;

	assert(inst->lhs && inst->rhs);
	lhs_val = inst->lhs->data;
	rhs_val = inst->rhs->data;
	assert(lhs_val && rhs_val);

	assert(inst->result->data == NULL);
	result = ecalloc(1, sizeof(*result));
	result->kind = map_type_to_value_kind(
			I8_REG_VALUE,
			inst->result->type);
	result->inner.reg = alloc_reg(AUTO_ALLOC_REG, result, fn);
	if (result->inner.reg == REG_COUNT)
		eabort("alloc_reg()");
	inst->result->data = result;

	if (IS_REG(lhs_val->kind)) {
		if (inst->lhs->scope_end == inst_outer) {
			drop_reg(result->inner.reg, fn);
			free(result);
			result = lhs_val;
			inst->lhs->data = NULL;
			inst->result->data = result;
		}
		str_from_value(&dst, lhs_val);
	} else if (IS_IMM(lhs_val->kind)) {
		estr_clean(&ctx->buf);
		if (gen_mov(&ctx->buf, result, lhs_val))
			eabort("gen_mov()");
		estr_append_str(&ctx->text, &ctx->buf);
		str_from_value(&dst, result);
	}

	str_from_value(&src, rhs_val);

	estr_clean(&ctx->buf);
	ctx->buf.len = snprintf(ctx->buf.s, ctx->buf.siz,
			"sub%c %s, %s\n",
			get_inst_suffix(lhs_val->kind),
			src.s, dst.s);

	estr_append_str(&ctx->text, &ctx->buf);

	str_free(&dst);
	str_free(&src);

	if (inst->lhs->scope_end == inst_outer)
		drop_value(inst->lhs, fn);
	if (inst->rhs->scope_end == inst_outer)
		drop_value(inst->rhs, fn);

	return 0;
}

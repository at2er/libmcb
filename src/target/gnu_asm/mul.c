/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "mcb/func.h"
#include "mcb/inst.h"
#include "mcb/value.h"

#include "gnu_asm.h"
#include "inst.h"
#include "value.h"

#include "../../str.h"

int
build_mul_inst(struct mcb_inst *inst_outer,
		struct mcb_func *fn,
		struct gnu_asm *ctx)
{
	struct str src;
	struct mcb_mul_inst *inst;
	struct gnu_asm_value *lhs_val, *rhs_val, *result;

	assert(inst_outer && fn && ctx);
	inst = &inst_outer->inner.mul;
	assert(inst->lhs && inst->rhs);
	lhs_val = inst->lhs->data;
	rhs_val = inst->rhs->data;
	assert(lhs_val && rhs_val);

	assert(inst->result->data == NULL);
	result = calloc(1, sizeof(*result));
	if (!result)
		return 1;
	result->kind = map_type_to_value_kind(
			I8_REG_VALUE,
			inst->result->type);
	result->inner.reg = alloc_reg(fn);
	if (result->inner.reg == REG_COUNT)
		return 1;
	inst->result->data = result;

	if (!str_clean(&ctx->buf))
		return 1;
	ctx->buf.len = snprintf(ctx->buf.s, ctx->buf.siz,
			"mul%c %s\n",
			get_inst_suffix(lhs_val->kind),
			src.s);
	if (!str_append_str(&ctx->text, &ctx->buf))
		return 1;

	str_free(&src);

	if (inst->lhs->scope_end == inst_outer)
		drop_value(inst->lhs, fn);
	if (inst->rhs->scope_end == inst_outer)
		drop_value(inst->rhs, fn);

	return 0;
}

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
#include "func.h"
#include "gen_mov.h"
#include "gnu_asm.h"
#include "inst.h"
#include "value.h"

#include "../../err.h"
#include "../../str.h"

static int gen_imul(enum GNU_ASM_VALUE_KIND result_kind,
		struct str *src,
		struct gnu_asm *ctx);
static int gen_mul(enum GNU_ASM_VALUE_KIND result_kind,
		struct str *src,
		struct gnu_asm *ctx);
static void get_lhs_and_rhs(
		struct gnu_asm_value **lhs,
		struct gnu_asm_value **rhs,
		const struct mcb_mul_inst *inst);
static int get_mul_operands(struct str *src,
		struct gnu_asm_value *result,
		struct gnu_asm_value *lhs,
		struct gnu_asm_value *rhs,
		struct mcb_func *fn,
		struct gnu_asm *ctx);
static struct gnu_asm_value *get_result(
		struct mcb_mul_inst *inst,
		struct mcb_func *fn);
static int mov_rax_user(struct mcb_func *fn, struct gnu_asm *ctx);
static int mul_by_imm(struct str *str,
		const struct gnu_asm_value *val,
		struct mcb_func *fn,
		struct gnu_asm *ctx);

int
gen_imul(enum GNU_ASM_VALUE_KIND result_kind,
		struct str *src,
		struct gnu_asm *ctx)
{
	return snprintf(ctx->buf.s, ctx->buf.siz,
			"imul%c %s\n",
			get_inst_suffix(result_kind),
			src->s);
}

int
gen_mul(enum GNU_ASM_VALUE_KIND result_kind,
		struct str *src,
		struct gnu_asm *ctx)
{
	return snprintf(ctx->buf.s, ctx->buf.siz,
			"mul%c %s\n",
			get_inst_suffix(result_kind),
			src->s);
}

void
get_lhs_and_rhs(struct gnu_asm_value **lhs,
		struct gnu_asm_value **rhs,
		const struct mcb_mul_inst *inst)
{
	assert(inst);
	assert(inst->lhs && inst->rhs);
	assert(lhs && rhs);
	*lhs = inst->lhs->data;
	*rhs = inst->rhs->data;
	assert(*lhs && *rhs);
}

int
get_mul_operands(struct str *src,
		struct gnu_asm_value *result,
		struct gnu_asm_value *lhs,
		struct gnu_asm_value *rhs,
		struct mcb_func *fn,
		struct gnu_asm *ctx)
{
	if (IS_IMM(lhs->kind)) {
		if (!str_clean(&ctx->buf))
			return 1;
		if (gen_mov(&ctx->buf, result, lhs))
			return 1;
		if (!str_append_str(&ctx->text, &ctx->buf))
			return 1;
	}

	if (IS_IMM(rhs->kind)) {
		if (mul_by_imm(src, rhs, fn, ctx))
			return 1;
	}

	return 0;
}

struct gnu_asm_value *
get_result(struct mcb_mul_inst *inst, struct mcb_func *fn)
{
	struct gnu_asm_value *result;
	assert(inst && fn);
	assert(inst->result->data == NULL);
	result = calloc(1, sizeof(*result));
	if (!result)
		return NULL;
	result->kind = map_type_to_value_kind(
			I8_REG_VALUE,
			inst->result->type);
	result->inner.reg = alloc_reg(result, fn);
	if (result->inner.reg == REG_COUNT)
		return NULL;
	inst->result->data = result;
	return result;
}

int
mov_rax_user(struct mcb_func *fn, struct gnu_asm *ctx)
{
	struct gnu_asm_func *f;
	struct gnu_asm_value src;
	struct gnu_asm_value *user;
	assert(fn && ctx);
	f = fn->data;
	assert(f);
	user = f->reg_allocated[RAX];
	assert(user);

	assert(IS_REG(user->kind));

	src = *user;
	user->inner.reg = alloc_reg(user, fn);
	if (user->inner.reg == REG_COUNT)
		ereturn(1, "alloc_reg()");

	if (!str_clean(&ctx->buf))
		ereturn(1, "str_clean(&ctx->buf)");
	if (gen_mov(&ctx->buf, user, &src))
		ereturn(1, "gen_mov()");
	if (!str_append_str(&ctx->text, &ctx->buf))
		ereturn(1, "str_append_str()");

	return 0;
}

int
mul_by_imm(struct str *str,
		const struct gnu_asm_value *val,
		struct mcb_func *fn,
		struct gnu_asm *ctx)
{
	struct gnu_asm_value tmp;
	assert(str && val && fn && ctx);

	tmp.kind = remap_value_kind(I8_REG_VALUE, val->kind);
	tmp.inner.reg = alloc_reg(&tmp, fn);

	if (tmp.inner.reg == REG_COUNT)
		return 1;
	if (!str_clean(&ctx->buf))
		return 1;
	if (gen_mov(&ctx->buf, &tmp, val))
		ereturn (1, "gen_mov()");
	if (!str_append_str(&ctx->text, &ctx->buf))
		return 1;
	if (!str_from_value(str, &tmp))
		return 1;

	drop_reg(tmp.inner.reg, fn);
	return 0;
}

int
build_mul_inst(struct mcb_inst *inst_outer,
		struct mcb_func *fn,
		struct gnu_asm *ctx)
{
	struct mcb_mul_inst *inst;
	int len;
	struct gnu_asm_value *lhs_val, *rhs_val, *result;
	struct str src;

	assert(inst_outer && fn && ctx);
	inst = &inst_outer->inner.mul;

	assert(inst->result && inst->result->scope_end);
	if (inst->result->scope_end == inst_outer)
		return 0;

	get_lhs_and_rhs(&lhs_val, &rhs_val, inst);
	if (((struct gnu_asm_func*)fn->data)->reg_allocated[RAX]) {
		if (mov_rax_user(fn, ctx))
			return 1;
		drop_reg(RAX, fn);
	}
	result = get_result(inst, fn);
	if (!result)
		ereturn(1, "get_result()");

	if (get_mul_operands(&src, result, lhs_val, rhs_val, fn, ctx))
		ereturn(1, "get_mul_operands()");

	if (!str_clean(&ctx->buf))
		ereturn(1, "str_clean(&ctx->buf)");

	if (IS_INT(result->kind)) {
		len = gen_imul(result->kind, &src, ctx);
	} else {
		len = gen_mul(result->kind, &src, ctx);
	}

	if (len < 0)
		return 1;
	ctx->buf.len = len;
	if (!str_append_str(&ctx->text, &ctx->buf))
		return 1;

	if (inst->lhs->scope_end == inst_outer)
		drop_value(inst->lhs, fn);
	if (inst->rhs->scope_end == inst_outer)
		drop_value(inst->rhs, fn);

	return 0;
}

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

static int store_to_var(struct mcb_inst *inst_outer, struct gnu_asm *ctx);
static int store_imm(struct mcb_store_inst *inst);

int
store_to_var(struct mcb_inst *inst_outer, struct gnu_asm *ctx)
{
	struct text_block *blk;
	struct mcb_store_inst *inst = &inst_outer->inner.store;
	struct gnu_asm_value src;
	struct gnu_asm_value *val = inst->container->data;
	assert(val);
	if (inst->container->scope_end == inst_outer)
		return 0;

	src.kind = remap_value_kind(I8_IMM_VALUE, val->kind);
	src.inner.imm.i = inst->operand.i;

	estr_clean(&ctx->buf);
	if (gen_mov(&ctx->buf, val, &src))
		eabort("gen_mov()");
	blk = text_block_from_str(&ctx->buf);
	append_text_block(&ctx->text, blk);

	return 0;
}

int
store_imm(struct mcb_store_inst *inst)
{
	struct gnu_asm_value *v = ecalloc(1, sizeof(*v));
	v->container = inst->container;
	v->kind = map_type_to_value_kind(
			I8_IMM_VALUE,
			inst->container->type);
	if (v->kind == UNKOWN_VALUE)
		eabort("map_type_to_value_kind()");
	v->inner.imm.i = inst->operand.i;
	inst->container->data = v;
	return 0;
}

int
build_store_inst(struct mcb_inst *inst_outer,
		struct gnu_asm *ctx)
{
	struct mcb_store_inst *inst;
	assert(inst_outer && ctx);
	inst = &inst_outer->inner.store;
	assert(inst);
	assert(inst->container);
	switch (inst->container->kind) {
	case MCB_NORMAL_VALUE:
		return store_imm(inst);
	case MCB_FUNC_ARG_VALUE:
		ereturn(1, "store to value of function argument.");
	case MCB_VAR_VALUE:
		return store_to_var(inst_outer, ctx);
	}
	return 0;
}

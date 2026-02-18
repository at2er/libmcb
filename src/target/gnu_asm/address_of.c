/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "mcb/func.h"
#include "mcb/inst.h"
#include "mcb/inst/address_of.h"
#include "mcb/value.h"

#define LIBMCB_STRIP
#include "gen_mov.h"
#include "gnu_asm.h"
#include "inst.h"
#include "mem.h"
#include "reg.h"
#include "value.h"

#include "../../ealloc.h"
#include "../../err.h"
#include "../../str.h"
#include "../../text_block.h"

static int address_of_mem(
		struct gnu_asm_value *val,
		struct mcb_address_of_inst *inst,
		struct mcb_func *fn,
		struct gnu_asm *ctx);

int
address_of_mem(struct gnu_asm_value *val,
		struct mcb_address_of_inst *inst,
		struct mcb_func *fn,
		struct gnu_asm *ctx)
{
	struct str dst, src;
	struct text_block *blk;
	int len;
	struct gnu_asm_value *result;
	struct gnu_asm_mem_obj *mem;
	assert(val && inst && fn && ctx);
	assert(inst->result);

	result = inst->result->data = ecalloc(1, sizeof(*result));
	result->container = inst->result;
	result->kind = map_type_to_value_kind(
			I8_REG_VALUE,
			inst->result->type);
	result->inner.reg = alloc_reg(AUTO_ALLOC_REG, result, fn);
	if (result->inner.reg == REG_COUNT)
		eabort("alloc_reg()");

	str_from_value(&dst, result);
	str_from_mem(&src, val);

	mem = ecalloc(1, sizeof(*mem));
	mem->base = result->inner.reg;
	mem->kind = VAR_MEM;
	mem->offset = 0;
	mem->user = result;
	result->kind = map_type_to_value_kind(I8_MEM_VALUE, inst->val->type);
	result->inner.mem = mem;

	estr_clean(&ctx->buf);
	len = snprintf(ctx->buf.s, ctx->buf.siz,
			"lea%c %s, %s\n",
			get_inst_suffix(result->kind),
			src.s, dst.s);
	if (len < 0)
		eabort("snprintf()");
	ctx->buf.len = len;
	blk = text_block_from_str(&ctx->buf);
	append_text_block(&ctx->text, blk);

	return 0;
}

int
build_address_of_inst(struct mcb_inst *inst_outer,
		struct mcb_func *fn,
		struct gnu_asm *ctx)
{
	struct mcb_address_of_inst *inst;
	struct gnu_asm_value *val;
	assert(inst_outer && fn && ctx);
	inst = &inst_outer->inner.address_of;
	assert(inst);
	
	if (inst->result->scope_end == inst_outer)
		return 0;

	assert(inst->val);
	val = inst->val->data;
	assert(val);

	switch (val->kind) {
	CASE_IMM_VALUE: ereturn(1, "get address of immediate");
	CASE_MEM_VALUE: return address_of_mem(val, inst, fn, ctx);
	CASE_REG_VALUE: ereturn(1, "get address of register");
	default: eabort("get address of 'unknown value'");
	}

	return 0;
}

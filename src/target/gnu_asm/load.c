/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "mcb/func.h"
#include "mcb/inst.h"
#include "mcb/inst/load.h"
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

int
build_load_inst(struct mcb_inst *inst_outer,
		struct mcb_func *fn,
		struct gnu_asm *ctx)
{
	struct gnu_asm_value *address, *result;
	struct text_block *blk;
	struct mcb_load_inst *inst;
	assert(inst_outer && fn && ctx);
	inst = &inst_outer->inner.load;
	assert(inst);
	
	if (inst->result->scope_end == inst_outer)
		return 0;

	assert(inst->address);
	address = inst->address->data;
	assert(address);

	assert(inst->result);
	result = inst->result->data = ecalloc(1, sizeof(*result));
	result->container = inst->result;
	result->kind = map_type_to_value_kind(
			I8_REG_VALUE,
			inst->result->type);
	result->inner.reg = alloc_reg(AUTO_ALLOC_REG, result, fn);
	if (result->inner.reg == REG_COUNT)
		eabort("alloc_reg()");

	estr_clean(&ctx->buf);
	if (gen_mov(&ctx->buf, result, address))
		eabort("gen_mov()");
	blk = text_block_from_str(&ctx->buf);
	append_text_block(&ctx->text, blk);

	return 0;
}

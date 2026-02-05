/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "mcb/func.h"
#include "mcb/inst.h"
#include "mcb/value.h"

#include "gen_mov.h"
#include "gnu_asm.h"
#include "inst.h"
#include "value.h"

#include "../../str.h"

int
build_ret_inst(struct mcb_inst *inst_outer,
		struct mcb_func *fn,
		struct gnu_asm *ctx)
{
	struct gnu_asm_value dst, *src;
	struct mcb_ret_inst *inst = &inst_outer->inner.ret;
	assert(inst && fn && ctx);
	assert(inst->val);
	assert(inst->val->data);
	src = inst->val->data;

	dst.kind = map_type_to_value_kind(
			I8_REG_VALUE,
			inst->val->type);
	dst.inner.reg = RAX;

	if (IS_REG(src->kind) && src->inner.reg == dst.inner.reg)
		goto gen_ret;

	if (!str_clean(&ctx->buf))
		return 1;
	if (gen_mov(&ctx->buf, &dst, src))
		return 1;
	if (!str_append_str(&ctx->text, &ctx->buf))
		return 1;

gen_ret:
	if (!str_append_cstr(&ctx->text, "ret\n"))
		return 1;

	return 0;
}

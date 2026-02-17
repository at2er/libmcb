/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>

#define LIBMCB_STRIP
#include "inst.h"

#include "../../err.h"

int
build_inst(struct mcb_inst *inst,
		struct mcb_func *fn,
		struct gnu_asm *ctx)
{
	assert(inst && fn && ctx);

	switch (inst->kind) {
	case MCB_ADD_INST:    return build_add_inst(inst, fn, ctx);
	case MCB_BRANCH_INST: return build_branch_inst(inst, fn, ctx);
	case MCB_CALL_INST:   return build_call_inst(inst, fn, ctx);
	case MCB_CMP_INST:    return build_cmp_inst(inst, fn, ctx);
	case MCB_DIV_INST:    return build_div_inst(inst, fn, ctx);
	case MCB_MUL_INST:    return build_mul_inst(inst, fn, ctx);
	case MCB_RET_INST:    return build_ret_inst(inst, fn, ctx);
	case MCB_STORE_INST:  return build_store_inst(inst, fn, ctx);
	case MCB_SUB_INST:    return build_sub_inst(inst, fn, ctx);
	}

	return 0;
}

char
get_inst_suffix(enum GNU_ASM_VALUE_KIND dst_kind)
{
	/* TODO */
	switch (dst_kind) {
	case UNKOWN_VALUE:
		return '\0';
	CASE_I8_VALUE:  return 'l';
	CASE_I16_VALUE: return 'l';
	CASE_I32_VALUE: return 'l';
	CASE_I64_VALUE: return 'q';
	case CMP_RESULT_VALUE:
		eabort("unexpected dst_kind 'CMP_RESULT_VALUE'");
		break;
	}
	return '\0';
}

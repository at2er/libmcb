/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include "gnu_asm.h"
#include "mcb/inst/mov.h"
#include "mcb/mcb.h"
#include "mcb/target.h"
#include "mcb/text_block.h"
#include "mov.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

static int mov_m2m(struct mcb_context *context,
		const struct mcb_operand *dst,
		const struct mcb_operand *src);

int mov_m2m(struct mcb_context *context,
		const struct mcb_operand *dst,
		const struct mcb_operand *src)
{
	struct mcb_operand tmp;
	if (mcb_gnu_asm_alloc_reg(context, &tmp, src, src->size))
		return 1;
	if (mcb_gnu_asm_mov(context, dst, &tmp))
		return 1;
	if (mcb_gnu_asm_drop_reg(context, &tmp))
		return 1;
	return 0;
}

int mcb_gnu_asm_mov(struct mcb_context *context,
		const struct mcb_operand *dst,
		const struct mcb_operand *src)
{
	struct mcb_text_block *blk;
	const char *temp = "mov%c %s, %s\n";
	MCB_DEF_STR_IN_STACK(dst_str, __buf0, OPERAND_BUFSIZ);
	MCB_DEF_STR_IN_STACK(src_str, __buf1, OPERAND_BUFSIZ);
	MCB_IMPL(mcb_impl_mov(context, dst, src));
	if (dst->type == MCB_REG && src->type == MCB_REG)
		if (dst->inner.reg == src->inner.reg)
			return 0;
	if (dst->type == MCB_MEM && src->type == MCB_MEM)
		return mov_m2m(context, dst, src);
	if (mcb_gnu_asm_get_operand(&dst_str, dst))
		return 1;
	if (mcb_gnu_asm_get_operand(&src_str, src))
		return 1;
	blk = mcb_create_text_block(strlen(temp) + dst_str.len + src_str.len);
	blk->str_siz = snprintf(blk->str, blk->str_siz, temp,
			mcb_gnu_asm_get_inst_suffix(dst->size),
			src_str.s,
			dst_str.s);
	mcb_join_text_block(&CUR_CONTEXT(context)->text_block, blk);
	return 0;
}

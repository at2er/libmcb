/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include "calc.h"
#include "gnu_asm.h"
#include "mcb/mcb.h"
#include "mcb/operand.h"
#include "mcb/size.h"
#include "mcb/target.h"
#include "mcb/text_block.h"
#include "mov.h"
#include "reg.h"
#include "utils.h"
#include "../die.h"

#define TMP_REG RBX
#define CHECK_ANY_SIGNED(S0, S1) \
	(MCB_IS_SIGNED_SIZE((S0)->size) || MCB_IS_SIGNED_SIZE((S1)->size))
#define GET_TEMP_WITH_SIGNED(RESULT, S0, S1, TEMP_SIGNED, TEMP_UNSIGNED) \
	RESULT = CHECK_ANY_SIGNED(S0, S1) ? TEMP_SIGNED : TEMP_UNSIGNED

static int calc_with_rAX_only(struct mcb_context *context,
		const struct mcb_operand *dst,
		const struct mcb_operand *s0,
		const struct mcb_operand *s1,
		const char *temp);
static void get_add_src(struct mcb_context *context,
		const struct mcb_operand *dst,
		const struct mcb_operand *s0,
		const struct mcb_operand *s1,
		struct mcb_str *str);
static void get_dst(const struct mcb_operand *dst, struct mcb_str *str);
static bool get_rAX_only_dst(struct mcb_context *context,
		struct mcb_operand *result,
		const struct mcb_operand *dst,
		const struct mcb_operand *s0);
static bool get_rAX_only_src(struct mcb_context *context,
		struct mcb_operand *result,
		const struct mcb_operand *dst,
		const struct mcb_operand *s1);
static void get_sub_src(struct mcb_context *context,
		const struct mcb_operand *dst,
		const struct mcb_operand *s0,
		const struct mcb_operand *s1,
		struct mcb_str *str);
static int is_operand_in_rAX(const struct mcb_operand *src);

static const enum REG_KIND block_rDX[] = {RDX,-1};

int calc_with_rAX_only(struct mcb_context *context,
		const struct mcb_operand *dst,
		const struct mcb_operand *s0,
		const struct mcb_operand *s1,
		const char *temp)
{
	struct mcb_text_block *blk;
	struct mcb_operand rAX = {0}, operand = {0};
	bool rAX_allocated = false, operand_allocated = false;
	MCB_DEF_STR_IN_STACK(src_str, __buf0, OPERAND_BUFSIZ);
	MCB_IMPL(mcb_impl_calc(context, dst, s0, s1));

	rAX_allocated = get_rAX_only_dst(context, &rAX, dst, s0);
	operand_allocated = get_rAX_only_src(context, &operand, dst, s1);
	if (mcb_gnu_asm_get_operand(&src_str, &operand))
		return 1;

	blk = mcb_create_text_block(0);
	blk->str_siz = snprintf(blk->str, blk->str_siz, temp,
			mcb_gnu_asm_get_inst_suffix(dst->size),
			src_str.s);
	mcb_join_text_block(&CUR_CONTEXT(context)->text_block, blk);

	if (mcb_gnu_asm_mov(context, dst, &rAX))
		return 1;

	if (rAX_allocated)
		if (mcb_gnu_asm_drop_reg(context, &rAX))
			return 1;
	if (operand_allocated)
		if (mcb_gnu_asm_drop_reg(context, &operand))
			return 1;

	return 0;
}

void get_add_src(struct mcb_context *context,
		const struct mcb_operand *dst,
		const struct mcb_operand *s0,
		const struct mcb_operand *s1,
		struct mcb_str *str)
{
	if (is_operand_in_rAX(s0)) {
		if (mcb_gnu_asm_get_operand(str, s1))
			goto die_get_src;
		return;
	} else if (is_operand_in_rAX(s1)) {
		if (mcb_gnu_asm_get_operand(str, s0))
			goto die_get_src;
		return;
	}
	if (mcb_gnu_asm_mov(context, dst, s0))
		die(DIE_MSG"failed to gen mov\n", DIE_MSG_ARG);
	if (mcb_gnu_asm_get_operand(str, s1))
		goto die_get_src;
	return;
die_get_src:
	die(DIE_MSG"failed to get src operand\n", DIE_MSG_ARG);
	return;
}

void get_dst(const struct mcb_operand *dst, struct mcb_str *str)
{
	if (mcb_gnu_asm_get_operand(str, dst))
		die(DIE_MSG"failed to get dst operand\n", DIE_MSG_ARG);
	return;
}

bool get_rAX_only_dst(struct mcb_context *context,
		struct mcb_operand *result,
		const struct mcb_operand *dst,
		const struct mcb_operand *s0)
{
	if (is_operand_in_rAX(dst)) {
		if (s0->type == MCB_IMM) {
			if (mcb_gnu_asm_mov(context, dst, s0))
				goto die_mov_imm;
		} else if (!is_operand_in_rAX(s0)) {
			goto alloc;
		}
		*result = *dst;
		return false;
	}

alloc:
	if (mcb_gnu_asm_alloc_reg_force(RAX, context, result, s0, s0->size))
		goto die_alloc;
	return true;

die_mov_imm:
	die(DIE_MSG"failed to move immediate to register rAX\n", DIE_MSG_ARG);
	return false;
die_alloc:
	die(DIE_MSG"failed to allocate register rAX\n", DIE_MSG_ARG);
	return false;
}

bool get_rAX_only_src(struct mcb_context *context,
		struct mcb_operand *result,
		const struct mcb_operand *dst,
		const struct mcb_operand *s1)
{
	enum MCB_SIZE size = s1->size;
	if (s1->type == MCB_REG) {
		*result = *s1;
		return false;
	}
	if (s1->type == MCB_IMM)
		size = dst->size;
	if (mcb_gnu_asm_alloc_reg_without(block_rDX, context,
				result, s1, size))
		goto die_alloc;
	return true;
die_alloc:
	die(DIE_MSG"failed to allocate register src\n", DIE_MSG_ARG);
	return false;
}

void get_sub_src(struct mcb_context *context,
		const struct mcb_operand *dst,
		const struct mcb_operand *s0,
		const struct mcb_operand *s1,
		struct mcb_str *str)
{
	if (mcb_gnu_asm_mov(context, dst, s0))
		die(DIE_MSG"failed to gen mov\n", DIE_MSG_ARG);
	if (mcb_gnu_asm_get_operand(str, s1))
		goto die_get_src;
	return;
die_get_src:
	die(DIE_MSG"failed to get src operand\n", DIE_MSG_ARG);
	return;
}

int is_operand_in_rAX(const struct mcb_operand *src)
{
	struct reg *reg;
	if (src->type != MCB_REG)
		return 0;
	if (!(reg = src->inner.data))
		return 1;
	if (reg->kind == mcb_gnu_asm_offset_reg(RAX, src->size))
		return 1;
	return 0;
}

int mcb_gnu_asm_add(struct mcb_context *context,
		const struct mcb_operand *dst,
		const struct mcb_operand *s0,
		const struct mcb_operand *s1)
{
	struct mcb_text_block *blk;
	const char *temp = "add%c %s, %s\n";
	MCB_DEF_STR_IN_STACK(dst_str, __buf0, OPERAND_BUFSIZ);
	MCB_DEF_STR_IN_STACK(src_str, __buf1, OPERAND_BUFSIZ);
	MCB_IMPL(mcb_impl_calc(context, dst, s0, s1));
	get_dst(dst, &dst_str);
	get_add_src(context, dst, s0, s1, &src_str);
	blk = mcb_create_text_block(0);
	blk->str_siz = snprintf(blk->str, blk->str_siz, temp,
			mcb_gnu_asm_get_inst_suffix(dst->size),
			src_str.s, dst_str.s);
	mcb_join_text_block(&CUR_CONTEXT(context)->text_block, blk);
	return 0;
}

int mcb_gnu_asm_div(struct mcb_context *context,
		const struct mcb_operand *dst,
		const struct mcb_operand *s0,
		const struct mcb_operand *s1)
{
	const char *temp_signed = "idiv%c %s\n",
	           *temp_unsigned = "div%c %s\n",
	           *temp;
	GET_TEMP_WITH_SIGNED(temp, s0, s1, temp_signed, temp_unsigned);
	return calc_with_rAX_only(context, dst, s0, s1, temp);
}

int mcb_gnu_asm_mul(struct mcb_context *context,
		const struct mcb_operand *dst,
		const struct mcb_operand *s0,
		const struct mcb_operand *s1)
{
	const char *temp_signed = "imul%c %s\n",
	           *temp_unsigned = "mul%c %s\n",
	           *temp;
	GET_TEMP_WITH_SIGNED(temp, s0, s1, temp_signed, temp_unsigned);
	return calc_with_rAX_only(context, dst, s0, s1, temp);
}

int mcb_gnu_asm_sub(struct mcb_context *context,
		const struct mcb_operand *dst,
		const struct mcb_operand *s0,
		const struct mcb_operand *s1)
{
	struct mcb_text_block *blk;
	const char *temp = "sub%c %s, %s\n";
	MCB_DEF_STR_IN_STACK(dst_str, __buf0, OPERAND_BUFSIZ);
	MCB_DEF_STR_IN_STACK(src_str, __buf1, OPERAND_BUFSIZ);
	MCB_IMPL(mcb_impl_calc(context, dst, s0, s1));
	get_dst(dst, &dst_str);
	get_sub_src(context, dst, s0, s1, &src_str);
	blk = mcb_create_text_block(0);
	blk->str_siz = snprintf(blk->str, blk->str_siz, temp,
			mcb_gnu_asm_get_inst_suffix(dst->size),
			src_str.s, dst_str.s);
	mcb_join_text_block(&CUR_CONTEXT(context)->text_block, blk);
	return 0;
}

/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include "func.h"
#include "gnu_asm.h"
#include "mcb/inst/func.h"
#include "mcb/mcb.h"
#include "mcb/operand.h"
#include "mcb/size.h"
#include "mcb/target.h"
#include "mcb/text_block.h"
#include "reg.h"
#include "utils.h"
#include "../utils.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int alloc_func_arg(struct mcb_context *context,
		uint16_t index,
		struct mcb_operand *container);
static struct mcb_text_block *get_globl_label(const char *name);
static struct mcb_text_block *get_label(const char *name);
static struct mcb_text_block *get_weak_label(const char *name);
static struct mcb_text_block *ret_with_operand(struct mcb_context *context,
		const struct mcb_operand *src,
		enum REG_KIND result_reg);

static const enum REG_KIND func_args[] = {
	RDI, RSI, RDX, RCX, R8, R9
};

int alloc_func_arg(struct mcb_context *context,
		uint16_t index,
		struct mcb_operand *container)
{
	if (index > LENGTH(func_args))
		return 1;
	if (container->size >= MCB_SIZE_COUNT)
		return 1;
	return mcb_gnu_asm_alloc_reg_force(func_args[index],
			context, container, NULL, container->size);
}

struct mcb_text_block *get_globl_label(const char *name)
{
	const char *temp = ".globl %s\n%s:\n";
	struct mcb_text_block *blk = mcb_create_text_block(
			strlen(temp) + strlen(name) * 2);
	blk->str_siz = snprintf(blk->str, blk->str_siz, temp, name, name);
	return blk;
}

struct mcb_text_block *get_label(const char *name)
{
	const char *temp = "%s:\n";
	struct mcb_text_block *blk = mcb_create_text_block(
			strlen(temp) + strlen(name));
	blk->str_siz = snprintf(blk->str, blk->str_siz, temp, name);
	return blk;
}

struct mcb_text_block *get_weak_label(const char *name)
{
	const char *temp = ".weak %s\n%s:\n";
	struct mcb_text_block *blk = mcb_create_text_block(
			strlen(temp) + strlen(name) * 2);
	blk->str_siz = snprintf(blk->str, blk->str_siz, temp, name, name);
	return blk;
}

struct mcb_text_block *ret_with_operand(struct mcb_context *context,
		const struct mcb_operand *src,
		enum REG_KIND result_reg)
{
	struct mcb_text_block *blk;
	const char *temp = "mov%c %s, %%%s\nret\n";
	MCB_DEF_STR_IN_STACK(src_str, __buf0, OPERAND_BUFSIZ);
	if (mcb_gnu_asm_get_operand(&src_str, src))
		return NULL;
	blk = mcb_create_text_block(0);
	blk->str_siz = snprintf(blk->str, blk->str_siz, temp,
			mcb_gnu_asm_get_inst_suffix(src->size),
			src_str.s,
			mcb_gnu_asm_get_reg_str(result_reg));
	return blk;
}

int mcb_gnu_asm_call_func(struct mcb_context *context,
		uint16_t argc,
		const struct mcb_operand *args,
		const char *name)
{
	struct mcb_text_block *blk;
	const char *temp = "call %s\n";
	MCB_IMPL(mcb_impl_call_func(context, argc, args, name));
	blk = mcb_create_text_block(0);
	blk->str_siz = snprintf(blk->str, blk->str_siz, temp, name);
	mcb_join_text_block(&CUR_CONTEXT(context)->text_block, blk);
	return 0;
}

int mcb_gnu_asm_enter_func(struct mcb_context *context,
		enum MCB_ENTER_FUNC_FLAGS flags,
		struct mcb_operand *args[],
		enum MCB_SIZE size,
		const char *name)
{
	uint16_t arg_count = 0;
	struct mcb_text_block *blk;
	MCB_IMPL(mcb_impl_enter_func(context, flags, args, size, name));
	for (; args[arg_count] != NULL; arg_count++) {
		if (alloc_func_arg(context, arg_count, args[arg_count]))
			return 1;
	}
	if (flags & MCB_ENTER_FUNC_FLAG_EXPORT) {
		blk = get_globl_label(name);
	} else if (flags & MCB_ENTER_FUNC_FLAG_EXPORT_WEAK) {
		blk = get_weak_label(name);
	} else {
		blk = get_label(name);
	}
	mcb_join_text_block(&CUR_CONTEXT(context)->text_block, blk);
	CUR_CONTEXT(context)->func_arg_operands = calloc(arg_count,
			sizeof(struct mcb_operand*));
	CUR_CONTEXT(context)->func_begin_block = blk;
	memcpy(CUR_CONTEXT(context)->func_arg_operands, args,
			arg_count * sizeof(struct mcb_operand*));
	return 0;
}

int mcb_gnu_asm_exit_func(struct mcb_context *context)
{
	struct mcb_text_block *blk;
	const char *temp = "subq $%lu, %%rsp\n";
	MCB_IMPL(mcb_impl_exit_func(context));
	mcb_gnu_asm_clean_reg_stats(CUR_CONTEXT(context)->reg_stats);
	mcb_gnu_asm_clean_mem_stat(&CUR_CONTEXT(context)->mem_stat);
	if (CUR_CONTEXT(context)->mem_stat.stack_top == 0)
		return 0;
	blk = mcb_create_text_block(0);
	blk->str_siz = snprintf(blk->str, blk->str_siz, temp,
			(CUR_CONTEXT(context)->mem_stat.stack_top + 15) & -16);
	mcb_insert_text_block(CUR_CONTEXT(context)->func_begin_block,
			NULL, blk);
	return 0;
}

int mcb_gnu_asm_func_ret(struct mcb_context *context,
		const struct mcb_operand *src)
{
	struct mcb_text_block *blk;
	enum REG_KIND result_reg;
	MCB_IMPL(mcb_impl_func_ret(context, src));
	result_reg = mcb_gnu_asm_offset_reg(RAX, src->size);
	if (result_reg == -1)
		return 1;
	if (src->type == MCB_REG && ((struct reg*)src->inner.data)
			->kind == result_reg) {
		blk = mcb_create_text_block(0);
		blk->str_siz = snprintf(blk->str, blk->str_siz, "ret\n");
	} else {
		blk = ret_with_operand(context, src, result_reg);
	}
	mcb_join_text_block(&CUR_CONTEXT(context)->text_block, blk);
	return 0;
}

int mcb_gnu_asm_prepare_func_call_arg(struct mcb_context *context,
		uint16_t index,
		struct mcb_operand *dst,
		enum MCB_SIZE size)
{
	MCB_IMPL(mcb_impl_prepare_func_call_arg(context, index, dst, size));
	return mcb_gnu_asm_alloc_reg_force(func_args[index], context,
			dst, NULL, size);
}

/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include "gnu_asm.h"
#include "mcb/inst/reg.h"
#include "mcb/mcb.h"
#include "mcb/operand.h"
#include "mcb/target.h"
#include "mem.h"
#include "mov.h"
#include "reg.h"
#include "../die.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

static bool check_in_list(const enum REG_KIND regs[], enum REG_KIND reg);
static enum REG_KIND get_basic_reg_kind(enum REG_KIND reg);
static int init_reg(struct mcb_context *context,
		struct mcb_operand *dst,
		const struct mcb_operand *val);
static int save_reg(struct mcb_context *context,
		struct reg *container,
		const struct mcb_operand *src);
static void set_reg_stat(struct mcb_context *context,
		enum REG_KIND reg,
		enum REG_STAT stat);

static char *regs[REG_COUNT] = {
	[RBP]  = "rbp",  [RSP]  = "rsp",
	[RAX]  = "rax",  [RBX]  = "rbx",  [RCX]  = "rcx",  [RDX]  = "rdx",
	[RDI]  = "rdi",  [RSI]  = "rsi",
	[R8]   = "r8",   [R9]   = "r9",   [R10]  = "r10",  [R11]  = "r11",
	[R12]  = "r12",  [R13]  = "r13",  [R14]  = "r14",  [R15]  = "r15",

	[EBP]  = "ebp",  [ESP]  = "esp",
	[EAX]  = "eax",  [EBX]  = "ebx",  [ECX]  = "ecx",  [EDX]  = "edx",
	[EDI]  = "edi",  [ESI]  = "esi",
	[R8D]  = "r8d",  [R9D]  = "r9d",  [R10D] = "r10d", [R11D] = "r11d",
	[R12D] = "r12d", [R13D] = "r13d", [R14D] = "r14d", [R15D] = "r15d",

	[BP]   = "bp",   [SP]   = "sp",
	[AX]   = "ax",   [BX]   = "bx",   [CX]   = "cx",   [DX]   = "dx",
	[DI]   = "di",   [SI]   = "si",
	[R8W]  = "r8w",  [R9W]  = "r9w",  [R10W] = "r10w", [R11W] = "r11w",
	[R12W] = "r12w", [R13W] = "r13w", [R14W] = "r14w", [R15W] = "r15w",

	[BPL]  = "bpl",  [SPL]  = "spl",
	[AL]   = "al",   [BL]   = "bl",   [CL]   = "cl",   [DL]   = "dl",
	[DIL]  = "dil",  [SIL]  = "sil",
	[R8B]  = "r8b",  [R9B]  = "r9b",  [R10B] = "r10b", [R11B] = "r11b",
	[R12B] = "r12b", [R13B] = "r13b", [R14B] = "r14b", [R15B] = "r15b"
};

static const enum REG_KIND reg_alloc_priority[] = {
	RAX, RBX,
	R10, R11, R12, R13, R14, R15,
	R9,  R8,  RCX, RDX, RSI, RDI
};

bool check_in_list(const enum REG_KIND regs[], enum REG_KIND reg)
{
	if (!regs)
		return false;
	for (uint16_t i = 0; regs[i] != -1; i++) {
		if (reg == regs[i])
			return true;
	}
	return false;
}

enum REG_KIND get_basic_reg_kind(enum REG_KIND reg)
{
	if (reg <= R15)
		return reg;
	if (reg >= EBP && reg <= R15D)
		return reg - EBP;
	if (reg >= BP  && reg <= R15W)
		return reg - BP;
	if (reg >= BPL && reg <= R15B)
		return reg - BPL;
	die(DIE_MSG"\n", DIE_MSG_ARG);
	return -1;
}

int init_reg(struct mcb_context *context,
		struct mcb_operand *dst,
		const struct mcb_operand *val)
{
	if (!val)
		return 0;
	return mcb_gnu_asm_mov(context, dst, val);
}

int save_reg(struct mcb_context *context,
		struct reg *container,
		const struct mcb_operand *src)
{
	container->old_count++;
	if (container->old_capacity < container->old_count) {
		container->old_capacity++;
		container->old = realloc(container->old,
				container->old_capacity
				* sizeof(*container->old));
	}
	return mcb_gnu_asm_alloc_mem(context,
			&container->old[container->old_count - 1],
			src,
			src->size);
}

void set_reg_stat(struct mcb_context *context,
		enum REG_KIND reg,
		enum REG_STAT stat)
{
	assert((reg = get_basic_reg_kind(reg)) != -1);
	CUR_CONTEXT(context)->reg_stats[reg] = stat;
}

int mcb_gnu_asm_alloc_reg(struct mcb_context *context,
		struct mcb_operand *dst,
		const struct mcb_operand *val,
		enum MCB_SIZE size)
{
	return mcb_gnu_asm_alloc_reg_without(NULL, context, dst, val, size);
}

int mcb_gnu_asm_alloc_reg_force(enum REG_KIND kind,
		struct mcb_context *context,
		struct mcb_operand *dst,
		const struct mcb_operand *val,
		enum MCB_SIZE size)
{
	enum REG_KIND kind_base;
	struct reg *self;
	MCB_IMPL(mcb_impl_alloc_reg(context, dst, val, size));
	kind_base = get_basic_reg_kind(kind);

	self = calloc(1, sizeof(*self));
	self->kind = mcb_gnu_asm_offset_reg(kind, size);
	dst->inner.data = self;

	if (CUR_CONTEXT(context)->reg_stats[kind_base] != REG_NOT_USE) {
		if (save_reg(context, self, dst))
			return 1;
	}

	set_reg_stat(context, kind_base, REG_USING);
	return init_reg(context, dst, val);
}

int mcb_gnu_asm_alloc_reg_without(const enum REG_KIND regs[],
		struct mcb_context *context,
		struct mcb_operand *dst,
		const struct mcb_operand *val,
		enum MCB_SIZE size)
{
	enum REG_STAT *reg_stats;
	struct reg *self = NULL;
	MCB_IMPL(mcb_impl_alloc_reg(context, dst, val, size));
	reg_stats = CUR_CONTEXT(context)->reg_stats;
	for (int i = 0; reg_alloc_priority[i] != -1; i++) {
		if (reg_stats[reg_alloc_priority[i]] != REG_NOT_USE)
			continue;
		if (check_in_list(regs, reg_alloc_priority[i]))
			continue;
		self = calloc(1, sizeof(*self));
		self->kind = mcb_gnu_asm_offset_reg(reg_alloc_priority[i], size);
		dst->inner.data = self;
		set_reg_stat(context, self->kind, REG_USING);
		return init_reg(context, dst, val);
	}
	//TODO: save reg and alloc
	abort();
	return init_reg(context, dst, val);
}

void mcb_gnu_asm_clean_reg_stats(enum REG_STAT stats[EBP])
{
	for (int i = 0; i < EBP; i++)
		stats[i] = REG_NOT_USE;
}

int mcb_gnu_asm_drop_reg(struct mcb_context *context,
		const struct mcb_operand *reg)
{
	struct reg *self;
	assert(reg && reg->type == MCB_REG);
	self = reg->inner.data;
	if (!self->old && !self->old_count) {
		set_reg_stat(context, self->kind, REG_NOT_USE);
		return 0;
	}
	self->old_count--;
	if (mcb_gnu_asm_mov(context, reg, &self->old[self->old_count]))
		return 1;
	return mcb_gnu_asm_drop_mem(context,
			self->old[self->old_count].inner.mem);
}

const char *mcb_gnu_asm_get_reg_str(enum REG_KIND reg)
{
	if (reg >= REG_COUNT)
		return NULL;
	return regs[reg];
}

enum REG_KIND mcb_gnu_asm_offset_reg(enum REG_KIND base,
		enum MCB_SIZE size)
{
	//TODO: Smaller registers.
	switch (size) {
	case MCB_I64: case MCB_U64: return base;
	case MCB_I32: case MCB_U32: return base + EBP;
	case MCB_I16: case MCB_U16: return base + BP;
	case MCB_I8:  case MCB_U8:  return base + BPL;
	default: break;
	}
	return -1;
}

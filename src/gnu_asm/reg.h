/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_GNU_ASM_REG_H
#define LIBMCB_GNU_ASM_REG_H
#include "mcb/operand.h"
#include "mcb/size.h"
#include <stdbool.h>
#include <stdint.h>

struct mcb_context;

enum REG_KIND {
	RBP, RSP,
	RAX, RBX, RCX, RDX,
	RDI, RSI,
	R8,  R9,  R10, R11, R12, R13, R14, R15,

	EBP, ESP,
	EAX, EBX, ECX, EDX,
	EDI, ESI,
	R8D, R9D, R10D, R11D, R12D, R13D, R14D, R15D,

	BP,  SP,
	AX,  BX, CX, DX,
	DI,  SI,
	R8W, R9W, R10W, R11W, R12W, R13W, R14W, R15W,

	BPL, SPL,
	AL,  BL,  CL, DL,
	DIL, SIL,
	R8B, R9B, R10B, R11B, R12B, R13B, R14B, R15B,

	REG_COUNT
};

enum REG_STAT {
	REG_NOT_USE,
	REG_USING,
	REG_IN_MEM
};

struct reg {
	enum REG_KIND kind;
	struct mcb_operand *old;
	uint32_t old_capacity, old_count;
};

int mcb_gnu_asm_alloc_reg(struct mcb_context *context,
		struct mcb_operand *dst,
		const struct mcb_operand *val,
		enum MCB_SIZE size);

int mcb_gnu_asm_alloc_reg_force(enum REG_KIND kind,
		struct mcb_context *context,
		struct mcb_operand *dst,
		const struct mcb_operand *val,
		enum MCB_SIZE size);

int mcb_gnu_asm_alloc_reg_without(const enum REG_KIND regs[],
		struct mcb_context *context,
		struct mcb_operand *dst,
		const struct mcb_operand *val,
		enum MCB_SIZE size);

void mcb_gnu_asm_clean_reg_stats(enum REG_STAT stats[EBP]);

int mcb_gnu_asm_drop_reg(struct mcb_context *context,
		const struct mcb_operand *reg);

const char *mcb_gnu_asm_get_reg_str(enum REG_KIND reg);

enum REG_KIND mcb_gnu_asm_offset_reg(enum REG_KIND base, enum MCB_SIZE size);

#endif

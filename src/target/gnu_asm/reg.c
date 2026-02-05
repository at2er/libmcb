/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include "func.h"
#include "reg.h"
#include "../../macros.h"

static const enum GNU_ASM_REG reg_alloc_priority[] = {
	RAX, RBX,
	R10, R11, R12, R13, R14, R15,
	R9,  R8,  RCX, RDX, RSI, RDI
};

static const char *regs[REG_COUNT][4] = {
	[RBP]  = {"%rbp", "%ebp",  "%bp",   "%bpl" },
	[RSP]  = {"%rsp", "%esp",  "%sp",   "%spl" },
	[RAX]  = {"%rax", "%eax",  "%ax",   "%al"  },
	[RBX]  = {"%rbx", "%ebx",  "%bx",   "%bl"  },
	[RCX]  = {"%rcx", "%ecx",  "%cx",   "%cl"  },
	[RDX]  = {"%rdx", "%edx",  "%dx",   "%dl"  },
	[RDI]  = {"%rdi", "%edi",  "%di",   "%dil" },
	[RSI]  = {"%rsi", "%esi",  "%si",   "%sil" },
	[R8 ]  = {"%r8",  "%r8d",  "%r8w",  "%r8b" },
	[R9 ]  = {"%r9",  "%r9d",  "%r9w",  "%r9b" },
	[R10]  = {"%r10", "%r10d", "%r10w", "%r10b"},
	[R11]  = {"%r11", "%r11d", "%r11w", "%r11b"},
	[R12]  = {"%r12", "%r12d", "%r12w", "%r12b"},
	[R13]  = {"%r13", "%r13d", "%r13w", "%r13b"},
	[R14]  = {"%r14", "%r14d", "%r14w", "%r14b"},
	[R15]  = {"%r15", "%r15d", "%r15w", "%r15b"}
};

enum GNU_ASM_REG
alloc_reg(struct mcb_func *fn)
{
	struct gnu_asm_func *f = fn->data;
	assert(f);
	for (unsigned int i = 0; i < LENGTH(reg_alloc_priority); i++) {
		if (!f->reg_allocated[reg_alloc_priority[i]]) {
			f->reg_allocated[reg_alloc_priority[i]] = true;
			return reg_alloc_priority[i];
		}
	}
	return REG_COUNT;
}

void
drop_reg(enum GNU_ASM_REG reg, struct mcb_func *fn)
{
	struct gnu_asm_func *f;
	assert(fn);
	f = fn->data;
	f->reg_allocated[reg] = false;
}

int
reg_offset_from_kind(enum GNU_ASM_VALUE_KIND kind)
{
	switch (kind) {
	case UNKOWN_VALUE:  break;
	case I8_REG_VALUE:  case I8_IMM_VALUE:  return 3;
	case I16_REG_VALUE: case I16_IMM_VALUE: return 2;
	case I32_REG_VALUE: case I32_IMM_VALUE: return 1;
	case I64_REG_VALUE: case I64_IMM_VALUE: return 0;
	}
	return -1;
}

struct str *
str_from_reg(struct str *s, enum GNU_ASM_REG r, int siz)
{
	assert(s);
	if (!str_empty(s))
		return NULL;
	if (r >= REG_COUNT)
		return NULL;
	if (!str_from_cstr(s, regs[r][siz]))
		return NULL;
	return s;
}

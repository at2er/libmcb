/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include "mcb/operand.h"
#include "mcb/str.h"
#include "mem.h"
#include "reg.h"
#include "utils.h"
#include <stdio.h>

static int get_imm_operand(struct mcb_str *out, const struct mcb_operand *src);
static int get_mem_operand(struct mcb_str *out, const struct mcb_operand *src);
static int get_reg_operand(struct mcb_str *out, const struct mcb_operand *src);

static const char suffix_table[4] = {
	[MCB_I8] = 'b',
	[MCB_I16] = 'w',
	[MCB_I32] = 'l',
	[MCB_I64] = 'q',
};

int get_imm_operand(struct mcb_str *out, const struct mcb_operand *src)
{
	const char *temp = MCB_IS_UNSIGNED_SIZE(src->size) ? "$%lu" :  "$%ld";
	out->len = snprintf(out->s, out->len, temp,
			MCB_IS_UNSIGNED_SIZE(src->size)
			? src->inner.uint
			: src->inner.sint);
	return 0;
}

int get_mem_operand(struct mcb_str *out, const struct mcb_operand *src)
{
	struct mem_elem *mem = src->inner.mem;
	const char *temp = "-%ld(%%%s)";
	snprintf(out->s, out->len, temp,
			mem->offset,
			mcb_gnu_asm_get_reg_str(mem->base));
	return 0;
}

int get_reg_operand(struct mcb_str *out, const struct mcb_operand *src)
{
	struct reg *r = src->inner.data;
	const char *temp = "%%%s";
	out->len = snprintf(out->s, out->len, temp,
			mcb_gnu_asm_get_reg_str(r->kind));
	return 0;
}

int mcb_gnu_asm_get_operand(struct mcb_str *out, const struct mcb_operand *src)
{
	if (src->type >= MCB_OPERAND_TYPE_COUNT)
		return 1;
	switch (src->type) {
	case MCB_IMM: return get_imm_operand(out, src); break;
	case MCB_MEM: return get_mem_operand(out, src); break;
	case MCB_REG: return get_reg_operand(out, src); break;
	default: break;
	}
	return 1;
}

char mcb_gnu_asm_get_inst_suffix(enum MCB_SIZE size)
{
	if (size >= MCB_SIZE_COUNT)
		return 'E';
	if (MCB_IS_UNSIGNED_SIZE(size))
		return suffix_table[size - MCB_I64];
	return suffix_table[size];
}

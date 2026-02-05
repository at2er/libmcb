/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "mcb/func.h"
#include "mcb/value.h"

#include "reg.h"
#include "value.h"

#include "../../str.h"

void
drop_value(struct mcb_value *val, struct mcb_func *fn)
{
	struct gnu_asm_value *gval;
	assert(val);
	gval = val->data;
	assert(gval);

	if (IS_REG(gval->kind))
		drop_reg(gval->inner.reg, fn);

	free(gval);
	val->data = NULL;
}

enum GNU_ASM_VALUE_KIND
map_type_to_value_kind(enum GNU_ASM_VALUE_KIND base, enum MCB_TYPE t)
{
	assert(base != UNKOWN_VALUE);
	switch (t) {
	case MCB_U8:  case MCB_I8:  return base;
	case MCB_U16: case MCB_I16: return base + 1;
	case MCB_U32: case MCB_I32: return base + 2;
	case MCB_U64: case MCB_I64: return base + 3;
	}
	return UNKOWN_VALUE;
}

struct str *
str_from_imm(struct str *s, struct gnu_asm_value *v)
{
	int ret = 0;
	assert(s && v);
	if (!str_empty(s))
		return NULL;
	if (!str_realloc(s, 32))
		return NULL;
	if (IS_INT_IMM(v->kind)) {
		ret = snprintf(s->s, s->siz, "$%ld", v->inner.imm.i);
		if (ret < 0)
			return NULL;
		s->len = ret;
		return s;
	}
	return NULL;
}

struct str *
str_from_value(struct str *s, struct gnu_asm_value *v)
{
	int reg_off;
	assert(s && v);
	if (IS_IMM(v->kind))
		return str_from_imm(s, v);
	if (IS_REG(v->kind)) {
		reg_off = reg_offset_from_kind(v->kind);
		assert(reg_off != -1);
		return str_from_reg(s, v->inner.reg, reg_off);
	}
	return NULL;
}

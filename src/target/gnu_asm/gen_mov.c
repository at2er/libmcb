/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stdio.h>

#include "gen_mov.h"
#include "inst.h"
#include "reg.h"
#include "value.h"

#include "../../str.h"

int
gen_mov(struct str *s,
		struct gnu_asm_value *dst,
		struct gnu_asm_value *src)
{
	struct str dst_str, src_str;
	assert(s && dst && src);

	if (IS_IMM(dst->kind))
		return 1;

	if (IS_REG(dst->kind) && IS_REG(src->kind)) {
		if (dst->inner.reg == src->inner.reg)
			return 0;
	}

	if (!str_from_value(&dst_str, dst))
		return 1;
	if (!str_from_value(&src_str, src))
		return 1;

	if (!str_realloc(s, 16 + dst_str.len + src_str.len + 1))
		return 1;
	s->len = snprintf(s->s, s->siz, "mov%c %s, %s\n",
			get_inst_suffix(dst->kind),
			src_str.s,
			dst_str.s);

	str_free(&dst_str);
	str_free(&src_str);

	return 0;
}

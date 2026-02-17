/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stdio.h>

#define LIBMCB_STRIP
#include "gen_mov.h"
#include "inst.h"
#include "reg.h"
#include "value.h"

#include "../../err.h"
#include "../../str.h"

int
gen_mov(struct str *s,
		const struct gnu_asm_value *dst,
		const struct gnu_asm_value *src)
{
	int len;
	struct str dst_str, src_str;
	assert(s && dst && src);

	if (IS_IMM(dst->kind))
		ereturn(1, "move to imm");

	if (IS_REG(dst->kind) && IS_REG(src->kind)) {
		if (dst->inner.reg == src->inner.reg)
			return 0;
	}

	str_from_value(&dst_str, dst);
	str_from_value(&src_str, src);

	estr_realloc(s, 16 + dst_str.len + src_str.len + 1);
	len = snprintf(s->s, s->siz, "mov%c %s, %s\n",
			get_inst_suffix(dst->kind),
			src_str.s,
			dst_str.s);
	if (len < 0)
		eabort("snprintf()");
	s->len = len;

	str_free(&dst_str);
	str_free(&src_str);

	return 0;
}

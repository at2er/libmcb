/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include "mcb/func.h"

#define LIBMCB_STRIP
#include "func.h"
#include "gen_mov.h"
#include "gnu_asm.h"
#include "reg.h"
#include "utils.h"
#include "value.h"

#include "../../err.h"
#include "../../str.h"

int
mov_reg_user(enum GNU_ASM_REG reg, struct mcb_func *fn, struct gnu_asm *ctx)
{
	struct gnu_asm_func *f;
	struct gnu_asm_value src;
	struct gnu_asm_value *user;
	assert(fn && ctx);
	f = fn->data;
	assert(f);
	user = f->reg_allocated[reg];
	assert(user);

	assert(IS_REG(user->kind));

	src = *user;
	user->inner.reg = alloc_reg(AUTO_ALLOC_REG, user, fn);
	if (user->inner.reg == REG_COUNT)
		ereturn(1, "alloc_reg()");

	if (!str_clean(&ctx->buf))
		ereturn(1, "str_clean(&ctx->buf)");
	if (gen_mov(&ctx->buf, user, &src))
		ereturn(1, "gen_mov()");
	if (!str_append_str(&ctx->text, &ctx->buf))
		ereturn(1, "str_append_str()");

	return 0;
}

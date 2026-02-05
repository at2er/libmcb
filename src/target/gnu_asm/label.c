/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stdio.h>
#include "mcb/func.h"
#include "mcb/label.h"

#include "label.h"

#include "../../str.h"

int
define_label(struct mcb_label *label,
		struct mcb_func *fn,
		struct gnu_asm *ctx)
{
	assert(label && ctx);

	if (!str_clean(&ctx->buf))
		return 1;

	ctx->buf.len = snprintf(ctx->buf.s, ctx->buf.siz,
			".L%s__%s:\n",
			fn->name,
			label->name);

	if (!str_append_str(&ctx->text, &ctx->buf))
		return 1;

	return 0;
}

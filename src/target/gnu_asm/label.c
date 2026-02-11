/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stdio.h>
#include "mcb/func.h"
#include "mcb/label.h"

#define LIBMCB_STRIP
#include "label.h"

#include "../../err.h"
#include "../../str.h"

void
define_label(struct mcb_label *label,
		struct mcb_func *fn,
		struct gnu_asm *ctx)
{
	int len;
	assert(label && ctx);

	estr_clean(&ctx->buf);
	len = snprintf(ctx->buf.s, ctx->buf.siz,
			".L%s__%s:\n",
			fn->name,
			label->name);
	if (len < 0)
		eabort("snprintf()");
	ctx->buf.len = len;
	estr_append_str(&ctx->text, &ctx->buf);
}

/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	char *unwarped_label = NULL;
	assert(label && ctx);

	estr_clean(&ctx->buf);
	unwarped_label = unwarp_label(label, fn);
	if (!unwarped_label)
		eabort("unwarp_label()");
	estr_append_cstr(&ctx->text, unwarped_label);
	estr_append_cstr(&ctx->text, ":\n");
	free(unwarped_label);
}

char *
unwarp_label(struct mcb_label *label, struct mcb_func *fn)
{
	int len;
	size_t siz;
	char *result;

	assert(label && fn);

	siz = 5 + strlen(fn->name) + strlen(label->name);
	result = malloc(siz);
	len = snprintf(result, siz, ".L%s__%s",
			fn->name, label->name);
	if (len < 0)
		eabort("snprintf()");
	return result;
}

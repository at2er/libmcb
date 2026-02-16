/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "mcb/func.h"
#include "mcb/inst.h"
#include "mcb/inst/branch.h"
#include "mcb/label.h"
#include "mcb/value.h"

#define LIBMCB_STRIP
#include "cmp.h"
#include "gen_mov.h"
#include "gnu_asm.h"
#include "inst.h"
#include "label.h"
#include "value.h"

#include "../../ealloc.h"
#include "../../err.h"
#include "../../str.h"

static const char *jcc_template[] = {
	[MCB_GT] = "jg %s\n",
	[MCB_LE] = "jle %s\n"
};

int
build_branch_inst(struct mcb_inst *inst_outer,
		struct mcb_func *fn,
		struct gnu_asm *ctx)
{
	struct gnu_asm_value *cmp_result;
	struct mcb_branch_inst *inst;
	int len;
	struct mcb_label *on_true, *on_false;
	char *unwarped_label = NULL;

	assert(inst_outer && fn && ctx);
	inst = &inst_outer->inner.branch;
	assert(inst);

	assert(inst->cmp_result);
	cmp_result = inst->cmp_result->data;
	on_true = inst->on_true;
	on_false = inst->on_false;
	assert(cmp_result && on_true && on_false);

	unwarped_label = unwarp_label(on_false, fn);
	if (!unwarped_label)
		eabort("unwarp_label()");

	estr_clean(&ctx->buf);
	len = snprintf(ctx->buf.s, ctx->buf.siz,
			jcc_template[cmp_result->inner.operator],
			unwarped_label);
	free(unwarped_label);
	if (len < 0)
		return 1;
	ctx->buf.len = len;
	estr_append_str(&ctx->text, &ctx->buf);

	return 0;
}

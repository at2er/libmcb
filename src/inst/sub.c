/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stdlib.h>
#include "mcb/func.h"
#include "mcb/inst/sub.h"
#include "mcb/value.h"
#include "utils.h"

int
mcb_inst_sub(struct mcb_value *result,
		struct mcb_value *lhs,
		struct mcb_value *rhs,
		struct mcb_func *fn)
{
	struct mcb_inst *inst;
	if (!result || !lhs || !rhs || !fn)
		return 1;
	inst = calloc(1, sizeof(*inst));
	if (!inst)
		return 1;
	inst->kind = MCB_SUB_INST;
	inst->inner.sub.result = result;
	inst->inner.sub.lhs = lhs;
	inst->inner.sub.rhs = rhs;
	if (mcb_use_value(inst, result))
		return 1;
	if (mcb_use_value(inst, lhs))
		return 1;
	if (mcb_use_value(inst, rhs))
		return 1;
	return mcb_append_inst(inst, fn);
}

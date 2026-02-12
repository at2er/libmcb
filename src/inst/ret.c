/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stdlib.h>
#include "mcb/func.h"
#include "mcb/inst/ret.h"
#include "mcb/value.h"
#include "utils.h"

#include "../ealloc.h"

int
mcb_inst_ret(struct mcb_value *val, struct mcb_func *fn)
{
	struct mcb_inst *inst;
	if (!val || !fn)
		return 1;
	inst = ecalloc(1, sizeof(*inst));
	inst->kind = MCB_RET_INST;
	inst->inner.ret.val = val;
	if (mcb_use_value(inst, val))
		return 1;
	return mcb_append_inst(inst, fn);
}

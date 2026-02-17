/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stdlib.h>
#include "mcb/func.h"
#include "mcb/inst/store.h"
#include "mcb/value.h"
#include "utils.h"

#include "../ealloc.h"

int
mcb_inst_store_int(struct mcb_value *container,
		int64_t data,
		struct mcb_func *fn)
{
	struct mcb_inst *inst;
	if (!container || !fn)
		return 1;
	inst = ecalloc(1, sizeof(*inst));
	inst->kind = MCB_STORE_INST;
	inst->inner.store.container = container;
	inst->inner.store.operand.i = data;
	if (mcb_use_value(inst, container))
		return 1;
	return mcb_append_inst(inst, fn);
}

int
mcb_inst_store_uint(struct mcb_value *container,
		uint64_t data,
		struct mcb_func *fn)
{
	struct mcb_inst *inst;
	if (!container || !fn)
		return 1;
	inst = calloc(1, sizeof(*inst));
	if (!inst)
		return 1;
	inst->kind = MCB_STORE_INST;
	inst->inner.store.container = container;
	inst->inner.store.operand.u = data;
	return mcb_append_inst(inst, fn);
}

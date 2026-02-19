/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stdlib.h>
#include "mcb/func.h"
#include "mcb/inst/alloc_struct.h"
#include "mcb/struct.h"
#include "mcb/value.h"
#include "utils.h"

#include "../ealloc.h"
#include "../err.h"

int
mcb_inst_alloc_struct(
		struct mcb_value *container,
		struct mcb_struct *structure,
		struct mcb_func *fn)
{
	struct mcb_inst *inst;
	if (!container || !structure || !fn)
		ereturn(1, "!container || !structure || !fn");
	container->kind = MCB_STRUCT_VALUE;
	container->inner.structure.structure = structure;
	container->inner.structure.values = ecalloc(
			structure->elems_count,
			sizeof(*container->inner.structure.values));
	inst = ecalloc(1, sizeof(*inst));
	inst->kind = MCB_ALLOC_STRUCT_INST;
	inst->inner.alloc_struct.container = container;
	inst->inner.alloc_struct.structure = structure;
	if (mcb_use_value(inst, container))
		goto err_free_inst;
	if (mcb_append_inst(inst, fn))
		goto err_free_inst;
	return 0;
err_free_inst:
	free(inst);
	return 1;
}

/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stdlib.h>
#include "mcb/func.h"
#include "mcb/inst/alloc_var.h"
#include "mcb/value.h"
#include "utils.h"

#include "../ealloc.h"
#include "../err.h"

int
mcb_inst_alloc_var(struct mcb_value *container, struct mcb_func *fn)
{
	struct mcb_inst *inst;
	if (!container || !fn)
		ereturn(1, "!container || !fn");
	container->kind = MCB_VAR_VALUE;
	inst = ecalloc(1, sizeof(*inst));
	inst->kind = MCB_ALLOC_VAR_INST;
	inst->inner.alloc_var.container = container;
	if (mcb_use_value(inst, container))
		goto err_free_inst;
	if (mcb_append_inst(inst, fn))
		goto err_free_inst;
	return 0;
err_free_inst:
	free(inst);
	return 1;
}

/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "mcb/func.h"
#include "mcb/inst.h"
#include "mcb/value.h"

#define LIBMCB_STRIP
#include "gnu_asm.h"
#include "inst.h"
#include "value.h"

#include "../../str.h"

int
build_store_inst(struct mcb_inst *inst_outer,
		struct mcb_func *fn,
		struct gnu_asm *ctx)
{
	struct mcb_store_inst *inst = &inst_outer->inner.store;
	struct gnu_asm_value *v;
	assert(inst && fn && ctx);
	v = calloc(1, sizeof(*v));
	if (!v)
		return 1;
	v->kind = map_type_to_value_kind(
			I8_IMM_VALUE,
			inst->container->type);
	if (v->kind == UNKOWN_VALUE)
		return 1;
	v->inner.imm.i = inst->operand.i;
	inst->container->data = v;
	return 0;
}

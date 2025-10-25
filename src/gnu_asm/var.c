/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include "mcb/inst/var.h"
#include "mcb/mcb.h"
#include "mcb/target.h"
#include "mem.h"
#include "var.h"

int mcb_gnu_asm_alloc_var(struct mcb_context *context,
			struct mcb_operand *dst,
			const struct mcb_operand *val,
			enum MCB_SIZE size)
{
	MCB_IMPL(mcb_impl_alloc_var(context, dst, val, size));
	if (mcb_gnu_asm_alloc_mem(context, dst, val, size))
		return 1;
	return 0;
}

int mcb_gnu_asm_drop_var(struct mcb_context *context,
			const struct mcb_operand *src)
{
	MCB_IMPL(mcb_impl_drop_var(context, src));
	if (mcb_gnu_asm_drop_mem(context, src->inner.mem))
		return 1;
	return 0;
}

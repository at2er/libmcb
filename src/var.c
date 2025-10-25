/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include "mcb/inst/var.h"
#include "mcb/operand.h"

int mcb_impl_alloc_var(struct mcb_context *context,
			struct mcb_operand *dst,
			const struct mcb_operand *val,
			enum MCB_SIZE size)
{
	dst->size = size;
	return 0;
}

int mcb_impl_drop_var(struct mcb_context *context,
			const struct mcb_operand *src)
{
	if (src->type == MCB_IMM)
		return 1;
	return 0;
}

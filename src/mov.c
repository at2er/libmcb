/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include "mcb/inst/mov.h"

int mcb_impl_mov(struct mcb_context *context,
		const struct mcb_operand *dst,
		const struct mcb_operand *src)
{
	if (src->type >= MCB_OPERAND_TYPE_COUNT)
		return 1;
	if (dst->type == MCB_IMM)
		return 1;
	return 0;
}

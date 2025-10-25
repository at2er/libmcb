/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include "mcb/inst/mem.h"
#include "mcb/operand.h"

int mcb_impl_pop(struct mcb_context *context,
		struct mcb_operand *dst,
		const struct mcb_operand *src)
{
	if (dst->type == MCB_IMM)
		return 1;
	return 0;
}

int mcb_impl_push(struct mcb_context *context,
		struct mcb_operand *dst,
		const struct mcb_operand *src)
{
	if (src->type >= MCB_OPERAND_TYPE_COUNT)
		return 1;
	dst->size = src->size;
	return 0;
}

int mcb_impl_read_mem(struct mcb_context *context,
		struct mcb_operand *dst,
		const struct mcb_operand *src)
{
	if (dst->type == MCB_IMM)
		return 1;
	return 0;
}

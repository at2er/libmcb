/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include "mcb/inst/reg.h"

int mcb_impl_alloc_reg(struct mcb_context *context,
		struct mcb_operand *dst,
		const struct mcb_operand *val,
		enum MCB_SIZE size)
{
	if (!context || !dst)
		return 1;
	if (size >= MCB_SIZE_COUNT)
		return 1;
	dst->size = size;
	dst->type = MCB_REG;
	return 0;
}

int mcb_impl_drop_reg(struct mcb_context *context,
		const struct mcb_operand *src)
{
	if (!context || !src)
		return 1;
	return 0;
}

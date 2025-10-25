/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include "mcb/inst/calc.h"

int mcb_impl_calc(struct mcb_context *context,
		const struct mcb_operand *dst,
		const struct mcb_operand *s0,
		const struct mcb_operand *s1)
{
	if (!dst || !s0 || !s1)
		return 1;
	if (dst->type == MCB_IMM)
		return 1;
	return 0;
}

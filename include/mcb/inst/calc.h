/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_INST_CALC_H
#define LIBMCB_INST_CALC_H
#include "mcb/inst/inst.h"
#include "mcb/operand.h"

struct mcb_context;

/* order: s0 -> s1 -> dst */
_MCB_DEF_INST(mcb_calc_f, mcb_impl_calc,
		struct mcb_context *context,
		const struct mcb_operand *dst,
		const struct mcb_operand *s0,
		const struct mcb_operand *s1);

#endif

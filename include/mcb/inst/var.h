/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_INST_VAR_H
#define LIBMCB_INST_VAR_H
#include "mcb/inst/inst.h"
#include "mcb/operand.h"
#include "mcb/size.h"

struct mcb_context;

/**
 * Allocate a variable with value `val`, and the
 * information of the register store to `dst`.
 * To access the variable, use `dst`.
 */
_MCB_DEF_INST(mcb_alloc_var_f, mcb_impl_alloc_var,
		struct mcb_context *context,
		struct mcb_operand *dst,
		const struct mcb_operand *val,
		enum MCB_SIZE size);
_MCB_DEF_INST(mcb_drop_var_f, mcb_impl_drop_var,
		struct mcb_context *context,
		const struct mcb_operand *src);

#endif

/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_INST_MEM_H
#define LIBMCB_INST_MEM_H
#include "mcb/inst/inst.h"
#include "mcb/operand.h"

struct mcb_context;

_MCB_DEF_INST(mcb_pop_f, mcb_impl_pop,
		struct mcb_context *context,
		struct mcb_operand *dst,
		const struct mcb_operand *src);

_MCB_DEF_INST(mcb_push_f, mcb_impl_push,
		struct mcb_context *context,
		struct mcb_operand *src,
		const struct mcb_operand *val);

_MCB_DEF_INST(mcb_read_mem_f, mcb_impl_read_mem,
		struct mcb_context *context,
		struct mcb_operand *dst,
		const struct mcb_operand *src);

#endif

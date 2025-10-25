/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_INST_MOV_H
#define LIBMCB_INST_MOV_H
#include "mcb/inst/inst.h"
#include "mcb/operand.h"

struct mcb_context;

_MCB_DEF_INST(mcb_mov_f, mcb_impl_mov,
		struct mcb_context *context,
		const struct mcb_operand *dst,
		const struct mcb_operand *src);

#endif

/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_GNU_ASM_MOV_H
#define LIBMCB_GNU_ASM_MOV_H
#include "mcb/inst/mov.h"
#include "mcb/operand.h"

int mcb_gnu_asm_mov(struct mcb_context *context,
		const struct mcb_operand *dst,
		const struct mcb_operand *src);

#endif

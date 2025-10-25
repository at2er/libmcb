/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_GNU_ASM_CALC_H
#define LIBMCB_GNU_ASM_CALC_H
#include "mcb/inst/calc.h"

int mcb_gnu_asm_add(struct mcb_context *context,
		const struct mcb_operand *dst,
		const struct mcb_operand *s0,
		const struct mcb_operand *s1);

int mcb_gnu_asm_div(struct mcb_context *context,
		const struct mcb_operand *dst,
		const struct mcb_operand *s0,
		const struct mcb_operand *s1);

int mcb_gnu_asm_mul(struct mcb_context *context,
		const struct mcb_operand *dst,
		const struct mcb_operand *s0,
		const struct mcb_operand *s1);

int mcb_gnu_asm_sub(struct mcb_context *context,
		const struct mcb_operand *dst,
		const struct mcb_operand *s0,
		const struct mcb_operand *s1);

#endif

/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_GNU_ASM_VAR_H
#define LIBMCB_GNU_ASM_VAR_H
#include "mcb/inst/var.h"

struct mcb_context;

int mcb_gnu_asm_alloc_var(struct mcb_context *context,
			struct mcb_operand *dst,
			const struct mcb_operand *val,
			enum MCB_SIZE size);

int mcb_gnu_asm_drop_var(struct mcb_context *context,
			const struct mcb_operand *src);

#endif

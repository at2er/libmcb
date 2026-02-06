/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_TARGET_GNU_ASM_FUNC_H
#define LIBMCB_TARGET_GNU_ASM_FUNC_H
#include <stdbool.h>
#include "gnu_asm.h"
#include "reg.h"
#include "value.h"

struct gnu_asm_func {
	struct gnu_asm_value *reg_allocated[REG_COUNT];
};

#ifdef LIBMCB_STRIP
#define define_func mcb__gnu_asm_define_func
#endif

int mcb__gnu_asm_define_func(struct mcb_func *fn, struct gnu_asm *ctx);

#endif

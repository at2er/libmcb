/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_TARGET_GNU_ASM_FUNC_H
#define LIBMCB_TARGET_GNU_ASM_FUNC_H
#include <stdbool.h>
#include "gnu_asm.h"
#include "reg.h"

struct gnu_asm_func {
	bool reg_allocated[REG_COUNT];
};

#ifndef LIBMCB_NO_STRIP
#define define_func mcb__gnu_asm_define_func
#endif

int mcb__gnu_asm_define_func(struct mcb_func *fn, struct gnu_asm *ctx);

#endif

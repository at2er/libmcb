/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_TARGET_GNU_ASM_UTILS_H
#define LIBMCB_TARGET_GNU_ASM_UTILS_H
#include "mcb/func.h"

#include "gnu_asm.h"
#include "reg.h"

#ifdef LIBMCB_STRIP
#define mov_reg_user mcb__gnu_asm_mov_reg_user
#endif

int mcb__gnu_asm_mov_reg_user(
		enum GNU_ASM_REG reg,
		struct mcb_func *fn,
		struct gnu_asm *ctx);

#endif

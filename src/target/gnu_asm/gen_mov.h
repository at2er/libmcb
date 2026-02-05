/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_TARGET_GNU_ASM_GEN_MOV_H
#define LIBMCB_TARGET_GNU_ASM_GEN_MOV_H
#include "value.h"
#include "../../str.h"

#ifndef LIBMCB_NO_STRIP
#define gen_mov mcb__gnu_asm_gen_mov
#endif

int mcb__gnu_asm_gen_mov(struct str *s,
		struct gnu_asm_value *dst,
		struct gnu_asm_value *src);

#endif

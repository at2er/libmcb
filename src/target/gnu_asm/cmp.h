/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_TARGET_GNU_ASM_CMP_H
#define LIBMCB_TARGET_GNU_ASM_CMP_H
#include "mcb/inst/cmp.h"

#ifdef LIBMCB_STRIP
#define reverse_cmp_op mcb__gnu_asm_reverse_cmp_op
#endif

enum MCB_CMP_OPERATOR mcb__gnu_asm_reverse_cmp_op(enum MCB_CMP_OPERATOR op);

#endif

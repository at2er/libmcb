/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_GNU_ASM_UTILS_H
#define LIBMCB_GNU_ASM_UTILS_H
#include "mcb/operand.h"
#include "mcb/str.h"

#define OPERAND_BUFSIZ 32

int mcb_gnu_asm_get_operand(struct mcb_str *out, const struct mcb_operand *src);
char mcb_gnu_asm_get_inst_suffix(enum MCB_SIZE size);

#endif

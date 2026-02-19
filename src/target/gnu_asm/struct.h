/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_TARGET_GNU_ASM_STRUCT_H
#define LIBMCB_TARGET_GNU_ASM_STRUCT_H
#include "mcb/struct.h"
#include "mcb/context.h"

#include "gnu_asm.h"

struct gnu_asm_struct_value {
	struct gnu_asm_value **values;
};

#endif

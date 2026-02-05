/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_SRC_TARGET_GNU_ASM_H
#define LIBMCB_SRC_TARGET_GNU_ASM_H
#include <stdio.h>
#include "mcb/context.h"
#include "../../str.h"

struct gnu_asm {
	struct str buf;
	struct mcb_context *ctx;
	FILE *stream;
	struct str text;
};

#endif

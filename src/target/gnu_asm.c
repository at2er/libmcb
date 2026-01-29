/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include "mcb/target/gnu_asm.h"

int mcb_target_gnu_asm(FILE *stream, struct mcb_context *ctx)
{
	for (size_t i = 0; i < ctx->fn_arr_count; i++) {
	}
	return 0;
}

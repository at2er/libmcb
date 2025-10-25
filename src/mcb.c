/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include "gnu_asm.h"
#include "mcb/mcb.h"
#include "mcb/target.h"
#include <assert.h>
#include <memory.h>

const char *mcb_target_assembler = "as";
const char *mcb_target_linker = "ld";

enum MCB_INIT_RESULT mcb_init(struct mcb_context *context, enum MCB_MODE mode)
{
	memset(context, 0, sizeof(*context));

	if (mode >= MCB_MODE_COUNT)
		return MCB_INIT_FAILED_GET_MODE;
	switch (mode) {
	case MCB_MODE_GNU_ASM:
		if (mcb_gnu_asm_init(context))
			return MCB_INIT_FAILED_TARGET_INIT;
		break;
	default: break;
	}
	return MCB_INIT_SUCCESS;
}

int mcb_fini(struct mcb_context *context)
{
	if (MCB_CALL_0(context, fini))
		return 1;
	return 0;
}

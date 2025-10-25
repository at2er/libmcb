/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_GNU_ASM_FUNC_H
#define LIBMCB_GNU_ASM_FUNC_H
#include "mcb/inst/func.h"
#include "mcb/size.h"

int mcb_gnu_asm_call_func(struct mcb_context *context,
		uint16_t argc,
		const struct mcb_operand *args,
		const char *name);

int mcb_gnu_asm_enter_func(struct mcb_context *context,
		enum MCB_ENTER_FUNC_FLAGS flags,
		struct mcb_operand *args[],
		enum MCB_SIZE size,
		const char *name);

int mcb_gnu_asm_exit_func(struct mcb_context *context);

int mcb_gnu_asm_func_ret(struct mcb_context *context,
		const struct mcb_operand *src);

int mcb_gnu_asm_prepare_func_call_arg(struct mcb_context *context,
		uint16_t index,
		struct mcb_operand *dst,
		enum MCB_SIZE size);

#endif

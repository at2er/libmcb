/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include "mcb/inst/syscall.h"
#include "mcb/mcb.h"
#include "mcb/target.h"
#include "syscall.h"

int mcb_gnu_asm_prepare_syscall_arg(struct mcb_context *context,
		uint16_t index,
		struct mcb_operand *dst,
		enum MCB_SIZE size)
{
	return 0;
}

int mcb_gnu_asm_syscall(struct mcb_context *context,
		uint16_t argc,
		const struct mcb_operand *args,
		uint64_t callnum)
{
	MCB_IMPL(mcb_impl_syscall(context, argc, args, callnum));
	return 0;
}

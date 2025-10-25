/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include "mcb/inst/syscall.h"

int mcb_impl_prepare_syscall_arg(struct mcb_context *context,
		uint16_t index,
		struct mcb_operand *dst,
		enum MCB_SIZE size)
{
	return 0;
}

int mcb_impl_syscall(struct mcb_context *context,
		uint16_t argc,
		const struct mcb_operand *args,
		uint64_t callnum)
{
	return 0;
}

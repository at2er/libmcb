/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include "mcb/inst/func.h"
#include "mcb/operand.h"
#include "mcb/size.h"

int mcb_impl_call_func(struct mcb_context *context,
		uint16_t argc,
		const struct mcb_operand *args,
		const char *name)
{
	return 0;
}

int mcb_impl_enter_func(struct mcb_context *context,
		enum MCB_ENTER_FUNC_FLAGS flags,
		struct mcb_operand *args[],
		enum MCB_SIZE size,
		const char *name)
{
	if (flags
			& MCB_ENTER_FUNC_FLAG_EXPORT
			& MCB_ENTER_FUNC_FLAG_EXPORT_WEAK)
		return 1;
	return 0;
}

int mcb_impl_exit_func(struct mcb_context *context)
{
	return 0;
}

int mcb_impl_func_ret(struct mcb_context *context,
		const struct mcb_operand *src)
{
	return 0;
}

int mcb_impl_prepare_func_call_arg(struct mcb_context *context,
		uint16_t index,
		struct mcb_operand *dst,
		enum MCB_SIZE size)
{
	return 0;
}

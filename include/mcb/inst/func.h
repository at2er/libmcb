/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_INST_FUNC_H
#define LIBMCB_INST_FUNC_H
#include "mcb/inst/inst.h"
#include "mcb/operand.h"
#include "mcb/size.h"

struct mcb_context;

enum MCB_ENTER_FUNC_FLAGS {
	MCB_ENTER_FUNC_FLAG_EXPORT = 1,
	MCB_ENTER_FUNC_FLAG_EXPORT_WEAK = 1 << 1
};

_MCB_DEF_INST(mcb_call_func_f, mcb_impl_call_func,
		struct mcb_context *context,
		uint16_t argc,
		const struct mcb_operand *args,
		const char *name);

/**
 * @param args:
 *     Don't free any element in `args` before calling `exit_func`.
 *     But free `args` is allow.
 *     Array of end with 'NULL',
 *     If no arguments, index 0 is `NULL`.
 */
_MCB_DEF_INST(mcb_enter_func_f, mcb_impl_enter_func,
		struct mcb_context *context,
		enum MCB_ENTER_FUNC_FLAGS flags,
		struct mcb_operand *args[],
		enum MCB_SIZE size,
		const char *name);

_MCB_DEF_INST(mcb_exit_func_f, mcb_impl_exit_func,
		struct mcb_context *context);

/**
 * Only generate a function return instruction.
 * Won't exit the function.
 */
_MCB_DEF_INST(mcb_func_ret_f, mcb_impl_func_ret,
		struct mcb_context *context,
		const struct mcb_operand *src);

_MCB_DEF_INST(mcb_prepare_func_call_arg_f,
		mcb_impl_prepare_func_call_arg,
		struct mcb_context *context,
		uint16_t index,
		struct mcb_operand *dst,
		enum MCB_SIZE size);

#endif

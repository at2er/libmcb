/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_INST_SYSCALL_H
#define LIBMCB_INST_SYSCALL_H
#include "mcb/inst/inst.h"
#include "mcb/operand.h"
#include <stdint.h>

struct mcb_context;

_MCB_DEF_INST(mcb_prepare_syscall_arg_f,
		mcb_impl_prepare_syscall_arg,
		struct mcb_context *context,
		uint16_t index,
		struct mcb_operand *dst,
		enum MCB_SIZE size);

_MCB_DEF_INST(mcb_syscall_f, mcb_impl_syscall,
		struct mcb_context *context,
		uint16_t argc,
		const struct mcb_operand *args,
		uint64_t callnum);

#endif

/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_GNU_ASM_SYSCALL_H
#define LIBMCB_GNU_ASM_SYSCALL_H
#include "mcb/mcb.h"
#include "mcb/operand.h"
#include <stdint.h>

int mcb_gnu_asm_prepare_syscall_arg(struct mcb_context *context,
		uint16_t index,
		struct mcb_operand *dst,
		enum MCB_SIZE size);

int mcb_gnu_asm_syscall(struct mcb_context *context,
		uint16_t argc,
		const struct mcb_operand *args,
		uint64_t callnum);

#endif

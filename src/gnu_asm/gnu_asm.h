/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_GNU_ASM_H
#define LIBMCB_GNU_ASM_H
#include "mcb/text_block.h"
#include "mem.h"
#include "reg.h"

#define CUR_CONTEXT(CONTEXT) ((struct mcb_target_gnu_asm*)(CONTEXT)->target)

struct mcb_target_gnu_asm {
	struct mcb_operand **func_arg_operands;
	struct mcb_text_block *func_begin_block;
	struct mem_stat mem_stat;
	enum REG_STAT reg_stats[EBP];
	struct mcb_text_block_root text_block;
};

#endif

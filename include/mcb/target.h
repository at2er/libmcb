/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_TARGET_H
#define LIBMCB_TARGET_H
#include "mcb/inst/calc.h"
#include "mcb/inst/func.h"
#include "mcb/inst/inst.h"
#include "mcb/inst/mem.h"
#include "mcb/inst/mov.h"
#include "mcb/inst/reg.h"
#include "mcb/inst/syscall.h"
#include "mcb/inst/var.h"
#include "mcb/operand.h"
#include "mcb/size.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define MCB_IMPL(SUPER) if (SUPER) return 1

struct mcb_context;

enum MCB_MODE {
	MCB_MODE_GNU_ASM,

	MCB_MODE_COUNT
};

extern const char *mcb_target_assembler; /* mcb.c */
extern const char *mcb_target_linker; /* mcb.c */

/**
 * Assemble code file from `gen_code`, and append
 * the copy of `output` to `context` for `link`.
 */
typedef int (*mcb_assemble_f)(struct mcb_context *context,
		const char *fpath,
		const char *output);

typedef int (*mcb_fini_f)(struct mcb_context *context);

/**
 * Generate the code, like '*.s' and clean up generated code
 * in `context`, so call other instruction generater for
 * another file is safety.
 */
typedef int (*mcb_gen_code_f)(struct mcb_context *context, FILE *output);

typedef int (*mcb_link_f)(struct mcb_context *context, const char *output);

struct mcb_target_functions {
	mcb_fini_f                  fini;
	mcb_calc_f                  add; /* s0 + s1 = dst */
	mcb_alloc_reg_f             alloc_reg;
	mcb_alloc_var_f             alloc_var;
	mcb_assemble_f              assemble;
	mcb_call_func_f             call_func;
	mcb_calc_f                  div; /* s0 / s1 = dst */
	mcb_drop_reg_f              drop_reg;
	mcb_drop_var_f              drop_var;
	mcb_enter_func_f            enter_func;
	mcb_exit_func_f             exit_func;
	mcb_func_ret_f              func_ret;
	mcb_gen_code_f              gen_code;
	mcb_link_f                  link;
	mcb_mov_f                   mov;
	mcb_calc_f                  mul; /* s0 * s1 = dst */
	mcb_read_mem_f              read_mem;
	mcb_pop_f                   pop;
	mcb_prepare_func_call_arg_f prepare_func_call_arg;
	mcb_prepare_syscall_arg_f   prepare_syscall_arg;
	mcb_push_f                  push;
	mcb_calc_f                  sub; /* s0 - s1 = dst */
	mcb_syscall_f               syscall;
};

#endif

/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_TARGET_GNU_ASM_INST_H
#define LIBMCB_TARGET_GNU_ASM_INST_H
#include "mcb/func.h"
#include "mcb/inst.h"

#include "gnu_asm.h"
#include "value.h"

#ifdef LIBMCB_STRIP
#define build_add_inst   mcb__gnu_asm_build_add_inst
#define build_div_inst   mcb__gnu_asm_build_div_inst
#define build_inst       mcb__gnu_asm_build_inst
#define build_mul_inst   mcb__gnu_asm_build_mul_inst
#define build_ret_inst   mcb__gnu_asm_build_ret_inst
#define build_store_inst mcb__gnu_asm_build_store_inst
#define build_sub_inst   mcb__gnu_asm_build_sub_inst
#define get_inst_suffix  mcb__gnu_asm_get_inst_suffix
#endif

int mcb__gnu_asm_build_add_inst(struct mcb_inst *inst_outer,
		struct mcb_func *fn,
		struct gnu_asm *ctx);
int mcb__gnu_asm_build_div_inst(struct mcb_inst *inst_outer,
		struct mcb_func *fn,
		struct gnu_asm *ctx);
int mcb__gnu_asm_build_inst(struct mcb_inst *inst,
		struct mcb_func *fn,
		struct gnu_asm *ctx);
int mcb__gnu_asm_build_mul_inst(struct mcb_inst *inst_outer,
		struct mcb_func *fn,
		struct gnu_asm *ctx);
int mcb__gnu_asm_build_ret_inst(struct mcb_inst *inst_outer,
		struct mcb_func *fn,
		struct gnu_asm *ctx);
int mcb__gnu_asm_build_store_inst(struct mcb_inst *inst_outer,
		struct mcb_func *fn,
		struct gnu_asm *ctx);
int mcb__gnu_asm_build_sub_inst(struct mcb_inst *inst_outer,
		struct mcb_func *fn,
		struct gnu_asm *ctx);
char mcb__gnu_asm_get_inst_suffix(enum GNU_ASM_VALUE_KIND dst_kind);

#endif

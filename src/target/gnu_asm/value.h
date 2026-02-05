/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_TARGET_GNU_ASM_VALUE_H
#define LIBMCB_TARGET_GNU_ASM_VALUE_H
#include "reg.h"
#include "value_kind.h"

union gnu_asm_imm_value {
	int64_t i;
	uint64_t ui;
};

struct gnu_asm_value {
	enum GNU_ASM_VALUE_KIND kind;
	union {
		union gnu_asm_imm_value imm;
		enum GNU_ASM_REG reg;
	} inner;
};

#ifndef LIBMCB_NO_STRIP
#define drop_value             mcb__gnu_asm_drop_value
#define map_type_to_value_kind mcb__gnu_asm_map_type_to_value_kind
#define str_from_value         mcb__gnu_asm_str_from_value
#define str_from_imm           mcb__gnu_asm_str_from_imm
#endif

void mcb__gnu_asm_drop_value(struct mcb_value *val, struct mcb_func *fn);
enum GNU_ASM_VALUE_KIND mcb__gnu_asm_map_type_to_value_kind(
		enum GNU_ASM_VALUE_KIND base,
		enum MCB_TYPE t);
struct str *mcb__gnu_asm_str_from_value(struct str *s, struct gnu_asm_value *v);
struct str *mcb__gnu_asm_str_from_imm(struct str *s, struct gnu_asm_value *v);

#endif

/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_GNU_ASM_MEM_H
#define LIBMCB_GNU_ASM_MEM_H
#include "mcb/inst/mem.h"
#include "mcb/operand.h"
#include "reg.h"
#include <stdbool.h>
#include <stdint.h>

struct mem_elem {
	struct mem_elem *next, *prev;

	enum REG_KIND base;
	enum MCB_BYTES bytes;
	uint64_t offset;
	bool using;
};

struct mem_stat {
	struct mem_elem *begin, *end;
	uint64_t stack_top;
};

int mcb_gnu_asm_alloc_mem(struct mcb_context *context,
		struct mcb_operand *dst,
		const struct mcb_operand *val,
		enum MCB_SIZE size);
void mcb_gnu_asm_clean_mem_stat(struct mem_stat *self);
int mcb_gnu_asm_drop_mem(struct mcb_context *context,
		mcb_mem_elem_t *src);
int mcb_gnu_asm_pop(struct mcb_context *context,
		struct mcb_operand *dst,
		const struct mcb_operand *src);
int mcb_gnu_asm_push(struct mcb_context *context,
		struct mcb_operand *dst,
		const struct mcb_operand *src);
int mcb_gnu_asm_read_mem(struct mcb_context *context,
		struct mcb_operand *dst,
		const struct mcb_operand *src);

#endif

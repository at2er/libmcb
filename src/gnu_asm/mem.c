/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include "gnu_asm.h"
#include "mcb/inst/mem.h"
#include "mcb/mcb.h"
#include "mcb/operand.h"
#include "mcb/target.h"
#include "mcb/utils.h"
#include "mem.h"
#include "mov.h"
#include <assert.h>
#include <stdlib.h>

static void free_mem_elem(struct mem_elem *self);
static struct mem_elem *get_free_mem_elem(
		struct mcb_context *context,
		enum MCB_BYTES bytes);

void free_mem_elem(struct mem_elem *self)
{
	if (!self)
		return;
	free(self);
}

struct mem_elem *get_free_mem_elem(struct mcb_context *context,
		enum MCB_BYTES bytes)
{
	mcb_list_for_each(struct mem_elem, cur,
			CUR_CONTEXT(context)->mem_stat.begin) {
		if (!cur->using && cur->bytes == bytes)
			return cur;
	}
	return NULL;
}

int mcb_gnu_asm_alloc_mem(struct mcb_context *context,
		struct mcb_operand *dst,
		const struct mcb_operand *val,
		enum MCB_SIZE size)
{
	enum MCB_BYTES bytes = mcb_size_to_bytes(size);
	struct mem_elem *elem = get_free_mem_elem(context, bytes);
	if (elem)
		goto init;
	elem = calloc(1, sizeof(*elem));
	elem->bytes = bytes;
	elem->prev = CUR_CONTEXT(context)->mem_stat.end;
	elem->using = true;
	if (CUR_CONTEXT(context)->mem_stat.end)
		CUR_CONTEXT(context)->mem_stat.end->next = elem;
	CUR_CONTEXT(context)->mem_stat.end = elem;
	CUR_CONTEXT(context)->mem_stat.stack_top += bytes;
	elem->offset += CUR_CONTEXT(context)->mem_stat.stack_top;
init:
	dst->inner.mem = elem;
	dst->size = size;
	dst->type = MCB_MEM;
	if (val)
		return mcb_gnu_asm_mov(context, dst, val);
	return 0;
}

void mcb_gnu_asm_clean_mem_stat(struct mem_stat *self)
{
	assert(self);
	mcb_list_for_each(struct mem_elem, cur, self->begin) {
		free_mem_elem(cur);
	}
	self->stack_top = 0;
}

int mcb_gnu_asm_drop_mem(struct mcb_context *context,
		mcb_mem_elem_t *src)
{
	struct mem_elem *elem = src, *last;
	elem->using = false;
	if (elem->next)
		return 0;
	last = CUR_CONTEXT(context)->mem_stat.end;
	CUR_CONTEXT(context)->mem_stat.end = last->prev;
	if (CUR_CONTEXT(context)->mem_stat.end)
		CUR_CONTEXT(context)->mem_stat.end->next = NULL;
	CUR_CONTEXT(context)->mem_stat.stack_top -= elem->bytes;
	free_mem_elem(elem);
	return 0;
}

int mcb_gnu_asm_push(struct mcb_context *context,
		struct mcb_operand *dst,
		const struct mcb_operand *src)
{
	MCB_IMPL(mcb_impl_push(context, dst, src));
	return mcb_gnu_asm_alloc_mem(context, dst, src, src->size);
}

int mcb_gnu_asm_pop(struct mcb_context *context,
		struct mcb_operand *dst,
		const struct mcb_operand *src)
{
	MCB_IMPL(mcb_impl_pop(context, dst, src));
	if (mcb_gnu_asm_drop_mem(context, src->inner.mem))
		return 1;
	return mcb_gnu_asm_mov(context, dst, src);
}

int mcb_gnu_asm_read_mem(struct mcb_context *context,
		struct mcb_operand *dst,
		const struct mcb_operand *src)
{
	MCB_IMPL(mcb_impl_read_mem(context, dst, src));
	return 0;
}

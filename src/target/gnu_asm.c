/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mcb/target/gnu_asm.h"
#include "../str.h"
#include "../macros.h"

#define IS_INT_IMM(VALUE_KIND) \
	((VALUE_KIND) >= I8_IMM_VALUE && \
	 (VALUE_KIND) <= I64_IMM_VALUE)
// #define IS_UINT_IMM(KIND)
// 	((KIND) >= U8_IMM_VALUE &&
// 	 (KIND) <= U64_IMM_VALUE)
#define IS_IMM(VALUE_KIND) \
	(IS_INT_IMM(VALUE_KIND))

#define IS_REG(VALUE_KIND) \
	((VALUE_KIND) >= I8_REG_VALUE && \
	 (VALUE_KIND) <= I64_REG_VALUE)

enum GNU_ASM_VALUE_KIND {
	UNKOWN_VALUE,

	I8_REG_VALUE,
	I16_REG_VALUE,
	I32_REG_VALUE,
	I64_REG_VALUE,

	I8_IMM_VALUE,
	I16_IMM_VALUE,
	I32_IMM_VALUE,
	I64_IMM_VALUE
};

enum GNU_ASM_REG {
	RBP, RSP,
	RAX, RBX, RCX, RDX,
	RDI, RSI,
	R8,  R9,  R10, R11, R12, R13, R14, R15,
	REG_COUNT
};

struct gnu_asm {
	struct str buf;
	struct mcb_context *ctx;
	FILE *stream;
	struct str text;
};

struct gnu_asm_func {
	bool reg_allocated[REG_COUNT];
};

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

static enum GNU_ASM_REG alloc_reg(struct mcb_func *fn);
static int apply_add_inst(struct mcb_inst *inst_outer,
		struct mcb_func *fn,
		struct gnu_asm *ctx);
static int apply_inst(struct mcb_inst *inst,
		struct mcb_func *fn,
		struct gnu_asm *ctx);
static int apply_ret_inst(struct mcb_inst *inst_outer,
		struct mcb_func *fn,
		struct gnu_asm *ctx);
static int apply_store_inst(struct mcb_inst *inst_outer,
		struct mcb_func *fn,
		struct gnu_asm *ctx);
static int def_fn(struct mcb_func *fn, struct gnu_asm *ctx);
static int def_fn_begin(struct mcb_func *fn, struct gnu_asm *ctx);
static int def_label(struct mcb_label *label,
		struct mcb_func *fn,
		struct gnu_asm *ctx);
static void drop_reg(enum GNU_ASM_REG reg, struct mcb_func *fn);
static int gen_mov(struct str *s,
		struct gnu_asm_value *dst,
		struct gnu_asm_value *src);
static char get_inst_suffix(enum GNU_ASM_VALUE_KIND dst_kind);
static enum GNU_ASM_VALUE_KIND map_type_to_value_kind(
		enum GNU_ASM_VALUE_KIND base,
		enum MCB_TYPE t);
static int reg_offset_from_kind(enum GNU_ASM_VALUE_KIND kind);
static struct str *str_from_imm(struct str *s, struct gnu_asm_value *v);
static struct str *str_from_reg(struct str *s, enum GNU_ASM_REG r, int siz);
static struct str *str_from_value(struct str *s, struct gnu_asm_value *v);

static const char *regs[REG_COUNT][4] = {
	[RBP]  = {"%rbp", "%ebp",  "%bp",   "%bpl" },
	[RSP]  = {"%rsp", "%esp",  "%sp",   "%spl" },
	[RAX]  = {"%rax", "%eax",  "%ax",   "%al"  },
	[RBX]  = {"%rbx", "%ebx",  "%bx",   "%bl"  },
	[RCX]  = {"%rcx", "%ecx",  "%cx",   "%cl"  },
	[RDX]  = {"%rdx", "%edx",  "%dx",   "%dl"  },
	[RDI]  = {"%rdi", "%edi",  "%di",   "%dil" },
	[RSI]  = {"%rsi", "%esi",  "%si",   "%sil" },
	[R8 ]  = {"%r8",  "%r8d",  "%r8w",  "%r8b" },
	[R9 ]  = {"%r9",  "%r9d",  "%r9w",  "%r9b" },
	[R10]  = {"%r10", "%r10d", "%r10w", "%r10b"},
	[R11]  = {"%r11", "%r11d", "%r11w", "%r11b"},
	[R12]  = {"%r12", "%r12d", "%r12w", "%r12b"},
	[R13]  = {"%r13", "%r13d", "%r13w", "%r13b"},
	[R14]  = {"%r14", "%r14d", "%r14w", "%r14b"},
	[R15]  = {"%r15", "%r15d", "%r15w", "%r15b"}
};

static const enum GNU_ASM_REG reg_alloc_priority[] = {
	RAX, RBX,
	R10, R11, R12, R13, R14, R15,
	R9,  R8,  RCX, RDX, RSI, RDI
};

enum GNU_ASM_REG
alloc_reg(struct mcb_func *fn)
{
	struct gnu_asm_func *f = fn->data;
	assert(f);
	for (unsigned int i = 0; i < LENGTH(reg_alloc_priority); i++) {
		if (!f->reg_allocated[reg_alloc_priority[i]]) {
			f->reg_allocated[reg_alloc_priority[i]] = true;
			return reg_alloc_priority[i];
		}
	}
	return REG_COUNT;
}

int
apply_add_inst(struct mcb_inst *inst_outer,
		struct mcb_func *fn,
		struct gnu_asm *ctx)
{
	struct str dst, src;
	struct mcb_add_inst *inst = &inst_outer->inner.add;
	struct gnu_asm_value *lhs_val, *rhs_val, *result, *tmp;

	assert(inst && fn && ctx);
	assert(inst->lhs && inst->rhs);
	lhs_val = inst->lhs->data;
	rhs_val = inst->rhs->data;
	assert(lhs_val && rhs_val);

	assert(inst->result->data == NULL);
	result = calloc(1, sizeof(*result));
	if (!result)
		return 1;
	result->kind = map_type_to_value_kind(
			I8_REG_VALUE,
			inst->result->type);
	result->inner.reg = alloc_reg(fn);
	if (result->inner.reg == REG_COUNT)
		return 1;
	inst->result->data = result;

	if (IS_REG(rhs_val->kind) && !IS_REG(lhs_val->kind)
			&& inst->rhs->scope_end == inst_outer) {
		tmp = lhs_val;
		lhs_val = rhs_val;
		rhs_val = tmp;
	}

	if (IS_REG(lhs_val->kind)) {
		if (inst->lhs->scope_end == inst_outer) {
			drop_reg(result->inner.reg, fn);
			free(result);
			result = lhs_val;
			inst->result->data = result;
		}
		if (!str_from_value(&dst, lhs_val))
			return 1;
	}

	if (IS_IMM(lhs_val->kind)) {
		if (!str_clean(&ctx->buf))
			return 1;
		if (gen_mov(&ctx->buf, result, lhs_val))
			return 1;
		if (!str_append_str(&ctx->text, &ctx->buf))
			return 1;
		if (!str_from_value(&dst, result))
			return 1;
	}

	if (!str_from_imm(&src, rhs_val))
		return 1;

	if (!str_clean(&ctx->buf))
		return 1;
	ctx->buf.len = snprintf(ctx->buf.s, ctx->buf.siz,
			"add%c %s, %s\n",
			get_inst_suffix(lhs_val->kind),
			src.s, dst.s);

	if (!str_append_str(&ctx->text, &ctx->buf))
		return 1;

	str_free(&dst);
	str_free(&src);

	return 0;
}

int
apply_inst(struct mcb_inst *inst,
		struct mcb_func *fn,
		struct gnu_asm *ctx)
{
	assert(inst && fn && ctx);

	switch (inst->kind) {
	case MCB_ADD_INST:
		return apply_add_inst(inst, fn, ctx);
	case MCB_RET_INST:
		return apply_ret_inst(inst, fn, ctx);
	case MCB_STORE_INST:
		return apply_store_inst(inst, fn, ctx);
	}

	return 0;
}

int
apply_ret_inst(struct mcb_inst *inst_outer,
		struct mcb_func *fn,
		struct gnu_asm *ctx)
{
	struct gnu_asm_value dst, *src;
	struct mcb_ret_inst *inst = &inst_outer->inner.ret;
	assert(inst && fn && ctx);
	assert(inst->val);
	assert(inst->val->data);
	src = inst->val->data;

	dst.kind = map_type_to_value_kind(
			I8_REG_VALUE,
			inst->val->type);
	dst.inner.reg = RAX;

	if (IS_REG(src->kind) && src->inner.reg == dst.inner.reg)
		goto gen_ret;

	if (!str_clean(&ctx->buf))
		return 1;
	if (gen_mov(&ctx->buf, &dst, src))
		return 1;
	if (!str_append_str(&ctx->text, &ctx->buf))
		return 1;

gen_ret:
	if (!str_append_cstr(&ctx->text, "ret\n"))
		return 1;

	return 0;
}

int
apply_store_inst(struct mcb_inst *inst_outer,
		struct mcb_func *fn,
		struct gnu_asm *ctx)
{
	struct mcb_store_inst *inst = &inst_outer->inner.store;
	struct gnu_asm_value *v;
	assert(inst && fn && ctx);
	v = calloc(1, sizeof(*v));
	if (!v)
		return 1;
	v->kind = map_type_to_value_kind(
			I8_IMM_VALUE,
			inst->container->type);
	if (v->kind == UNKOWN_VALUE)
		return 1;
	v->inner.imm.i = inst->operand.i;
	inst->container->data = v;
	return 0;
}

int
def_fn(struct mcb_func *fn, struct gnu_asm *ctx)
{
	assert(fn && ctx);

	if (def_fn_begin(fn, ctx))
		return 1;
	for (size_t i = 0, label_i = 0; i < fn->inst_arr_count; i++) {
		if (fn->label_arr && label_i < fn->label_arr_count &&
				fn->label_arr[label_i]->beg ==
				fn->inst_arr[i]) {
			if (def_label(fn->label_arr[label_i], fn, ctx))
				return 1;
			label_i++;
		}

		if (apply_inst(fn->inst_arr[i], fn, ctx))
			return 1;
	}
	return 0;
}

int
def_fn_begin(struct mcb_func *fn, struct gnu_asm *ctx)
{
	struct gnu_asm_func *f;
	assert(fn && ctx);

	f = calloc(1, sizeof(*f));
	if (!f)
		return 1;
	fn->data = f;

	if (!str_clean(&ctx->buf))
		return 1;

	if (fn->export_type == MCB_FUNC_EXPORT)
		ctx->buf.len = snprintf(
				ctx->buf.s,
				ctx->buf.siz,
				".globl %s\n", fn->name);

	ctx->buf.len += snprintf(
			&ctx->buf.s[ctx->buf.len],
			ctx->buf.siz - ctx->buf.len,
			"%s:\n", fn->name);

	if (!str_append_str(&ctx->text, &ctx->buf))
		return 1;

	return 0;
}

int
def_label(struct mcb_label *label,
		struct mcb_func *fn,
		struct gnu_asm *ctx)
{
	assert(label && ctx);

	if (!str_clean(&ctx->buf))
		return 1;

	ctx->buf.len = snprintf(ctx->buf.s, ctx->buf.siz,
			".L%s__%s:\n",
			fn->name,
			label->name);

	if (!str_append_str(&ctx->text, &ctx->buf))
		return 1;

	return 0;
}

void
drop_reg(enum GNU_ASM_REG reg, struct mcb_func *fn)
{
	struct gnu_asm_func *f;
	assert(fn);
	f = fn->data;
	f->reg_allocated[reg] = false;
}

int
gen_mov(struct str *s,
		struct gnu_asm_value *dst,
		struct gnu_asm_value *src)
{
	struct str dst_str, src_str;
	assert(s && dst && src);

	if (IS_IMM(dst->kind))
		return 1;

	if (!str_from_value(&dst_str, dst))
		return 1;
	if (!str_from_value(&src_str, src))
		return 1;

	if (!str_realloc(s, 16 + dst_str.len + src_str.len + 1))
		return 1;
	s->len = snprintf(s->s, s->siz, "mov%c %s, %s\n",
			get_inst_suffix(dst->kind),
			src_str.s,
			dst_str.s);

	str_free(&dst_str);
	str_free(&src_str);

	return 0;
}

char
get_inst_suffix(enum GNU_ASM_VALUE_KIND dst_kind)
{
	/* TODO */
	switch (dst_kind) {
	case UNKOWN_VALUE:
		return '\0';

	case I8_IMM_VALUE:
	case I8_REG_VALUE:
		return 'l';
	case I16_IMM_VALUE:
	case I16_REG_VALUE:
		return 'l';

	case I32_IMM_VALUE:
	case I32_REG_VALUE:
		return 'l';
	case I64_IMM_VALUE:
	case I64_REG_VALUE:
		return 'q';
	}
	return '\0';
}

enum GNU_ASM_VALUE_KIND
map_type_to_value_kind(enum GNU_ASM_VALUE_KIND base, enum MCB_TYPE t)
{
	assert(base != UNKOWN_VALUE);
	switch (t) {
	case MCB_U8:  case MCB_I8:  return base;
	case MCB_U16: case MCB_I16: return base + 1;
	case MCB_U32: case MCB_I32: return base + 2;
	case MCB_U64: case MCB_I64: return base + 3;
	}
	return UNKOWN_VALUE;
}

int
reg_offset_from_kind(enum GNU_ASM_VALUE_KIND kind)
{
	switch (kind) {
	case UNKOWN_VALUE:  break;
	case I8_REG_VALUE:  case I8_IMM_VALUE:  return 3;
	case I16_REG_VALUE: case I16_IMM_VALUE: return 2;
	case I32_REG_VALUE: case I32_IMM_VALUE: return 1;
	case I64_REG_VALUE: case I64_IMM_VALUE: return 0;
	}
	return -1;
}

struct str *
str_from_imm(struct str *s, struct gnu_asm_value *v)
{
	int ret = 0;
	assert(s && v);
	if (!str_empty(s))
		return NULL;
	if (!str_realloc(s, 32))
		return NULL;
	if (IS_INT_IMM(v->kind)) {
		ret = snprintf(s->s, s->siz, "$%ld", v->inner.imm.i);
		if (ret < 0)
			return NULL;
		s->len = ret;
		return s;
	}
	return NULL;
}

struct str *
str_from_reg(struct str *s, enum GNU_ASM_REG r, int siz)
{
	assert(s);
	if (!str_empty(s))
		return NULL;
	if (r >= REG_COUNT)
		return NULL;
	if (!str_from_cstr(s, regs[r][siz]))
		return NULL;
	return s;
}

struct str *
str_from_value(struct str *s, struct gnu_asm_value *v)
{
	int reg_off;
	assert(s && v);
	if (IS_IMM(v->kind))
		return str_from_imm(s, v);
	if (IS_REG(v->kind)) {
		reg_off = reg_offset_from_kind(v->kind);
		assert(reg_off != -1);
		return str_from_reg(s, v->inner.reg, reg_off);
	}
	return NULL;
}

int
mcb_target_gnu_asm(FILE *stream, struct mcb_context *ctx)
{
	struct gnu_asm gnu_asm_ctx = {0};
	int ret = 0;

	if (!stream || !ctx)
		return 1;

	if (!str_from_cstr(&gnu_asm_ctx.text, ".text\n"))
		return 1;
	if (!str_empty(&gnu_asm_ctx.buf))
		return 1;
	if (!str_expand_siz(&gnu_asm_ctx.buf, 8192))
		return 1;
	gnu_asm_ctx.ctx = ctx;
	gnu_asm_ctx.stream = stream;

	for (size_t i = 0; i < ctx->fn_arr_count; i++) {
		ret = def_fn(ctx->fn_arr[i], &gnu_asm_ctx);
		if (ret)
			return 1;
	}

	fputs(gnu_asm_ctx.text.s, stream);

	return 0;
}

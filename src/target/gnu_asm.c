/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "mcb/target/gnu_asm.h"
#include "../str.h"

#define IS_INT_IMM(VALUE_KIND) \
	((VALUE_KIND) >= GNU_ASM_I8_IMM_VALUE && \
	 (VALUE_KIND) <= GNU_ASM_I64_IMM_VALUE)
// #define IS_UINT_IMM(KIND)
// 	((KIND) >= GNU_ASM_U8_IMM_VALUE &&
// 	 (KIND) <= GNU_ASM_U64_IMM_VALUE)
#define IS_IMM(VALUE_KIND) \
	(IS_INT_IMM(VALUE_KIND))

#define IS_REG(VALUE_KIND) \
	((VALUE_KIND) >= GNU_ASM_I8_REG_VALUE && \
	 (VALUE_KIND) <= GNU_ASM_I64_REG_VALUE)

enum GNU_ASM_VALUE_KIND {
	GNU_ASM_UNKOWN_VALUE,

	GNU_ASM_I8_REG_VALUE,
	GNU_ASM_I16_REG_VALUE,
	GNU_ASM_I32_REG_VALUE,
	GNU_ASM_I64_REG_VALUE,

	GNU_ASM_I8_IMM_VALUE,
	GNU_ASM_I16_IMM_VALUE,
	GNU_ASM_I32_IMM_VALUE,
	GNU_ASM_I64_IMM_VALUE
};

enum GNU_ASM_REG {
	RBP, RSP,
	RAX, RBX, RCX, RDX,
	RDI, RSI,
	R8,  R9,  R10, R11, R12, R13, R14, R15,

	EBP, ESP,
	EAX, EBX, ECX, EDX,
	EDI, ESI,
	R8D, R9D, R10D, R11D, R12D, R13D, R14D, R15D,

	BP,  SP,
	AX,  BX, CX, DX,
	DI,  SI,
	R8W, R9W, R10W, R11W, R12W, R13W, R14W, R15W,

	BPL, SPL,
	AL,  BL,  CL, DL,
	DIL, SIL,
	R8B, R9B, R10B, R11B, R12B, R13B, R14B, R15B,

	REG_COUNT
};

struct gnu_asm {
	struct str buf;
	struct mcb_context *ctx;
	FILE *stream;
	struct str text;
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

static int apply_add_inst(struct mcb_add_inst *inst,
		struct mcb_func *fn,
		struct gnu_asm *ctx);
static int apply_inst(struct mcb_inst *inst,
		struct mcb_func *fn,
		struct gnu_asm *ctx);
static int apply_ret_inst(struct mcb_ret_inst *inst,
		struct mcb_func *fn,
		struct gnu_asm *ctx);
static int apply_store_inst(struct mcb_store_inst *inst,
		struct mcb_func *fn,
		struct gnu_asm *ctx);
static int def_fn(struct mcb_func *fn, struct gnu_asm *ctx);
static int def_fn_begin(struct mcb_func *fn, struct gnu_asm *ctx);
static int def_label(struct mcb_label *label,
		struct mcb_func *fn,
		struct gnu_asm *ctx);
static int gen_mov(struct str *s,
		struct gnu_asm_value *dst,
		struct gnu_asm_value *src);
static char get_inst_suffix(enum GNU_ASM_VALUE_KIND dst_kind);
static enum GNU_ASM_VALUE_KIND map_type_to_value_kind(
		enum GNU_ASM_VALUE_KIND base,
		enum MCB_TYPE t);
static enum GNU_ASM_REG reg_offset(enum GNU_ASM_REG base,
		enum GNU_ASM_VALUE_KIND kind);
static struct str *str_from_imm(struct str *s, struct gnu_asm_value *v);
static struct str *str_from_reg(struct str *s, enum GNU_ASM_REG r);
static struct str *str_from_value(struct str *s, struct gnu_asm_value *v);

static const char *regs[REG_COUNT] = {
	[RBP]  = "%rbp",  [RSP]  = "%rsp",
	[RAX]  = "%rax",  [RBX]  = "%rbx",  [RCX]  = "%rcx",  [RDX]  = "%rdx",
	[RDI]  = "%rdi",  [RSI]  = "%rsi",
	[R8]   = "%r8",   [R9]   = "%r9",   [R10]  = "%r10",  [R11]  = "%r11",
	[R12]  = "%r12",  [R13]  = "%r13",  [R14]  = "%r14",  [R15]  = "%r15",

	[EBP]  = "%ebp",  [ESP]  = "%esp",
	[EAX]  = "%eax",  [EBX]  = "%ebx",  [ECX]  = "%ecx",  [EDX]  = "%edx",
	[EDI]  = "%edi",  [ESI]  = "%esi",
	[R8D]  = "%r8d",  [R9D]  = "%r9d",  [R10D] = "%r10d", [R11D] = "%r11d",
	[R12D] = "%r12d", [R13D] = "%r13d", [R14D] = "%r14d", [R15D] = "%r15d",

	[BP]   = "%bp",   [SP]   = "%sp",
	[AX]   = "%ax",   [BX]   = "%bx",   [CX]   = "%cx",   [DX]   = "%dx",
	[DI]   = "%di",   [SI]   = "%si",
	[R8W]  = "%r8w",  [R9W]  = "%r9w",  [R10W] = "%r10w", [R11W] = "%r11w",
	[R12W] = "%r12w", [R13W] = "%r13w", [R14W] = "%r14w", [R15W] = "%r15w",

	[BPL]  = "%bpl",  [SPL]  = "%spl",
	[AL]   = "%al",   [BL]   = "%bl",   [CL]   = "%cl",   [DL]   = "%dl",
	[DIL]  = "%dil",  [SIL]  = "%sil",
	[R8B]  = "%r8b",  [R9B]  = "%r9b",  [R10B] = "%r10b", [R11B] = "%r11b",
	[R12B] = "%r12b", [R13B] = "%r13b", [R14B] = "%r14b", [R15B] = "%r15b"
};

int
apply_add_inst(struct mcb_add_inst *inst,
		struct mcb_func *fn,
		struct gnu_asm *ctx)
{
	struct str dst, src;
	struct gnu_asm_value *lhs_val, *rhs_val, *result;

	assert(inst && fn && ctx);
	assert(inst->lhs && inst->rhs);
	lhs_val = inst->lhs->data;
	rhs_val = inst->rhs->data;
	assert(lhs_val && rhs_val);

	assert(inst->result->data == NULL);
	result = calloc(1, sizeof(*result));
	result->kind = map_type_to_value_kind(
			GNU_ASM_I8_REG_VALUE,
			inst->result->type);
	result->inner.reg = reg_offset(RAX, result->kind);
	inst->result->data = result;

	switch (inst->lhs->type) {
	case MCB_U8: case MCB_I8:
	case MCB_U16: case MCB_I16:
	case MCB_U32: case MCB_I32:
	case MCB_U64: case MCB_I64:
		if (!str_clean(&ctx->buf))
			return 1;
		if (gen_mov(&ctx->buf, result, lhs_val))
			return 1;
		if (!str_append_str(&ctx->text, &ctx->buf))
			return 1;
		if (!str_from_value(&dst, result))
			return 1;
		break;
	}

	switch (inst->rhs->type) {
	case MCB_U8: case MCB_I8:
	case MCB_U16: case MCB_I16:
	case MCB_U32: case MCB_I32:
	case MCB_U64: case MCB_I64:
		if (!str_from_imm(&src, rhs_val))
			return 1;
		break;
	}

	if (!str_clean(&ctx->buf))
		return 1;
	ctx->buf.len = snprintf(ctx->buf.s, ctx->buf.siz,
			"add%c %s, %s\n",
			get_inst_suffix(lhs_val->kind),
			src.s,
			dst.s);

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
		return apply_add_inst(&inst->inner.add, fn, ctx);
	case MCB_RET_INST:
		return apply_ret_inst(&inst->inner.ret, fn, ctx);
	case MCB_STORE_INST:
		return apply_store_inst(&inst->inner.store, fn, ctx);
	}

	return 0;
}

int
apply_ret_inst(struct mcb_ret_inst *inst,
		struct mcb_func *fn,
		struct gnu_asm *ctx)
{
	struct gnu_asm_value dst, *src;
	assert(inst && fn && ctx);
	assert(inst->val);
	assert(inst->val->data);
	src = inst->val->data;

	dst.kind = map_type_to_value_kind(
			GNU_ASM_I8_REG_VALUE,
			inst->val->type);
	dst.inner.reg = reg_offset(RAX, src->kind);

	if (IS_REG(src->kind) && src->inner.reg == dst.inner.reg)
		goto gen_ret;

	if (!str_clean(&ctx->buf))
		return 1;
	if (gen_mov(&ctx->buf, &dst, src))
		return 1;
	if (!str_append_str(&ctx->text, &ctx->buf))
		return 1;

gen_ret:
	/* mov ^src, ^dst (%rax)
	 * ret */
	if (!str_append_cstr(&ctx->text, "ret\n"))
		return 1;

	return 0;
}

int
apply_store_inst(struct mcb_store_inst *inst,
		struct mcb_func *fn,
		struct gnu_asm *ctx)
{
	struct gnu_asm_value *v;
	assert(inst && fn && ctx);
	v = calloc(1, sizeof(*v));
	if (!v)
		return 1;
	v->kind = map_type_to_value_kind(
			GNU_ASM_I8_IMM_VALUE,
			inst->container->type);
	if (v->kind == GNU_ASM_UNKOWN_VALUE)
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
	assert(fn && ctx);

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
	case GNU_ASM_UNKOWN_VALUE:
		return '\0';

	case GNU_ASM_I8_IMM_VALUE:
	case GNU_ASM_I8_REG_VALUE:
		return 'l';
	case GNU_ASM_I16_IMM_VALUE:
	case GNU_ASM_I16_REG_VALUE:
		return 'l';

	case GNU_ASM_I32_IMM_VALUE:
	case GNU_ASM_I32_REG_VALUE:
		return 'l';
	case GNU_ASM_I64_IMM_VALUE:
	case GNU_ASM_I64_REG_VALUE:
		return 'q';
	}
	return '\0';
}

enum GNU_ASM_VALUE_KIND
map_type_to_value_kind(enum GNU_ASM_VALUE_KIND base, enum MCB_TYPE t)
{
	assert(base != GNU_ASM_UNKOWN_VALUE);
	switch (t) {
	case MCB_U8:  case MCB_I8:  return base;
	case MCB_U16: case MCB_I16: return base + 1;
	case MCB_U32: case MCB_I32: return base + 2;
	case MCB_U64: case MCB_I64: return base + 3;
	}
	return GNU_ASM_UNKOWN_VALUE;
}

enum GNU_ASM_REG
reg_offset(enum GNU_ASM_REG base, enum GNU_ASM_VALUE_KIND kind)
{
	assert(base != REG_COUNT);
	assert(kind != GNU_ASM_UNKOWN_VALUE);
	switch (kind) {
	case GNU_ASM_I64_REG_VALUE: return base;
	case GNU_ASM_I32_REG_VALUE: return base + EBP;
	case GNU_ASM_I16_REG_VALUE: return base + BP;
	case GNU_ASM_I8_REG_VALUE:  return base + BPL;
	default: break;
	}
	return REG_COUNT;
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
str_from_reg(struct str *s, enum GNU_ASM_REG r)
{
	assert(s);
	if (!str_empty(s))
		return NULL;
	if (r >= REG_COUNT)
		return NULL;
	if (!str_from_cstr(s, regs[r]))
		return NULL;
	return s;
}

struct str *
str_from_value(struct str *s, struct gnu_asm_value *v)
{
	assert(s && v);
	if (IS_IMM(v->kind))
		return str_from_imm(s, v);
	if (IS_REG(v->kind))
		return str_from_reg(s, v->inner.reg);
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

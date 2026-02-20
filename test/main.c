#include <assert.h>
#include <stdio.h>
#include "mcb/array.h"
#include "mcb/context.h"
#include "mcb/func.h"
#include "mcb/inst.h"
#include "mcb/label.h"
#include "mcb/target/gnu_asm.h"
#include "mcb/value.h"

#define UTILSH_EALLOC_IMPL
#include "../src/ealloc.h"

static void define_main_fn(struct mcb_context *ctx);

void
define_main_fn(struct mcb_context *ctx)
{
	/* struct A {
	 *     i:i32
	 *     l:i64
	 * }
	 *
	 * fn _start(%a0:i32)
	 * entry:
	 *     %v0:struct A = alloc_struct
	 *     %v0.i = store 114
	 *     %v0.l = store 514
	 *     %v1:ptr = address_of %v0
	 *     %v2:struct A = load %v1
	 *     %v2.l = store 60
	 *     ret %v2.l
	 */
	struct mcb_func *main_fn =
		mcb_define_func("_start", MCB_I32, MCB_EXPORT_FUNC, ctx);
	struct mcb_func_arg *a0 =
		mcb_define_func_arg("a0", MCB_I32, main_fn);
	assert(a0);

	struct mcb_label *entry = mcb_define_label("entry");
	mcb_append_label(entry, main_fn);

	struct mcb_struct *a_struct = mcb_declare_struct("A", ctx);
	int a_i_idx = mcb_declare_struct_elem("i", MCB_I32, a_struct);
	int a_l_idx = mcb_declare_struct_elem("l", MCB_I64, a_struct);
	assert(1 || a_i_idx);
	assert(1 || a_l_idx);

	struct mcb_value *v0 = mcb_define_struct_value("v0", a_struct, main_fn);
	mcb_inst_alloc_struct(v0, a_struct, main_fn);

	struct mcb_value *v0_i = mcb_get_value_from_struct(v0, a_struct, a_i_idx, main_fn);
	struct mcb_value *v0_l = mcb_get_value_from_struct(v0, a_struct, a_l_idx, main_fn);
	mcb_inst_store_int(v0_i, 114, main_fn);
	mcb_inst_store_int(v0_l, 514, main_fn);

	struct mcb_value *__v = mcb_define_value("__v", MCB_I32, main_fn);
	mcb_inst_add(__v, v0_i, v0_l, main_fn); /* force generate 114 and 514 */

	struct mcb_value *v1 = mcb_define_value("v1", MCB_PTR, main_fn);
	struct mcb_value *v2 = mcb_define_struct_value("v2", a_struct, main_fn);
	mcb_inst_address_of(v1, v0, main_fn);
	mcb_inst_load(v2, v1, main_fn);

	struct mcb_value *v2_l = mcb_get_value_from_struct(v2, a_struct, a_l_idx, main_fn);
	mcb_inst_store_int(v2_l, 60, main_fn);

	mcb_inst_ret(v2_l, main_fn);
}

int
main(void)
{
	struct mcb_context ctx;
	mcb_define_context(&ctx);

	define_main_fn(&ctx);

	// FILE *fp = fopen("/tmp/libmcb_out.s", "w");

	/* output */
	if (mcb_target_gnu_asm(stdout, &ctx))
		return 1;

	// fclose(fp);

	mcb_destory_context(&ctx);
	return 0;
}

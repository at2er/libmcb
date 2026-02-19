#include <assert.h>
#include <stdio.h>
#include "mcb/context.h"
#include "mcb/func.h"
#include "mcb/inst/add.h"
#include "mcb/inst/address_of.h"
#include "mcb/inst/alloc_struct.h"
#include "mcb/inst/alloc_var.h"
#include "mcb/inst/load.h"
#include "mcb/inst/ret.h"
#include "mcb/inst/store.h"
#include "mcb/label.h"
#include "mcb/struct.h"
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
	 * fn main(%a0:i32)
	 * entry:
	 *     %v0:struct A = alloc_struct
	 *     %v0.i = store 114
	 *     ret %v0.i
	 */
	struct mcb_func *main_fn =
		mcb_define_func("main", MCB_I32, MCB_EXPORT_FUNC, ctx);
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

	struct mcb_value *v0 = mcb_define_value("v0", MCB_STRUCT, main_fn);
	mcb_inst_alloc_struct(v0, a_struct, main_fn);

	struct mcb_value *v0_i = mcb_get_value_from_struct(v0, a_i_idx, main_fn);
	struct mcb_value *v0_l = mcb_get_value_from_struct(v0, a_l_idx, main_fn);
	mcb_inst_store_int(v0_i, 114, main_fn);
	mcb_inst_store_int(v0_l, 514, main_fn);

	struct mcb_value *v1 = mcb_define_value("v1", MCB_I32, main_fn);
	mcb_inst_add(v1, v0_i, v0_l, main_fn);

	mcb_inst_ret(v1, main_fn);
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

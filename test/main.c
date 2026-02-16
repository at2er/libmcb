#include <assert.h>
#include <stdio.h>
#include "mcb/context.h"
#include "mcb/func.h"
#include "mcb/inst/add.h"
#include "mcb/inst/branch.h"
#include "mcb/inst/call.h"
#include "mcb/inst/cmp.h"
#include "mcb/inst/ret.h"
#include "mcb/inst/store.h"
#include "mcb/inst/sub.h"
#include "mcb/label.h"
#include "mcb/target/gnu_asm.h"
#include "mcb/value.h"

#define UTILSH_EALLOC_IMPL
#include "../src/ealloc.h"

static struct mcb_func *define_fib_fn(struct mcb_context *ctx);
static void define_main_fn(struct mcb_func *fib_fn, struct mcb_context *ctx);

struct mcb_func *
define_fib_fn(struct mcb_context *ctx)
{
	/* fn fib(%a0:i32): i32
	 * entry:
	 *     %v0:i32     = store 2
	 *     %va0:i32    = func_arg %a0
	 *     %vcmpr0:cmp = cmp %va0, le, %v0
	 *     branch %vcmpr0, if_va0_le_v0, if_va0_le_v0_end
	 * if_va0_le_v0:
	 *     ret %va0
	 * if_va0_le_v0_end:
	 *     %v1:i32     = store 1
	 *     %v2:i32     = sub %va0, %v1
	 *     %vcall0:i32 = call fib, %v2
	 *     %v3:i32     = store 2
	 *     %v4:i32     = sub %va0, %v3
	 *     %vcall1:i32 = call fib, %v4
	 *     %v5:i32     = add %vcall0, %vcall1
	 *     ret %5
	 */
	struct mcb_func *fib_fn =
		mcb_define_func("fib", MCB_I32, MCB_LOCAL_FUNC, ctx);
	struct mcb_func_arg *a0 =
		mcb_define_func_arg("a0", MCB_I32, fib_fn);

	struct mcb_label *entry =
		mcb_define_label("entry");
	mcb_append_label(entry, fib_fn);

	struct mcb_value *v0 =
		mcb_define_value("v0", MCB_I32, fib_fn);
	mcb_inst_store_int(v0, 1, fib_fn);

	struct mcb_value *va0 =
		mcb_define_value_from_func_arg("va0", a0, fib_fn);

	struct mcb_value *vcmpr0 = /* compare result */
		mcb_define_value("vcmpr0", MCB_CMP_RESULT, fib_fn);
	mcb_inst_cmp(vcmpr0, va0, MCB_LE, v0, fib_fn);

	struct mcb_label *if_va0_le_v0 =
		mcb_define_label("if_va0_le_v0");
	struct mcb_label *if_va0_le_v0_end =
		mcb_define_label("if_va0_le_v0_end");
	mcb_inst_branch(vcmpr0, if_va0_le_v0, if_va0_le_v0_end, fib_fn);
	mcb_append_label(if_va0_le_v0, fib_fn);
	mcb_inst_ret(va0, fib_fn);
	mcb_append_label(if_va0_le_v0_end, fib_fn);

	struct mcb_value *v1 =
		mcb_define_value("v1", MCB_I32, fib_fn);
	mcb_inst_store_int(v1, 1, fib_fn);
	struct mcb_value *v2 =
		mcb_define_value("v2", MCB_I32, fib_fn);
	mcb_inst_sub(v2, va0, v1, fib_fn);
	struct mcb_value *vcall0 =
		mcb_define_value("vcall0", MCB_I32, fib_fn);
	mcb_inst_va_call(vcall0, fib_fn, fib_fn, v2);
	
	struct mcb_value *v3 =
		mcb_define_value("v3", MCB_I32, fib_fn);
	mcb_inst_store_int(v3, 2, fib_fn);
	struct mcb_value *v4 =
		mcb_define_value("v4", MCB_I32, fib_fn);
	mcb_inst_sub(v4, va0, v3, fib_fn);
	struct mcb_value *vcall1 =
		mcb_define_value("vcall1", MCB_I32, fib_fn);
	mcb_inst_va_call(vcall1, fib_fn, fib_fn, v4);
	
	struct mcb_value *v5 =
		mcb_define_value("v5", MCB_I32, fib_fn);
	mcb_inst_add(v5, vcall0, vcall1, fib_fn);
	mcb_inst_ret(v5, fib_fn);
	return fib_fn;
}

void
define_main_fn(struct mcb_func *fib_fn, struct mcb_context *ctx)
{
	/* fn main: i32
	 * entry:
	 *     %v0:i32     = store 35
	 *     %vcall0:i32 = call fib, %v0
	 *     ret %vcall0
	 */
	struct mcb_func *main_fn =
		mcb_define_func("main", MCB_I32, MCB_EXPORT_FUNC, ctx);
	struct mcb_func_arg *a0 =
		mcb_define_func_arg("a0", MCB_I32, main_fn);
	assert(a0);

	struct mcb_label *entry =
		mcb_define_label("entry");
	mcb_append_label(entry, main_fn);

	struct mcb_value *va0 =
		mcb_define_value_from_func_arg("va0", a0, fib_fn);
	assert(va0);

	struct mcb_value *v0 =
		mcb_define_value("v0", MCB_I32, main_fn);
	mcb_inst_store_int(v0, 1, main_fn);

	struct mcb_value *vcall0 =
		mcb_define_value("vcall0", MCB_I32, main_fn);
	mcb_inst_va_call(vcall0, fib_fn, main_fn, v0);

	mcb_inst_ret(vcall0, main_fn);
}

int
main(void)
{
	struct mcb_context ctx;
	mcb_define_context(&ctx);

	struct mcb_func *fib_fn = define_fib_fn(&ctx);
	define_main_fn(fib_fn, &ctx);

	FILE *fp = fopen("/tmp/libmcb_out.s", "w");

	/* output */
	if (mcb_target_gnu_asm(fp, &ctx))
		return 1;

	fclose(fp);

	mcb_destory_context(&ctx);
	return 0;
}

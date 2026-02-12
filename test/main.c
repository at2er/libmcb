#include <assert.h>
#include <stdio.h>
#include "mcb/context.h"
#include "mcb/func.h"
#include "mcb/inst/call.h"
#include "mcb/inst/ret.h"
#include "mcb/inst/store.h"
#include "mcb/label.h"
#include "mcb/target/gnu_asm.h"
#include "mcb/value.h"

#define UTILSH_EALLOC_IMPL
#include "../src/ealloc.h"

static void define_main_fn(struct mcb_func *test_fn, struct mcb_context *ctx);
static struct mcb_func *define_test_fn(struct mcb_context *ctx);

void
define_main_fn(struct mcb_func *test_fn, struct mcb_context *ctx)
{
	struct mcb_func *main_fn =
		mcb_define_func("main", MCB_I32, MCB_EXPORT_FUNC, ctx);
	struct mcb_func_arg *a0 =
		mcb_define_func_arg("a0", MCB_I32, main_fn);
	assert(a0);

	struct mcb_label *entry =
		mcb_define_label("entry", main_fn);
	assert(entry);

	struct mcb_value *va0 =
		mcb_define_value_from_func_arg("va0", a0, test_fn);
	assert(va0);

	struct mcb_value *v0 =
		mcb_define_value("v0", MCB_I32, main_fn);
	mcb_inst_store_int(v0, 1, main_fn);

	struct mcb_value *vcall0 =
		mcb_define_value("vcall0", MCB_I32, main_fn);
	mcb_inst_va_call(vcall0, test_fn, main_fn, v0);

	mcb_inst_ret(vcall0, main_fn);
}

struct mcb_func *
define_test_fn(struct mcb_context *ctx)
{
	struct mcb_func *test_fn =
		mcb_define_func("test", MCB_I32, MCB_LOCAL_FUNC, ctx);
	struct mcb_func_arg *a0 =
		mcb_define_func_arg("a0", MCB_I32, test_fn);

	struct mcb_label *entry =
		mcb_define_label("entry", test_fn);
	assert(entry);

	struct mcb_value *va0 =
		mcb_define_value_from_func_arg("va0", a0, test_fn);

	mcb_inst_ret(va0, test_fn);
	return test_fn;
}

int
main(void)
{
	/*
	 * fn test(%a0: i32): i32
	 * entry:
	 * %va0:i32 = from arg %a0
	 * ret %va0
	 *
	 * fn main(%a0: i32): i32
	 * entry:
	 * %v0:i32 = store 1145
	 * call test, %v0
	 */

	struct mcb_context ctx;
	mcb_define_context(&ctx);

	struct mcb_func *test_fn = define_test_fn(&ctx);
	define_main_fn(test_fn, &ctx);

	/* output */
	if (mcb_target_gnu_asm(stdout, &ctx))
		return 1;
	mcb_destory_context(&ctx);
	return 0;
}

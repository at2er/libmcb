#include <assert.h>
#include <stdio.h>
#include "mcb/context.h"
#include "mcb/func.h"
#include "mcb/inst/address_of.h"
#include "mcb/inst/alloc_var.h"
#include "mcb/inst/ret.h"
#include "mcb/inst/store.h"
#include "mcb/label.h"
#include "mcb/target/gnu_asm.h"
#include "mcb/value.h"

#define UTILSH_EALLOC_IMPL
#include "../src/ealloc.h"

static void define_main_fn(struct mcb_context *ctx);

void
define_main_fn(struct mcb_context *ctx)
{
	struct mcb_func *main_fn =
		mcb_define_func("main", MCB_I32, MCB_EXPORT_FUNC, ctx);
	struct mcb_func_arg *a0 =
		mcb_define_func_arg("a0", MCB_I32, main_fn);
	assert(a0);

	struct mcb_label *entry = mcb_define_label("entry");
	mcb_append_label(entry, main_fn);

	struct mcb_value *v0 = mcb_define_value("v0", MCB_I32, main_fn);
	struct mcb_value *v1 = mcb_define_value("v1", MCB_PTR, main_fn);
	struct mcb_value *v2 = mcb_define_value("v2", MCB_I32, main_fn);

	mcb_inst_alloc_var(v0, main_fn);
	mcb_inst_store_int(v0, 1145, main_fn);
	mcb_inst_address_of(v1, v0, main_fn);
	// mcb_inst_load(v2, v1, main_fn);

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

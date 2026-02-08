#include <stdio.h>
#include "mcb/context.h"
#include "mcb/func.h"
#include "mcb/inst/div.h"
#include "mcb/inst/ret.h"
#include "mcb/inst/store.h"
#include "mcb/label.h"
#include "mcb/target/gnu_asm.h"
#include "mcb/value.h"

int
main(void)
{
	/*
	 * fn main(%a0: i32): i32
	 * entry:
	 *  RAX       ^ %v0:i32 = store $4 ; but register of %v0 is allocated by div
	 *  RBX    ^  | %v1:i32 = store $2
	 *  RDX ^  |  | %v2:i32 = NULL
	 *      |  #  | %v3:i32 = div %v2, %v0, %v1 <- %v1
	 *      v  v  | %v4:i32 = div %v2, %v0, %v3 <- %v0, %v2, %v3 ; swap %rax, %rbx
	 *            v ret %v4                     <- %v4
	 *
	 * ==> 1
	 */

	struct mcb_context ctx;
	mcb_define_context(&ctx);

	struct mcb_func main_fn;
	struct mcb_func_arg a0;
	mcb_define_func(&main_fn, "main", MCB_I32, MCB_FUNC_EXPORT, &ctx);
	mcb_define_func_arg(&a0, "a0", MCB_I32, &main_fn);

	struct mcb_label entry;
	mcb_define_label(&entry, "entry", &main_fn);

	struct mcb_value v0;
	mcb_define_value(&v0, "v0", MCB_I32, &main_fn);
	mcb_inst_store_int(&v0, 4, &main_fn);

	struct mcb_value v1;
	mcb_define_value(&v1, "v1", MCB_I32, &main_fn);
	mcb_inst_store_int(&v1, 2, &main_fn);

	struct mcb_value v2;
	mcb_define_value(&v2, "v2", MCB_I32, &main_fn);

	struct mcb_value v3;
	mcb_define_value(&v3, "v3", MCB_I32, &main_fn);
	mcb_inst_div(&v3, &v2, &v0, &v1, &main_fn);

	struct mcb_value v4;
	mcb_define_value(&v4, "v4", MCB_I32, &main_fn);
	mcb_inst_div(&v4, &v2, &v0, &v3, &main_fn);

	mcb_inst_ret(&v4, &main_fn);

	/* output */
	if (mcb_target_gnu_asm(stdout, &ctx))
		return 1;
	mcb_destory_context(&ctx);
	return 0;
}

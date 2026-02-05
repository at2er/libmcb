#include <stdio.h>
#include "mcb/context.h"
#include "mcb/func.h"
#include "mcb/inst/add.h"
#include "mcb/inst/ret.h"
#include "mcb/inst/store.h"
#include "mcb/inst/sub.h"
#include "mcb/label.h"
#include "mcb/target/gnu_asm.h"
#include "mcb/value.h"

int
main(void)
{
	/*
	 *
	 * int main() {
	 *     return 1 + 2 + 3;
	 * }
	 *
	 * // I don't know how to draw...
	 * fn main(%a0: i32): i32
	 * entry:
	 *         %v0:i32 = store $1
	 *         %v1:i32 = store $2
	 *    RAX^ %v2:i32 = add %v0, %v1 <- %v0
	 *   RBX^v %v3:i32 = sub %v1, %v2 <- %v1, %v2
	 *      |  %v4:i32 = store $1
	 *      |  %v5:i32 = store $2
	 *  RAX-+> %v6:i32 = add %v4, %v5 <- %v4, %v5, %v6
	 *      v  ret %v3                <- %v4
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
	mcb_inst_store_int(&v0, 1, &main_fn);

	struct mcb_value v1;
	mcb_define_value(&v1, "v1", MCB_I32, &main_fn);
	mcb_inst_store_int(&v1, 2, &main_fn);

	struct mcb_value v2;
	mcb_define_value(&v2, "v2", MCB_I32, &main_fn);
	mcb_inst_add(&v2, &v0, &v1, &main_fn);

	struct mcb_value v3;
	mcb_define_value(&v3, "v3", MCB_I32, &main_fn);
	mcb_inst_sub(&v3, &v1, &v2, &main_fn);

	struct mcb_value v4;
	mcb_define_value(&v4, "v4", MCB_I32, &main_fn);
	mcb_inst_store_int(&v4, 1, &main_fn);

	struct mcb_value v5;
	mcb_define_value(&v5, "v5", MCB_I32, &main_fn);
	mcb_inst_store_int(&v5, 2, &main_fn);

	struct mcb_value v6;
	mcb_define_value(&v6, "v6", MCB_I32, &main_fn);
	mcb_inst_add(&v6, &v4, &v5, &main_fn);

	mcb_inst_ret(&v3, &main_fn);

	/* output */
	if (mcb_target_gnu_asm(stdout, &ctx))
		return 1;
	mcb_destory_context(&ctx);
	return 0;
}

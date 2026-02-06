#include <stdio.h>
#include "mcb/context.h"
#include "mcb/func.h"
#include "mcb/inst/add.h"
#include "mcb/inst/mul.h"
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
	 *         %v0:i32 = store $1
	 *         %v1:i32 = store $2
	 *  +-RAX^ %v2:i32 = mul %v0, %v1 <- %v0, %v1
	 * RBX   v ret %v2                <- %v2
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
	mcb_inst_mul(&v3, &v0, &v1, &main_fn);

	mcb_inst_ret(&v3, &main_fn);

	/*
	 *                .text
	 *                .globl main
	 *            main:
	 *            .Lmain__entry:
	 *                &enter add
	 *   RAX +----+       movl $1, %eax
	 *       |    |       addl $2, %eax
	 *       |    |   &enter imul
	 *       | RBX#       movl %eax, %ebx ; swap %eax, %ebx
	 *    RAX#    |       movl $1, %eax
	 *  r10d^|    |       movl $2, %r10d
	 *      ||    |       imull %r10d
	 *      vv    |       ret
	 *            |   movl %ebx, %eax
	 *            v   ret
	 */

	/* output */
	if (mcb_target_gnu_asm(stdout, &ctx))
		return 1;
	mcb_destory_context(&ctx);
	return 0;
}

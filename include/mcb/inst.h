/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_INST_H
#define LIBMCB_INST_H
#include "mcb/inst/add.h"
#include "mcb/inst/call.h"
#include "mcb/inst/cmp.h"
#include "mcb/inst/div.h"
#include "mcb/inst/mul.h"
#include "mcb/inst/ret.h"
#include "mcb/inst/store.h"
#include "mcb/inst/sub.h"

enum MCB_INST_KIND {
	MCB_ADD_INST,
	MCB_CALL_INST,
	MCB_CMP_INST,
	MCB_DIV_INST,
	MCB_MUL_INST,
	MCB_RET_INST,
	MCB_STORE_INST,
	MCB_SUB_INST
};

struct mcb_inst {
	enum MCB_INST_KIND kind;
	union {
		struct mcb_add_inst   add;
		struct mcb_call_inst  call;
		struct mcb_cmp_inst   cmp;
		struct mcb_div_inst   div;
		struct mcb_mul_inst   mul;
		struct mcb_ret_inst   ret;
		struct mcb_store_inst store;
		struct mcb_sub_inst   sub;
	} inner;
};

#endif

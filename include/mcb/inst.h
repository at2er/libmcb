/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_INST_H
#define LIBMCB_INST_H
#include <stdint.h>

enum MCB_INST_KIND {
	MCB_ADD_INST,
	MCB_MUL_INST,
	MCB_RET_INST,
	MCB_STORE_INST,
	MCB_SUB_INST
};

struct mcb_inst {
	enum MCB_INST_KIND kind;
	union {
		struct mcb_add_inst {
			struct mcb_value *result;
			struct mcb_value *lhs, *rhs;
		} add;
		struct mcb_mul_inst {
			struct mcb_value *result;
			struct mcb_value *lhs, *rhs;
		} mul;
		struct mcb_ret_inst {
			struct mcb_value *val;
		} ret;
		struct mcb_store_inst {
			struct mcb_value *container;
			union {int64_t i; uint64_t u;} operand;
		} store;
		struct mcb_sub_inst {
			struct mcb_value *result;
			struct mcb_value *lhs, *rhs;
		} sub;
	} inner;
};

#endif

/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_INST_H
#define LIBMCB_INST_H
#include <stdint.h>

enum MCB_INST_KIND {
	MCB_ADD_INST,
	MCB_RET_INST,
	MCB_STORE_INST
};

struct mcb_inst {
	enum MCB_INST_KIND kind;
	union {
		struct mcb_add_inst {
			struct mcb_value *result;
			struct mcb_value *lhs, *rhs;
		} add;
		struct mcb_ret_inst {
			struct mcb_value *val;
		} ret;
		struct mcb_store_inst {
			struct mcb_value *container;
			union {int64_t i; uint64_t u;} operand;
		} store;
	} inner;
};

void mcb_destory_inst(struct mcb_inst *inst);
void mcb_destory_add_inst(struct mcb_add_inst *inst);
void mcb_destory_ret_inst(struct mcb_ret_inst *inst);
void mcb_destory_store_inst(struct mcb_store_inst *inst);

#endif

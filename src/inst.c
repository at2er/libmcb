/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include "mcb/inst.h"

void mcb_destory_inst(struct mcb_inst *inst)
{
	switch (inst->kind) {
	case MCB_ADD_INST:
		// mcb_destory_add_inst(&inst->inner.add);
		break;
	case MCB_RET_INST:
		// mcb_destory_ret_inst(&inst->inner.ret);
		break;
	case MCB_STORE_INST:
		// mcb_destory_store_inst(&inst->inner.store);
		break;
	}
}

// void mcb_destory_add_inst(struct mcb_add_inst *inst)
// {
// }
//
// void mcb_destory_ret_inst(struct mcb_ret_inst *inst)
// {
// }
//
// void mcb_destory_store_inst(struct mcb_store_inst *inst)
// {
// }

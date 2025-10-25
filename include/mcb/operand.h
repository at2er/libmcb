/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_OPERAND_H
#define LIBMCB_OPERAND_H
#include "mcb/size.h"
#include <stdint.h>

typedef void mcb_mem_elem_t;

enum MCB_OPERAND_TYPE {
	MCB_IMM,
	MCB_MEM,
	MCB_REG,
	MCB_OPERAND_TYPE_COUNT
};

struct mcb_operand {
	union mcb_operand_inner {
		void *data;
		int64_t sint;
		uint64_t uint, reg;
		/* different memory element implement in backend */
		mcb_mem_elem_t *mem;
	} inner;
	enum MCB_SIZE size;
	enum MCB_OPERAND_TYPE type;
};

#endif

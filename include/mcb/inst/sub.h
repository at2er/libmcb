/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_INST_SUB_H
#define LIBMCB_INST_SUB_H
#include "mcb/func.h"
#include "mcb/value.h"

int mcb_inst_sub(struct mcb_value *result,
		struct mcb_value *lhs,
		struct mcb_value *rhs,
		struct mcb_func *fn);

#endif

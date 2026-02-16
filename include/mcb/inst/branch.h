/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_INST_BRANCH_H
#define LIBMCB_INST_BRANCH_H
#include "mcb/func.h"
#include "mcb/label.h"
#include "mcb/value.h"

struct mcb_branch_inst {
	struct mcb_value *cmp_result;
	struct mcb_label *on_true, *on_false;
};

int mcb_inst_branch(struct mcb_value *cmp_result,
		struct mcb_label *on_true,
		struct mcb_label *on_false,
		struct mcb_func *fn);

#endif

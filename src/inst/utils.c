/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include "mcb/func.h"
#include "mcb/inst.h"
#include "mcb/label.h"
#include "utils.h"
#include "../darr.h"

int mcb_append_inst(struct mcb_inst *inst, struct mcb_func *fn)
{
	struct mcb_label *last_label = NULL;
	if (!inst || !fn)
		return 1;
	if (fn->label_arr_count)
		last_label = fn->label_arr[fn->label_arr_count - 1];
	if (last_label && last_label->beg == NULL)
		last_label->beg = last_label->end = inst;
	darr_append(fn->inst_arr, fn->inst_arr_count, inst);
	return 0;
}

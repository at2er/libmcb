/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <string.h>
#include "darr.h"
#include "mcb/func.h"
#include "mcb/value.h"

int
mcb_define_value(struct mcb_value *val,
		const char *name,
		enum MCB_TYPE type,
		struct mcb_func *fn)
{
	if (!val || !name || !fn)
		return 1;
	memset(val, 0, sizeof(*val));
	val->name = strdup(name);
	if (!val->name)
		return 1;
	val->type = type;
	darr_append(fn->value_arr, fn->value_arr_count, val);
	return 0;
}

void
mcb_destory_value(struct mcb_value *val)
{
	if (!val)
		return;
	free(val->name);
}

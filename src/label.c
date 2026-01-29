/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <string.h>
#include "darr.h"
#include "mcb/func.h"
#include "mcb/label.h"

int mcb_define_label(struct mcb_label *l,
		const char *name,
		struct mcb_func *fn)
{
	assert(l && name && fn);
	memset(l, 0, sizeof(*l));
	l->name = strdup(name);
	if (!l->name)
		return 1;
	darr_append(fn->label_arr, fn->label_arr_count, l);
	return 0;
}

void mcb_destory_label(struct mcb_label *l)
{
	free(l->name);
}

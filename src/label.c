/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <string.h>
#include "mcb/func.h"
#include "mcb/label.h"

#include "darr.h"
#include "ealloc.h"
#include "err.h"

struct mcb_label *
mcb_define_label(const char *name, struct mcb_func *fn)
{
	struct mcb_label *l;
	if (!name || !fn)
		ereturn(NULL, "!name || !fn");
	l = ecalloc(1, sizeof(*l));
	l->name = strdup(name);
	if (!l->name)
		goto err_free_l;
	darr_append(fn->label_arr, fn->label_arr_count, l);
	return l;
err_free_l:
	free(l);
	return NULL;
}

void
mcb_destory_label(struct mcb_label *l)
{
	if (!l)
		return;
	free(l->name);
}

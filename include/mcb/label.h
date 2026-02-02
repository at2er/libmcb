/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_LABEL_H
#define LIBMCB_LABEL_H
#include "mcb/inst.h"

struct mcb_func;

struct mcb_label {
	char *name;

	struct mcb_inst *beg;
};

int mcb_define_label(struct mcb_label *l,
		const char *name,
		struct mcb_func *fn);

void mcb_destory_label(struct mcb_label *l);

#endif

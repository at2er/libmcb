/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_INST_ALLOC_VAR_H
#define LIBMCB_INST_ALLOC_VAR_H
#include "mcb/func.h"
#include "mcb/value.h"

struct mcb_alloc_var_inst {
	struct mcb_value *container;
};

int mcb_inst_alloc_var(
		struct mcb_value *container,
		struct mcb_func *fn);

#endif

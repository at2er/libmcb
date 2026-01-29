/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_INST_STORE_H
#define LIBMCB_INST_STORE_H
#include <stdint.h>
#include "mcb/func.h"
#include "mcb/value.h"

int mcb_inst_store_int(struct mcb_value *container,
		int64_t data,
		struct mcb_func *fn);

int mcb_inst_store_uint(struct mcb_value *container,
		uint64_t data,
		struct mcb_func *fn);

#endif

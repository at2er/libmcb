/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_INST_ADDRESS_OF_H
#define LIBMCB_INST_ADDRESS_OF_H
#include "mcb/func.h"
#include "mcb/value.h"

struct mcb_address_of_inst {
	struct mcb_value *result;
	struct mcb_value *val;
};

int mcb_inst_address_of(
		struct mcb_value *result,
		struct mcb_value *val,
		struct mcb_func *fn);

#endif

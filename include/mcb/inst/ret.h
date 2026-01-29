/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_INST_RET_H
#define LIBMCB_INST_RET_H
#include "mcb/func.h"
#include "mcb/value.h"

int mcb_inst_ret(struct mcb_value *val, struct mcb_func *fn);

#endif

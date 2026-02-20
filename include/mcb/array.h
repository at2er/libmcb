/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_ARRAY_H
#define LIBMCB_ARRAY_H
#include <stddef.h>
#include "type.h"
#include "value.h"

struct mcb_value *mcb_define_array(
		const char *name,
		enum MCB_TYPE elem_type,
		size_t size,
		struct mcb_func *fn);

struct mcb_value *mcb_get_value_from_array(
		struct mcb_value *container,
		size_t idx,
		struct mcb_func *fn);

#endif

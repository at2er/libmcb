/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_VALUE_H
#define LIBMCB_VALUE_H
#include "type.h"

struct mcb_func;

struct mcb_value {
	char *name;
	enum MCB_TYPE type;

	struct mcb_inst *scope_end;

	void *data;
};

int mcb_define_value(struct mcb_value *val,
		const char *name,
		enum MCB_TYPE type,
		struct mcb_func *fn);

void mcb_destory_value(struct mcb_value *val);

#endif

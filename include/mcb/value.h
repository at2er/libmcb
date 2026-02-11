/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_VALUE_H
#define LIBMCB_VALUE_H
#include "type.h"

struct mcb_func;
struct mcb_func_arg;

enum MCB_VALUE_KIND {
	MCB_NORMAL_VALUE,
	MCB_FUNC_ARG_VALUE
};

struct mcb_value {
	char *name;
	enum MCB_VALUE_KIND kind;
	enum MCB_TYPE type;

	struct mcb_inst *scope_end;

	union {
		struct mcb_func_arg *func_arg;
	} inner;

	void *data;
};

struct mcb_value *mcb_define_value(
		const char *name,
		enum MCB_TYPE type,
		struct mcb_func *fn);

struct mcb_value *mcb_define_value_from_func_arg(
		const char *name,
		struct mcb_func_arg *func_arg,
		struct mcb_func *fn);

void mcb_destory_value(struct mcb_value *val);

#endif

/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_FUNC_H
#define LIBMCB_FUNC_H
#include <stddef.h>
#include "mcb/label.h"
#include "mcb/type.h"
#include "mcb/value.h"

struct mcb_context;

struct mcb_func_arg {
	char *name;

	enum MCB_TYPE type;
};

struct mcb_func {
	int argc;
	struct mcb_func_arg **args;

	char *name;

	enum MCB_TYPE type;

	struct mcb_inst **inst_arr;
	size_t inst_arr_count;

	struct mcb_label **label_arr;
	size_t label_arr_count;

	struct mcb_value **value_arr;
	size_t value_arr_count;
};

int mcb_define_func(struct mcb_func *fn,
		const char *name,
		enum MCB_TYPE type,
		struct mcb_context *ctx);

int mcb_define_func_arg(struct mcb_func_arg *arg,
		const char *name,
		enum MCB_TYPE type,
		struct mcb_func *fn);

void mcb_destory_func(struct mcb_func *fn);
void mcb_destory_func_arg(struct mcb_func_arg *arg);

#endif

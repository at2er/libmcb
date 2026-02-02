/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "darr.h"
#include "mcb/context.h"
#include "mcb/func.h"

int
mcb_define_func(struct mcb_func *fn,
		const char *name,
		enum MCB_TYPE type,
		enum MCB_FUNC_EXPORT_TYPE export_type,
		struct mcb_context *ctx)
{
	if (!fn || !name || !ctx)
		return 1;
	memset(fn, 0, sizeof(*fn));
	fn->name = strdup(name);
	if (!fn->name)
		return 1;
	fn->export_type = export_type;
	fn->type = type;
	darr_append(ctx->fn_arr, ctx->fn_arr_count, fn);
	return 0;
}

int
mcb_define_func_arg(struct mcb_func_arg *arg,
		const char *name,
		enum MCB_TYPE type,
		struct mcb_func *fn)
{
	if (!arg || !name || !fn)
		return 1;
	memset(arg, 0, sizeof(*arg));
	arg->name = strdup(name);
	if (!arg->name)
		return 1;
	arg->type = type;
	darr_append(fn->args, fn->argc, arg);
	return 0;
}

void
mcb_destory_func(struct mcb_func *fn)
{
	if (!fn)
		return;
	for (int i = 0; i < fn->argc; i++)
		mcb_destory_func_arg(fn->args[i]);
	free(fn->args);

	for (size_t i = 0; i < fn->inst_arr_count; i++)
		mcb_destory_inst(fn->inst_arr[i]);
	free(fn->inst_arr);

	for (size_t i = 0; i < fn->label_arr_count; i++)
		mcb_destory_label(fn->label_arr[i]);
	free(fn->label_arr);

	for (size_t i = 0; i < fn->value_arr_count; i++)
		mcb_destory_value(fn->value_arr[i]);
	free(fn->value_arr);
}

void
mcb_destory_func_arg(struct mcb_func_arg *arg)
{
	if (!arg)
		return;
	free(arg->name);
}

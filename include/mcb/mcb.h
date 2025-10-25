/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_H
#define LIBMCB_H
#include "mcb/target.h"
#include <stddef.h>
#include <stdint.h>

#define MCB_CALL(CONTEXT, FUNC, ...) \
	MCB_FUNC((CONTEXT), FUNC)((CONTEXT), __VA_ARGS__)

#define MCB_CALL_0(CONTEXT, FUNC) \
	MCB_FUNC((CONTEXT), FUNC)((CONTEXT))

#define MCB_FUNC(CONTEXT, FUNC) \
	(CONTEXT)->cur_target_functions->FUNC

/* The caller of the library should not modify its contents. */
struct mcb_context {
	const struct mcb_target_functions *cur_target_functions;
	void *target;

	const char **assembled_files;
	size_t assembled_files_count;
};

enum MCB_INIT_RESULT {
	MCB_INIT_SUCCESS,

	MCB_INIT_FAILED_GET_MODE,
	MCB_INIT_FAILED_TARGET_INIT
};

enum MCB_INIT_RESULT mcb_init(struct mcb_context *context, enum MCB_MODE mode);

int mcb_fini(struct mcb_context *context);

#endif

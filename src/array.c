/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "mcb/array.h"
#include "mcb/context.h"
#include "mcb/value.h"

#include "darr.h"
#include "ealloc.h"
#include "err.h"

struct mcb_value *
mcb_define_array(const char *name,
		enum MCB_TYPE elem_type,
		size_t size,
		struct mcb_func *fn)
{
	struct mcb_array_value *array;
	struct mcb_value *result;
	if (!name || !fn)
		ereturn(NULL, "!name || !fn");
	result = mcb_define_value(name, MCB_ARRAY, fn);
	result->kind = MCB_ARRAY_VALUE;
	array = &result->inner.array;
	array->elems = ecalloc(size, sizeof(*array->elems));
	for (size_t i = 0; i < size; i++) {
		array->elems[i] = ecalloc(1, sizeof(*array->elems[0]));
		array->elems[i]->inner.array_elem.array_container = result;
		array->elems[i]->kind = MCB_ARRAY_ELEM_VALUE;
		array->elems[i]->type = elem_type;
	}
	array->size = size;
	return result;
}

struct mcb_value *
mcb_get_value_from_array(
		struct mcb_value *container,
		size_t idx,
		struct mcb_func *fn)
{
	if (!container || !fn)
		ereturn(NULL, "!container || !fn");
	if (container->kind != MCB_ARRAY_VALUE)
		ereturn(NULL, "container not array");
	if (!container->inner.array.elems)
		ereturn(NULL, "array container not has elements");
	if (idx >= container->inner.array.size)
		ereturn(NULL, "element not in array");
	return container->inner.array.elems[idx];
}

/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include "mcb/size.h"
#include <assert.h>

enum MCB_BYTES mcb_size_to_bytes(enum MCB_SIZE size)
{
	assert(size < MCB_SIZE_COUNT);
	switch (size) {
	case MCB_I8:  case MCB_U8:  return MCB_BYTE_1; break;
	case MCB_I16: case MCB_U16: return MCB_BYTE_2; break;
	case MCB_I32: case MCB_U32: return MCB_BYTE_4; break;
	case MCB_I64: case MCB_U64: return MCB_BYTE_8; break;
	default: return MCB_BYTE_8; break;
	}
}

/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_SIZE_H
#define LIBMCB_SIZE_H

#define MCB_IS_SIGNED_SIZE(X)   ((X) < MCB_U64)
#define MCB_IS_UNSIGNED_SIZE(X) ((X) > MCB_I64)

enum MCB_BYTES {
	MCB_BYTE_1 = 1,
	MCB_BYTE_2 = 2,
	MCB_BYTE_4 = 4,
	MCB_BYTE_8 = 8,
	MCB_BYTES_COUNT = 0
};

enum MCB_SIZE {
	MCB_I8, MCB_I16, MCB_I32, MCB_I64,
	MCB_U8, MCB_U16, MCB_U32, MCB_U64,
	MCB_SIZE_COUNT
};

enum MCB_BYTES mcb_size_to_bytes(enum MCB_SIZE size);

#endif

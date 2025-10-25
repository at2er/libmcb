/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_INST_H
#define LIBMCB_INST_H

#define _MCB_DEF_INST(TYPE_NAME, IMPL_NAME, ...) \
	typedef int (* TYPE_NAME )(__VA_ARGS__); \
	int IMPL_NAME(__VA_ARGS__)

#endif

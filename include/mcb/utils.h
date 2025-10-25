/* SPDX-License-Identifier: MIT */
#ifndef LIBMCB_UTILS_H
#define LIBMCB_UTILS_H

#define mcb_list_for_each(TYPE, CUR, BEGIN) \
	for (TYPE *CUR = BEGIN, \
			*__mcb_list_for_each__next; \
			__mcb_list_for_each__next = CUR ? CUR->next : NULL, \
			CUR != NULL; \
			CUR = __mcb_list_for_each__next)

#endif

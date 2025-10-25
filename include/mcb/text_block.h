/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#ifndef LIBMCB_TEXT_BLOCK_H
#define LIBMCB_TEXT_BLOCK_H
#include <stdio.h>

#define MCB_DEFAULT_TEXT_BLOCK_STR_SIZ 64

struct mcb_text_block {
	struct mcb_text_block *next, *prev;
	char *str;
	size_t str_siz;
};

struct mcb_text_block_root {
	struct mcb_text_block *begin, *end;
};

/**
 * Create a text block and allocate the size of `siz` to `str`.
 * @param siz: pass 0 will allocate 64 bytes with `calloc`.
 */
struct mcb_text_block *mcb_create_text_block(const size_t siz);

void mcb_free_text_block(struct mcb_text_block *blk);

/**
 * Insert `blk` between `b0` and `b1`.
 * @param b0: mustn't be NULL.
 * @param b1: pass NULL will insert between `b0` and the next of `b0`.
 */
void mcb_insert_text_block(struct mcb_text_block *b0,
		struct mcb_text_block *b1,
		struct mcb_text_block *blk);

void mcb_init_text_block_root(struct mcb_text_block_root *root);

void mcb_join_text_block(struct mcb_text_block_root *root,
		struct mcb_text_block *blk);

/**
 * Write all text block to file and free all text block.
 */
void mcb_write_text_block(const struct mcb_text_block_root *root, FILE *fp);

#endif

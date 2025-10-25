/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include "mcb/text_block.h"
#include "mcb/utils.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct mcb_text_block *mcb_create_text_block(const size_t siz)
{
	struct mcb_text_block *res = calloc(1, sizeof(*res));
	res->str_siz = siz == 0
		? MCB_DEFAULT_TEXT_BLOCK_STR_SIZ
		: siz;
	res->str = calloc(res->str_siz, sizeof(*res->str));
	return res;
}

void mcb_free_text_block(struct mcb_text_block *blk)
{
	if (!blk)
		return;
	free(blk->str);
	free(blk);
}

void mcb_insert_text_block(struct mcb_text_block *b0,
		struct mcb_text_block *b1,
		struct mcb_text_block *blk)
{
	assert(b0 && blk);
	if (b1) {
		blk->next = b1;
		b1->prev = blk;
	} else {
		blk->next = b0->next;
		b0->next->prev = blk;
	}
	blk->prev = b0;
	b0->next = blk;
}

void mcb_init_text_block_root(struct mcb_text_block_root *root)
{
	assert(root);
	root->begin = NULL;
	root->end = NULL;
}

void mcb_join_text_block(struct mcb_text_block_root *root,
		struct mcb_text_block *blk)
{
	assert(root && blk);
	blk->next = NULL;
	blk->prev = root->end;
	if (blk->prev) {
		blk->prev->next = blk;
	} else {
		root->begin = blk;
	}
	root->end = blk;
}

void mcb_write_text_block(const struct mcb_text_block_root *root, FILE *fp)
{
	mcb_list_for_each(struct mcb_text_block, cur, root->begin) {
		if (cur->str)
			fwrite(cur->str, sizeof(*cur->str), cur->str_siz, fp);
		mcb_free_text_block(cur);
	}
	fflush(fp);
}

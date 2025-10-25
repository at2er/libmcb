/* This file is part of libmcb.
   SPDX-License-Identifier: LGPL-3.0-or-later
*/
#include "calc.h"
#include "func.h"
#include "gnu_asm.h"
#include "mcb/mcb.h"
#include "mcb/target.h"
#include "mcb/text_block.h"
#include "mov.h"
#include "reg.h"
#include "syscall.h"
#include "var.h"
#include "../darr.h"
#include "../die.h"
#include "../gnu_asm.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifdef __unix__
#include <unistd.h>
#include <sys/wait.h>
#else
#error unsupport platform
#endif

static int assemble(struct mcb_context *context,
		const char *fpath,
		const char *output);
static int call_assembler(const char *assembler,
		const char *fpath,
		const char *output);
static int call_linker(const char *linker,
		const char **files,
		size_t count,
		const char *output);
static int done(struct mcb_context *context);
static int gen_code(struct mcb_context *context, FILE *output);
static int link_files(struct mcb_context *context, const char *output);

static const struct mcb_target_functions target_functions = {
	.fini                  = done,
	.add                   = mcb_gnu_asm_add,
	.assemble              = assemble,
	.alloc_reg             = mcb_gnu_asm_alloc_reg,
	.alloc_var             = mcb_gnu_asm_alloc_var,
	.call_func             = mcb_gnu_asm_call_func,
	.div                   = mcb_gnu_asm_div,
	.drop_reg              = mcb_gnu_asm_drop_reg,
	.drop_var              = mcb_gnu_asm_drop_var,
	.enter_func            = mcb_gnu_asm_enter_func,
	.exit_func             = mcb_gnu_asm_exit_func,
	.func_ret              = mcb_gnu_asm_func_ret,
	.gen_code              = gen_code,
	.link                  = link_files,
	.mov                   = mcb_gnu_asm_mov,
	.mul                   = mcb_gnu_asm_mul,
	.pop                   = mcb_gnu_asm_pop,
	.push                  = mcb_gnu_asm_push,
	.read_mem              = mcb_gnu_asm_read_mem,
	.sub                   = mcb_gnu_asm_sub,
	.syscall               = mcb_gnu_asm_syscall,
	.prepare_func_call_arg = mcb_gnu_asm_prepare_func_call_arg,
	.prepare_syscall_arg   = mcb_gnu_asm_prepare_syscall_arg
};

int assemble(struct mcb_context *context,
		const char *fpath,
		const char *output)
{
	char *output_copy;
	int ret;

	if ((ret = fork()) < 0)
		die("failed to fork\n");
	if (ret == 0)
		return call_assembler(mcb_target_assembler, fpath, output);
	wait(NULL);

	output_copy = calloc(strlen(output) + 1, sizeof(char));
	strcpy(output_copy, output);
	darr_append(context->assembled_files,
			context->assembled_files_count,
			output_copy);
	return 0;
}

int call_assembler(const char *assembler,
		const char *fpath,
		const char *output)
{
	char *cmd[] = {
		(char*)assembler,
		(char*)fpath,
		"-o", (char*)output,
		NULL
	};
	execvp(mcb_target_assembler, cmd);
	return 0;
}

int call_linker(const char *linker,
		const char **files,
		size_t count,
		const char *output)
{
	char **cmd;
	size_t cmd_len = 4 + count;
	size_t offset = 1;
	cmd = calloc(cmd_len, sizeof(*cmd));
	cmd[0] = (char*)mcb_target_linker;
	for (size_t i = 0; i < count; i++, offset++)
		cmd[offset] = (char*)files[i];
	cmd[offset] = "-o";
	cmd[offset + 1] = (char*)output;
	cmd[offset + 2] = NULL;
	execvp(mcb_target_linker, cmd);
	return 0;
}

int done(struct mcb_context *context)
{
	free(context->target);
	return 0;
}

int gen_code(struct mcb_context *context, FILE *output)
{
	mcb_write_text_block(&CUR_CONTEXT(context)->text_block, output);
	return 0;
}

int link_files(struct mcb_context *context, const char *output)
{
	int ret;

	if ((ret = fork()) < 0)
		die("failed to fork\n");
	if (ret == 0) {
		return call_linker(mcb_target_linker,
				context->assembled_files,
				context->assembled_files_count,
				output);
	}
	wait(NULL);

	return 0;
}

int mcb_gnu_asm_init(struct mcb_context *context)
{
	context->target = calloc(1, sizeof(*CUR_CONTEXT(context)));
	mcb_init_text_block_root(&CUR_CONTEXT(context)->text_block);
	context->cur_target_functions = &target_functions;
	return 0;
}

/* SPDX-License-Identifier: MIT */
#ifndef LIBMCB_DIE_H
#define LIBMCB_DIE_H

#define DIE_MSG "libmcb: %s: %s: "
#define DIE_MSG_ARG __FILE__, __func__

void die(const char *msg, ...);

#endif

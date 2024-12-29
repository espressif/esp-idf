/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "sdkconfig.h"

#if CONFIG_LIBC_NEWLIB
#define _REENT_BACKWARD_BINARY_COMPAT
#define _REENT_SDIDINIT(_ptr)  ((_ptr)->_reserved_0)
#define _REENT_SGLUE(_ptr)     (__sglue)

#include_next<sys/reent.h>
#endif // CONFIG_LIBC_NEWLIB

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_LIBC_NEWLIB

extern void   __sinit(struct _reent *);

extern struct _glue __sglue;
extern struct _reent * _global_impure_ptr;

#else // CONFIG_LIBC_NEWLIB

struct _reent {};
#define __getreent() NULL

int _system_r(struct _reent *r, const char *str);
int _raise_r(struct _reent *r, int sig);

#endif // CONFIG_LIBC_NEWLIB

#ifdef __cplusplus
}
#endif

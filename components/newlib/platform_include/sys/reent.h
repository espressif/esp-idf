/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define _REENT_BACKWARD_BINARY_COMPAT
#define _REENT_SDIDINIT(_ptr)  ((_ptr)->_reserved_0)
#define _REENT_SGLUE(_ptr)     (__sglue)

#include_next<sys/reent.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void   __sinit(struct _reent *);

extern struct _glue __sglue;
extern struct _reent * _global_impure_ptr;

#ifdef __cplusplus
}
#endif

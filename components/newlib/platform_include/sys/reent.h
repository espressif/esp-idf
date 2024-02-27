/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#if __NEWLIB__ > 4 || ( __NEWLIB__ == 4 && __NEWLIB_MINOR__ > 1 ) /* TODO: IDF-8134 */
#define _REENT_BACKWARD_BINARY_COMPAT
#define _REENT_SDIDINIT(_ptr)  ((_ptr)->_reserved_0)
#define _REENT_SGLUE(_ptr)     (__sglue)
#else
#define _REENT_CLEANUP(_ptr)   ((_ptr)->__cleanup)
#define _REENT_STDIN(_ptr)     ((_ptr)->_stdin)
#define _REENT_STDOUT(_ptr)    ((_ptr)->_stdout)
#define _REENT_STDERR(_ptr)    ((_ptr)->_stderr)
#define _REENT_SDIDINIT(_ptr)  ((_ptr)->__sdidinit)
#define _REENT_SGLUE(_ptr)     ((_ptr)->__sglue)
#endif

#include_next<sys/reent.h>

#ifdef __cplusplus
extern "C" {
#endif

#if __NEWLIB__ > 4 || ( __NEWLIB__ == 4 && __NEWLIB_MINOR__ > 1 ) /* TODO: IDF-8134 */

extern void   __sinit(struct _reent *);

extern struct _glue __sglue;
extern struct _reent * _global_impure_ptr;

#else /* __NEWLIB__ > 4 || ( __NEWLIB__ == 4 && __NEWLIB_MINOR__ > 1 ) */

/* This function is not part of the newlib API, it is defined in libc/stdio/local.h
 * There is no nice way to get __cleanup member populated while avoiding __sinit,
 * so extern declaration is used here.
 */
extern void _cleanup_r(struct _reent *);

#endif /* __NEWLIB__ > 4 || ( __NEWLIB__ == 4 && __NEWLIB_MINOR__ > 1 ) */

#ifdef __cplusplus
}
#endif

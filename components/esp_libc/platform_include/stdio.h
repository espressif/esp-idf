/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include "sdkconfig.h"

#if CONFIG_LIBC_PICOLIBC_NEWLIB_COMPATIBILITY
#include <stddef.h>
#include <sys/features.h>
/* Undefine __BSD_VISIBLE to set proper funopen declaration */
#pragma push_macro("__BSD_VISIBLE")
#undef __BSD_VISIBLE
#endif

#include_next <stdio.h>
#if CONFIG_LIBC_PICOLIBC_NEWLIB_COMPATIBILITY
#pragma pop_macro("__BSD_VISIBLE")
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_LIBC_PICOLIBC_NEWLIB_COMPATIBILITY

#if __BSD_VISIBLE
FILE    *funopen(const void *cookie,
                 int (*readfn)(void *cookie, char *buf,
                               int n),
                 int (*writefn)(void *cookie, const char *buf,
                                int n),
                 __off_t (*seekfn)(void *cookie, __off_t off, int whence),
                 int (*closefn)(void *cookie));
# define    fropen(__cookie, __fn) funopen(__cookie, __fn, NULL, NULL, NULL)
# define    fwopen(__cookie, __fn) funopen(__cookie, NULL, __fn, NULL, NULL)
#endif /*__BSD_VISIBLE */

void flockfile(FILE *);
void funlockfile(FILE *);
FILE *open_memstream(char **, size_t *);
#endif

#ifdef __cplusplus
}
#endif

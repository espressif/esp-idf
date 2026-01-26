/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
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

typedef ssize_t cookie_read_function_t(void *cookie, char *buf, size_t n);
typedef ssize_t cookie_write_function_t(void *cookie, const char *buf, size_t n);
typedef int cookie_seek_function_t(void *cookie, __off_t *off, int whence);
typedef int cookie_close_function_t(void *cookie);
typedef struct {
    /* These four struct member names are dictated by Linux; hopefully,
       they don't conflict with any macros.  */
    cookie_read_function_t  *read;
    cookie_write_function_t *write;
    cookie_seek_function_t  *seek;
    cookie_close_function_t *close;
} cookie_io_functions_t;
FILE *fopencookie(void *cookie, const char *mode, cookie_io_functions_t functions);
#endif

#ifdef __cplusplus
}
#endif

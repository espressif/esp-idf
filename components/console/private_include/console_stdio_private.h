/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * This file provides thread-local storage for stdin/stdout streams when using
 * picolibc to have thread-local stdio streams instead of global ones.
 * It enables per-thread stdio redirection by:
 * - Defining thread-local FILE* variables (tls_stdin, tls_stdout)
 * - Redefining standard I/O macros (stdin, stdout, printf, scanf, etc.) to use TLS streams
 * - Providing initialization and cleanup functions for TLS stdio streams
 */

#pragma once
#include <stdio.h>
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_LIBC_PICOLIBC && !CONFIG_LIBC_PICOLIBC_NEWLIB_COMPATIBILITY
#define tls_stdin linenoise_stdin
#define tls_stdout linenoise_stdout
#endif

#if CONFIG_LIBC_PICOLIBC
extern __thread FILE *tls_stdin;
extern __thread FILE *tls_stdout;
#endif

static inline void linenoise_init_with_global_stdio(void)
{
#if CONFIG_LIBC_PICOLIBC
    tls_stdin = stdin;
    tls_stdout = stdout;
#endif
}

static inline void linenoise_close_stdio(void)
{
#if CONFIG_LIBC_PICOLIBC
    if (tls_stdin != stdin) {
        fclose(tls_stdin);
    }
    if (tls_stdout != stdout) {
        fclose(tls_stdout);
    }
#endif
}

#if CONFIG_LIBC_PICOLIBC
#undef  stdin
#define stdin  tls_stdin

#undef  stdout
#define stdout tls_stdout

#define printf(...)          fprintf(tls_stdout, __VA_ARGS__)
#define vprintf(fmt, ap)     vfprintf(tls_stdout, fmt, ap)
#ifdef putchar
#undef putchar
#endif
#define putchar(c)           fputc((c), tls_stdout)
#define puts(s)              fputs((s), tls_stdout)

#define scanf(...)           fscanf(tls_stdin, __VA_ARGS__)
#define vscanf(fmt, ap)      vfscanf(tls_stdin, fmt, ap)
#ifdef getchar
#undef getchar
#endif
#define getchar()            fgetc(tls_stdin)
#define gets(buf)            fgets((buf), sizeof(buf), tls_stdin)
#endif

#ifdef __cplusplus
}
#endif

/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "hal/assert.h"

// Implementation of __assert_func for macOS.
void __assert_func(const char *file, int line, const char *func, const char *expr)
{
    fprintf(stderr, "assert failed at %s:%d (%s): %s\n", file, line, func, expr);
    abort();
}

// Defining this symbol as well, since `hal` component will add "-U __assert_func" linker option,
// and symbols are prefixed with an additional underscore on macOS.
// (Can't use __attribute__((alias)) because aliases are not supported on macOS.)
void _assert_func(const char *file, int line, const char *func, const char *expr)
{
    __assert_func(file, line, func, expr);
}

/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

static inline void panic_print_str(const char* str)
{
    /* Ignore "Backtrace:" string. */
    if (strcmp(str, "Backtrace:") != 0) {
        printf("%s", str);
    }
}

static inline void panic_print_hex(const uint32_t value)
{
    printf("%x", value);
}

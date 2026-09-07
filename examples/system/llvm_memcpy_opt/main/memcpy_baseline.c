/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <string.h>

__attribute__((noinline))
void memcpy_baseline_1024(void *dst, const void *src)
{
    dst = __builtin_assume_aligned(dst, 16);
    src = __builtin_assume_aligned(src, 16);
    memcpy(dst, src, 1024);
}

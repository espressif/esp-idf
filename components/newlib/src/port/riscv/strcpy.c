/*
 * SPDX-FileCopyrightText: 2017 SiFive Inc.
 *
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD AND Apache-2.0
 *
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 */
/* Copyright (c) 2017  SiFive Inc. All rights reserved.

   This copyrighted material is made available to anyone wishing to use,
   modify, copy, or redistribute it subject to the terms and conditions
   of the FreeBSD License.   This program is distributed in the hope that
   it will be useful, but WITHOUT ANY WARRANTY expressed or implied,
   including the implied warranties of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  A copy of this license is available at
   http://www.opensource.org/licenses.
*/

#include <string.h>
#include <stdint.h>

__attribute__((always_inline))
static inline
unsigned long __newlib__libc_detect_null(unsigned long w)
{
    unsigned long mask = 0x7f7f7f7f;
    if (sizeof(long) == 8) {
        mask = ((mask << 16) << 16) | mask;
    }
    return ~(((w & mask) + mask) | w | mask);
}

__attribute__((optimize("-Os")))
char *strcpy(char *dst, const char *src)
{
    char *dst0 = dst;

#if !(__riscv_misaligned_slow || __riscv_misaligned_fast)
    int misaligned = ((uintptr_t)dst | (uintptr_t)src) & (sizeof(long) - 1);
    if (__builtin_expect(!misaligned, 1))
#endif
    {
        long *ldst = (long *)dst;
        const long *lsrc = (const long *)src;

        while (!__newlib__libc_detect_null(*lsrc)) {
            *ldst++ = *lsrc++;
        }

        dst = (char *)ldst;
        src = (const char *)lsrc;

        char c0 = src[0];
        char c1 = src[1];
        char c2 = src[2];
        if (!(*dst++ = c0)) {
            return dst0;
        }
        if (!(*dst++ = c1)) {
            return dst0;
        }
        char c3 = src[3];
        if (!(*dst++ = c2)) {
            return dst0;
        }
        if (sizeof(long) == 4) {
            goto out;
        }
        char c4 = src[4];
        if (!(*dst++ = c3)) {
            return dst0;
        }
        char c5 = src[5];
        if (!(*dst++ = c4)) {
            return dst0;
        }
        char c6 = src[6];
        if (!(*dst++ = c5)) {
            return dst0;
        }
        if (!(*dst++ = c6)) {
            return dst0;
        }

out:
        *dst++ = 0;
        return dst0;
    }

    char ch;
    do {
        ch = *src;
        src++;
        dst++;
        *(dst - 1) = ch;
    } while (ch);

    return dst0;
}

// Hook to force the linker to include this file
void esp_libc_include_strcpy_impl(void)
{
}

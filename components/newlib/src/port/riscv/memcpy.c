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
#include "esp_attr.h"
#include "../../string/local.h"

#define unlikely(X) __builtin_expect (!!(X), 0)

IRAM_ATTR
void *
__attribute__((optimize("-Os")))
__inhibit_loop_to_libcall
memcpy(void *__restrict aa, const void *__restrict bb, size_t n)
{
#define BODY(a, b, t) { \
    t tt = *b; \
    a++, b++; \
    *(a - 1) = tt; \
  }

    char *a = (char *)aa;
    const char *b = (const char *)bb;
    char *end = a + n;
    uintptr_t msk = sizeof(long) - 1;
#if __riscv_misaligned_slow || __riscv_misaligned_fast
    if (n < sizeof(long))
#else
    if (unlikely((((uintptr_t)a & msk) != ((uintptr_t)b & msk))
                 || n < sizeof(long)))
#endif
    {
small:
        if (__builtin_expect(a < end, 1))
            while (a < end) {
                BODY(a, b, char);
            }
        return aa;
    }

    if (unlikely(((uintptr_t)a & msk) != 0))
        while ((uintptr_t)a & msk) {
            BODY(a, b, char);
        }

    long *la = (long *)a;
    const long *lb = (const long *)b;
    long *lend = (long *)((uintptr_t)end & ~msk);

    if (unlikely(lend - la > 8)) {
        while (lend - la > 8) {
            long b0 = *lb++;
            long b1 = *lb++;
            long b2 = *lb++;
            long b3 = *lb++;
            long b4 = *lb++;
            long b5 = *lb++;
            long b6 = *lb++;
            long b7 = *lb++;
            long b8 = *lb++;
            *la++ = b0;
            *la++ = b1;
            *la++ = b2;
            *la++ = b3;
            *la++ = b4;
            *la++ = b5;
            *la++ = b6;
            *la++ = b7;
            *la++ = b8;
        }
    }

    while (la < lend) {
        BODY(la, lb, long);
    }

    a = (char *)la;
    b = (const char *)lb;
    if (unlikely(a < end)) {
        goto small;
    }
    return aa;
}

// Hook to force the linker to include this file
void esp_libc_include_memcpy_impl(void)
{
}

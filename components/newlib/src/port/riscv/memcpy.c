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
#include "string/local.h"

#define unlikely(X) __builtin_expect (!!(X), 0)

void *
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
            /*
             *   long b0 = *lb++;
             *   long b1 = *lb++;
             *   long b2 = *lb++;
             *   long b3 = *lb++;
             *   long b4 = *lb++;
             *   long b5 = *lb++;
             *   long b6 = *lb++;
             *   long b7 = *lb++;
             *   long b8 = *lb++;
             *   *la++ = b0;
             *   *la++ = b1;
             *   *la++ = b2;
             *   *la++ = b3;
             *   *la++ = b4;
             *   *la++ = b5;
             *   *la++ = b6;
             *   *la++ = b7;
             *   *la++ = b8;
             */
            long src0, src1, src2, src3;
            long src4, src5, src6, src7;
            long src8;
            /* DIG-694: need at least 2 instructions between lw and sw */
            asm volatile("lw   %0,  0(%10)\n"   // scr0 = lb[0];
                         "lw   %1,  4(%10)\n"   // scr1 = lb[1];
                         "lw   %2,  8(%10)\n"   // scr2 = lb[2];
                         "lw   %3, 12(%10)\n"   // scr3 = lb[3];
                         "lw   %4, 16(%10)\n"   // scr4 = lb[4];
                         "lw   %5, 20(%10)\n"   // scr5 = lb[5];
                         "lw   %6, 24(%10)\n"   // scr6 = lb[6];
                         "lw   %7, 28(%10)\n"   // scr7 = lb[7];
                         "lw   %8, 32(%10)\n"   // scr8 = lb[8];
                         "addi %9,  %9,   36\n" // la += 8 * 9;
                         "addi %10, %10,  36\n" // lb += 8 * 9;
                         "sw   %0, -36(%9)\n"   // *(la - 9) = src;
                         "sw   %1, -32(%9)\n"   // *(la - 8) = src;
                         "sw   %2, -28(%9)\n"   // *(la - 7) = src;
                         "sw   %3, -24(%9)\n"   // *(la - 6) = src;
                         "sw   %4, -20(%9)\n"   // *(la - 5) = src;
                         "sw   %5, -16(%9)\n"   // *(la - 4) = src;
                         "sw   %6, -12(%9)\n"   // *(la - 3) = src;
                         "sw   %7,  -8(%9)\n"   // *(la - 2) = src;
                         "sw   %8,  -4(%9)\n"   // *(la - 1) = src;
                         : "=r"(src0), "=r"(src1), "=r"(src2), "=r"(src3),
                         "=r"(src4), "=r"(src5), "=r"(src6), "=r"(src7),
                         "=r"(src8),
                         "+r"(la), "+r"(lb)
                         :: "memory");
        }
    }
    while (la < lend) {
        /*
         *   BODY(la, lb, long);
         */
        long src0;
#ifdef __OPTIMIZE_SIZE__
#error "Enabled Os optimization may not work properly for DIG-694"
        /*
         * Replacing the string:
         *   *la++ = tt;
         * To:
         *   "addi %2, %4,  4\n" // la++;
         *   "sw   %0, -4(%4)\n" // *(la-1) = src0;
         * May break some optimizations and slightly reduce performance.
         */
#endif
        /* DIG-694: need at least 2 instructions between lw and sw */
        asm volatile("lw   %0,  0(%1)\n" // long src0 = *lb;
                     "addi %1, %1,  4\n" // lb++;
                     "addi %2, %2,  4\n" // la++;
                     "sw   %0, -4(%2)\n" // *(la-1) = src0;
                     : "=r"(src0), "+r"(lb), "+r"(la)
                     :: "memory");
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

/*
 * SPDX-FileCopyrightText: 1994-2009 Red Hat, Inc.
 *
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD AND Apache-2.0
 *
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 */
#include <string.h>
#include <_ansi.h>
#include <stddef.h>
#include <limits.h>
#include "string/local.h"

void *
__inhibit_loop_to_libcall
memmove(void *dst_void,
        const void *src_void,
        size_t length)
{
    char *dst = dst_void;
    const char *src = src_void;
    long *aligned_dst;
    const long *aligned_src;

    if (src < dst && dst < src + length) {
        /* Destructive overlap...have to copy backwards */
        src += length;
        dst += length;

        if (!TOO_SMALL_LITTLE_BLOCK(length) && !UNALIGNED_X_Y(src, dst)) {
            aligned_dst = (long*)dst;
            aligned_src = (long*)src;

            /* Copy one long word at a time if possible.  */
            while (!TOO_SMALL_LITTLE_BLOCK(length)) {
                /*
                 *   const long src0 = *--aligned_src;
                 *   *--aligned_dst = src0;
                 *   length -= LITTLE_BLOCK_SIZE;
                 */
                long src0;
                /* DIG-694: need at least 2 instructions between lw and sw */
                asm volatile("lw   %0, -4(%1)\n" // src0 = *(aligned_src - 1);
                             "addi %1, %1, -4\n" // aligned_src--;
                             "addi %2, %2, -4\n" // aligned_dst--;
                             "addi %3, %3, -4\n" // length -= LITTLE_BLOCK_SIZE;
                             "sw   %0,  0(%2)\n" // aligned_dst = src0;
                             : "=r"(src0), "+r"(aligned_src), "+r"(aligned_dst), "+r"(length)
                             :: "memory");
            }

            /* Pick up any residual with a byte copier.  */
            dst = (char*)aligned_dst;
            src = (char*)aligned_src;
        }

        while (length--) {
            *--dst = *--src;
        }
    } else {
        /* Use optimizing algorithm for a non-destructive copy to closely
           match memcpy. If the size is small or either SRC or DST is unaligned,
           then punt into the byte copy loop.  This should be rare.  */
        if (!TOO_SMALL_LITTLE_BLOCK(length) && !UNALIGNED_X_Y(src, dst)) {
            aligned_dst = (long*)dst;
            aligned_src = (long*)src;

            /* Copy 8X long words at a time if possible.  */
            while (length >= BIG_BLOCK_SIZE * 2) {
                /*
                 *   const long src0 = *aligned_src++;
                 *   const long src1 = *aligned_src++;
                 *   const long src2 = *aligned_src++;
                 *   const long src3 = *aligned_src++;
                 *   const long src4 = *aligned_src++;
                 *   const long src5 = *aligned_src++;
                 *   const long src6 = *aligned_src++;
                 *   const long src7 = *aligned_src++;
                 *   *aligned_dst++ = src0;
                 *   *aligned_dst++ = src1;
                 *   *aligned_dst++ = src2;
                 *   *aligned_dst++ = src3;
                 *   *aligned_dst++ = src4;
                 *   *aligned_dst++ = src5;
                 *   *aligned_dst++ = src6;
                 *   *aligned_dst++ = src7;
                 */
                long src0, src1, src2, src3;
                long src4, src5, src6, src7;
                /* DIG-694: need at least 2 instructions between lw and sw */
                asm volatile("lw   %0,  0(%8)\n"    // src0 = aligned_src[0];
                             "lw   %1,  4(%8)\n"    // src1 = aligned_src[1];
                             "lw   %2,  8(%8)\n"    // src2 = aligned_src[2];
                             "lw   %3, 12(%8)\n"    // src3 = aligned_src[3];
                             "lw   %4, 16(%8)\n"    // src4 = aligned_src[4];
                             "lw   %5, 20(%8)\n"    // src5 = aligned_src[5];
                             "lw   %6, 24(%8)\n"    // src6 = aligned_src[6];
                             "lw   %7, 28(%8)\n"    // src7 = aligned_src[7];
                             "addi %8,  %8,  32\n"  // aligned_src += BIG_BLOCK_SIZE * 2;
                             "addi %9,  %9,  32\n"  // aligned_dst += BIG_BLOCK_SIZE * 2;
                             "addi %10, %10, -32\n" // length -= BIG_BLOCK_SIZE * 2;
                             "sw   %0, -32(%9)\n"   // *(aligned_dst - 8) = src0;
                             "sw   %1, -28(%9)\n"   // *(aligned_dst - 7) = src1;
                             "sw   %2, -24(%9)\n"   // *(aligned_dst - 6) = src2;
                             "sw   %3, -20(%9)\n"   // *(aligned_dst - 5) = src3;
                             "sw   %4, -16(%9)\n"   // *(aligned_dst - 4) = src4;
                             "sw   %5, -12(%9)\n"   // *(aligned_dst - 3) = src5;
                             "sw   %6,  -8(%9)\n"   // *(aligned_dst - 2) = src6;
                             "sw   %7,  -4(%9)\n"   // *(aligned_dst - 1) = src7;
                             : "=r"(src0), "=r"(src1), "=r"(src2), "=r"(src3),
                             "=r"(src4), "=r"(src5), "=r"(src6), "=r"(src7),
                             "+r"(aligned_src), "+r"(aligned_dst), "+r"(length)
                             :: "memory");
            }

            /* Copy one long word at a time if possible.  */
            while (!TOO_SMALL_LITTLE_BLOCK(length)) {
                /*
                 *   const long src0 = *aligned_src++;
                 *   *aligned_dst++ = src0;
                 *   length -= LITTLE_BLOCK_SIZE;
                 */
                long src0;
                /* DIG-694: need at least 2 instructions between lw and sw */
                asm volatile("lw   %0,  0(%4)\n" // long src0 = *aligned_src;
                             "addi %1, %4,  4\n" // aligned_src++;
                             "addi %2, %5,  4\n" // aligned_dst++;
                             "addi %3, %6, -4\n" // length -= LITTLE_BLOCK_SIZE;
                             "sw   %0, -4(%5)\n" // *(aligned_dst-1) = src0;
                             : "=r"(src0), "=r"(aligned_src), "=r"(aligned_dst), "=r"(length)
                             : "r"(aligned_src), "r"(aligned_dst), "r"(length));
            }

            /* Pick up any residual with a byte copier.  */
            dst = (char*)aligned_dst;
            src = (char*)aligned_src;
        }

        while (length--) {
            *dst++ = *src++;
        }
    }

    return dst_void;
}

// Hook to force the linker to include this file
void esp_libc_include_memmove_impl(void)
{
}

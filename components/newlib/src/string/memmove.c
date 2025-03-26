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
#include "local.h"

__attribute__((optimize("-Os")))
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
                *--aligned_dst = *--aligned_src;
                length -= LITTLE_BLOCK_SIZE;
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

            /* Copy 4X long words at a time if possible.  */
            while (!TOO_SMALL_BIG_BLOCK(length)) {
                *aligned_dst++ = *aligned_src++;
                *aligned_dst++ = *aligned_src++;
                *aligned_dst++ = *aligned_src++;
                *aligned_dst++ = *aligned_src++;
                length -= BIG_BLOCK_SIZE;
            }

            /* Copy one long word at a time if possible.  */
            while (!TOO_SMALL_LITTLE_BLOCK(length)) {
                *aligned_dst++ = *aligned_src++;
                length -= LITTLE_BLOCK_SIZE;
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

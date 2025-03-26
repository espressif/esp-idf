/*
 * SPDX-FileCopyrightText: 1994-2009 Red Hat, Inc.
 *
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD AND Apache-2.0
 *
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 */
#include <string.h>
#include <limits.h>
#include "local.h"

__attribute__((optimize("-Os")))
char *
strncpy(char *__restrict dst0,
        const char *__restrict src0,
        size_t count)
{
    char *dst = dst0;
    const char *src = src0;
    long *aligned_dst;
    const long *aligned_src;

    /* If SRC and DEST is aligned and count large enough, then copy words.  */
    if (!UNALIGNED_X_Y(src, dst) && !TOO_SMALL_LITTLE_BLOCK(count)) {
        aligned_dst = (long*)dst;
        aligned_src = (long*)src;

        /* SRC and DEST are both "long int" aligned, try to do "long int"
        sized copies.  */
        while (!TOO_SMALL_LITTLE_BLOCK(count) && !DETECT_NULL(*aligned_src)) {
            count -= sizeof(long int);
            *aligned_dst++ = *aligned_src++;
        }

        dst = (char*)aligned_dst;
        src = (char*)aligned_src;
    }

    while (count > 0) {
        --count;
        if ((*dst++ = *src++) == '\0') {
            break;
        }
    }

    while (count-- > 0) {
        *dst++ = '\0';
    }

    return dst0;
}

// Hook to force the linker to include this file
void esp_libc_include_strncpy_impl(void)
{
}

/*
 * SPDX-FileCopyrightText: 1994-2009 Red Hat, Inc.
 *
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD AND Apache-2.0
 *
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 */
#include <string.h>
#include "local.h"

__attribute__((optimize("-Os")))
int
memcmp(const void *m1,
       const void *m2,
       size_t n)
{
    unsigned char *s1 = (unsigned char *) m1;
    unsigned char *s2 = (unsigned char *) m2;
    unsigned long *a1;
    unsigned long *a2;

    /* If the size is too small, or either pointer is unaligned,
       then we punt to the byte compare loop.  Hopefully this will
       not turn up in inner loops.  */
    if (!TOO_SMALL_LITTLE_BLOCK(n) && !UNALIGNED_X_Y(s1, s2)) {
        /* Otherwise, load and compare the blocks of memory one
           word at a time.  */
        a1 = (unsigned long*) s1;
        a2 = (unsigned long*) s2;
        while (!TOO_SMALL_LITTLE_BLOCK(n)) {
            if (*a1 != *a2) {
                break;
            }
            a1++;
            a2++;
            n -= LITTLE_BLOCK_SIZE;
        }

        /* check m mod LITTLE_BLOCK_SIZE remaining characters */

        s1 = (unsigned char*)a1;
        s2 = (unsigned char*)a2;
    }

    while (n--) {
        if (*s1 != *s2) {
            return *s1 - *s2;
        }
        s1++;
        s2++;
    }

    return 0;
}

// Hook to force the linker to include this file
void esp_libc_include_memcmp_impl(void)
{
}

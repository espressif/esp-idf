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
int
strncmp(const char *s1,
        const char *s2,
        size_t n)
{
    unsigned long *a1;
    unsigned long *a2;

    if (n == 0) {
        return 0;
    }

    /* If s1 or s2 are unaligned, then compare bytes. */
    if (!UNALIGNED_X_Y(s1, s2)) {
        /* If s1 and s2 are word-aligned, compare them a word at a time. */
        a1 = (unsigned long*)s1;
        a2 = (unsigned long*)s2;
        while (n >= sizeof(long) && *a1 == *a2) {
            n -= sizeof(long);

            /* If we've run out of bytes or hit a null, return zero
            since we already know *a1 == *a2.  */
            if (n == 0 || DETECT_NULL(*a1)) {
                return 0;
            }

            a1++;
            a2++;
        }

        /* A difference was detected in last few bytes of s1, so search bytewise */
        s1 = (char*)a1;
        s2 = (char*)a2;
    }

    while (n-- > 0 && *s1 == *s2) {
        /* If we've run out of bytes or hit a null, return zero
        since we already know *s1 == *s2.  */
        if (n == 0 || *s1 == '\0') {
            return 0;
        }
        s1++;
        s2++;
    }
    return (*(unsigned char *) s1) - (*(unsigned char *) s2);
}

// Hook to force the linker to include this file
void esp_libc_include_strncmp_impl(void)
{
}

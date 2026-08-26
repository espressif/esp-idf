/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * This header file adds the libbsd defines of header cdefs.h.
 * It also defines __uintptr_t which is still used by older libbsd versions (< 0.11.3)
 * but not (or not guaranteed to be) defined on Linux.
 */

#pragma once

#if (defined(__APPLE__) && defined(__MACH__))
// MacOS

#if !defined(__containerof)
#define __containerof(ptr, type, member) ({         \
    const typeof( ((type *)0)->member ) *__mptr = (ptr); \
    (type *)( (char *)__mptr - offsetof(type,member) );})
#endif

#include_next <sys/cdefs.h> // include the actual cdefs.h (which does not contain containerof)

#else
// Linux

#include <stdint.h>

// We need a define. We can't typedef here since, depending on the include order,
// uintptr_t may not be defined yet.
#define __uintptr_t uintptr_t

#include_next <sys/cdefs.h>
#include <bsd/sys/cdefs.h>

#endif

/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <errno.h>

// reent struct is not used for Linux platform. It's defined in Newlib.
// To make it compatible with vfs.c, following dummy struct and macros are defined.
struct _reent {
    int i;
};

// __getreent() returns NULL on Linux, so __errno_r() must not dereference its
// argument. Map it to the real (thread-local) errno instead, matching esp_libc.
#define __errno_r(r)  errno
#define __getreent()  NULL

/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

// reent struct is not used for Linux platform. It's defined in Newlib.
// To make it compatible with vfs.c, following dummy struct and macros are defined.
struct _reent {
    int i;
};

#define __errno_r(r)  r->i
#define __getreent()  NULL

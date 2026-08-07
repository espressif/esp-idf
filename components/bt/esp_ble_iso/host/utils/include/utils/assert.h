/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Also registered as lib_ext_funcs._assert, so tag/info stay free-form for the
 * lib's own call sites; BT_LE_ASSERT() passes the stringified condition. */
void bt_le_assert(const char *tag, size_t info,
                  const char *file, int line, const char *func) __attribute__((noreturn));

/* Use instead of assert(): CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_DISABLE sets
 * -DNDEBUG, which turns every assert() into a no-op and lets execution fall
 * through onto the very pointer it was guarding. This one always checks. */
#define BT_LE_ASSERT(_cond) \
    do { \
        if (!(_cond)) { \
            bt_le_assert(#_cond, 0, __FILE_NAME__, __LINE__, __func__); \
        } \
    } while (0)

#ifdef __cplusplus
}
#endif

/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* This header file wraps newlib's own unmodified assert.h and adds
   support for silent assertion failure.
*/
#pragma once
#include <sdkconfig.h>
#include <stdlib.h>
#include <stdint.h>

#include_next <assert.h>

/* moved part of libc provided assert to here allows
 * tweaking the assert macro to use __builtin_expect()
 * and reduce jumps in the "asserts OK" code path
 *
 * Note: using __builtin_expect() not likely() to avoid defining the likely
 * macro in namespace of non-IDF code that may include this standard header.
 */
#undef assert

/* __FILENAME__ points to the file name instead of path + filename
 * e.g __FILE__ points to "/apps/test.c" where as __FILENAME__ points to "test.c"
 */
#define __FILENAME__ (__builtin_strrchr( "/" __FILE__, '/') + 1)

#if defined(NDEBUG)

#define assert(__e) ((void)(__e))

#elif CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_SILENT

#define assert(__e) (__builtin_expect(!!(__e), 1) ? (void)0 : __assert_func(NULL, 0, NULL, NULL))

#else // !CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_SILENT

#define assert(__e) (__builtin_expect(!!(__e), 1) ? (void)0 : __assert_func (__FILENAME__, __LINE__, \
                                                                             __ASSERT_FUNC, #__e))
#endif

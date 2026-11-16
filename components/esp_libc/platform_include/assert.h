/*
 * SPDX-FileCopyrightText: 2017-2026 Espressif Systems (Shanghai) CO LTD
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

#if CONFIG_COMPILER_ASSERT_NDEBUG_EVALUATE
#define assert(__e) ((void)(__e))
#else
#define assert(__e) ((void)0)
#endif //CONFIG_COMPILER_ASSERT_NDEBUG_EVALUATE

#elif CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_SILENT

/* A failed silent assertion is identified by the return address of its call to __assert_func(), so
 * each assertion needs a call of its own. As __assert_func() never returns, the failure paths of
 * several assertions in one function are identical, and the compiler merges them into one call,
 * which then reports one of those assertions for all of them. Calling it through a declaration
 * without noreturn lets an empty asm follow the call, and an operand unique to each assertion makes
 * every failure path different, so none can be merged. __builtin_unreachable() still tells the
 * compiler that the assertion does not continue.
 */
void __assert_func_silent(const char *file, int line, const char *func, const char *expr) __asm__("__assert_func");
#define assert(__e) (__builtin_expect(!!(__e), 1) ? (void)0 : __extension__ ({ \
        __assert_func_silent(NULL, 0, NULL, NULL); \
        __asm__ __volatile__("" :: "i"(__COUNTER__)); \
        __builtin_unreachable(); \
    }))

#else // !CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_SILENT

#define assert(__e) (__builtin_expect(!!(__e), 1) ? (void)0 : __assert_func (__FILENAME__, __LINE__, \
                                                                             __ASSERT_FUNC, #__e))
#endif

/*
 * SPDX-FileCopyrightText: 2016-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

/*
 * The likely and unlikely macro pairs:
 * These macros are useful to place when application
 * knows the majority occurrence of a decision paths,
 * placing one of these macros can hint the compiler
 * to reorder instructions producing more optimized
 * code.
 */
#if (CONFIG_COMPILER_OPTIMIZATION_PERF)
#ifndef likely
#define likely(x)      __builtin_expect(!!(x), 1)
#endif
#ifndef unlikely
#define unlikely(x)    __builtin_expect(!!(x), 0)
#endif
#else
#ifndef likely
#define likely(x)      (x)
#endif
#ifndef unlikely
#define unlikely(x)    (x)
#endif
#endif

/*
 * Utility macros used for designated initializers, which work differently
 * in C99 and C++ standards mainly for aggregate types.
 * The member separator, comma, is already part of the macro, please omit the trailing comma.
 * Usage example:
 *   struct config_t { char* pchr; char arr[SIZE]; } config = {
 *              ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(pchr)
 *              ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_STR(arr, "Value")
 *          };
 */
#if defined(__cplusplus) && __cplusplus >= 202002L
#define ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_STR(member, value)  .member = value,
#define ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(member) .member = { },
#elif defined(__cplusplus) && __cplusplus < 202002L
#define ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_STR(member, value)  { .member = value },
#define ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(member) .member = { },
#else
#define ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_STR(member, value)  .member = value,
#define ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(member)
#endif

#define __COMPILER_PRAGMA__(string) _Pragma(#string)
#define _COMPILER_PRAGMA_(string) __COMPILER_PRAGMA__(string)

#if __clang__
#define ESP_COMPILER_DIAGNOSTIC_PUSH_IGNORE(warning) \
    __COMPILER_PRAGMA__(clang diagnostic push) \
    __COMPILER_PRAGMA__(clang diagnostic ignored "-Wunknown-warning-option") \
    __COMPILER_PRAGMA__(clang diagnostic ignored warning)
#define ESP_COMPILER_DIAGNOSTIC_POP(warning) \
    __COMPILER_PRAGMA__(clang diagnostic pop)
#elif __GNUC__
#define ESP_COMPILER_DIAGNOSTIC_PUSH_IGNORE(warning) \
    __COMPILER_PRAGMA__(GCC diagnostic push) \
    __COMPILER_PRAGMA__(GCC diagnostic ignored "-Wpragmas") \
    __COMPILER_PRAGMA__(GCC diagnostic ignored warning)
#define ESP_COMPILER_DIAGNOSTIC_POP(warning) \
    __COMPILER_PRAGMA__(GCC diagnostic pop)
#else
#define ESP_COMPILER_DIAGNOSTIC_PUSH_IGNORE(warning)
#define ESP_COMPILER_DIAGNOSTIC_POP(warning)
#endif

#if __clang_analyzer__ || CONFIG_COMPILER_STATIC_ANALYZER
#define ESP_STATIC_ANALYZER_CHECK(_expr_, _ret_) do { if ((_expr_)) { return (_ret_); } } while(0)
#else
#define ESP_STATIC_ANALYZER_CHECK(_expr_, _ret_)
#endif

/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_LIBC_PICOLIBC
#if defined(__cplusplus) && __cplusplus >= 201103L
#define __noreturn [[noreturn]]
#elif __has_attribute(__noreturn__)
#define __noreturn __attribute__((__noreturn__))
#else
#define __noreturn
#endif
#else
#define __noreturn
#endif

__noreturn void __assert_func(const char *file, int line, const char *func, const char *expr);
__noreturn void abort(void);

#ifndef __ASSERT_FUNC
#ifdef __ASSERT_FUNCTION
#define __ASSERT_FUNC __ASSERT_FUNCTION
#else
#define __ASSERT_FUNC "??"
#endif
#endif

#if BOOTLOADER_BUILD
// Bootloader has very limited size, while full assertion takes up quite a lot bytes as it prints file, line, and
// function info. Therefore, we set the HAL assertion level in bootloader to be no larger than 1 (silent).
#if CONFIG_HAL_DEFAULT_ASSERTION_LEVEL == 2
#undef CONFIG_HAL_DEFAULT_ASSERTION_LEVEL
#define CONFIG_HAL_DEFAULT_ASSERTION_LEVEL 1
#endif
#endif

#if IS_ULP_COCPU
#define HAL_ASSERT(__e) ((void)(__e))
#elif CONFIG_HAL_DEFAULT_ASSERTION_LEVEL == 1 // silent
// Keeps each assertion's call to abort() separate, so the address it reports is the assertion's own.
// See the silent assert() in esp_libc's assert.h.
void __hal_abort_silent(void) __asm__("abort");
#define HAL_ASSERT(__e) (__builtin_expect(!!(__e), 1) ? (void)0 : __extension__ ({ \
        __hal_abort_silent(); \
        __asm__ __volatile__("" :: "i"(__COUNTER__)); \
        __builtin_unreachable(); \
    }))
#elif CONFIG_HAL_DEFAULT_ASSERTION_LEVEL == 2 // full assertion
#define HAL_ASSERT(__e) (__builtin_expect(!!(__e), 1) ? (void)0 : __assert_func(__FILE__, __LINE__, __ASSERT_FUNC, #__e))
#else // no assert
#define HAL_ASSERT(__e) (__builtin_expect(!!(__e), 1) ? (void)0 : __builtin_unreachable())
#endif

#ifdef __cplusplus
}
#endif

/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void __assert_func(const char *file, int line, const char *func, const char *expr);
extern void abort(void);

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
#define HAL_ASSERT(__e) (__builtin_expect(!!(__e), 1) ? (void)0 : abort())
#elif CONFIG_HAL_DEFAULT_ASSERTION_LEVEL == 2 // full assertion
#define HAL_ASSERT(__e) (__builtin_expect(!!(__e), 1) ? (void)0 : __assert_func(__FILE__, __LINE__, __ASSERT_FUNC, #__e))
#else // no assert
#define HAL_ASSERT(__e) ((void)(__e))
#endif

#ifdef __cplusplus
}
#endif

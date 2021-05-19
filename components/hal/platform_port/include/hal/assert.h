// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
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

#if CONFIG_HAL_DEFAULT_ASSERTION_LEVEL == 1 // silent
#define HAL_ASSERT(__e) (__builtin_expect(!!(__e), 1) ? (void)0 : abort())
#elif CONFIG_HAL_DEFAULT_ASSERTION_LEVEL == 2 // full assertion
#define HAL_ASSERT(__e) (__builtin_expect(!!(__e), 1) ? (void)0 : __assert_func(__FILE__, __LINE__, __ASSERT_FUNC, #__e))
#else // no assert
#define HAL_ASSERT(__e) ((void)(__e))
#endif

#ifdef __cplusplus
}
#endif

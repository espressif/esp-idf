// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
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

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
#include "esp_compiler.h"

#include_next <assert.h>

#if defined(CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_SILENT) && !defined(NDEBUG)
    #undef assert
    #define assert(__e) (likely(__e)) ? (void)0 : abort()
#else 
    /* moved part of toolchain provided assert to there then
     * we can tweak the original assert macro to perform likely
     * before deliver it to original toolchain implementation 
     */
    #undef assert
    #ifdef NDEBUG 
    # define assert(__e) ((void)0)
    #else
    # define assert(__e) (likely(__e) ? (void)0 : __assert_func (__FILE__, __LINE__, \
                                __ASSERT_FUNC, #__e))
    #endif                            
#endif

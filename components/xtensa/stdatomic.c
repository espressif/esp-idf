// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

//replacement for gcc built-in functions

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "xtensa/config/core-isa.h"


#define CMP_EXCHANGE(n, type) bool __atomic_compare_exchange_ ## n (type* mem, type* expect, type desired, int success, int failure) \
{ \
    bool ret = false; \
    unsigned state = portENTER_CRITICAL_NESTED(); \
    if (*mem == *expect) { \
        ret = true; \
        *mem = desired; \
    } else { \
        *expect = *mem; \
    } \
    portEXIT_CRITICAL_NESTED(state); \
    return ret; \
}

#define FETCH_ADD(n, type) type __atomic_fetch_add_ ## n (type* ptr, type value, int memorder) \
{ \
    unsigned state = portENTER_CRITICAL_NESTED(); \
    type ret = *ptr; \
    *ptr = *ptr + value; \
    portEXIT_CRITICAL_NESTED(state); \
    return ret; \
}

#define FETCH_SUB(n, type) type __atomic_fetch_sub_ ## n (type* ptr, type value, int memorder) \
{ \
    unsigned state = portENTER_CRITICAL_NESTED(); \
    type ret = *ptr; \
    *ptr = *ptr - value; \
    portEXIT_CRITICAL_NESTED(state); \
    return ret; \
}


//this piece of code should only be compiled if the cpu doesn't support atomic compare and swap (s32c1i)
#if XCHAL_HAVE_S32C1I == 0

#pragma GCC diagnostic ignored "-Wbuiltin-declaration-mismatch"

CMP_EXCHANGE(1, uint8_t)
CMP_EXCHANGE(2, uint16_t)
CMP_EXCHANGE(4, uint32_t)
CMP_EXCHANGE(8, uint64_t)

FETCH_ADD(1, uint8_t)
FETCH_ADD(2, uint16_t)
FETCH_ADD(4, uint32_t)
FETCH_ADD(8, uint64_t)

FETCH_SUB(1, uint8_t)
FETCH_SUB(2, uint16_t)
FETCH_SUB(4, uint32_t)
FETCH_SUB(8, uint64_t)

#endif
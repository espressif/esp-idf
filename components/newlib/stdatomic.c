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
#include <stdbool.h>
#include <stdint.h>

#ifdef __XTENSA__

#include "xtensa/config/core-isa.h"
#include "xtensa/xtruntime.h"

// This allows nested interrupts disabling and restoring via local registers or stack.
// They can be called from interrupts too.
// WARNING: Only applies to current CPU.
#define _ATOMIC_ENTER_CRITICAL(void) ({ \
	unsigned state = XTOS_SET_INTLEVEL(XCHAL_EXCM_LEVEL); \
	atomic_benchmark_intr_disable(); \
	state; \
})

#define _ATOMIC_EXIT_CRITICAL(state)   do { \
    atomic_benchmark_intr_restore(state); \
    XTOS_RESTORE_JUST_INTLEVEL(state); \
    } while (0)

#ifndef XCHAL_HAVE_S32C1I
#error "XCHAL_HAVE_S32C1I not defined, include correct header!"
#endif

#define NO_ATOMICS_SUPPORT (XCHAL_HAVE_S32C1I == 0)
#else // RISCV

#include "freertos/portmacro.h"

// This allows nested interrupts disabling and restoring via local registers or stack.
// They can be called from interrupts too.
// WARNING: Only applies to current CPU.
#define _ATOMIC_ENTER_CRITICAL(void) ({ \
	unsigned state = portENTER_CRITICAL_NESTED(); \
	atomic_benchmark_intr_disable(); \
	state; \
})

#define _ATOMIC_EXIT_CRITICAL(state)   do { \
    atomic_benchmark_intr_restore(state); \
    portEXIT_CRITICAL_NESTED(state); \
    } while (0)

#define NO_ATOMICS_SUPPORT 1    // [todo] Get the equivalent XCHAL_HAVE_S32C1I check for RISCV 

#endif


//reserved to measure atomic operation time
#define atomic_benchmark_intr_disable()
#define atomic_benchmark_intr_restore(STATE)

#define CMP_EXCHANGE(n, type) bool __atomic_compare_exchange_ ## n (type* mem, type* expect, type desired, int success, int failure) \
{ \
    bool ret = false; \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    if (*mem == *expect) { \
        ret = true; \
        *mem = desired; \
    } else { \
        *expect = *mem; \
    } \
    _ATOMIC_EXIT_CRITICAL(state); \
    return ret; \
}

#define FETCH_ADD(n, type) type __atomic_fetch_add_ ## n (type* ptr, type value, int memorder) \
{ \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    type ret = *ptr; \
    *ptr = *ptr + value; \
    _ATOMIC_EXIT_CRITICAL(state); \
    return ret; \
}

#define FETCH_SUB(n, type) type __atomic_fetch_sub_ ## n (type* ptr, type value, int memorder) \
{ \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    type ret = *ptr; \
    *ptr = *ptr - value; \
    _ATOMIC_EXIT_CRITICAL(state); \
    return ret; \
}

#define FETCH_AND(n, type) type __atomic_fetch_and_ ## n (type* ptr, type value, int memorder) \
{ \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    type ret = *ptr; \
    *ptr = *ptr & value; \
    _ATOMIC_EXIT_CRITICAL(state); \
    return ret; \
}

#define FETCH_OR(n, type) type __atomic_fetch_or_ ## n (type* ptr, type value, int memorder) \
{ \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    type ret = *ptr; \
    *ptr = *ptr | value; \
    _ATOMIC_EXIT_CRITICAL(state); \
    return ret; \
}

#define FETCH_XOR(n, type) type __atomic_fetch_xor_ ## n (type* ptr, type value, int memorder) \
{ \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    type ret = *ptr; \
    *ptr = *ptr ^ value; \
    _ATOMIC_EXIT_CRITICAL(state); \
    return ret; \
}

#pragma GCC diagnostic ignored "-Wbuiltin-declaration-mismatch"

#if NO_ATOMICS_SUPPORT

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

FETCH_AND(1, uint8_t)
FETCH_AND(2, uint16_t)
FETCH_AND(4, uint32_t)
FETCH_AND(8, uint64_t)

FETCH_OR(1, uint8_t)
FETCH_OR(2, uint16_t)
FETCH_OR(4, uint32_t)
FETCH_OR(8, uint64_t)

FETCH_XOR(1, uint8_t)
FETCH_XOR(2, uint16_t)
FETCH_XOR(4, uint32_t)
FETCH_XOR(8, uint64_t)

#endif

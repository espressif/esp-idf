/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

//replacement for gcc built-in functions

#include "sdkconfig.h"
#include <stdbool.h>
#include <stdint.h>
#include "soc/soc_caps.h"
#include "freertos/FreeRTOS.h"

#ifdef __XTENSA__
#include "xtensa/config/core-isa.h"

#ifndef XCHAL_HAVE_S32C1I
#error "XCHAL_HAVE_S32C1I not defined, include correct header!"
#endif

#define HAS_ATOMICS_32 (XCHAL_HAVE_S32C1I == 1)
// no 64-bit atomics on Xtensa
#define HAS_ATOMICS_64 0

#else // RISCV

// GCC toolchain will define this pre-processor if "A" extension is supported
#ifndef __riscv_atomic
#define __riscv_atomic      0
#endif

#define HAS_ATOMICS_32 (__riscv_atomic == 1)
#define HAS_ATOMICS_64 ((__riscv_atomic == 1) && (__riscv_xlen == 64))
#endif // (__XTENSA__, __riscv)

#if SOC_CPU_CORES_NUM == 1

// Single core SoC: atomics can be implemented using portSET_INTERRUPT_MASK_FROM_ISR
// and portCLEAR_INTERRUPT_MASK_FROM_ISR, which disables and enables interrupts.
#define _ATOMIC_ENTER_CRITICAL() ({ \
    unsigned state = portSET_INTERRUPT_MASK_FROM_ISR(); \
    state; \
})

#define _ATOMIC_EXIT_CRITICAL(state)   do { \
    portCLEAR_INTERRUPT_MASK_FROM_ISR(state); \
    } while (0)

#else // SOC_CPU_CORES_NUM

_Static_assert(HAS_ATOMICS_32, "32-bit atomics should be supported if SOC_CPU_CORES_NUM > 1");
// Only need to implement 64-bit atomics here. Use a single global portMUX_TYPE spinlock
// to emulate the atomics.
static portMUX_TYPE s_atomic_lock = portMUX_INITIALIZER_UNLOCKED;

// Return value is not used but kept for compatibility with the single-core version above.
#define _ATOMIC_ENTER_CRITICAL() ({ \
    portENTER_CRITICAL_SAFE(&s_atomic_lock); \
    0; \
})

#define _ATOMIC_EXIT_CRITICAL(state)  do { \
    (void) (state); \
    portEXIT_CRITICAL_SAFE(&s_atomic_lock); \
} while(0)

#endif // SOC_CPU_CORES_NUM

#ifdef __clang__
// Clang doesn't allow to define "__sync_*" atomics. The workaround is to define function with name "__sync_*_builtin",
// which implements "__sync_*" atomic functionality and use asm directive to set the value of symbol "__sync_*" to the name
// of defined function.

#define CLANG_ATOMIC_SUFFIX(name_) name_ ## _builtin
#define CLANG_DECLARE_ALIAS(name_) \
__asm__(".type " # name_ ", @function\n"        \
        ".global " #name_ "\n"                  \
        ".equ " #name_ ", " #name_ "_builtin");

#else // __clang__

#define CLANG_ATOMIC_SUFFIX(name_) name_
#define CLANG_DECLARE_ALIAS(name_)

#endif // __clang__

#define ATOMIC_LOAD(n, type) type __atomic_load_ ## n (const type* mem, int memorder) \
{                                                   \
    unsigned state = _ATOMIC_ENTER_CRITICAL();      \
    type ret = *mem;                                \
    _ATOMIC_EXIT_CRITICAL(state);                   \
    return ret;                                     \
}

#define ATOMIC_STORE(n, type) void __atomic_store_ ## n (type* mem, type val, int memorder) \
{                                                   \
    unsigned state = _ATOMIC_ENTER_CRITICAL();      \
    *mem = val;                                     \
    _ATOMIC_EXIT_CRITICAL(state);                   \
}

#define ATOMIC_EXCHANGE(n, type) type __atomic_exchange_ ## n (type* mem, type val, int memorder) \
{                                                   \
    unsigned state = _ATOMIC_ENTER_CRITICAL();      \
    type ret = *mem;                                \
    *mem = val;                                     \
    _ATOMIC_EXIT_CRITICAL(state);                   \
    return ret;                                     \
}

#define CMP_EXCHANGE(n, type) bool __atomic_compare_exchange_ ## n (type* mem, type* expect, type desired, bool weak, int success, int failure) \
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

#define ADD_FETCH(n, type) type __atomic_add_fetch_ ## n (type* ptr, type value, int memorder) \
{ \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    type ret = *ptr + value; \
    *ptr = ret; \
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

#define SUB_FETCH(n, type) type __atomic_sub_fetch_ ## n (type* ptr, type value, int memorder) \
{ \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    type ret = *ptr - value; \
    *ptr = ret; \
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

#define AND_FETCH(n, type) type __atomic_and_fetch_ ## n (type* ptr, type value, int memorder) \
{ \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    type ret = *ptr & value; \
    *ptr = ret; \
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

#define OR_FETCH(n, type) type __atomic_or_fetch_ ## n (type* ptr, type value, int memorder) \
{ \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    type ret = *ptr | value; \
    *ptr = ret; \
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

#define XOR_FETCH(n, type) type __atomic_xor_fetch_ ## n (type* ptr, type value, int memorder) \
{ \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    type ret = *ptr ^ value; \
    *ptr = ret; \
    _ATOMIC_EXIT_CRITICAL(state); \
    return ret; \
}

#define FETCH_NAND(n, type) type __atomic_fetch_nand_ ## n (type* ptr, type value, int memorder) \
{ \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    type ret = *ptr; \
    *ptr = ~(*ptr & value); \
    _ATOMIC_EXIT_CRITICAL(state); \
    return ret; \
}

#define NAND_FETCH(n, type) type __atomic_nand_fetch_ ## n (type* ptr, type value, int memorder) \
{ \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    type ret = ~(*ptr & value); \
    *ptr = ret; \
    _ATOMIC_EXIT_CRITICAL(state); \
    return ret; \
}

#define SYNC_FETCH_OP(op, n, type) type CLANG_ATOMIC_SUFFIX(__sync_fetch_and_ ## op ##_ ## n) (type* ptr, type value) \
{                                                                                \
    return __atomic_fetch_ ## op ##_ ## n (ptr, value, __ATOMIC_SEQ_CST);        \
}                                                                                \
CLANG_DECLARE_ALIAS( __sync_fetch_and_ ## op ##_ ## n )

#define SYNC_OP_FETCH(op, n, type) type CLANG_ATOMIC_SUFFIX(__sync_ ## op ##_and_fetch_ ## n) (type* ptr, type value) \
{                                                                                \
    return __atomic_ ## op ##_fetch_ ## n (ptr, value, __ATOMIC_SEQ_CST);        \
}                                                                                \
CLANG_DECLARE_ALIAS( __sync_ ## op ##_and_fetch_ ## n )

#define SYNC_BOOL_CMP_EXCHANGE(n, type) bool  CLANG_ATOMIC_SUFFIX(__sync_bool_compare_and_swap_ ## n)  (type *ptr, type oldval, type newval) \
{                                                                                \
    bool ret = false;                                                            \
    unsigned state = _ATOMIC_ENTER_CRITICAL();                                   \
    if (*ptr == oldval) {                                                        \
        *ptr = newval;                                                           \
        ret = true;                                                              \
    }                                                                            \
    _ATOMIC_EXIT_CRITICAL(state);                                                \
    return ret;                                                                  \
}                                                                                \
CLANG_DECLARE_ALIAS( __sync_bool_compare_and_swap_ ## n )

#define SYNC_VAL_CMP_EXCHANGE(n, type) type  CLANG_ATOMIC_SUFFIX(__sync_val_compare_and_swap_ ## n)  (type *ptr, type oldval, type newval) \
{                                                                                \
    unsigned state = _ATOMIC_ENTER_CRITICAL();                                   \
    type ret = *ptr;                                                             \
    if (*ptr == oldval) {                                                        \
        *ptr = newval;                                                           \
    }                                                                            \
    _ATOMIC_EXIT_CRITICAL(state);                                                \
    return ret;                                                                  \
}                                                                                \
CLANG_DECLARE_ALIAS( __sync_val_compare_and_swap_ ## n )

#define SYNC_LOCK_TEST_AND_SET(n, type) type  CLANG_ATOMIC_SUFFIX(__sync_lock_test_and_set_ ## n)  (type *ptr, type val) \
{                                                                                \
    unsigned state = _ATOMIC_ENTER_CRITICAL();                                   \
    type ret = *ptr;                                                             \
    *ptr = val;                                                                  \
    _ATOMIC_EXIT_CRITICAL(state);                                                \
    return ret;                                                                  \
}                                                                                \
CLANG_DECLARE_ALIAS( __sync_lock_test_and_set_ ## n )

#define SYNC_LOCK_RELEASE(n, type) void  CLANG_ATOMIC_SUFFIX(__sync_lock_release_ ## n)  (type *ptr) \
{                                                                                \
    unsigned state = _ATOMIC_ENTER_CRITICAL();                                   \
    *ptr = 0;                                                                    \
    _ATOMIC_EXIT_CRITICAL(state);                                                \
}                                                                                \
CLANG_DECLARE_ALIAS( __sync_lock_release_ ## n )


#if !HAS_ATOMICS_32

ATOMIC_EXCHANGE(1, uint8_t)
ATOMIC_EXCHANGE(2, uint16_t)
ATOMIC_EXCHANGE(4, uint32_t)

CMP_EXCHANGE(1, uint8_t)
CMP_EXCHANGE(2, uint16_t)
CMP_EXCHANGE(4, uint32_t)

FETCH_ADD(1, uint8_t)
FETCH_ADD(2, uint16_t)
FETCH_ADD(4, uint32_t)

ADD_FETCH(1, uint8_t)
ADD_FETCH(2, uint16_t)
ADD_FETCH(4, uint32_t)

FETCH_SUB(1, uint8_t)
FETCH_SUB(2, uint16_t)
FETCH_SUB(4, uint32_t)

SUB_FETCH(1, uint8_t)
SUB_FETCH(2, uint16_t)
SUB_FETCH(4, uint32_t)

FETCH_AND(1, uint8_t)
FETCH_AND(2, uint16_t)
FETCH_AND(4, uint32_t)

AND_FETCH(1, uint8_t)
AND_FETCH(2, uint16_t)
AND_FETCH(4, uint32_t)

FETCH_OR(1, uint8_t)
FETCH_OR(2, uint16_t)
FETCH_OR(4, uint32_t)

OR_FETCH(1, uint8_t)
OR_FETCH(2, uint16_t)
OR_FETCH(4, uint32_t)

FETCH_XOR(1, uint8_t)
FETCH_XOR(2, uint16_t)
FETCH_XOR(4, uint32_t)

XOR_FETCH(1, uint8_t)
XOR_FETCH(2, uint16_t)
XOR_FETCH(4, uint32_t)

FETCH_NAND(1, uint8_t)
FETCH_NAND(2, uint16_t)
FETCH_NAND(4, uint32_t)

NAND_FETCH(1, uint8_t)
NAND_FETCH(2, uint16_t)
NAND_FETCH(4, uint32_t)

SYNC_FETCH_OP(add, 1, uint8_t)
SYNC_FETCH_OP(add, 2, uint16_t)
SYNC_FETCH_OP(add, 4, uint32_t)

SYNC_OP_FETCH(add, 1, uint8_t)
SYNC_OP_FETCH(add, 2, uint16_t)
SYNC_OP_FETCH(add, 4, uint32_t)

SYNC_FETCH_OP(sub, 1, uint8_t)
SYNC_FETCH_OP(sub, 2, uint16_t)
SYNC_FETCH_OP(sub, 4, uint32_t)

SYNC_OP_FETCH(sub, 1, uint8_t)
SYNC_OP_FETCH(sub, 2, uint16_t)
SYNC_OP_FETCH(sub, 4, uint32_t)

SYNC_FETCH_OP(and, 1, uint8_t)
SYNC_FETCH_OP(and, 2, uint16_t)
SYNC_FETCH_OP(and, 4, uint32_t)

SYNC_OP_FETCH(and, 1, uint8_t)
SYNC_OP_FETCH(and, 2, uint16_t)
SYNC_OP_FETCH(and, 4, uint32_t)

SYNC_FETCH_OP(or, 1, uint8_t)
SYNC_FETCH_OP(or, 2, uint16_t)
SYNC_FETCH_OP(or, 4, uint32_t)

SYNC_OP_FETCH(or, 1, uint8_t)
SYNC_OP_FETCH(or, 2, uint16_t)
SYNC_OP_FETCH(or, 4, uint32_t)

SYNC_FETCH_OP(xor, 1, uint8_t)
SYNC_FETCH_OP(xor, 2, uint16_t)
SYNC_FETCH_OP(xor, 4, uint32_t)

SYNC_OP_FETCH(xor, 1, uint8_t)
SYNC_OP_FETCH(xor, 2, uint16_t)
SYNC_OP_FETCH(xor, 4, uint32_t)

SYNC_FETCH_OP(nand, 1, uint8_t)
SYNC_FETCH_OP(nand, 2, uint16_t)
SYNC_FETCH_OP(nand, 4, uint32_t)

SYNC_OP_FETCH(nand, 1, uint8_t)
SYNC_OP_FETCH(nand, 2, uint16_t)
SYNC_OP_FETCH(nand, 4, uint32_t)

SYNC_BOOL_CMP_EXCHANGE(1, uint8_t)
SYNC_BOOL_CMP_EXCHANGE(2, uint16_t)
SYNC_BOOL_CMP_EXCHANGE(4, uint32_t)

SYNC_VAL_CMP_EXCHANGE(1, uint8_t)
SYNC_VAL_CMP_EXCHANGE(2, uint16_t)
SYNC_VAL_CMP_EXCHANGE(4, uint32_t)


SYNC_LOCK_TEST_AND_SET(1, uint8_t)
SYNC_LOCK_TEST_AND_SET(2, uint16_t)
SYNC_LOCK_TEST_AND_SET(4, uint32_t)

SYNC_LOCK_RELEASE(1, uint8_t)
SYNC_LOCK_RELEASE(2, uint16_t)
SYNC_LOCK_RELEASE(4, uint32_t)

// LLVM has not implemented native atomic load/stores for riscv targets without the Atomic extension. LLVM thread: https://reviews.llvm.org/D47553.
// Even though GCC does transform them, these libcalls need to be available for the case where a LLVM based project links against IDF.
ATOMIC_LOAD(1, uint8_t)
ATOMIC_LOAD(2, uint16_t)
ATOMIC_LOAD(4, uint32_t)
ATOMIC_STORE(1, uint8_t)
ATOMIC_STORE(2, uint16_t)
ATOMIC_STORE(4, uint32_t)

#endif // !HAS_ATOMICS_32

#if !HAS_ATOMICS_64

ATOMIC_EXCHANGE(8, uint64_t)

CMP_EXCHANGE(8, uint64_t)

FETCH_ADD(8, uint64_t)

FETCH_SUB(8, uint64_t)

FETCH_AND(8, uint64_t)

FETCH_OR(8, uint64_t)

FETCH_XOR(8, uint64_t)

FETCH_NAND(8, uint64_t)

ADD_FETCH(8, uint64_t)

SUB_FETCH(8, uint64_t)

AND_FETCH(8, uint64_t)

OR_FETCH(8, uint64_t)

XOR_FETCH(8, uint64_t)

NAND_FETCH(8, uint64_t)

SYNC_FETCH_OP(add, 8, uint64_t)

SYNC_FETCH_OP(sub, 8, uint64_t)

SYNC_FETCH_OP(and, 8, uint64_t)

SYNC_FETCH_OP(or, 8, uint64_t)

SYNC_FETCH_OP(xor, 8, uint64_t)

SYNC_FETCH_OP(nand, 8, uint64_t)

SYNC_OP_FETCH(add, 8, uint64_t)

SYNC_OP_FETCH(sub, 8, uint64_t)

SYNC_OP_FETCH(and, 8, uint64_t)

SYNC_OP_FETCH(or, 8, uint64_t)

SYNC_OP_FETCH(xor, 8, uint64_t)

SYNC_OP_FETCH(nand, 8, uint64_t)

SYNC_BOOL_CMP_EXCHANGE(8, uint64_t)

SYNC_VAL_CMP_EXCHANGE(8, uint64_t)

SYNC_LOCK_TEST_AND_SET(8, uint64_t)
SYNC_LOCK_RELEASE(8, uint64_t)

// LLVM has not implemented native atomic load/stores for riscv targets without the Atomic extension. LLVM thread: https://reviews.llvm.org/D47553.
// Even though GCC does transform them, these libcalls need to be available for the case where a LLVM based project links against IDF.
ATOMIC_LOAD(8, uint64_t)
ATOMIC_STORE(8, uint64_t)

#endif // !HAS_ATOMICS_64

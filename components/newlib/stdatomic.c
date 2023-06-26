/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

//replacement for gcc built-in functions

#include "sdkconfig.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
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
#if CONFIG_FREERTOS_SMP
#define _ATOMIC_ENTER_CRITICAL() ({ \
    unsigned state = portDISABLE_INTERRUPTS(); \
    state; \
})

#define _ATOMIC_EXIT_CRITICAL(state)   do { \
    portRESTORE_INTERRUPTS(state); \
    } while (0)
#else // CONFIG_FREERTOS_SMP
#define _ATOMIC_ENTER_CRITICAL() ({ \
    unsigned state = portSET_INTERRUPT_MASK_FROM_ISR(); \
    state; \
})

#define _ATOMIC_EXIT_CRITICAL(state)   do { \
    portCLEAR_INTERRUPT_MASK_FROM_ISR(state); \
    } while (0)
#endif
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

#define ATOMIC_LOAD(n, type) type __atomic_load_ ## n (const volatile void* mem, int memorder) \
{                                                   \
    unsigned state = _ATOMIC_ENTER_CRITICAL();      \
    type ret = *(const volatile type*)mem;                                \
    _ATOMIC_EXIT_CRITICAL(state);                   \
    return ret;                                     \
}

#define ATOMIC_STORE(n, type) void __atomic_store_ ## n (volatile void * mem, type val, int memorder) \
{                                                   \
    unsigned state = _ATOMIC_ENTER_CRITICAL();      \
    *(volatile type *)mem = val;                                     \
    _ATOMIC_EXIT_CRITICAL(state);                   \
}

#define ATOMIC_EXCHANGE(n, type) type __atomic_exchange_ ## n (volatile void* mem, type val, int memorder) \
{                                                   \
    unsigned state = _ATOMIC_ENTER_CRITICAL();      \
    type ret = *(volatile type*)mem;                                \
    *(volatile type*)mem = val;                                     \
    _ATOMIC_EXIT_CRITICAL(state);                   \
    return ret;                                     \
}

#define CMP_EXCHANGE(n, type) bool __atomic_compare_exchange_ ## n (volatile void* mem, void* expect, type desired, bool weak, int success, int failure) \
{ \
    bool ret = false; \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    if (*(volatile type*)mem == *(type*)expect) { \
        ret = true; \
        *(volatile type*)mem = desired; \
    } else { \
        *(type*)expect = *(volatile type*)mem; \
    } \
    _ATOMIC_EXIT_CRITICAL(state); \
    return ret; \
}

#define FETCH_ADD(n, type) type __atomic_fetch_add_ ## n (volatile void* ptr, type value, int memorder) \
{ \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    type ret = *(volatile type*)ptr; \
    *(volatile type*)ptr = *(volatile type*)ptr + value; \
    _ATOMIC_EXIT_CRITICAL(state); \
    return ret; \
}

#define ADD_FETCH(n, type) type __atomic_add_fetch_ ## n (volatile void* ptr, type value, int memorder) \
{ \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    type ret = *(volatile type*)ptr + value; \
    *(volatile type*)ptr = ret; \
    _ATOMIC_EXIT_CRITICAL(state); \
    return ret; \
}

#define FETCH_SUB(n, type) type __atomic_fetch_sub_ ## n (volatile void* ptr, type value, int memorder) \
{ \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    type ret = *(volatile type*)ptr; \
    *(volatile type*)ptr = *(volatile type*)ptr - value; \
    _ATOMIC_EXIT_CRITICAL(state); \
    return ret; \
}

#define SUB_FETCH(n, type) type __atomic_sub_fetch_ ## n (volatile void* ptr, type value, int memorder) \
{ \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    type ret = *(volatile type*)ptr - value; \
    *(volatile type*)ptr = ret; \
    _ATOMIC_EXIT_CRITICAL(state); \
    return ret; \
}

#define FETCH_AND(n, type) type __atomic_fetch_and_ ## n (volatile void* ptr, type value, int memorder) \
{ \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    type ret = *(volatile type*)ptr; \
    *(volatile type*)ptr = *(volatile type*)ptr & value; \
    _ATOMIC_EXIT_CRITICAL(state); \
    return ret; \
}

#define AND_FETCH(n, type) type __atomic_and_fetch_ ## n (volatile void* ptr, type value, int memorder) \
{ \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    type ret = *(volatile type*)ptr & value; \
    *(volatile type*)ptr = ret; \
    _ATOMIC_EXIT_CRITICAL(state); \
    return ret; \
}

#define FETCH_OR(n, type) type __atomic_fetch_or_ ## n (volatile void* ptr, type value, int memorder) \
{ \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    type ret = *(volatile type*)ptr; \
    *(volatile type*)ptr = *(volatile type*)ptr | value; \
    _ATOMIC_EXIT_CRITICAL(state); \
    return ret; \
}

#define OR_FETCH(n, type) type __atomic_or_fetch_ ## n (volatile void* ptr, type value, int memorder) \
{ \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    type ret = *(volatile type*)ptr | value; \
    *(volatile type*)ptr = ret; \
    _ATOMIC_EXIT_CRITICAL(state); \
    return ret; \
}

#define FETCH_XOR(n, type) type __atomic_fetch_xor_ ## n (volatile void* ptr, type value, int memorder) \
{ \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    type ret = *(volatile type*)ptr; \
    *(volatile type*)ptr = *(volatile type*)ptr ^ value; \
    _ATOMIC_EXIT_CRITICAL(state); \
    return ret; \
}

#define XOR_FETCH(n, type) type __atomic_xor_fetch_ ## n (volatile void* ptr, type value, int memorder) \
{ \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    type ret = *(volatile type*)ptr ^ value; \
    *(volatile type*)ptr = ret; \
    _ATOMIC_EXIT_CRITICAL(state); \
    return ret; \
}

#define FETCH_NAND(n, type) type __atomic_fetch_nand_ ## n (volatile void* ptr, type value, int memorder) \
{ \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    type ret = *(volatile type*)ptr; \
    *(volatile type*)ptr = ~(*(volatile type*)ptr & value); \
    _ATOMIC_EXIT_CRITICAL(state); \
    return ret; \
}

#define NAND_FETCH(n, type) type __atomic_nand_fetch_ ## n (volatile void* ptr, type value, int memorder) \
{ \
    unsigned state = _ATOMIC_ENTER_CRITICAL(); \
    type ret = ~(*(volatile type*)ptr & value); \
    *(volatile type*)ptr = ret; \
    _ATOMIC_EXIT_CRITICAL(state); \
    return ret; \
}

#define SYNC_FETCH_OP(op, n, type) type CLANG_ATOMIC_SUFFIX(__sync_fetch_and_ ## op ##_ ## n) (volatile void* ptr, type value) \
{                                                                                \
    return __atomic_fetch_ ## op ##_ ## n (ptr, value, __ATOMIC_SEQ_CST);        \
}                                                                                \
CLANG_DECLARE_ALIAS( __sync_fetch_and_ ## op ##_ ## n )

#define SYNC_OP_FETCH(op, n, type) type CLANG_ATOMIC_SUFFIX(__sync_ ## op ##_and_fetch_ ## n) (volatile void* ptr, type value) \
{                                                                                \
    return __atomic_ ## op ##_fetch_ ## n (ptr, value, __ATOMIC_SEQ_CST);        \
}                                                                                \
CLANG_DECLARE_ALIAS( __sync_ ## op ##_and_fetch_ ## n )

#define SYNC_BOOL_CMP_EXCHANGE(n, type) bool  CLANG_ATOMIC_SUFFIX(__sync_bool_compare_and_swap_ ## n)  (volatile void* ptr, type oldval, type newval) \
{                                                                                \
    bool ret = false;                                                            \
    unsigned state = _ATOMIC_ENTER_CRITICAL();                                   \
    if (*(volatile type*)ptr == oldval) {                                                        \
        *(volatile type*)ptr = newval;                                                           \
        ret = true;                                                              \
    }                                                                            \
    _ATOMIC_EXIT_CRITICAL(state);                                                \
    return ret;                                                                  \
}                                                                                \
CLANG_DECLARE_ALIAS( __sync_bool_compare_and_swap_ ## n )

#define SYNC_VAL_CMP_EXCHANGE(n, type) type  CLANG_ATOMIC_SUFFIX(__sync_val_compare_and_swap_ ## n)  (volatile void* ptr, type oldval, type newval) \
{                                                                                \
    unsigned state = _ATOMIC_ENTER_CRITICAL();                                   \
    type ret = *(volatile type*)ptr;                                                             \
    if (*(volatile type*)ptr == oldval) {                                                        \
        *(volatile type*)ptr = newval;                                                           \
    }                                                                            \
    _ATOMIC_EXIT_CRITICAL(state);                                                \
    return ret;                                                                  \
}                                                                                \
CLANG_DECLARE_ALIAS( __sync_val_compare_and_swap_ ## n )

#define SYNC_LOCK_TEST_AND_SET(n, type) type  CLANG_ATOMIC_SUFFIX(__sync_lock_test_and_set_ ## n)  (volatile void* ptr, type val) \
{                                                                                \
    unsigned state = _ATOMIC_ENTER_CRITICAL();                                   \
    type ret = *(volatile type*)ptr;                                                             \
    *(volatile type*)ptr = val;                                                                  \
    _ATOMIC_EXIT_CRITICAL(state);                                                \
    return ret;                                                                  \
}                                                                                \
CLANG_DECLARE_ALIAS( __sync_lock_test_and_set_ ## n )

#define SYNC_LOCK_RELEASE(n, type) void  CLANG_ATOMIC_SUFFIX(__sync_lock_release_ ## n)  (volatile void* ptr) \
{                                                                                \
    unsigned state = _ATOMIC_ENTER_CRITICAL();                                   \
    *(volatile type*)ptr = 0;                                                                    \
    _ATOMIC_EXIT_CRITICAL(state);                                                \
}                                                                                \
CLANG_DECLARE_ALIAS( __sync_lock_release_ ## n )


#if !HAS_ATOMICS_32

_Static_assert(sizeof(unsigned char) == 1, "atomics require a 1-byte type");
_Static_assert(sizeof(short unsigned int) == 2, "atomics require a 2-bytes type");
_Static_assert(sizeof(unsigned int) == 4, "atomics require a 4-bytes type");

ATOMIC_EXCHANGE(1, unsigned char)
ATOMIC_EXCHANGE(2, short unsigned int)
ATOMIC_EXCHANGE(4, unsigned int)

CMP_EXCHANGE(1, unsigned char)
CMP_EXCHANGE(2, short unsigned int)
CMP_EXCHANGE(4, unsigned int)

FETCH_ADD(1, unsigned char)
FETCH_ADD(2, short unsigned int)
FETCH_ADD(4, unsigned int)

ADD_FETCH(1, unsigned char)
ADD_FETCH(2, short unsigned int)
ADD_FETCH(4, unsigned int)

FETCH_SUB(1, unsigned char)
FETCH_SUB(2, short unsigned int)
FETCH_SUB(4, unsigned int)

SUB_FETCH(1, unsigned char)
SUB_FETCH(2, short unsigned int)
SUB_FETCH(4, unsigned int)

FETCH_AND(1, unsigned char)
FETCH_AND(2, short unsigned int)
FETCH_AND(4, unsigned int)

AND_FETCH(1, unsigned char)
AND_FETCH(2, short unsigned int)
AND_FETCH(4, unsigned int)

FETCH_OR(1, unsigned char)
FETCH_OR(2, short unsigned int)
FETCH_OR(4, unsigned int)

OR_FETCH(1, unsigned char)
OR_FETCH(2, short unsigned int)
OR_FETCH(4, unsigned int)

FETCH_XOR(1, unsigned char)
FETCH_XOR(2, short unsigned int)
FETCH_XOR(4, unsigned int)

XOR_FETCH(1, unsigned char)
XOR_FETCH(2, short unsigned int)
XOR_FETCH(4, unsigned int)

FETCH_NAND(1, unsigned char)
FETCH_NAND(2, short unsigned int)
FETCH_NAND(4, unsigned int)

NAND_FETCH(1, unsigned char)
NAND_FETCH(2, short unsigned int)
NAND_FETCH(4, unsigned int)

SYNC_FETCH_OP(add, 1, unsigned char)
SYNC_FETCH_OP(add, 2, short unsigned int)
SYNC_FETCH_OP(add, 4, unsigned int)

SYNC_OP_FETCH(add, 1, unsigned char)
SYNC_OP_FETCH(add, 2, short unsigned int)
SYNC_OP_FETCH(add, 4, unsigned int)

SYNC_FETCH_OP(sub, 1, unsigned char)
SYNC_FETCH_OP(sub, 2, short unsigned int)
SYNC_FETCH_OP(sub, 4, unsigned int)

SYNC_OP_FETCH(sub, 1, unsigned char)
SYNC_OP_FETCH(sub, 2, short unsigned int)
SYNC_OP_FETCH(sub, 4, unsigned int)

SYNC_FETCH_OP(and, 1, unsigned char)
SYNC_FETCH_OP(and, 2, short unsigned int)
SYNC_FETCH_OP(and, 4, unsigned int)

SYNC_OP_FETCH(and, 1, unsigned char)
SYNC_OP_FETCH(and, 2, short unsigned int)
SYNC_OP_FETCH(and, 4, unsigned int)

SYNC_FETCH_OP(or, 1, unsigned char)
SYNC_FETCH_OP(or, 2, short unsigned int)
SYNC_FETCH_OP(or, 4, unsigned int)

SYNC_OP_FETCH(or, 1, unsigned char)
SYNC_OP_FETCH(or, 2, short unsigned int)
SYNC_OP_FETCH(or, 4, unsigned int)

SYNC_FETCH_OP(xor, 1, unsigned char)
SYNC_FETCH_OP(xor, 2, short unsigned int)
SYNC_FETCH_OP(xor, 4, unsigned int)

SYNC_OP_FETCH(xor, 1, unsigned char)
SYNC_OP_FETCH(xor, 2, short unsigned int)
SYNC_OP_FETCH(xor, 4, unsigned int)

SYNC_FETCH_OP(nand, 1, unsigned char)
SYNC_FETCH_OP(nand, 2, short unsigned int)
SYNC_FETCH_OP(nand, 4, unsigned int)

SYNC_OP_FETCH(nand, 1, unsigned char)
SYNC_OP_FETCH(nand, 2, short unsigned int)
SYNC_OP_FETCH(nand, 4, unsigned int)

SYNC_BOOL_CMP_EXCHANGE(1, unsigned char)
SYNC_BOOL_CMP_EXCHANGE(2, short unsigned int)
SYNC_BOOL_CMP_EXCHANGE(4, unsigned int)

SYNC_VAL_CMP_EXCHANGE(1, unsigned char)
SYNC_VAL_CMP_EXCHANGE(2, short unsigned int)
SYNC_VAL_CMP_EXCHANGE(4, unsigned int)


SYNC_LOCK_TEST_AND_SET(1, unsigned char)
SYNC_LOCK_TEST_AND_SET(2, short unsigned int)
SYNC_LOCK_TEST_AND_SET(4, unsigned int)

SYNC_LOCK_RELEASE(1, unsigned char)
SYNC_LOCK_RELEASE(2, short unsigned int)
SYNC_LOCK_RELEASE(4, unsigned int)

// LLVM has not implemented native atomic load/stores for riscv targets without the Atomic extension. LLVM thread: https://reviews.llvm.org/D47553.
// Even though GCC does transform them, these libcalls need to be available for the case where a LLVM based project links against IDF.
ATOMIC_LOAD(1, unsigned char)
ATOMIC_LOAD(2, short unsigned int)
ATOMIC_LOAD(4, unsigned int)
ATOMIC_STORE(1, unsigned char)
ATOMIC_STORE(2, short unsigned int)
ATOMIC_STORE(4, unsigned int)

#elif __riscv_atomic == 1

bool CLANG_ATOMIC_SUFFIX(__atomic_always_lock_free) (unsigned int size, const volatile void *) {
  return size <= sizeof(int);
}
CLANG_DECLARE_ALIAS( __atomic_always_lock_free)

bool CLANG_ATOMIC_SUFFIX(__atomic_is_lock_free) (unsigned int size, const volatile void *) {
  return size <= sizeof(int);
}
CLANG_DECLARE_ALIAS( __atomic_is_lock_free)

#endif // !HAS_ATOMICS_32

#if !HAS_ATOMICS_64

_Static_assert(sizeof(long long unsigned int) == 8, "atomics require a 8-bytes type");

ATOMIC_EXCHANGE(8, long long unsigned int)

CMP_EXCHANGE(8, long long unsigned int)

FETCH_ADD(8, long long unsigned int)

FETCH_SUB(8, long long unsigned int)

FETCH_AND(8, long long unsigned int)

FETCH_OR(8, long long unsigned int)

FETCH_XOR(8, long long unsigned int)

FETCH_NAND(8, long long unsigned int)

ADD_FETCH(8, long long unsigned int)

SUB_FETCH(8, long long unsigned int)

AND_FETCH(8, long long unsigned int)

OR_FETCH(8, long long unsigned int)

XOR_FETCH(8, long long unsigned int)

NAND_FETCH(8, long long unsigned int)

SYNC_FETCH_OP(add, 8, long long unsigned int)

SYNC_FETCH_OP(sub, 8, long long unsigned int)

SYNC_FETCH_OP(and, 8, long long unsigned int)

SYNC_FETCH_OP(or, 8, long long unsigned int)

SYNC_FETCH_OP(xor, 8, long long unsigned int)

SYNC_FETCH_OP(nand, 8, long long unsigned int)

SYNC_OP_FETCH(add, 8, long long unsigned int)

SYNC_OP_FETCH(sub, 8, long long unsigned int)

SYNC_OP_FETCH(and, 8, long long unsigned int)

SYNC_OP_FETCH(or, 8, long long unsigned int)

SYNC_OP_FETCH(xor, 8, long long unsigned int)

SYNC_OP_FETCH(nand, 8, long long unsigned int)

SYNC_BOOL_CMP_EXCHANGE(8, long long unsigned int)

SYNC_VAL_CMP_EXCHANGE(8, long long unsigned int)

SYNC_LOCK_TEST_AND_SET(8, long long unsigned int)
SYNC_LOCK_RELEASE(8, long long unsigned int)

// LLVM has not implemented native atomic load/stores for riscv targets without the Atomic extension. LLVM thread: https://reviews.llvm.org/D47553.
// Even though GCC does transform them, these libcalls need to be available for the case where a LLVM based project links against IDF.
ATOMIC_LOAD(8, long long unsigned int)
ATOMIC_STORE(8, long long unsigned int)

#endif // !HAS_ATOMICS_64

// Clang generates calls to the __atomic_load/__atomic_store functions for object size more then 4 bytes
void CLANG_ATOMIC_SUFFIX( __atomic_load ) (size_t size, const volatile void *src, void *dest, int model) {
    unsigned state = _ATOMIC_ENTER_CRITICAL();
    memcpy(dest, (const void *)src, size);
    _ATOMIC_EXIT_CRITICAL(state);
}
CLANG_DECLARE_ALIAS( __atomic_load )

void CLANG_ATOMIC_SUFFIX( __atomic_store ) (size_t size, volatile void *dest, void *src, int model) {
    unsigned state = _ATOMIC_ENTER_CRITICAL();
    memcpy((void *)dest, (const void *)src, size);
    _ATOMIC_EXIT_CRITICAL(state);
}
CLANG_DECLARE_ALIAS( __atomic_store)

bool CLANG_ATOMIC_SUFFIX(__atomic_compare_exchange) (size_t size, volatile void *ptr, void *expected, void *desired, int success_memorder, int failure_memorder) {
    bool ret = false;
    unsigned state = _ATOMIC_ENTER_CRITICAL();
    if (!memcmp((void *)ptr, expected, size)) {
        memcpy((void *)ptr, (const void *)desired, size);
        ret = true;
    } else {
        memcpy((void *)expected, (const void *)ptr, size);
    }
    _ATOMIC_EXIT_CRITICAL(state);
    return ret;
}
CLANG_DECLARE_ALIAS( __atomic_compare_exchange)

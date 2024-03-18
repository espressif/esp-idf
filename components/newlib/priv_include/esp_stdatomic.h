/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "freertos/FreeRTOS.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"

#ifdef __XTENSA__
#include "xtensa/config/core-isa.h"

#ifndef XCHAL_HAVE_S32C1I
#error "XCHAL_HAVE_S32C1I not defined, include correct header!"
#endif // XCHAL_HAVE_S32C1I

#ifndef CONFIG_STDATOMIC_S32C1I_SPIRAM_WORKAROUND
#define CONFIG_STDATOMIC_S32C1I_SPIRAM_WORKAROUND 0
#endif

#define HAS_ATOMICS_32 ((XCHAL_HAVE_S32C1I == 1) && !CONFIG_STDATOMIC_S32C1I_SPIRAM_WORKAROUND)

// no 64-bit atomics on Xtensa
#define HAS_ATOMICS_64 0
#else // RISCV
// GCC toolchain will define this pre-processor if "A" extension is supported
#ifndef __riscv_atomic
#define __riscv_atomic 0
#endif

#define HAS_ATOMICS_32 (__riscv_atomic == 1)
#define HAS_ATOMICS_64 ((__riscv_atomic == 1) && (__riscv_xlen == 64))
#endif // (__XTENSA__, __riscv)

#if SOC_CPU_CORES_NUM == 1
// Single core SoC: atomics can be implemented using portSET_INTERRUPT_MASK_FROM_ISR
// and portCLEAR_INTERRUPT_MASK_FROM_ISR, which disables and enables interrupts.
#if CONFIG_FREERTOS_SMP
#define _ATOMIC_ENTER_CRITICAL() unsigned int state = portDISABLE_INTERRUPTS();
#define _ATOMIC_EXIT_CRITICAL() portRESTORE_INTERRUPTS(state)
#else // CONFIG_FREERTOS_SMP
#define _ATOMIC_ENTER_CRITICAL() unsigned int state = portSET_INTERRUPT_MASK_FROM_ISR()
#define _ATOMIC_EXIT_CRITICAL() portCLEAR_INTERRUPT_MASK_FROM_ISR(state)
#endif // CONFIG_FREERTOS_SMP
#else // SOC_CPU_CORES_NUM

#define _ATOMIC_ENTER_CRITICAL() portENTER_CRITICAL_SAFE(&s_atomic_lock);
#define _ATOMIC_EXIT_CRITICAL() portEXIT_CRITICAL_SAFE(&s_atomic_lock);

#endif // SOC_CPU_CORES_NUM

#if CONFIG_STDATOMIC_S32C1I_SPIRAM_WORKAROUND

#define _ATOMIC_IF_NOT_EXT_RAM() \
    if (!((uintptr_t)ptr >= SOC_EXTRAM_DATA_LOW && (uintptr_t) ptr < SOC_EXTRAM_DATA_HIGH))

#define _ATOMIC_HW_STUB_OP_FUNCTION(n, type, name_1, name_2) \
    _ATOMIC_IF_NOT_EXT_RAM() { \
        type __atomic_s32c1i_ ##name_1 ##_ ##name_2 ##_ ##n (volatile void* ptr, type value, int memorder); \
        return __atomic_s32c1i_ ##name_1 ##_ ##name_2 ##_ ##n (ptr, value, memorder); \
    }

#define _ATOMIC_HW_STUB_EXCHANGE(n, type) \
    _ATOMIC_IF_NOT_EXT_RAM() { \
        type __atomic_s32c1i_exchange_ ## n (volatile void* ptr, type value, int memorder); \
        return __atomic_s32c1i_exchange_ ## n (ptr, value, memorder); \
    }

#define _ATOMIC_HW_STUB_STORE(n, type) \
    _ATOMIC_IF_NOT_EXT_RAM() { \
        void __atomic_s32c1i_store_ ## n (volatile void * ptr, type value, int memorder); \
        __atomic_s32c1i_store_ ## n (ptr, value, memorder); \
        return; \
    }

#define _ATOMIC_HW_STUB_CMP_EXCHANGE(n, type) \
    _ATOMIC_IF_NOT_EXT_RAM() { \
        bool __atomic_s32c1i_compare_exchange_ ## n (volatile void* ptr, void* expected, type desired, bool weak, int success, int failure); \
        return __atomic_s32c1i_compare_exchange_ ## n (ptr, expected, desired, weak, success, failure); \
    }

#define _ATOMIC_HW_STUB_LOAD(n, type) \
    _ATOMIC_IF_NOT_EXT_RAM() { \
        type __atomic_s32c1i_load_ ## n (const volatile void* ptr, int memorder); \
        return __atomic_s32c1i_load_ ## n (ptr,memorder); \
    }

#define _ATOMIC_HW_STUB_SYNC_BOOL_CMP_EXCHANGE(n, type) \
    _ATOMIC_IF_NOT_EXT_RAM() { \
        bool __sync_s32c1i_bool_compare_and_swap_ ## n (volatile void* ptr, type expected, type desired); \
        return __sync_s32c1i_bool_compare_and_swap_ ## n (ptr, expected, desired); \
    }

#define _ATOMIC_HW_STUB_SYNC_VAL_CMP_EXCHANGE(n, type) \
    _ATOMIC_IF_NOT_EXT_RAM() { \
        type __sync_s32c1i_val_compare_and_swap_ ## n (volatile void* ptr, type expected, type desired); \
        return __sync_s32c1i_val_compare_and_swap_ ## n (ptr, expected, desired); \
    }

#define _ATOMIC_HW_STUB_SYNC_LOCK_TEST_AND_SET(n, type) \
    _ATOMIC_IF_NOT_EXT_RAM() { \
        type __sync_s32c1i_lock_test_and_set_ ## n (volatile void* ptr, type value); \
        return __sync_s32c1i_lock_test_and_set_ ## n (ptr, value); \
    }

#define _ATOMIC_HW_STUB_SYNC_LOCK_RELEASE(n, type) \
    _ATOMIC_IF_NOT_EXT_RAM() { \
        void __sync_s32c1i_lock_release_ ## n (volatile void* ptr); \
        __sync_s32c1i_lock_release_ ## n (ptr); \
        return; \
    }

#else // CONFIG_STDATOMIC_S32C1I_SPIRAM_WORKAROUND

#define _ATOMIC_HW_STUB_OP_FUNCTION(n, type, name_1, name_2)
#define _ATOMIC_HW_STUB_EXCHANGE(n, type)
#define _ATOMIC_HW_STUB_STORE(n, type)
#define _ATOMIC_HW_STUB_CMP_EXCHANGE(n, type)
#define _ATOMIC_HW_STUB_LOAD(n, type)
#define _ATOMIC_HW_STUB_SYNC_BOOL_CMP_EXCHANGE(n, type)
#define _ATOMIC_HW_STUB_SYNC_VAL_CMP_EXCHANGE(n, type)
#define _ATOMIC_HW_STUB_SYNC_LOCK_TEST_AND_SET(n, type)
#define _ATOMIC_HW_STUB_SYNC_LOCK_RELEASE(n, type)

#endif // CONFIG_STDATOMIC_S32C1I_SPIRAM_WORKAROUND

#ifdef __clang__
// Clang doesn't allow to define "__sync_*" atomics. The workaround is to define function with name "__sync_*_builtin",
// which implements "__sync_*" atomic functionality and use asm directive to set the value of symbol "__sync_*" to the name
// of defined function.

#define CLANG_ATOMIC_SUFFIX(name_) name_ ## _builtin
#define CLANG_DECLARE_ALIAS(name_) \
__asm__(".type " # name_ ", @function\n" \
        ".global " #name_ "\n" \
        ".equ " #name_ ", " #name_ "_builtin");

#else // __clang__

#define CLANG_ATOMIC_SUFFIX(name_) name_
#define CLANG_DECLARE_ALIAS(name_)

#endif // __clang__

#define ATOMIC_OP_FUNCTIONS(n, type, name, operation, inverse) \
    _ATOMIC_OP_FUNCTION(n, type, fetch, name, old, operation, inverse) \
    _ATOMIC_OP_FUNCTION(n, type, name, fetch, new, operation, inverse)

#define _ATOMIC_OP_FUNCTION(n, type, name_1, name_2, ret_var, operation, inverse) \
type __atomic_ ##name_1 ##_ ##name_2 ##_ ##n (volatile void* ptr, type value, int memorder) \
{ \
    type old, new; \
    _ATOMIC_HW_STUB_OP_FUNCTION(n, type, name_1, name_2); \
    _ATOMIC_ENTER_CRITICAL(); \
    old = (*(volatile type*)ptr); \
    new = inverse(old operation value); \
    *(volatile type*)ptr = new; \
    _ATOMIC_EXIT_CRITICAL(); \
    return ret_var; \
}

#define ATOMIC_LOAD(n, type) \
type __atomic_load_ ## n (const volatile void* ptr, int memorder) \
{ \
    type old; \
    _ATOMIC_HW_STUB_LOAD(n, type); \
    _ATOMIC_ENTER_CRITICAL(); \
    old = *(const volatile type*)ptr; \
    _ATOMIC_EXIT_CRITICAL(); \
    return old; \
}

#define ATOMIC_CMP_EXCHANGE(n, type) \
bool __atomic_compare_exchange_ ## n (volatile void* ptr, void* expected, type desired, bool weak, int success, int failure) \
{ \
    bool ret = false; \
    _ATOMIC_HW_STUB_CMP_EXCHANGE(n, type); \
    _ATOMIC_ENTER_CRITICAL(); \
    if (*(volatile type*)ptr == *(type*)expected) { \
        ret = true; \
        *(volatile type*)ptr = desired; \
    } else { \
        *(type*)expected = *(volatile type*)ptr; \
    } \
    _ATOMIC_EXIT_CRITICAL(); \
    return ret; \
}

#define ATOMIC_STORE(n, type) \
void __atomic_store_ ## n (volatile void * ptr, type value, int memorder) \
{ \
    _ATOMIC_HW_STUB_STORE(n, type); \
    _ATOMIC_ENTER_CRITICAL(); \
    *(volatile type*)ptr = value; \
    _ATOMIC_EXIT_CRITICAL(); \
}

#define ATOMIC_EXCHANGE(n, type) \
type __atomic_exchange_ ## n (volatile void* ptr, type value, int memorder) \
{ \
    type old; \
    _ATOMIC_HW_STUB_EXCHANGE(n, type); \
    _ATOMIC_ENTER_CRITICAL(); \
    old = *(volatile type*)ptr; \
    *(volatile type*)ptr = value; \
    _ATOMIC_EXIT_CRITICAL(); \
    return old; \
}

#define SYNC_OP_FUNCTIONS(n, type, name) \
    _SYNC_OP_FUNCTION(n, type, fetch, name) \
    _SYNC_OP_FUNCTION(n, type, name, fetch)

#define _SYNC_OP_FUNCTION(n, type, name_1, name_2) \
type CLANG_ATOMIC_SUFFIX(__sync_ ##name_1 ##_and_ ##name_2 ##_ ##n) (volatile void* ptr, type value) \
{ \
    return __atomic_ ##name_1 ##_ ##name_2 ##_ ##n (ptr, value, __ATOMIC_SEQ_CST); \
} \
CLANG_DECLARE_ALIAS( __sync_##name_1 ##_and_ ##name_2 ##_ ##n )

#define SYNC_BOOL_CMP_EXCHANGE(n, type) \
bool CLANG_ATOMIC_SUFFIX(__sync_bool_compare_and_swap_ ## n)  (volatile void* ptr, type expected, type desired) \
{ \
    bool ret = false; \
    _ATOMIC_HW_STUB_SYNC_BOOL_CMP_EXCHANGE(n, type); \
    _ATOMIC_ENTER_CRITICAL(); \
    if (*(volatile type*)ptr == expected) { \
        *(volatile type*)ptr = desired; \
        ret = true; \
    } \
    _ATOMIC_EXIT_CRITICAL(); \
    return ret; \
} \
CLANG_DECLARE_ALIAS( __sync_bool_compare_and_swap_ ## n )

#define SYNC_VAL_CMP_EXCHANGE(n, type) \
type CLANG_ATOMIC_SUFFIX(__sync_val_compare_and_swap_ ## n) (volatile void* ptr, type expected, type desired) \
{ \
    type old; \
    _ATOMIC_HW_STUB_SYNC_VAL_CMP_EXCHANGE(n, type); \
    _ATOMIC_ENTER_CRITICAL(); \
    old = *(volatile type*)ptr; \
    if (old == expected) { \
        *(volatile type*)ptr = desired; \
    } \
    _ATOMIC_EXIT_CRITICAL(); \
    return old; \
} \
CLANG_DECLARE_ALIAS( __sync_val_compare_and_swap_ ## n )

#define SYNC_LOCK_TEST_AND_SET(n, type) \
type CLANG_ATOMIC_SUFFIX(__sync_lock_test_and_set_ ## n) (volatile void* ptr, type value) \
{ \
    type old; \
    _ATOMIC_HW_STUB_SYNC_LOCK_TEST_AND_SET(n, type); \
    _ATOMIC_ENTER_CRITICAL(); \
    old = *(volatile type*)ptr; \
    *(volatile type*)ptr = value; \
    _ATOMIC_EXIT_CRITICAL(); \
    return old; \
} \
CLANG_DECLARE_ALIAS( __sync_lock_test_and_set_ ## n )

#define SYNC_LOCK_RELEASE(n, type) \
void CLANG_ATOMIC_SUFFIX(__sync_lock_release_ ## n) (volatile void* ptr) \
{ \
    _ATOMIC_HW_STUB_SYNC_LOCK_RELEASE(n, type); \
    _ATOMIC_ENTER_CRITICAL(); \
    *(volatile type*)ptr = 0; \
    _ATOMIC_EXIT_CRITICAL(); \
} \
CLANG_DECLARE_ALIAS( __sync_lock_release_ ## n )

#define ATOMIC_FUNCTIONS(n, type) \
    ATOMIC_EXCHANGE(n, type) \
    ATOMIC_CMP_EXCHANGE(n, type) \
    ATOMIC_OP_FUNCTIONS(n, type, add, +, ) \
    ATOMIC_OP_FUNCTIONS(n, type, sub, -, ) \
    ATOMIC_OP_FUNCTIONS(n, type, and, &, ) \
    ATOMIC_OP_FUNCTIONS(n, type, or, |, ) \
    ATOMIC_OP_FUNCTIONS(n, type, xor, ^, ) \
    ATOMIC_OP_FUNCTIONS(n, type, nand, &, ~) \
/* LLVM has not implemented native atomic load/stores for riscv targets without the Atomic extension. LLVM thread: https://reviews.llvm.org/D47553. \
 * Even though GCC does transform them, these libcalls need to be available for the case where a LLVM based project links against IDF. */ \
    ATOMIC_LOAD(n, type) \
    ATOMIC_STORE(n, type) \
    SYNC_OP_FUNCTIONS(n, type, add) \
    SYNC_OP_FUNCTIONS(n, type, sub) \
    SYNC_OP_FUNCTIONS(n, type, and) \
    SYNC_OP_FUNCTIONS(n, type, or) \
    SYNC_OP_FUNCTIONS(n, type, xor) \
    SYNC_OP_FUNCTIONS(n, type, nand) \
    SYNC_BOOL_CMP_EXCHANGE(n, type) \
    SYNC_VAL_CMP_EXCHANGE(n, type) \
    SYNC_LOCK_TEST_AND_SET(n, type) \
    SYNC_LOCK_RELEASE(n, type)

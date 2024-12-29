/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

//replacement for gcc built-in functions

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "esp_stdatomic.h"
#include "freertos/FreeRTOS.h"
#include "sdkconfig.h"

#if SOC_CPU_CORES_NUM > 1
#if !CONFIG_STDATOMIC_S32C1I_SPIRAM_WORKAROUND
_Static_assert(HAS_ATOMICS_32, "32-bit atomics should be supported if SOC_CPU_CORES_NUM > 1");
#endif // CONFIG_STDATOMIC_S32C1I_SPIRAM_WORKAROUND
// Only need to implement 64-bit atomics here. Use a single global portMUX_TYPE spinlock
// to emulate the atomics.
static portMUX_TYPE s_atomic_lock = portMUX_INITIALIZER_UNLOCKED;
#endif

#if !HAS_ATOMICS_32

_Static_assert(sizeof(unsigned char) == 1, "atomics require a 1-byte type");
_Static_assert(sizeof(short unsigned int) == 2, "atomics require a 2-bytes type");
_Static_assert(sizeof(unsigned int) == 4, "atomics require a 4-bytes type");

ATOMIC_FUNCTIONS(1, unsigned char)
ATOMIC_FUNCTIONS(2, short unsigned int)
ATOMIC_FUNCTIONS(4, unsigned int)

#elif __riscv_atomic == 1

bool CLANG_ATOMIC_SUFFIX(__atomic_always_lock_free)(unsigned int size, const volatile void *)
{
    return size <= sizeof(int);
}
CLANG_DECLARE_ALIAS(__atomic_always_lock_free)

bool CLANG_ATOMIC_SUFFIX(__atomic_is_lock_free)(unsigned int size, const volatile void *)
{
    return size <= sizeof(int);
}
CLANG_DECLARE_ALIAS(__atomic_is_lock_free)

#endif // !HAS_ATOMICS_32

#if !HAS_ATOMICS_64

#if CONFIG_STDATOMIC_S32C1I_SPIRAM_WORKAROUND
#undef _ATOMIC_HW_STUB_OP_FUNCTION
#undef _ATOMIC_HW_STUB_EXCHANGE
#undef _ATOMIC_HW_STUB_STORE
#undef _ATOMIC_HW_STUB_CMP_EXCHANGE
#undef _ATOMIC_HW_STUB_LOAD
#undef _ATOMIC_HW_STUB_SYNC_BOOL_CMP_EXCHANGE
#undef _ATOMIC_HW_STUB_SYNC_VAL_CMP_EXCHANGE
#undef _ATOMIC_HW_STUB_SYNC_LOCK_TEST_AND_SET
#undef _ATOMIC_HW_STUB_SYNC_LOCK_RELEASE

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

_Static_assert(sizeof(long long unsigned int) == 8, "atomics require a 8-bytes type");

ATOMIC_FUNCTIONS(8, long long unsigned int)

#endif // !HAS_ATOMICS_64

// Clang generates calls to the __atomic_load/__atomic_store functions for object size more then 4 bytes
void CLANG_ATOMIC_SUFFIX(__atomic_load)(size_t size, const volatile void *src, void *dest, int model)
{
    _ATOMIC_ENTER_CRITICAL();
    memcpy(dest, (const void *)src, size);
    _ATOMIC_EXIT_CRITICAL();
}
CLANG_DECLARE_ALIAS(__atomic_load)

void CLANG_ATOMIC_SUFFIX(__atomic_store)(size_t size, volatile void *dest, void *src, int model)
{
    _ATOMIC_ENTER_CRITICAL();
    memcpy((void *)dest, (const void *)src, size);
    _ATOMIC_EXIT_CRITICAL();
}
CLANG_DECLARE_ALIAS(__atomic_store)

bool CLANG_ATOMIC_SUFFIX(__atomic_compare_exchange)(size_t size, volatile void *ptr, void *expected, void *desired, int success_memorder, int failure_memorder)
{
    bool ret = false;
    _ATOMIC_ENTER_CRITICAL();
    if (!memcmp((void *)ptr, expected, size)) {
        memcpy((void *)ptr, (const void *)desired, size);
        ret = true;
    } else {
        memcpy((void *)expected, (const void *)ptr, size);
    }
    _ATOMIC_EXIT_CRITICAL();
    return ret;
}
CLANG_DECLARE_ALIAS(__atomic_compare_exchange)

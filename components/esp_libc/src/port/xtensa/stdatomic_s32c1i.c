/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include <stdint.h>
#include "esp_stdatomic.h"

#undef SYNC_OP_FUNCTIONS
#undef _ATOMIC_OP_FUNCTION
#undef ATOMIC_LOAD
#undef ATOMIC_CMP_EXCHANGE
#undef ATOMIC_STORE
#undef ATOMIC_EXCHANGE
#undef SYNC_BOOL_CMP_EXCHANGE
#undef SYNC_VAL_CMP_EXCHANGE
#undef SYNC_LOCK_TEST_AND_SET
#undef SYNC_LOCK_RELEASE

#define SYNC_OP_FUNCTIONS(n, type, name)

#define _ATOMIC_OP_FUNCTION(n, type, name_1, name_2, ret_var, operation, inverse) \
type __atomic_s32c1i_ ##name_1 ##_ ##name_2 ##_ ##n (volatile void* ptr, type value, int memorder) \
{ \
    return __atomic_ ##name_1 ##_ ##name_2 ##_ ##n (ptr, value, memorder); \
}

#define ATOMIC_LOAD(n, type) \
type __atomic_s32c1i_load_ ## n (const volatile void* ptr, int memorder) \
{ \
    return __atomic_load_ ## n (ptr, memorder); \
}

#define ATOMIC_CMP_EXCHANGE(n, type) \
bool __atomic_s32c1i_compare_exchange_ ## n (volatile void* ptr, void* expected, type desired, bool weak, int success, int failure) \
{ \
    return __atomic_compare_exchange_ ## n (ptr, expected, desired, weak, success, failure); \
}

#define ATOMIC_STORE(n, type) \
void __atomic_s32c1i_store_ ## n (volatile void * ptr, type value, int memorder) \
{ \
    __atomic_store_ ## n (ptr, value, memorder); \
}

#define ATOMIC_EXCHANGE(n, type) \
type __atomic_s32c1i_exchange_ ## n (volatile void* ptr, type value, int memorder) \
{ \
    return __atomic_exchange_ ## n (ptr, value, memorder); \
}

#define SYNC_BOOL_CMP_EXCHANGE(n, type) \
bool __sync_s32c1i_bool_compare_and_swap_ ## n (volatile void* ptr, type expected, type desired) \
{ \
    return __sync_bool_compare_and_swap_ ## n (ptr, expected, desired); \
}

#define SYNC_VAL_CMP_EXCHANGE(n, type) \
type __sync_s32c1i_val_compare_and_swap_ ## n (volatile void* ptr, type expected, type desired) \
{ \
    return __sync_val_compare_and_swap_ ## n (ptr, expected, desired); \
}

#define SYNC_LOCK_TEST_AND_SET(n, type) \
type __sync_s32c1i_lock_test_and_set_ ## n (volatile void* ptr, type value) \
{ \
    return __sync_lock_test_and_set_ ## n  (ptr, value); \
}

#define SYNC_LOCK_RELEASE(n, type) \
void __sync_s32c1i_lock_release_ ## n (volatile void* ptr) \
{ \
    __sync_lock_release_ ## n (ptr); \
}

ATOMIC_FUNCTIONS(1, unsigned char)
ATOMIC_FUNCTIONS(2, short unsigned int)
ATOMIC_FUNCTIONS(4, unsigned int)

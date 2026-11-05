/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef OSI_ATOMIC_H_
#define OSI_ATOMIC_H_

#include <stdbool.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "esp_memory_utils.h"
#include "sdkconfig.h"

/*
 * Each osi_atomic_* below matches one __atomic_* builtin, for a uint32_t
 * that carries its own fallback lock. Internal RAM uses the builtin and
 * passes the memory order through. Any other address takes the lock, which
 * is already a full barrier, so the order is unused on that path.
 */

#if defined(__XTENSA__)
#include "xtensa/config/core-isa.h"
#if XCHAL_HAVE_S32C1I && !CONFIG_STDATOMIC_S32C1I_SPIRAM_WORKAROUND
#define OSI_ATOMIC_U32_NATIVE 1
#endif
#elif defined(__riscv_atomic) && (__riscv_atomic == 1)
#define OSI_ATOMIC_U32_NATIVE 1
#endif

#ifndef OSI_ATOMIC_U32_NATIVE
#define OSI_ATOMIC_U32_NATIVE 0
#endif

typedef struct {
    uint32_t value;
    portMUX_TYPE lock;
    bool native;
} osi_atomic_u32_t;

/* Static initializer: value is set, lock is unlocked, native is false so the
 * fallback spinlock is used until osi_atomic_u32_init() can probe the address. */
#define OSI_ATOMIC_U32_INITIALIZER(value_) \
    { (value_), (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED, false }

static inline void osi_atomic_u32_init(osi_atomic_u32_t *atomic, uint32_t value)
{
    atomic->value = value;
    atomic->lock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
#if OSI_ATOMIC_U32_NATIVE
    atomic->native = esp_ptr_internal(&atomic->value);
#else
    atomic->native = false;
#endif
}

static inline __attribute__((always_inline))
uint32_t osi_atomic_load_n(osi_atomic_u32_t *atomic, int memorder)
{
    uint32_t value;

#if OSI_ATOMIC_U32_NATIVE
    if (atomic->native) {
        return __atomic_load_n(&atomic->value, memorder);
    }
#else
    (void)memorder;
#endif
    portENTER_CRITICAL_SAFE(&atomic->lock);
    value = atomic->value;
    portEXIT_CRITICAL_SAFE(&atomic->lock);
    return value;
}

static inline __attribute__((always_inline))
void osi_atomic_store_n(osi_atomic_u32_t *atomic, uint32_t value, int memorder)
{
#if OSI_ATOMIC_U32_NATIVE
    if (atomic->native) {
        __atomic_store_n(&atomic->value, value, memorder);
        return;
    }
#else
    (void)memorder;
#endif
    portENTER_CRITICAL_SAFE(&atomic->lock);
    atomic->value = value;
    portEXIT_CRITICAL_SAFE(&atomic->lock);
}

static inline __attribute__((always_inline))
bool osi_atomic_compare_exchange_n(osi_atomic_u32_t *atomic, uint32_t *expected,
                                   uint32_t desired, bool weak,
                                   int success_memorder, int failure_memorder)
{
    bool exchanged = false;

#if OSI_ATOMIC_U32_NATIVE
    if (atomic->native) {
        return __atomic_compare_exchange_n(&atomic->value, expected, desired, weak,
                                           success_memorder, failure_memorder);
    }
#else
    (void)weak;
    (void)success_memorder;
    (void)failure_memorder;
#endif
    portENTER_CRITICAL_SAFE(&atomic->lock);
    if (atomic->value == *expected) {
        atomic->value = desired;
        exchanged = true;
    } else {
        *expected = atomic->value;
    }
    portEXIT_CRITICAL_SAFE(&atomic->lock);
    return exchanged;
}

static inline __attribute__((always_inline))
uint32_t osi_atomic_fetch_sub(osi_atomic_u32_t *atomic, uint32_t value, int memorder)
{
    uint32_t previous;

#if OSI_ATOMIC_U32_NATIVE
    if (atomic->native) {
        return __atomic_fetch_sub(&atomic->value, value, memorder);
    }
#else
    (void)memorder;
#endif
    portENTER_CRITICAL_SAFE(&atomic->lock);
    previous = atomic->value;
    atomic->value = previous - value;
    portEXIT_CRITICAL_SAFE(&atomic->lock);
    return previous;
}

#endif /* OSI_ATOMIC_H_ */

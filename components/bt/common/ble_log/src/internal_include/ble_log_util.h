/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BLE_LOG_UTIL_H__
#define __BLE_LOG_UTIL_H__

/* ----------------- */
/* BLE Log - Utility */
/* ----------------- */

/* INCLUDE */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "esp_bit_defs.h"
#include "freertos/FreeRTOS.h"

#ifndef UNIT_TEST
#include "esp_heap_caps.h"
#include "esp_rom_serial_output.h"
#endif /* !UNIT_TEST */

/* MACRO */
/* Unit test */
#ifndef UNIT_TEST

/* Reference counting macros */
#define BLE_LOG_REF_COUNT_ACQUIRE(VAR)          __atomic_fetch_add(VAR, 1, __ATOMIC_ACQUIRE)
#define BLE_LOG_REF_COUNT_RELEASE(VAR)          __atomic_fetch_sub(VAR, 1, __ATOMIC_RELEASE)

/* Specifier */
#define BLE_LOG_STATIC                          static
#define BLE_LOG_INLINE                          inline

/* Section */
#define BLE_LOG_IRAM_ATTR                       IRAM_ATTR

/* Memory operation */
#define BLE_LOG_MEM_CAP                         (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA)
#define BLE_LOG_MALLOC(size)                    heap_caps_malloc(size, BLE_LOG_MEM_CAP)
#define BLE_LOG_FREE(ptr)                       heap_caps_free(ptr)
#define BLE_LOG_MEMCPY(dst, src, len)           memcpy(dst, src, len)
#define BLE_LOG_MEMSET(ptr, value, len)         memset(ptr, value, len)

/* Critical section wrapper */
extern portMUX_TYPE ble_log_spin_lock;
#define BLE_LOG_ENTER_CRITICAL()                portENTER_CRITICAL_SAFE(&ble_log_spin_lock);
#define BLE_LOG_EXIT_CRITICAL()                 portEXIT_CRITICAL_SAFE(&ble_log_spin_lock);

#define BLE_LOG_ACQUIRE_SPIN_LOCK(spin_lock)    portENTER_CRITICAL_SAFE(spin_lock)
#define BLE_LOG_RELEASE_SPIN_LOCK(spin_lock)    portEXIT_CRITICAL_SAFE(spin_lock)

#define BLE_LOG_IN_ISR()                        xPortInIsrContext()
#define BLE_LOG_CONSOLE                         esp_rom_printf
#define BLE_LOG_ASSERT(expr)                    assert(expr)

extern void esp_panic_handler_feed_wdts(void);
#define BLE_LOG_FEED_WDT()                      esp_panic_handler_feed_wdts()

/* INLINE */
BLE_LOG_IRAM_ATTR static inline
bool ble_log_cas_acquire(volatile bool *cas_lock)
{
    bool expected = false;
    return __atomic_compare_exchange_n(
        cas_lock, &expected, true, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED
    );
}

BLE_LOG_IRAM_ATTR static inline
void ble_log_cas_release(volatile bool *cas_lock)
{
    __atomic_store_n(cas_lock, false, __ATOMIC_RELEASE);
}

#else /* UNIT_TEST */

/* Reference counting macros */
#define BLE_LOG_REF_COUNT_ACQUIRE(VAR)          (*VAR)++
#define BLE_LOG_REF_COUNT_RELEASE(VAR)          (*VAR)--

/* Specifier*/
#define BLE_LOG_STATIC
#define BLE_LOG_INLINE

/* Section */
#define BLE_LOG_IRAM_ATTR

/* Memory operation */
void *mocked_malloc(size_t size);
void mocked_free(void *ptr);
void mocked_memcpy(void *dst, const void *src, size_t len);
void mocked_memset(void *ptr, int value, size_t len);
#define BLE_LOG_MALLOC(size)                    mocked_malloc(size)
#define BLE_LOG_FREE(ptr)                       mocked_free(ptr)
#define BLE_LOG_MEMCPY(dst, src, len)           mocked_memcpy(dst, src, len)
#define BLE_LOG_MEMSET(ptr, value, len)         mocked_memset(ptr, value, len)

/* Critical section wrapper */
void mocked_enter_critical(void);
void mocked_exit_critical(void);
#define BLE_LOG_ENTER_CRITICAL()                mocked_enter_critical()
#define BLE_LOG_EXIT_CRITICAL()                 mocked_exit_critical()

/* FreeRTOS API wrapper */
bool mocked_in_isr(void);
#define BLE_LOG_IN_ISR()                        mocked_in_isr()

/* Spin lock wrapper */
void mocked_acquire_spin_lock(void *spin_lock);
void mocked_release_spin_lock(void *spin_lock);
#define BLE_LOG_ACQUIRE_SPIN_LOCK(spin_lock)    mocked_acquire_spin_lock(spin_lock)
#define BLE_LOG_RELEASE_SPIN_LOCK(spin_lock)    mocked_release_spin_lock(spin_lock)

/* Printf wrapper */
void mocked_printf(const char *fmt, ...);
#define BLE_LOG_CONSOLE                         mocked_printf

/* Assert wrapper */
void mocked_assert(bool expr);
#define BLE_LOG_ASSERT(expr)                    mocked_assert(expr)

#define BLE_LOG_FEED_WDT()

bool ble_log_cas_acquire(volatile bool *cas_lock);
void ble_log_cas_release(volatile bool *cas_lock);
#endif /* UNIT_TEST */

#define BLE_LOG_VERSION                         (3)

/* TYPEDEF */
typedef enum {
    BLE_LOG_INT_SRC_INIT_DONE,
    BLE_LOG_INT_SRC_TS,
    BLE_LOG_INT_SRC_ENH_STAT,
    BLE_LOG_INT_SRC_INFO,
    BLE_LOG_INT_SRC_FLUSH,
    BLE_LOG_INT_SRC_MAX,
} ble_log_int_src_t;

typedef struct {
    uint8_t int_src_code;
    uint8_t version;
} __attribute__((packed)) ble_log_info_t;

/* INTERFACE */
uint32_t ble_log_fast_checksum(const uint8_t *data, size_t len);

#endif /* __BLE_LOG_UTIL_H__ */

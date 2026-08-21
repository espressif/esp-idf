/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
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

#include "esp_heap_caps.h"
#include "esp_rom_serial_output.h"

/* MACRO */
#define BLE_LOG_ATOMIC_LOAD_ACQUIRE(VAR)         __atomic_load_n(&(VAR), __ATOMIC_ACQUIRE)
#define BLE_LOG_ATOMIC_LOAD_RELAXED(VAR)         __atomic_load_n(&(VAR), __ATOMIC_RELAXED)
#define BLE_LOG_ATOMIC_STORE_RELEASE(VAR, VALUE) __atomic_store_n(&(VAR), (VALUE), __ATOMIC_RELEASE)
#define BLE_LOG_ATOMIC_STORE_RELAXED(VAR, VALUE) __atomic_store_n(&(VAR), (VALUE), __ATOMIC_RELAXED)

/* Reference counting macros */
#define BLE_LOG_REF_COUNT_ACQUIRE(VAR)          __atomic_fetch_add(VAR, 1, __ATOMIC_ACQUIRE)
#define BLE_LOG_REF_COUNT_RELEASE(VAR)          __atomic_fetch_sub(VAR, 1, __ATOMIC_RELEASE)
/* Closing gate: pairs an inited-flag store with a reference-count load (and
 * vice versa) at seq_cst so deinit and a submitter cannot both observe the
 * pre-transition values on SMP (store-buffer / Dekker pattern). */
#define BLE_LOG_ATOMIC_LOAD_SEQ_CST(VAR)        __atomic_load_n(&(VAR), __ATOMIC_SEQ_CST)
#define BLE_LOG_ATOMIC_STORE_SEQ_CST(VAR, VALUE) __atomic_store_n(&(VAR), (VALUE), __ATOMIC_SEQ_CST)
#define BLE_LOG_REF_COUNT_ACQUIRE_SEQ_CST(VAR)  __atomic_fetch_add(VAR, 1, __ATOMIC_SEQ_CST)

/* Specifier */
#define BLE_LOG_STATIC                          static
#define BLE_LOG_INLINE                          inline

/* Section */
#define BLE_LOG_DRAM_ATTR                       DRAM_ATTR
#if defined(CONFIG_IDF_TARGET_ESP32C2)
#define BLE_LOG_IRAM_ATTR                       _SECTION_ATTR_IMPL(".ble_log_iram1", __COUNTER__)
#else
#define BLE_LOG_IRAM_ATTR                       IRAM_ATTR
#endif

/* Memory operation */
#define BLE_LOG_MEM_CAP                         (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA)
#define BLE_LOG_MALLOC(size)                    heap_caps_malloc(size, BLE_LOG_MEM_CAP)
#define BLE_LOG_FREE(ptr)                       heap_caps_free(ptr)
/* GDMA burst alignment: weighted arbitration requires buffers aligned to burst size (32B) */
#define BLE_LOG_BUF_ALIGN_BYTES                 (32U)
#define BLE_LOG_ALIGNED_MALLOC(size)            heap_caps_aligned_alloc(BLE_LOG_BUF_ALIGN_BYTES, size, BLE_LOG_MEM_CAP)
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

#define BLE_LOG_VERSION                         (6)
#define BLE_LOG_IDF_COMMIT_LEN                  (12)
/* Lib commit hashes are at most 10 hex chars; zero-padded when shorter */
#define BLE_LOG_LIB_COMMIT_LEN                  (10)

/* TYPEDEF */
typedef enum {
    BLE_LOG_INT_SRC_INIT_DONE,
    BLE_LOG_INT_SRC_TS,
    BLE_LOG_INT_SRC_ENH_STAT,
    BLE_LOG_INT_SRC_INFO,
    BLE_LOG_INT_SRC_FLUSH,
    BLE_LOG_INT_SRC_BUF_UTIL,
    BLE_LOG_INT_SRC_FINAL_STAT,
    BLE_LOG_INT_SRC_VERSION_INFO,
    BLE_LOG_INT_SRC_MAX,
} ble_log_int_src_t;

typedef struct {
    uint8_t int_src_code;
    uint8_t version;
} __attribute__((packed)) ble_log_info_t;

typedef struct {
    uint8_t int_src_code;
    uint8_t version;
    uint8_t idf_commit[BLE_LOG_IDF_COMMIT_LEN];
    uint8_t controller_commit[BLE_LOG_LIB_COMMIT_LEN];
    uint8_t btdm_common_commit[BLE_LOG_LIB_COMMIT_LEN];
    uint8_t mesh_commit[BLE_LOG_LIB_COMMIT_LEN];
    uint8_t audio_commit[BLE_LOG_LIB_COMMIT_LEN];
    uint16_t chip_model;
    uint16_t chip_revision;
} __attribute__((packed)) ble_log_version_info_t;

/* INTERFACE */
uint32_t ble_log_fast_checksum(const uint8_t *data, size_t len);

/* Acquire a lifetime reference only while the closing gate remains open. */
bool ble_log_ref_count_try_acquire(volatile uint32_t *ref_count,
                                   const uint32_t *inited);

/* Task-context wait; returns false if the count stays above max for one second. */
bool ble_log_ref_count_wait(volatile uint32_t *ref_count, uint32_t max_ref_count);

#endif /* __BLE_LOG_UTIL_H__ */

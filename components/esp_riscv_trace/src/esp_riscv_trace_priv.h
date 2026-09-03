/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/lock.h>
#include "hal/riscv_trace_hal.h"
#include "esp_riscv_trace.h"
#include "esp_riscv_trace_snapshot.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Opaque handle to a RISC-V trace encoder instance (driver-internal). */
typedef struct esp_riscv_trace_context_t *esp_riscv_trace_handle_t;

/** @brief Driver lifecycle state. */
typedef enum {
    ESP_RISCV_TRACE_STATE_CREATED = 0, /*!< Configured, not yet started */
    ESP_RISCV_TRACE_STATE_STARTED = 1, /*!< Capturing */
    ESP_RISCV_TRACE_STATE_STOPPED = 2, /*!< Stopped. Buffer synced and readable */
} esp_riscv_trace_state_t;

struct esp_riscv_trace_context_t {
    riscv_trace_hal_context_t hal;
    _lock_t lock;
    esp_riscv_trace_state_t state;
    int core_id;
    uint8_t *buffer;
    size_t buffer_size;
    bool auto_restart;
};

/* Snapshot writes. */
void esp_riscv_trace_snapshot_early_init(int core_id);
void esp_riscv_trace_snapshot_write_core_desc(int core_id, const esp_riscv_trace_handle_t handle);
void esp_riscv_trace_snapshot_start(int core_id);
void esp_riscv_trace_snapshot_stop(int core_id, uint32_t fifo_status, uint32_t intr_status,
                                   uint32_t head_offset, bool head_valid);

typedef struct {
    bool present;             /*!< Write this core */
    bool capturing;           /*!< Encoder was running at freeze */
    uint16_t state;
    bool head_valid;          /*!< head_offset is the hardware write position */
    uint32_t head_offset;
    uint32_t fifo_status_raw;
    uint32_t intr_status_raw;
    bool fifo_empty;
    bool memory_full;
    bool fifo_overflow;
} esp_riscv_trace_snapshot_panic_core_t;

void esp_riscv_trace_snapshot_panic_write(const esp_riscv_trace_snapshot_panic_core_t *cores, int count);

/* Waits for the encoder FIFOs to empty and records the final per-core status. */
void esp_riscv_trace_snapshot_finalize(void);

#ifdef __cplusplus
}
#endif

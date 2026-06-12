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

#ifdef __cplusplus
}
#endif

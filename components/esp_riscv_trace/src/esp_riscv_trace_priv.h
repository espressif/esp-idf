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
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "esp_rom_sys.h"
#include "hal/riscv_trace_ll.h"
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

/* Encoder settings that describe a capture. */
typedef struct {
    bool mem_loop;
    uint32_t resync_mode;
    uint32_t resync_threshold;
} esp_riscv_trace_capture_cfg_t;

struct esp_riscv_trace_context_t {
    riscv_trace_hal_context_t hal;
    _lock_t lock;
    esp_riscv_trace_state_t state;
    int core_id;
    uint8_t *buffer;
    size_t buffer_size;
    bool auto_restart;
    intr_handle_t intr_handle;      /*!< NULL when no trace interrupt is registered for this core. */
    uint32_t intr_mask;             /*!< Interrupt bits enabled while capturing (riscv_trace_intr_t flags). */
    /* OpenOCD capture, see esp_riscv_trace_ocd.c */
    volatile bool busy;             /*!< Set while lock is held. The OpenOCD stubs cannot take the lock and read this */
    esp_riscv_trace_capture_cfg_t saved_cfg; /*!< App settings, restored when OpenOCD releases the encoder */
};

/* Handle of a core, or NULL when the core is not traced. */
esp_riscv_trace_handle_t esp_riscv_trace_get_handle(int core_id);

#if CONFIG_ESP_RISCV_TRACE_OPENOCD_CONTROL
/* True while an OpenOCD capture runs on the core. The public API is refused then. */
bool esp_riscv_trace_ocd_capture_active(int core_id);
#else
static inline bool esp_riscv_trace_ocd_capture_active(int core_id)
{
    (void)core_id;
    return false;
}
#endif

#define ESP_RISCV_TRACE_FIFO_TIMEOUT_US   2000
#define ESP_RISCV_TRACE_FIFO_POLL_STEP_US 10

/* Waits for the encoder FIFO to empty. Returns false on timeout. Safe from the panic path and the OpenOCD stubs. */
FORCE_INLINE_ATTR bool esp_riscv_trace_wait_fifo_empty(trace_dev_t *dev)
{
    /* Right after trigger off the status can still show the state before the stop */
    esp_rom_delay_us(ESP_RISCV_TRACE_FIFO_POLL_STEP_US);
    for (uint32_t waited_us = 0; !(riscv_trace_ll_get_fifo_status(dev) & TRACE_FIFO_EMPTY_M);
            waited_us += ESP_RISCV_TRACE_FIFO_POLL_STEP_US) {
        if (waited_us >= ESP_RISCV_TRACE_FIFO_TIMEOUT_US) {
            return false;
        }
        esp_rom_delay_us(ESP_RISCV_TRACE_FIFO_POLL_STEP_US);
    }
    return true;
}

/* Converts the write pointer to a buffer offset. Returns false when it is outside the buffer. */
FORCE_INLINE_ATTR bool esp_riscv_trace_head_offset(esp_riscv_trace_handle_t handle, uint32_t current,
                                                   uint32_t *head_offset)
{
    uint32_t base = (uint32_t)handle->buffer;
    bool valid = (current >= base) && (current <= base + handle->buffer_size);
    *head_offset = valid ? current - base : 0;
    return valid;
}

/* Snapshot writes. */
void esp_riscv_trace_snapshot_early_init(int core_id);
void esp_riscv_trace_snapshot_write_core_desc(int core_id, const esp_riscv_trace_handle_t handle);
void esp_riscv_trace_snapshot_start(int core_id, const esp_riscv_trace_capture_cfg_t *cfg);
void esp_riscv_trace_snapshot_stop(int core_id, uint32_t fifo_status, uint32_t intr_status,
                                   uint32_t head_offset, bool head_valid);

/* Lock-free variants for the OpenOCD stubs, which run while every core is halted.
 * Call them only when esp_riscv_trace_snapshot_busy() is false. */
bool esp_riscv_trace_snapshot_busy(void);
void esp_riscv_trace_snapshot_start_nolock(int core_id, const esp_riscv_trace_capture_cfg_t *cfg);
void esp_riscv_trace_snapshot_stop_nolock(int core_id, uint32_t fifo_status, uint32_t intr_status,
                                          uint32_t head_offset, bool head_valid);

typedef struct {
    bool present;             /*!< Write this core */
    bool capturing;           /*!< Encoder was running at freeze */
    uint16_t state;
    bool head_valid;          /*!< head_offset is the hardware write position */
    uint32_t head_offset;
    uint32_t fifo_status_raw;
    uint32_t intr_status_raw;
} esp_riscv_trace_snapshot_panic_core_t;

void esp_riscv_trace_snapshot_panic_write(const esp_riscv_trace_snapshot_panic_core_t *cores, int count);

/* Waits for the encoder FIFOs to empty and records the final per-core status. */
void esp_riscv_trace_snapshot_finalize(void);

#ifdef __cplusplus
}
#endif

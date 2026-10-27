/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Clock sample captured in one short sampling window. */
typedef struct {
    uint8_t int_src_code;
    uint8_t io_level;
    uint32_t lc_ts;
    uint32_t esp_ts;
    uint32_t os_ts;
} __attribute__((packed)) ble_log_ts_info_t;

#define BLE_LOG_SYNC_BYTES           7
#define BLE_LOG_SYNC_BAUDRATE        1000000

#if CONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED
bool ble_log_sync_init(void);
void ble_log_sync_deinit(void);
#endif

/* Samples LC/ESP/OS clocks without emitting a label (INIT/FLUSH).
 * io_level reports the sampled pin level, or zero when sync is unavailable;
 * it does not indicate whether a complete external marker was emitted. */
void ble_log_sync_sample(ble_log_ts_info_t *info);
/* Called by the serialized periodic timer after owning the snapshot transport
 * and count; deinit must wait for that callback to exit. Produces falling T0
 * and samples in the same critical section, then completes a >=12 us BREAK,
 * >=2 us separator and count24 + ESP time32 label outside the critical section.
 * Falls back to ordinary sampling if no T0 was produced; keeps the T0 sample
 * even if the following label fails. UART completes sending without a callback. */
void ble_log_sync_sample_anchor(ble_log_ts_info_t *info, uint32_t anchor_count);

#ifdef __cplusplus
}
#endif

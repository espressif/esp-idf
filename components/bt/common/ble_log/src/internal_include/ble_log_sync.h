/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "ble_log_lbm_v2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_LOG_SYNC_BYTES           7
#define BLE_LOG_SYNC_BAUDRATE        1000000

#if CONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED
bool ble_log_sync_init(void);
void ble_log_sync_deinit(void);
/* True means UART TX inversion produced falling T0 and info was sampled in the
 * same critical section, even if the following UART label fails. False leaves info alone.
 * Outside the critical section, complete a >=12 us BREAK and >=2 us high
 * separator, then submit count24 + the sampled ESP time32 (little-endian).
 * UART sends and enforces trailing idle without a software completion callback.
 * UART owns the output throughout; only inversion and sampling run in the
 * critical section.
 * Called only by the serialized periodic timer after owning the snapshot
 * transport/count; deinit must wait for that callback to exit. */
bool ble_log_sync_send(uint32_t anchor_count, ble_log_ts_info_t *info);
#endif

#ifdef __cplusplus
}
#endif

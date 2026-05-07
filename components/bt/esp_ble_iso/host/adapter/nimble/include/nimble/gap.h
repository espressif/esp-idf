/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_NIMBLE_APP_GAP_H_
#define HOST_NIMBLE_APP_GAP_H_

#include <stdint.h>

#include "host/ble_gap.h"

#ifdef __cplusplus
extern "C" {
#endif

void bt_le_nimble_gap_post_event(void *param);

int bt_le_nimble_scan_start(const struct bt_le_scan_param *param, ble_gap_event_fn *cb);

int bt_le_nimble_scan_stop(void);

int bt_le_nimble_iso_disconnect(uint16_t conn_handle, uint8_t reason);

#ifdef __cplusplus
}
#endif

#endif /* HOST_NIMBLE_APP_GAP_H_ */

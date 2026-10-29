/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_BLUEDROID_APP_GAP_H_
#define HOST_BLUEDROID_APP_GAP_H_

#include <stdint.h>

#include <zephyr/bluetooth/bluetooth.h>

#ifdef __cplusplus
extern "C" {
#endif

void bt_le_bluedroid_gap_post_event(uint16_t event, void *param);

int bt_le_bluedroid_gap_init(void);

uint32_t bt_le_bluedroid_gap_conn_interval_us_get(const bt_addr_le_t *dst);

#ifdef __cplusplus
}
#endif

#endif /* HOST_BLUEDROID_APP_GAP_H_ */

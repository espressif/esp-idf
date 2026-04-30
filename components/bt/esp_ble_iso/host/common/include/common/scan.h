/*
 * SPDX-FileCopyrightText: 2017-2021 Nordic Semiconductor
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_COMMON_SCAN_H_
#define HOST_COMMON_SCAN_H_

#include <stdint.h>

#include "sdkconfig.h"

#include "nimble/gap.h"

#ifdef __cplusplus
extern "C" {
#endif

void bt_le_scan_recv_listener(uint16_t event_type,
                              uint8_t addr_type,
                              uint8_t addr_val[6],
                              int8_t rssi,
                              int8_t tx_power,
                              uint8_t sid,
                              uint8_t pri_phy,
                              uint8_t sec_phy,
                              uint16_t per_adv_itvl,
                              uint8_t data_len,
                              uint8_t *data);

struct bt_le_per_adv_sync *bt_le_per_adv_sync_find_safe(uint16_t sync_handle);

int bt_le_per_adv_sync_new(uint16_t sync_handle,
                           uint8_t sid,
                           uint8_t phy,
                           uint16_t interval,
                           uint8_t addr_type,
                           const uint8_t addr[6],
                           struct bt_le_per_adv_sync **out_sync);

int bt_le_per_adv_sync_delete(uint16_t sync_handle);

int bt_le_per_adv_sync_establish_listener(uint16_t sync_handle);

int bt_le_per_adv_sync_lost_listener(uint16_t sync_handle);

int bt_le_per_adv_sync_report_recv_listener(uint16_t sync_handle,
                                            const uint8_t *data,
                                            size_t data_length);

#ifdef __cplusplus
}
#endif

#endif /* HOST_COMMON_SCAN_H_ */

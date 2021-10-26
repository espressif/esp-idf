/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BTC_BLE_MESH_BLE_H_
#define _BTC_BLE_MESH_BLE_H_

#include <stdint.h>
#include "btc/btc_manage.h"
#include "mesh_bearer_adapt.h"
#include "esp_ble_mesh_ble_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef union {
    struct {
        esp_ble_mesh_ble_adv_param_t param;
        esp_ble_mesh_ble_adv_data_t data;
    } start_ble_adv;
    struct {
        uint8_t index;
    } stop_ble_adv;
    struct {
        esp_ble_mesh_ble_scan_param_t param;
    } start_ble_scan;
    struct {
        /* RFU */
    } stop_ble_scan;
} btc_ble_mesh_ble_args_t;

typedef enum {
    BTC_BLE_MESH_ACT_START_BLE_ADV,
    BTC_BLE_MESH_ACT_STOP_BLE_ADV,
    BTC_BLE_MESH_ACT_START_BLE_SCAN,
    BTC_BLE_MESH_ACT_STOP_BLE_SCAN,
} btc_ble_mesh_ble_act_t;

void bt_mesh_ble_scan_cb_evt_to_btc(const bt_mesh_addr_t *addr,
                                    uint8_t adv_type, uint8_t data[],
                                    uint16_t length, int8_t rssi);

void btc_ble_mesh_ble_call_handler(btc_msg_t *msg);

void btc_ble_mesh_ble_cb_handler(btc_msg_t *msg);

#ifdef __cplusplus
}
#endif

#endif /* _BTC_BLE_MESH_BLE_H_ */

// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

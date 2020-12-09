// Copyright 2017-2020 Espressif Systems (Shanghai) PTE LTD
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

#include <stdint.h>
#include <string.h>

#include "esp_err.h"

#include "btc_ble_mesh_ble.h"
#include "esp_ble_mesh_ble_api.h"

#if CONFIG_BLE_MESH_SUPPORT_BLE_SCAN

esp_err_t esp_ble_mesh_register_ble_callback(esp_ble_mesh_ble_cb_t callback)
{
    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    return (btc_profile_cb_set(BTC_PID_BLE_MESH_BLE_COEX, callback) == 0 ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_start_ble_scanning(esp_ble_mesh_ble_scan_param_t *param)
{
    btc_ble_mesh_ble_args_t arg = {0};
    btc_msg_t msg = {0};

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_BLE_MESH_BLE_COEX;
    msg.act = BTC_BLE_MESH_ACT_START_BLE_SCAN;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_ble_args_t), NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_stop_ble_scanning(void)
{
    btc_ble_mesh_ble_args_t arg = {0};
    btc_msg_t msg = {0};

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_BLE_MESH_BLE_COEX;
    msg.act = BTC_BLE_MESH_ACT_STOP_BLE_SCAN;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_ble_args_t), NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_SCAN */

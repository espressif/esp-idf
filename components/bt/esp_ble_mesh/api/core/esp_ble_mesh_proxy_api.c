// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
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

#include "btc/btc_manage.h"

#include "esp_err.h"

#include "btc_ble_mesh_prov.h"
#include "esp_ble_mesh_defs.h"

esp_err_t esp_ble_mesh_proxy_identity_enable(void)
{
    btc_msg_t msg = {0};

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROXY_IDENTITY_ENABLE;

    return (btc_transfer_context(&msg, NULL, 0, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_proxy_gatt_enable(void)
{
    btc_msg_t msg = {0};

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROXY_GATT_ENABLE;

    return (btc_transfer_context(&msg, NULL, 0, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_proxy_gatt_disable(void)
{
    btc_msg_t msg = {0};

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROXY_GATT_DISABLE;

    return (btc_transfer_context(&msg, NULL, 0, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_proxy_client_connect(esp_ble_mesh_bd_addr_t addr,
        esp_ble_mesh_addr_type_t addr_type, uint16_t net_idx)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (!addr || addr_type > ESP_BLE_MESH_ADDR_TYPE_RANDOM) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROXY_CLIENT_CONNECT;

    memcpy(arg.proxy_client_connect.addr, addr, BD_ADDR_LEN);
    arg.proxy_client_connect.addr_type = addr_type;
    arg.proxy_client_connect.net_idx = net_idx;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_proxy_client_disconnect(uint8_t conn_handle)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROXY_CLIENT_DISCONNECT;

    arg.proxy_client_disconnect.conn_handle = conn_handle;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_proxy_client_set_filter_type(uint8_t conn_handle,
        uint16_t net_idx, esp_ble_mesh_proxy_filter_type_t filter_type)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (filter_type > PROXY_FILTER_BLACKLIST) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROXY_CLIENT_SET_FILTER_TYPE;

    arg.proxy_client_set_filter_type.conn_handle = conn_handle;
    arg.proxy_client_set_filter_type.net_idx = net_idx;
    arg.proxy_client_set_filter_type.filter_type = filter_type;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_proxy_client_add_filter_addr(uint8_t conn_handle,
        uint16_t net_idx, uint16_t *addr, uint16_t addr_num)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (!addr || addr_num == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROXY_CLIENT_ADD_FILTER_ADDR;

    arg.proxy_client_add_filter_addr.conn_handle = conn_handle;
    arg.proxy_client_add_filter_addr.net_idx = net_idx;
    arg.proxy_client_add_filter_addr.addr_num = addr_num;
    arg.proxy_client_add_filter_addr.addr = addr;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), btc_ble_mesh_prov_arg_deep_copy)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_proxy_client_remove_filter_addr(uint8_t conn_handle,
        uint16_t net_idx, uint16_t *addr, uint16_t addr_num)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (!addr || addr_num == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROXY_CLIENT_REMOVE_FILTER_ADDR;

    arg.proxy_client_remove_filter_addr.conn_handle = conn_handle;
    arg.proxy_client_remove_filter_addr.net_idx = net_idx;
    arg.proxy_client_remove_filter_addr.addr_num = addr_num;
    arg.proxy_client_remove_filter_addr.addr = addr;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), btc_ble_mesh_prov_arg_deep_copy)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

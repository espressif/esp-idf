/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "btc/btc_manage.h"
#include "btc_ble_mesh_prb_model.h"
#include "esp_ble_mesh_prb_model_api.h"

#if CONFIG_BLE_MESH_PRB_CLI
esp_err_t esp_ble_mesh_register_prb_client_callback(esp_ble_mesh_prb_client_cb_t callback)
{
    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    return (btc_profile_cb_set(BTC_PID_PRB_CLIENT, callback) == 0 ? ESP_OK : ESP_FAIL);
}

static bool private_beacon_client_send_need_param(esp_ble_mesh_opcode_t opcode)
{
    switch (opcode) {
    case ESP_BLE_MESH_MODEL_OP_PRIV_BEACON_SET:
    case ESP_BLE_MESH_MODEL_OP_PRIV_GATT_PROXY_SET:
    case ESP_BLE_MESH_MODEL_OP_PRIV_NODE_IDENTITY_GET:
    case ESP_BLE_MESH_MODEL_OP_PRIV_NODE_IDENTITY_SET:
        return true;
    default:
        return false;
    }
}

esp_err_t esp_ble_mesh_prb_client_send(esp_ble_mesh_client_common_param_t *params,
                                       esp_ble_mesh_prb_client_msg_t *msg)
{
    btc_ble_mesh_prb_client_args_t btc_arg = {0};
    btc_msg_t btc_msg = {0};

    if (params == NULL || params->model == NULL ||
        params->ctx.net_idx == ESP_BLE_MESH_KEY_UNUSED ||
        !ESP_BLE_MESH_ADDR_IS_UNICAST(params->ctx.addr) ||
        (private_beacon_client_send_need_param(params->opcode) && msg == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    btc_msg.sig = BTC_SIG_API_CALL;
    btc_msg.pid = BTC_PID_PRB_CLIENT;
    btc_msg.act = BTC_BLE_MESH_ACT_PRB_CLIENT_SEND;

    btc_arg.prb_send.params = params;
    btc_arg.prb_send.msg = msg;

    return (btc_transfer_context(&btc_msg, &btc_arg, sizeof(btc_ble_mesh_prb_client_args_t),
            btc_ble_mesh_prb_client_arg_deep_copy,
            btc_ble_mesh_prb_client_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#endif /* CONFIG_BLE_MESH_PRB_CLI */

#if CONFIG_BLE_MESH_PRB_SRV
esp_err_t esp_ble_mesh_register_prb_server_callback(esp_ble_mesh_prb_server_cb_t callback)
{
    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    return (btc_profile_cb_set(BTC_PID_PRB_SERVER, callback) == 0 ? ESP_OK : ESP_FAIL);
}
#endif /* CONFIG_BLE_MESH_PRB_SRV */

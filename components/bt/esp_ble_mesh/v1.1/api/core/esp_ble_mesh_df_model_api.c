/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "btc/btc_manage.h"

#include "btc_ble_mesh_df_model.h"
#include "esp_ble_mesh_df_model_api.h"

#if CONFIG_BLE_MESH_DF_CLI
esp_err_t esp_ble_mesh_register_df_client_callback(esp_ble_mesh_df_client_cb_t callback)
{
    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    return (btc_profile_cb_set(BTC_PID_DF_CLIENT, callback) == 0 ? ESP_OK : ESP_FAIL);
}

static bool directed_forwarding_client_get_need_param(esp_ble_mesh_opcode_t opcode)
{
    switch (opcode) {
    case ESP_BLE_MESH_MODEL_OP_DIRECTED_CONTROL_GET:
    case ESP_BLE_MESH_MODEL_OP_PATH_METRIC_GET:
    case ESP_BLE_MESH_MODEL_OP_DISCOVERY_TABLE_CAPS_GET:
    case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_ENTRIES_CNT_GET:
    case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_ENTRIES_GET:
    case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEPS_GET:
    case ESP_BLE_MESH_MODEL_OP_WANTED_LANES_GET:
    case ESP_BLE_MESH_MODEL_OP_TWO_WAY_PATH_GET:
    case ESP_BLE_MESH_MODEL_OP_PATH_ECHO_INTERVAL_GET:
    case ESP_BLE_MESH_MODEL_OP_DIRECTED_PUB_POLICY_GET:
        return true;
    default:
        return false;
    }
}

esp_err_t esp_ble_mesh_df_client_get_state(esp_ble_mesh_client_common_param_t *params,
                                           esp_ble_mesh_df_client_get_t *get)
{
    btc_ble_mesh_df_client_args_t arg = {0};
    btc_msg_t msg = {0};

    if (params == NULL || params->model == NULL ||
        params->ctx.net_idx == ESP_BLE_MESH_KEY_UNUSED ||
        !ESP_BLE_MESH_ADDR_IS_UNICAST(params->ctx.addr) ||
        (directed_forwarding_client_get_need_param(params->opcode) && get == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_DF_CLIENT;
    msg.act = BTC_BLE_MESH_ACT_DF_CLIENT_GET_STATE;

    arg.df_get.params = params;
    arg.df_get.get = get;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_df_client_args_t),
            btc_ble_mesh_df_client_arg_deep_copy,
            btc_ble_mesh_df_client_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_df_client_set_state(esp_ble_mesh_client_common_param_t *params,
                                           esp_ble_mesh_df_client_set_t *set)
{
    btc_ble_mesh_df_client_args_t arg = {0};
    btc_msg_t msg = {0};

    if (params == NULL || params->model == NULL || set == NULL ||
        params->ctx.net_idx == ESP_BLE_MESH_KEY_UNUSED ||
        !ESP_BLE_MESH_ADDR_IS_UNICAST(params->ctx.addr)) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_DF_CLIENT;
    msg.act = BTC_BLE_MESH_ACT_DF_CLIENT_SET_STATE;

    arg.df_set.params = params;
    arg.df_set.set = set;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_df_client_args_t),
            btc_ble_mesh_df_client_arg_deep_copy,
            btc_ble_mesh_df_client_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#endif /* CONFIG_BLE_MESH_DF_CLI */

#if CONFIG_BLE_MESH_DF_SRV
esp_err_t esp_ble_mesh_register_df_server_callback(esp_ble_mesh_df_server_cb_t callback)
{
    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    return (btc_profile_cb_set(BTC_PID_DF_SERVER, callback) == 0 ? ESP_OK : ESP_FAIL);
}
#endif /* CONFIG_BLE_MESH_DF_SRV */

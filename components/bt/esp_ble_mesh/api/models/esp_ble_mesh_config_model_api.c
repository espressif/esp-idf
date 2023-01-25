/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "btc/btc_manage.h"

#include "btc_ble_mesh_config_model.h"
#include "esp_ble_mesh_config_model_api.h"

#if CONFIG_BLE_MESH_CFG_CLI
esp_err_t esp_ble_mesh_register_config_client_callback(esp_ble_mesh_cfg_client_cb_t callback)
{
    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    return (btc_profile_cb_set(BTC_PID_CONFIG_CLIENT, callback) == 0 ? ESP_OK : ESP_FAIL);
}

static bool config_client_get_need_param(esp_ble_mesh_opcode_t opcode)
{
    switch (opcode) {
    case ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET:
    case ESP_BLE_MESH_MODEL_OP_MODEL_PUB_GET:
    case ESP_BLE_MESH_MODEL_OP_SIG_MODEL_SUB_GET:
    case ESP_BLE_MESH_MODEL_OP_VENDOR_MODEL_SUB_GET:
    case ESP_BLE_MESH_MODEL_OP_APP_KEY_GET:
    case ESP_BLE_MESH_MODEL_OP_NODE_IDENTITY_GET:
    case ESP_BLE_MESH_MODEL_OP_SIG_MODEL_APP_GET:
    case ESP_BLE_MESH_MODEL_OP_VENDOR_MODEL_APP_GET:
    case ESP_BLE_MESH_MODEL_OP_KEY_REFRESH_PHASE_GET:
    case ESP_BLE_MESH_MODEL_OP_LPN_POLLTIMEOUT_GET:
        return true;
    default:
        return false;
    }
}

esp_err_t esp_ble_mesh_config_client_get_state(esp_ble_mesh_client_common_param_t *params,
                                               esp_ble_mesh_cfg_client_get_state_t *get_state)
{
    btc_ble_mesh_config_client_args_t arg = {0};
    btc_msg_t msg = {0};

    if (params == NULL || params->model == NULL ||
        params->ctx.net_idx == ESP_BLE_MESH_KEY_UNUSED ||
        !ESP_BLE_MESH_ADDR_IS_UNICAST(params->ctx.addr) ||
        (config_client_get_need_param(params->opcode) && get_state == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_CONFIG_CLIENT;
    msg.act = BTC_BLE_MESH_ACT_CONFIG_CLIENT_GET_STATE;
    arg.cfg_client_get_state.params = params;
    arg.cfg_client_get_state.get_state = get_state;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_config_client_args_t), btc_ble_mesh_config_client_arg_deep_copy,
                btc_ble_mesh_config_client_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_config_client_set_state(esp_ble_mesh_client_common_param_t *params,
                                               esp_ble_mesh_cfg_client_set_state_t *set_state)
{
    btc_ble_mesh_config_client_args_t arg = {0};
    btc_msg_t msg = {0};

    if (params == NULL || params->model == NULL ||
        params->ctx.net_idx == ESP_BLE_MESH_KEY_UNUSED ||
        !ESP_BLE_MESH_ADDR_IS_UNICAST(params->ctx.addr) ||
        (params->opcode != ESP_BLE_MESH_MODEL_OP_NODE_RESET && set_state == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_CONFIG_CLIENT;
    msg.act = BTC_BLE_MESH_ACT_CONFIG_CLIENT_SET_STATE;
    arg.cfg_client_set_state.params = params;
    arg.cfg_client_set_state.set_state = set_state;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_config_client_args_t), btc_ble_mesh_config_client_arg_deep_copy,
                btc_ble_mesh_config_client_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#endif /* CONFIG_BLE_MESH_CFG_CLI */

esp_err_t esp_ble_mesh_register_config_server_callback(esp_ble_mesh_cfg_server_cb_t callback)
{
    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    return (btc_profile_cb_set(BTC_PID_CONFIG_SERVER, callback) == 0 ? ESP_OK : ESP_FAIL);
}

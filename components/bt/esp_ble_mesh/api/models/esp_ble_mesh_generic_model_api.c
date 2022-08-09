/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "btc/btc_manage.h"

#include "btc_ble_mesh_generic_model.h"
#include "esp_ble_mesh_generic_model_api.h"

#if CONFIG_BLE_MESH_GENERIC_CLIENT
esp_err_t esp_ble_mesh_register_generic_client_callback(esp_ble_mesh_generic_client_cb_t callback)
{
    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    return (btc_profile_cb_set(BTC_PID_GENERIC_CLIENT, callback) == 0 ? ESP_OK : ESP_FAIL);
}

static bool generic_client_get_need_param(esp_ble_mesh_opcode_t opcode)
{
    switch (opcode) {
    case ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET:
    case ESP_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_GET:
    case ESP_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTY_GET:
    case ESP_BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_GET:
        return true;
    default:
        return false;
    }
}

esp_err_t esp_ble_mesh_generic_client_get_state(esp_ble_mesh_client_common_param_t *params,
                                                esp_ble_mesh_generic_client_get_state_t *get_state)
{
    btc_ble_mesh_generic_client_args_t arg = {0};
    btc_msg_t msg = {0};

    if (params == NULL || params->model == NULL ||
        params->ctx.net_idx == ESP_BLE_MESH_KEY_UNUSED ||
        params->ctx.app_idx == ESP_BLE_MESH_KEY_UNUSED ||
        params->ctx.addr == ESP_BLE_MESH_ADDR_UNASSIGNED ||
        (generic_client_get_need_param(params->opcode) && get_state == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GENERIC_CLIENT;
    msg.act = BTC_BLE_MESH_ACT_GENERIC_CLIENT_GET_STATE;
    arg.generic_client_get_state.params = params;
    arg.generic_client_get_state.get_state = get_state;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_generic_client_args_t), btc_ble_mesh_generic_client_arg_deep_copy,
                btc_ble_mesh_generic_client_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_generic_client_set_state(esp_ble_mesh_client_common_param_t *params,
                                                esp_ble_mesh_generic_client_set_state_t *set_state)
{
    btc_ble_mesh_generic_client_args_t arg = {0};
    btc_msg_t msg = {0};

    if (params == NULL || params->model == NULL || set_state == NULL ||
        params->ctx.net_idx == ESP_BLE_MESH_KEY_UNUSED ||
        params->ctx.app_idx == ESP_BLE_MESH_KEY_UNUSED ||
        params->ctx.addr == ESP_BLE_MESH_ADDR_UNASSIGNED) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GENERIC_CLIENT;
    msg.act = BTC_BLE_MESH_ACT_GENERIC_CLIENT_SET_STATE;
    arg.generic_client_set_state.params = params;
    arg.generic_client_set_state.set_state = set_state;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_generic_client_args_t), btc_ble_mesh_generic_client_arg_deep_copy,
                btc_ble_mesh_generic_client_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#endif /* CONFIG_BLE_MESH_GENERIC_CLIENT */

#if CONFIG_BLE_MESH_GENERIC_SERVER
esp_err_t esp_ble_mesh_register_generic_server_callback(esp_ble_mesh_generic_server_cb_t callback)
{
    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    return (btc_profile_cb_set(BTC_PID_GENERIC_SERVER, callback) == 0 ? ESP_OK : ESP_FAIL);
}
#endif /* CONFIG_BLE_MESH_GENERIC_SERVER */

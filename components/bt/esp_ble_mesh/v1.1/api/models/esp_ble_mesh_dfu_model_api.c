/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "btc/btc_manage.h"

#include "btc_ble_mesh_dfu_model.h"
#include "esp_ble_mesh_dfu_model_api.h"

#if CONFIG_BLE_MESH_DFU_CLI

static bool is_get_param_needed(esp_ble_mesh_opcode_t opcode)
{
    switch (opcode) {
    case ESP_BLE_MESH_DFU_OP_UPDATE_INFO_GET:
    case ESP_BLE_MESH_DFU_OP_UPDATE_METADATA_CHECK:
    case ESP_BLE_MESH_DFU_OP_UPDATE_START:
        return true;
    default:
        return false;
    }
}

esp_err_t esp_ble_mesh_dfu_client_get_state(esp_ble_mesh_client_common_param_t *params,
                                            esp_ble_mesh_dfu_client_get_t *get)
{
    btc_ble_mesh_dfu_client_args_t arg = {0};
    btc_msg_t msg = {0};

    if (params == NULL || params->model == NULL ||
            params->ctx.net_idx == ESP_BLE_MESH_KEY_UNUSED ||
            !ESP_BLE_MESH_ADDR_IS_UNICAST(params->ctx.addr) ||
            (is_get_param_needed(params->opcode) && get == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_DFU_CLIENT;
    msg.act = BTC_BLE_MESH_ACT_DFU_CLIENT_GET_STATE;

    arg.dfu_get.params = params;
    arg.dfu_get.get = get;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_dfu_client_args_t),
                                 btc_ble_mesh_dfu_client_arg_deep_copy,
                                 btc_ble_mesh_dfu_client_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_dfu_cli_img_send(esp_ble_mesh_dfu_cli_t *cli,
                                        esp_ble_mesh_blob_cli_inputs_t *inputs,
                                        esp_ble_mesh_blob_io_t *io,
                                        esp_ble_mesh_dfu_cli_xfer_t *xfer)
{
    btc_ble_mesh_dfu_client_args_t arg = {0};
    btc_msg_t msg = {0};

    if (cli == NULL || inputs == NULL ||
        io == NULL || xfer == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_DFU_CLIENT;
    msg.act = BTC_BLE_MESH_ACT_DFU_CLIENT_IMG_SEND;

    arg.send_arg.cli = cli;
    arg.send_arg.inputs = inputs;
    arg.send_arg.io = io;
    arg.send_arg.xfer = xfer;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_dfu_client_args_t),
                                 btc_ble_mesh_dfu_client_arg_deep_copy,
                                 btc_ble_mesh_dfu_client_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

uint8_t esp_ble_mesh_dfu_cli_progress(esp_ble_mesh_dfu_cli_t *cli)
{
    if (!cli) {
        return 0;
    }
    return btc_ble_mesh_dfu_cli_progress(cli);
}

esp_err_t esp_ble_mesh_register_dfu_client_callback(esp_ble_mesh_dfu_client_cb_t callback)
{
    return (btc_profile_cb_set(BTC_PID_DFU_CLIENT, callback) == 0 ? ESP_OK : ESP_FAIL);
}

#endif /* CONFIG_BLE_MESH_DFU_CLI */

#if CONFIG_BLE_MESH_DFU_SRV
void esp_ble_mesh_dfu_srv_verified(esp_ble_mesh_dfu_srv_t *srv)
{
    if (!srv) {
        return;
    }

    btc_ble_mesh_dfu_srv_verified(srv);
}

void esp_ble_mesh_dfu_srv_rejected(esp_ble_mesh_dfu_srv_t *srv)
{
    if (!srv) {
        return;
    }

    btc_ble_mesh_dfu_srv_rejected(srv);
}

void esp_ble_mesh_dfu_srv_cancel(esp_ble_mesh_dfu_srv_t *srv)
{
    if (!srv) {
        return;
    }

    btc_ble_mesh_dfu_srv_cancel(srv);
}

void esp_ble_mesh_dfu_srv_applied(esp_ble_mesh_dfu_srv_t *srv)
{
    if (!srv) {
        return;
    }

    btc_ble_mesh_dfu_srv_applied(srv);
}

bool esp_ble_mesh_dfu_srv_is_busy(const esp_ble_mesh_dfu_srv_t *srv)
{
    if (!srv) {
        return false;
    }

    return btc_ble_mesh_dfu_srv_is_busy(srv);
}

uint8_t esp_ble_mesh_dfu_srv_progress(const esp_ble_mesh_dfu_srv_t *srv)
{
    if (!srv) {
        return 0;
    }

    return btc_ble_mesh_dfu_srv_progress(srv);
}
#endif /* CONFIG_BLE_MESH_DFU_SRV */

#if CONFIG_BLE_MESH_DFD_CLI

esp_err_t esp_ble_mesh_register_dfd_cli_callback(esp_ble_mesh_dfd_client_cb_t callback)
{
    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    return (btc_profile_cb_set(BTC_PID_DFD_CLIENT, callback) == 0 ? ESP_OK : ESP_FAIL);
}

static bool dfd_client_opcode_need_param(esp_ble_mesh_opcode_t opcode)
{
    switch (opcode)
    {
    /* Get opcode */
    case ESP_BLE_MESH_DFD_OP_RECEIVERS_GET:
    case ESP_BLE_MESH_DFD_OP_FW_GET:
    case ESP_BLE_MESH_DFD_OP_FW_GET_BY_INDEX:
    /* Set opcode */
    case ESP_BLE_MESH_DFD_OP_RECEIVERS_ADD:
    case ESP_BLE_MESH_DFD_OP_START:
    case ESP_BLE_MESH_DFD_OP_UPLOAD_START:
    case ESP_BLE_MESH_DFD_OP_UPLOAD_START_OOB:
    case ESP_BLE_MESH_DFD_OP_FW_DELETE:
        return true;
    default:
        return false;
    }
}

esp_err_t esp_ble_mesh_dfd_cli_get(esp_ble_mesh_client_common_param_t *params,
                                   esp_ble_mesh_dfd_client_get_param_t *get_param)
{
    btc_ble_mesh_dfd_client_args_t arg = {0};
    btc_msg_t msg = {0};

    if (params == NULL || params->model == NULL ||
        params->ctx.net_idx == ESP_BLE_MESH_KEY_UNUSED ||
        !ESP_BLE_MESH_ADDR_IS_UNICAST(params->ctx.addr) ||
        (dfd_client_opcode_need_param(params->opcode) && get_param == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_DFD_CLIENT;
    msg.act = ESP_BLE_MESH_ACT_DFD_CLIENT_GET;
    arg.dfd_client_get.params = params;
    arg.dfd_client_get.get = get_param;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_dfd_client_args_t), btc_ble_mesh_dfd_client_arg_deep_copy,
            btc_ble_mesh_dfd_client_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_dfd_cli_set(esp_ble_mesh_client_common_param_t *params,
                                   esp_ble_mesh_dfd_client_set_param_t *set_param)
{
    btc_ble_mesh_dfd_client_args_t arg = {0};
    btc_msg_t msg = {0};

    if (params == NULL || params->model == NULL ||
        params->ctx.net_idx == ESP_BLE_MESH_KEY_UNUSED ||
        !ESP_BLE_MESH_ADDR_IS_UNICAST(params->ctx.addr) ||
        (dfd_client_opcode_need_param(params->opcode) && set_param == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (params->opcode) {
        case ESP_BLE_MESH_DFD_OP_RECEIVERS_ADD:
            if (set_param->receivers_add.receivers_cnt == 0) {
                return ESP_ERR_INVALID_ARG;
            }
        break;
        case ESP_BLE_MESH_DFD_OP_UPLOAD_START:
            if (set_param->dist_upload_start.fw_size == 0) {
                return ESP_ERR_INVALID_ARG;
            }
            if (set_param->dist_upload_start.fwid == NULL) {
                return ESP_ERR_INVALID_ARG;
            }
        break;
        case ESP_BLE_MESH_DFD_OP_UPLOAD_START_OOB:
            if (set_param->dist_upload_oob_start.url == NULL) {
                return ESP_ERR_INVALID_ARG;
            }
        break;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_DFD_CLIENT;
    msg.act = ESP_BLE_MESH_ACT_DFD_CLIENT_SET;
    arg.dfd_client_set.params = params;
    arg.dfd_client_set.set = set_param;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_dfd_client_args_t), btc_ble_mesh_dfd_client_arg_deep_copy,
            btc_ble_mesh_dfd_client_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#endif /* CONFIG_BLE_MESH_DFD_CLI */

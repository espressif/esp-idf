/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "btc_ble_mesh_model_common.h"
#include "btc_ble_mesh_dfu_model.h"
#include "esp_ble_mesh_dfu_model_api.h"
#include "mesh_v1.1/dfu/dfu_cli.h"
#include "mesh_v1.1/dfu/dfu_srv.h"

#if CONFIG_BLE_MESH_DFU_CLI
/* Device Firmware Update Client model related functions */

static inline void btc_ble_mesh_dfu_client_cb_to_app(btc_ble_mesh_dfu_client_cb_evt_t event,
                                                     esp_ble_mesh_dfu_client_cb_param_t *param)
{
    esp_ble_mesh_dfu_client_cb_t btc_ble_mesh_cb =
        btc_profile_cb_get(BTC_PID_DFU_CLIENT);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

void btc_ble_mesh_dfu_client_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_ble_mesh_dfu_client_args_t *dst = p_dest;
    btc_ble_mesh_dfu_client_args_t *src = p_src;

    if (!msg || !dst || !src) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_DFU_CLIENT_GET_STATE: {
        dst->dfu_get.params = bt_mesh_calloc(sizeof(esp_ble_mesh_client_common_param_t));
        if (dst->dfu_get.params) {
            memcpy(dst->dfu_get.params, src->dfu_get.params,
                   sizeof(esp_ble_mesh_client_common_param_t));
        } else {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            break;
        }
        if (src->dfu_get.get) {
            dst->dfu_get.get = bt_mesh_calloc(sizeof(esp_ble_mesh_dfu_client_get_t));
            if (dst->dfu_get.get) {
                memcpy(dst->dfu_get.get, src->dfu_get.get,
                       sizeof(esp_ble_mesh_dfu_client_get_t));
                if (src->dfu_get.params->opcode == ESP_BLE_MESH_DFU_OP_UPDATE_METADATA_CHECK) {
                    if (src->dfu_get.get->dfu_metadata_check.metadata) {
                        dst->dfu_get.get->dfu_metadata_check.metadata = bt_mesh_alloc_buf(src->dfu_get.get->dfu_metadata_check.metadata->len);
                        if (dst->dfu_get.get->dfu_metadata_check.metadata) {
                            net_buf_simple_add_mem(dst->dfu_get.get->dfu_metadata_check.metadata,
                                                   src->dfu_get.get->dfu_metadata_check.metadata->data,
                                                   src->dfu_get.get->dfu_metadata_check.metadata->len);
                        } else {
                            BT_ERR("Out of memory for metadata");
                            return;
                        }
                    } else {
                        BT_ERR("Metadata should be exists");
                        return;
                    }
                }
            } else {
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            }
        }
        break;
    }
    default:
        BT_DBG("%s, Unknown act %d", __func__, msg->act);
        break;
    }
}

void btc_ble_mesh_dfu_client_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_mesh_dfu_client_args_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_dfu_client_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_DFU_CLIENT_GET_STATE:
        if (arg->dfu_get.params) {
            bt_mesh_free(arg->dfu_get.params);
        }
        if (arg->dfu_get.get) {
            bt_mesh_free(arg->dfu_get.get);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_dfu_client_copy_req_data(btc_msg_t *msg, void *p_dest, void *p_src)
{
    esp_ble_mesh_dfu_client_cb_param_t *p_dest_data = p_dest;
    esp_ble_mesh_dfu_client_cb_param_t *p_src_data = p_src;
    uint16_t length = 0;

    if (!msg || !p_src_data || !p_dest_data) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    if (p_src_data->params) {
        p_dest_data->params = bt_mesh_calloc(sizeof(esp_ble_mesh_client_common_param_t));
        if (!p_dest_data->params) {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            return;
        }

        memcpy(p_dest_data->params, p_src_data->params, sizeof(esp_ble_mesh_client_common_param_t));
    }

    switch (msg->act) {
    case ESP_BLE_MESH_DFU_CLIENT_RECV_GET_RSP_EVT:
        if (p_src_data->params) {
            switch (p_src_data->params->opcode) {
            case ESP_BLE_MESH_DFU_OP_UPDATE_INFO_GET:
                if (p_src_data->recv.dfu_update_info_status.fw_info_list_cnt) {
                    length = p_src_data->recv.dfu_update_info_status.fw_info_list_cnt * sizeof(esp_ble_mesh_firmware_info_t);
                    p_dest_data->recv.dfu_update_info_status.fw_info_list = bt_mesh_calloc(length);
                    if (!p_dest_data->recv.dfu_update_info_status.fw_info_list) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        return;
                    }
                    memcpy(p_dest_data->recv.dfu_update_info_status.fw_info_list,
                           p_src_data->recv.dfu_update_info_status.fw_info_list,
                           length);
                }
                break;
            default:
                break;
            }
        }
    case ESP_BLE_MESH_DFU_CLIENT_SEND_COMP_EVT:
    case ESP_BLE_MESH_DFU_CLIENT_TIMEOUT_EVT:
    case ESP_BLE_MESH_DFU_CLIENT_IMG_SEND_CMP_EVT:
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_dfu_client_free_req_data(btc_msg_t *msg)
{
    esp_ble_mesh_dfu_client_cb_param_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    if (msg->act >= ESP_BLE_MESH_DFU_CLIENT_EVT_MAX) {
        BT_ERR("%s, Invalid event %d", __func__, msg->act);
        return;
    }

    arg = (esp_ble_mesh_dfu_client_cb_param_t *)msg->arg;

    switch (msg->act) {
    case ESP_BLE_MESH_DFU_CLIENT_RECV_GET_RSP_EVT:
        if (arg->params) {
            switch (arg->params->opcode) {
            case ESP_BLE_MESH_DFU_OP_UPDATE_INFO_GET:
                bt_mesh_free(arg->recv.dfu_update_info_status.fw_info_list);
                break;
            default:
                break;
            }
        }
    case ESP_BLE_MESH_DFU_CLIENT_SEND_COMP_EVT:
    case ESP_BLE_MESH_DFU_CLIENT_TIMEOUT_EVT:
    case ESP_BLE_MESH_DFU_CLIENT_IMG_SEND_CMP_EVT:
        break;
    default:
        BT_WARN("Unprocessed event %d", msg->act);
        break;
    }

    if (arg->params) {
        bt_mesh_free(arg->params);
    }

    return;
}

static void btc_ble_mesh_dfu_client_cb(esp_ble_mesh_dfu_client_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_DFU_CLIENT)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_DFU_CLIENT;
    msg.act = act;

    btc_transfer_context(&msg, cb_params, sizeof(esp_ble_mesh_dfu_client_cb_param_t),
                         btc_ble_mesh_dfu_client_copy_req_data,
                         btc_ble_mesh_dfu_client_free_req_data);
}

void bt_mesh_dfu_client_cb_evt_to_btc(uint32_t opcode, uint8_t event,
                                      struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx,
                                      const uint8_t *val, size_t len)
{
    esp_ble_mesh_dfu_client_cb_param_t cb_params = {0};
    esp_ble_mesh_client_common_param_t params = {0};
    uint8_t act = 0;

    if (!model || !ctx || len > sizeof(cb_params.recv)) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (event) {
    case BTC_BLE_MESH_EVT_DFU_CLIENT_TIMEOUT:
        act = ESP_BLE_MESH_DFU_CLIENT_TIMEOUT_EVT;
        break;
    case BTC_BLE_MESH_EVT_DFU_CLIENT_RECV_GET_RSP:
        act = ESP_BLE_MESH_DFU_CLIENT_RECV_GET_RSP_EVT;
        break;
    case BTC_BLE_MESH_EVT_DFU_CLIENT_IMG_SEND_CMP:
        act = ESP_BLE_MESH_DFU_CLIENT_IMG_SEND_CMP_EVT;
        break;
    default:
        BT_ERR("Unknown Device Firmware Update client event type %d", event);
        return;
    }

    params.opcode = opcode;
    params.model = (esp_ble_mesh_model_t *)model;
    params.ctx.net_idx = ctx->net_idx;
    params.ctx.app_idx = ctx->app_idx;
    params.ctx.addr = ctx->addr;
    params.ctx.recv_ttl = ctx->recv_ttl;
    params.ctx.recv_op = ctx->recv_op;
    params.ctx.recv_dst = ctx->recv_dst;
    params.ctx.recv_rssi = ctx->recv_rssi;
    params.ctx.send_ttl = ctx->send_ttl;

    cb_params.params = &params;

    if (val && len) {
        memcpy(&cb_params.recv, val, len);
    }

    btc_ble_mesh_dfu_client_cb(&cb_params, act);
}

static int btc_ble_mesh_dfu_client_get_state(esp_ble_mesh_client_common_param_t *params,
                                             esp_ble_mesh_dfu_client_get_t *get)
{
    bt_mesh_client_common_param_t param = {0};
    struct bt_mesh_dfu_cli *dfu_cli = NULL;

    if (params == NULL ||
            params->model == NULL ||
            params->model->user_data == NULL) {
        BT_ERR("%s, Invalid parameter %d %p %p", __func__, params == NULL, params->model, params->model->user_data);
        return -EINVAL;
    }

    dfu_cli = (struct bt_mesh_dfu_cli *)params->model->user_data;

    switch (params->opcode) {
    case ESP_BLE_MESH_DFU_OP_UPDATE_INFO_GET:
    case ESP_BLE_MESH_DFU_OP_UPDATE_METADATA_CHECK:
    case ESP_BLE_MESH_DFU_OP_UPDATE_START:
        if (get == NULL) {
            BT_ERR("Invalid Device Firmware Update Get");
            return -EINVAL;
        }
        break;
    default:
        break;
    }

    btc_ble_mesh_set_client_common_param(params, &param, false);

    switch (param.opcode) {
    case ESP_BLE_MESH_DFU_OP_UPDATE_INFO_GET:
        return bt_mesh_dfu_cli_imgs_get(dfu_cli, &param.ctx, (bt_mesh_dfu_img_cb_t) get->dfu_update_info_get.img_cb,
                                        NULL, get->dfu_update_info_get.first_index,
                                        get->dfu_update_info_get.entries_limit);
    case ESP_BLE_MESH_DFU_OP_UPDATE_METADATA_CHECK:
        return bt_mesh_dfu_cli_metadata_check(dfu_cli, &param.ctx, get->dfu_metadata_check.update_firmware_index,
                                              get->dfu_metadata_check.metadata);
    case ESP_BLE_MESH_DFU_OP_UPDATE_GET:
        return bt_mesh_dfu_cli_status_get(dfu_cli, &param.ctx);
    case ESP_BLE_MESH_DFU_OP_UPDATE_START:
        // automatic start
        return 0;
    case ESP_BLE_MESH_DFU_OP_UPDATE_CANCEL:
        return bt_mesh_dfu_cli_cancel(dfu_cli, &param.ctx);
    case ESP_BLE_MESH_DFU_OP_UPDATE_APPLY:
        return bt_mesh_dfu_cli_apply(dfu_cli);
    default:
        BT_ERR("Invalid Device firmware Get opcode 0x%04x", param.opcode);
        return -EINVAL;
    }
}

void btc_ble_mesh_dfu_client_call_handler(btc_msg_t *msg)
{
    esp_ble_mesh_dfu_client_cb_param_t cb = {0};
    btc_ble_mesh_dfu_client_args_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_dfu_client_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_DFU_CLIENT_GET_STATE:
        cb.params = arg->dfu_get.params;
        cb.send.err_code = btc_ble_mesh_dfu_client_get_state(arg->dfu_get.params,
                                                             arg->dfu_get.get);
        btc_ble_mesh_dfu_client_cb(&cb,
                                   ESP_BLE_MESH_DFU_CLIENT_SEND_COMP_EVT);
        break;
    case BTC_BLE_MESH_ACT_DFU_CLIENT_IMG_SEND:
        cb.send.err_code = bt_mesh_dfu_cli_send((struct bt_mesh_dfu_cli *)arg->send_arg.cli,
                                                (struct bt_mesh_blob_cli_inputs *)arg->send_arg.inputs,
                                                (struct bt_mesh_blob_io *)arg->send_arg.io,
                                                (struct bt_mesh_dfu_cli_xfer *)arg->send_arg.xfer);
        btc_ble_mesh_dfu_client_cb(&cb,
                                   ESP_BLE_MESH_DFU_CLIENT_IMG_SEND_CMP_EVT);
        break;
    default:
        break;
    }

    btc_ble_mesh_dfu_client_arg_deep_free(msg);
}

void btc_ble_mesh_dfu_client_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_dfu_client_cb_param_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_dfu_client_cb_param_t *)msg->arg;

    if (msg->act < ESP_BLE_MESH_DFU_CLIENT_EVT_MAX) {
        btc_ble_mesh_dfu_client_cb_to_app(msg->act, arg);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }

    btc_ble_mesh_dfu_client_free_req_data(msg);
}

uint8_t btc_ble_mesh_dfu_cli_progress(struct esp_ble_mesh_dfu_cli *cli)
{
    return bt_mesh_dfu_cli_progress((struct bt_mesh_dfu_cli *)cli);
}

#endif /* CONFIG_BLE_MESH_DFU_CLI */

#if CONFIG_BLE_MESH_DFU_SRV

void btc_ble_mesh_dfu_srv_verified(esp_ble_mesh_dfu_srv_t *srv)
{
    bt_mesh_dfu_srv_verified((struct bt_mesh_dfu_srv *)srv);
}

void btc_ble_mesh_dfu_srv_rejected(esp_ble_mesh_dfu_srv_t *srv)
{
    bt_mesh_dfu_srv_rejected((struct bt_mesh_dfu_srv *)srv);
}

void btc_ble_mesh_dfu_srv_cancel(esp_ble_mesh_dfu_srv_t *srv)
{
    bt_mesh_dfu_srv_cancel((struct bt_mesh_dfu_srv *)srv);
}

void btc_ble_mesh_dfu_srv_applied(esp_ble_mesh_dfu_srv_t *srv)
{
    bt_mesh_dfu_srv_applied((struct bt_mesh_dfu_srv *)srv);
}

bool btc_ble_mesh_dfu_srv_is_busy(const esp_ble_mesh_dfu_srv_t *srv)
{
    return bt_mesh_dfu_srv_is_busy((struct bt_mesh_dfu_srv *)srv);
}

uint8_t btc_ble_mesh_dfu_srv_progress(const esp_ble_mesh_dfu_srv_t *srv)
{
    return bt_mesh_dfu_srv_progress((struct bt_mesh_dfu_srv *)srv);
}
#endif /* CONFIG_BLE_MESH_DFU_SRV */

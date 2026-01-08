/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
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
#include "mesh_v1.1/dfu/dfd.h"
#include "mesh_v1.1/dfu/dfd_cli.h"
#include "mesh_v1.1/dfu/dfd_srv.h"

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
                            bt_mesh_free(dst->dfu_get.get);
                            bt_mesh_free(dst->dfu_get.params);
                            BT_ERR("Out of memory for metadata");
                            return;
                        }
                    } else {
                        bt_mesh_free(dst->dfu_get.get);
                        bt_mesh_free(dst->dfu_get.params);
                        BT_ERR("Metadata should be exists");
                        return;
                    }
                }
            } else {
                bt_mesh_free(dst->dfu_get.params);
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            }
        }
        break;
    }
    case BTC_BLE_MESH_ACT_DFU_CLIENT_IMG_SEND:
        /* That will be freed when dfu completed or failed not on btc deep free */
        dst->send_arg.inputs =(struct esp_ble_mesh_blob_cli_inputs *)
            dfu_targets_alloc((struct bt_mesh_blob_cli_inputs *)src->send_arg.inputs);
        break;
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
        if (arg->dfu_get.get) {
            switch (arg->dfu_get.params->opcode) {
                case ESP_BLE_MESH_DFU_OP_UPDATE_METADATA_CHECK:
                    if (arg->dfu_get.get->dfu_metadata_check.metadata) {
                        bt_mesh_free_buf(arg->dfu_get.get->dfu_metadata_check.metadata);
                    }
                break;
                default:
                break;
            }
            bt_mesh_free(arg->dfu_get.get);
        }
        if (arg->dfu_get.params) {
            bt_mesh_free(arg->dfu_get.params);
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
                        bt_mesh_free(p_dest_data->params);
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        return;
                    }
                    memcpy(p_dest_data->recv.dfu_update_info_status.fw_info_list,
                           p_src_data->recv.dfu_update_info_status.fw_info_list,
                           length);
                } else {
                    p_dest_data->recv.dfu_update_info_status.fw_info_list = NULL;
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

#if CONFIG_BLE_MESH_DFD_CLI
static inline void btc_ble_mesh_dfd_client_cb_to_app(btc_ble_mesh_dfd_client_cb_evt_t event,
                                                     esp_ble_mesh_dfd_client_cb_param_t *param)
{
    esp_ble_mesh_dfd_client_cb_t btc_ble_mesh_cb =
                        (esp_ble_mesh_dfd_client_cb_t)btc_profile_cb_get(BTC_PID_DFD_CLIENT);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

static inline bool dfd_client_param_need(uint32_t opcode)
{
    switch (opcode) {
    case ESP_BLE_MESH_DFD_OP_RECEIVERS_GET:
    case ESP_BLE_MESH_DFD_OP_FW_GET:
    case ESP_BLE_MESH_DFD_OP_FW_GET_BY_INDEX:
    case ESP_BLE_MESH_DFD_OP_RECEIVERS_ADD:
    case ESP_BLE_MESH_DFD_OP_START:
    case ESP_BLE_MESH_DFD_OP_UPLOAD_START:
    case ESP_BLE_MESH_DFD_OP_UPLOAD_START_OOB:
    case ESP_BLE_MESH_DFD_OP_FW_DELETE:
        return true;
    default:
    break;
    }
    return false;
}

static int btc_ble_mesh_dfd_client_get(esp_ble_mesh_client_common_param_t *params,
                                       esp_ble_mesh_dfd_client_get_param_t *get)
{
    bt_mesh_client_common_param_t param = {0};

    if (params == NULL) {
        BT_ERR("%s:InvParam", __func__);
        return -EINVAL;
    }

    if (dfd_client_param_need(params->opcode) != (!!get)) {
        BT_ERR("%s:InvParam", __func__);
        return -EINVAL;
    }

    switch(params->opcode) {
        case ESP_BLE_MESH_DFD_OP_RECEIVERS_GET:
        case ESP_BLE_MESH_DFD_OP_FW_GET:
        case ESP_BLE_MESH_DFD_OP_FW_GET_BY_INDEX:
            if (get == NULL) {
                BT_ERR("%s:InvParam", __func__);
                return -EINVAL;
            }
        break;
        default:
        break;
    }

    btc_ble_mesh_set_client_common_param(params, &param, false);

    switch (params->opcode) {
        case ESP_BLE_MESH_DFD_OP_RECEIVERS_GET:
            return bt_mesh_dfd_cli_receivers_get(&param, get->receivers_get.first_index,
                                                  get->receivers_get.entries_limit);
        case ESP_BLE_MESH_DFD_OP_CAPABILITIES_GET:
            return bt_mesh_dfd_cli_distribution_capabilities_get(&param);
        case ESP_BLE_MESH_DFD_OP_GET:
            return bt_mesh_dfd_cli_distribution_get(&param);
        case ESP_BLE_MESH_DFD_OP_UPLOAD_GET:
            return bt_mesh_dfd_cli_distribution_upload_get(&param);
        case ESP_BLE_MESH_DFD_OP_FW_GET:
            return bt_mesh_dfd_cli_firmware_get(&param, get->dist_fw_get.fwid);
        case ESP_BLE_MESH_DFD_OP_FW_GET_BY_INDEX:
            return bt_mesh_dfd_cli_firmware_get_by_index(&param, get->dist_fw_get_by_idx.dist_fw_idx);
        default:
            BT_ERR("UknOpc:%04x", params->opcode);
            return -EINVAL;
    }
}

static int btc_ble_mesh_dfd_client_set(esp_ble_mesh_client_common_param_t *params,
                                       esp_ble_mesh_dfd_client_set_param_t *set)
{
    bt_mesh_client_common_param_t param = {0};

    if (params == NULL) {
        BT_ERR("%s:InvParam", __func__);
        return -EINVAL;
    }

    if (dfd_client_param_need(params->opcode) != (!!set)) {
        BT_ERR("%s:InvParam", __func__);
        return -EINVAL;
    }

    btc_ble_mesh_set_client_common_param(params, &param, false);

    switch (params->opcode) {
        case ESP_BLE_MESH_DFD_OP_RECEIVERS_ADD:
            return bt_mesh_dfd_cli_receivers_add(&param, (dfd_cli_receiver_entry_t *)set->receivers_add.receivers, set->receivers_add.receivers_cnt);
        case ESP_BLE_MESH_DFD_OP_RECEIVERS_DELETE_ALL:
            return bt_mesh_dfd_cli_receivers_delete_all(&param);
        case ESP_BLE_MESH_DFD_OP_START:
            return bt_mesh_dfd_cli_distribution_start(&param, (dfd_cli_dist_start_t *)&set->dist_start);
        case ESP_BLE_MESH_DFD_OP_SUSPEND:
            return bt_mesh_dfd_cli_distribution_suspend(&param);
        case ESP_BLE_MESH_DFD_OP_CANCEL:
            return bt_mesh_dfd_cli_distribution_cancel(&param);
        case ESP_BLE_MESH_DFD_OP_APPLY:
            return bt_mesh_dfd_cli_distribution_apply(&param);
        case ESP_BLE_MESH_DFD_OP_UPLOAD_START:
            return bt_mesh_dfd_cli_distribution_upload_start(&param, (dfd_cli_dist_upload_start_t *)&set->dist_upload_start);
        case ESP_BLE_MESH_DFD_OP_UPLOAD_START_OOB:
            return bt_mesh_dfd_cli_distribution_upload_oob_start(&param, (dfd_cli_dist_upload_oob_start_t *)&set->dist_upload_oob_start);
        case ESP_BLE_MESH_DFD_OP_UPLOAD_CANCEL:
            return  bt_mesh_dfd_cli_distribution_upload_oob_cancel(&param);
        case ESP_BLE_MESH_DFD_OP_FW_DELETE:
            return bt_mesh_dfd_cli_firmware_get_delete(&param, set->dist_fw_del.fwid);
        case ESP_BLE_MESH_DFD_OP_FW_DELETE_ALL:
            return bt_mesh_dfd_cli_firmware_delete_all(&param);
        default:
            BT_ERR("UknOpc:%04x", params->opcode);
            return -EINVAL;
    }
}

void btc_ble_mesh_dfd_client_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_dfd_client_cb_param_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_dfd_client_cb_param_t *)msg->arg;

    if (msg->act < ESP_BLE_MESH_EVT_DFD_CLIENT_MAX) {
        btc_ble_mesh_dfd_client_cb_to_app(msg->act, arg);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }

    btc_ble_mesh_dfd_client_rsp_deep_free(msg);
}

void btc_ble_mesh_dfd_client_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_ble_mesh_dfd_client_args_t *dst = (btc_ble_mesh_dfd_client_args_t *)p_dest;
    btc_ble_mesh_dfd_client_args_t *src = (btc_ble_mesh_dfd_client_args_t *)p_src;

    if (!msg || !dst || !src) {
        BT_ERR("%s,InvParam", __func__);
        return;
    }

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_DFD_CLIENT_GET:
        dst->dfd_client_get.params =
            (esp_ble_mesh_client_common_param_t *)bt_mesh_calloc(sizeof(esp_ble_mesh_client_common_param_t));
        if (dst->dfd_client_get.params == NULL) {
            BT_ERR("%s:%d,OutMem", __func__, __LINE__);
            break;
        }
        memcpy(dst->dfd_client_get.params, src->dfd_client_get.params, sizeof(esp_ble_mesh_client_common_param_t));

        if (src->dfd_client_get.get) {
            dst->dfd_client_get.get = (esp_ble_mesh_dfd_client_get_param_t *)bt_mesh_calloc(sizeof(esp_ble_mesh_dfd_client_get_param_t));
            if (dst->dfd_client_get.get == NULL) {
                bt_mesh_free(dst->dfd_client_get.params);
                dst->dfd_client_get.params = NULL;
                BT_ERR("%s:%d,OutMem", __func__, __LINE__);
                break;
            }
            memcpy(dst->dfd_client_get.get, src->dfd_client_get.get, sizeof(esp_ble_mesh_dfd_client_get_param_t));
        }

        switch (dst->dfd_client_get.params->opcode) {
            case ESP_BLE_MESH_DFD_OP_FW_GET:
                if (src->dfd_client_get.get->dist_fw_get.fwid == NULL) {
                    BT_ERR("%s:%d,InvParam", __func__, __LINE__);
                    bt_mesh_free(dst->dfd_client_get.params);
                    dst->dfd_client_get.params = NULL;
                    bt_mesh_free(dst->dfd_client_get.get);
                    dst->dfd_client_get.get = NULL;
                    break;
                }

                dst->dfd_client_get.get->dist_fw_get.fwid =
                    bt_mesh_alloc_buf(src->dfd_client_get.get->dist_fw_get.fwid->len);
                if (dst->dfd_client_get.get->dist_fw_get.fwid == NULL) {
                    bt_mesh_free(dst->dfd_client_get.params);
                    dst->dfd_client_get.params = NULL;
                    bt_mesh_free(dst->dfd_client_get.get);
                    dst->dfd_client_get.get = NULL;
                    BT_ERR("%s:%d,OutMem", __func__, __LINE__);
                    break;
                }
                net_buf_simple_add_mem(
                    dst->dfd_client_get.get->dist_fw_get.fwid,
                    src->dfd_client_get.get->dist_fw_get.fwid->data,
                    src->dfd_client_get.get->dist_fw_get.fwid->len);
            break;
            default:
                break;
        }
        break;
    case BTC_BLE_MESH_ACT_DFD_CLIENT_SET:
        dst->dfd_client_set.params =
            (esp_ble_mesh_client_common_param_t *)bt_mesh_calloc(sizeof(esp_ble_mesh_client_common_param_t));
        if (dst->dfd_client_set.params == NULL) {
            BT_ERR("%s:%d,OutMem", __func__, __LINE__);
            break;
        }
        memcpy(dst->dfd_client_set.params, src->dfd_client_set.params, sizeof(esp_ble_mesh_client_common_param_t));

        if (src->dfd_client_set.set) {
            dst->dfd_client_set.set = (esp_ble_mesh_dfd_client_set_param_t *)bt_mesh_calloc(sizeof(esp_ble_mesh_dfd_client_set_param_t));
            if (dst->dfd_client_set.set == NULL) {
                bt_mesh_free(dst->dfd_client_set.params);
                dst->dfd_client_set.params = NULL;
                BT_ERR("%s:%d,OutMem", __func__, __LINE__);
                break;
            }
            memcpy(dst->dfd_client_set.set, src->dfd_client_set.set, sizeof(esp_ble_mesh_dfd_client_set_param_t));
        }
        switch (dst->dfd_client_set.params->opcode) {
            case ESP_BLE_MESH_DFD_OP_RECEIVERS_ADD:
                if (src->dfd_client_set.set->receivers_add.receivers_cnt == 0) {
                    dst->dfd_client_set.set->receivers_add.receivers = NULL;
                    bt_mesh_free(dst->dfd_client_set.params);
                    dst->dfd_client_set.params = NULL;
                    bt_mesh_free(dst->dfd_client_set.set);
                    dst->dfd_client_set.set = NULL;
                    BT_ERR("%s:%d,InvParam", __func__, __LINE__);
                    break;
                }
                dst->dfd_client_set.set->receivers_add.receivers =
                    (esp_ble_mesh_dfd_cli_receiver_entry_t *)bt_mesh_calloc(dst->dfd_client_set.set->receivers_add.receivers_cnt *
                                                                            sizeof(esp_ble_mesh_dfd_cli_receiver_entry_t));
                if (dst->dfd_client_set.set->receivers_add.receivers == NULL) {
                    bt_mesh_free(dst->dfd_client_set.params);
                    dst->dfd_client_set.params = NULL;
                    bt_mesh_free(dst->dfd_client_set.set);
                    dst->dfd_client_set.set = NULL;
                    BT_ERR("%s:%d,OutMem", __func__, __LINE__);
                    break;
                }
                memcpy(dst->dfd_client_set.set->receivers_add.receivers, src->dfd_client_set.set->receivers_add.receivers,
                       dst->dfd_client_set.set->receivers_add.receivers_cnt * sizeof(esp_ble_mesh_dfd_cli_receiver_entry_t));
            break;
            case ESP_BLE_MESH_DFD_OP_UPLOAD_START:
                if (src->dfd_client_set.set->dist_upload_start.fwid == NULL) {
                    dst->dfd_client_set.set->dist_upload_start.fwid = NULL;
                    bt_mesh_free(dst->dfd_client_set.params);
                    dst->dfd_client_set.params = NULL;
                    bt_mesh_free(dst->dfd_client_set.set);
                    dst->dfd_client_set.set = NULL;
                    BT_ERR("%s:%d,InvParam", __func__, __LINE__);
                    break;
                }

                dst->dfd_client_set.set->dist_upload_start.fwid =
                        bt_mesh_alloc_buf(src->dfd_client_set.set->dist_upload_start.fwid->len);
                if (dst->dfd_client_set.set->dist_upload_start.fwid == NULL) {
                    bt_mesh_free(dst->dfd_client_set.params);
                    dst->dfd_client_set.params = NULL;
                    bt_mesh_free(dst->dfd_client_set.set);
                    dst->dfd_client_set.set = NULL;
                    BT_ERR("%s:%d,OutMem", __func__, __LINE__);
                    break;
                }
                net_buf_simple_add_mem(
                    dst->dfd_client_set.set->dist_upload_start.fwid,
                    src->dfd_client_set.set->dist_upload_start.fwid->data,
                    src->dfd_client_set.set->dist_upload_start.fwid->len);

                if (src->dfd_client_set.set->dist_upload_start.fw_metadata->len == 0) {
                    dst->dfd_client_set.set->dist_upload_start.fw_metadata = NULL;
                    break;
                } else {
                    dst->dfd_client_set.set->dist_upload_start.fw_metadata =
                        bt_mesh_alloc_buf(src->dfd_client_set.set->dist_upload_start.fw_metadata->len);
                    if (dst->dfd_client_set.set->dist_upload_start.fw_metadata == NULL) {
                        bt_mesh_free(dst->dfd_client_set.params);
                        dst->dfd_client_set.params = NULL;
                        bt_mesh_free(dst->dfd_client_set.set);
                        dst->dfd_client_set.set = NULL;
                        bt_mesh_free_buf(dst->dfd_client_set.set->dist_upload_start.fwid);
                        dst->dfd_client_set.set->dist_upload_start.fwid = NULL;
                        BT_ERR("%s:%d,OutMem", __func__, __LINE__);
                        break;
                    }
                    net_buf_simple_add_mem(
                        dst->dfd_client_set.set->dist_upload_start.fw_metadata,
                        src->dfd_client_set.set->dist_upload_start.fw_metadata->data,
                        src->dfd_client_set.set->dist_upload_start.fw_metadata->len);
                }
            break;
            case ESP_BLE_MESH_DFD_OP_UPLOAD_START_OOB:
                if (src->dfd_client_set.set->dist_upload_oob_start.url == NULL ||
                    src->dfd_client_set.set->dist_upload_oob_start.fwid == NULL) {
                    bt_mesh_free(dst->dfd_client_set.params);
                    dst->dfd_client_set.params = NULL;
                    bt_mesh_free(dst->dfd_client_set.set);
                    dst->dfd_client_set.set = NULL;
                    BT_ERR("%s:%d,InvParam", __func__, __LINE__);
                    break;
                }
                dst->dfd_client_set.set->dist_upload_oob_start.url =
                        bt_mesh_alloc_buf(src->dfd_client_set.set->dist_upload_oob_start.url->len);
                if (dst->dfd_client_set.set->dist_upload_oob_start.url == NULL) {
                    bt_mesh_free(dst->dfd_client_set.params);
                    dst->dfd_client_set.params = NULL;
                    bt_mesh_free(dst->dfd_client_set.set);
                    dst->dfd_client_set.set = NULL;
                    BT_ERR("%s:%d,OutMem", __func__, __LINE__);
                    break;
                }
                dst->dfd_client_set.set->dist_upload_oob_start.fwid =
                        bt_mesh_alloc_buf(src->dfd_client_set.set->dist_upload_oob_start.fwid->len);
                if (dst->dfd_client_set.set->dist_upload_oob_start.fwid == NULL) {
                    bt_mesh_free(dst->dfd_client_set.params);
                    dst->dfd_client_set.params = NULL;
                    bt_mesh_free(dst->dfd_client_set.set);
                    dst->dfd_client_set.set = NULL;
                    bt_mesh_free_buf(dst->dfd_client_set.set->dist_upload_oob_start.url);
                    dst->dfd_client_set.set->dist_upload_oob_start.url = NULL;
                    BT_ERR("%s:%d,OutMem", __func__, __LINE__);
                    break;
                }
                net_buf_simple_add_mem(
                    dst->dfd_client_set.set->dist_upload_oob_start.url,
                    src->dfd_client_set.set->dist_upload_oob_start.url->data,
                    src->dfd_client_set.set->dist_upload_oob_start.url->len);
                net_buf_simple_add_mem(
                    dst->dfd_client_set.set->dist_upload_oob_start.fwid,
                    src->dfd_client_set.set->dist_upload_oob_start.fwid->data,
                    src->dfd_client_set.set->dist_upload_oob_start.fwid->len);
            break;
            case ESP_BLE_MESH_DFD_OP_FW_DELETE:
                if (src->dfd_client_set.set->dist_fw_del.fwid == NULL) {
                    bt_mesh_free(dst->dfd_client_set.params);
                    dst->dfd_client_set.params = NULL;
                    bt_mesh_free(dst->dfd_client_set.set);
                    dst->dfd_client_set.set = NULL;
                    BT_ERR("%s:%d,InvParam", __func__, __LINE__);
                    break;
                }
                dst->dfd_client_set.set->dist_fw_del.fwid =
                        bt_mesh_alloc_buf(src->dfd_client_set.set->dist_fw_del.fwid->len);
                if (dst->dfd_client_set.set->dist_fw_del.fwid == NULL) {
                    bt_mesh_free(dst->dfd_client_set.params);
                    dst->dfd_client_set.params = NULL;
                    bt_mesh_free(dst->dfd_client_set.set);
                    dst->dfd_client_set.set = NULL;
                    BT_ERR("%s:%d,OutMem", __func__, __LINE__);
                    break;
                }
                net_buf_simple_add_mem(
                    dst->dfd_client_set.set->dist_fw_del.fwid,
                    src->dfd_client_set.set->dist_fw_del.fwid->data,
                    src->dfd_client_set.set->dist_fw_del.fwid->len);
            break;
            default:
                break;
        }
    default:
        BT_DBG("%s, Unknown act %d", __func__, msg->act);
        break;
    }
}

void btc_ble_mesh_dfd_client_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_mesh_dfd_client_args_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    if (msg->act >= ESP_BLE_MESH_ACT_DFD_CLIENT_MAX) {
        BT_ERR("%s, Invalid event %d", __func__, msg->act);
        return;
    }

    arg = (btc_ble_mesh_dfd_client_args_t *)msg->arg;

    switch (msg->act) {
    case ESP_BLE_MESH_ACT_DFD_CLIENT_GET:
        if (arg->dfd_client_get.params == NULL) {
            BT_ERR("%s:%d,InvParam", __func__, __LINE__);
            break;
        }
        switch (arg->dfd_client_get.params->opcode) {
            case ESP_BLE_MESH_DFD_OP_FW_GET:
                if (arg->dfd_client_get.get->dist_fw_get.fwid == NULL) {
                    BT_ERR("%s:%d,InvParam", __func__, __LINE__);
                    break;
                }
                bt_mesh_free_buf(arg->dfd_client_get.get->dist_fw_get.fwid);
            break;
            default:
            break;
        }
        if (arg->dfd_client_get.get) {
            bt_mesh_free(arg->dfd_client_get.get);
        }
        bt_mesh_free(arg->dfd_client_get.params);
    break;
    case ESP_BLE_MESH_ACT_DFD_CLIENT_SET:
        if (arg->dfd_client_set.params == NULL) {
            BT_ERR("%s:%d,InvParam", __func__, __LINE__);
            break;
        }
        switch (arg->dfd_client_set.params->opcode) {
            case ESP_BLE_MESH_DFD_OP_RECEIVERS_ADD:
                if (arg->dfd_client_set.set->receivers_add.receivers == NULL) {
                    BT_ERR("%s:%d,InvParam", __func__, __LINE__);
                    break;
                }
                bt_mesh_free(arg->dfd_client_set.set->receivers_add.receivers);
            break;
            case ESP_BLE_MESH_DFD_OP_UPLOAD_START:
                if (arg->dfd_client_set.set->dist_upload_start.fwid == NULL) {
                    BT_ERR("%s:%d,InvParam", __func__, __LINE__);
                    break;
                }
                bt_mesh_free_buf(arg->dfd_client_set.set->dist_upload_start.fwid);
                if (arg->dfd_client_set.set->dist_upload_start.fw_metadata) {
                    bt_mesh_free_buf(arg->dfd_client_set.set->dist_upload_start.fw_metadata);
                }
            break;
            case ESP_BLE_MESH_DFD_OP_UPLOAD_START_OOB:
                if (arg->dfd_client_set.set->dist_upload_oob_start.url == NULL ||
                    arg->dfd_client_set.set->dist_upload_oob_start.fwid == NULL) {
                    BT_ERR("%s:%d,InvParam", __func__, __LINE__);
                    break;
                }
                bt_mesh_free_buf(arg->dfd_client_set.set->dist_upload_oob_start.url);
                bt_mesh_free_buf(arg->dfd_client_set.set->dist_upload_oob_start.fwid);
            break;
            case ESP_BLE_MESH_DFD_OP_FW_DELETE:
                if (arg->dfd_client_set.set->dist_fw_del.fwid == NULL) {
                    BT_ERR("%s:%d,InvParam", __func__, __LINE__);
                    break;
                }
                bt_mesh_free_buf(arg->dfd_client_set.set->dist_fw_del.fwid);
            break;
        }
        if (arg->dfd_client_set.set) {
            bt_mesh_free(arg->dfd_client_set.set);
        }
        bt_mesh_free(arg->dfd_client_set.params);
    break;
    default:
        BT_WARN("Unprocessed event %d", msg->act);
        break;
    }

    return;
}

void btc_ble_mesh_dfd_client_rsp_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    esp_ble_mesh_dfd_client_cb_param_t *dst =(esp_ble_mesh_dfd_client_cb_param_t *) p_dest;
    esp_ble_mesh_dfd_client_cb_param_t *src =(esp_ble_mesh_dfd_client_cb_param_t *) p_src;

    if (!msg || !dst || !src) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    dst->err_code = src->err_code;

    if (src->params) {
        dst->params = bt_mesh_calloc(sizeof(esp_ble_mesh_client_common_param_t));
        if (dst->params == NULL) {
            BT_ERR("%s:%d,OutOfMem", __func__, __LINE__);
            return;
        }
        memcpy(dst->params, src->params, sizeof(esp_ble_mesh_client_common_param_t));
    }

    switch (msg->act) {
        case ESP_BLE_MESH_EVT_DFD_CLIENT_RECV_RSP:
            switch(dst->params->opcode) {
                case BLE_MESH_DFD_OP_RECEIVERS_LIST:
                    dst->status_cb.receiver_list.first_index = src->status_cb.receiver_list.first_index;
                    dst->status_cb.receiver_list.entries_cnt = src->status_cb.receiver_list.entries_cnt;
                    if (dst->status_cb.receiver_list.entries_cnt) {
                        dst->status_cb.receiver_list.entries = bt_mesh_calloc(sizeof(esp_ble_mesh_dfd_target_node_entry_t) * dst->status_cb.receiver_list.entries_cnt);
                        if (dst->status_cb.receiver_list.entries == NULL) {
                            BT_ERR("%s:%d,OutOfMem", __func__, __LINE__);
                            return;
                        }
                        memcpy(dst->status_cb.receiver_list.entries, src->status_cb.receiver_list.entries,
                            sizeof(esp_ble_mesh_dfd_target_node_entry_t) * dst->status_cb.receiver_list.entries_cnt);
                    } else {
                        dst->status_cb.receiver_list.entries = NULL;
                    }
                break;
                case BLE_MESH_DFD_OP_CAPABILITIES_STATUS:
                    dst->status_cb.dist_caps.max_receiver_list_sz = src->status_cb.dist_caps.max_receiver_list_sz;
                    dst->status_cb.dist_caps.max_fw_list_sz = src->status_cb.dist_caps.max_fw_list_sz;
                    dst->status_cb.dist_caps.max_fw_sz = src->status_cb.dist_caps.max_fw_sz;
                    dst->status_cb.dist_caps.max_upload_space = src->status_cb.dist_caps.max_upload_space;
                    dst->status_cb.dist_caps.remaining_upload_space = src->status_cb.dist_caps.remaining_upload_space;
                    dst->status_cb.dist_caps.oob_retrieval_supported = src->status_cb.dist_caps.oob_retrieval_supported;
                    if (src->status_cb.dist_caps.supported_url_scheme_names) {
                     dst->status_cb.dist_caps.supported_url_scheme_names =
                        bt_mesh_alloc_buf(src->status_cb.dist_caps.supported_url_scheme_names->len);
                        if (dst->status_cb.dist_caps.supported_url_scheme_names == NULL) {
                            BT_ERR("%s:%d,OutOfMem", __func__, __LINE__);
                            return;
                        }
                        net_buf_simple_add_mem(
                            dst->status_cb.dist_caps.supported_url_scheme_names,
                            src->status_cb.dist_caps.supported_url_scheme_names->data,
                            src->status_cb.dist_caps.supported_url_scheme_names->len);
                    } else {
                        dst->status_cb.dist_caps.supported_url_scheme_names = NULL;
                    }
                break;
                case BLE_MESH_DFD_OP_UPLOAD_STATUS:
                    dst->status_cb.upload_status.status = src->status_cb.upload_status.status;
                    dst->status_cb.upload_status.upload_phase = src->status_cb.upload_status.upload_phase;
                    dst->status_cb.upload_status.upload_progress = src->status_cb.upload_status.upload_progress;
                    dst->status_cb.upload_status.upload_type = src->status_cb.upload_status.upload_type;

                    if (dst->status_cb.upload_status.upload_progress == ESP_BLE_MESH_DFD_UPLOAD_PROGRESS_UNSET) {
                        dst->status_cb.upload_status.fwid = NULL;
                        return;
                    }

                    if (dst->status_cb.upload_status.upload_type == ESP_BLE_MESH_DFD_UPLOAD_TYPE_INBAND) {
                        if (src->status_cb.upload_status.fwid) {
                            dst->status_cb.upload_status.fwid = bt_mesh_alloc_buf(src->status_cb.upload_status.fwid->len);
                            if (dst->status_cb.upload_status.fwid == NULL) {
                                BT_ERR("%s:%d,OutOfMem", __func__, __LINE__);
                                return;
                            }
                            net_buf_simple_add_mem(
                                dst->status_cb.upload_status.fwid,
                                src->status_cb.upload_status.fwid->data,
                                src->status_cb.upload_status.fwid->len);
                        } else {
                            BT_ERR("%s:%d,InvParam", __func__, __LINE__);
                        }
                    } else {
                        if(src->status_cb.upload_status.oob_fwid) {
                            dst->status_cb.upload_status.oob_fwid = bt_mesh_alloc_buf(src->status_cb.upload_status.oob_fwid->len);
                            if (dst->status_cb.upload_status.oob_fwid == NULL) {
                                BT_ERR("%s:%d,OutOfMem", __func__, __LINE__);
                                return;
                            }
                            net_buf_simple_add_mem(
                                dst->status_cb.upload_status.oob_fwid,
                                src->status_cb.upload_status.oob_fwid->data,
                                src->status_cb.upload_status.oob_fwid->len);
                        } else {
                            BT_ERR("%s:%d,InvParam", __func__, __LINE__);
                        }
                    }
                break;
                case BLE_MESH_DFD_OP_FW_STATUS:
                    dst->status_cb.firmware_status.status = src->status_cb.firmware_status.status;
                    dst->status_cb.firmware_status.entry_cnt = src->status_cb.firmware_status.entry_cnt;
                    dst->status_cb.firmware_status.fw_idx = src->status_cb.firmware_status.fw_idx;
                    if (src->status_cb.firmware_status.fwid) {
                        dst->status_cb.firmware_status.fwid = bt_mesh_alloc_buf(src->status_cb.firmware_status.fwid->len);
                        if (dst->status_cb.firmware_status.fwid == NULL) {
                            BT_ERR("%s:%d,OutOfMem", __func__, __LINE__);
                            return;
                        }
                        net_buf_simple_add_mem(
                            dst->status_cb.firmware_status.fwid,
                            src->status_cb.firmware_status.fwid->data,
                            src->status_cb.firmware_status.fwid->len);
                    } else {
                        dst->status_cb.firmware_status.fwid = NULL;
                    }
                break;
                case BLE_MESH_DFD_OP_RECEIVERS_STATUS:
                    dst->status_cb.receiver_status.status = src->status_cb.receiver_status.status;
                    dst->status_cb.receiver_status.receiver_list_cnt = src->status_cb.receiver_status.receiver_list_cnt;
                break;
                case BLE_MESH_DFD_OP_STATUS:
                    dst->status_cb.dist_status.status = src->status_cb.dist_status.status;
                    dst->status_cb.dist_status.dist_phase = src->status_cb.dist_status.dist_phase;
                    dst->status_cb.dist_status.multicast_address = src->status_cb.dist_status.multicast_address;
                    dst->status_cb.dist_status.appkey_idx = src->status_cb.dist_status.appkey_idx;
                    dst->status_cb.dist_status.ttl = src->status_cb.dist_status.ttl;
                    dst->status_cb.dist_status.timeout_base = src->status_cb.dist_status.timeout_base;
                    dst->status_cb.dist_status.trans_mode = src->status_cb.dist_status.trans_mode;
                    dst->status_cb.dist_status.update_policy = src->status_cb.dist_status.update_policy;
                    dst->status_cb.dist_status.firmware_image_index = src->status_cb.dist_status.firmware_image_index;
                break;
                default:
                    BT_ERR("Unknown opcode %04x", dst->params->opcode);
                break;

            }
        break;
        case ESP_BLE_MESH_EVT_DFD_CLIENT_TIMEOUT:
        break;
        case ESP_BLE_MESH_ACT_DFD_CLIEND_SEND_COMP:
        break;
        default:
         BT_ERR("Unknown event %d", msg->act);
    }

}

void btc_ble_mesh_dfd_client_rsp_deep_free(btc_msg_t *msg)
{
    esp_ble_mesh_dfd_client_cb_param_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s:%d,InvParam", __func__, __LINE__);
        return;
    }

    arg = (esp_ble_mesh_dfd_client_cb_param_t *)(msg->arg);

    if (arg->params == NULL &&
        msg->act != ESP_BLE_MESH_ACT_DFD_CLIEND_SEND_COMP) {
        BT_ERR("%s:%d,InvParam", __func__, __LINE__);
        return;
    }

    switch (msg->act) {
        case ESP_BLE_MESH_EVT_DFD_CLIENT_RECV_RSP:
            switch (arg->params->opcode) {
                case BLE_MESH_DFD_OP_RECEIVERS_LIST:
                    if (arg->status_cb.receiver_list.entries) {
                        bt_mesh_free(arg->status_cb.receiver_list.entries);
                        arg->status_cb.receiver_list.entries = NULL;
                    }
                break;
                case BLE_MESH_DFD_OP_CAPABILITIES_STATUS:
                    if (arg->status_cb.dist_caps.supported_url_scheme_names) {
                        bt_mesh_free_buf(arg->status_cb.dist_caps.supported_url_scheme_names);
                        arg->status_cb.dist_caps.supported_url_scheme_names = NULL;
                    }
                break;
                case BLE_MESH_DFD_OP_UPLOAD_STATUS:
                    /**
                     * firmware_id and upload_oob_firmware_id are a union
                     * structure, so only one pointer needs to be released
                     */
                    if (arg->status_cb.upload_status.fwid) {
                        bt_mesh_free_buf(arg->status_cb.upload_status.fwid);
                        arg->status_cb.upload_status.fwid = NULL;
                    }
                break;
                case BLE_MESH_DFD_OP_FW_STATUS:
                    if (arg->status_cb.firmware_status.fwid) {
                        bt_mesh_free_buf(arg->status_cb.firmware_status.fwid);
                        arg->status_cb.firmware_status.fwid = NULL;
                    }
                break;
            }
        break;
        default:
        break;
    }

    if (arg->params) {
        bt_mesh_free(arg->params);
    }
}

static void btc_ble_mesh_dfd_client_callback(esp_ble_mesh_dfd_client_cb_param_t *cb_params,
                                             uint8_t act)
{
    btc_msg_t msg = {0};

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_DFD_CLIENT)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_DFD_CLIENT;
    msg.act = act;

    btc_transfer_context(&msg, cb_params, cb_params == NULL ? 0 : sizeof(esp_ble_mesh_dfd_client_cb_param_t),
                         btc_ble_mesh_dfd_client_rsp_deep_copy, btc_ble_mesh_dfd_client_rsp_deep_free);
}

void bt_mesh_dfd_client_cb_evt_to_btc(btc_ble_mesh_dfd_client_cb_evt_t event,
                                      struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx,
                                      const uint8_t *val, size_t len)
{
    esp_ble_mesh_dfd_client_cb_param_t cb_params = {0};
    esp_ble_mesh_client_common_param_t params = {0};
    uint8_t act = 0U;

    if (!model || !ctx || len > sizeof(cb_params.status_cb)) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (event) {
        case BTC_BLE_MESH_EVT_DFD_CLIENT_RECV_RSP:
            act = ESP_BLE_MESH_EVT_DFD_CLIENT_RECV_RSP;
        break;
        case BTC_BLE_MESH_EVT_DFD_CLIENT_TIMEOUT:
            act = ESP_BLE_MESH_EVT_DFD_CLIENT_TIMEOUT;
        break;
        default:
         BT_ERR("Unknown event %d", event);
        break;
    }

    params.opcode = ctx->recv_op;
    params.model = (esp_ble_mesh_model_t *)model;
    btc_ble_mesh_msg_ctx_copy((struct bt_mesh_msg_ctx *)&params.ctx, ctx, false);

    cb_params.params = &params;
    cb_params.err_code = 0;

    if (val && len) {
        memcpy(&cb_params.status_cb, val, len);
    }

    btc_ble_mesh_dfd_client_callback(&cb_params, act);
    return;
}

void btc_ble_mesh_dfd_client_call_handler(btc_msg_t *msg)
{
    esp_ble_mesh_dfd_client_cb_param_t cb = {0};
    btc_ble_mesh_dfd_client_args_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s,InvParam", __func__);
        return;
    }

    arg = (btc_ble_mesh_dfd_client_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_DFD_CLIENT_GET:
        cb.params = arg->dfd_client_get.params;
        cb.err_code = btc_ble_mesh_dfd_client_get(arg->dfd_client_get.params, arg->dfd_client_get.get);
        btc_ble_mesh_dfd_client_callback(&cb, ESP_BLE_MESH_ACT_DFD_CLIEND_SEND_COMP);
        break;
    case BTC_BLE_MESH_ACT_DFD_CLIENT_SET:
        cb.params = arg->dfd_client_set.params;
        cb.err_code = btc_ble_mesh_dfd_client_set(arg->dfd_client_set.params, arg->dfd_client_set.set);
        btc_ble_mesh_dfd_client_callback(&cb, ESP_BLE_MESH_ACT_DFD_CLIEND_SEND_COMP);
        break;
    default:
        break;
    }

    btc_ble_mesh_dfd_client_arg_deep_free(msg);
}
#endif /* CONFIG_BLE_MESH_DFD_CLI */

#if CONFIG_BLE_MESH_DFD_SRV
#if CONFIG_BLE_MESH_DFD_SRV_OOB_UPLOAD
int btc_ble_mesh_dfd_srv_oob_check_complete(struct esp_ble_mesh_dfd_srv *srv,
                                            const struct esp_ble_mesh_dfu_slot *slot, int status,
                                            uint8_t *fwid, size_t fwid_len)
{
    bt_mesh_dfd_srv_oob_check_complete((struct bt_mesh_dfd_srv *)srv, (struct bt_mesh_dfu_slot *)slot, status, fwid, fwid_len);
    return 0;
}
int btc_ble_mesh_dfd_srv_oob_store_complete(struct esp_ble_mesh_dfd_srv *srv,
                                        const struct esp_ble_mesh_dfu_slot *slot, bool success,
                                        size_t size, const uint8_t *metadata, size_t metadata_len)
{
    bt_mesh_dfd_srv_oob_store_complete((struct bt_mesh_dfd_srv *)srv, (struct bt_mesh_dfu_slot *)slot, success, size, metadata, metadata_len);
    return 0;
}
#endif /* CONFIG_BLE_MESH_DFD_SRV_OOB_UPLOAD */
#endif /* CONFIG_BLE_MESH_DFD_SRV */

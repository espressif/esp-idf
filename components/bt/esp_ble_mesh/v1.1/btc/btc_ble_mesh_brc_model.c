/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "btc_ble_mesh_model_common.h"
#include "btc_ble_mesh_brc_model.h"
#include "esp_ble_mesh_brc_model_api.h"

#if CONFIG_BLE_MESH_BRC_CLI

extern int bt_mesh_subnet_bridge_get(void *param);
extern int bt_mesh_subnet_bridge_set(void *param, uint8_t subnet_bridge);
extern int bt_mesh_bridging_table_add(void *param, void *add);
extern int bt_mesh_bridging_table_remove(void *param, void *remove);
extern int bt_mesh_bridged_subnets_get(void *param, uint8_t bridge_filter,
                                       uint16_t net_idx, uint8_t bridge_start_idx);
extern int bt_mesh_bridging_table_get(void *param,
                                      uint16_t bridge_net_idx_1,
                                      uint16_t bridge_net_idx_2,
                                      uint16_t bridge_start_idx);
extern int bt_mesh_bridging_table_size_get(void *param);

/* Bridge Configuration Client model related functions */

static inline void btc_ble_mesh_brc_client_cb_to_app(esp_ble_mesh_brc_client_cb_event_t event,
                                                     esp_ble_mesh_brc_client_cb_param_t *param)
{
    esp_ble_mesh_brc_client_cb_t btc_ble_mesh_cb =
        btc_profile_cb_get(BTC_PID_BRC_CLIENT);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

void btc_ble_mesh_brc_client_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_ble_mesh_brc_client_args_t *dst = p_dest;
    btc_ble_mesh_brc_client_args_t *src = p_src;

    if (!msg || !dst || !src) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_BRC_CLIENT_SEND:
        dst->brc_send.params = NULL;
        dst->brc_send.msg = NULL;

        dst->brc_send.params = bt_mesh_calloc(sizeof(esp_ble_mesh_client_common_param_t));
        if (dst->brc_send.params) {
            memcpy(dst->brc_send.params, src->brc_send.params,
                   sizeof(esp_ble_mesh_client_common_param_t));
        } else {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            break;
        }
        if (src->brc_send.msg) {
            dst->brc_send.msg = bt_mesh_calloc(sizeof(esp_ble_mesh_brc_client_msg_t));
            if (dst->brc_send.msg) {
                memcpy(dst->brc_send.msg, src->brc_send.msg,
                       sizeof(esp_ble_mesh_brc_client_msg_t));
            } else {
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                /* Free the previously allocated resources */
                bt_mesh_free(dst->brc_send.params);
                dst->brc_send.params = NULL;
            }
        }
        break;
    default:
        BT_DBG("%s, Unknown act %d", __func__, msg->act);
        break;
    }
}

void btc_ble_mesh_brc_client_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_mesh_brc_client_args_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_brc_client_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_BRC_CLIENT_SEND:
        if (arg->brc_send.msg) {
            bt_mesh_free(arg->brc_send.msg);
        }
        if (arg->brc_send.params) {
            bt_mesh_free(arg->brc_send.params);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_brc_client_copy_req_data(btc_msg_t *msg, void *p_dest, void *p_src)
{
    esp_ble_mesh_brc_client_cb_param_t *p_dest_data = p_dest;
    esp_ble_mesh_brc_client_cb_param_t *p_src_data = p_src;
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
    case ESP_BLE_MESH_BRC_CLIENT_RECV_RSP_EVT:
    case ESP_BLE_MESH_BRC_CLIENT_RECV_PUB_EVT:
        if (p_src_data->params) {
            switch (p_src_data->params->opcode) {
            case ESP_BLE_MESH_MODEL_OP_BRIDGED_SUBNETS_GET:
            case ESP_BLE_MESH_MODEL_OP_BRIDGED_SUBNETS_LIST:
                if (p_src_data->recv.bridged_subnets_list.net_idx_pair) {
                    length = p_src_data->recv.bridged_subnets_list.bridged_entry_list_size * sizeof(esp_ble_mesh_bridge_net_idx_pair_entry_t);
                    p_dest_data->recv.bridged_subnets_list.net_idx_pair = bt_mesh_calloc(length);
                    if (!p_dest_data->recv.bridged_subnets_list.net_idx_pair) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        /* Free the previously allocated resources */
                        bt_mesh_free(p_dest_data->params);
                        p_dest_data->params = NULL;
                        return;
                    }
                    memcpy(p_dest_data->recv.bridged_subnets_list.net_idx_pair,
                           p_src_data->recv.bridged_subnets_list.net_idx_pair, length);
                }
                break;
            case ESP_BLE_MESH_MODEL_OP_BRIDGING_TABLE_GET:
            case ESP_BLE_MESH_MODEL_OP_BRIDGING_TABLE_LIST:
                if (p_src_data->recv.bridging_table_list.bridged_addr_list) {
                    length = p_src_data->recv.bridging_table_list.bridged_addr_list_size * sizeof(esp_ble_mesh_bridged_addr_list_entry_t);
                    p_dest_data->recv.bridging_table_list.bridged_addr_list = bt_mesh_calloc(length);
                    if (!p_dest_data->recv.bridging_table_list.bridged_addr_list) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        /* Free the previously allocated resources */
                        bt_mesh_free(p_dest_data->params);
                        p_dest_data->params = NULL;
                        return;
                    }
                    memcpy(p_dest_data->recv.bridging_table_list.bridged_addr_list,
                           p_src_data->recv.bridging_table_list.bridged_addr_list, length);
                }
                break;
            default:
                break;
            }
        }
    case ESP_BLE_MESH_BRC_CLIENT_SEND_COMP_EVT:
    case ESP_BLE_MESH_BRC_CLIENT_SEND_TIMEOUT_EVT:
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_brc_client_free_req_data(btc_msg_t *msg)
{
    esp_ble_mesh_brc_client_cb_param_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_brc_client_cb_param_t *)msg->arg;

    switch (msg->act) {
    case ESP_BLE_MESH_BRC_CLIENT_RECV_RSP_EVT:
    case ESP_BLE_MESH_BRC_CLIENT_RECV_PUB_EVT:
        if (arg->params) {
            switch (arg->params->opcode) {
            case ESP_BLE_MESH_MODEL_OP_BRIDGED_SUBNETS_GET:
            case ESP_BLE_MESH_MODEL_OP_BRIDGED_SUBNETS_LIST:
                bt_mesh_free(arg->recv.bridged_subnets_list.net_idx_pair);
                break;
            case ESP_BLE_MESH_MODEL_OP_BRIDGING_TABLE_GET:
            case ESP_BLE_MESH_MODEL_OP_BRIDGING_TABLE_LIST:
                bt_mesh_free(arg->recv.bridging_table_list.bridged_addr_list);
                break;
            default:
                break;
            }
        }
    case ESP_BLE_MESH_BRC_CLIENT_SEND_COMP_EVT:
    case ESP_BLE_MESH_BRC_CLIENT_SEND_TIMEOUT_EVT:
        if (arg->params) {
            bt_mesh_free(arg->params);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_brc_client_cb(esp_ble_mesh_brc_client_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_BRC_CLIENT)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_BRC_CLIENT;
    msg.act = act;

    btc_transfer_context(&msg, cb_params, sizeof(esp_ble_mesh_brc_client_cb_param_t),
                         btc_ble_mesh_brc_client_copy_req_data,
                         btc_ble_mesh_brc_client_free_req_data);
}

void bt_mesh_brc_client_cb_evt_to_btc(uint32_t opcode, uint8_t event,
                                      struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx,
                                      const uint8_t *val, size_t len)
{
    esp_ble_mesh_brc_client_cb_param_t cb_params = {0};
    esp_ble_mesh_client_common_param_t params = {0};
    uint8_t act = 0;

    if (!model || !ctx || len > sizeof(cb_params.recv)) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (event) {
    case BTC_BLE_MESH_EVT_BRC_CLIENT_RECV_RSP:
        act = ESP_BLE_MESH_BRC_CLIENT_RECV_RSP_EVT;
        break;
    case BTC_BLE_MESH_EVT_BRC_CLIENT_RECV_PUB:
        act = ESP_BLE_MESH_BRC_CLIENT_RECV_PUB_EVT;
        break;
    case BTC_BLE_MESH_EVT_BRC_CLIENT_SEND_TIMEOUT:
        act = ESP_BLE_MESH_BRC_CLIENT_SEND_TIMEOUT_EVT;
        break;
    default:
        BT_ERR("Unknown Bridge Config client event type %d", event);
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

    btc_ble_mesh_brc_client_cb(&cb_params, act);
}

void btc_ble_mesh_brc_client_recv_pub_cb(uint32_t opcode,
                                         struct bt_mesh_model *model,
                                         struct bt_mesh_msg_ctx *ctx,
                                         struct net_buf_simple *buf)
{
    if (!model || !ctx || !buf) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_brc_client_cb_evt_to_btc(opcode,
        ESP_BLE_MESH_BRC_CLIENT_RECV_PUB_EVT,
        model, ctx, buf->data, buf->len);
}

static int btc_ble_mesh_brc_client_send(esp_ble_mesh_client_common_param_t *params,
                                        esp_ble_mesh_brc_client_msg_t *msg)
{
    bt_mesh_client_common_param_t param = {0};

    if (params == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    switch (params->opcode) {
    case ESP_BLE_MESH_MODEL_OP_BRIDGED_SUBNETS_GET:
    case ESP_BLE_MESH_MODEL_OP_SUBNET_BRIDGE_SET:
    case ESP_BLE_MESH_MODEL_OP_BRIDGING_TABLE_GET:
    case ESP_BLE_MESH_MODEL_OP_BRIDGING_TABLE_ADD:
    case ESP_BLE_MESH_MODEL_OP_BRIDGING_TABLE_REMOVE:
        if (msg == NULL) {
            BT_ERR("Invalid Bridge Config message, opcode 0x%04x", params->opcode);
            return -EINVAL;
        }
        break;
    default:
        break;
    }

    btc_ble_mesh_set_client_common_param(params, &param, true);

    switch (param.opcode) {
    case ESP_BLE_MESH_MODEL_OP_SUBNET_BRIDGE_GET:
        return bt_mesh_subnet_bridge_get(&param);
    case ESP_BLE_MESH_MODEL_OP_BRIDGED_SUBNETS_GET:
        return bt_mesh_bridged_subnets_get(&param, msg->bridged_subnets_get.bridge_filter, msg->bridged_subnets_get.bridge_net_idx, msg->bridged_subnets_get.bridge_start_idx);
    case ESP_BLE_MESH_MODEL_OP_BRIDGING_TABLE_GET:
        return bt_mesh_bridging_table_get(&param, msg->bridging_table_get.bridge_net_idx_1, msg->bridging_table_get.bridge_net_idx_2, msg->bridging_table_get.bridge_start_idx);
    case ESP_BLE_MESH_MODEL_OP_BRIDGING_TABLE_SIZE_GET:
        return bt_mesh_bridging_table_size_get(&param);
    case ESP_BLE_MESH_MODEL_OP_SUBNET_BRIDGE_SET:
        return bt_mesh_subnet_bridge_set(&param, msg->subnet_bridge_set.subnet_bridge);
    case ESP_BLE_MESH_MODEL_OP_BRIDGING_TABLE_ADD:
        return bt_mesh_bridging_table_add(&param, &msg->bridging_table_add);
    case ESP_BLE_MESH_MODEL_OP_BRIDGING_TABLE_REMOVE:
        return bt_mesh_bridging_table_remove(&param, &msg->bridging_table_remove);
    default:
        BT_ERR("Invalid Bridge Config opcode 0x%04x", param.opcode);
        return -EINVAL;
    }
}

void btc_ble_mesh_brc_client_call_handler(btc_msg_t *msg)
{
    esp_ble_mesh_brc_client_cb_param_t cb = {0};

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    btc_ble_mesh_brc_client_args_t *arg = (btc_ble_mesh_brc_client_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_BRC_CLIENT_SEND:
        cb.params = arg->brc_send.params;
        cb.send.err_code = btc_ble_mesh_brc_client_send(arg->brc_send.params,
                                                        arg->brc_send.msg);
        btc_ble_mesh_brc_client_cb(&cb,
            ESP_BLE_MESH_BRC_CLIENT_SEND_COMP_EVT);
        break;
    default:
        break;
    }

    btc_ble_mesh_brc_client_arg_deep_free(msg);
}

void btc_ble_mesh_brc_client_cb_handler(btc_msg_t *msg)
{
    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    esp_ble_mesh_brc_client_cb_param_t *arg = (esp_ble_mesh_brc_client_cb_param_t *)(msg->arg);

    if (msg->act < ESP_BLE_MESH_BRC_CLIENT_EVT_MAX) {
        btc_ble_mesh_brc_client_cb_to_app(msg->act, arg);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }

    btc_ble_mesh_brc_client_free_req_data(msg);
}
#endif  /* CONFIG_BLE_MESH_BRC_CLI */

#if CONFIG_BLE_MESH_BRC_SRV

/* Bridge Config Server model related functions */
static inline void btc_ble_mesh_brc_server_cb_to_app(esp_ble_mesh_brc_server_cb_event_t event,
                                                     esp_ble_mesh_brc_server_cb_param_t *param)
{
    esp_ble_mesh_brc_server_cb_t btc_ble_mesh_cb =
        btc_profile_cb_get(BTC_PID_BRC_SERVER);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

static void btc_ble_mesh_brc_server_cb(esp_ble_mesh_brc_server_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_BRC_SERVER)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_BRC_SERVER;
    msg.act = act;

    btc_transfer_context(&msg, cb_params, sizeof(esp_ble_mesh_brc_server_cb_param_t), NULL, NULL);
}

void bt_mesh_brc_server_cb_evt_to_btc(uint8_t event,
                                      struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx,
                                      const uint8_t *val, size_t len)
{
    esp_ble_mesh_brc_server_cb_param_t cb_params = {0};
    uint8_t act = 0;

    if (!model || !ctx || len > sizeof(cb_params.value)) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (event) {
    case BTC_BLE_MESH_EVT_BRC_SERVER_STATE_CHANGE:
        act = ESP_BLE_MESH_BRC_SERVER_STATE_CHANGE_EVT;
        break;
    default:
        BT_ERR("Unknown Bridge Config server event type %d", event);
        return;
    }

    cb_params.model = (esp_ble_mesh_model_t *)model;
    cb_params.ctx.net_idx = ctx->net_idx;
    cb_params.ctx.app_idx = ctx->app_idx;
    cb_params.ctx.addr = ctx->addr;
    cb_params.ctx.recv_ttl = ctx->recv_ttl;
    cb_params.ctx.recv_op = ctx->recv_op;
    cb_params.ctx.recv_dst = ctx->recv_dst;
    cb_params.ctx.recv_rssi = ctx->recv_rssi;
    cb_params.ctx.send_ttl = ctx->send_ttl;

    if (val && len) {
        memcpy(&cb_params.value, val, len);
    }

    btc_ble_mesh_brc_server_cb(&cb_params, act);
}

void btc_ble_mesh_brc_server_cb_handler(btc_msg_t *msg)
{
    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    esp_ble_mesh_brc_server_cb_param_t *arg = (esp_ble_mesh_brc_server_cb_param_t *)(msg->arg);

    if (msg->act < ESP_BLE_MESH_BRC_SERVER_EVT_MAX) {
        btc_ble_mesh_brc_server_cb_to_app(msg->act, arg);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }
}
#endif  /* CONFIG_BLE_MESH_BRC_SRV */

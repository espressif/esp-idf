/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "btc_ble_mesh_model_common.h"
#include "btc_ble_mesh_agg_model.h"
#include "esp_ble_mesh_agg_model_api.h"

#if CONFIG_BLE_MESH_AGG_CLI

extern int bt_mesh_agg_sequence(void *param, void *sequence);

/* Opcodes Aggregator Client model related functions */

static inline void btc_ble_mesh_agg_client_cb_to_app(esp_ble_mesh_agg_client_cb_event_t event,
                                                     esp_ble_mesh_agg_client_cb_param_t *param)
{
    esp_ble_mesh_agg_client_cb_t btc_ble_mesh_cb =
        btc_profile_cb_get(BTC_PID_AGG_CLIENT);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

void btc_ble_mesh_agg_client_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_ble_mesh_agg_client_args_t *dst = p_dest;
    btc_ble_mesh_agg_client_args_t *src = p_src;
    uint16_t length = 0;

    if (!msg || !dst || !src) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_AGG_CLIENT_SEND:
        dst->agg_send.params = NULL;
        dst->agg_send.msg = NULL;

        dst->agg_send.params = bt_mesh_calloc(sizeof(esp_ble_mesh_client_common_param_t));
        if (!dst->agg_send.params) {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            return;
        }

        dst->agg_send.msg = bt_mesh_calloc(sizeof(esp_ble_mesh_agg_client_msg_t));
        if (!dst->agg_send.msg) {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            /* Free the previously allocated resources */
            bt_mesh_free(dst->agg_send.params);
            dst->agg_send.params = NULL;
            return;
        }

        memcpy(dst->agg_send.params, src->agg_send.params, sizeof(esp_ble_mesh_client_common_param_t));
        memcpy(dst->agg_send.msg, src->agg_send.msg, sizeof(esp_ble_mesh_agg_client_msg_t));

        if (src->agg_send.params->opcode == ESP_BLE_MESH_MODEL_OP_AGG_SEQUENCE) {
            if (src->agg_send.msg->agg_sequence.items) {
                length = src->agg_send.msg->agg_sequence.items->len;
                dst->agg_send.msg->agg_sequence.items = bt_mesh_alloc_buf(length);
                if (!dst->agg_send.msg->agg_sequence.items) {
                    BT_ERR("%s, Out of memory", __func__);
                    /* Free the previously allocated resources */
                    bt_mesh_free(dst->agg_send.params);
                    dst->agg_send.params = NULL;
                    bt_mesh_free(dst->agg_send.msg);
                    dst->agg_send.msg = NULL;
                    break;
                }

                net_buf_simple_add_mem(dst->agg_send.msg->agg_sequence.items,
                                        src->agg_send.msg->agg_sequence.items->data,
                                        src->agg_send.msg->agg_sequence.items->len);
            }
        }
        break;
    default:
        BT_DBG("%s, Unknown act %d", __func__, msg->act);
        break;
    }
}

void btc_ble_mesh_agg_client_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_mesh_agg_client_args_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_agg_client_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_AGG_CLIENT_SEND:
        if (arg->agg_send.msg) {
            if (arg->agg_send.params) {
                if (arg->agg_send.params->opcode == ESP_BLE_MESH_MODEL_OP_AGG_SEQUENCE) {
                    bt_mesh_free_buf(arg->agg_send.msg->agg_sequence.items);
                }
            }
            bt_mesh_free(arg->agg_send.msg);
        }
        if (arg->agg_send.params) {
            bt_mesh_free(arg->agg_send.params);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_agg_client_copy_req_data(btc_msg_t *msg, void *p_dest, void *p_src)
{
    esp_ble_mesh_agg_client_cb_param_t *p_dest_data = p_dest;
    esp_ble_mesh_agg_client_cb_param_t *p_src_data = p_src;
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
    case ESP_BLE_MESH_AGG_CLIENT_RECV_RSP_EVT:
    case ESP_BLE_MESH_AGG_CLIENT_RECV_PUB_EVT:
        if (p_src_data->params) {
            switch (p_src_data->params->opcode) {
            case ESP_BLE_MESH_MODEL_OP_AGG_SEQUENCE:
            case ESP_BLE_MESH_MODEL_OP_AGG_STATUS:
                if (p_src_data->recv.agg_status.items) {
                    length = p_src_data->recv.agg_status.items->len;
                    p_dest_data->recv.agg_status.items = bt_mesh_alloc_buf(length);
                    if (!p_dest_data->recv.agg_status.items) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        /* Free the previously allocated resources */
                        bt_mesh_free(p_dest_data->params);
                        p_dest_data->params = NULL;
                        return;
                    }

                    net_buf_simple_add_mem(p_dest_data->recv.agg_status.items,
                                           p_src_data->recv.agg_status.items->data,
                                           p_src_data->recv.agg_status.items->len);
                }
                break;
            default:
                break;
            }
        }
    case ESP_BLE_MESH_AGG_CLIENT_SEND_COMP_EVT:
    case ESP_BLE_MESH_AGG_CLIENT_SEND_TIMEOUT_EVT:
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_agg_client_free_req_data(btc_msg_t *msg)
{
    esp_ble_mesh_agg_client_cb_param_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_agg_client_cb_param_t *)msg->arg;

    switch (msg->act) {
    case ESP_BLE_MESH_AGG_CLIENT_RECV_RSP_EVT:
    case ESP_BLE_MESH_AGG_CLIENT_RECV_PUB_EVT:
        if (arg->params) {
            switch (arg->params->opcode) {
            case ESP_BLE_MESH_MODEL_OP_AGG_SEQUENCE:
            case ESP_BLE_MESH_MODEL_OP_AGG_STATUS:
                bt_mesh_free_buf(arg->recv.agg_status.items);
                break;
            default:
                break;
            }
        }
    case ESP_BLE_MESH_AGG_CLIENT_SEND_COMP_EVT:
    case ESP_BLE_MESH_AGG_CLIENT_SEND_TIMEOUT_EVT:
        if (arg->params) {
            bt_mesh_free(arg->params);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_agg_client_cb(esp_ble_mesh_agg_client_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_AGG_CLIENT)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_AGG_CLIENT;
    msg.act = act;

    btc_transfer_context(&msg, cb_params, sizeof(esp_ble_mesh_agg_client_cb_param_t),
                         btc_ble_mesh_agg_client_copy_req_data,
                         btc_ble_mesh_agg_client_free_req_data);
}

void bt_mesh_agg_client_cb_evt_to_btc(uint32_t opcode, uint8_t event,
                                      struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx,
                                      const uint8_t *val, size_t len)
{
    esp_ble_mesh_agg_client_cb_param_t cb_params = {0};
    esp_ble_mesh_client_common_param_t params = {0};
    uint8_t act = 0;

    if (!model || !ctx || len > sizeof(cb_params.recv)) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (event) {
    case BTC_BLE_MESH_EVT_AGG_CLIENT_SEND_TIMEOUT:
        act = ESP_BLE_MESH_AGG_CLIENT_SEND_TIMEOUT_EVT;
        break;
    case BTC_BLE_MESH_EVT_AGG_CLIENT_RECV_RSP:
        act = ESP_BLE_MESH_AGG_CLIENT_RECV_RSP_EVT;
        break;
    case BTC_BLE_MESH_EVT_AGG_CLIENT_RECV_PUB:
        act = ESP_BLE_MESH_AGG_CLIENT_RECV_PUB_EVT;
        break;
    default:
        BT_ERR("Unknown Opcodes Aggregator client event type %d", event);
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

    btc_ble_mesh_agg_client_cb(&cb_params, act);
}

void btc_ble_mesh_agg_client_recv_pub_cb(uint32_t opcode,
                                         struct bt_mesh_model *model,
                                         struct bt_mesh_msg_ctx *ctx,
                                         struct net_buf_simple *buf)
{
    if (!model || !ctx || !buf) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_agg_client_cb_evt_to_btc(opcode,
        BTC_BLE_MESH_EVT_AGG_CLIENT_RECV_PUB,
        model, ctx, buf->data, buf->len);
}

static int btc_ble_mesh_agg_client_send(esp_ble_mesh_client_common_param_t *params,
                                        esp_ble_mesh_agg_client_msg_t *set)
{
    bt_mesh_client_common_param_t param = {0};

    if (params == NULL || set == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    btc_ble_mesh_set_client_common_param(params, &param, false);

    switch (param.opcode) {
    case ESP_BLE_MESH_MODEL_OP_AGG_SEQUENCE:
        return bt_mesh_agg_sequence(&param, &set->agg_sequence);
    default:
        BT_ERR("Invalid Opcodes Aggregator opcode 0x%04x", param.opcode);
        return -EINVAL;
    }
}

void btc_ble_mesh_agg_client_call_handler(btc_msg_t *msg)
{
    esp_ble_mesh_agg_client_cb_param_t cb = {0};
    btc_ble_mesh_agg_client_args_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_agg_client_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_AGG_CLIENT_SEND:
        cb.params = arg->agg_send.params;
        cb.send.err_code = btc_ble_mesh_agg_client_send(arg->agg_send.params,
                                                        arg->agg_send.msg);
        btc_ble_mesh_agg_client_cb(&cb,
            ESP_BLE_MESH_AGG_CLIENT_SEND_COMP_EVT);
        break;
    default:
        break;
    }

    btc_ble_mesh_agg_client_arg_deep_free(msg);
}

void btc_ble_mesh_agg_client_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_agg_client_cb_param_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_agg_client_cb_param_t *)msg->arg;

    if (msg->act < ESP_BLE_MESH_AGG_CLIENT_EVT_MAX) {
        btc_ble_mesh_agg_client_cb_to_app(msg->act, arg);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }

    btc_ble_mesh_agg_client_free_req_data(msg);
}

#endif /* CONFIG_BLE_MESH_AGG_CLI */

#if CONFIG_BLE_MESH_AGG_SRV

/* Opcodes Aggregator Server model related functions */

static inline void btc_ble_mesh_agg_server_cb_to_app(esp_ble_mesh_agg_server_cb_event_t event,
                                                     esp_ble_mesh_agg_server_cb_param_t *param)
{
    esp_ble_mesh_agg_server_cb_t btc_ble_mesh_cb =
        btc_profile_cb_get(BTC_PID_AGG_SERVER);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

static void btc_ble_mesh_agg_server_copy_req_data(btc_msg_t *msg, void *p_dest, void *p_src)
{
    esp_ble_mesh_agg_server_cb_param_t *p_dest_data = p_dest;
    esp_ble_mesh_agg_server_cb_param_t *p_src_data = p_src;
    uint16_t length = 0U;

    if (!msg || !p_src_data || !p_dest_data) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case ESP_BLE_MESH_AGG_SERVER_RECV_MSG_EVT:
        if (p_src_data->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_AGG_SEQUENCE) {
            if (p_src_data->recv.agg_sequence.items) {
                length = p_src_data->recv.agg_sequence.items->len;
                p_dest_data->recv.agg_sequence.items = bt_mesh_alloc_buf(length);
                if (!p_dest_data->recv.agg_sequence.items) {
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                    break;
                }

                net_buf_simple_add_mem(p_dest_data->recv.agg_sequence.items,
                                       p_src_data->recv.agg_sequence.items->data,
                                       p_src_data->recv.agg_sequence.items->len);
            }
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_agg_server_free_req_data(btc_msg_t *msg)
{
    esp_ble_mesh_agg_server_cb_param_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_agg_server_cb_param_t *)msg->arg;

    switch (msg->act) {
    case ESP_BLE_MESH_AGG_SERVER_RECV_MSG_EVT:
        if (arg->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_AGG_SEQUENCE) {
            bt_mesh_free_buf(arg->recv.agg_sequence.items);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_agg_server_cb(
                esp_ble_mesh_agg_server_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_AGG_SERVER)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_AGG_SERVER;
    msg.act = act;

    btc_transfer_context(&msg, cb_params, sizeof(esp_ble_mesh_agg_server_cb_param_t),
                         btc_ble_mesh_agg_server_copy_req_data,
                         btc_ble_mesh_agg_server_free_req_data);
}

void bt_mesh_agg_server_cb_evt_to_btc(uint8_t event,
                                      struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx,
                                      const void *val, size_t len)
{
    esp_ble_mesh_agg_server_cb_param_t cb_params = {0};
    uint8_t act = 0;

    if (!model || !ctx || len > sizeof(cb_params.recv)) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (event) {
    case BTC_BLE_MESH_EVT_AGG_SERVER_RECV_MSG:
        act = ESP_BLE_MESH_AGG_SERVER_RECV_MSG_EVT;
        if (val && len) {
            memcpy(&cb_params.recv, val, len);
        }
        break;
    default:
        BT_ERR("Unknown Opcodes Aggregator server event type %d", event);
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

    btc_ble_mesh_agg_server_cb(&cb_params, act);
}

void btc_ble_mesh_agg_server_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_agg_server_cb_param_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_agg_server_cb_param_t *)msg->arg;

    if (msg->act < ESP_BLE_MESH_AGG_SERVER_EVT_MAX) {
        btc_ble_mesh_agg_server_cb_to_app(msg->act, arg);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }

    btc_ble_mesh_agg_server_free_req_data(msg);
}

#endif /* CONFIG_BLE_MESH_AGG_SRV */

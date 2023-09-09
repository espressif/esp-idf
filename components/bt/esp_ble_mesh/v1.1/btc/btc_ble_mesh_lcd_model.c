/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "btc_ble_mesh_model_common.h"
#include "btc_ble_mesh_lcd_model.h"
#include "esp_ble_mesh_lcd_model_api.h"

#if CONFIG_BLE_MESH_LCD_CLI

extern int bt_mesh_large_comp_data_get(void *param, uint8_t page, uint16_t offset);
extern int bt_mesh_models_metadata_get(void *param, uint8_t metadata_page, uint16_t offset);

/* Large Composition Data Client model related functions */

static inline void btc_ble_mesh_lcd_client_cb_to_app(esp_ble_mesh_lcd_client_cb_event_t event,
                                                     esp_ble_mesh_lcd_client_cb_param_t *param)
{
    esp_ble_mesh_lcd_client_cb_t btc_ble_mesh_cb =
        btc_profile_cb_get(BTC_PID_LCD_CLIENT);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

void btc_ble_mesh_lcd_client_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_ble_mesh_lcd_client_args_t *dst = p_dest;
    btc_ble_mesh_lcd_client_args_t *src = p_src;

    if (!msg || !dst || !src) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_LCD_CLIENT_SEND:
        dst->lcd_send.params = bt_mesh_calloc(sizeof(esp_ble_mesh_client_common_param_t));
        dst->lcd_send.msg = bt_mesh_calloc(sizeof(esp_ble_mesh_lcd_client_msg_t));
        if (dst->lcd_send.params && dst->lcd_send.msg) {
            memcpy(dst->lcd_send.params, src->lcd_send.params,
                   sizeof(esp_ble_mesh_client_common_param_t));
            memcpy(dst->lcd_send.msg, src->lcd_send.msg,
                   sizeof(esp_ble_mesh_lcd_client_msg_t));
        } else {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
        }
        break;
    default:
        BT_DBG("%s, Unknown act %d", __func__, msg->act);
        break;
    }
}

void btc_ble_mesh_lcd_client_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_mesh_lcd_client_args_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_lcd_client_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_LCD_CLIENT_SEND:
        if (arg->lcd_send.msg) {
            bt_mesh_free(arg->lcd_send.msg);
        }
        if (arg->lcd_send.params) {
            bt_mesh_free(arg->lcd_send.params);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_lcd_client_copy_req_data(btc_msg_t *msg, void *p_dest, void *p_src)
{
    esp_ble_mesh_lcd_client_cb_param_t *p_dest_data = p_dest;
    esp_ble_mesh_lcd_client_cb_param_t *p_src_data = p_src;
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
    case ESP_BLE_MESH_LCD_CLIENT_RECV_RSP_EVT:
    case ESP_BLE_MESH_LCD_CLIENT_RECV_PUB_EVT:
        if (p_src_data->params) {
            switch (p_src_data->params->opcode) {
            case ESP_BLE_MESH_MODEL_OP_LARGE_COMP_DATA_GET:
            case ESP_BLE_MESH_MODEL_OP_LARGE_COMP_DATA_STATUS:
                if (p_src_data->recv.large_comp_data_status.data) {
                    length = p_src_data->recv.large_comp_data_status.data->len;
                    p_dest_data->recv.large_comp_data_status.data = bt_mesh_alloc_buf(length);
                    if (!p_dest_data->recv.large_comp_data_status.data) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        return;
                    }

                    net_buf_simple_add_mem(p_dest_data->recv.large_comp_data_status.data,
                                           p_src_data->recv.large_comp_data_status.data->data,
                                           p_src_data->recv.large_comp_data_status.data->len);
                }
                break;
            case ESP_BLE_MESH_MODEL_OP_MODELS_METADATA_GET:
            case ESP_BLE_MESH_MODEL_OP_MODELS_METADATA_STATUS:
                if (p_src_data->recv.models_metadata_status.data) {
                    length = p_src_data->recv.models_metadata_status.data->len;
                    p_dest_data->recv.models_metadata_status.data = bt_mesh_alloc_buf(length);
                    if (!p_dest_data->recv.models_metadata_status.data) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        return;
                    }

                    net_buf_simple_add_mem(p_dest_data->recv.models_metadata_status.data,
                                           p_src_data->recv.models_metadata_status.data->data,
                                           p_src_data->recv.models_metadata_status.data->len);
                }
                break;
            default:
                break;
            }
        }
    case ESP_BLE_MESH_LCD_CLIENT_SEND_COMP_EVT:
    case ESP_BLE_MESH_LCD_CLIENT_SEND_TIMEOUT_EVT:
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_lcd_client_free_req_data(btc_msg_t *msg)
{
    esp_ble_mesh_lcd_client_cb_param_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_lcd_client_cb_param_t *)msg->arg;

    switch (msg->act) {
    case ESP_BLE_MESH_LCD_CLIENT_RECV_RSP_EVT:
    case ESP_BLE_MESH_LCD_CLIENT_RECV_PUB_EVT:
        if (arg->params) {
            switch (arg->params->opcode) {
            case ESP_BLE_MESH_MODEL_OP_LARGE_COMP_DATA_GET:
            case ESP_BLE_MESH_MODEL_OP_LARGE_COMP_DATA_STATUS:
                bt_mesh_free_buf(arg->recv.large_comp_data_status.data);
                break;
            case ESP_BLE_MESH_MODEL_OP_MODELS_METADATA_GET:
            case ESP_BLE_MESH_MODEL_OP_MODELS_METADATA_STATUS:
                bt_mesh_free_buf(arg->recv.models_metadata_status.data);
                break;
            default:
                break;
            }
        }
    case ESP_BLE_MESH_LCD_CLIENT_SEND_COMP_EVT:
    case ESP_BLE_MESH_LCD_CLIENT_SEND_TIMEOUT_EVT:
        if (arg->params) {
            bt_mesh_free(arg->params);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_lcd_client_cb(esp_ble_mesh_lcd_client_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_LCD_CLIENT)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_LCD_CLIENT;
    msg.act = act;

    btc_transfer_context(&msg, cb_params, sizeof(esp_ble_mesh_lcd_client_cb_param_t),
                         btc_ble_mesh_lcd_client_copy_req_data,
                         btc_ble_mesh_lcd_client_free_req_data);
}

void bt_mesh_lcd_client_cb_evt_to_btc(uint32_t opcode, uint8_t event,
                                      struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx,
                                      const void *val, size_t len)
{
    esp_ble_mesh_lcd_client_cb_param_t cb_params = {0};
    esp_ble_mesh_client_common_param_t params = {0};
    uint8_t act = 0;

    if (!model || !ctx || len > sizeof(cb_params.recv)) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (event) {
    case BTC_BLE_MESH_EVT_LCD_CLIENT_SEND_TIMEOUT:
        act = ESP_BLE_MESH_LCD_CLIENT_SEND_TIMEOUT_EVT;
        break;
    case BTC_BLE_MESH_EVT_LCD_CLIENT_RECV_RSP:
        act = ESP_BLE_MESH_LCD_CLIENT_RECV_RSP_EVT;
        break;
    case BTC_BLE_MESH_EVT_LCD_CLIENT_RECV_PUB:
        act = ESP_BLE_MESH_LCD_CLIENT_RECV_PUB_EVT;
        break;
    default:
        BT_ERR("Unknown Large Comp Data client event type %d", event);
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

    btc_ble_mesh_lcd_client_cb(&cb_params, act);
}

void btc_ble_mesh_lcd_client_recv_pub_cb(uint32_t opcode,
                                         struct bt_mesh_model *model,
                                         struct bt_mesh_msg_ctx *ctx,
                                         struct net_buf_simple *buf)
{
    if (!model || !ctx || !buf) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_lcd_client_cb_evt_to_btc(opcode,
        BTC_BLE_MESH_EVT_LCD_CLIENT_RECV_PUB,
        model, ctx, buf->data, buf->len);
}

static int btc_ble_mesh_lcd_client_send(esp_ble_mesh_client_common_param_t *params,
                                        esp_ble_mesh_lcd_client_msg_t *msg)
{
    bt_mesh_client_common_param_t param = {0};

    if (params == NULL || msg == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    btc_ble_mesh_set_client_common_param(params, &param, true);

    switch (param.opcode) {
    case ESP_BLE_MESH_MODEL_OP_LARGE_COMP_DATA_GET:
        return bt_mesh_large_comp_data_get(&param, msg->large_comp_data_get.page,
                                           msg->large_comp_data_get.offset);
    case ESP_BLE_MESH_MODEL_OP_MODELS_METADATA_GET:
        return bt_mesh_models_metadata_get(&param, msg->models_metadata_get.metadata_page,
                                           msg->models_metadata_get.offset);
    default:
        BT_ERR("Invalid Large Comp Data Set opcode 0x%04x", param.opcode);
        return -EINVAL;
    }
}

void btc_ble_mesh_lcd_client_call_handler(btc_msg_t *msg)
{
    esp_ble_mesh_lcd_client_cb_param_t cb = {0};
    btc_ble_mesh_lcd_client_args_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_lcd_client_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_LCD_CLIENT_SEND:
        cb.params = arg->lcd_send.params;
        cb.send.err_code = btc_ble_mesh_lcd_client_send(arg->lcd_send.params,
                                                        arg->lcd_send.msg);
        btc_ble_mesh_lcd_client_cb(&cb, ESP_BLE_MESH_LCD_CLIENT_SEND_COMP_EVT);
        break;
    default:
        break;
    }

    btc_ble_mesh_lcd_client_arg_deep_free(msg);
}

void btc_ble_mesh_lcd_client_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_lcd_client_cb_param_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_lcd_client_cb_param_t *)msg->arg;

    if (msg->act < ESP_BLE_MESH_LCD_CLIENT_EVT_MAX) {
        btc_ble_mesh_lcd_client_cb_to_app(msg->act, arg);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }

    btc_ble_mesh_lcd_client_free_req_data(msg);
}

#endif /* CONFIG_BLE_MESH_LCD_CLI */

#if CONFIG_BLE_MESH_LCD_SRV

/* Large Comp Data Server model related functions */

static inline void btc_ble_mesh_lcd_server_cb_to_app(esp_ble_mesh_lcd_server_cb_event_t event,
                                                     esp_ble_mesh_lcd_server_cb_param_t *param)
{
    esp_ble_mesh_lcd_server_cb_t btc_ble_mesh_cb =
        btc_profile_cb_get(BTC_PID_LCD_SERVER);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

static void btc_ble_mesh_lcd_server_cb(
                esp_ble_mesh_lcd_server_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_LCD_SERVER)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_LCD_SERVER;
    msg.act = act;

    btc_transfer_context(&msg, cb_params, sizeof(esp_ble_mesh_lcd_server_cb_param_t), NULL, NULL);
}

void bt_mesh_lcd_server_cb_evt_to_btc(uint8_t event,
                                      struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx,
                                      const void *val, size_t len)
{
    esp_ble_mesh_lcd_server_cb_param_t cb_params = {0};
    uint8_t act = 0;

    if (!model || !ctx || len > sizeof(cb_params.value)) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (event) {
    case BTC_BLE_MESH_EVT_LCD_SERVER_STATE_CHANGE:
        act = ESP_BLE_MESH_LCD_SERVER_STATE_CHANGE_EVT;
        break;
    default:
        BT_ERR("Unknown Large Comp Data server event type %d", event);
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

    btc_ble_mesh_lcd_server_cb(&cb_params, act);
}

void btc_ble_mesh_lcd_server_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_lcd_server_cb_param_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_lcd_server_cb_param_t *)msg->arg;

    if (msg->act < ESP_BLE_MESH_LCD_SERVER_EVT_MAX) {
        btc_ble_mesh_lcd_server_cb_to_app(msg->act, arg);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }
}

#endif /* CONFIG_BLE_MESH_LCD_SRV */

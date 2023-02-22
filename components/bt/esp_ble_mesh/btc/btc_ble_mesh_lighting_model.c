/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "btc_ble_mesh_lighting_model.h"
#include "esp_ble_mesh_lighting_model_api.h"

#if CONFIG_BLE_MESH_LIGHTING_CLIENT
#include "lighting_client.h"

/* Lighting Client Models related functions */

static inline void btc_ble_mesh_lighting_client_cb_to_app(esp_ble_mesh_light_client_cb_event_t event,
                                                          esp_ble_mesh_light_client_cb_param_t *param)
{
    esp_ble_mesh_light_client_cb_t btc_ble_mesh_cb =
        (esp_ble_mesh_light_client_cb_t)btc_profile_cb_get(BTC_PID_LIGHTING_CLIENT);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

void btc_ble_mesh_lighting_client_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_ble_mesh_lighting_client_args_t *dst = (btc_ble_mesh_lighting_client_args_t *)p_dest;
    btc_ble_mesh_lighting_client_args_t *src = (btc_ble_mesh_lighting_client_args_t *)p_src;

    if (!msg || !dst || !src) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_LIGHTING_CLIENT_GET_STATE: {
        dst->light_client_get_state.params = (esp_ble_mesh_client_common_param_t *)bt_mesh_malloc(sizeof(esp_ble_mesh_client_common_param_t));
        if (dst->light_client_get_state.params) {
            memcpy(dst->light_client_get_state.params, src->light_client_get_state.params,
                   sizeof(esp_ble_mesh_client_common_param_t));
        } else {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            break;
        }
        if (src->light_client_get_state.get_state) {
            dst->light_client_get_state.get_state = (esp_ble_mesh_light_client_get_state_t *)bt_mesh_malloc(sizeof(esp_ble_mesh_light_client_get_state_t));
            if (dst->light_client_get_state.get_state) {
                memcpy(dst->light_client_get_state.get_state, src->light_client_get_state.get_state,
                    sizeof(esp_ble_mesh_light_client_get_state_t));
            } else {
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            }
        }
        break;
    }
    case BTC_BLE_MESH_ACT_LIGHTING_CLIENT_SET_STATE: {
        dst->light_client_set_state.params = (esp_ble_mesh_client_common_param_t *)bt_mesh_malloc(sizeof(esp_ble_mesh_client_common_param_t));
        dst->light_client_set_state.set_state = (esp_ble_mesh_light_client_set_state_t *)bt_mesh_malloc(sizeof(esp_ble_mesh_light_client_set_state_t));
        if (dst->light_client_set_state.params && dst->light_client_set_state.set_state) {
            memcpy(dst->light_client_set_state.params, src->light_client_set_state.params,
                   sizeof(esp_ble_mesh_client_common_param_t));
            memcpy(dst->light_client_set_state.set_state, src->light_client_set_state.set_state,
                   sizeof(esp_ble_mesh_light_client_set_state_t));
        } else {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
        }
        break;
    }
    default:
        BT_DBG("%s, Unknown act %d", __func__, msg->act);
        break;
    }
}

void btc_ble_mesh_lighting_client_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_mesh_lighting_client_args_t *arg = NULL;

    if (!msg || !msg->arg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_lighting_client_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_LIGHTING_CLIENT_GET_STATE:
        if (arg->light_client_get_state.params) {
            bt_mesh_free(arg->light_client_get_state.params);
        }
        if (arg->light_client_get_state.get_state) {
            bt_mesh_free(arg->light_client_get_state.get_state);
        }
        break;
    case BTC_BLE_MESH_ACT_LIGHTING_CLIENT_SET_STATE:
        if (arg->light_client_set_state.params) {
            bt_mesh_free(arg->light_client_set_state.params);
        }
        if (arg->light_client_set_state.set_state) {
            bt_mesh_free(arg->light_client_set_state.set_state);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_lighting_client_copy_req_data(btc_msg_t *msg, void *p_dest, void *p_src)
{
    esp_ble_mesh_light_client_cb_param_t *p_dest_data = (esp_ble_mesh_light_client_cb_param_t *)p_dest;
    esp_ble_mesh_light_client_cb_param_t *p_src_data = (esp_ble_mesh_light_client_cb_param_t *)p_src;
    uint16_t length = 0U;

    if (!msg || !p_src_data || !p_dest_data) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    if (p_src_data->params) {
        p_dest_data->params = bt_mesh_malloc(sizeof(esp_ble_mesh_client_common_param_t));
        if (!p_dest_data->params) {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            return;
        }

        memcpy(p_dest_data->params, p_src_data->params, sizeof(esp_ble_mesh_client_common_param_t));
    }

    switch (msg->act) {
    case ESP_BLE_MESH_LIGHT_CLIENT_GET_STATE_EVT:
    case ESP_BLE_MESH_LIGHT_CLIENT_SET_STATE_EVT:
    case ESP_BLE_MESH_LIGHT_CLIENT_PUBLISH_EVT:
        if (p_src_data->params) {
            switch (p_src_data->params->opcode) {
            case ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_GET:
            case ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET:
            case ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_STATUS:
                if (p_src_data->status_cb.lc_property_status.property_value) {
                    length = p_src_data->status_cb.lc_property_status.property_value->len;
                    p_dest_data->status_cb.lc_property_status.property_value = bt_mesh_alloc_buf(length);
                    if (!p_dest_data->status_cb.lc_property_status.property_value) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        return;
                    }
                    net_buf_simple_add_mem(p_dest_data->status_cb.lc_property_status.property_value,
                                           p_src_data->status_cb.lc_property_status.property_value->data,
                                           p_src_data->status_cb.lc_property_status.property_value->len);
                }
                break;
            default:
                break;
            }
        }
    case ESP_BLE_MESH_LIGHT_CLIENT_TIMEOUT_EVT:
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_lighting_client_free_req_data(btc_msg_t *msg)
{
    esp_ble_mesh_light_client_cb_param_t *arg = NULL;

    if (!msg || !msg->arg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_light_client_cb_param_t *)(msg->arg);

    switch (msg->act) {
    case ESP_BLE_MESH_LIGHT_CLIENT_GET_STATE_EVT:
    case ESP_BLE_MESH_LIGHT_CLIENT_SET_STATE_EVT:
    case ESP_BLE_MESH_LIGHT_CLIENT_PUBLISH_EVT:
        if (arg->params) {
            switch (arg->params->opcode) {
            case ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_GET:
            case ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET:
            case ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_STATUS:
                bt_mesh_free_buf(arg->status_cb.lc_property_status.property_value);
                break;
            default:
                break;
            }
        }
    case ESP_BLE_MESH_LIGHT_CLIENT_TIMEOUT_EVT:
        if (arg->params) {
            bt_mesh_free(arg->params);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_lighting_client_callback(esp_ble_mesh_light_client_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    BT_DBG("%s", __func__);

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_LIGHTING_CLIENT)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_LIGHTING_CLIENT;
    msg.act = act;

    btc_transfer_context(&msg, cb_params, cb_params == NULL ? 0 : sizeof(esp_ble_mesh_light_client_cb_param_t),
                         btc_ble_mesh_lighting_client_copy_req_data, btc_ble_mesh_lighting_client_free_req_data);
}

void bt_mesh_lighting_client_cb_evt_to_btc(uint32_t opcode, uint8_t evt_type,
                                           struct bt_mesh_model *model,
                                           struct bt_mesh_msg_ctx *ctx,
                                           const uint8_t *val, size_t len)
{
    esp_ble_mesh_light_client_cb_param_t cb_params = {0};
    esp_ble_mesh_client_common_param_t params = {0};
    uint8_t act = 0U;

    if (!model || !ctx) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (evt_type) {
    case BTC_BLE_MESH_EVT_LIGHTING_CLIENT_GET_STATE:
        act = ESP_BLE_MESH_LIGHT_CLIENT_GET_STATE_EVT;
        break;
    case BTC_BLE_MESH_EVT_LIGHTING_CLIENT_SET_STATE:
        act = ESP_BLE_MESH_LIGHT_CLIENT_SET_STATE_EVT;
        break;
    case BTC_BLE_MESH_EVT_LIGHTING_CLIENT_PUBLISH:
        act = ESP_BLE_MESH_LIGHT_CLIENT_PUBLISH_EVT;
        break;
    case BTC_BLE_MESH_EVT_LIGHTING_CLIENT_TIMEOUT:
        act = ESP_BLE_MESH_LIGHT_CLIENT_TIMEOUT_EVT;
        break;
    default:
        BT_ERR("Unknown Lighting client event type %d", evt_type);
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

    cb_params.error_code = 0;
    cb_params.params = &params;

    if (val && len) {
        memcpy(&cb_params.status_cb, val, MIN(len, sizeof(cb_params.status_cb)));
    }

    btc_ble_mesh_lighting_client_callback(&cb_params, act);
    return;
}

void btc_ble_mesh_lighting_client_publish_callback(uint32_t opcode, struct bt_mesh_model *model,
                                                   struct bt_mesh_msg_ctx *ctx,
                                                   struct net_buf_simple *buf)
{
    if (!model || !ctx || !buf) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_lighting_client_cb_evt_to_btc(opcode, BTC_BLE_MESH_EVT_LIGHTING_CLIENT_PUBLISH,
                                          model, ctx, buf->data, buf->len);
    return;
}

void btc_ble_mesh_lighting_client_call_handler(btc_msg_t *msg)
{
    esp_ble_mesh_client_common_param_t *params = NULL;
    btc_ble_mesh_lighting_client_args_t *arg = NULL;
    esp_ble_mesh_light_client_cb_param_t cb = {0};
    bt_mesh_client_common_param_t common = {0};

    if (!msg || !msg->arg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_lighting_client_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_LIGHTING_CLIENT_GET_STATE: {
        params = arg->light_client_get_state.params;
        common.opcode = params->opcode;
        common.model = (struct bt_mesh_model *)params->model;
        common.ctx.net_idx = params->ctx.net_idx;
        common.ctx.app_idx = params->ctx.app_idx;
        common.ctx.addr = params->ctx.addr;
        common.ctx.send_rel = params->ctx.send_rel;
        common.ctx.send_ttl = params->ctx.send_ttl;
        common.msg_timeout = params->msg_timeout;
        common.msg_role = params->msg_role;

        cb.params = arg->light_client_get_state.params;
        cb.error_code = bt_mesh_light_client_get_state(&common, arg->light_client_get_state.get_state);
        if (cb.error_code) {
            /* If send failed, callback error_code to app layer immediately */
            btc_ble_mesh_lighting_client_callback(&cb, ESP_BLE_MESH_LIGHT_CLIENT_GET_STATE_EVT);
        }
        break;
    }
    case BTC_BLE_MESH_ACT_LIGHTING_CLIENT_SET_STATE: {
        params = arg->light_client_set_state.params;
        common.opcode = params->opcode;
        common.model = (struct bt_mesh_model *)params->model;
        common.ctx.net_idx = params->ctx.net_idx;
        common.ctx.app_idx = params->ctx.app_idx;
        common.ctx.addr = params->ctx.addr;
        common.ctx.send_rel = params->ctx.send_rel;
        common.ctx.send_ttl = params->ctx.send_ttl;
        common.msg_timeout = params->msg_timeout;
        common.msg_role = params->msg_role;

        cb.params = arg->light_client_set_state.params;
        cb.error_code = bt_mesh_light_client_set_state(&common, arg->light_client_set_state.set_state);
        if (cb.error_code) {
            /* If send failed, callback error_code to app layer immediately */
            btc_ble_mesh_lighting_client_callback(&cb, ESP_BLE_MESH_LIGHT_CLIENT_SET_STATE_EVT);
        }
        break;
    }
    default:
        break;
    }

    btc_ble_mesh_lighting_client_arg_deep_free(msg);
    return;
}

void btc_ble_mesh_lighting_client_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_light_client_cb_param_t *param = NULL;

    if (!msg || !msg->arg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    param = (esp_ble_mesh_light_client_cb_param_t *)(msg->arg);

    if (msg->act < ESP_BLE_MESH_LIGHT_CLIENT_EVT_MAX) {
        btc_ble_mesh_lighting_client_cb_to_app(msg->act, param);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }

    btc_ble_mesh_lighting_client_free_req_data(msg);
    return;
}

#endif /* CONFIG_BLE_MESH_LIGHTING_CLIENT */

#if CONFIG_BLE_MESH_LIGHTING_SERVER

/* Lighting Server Models related functions */

static inline void btc_ble_mesh_lighting_server_cb_to_app(esp_ble_mesh_lighting_server_cb_event_t event,
                                                          esp_ble_mesh_lighting_server_cb_param_t *param)
{
    esp_ble_mesh_lighting_server_cb_t btc_ble_mesh_cb =
        (esp_ble_mesh_lighting_server_cb_t)btc_profile_cb_get(BTC_PID_LIGHTING_SERVER);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

static void btc_ble_mesh_lighting_server_copy_req_data(btc_msg_t *msg, void *p_dest, void *p_src)
{
    esp_ble_mesh_lighting_server_cb_param_t *p_dest_data = (esp_ble_mesh_lighting_server_cb_param_t *)p_dest;
    esp_ble_mesh_lighting_server_cb_param_t *p_src_data = (esp_ble_mesh_lighting_server_cb_param_t *)p_src;
    uint16_t length = 0U;

    if (!msg || !p_src_data || !p_dest_data) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case ESP_BLE_MESH_LIGHTING_SERVER_STATE_CHANGE_EVT:
        if (p_src_data->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET ||
                p_src_data->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET_UNACK) {
            if (p_src_data->value.state_change.lc_property_set.property_value) {
                length = p_src_data->value.state_change.lc_property_set.property_value->len;
                p_dest_data->value.state_change.lc_property_set.property_value = bt_mesh_alloc_buf(length);
                if (p_dest_data->value.state_change.lc_property_set.property_value == NULL) {
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                    return;
                }
                net_buf_simple_add_mem(p_dest_data->value.state_change.lc_property_set.property_value,
                                       p_src_data->value.state_change.lc_property_set.property_value->data,
                                       p_src_data->value.state_change.lc_property_set.property_value->len);
            }
        }
        break;
    case ESP_BLE_MESH_LIGHTING_SERVER_RECV_SET_MSG_EVT:
        if (p_src_data->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET ||
                p_src_data->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET_UNACK) {
            if (p_src_data->value.set.lc_property.property_value) {
                length = p_src_data->value.set.lc_property.property_value->len;
                p_dest_data->value.set.lc_property.property_value = bt_mesh_alloc_buf(length);
                if (p_dest_data->value.set.lc_property.property_value == NULL) {
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                    return;
                }
                net_buf_simple_add_mem(p_dest_data->value.set.lc_property.property_value,
                                       p_src_data->value.set.lc_property.property_value->data,
                                       p_src_data->value.set.lc_property.property_value->len);
            }
        }
        break;
    case ESP_BLE_MESH_LIGHTING_SERVER_RECV_STATUS_MSG_EVT:
        if (p_src_data->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_SENSOR_STATUS) {
            if (p_src_data->value.status.sensor_status.data) {
                length = p_src_data->value.status.sensor_status.data->len;
                p_dest_data->value.status.sensor_status.data = bt_mesh_alloc_buf(length);
                if (p_dest_data->value.status.sensor_status.data == NULL) {
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                    return;
                }
                net_buf_simple_add_mem(p_dest_data->value.status.sensor_status.data,
                                       p_src_data->value.status.sensor_status.data->data,
                                       p_src_data->value.status.sensor_status.data->len);
            }
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_lighting_server_free_req_data(btc_msg_t *msg)
{
    esp_ble_mesh_lighting_server_cb_param_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_lighting_server_cb_param_t *)(msg->arg);

    switch (msg->act) {
    case ESP_BLE_MESH_LIGHTING_SERVER_STATE_CHANGE_EVT:
        if (arg->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET ||
                arg->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET_UNACK) {
            bt_mesh_free_buf(arg->value.state_change.lc_property_set.property_value);
        }
        break;
    case ESP_BLE_MESH_LIGHTING_SERVER_RECV_SET_MSG_EVT:
        if (arg->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET ||
                arg->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET_UNACK) {
            bt_mesh_free_buf(arg->value.set.lc_property.property_value);
        }
        break;
    case ESP_BLE_MESH_LIGHTING_SERVER_RECV_STATUS_MSG_EVT:
        if (arg->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_SENSOR_STATUS) {
            bt_mesh_free_buf(arg->value.status.sensor_status.data);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_lighting_server_callback(esp_ble_mesh_lighting_server_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    BT_DBG("%s", __func__);

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_LIGHTING_SERVER)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_LIGHTING_SERVER;
    msg.act = act;

    btc_transfer_context(&msg, cb_params, cb_params == NULL ? 0 : sizeof(esp_ble_mesh_lighting_server_cb_param_t),
                         btc_ble_mesh_lighting_server_copy_req_data, btc_ble_mesh_lighting_server_free_req_data);
}

void bt_mesh_lighting_server_cb_evt_to_btc(uint8_t evt_type, struct bt_mesh_model *model,
                                           struct bt_mesh_msg_ctx *ctx,
                                           const uint8_t *val, size_t len)
{
    esp_ble_mesh_lighting_server_cb_param_t cb_params = {0};
    uint8_t act = 0U;

    if (model == NULL || ctx == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (evt_type) {
    case BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE:
        act = ESP_BLE_MESH_LIGHTING_SERVER_STATE_CHANGE_EVT;
        break;
    case BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_GET_MSG:
        act = ESP_BLE_MESH_LIGHTING_SERVER_RECV_GET_MSG_EVT;
        break;
    case BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_SET_MSG:
        act = ESP_BLE_MESH_LIGHTING_SERVER_RECV_SET_MSG_EVT;
        break;
    case BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_STATUS_MSG:
        act = ESP_BLE_MESH_LIGHTING_SERVER_RECV_STATUS_MSG_EVT;
        break;
    default:
        BT_ERR("Unknown Lighting server event type %d", evt_type);
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
        memcpy(&cb_params.value, val, MIN(len, sizeof(cb_params.value)));
    }

    btc_ble_mesh_lighting_server_callback(&cb_params, act);
    return;
}

void btc_ble_mesh_lighting_server_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_lighting_server_cb_param_t *param = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    param = (esp_ble_mesh_lighting_server_cb_param_t *)(msg->arg);

    if (msg->act < ESP_BLE_MESH_LIGHTING_SERVER_EVT_MAX) {
        btc_ble_mesh_lighting_server_cb_to_app(msg->act, param);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }

    btc_ble_mesh_lighting_server_free_req_data(msg);
    return;
}

#endif /* CONFIG_BLE_MESH_LIGHTING_SERVER */

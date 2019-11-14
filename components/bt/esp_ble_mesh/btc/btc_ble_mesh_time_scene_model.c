// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>
#include <errno.h>

#include "btc/btc_manage.h"
#include "osi/allocator.h"

#include "time_scene_client.h"
#include "btc_ble_mesh_time_scene_model.h"
#include "esp_ble_mesh_time_scene_model_api.h"

/* Time and Scenes Client Models related functions */

static inline void btc_ble_mesh_time_scene_client_cb_to_app(esp_ble_mesh_time_scene_client_cb_event_t event,
        esp_ble_mesh_time_scene_client_cb_param_t *param)
{
    esp_ble_mesh_time_scene_client_cb_t btc_ble_mesh_cb =
        (esp_ble_mesh_time_scene_client_cb_t)btc_profile_cb_get(BTC_PID_TIME_SCENE_CLIENT);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

void btc_ble_mesh_time_scene_client_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_ble_mesh_time_scene_client_args_t *dst = (btc_ble_mesh_time_scene_client_args_t *)p_dest;
    btc_ble_mesh_time_scene_client_args_t *src = (btc_ble_mesh_time_scene_client_args_t *)p_src;

    if (!msg || !dst || !src) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_TIME_SCENE_CLIENT_GET_STATE: {
        dst->time_scene_client_get_state.params = (esp_ble_mesh_client_common_param_t *)osi_malloc(sizeof(esp_ble_mesh_client_common_param_t));
        dst->time_scene_client_get_state.get_state = (esp_ble_mesh_time_scene_client_get_state_t *)osi_malloc(sizeof(esp_ble_mesh_time_scene_client_get_state_t));
        if (dst->time_scene_client_get_state.params && dst->time_scene_client_get_state.get_state) {
            memcpy(dst->time_scene_client_get_state.params, src->time_scene_client_get_state.params,
                   sizeof(esp_ble_mesh_client_common_param_t));
            memcpy(dst->time_scene_client_get_state.get_state, src->time_scene_client_get_state.get_state,
                   sizeof(esp_ble_mesh_time_scene_client_get_state_t));
        } else {
            LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
        }
        break;
    }
    case BTC_BLE_MESH_ACT_TIME_SCENE_CLIENT_SET_STATE: {
        dst->time_scene_client_set_state.params = (esp_ble_mesh_client_common_param_t *)osi_malloc(sizeof(esp_ble_mesh_client_common_param_t));
        dst->time_scene_client_set_state.set_state = (esp_ble_mesh_time_scene_client_set_state_t *)osi_malloc(sizeof(esp_ble_mesh_time_scene_client_set_state_t));
        if (dst->time_scene_client_set_state.params && dst->time_scene_client_set_state.set_state) {
            memcpy(dst->time_scene_client_set_state.params, src->time_scene_client_set_state.params,
                   sizeof(esp_ble_mesh_client_common_param_t));
            memcpy(dst->time_scene_client_set_state.set_state, src->time_scene_client_set_state.set_state,
                   sizeof(esp_ble_mesh_time_scene_client_set_state_t));
        } else {
            LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
        }
        break;
    }
    default:
        LOG_DEBUG("%s, Unknown deep copy act %d", __func__, msg->act);
        break;
    }
}

void btc_ble_mesh_time_scene_client_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_mesh_time_scene_client_args_t *arg = NULL;

    if (!msg || !msg->arg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_time_scene_client_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_TIME_SCENE_CLIENT_GET_STATE:
        if (arg->time_scene_client_get_state.params) {
            osi_free(arg->time_scene_client_get_state.params);
        }
        if (arg->time_scene_client_get_state.get_state) {
            osi_free(arg->time_scene_client_get_state.get_state);
        }
        break;
    case BTC_BLE_MESH_ACT_TIME_SCENE_CLIENT_SET_STATE:
        if (arg->time_scene_client_set_state.params) {
            osi_free(arg->time_scene_client_set_state.params);
        }
        if (arg->time_scene_client_set_state.set_state) {
            osi_free(arg->time_scene_client_set_state.set_state);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_time_scene_client_copy_req_data(btc_msg_t *msg, void *p_dest, void *p_src)
{
    esp_ble_mesh_time_scene_client_cb_param_t *p_dest_data = (esp_ble_mesh_time_scene_client_cb_param_t *)p_dest;
    esp_ble_mesh_time_scene_client_cb_param_t *p_src_data = (esp_ble_mesh_time_scene_client_cb_param_t *)p_src;
    u16_t length;

    if (!msg || !p_src_data || !p_dest_data) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    if (p_src_data->params) {
        p_dest_data->params = osi_malloc(sizeof(esp_ble_mesh_client_common_param_t));
        if (!p_dest_data->params) {
            LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
            return;
        }

        memcpy(p_dest_data->params, p_src_data->params, sizeof(esp_ble_mesh_client_common_param_t));
    }

    switch (msg->act) {
    case ESP_BLE_MESH_TIME_SCENE_CLIENT_GET_STATE_EVT:
    case ESP_BLE_MESH_TIME_SCENE_CLIENT_SET_STATE_EVT:
    case ESP_BLE_MESH_TIME_SCENE_CLIENT_PUBLISH_EVT:
        if (p_src_data->params) {
            switch (p_src_data->params->opcode) {
            case ESP_BLE_MESH_MODEL_OP_SCENE_STORE:
            case ESP_BLE_MESH_MODEL_OP_SCENE_REGISTER_GET:
            case ESP_BLE_MESH_MODEL_OP_SCENE_DELETE:
            case ESP_BLE_MESH_MODEL_OP_SCENE_REGISTER_STATUS:
                if (p_src_data->status_cb.scene_register_status.scenes) {
                    length = p_src_data->status_cb.scene_register_status.scenes->len;
                    p_dest_data->status_cb.scene_register_status.scenes = bt_mesh_alloc_buf(length);
                    if (!p_dest_data->status_cb.scene_register_status.scenes) {
                        LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
                        return;
                    }
                    net_buf_simple_add_mem(p_dest_data->status_cb.scene_register_status.scenes,
                                           p_src_data->status_cb.scene_register_status.scenes->data,
                                           p_src_data->status_cb.scene_register_status.scenes->len);
                }
                break;
            default:
                break;
            }
        }
    case ESP_BLE_MESH_TIME_SCENE_CLIENT_TIMEOUT_EVT:
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_time_scene_client_free_req_data(btc_msg_t *msg)
{
    esp_ble_mesh_time_scene_client_cb_param_t *arg = NULL;

    if (!msg || !msg->arg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_time_scene_client_cb_param_t *)(msg->arg);

    switch (msg->act) {
    case ESP_BLE_MESH_TIME_SCENE_CLIENT_GET_STATE_EVT:
    case ESP_BLE_MESH_TIME_SCENE_CLIENT_SET_STATE_EVT:
    case ESP_BLE_MESH_TIME_SCENE_CLIENT_PUBLISH_EVT:
        if (arg->params) {
            switch (arg->params->opcode) {
            case ESP_BLE_MESH_MODEL_OP_SCENE_STORE:
            case ESP_BLE_MESH_MODEL_OP_SCENE_REGISTER_GET:
            case ESP_BLE_MESH_MODEL_OP_SCENE_DELETE:
            case ESP_BLE_MESH_MODEL_OP_SCENE_REGISTER_STATUS:
                bt_mesh_free_buf(arg->status_cb.scene_register_status.scenes);
                break;
            default:
                break;
            }
        }
    case ESP_BLE_MESH_TIME_SCENE_CLIENT_TIMEOUT_EVT:
        if (arg->params) {
            osi_free(arg->params);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_time_scene_client_callback(esp_ble_mesh_time_scene_client_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    LOG_DEBUG("%s", __func__);

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_TIME_SCENE_CLIENT)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_TIME_SCENE_CLIENT;
    msg.act = act;

    btc_transfer_context(&msg, cb_params,
                         sizeof(esp_ble_mesh_time_scene_client_cb_param_t), btc_ble_mesh_time_scene_client_copy_req_data);
}

void bt_mesh_time_scene_client_cb_evt_to_btc(u32_t opcode, u8_t evt_type,
        struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        const u8_t *val, size_t len)
{
    esp_ble_mesh_time_scene_client_cb_param_t cb_params = {0};
    esp_ble_mesh_client_common_param_t params = {0};
    size_t length;
    uint8_t act;

    if (!model || !ctx) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    switch (evt_type) {
    case BTC_BLE_MESH_EVT_TIME_SCENE_CLIENT_GET_STATE:
        act = ESP_BLE_MESH_TIME_SCENE_CLIENT_GET_STATE_EVT;
        break;
    case BTC_BLE_MESH_EVT_TIME_SCENE_CLIENT_SET_STATE:
        act = ESP_BLE_MESH_TIME_SCENE_CLIENT_SET_STATE_EVT;
        break;
    case BTC_BLE_MESH_EVT_TIME_SCENE_CLIENT_PUBLISH:
        act = ESP_BLE_MESH_TIME_SCENE_CLIENT_PUBLISH_EVT;
        break;
    case BTC_BLE_MESH_EVT_TIME_SCENE_CLIENT_TIMEOUT:
        act = ESP_BLE_MESH_TIME_SCENE_CLIENT_TIMEOUT_EVT;
        break;
    default:
        LOG_ERROR("%s, Unknown time scene client event type %d", __func__, evt_type);
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

    cb_params.error_code = 0;
    cb_params.params = &params;

    if (val && len) {
        length = (len <= sizeof(cb_params.status_cb)) ? len : sizeof(cb_params.status_cb);
        memcpy(&cb_params.status_cb, val, length);
    }

    btc_ble_mesh_time_scene_client_callback(&cb_params, act);
    return;
}

void btc_ble_mesh_time_scene_client_publish_callback(u32_t opcode,
        struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        struct net_buf_simple *buf)
{
    if (!model || !ctx || !buf) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_time_scene_client_cb_evt_to_btc(opcode,
                                            BTC_BLE_MESH_EVT_TIME_SCENE_CLIENT_PUBLISH, model, ctx, buf->data, buf->len);
    return;
}

void btc_ble_mesh_time_scene_client_call_handler(btc_msg_t *msg)
{
    btc_ble_mesh_time_scene_client_args_t *arg = NULL;
    esp_ble_mesh_client_common_param_t *params = NULL;
    esp_ble_mesh_time_scene_client_cb_param_t cb = {0};
    bt_mesh_client_common_param_t common = {0};
    bt_mesh_role_param_t role_param = {0};

    if (!msg || !msg->arg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_time_scene_client_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_TIME_SCENE_CLIENT_GET_STATE: {
        params = arg->time_scene_client_get_state.params;
        role_param.model = (struct bt_mesh_model *)params->model;
        role_param.role = params->msg_role;
        if (bt_mesh_set_client_model_role(&role_param)) {
            LOG_ERROR("%s, Failed to set model role", __func__);
            break;
        }
        common.opcode = params->opcode;
        common.model = (struct bt_mesh_model *)params->model;
        common.ctx.net_idx = params->ctx.net_idx;
        common.ctx.app_idx = params->ctx.app_idx;
        common.ctx.addr = params->ctx.addr;
        common.ctx.send_rel = params->ctx.send_rel;
        common.ctx.send_ttl = params->ctx.send_ttl;
        common.msg_timeout = params->msg_timeout;

        cb.params = arg->time_scene_client_get_state.params;
        cb.error_code = bt_mesh_time_scene_client_get_state(&common,
                        (void *)arg->time_scene_client_get_state.get_state, (void *)&cb.status_cb);
        if (cb.error_code) {
            /* If send failed, callback error_code to app layer immediately */
            btc_ble_mesh_time_scene_client_callback(&cb, ESP_BLE_MESH_TIME_SCENE_CLIENT_GET_STATE_EVT);
        }
        break;
    }
    case BTC_BLE_MESH_ACT_TIME_SCENE_CLIENT_SET_STATE: {
        params = arg->time_scene_client_set_state.params;
        role_param.model = (struct bt_mesh_model *)params->model;
        role_param.role = params->msg_role;
        if (bt_mesh_set_client_model_role(&role_param)) {
            LOG_ERROR("%s, Failed to set model role", __func__);
            break;
        }
        common.opcode = params->opcode;
        common.model = (struct bt_mesh_model *)params->model;
        common.ctx.net_idx = params->ctx.net_idx;
        common.ctx.app_idx = params->ctx.app_idx;
        common.ctx.addr = params->ctx.addr;
        common.ctx.send_rel = params->ctx.send_rel;
        common.ctx.send_ttl = params->ctx.send_ttl;
        common.msg_timeout = params->msg_timeout;

        cb.params = arg->time_scene_client_set_state.params;
        cb.error_code = bt_mesh_time_scene_client_set_state(&common,
                        (void *)arg->time_scene_client_set_state.set_state, (void *)&cb.status_cb);
        if (cb.error_code) {
            /* If send failed, callback error_code to app layer immediately */
            btc_ble_mesh_time_scene_client_callback(&cb, ESP_BLE_MESH_TIME_SCENE_CLIENT_SET_STATE_EVT);
        }
        break;
    }
    default:
        break;
    }

    btc_ble_mesh_time_scene_client_arg_deep_free(msg);
    return;
}

void btc_ble_mesh_time_scene_client_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_time_scene_client_cb_param_t *param = NULL;

    if (!msg || !msg->arg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    param = (esp_ble_mesh_time_scene_client_cb_param_t *)(msg->arg);

    if (msg->act < ESP_BLE_MESH_TIME_SCENE_CLIENT_EVT_MAX) {
        btc_ble_mesh_time_scene_client_cb_to_app(msg->act, param);
    } else {
        LOG_ERROR("%s, Unknown msg->act = %d", __func__, msg->act);
    }

    btc_ble_mesh_time_scene_client_free_req_data(msg);
    return;
}

/* Time and Scenes Server Models related functions */

static inline void btc_ble_mesh_time_scene_server_cb_to_app(
    esp_ble_mesh_time_scene_server_cb_event_t event,
    esp_ble_mesh_time_scene_server_cb_param_t *param)
{
    esp_ble_mesh_time_scene_server_cb_t btc_ble_mesh_cb =
        (esp_ble_mesh_time_scene_server_cb_t)btc_profile_cb_get(BTC_PID_TIME_SCENE_SERVER);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

static void btc_ble_mesh_time_scene_server_callback(esp_ble_mesh_time_scene_server_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    LOG_DEBUG("%s", __func__);

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_TIME_SCENE_SERVER)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_TIME_SCENE_SERVER;
    msg.act = act;

    btc_transfer_context(
        &msg, cb_params, sizeof(esp_ble_mesh_time_scene_server_cb_param_t), NULL);
}

void bt_mesh_time_scene_server_cb_evt_to_btc(u8_t evt_type,
        struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        const u8_t *val, size_t len)
{
    esp_ble_mesh_time_scene_server_cb_param_t cb_params = {0};
    size_t length;
    uint8_t act;

    if (model == NULL || ctx == NULL) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    switch (evt_type) {
    case BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_STATE_CHANGE:
        act = ESP_BLE_MESH_TIME_SCENE_SERVER_STATE_CHANGE_EVT;
        break;
    case BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_RECV_GET_MSG:
        act = ESP_BLE_MESH_TIME_SCENE_SERVER_RECV_GET_MSG_EVT;
        break;
    case BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_RECV_SET_MSG:
        act = ESP_BLE_MESH_TIME_SCENE_SERVER_RECV_SET_MSG_EVT;
        break;
    case BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_RECV_STATUS_MSG:
        act = ESP_BLE_MESH_TIME_SCENE_SERVER_RECV_STATUS_MSG_EVT;
        break;
    default:
        LOG_ERROR("%s, Unknown Time Scene Server event type", __func__);
        return;
    }

    cb_params.model = (esp_ble_mesh_model_t *)model;
    cb_params.ctx.net_idx = ctx->net_idx;
    cb_params.ctx.app_idx = ctx->app_idx;
    cb_params.ctx.addr = ctx->addr;
    cb_params.ctx.recv_ttl = ctx->recv_ttl;
    cb_params.ctx.recv_op = ctx->recv_op;
    cb_params.ctx.recv_dst = ctx->recv_dst;

    if (val && len) {
        length = (len <= sizeof(cb_params.value)) ? len : sizeof(cb_params.value);
        memcpy(&cb_params.value, val, length);
    }

    btc_ble_mesh_time_scene_server_callback(&cb_params, act);
    return;
}

void btc_ble_mesh_time_scene_server_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_time_scene_server_cb_param_t *param = NULL;

    if (!msg || !msg->arg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    param = (esp_ble_mesh_time_scene_server_cb_param_t *)(msg->arg);

    if (msg->act < ESP_BLE_MESH_TIME_SCENE_SERVER_EVT_MAX) {
        btc_ble_mesh_time_scene_server_cb_to_app(msg->act, param);
    } else {
        LOG_ERROR("%s, Unknown msg->act = %d", __func__, msg->act);
    }

    return;
}


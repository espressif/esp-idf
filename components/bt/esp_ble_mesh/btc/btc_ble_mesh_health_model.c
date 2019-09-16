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
#include "btc/btc_task.h"
#include "osi/allocator.h"

#include "health_srv.h"
#include "health_cli.h"
#include "mesh_common.h"

#include "btc_ble_mesh_health_model.h"
#include "esp_ble_mesh_defs.h"

extern s32_t health_msg_timeout;

/* Health Client Model related functions */

static inline void btc_ble_mesh_health_client_cb_to_app(esp_ble_mesh_health_client_cb_event_t event,
        esp_ble_mesh_health_client_cb_param_t *param)
{
    esp_ble_mesh_health_client_cb_t btc_ble_mesh_cb =
        (esp_ble_mesh_health_client_cb_t)btc_profile_cb_get(BTC_PID_HEALTH_CLIENT);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

void btc_ble_mesh_health_client_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_ble_mesh_health_client_args_t *dst = (btc_ble_mesh_health_client_args_t *)p_dest;
    btc_ble_mesh_health_client_args_t *src = (btc_ble_mesh_health_client_args_t *)p_src;

    if (!msg || !dst || !src) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_HEALTH_CLIENT_GET_STATE: {
        dst->health_client_get_state.params = (esp_ble_mesh_client_common_param_t *)osi_malloc(sizeof(esp_ble_mesh_client_common_param_t));
        dst->health_client_get_state.get_state = (esp_ble_mesh_health_client_get_state_t *)osi_malloc(sizeof(esp_ble_mesh_health_client_get_state_t));
        if (dst->health_client_get_state.params && dst->health_client_get_state.get_state) {
            memcpy(dst->health_client_get_state.params, src->health_client_get_state.params,
                   sizeof(esp_ble_mesh_client_common_param_t));
            memcpy(dst->health_client_get_state.get_state, src->health_client_get_state.get_state,
                   sizeof(esp_ble_mesh_health_client_get_state_t));
        } else {
            LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
        }
        break;
    }
    case BTC_BLE_MESH_ACT_HEALTH_CLIENT_SET_STATE: {
        dst->health_client_set_state.params = (esp_ble_mesh_client_common_param_t *)osi_malloc(sizeof(esp_ble_mesh_client_common_param_t));
        dst->health_client_set_state.set_state = (esp_ble_mesh_health_client_set_state_t *)osi_malloc(sizeof(esp_ble_mesh_health_client_set_state_t));
        if (dst->health_client_set_state.params && dst->health_client_set_state.set_state) {
            memcpy(dst->health_client_set_state.params, src->health_client_set_state.params,
                   sizeof(esp_ble_mesh_client_common_param_t));
            memcpy(dst->health_client_set_state.set_state, src->health_client_set_state.set_state,
                   sizeof(esp_ble_mesh_health_client_set_state_t));
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

static void btc_ble_mesh_health_client_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_mesh_health_client_args_t *arg = NULL;

    if (!msg || !msg->arg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_health_client_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_HEALTH_CLIENT_GET_STATE:
        if (arg->health_client_get_state.params) {
            osi_free(arg->health_client_get_state.params);
        }
        if (arg->health_client_get_state.get_state) {
            osi_free(arg->health_client_get_state.get_state);
        }
        break;
    case BTC_BLE_MESH_ACT_HEALTH_CLIENT_SET_STATE:
        if (arg->health_client_set_state.params) {
            osi_free(arg->health_client_set_state.params);
        }
        if (arg->health_client_set_state.set_state) {
            osi_free(arg->health_client_set_state.set_state);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_health_client_copy_req_data(btc_msg_t *msg, void *p_dest, void *p_src)
{
    esp_ble_mesh_health_client_cb_param_t *p_dest_data = (esp_ble_mesh_health_client_cb_param_t *)p_dest;
    esp_ble_mesh_health_client_cb_param_t *p_src_data = (esp_ble_mesh_health_client_cb_param_t *)p_src;
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
    case ESP_BLE_MESH_HEALTH_CLIENT_GET_STATE_EVT:
    case ESP_BLE_MESH_HEALTH_CLIENT_SET_STATE_EVT:
    case ESP_BLE_MESH_HEALTH_CLIENT_PUBLISH_EVT:
        if (p_src_data->params) {
            switch (p_src_data->params->opcode) {
            case OP_HEALTH_CURRENT_STATUS:
                if (p_src_data->status_cb.current_status.fault_array) {
                    length = p_src_data->status_cb.current_status.fault_array->len;
                    p_dest_data->status_cb.current_status.fault_array = bt_mesh_alloc_buf(length);
                    if (!p_dest_data->status_cb.current_status.fault_array) {
                        LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
                        return;
                    }
                    net_buf_simple_add_mem(p_dest_data->status_cb.current_status.fault_array,
                                           p_src_data->status_cb.current_status.fault_array->data,
                                           p_src_data->status_cb.current_status.fault_array->len);
                }
                break;
            case OP_HEALTH_FAULT_GET:
            case OP_HEALTH_FAULT_CLEAR:
            case OP_HEALTH_FAULT_TEST:
            case OP_HEALTH_FAULT_STATUS:
                if (p_src_data->status_cb.fault_status.fault_array) {
                    length = p_src_data->status_cb.fault_status.fault_array->len;
                    p_dest_data->status_cb.fault_status.fault_array = bt_mesh_alloc_buf(length);
                    if (!p_dest_data->status_cb.fault_status.fault_array) {
                        LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
                        return;
                    }
                    net_buf_simple_add_mem(p_dest_data->status_cb.fault_status.fault_array,
                                           p_src_data->status_cb.fault_status.fault_array->data,
                                           p_src_data->status_cb.fault_status.fault_array->len);
                }
                break;
            default:
                break;
            }
        }
    case ESP_BLE_MESH_HEALTH_CLIENT_TIMEOUT_EVT:
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_health_client_free_req_data(btc_msg_t *msg)
{
    esp_ble_mesh_health_client_cb_param_t *arg = NULL;

    if (!msg || !msg->arg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_health_client_cb_param_t *)(msg->arg);

    switch (msg->act) {
    case ESP_BLE_MESH_HEALTH_CLIENT_GET_STATE_EVT:
    case ESP_BLE_MESH_HEALTH_CLIENT_SET_STATE_EVT:
    case ESP_BLE_MESH_HEALTH_CLIENT_PUBLISH_EVT:
        if (arg->params) {
            switch (arg->params->opcode) {
            case OP_HEALTH_CURRENT_STATUS:
                bt_mesh_free_buf(arg->status_cb.current_status.fault_array);
                break;
            case OP_HEALTH_FAULT_GET:
            case OP_HEALTH_FAULT_CLEAR:
            case OP_HEALTH_FAULT_TEST:
            case OP_HEALTH_FAULT_STATUS:
                bt_mesh_free_buf(arg->status_cb.fault_status.fault_array);
                break;
            default:
                break;
            }
        }
    case ESP_BLE_MESH_HEALTH_CLIENT_TIMEOUT_EVT:
        if (arg->params) {
            osi_free(arg->params);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_health_client_callback(esp_ble_mesh_health_client_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    LOG_DEBUG("%s", __func__);

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_HEALTH_CLIENT;
    msg.act = act;

    btc_transfer_context(&msg, cb_params,
        sizeof(esp_ble_mesh_health_client_cb_param_t), btc_ble_mesh_health_client_copy_req_data);
}

void bt_mesh_health_client_cb_evt_to_btc(u32_t opcode, u8_t evt_type,
        struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        const u8_t *val, u16_t len)
{
    esp_ble_mesh_health_client_cb_param_t cb_params = {0};
    esp_ble_mesh_client_common_param_t params = {0};
    size_t length;
    uint8_t act;

    if (!model || !ctx) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    switch (evt_type) {
    case BTC_BLE_MESH_EVT_HEALTH_CLIENT_GET_STATE:
        act = ESP_BLE_MESH_HEALTH_CLIENT_GET_STATE_EVT;
        break;
    case BTC_BLE_MESH_EVT_HEALTH_CLIENT_SET_STATE:
        act = ESP_BLE_MESH_HEALTH_CLIENT_SET_STATE_EVT;
        break;
    case BTC_BLE_MESH_EVT_HEALTH_CLIENT_PUBLISH:
        act = ESP_BLE_MESH_HEALTH_CLIENT_PUBLISH_EVT;
        break;
    case BTC_BLE_MESH_EVT_HEALTH_CLIENT_TIMEOUT:
        act = ESP_BLE_MESH_HEALTH_CLIENT_TIMEOUT_EVT;
        break;
    default:
        LOG_ERROR("%s, Unknown health client event type %d", __func__, evt_type);
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

    btc_ble_mesh_health_client_callback(&cb_params, act);
    return;
}

void btc_ble_mesh_health_publish_callback(u32_t opcode,
        struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        struct net_buf_simple *buf)
{
    if (!model || !ctx || !buf) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_health_client_cb_evt_to_btc(opcode,
        BTC_BLE_MESH_EVT_HEALTH_CLIENT_PUBLISH, model, ctx, buf->data, buf->len);
    return;
}

static int btc_ble_mesh_health_client_get_state(esp_ble_mesh_client_common_param_t *params,
        esp_ble_mesh_health_client_get_state_t *get,
        esp_ble_mesh_health_client_cb_param_t *cb)
{
    struct bt_mesh_msg_ctx ctx = {0};

    if (!params || !cb) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    ctx.net_idx = params->ctx.net_idx;
    ctx.app_idx = params->ctx.app_idx;
    ctx.addr = params->ctx.addr;
    ctx.send_rel = params->ctx.send_rel;
    ctx.send_ttl = params->ctx.send_ttl;

    health_msg_timeout = params->msg_timeout;

    switch (params->opcode) {
    case ESP_BLE_MESH_MODEL_OP_ATTENTION_GET:
        return (cb->error_code = bt_mesh_health_attention_get(&ctx));
    case ESP_BLE_MESH_MODEL_OP_HEALTH_PERIOD_GET:
        return (cb->error_code = bt_mesh_health_period_get(&ctx));
    case ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_GET:
        return (cb->error_code = bt_mesh_health_fault_get(&ctx, get->fault_get.company_id));
    default:
        LOG_ERROR("%s, Invalid opcode 0x%x", __func__, params->opcode);
        return (cb->error_code = -EINVAL);
    }

    return 0;
}

static int btc_ble_mesh_health_client_set_state(esp_ble_mesh_client_common_param_t *params,
        esp_ble_mesh_health_client_set_state_t *set,
        esp_ble_mesh_health_client_cb_param_t *cb)
{
    struct bt_mesh_msg_ctx ctx = {0};

    if (!params || !set || !cb) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    ctx.net_idx = params->ctx.net_idx;
    ctx.app_idx = params->ctx.app_idx;
    ctx.addr = params->ctx.addr;
    ctx.send_rel = params->ctx.send_rel;
    ctx.send_ttl = params->ctx.send_ttl;

    health_msg_timeout = params->msg_timeout;

    switch (params->opcode) {
    case ESP_BLE_MESH_MODEL_OP_ATTENTION_SET:
        return (cb->error_code =
                    bt_mesh_health_attention_set(&ctx, set->attention_set.attention, true));
    case ESP_BLE_MESH_MODEL_OP_ATTENTION_SET_UNACK:
        return (cb->error_code =
                    bt_mesh_health_attention_set(&ctx, set->attention_set.attention, false));
    case ESP_BLE_MESH_MODEL_OP_HEALTH_PERIOD_SET:
        return (cb->error_code =
                    bt_mesh_health_period_set(&ctx, set->period_set.fast_period_divisor, true));
    case ESP_BLE_MESH_MODEL_OP_HEALTH_PERIOD_SET_UNACK:
        return (cb->error_code =
                    bt_mesh_health_period_set(&ctx, set->period_set.fast_period_divisor, false));
    case ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_TEST:
        return (cb->error_code =
                    bt_mesh_health_fault_test(&ctx, set->fault_test.company_id, set->fault_test.test_id, true));
    case ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_TEST_UNACK:
        return (cb->error_code =
                    bt_mesh_health_fault_test(&ctx, set->fault_test.company_id, set->fault_test.test_id, false));
    case ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_CLEAR:
        return (cb->error_code =
                    bt_mesh_health_fault_clear(&ctx, set->fault_clear.company_id, true));
    case ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_CLEAR_UNACK:
        return (cb->error_code =
                    bt_mesh_health_fault_clear(&ctx, set->fault_clear.company_id, false));
    default:
        LOG_ERROR("%s, Invalid opcode 0x%x", __func__, params->opcode);
        return (cb->error_code = -EINVAL);
    }

    return 0;
}

void btc_ble_mesh_health_client_call_handler(btc_msg_t *msg)
{
    btc_ble_mesh_health_client_args_t *arg = NULL;
    esp_ble_mesh_health_client_cb_param_t cb = {0};
    bt_mesh_role_param_t role_param = {0};

    if (!msg || !msg->arg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_health_client_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_HEALTH_CLIENT_GET_STATE: {
        cb.params = arg->health_client_get_state.params;
        role_param.model = (struct bt_mesh_model *)cb.params->model;
        role_param.role = cb.params->msg_role;
        if (bt_mesh_set_client_model_role(&role_param)) {
            LOG_ERROR("%s, Failed to set model role", __func__);
            break;
        }
        btc_ble_mesh_health_client_get_state(arg->health_client_get_state.params,
            arg->health_client_get_state.get_state, &cb);
        if (cb.error_code) {
            /* If send failed, callback error_code to app layer immediately */
            btc_ble_mesh_health_client_callback(&cb, ESP_BLE_MESH_HEALTH_CLIENT_GET_STATE_EVT);
        }
        break;
    }
    case BTC_BLE_MESH_ACT_HEALTH_CLIENT_SET_STATE: {
        cb.params = arg->health_client_set_state.params;
        role_param.model = (struct bt_mesh_model *)cb.params->model;
        role_param.role = cb.params->msg_role;
        if (bt_mesh_set_client_model_role(&role_param)) {
            LOG_ERROR("%s, Failed to set model role", __func__);
            break;
        }
        btc_ble_mesh_health_client_set_state(arg->health_client_set_state.params,
            arg->health_client_set_state.set_state, &cb);
        if (cb.error_code) {
            /* If send failed, callback error_code to app layer immediately */
            btc_ble_mesh_health_client_callback(&cb, ESP_BLE_MESH_HEALTH_CLIENT_SET_STATE_EVT);
        }
        break;
    }
    default:
        break;
    }

    btc_ble_mesh_health_client_arg_deep_free(msg);
    return;
}

void btc_ble_mesh_health_client_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_health_client_cb_param_t *param = NULL;

    if (!msg || !msg->arg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    param = (esp_ble_mesh_health_client_cb_param_t *)(msg->arg);

    if (msg->act < ESP_BLE_MESH_HEALTH_CLIENT_EVT_MAX) {
        btc_ble_mesh_health_client_cb_to_app(msg->act, param);
    } else {
        LOG_ERROR("%s, Unknown msg->act = %d", __func__, msg->act);
    }

    btc_ble_mesh_health_client_free_req_data(msg);
    return;
}

/* Health Server Model related functions */

static inline void btc_ble_mesh_health_server_cb_to_app(esp_ble_mesh_health_server_cb_event_t event,
        esp_ble_mesh_health_server_cb_param_t *param)
{
    esp_ble_mesh_health_server_cb_t btc_ble_mesh_cb =
        (esp_ble_mesh_health_server_cb_t)btc_profile_cb_get(BTC_PID_HEALTH_SERVER);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

void btc_ble_mesh_health_server_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    if (!msg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_HEALTH_SERVER_FAULT_UPDATE:
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_health_server_arg_deep_free(btc_msg_t *msg)
{
    if (!msg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_HEALTH_SERVER_FAULT_UPDATE:
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_health_server_copy_req_data(btc_msg_t *msg, void *p_dest, void *p_src)
{
    if (!msg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case ESP_BLE_MESH_HEALTH_SERVER_FAULT_UPDATE_COMPLETE_EVT:
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_health_server_free_req_data(btc_msg_t *msg)
{
    if (!msg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case ESP_BLE_MESH_HEALTH_SERVER_FAULT_UPDATE_COMPLETE_EVT:
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_health_server_callback(esp_ble_mesh_health_server_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    LOG_DEBUG("%s", __func__);

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_HEALTH_SERVER;
    msg.act = act;

    btc_transfer_context(&msg, cb_params,
        sizeof(esp_ble_mesh_health_server_cb_param_t), btc_ble_mesh_health_server_copy_req_data);
}

void btc_ble_mesh_health_server_call_handler(btc_msg_t *msg)
{
    esp_ble_mesh_health_server_cb_param_t health_server_cb = {0};
    btc_ble_mesh_health_server_args_t *arg = NULL;

    if (!msg || !msg->arg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_health_server_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_HEALTH_SERVER_FAULT_UPDATE:
        health_server_cb.error_code =
            bt_mesh_fault_update((struct bt_mesh_elem *)arg->fault_update.element);
        btc_ble_mesh_health_server_callback(
            &health_server_cb, ESP_BLE_MESH_HEALTH_SERVER_FAULT_UPDATE_COMPLETE_EVT);
        break;
    default:
        break;
    }

    btc_ble_mesh_health_server_arg_deep_free(msg);
    return;
}

void btc_ble_mesh_health_server_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_health_server_cb_param_t *param = NULL;

    if (!msg || !msg->arg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    param = (esp_ble_mesh_health_server_cb_param_t *)(msg->arg);

    if (msg->act < ESP_BLE_MESH_HEALTH_SERVER_EVT_MAX) {
        btc_ble_mesh_health_server_cb_to_app(msg->act, param);
    } else {
        LOG_ERROR("%s, Unknown msg->act = %d", __func__, msg->act);
    }

    btc_ble_mesh_health_server_free_req_data(msg);
    return;
}

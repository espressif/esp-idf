/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "btc_ble_mesh_model_common.h"
#include "btc_ble_mesh_health_model.h"
#include "esp_ble_mesh_health_model_api.h"

#if CONFIG_BLE_MESH_HEALTH_CLI
#include "mesh/health_cli.h"

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
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_HEALTH_CLIENT_GET_STATE: {
        dst->health_client_get_state.params = (esp_ble_mesh_client_common_param_t *)bt_mesh_malloc(sizeof(esp_ble_mesh_client_common_param_t));
        if (dst->health_client_get_state.params) {
            memcpy(dst->health_client_get_state.params, src->health_client_get_state.params,
                   sizeof(esp_ble_mesh_client_common_param_t));
        } else {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            break;
        }
        if (src->health_client_get_state.get_state) {
            dst->health_client_get_state.get_state = (esp_ble_mesh_health_client_get_state_t *)bt_mesh_malloc(sizeof(esp_ble_mesh_health_client_get_state_t));
            if (dst->health_client_get_state.get_state) {
                memcpy(dst->health_client_get_state.get_state, src->health_client_get_state.get_state,
                    sizeof(esp_ble_mesh_health_client_get_state_t));
            } else {
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            }
        }
        break;
    }
    case BTC_BLE_MESH_ACT_HEALTH_CLIENT_SET_STATE: {
        dst->health_client_set_state.params = (esp_ble_mesh_client_common_param_t *)bt_mesh_malloc(sizeof(esp_ble_mesh_client_common_param_t));
        dst->health_client_set_state.set_state = (esp_ble_mesh_health_client_set_state_t *)bt_mesh_malloc(sizeof(esp_ble_mesh_health_client_set_state_t));
        if (dst->health_client_set_state.params && dst->health_client_set_state.set_state) {
            memcpy(dst->health_client_set_state.params, src->health_client_set_state.params,
                   sizeof(esp_ble_mesh_client_common_param_t));
            memcpy(dst->health_client_set_state.set_state, src->health_client_set_state.set_state,
                   sizeof(esp_ble_mesh_health_client_set_state_t));
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

void btc_ble_mesh_health_client_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_mesh_health_client_args_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_health_client_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_HEALTH_CLIENT_GET_STATE:
        if (arg->health_client_get_state.params) {
            bt_mesh_free(arg->health_client_get_state.params);
        }
        if (arg->health_client_get_state.get_state) {
            bt_mesh_free(arg->health_client_get_state.get_state);
        }
        break;
    case BTC_BLE_MESH_ACT_HEALTH_CLIENT_SET_STATE:
        if (arg->health_client_set_state.params) {
            bt_mesh_free(arg->health_client_set_state.params);
        }
        if (arg->health_client_set_state.set_state) {
            bt_mesh_free(arg->health_client_set_state.set_state);
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
    case ESP_BLE_MESH_HEALTH_CLIENT_GET_STATE_EVT:
    case ESP_BLE_MESH_HEALTH_CLIENT_SET_STATE_EVT:
    case ESP_BLE_MESH_HEALTH_CLIENT_PUBLISH_EVT:
        if (p_src_data->params) {
            switch (p_src_data->params->opcode) {
            case ESP_BLE_MESH_MODEL_OP_HEALTH_CURRENT_STATUS:
                if (p_src_data->status_cb.current_status.fault_array) {
                    length = p_src_data->status_cb.current_status.fault_array->len;
                    p_dest_data->status_cb.current_status.fault_array = bt_mesh_alloc_buf(length);
                    if (!p_dest_data->status_cb.current_status.fault_array) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        return;
                    }
                    net_buf_simple_add_mem(p_dest_data->status_cb.current_status.fault_array,
                                           p_src_data->status_cb.current_status.fault_array->data,
                                           p_src_data->status_cb.current_status.fault_array->len);
                }
                break;
            case ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_GET:
            case ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_CLEAR:
            case ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_TEST:
            case ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_STATUS:
                if (p_src_data->status_cb.fault_status.fault_array) {
                    length = p_src_data->status_cb.fault_status.fault_array->len;
                    p_dest_data->status_cb.fault_status.fault_array = bt_mesh_alloc_buf(length);
                    if (!p_dest_data->status_cb.fault_status.fault_array) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
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

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_health_client_cb_param_t *)(msg->arg);

    switch (msg->act) {
    case ESP_BLE_MESH_HEALTH_CLIENT_GET_STATE_EVT:
    case ESP_BLE_MESH_HEALTH_CLIENT_SET_STATE_EVT:
    case ESP_BLE_MESH_HEALTH_CLIENT_PUBLISH_EVT:
        if (arg->params) {
            switch (arg->params->opcode) {
            case ESP_BLE_MESH_MODEL_OP_HEALTH_CURRENT_STATUS:
                bt_mesh_free_buf(arg->status_cb.current_status.fault_array);
                break;
            case ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_GET:
            case ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_CLEAR:
            case ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_TEST:
            case ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_STATUS:
                bt_mesh_free_buf(arg->status_cb.fault_status.fault_array);
                break;
            default:
                break;
            }
        }
    case ESP_BLE_MESH_HEALTH_CLIENT_TIMEOUT_EVT:
        if (arg->params) {
            bt_mesh_free(arg->params);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_health_client_callback(esp_ble_mesh_health_client_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_HEALTH_CLIENT)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_HEALTH_CLIENT;
    msg.act = act;

    btc_transfer_context(&msg, cb_params, cb_params == NULL ? 0 : sizeof(esp_ble_mesh_health_client_cb_param_t),
                         btc_ble_mesh_health_client_copy_req_data, btc_ble_mesh_health_client_free_req_data);
}

void bt_mesh_health_client_cb_evt_to_btc(uint32_t opcode, uint8_t evt_type,
                                         struct bt_mesh_model *model,
                                         struct bt_mesh_msg_ctx *ctx,
                                         const uint8_t *val, uint16_t len)
{
    esp_ble_mesh_health_client_cb_param_t cb_params = {0};
    esp_ble_mesh_client_common_param_t params = {0};
    uint8_t act = 0U;

    if (!model || !ctx || len > sizeof(cb_params.status_cb)) {
        BT_ERR("%s, Invalid parameter", __func__);
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
        BT_ERR("Unknown Health client event type %d", evt_type);
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
        memcpy(&cb_params.status_cb, val, len);
    }

    btc_ble_mesh_health_client_callback(&cb_params, act);
}

void btc_ble_mesh_health_publish_callback(uint32_t opcode,
                                          struct bt_mesh_model *model,
                                          struct bt_mesh_msg_ctx *ctx,
                                          struct net_buf_simple *buf)
{
    if (!model || !ctx || !buf) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_health_client_cb_evt_to_btc(opcode, BTC_BLE_MESH_EVT_HEALTH_CLIENT_PUBLISH,
                                        model, ctx, buf->data, buf->len);
}

static int btc_ble_mesh_health_client_get_state(esp_ble_mesh_client_common_param_t *params,
                                                esp_ble_mesh_health_client_get_state_t *get)
{
    bt_mesh_client_common_param_t param = {0};

    if (params == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    if (params->opcode == ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_GET && get == NULL) {
        BT_ERR("Invalid Health Get");
        return -EINVAL;
    }

    btc_ble_mesh_set_client_common_param(params, &param, false);

    switch (param.opcode) {
    case ESP_BLE_MESH_MODEL_OP_ATTENTION_GET:
        return bt_mesh_health_attention_get(&param);
    case ESP_BLE_MESH_MODEL_OP_HEALTH_PERIOD_GET:
        return bt_mesh_health_period_get(&param);
    case ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_GET:
        return bt_mesh_health_fault_get(&param, get->fault_get.company_id);
    default:
        BT_ERR("Invalid Health Get opcode 0x%04x", param.opcode);
        return -EINVAL;
    }
}

static int btc_ble_mesh_health_client_set_state(esp_ble_mesh_client_common_param_t *params,
                                                esp_ble_mesh_health_client_set_state_t *set)
{
    bt_mesh_client_common_param_t param = {0};

    if (params == NULL || set == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    btc_ble_mesh_set_client_common_param(params, &param, false);

    switch (param.opcode) {
    case ESP_BLE_MESH_MODEL_OP_ATTENTION_SET:
        return bt_mesh_health_attention_set(&param, set->attention_set.attention, true);
    case ESP_BLE_MESH_MODEL_OP_ATTENTION_SET_UNACK:
        return bt_mesh_health_attention_set(&param, set->attention_set.attention, false);
    case ESP_BLE_MESH_MODEL_OP_HEALTH_PERIOD_SET:
        return bt_mesh_health_period_set(&param, set->period_set.fast_period_divisor, true);
    case ESP_BLE_MESH_MODEL_OP_HEALTH_PERIOD_SET_UNACK:
        return bt_mesh_health_period_set(&param, set->period_set.fast_period_divisor, false);
    case ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_TEST:
        return bt_mesh_health_fault_test(&param, set->fault_test.company_id, set->fault_test.test_id, true);
    case ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_TEST_UNACK:
        return bt_mesh_health_fault_test(&param, set->fault_test.company_id, set->fault_test.test_id, false);
    case ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_CLEAR:
        return bt_mesh_health_fault_clear(&param, set->fault_clear.company_id, true);
    case ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_CLEAR_UNACK:
        return bt_mesh_health_fault_clear(&param, set->fault_clear.company_id, false);
    default:
        BT_ERR("Invalid Health Set opcode 0x%04x", param.opcode);
        return -EINVAL;
    }
}

void btc_ble_mesh_health_client_call_handler(btc_msg_t *msg)
{
    btc_ble_mesh_health_client_args_t *arg = NULL;
    esp_ble_mesh_health_client_cb_param_t cb = {0};

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_health_client_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_HEALTH_CLIENT_GET_STATE: {
        cb.params = arg->health_client_get_state.params;
        cb.error_code = btc_ble_mesh_health_client_get_state(arg->health_client_get_state.params,
                                                             arg->health_client_get_state.get_state);
        if (cb.error_code) {
            /* If send failed, callback error_code to app layer immediately */
            btc_ble_mesh_health_client_callback(&cb, ESP_BLE_MESH_HEALTH_CLIENT_GET_STATE_EVT);
        }
        break;
    }
    case BTC_BLE_MESH_ACT_HEALTH_CLIENT_SET_STATE: {
        cb.params = arg->health_client_set_state.params;
        cb.error_code = btc_ble_mesh_health_client_set_state(arg->health_client_set_state.params,
                                                             arg->health_client_set_state.set_state);
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
}

void btc_ble_mesh_health_client_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_health_client_cb_param_t *param = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    param = (esp_ble_mesh_health_client_cb_param_t *)(msg->arg);

    if (msg->act < ESP_BLE_MESH_HEALTH_CLIENT_EVT_MAX) {
        btc_ble_mesh_health_client_cb_to_app(msg->act, param);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }

    btc_ble_mesh_health_client_free_req_data(msg);
}

#endif /* CONFIG_BLE_MESH_HEALTH_CLI */

#if CONFIG_BLE_MESH_HEALTH_SRV
#include "mesh/health_srv.h"

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
        BT_ERR("%s, Invalid parameter", __func__);
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
        BT_ERR("%s, Invalid parameter", __func__);
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
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case ESP_BLE_MESH_HEALTH_SERVER_FAULT_UPDATE_COMP_EVT:
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_health_server_free_req_data(btc_msg_t *msg)
{
    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case ESP_BLE_MESH_HEALTH_SERVER_FAULT_UPDATE_COMP_EVT:
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_health_server_callback(esp_ble_mesh_health_server_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_HEALTH_SERVER)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_HEALTH_SERVER;
    msg.act = act;

    btc_transfer_context(&msg, cb_params, cb_params == NULL ? 0 : sizeof(esp_ble_mesh_health_server_cb_param_t),
                         btc_ble_mesh_health_server_copy_req_data, btc_ble_mesh_health_server_free_req_data);
}

void btc_ble_mesh_health_server_call_handler(btc_msg_t *msg)
{
    esp_ble_mesh_health_server_cb_param_t param = {0};
    btc_ble_mesh_health_server_args_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_health_server_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_HEALTH_SERVER_FAULT_UPDATE:
        param.fault_update_comp.element = arg->health_fault_update.element;
        param.fault_update_comp.error_code =
            bt_mesh_fault_update((struct bt_mesh_elem *)arg->health_fault_update.element);
        btc_ble_mesh_health_server_callback(&param, ESP_BLE_MESH_HEALTH_SERVER_FAULT_UPDATE_COMP_EVT);
        break;
    default:
        break;
    }

    btc_ble_mesh_health_server_arg_deep_free(msg);
}

void btc_ble_mesh_health_server_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_health_server_cb_param_t *param = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    param = (esp_ble_mesh_health_server_cb_param_t *)(msg->arg);

    if (msg->act < ESP_BLE_MESH_HEALTH_SERVER_EVT_MAX) {
        btc_ble_mesh_health_server_cb_to_app(msg->act, param);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }

    btc_ble_mesh_health_server_free_req_data(msg);
}

void btc_ble_mesh_health_server_fault_clear(struct bt_mesh_model *model, uint16_t company_id)
{
    esp_ble_mesh_health_server_cb_param_t param = {0};

    param.fault_clear.model = (esp_ble_mesh_model_t *)model;
    param.fault_clear.company_id = company_id;

    btc_ble_mesh_health_server_callback(&param, ESP_BLE_MESH_HEALTH_SERVER_FAULT_CLEAR_EVT);
}

void btc_ble_mesh_health_server_fault_test(struct bt_mesh_model *model,
                                           uint8_t test_id, uint16_t company_id)
{
    esp_ble_mesh_health_server_cb_param_t param = {0};

    param.fault_test.model = (esp_ble_mesh_model_t *)model;
    param.fault_test.test_id = test_id;
    param.fault_test.company_id = company_id;

    btc_ble_mesh_health_server_callback(&param, ESP_BLE_MESH_HEALTH_SERVER_FAULT_TEST_EVT);
}

void btc_ble_mesh_health_server_attention_on(struct bt_mesh_model *model, uint8_t time)
{
    esp_ble_mesh_health_server_cb_param_t param = {0};

    param.attention_on.model = (esp_ble_mesh_model_t *)model;
    param.attention_on.time = time;

    btc_ble_mesh_health_server_callback(&param, ESP_BLE_MESH_HEALTH_SERVER_ATTENTION_ON_EVT);
}

void btc_ble_mesh_health_server_attention_off(struct bt_mesh_model *model)
{
    esp_ble_mesh_health_server_cb_param_t param = {0};

    param.attention_off.model = (esp_ble_mesh_model_t *)model;

    btc_ble_mesh_health_server_callback(&param, ESP_BLE_MESH_HEALTH_SERVER_ATTENTION_OFF_EVT);
}
#endif /* CONFIG_BLE_MESH_HEALTH_SRV */

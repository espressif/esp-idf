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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "btc/btc_manage.h"
#include "osi/allocator.h"

#include "sdkconfig.h"

#include "mesh_util.h"
#include "mesh_main.h"
#include "mesh_access.h"
#include "mesh_proxy.h"
#include "cfg_cli.h"
#include "health_cli.h"
#include "cfg_srv.h"
#include "health_srv.h"

#include "mesh.h"
#include "access.h"
#include "transport.h"
#include "proxy_server.h"
#include "prov.h"
#include "proxy_client.h"
#include "provisioner_prov.h"
#include "provisioner_main.h"

#include "generic_client.h"
#include "lighting_client.h"
#include "sensor_client.h"
#include "time_scene_client.h"
#include "client_common.h"
#include "state_binding.h"

#include "btc_ble_mesh_prov.h"
#include "btc_ble_mesh_config_model.h"
#include "btc_ble_mesh_health_model.h"
#include "btc_ble_mesh_generic_model.h"
#include "btc_ble_mesh_time_scene_model.h"
#include "btc_ble_mesh_sensor_model.h"
#include "btc_ble_mesh_lighting_model.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_networking_api.h"

static inline void btc_ble_mesh_prov_cb_to_app(esp_ble_mesh_prov_cb_event_t event,
        esp_ble_mesh_prov_cb_param_t *param)
{
    esp_ble_mesh_prov_cb_t btc_ble_mesh_cb =
        (esp_ble_mesh_prov_cb_t)btc_profile_cb_get(BTC_PID_PROV);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

static inline void btc_ble_mesh_model_cb_to_app(esp_ble_mesh_model_cb_event_t event,
        esp_ble_mesh_model_cb_param_t *param)
{
    esp_ble_mesh_model_cb_t btc_ble_mesh_cb =
        (esp_ble_mesh_model_cb_t)btc_profile_cb_get(BTC_PID_MODEL);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

void btc_ble_mesh_prov_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_ble_mesh_prov_args_t *dst = (btc_ble_mesh_prov_args_t *)p_dest;
    btc_ble_mesh_prov_args_t *src = (btc_ble_mesh_prov_args_t *)p_src;

    if (!msg || !dst || !src) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_PROXY_CLIENT_ADD_FILTER_ADDR:
        LOG_DEBUG("%s, BTC_BLE_MESH_ACT_PROXY_CLIENT_ADD_FILTER_ADDR", __func__);
        dst->proxy_client_add_filter_addr.addr = (uint16_t *)osi_calloc(src->proxy_client_add_filter_addr.addr_num << 1);
        if (dst->proxy_client_add_filter_addr.addr) {
            memcpy(dst->proxy_client_add_filter_addr.addr, src->proxy_client_add_filter_addr.addr,
                   src->proxy_client_add_filter_addr.addr_num << 1);
        } else {
            LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
        }
        break;
    case BTC_BLE_MESH_ACT_PROXY_CLIENT_REMOVE_FILTER_ADDR:
        LOG_DEBUG("%s, BTC_BLE_MESH_ACT_PROXY_CLIENT_REMOVE_FILTER_ADDR", __func__);
        dst->proxy_client_remove_filter_addr.addr = osi_calloc(src->proxy_client_remove_filter_addr.addr_num << 1);
        if (dst->proxy_client_remove_filter_addr.addr) {
            memcpy(dst->proxy_client_remove_filter_addr.addr, src->proxy_client_remove_filter_addr.addr,
                   src->proxy_client_remove_filter_addr.addr_num << 1);
        } else {
            LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
        }
        break;
    default:
        LOG_DEBUG("%s, Unknown deep copy act %d", __func__, msg->act);
        break;
    }
}

static void btc_ble_mesh_prov_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_mesh_prov_args_t *arg = NULL;

    if (!msg || !msg->arg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_prov_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_PROXY_CLIENT_ADD_FILTER_ADDR:
        if (arg->proxy_client_add_filter_addr.addr) {
            osi_free(arg->proxy_client_add_filter_addr.addr);
        }
        break;
    case BTC_BLE_MESH_ACT_PROXY_CLIENT_REMOVE_FILTER_ADDR:
        if (arg->proxy_client_remove_filter_addr.addr) {
            osi_free(arg->proxy_client_remove_filter_addr.addr);
        }
        break;
    default:
        break;
    }
}

void btc_ble_mesh_model_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_ble_mesh_model_args_t *dst = (btc_ble_mesh_model_args_t *)p_dest;
    btc_ble_mesh_model_args_t *src = (btc_ble_mesh_model_args_t *)p_src;

    if (!msg || !dst || !src) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_SERVER_MODEL_SEND:
    case BTC_BLE_MESH_ACT_CLIENT_MODEL_SEND: {
        LOG_DEBUG("%s, BTC_BLE_MESH_ACT_MODEL_SEND, src->model_send.length = %d", __func__, src->model_send.length);
        dst->model_send.data = src->model_send.length ? (uint8_t *)osi_malloc(src->model_send.length) : NULL;
        dst->model_send.ctx = osi_malloc(sizeof(esp_ble_mesh_msg_ctx_t));
        if (src->model_send.length) {
            if (dst->model_send.data) {
                memcpy(dst->model_send.data, src->model_send.data, src->model_send.length);
            } else {
                LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
            }
        }
        if (dst->model_send.ctx) {
            memcpy(dst->model_send.ctx, src->model_send.ctx, sizeof(esp_ble_mesh_msg_ctx_t));
        } else {
            LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
        }
        break;
    }
    case BTC_BLE_MESH_ACT_SERVER_MODEL_UPDATE_STATE:
        LOG_DEBUG("%s, BTC_BLE_MESH_ACT_SERVER_MODEL_UPDATE_STATE", __func__);
        dst->model_update_state.value = osi_malloc(sizeof(esp_ble_mesh_server_state_value_t));
        if (dst->model_update_state.value) {
            memcpy(dst->model_update_state.value, src->model_update_state.value,
                   sizeof(esp_ble_mesh_server_state_value_t));
        } else {
            LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
        }
        break;
    default:
        LOG_DEBUG("%s, Unknown deep copy act %d", __func__, msg->act);
        break;
    }
}

static void btc_ble_mesh_model_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_mesh_model_args_t *arg = NULL;

    if (!msg || !msg->arg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_model_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_SERVER_MODEL_SEND:
    case BTC_BLE_MESH_ACT_CLIENT_MODEL_SEND:
        if (arg->model_send.data) {
            osi_free(arg->model_send.data);
        }
        if (arg->model_send.ctx) {
            osi_free(arg->model_send.ctx);
        }
        break;
    case BTC_BLE_MESH_ACT_SERVER_MODEL_UPDATE_STATE:
        if (arg->model_update_state.value) {
            osi_free(arg->model_update_state.value);
        }
        break;
    default:
        break;
    }

    return;
}

static void btc_ble_mesh_model_copy_req_data(btc_msg_t *msg, void *p_dest, void *p_src)
{
    esp_ble_mesh_model_cb_param_t *p_dest_data = (esp_ble_mesh_model_cb_param_t *)p_dest;
    esp_ble_mesh_model_cb_param_t *p_src_data = (esp_ble_mesh_model_cb_param_t *)p_src;

    if (!msg || !p_src_data || !p_dest_data) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case ESP_BLE_MESH_MODEL_OPERATION_EVT: {
        if (p_src_data->model_operation.ctx && p_src_data->model_operation.msg) {
            p_dest_data->model_operation.ctx = osi_malloc(sizeof(esp_ble_mesh_msg_ctx_t));
            p_dest_data->model_operation.msg = p_src_data->model_operation.length ? (uint8_t *)osi_malloc(p_src_data->model_operation.length) : NULL;
            if (p_dest_data->model_operation.ctx) {
                memcpy(p_dest_data->model_operation.ctx, p_src_data->model_operation.ctx, sizeof(esp_ble_mesh_msg_ctx_t));
            } else {
                LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
            }
            if (p_src_data->model_operation.length) {
                if (p_dest_data->model_operation.msg) {
                    memcpy(p_dest_data->model_operation.msg, p_src_data->model_operation.msg, p_src_data->model_operation.length);
                } else {
                    LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
                }
            }
        }
        break;
    }
    case ESP_BLE_MESH_CLIENT_MODEL_RECV_PUBLISH_MSG_EVT: {
        if (p_src_data->client_recv_publish_msg.ctx && p_src_data->client_recv_publish_msg.msg) {
            p_dest_data->client_recv_publish_msg.ctx = osi_malloc(sizeof(esp_ble_mesh_msg_ctx_t));
            p_dest_data->client_recv_publish_msg.msg = p_src_data->client_recv_publish_msg.length ? (uint8_t *)osi_malloc(p_src_data->client_recv_publish_msg.length) : NULL;
            if (p_dest_data->client_recv_publish_msg.ctx) {
                memcpy(p_dest_data->client_recv_publish_msg.ctx, p_src_data->client_recv_publish_msg.ctx, sizeof(esp_ble_mesh_msg_ctx_t));
            } else {
                LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
            }
            if (p_src_data->client_recv_publish_msg.length) {
                if (p_dest_data->client_recv_publish_msg.msg) {
                    memcpy(p_dest_data->client_recv_publish_msg.msg, p_src_data->client_recv_publish_msg.msg, p_src_data->client_recv_publish_msg.length);
                } else {
                    LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
                }
            }
        }
        break;
    }
    case ESP_BLE_MESH_MODEL_SEND_COMP_EVT: {
        if (p_src_data->model_send_comp.ctx) {
            p_dest_data->model_send_comp.ctx = osi_malloc(sizeof(esp_ble_mesh_msg_ctx_t));
            if (p_dest_data->model_send_comp.ctx) {
                memcpy(p_dest_data->model_send_comp.ctx, p_src_data->model_send_comp.ctx, sizeof(esp_ble_mesh_msg_ctx_t));
            } else {
                LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
            }
        }
        break;
    }
    case ESP_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT: {
        if (p_src_data->client_send_timeout.ctx) {
            p_dest_data->client_send_timeout.ctx = osi_malloc(sizeof(esp_ble_mesh_msg_ctx_t));
            if (p_dest_data->client_send_timeout.ctx) {
                memcpy(p_dest_data->client_send_timeout.ctx, p_src_data->client_send_timeout.ctx, sizeof(esp_ble_mesh_msg_ctx_t));
            } else {
                LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
            }
        }
        break;
    }
    default:
        break;
    }
}

static void btc_ble_mesh_model_free_req_data(btc_msg_t *msg)
{
    esp_ble_mesh_model_cb_param_t *arg = NULL;

    if (!msg || !msg->arg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_model_cb_param_t *)(msg->arg);

    switch (msg->act) {
    case ESP_BLE_MESH_MODEL_OPERATION_EVT: {
        if (arg->model_operation.msg) {
            osi_free(arg->model_operation.msg);
        }
        if (arg->model_operation.ctx) {
            osi_free(arg->model_operation.ctx);
        }
        break;
    }
    case ESP_BLE_MESH_CLIENT_MODEL_RECV_PUBLISH_MSG_EVT: {
        if (arg->client_recv_publish_msg.msg) {
            osi_free(arg->client_recv_publish_msg.msg);
        }
        if (arg->client_recv_publish_msg.ctx) {
            osi_free(arg->client_recv_publish_msg.ctx);
        }
        break;
    }
    case ESP_BLE_MESH_MODEL_SEND_COMP_EVT: {
        if (arg->model_send_comp.ctx) {
            osi_free(arg->model_send_comp.ctx);
        }
        break;
    }
    case ESP_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT: {
        if (arg->client_send_timeout.ctx) {
            osi_free(arg->client_send_timeout.ctx);
        }
        break;
    }
    default:
        break;
    }
}

static bt_status_t btc_ble_mesh_model_callback(esp_ble_mesh_model_cb_param_t *param, uint8_t act)
{
    btc_msg_t msg = {0};
    bt_status_t ret;

    LOG_DEBUG("%s", __func__);

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_MODEL)) {
        return BT_STATUS_SUCCESS;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_MODEL;
    msg.act = act;

    ret = btc_transfer_context(&msg, param,
                               sizeof(esp_ble_mesh_model_cb_param_t), btc_ble_mesh_model_copy_req_data);
    if (ret != BT_STATUS_SUCCESS) {
        LOG_ERROR("%s, btc_transfer_context failed", __func__);
    }
    return ret;
}

static void btc_ble_mesh_server_model_op_cb(struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        struct net_buf_simple *buf)
{
    esp_ble_mesh_model_cb_param_t mesh_param = {0};

    mesh_param.model_operation.opcode = ctx->recv_op;
    mesh_param.model_operation.model = (esp_ble_mesh_model_t *)model;
    mesh_param.model_operation.ctx = (esp_ble_mesh_msg_ctx_t *)ctx;
    mesh_param.model_operation.length = buf->len;
    mesh_param.model_operation.msg = buf->data;

    btc_ble_mesh_model_callback(&mesh_param, ESP_BLE_MESH_MODEL_OPERATION_EVT);
    return;
}

static void btc_ble_mesh_client_model_op_cb(struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        struct net_buf_simple *buf)
{
    esp_ble_mesh_model_cb_param_t mesh_param = {0};
    bt_mesh_client_node_t *node = NULL;

    if (!model || !model->user_data || !ctx || !buf) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_client_model_lock();

    node = bt_mesh_is_client_recv_publish_msg(model, ctx, buf, false);
    if (node == NULL) {
        mesh_param.client_recv_publish_msg.opcode = ctx->recv_op;
        mesh_param.client_recv_publish_msg.model = (esp_ble_mesh_model_t *)model;
        mesh_param.client_recv_publish_msg.ctx = (esp_ble_mesh_msg_ctx_t *)ctx;
        mesh_param.client_recv_publish_msg.length = buf->len;
        mesh_param.client_recv_publish_msg.msg = buf->data;
        btc_ble_mesh_model_callback(&mesh_param, ESP_BLE_MESH_CLIENT_MODEL_RECV_PUBLISH_MSG_EVT);
    } else {
        mesh_param.model_operation.opcode = ctx->recv_op;
        mesh_param.model_operation.model = (esp_ble_mesh_model_t *)model;
        mesh_param.model_operation.ctx = (esp_ble_mesh_msg_ctx_t *)ctx;
        mesh_param.model_operation.length = buf->len;
        mesh_param.model_operation.msg = buf->data;
        if (!k_delayed_work_free(&node->timer)) {
            btc_ble_mesh_model_callback(&mesh_param, ESP_BLE_MESH_MODEL_OPERATION_EVT);
            // Don't forget to release the node at the end.
            bt_mesh_client_free_node(node);
        }
    }

    bt_mesh_client_model_unlock();
    return;
}

static void btc_ble_mesh_client_model_timeout_cb(struct k_work *work)
{
    esp_ble_mesh_model_cb_param_t mesh_param = {0};
    struct k_delayed_work *timer = NULL;
    bt_mesh_client_node_t *node = NULL;

    bt_mesh_client_model_lock();

    timer = CONTAINER_OF(work, struct k_delayed_work, work);

    if (timer && !k_delayed_work_free(timer)) {
        node = CONTAINER_OF(work, bt_mesh_client_node_t, timer.work);
        if (node) {
            mesh_param.client_send_timeout.opcode = node->opcode;
            mesh_param.client_send_timeout.model = (esp_ble_mesh_model_t *)node->ctx.model;
            mesh_param.client_send_timeout.ctx = (esp_ble_mesh_msg_ctx_t *)&node->ctx;
            btc_ble_mesh_model_callback(&mesh_param, ESP_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT);
            // Don't forget to release the node at the end.
            bt_mesh_client_free_node(node);
        }
    }

    bt_mesh_client_model_unlock();
    return;
}

static void btc_ble_mesh_model_send_comp_cb(esp_ble_mesh_model_t *model, esp_ble_mesh_msg_ctx_t *ctx, u32_t opcode, int err)
{
    esp_ble_mesh_model_cb_param_t mesh_param = {0};

    mesh_param.model_send_comp.err_code = err;
    mesh_param.model_send_comp.opcode = opcode;
    mesh_param.model_send_comp.model = model;
    mesh_param.model_send_comp.ctx = ctx;

    btc_ble_mesh_model_callback(&mesh_param, ESP_BLE_MESH_MODEL_SEND_COMP_EVT);
    return;
}

static void btc_ble_mesh_model_publish_comp_cb(esp_ble_mesh_model_t *model, int err)
{
    esp_ble_mesh_model_cb_param_t mesh_param = {0};

    mesh_param.model_publish_comp.err_code = err;
    mesh_param.model_publish_comp.model = model;

    btc_ble_mesh_model_callback(&mesh_param, ESP_BLE_MESH_MODEL_PUBLISH_COMP_EVT);
    return;
}

static int btc_ble_mesh_model_publish_update(struct bt_mesh_model *mod)
{
    esp_ble_mesh_model_cb_param_t mesh_param = {0};
    bt_status_t ret;

    LOG_DEBUG("%s", __func__);

    mesh_param.model_publish_update.model = (esp_ble_mesh_model_t *)mod;

    ret = btc_ble_mesh_model_callback(&mesh_param, ESP_BLE_MESH_MODEL_PUBLISH_UPDATE_EVT);
    return (ret == BT_STATUS_SUCCESS) ? 0 : -1;
}

static void btc_ble_mesh_server_model_update_state_comp_cb(esp_ble_mesh_model_t *model,
        esp_ble_mesh_server_state_type_t type, int err)
{
    esp_ble_mesh_model_cb_param_t mesh_param = {0};

    mesh_param.server_model_update_state.err_code = err;
    mesh_param.server_model_update_state.model = model;
    mesh_param.server_model_update_state.type = type;

    btc_ble_mesh_model_callback(&mesh_param, ESP_BLE_MESH_SERVER_MODEL_UPDATE_STATE_COMP_EVT);
    return;
}

static bt_status_t btc_ble_mesh_prov_callback(esp_ble_mesh_prov_cb_param_t *param, uint8_t act)
{
    btc_msg_t msg = {0};
    bt_status_t ret;

    LOG_DEBUG("%s", __func__);

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_PROV)) {
        return BT_STATUS_SUCCESS;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_PROV;
    msg.act = act;

    ret = btc_transfer_context(&msg, param, sizeof(esp_ble_mesh_prov_cb_param_t), NULL);
    if (ret != BT_STATUS_SUCCESS) {
        LOG_ERROR("%s, btc_transfer_context failed", __func__);
    }
    return ret;
}

#if CONFIG_BLE_MESH_NODE
static void btc_ble_mesh_oob_pub_key_cb(void)
{
    LOG_DEBUG("%s", __func__);

    btc_ble_mesh_prov_callback(NULL, ESP_BLE_MESH_NODE_PROV_OOB_PUB_KEY_EVT);
    return;
}

static int btc_ble_mesh_output_number_cb(bt_mesh_output_action_t act, u32_t num)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    bt_status_t ret;

    LOG_DEBUG("%s", __func__);

    mesh_param.node_prov_output_num.action = (esp_ble_mesh_output_action_t)act;
    mesh_param.node_prov_output_num.number = num;

    ret = btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_NODE_PROV_OUTPUT_NUMBER_EVT);
    return (ret == BT_STATUS_SUCCESS) ? 0 : -1;
}

static int btc_ble_mesh_output_string_cb(const char *str)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    bt_status_t ret;

    LOG_DEBUG("%s", __func__);

    strncpy(mesh_param.node_prov_output_str.string, str, strlen(str));

    ret = btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_NODE_PROV_OUTPUT_STRING_EVT);
    return (ret == BT_STATUS_SUCCESS) ? 0 : -1;
}

static int btc_ble_mesh_input_cb(bt_mesh_input_action_t act, u8_t size)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    bt_status_t ret;

    LOG_DEBUG("%s", __func__);

    mesh_param.node_prov_input.action = (esp_ble_mesh_input_action_t)act;
    mesh_param.node_prov_input.size = size;

    ret = btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_NODE_PROV_INPUT_EVT);
    return (ret == BT_STATUS_SUCCESS) ? 0 : -1;
}

static void btc_ble_mesh_link_open_cb(bt_mesh_prov_bearer_t bearer)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    LOG_DEBUG("%s", __func__);

    mesh_param.node_prov_link_open.bearer = (esp_ble_mesh_prov_bearer_t)bearer;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_NODE_PROV_LINK_OPEN_EVT);
    return;
}

static void btc_ble_mesh_link_close_cb(bt_mesh_prov_bearer_t bearer)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    LOG_DEBUG("%s", __func__);

    mesh_param.node_prov_link_close.bearer = (esp_ble_mesh_prov_bearer_t)bearer;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT);
    return;
}

static void btc_ble_mesh_complete_cb(u16_t net_idx, const u8_t net_key[16], u16_t addr, u8_t flags, u32_t iv_index)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    LOG_DEBUG("%s", __func__);

    mesh_param.node_prov_complete.net_idx = net_idx;
    memcpy(mesh_param.node_prov_complete.net_key, net_key, 16);
    mesh_param.node_prov_complete.addr = addr;
    mesh_param.node_prov_complete.flags = flags;
    mesh_param.node_prov_complete.iv_index = iv_index;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_NODE_PROV_COMPLETE_EVT);
    return;
}

static void btc_ble_mesh_reset_cb(void)
{
    LOG_DEBUG("%s", __func__);

    btc_ble_mesh_prov_callback(NULL, ESP_BLE_MESH_NODE_PROV_RESET_EVT);
    return;
}
#endif /* CONFIG_BLE_MESH_NODE */

static void btc_ble_mesh_prov_register_complete_cb(int err_code)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    LOG_DEBUG("%s", __func__);

    mesh_param.prov_register_comp.err_code = err_code;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROV_REGISTER_COMP_EVT);
    return;
}

static void btc_ble_mesh_prov_set_complete_cb(esp_ble_mesh_prov_cb_param_t *param, uint8_t act)
{
    LOG_DEBUG("%s", __func__);

    btc_ble_mesh_prov_callback(param, act);
    return;
}

#if CONFIG_BLE_MESH_PROVISIONER
static void btc_ble_mesh_provisioner_recv_unprov_adv_pkt_cb(
    const u8_t addr[6], const u8_t addr_type,
    const u8_t adv_type, const u8_t dev_uuid[16],
    u16_t oob_info, bt_mesh_prov_bearer_t bearer)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    LOG_DEBUG("%s", __func__);

    if (addr == NULL || dev_uuid == NULL ||
            (bearer != BLE_MESH_PROV_ADV && bearer != BLE_MESH_PROV_GATT)) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    memcpy(mesh_param.provisioner_recv_unprov_adv_pkt.dev_uuid, dev_uuid, 16);
    memcpy(mesh_param.provisioner_recv_unprov_adv_pkt.addr, addr, BLE_MESH_ADDR_LEN);
    mesh_param.provisioner_recv_unprov_adv_pkt.addr_type = addr_type;
    mesh_param.provisioner_recv_unprov_adv_pkt.oob_info = oob_info;
    mesh_param.provisioner_recv_unprov_adv_pkt.adv_type = adv_type;
    mesh_param.provisioner_recv_unprov_adv_pkt.bearer = bearer;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROVISIONER_RECV_UNPROV_ADV_PKT_EVT);
    return;
}

static int btc_ble_mesh_provisioner_prov_read_oob_pub_key_cb(u8_t link_idx)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    bt_status_t ret;

    LOG_DEBUG("%s", __func__);

    mesh_param.provisioner_prov_read_oob_pub_key.link_idx = link_idx;

    ret = btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROVISIONER_PROV_READ_OOB_PUB_KEY_EVT);
    return (ret == BT_STATUS_SUCCESS) ? 0 : -1;
}

static int btc_ble_mesh_provisioner_prov_input_cb(u8_t method,
        bt_mesh_output_action_t act, u8_t size, u8_t link_idx)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    bt_status_t ret;

    LOG_DEBUG("%s", __func__);

    mesh_param.provisioner_prov_input.method = (esp_ble_mesh_oob_method_t)method;
    mesh_param.provisioner_prov_input.action = (esp_ble_mesh_output_action_t)act;
    mesh_param.provisioner_prov_input.size = size;
    mesh_param.provisioner_prov_input.link_idx = link_idx;

    ret = btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROVISIONER_PROV_INPUT_EVT);
    return (ret == BT_STATUS_SUCCESS) ? 0 : -1;
}

static int btc_ble_mesh_provisioner_prov_output_cb(u8_t method,
        bt_mesh_input_action_t act, void *data, u8_t size, u8_t link_idx)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    bt_status_t ret;

    LOG_DEBUG("%s", __func__);

    mesh_param.provisioner_prov_output.method = (esp_ble_mesh_oob_method_t)method;
    mesh_param.provisioner_prov_output.action = (esp_ble_mesh_input_action_t)act;
    mesh_param.provisioner_prov_output.size = size;
    mesh_param.provisioner_prov_output.link_idx = link_idx;
    if (act == BLE_MESH_ENTER_STRING) {
        strncpy(mesh_param.provisioner_prov_output.string, (char *)data, size);
    } else {
        mesh_param.provisioner_prov_output.number = sys_get_le32((u8_t *)data);
    }

    ret = btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROVISIONER_PROV_OUTPUT_EVT);
    return (ret == BT_STATUS_SUCCESS) ? 0 : -1;
}

static void btc_ble_mesh_provisioner_link_open_cb(bt_mesh_prov_bearer_t bearer)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    LOG_DEBUG("%s", __func__);

    mesh_param.provisioner_prov_link_open.bearer = (esp_ble_mesh_prov_bearer_t)bearer;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROVISIONER_PROV_LINK_OPEN_EVT);
    return;
}

static void btc_ble_mesh_provisioner_link_close_cb(bt_mesh_prov_bearer_t bearer, u8_t reason)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    LOG_DEBUG("%s", __func__);

    mesh_param.provisioner_prov_link_close.bearer = (esp_ble_mesh_prov_bearer_t)bearer;
    mesh_param.provisioner_prov_link_close.reason = reason;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROVISIONER_PROV_LINK_CLOSE_EVT);
    return;
}

static void btc_ble_mesh_provisioner_prov_complete_cb(
    int node_idx, const u8_t device_uuid[16],
    u16_t unicast_addr, u8_t element_num,
    u16_t netkey_idx)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    LOG_DEBUG("%s", __func__);

    mesh_param.provisioner_prov_complete.node_idx = node_idx;
    mesh_param.provisioner_prov_complete.unicast_addr = unicast_addr;
    mesh_param.provisioner_prov_complete.element_num = element_num;
    mesh_param.provisioner_prov_complete.netkey_idx = netkey_idx;
    memcpy(mesh_param.provisioner_prov_complete.device_uuid, device_uuid, sizeof(esp_ble_mesh_octet16_t));

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROVISIONER_PROV_COMPLETE_EVT);
    return;
}
#endif /* CONFIG_BLE_MESH_PROVISIONER */

static void btc_ble_mesh_heartbeat_msg_recv_cb(u8_t hops, u16_t feature)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    LOG_DEBUG("%s", __func__);

    mesh_param.heartbeat_msg_recv.hops = hops;
    mesh_param.heartbeat_msg_recv.feature = feature;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_HEARTBEAT_MESSAGE_RECV_EVT);
    return;
}

#if CONFIG_BLE_MESH_NODE
#if CONFIG_BLE_MESH_LOW_POWER
static void btc_ble_mesh_lpn_cb(u16_t friend_addr, bool established)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    u8_t act;

    LOG_DEBUG("%s", __func__);

    if (established) {
        mesh_param.lpn_friendship_establish.friend_addr = friend_addr;
        act = ESP_BLE_MESH_LPN_FRIENDSHIP_ESTABLISH_EVT;
    } else {
        mesh_param.lpn_friendship_terminate.friend_addr = friend_addr;
        act = ESP_BLE_MESH_LPN_FRIENDSHIP_TERMINATE_EVT;
    }

    btc_ble_mesh_prov_callback(&mesh_param, act);
    return;
}
#endif /* CONFIG_BLE_MESH_LOW_POWER */

#if CONFIG_BLE_MESH_FRIEND
void btc_ble_mesh_friend_cb(bool establish, u16_t lpn_addr, u8_t reason)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    u8_t act;

    LOG_DEBUG("%s", __func__);

    if (!BLE_MESH_ADDR_IS_UNICAST(lpn_addr)) {
        LOG_ERROR("%s, Not a unicast address", __func__);
        return;
    }

    if (establish) {
        mesh_param.frnd_friendship_establish.lpn_addr = lpn_addr;
        act = ESP_BLE_MESH_FRIEND_FRIENDSHIP_ESTABLISH_EVT;
    } else {
        mesh_param.frnd_friendship_terminate.lpn_addr = lpn_addr;
        mesh_param.frnd_friendship_terminate.reason = reason;
        act = ESP_BLE_MESH_FRIEND_FRIENDSHIP_TERMINATE_EVT;
    }

    btc_ble_mesh_prov_callback(&mesh_param, act);
    return;
}
#endif /* CONFIG_BLE_MESH_FRIEND */
#endif /* CONFIG_BLE_MESH_NODE */

#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
static void btc_ble_mesh_proxy_client_adv_recv_cb(const bt_mesh_addr_t *addr,
        u8_t type, bt_mesh_proxy_adv_ctx_t *ctx)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    if (!addr || !ctx || type != BLE_MESH_PROXY_ADV_NET_ID) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    LOG_DEBUG("%s", __func__);

    mesh_param.proxy_client_recv_adv_pkt.addr_type = addr->type;
    memcpy(mesh_param.proxy_client_recv_adv_pkt.addr, addr->val, BD_ADDR_LEN);
    mesh_param.proxy_client_recv_adv_pkt.net_idx = ctx->net_id.net_idx;
    memcpy(mesh_param.proxy_client_recv_adv_pkt.net_id, ctx->net_id.net_id, 8);

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROXY_CLIENT_RECV_ADV_PKT_EVT);
    return;
}

static void btc_ble_mesh_proxy_client_connect_cb(const bt_mesh_addr_t *addr,
        u8_t conn_handle, u16_t net_idx)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    if (!addr || conn_handle >= BLE_MESH_MAX_CONN) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    LOG_DEBUG("%s", __func__);

    mesh_param.proxy_client_connected.addr_type = addr->type;
    memcpy(mesh_param.proxy_client_connected.addr, addr->val, BD_ADDR_LEN);
    mesh_param.proxy_client_connected.conn_handle = conn_handle;
    mesh_param.proxy_client_connected.net_idx = net_idx;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROXY_CLIENT_CONNECTED_EVT);
    return;
}

static void btc_ble_mesh_proxy_client_disconnect_cb(const bt_mesh_addr_t *addr,
        u8_t conn_handle, u16_t net_idx, u8_t reason)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    if (!addr || conn_handle >= BLE_MESH_MAX_CONN) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    LOG_DEBUG("%s", __func__);

    mesh_param.proxy_client_disconnected.addr_type = addr->type;
    memcpy(mesh_param.proxy_client_disconnected.addr, addr->val, BD_ADDR_LEN);
    mesh_param.proxy_client_disconnected.conn_handle = conn_handle;
    mesh_param.proxy_client_disconnected.net_idx = net_idx;
    mesh_param.proxy_client_disconnected.reason = reason;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROXY_CLIENT_DISCONNECTED_EVT);
    return;
}

static void btc_ble_mesh_proxy_client_filter_status_recv_cb(u8_t conn_handle,
        u16_t src, u16_t net_idx, u8_t filter_type, u16_t list_size)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    if (conn_handle >= BLE_MESH_MAX_CONN) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    LOG_DEBUG("%s", __func__);

    mesh_param.proxy_client_recv_filter_status.conn_handle = conn_handle;
    mesh_param.proxy_client_recv_filter_status.server_addr = src;
    mesh_param.proxy_client_recv_filter_status.net_idx = net_idx;
    mesh_param.proxy_client_recv_filter_status.filter_type = filter_type;
    mesh_param.proxy_client_recv_filter_status.list_size = list_size;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROXY_CLIENT_RECV_FILTER_STATUS_EVT);
    return;
}
#endif /* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */

int btc_ble_mesh_client_model_init(esp_ble_mesh_model_t *model)
{
    __ASSERT(model && model->op, "%s, Invalid parameter", __func__);
    esp_ble_mesh_model_op_t *op = model->op;
    while (op != NULL && op->opcode != 0) {
        op->param_cb = (esp_ble_mesh_cb_t)btc_ble_mesh_client_model_op_cb;
        op++;
    }
    return bt_mesh_client_init((struct bt_mesh_model *)model);
}

int32_t btc_ble_mesh_model_pub_period_get(esp_ble_mesh_model_t *mod)
{
    return bt_mesh_model_pub_period_get((struct bt_mesh_model *)mod);
}

uint16_t btc_ble_mesh_get_primary_addr(void)
{
    return bt_mesh_primary_addr();
}

uint16_t *btc_ble_mesh_model_find_group(esp_ble_mesh_model_t *mod, uint16_t addr)
{
    return bt_mesh_model_find_group((struct bt_mesh_model *)mod, addr);
}

esp_ble_mesh_elem_t *btc_ble_mesh_elem_find(u16_t addr)
{
    return (esp_ble_mesh_elem_t *)bt_mesh_elem_find(addr);
}

uint8_t btc_ble_mesh_elem_count(void)
{
    return bt_mesh_elem_count();
}

esp_ble_mesh_model_t *btc_ble_mesh_model_find_vnd(const esp_ble_mesh_elem_t *elem,
        uint16_t company, uint16_t id)
{
    return (esp_ble_mesh_model_t *)bt_mesh_model_find_vnd((struct bt_mesh_elem *)elem, company, id);
}

esp_ble_mesh_model_t *btc_ble_mesh_model_find(const esp_ble_mesh_elem_t *elem, uint16_t id)
{
    return (esp_ble_mesh_model_t *)bt_mesh_model_find((struct bt_mesh_elem *)elem, id);
}

const esp_ble_mesh_comp_t *btc_ble_mesh_comp_get(void)
{
    return (const esp_ble_mesh_comp_t *)bt_mesh_comp_get();
}

/* Configuration Models */
extern const struct bt_mesh_model_op bt_mesh_cfg_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_cfg_cli_op[];
/* Health Models */
extern const struct bt_mesh_model_op bt_mesh_health_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_health_cli_op[];
/* Generic Client Models */
extern const struct bt_mesh_model_op gen_onoff_cli_op[];
extern const struct bt_mesh_model_op gen_level_cli_op[];
extern const struct bt_mesh_model_op gen_def_trans_time_cli_op[];
extern const struct bt_mesh_model_op gen_power_onoff_cli_op[];
extern const struct bt_mesh_model_op gen_power_level_cli_op[];
extern const struct bt_mesh_model_op gen_battery_cli_op[];
extern const struct bt_mesh_model_op gen_location_cli_op[];
extern const struct bt_mesh_model_op gen_property_cli_op[];
/* Lighting Client Models */
extern const struct bt_mesh_model_op light_lightness_cli_op[];
extern const struct bt_mesh_model_op light_ctl_cli_op[];
extern const struct bt_mesh_model_op light_hsl_cli_op[];
extern const struct bt_mesh_model_op light_xyl_cli_op[];
extern const struct bt_mesh_model_op light_lc_cli_op[];
/* Sensor Client Models */
extern const struct bt_mesh_model_op sensor_cli_op[];
/* Time and Scenes Client Models */
extern const struct bt_mesh_model_op time_cli_op[];
extern const struct bt_mesh_model_op scene_cli_op[];
extern const struct bt_mesh_model_op scheduler_cli_op[];
/* Generic Server Models */
extern const struct bt_mesh_model_op gen_onoff_srv_op[];
extern const struct bt_mesh_model_op gen_level_srv_op[];
extern const struct bt_mesh_model_op gen_def_trans_time_srv_op[];
extern const struct bt_mesh_model_op gen_power_onoff_srv_op[];
extern const struct bt_mesh_model_op gen_power_onoff_setup_srv_op[];
extern const struct bt_mesh_model_op gen_power_level_srv_op[];
extern const struct bt_mesh_model_op gen_power_level_setup_srv_op[];
extern const struct bt_mesh_model_op gen_battery_srv_op[];
extern const struct bt_mesh_model_op gen_location_srv_op[];
extern const struct bt_mesh_model_op gen_location_setup_srv_op[];
extern const struct bt_mesh_model_op gen_user_prop_srv_op[];
extern const struct bt_mesh_model_op gen_admin_prop_srv_op[];
extern const struct bt_mesh_model_op gen_manu_prop_srv_op[];
extern const struct bt_mesh_model_op gen_client_prop_srv_op[];
/* Lighting Server Models */
extern const struct bt_mesh_model_op light_lightness_srv_op[];
extern const struct bt_mesh_model_op light_lightness_setup_srv_op[];
extern const struct bt_mesh_model_op light_ctl_srv_op[];
extern const struct bt_mesh_model_op light_ctl_setup_srv_op[];
extern const struct bt_mesh_model_op light_ctl_temp_srv_op[];
extern const struct bt_mesh_model_op light_hsl_srv_op[];
extern const struct bt_mesh_model_op light_hsl_hue_srv_op[];
extern const struct bt_mesh_model_op light_hsl_sat_srv_op[];
extern const struct bt_mesh_model_op light_hsl_setup_srv_op[];
extern const struct bt_mesh_model_op light_xyl_srv_op[];
extern const struct bt_mesh_model_op light_xyl_setup_srv_op[];
extern const struct bt_mesh_model_op light_lc_srv_op[];
extern const struct bt_mesh_model_op light_lc_setup_srv_op[];
/* Time and Scenes Server Models */
extern const struct bt_mesh_model_op time_srv_op[];
extern const struct bt_mesh_model_op time_setup_srv_op[];
extern const struct bt_mesh_model_op scene_srv_op[];
extern const struct bt_mesh_model_op scene_setup_srv_op[];
extern const struct bt_mesh_model_op scheduler_srv_op[];
extern const struct bt_mesh_model_op scheduler_setup_srv_op[];
/* Sensor Server Models */
extern const struct bt_mesh_model_op sensor_srv_op[];
extern const struct bt_mesh_model_op sensor_setup_srv_op[];

static void btc_ble_mesh_model_op_add(esp_ble_mesh_model_t *model)
{
    esp_ble_mesh_model_op_t *op = NULL;

    if (!model) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    /* For SIG client and server models, model->op will be NULL and initialized here.
     * For vendor models whose opcode is 3 bytes, model->op will be initialized here.
     */
    if ((model->op != NULL) && (model->op->opcode >= 0x10000)) {
        goto add_model_op;
    }

    switch (model->model_id) {
    case BLE_MESH_MODEL_ID_CFG_SRV: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_cfg_srv_op;
        struct bt_mesh_cfg_srv *srv = (struct bt_mesh_cfg_srv *)model->user_data;
        if (srv) {
            srv->hb_sub.func = btc_ble_mesh_heartbeat_msg_recv_cb;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_CFG_CLI: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_cfg_cli_op;
        bt_mesh_config_client_t *cli = (bt_mesh_config_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_config_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_HEALTH_SRV: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_health_srv_op;
        struct bt_mesh_health_srv *srv = (struct bt_mesh_health_srv *)model->user_data;
        if (srv) {
            srv->cb.fault_clear = btc_ble_mesh_health_server_fault_clear;
            srv->cb.fault_test = btc_ble_mesh_health_server_fault_test;
            srv->cb.attn_on = btc_ble_mesh_health_server_attention_on;
            srv->cb.attn_off = btc_ble_mesh_health_server_attention_off;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_HEALTH_CLI: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_health_cli_op;
        bt_mesh_health_client_t *cli = (bt_mesh_health_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_health_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_ONOFF_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)gen_onoff_cli_op);
        bt_mesh_gen_onoff_client_t *cli = (bt_mesh_gen_onoff_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_generic_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_LEVEL_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)gen_level_cli_op);
        bt_mesh_gen_level_client_t *cli = (bt_mesh_gen_level_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_generic_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)gen_def_trans_time_cli_op);
        bt_mesh_gen_def_trans_time_client_t *cli = (bt_mesh_gen_def_trans_time_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_generic_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)gen_power_onoff_cli_op);
        bt_mesh_gen_power_onoff_client_t *cli = (bt_mesh_gen_power_onoff_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_generic_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)gen_power_level_cli_op);
        bt_mesh_gen_power_level_client_t *cli = (bt_mesh_gen_power_level_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_generic_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_BATTERY_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)gen_battery_cli_op);
        bt_mesh_gen_battery_client_t *cli = (bt_mesh_gen_battery_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_generic_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_LOCATION_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)gen_location_cli_op);
        bt_mesh_gen_location_client_t *cli = (bt_mesh_gen_location_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_generic_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_PROP_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)gen_property_cli_op);
        bt_mesh_gen_property_client_t *cli = (bt_mesh_gen_property_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_generic_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)light_lightness_cli_op);
        bt_mesh_light_lightness_client_t *cli = (bt_mesh_light_lightness_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_lighting_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_CTL_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)light_ctl_cli_op);
        bt_mesh_light_ctl_client_t *cli = (bt_mesh_light_ctl_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_lighting_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_HSL_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)light_hsl_cli_op);
        bt_mesh_light_hsl_client_t *cli = (bt_mesh_light_hsl_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_lighting_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_XYL_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)light_xyl_cli_op);
        bt_mesh_light_xyl_client_t *cli = (bt_mesh_light_xyl_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_lighting_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_LC_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)light_lc_cli_op);
        bt_mesh_light_lc_client_t *cli = (bt_mesh_light_lc_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_lighting_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_SENSOR_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)sensor_cli_op);
        bt_mesh_sensor_client_t *cli = (bt_mesh_sensor_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_sensor_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_TIME_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)time_cli_op);
        bt_mesh_time_client_t *cli = (bt_mesh_time_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_time_scene_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_SCENE_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)scene_cli_op);
        bt_mesh_scene_client_t *cli = (bt_mesh_scene_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_time_scene_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_SCHEDULER_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)scheduler_cli_op);
        bt_mesh_scheduler_client_t *cli = (bt_mesh_scheduler_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_time_scene_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_ONOFF_SRV:
        model->op = (esp_ble_mesh_model_op_t *)gen_onoff_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_LEVEL_SRV:
        model->op = (esp_ble_mesh_model_op_t *)gen_level_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_SRV:
        model->op = (esp_ble_mesh_model_op_t *)gen_def_trans_time_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV:
        model->op = (esp_ble_mesh_model_op_t *)gen_power_onoff_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SETUP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)gen_power_onoff_setup_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV:
        model->op = (esp_ble_mesh_model_op_t *)gen_power_level_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SETUP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)gen_power_level_setup_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_BATTERY_SRV:
        model->op = (esp_ble_mesh_model_op_t *)gen_battery_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_LOCATION_SRV:
        model->op = (esp_ble_mesh_model_op_t *)gen_location_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_USER_PROP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)gen_user_prop_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_ADMIN_PROP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)gen_admin_prop_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_MANUFACTURER_PROP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)gen_manu_prop_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_CLIENT_PROP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)gen_client_prop_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_LOCATION_SETUP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)gen_location_setup_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV:
        model->op = (esp_ble_mesh_model_op_t *)light_lightness_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)light_lightness_setup_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_CTL_SRV:
        model->op = (esp_ble_mesh_model_op_t *)light_ctl_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)light_ctl_setup_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)light_ctl_temp_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_HSL_SRV:
        model->op = (esp_ble_mesh_model_op_t *)light_hsl_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_HSL_HUE_SRV:
        model->op = (esp_ble_mesh_model_op_t *)light_hsl_hue_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_HSL_SAT_SRV:
        model->op = (esp_ble_mesh_model_op_t *)light_hsl_sat_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)light_hsl_setup_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_XYL_SRV:
        model->op = (esp_ble_mesh_model_op_t *)light_xyl_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_XYL_SETUP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)light_xyl_setup_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_LC_SRV:
        model->op = (esp_ble_mesh_model_op_t *)light_lc_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_LC_SETUP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)light_lc_setup_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_TIME_SRV:
        model->op = (esp_ble_mesh_model_op_t *)time_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_TIME_SETUP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)time_setup_srv_op;
        if (model->pub) {
            /* Time Setup Server model does not support subscribing nor publishing. */
            LOG_ERROR("%s, Time Setup Server shall not support publication", __func__);
            return;
        }
        break;
    case BLE_MESH_MODEL_ID_SCENE_SRV:
        model->op = (esp_ble_mesh_model_op_t *)scene_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_SCENE_SETUP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)scene_setup_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_SCHEDULER_SRV:
        model->op = (esp_ble_mesh_model_op_t *)scheduler_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_SCHEDULER_SETUP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)scheduler_setup_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_SENSOR_SRV:
        model->op = (esp_ble_mesh_model_op_t *)sensor_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_SENSOR_SETUP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)sensor_setup_srv_op;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    default:
        goto add_model_op;
    }
    return;

add_model_op:
    if (model->pub) {
        model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
    }
    op = model->op;
    while (op != NULL && op->opcode != 0) {
        op->param_cb = (esp_ble_mesh_cb_t)btc_ble_mesh_server_model_op_cb;
        op++;
    }
    return;
}

void btc_ble_mesh_prov_call_handler(btc_msg_t *msg)
{
    esp_ble_mesh_prov_cb_param_t param = {0};
    btc_ble_mesh_prov_args_t *arg = NULL;
    uint8_t act;

    if (!msg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_prov_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_MESH_INIT: {
        int err_code;
        for (int i = 0; i < arg->mesh_init.comp->element_count; i++) {
            esp_ble_mesh_elem_t *elem = &arg->mesh_init.comp->elements[i];
            /* For SIG models */
            for (int j = 0; j < elem->sig_model_count; j++) {
                esp_ble_mesh_model_t *sig_model = &elem->sig_models[j];
                /* The opcode of sig model should be 1 or 2 bytes. */
                if (sig_model && sig_model->op && (sig_model->op->opcode >= 0x10000)) {
                    err_code = -EINVAL;
                    btc_ble_mesh_prov_register_complete_cb(err_code);
                    return;
                }
                btc_ble_mesh_model_op_add(sig_model);
            }
            /* For vendor models */
            for (int k = 0; k < elem->vnd_model_count; k++) {
                esp_ble_mesh_model_t *vnd_model = &elem->vnd_models[k];
                /* The opcode of vendor model should be 3 bytes. */
                if (vnd_model && vnd_model->op && vnd_model->op->opcode < 0x10000) {
                    err_code = -EINVAL;
                    btc_ble_mesh_prov_register_complete_cb(err_code);
                    return;
                }
                btc_ble_mesh_model_op_add(vnd_model);
            }
        }
#if CONFIG_BLE_MESH_NODE
        arg->mesh_init.prov->oob_pub_key_cb = (esp_ble_mesh_cb_t)btc_ble_mesh_oob_pub_key_cb;
        arg->mesh_init.prov->output_num_cb = (esp_ble_mesh_cb_t)btc_ble_mesh_output_number_cb;
        arg->mesh_init.prov->output_str_cb = (esp_ble_mesh_cb_t)btc_ble_mesh_output_string_cb;
        arg->mesh_init.prov->input_cb = (esp_ble_mesh_cb_t)btc_ble_mesh_input_cb;
        arg->mesh_init.prov->link_open_cb = (esp_ble_mesh_cb_t)btc_ble_mesh_link_open_cb;
        arg->mesh_init.prov->link_close_cb = (esp_ble_mesh_cb_t)btc_ble_mesh_link_close_cb;
        arg->mesh_init.prov->complete_cb = (esp_ble_mesh_cb_t)btc_ble_mesh_complete_cb;
        arg->mesh_init.prov->reset_cb = (esp_ble_mesh_cb_t)btc_ble_mesh_reset_cb;
#if CONFIG_BLE_MESH_LOW_POWER
        bt_mesh_lpn_set_cb(btc_ble_mesh_lpn_cb);
#endif /* CONFIG_BLE_MESH_LOW_POWER */
#if CONFIG_BLE_MESH_FRIEND
        bt_mesh_friend_set_cb(btc_ble_mesh_friend_cb);
#endif /* CONFIG_BLE_MESH_FRIEND */
#endif /* CONFIG_BLE_MESH_NODE */
#if CONFIG_BLE_MESH_PROVISIONER
        arg->mesh_init.prov->provisioner_prov_read_oob_pub_key = (esp_ble_mesh_cb_t)btc_ble_mesh_provisioner_prov_read_oob_pub_key_cb;
        arg->mesh_init.prov->provisioner_prov_input = (esp_ble_mesh_cb_t)btc_ble_mesh_provisioner_prov_input_cb;
        arg->mesh_init.prov->provisioner_prov_output = (esp_ble_mesh_cb_t)btc_ble_mesh_provisioner_prov_output_cb;
        arg->mesh_init.prov->provisioner_link_open = (esp_ble_mesh_cb_t)btc_ble_mesh_provisioner_link_open_cb;
        arg->mesh_init.prov->provisioner_link_close = (esp_ble_mesh_cb_t)btc_ble_mesh_provisioner_link_close_cb;
        arg->mesh_init.prov->provisioner_prov_comp = (esp_ble_mesh_cb_t)btc_ble_mesh_provisioner_prov_complete_cb;
        bt_mesh_prov_adv_pkt_cb_register(btc_ble_mesh_provisioner_recv_unprov_adv_pkt_cb);
#endif /* CONFIG_BLE_MESH_PROVISIONER */
#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
        bt_mesh_proxy_client_set_adv_recv_cb(btc_ble_mesh_proxy_client_adv_recv_cb);
        bt_mesh_proxy_client_set_conn_cb(btc_ble_mesh_proxy_client_connect_cb);
        bt_mesh_proxy_client_set_disconn_cb(btc_ble_mesh_proxy_client_disconnect_cb);
        bt_mesh_proxy_client_set_filter_status_cb(btc_ble_mesh_proxy_client_filter_status_recv_cb);
#endif /* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */
        err_code = bt_mesh_init((struct bt_mesh_prov *)arg->mesh_init.prov,
                                (struct bt_mesh_comp *)arg->mesh_init.comp);
        /* Give the semaphore when BLE Mesh initialization is finished. */
        xSemaphoreGive(arg->mesh_init.semaphore);
        btc_ble_mesh_prov_register_complete_cb(err_code);
        return;
    }
#if CONFIG_BLE_MESH_NODE
    case BTC_BLE_MESH_ACT_PROV_ENABLE:
        LOG_DEBUG("%s, BTC_BLE_MESH_ACT_PROV_ENABLE, bearers = %d", __func__, arg->node_prov_enable.bearers);
        act = ESP_BLE_MESH_NODE_PROV_ENABLE_COMP_EVT;
        param.node_prov_enable_comp.err_code = bt_mesh_prov_enable(arg->node_prov_enable.bearers);
        break;
    case BTC_BLE_MESH_ACT_PROV_DISABLE:
        LOG_DEBUG("%s, BTC_BLE_MESH_ACT_PROV_DISABLE, bearers = %d", __func__, arg->node_prov_disable.bearers);
        act = ESP_BLE_MESH_NODE_PROV_DISABLE_COMP_EVT;
        param.node_prov_disable_comp.err_code = bt_mesh_prov_disable(arg->node_prov_disable.bearers);
        break;
    case BTC_BLE_MESH_ACT_NODE_RESET:
        LOG_DEBUG("%s, BTC_BLE_MESH_ACT_NODE_RESET", __func__);
        bt_mesh_reset();
        return;
    case BTC_BLE_MESH_ACT_SET_OOB_PUB_KEY:
        act = ESP_BLE_MESH_NODE_PROV_SET_OOB_PUB_KEY_COMP_EVT;
        param.node_prov_set_oob_pub_key_comp.err_code =
            bt_mesh_set_oob_pub_key(arg->set_oob_pub_key.pub_key_x,
                                    arg->set_oob_pub_key.pub_key_y,
                                    arg->set_oob_pub_key.private_key);
        break;
    case BTC_BLE_MESH_ACT_INPUT_NUMBER:
        act = ESP_BLE_MESH_NODE_PROV_INPUT_NUMBER_COMP_EVT;
        param.node_prov_input_num_comp.err_code = bt_mesh_input_number(arg->input_number.number);
        break;
    case BTC_BLE_MESH_ACT_INPUT_STRING:
        act = ESP_BLE_MESH_NODE_PROV_INPUT_STRING_COMP_EVT;
        param.node_prov_input_str_comp.err_code = bt_mesh_input_string(arg->input_string.string);
        break;
    case BTC_BLE_MESH_ACT_SET_DEVICE_NAME:
        act = ESP_BLE_MESH_NODE_SET_UNPROV_DEV_NAME_COMP_EVT;
        param.node_set_unprov_dev_name_comp.err_code = bt_mesh_set_device_name(arg->set_device_name.name);
        break;
#if defined(CONFIG_BLE_MESH_GATT_PROXY_SERVER)
    case BTC_BLE_MESH_ACT_PROXY_IDENTITY_ENABLE:
        act = ESP_BLE_MESH_NODE_PROXY_IDENTITY_ENABLE_COMP_EVT;
        param.node_proxy_identity_enable_comp.err_code = bt_mesh_proxy_identity_enable();
        break;
    case BTC_BLE_MESH_ACT_PROXY_GATT_ENABLE:
        act = ESP_BLE_MESH_NODE_PROXY_GATT_ENABLE_COMP_EVT;
        param.node_proxy_gatt_enable_comp.err_code = bt_mesh_proxy_gatt_enable();
        break;
    case BTC_BLE_MESH_ACT_PROXY_GATT_DISABLE:
        act = ESP_BLE_MESH_NODE_PROXY_GATT_DISABLE_COMP_EVT;
        param.node_proxy_gatt_disable_comp.err_code = bt_mesh_proxy_gatt_disable();
        break;
#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */
#endif /* CONFIG_BLE_MESH_NODE */
#if CONFIG_BLE_MESH_PROVISIONER
    case BTC_BLE_MESH_ACT_PROVISIONER_READ_OOB_PUB_KEY:
        act = ESP_BLE_MESH_PROVISIONER_PROV_READ_OOB_PUB_KEY_COMP_EVT;
        param.provisioner_prov_read_oob_pub_key_comp.err_code =
            bt_mesh_prov_read_oob_pub_key(arg->provisioner_read_oob_pub_key.link_idx,
                                          arg->provisioner_read_oob_pub_key.pub_key_x,
                                          arg->provisioner_read_oob_pub_key.pub_key_y);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_INPUT_STR:
        act = ESP_BLE_MESH_PROVISIONER_PROV_INPUT_STRING_COMP_EVT;
        param.provisioner_prov_input_str_comp.err_code =
            bt_mesh_prov_set_oob_input_data(arg->provisioner_input_str.link_idx,
                                            (const u8_t *)&arg->provisioner_input_str.string, false);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_INPUT_NUM:
        act = ESP_BLE_MESH_PROVISIONER_PROV_INPUT_NUMBER_COMP_EVT;
        param.provisioner_prov_input_num_comp.err_code =
            bt_mesh_prov_set_oob_input_data(arg->provisioner_input_num.link_idx,
                                            (const u8_t *)&arg->provisioner_input_num.number, true);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_ENABLE:
        act = ESP_BLE_MESH_PROVISIONER_PROV_ENABLE_COMP_EVT;
        param.provisioner_prov_enable_comp.err_code =
            bt_mesh_provisioner_enable(arg->provisioner_enable.bearers);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_DISABLE:
        act = ESP_BLE_MESH_PROVISIONER_PROV_DISABLE_COMP_EVT;
        param.provisioner_prov_disable_comp.err_code =
            bt_mesh_provisioner_disable(arg->provisioner_disable.bearers);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_DEV_ADD: {
        struct bt_mesh_unprov_dev_add add_dev = {0};
        add_dev.addr_type = arg->provisioner_dev_add.add_dev.addr_type;
        add_dev.oob_info = arg->provisioner_dev_add.add_dev.oob_info;
        add_dev.bearer = arg->provisioner_dev_add.add_dev.bearer;
        memcpy(add_dev.addr, arg->provisioner_dev_add.add_dev.addr, 6);
        memcpy(add_dev.uuid, arg->provisioner_dev_add.add_dev.uuid, 16);
        act = ESP_BLE_MESH_PROVISIONER_ADD_UNPROV_DEV_COMP_EVT;
        param.provisioner_add_unprov_dev_comp.err_code =
            bt_mesh_provisioner_add_unprov_dev(&add_dev, arg->provisioner_dev_add.flags);
        break;
    }
    case BTC_BLE_MESH_ACT_PROVISIONER_DEV_DEL: {
        struct bt_mesh_device_delete del_dev = {0};
        if (arg->provisioner_dev_del.del_dev.flag & DEL_DEV_ADDR_FLAG) {
            del_dev.addr_type = arg->provisioner_dev_del.del_dev.addr_type;
            memcpy(del_dev.addr, arg->provisioner_dev_del.del_dev.addr, 6);
        } else if (arg->provisioner_dev_del.del_dev.flag & DEL_DEV_UUID_FLAG) {
            memcpy(del_dev.uuid, arg->provisioner_dev_del.del_dev.uuid, 16);
        }
        act = ESP_BLE_MESH_PROVISIONER_DELETE_DEV_COMP_EVT;
        param.provisioner_delete_dev_comp.err_code = bt_mesh_provisioner_delete_device(&del_dev);
        break;
    }
    case BTC_BLE_MESH_ACT_PROVISIONER_SET_DEV_UUID_MATCH:
        act = ESP_BLE_MESH_PROVISIONER_SET_DEV_UUID_MATCH_COMP_EVT;
        param.provisioner_set_dev_uuid_match_comp.err_code =
            bt_mesh_provisioner_set_dev_uuid_match(arg->set_dev_uuid_match.offset,
                    arg->set_dev_uuid_match.match_len,
                    arg->set_dev_uuid_match.match_val,
                    arg->set_dev_uuid_match.prov_after_match);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_SET_PROV_DATA_INFO: {
        struct bt_mesh_prov_data_info info = {0};
        info.flag = arg->set_prov_data_info.prov_data.flag;
        if (arg->set_prov_data_info.prov_data.flag & PROV_DATA_NET_IDX_FLAG) {
            info.net_idx = arg->set_prov_data_info.prov_data.net_idx;
        } else if (arg->set_prov_data_info.prov_data.flag & PROV_DATA_FLAGS_FLAG) {
            info.flags = arg->set_prov_data_info.prov_data.flags;
        } else if (arg->set_prov_data_info.prov_data.flag & PROV_DATA_IV_INDEX_FLAG) {
            info.iv_index = arg->set_prov_data_info.prov_data.iv_index;
        }
        act = ESP_BLE_MESH_PROVISIONER_SET_PROV_DATA_INFO_COMP_EVT;
        param.provisioner_set_prov_data_info_comp.err_code =
            bt_mesh_provisioner_set_prov_data_info(&info);
        break;
    }
    case BTC_BLE_MESH_ACT_PROVISIONER_SET_NODE_NAME:
        act = ESP_BLE_MESH_PROVISIONER_SET_NODE_NAME_COMP_EVT;
        param.provisioner_set_node_name_comp.err_code =
            bt_mesh_provisioner_set_node_name(arg->set_node_name.index, arg->set_node_name.name);
        if (param.provisioner_set_node_name_comp.err_code) {
            param.provisioner_set_node_name_comp.node_index = ESP_BLE_MESH_INVALID_NODE_INDEX;
        } else {
            param.provisioner_set_node_name_comp.node_index = arg->set_node_name.index;
        }
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_SET_LOCAL_APP_KEY: {
        const u8_t *app_key = NULL;
        const u8_t zero[16] = {0};
        if (memcmp(arg->add_local_app_key.app_key, zero, 16)) {
            app_key = arg->add_local_app_key.app_key;
        }
        act = ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT;
        param.provisioner_add_app_key_comp.err_code =
            bt_mesh_provisioner_local_app_key_add(app_key, arg->add_local_app_key.net_idx,
                    &arg->add_local_app_key.app_idx);
        if (param.provisioner_add_app_key_comp.err_code) {
            param.provisioner_add_app_key_comp.app_idx = ESP_BLE_MESH_KEY_UNUSED;
        } else {
            param.provisioner_add_app_key_comp.app_idx = arg->add_local_app_key.app_idx;
        }
        break;
    }
    case BTC_BLE_MESH_ACT_PROVISIONER_BIND_LOCAL_MOD_APP:
        act = ESP_BLE_MESH_PROVISIONER_BIND_APP_KEY_TO_MODEL_COMP_EVT;
        param.provisioner_bind_app_key_to_model_comp.err_code =
            bt_mesh_provisioner_bind_local_model_app_idx(arg->local_mod_app_bind.elem_addr,
                    arg->local_mod_app_bind.model_id,
                    arg->local_mod_app_bind.cid,
                    arg->local_mod_app_bind.app_idx);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_ADD_LOCAL_NET_KEY: {
        const u8_t *net_key = NULL;
        const u8_t zero[16] = {0};
        if (memcmp(arg->add_local_net_key.net_key, zero, 16)) {
            net_key = arg->add_local_net_key.net_key;
        }
        act = ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_NET_KEY_COMP_EVT;
        param.provisioner_add_net_key_comp.err_code =
            bt_mesh_provisioner_local_net_key_add(net_key, &arg->add_local_net_key.net_idx);
        if (param.provisioner_add_net_key_comp.err_code) {
            param.provisioner_add_net_key_comp.net_idx = ESP_BLE_MESH_KEY_UNUSED;
        } else {
            param.provisioner_add_net_key_comp.net_idx = arg->add_local_net_key.net_idx;
        }
        break;
    }
#endif /* CONFIG_BLE_MESH_PROVISIONER */
#if CONFIG_BLE_MESH_FAST_PROV
    case BTC_BLE_MESH_ACT_SET_FAST_PROV_INFO:
        act = ESP_BLE_MESH_SET_FAST_PROV_INFO_COMP_EVT;
        param.set_fast_prov_info_comp.status_unicast =
            bt_mesh_set_fast_prov_unicast_addr_range(arg->set_fast_prov_info.unicast_min,
                    arg->set_fast_prov_info.unicast_max);
        param.set_fast_prov_info_comp.status_net_idx =
            bt_mesh_set_fast_prov_net_idx(arg->set_fast_prov_info.net_idx);
        bt_mesh_set_fast_prov_flags_iv_index(arg->set_fast_prov_info.flags,
                                             arg->set_fast_prov_info.iv_index);
        param.set_fast_prov_info_comp.status_match =
            bt_mesh_provisioner_set_dev_uuid_match(arg->set_fast_prov_info.offset,
                    arg->set_fast_prov_info.match_len,
                    arg->set_fast_prov_info.match_val, false);
        break;
    case BTC_BLE_MESH_ACT_SET_FAST_PROV_ACTION:
        act = ESP_BLE_MESH_SET_FAST_PROV_ACTION_COMP_EVT;
        param.set_fast_prov_action_comp.status_action =
            bt_mesh_set_fast_prov_action(arg->set_fast_prov_action.action);
        break;
#endif /* CONFIG_BLE_MESH_FAST_PROV */
#if CONFIG_BLE_MESH_LOW_POWER
    case BTC_BLE_MESH_ACT_LPN_ENABLE:
        act = ESP_BLE_MESH_LPN_ENABLE_COMP_EVT;
        param.lpn_enable_comp.err_code = bt_mesh_lpn_set(true, false);
        break;
    case BTC_BLE_MESH_ACT_LPN_DISABLE:
        act = ESP_BLE_MESH_LPN_DISABLE_COMP_EVT;
        param.lpn_disable_comp.err_code = bt_mesh_lpn_set(false, arg->lpn_disable.force);
        break;
    case BTC_BLE_MESH_ACT_LPN_POLL:
        act = ESP_BLE_MESH_LPN_POLL_COMP_EVT;
        param.lpn_poll_comp.err_code = bt_mesh_lpn_poll();
        break;
#endif /* CONFIG_BLE_MESH_LOW_POWER */
#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    case BTC_BLE_MESH_ACT_PROXY_CLIENT_CONNECT:
        act = ESP_BLE_MESH_PROXY_CLIENT_CONNECT_COMP_EVT;
        memcpy(param.proxy_client_connect_comp.addr, arg->proxy_client_connect.addr, BD_ADDR_LEN);
        param.proxy_client_connect_comp.addr_type = arg->proxy_client_connect.addr_type;
        param.proxy_client_connect_comp.net_idx = arg->proxy_client_connect.net_idx;
        param.proxy_client_connect_comp.err_code =
            bt_mesh_proxy_client_connect(arg->proxy_client_connect.addr,
                                         arg->proxy_client_connect.addr_type,
                                         arg->proxy_client_connect.net_idx);
        break;
    case BTC_BLE_MESH_ACT_PROXY_CLIENT_DISCONNECT:
        act = ESP_BLE_MESH_PROXY_CLIENT_DISCONNECT_COMP_EVT;
        param.proxy_client_disconnect_comp.conn_handle = arg->proxy_client_disconnect.conn_handle;
        param.proxy_client_disconnect_comp.err_code =
            bt_mesh_proxy_client_disconnect(arg->proxy_client_disconnect.conn_handle);
        break;
    case BTC_BLE_MESH_ACT_PROXY_CLIENT_SET_FILTER_TYPE: {
        struct bt_mesh_proxy_cfg_pdu pdu = {
            .opcode = BLE_MESH_PROXY_CFG_FILTER_SET,
            .set.filter_type = arg->proxy_client_set_filter_type.filter_type,
        };
        act = ESP_BLE_MESH_PROXY_CLIENT_SET_FILTER_TYPE_COMP_EVT;
        param.proxy_client_set_filter_type_comp.conn_handle = arg->proxy_client_set_filter_type.conn_handle;
        param.proxy_client_set_filter_type_comp.net_idx = arg->proxy_client_set_filter_type.net_idx;
        param.proxy_client_set_filter_type_comp.err_code =
            bt_mesh_proxy_client_send_cfg(arg->proxy_client_set_filter_type.conn_handle,
                                          arg->proxy_client_set_filter_type.net_idx, &pdu);
        break;
    }
    case BTC_BLE_MESH_ACT_PROXY_CLIENT_ADD_FILTER_ADDR: {
        struct bt_mesh_proxy_cfg_pdu pdu = {
            .opcode = BLE_MESH_PROXY_CFG_FILTER_ADD,
            .add.addr = arg->proxy_client_add_filter_addr.addr,
            .add.addr_num = arg->proxy_client_add_filter_addr.addr_num,
        };
        act = ESP_BLE_MESH_PROXY_CLIENT_ADD_FILTER_ADDR_COMP_EVT;
        param.proxy_client_add_filter_addr_comp.conn_handle = arg->proxy_client_add_filter_addr.conn_handle;
        param.proxy_client_add_filter_addr_comp.net_idx = arg->proxy_client_add_filter_addr.net_idx;
        param.proxy_client_add_filter_addr_comp.err_code =
            bt_mesh_proxy_client_send_cfg(arg->proxy_client_add_filter_addr.conn_handle,
                                          arg->proxy_client_add_filter_addr.net_idx, &pdu);
        break;
    }
    case BTC_BLE_MESH_ACT_PROXY_CLIENT_REMOVE_FILTER_ADDR: {
        struct bt_mesh_proxy_cfg_pdu pdu = {
            .opcode = BLE_MESH_PROXY_CFG_FILTER_REMOVE,
            .remove.addr = arg->proxy_client_remove_filter_addr.addr,
            .remove.addr_num = arg->proxy_client_remove_filter_addr.addr_num,
        };
        act = ESP_BLE_MESH_PROXY_CLIENT_REMOVE_FILTER_ADDR_COMP_EVT;
        param.proxy_client_remove_filter_addr_comp.conn_handle = arg->proxy_client_remove_filter_addr.conn_handle;
        param.proxy_client_remove_filter_addr_comp.net_idx = arg->proxy_client_remove_filter_addr.net_idx;
        param.proxy_client_remove_filter_addr_comp.err_code =
            bt_mesh_proxy_client_send_cfg(arg->proxy_client_remove_filter_addr.conn_handle,
                                          arg->proxy_client_remove_filter_addr.net_idx, &pdu);
        break;
    }
#endif /* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */
    default:
        LOG_WARN("%s, Invalid msg->act %d", __func__, msg->act);
        return;
    }

    /* Callback operation completion events */
    btc_ble_mesh_prov_set_complete_cb(&param, act);

    if (msg->arg) {
        btc_ble_mesh_prov_arg_deep_free(msg);
    }
    return;
}

void btc_ble_mesh_prov_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_prov_cb_param_t *param = NULL;

    if (!msg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    param = (esp_ble_mesh_prov_cb_param_t *)(msg->arg);

    if (msg->act < ESP_BLE_MESH_PROV_EVT_MAX) {
        btc_ble_mesh_prov_cb_to_app(msg->act, param);
    } else {
        LOG_ERROR("%s, Unknown msg->act = %d", __func__, msg->act);
    }
}

void btc_ble_mesh_model_call_handler(btc_msg_t *msg)
{
    btc_ble_mesh_model_args_t *arg = NULL;
    int err;

    if (!msg || !msg->arg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_model_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_MODEL_PUBLISH: {
        if (arg->model_publish.device_role == PROVISIONER) {
            bt_mesh_role_param_t common = {0};
            common.model = (struct bt_mesh_model *)(arg->model_publish.model);
            common.role  = arg->model_publish.device_role;
            if (bt_mesh_set_client_model_role(&common)) {
                LOG_ERROR("%s, Failed to set model role", __func__);
                break;
            }
        }
        err = bt_mesh_model_publish((struct bt_mesh_model *)arg->model_publish.model);
        btc_ble_mesh_model_publish_comp_cb(arg->model_publish.model, err);
        break;
    }
    case BTC_BLE_MESH_ACT_SERVER_MODEL_SEND: {
        /* arg->model_send.length contains opcode & message, 8 is used for TransMIC */
        struct net_buf_simple *buf = bt_mesh_alloc_buf(arg->model_send.length + 8);
        if (!buf) {
            LOG_ERROR("%s, Failed to allocate memory", __func__);
            break;
        }
        net_buf_simple_add_mem(buf, arg->model_send.data, arg->model_send.length);
        arg->model_send.ctx->srv_send = true;
        err = bt_mesh_model_send((struct bt_mesh_model *)arg->model_send.model,
                                 (struct bt_mesh_msg_ctx *)arg->model_send.ctx,
                                 buf, NULL, NULL);
        bt_mesh_free_buf(buf);
        btc_ble_mesh_model_send_comp_cb(arg->model_send.model, arg->model_send.ctx,
                                        arg->model_send.opcode, err);
        break;
    }
    case BTC_BLE_MESH_ACT_CLIENT_MODEL_SEND: {
        bt_mesh_role_param_t common = {0};
        /* arg->model_send.length contains opcode & message, 8 is used for TransMIC */
        struct net_buf_simple *buf = bt_mesh_alloc_buf(arg->model_send.length + 8);
        if (!buf) {
            LOG_ERROR("%s, Failed to allocate memory", __func__);
            break;
        }
        net_buf_simple_add_mem(buf, arg->model_send.data, arg->model_send.length);
        arg->model_send.ctx->srv_send = false;
        common.model = (struct bt_mesh_model *)(arg->model_send.model);
        common.role  = arg->model_send.device_role;
        if (bt_mesh_set_client_model_role(&common)) {
            LOG_ERROR("%s, Failed to set model role", __func__);
            break;
        }
        err = bt_mesh_client_send_msg((struct bt_mesh_model *)arg->model_send.model,
                                      arg->model_send.opcode,
                                      (struct bt_mesh_msg_ctx *)arg->model_send.ctx, buf,
                                      btc_ble_mesh_client_model_timeout_cb, arg->model_send.msg_timeout,
                                      arg->model_send.need_rsp, NULL, NULL);
        bt_mesh_free_buf(buf);
        btc_ble_mesh_model_send_comp_cb(arg->model_send.model, arg->model_send.ctx,
                                        arg->model_send.opcode, err);
        break;
    }
    case BTC_BLE_MESH_ACT_SERVER_MODEL_UPDATE_STATE:
        err = bt_mesh_update_binding_state(
                  (struct bt_mesh_model *)arg->model_update_state.model, arg->model_update_state.type,
                  (bt_mesh_server_state_value_t *)arg->model_update_state.value);
        btc_ble_mesh_server_model_update_state_comp_cb(arg->model_update_state.model,
                arg->model_update_state.type, err);
        break;
    default:
        LOG_WARN("%s, Unknown msg->act %d", __func__, msg->act);
        break;
    }

    btc_ble_mesh_model_arg_deep_free(msg);
    return;
}

void btc_ble_mesh_model_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_model_cb_param_t *param = NULL;

    if (!msg || !msg->arg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    param = (esp_ble_mesh_model_cb_param_t *)(msg->arg);

    if (msg->act < ESP_BLE_MESH_MODEL_EVT_MAX) {
        btc_ble_mesh_model_cb_to_app(msg->act, param);
    } else {
        LOG_ERROR("%s, Unknown msg->act = %d", __func__, msg->act);
    }

    btc_ble_mesh_model_free_req_data(msg);
    return;
}

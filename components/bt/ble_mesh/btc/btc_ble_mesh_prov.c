// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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
#include "btc_ble_mesh_prov.h"

#include "sdkconfig.h"

#include "mesh.h"
#include "mesh_util.h"
#include "mesh_main.h"
#include "mesh_access.h"
#include "access.h"
#include "transport.h"
#include "proxy.h"
#include "include/proxy.h"

#include "cfg_cli.h"
#include "health_cli.h"
#include "generic_client.h"
#include "light_client.h"
#include "sensor_client.h"
#include "time_scene_client.h"
#include "provisioner_prov.h"
#include "bt_mesh_client_common.h"

#include "btc_ble_mesh_config_client.h"
#include "btc_ble_mesh_health.h"
#include "btc_ble_mesh_generic_client.h"
#include "btc_ble_mesh_time_scene_client.h"
#include "btc_ble_mesh_sensor_client.h"
#include "btc_ble_mesh_light_client.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_networking_api.h"

#if CONFIG_BT_MESH

#define BLE_MESH_MAX_DATA_SIZE 379

typedef struct {
    sys_slist_t queue;
} client_internal_data_t;

typedef struct {
    sys_snode_t client_node;
    u16_t tx_dst; /* Destination address of the sending message */
    u32_t op_pending; /* Indicate the status message waiting for */
    /** Publish Period Timer. Only for stack-internal use. */
    struct k_delayed_work timer;
} mesh_client_node_t;

static void btc_ble_mesh_server_model_op_cb(struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        struct net_buf_simple *buf);
static int btc_output_number_cb(bt_mesh_output_action_t act, u32_t num);
static int btc_output_string_cb(const char *str);
static int btc_input_cb(bt_mesh_input_action_t act, u8_t size);
static void btc_link_open_cb(bt_mesh_prov_bearer_t bearer);
static void btc_link_close_cb(bt_mesh_prov_bearer_t bearer);
static void btc_complete_cb(u16_t net_idx, u16_t addr);
static void btc_reset_cb(void);

static const btc_mesh_cb_t interface = {
    btc_ble_mesh_server_model_op_cb,
    btc_output_number_cb,
    btc_output_string_cb,
    btc_input_cb,
    btc_link_open_cb,
    btc_link_close_cb,
    btc_complete_cb,
    btc_reset_cb
};

void btc_ble_mesh_prov_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_ble_mesh_model_args_t *dst = (btc_ble_mesh_model_args_t *)p_dest;
    btc_ble_mesh_model_args_t *src = (btc_ble_mesh_model_args_t *)p_src;

    if (!msg || !dst || !src) {
        LOG_ERROR("%s: parameter is NULL", __func__);
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
                LOG_ERROR("%s %d no mem", __func__, msg->act);
            }
        }
        if (dst->model_send.ctx) {
            memcpy(dst->model_send.ctx, src->model_send.ctx, sizeof(esp_ble_mesh_msg_ctx_t));
        } else {
            LOG_ERROR("%s %d no mem", __func__, msg->act);
        }
        LOG_DEBUG("dst->model_send.data = %s", bt_hex(dst->model_send.data, src->model_send.length));
        break;
    }
    default:
        LOG_DEBUG("%s Unhandled deep copy %d", __func__, msg->act);
        break;
    }
}

void btc_ble_mesh_prov_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_mesh_model_args_t *arg = NULL;

    if (!msg || !msg->arg) {
        LOG_ERROR("%s: parameter is NULL", __func__);
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
    default:
        break;
    }

    return;
}

static void btc_ble_mesh_copy_req_data(btc_msg_t *msg, void *p_dest, void *p_src)
{
    esp_ble_mesh_model_cb_param_t *p_dest_data = (esp_ble_mesh_model_cb_param_t *)p_dest;
    esp_ble_mesh_model_cb_param_t *p_src_data = (esp_ble_mesh_model_cb_param_t *)p_src;

    if (!msg || !p_src_data || !p_dest_data) {
        LOG_ERROR("%s: parameter is NULL", __func__);
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
                LOG_ERROR("%s %d no mem", __func__, msg->act);
            }
            if (p_src_data->model_operation.length) {
                if (p_dest_data->model_operation.msg) {
                    memcpy(p_dest_data->model_operation.msg, p_src_data->model_operation.msg, p_src_data->model_operation.length);
                } else {
                    LOG_ERROR("%s %d no mem", __func__, msg->act);
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
                LOG_ERROR("%s %d no mem", __func__, msg->act);
            }
            if (p_src_data->client_recv_publish_msg.length) {
                if (p_dest_data->client_recv_publish_msg.msg) {
                    memcpy(p_dest_data->client_recv_publish_msg.msg, p_src_data->client_recv_publish_msg.msg, p_src_data->client_recv_publish_msg.length);
                } else {
                    LOG_ERROR("%s %d no mem", __func__, msg->act);
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
                LOG_ERROR("%s %d no mem", __func__, msg->act);
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
                LOG_ERROR("%s %d no mem", __func__, msg->act);
            }
        }
        break;
    }
    default:
        break;
    }
}

static void btc_ble_mesh_free_req_data(btc_msg_t *msg)
{
    esp_ble_mesh_model_cb_param_t *arg = NULL;

    if (!msg || !msg->arg) {
        LOG_ERROR("%s: parameter is NULL", __func__);
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

static inline void btc_ble_mesh_cb_to_app(esp_ble_mesh_prov_cb_event_t event,
        esp_ble_mesh_prov_cb_param_t *param)
{
    esp_mesh_prov_cb_t btc_mesh_cb = (esp_mesh_prov_cb_t)btc_profile_cb_get(BTC_PID_PROV);
    if (btc_mesh_cb) {
        btc_mesh_cb(event, param);
    }
}

static inline void btc_ble_mesh_model_cb_to_app(esp_ble_mesh_model_cb_event_t event,
        esp_ble_mesh_model_cb_param_t *param)
{
    esp_mesh_model_cb_t btc_mesh_cb = (esp_mesh_model_cb_t)btc_profile_cb_get(BTC_PID_MODEL);
    if (btc_mesh_cb) {
        btc_mesh_cb(event, param);
    }
}

extern u32_t mesh_opcode;

static void btc_ble_mesh_server_model_op_cb(struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        struct net_buf_simple *buf)
{
    btc_msg_t msg = {0};
    bt_status_t ret;

    esp_ble_mesh_model_cb_param_t mesh_param = {0};
    mesh_param.model_operation.opcode = mesh_opcode;
    mesh_param.model_operation.model = (esp_ble_mesh_model_t *)model;
    mesh_param.model_operation.ctx = (esp_ble_mesh_msg_ctx_t *)ctx;
    mesh_param.model_operation.length = buf->len;
    mesh_param.model_operation.msg = buf->data;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_MODEL;
    msg.act = ESP_BLE_MESH_MODEL_OPERATION_EVT;
    ret = btc_transfer_context(&msg, &mesh_param,
                               sizeof(esp_ble_mesh_model_cb_param_t), btc_ble_mesh_copy_req_data);

    if (ret != BT_STATUS_SUCCESS) {
        LOG_ERROR("%s btc_transfer_context failed\n", __func__);
    }
    return;
}

static void btc_ble_mesh_client_model_op_cb(struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        struct net_buf_simple *buf)
{
    btc_msg_t msg = {0};
    bt_status_t ret;
    esp_ble_mesh_model_cb_param_t mesh_param = {0};
    bt_mesh_client_common_t *client_op = NULL;
    bt_mesh_internal_data_t *data = NULL;
    bt_mesh_client_node_t *node = NULL;

    if (!model || !model->user_data || !ctx || !buf) {
        LOG_ERROR("%s: parameter is NULL", __func__);
        return;
    }

    client_op = (bt_mesh_client_common_t *)model->user_data;
    data      = (bt_mesh_internal_data_t *)client_op->internal_data;

    node = bt_mesh_is_model_message_publish(model, ctx, buf, false);
    if (node == NULL) {
        msg.act = ESP_BLE_MESH_CLIENT_MODEL_RECV_PUBLISH_MSG_EVT;
        mesh_param.client_recv_publish_msg.opcode = mesh_opcode;
        mesh_param.client_recv_publish_msg.model = (esp_ble_mesh_model_t *)model;
        mesh_param.client_recv_publish_msg.ctx = (esp_ble_mesh_msg_ctx_t *)ctx;
        mesh_param.client_recv_publish_msg.length = buf->len;
        mesh_param.client_recv_publish_msg.msg = buf->data;
    } else {
        msg.act = ESP_BLE_MESH_MODEL_OPERATION_EVT;
        mesh_param.model_operation.opcode = mesh_opcode;
        mesh_param.model_operation.model = (esp_ble_mesh_model_t *)model;
        mesh_param.model_operation.ctx = (esp_ble_mesh_msg_ctx_t *)ctx;
        mesh_param.model_operation.length = buf->len;
        mesh_param.model_operation.msg = buf->data;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_MODEL;
    if (msg.act != ESP_BLE_MESH_CLIENT_MODEL_RECV_PUBLISH_MSG_EVT) {
        // Don't forget to release the node at the end.
        bt_mesh_client_free_node(&data->queue, node);
    }
    ret = btc_transfer_context(&msg, &mesh_param,
                               sizeof(esp_ble_mesh_model_cb_param_t), btc_ble_mesh_copy_req_data);

    if (ret != BT_STATUS_SUCCESS) {
        LOG_ERROR("%s btc_transfer_context failed\n", __func__);
    }
    return;
}

static void btc_ble_mesh_model_send_comp_cb(esp_ble_mesh_model_t *model, esp_ble_mesh_msg_ctx_t *ctx, int err)
{
    btc_msg_t msg = {0};
    bt_status_t ret;

    esp_ble_mesh_model_cb_param_t mesh_param = {0};
    mesh_param.model_send_comp.err_code = err;
    mesh_param.model_send_comp.model = model;
    mesh_param.model_send_comp.ctx   = ctx;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_MODEL;
    msg.act = ESP_BLE_MESH_MODEL_SEND_COMP_EVT;
    ret = btc_transfer_context(&msg, &mesh_param,
                               sizeof(esp_ble_mesh_model_cb_param_t), btc_ble_mesh_copy_req_data);

    if (ret != BT_STATUS_SUCCESS) {
        LOG_ERROR("%s btc_transfer_context failed\n", __func__);
    }
    return;
}

static void btc_ble_mesh_model_publish_comp_cb(esp_ble_mesh_model_t *model, int err)
{
    btc_msg_t msg = {0};
    bt_status_t ret;

    esp_ble_mesh_model_cb_param_t mesh_param = {0};
    mesh_param.model_publish_comp.err_code = err;
    mesh_param.model_publish_comp.model = model;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_MODEL;
    msg.act = ESP_BLE_MESH_MODEL_PUBLISH_COMP_EVT;
    ret = btc_transfer_context(&msg, &mesh_param,
                               sizeof(esp_ble_mesh_model_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        LOG_ERROR("%s btc_transfer_context failed\n", __func__);
    }
    return;
}

static int btc_output_number_cb(bt_mesh_output_action_t act, u32_t num)
{
    LOG_DEBUG("%s", __func__);
    btc_msg_t msg = {0};
    bt_status_t ret;
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    mesh_param.node_prov_output_num.action = (esp_ble_mesh_output_action_t)act;
    mesh_param.node_prov_output_num.number = num;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_PROV;
    msg.act = ESP_BLE_MESH_NODE_PROV_OUTPUT_NUMBER_EVT;
    ret = btc_transfer_context(&msg, &mesh_param,
                               sizeof(esp_ble_mesh_prov_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        LOG_ERROR("%s btc_transfer_context failed\n", __func__);
    }

    return 0;
}

static int btc_output_string_cb(const char *str)
{
    LOG_DEBUG("%s", __func__);
    btc_msg_t msg = {0};
    bt_status_t ret;
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    mesh_param.node_prov_output_str.str = (char *)str;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_PROV;
    msg.act = ESP_BLE_MESH_NODE_PROV_OUTPUT_STRING_EVT;
    ret = btc_transfer_context(&msg, &mesh_param,
                               sizeof(esp_ble_mesh_prov_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        LOG_ERROR("%s btc_transfer_context failed\n", __func__);
    }

    return 0;
}

static int btc_input_cb(bt_mesh_input_action_t act, u8_t size)
{
    LOG_DEBUG("%s", __func__);
    btc_msg_t msg = {0};
    bt_status_t ret;
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    mesh_param.node_prov_input.action = (esp_ble_mesh_input_action_t)act;
    mesh_param.node_prov_input.size = size;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_PROV;
    msg.act = ESP_BLE_MESH_NODE_PROV_INPUT_EVT;
    ret = btc_transfer_context(&msg, &mesh_param,
                               sizeof(esp_ble_mesh_prov_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        LOG_ERROR("%s btc_transfer_context failed\n", __func__);
    }

    return 0;
}

static void btc_link_open_cb(bt_mesh_prov_bearer_t bearer)
{
    LOG_DEBUG("%s", __func__);
    btc_msg_t msg = {0};
    bt_status_t ret;
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    mesh_param.node_prov_link_open.bearer = (esp_ble_mesh_prov_bearer_t)bearer;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_PROV;
    msg.act = ESP_BLE_MESH_NODE_PROV_LINK_OPEN_EVT;
    ret = btc_transfer_context(&msg, &mesh_param,
                               sizeof(esp_ble_mesh_prov_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        LOG_ERROR("%s btc_transfer_context failed\n", __func__);
    }

    return;
}

static void btc_link_close_cb(bt_mesh_prov_bearer_t bearer)
{
    LOG_DEBUG("%s", __func__);
    btc_msg_t msg = {0};
    bt_status_t ret;
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    mesh_param.node_prov_link_close.bearer = (esp_ble_mesh_prov_bearer_t)bearer;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_PROV;
    msg.act = ESP_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT;
    ret = btc_transfer_context(&msg, &mesh_param,
                               sizeof(esp_ble_mesh_prov_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        LOG_ERROR("%s btc_transfer_context failed\n", __func__);
    }

    return;
}

static void btc_complete_cb(u16_t net_idx, u16_t addr)
{
    LOG_DEBUG("%s", __func__);
    btc_msg_t msg = {0};
    bt_status_t ret;

    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    mesh_param.node_prov_complete.net_idx = net_idx;
    mesh_param.node_prov_complete.addr = addr;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_PROV;
    msg.act = ESP_BLE_MESH_NODE_PROV_COMPLETE_EVT;
    ret = btc_transfer_context(&msg, &mesh_param,
                               sizeof(esp_ble_mesh_prov_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        LOG_ERROR("%s btc_transfer_context failed\n", __func__);
    }

    return;
}

static void btc_prov_register_complete_cb(int err_code)
{
    LOG_DEBUG("%s", __func__);
    btc_msg_t msg = {0};
    bt_status_t ret;

    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    mesh_param.prov_register_comp.err_code = err_code;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_PROV;
    msg.act = ESP_BLE_MESH_PROV_REGISTER_COMP_EVT;
    ret = btc_transfer_context(&msg, &mesh_param,
                               sizeof(esp_ble_mesh_prov_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        LOG_ERROR("%s btc_transfer_context failed\n", __func__);
    }

    return;
}

static void btc_client_model_timeout_cb(struct k_work *work)
{
    bt_mesh_client_node_t *node = CONTAINER_OF(work,
                                  bt_mesh_client_node_t,
                                  timer.work);
    __ASSERT(node, "unknown mesh client node.");
    btc_msg_t msg = {0};
    bt_status_t ret;
    bt_mesh_internal_data_t *data = NULL;
    bt_mesh_client_common_t *client_op = node->ctx.model->user_data;
    data = (bt_mesh_internal_data_t *)client_op->internal_data;
    __ASSERT(client_op && data, "Invalid client user data or internal data.");
    esp_ble_mesh_model_cb_param_t mesh_param = {0};
    mesh_param.client_send_timeout.opcode = node->opcode;
    mesh_param.client_send_timeout.model = (esp_ble_mesh_model_t *)node->ctx.model;
    mesh_param.client_send_timeout.ctx   = (esp_ble_mesh_msg_ctx_t *)&node->ctx;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_MODEL;
    msg.act = ESP_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT;
    ret = btc_transfer_context(&msg, &mesh_param,
                               sizeof(esp_ble_mesh_model_cb_param_t), btc_ble_mesh_copy_req_data);

    if (ret != BT_STATUS_SUCCESS) {
        LOG_ERROR("%s btc_transfer_context failed\n", __func__);
    }
    // Don't forget to release the node at the end.
    bt_mesh_client_free_node(&data->queue, node);
    return;
}

static void btc_model_publish_update(struct bt_mesh_model *mod)
{
    LOG_DEBUG("%s", __func__);
    btc_msg_t msg = {0};
    bt_status_t ret;

    esp_ble_mesh_model_cb_param_t mesh_param = {0};
    mesh_param.model_publish_update.model = (esp_ble_mesh_model_t *)mod;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_MODEL;
    msg.act = ESP_BLE_MESH_MODEL_PUBLISH_UPDATE_EVT;
    ret = btc_transfer_context(&msg, &mesh_param,
                               sizeof(esp_ble_mesh_model_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        LOG_ERROR("%s btc_transfer_context failed\n", __func__);
    }

    return;
}

static void btc_prov_set_complete_cb(esp_ble_mesh_prov_cb_param_t *param, uint8_t act)
{
    LOG_DEBUG("%s", __func__);
    btc_msg_t msg = {0};
    bt_status_t ret;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_PROV;
    msg.act = act;
    ret = btc_transfer_context(&msg, param,
                               sizeof(esp_ble_mesh_prov_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        LOG_ERROR("%s btc_transfer_context failed\n", __func__);
    }

    return;
}

#if (CONFIG_BT_MESH_PROVISIONER)
static void btc_provisioner_link_open_cb(bt_mesh_prov_bearer_t bearer)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    btc_msg_t msg = {0};
    bt_status_t ret;

    LOG_DEBUG("%s", __func__);

    mesh_param.provisioner_prov_link_open.bearer = (esp_ble_mesh_prov_bearer_t)bearer;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_PROV;
    msg.act = ESP_BLE_MESH_PROVISIONER_PROV_LINK_OPEN_EVT;
    ret = btc_transfer_context(&msg, &mesh_param,
                               sizeof(esp_ble_mesh_prov_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        LOG_ERROR("%s btc_transfer_context failed\n", __func__);
    }

    return;
}

static void btc_provisioner_link_close_cb(bt_mesh_prov_bearer_t bearer, u8_t reason)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    btc_msg_t msg = {0};
    bt_status_t ret;

    LOG_DEBUG("%s", __func__);

    mesh_param.provisioner_prov_link_close.bearer = (esp_ble_mesh_prov_bearer_t)bearer;
    mesh_param.provisioner_prov_link_close.reason = reason;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_PROV;
    msg.act = ESP_BLE_MESH_PROVISIONER_PROV_LINK_CLOSE_EVT;
    ret = btc_transfer_context(&msg, &mesh_param,
                               sizeof(esp_ble_mesh_prov_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        LOG_ERROR("%s btc_transfer_context failed\n", __func__);
    }

    return;
}

static void btc_provisioner_prov_complete_cb(int node_idx, const u8_t device_uuid[16],
        u16_t unicast_addr, u8_t element_num,
        u16_t netkey_idx)
{
    LOG_DEBUG("%s", __func__);
    btc_msg_t msg = {0};
    bt_status_t ret;

    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    mesh_param.provisioner_prov_complete.node_idx = node_idx;
    mesh_param.provisioner_prov_complete.unicast_addr = unicast_addr;
    mesh_param.provisioner_prov_complete.element_num = element_num;
    mesh_param.provisioner_prov_complete.netkey_idx = netkey_idx;
    memcpy(mesh_param.provisioner_prov_complete.device_uuid, device_uuid, sizeof(esp_ble_mesh_octet16_t));
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_PROV;
    msg.act = ESP_BLE_MESH_PROVISIONER_PROV_COMPLETE_EVT;
    ret = btc_transfer_context(&msg, &mesh_param,
                               sizeof(esp_ble_mesh_prov_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        LOG_ERROR("%s btc_transfer_context failed\n", __func__);
    }

    return;
}
#endif /* CONFIG_BT_MESH_PROVISIONER */

static void btc_reset_cb(void)
{
    btc_msg_t msg = {0};
    bt_status_t ret;

    LOG_DEBUG("%s", __func__);

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_PROV;
    msg.act = ESP_BLE_MESH_NODE_PROV_RESET_EVT;
    ret = btc_transfer_context(&msg, NULL, 0, NULL);

    if (ret != BT_STATUS_SUCCESS) {
        LOG_ERROR("%s btc_transfer_context failed\n", __func__);
    }

    return;
}

int btc_ble_mesh_client_init(esp_ble_mesh_model_t *model)
{
    __ASSERT(model && model->op, "The client model or operation is invalid(NULL).");
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

esp_ble_mesh_model_t *btc_ble_mesh_model_find_vnd(esp_ble_mesh_elem_t *elem,
        uint16_t company, uint16_t id)
{
    return (esp_ble_mesh_model_t *)bt_mesh_model_find_vnd((struct bt_mesh_elem *)elem, company, id);
}

esp_ble_mesh_model_t *btc_ble_mesh_model_find(esp_ble_mesh_elem_t *elem,
        u16_t id)
{
    return (esp_ble_mesh_model_t *)bt_mesh_model_find((struct bt_mesh_elem *)elem, id);
}

const esp_ble_mesh_comp_t *btc_ble_mesh_comp_get(void)
{
    return (const esp_ble_mesh_comp_t *)bt_mesh_comp_get();
}

extern const struct bt_mesh_model_op bt_mesh_cfg_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_cfg_cli_op[];
extern const struct bt_mesh_model_op bt_mesh_health_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_health_cli_op[];
// For Generic Client
extern const struct bt_mesh_model_op gen_onoff_cli_op[];
extern const struct bt_mesh_model_op gen_level_cli_op[];
extern const struct bt_mesh_model_op gen_def_trans_time_cli_op[];
extern const struct bt_mesh_model_op gen_power_onoff_cli_op[];
extern const struct bt_mesh_model_op gen_power_level_cli_op[];
extern const struct bt_mesh_model_op gen_battery_cli_op[];
extern const struct bt_mesh_model_op gen_location_cli_op[];
extern const struct bt_mesh_model_op gen_property_cli_op[];
// For Light Lightness CLient
extern const struct bt_mesh_model_op light_lightness_cli_op[];
extern const struct bt_mesh_model_op light_ctl_cli_op[];
extern const struct bt_mesh_model_op light_hsl_cli_op[];
// For Sensor Client
extern const struct bt_mesh_model_op sensor_cli_op[];
// For Time Scene Client
extern const struct bt_mesh_model_op scene_cli_op[];

static void btc_mesh_model_op_add(esp_ble_mesh_model_t *model)
{
    esp_ble_mesh_model_op_t *op = NULL;
    esp_ble_mesh_model_pub_t *pub = NULL;

    if (!model) {
        return;
    }

    /* The vendor model opcode is 3 bytes. */
    if ((model->op != NULL) && (model->op->opcode >= 0x10000)) {
        goto add_model_op;
    }

    switch (model->model_id) {
    case BT_MESH_MODEL_ID_CFG_SRV: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_cfg_srv_op;
        break;
    }
    case BT_MESH_MODEL_ID_CFG_CLI: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_cfg_cli_op;
        bt_mesh_config_client_t *cli = (bt_mesh_config_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_mesh_cfg_client_publish_callback;
        }
        break;
    }
    case BT_MESH_MODEL_ID_HEALTH_SRV: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_health_srv_op;
        break;
    }
    case BT_MESH_MODEL_ID_HEALTH_CLI: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_health_cli_op;
        bt_mesh_health_client_t *cli = (bt_mesh_health_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_mesh_health_publish_callback;
        }
        break;
    }
    case BT_MESH_MODEL_ID_GEN_ONOFF_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)gen_onoff_cli_op);
        bt_mesh_gen_onoff_cli_t *cli = (bt_mesh_gen_onoff_cli_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_mesh_generic_client_publish_callback;
        }
        break;
    }
    case BT_MESH_MODEL_ID_GEN_LEVEL_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)gen_level_cli_op);
        bt_mesh_gen_level_cli_t *cli = (bt_mesh_gen_level_cli_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_mesh_generic_client_publish_callback;
        }
        break;
    }
    case BT_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)gen_def_trans_time_cli_op);
        bt_mesh_gen_def_trans_time_cli_t *cli = (bt_mesh_gen_def_trans_time_cli_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_mesh_generic_client_publish_callback;
        }
        break;
    }
    case BT_MESH_MODEL_ID_GEN_POWER_ONOFF_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)gen_power_onoff_cli_op);
        bt_mesh_gen_power_onoff_cli_t *cli = (bt_mesh_gen_power_onoff_cli_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_mesh_generic_client_publish_callback;
        }
        break;
    }
    case BT_MESH_MODEL_ID_GEN_POWER_LEVEL_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)gen_power_level_cli_op);
        bt_mesh_gen_power_level_cli_t *cli = (bt_mesh_gen_power_level_cli_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_mesh_generic_client_publish_callback;
        }
        break;
    }
    case BT_MESH_MODEL_ID_GEN_BATTERY_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)gen_battery_cli_op);
        bt_mesh_gen_battery_cli_t *cli = (bt_mesh_gen_battery_cli_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_mesh_generic_client_publish_callback;
        }
        break;
    }
    case BT_MESH_MODEL_ID_GEN_LOCATION_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)gen_location_cli_op);
        bt_mesh_gen_location_cli_t *cli = (bt_mesh_gen_location_cli_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_mesh_generic_client_publish_callback;
        }
        break;
    }
    case BT_MESH_MODEL_ID_GEN_PROP_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)gen_property_cli_op);
        bt_mesh_gen_property_cli_t *cli = (bt_mesh_gen_property_cli_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_mesh_generic_client_publish_callback;
        }
        break;
    }
    case BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)light_lightness_cli_op);
        bt_mesh_light_lightness_cli_t *cli = (bt_mesh_light_lightness_cli_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_mesh_light_client_publish_callback;
        }
        break;
    }
    case BT_MESH_MODEL_ID_LIGHT_CTL_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)light_ctl_cli_op);
        bt_mesh_light_ctl_cli_t *cli = (bt_mesh_light_ctl_cli_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_mesh_light_client_publish_callback;
        }
        break;
    }
    case BT_MESH_MODEL_ID_LIGHT_HSL_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)light_hsl_cli_op);
        bt_mesh_light_hsl_cli_t *cli = (bt_mesh_light_hsl_cli_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_mesh_light_client_publish_callback;
        }
        break;
    }
    case BT_MESH_MODEL_ID_SENSOR_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)sensor_cli_op);
        bt_mesh_sensor_client_t *cli = (bt_mesh_sensor_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_mesh_sensor_client_publish_callback;
        }
        break;
    }
    case BT_MESH_MODEL_ID_SCENE_CLI: {
        model->op = ((esp_ble_mesh_model_op_t *)scene_cli_op);
        bt_mesh_time_scene_client_t *cli = (bt_mesh_time_scene_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_mesh_time_scene_client_publish_callback;
        }
        break;
    }
    default: {
        goto add_model_op;
    }

    }

    return;

add_model_op:
    op = model->op;
    pub = model->pub;
    if (pub != NULL) {
        pub->update = (esp_ble_mesh_cb_t)btc_model_publish_update;
    }
    while (op != NULL && op->opcode != 0) {
        if (op->min_len > BLE_MESH_MAX_DATA_SIZE || op->param_cb != 0) {
            --op;
            LOG_ERROR("%s, Invalid model operation min_len = %d, or param callbak = 0x%x, the last valid opcode = 0x%x",
                      __func__, op->min_len, op->param_cb, op->opcode);
            return;
        }
        op->param_cb = (esp_ble_mesh_cb_t)btc_ble_mesh_server_model_op_cb;
        op++;
    }

    return;

}

void btc_mesh_prov_call_handler(btc_msg_t *msg)
{
    btc_ble_mesh_prov_args_t *arg = NULL;
    esp_ble_mesh_prov_cb_param_t param = {0};
    uint8_t act;

    if (!msg) {
        LOG_ERROR("%s: parameter is NULL", __func__);
        return;
    }
    arg = (btc_ble_mesh_prov_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_APP_REGISTER: {
        //struct bt_mesh_comp *comp = NULL;
        int err_code;
        //ESP_BLE_MESH_ELEMENT *elem = arg->mesh_reg.comp->comp;
        for (int i = 0; i < arg->mesh_reg.comp->element_count; i++) {
            esp_ble_mesh_elem_t *elem = &arg->mesh_reg.comp->elements[i];
            for (int j = 0; j < elem->sig_model_count; j++) {
                esp_ble_mesh_model_t *models = &elem->sig_models[j];
                /* The opcode of sig model should be 1 or 2 bytes. */
                if ((models != NULL) && (models->op != NULL) && (models->op->opcode >= 0x10000)) {
                    err_code = -EINVAL;
                    btc_prov_register_complete_cb(err_code);
                    return;
                }

                btc_mesh_model_op_add(models);
            }

            for (int k = 0; k < elem->vnd_model_count; k++) {
                esp_ble_mesh_model_t *vnd_models = &elem->vnd_models[k];
                /* The opcode of vendor model should be 3 bytes. */
                if (vnd_models->op->opcode < 0x10000) {
                    err_code = -EINVAL;
                    btc_prov_register_complete_cb(err_code);
                    return;
                }
                btc_mesh_model_op_add(vnd_models);
            }
        }
#if CONFIG_BT_MESH_NODE
        arg->mesh_reg.prov->output_num_cb = (esp_ble_mesh_cb_t)btc_output_number_cb;
        arg->mesh_reg.prov->output_str_cb = (esp_ble_mesh_cb_t)btc_output_string_cb;
        arg->mesh_reg.prov->input_cb = (esp_ble_mesh_cb_t)btc_input_cb;
        arg->mesh_reg.prov->link_open_cb = (esp_ble_mesh_cb_t)btc_link_open_cb;
        arg->mesh_reg.prov->link_close_cb = (esp_ble_mesh_cb_t)btc_link_close_cb;
        arg->mesh_reg.prov->complete_cb = (esp_ble_mesh_cb_t)btc_complete_cb;
        arg->mesh_reg.prov->reset_cb = (esp_ble_mesh_cb_t)btc_reset_cb;
#endif /* CONFIG_BT_MESH_NODE */
#if (CONFIG_BT_MESH_PROVISIONER)
        arg->mesh_reg.prov->provisioner_link_open = (esp_ble_mesh_cb_t)btc_provisioner_link_open_cb;
        arg->mesh_reg.prov->provisioner_link_close = (esp_ble_mesh_cb_t)btc_provisioner_link_close_cb;
        arg->mesh_reg.prov->provisioner_prov_comp = (esp_ble_mesh_cb_t)btc_provisioner_prov_complete_cb;
#endif /* CONFIG_BT_MESH_PROVISIONER */
        err_code = bt_mesh_init((struct bt_mesh_prov *)arg->mesh_reg.prov,
                                (struct bt_mesh_comp *)arg->mesh_reg.comp);
        btc_prov_register_complete_cb(err_code);
        break;
    }
#if CONFIG_BT_MESH_NODE
    case BTC_BLE_MESH_ACT_PROV_ENABLE:
        LOG_DEBUG("%s, BTC_BLE_MESH_ACT_PROV_ENABLE, bearers = %d", __func__, arg->mesh_prov_enable.bearers);
        act = ESP_BLE_MESH_NODE_PROV_ENABLE_COMP_EVT;
        param.node_prov_enable_comp.err_code = bt_mesh_prov_enable(arg->mesh_prov_enable.bearers);
        goto btc_prov_cb;
    case BTC_BLE_MESH_ACT_PROV_DISABLE:
        LOG_DEBUG("%s, BTC_BLE_MESH_ACT_PROV_DISABLE, bearers = %d", __func__, arg->mesh_prov_disable.bearers);
        act = ESP_BLE_MESH_NODE_PROV_DISABLE_COMP_EVT;
        param.node_prov_disable_comp.err_code = bt_mesh_prov_disable(arg->mesh_prov_disable.bearers);
        goto btc_prov_cb;
    case BTC_BLE_MESH_ACT_NODE_RESET:
        LOG_DEBUG("%s, BTC_BLE_MESH_ACT_NODE_RESET", __func__);
        bt_mesh_reset();
        break;
    case BTC_BLE_MESH_ACT_INPUT_NUMBER:
        act = ESP_BLE_MESH_NODE_PROV_INPUT_NUM_COMP_EVT;
        param.node_prov_input_num_comp.err_code = bt_mesh_input_number(arg->input_num.num);
        goto btc_prov_cb;
    case BTC_BLE_MESH_ACT_INPUT_STR:
        act = ESP_BLE_MESH_NODE_PROV_INPUT_STR_COMP_EVT;
        param.node_prov_input_str_comp.err_code = bt_mesh_input_string(arg->input_str.str);
        goto btc_prov_cb;
    case BTC_BLE_MESH_ACT_SET_DEVICE_NAME:
        act = ESP_BLE_MESH_NODE_SET_UNPROV_DEV_NAME_COMP_EVT;
        param.node_set_unprov_dev_name_comp.err_code = bt_mesh_set_device_name(arg->set_device_name.name);
        goto btc_prov_cb;
    case BTC_BLE_MESH_ACT_PROXY_IDENTITY_ENABLE:
        act = ESP_BLE_MESH_NODE_PROXY_IDENTITY_ENABLE_COMP_EVT;
        param.node_proxy_identity_enable_comp.err_code = bt_mesh_proxy_identity_enable();
        goto btc_prov_cb;
    case BTC_BLE_MESH_ACT_PROXY_GATT_ENABLE:
        act = ESP_BLE_MESH_NODE_PROXY_GATT_ENABLE_COMP_EVT;
        param.node_proxy_gatt_enable_comp.err_code = bt_mesh_proxy_gatt_enable();
        goto btc_prov_cb;
    case BTC_BLE_MESH_ACT_PROXY_GATT_DISABLE:
        act = ESP_BLE_MESH_NODE_PROXY_GATT_DISABLE_COMP_EVT;
        param.node_proxy_gatt_disable_comp.err_code = bt_mesh_proxy_gatt_disable();
        goto btc_prov_cb;
#endif /* CONFIG_BT_MESH_NODE */
#if (CONFIG_BT_MESH_PROVISIONER)
    case BTC_BLE_MESH_ACT_PROVISIONER_INPUT_STR:
        /* The BLE Mesh stack has not implemented the following functions now. */
        //bt_mesh_prov_input_string(arg->provisioner_input_str.str);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_INPUT_NUM:
        /* The BLE Mesh stack has not implemented the following functions now. */
        //bt_mesh_prov_input_number(arg->provisioner_input_num.num);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_ENABLE:
        act = ESP_BLE_MESH_PROVISIONER_PROV_ENABLE_COMP_EVT;
        param.provisioner_prov_enable_comp.err_code =
            bt_mesh_provisioner_enable(arg->provisioner_enable.bearers);
        goto btc_prov_cb;
    case BTC_BLE_MESH_ACT_PROVISIONER_DISABLE:
        act = ESP_BLE_MESH_PROVISIONER_PROV_DISABLE_COMP_EVT;
        param.provisioner_prov_disable_comp.err_code =
            bt_mesh_provisioner_disable(arg->provisioner_disable.bearers);
        goto btc_prov_cb;
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
        goto btc_prov_cb;
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
        goto btc_prov_cb;
    }
    case BTC_BLE_MESH_ACT_PROVISIONER_SET_DEV_UUID_MATCH:
        act = ESP_BLE_MESH_PROVISIONER_SET_DEV_UUID_MATCH_COMP_EVT;
        param.provisioner_set_dev_uuid_match_comp.err_code =
            bt_mesh_provisioner_set_dev_uuid_match(arg->set_dev_uuid_match.offset,
                    arg->set_dev_uuid_match.match_len,
                    arg->set_dev_uuid_match.match_val,
                    arg->set_dev_uuid_match.prov_after_match);
        goto btc_prov_cb;
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
        goto btc_prov_cb;
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
        goto btc_prov_cb;
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
        goto btc_prov_cb;
    }
    case BTC_BLE_MESH_ACT_PROVISIONER_BIND_LOCAL_MOD_APP:
        act = ESP_BLE_MESH_PROVISIONER_BIND_APP_KEY_TO_MODEL_COMP_EVT;
        param.provisioner_bind_app_key_to_model_comp.err_code =
            bt_mesh_provisioner_bind_local_model_app_idx(arg->local_mod_app_bind.elem_addr,
                    arg->local_mod_app_bind.model_id,
                    arg->local_mod_app_bind.cid,
                    arg->local_mod_app_bind.app_idx);
        goto btc_prov_cb;
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
        goto btc_prov_cb;
    }
#endif /* CONFIG_BT_MESH_PROVISIONER */
    default:
        LOG_WARN("%s, Invalid msg->act %d", __func__, msg->act);
        break;
    }
    return;

btc_prov_cb:
    btc_prov_set_complete_cb(&param, act);
    return;
}

void btc_mesh_model_call_handler(btc_msg_t *msg)
{
    btc_ble_mesh_model_args_t *arg = NULL;

    if (!msg || !msg->arg) {
        LOG_ERROR("%s: parameter is NULL", __func__);
        return;
    }

    arg = (btc_ble_mesh_model_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_MODEL_PUBLISH: {
        if (arg->model_publish.device_role == PROVISIONER) {
            bt_mesh_role_param_t common = {0};
            common.model = (struct bt_mesh_model *)(arg->model_publish.model);
            common.role  = arg->model_publish.device_role;
            if (bt_mesh_copy_msg_role(&common)) {
                LOG_ERROR("%s: copy msg_role fail", __func__);
                return;
            }
        }
        int err = bt_mesh_model_publish((struct bt_mesh_model *)arg->model_publish.model);
        btc_ble_mesh_model_publish_comp_cb(arg->model_publish.model, err);
        break;
    }
    case BTC_BLE_MESH_ACT_SERVER_MODEL_SEND: {
        struct net_buf_simple *msg = NET_BUF_SIMPLE(MIN(BT_MESH_TX_SDU_MAX, ESP_BLE_MESH_SDU_MAX_LEN));
        net_buf_simple_init(msg, 0);
        net_buf_simple_add_mem(msg, arg->model_send.data, arg->model_send.length);
        arg->model_send.ctx->srv_send = true;
        int err = bt_mesh_model_send((struct bt_mesh_model *)arg->model_send.model,
                                     (struct bt_mesh_msg_ctx *)arg->model_send.ctx,
                                     msg, NULL, NULL);
        btc_ble_mesh_model_send_comp_cb(arg->model_send.model, arg->model_send.ctx, err);
        break;
    }
    case BTC_BLE_MESH_ACT_CLIENT_MODEL_SEND: {
        struct net_buf_simple *msg = NET_BUF_SIMPLE(MIN(BT_MESH_TX_SDU_MAX, ESP_BLE_MESH_SDU_MAX_LEN));
        bt_mesh_role_param_t common = {0};
        net_buf_simple_init(msg, 0);
        net_buf_simple_add_mem(msg, arg->model_send.data, arg->model_send.length);
        arg->model_send.ctx->srv_send = false;
        common.model = (struct bt_mesh_model *)(arg->model_send.model);
        common.role  = arg->model_send.device_role;
        if (bt_mesh_copy_msg_role(&common)) {
            LOG_ERROR("%s: copy msg_role fail", __func__);
            return;
        }
        int err = bt_mesh_client_send_msg((struct bt_mesh_model *)arg->model_send.model,
                                          arg->model_send.opcode,
                                          (struct bt_mesh_msg_ctx *)arg->model_send.ctx, msg,
                                          btc_client_model_timeout_cb, arg->model_send.msg_timeout,
                                          arg->model_send.need_rsp, NULL, NULL);
        btc_ble_mesh_model_send_comp_cb(arg->model_send.model, arg->model_send.ctx, err);
        break;
    }
    default:
        LOG_WARN("%s, Invalid msg->act %d", __func__, msg->act);
        break;
    }

    btc_ble_mesh_prov_arg_deep_free(msg);
}

void btc_mesh_model_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_model_cb_param_t *param = NULL;

    if (!msg || !msg->arg) {
        LOG_ERROR("%s: parameter is NULL", __func__);
        return;
    }

    param = (esp_ble_mesh_model_cb_param_t *)(msg->arg);

    if (msg->act < ESP_BLE_MESH_MODEL_EVT_MAX) {
        btc_ble_mesh_model_cb_to_app(msg->act, param);
    } else {
        LOG_ERROR("%s, unknown msg->act = %d", __func__, msg->act);
    }

    btc_ble_mesh_free_req_data(msg);
}

void btc_mesh_prov_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_prov_cb_param_t *param = NULL;

    if (!msg) {
        LOG_ERROR("%s: parameter is NULL", __func__);
        return;
    }

    param = (esp_ble_mesh_prov_cb_param_t *)(msg->arg);

    if (msg->act < ESP_BLE_MESH_PROV_EVT_MAX) {
        btc_ble_mesh_cb_to_app(msg->act, param);
    } else {
        LOG_ERROR("%s, unknown msg->act = %d", __func__, msg->act);
    }
}

const btc_mesh_cb_t *btc_mesh_prov_get_interface(void)
{
    return &interface;
}

#endif /* #if CONFIG_BT_MESH */

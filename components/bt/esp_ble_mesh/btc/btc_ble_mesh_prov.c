/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "btc_ble_mesh_prov.h"
#include "btc_ble_mesh_config_model.h"
#include "btc_ble_mesh_health_model.h"
#include "btc_ble_mesh_generic_model.h"
#include "btc_ble_mesh_time_scene_model.h"
#include "btc_ble_mesh_sensor_model.h"
#include "btc_ble_mesh_lighting_model.h"

#include "adv.h"
#include "mesh_kernel.h"
#include "mesh_proxy.h"
#include "mesh.h"
#include "access.h"
#include "prov.h"
#include "settings_uid.h"
#include "proxy_server.h"
#include "proxy_client.h"
#include "provisioner_prov.h"
#include "provisioner_main.h"

#if CONFIG_BLE_MESH_CFG_CLI
#include "cfg_cli.h"
#endif /* CONFIG_BLE_MESH_CFG_CLI */
#if CONFIG_BLE_MESH_HEALTH_CLI
#include "health_cli.h"
#endif /* CONFIG_BLE_MESH_HEALTH_CLI */
#include "cfg_srv.h"
#if CONFIG_BLE_MESH_HEALTH_SRV
#include "health_srv.h"
#endif /* CONFIG_BLE_MESH_HEALTH_SRV */
#if CONFIG_BLE_MESH_GENERIC_CLIENT
#include "generic_client.h"
#endif /* CONFIG_BLE_MESH_GENERIC_CLIENT */
#if CONFIG_BLE_MESH_LIGHTING_CLIENT
#include "lighting_client.h"
#endif /* CONFIG_BLE_MESH_LIGHTING_CLIENT */
#if CONFIG_BLE_MESH_SENSOR_CLI
#include "sensor_client.h"
#endif /* CONFIG_BLE_MESH_SENSOR_CLI */
#if CONFIG_BLE_MESH_TIME_SCENE_CLIENT
#include "time_scene_client.h"
#endif /* CONFIG_BLE_MESH_TIME_SCENE_CLIENT */
#include "client_common.h"
#include "state_binding.h"
#include "local_operation.h"

#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_networking_api.h"

#if CONFIG_BLE_MESH_DEINIT
static SemaphoreHandle_t deinit_comp_semaphore;
#endif

static inline void btc_ble_mesh_prov_cb_to_app_reprocess(esp_ble_mesh_prov_cb_event_t event,
                                                         esp_ble_mesh_prov_cb_param_t *param)
{
    switch (event) {
#if CONFIG_BLE_MESH_DEINIT
    case ESP_BLE_MESH_DEINIT_MESH_COMP_EVT:
        assert(deinit_comp_semaphore);
        /* Give the semaphore when BLE Mesh de-initialization is finished.
         * @note: At nimble host, once this lock is released, it will cause
         * the btc task to be deleted.
         */
        xSemaphoreGive(deinit_comp_semaphore);
        break;
#endif
    default:
        break;
    }
}

static inline void btc_ble_mesh_prov_cb_to_app(esp_ble_mesh_prov_cb_event_t event,
                                               esp_ble_mesh_prov_cb_param_t *param)
{
    esp_ble_mesh_prov_cb_t btc_ble_mesh_cb =
        (esp_ble_mesh_prov_cb_t)btc_profile_cb_get(BTC_PID_PROV);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }

    btc_ble_mesh_prov_cb_to_app_reprocess(event, param);
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
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_PROXY_CLIENT_ADD_FILTER_ADDR:
        dst->proxy_client_add_filter_addr.addr = (uint16_t *)bt_mesh_calloc(src->proxy_client_add_filter_addr.addr_num << 1);
        if (dst->proxy_client_add_filter_addr.addr) {
            memcpy(dst->proxy_client_add_filter_addr.addr, src->proxy_client_add_filter_addr.addr,
                   src->proxy_client_add_filter_addr.addr_num << 1);
        } else {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
        }
        break;
    case BTC_BLE_MESH_ACT_PROXY_CLIENT_REMOVE_FILTER_ADDR:
        dst->proxy_client_remove_filter_addr.addr = bt_mesh_calloc(src->proxy_client_remove_filter_addr.addr_num << 1);
        if (dst->proxy_client_remove_filter_addr.addr) {
            memcpy(dst->proxy_client_remove_filter_addr.addr, src->proxy_client_remove_filter_addr.addr,
                   src->proxy_client_remove_filter_addr.addr_num << 1);
        } else {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
        }
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_STORE_NODE_COMP_DATA:
        dst->store_node_comp_data.data = bt_mesh_calloc(src->store_node_comp_data.length);
        if (dst->store_node_comp_data.data) {
            memcpy(dst->store_node_comp_data.data, src->store_node_comp_data.data, src->store_node_comp_data.length);
        } else {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
        }
        break;
    default:
        BT_DBG("%s, Unknown act %d", __func__, msg->act);
        break;
    }
}

void btc_ble_mesh_prov_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_mesh_prov_args_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_prov_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_PROXY_CLIENT_ADD_FILTER_ADDR:
        if (arg->proxy_client_add_filter_addr.addr) {
            bt_mesh_free(arg->proxy_client_add_filter_addr.addr);
        }
        break;
    case BTC_BLE_MESH_ACT_PROXY_CLIENT_REMOVE_FILTER_ADDR:
        if (arg->proxy_client_remove_filter_addr.addr) {
            bt_mesh_free(arg->proxy_client_remove_filter_addr.addr);
        }
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_STORE_NODE_COMP_DATA:
        if (arg->store_node_comp_data.data) {
            bt_mesh_free(arg->store_node_comp_data.data);
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
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_SERVER_MODEL_SEND:
    case BTC_BLE_MESH_ACT_CLIENT_MODEL_SEND: {
        dst->model_send.data = src->model_send.length ? (uint8_t *)bt_mesh_malloc(src->model_send.length) : NULL;
        dst->model_send.ctx = bt_mesh_malloc(sizeof(esp_ble_mesh_msg_ctx_t));
        if (src->model_send.length) {
            if (dst->model_send.data) {
                memcpy(dst->model_send.data, src->model_send.data, src->model_send.length);
            } else {
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            }
        }
        if (dst->model_send.ctx) {
            memcpy(dst->model_send.ctx, src->model_send.ctx, sizeof(esp_ble_mesh_msg_ctx_t));
        } else {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
        }
        break;
    }
    case BTC_BLE_MESH_ACT_SERVER_MODEL_UPDATE_STATE:
        dst->model_update_state.value = bt_mesh_malloc(sizeof(esp_ble_mesh_server_state_value_t));
        if (dst->model_update_state.value) {
            memcpy(dst->model_update_state.value, src->model_update_state.value,
                   sizeof(esp_ble_mesh_server_state_value_t));
        } else {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
        }
        break;
    default:
        BT_DBG("%s, Unknown act %d", __func__, msg->act);
        break;
    }
}

void btc_ble_mesh_model_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_mesh_model_args_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_model_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_SERVER_MODEL_SEND:
    case BTC_BLE_MESH_ACT_CLIENT_MODEL_SEND:
        if (arg->model_send.data) {
            bt_mesh_free(arg->model_send.data);
        }
        if (arg->model_send.ctx) {
            bt_mesh_free(arg->model_send.ctx);
        }
        break;
    case BTC_BLE_MESH_ACT_SERVER_MODEL_UPDATE_STATE:
        if (arg->model_update_state.value) {
            bt_mesh_free(arg->model_update_state.value);
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
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case ESP_BLE_MESH_MODEL_OPERATION_EVT: {
        if (p_src_data->model_operation.ctx && p_src_data->model_operation.msg) {
            p_dest_data->model_operation.ctx = bt_mesh_malloc(sizeof(esp_ble_mesh_msg_ctx_t));
            p_dest_data->model_operation.msg = p_src_data->model_operation.length ? (uint8_t *)bt_mesh_malloc(p_src_data->model_operation.length) : NULL;
            if (p_dest_data->model_operation.ctx) {
                memcpy(p_dest_data->model_operation.ctx, p_src_data->model_operation.ctx, sizeof(esp_ble_mesh_msg_ctx_t));
            } else {
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            }
            if (p_src_data->model_operation.length) {
                if (p_dest_data->model_operation.msg) {
                    memcpy(p_dest_data->model_operation.msg, p_src_data->model_operation.msg, p_src_data->model_operation.length);
                } else {
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                }
            }
        }
        break;
    }
    case ESP_BLE_MESH_CLIENT_MODEL_RECV_PUBLISH_MSG_EVT: {
        if (p_src_data->client_recv_publish_msg.ctx && p_src_data->client_recv_publish_msg.msg) {
            p_dest_data->client_recv_publish_msg.ctx = bt_mesh_malloc(sizeof(esp_ble_mesh_msg_ctx_t));
            p_dest_data->client_recv_publish_msg.msg = p_src_data->client_recv_publish_msg.length ? (uint8_t *)bt_mesh_malloc(p_src_data->client_recv_publish_msg.length) : NULL;
            if (p_dest_data->client_recv_publish_msg.ctx) {
                memcpy(p_dest_data->client_recv_publish_msg.ctx, p_src_data->client_recv_publish_msg.ctx, sizeof(esp_ble_mesh_msg_ctx_t));
            } else {
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            }
            if (p_src_data->client_recv_publish_msg.length) {
                if (p_dest_data->client_recv_publish_msg.msg) {
                    memcpy(p_dest_data->client_recv_publish_msg.msg, p_src_data->client_recv_publish_msg.msg, p_src_data->client_recv_publish_msg.length);
                } else {
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                }
            }
        }
        break;
    }
    case ESP_BLE_MESH_MODEL_SEND_COMP_EVT: {
        if (p_src_data->model_send_comp.ctx) {
            p_dest_data->model_send_comp.ctx = bt_mesh_malloc(sizeof(esp_ble_mesh_msg_ctx_t));
            if (p_dest_data->model_send_comp.ctx) {
                memcpy(p_dest_data->model_send_comp.ctx, p_src_data->model_send_comp.ctx, sizeof(esp_ble_mesh_msg_ctx_t));
            } else {
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            }
        }
        break;
    }
    case ESP_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT: {
        if (p_src_data->client_send_timeout.ctx) {
            p_dest_data->client_send_timeout.ctx = bt_mesh_malloc(sizeof(esp_ble_mesh_msg_ctx_t));
            if (p_dest_data->client_send_timeout.ctx) {
                memcpy(p_dest_data->client_send_timeout.ctx, p_src_data->client_send_timeout.ctx, sizeof(esp_ble_mesh_msg_ctx_t));
            } else {
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
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

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_model_cb_param_t *)(msg->arg);

    switch (msg->act) {
    case ESP_BLE_MESH_MODEL_OPERATION_EVT: {
        if (arg->model_operation.msg) {
            bt_mesh_free(arg->model_operation.msg);
        }
        if (arg->model_operation.ctx) {
            bt_mesh_free(arg->model_operation.ctx);
        }
        break;
    }
    case ESP_BLE_MESH_CLIENT_MODEL_RECV_PUBLISH_MSG_EVT: {
        if (arg->client_recv_publish_msg.msg) {
            bt_mesh_free(arg->client_recv_publish_msg.msg);
        }
        if (arg->client_recv_publish_msg.ctx) {
            bt_mesh_free(arg->client_recv_publish_msg.ctx);
        }
        break;
    }
    case ESP_BLE_MESH_MODEL_SEND_COMP_EVT: {
        if (arg->model_send_comp.ctx) {
            bt_mesh_free(arg->model_send_comp.ctx);
        }
        break;
    }
    case ESP_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT: {
        if (arg->client_send_timeout.ctx) {
            bt_mesh_free(arg->client_send_timeout.ctx);
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
    bt_status_t ret = BT_STATUS_SUCCESS;

    BT_DBG("%s", __func__);

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_MODEL)) {
        return BT_STATUS_SUCCESS;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_MODEL;
    msg.act = act;

    ret = btc_transfer_context(&msg, param, param == NULL ? 0 : sizeof(esp_ble_mesh_model_cb_param_t),
                               btc_ble_mesh_model_copy_req_data, btc_ble_mesh_model_free_req_data);
    if (ret != BT_STATUS_SUCCESS) {
        BT_ERR("btc_transfer_context failed");
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
        BT_ERR("%s, Invalid parameter", __func__);
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
            bt_mesh_client_free_node(node);
            btc_ble_mesh_model_callback(&mesh_param, ESP_BLE_MESH_MODEL_OPERATION_EVT);
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
    struct bt_mesh_msg_ctx ctx = {0};

    bt_mesh_client_model_lock();

    timer = CONTAINER_OF(work, struct k_delayed_work, work);

    if (timer && !k_delayed_work_free(timer)) {
        node = CONTAINER_OF(work, bt_mesh_client_node_t, timer.work);
        if (node) {
            memcpy(&ctx, &node->ctx, sizeof(ctx));
            mesh_param.client_send_timeout.opcode = node->opcode;
            mesh_param.client_send_timeout.model = (esp_ble_mesh_model_t *)ctx.model;
            mesh_param.client_send_timeout.ctx = (esp_ble_mesh_msg_ctx_t *)&ctx;
            bt_mesh_client_free_node(node);
            btc_ble_mesh_model_callback(&mesh_param, ESP_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT);
        }
    }

    bt_mesh_client_model_unlock();
    return;
}

static void btc_ble_mesh_model_send_comp_cb(esp_ble_mesh_model_t *model,
                                            esp_ble_mesh_msg_ctx_t *ctx,
                                            uint32_t opcode, int err)
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
    bt_status_t ret = BT_STATUS_SUCCESS;

    BT_DBG("%s", __func__);

    mesh_param.model_publish_update.model = (esp_ble_mesh_model_t *)mod;

    ret = btc_ble_mesh_model_callback(&mesh_param, ESP_BLE_MESH_MODEL_PUBLISH_UPDATE_EVT);
    return (ret == BT_STATUS_SUCCESS) ? 0 : -1;
}

#if CONFIG_BLE_MESH_SERVER_MODEL
static void btc_ble_mesh_server_model_update_state_comp_cb(esp_ble_mesh_model_t *model,
                                                           esp_ble_mesh_server_state_type_t type,
                                                           int err)
{
    esp_ble_mesh_model_cb_param_t mesh_param = {0};

    mesh_param.server_model_update_state.err_code = err;
    mesh_param.server_model_update_state.model = model;
    mesh_param.server_model_update_state.type = type;

    btc_ble_mesh_model_callback(&mesh_param, ESP_BLE_MESH_SERVER_MODEL_UPDATE_STATE_COMP_EVT);
    return;
}
#endif /* CONFIG_BLE_MESH_SERVER_MODEL */

static bt_status_t btc_ble_mesh_prov_callback(esp_ble_mesh_prov_cb_param_t *param, uint8_t act)
{
    btc_msg_t msg = {0};
    bt_status_t ret = BT_STATUS_SUCCESS;

    BT_DBG("%s", __func__);

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_PROV)) {
        return BT_STATUS_SUCCESS;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_PROV;
    msg.act = act;

    ret = btc_transfer_context(&msg, param, param == NULL ? 0 : sizeof(esp_ble_mesh_prov_cb_param_t), NULL, NULL);
    if (ret != BT_STATUS_SUCCESS) {
        BT_ERR("btc_transfer_context failed");
    }
    return ret;
}

#if CONFIG_BLE_MESH_NODE
static void btc_ble_mesh_oob_pub_key_cb(void)
{
    BT_DBG("%s", __func__);

    btc_ble_mesh_prov_callback(NULL, ESP_BLE_MESH_NODE_PROV_OOB_PUB_KEY_EVT);
    return;
}

static int btc_ble_mesh_output_number_cb(bt_mesh_output_action_t act, uint32_t num)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    bt_status_t ret = BT_STATUS_SUCCESS;

    BT_DBG("%s", __func__);

    mesh_param.node_prov_output_num.action = (esp_ble_mesh_output_action_t)act;
    mesh_param.node_prov_output_num.number = num;

    ret = btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_NODE_PROV_OUTPUT_NUMBER_EVT);
    return (ret == BT_STATUS_SUCCESS) ? 0 : -1;
}

static int btc_ble_mesh_output_string_cb(const char *str)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    bt_status_t ret = BT_STATUS_SUCCESS;

    BT_DBG("%s", __func__);

    strncpy(mesh_param.node_prov_output_str.string, str,
        MIN(strlen(str), sizeof(mesh_param.node_prov_output_str.string)));

    ret = btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_NODE_PROV_OUTPUT_STRING_EVT);
    return (ret == BT_STATUS_SUCCESS) ? 0 : -1;
}

static int btc_ble_mesh_input_cb(bt_mesh_input_action_t act, uint8_t size)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    bt_status_t ret = BT_STATUS_SUCCESS;

    BT_DBG("%s", __func__);

    mesh_param.node_prov_input.action = (esp_ble_mesh_input_action_t)act;
    mesh_param.node_prov_input.size = size;

    ret = btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_NODE_PROV_INPUT_EVT);
    return (ret == BT_STATUS_SUCCESS) ? 0 : -1;
}

static void btc_ble_mesh_link_open_cb(bt_mesh_prov_bearer_t bearer)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    BT_DBG("%s", __func__);

    mesh_param.node_prov_link_open.bearer = (esp_ble_mesh_prov_bearer_t)bearer;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_NODE_PROV_LINK_OPEN_EVT);
    return;
}

static void btc_ble_mesh_link_close_cb(bt_mesh_prov_bearer_t bearer)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    BT_DBG("%s", __func__);

    mesh_param.node_prov_link_close.bearer = (esp_ble_mesh_prov_bearer_t)bearer;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT);
    return;
}

static void btc_ble_mesh_complete_cb(uint16_t net_idx, const uint8_t net_key[16],
                                     uint16_t addr, uint8_t flags, uint32_t iv_index)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    BT_DBG("%s", __func__);

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
    BT_DBG("%s", __func__);

    btc_ble_mesh_prov_callback(NULL, ESP_BLE_MESH_NODE_PROV_RESET_EVT);
    return;
}

const uint8_t *btc_ble_mesh_node_get_local_net_key(uint16_t net_idx)
{
    return bt_mesh_node_get_local_net_key(net_idx);
}

const uint8_t *btc_ble_mesh_node_get_local_app_key(uint16_t app_idx)
{
    return bt_mesh_node_get_local_app_key(app_idx);
}
#endif /* CONFIG_BLE_MESH_NODE */

static void btc_ble_mesh_prov_register_complete_cb(int err_code)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    BT_DBG("%s", __func__);

    mesh_param.prov_register_comp.err_code = err_code;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROV_REGISTER_COMP_EVT);
    return;
}

static void btc_ble_mesh_prov_set_complete_cb(esp_ble_mesh_prov_cb_param_t *param, uint8_t act)
{
    BT_DBG("%s", __func__);

    btc_ble_mesh_prov_callback(param, act);
    return;
}

#if CONFIG_BLE_MESH_PROVISIONER
static void btc_ble_mesh_provisioner_recv_unprov_adv_pkt_cb(const uint8_t addr[6], const uint8_t addr_type,
                                                            const uint8_t adv_type, const uint8_t dev_uuid[16],
                                                            uint16_t oob_info, bt_mesh_prov_bearer_t bearer,
                                                            int8_t rssi)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    BT_DBG("%s", __func__);

    if (addr == NULL || dev_uuid == NULL ||
            (bearer != BLE_MESH_PROV_ADV && bearer != BLE_MESH_PROV_GATT)) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    memcpy(mesh_param.provisioner_recv_unprov_adv_pkt.dev_uuid, dev_uuid, 16);
    memcpy(mesh_param.provisioner_recv_unprov_adv_pkt.addr, addr, BLE_MESH_ADDR_LEN);
    mesh_param.provisioner_recv_unprov_adv_pkt.addr_type = addr_type;
    mesh_param.provisioner_recv_unprov_adv_pkt.oob_info = oob_info;
    mesh_param.provisioner_recv_unprov_adv_pkt.adv_type = adv_type;
    mesh_param.provisioner_recv_unprov_adv_pkt.bearer = bearer;
    mesh_param.provisioner_recv_unprov_adv_pkt.rssi = rssi;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROVISIONER_RECV_UNPROV_ADV_PKT_EVT);
    return;
}

static int btc_ble_mesh_provisioner_prov_read_oob_pub_key_cb(uint8_t link_idx)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    bt_status_t ret = BT_STATUS_SUCCESS;

    BT_DBG("%s", __func__);

    mesh_param.provisioner_prov_read_oob_pub_key.link_idx = link_idx;

    ret = btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROVISIONER_PROV_READ_OOB_PUB_KEY_EVT);
    return (ret == BT_STATUS_SUCCESS) ? 0 : -1;
}

static int btc_ble_mesh_provisioner_prov_input_cb(uint8_t method, bt_mesh_output_action_t act,
                                                  uint8_t size, uint8_t link_idx)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    bt_status_t ret = BT_STATUS_SUCCESS;

    BT_DBG("%s", __func__);

    mesh_param.provisioner_prov_input.method = (esp_ble_mesh_oob_method_t)method;
    mesh_param.provisioner_prov_input.action = (esp_ble_mesh_output_action_t)act;
    mesh_param.provisioner_prov_input.size = size;
    mesh_param.provisioner_prov_input.link_idx = link_idx;

    ret = btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROVISIONER_PROV_INPUT_EVT);
    return (ret == BT_STATUS_SUCCESS) ? 0 : -1;
}

static int btc_ble_mesh_provisioner_prov_output_cb(uint8_t method, bt_mesh_input_action_t act,
                                                   void *data, uint8_t size, uint8_t link_idx)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    bt_status_t ret = BT_STATUS_SUCCESS;

    BT_DBG("%s", __func__);

    mesh_param.provisioner_prov_output.method = (esp_ble_mesh_oob_method_t)method;
    mesh_param.provisioner_prov_output.action = (esp_ble_mesh_input_action_t)act;
    mesh_param.provisioner_prov_output.size = size;
    mesh_param.provisioner_prov_output.link_idx = link_idx;
    if (act == BLE_MESH_ENTER_STRING) {
        strncpy(mesh_param.provisioner_prov_output.string, (char *)data, size);
    } else {
        mesh_param.provisioner_prov_output.number = sys_get_le32((uint8_t *)data);
    }

    ret = btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROVISIONER_PROV_OUTPUT_EVT);
    return (ret == BT_STATUS_SUCCESS) ? 0 : -1;
}

static void btc_ble_mesh_provisioner_link_open_cb(bt_mesh_prov_bearer_t bearer)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    BT_DBG("%s", __func__);

    mesh_param.provisioner_prov_link_open.bearer = (esp_ble_mesh_prov_bearer_t)bearer;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROVISIONER_PROV_LINK_OPEN_EVT);
    return;
}

static void btc_ble_mesh_provisioner_link_close_cb(bt_mesh_prov_bearer_t bearer, uint8_t reason)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    BT_DBG("%s", __func__);

    mesh_param.provisioner_prov_link_close.bearer = (esp_ble_mesh_prov_bearer_t)bearer;
    mesh_param.provisioner_prov_link_close.reason = reason;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROVISIONER_PROV_LINK_CLOSE_EVT);
    return;
}

static void btc_ble_mesh_provisioner_prov_complete_cb(uint16_t node_idx, const uint8_t device_uuid[16],
                                                      uint16_t unicast_addr, uint8_t element_num,
                                                      uint16_t netkey_idx)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    BT_DBG("%s", __func__);

    mesh_param.provisioner_prov_complete.node_idx = node_idx;
    mesh_param.provisioner_prov_complete.unicast_addr = unicast_addr;
    mesh_param.provisioner_prov_complete.element_num = element_num;
    mesh_param.provisioner_prov_complete.netkey_idx = netkey_idx;
    memcpy(mesh_param.provisioner_prov_complete.device_uuid, device_uuid, sizeof(esp_ble_mesh_octet16_t));

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROVISIONER_PROV_COMPLETE_EVT);
    return;
}

esp_ble_mesh_node_t *btc_ble_mesh_provisioner_get_node_with_uuid(const uint8_t uuid[16])
{
    return (esp_ble_mesh_node_t *)bt_mesh_provisioner_get_node_with_uuid(uuid);
}

esp_ble_mesh_node_t *btc_ble_mesh_provisioner_get_node_with_addr(uint16_t unicast_addr)
{
    return (esp_ble_mesh_node_t *)bt_mesh_provisioner_get_node_with_addr(unicast_addr);
}

esp_ble_mesh_node_t *btc_ble_mesh_provisioner_get_node_with_name(const char *name)
{
    return (esp_ble_mesh_node_t *)bt_mesh_provisioner_get_node_with_name(name);
}

uint16_t btc_ble_mesh_provisioner_get_prov_node_count(void)
{
    return bt_mesh_provisioner_get_node_count();
}

const esp_ble_mesh_node_t **btc_ble_mesh_provisioner_get_node_table_entry(void)
{
    return (const esp_ble_mesh_node_t **)bt_mesh_provisioner_get_node_table_entry();
}

#if CONFIG_BLE_MESH_PROVISIONER_RECV_HB
static void btc_ble_mesh_provisioner_recv_heartbeat_cb(uint16_t hb_src, uint16_t hb_dst,
                                                       uint8_t init_ttl, uint8_t rx_ttl,
                                                       uint8_t hops, uint16_t feat, int8_t rssi)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    mesh_param.provisioner_recv_heartbeat.hb_src = hb_src;
    mesh_param.provisioner_recv_heartbeat.hb_dst = hb_dst;
    mesh_param.provisioner_recv_heartbeat.init_ttl = init_ttl;
    mesh_param.provisioner_recv_heartbeat.rx_ttl = rx_ttl;
    mesh_param.provisioner_recv_heartbeat.hops = hops;
    mesh_param.provisioner_recv_heartbeat.feature = feat;
    mesh_param.provisioner_recv_heartbeat.rssi = rssi;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROVISIONER_RECV_HEARTBEAT_MESSAGE_EVT);
}
#endif /* CONFIG_BLE_MESH_PROVISIONER_RECV_HB */

#if CONFIG_BLE_MESH_USE_MULTIPLE_NAMESPACE
const char *btc_ble_mesh_provisioner_get_settings_uid(uint8_t index)
{
    return bt_mesh_provisioner_get_settings_uid(index);
}

uint8_t btc_ble_mesh_provisioner_get_settings_index(const char *uid)
{
    return bt_mesh_provisioner_get_settings_index(uid);
}

uint8_t btc_ble_mesh_provisioner_get_free_settings_count(void)
{
    return bt_mesh_provisioner_get_free_settings_count();
}
#endif /* CONFIG_BLE_MESH_USE_MULTIPLE_NAMESPACE */

#endif /* CONFIG_BLE_MESH_PROVISIONER */

static void btc_ble_mesh_node_recv_heartbeat_cb(uint8_t hops, uint16_t feature)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    BT_DBG("%s", __func__);

    mesh_param.heartbeat_msg_recv.hops = hops;
    mesh_param.heartbeat_msg_recv.feature = feature;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_HEARTBEAT_MESSAGE_RECV_EVT);
    return;
}

#if CONFIG_BLE_MESH_LOW_POWER
static void btc_ble_mesh_lpn_cb(uint16_t friend_addr, bool established)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    uint8_t act = 0U;

    BT_DBG("%s", __func__);

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
void btc_ble_mesh_friend_cb(bool establish, uint16_t lpn_addr, uint8_t reason)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};
    uint8_t act = 0U;

    BT_DBG("%s", __func__);

    if (!BLE_MESH_ADDR_IS_UNICAST(lpn_addr)) {
        BT_ERR("Not a unicast lpn address 0x%04x", lpn_addr);
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

#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
static void btc_ble_mesh_proxy_client_adv_recv_cb(const bt_mesh_addr_t *addr, uint8_t type,
                                                  bt_mesh_proxy_adv_ctx_t *ctx, int8_t rssi)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    if (!addr || !ctx || type != BLE_MESH_PROXY_ADV_NET_ID) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    BT_DBG("%s", __func__);

    mesh_param.proxy_client_recv_adv_pkt.addr_type = addr->type;
    memcpy(mesh_param.proxy_client_recv_adv_pkt.addr, addr->val, BD_ADDR_LEN);
    mesh_param.proxy_client_recv_adv_pkt.net_idx = ctx->net_id.net_idx;
    memcpy(mesh_param.proxy_client_recv_adv_pkt.net_id, ctx->net_id.net_id, 8);
    mesh_param.proxy_client_recv_adv_pkt.rssi = rssi;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROXY_CLIENT_RECV_ADV_PKT_EVT);
    return;
}

static void btc_ble_mesh_proxy_client_connect_cb(const bt_mesh_addr_t *addr,
                                                 uint8_t conn_handle, uint16_t net_idx)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    if (!addr || conn_handle >= BLE_MESH_MAX_CONN) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    BT_DBG("%s", __func__);

    mesh_param.proxy_client_connected.addr_type = addr->type;
    memcpy(mesh_param.proxy_client_connected.addr, addr->val, BD_ADDR_LEN);
    mesh_param.proxy_client_connected.conn_handle = conn_handle;
    mesh_param.proxy_client_connected.net_idx = net_idx;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROXY_CLIENT_CONNECTED_EVT);
    return;
}

static void btc_ble_mesh_proxy_client_disconnect_cb(const bt_mesh_addr_t *addr, uint8_t conn_handle,
                                                    uint16_t net_idx, uint8_t reason)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    if (!addr || conn_handle >= BLE_MESH_MAX_CONN) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    BT_DBG("%s", __func__);

    mesh_param.proxy_client_disconnected.addr_type = addr->type;
    memcpy(mesh_param.proxy_client_disconnected.addr, addr->val, BD_ADDR_LEN);
    mesh_param.proxy_client_disconnected.conn_handle = conn_handle;
    mesh_param.proxy_client_disconnected.net_idx = net_idx;
    mesh_param.proxy_client_disconnected.reason = reason;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROXY_CLIENT_DISCONNECTED_EVT);
    return;
}

static void btc_ble_mesh_proxy_client_filter_status_recv_cb(uint8_t conn_handle, uint16_t src, uint16_t net_idx,
                                                            uint8_t filter_type, uint16_t list_size)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    if (conn_handle >= BLE_MESH_MAX_CONN) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    BT_DBG("%s", __func__);

    mesh_param.proxy_client_recv_filter_status.conn_handle = conn_handle;
    mesh_param.proxy_client_recv_filter_status.server_addr = src;
    mesh_param.proxy_client_recv_filter_status.net_idx = net_idx;
    mesh_param.proxy_client_recv_filter_status.filter_type = filter_type;
    mesh_param.proxy_client_recv_filter_status.list_size = list_size;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROXY_CLIENT_RECV_FILTER_STATUS_EVT);
    return;
}
#endif /* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */

#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
static void btc_ble_mesh_proxy_server_connect_cb(uint8_t conn_handle)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    if (conn_handle >= BLE_MESH_MAX_CONN) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    BT_DBG("%s", __func__);

    mesh_param.proxy_server_connected.conn_handle = conn_handle;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROXY_SERVER_CONNECTED_EVT);
}

static void btc_ble_mesh_proxy_server_disconnect_cb(uint8_t conn_handle, uint8_t reason)
{
    esp_ble_mesh_prov_cb_param_t mesh_param = {0};

    if (conn_handle >= BLE_MESH_MAX_CONN) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    BT_DBG("%s", __func__);

    mesh_param.proxy_server_disconnected.conn_handle = conn_handle;
    mesh_param.proxy_server_disconnected.reason = reason;

    btc_ble_mesh_prov_callback(&mesh_param, ESP_BLE_MESH_PROXY_SERVER_DISCONNECTED_EVT);
}
#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */

int btc_ble_mesh_client_model_init(esp_ble_mesh_model_t *model)
{
    if (!bt_mesh_is_initialized()) {
        BT_ERR("Mesh stack is not initialized!");
        return -EINVAL;
    }

    __ASSERT(model && model->op, "Invalid parameter");
    esp_ble_mesh_model_op_t *op = model->op;
    while (op != NULL && op->opcode != 0) {
        op->param_cb = (esp_ble_mesh_cb_t)btc_ble_mesh_client_model_op_cb;
        op++;
    }
    return bt_mesh_client_init((struct bt_mesh_model *)model);
}

#if CONFIG_BLE_MESH_DEINIT
int btc_ble_mesh_client_model_deinit(esp_ble_mesh_model_t *model)
{
    return bt_mesh_client_deinit((struct bt_mesh_model *)model);
}
#endif /* CONFIG_BLE_MESH_DEINIT */

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

esp_ble_mesh_elem_t *btc_ble_mesh_elem_find(uint16_t addr)
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
extern const struct bt_mesh_model_cb bt_mesh_cfg_srv_cb;
#if CONFIG_BLE_MESH_CFG_CLI
extern const struct bt_mesh_model_op bt_mesh_cfg_cli_op[];
extern const struct bt_mesh_model_cb bt_mesh_cfg_cli_cb;
#endif /* CONFIG_BLE_MESH_CFG_CLI */

/* Health Models */
#if CONFIG_BLE_MESH_HEALTH_SRV
extern const struct bt_mesh_model_op bt_mesh_health_srv_op[];
extern const struct bt_mesh_model_cb bt_mesh_health_srv_cb;
#endif /* CONFIG_BLE_MESH_HEALTH_SRV */
#if CONFIG_BLE_MESH_HEALTH_CLI
extern const struct bt_mesh_model_op bt_mesh_health_cli_op[];
extern const struct bt_mesh_model_cb bt_mesh_health_cli_cb;
#endif /* CONFIG_BLE_MESH_HEALTH_CLI */

/* Generic Client Models */
#if CONFIG_BLE_MESH_GENERIC_CLIENT
extern const struct bt_mesh_model_op bt_mesh_gen_onoff_cli_op[];
extern const struct bt_mesh_model_op bt_mesh_gen_level_cli_op[];
extern const struct bt_mesh_model_op bt_mesh_gen_def_trans_time_cli_op[];
extern const struct bt_mesh_model_op bt_mesh_gen_power_onoff_cli_op[];
extern const struct bt_mesh_model_op bt_mesh_gen_power_level_cli_op[];
extern const struct bt_mesh_model_op bt_mesh_gen_battery_cli_op[];
extern const struct bt_mesh_model_op bt_mesh_gen_location_cli_op[];
extern const struct bt_mesh_model_op bt_mesh_gen_property_cli_op[];
extern const struct bt_mesh_model_cb bt_mesh_generic_client_cb;
#endif /* CONFIG_BLE_MESH_GENERIC_CLIENT */

/* Lighting Client Models */
#if CONFIG_BLE_MESH_LIGHTING_CLIENT
extern const struct bt_mesh_model_op bt_mesh_light_lightness_cli_op[];
extern const struct bt_mesh_model_op bt_mesh_light_ctl_cli_op[];
extern const struct bt_mesh_model_op bt_mesh_light_hsl_cli_op[];
extern const struct bt_mesh_model_op bt_mesh_light_xyl_cli_op[];
extern const struct bt_mesh_model_op bt_mesh_light_lc_cli_op[];
extern const struct bt_mesh_model_cb bt_mesh_lighting_client_cb;
#endif /* CONFIG_BLE_MESH_LIGHTING_CLIENT */

/* Sensor Client Models */
#if CONFIG_BLE_MESH_SENSOR_CLI
extern const struct bt_mesh_model_op bt_mesh_sensor_cli_op[];
extern const struct bt_mesh_model_cb bt_mesh_sensor_client_cb;
#endif /* CONFIG_BLE_MESH_SENSOR_CLI */

/* Time and Scenes Client Models */
#if CONFIG_BLE_MESH_TIME_SCENE_CLIENT
extern const struct bt_mesh_model_op bt_mesh_time_cli_op[];
extern const struct bt_mesh_model_op bt_mesh_scene_cli_op[];
extern const struct bt_mesh_model_op bt_mesh_scheduler_cli_op[];
extern const struct bt_mesh_model_cb bt_mesh_time_scene_client_cb;
#endif /* CONFIG_BLE_MESH_TIME_SCENE_CLIENT */

/* Generic Server Models */
#if CONFIG_BLE_MESH_GENERIC_SERVER
extern const struct bt_mesh_model_op bt_mesh_gen_onoff_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_gen_level_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_gen_def_trans_time_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_gen_power_onoff_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_gen_power_onoff_setup_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_gen_power_level_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_gen_power_level_setup_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_gen_battery_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_gen_location_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_gen_location_setup_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_gen_user_prop_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_gen_admin_prop_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_gen_manu_prop_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_gen_client_prop_srv_op[];
extern const struct bt_mesh_model_cb bt_mesh_gen_onoff_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_gen_level_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_gen_def_trans_time_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_gen_power_onoff_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_gen_power_onoff_setup_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_gen_power_level_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_gen_power_level_setup_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_gen_battery_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_gen_location_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_gen_location_setup_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_gen_user_prop_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_gen_admin_prop_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_gen_manu_prop_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_gen_client_prop_srv_cb;
#endif /* CONFIG_BLE_MESH_GENERIC_SERVER */

/* Lighting Server Models */
#if CONFIG_BLE_MESH_LIGHTING_SERVER
extern const struct bt_mesh_model_op bt_mesh_light_lightness_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_light_lightness_setup_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_light_ctl_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_light_ctl_setup_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_light_ctl_temp_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_light_hsl_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_light_hsl_hue_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_light_hsl_sat_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_light_hsl_setup_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_light_xyl_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_light_xyl_setup_srv_op[];
extern const struct bt_mesh_model_cb bt_mesh_light_lightness_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_light_lightness_setup_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_light_ctl_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_light_ctl_setup_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_light_ctl_temp_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_light_hsl_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_light_hsl_hue_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_light_hsl_sat_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_light_hsl_setup_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_light_xyl_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_light_xyl_setup_srv_cb;
extern const struct bt_mesh_model_op bt_mesh_light_lc_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_light_lc_setup_srv_op[];
extern const struct bt_mesh_model_cb bt_mesh_light_lc_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_light_lc_setup_srv_cb;
#endif /* CONFIG_BLE_MESH_LIGHTING_SERVER */

/* Time and Scenes Server Models */
#if CONFIG_BLE_MESH_TIME_SCENE_SERVER
extern const struct bt_mesh_model_op bt_mesh_time_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_time_setup_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_scene_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_scene_setup_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_scheduler_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_scheduler_setup_srv_op[];
extern const struct bt_mesh_model_cb bt_mesh_time_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_time_setup_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_scene_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_scene_setup_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_scheduler_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_scheduler_setup_srv_cb;
#endif /* CONFIG_BLE_MESH_TIME_SCENE_SERVER */

/* Sensor Server Models */
#if CONFIG_BLE_MESH_SENSOR_SERVER
extern const struct bt_mesh_model_op bt_mesh_sensor_srv_op[];
extern const struct bt_mesh_model_op bt_mesh_sensor_setup_srv_op[];
extern const struct bt_mesh_model_cb bt_mesh_sensor_srv_cb;
extern const struct bt_mesh_model_cb bt_mesh_sensor_setup_srv_cb;
#endif /* CONFIG_BLE_MESH_SENSOR_SERVER */

static void btc_ble_mesh_model_op_set(esp_ble_mesh_model_t *model)
{
    if (!model) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    /* For SIG client and server models, model->op will be NULL and initialized here.
     * For vendor models whose opcode is 3 bytes, model->op will be initialized here.
     */
    if (model->op && BLE_MESH_MODEL_OP_LEN(model->op->opcode) == 3) {
        goto set_vnd_op;
    }

    switch (model->model_id) {
    case BLE_MESH_MODEL_ID_CFG_SRV: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_cfg_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_cfg_srv_cb;
        struct bt_mesh_cfg_srv *srv = (struct bt_mesh_cfg_srv *)model->user_data;
        if (srv) {
            srv->hb_sub.func = btc_ble_mesh_node_recv_heartbeat_cb;
        }
        break;
    }
#if CONFIG_BLE_MESH_CFG_CLI
    case BLE_MESH_MODEL_ID_CFG_CLI: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_cfg_cli_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_cfg_cli_cb;
        bt_mesh_config_client_t *cli = (bt_mesh_config_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_config_client_publish_callback;
        }
        break;
    }
#endif /* CONFIG_BLE_MESH_CFG_CLI */
#if CONFIG_BLE_MESH_HEALTH_SRV
    case BLE_MESH_MODEL_ID_HEALTH_SRV: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_health_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_health_srv_cb;
        struct bt_mesh_health_srv *srv = (struct bt_mesh_health_srv *)model->user_data;
        if (srv) {
            srv->cb.fault_clear = btc_ble_mesh_health_server_fault_clear;
            srv->cb.fault_test = btc_ble_mesh_health_server_fault_test;
            srv->cb.attn_on = btc_ble_mesh_health_server_attention_on;
            srv->cb.attn_off = btc_ble_mesh_health_server_attention_off;
        }
        break;
    }
#endif /* CONFIG_BLE_MESH_HEALTH_SRV */
#if CONFIG_BLE_MESH_HEALTH_CLI
    case BLE_MESH_MODEL_ID_HEALTH_CLI: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_health_cli_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_health_cli_cb;
        bt_mesh_health_client_t *cli = (bt_mesh_health_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_health_publish_callback;
        }
        break;
    }
#endif /* CONFIG_BLE_MESH_HEALTH_CLI */
#if CONFIG_BLE_MESH_GENERIC_CLIENT
    case BLE_MESH_MODEL_ID_GEN_ONOFF_CLI: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_gen_onoff_cli_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_generic_client_cb;
        bt_mesh_gen_onoff_client_t *cli = (bt_mesh_gen_onoff_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_generic_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_LEVEL_CLI: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_gen_level_cli_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_generic_client_cb;
        bt_mesh_gen_level_client_t *cli = (bt_mesh_gen_level_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_generic_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_CLI: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_gen_def_trans_time_cli_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_generic_client_cb;
        bt_mesh_gen_def_trans_time_client_t *cli = (bt_mesh_gen_def_trans_time_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_generic_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_CLI: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_gen_power_onoff_cli_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_generic_client_cb;
        bt_mesh_gen_power_onoff_client_t *cli = (bt_mesh_gen_power_onoff_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_generic_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_CLI: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_gen_power_level_cli_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_generic_client_cb;
        bt_mesh_gen_power_level_client_t *cli = (bt_mesh_gen_power_level_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_generic_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_BATTERY_CLI: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_gen_battery_cli_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_generic_client_cb;
        bt_mesh_gen_battery_client_t *cli = (bt_mesh_gen_battery_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_generic_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_LOCATION_CLI: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_gen_location_cli_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_generic_client_cb;
        bt_mesh_gen_location_client_t *cli = (bt_mesh_gen_location_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_generic_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_PROP_CLI: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_gen_property_cli_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_generic_client_cb;
        bt_mesh_gen_property_client_t *cli = (bt_mesh_gen_property_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_generic_client_publish_callback;
        }
        break;
    }
#endif /* CONFIG_BLE_MESH_GENERIC_CLIENT */
#if CONFIG_BLE_MESH_LIGHTING_CLIENT
    case BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_light_lightness_cli_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_lighting_client_cb;
        bt_mesh_light_lightness_client_t *cli = (bt_mesh_light_lightness_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_lighting_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_CTL_CLI: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_light_ctl_cli_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_lighting_client_cb;
        bt_mesh_light_ctl_client_t *cli = (bt_mesh_light_ctl_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_lighting_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_HSL_CLI: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_light_hsl_cli_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_lighting_client_cb;
        bt_mesh_light_hsl_client_t *cli = (bt_mesh_light_hsl_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_lighting_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_XYL_CLI: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_light_xyl_cli_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_lighting_client_cb;
        bt_mesh_light_xyl_client_t *cli = (bt_mesh_light_xyl_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_lighting_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_LC_CLI: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_light_lc_cli_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_lighting_client_cb;
        bt_mesh_light_lc_client_t *cli = (bt_mesh_light_lc_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_lighting_client_publish_callback;
        }
        break;
    }
#endif /* CONFIG_BLE_MESH_LIGHTING_CLIENT */
#if CONFIG_BLE_MESH_SENSOR_CLI
    case BLE_MESH_MODEL_ID_SENSOR_CLI: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_sensor_cli_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_sensor_client_cb;
        bt_mesh_sensor_client_t *cli = (bt_mesh_sensor_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_sensor_client_publish_callback;
        }
        break;
    }
#endif /* CONFIG_BLE_MESH_SENSOR_CLI */
#if CONFIG_BLE_MESH_TIME_SCENE_CLIENT
    case BLE_MESH_MODEL_ID_TIME_CLI: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_time_cli_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_time_scene_client_cb;
        bt_mesh_time_client_t *cli = (bt_mesh_time_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_time_scene_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_SCENE_CLI: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_scene_cli_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_time_scene_client_cb;
        bt_mesh_scene_client_t *cli = (bt_mesh_scene_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_time_scene_client_publish_callback;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_SCHEDULER_CLI: {
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_scheduler_cli_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_time_scene_client_cb;
        bt_mesh_scheduler_client_t *cli = (bt_mesh_scheduler_client_t *)model->user_data;
        if (cli != NULL) {
            cli->publish_status = btc_ble_mesh_time_scene_client_publish_callback;
        }
        break;
    }
#endif /* CONFIG_BLE_MESH_TIME_SCENE_CLIENT */
#if CONFIG_BLE_MESH_GENERIC_SERVER
    case BLE_MESH_MODEL_ID_GEN_ONOFF_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_gen_onoff_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_gen_onoff_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_LEVEL_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_gen_level_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_gen_level_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_gen_def_trans_time_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_gen_def_trans_time_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_gen_power_onoff_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_gen_power_onoff_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SETUP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_gen_power_onoff_setup_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_gen_power_onoff_setup_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_gen_power_level_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_gen_power_level_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SETUP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_gen_power_level_setup_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_gen_power_level_setup_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_BATTERY_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_gen_battery_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_gen_battery_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_LOCATION_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_gen_location_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_gen_location_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_USER_PROP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_gen_user_prop_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_gen_user_prop_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_ADMIN_PROP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_gen_admin_prop_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_gen_admin_prop_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_MANUFACTURER_PROP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_gen_manu_prop_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_gen_manu_prop_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_CLIENT_PROP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_gen_client_prop_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_gen_client_prop_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_GEN_LOCATION_SETUP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_gen_location_setup_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_gen_location_setup_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
#endif /* CONFIG_BLE_MESH_GENERIC_SERVER */
#if CONFIG_BLE_MESH_LIGHTING_SERVER
    case BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_light_lightness_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_light_lightness_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_light_lightness_setup_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_light_lightness_setup_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_CTL_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_light_ctl_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_light_ctl_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_light_ctl_setup_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_light_ctl_setup_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_light_ctl_temp_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_light_ctl_temp_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_HSL_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_light_hsl_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_light_hsl_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_HSL_HUE_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_light_hsl_hue_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_light_hsl_hue_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_HSL_SAT_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_light_hsl_sat_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_light_hsl_sat_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_light_hsl_setup_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_light_hsl_setup_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_XYL_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_light_xyl_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_light_xyl_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_XYL_SETUP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_light_xyl_setup_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_light_xyl_setup_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_LC_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_light_lc_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_light_lc_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_LIGHT_LC_SETUP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_light_lc_setup_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_light_lc_setup_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
#endif /* CONFIG_BLE_MESH_LIGHTING_SERVER */
#if CONFIG_BLE_MESH_TIME_SCENE_SERVER
    case BLE_MESH_MODEL_ID_TIME_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_time_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_time_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_TIME_SETUP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_time_setup_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_time_setup_srv_cb;
        if (model->pub) {
            /* Time Setup Server model does not support subscribing nor publishing. */
            BT_ERR("Time Setup Server shall not support publication");
            return;
        }
        break;
    case BLE_MESH_MODEL_ID_SCENE_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_scene_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_scene_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_SCENE_SETUP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_scene_setup_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_scene_setup_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_SCHEDULER_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_scheduler_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_scheduler_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_SCHEDULER_SETUP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_scheduler_setup_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_scheduler_setup_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
#endif /* CONFIG_BLE_MESH_TIME_SCENE_SERVER */
#if CONFIG_BLE_MESH_SENSOR_SERVER
    case BLE_MESH_MODEL_ID_SENSOR_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_sensor_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_sensor_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
    case BLE_MESH_MODEL_ID_SENSOR_SETUP_SRV:
        model->op = (esp_ble_mesh_model_op_t *)bt_mesh_sensor_setup_srv_op;
        model->cb = (esp_ble_mesh_model_cbs_t *)&bt_mesh_sensor_setup_srv_cb;
        if (model->pub) {
            model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
        }
        break;
#endif /* CONFIG_BLE_MESH_SENSOR_SERVER */
    default:
        goto set_vnd_op;
    }
    return;

set_vnd_op:
    if (model->pub) {
        model->pub->update = (esp_ble_mesh_cb_t)btc_ble_mesh_model_publish_update;
    }
    esp_ble_mesh_model_op_t *op = model->op;
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
    uint8_t act = 0U;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_prov_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_MESH_INIT: {
        for (int i = 0; i < arg->mesh_init.comp->element_count; i++) {
            esp_ble_mesh_elem_t *elem = &arg->mesh_init.comp->elements[i];

            for (int j = 0; j < elem->sig_model_count; j++) {
                esp_ble_mesh_model_t *sig_model = &elem->sig_models[j];
                if (sig_model->op && BLE_MESH_MODEL_OP_LEN(sig_model->op->opcode) == 3) {
                    /* Opcode of SIG model must be 1 or 2 bytes. */
                    btc_ble_mesh_prov_register_complete_cb(-EINVAL);
                    return;
                }
                btc_ble_mesh_model_op_set(sig_model);
            }

            for (int k = 0; k < elem->vnd_model_count; k++) {
                esp_ble_mesh_model_t *vnd_model = &elem->vnd_models[k];
                if (vnd_model->op && BLE_MESH_MODEL_OP_LEN(vnd_model->op->opcode) < 3) {
                    /* Opcode of vendor model must be 3 bytes. */
                    btc_ble_mesh_prov_register_complete_cb(-EINVAL);
                    return;
                }
                btc_ble_mesh_model_op_set(vnd_model);
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
#endif /* CONFIG_BLE_MESH_NODE */
#if CONFIG_BLE_MESH_PROVISIONER
        arg->mesh_init.prov->provisioner_prov_read_oob_pub_key = (esp_ble_mesh_cb_t)btc_ble_mesh_provisioner_prov_read_oob_pub_key_cb;
        arg->mesh_init.prov->provisioner_prov_input = (esp_ble_mesh_cb_t)btc_ble_mesh_provisioner_prov_input_cb;
        arg->mesh_init.prov->provisioner_prov_output = (esp_ble_mesh_cb_t)btc_ble_mesh_provisioner_prov_output_cb;
        arg->mesh_init.prov->provisioner_link_open = (esp_ble_mesh_cb_t)btc_ble_mesh_provisioner_link_open_cb;
        arg->mesh_init.prov->provisioner_link_close = (esp_ble_mesh_cb_t)btc_ble_mesh_provisioner_link_close_cb;
        arg->mesh_init.prov->provisioner_prov_comp = (esp_ble_mesh_cb_t)btc_ble_mesh_provisioner_prov_complete_cb;
        bt_mesh_provisioner_adv_pkt_cb_register(btc_ble_mesh_provisioner_recv_unprov_adv_pkt_cb);
#endif /* CONFIG_BLE_MESH_PROVISIONER */
#if CONFIG_BLE_MESH_LOW_POWER
        bt_mesh_lpn_set_cb(btc_ble_mesh_lpn_cb);
#endif /* CONFIG_BLE_MESH_LOW_POWER */
#if CONFIG_BLE_MESH_FRIEND
        bt_mesh_friend_set_cb(btc_ble_mesh_friend_cb);
#endif /* CONFIG_BLE_MESH_FRIEND */
#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
        bt_mesh_proxy_client_set_adv_recv_cb(btc_ble_mesh_proxy_client_adv_recv_cb);
        bt_mesh_proxy_client_set_conn_cb(btc_ble_mesh_proxy_client_connect_cb);
        bt_mesh_proxy_client_set_disconn_cb(btc_ble_mesh_proxy_client_disconnect_cb);
        bt_mesh_proxy_client_set_filter_status_cb(btc_ble_mesh_proxy_client_filter_status_recv_cb);
#endif /* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */
#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
        bt_mesh_proxy_server_set_conn_cb(btc_ble_mesh_proxy_server_connect_cb);
        bt_mesh_proxy_server_set_disconn_cb(btc_ble_mesh_proxy_server_disconnect_cb);
#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */
        int err_code = bt_mesh_init((struct bt_mesh_prov *)arg->mesh_init.prov,
                                    (struct bt_mesh_comp *)arg->mesh_init.comp);
        /* Give the semaphore when BLE Mesh initialization is finished. */
        xSemaphoreGive(arg->mesh_init.semaphore);
        btc_ble_mesh_prov_register_complete_cb(err_code);
        return;
    }
#if CONFIG_BLE_MESH_NODE
    case BTC_BLE_MESH_ACT_PROV_ENABLE:
        act = ESP_BLE_MESH_NODE_PROV_ENABLE_COMP_EVT;
        param.node_prov_enable_comp.err_code = bt_mesh_prov_enable(arg->node_prov_enable.bearers);
        break;
    case BTC_BLE_MESH_ACT_PROV_DISABLE:
        act = ESP_BLE_MESH_NODE_PROV_DISABLE_COMP_EVT;
        param.node_prov_disable_comp.err_code = bt_mesh_prov_disable(arg->node_prov_disable.bearers);
        break;
    case BTC_BLE_MESH_ACT_NODE_RESET:
        bt_mesh_node_reset();
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
    case BTC_BLE_MESH_ACT_NODE_ADD_LOCAL_NET_KEY:
        act = ESP_BLE_MESH_NODE_ADD_LOCAL_NET_KEY_COMP_EVT;
        param.node_add_net_key_comp.net_idx = arg->node_add_local_net_key.net_idx;
        param.node_add_net_key_comp.err_code =
            bt_mesh_node_local_net_key_add(arg->node_add_local_net_key.net_idx,
                                           arg->node_add_local_net_key.net_key);
        break;
    case BTC_BLE_MESH_ACT_NODE_ADD_LOCAL_APP_KEY:
        act = ESP_BLE_MESH_NODE_ADD_LOCAL_APP_KEY_COMP_EVT;
        param.node_add_app_key_comp.net_idx = arg->node_add_local_app_key.net_idx;
        param.node_add_app_key_comp.app_idx = arg->node_add_local_app_key.app_idx;
        param.node_add_app_key_comp.err_code =
            bt_mesh_node_local_app_key_add(arg->node_add_local_app_key.net_idx,
                                           arg->node_add_local_app_key.app_idx,
                                           arg->node_add_local_app_key.app_key);
        break;
    case BTC_BLE_MESH_ACT_NODE_BIND_APP_KEY_TO_MODEL:
        act = ESP_BLE_MESH_NODE_BIND_APP_KEY_TO_MODEL_COMP_EVT;
        param.node_bind_app_key_to_model_comp.element_addr = arg->node_local_mod_app_bind.element_addr;
        param.node_bind_app_key_to_model_comp.model_id = arg->node_local_mod_app_bind.model_id;
        param.node_bind_app_key_to_model_comp.company_id = arg->node_local_mod_app_bind.company_id;
        param.node_bind_app_key_to_model_comp.app_idx = arg->node_local_mod_app_bind.app_idx;
        param.node_bind_app_key_to_model_comp.err_code =
            bt_mesh_node_bind_app_key_to_model(arg->node_local_mod_app_bind.element_addr,
                                               arg->node_local_mod_app_bind.model_id,
                                               arg->node_local_mod_app_bind.company_id,
                                               arg->node_local_mod_app_bind.app_idx);
        break;
#endif /* CONFIG_BLE_MESH_NODE */
#if (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
    CONFIG_BLE_MESH_GATT_PROXY_SERVER
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
        param.node_proxy_gatt_enable_comp.err_code = bt_mesh_proxy_server_gatt_enable();
        break;
    case BTC_BLE_MESH_ACT_PROXY_GATT_DISABLE:
        act = ESP_BLE_MESH_NODE_PROXY_GATT_DISABLE_COMP_EVT;
        param.node_proxy_gatt_disable_comp.err_code = bt_mesh_proxy_server_gatt_disable();
        break;
#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */
#endif /* (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || CONFIG_BLE_MESH_GATT_PROXY_SERVER */
#if CONFIG_BLE_MESH_PROVISIONER
    case BTC_BLE_MESH_ACT_PROVISIONER_READ_OOB_PUB_KEY:
        act = ESP_BLE_MESH_PROVISIONER_PROV_READ_OOB_PUB_KEY_COMP_EVT;
        param.provisioner_prov_read_oob_pub_key_comp.err_code =
            bt_mesh_provisioner_read_oob_pub_key(arg->provisioner_read_oob_pub_key.link_idx,
                                                 arg->provisioner_read_oob_pub_key.pub_key_x,
                                                 arg->provisioner_read_oob_pub_key.pub_key_y);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_INPUT_STR:
        act = ESP_BLE_MESH_PROVISIONER_PROV_INPUT_STRING_COMP_EVT;
        param.provisioner_prov_input_str_comp.err_code =
            bt_mesh_provisioner_set_oob_input_data(arg->provisioner_input_str.link_idx,
                                                   (const uint8_t *)&arg->provisioner_input_str.string,
                                                   false);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_INPUT_NUM:
        act = ESP_BLE_MESH_PROVISIONER_PROV_INPUT_NUMBER_COMP_EVT;
        param.provisioner_prov_input_num_comp.err_code =
            bt_mesh_provisioner_set_oob_input_data(arg->provisioner_input_num.link_idx,
                                                   (const uint8_t *)&arg->provisioner_input_num.number,
                                                   true);
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
    case BTC_BLE_MESH_ACT_PROVISIONER_PROV_DEV_WITH_ADDR:
        act = ESP_BLE_MESH_PROVISIONER_PROV_DEV_WITH_ADDR_COMP_EVT;
        param.provisioner_prov_dev_with_addr_comp.err_code =
            bt_mesh_provisioner_prov_device_with_addr(arg->provisioner_prov_dev_with_addr.uuid,
                arg->provisioner_prov_dev_with_addr.addr, arg->provisioner_prov_dev_with_addr.addr_type,
                arg->provisioner_prov_dev_with_addr.bearer, arg->provisioner_prov_dev_with_addr.oob_info,
                arg->provisioner_prov_dev_with_addr.unicast_addr);
        break;
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
    case BTC_BLE_MESH_ACT_PROVISIONER_SET_STATIC_OOB_VAL:
        act = ESP_BLE_MESH_PROVISIONER_SET_STATIC_OOB_VALUE_COMP_EVT;
        param.provisioner_set_static_oob_val_comp.err_code =
            bt_mesh_provisioner_set_static_oob_value(
                arg->set_static_oob_val.value, arg->set_static_oob_val.length);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_SET_PRIMARY_ELEM_ADDR:
        act = ESP_BLE_MESH_PROVISIONER_SET_PRIMARY_ELEM_ADDR_COMP_EVT;
        param.provisioner_set_primary_elem_addr_comp.err_code =
            bt_mesh_provisioner_set_primary_elem_addr(arg->set_primary_elem_addr.addr);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_SET_NODE_NAME:
        act = ESP_BLE_MESH_PROVISIONER_SET_NODE_NAME_COMP_EVT;
        param.provisioner_set_node_name_comp.node_index = arg->set_node_name.index;
        param.provisioner_set_node_name_comp.err_code =
            bt_mesh_provisioner_set_node_name(arg->set_node_name.index, arg->set_node_name.name);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_ADD_LOCAL_APP_KEY: {
        const uint8_t *app_key = NULL;
        const uint8_t zero[16] = {0};
        if (memcmp(arg->add_local_app_key.app_key, zero, 16)) {
            app_key = arg->add_local_app_key.app_key;
        }
        act = ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT;
        param.provisioner_add_app_key_comp.err_code =
            bt_mesh_provisioner_local_app_key_add(app_key, arg->add_local_app_key.net_idx,
                    &arg->add_local_app_key.app_idx);
        param.provisioner_add_app_key_comp.net_idx = arg->add_local_app_key.net_idx;
        param.provisioner_add_app_key_comp.app_idx = arg->add_local_app_key.app_idx;
        break;
    }
    case BTC_BLE_MESH_ACT_PROVISIONER_UPDATE_LOCAL_APP_KEY:
        act = ESP_BLE_MESH_PROVISIONER_UPDATE_LOCAL_APP_KEY_COMP_EVT;
        param.provisioner_update_app_key_comp.net_idx = arg->update_local_app_key.net_idx;
        param.provisioner_update_app_key_comp.app_idx = arg->update_local_app_key.app_idx;
        param.provisioner_update_app_key_comp.err_code =
            bt_mesh_provisioner_local_app_key_update(arg->update_local_app_key.app_key,
                    arg->update_local_app_key.net_idx, arg->update_local_app_key.app_idx);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_BIND_LOCAL_MOD_APP:
        act = ESP_BLE_MESH_PROVISIONER_BIND_APP_KEY_TO_MODEL_COMP_EVT;
        param.provisioner_bind_app_key_to_model_comp.element_addr = arg->local_mod_app_bind.elem_addr;
        param.provisioner_bind_app_key_to_model_comp.app_idx = arg->local_mod_app_bind.app_idx;
        param.provisioner_bind_app_key_to_model_comp.company_id = arg->local_mod_app_bind.cid;
        param.provisioner_bind_app_key_to_model_comp.model_id = arg->local_mod_app_bind.model_id;
        param.provisioner_bind_app_key_to_model_comp.err_code =
            bt_mesh_provisioner_bind_local_model_app_idx(arg->local_mod_app_bind.elem_addr,
                    arg->local_mod_app_bind.model_id,
                    arg->local_mod_app_bind.cid,
                    arg->local_mod_app_bind.app_idx);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_ADD_LOCAL_NET_KEY: {
        const uint8_t *net_key = NULL;
        const uint8_t zero[16] = {0};
        if (memcmp(arg->add_local_net_key.net_key, zero, 16)) {
            net_key = arg->add_local_net_key.net_key;
        }
        act = ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_NET_KEY_COMP_EVT;
        param.provisioner_add_net_key_comp.err_code =
            bt_mesh_provisioner_local_net_key_add(net_key, &arg->add_local_net_key.net_idx);
        param.provisioner_add_net_key_comp.net_idx = arg->add_local_net_key.net_idx;
        break;
    }
    case BTC_BLE_MESH_ACT_PROVISIONER_UPDATE_LOCAL_NET_KEY:
        act = ESP_BLE_MESH_PROVISIONER_UPDATE_LOCAL_NET_KEY_COMP_EVT;
        param.provisioner_update_net_key_comp.net_idx = arg->update_local_net_key.net_idx;
        param.provisioner_update_net_key_comp.err_code =
            bt_mesh_provisioner_local_net_key_update(arg->update_local_net_key.net_key,
                arg->update_local_net_key.net_idx);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_STORE_NODE_COMP_DATA:
        act = ESP_BLE_MESH_PROVISIONER_STORE_NODE_COMP_DATA_COMP_EVT;
        param.provisioner_store_node_comp_data_comp.addr = arg->store_node_comp_data.unicast_addr;
        param.provisioner_store_node_comp_data_comp.err_code =
            bt_mesh_provisioner_store_node_comp_data(arg->store_node_comp_data.unicast_addr,
                arg->store_node_comp_data.data, arg->store_node_comp_data.length);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_DELETE_NODE_WITH_UUID:
        act = ESP_BLE_MESH_PROVISIONER_DELETE_NODE_WITH_UUID_COMP_EVT;
        memcpy(param.provisioner_delete_node_with_uuid_comp.uuid, arg->delete_node_with_uuid.uuid, 16);
        param.provisioner_delete_node_with_uuid_comp.err_code =
            bt_mesh_provisioner_delete_node_with_uuid(arg->delete_node_with_uuid.uuid);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_DELETE_NODE_WITH_ADDR:
        act = ESP_BLE_MESH_PROVISIONER_DELETE_NODE_WITH_ADDR_COMP_EVT;
        param.provisioner_delete_node_with_addr_comp.unicast_addr = arg->delete_node_with_addr.unicast_addr;
        param.provisioner_delete_node_with_addr_comp.err_code =
            bt_mesh_provisioner_delete_node_with_node_addr(arg->delete_node_with_addr.unicast_addr);
        break;
#if CONFIG_BLE_MESH_PROVISIONER_RECV_HB
    case BTC_BLE_MESH_ACT_PROVISIONER_ENABLE_HEARTBEAT_RECV:
        act = ESP_BLE_MESH_PROVISIONER_ENABLE_HEARTBEAT_RECV_COMP_EVT;
        param.provisioner_enable_heartbeat_recv_comp.enable = arg->enable_heartbeat_recv.enable;
        param.provisioner_enable_heartbeat_recv_comp.err_code =
            bt_mesh_provisioner_recv_heartbeat(arg->enable_heartbeat_recv.enable ?
                                               btc_ble_mesh_provisioner_recv_heartbeat_cb : NULL);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_SET_HEARTBEAT_FILTER_TYPE:
        act = ESP_BLE_MESH_PROVISIONER_SET_HEARTBEAT_FILTER_TYPE_COMP_EVT;
        param.provisioner_set_heartbeat_filter_type_comp.type = arg->set_heartbeat_filter_type.type;
        param.provisioner_set_heartbeat_filter_type_comp.err_code =
            bt_mesh_provisioner_set_heartbeat_filter_type(arg->set_heartbeat_filter_type.type);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_SET_HEARTBEAT_FILTER_INFO:
        act = ESP_BLE_MESH_PROVISIONER_SET_HEARTBEAT_FILTER_INFO_COMP_EVT;
        param.provisioner_set_heartbeat_filter_info_comp.op = arg->set_heartbeat_filter_info.op;
        param.provisioner_set_heartbeat_filter_info_comp.hb_src = arg->set_heartbeat_filter_info.hb_src;
        param.provisioner_set_heartbeat_filter_info_comp.hb_dst = arg->set_heartbeat_filter_info.hb_dst;
        param.provisioner_set_heartbeat_filter_info_comp.err_code =
            bt_mesh_provisioner_set_heartbeat_filter_info(arg->set_heartbeat_filter_info.op,
                                                          arg->set_heartbeat_filter_info.hb_src,
                                                          arg->set_heartbeat_filter_info.hb_dst);
        break;
#endif /* CONFIG_BLE_MESH_PROVISIONER_RECV_HB */
#if CONFIG_BLE_MESH_SETTINGS
    case BTC_BLE_MESH_ACT_PROVISIONER_DIRECT_ERASE_SETTINGS:
        act = ESP_BLE_MESH_PROVISIONER_DIRECT_ERASE_SETTINGS_COMP_EVT;
        param.provisioner_direct_erase_settings_comp.err_code = bt_mesh_provisioner_direct_erase_settings();
        break;
#endif /* CONFIG_BLE_MESH_SETTINGS */
#if CONFIG_BLE_MESH_USE_MULTIPLE_NAMESPACE
    case BTC_BLE_MESH_ACT_PROVISIONER_OPEN_SETTINGS_WITH_INDEX:
        act = ESP_BLE_MESH_PROVISIONER_OPEN_SETTINGS_WITH_INDEX_COMP_EVT;
        param.provisioner_open_settings_with_index_comp.index = arg->open_settings_with_index.index;
        param.provisioner_open_settings_with_index_comp.err_code =
            bt_mesh_provisioner_open_settings_with_index(arg->open_settings_with_index.index);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_OPEN_SETTINGS_WITH_UID:
        act = ESP_BLE_MESH_PROVISIONER_OPEN_SETTINGS_WITH_UID_COMP_EVT;
        strncpy(param.provisioner_open_settings_with_uid_comp.uid,
                arg->open_settings_with_uid.uid, ESP_BLE_MESH_SETTINGS_UID_SIZE + 1);
        param.provisioner_open_settings_with_uid_comp.err_code =
            bt_mesh_provisioner_open_settings_with_uid(arg->open_settings_with_uid.uid,
                                                       &param.provisioner_open_settings_with_uid_comp.index);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_CLOSE_SETTINGS_WITH_INDEX:
        act = ESP_BLE_MESH_PROVISIONER_CLOSE_SETTINGS_WITH_INDEX_COMP_EVT;
        param.provisioner_close_settings_with_index_comp.index = arg->close_settings_with_index.index;
        param.provisioner_close_settings_with_index_comp.err_code =
            bt_mesh_provisioner_close_settings_with_index(arg->close_settings_with_index.index,
                                                          arg->close_settings_with_index.erase);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_CLOSE_SETTINGS_WITH_UID:
        act = ESP_BLE_MESH_PROVISIONER_CLOSE_SETTINGS_WITH_UID_COMP_EVT;
        strncpy(param.provisioner_close_settings_with_uid_comp.uid,
                arg->close_settings_with_uid.uid, ESP_BLE_MESH_SETTINGS_UID_SIZE + 1);
        param.provisioner_close_settings_with_uid_comp.err_code =
            bt_mesh_provisioner_close_settings_with_uid(arg->close_settings_with_uid.uid,
                                                        arg->close_settings_with_uid.erase,
                                                        &param.provisioner_close_settings_with_uid_comp.index);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_DELETE_SETTINGS_WITH_INDEX:
        act = ESP_BLE_MESH_PROVISIONER_DELETE_SETTINGS_WITH_INDEX_COMP_EVT;
        param.provisioner_delete_settings_with_index_comp.index = arg->delete_settings_with_index.index;
        param.provisioner_delete_settings_with_index_comp.err_code =
            bt_mesh_provisioner_delete_settings_with_index(arg->delete_settings_with_index.index);
        break;
    case BTC_BLE_MESH_ACT_PROVISIONER_DELETE_SETTINGS_WITH_UID:
        act = ESP_BLE_MESH_PROVISIONER_DELETE_SETTINGS_WITH_UID_COMP_EVT;
        strncpy(param.provisioner_delete_settings_with_uid_comp.uid,
                arg->delete_settings_with_uid.uid, ESP_BLE_MESH_SETTINGS_UID_SIZE + 1);
        param.provisioner_delete_settings_with_uid_comp.err_code =
            bt_mesh_provisioner_delete_settings_with_uid(arg->delete_settings_with_uid.uid,
                                                         &param.provisioner_delete_settings_with_uid_comp.index);
        break;
#endif /* CONFIG_BLE_MESH_USE_MULTIPLE_NAMESPACE */
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
            bt_mesh_proxy_client_cfg_send(arg->proxy_client_set_filter_type.conn_handle,
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
            bt_mesh_proxy_client_cfg_send(arg->proxy_client_add_filter_addr.conn_handle,
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
            bt_mesh_proxy_client_cfg_send(arg->proxy_client_remove_filter_addr.conn_handle,
                                          arg->proxy_client_remove_filter_addr.net_idx, &pdu);
        break;
    }
#endif /* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */
    case BTC_BLE_MESH_ACT_MODEL_SUBSCRIBE_GROUP_ADDR:
        act = ESP_BLE_MESH_MODEL_SUBSCRIBE_GROUP_ADDR_COMP_EVT;
        param.model_sub_group_addr_comp.element_addr = arg->model_sub_group_addr.element_addr;
        param.model_sub_group_addr_comp.company_id = arg->model_sub_group_addr.company_id;
        param.model_sub_group_addr_comp.model_id = arg->model_sub_group_addr.model_id;
        param.model_sub_group_addr_comp.group_addr = arg->model_sub_group_addr.group_addr;
        param.model_sub_group_addr_comp.err_code =
            bt_mesh_model_subscribe_group_addr(arg->model_sub_group_addr.element_addr,
                                               arg->model_sub_group_addr.company_id,
                                               arg->model_sub_group_addr.model_id,
                                               arg->model_sub_group_addr.group_addr);
        break;
    case BTC_BLE_MESH_ACT_MODEL_UNSUBSCRIBE_GROUP_ADDR:
        act = ESP_BLE_MESH_MODEL_UNSUBSCRIBE_GROUP_ADDR_COMP_EVT;
        param.model_unsub_group_addr_comp.element_addr = arg->model_unsub_group_addr.element_addr;
        param.model_unsub_group_addr_comp.company_id = arg->model_unsub_group_addr.company_id;
        param.model_unsub_group_addr_comp.model_id = arg->model_unsub_group_addr.model_id;
        param.model_unsub_group_addr_comp.group_addr = arg->model_unsub_group_addr.group_addr;
        param.model_unsub_group_addr_comp.err_code =
            bt_mesh_model_unsubscribe_group_addr(arg->model_unsub_group_addr.element_addr,
                                                 arg->model_unsub_group_addr.company_id,
                                                 arg->model_unsub_group_addr.model_id,
                                                 arg->model_unsub_group_addr.group_addr);
        break;
#if CONFIG_BLE_MESH_DEINIT
    case BTC_BLE_MESH_ACT_DEINIT_MESH:
        act = ESP_BLE_MESH_DEINIT_MESH_COMP_EVT;
        param.deinit_mesh_comp.err_code = bt_mesh_deinit((struct bt_mesh_deinit_param *)&arg->mesh_deinit.param);
        /* Temporarily save the deinit semaphore and release it after the mesh deinit complete event is handled in the app layer */
        deinit_comp_semaphore = arg->mesh_deinit.semaphore;
        break;
#endif /* CONFIG_BLE_MESH_DEINIT */
    default:
        BT_WARN("%s, Unknown act %d", __func__, msg->act);
        return;
    }

    /* Callback operation completion events */
    btc_ble_mesh_prov_set_complete_cb(&param, act);

    btc_ble_mesh_prov_arg_deep_free(msg);
    return;
}

void btc_ble_mesh_prov_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_prov_cb_param_t *param = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    param = (esp_ble_mesh_prov_cb_param_t *)(msg->arg);

    if (msg->act < ESP_BLE_MESH_PROV_EVT_MAX) {
        btc_ble_mesh_prov_cb_to_app(msg->act, param);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }
}

void btc_ble_mesh_model_call_handler(btc_msg_t *msg)
{
    btc_ble_mesh_model_args_t *arg = NULL;
    int err = 0;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_model_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_MODEL_PUBLISH: {
        if (arg->model_publish.device_role == PROVISIONER) {
            /* Currently Provisioner only supports client model */
            err = bt_mesh_set_client_model_role((struct bt_mesh_model *)arg->model_publish.model,
                                                arg->model_publish.device_role);
            if (err) {
                BT_ERR("Failed to set client role");
                btc_ble_mesh_model_publish_comp_cb(arg->model_publish.model, err);
                break;
            }
        }
        err = bt_mesh_model_publish((struct bt_mesh_model *)arg->model_publish.model);
        btc_ble_mesh_model_publish_comp_cb(arg->model_publish.model, err);
        break;
    }
    case BTC_BLE_MESH_ACT_SERVER_MODEL_SEND: {
        /* arg->model_send.length contains opcode & payload, plus extra 4-bytes TransMIC */
        struct net_buf_simple *buf = bt_mesh_alloc_buf(arg->model_send.length + BLE_MESH_MIC_SHORT);
        if (!buf) {
            BT_ERR("%s, Out of memory", __func__);
            btc_ble_mesh_model_send_comp_cb(arg->model_send.model, arg->model_send.ctx,
                                            arg->model_send.opcode, -ENOMEM);
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
        /* arg->model_send.length contains opcode & message, plus extra 4-bytes TransMIC */
        struct net_buf_simple *buf = bt_mesh_alloc_buf(arg->model_send.length + BLE_MESH_MIC_SHORT);
        if (!buf) {
            BT_ERR("%s, Out of memory", __func__);
            btc_ble_mesh_model_send_comp_cb(arg->model_send.model, arg->model_send.ctx,
                                            arg->model_send.opcode, -ENOMEM);
            break;
        }

        net_buf_simple_add_mem(buf, arg->model_send.data, arg->model_send.length);
        bt_mesh_client_common_param_t param = {
            .opcode = arg->model_send.opcode,
            .model = (struct bt_mesh_model *)arg->model_send.model,
            .ctx.net_idx = arg->model_send.ctx->net_idx,
            .ctx.app_idx = arg->model_send.ctx->app_idx,
            .ctx.addr = arg->model_send.ctx->addr,
            .ctx.send_rel = arg->model_send.ctx->send_rel,
            .ctx.send_ttl = arg->model_send.ctx->send_ttl,
            .ctx.srv_send = false,
            .msg_timeout = arg->model_send.msg_timeout,
            .msg_role = arg->model_send.device_role,
        };
        err = bt_mesh_client_send_msg(&param, buf, arg->model_send.need_rsp,
                                      btc_ble_mesh_client_model_timeout_cb);
        bt_mesh_free_buf(buf);
        btc_ble_mesh_model_send_comp_cb(arg->model_send.model, arg->model_send.ctx,
                                        arg->model_send.opcode, err);
        break;
    }
#if CONFIG_BLE_MESH_SERVER_MODEL
    case BTC_BLE_MESH_ACT_SERVER_MODEL_UPDATE_STATE:
        err = bt_mesh_update_binding_state(
                  (struct bt_mesh_model *)arg->model_update_state.model, arg->model_update_state.type,
                  (bt_mesh_server_state_value_t *)arg->model_update_state.value);
        btc_ble_mesh_server_model_update_state_comp_cb(arg->model_update_state.model,
                arg->model_update_state.type, err);
        break;
#endif /* CONFIG_BLE_MESH_SERVER_MODEL */
    default:
        BT_WARN("%s, Unknown act %d", __func__, msg->act);
        break;
    }

    btc_ble_mesh_model_arg_deep_free(msg);
    return;
}

void btc_ble_mesh_model_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_model_cb_param_t *param = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    param = (esp_ble_mesh_model_cb_param_t *)(msg->arg);

    if (msg->act < ESP_BLE_MESH_MODEL_EVT_MAX) {
        btc_ble_mesh_model_cb_to_app(msg->act, param);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }

    btc_ble_mesh_model_free_req_data(msg);
    return;
}

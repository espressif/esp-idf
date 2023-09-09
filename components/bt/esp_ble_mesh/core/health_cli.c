/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "btc_ble_mesh_health_model.h"

#include "mesh/config.h"
#include "foundation.h"
#include "mesh/common.h"

#if CONFIG_BLE_MESH_HEALTH_CLI
#include "mesh/health_cli.h"

static const bt_mesh_client_op_pair_t health_op_pair[] = {
    { OP_HEALTH_FAULT_GET,   OP_HEALTH_FAULT_STATUS  },
    { OP_HEALTH_FAULT_CLEAR, OP_HEALTH_FAULT_STATUS  },
    { OP_HEALTH_FAULT_TEST,  OP_HEALTH_FAULT_STATUS  },
    { OP_HEALTH_PERIOD_GET,  OP_HEALTH_PERIOD_STATUS },
    { OP_HEALTH_PERIOD_SET,  OP_HEALTH_PERIOD_STATUS },
    { OP_ATTENTION_GET,      OP_ATTENTION_STATUS     },
    { OP_ATTENTION_SET,      OP_ATTENTION_STATUS     },
};

static bt_mesh_mutex_t health_client_lock;

static void timeout_handler(struct k_work *work)
{
    struct k_delayed_work *timer = NULL;
    bt_mesh_client_node_t *node = NULL;
    struct bt_mesh_model *model = NULL;
    struct bt_mesh_msg_ctx ctx = {0};
    uint32_t opcode = 0U;

    BT_WARN("Receive health status message timeout");

    bt_mesh_mutex_lock(&health_client_lock);

    timer = CONTAINER_OF(work, struct k_delayed_work, work);

    if (timer && !k_delayed_work_free(timer)) {
        node = CONTAINER_OF(work, bt_mesh_client_node_t, timer.work);
        if (node) {
            memcpy(&ctx, &node->ctx, sizeof(ctx));
            opcode = node->opcode;
            model = node->model;
            bt_mesh_client_free_node(node);
            bt_mesh_health_client_cb_evt_to_btc(
                opcode, BTC_BLE_MESH_EVT_HEALTH_CLIENT_TIMEOUT, model, &ctx, NULL, 0);
        }
    }

    bt_mesh_mutex_unlock(&health_client_lock);
}

static void health_client_recv_status(struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx,
                                      void *status, size_t len)
{
    bt_mesh_client_node_t *node = NULL;
    struct net_buf_simple buf = {0};
    uint8_t evt_type = 0xFF;

    if (!model || !ctx || !status || !len) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    /* If it is a publish message, sent to the user directly. */
    buf.data = (uint8_t *)status;
    buf.len  = (uint16_t)len;

    bt_mesh_mutex_lock(&health_client_lock);

    node = bt_mesh_is_client_recv_publish_msg(model, ctx, &buf, true);
    if (!node) {
        BT_DBG("Unexpected Health Status 0x%04x", ctx->recv_op);
    } else {
        switch (node->opcode) {
        case OP_HEALTH_FAULT_GET:
        case OP_HEALTH_PERIOD_GET:
        case OP_ATTENTION_GET:
            evt_type = BTC_BLE_MESH_EVT_HEALTH_CLIENT_GET_STATE;
            break;
        case OP_HEALTH_FAULT_CLEAR:
        case OP_HEALTH_FAULT_TEST:
        case OP_HEALTH_PERIOD_SET:
        case OP_ATTENTION_SET:
            evt_type = BTC_BLE_MESH_EVT_HEALTH_CLIENT_SET_STATE;
            break;
        default:
            break;
        }

        if (!k_delayed_work_free(&node->timer)) {
            uint32_t opcode = node->opcode;
            bt_mesh_client_free_node(node);
            bt_mesh_health_client_cb_evt_to_btc(
                opcode, evt_type, model, ctx, (const uint8_t *)status, len);
        }
    }

    bt_mesh_mutex_unlock(&health_client_lock);

    switch (ctx->recv_op) {
    case OP_HEALTH_FAULT_STATUS: {
        struct bt_mesh_health_fault_status *val = status;
        bt_mesh_free_buf(val->fault_array);
        break;
    }
    case OP_HEALTH_CURRENT_STATUS: {
        struct bt_mesh_health_current_status *val = status;
        bt_mesh_free_buf(val->fault_array);
        break;
    }
    default:
        break;
    }
}

static void health_fault_status(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf)
{
    struct bt_mesh_health_fault_status status = {0};

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
           ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
           bt_hex(buf->data, buf->len));

    status.test_id = net_buf_simple_pull_u8(buf);
    status.cid = net_buf_simple_pull_le16(buf);
    status.fault_array = bt_mesh_alloc_buf(buf->len);
    if (!status.fault_array) {
        BT_ERR("%s, Out of memory", __func__);
        return;
    }

    net_buf_simple_add_mem(status.fault_array, buf->data, buf->len);

    health_client_recv_status(model, ctx, &status, sizeof(struct bt_mesh_health_fault_status));
}

static void health_current_status(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf)
{
    struct bt_mesh_health_current_status status = {0};

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
           ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
           bt_hex(buf->data, buf->len));

    status.test_id = net_buf_simple_pull_u8(buf);
    status.cid = net_buf_simple_pull_le16(buf);
    status.fault_array = bt_mesh_alloc_buf(buf->len);
    if (!status.fault_array) {
        BT_ERR("%s, Out of memory", __func__);
        return;
    }

    net_buf_simple_add_mem(status.fault_array, buf->data, buf->len);

    health_client_recv_status(model, ctx, &status, sizeof(struct bt_mesh_health_current_status));
}

static void health_period_status(struct bt_mesh_model *model,
                                 struct bt_mesh_msg_ctx *ctx,
                                 struct net_buf_simple *buf)
{
    uint8_t status = 0U;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
           ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
           bt_hex(buf->data, buf->len));

    status = net_buf_simple_pull_u8(buf);

    health_client_recv_status(model, ctx, &status, sizeof(uint8_t));
}

static void health_attention_status(struct bt_mesh_model *model,
                                    struct bt_mesh_msg_ctx *ctx,
                                    struct net_buf_simple *buf)
{
    uint8_t status = 0U;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
           ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
           bt_hex(buf->data, buf->len));

    status = net_buf_simple_pull_u8(buf);

    health_client_recv_status(model, ctx, &status, sizeof(uint8_t));
}

const struct bt_mesh_model_op bt_mesh_health_cli_op[] = {
    { OP_HEALTH_FAULT_STATUS,   3, health_fault_status     },
    { OP_HEALTH_CURRENT_STATUS, 3, health_current_status   },
    { OP_HEALTH_PERIOD_STATUS,  1, health_period_status    },
    { OP_ATTENTION_STATUS,      1, health_attention_status },
    BLE_MESH_MODEL_OP_END,
};

int bt_mesh_health_attention_get(bt_mesh_client_common_param_t *param)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_ATTENTION_GET, 0);

    bt_mesh_model_msg_init(&msg, OP_ATTENTION_GET);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_health_attention_set(bt_mesh_client_common_param_t *param,
                                 uint8_t attention, bool need_ack)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_ATTENTION_SET, 1);

    bt_mesh_model_msg_init(&msg, need_ack ? OP_ATTENTION_SET : OP_ATTENTION_SET_UNREL);
    net_buf_simple_add_u8(&msg, attention);

    return bt_mesh_client_send_msg(param, &msg, need_ack, timeout_handler);
}

int bt_mesh_health_period_get(bt_mesh_client_common_param_t *param)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_HEALTH_PERIOD_GET, 0);

    bt_mesh_model_msg_init(&msg, OP_HEALTH_PERIOD_GET);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_health_period_set(bt_mesh_client_common_param_t *param,
                              uint8_t divisor, bool need_ack)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_HEALTH_PERIOD_SET, 1);

    bt_mesh_model_msg_init(&msg, need_ack ? OP_HEALTH_PERIOD_SET : OP_HEALTH_PERIOD_SET_UNREL);
    net_buf_simple_add_u8(&msg, divisor);

    return bt_mesh_client_send_msg(param, &msg, need_ack, timeout_handler);
}

int bt_mesh_health_fault_test(bt_mesh_client_common_param_t *param,
                              uint16_t cid, uint8_t test_id, bool need_ack)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_HEALTH_FAULT_TEST, 3);

    bt_mesh_model_msg_init(&msg, need_ack ? OP_HEALTH_FAULT_TEST : OP_HEALTH_FAULT_TEST_UNREL);
    net_buf_simple_add_u8(&msg, test_id);
    net_buf_simple_add_le16(&msg, cid);

    return bt_mesh_client_send_msg(param, &msg, need_ack, timeout_handler);
}

int bt_mesh_health_fault_clear(bt_mesh_client_common_param_t *param,
                               uint16_t cid, bool need_ack)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_HEALTH_FAULT_CLEAR, 2);

    bt_mesh_model_msg_init(&msg, need_ack ? OP_HEALTH_FAULT_CLEAR : OP_HEALTH_FAULT_CLEAR_UNREL);
    net_buf_simple_add_le16(&msg, cid);

    return bt_mesh_client_send_msg(param, &msg, need_ack, timeout_handler);
}

int bt_mesh_health_fault_get(bt_mesh_client_common_param_t *param, uint16_t cid)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_HEALTH_FAULT_GET, 2);

    bt_mesh_model_msg_init(&msg, OP_HEALTH_FAULT_GET);
    net_buf_simple_add_le16(&msg, cid);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

static int health_cli_init(struct bt_mesh_model *model)
{
    health_internal_data_t *internal = NULL;
    bt_mesh_health_client_t *client = NULL;

    if (!model) {
        BT_ERR("Invalid Health Client model");
        return -EINVAL;
    }

    BT_DBG("primary %u", bt_mesh_model_in_primary(model));

    client = (bt_mesh_health_client_t *)model->user_data;
    if (!client) {
        BT_ERR("No Health Client context provided");
        return -EINVAL;
    }

    if (client->internal_data) {
        BT_WARN("%s, Already", __func__);
        return -EALREADY;
    }

    internal = bt_mesh_calloc(sizeof(health_internal_data_t));
    if (!internal) {
        BT_ERR("%s, Out of memory", __func__);
        return -ENOMEM;
    }

    sys_slist_init(&internal->queue);

    client->model = model;
    client->op_pair_size = ARRAY_SIZE(health_op_pair);
    client->op_pair = health_op_pair;
    client->internal_data = internal;

    bt_mesh_mutex_create(&health_client_lock);

    return 0;
}

#if CONFIG_BLE_MESH_DEINIT
static int health_cli_deinit(struct bt_mesh_model *model)
{
    bt_mesh_health_client_t *client = NULL;

    if (!model) {
        BT_ERR("Invalid Health Client model");
        return -EINVAL;
    }

    BT_DBG("primary %u", bt_mesh_model_in_primary(model));

    client = (bt_mesh_health_client_t *)model->user_data;
    if (!client) {
        BT_ERR("No Health Client context provided");
        return -EINVAL;
    }

    if (client->internal_data) {
        /* Remove items from the list */
        bt_mesh_client_clear_list(client->internal_data);

        /* Free the allocated internal data */
        bt_mesh_free(client->internal_data);
        client->internal_data = NULL;
    }

    bt_mesh_mutex_free(&health_client_lock);

    return 0;
}
#endif /* CONFIG_BLE_MESH_DEINIT */

const struct bt_mesh_model_cb bt_mesh_health_cli_cb = {
    .init = health_cli_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = health_cli_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

#endif /* CONFIG_BLE_MESH_HEALTH_CLI */

/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include "osi/allocator.h"
#include "osi/mutex.h"
#include "sdkconfig.h"
#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BLE_MESH_DEBUG_MODEL)

#include "mesh_types.h"
#include "mesh_util.h"
#include "mesh_trace.h"
#include "health_cli.h"

#include "foundation.h"
#include "mesh_common.h"
#include "btc_ble_mesh_health_model.h"

s32_t health_msg_timeout;

static bt_mesh_health_client_t *health_cli;

static const bt_mesh_client_op_pair_t health_op_pair[] = {
    { OP_HEALTH_FAULT_GET,   OP_HEALTH_FAULT_STATUS  },
    { OP_HEALTH_FAULT_CLEAR, OP_HEALTH_FAULT_STATUS  },
    { OP_HEALTH_FAULT_TEST,  OP_HEALTH_FAULT_STATUS  },
    { OP_HEALTH_PERIOD_GET,  OP_HEALTH_PERIOD_STATUS },
    { OP_HEALTH_PERIOD_SET,  OP_HEALTH_PERIOD_STATUS },
    { OP_ATTENTION_GET,      OP_ATTENTION_STATUS     },
    { OP_ATTENTION_SET,      OP_ATTENTION_STATUS     },
};

static osi_mutex_t health_client_mutex;

static void bt_mesh_health_client_mutex_new(void)
{
    static bool init;

    if (!init) {
        osi_mutex_new(&health_client_mutex);
        init = true;
    }
}

static void bt_mesh_health_client_lock(void)
{
    osi_mutex_lock(&health_client_mutex, OSI_MUTEX_MAX_TIMEOUT);
}

static void bt_mesh_health_client_unlock(void)
{
    osi_mutex_unlock(&health_client_mutex);
}

static void timeout_handler(struct k_work *work)
{
    struct k_delayed_work *timer = NULL;
    bt_mesh_client_node_t *node = NULL;

    BT_WARN("Receive health status message timeout");

    bt_mesh_health_client_lock();

    timer = CONTAINER_OF(work, struct k_delayed_work, work);

    if (timer && !k_delayed_work_free(timer)) {
        node = CONTAINER_OF(work, bt_mesh_client_node_t, timer.work);
        if (node) {
            bt_mesh_health_client_cb_evt_to_btc(node->opcode,
                                                BTC_BLE_MESH_EVT_HEALTH_CLIENT_TIMEOUT, node->ctx.model, &node->ctx, NULL, 0);
            // Don't forget to release the node at the end.
            bt_mesh_client_free_node(node);
        }
    }

    bt_mesh_health_client_unlock();

    return;
}

static void health_client_cancel(struct bt_mesh_model *model,
                                 struct bt_mesh_msg_ctx *ctx,
                                 void *status, size_t len)
{
    bt_mesh_client_node_t *node = NULL;
    struct net_buf_simple buf = {0};
    u8_t evt_type = 0xFF;

    if (!model || !ctx || !status || !len) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    /* If it is a publish message, sent to the user directly. */
    buf.data = (u8_t *)status;
    buf.len  = (u16_t)len;

    bt_mesh_health_client_lock();

    node = bt_mesh_is_client_recv_publish_msg(model, ctx, &buf, true);
    if (!node) {
        BT_DBG("Unexpected health status message 0x%x", ctx->recv_op);
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
            bt_mesh_health_client_cb_evt_to_btc(
                node->opcode, evt_type, model, ctx, (const u8_t *)status, len);
            // Don't forget to release the node at the end.
            bt_mesh_client_free_node(node);
        }
    }

    bt_mesh_health_client_unlock();

    switch (ctx->recv_op) {
    case OP_HEALTH_FAULT_STATUS: {
        struct bt_mesh_health_fault_status *val;
        val = (struct bt_mesh_health_fault_status *)status;
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
        BT_ERR("%s, Failed to allocate memory", __func__);
        return;
    }

    net_buf_simple_add_mem(status.fault_array, buf->data, buf->len);

    health_client_cancel(model, ctx, &status, sizeof(struct bt_mesh_health_fault_status));
}

static void health_current_status(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf)
{
    bt_mesh_client_node_t *node = NULL;
    u8_t test_id;
    u16_t cid;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
           ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
           bt_hex(buf->data, buf->len));

    /* Health current status is a publish message, sent to the user directly. */
    if (!(node = bt_mesh_is_client_recv_publish_msg(model, ctx, buf, true))) {
        return;
    }

    test_id = net_buf_simple_pull_u8(buf);
    cid = net_buf_simple_pull_le16(buf);

    BT_DBG("Test ID 0x%02x Company ID 0x%04x Fault Count %u",
           test_id, cid, buf->len);
}

static void health_period_status(struct bt_mesh_model *model,
                                 struct bt_mesh_msg_ctx *ctx,
                                 struct net_buf_simple *buf)
{
    u8_t status = 0;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
           ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
           bt_hex(buf->data, buf->len));

    status = net_buf_simple_pull_u8(buf);

    health_client_cancel(model, ctx, &status, sizeof(u8_t));
}

static void health_attention_status(struct bt_mesh_model *model,
                                    struct bt_mesh_msg_ctx *ctx,
                                    struct net_buf_simple *buf)
{
    u8_t status = 0;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
           ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
           bt_hex(buf->data, buf->len));

    status = net_buf_simple_pull_u8(buf);

    health_client_cancel(model, ctx, &status, sizeof(u8_t));
}

const struct bt_mesh_model_op bt_mesh_health_cli_op[] = {
    { OP_HEALTH_FAULT_STATUS,   3, health_fault_status     },
    { OP_HEALTH_CURRENT_STATUS, 3, health_current_status   },
    { OP_HEALTH_PERIOD_STATUS,  1, health_period_status    },
    { OP_ATTENTION_STATUS,      1, health_attention_status },
    BLE_MESH_MODEL_OP_END,
};

int bt_mesh_health_attention_get(struct bt_mesh_msg_ctx *ctx)
{
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 0 + 4);
    int err;

    if (!ctx || !ctx->addr) {
        return -EINVAL;
    }

    bt_mesh_model_msg_init(&msg, OP_ATTENTION_GET);

    err = bt_mesh_client_send_msg(health_cli->model, OP_ATTENTION_GET, ctx,
                                  &msg, timeout_handler, health_msg_timeout,
                                  true, NULL, NULL);
    if (err) {
        BT_ERR("%s, send failed (err %d)", __func__, err);
    }

    return err;
}

int bt_mesh_health_attention_set(struct bt_mesh_msg_ctx *ctx,
                                 u8_t attention, bool need_ack)
{
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 1 + 4);
    u32_t opcode;
    int err;

    if (!ctx || !ctx->addr) {
        return -EINVAL;
    }

    if (need_ack) {
        opcode = OP_ATTENTION_SET;
    } else {
        opcode = OP_ATTENTION_SET_UNREL;
    }
    bt_mesh_model_msg_init(&msg, opcode);
    net_buf_simple_add_u8(&msg, attention);

    err = bt_mesh_client_send_msg(health_cli->model, opcode, ctx, &msg,
                                  timeout_handler, health_msg_timeout,
                                  need_ack, NULL, NULL);
    if (err) {
        BT_ERR("%s, send failed (err %d)", __func__, err);
    }

    return err;
}

int bt_mesh_health_period_get(struct bt_mesh_msg_ctx *ctx)
{
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 0 + 4);
    int err;

    if (!ctx || !ctx->addr) {
        return -EINVAL;
    }

    bt_mesh_model_msg_init(&msg, OP_HEALTH_PERIOD_GET);

    err = bt_mesh_client_send_msg(health_cli->model, OP_HEALTH_PERIOD_GET,
                                  ctx, &msg, timeout_handler, health_msg_timeout,
                                  true, NULL, NULL);
    if (err) {
        BT_ERR("%s, send failed (err %d)", __func__, err);
    }

    return err;
}

int bt_mesh_health_period_set(struct bt_mesh_msg_ctx *ctx,
                              u8_t divisor, bool need_ack)
{
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 1 + 4);
    u32_t opcode;
    int err;

    if (!ctx || !ctx->addr) {
        return -EINVAL;
    }

    if (need_ack) {
        opcode = OP_HEALTH_PERIOD_SET;
    } else {
        opcode = OP_HEALTH_PERIOD_SET_UNREL;
    }
    bt_mesh_model_msg_init(&msg, opcode);
    net_buf_simple_add_u8(&msg, divisor);

    err = bt_mesh_client_send_msg(health_cli->model, opcode, ctx, &msg,
                                  timeout_handler, health_msg_timeout,
                                  need_ack, NULL, NULL);
    if (err) {
        BT_ERR("%s, send failed (err %d)", __func__, err);
    }

    return err;
}

int bt_mesh_health_fault_test(struct bt_mesh_msg_ctx *ctx,
                              u16_t cid, u8_t test_id, bool need_ack)
{
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 3 + 4);
    u32_t opcode;
    int err;

    if (!ctx || !ctx->addr) {
        return -EINVAL;
    }

    if (need_ack) {
        opcode = OP_HEALTH_FAULT_TEST;
    } else {
        opcode = OP_HEALTH_FAULT_TEST_UNREL;
    }
    bt_mesh_model_msg_init(&msg, opcode);
    net_buf_simple_add_u8(&msg, test_id);
    net_buf_simple_add_le16(&msg, cid);

    err = bt_mesh_client_send_msg(health_cli->model, opcode, ctx, &msg,
                                  timeout_handler, health_msg_timeout,
                                  need_ack, NULL, NULL);
    if (err) {
        BT_ERR("%s, send failed (err %d)", __func__, err);
    }

    return err;
}

int bt_mesh_health_fault_clear(struct bt_mesh_msg_ctx *ctx,
                               u16_t cid, bool need_ack)
{
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 2 + 4);
    u32_t opcode;
    int err;

    if (!ctx || !ctx->addr) {
        return -EINVAL;
    }

    if (need_ack) {
        opcode = OP_HEALTH_FAULT_CLEAR;
    } else {
        opcode = OP_HEALTH_FAULT_CLEAR_UNREL;
    }
    bt_mesh_model_msg_init(&msg, opcode);
    net_buf_simple_add_le16(&msg, cid);

    err = bt_mesh_client_send_msg(health_cli->model, opcode, ctx, &msg,
                                  timeout_handler, health_msg_timeout,
                                  need_ack, NULL, NULL);
    if (err) {
        BT_ERR("%s, send failed (err %d)", __func__, err);
    }

    return err;
}

int bt_mesh_health_fault_get(struct bt_mesh_msg_ctx *ctx, u16_t cid)
{
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 2 + 4);
    int err;

    if (!ctx || !ctx->addr) {
        return -EINVAL;
    }

    bt_mesh_model_msg_init(&msg, OP_HEALTH_FAULT_GET);
    net_buf_simple_add_le16(&msg, cid);

    err = bt_mesh_client_send_msg(health_cli->model, OP_HEALTH_FAULT_GET, ctx,
                                  &msg, timeout_handler, health_msg_timeout,
                                  true, NULL, NULL);
    if (err) {
        BT_ERR("%s, send failed (err %d)", __func__, err);
    }

    return err;
}

s32_t bt_mesh_health_cli_timeout_get(void)
{
    return health_msg_timeout;
}

void bt_mesh_health_cli_timeout_set(s32_t timeout)
{
    health_msg_timeout = timeout;
}

int bt_mesh_health_cli_set(struct bt_mesh_model *model)
{
    if (!model || !model->user_data) {
        BT_ERR("%s, No Health Client context for given model", __func__);
        return -EINVAL;
    }

    health_cli = model->user_data;

    return 0;
}

int bt_mesh_health_cli_init(struct bt_mesh_model *model, bool primary)
{
    health_internal_data_t *internal = NULL;
    bt_mesh_health_client_t *client = NULL;

    BT_DBG("primary %u", primary);

    if (!model) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    client = (bt_mesh_health_client_t *)model->user_data;
    if (!client) {
        BT_ERR("%s, No Health Client context provided", __func__);
        return -EINVAL;
    }

    /* TODO: call osi_free() when deinit function is invoked*/
    internal = osi_calloc(sizeof(health_internal_data_t));
    if (!internal) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        return -ENOMEM;
    }

    sys_slist_init(&internal->queue);

    client->model = model;
    client->op_pair_size = ARRAY_SIZE(health_op_pair);
    client->op_pair = health_op_pair;
    client->internal_data = internal;

    bt_mesh_health_client_mutex_new();

    /* Set the default health client pointer */
    if (!health_cli) {
        health_cli = client;
    }

    return 0;
}

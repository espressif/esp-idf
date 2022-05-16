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

#include "access.h"
#include "foundation.h"
#include "mesh_common.h"

#if CONFIG_BLE_MESH_HEALTH_SRV
#include "health_srv.h"

#define HEALTH_TEST_STANDARD    0x00

#define HEALTH_NO_FAULT         0x00

/* Health Server context of the primary element */
struct bt_mesh_health_srv *health_srv;

/**
 * When an Element receives a Health Fault Get, or a Health Fault Test, or
 * a Health Fault Test Unacknowledged, or a Health Fault Clear, or a Health
 * Fault Clear Unacknowledged message that is not successfully processed
 * (i.e. the Company ID field that does not identify any Health Fault state
 * present in the node), it shall ignore the message.
 * The Health Fault state is identified by Company ID and may be present in
 * the node for more than one Company ID.
 */

static uint8_t health_get_curr_fault_count(struct bt_mesh_model *model)
{
    struct bt_mesh_health_srv *srv = model->user_data;
    uint8_t count = 0U;
    size_t i = 0U;

    for (i = 0U; i < ARRAY_SIZE(srv->test.curr_faults); i++) {
        if (srv->test.curr_faults[i] != HEALTH_NO_FAULT) {
            count++;
        }
    }

    return count;
}

static void health_get_fault_value(struct bt_mesh_model *model,
                                   struct net_buf_simple *msg,
                                   bool current)
{
    struct bt_mesh_health_srv *srv = model->user_data;
    size_t array_size = 0U;
    size_t i = 0U;

    array_size = current ? ARRAY_SIZE(srv->test.curr_faults) : ARRAY_SIZE(srv->test.reg_faults);

    for (i = 0U; i < array_size; i++) {
        if (net_buf_simple_tailroom(msg) == 0) {
            return;
        }

        uint8_t fault = current ? srv->test.curr_faults[i] : srv->test.reg_faults[i];
        if (fault != HEALTH_NO_FAULT) {
            net_buf_simple_add_u8(msg, fault);
        }
    }
}

static bool health_is_test_id_exist(struct bt_mesh_model *model, uint8_t test_id)
{
    struct bt_mesh_health_srv *srv = model->user_data;
    int i;

    for (i = 0; i < srv->test.id_count; i++) {
        if (srv->test.test_ids[i] == test_id) {
            return true;
        }
    }

    return false;
}

static int health_send_fault_status(struct bt_mesh_model *model,
                                    struct bt_mesh_msg_ctx *ctx)
{
    struct bt_mesh_health_srv *srv = model->user_data;
    struct net_buf_simple *msg = NULL;
    int err = 0;

    msg = bt_mesh_alloc_buf(4 + ARRAY_SIZE(srv->test.reg_faults) + 4);
    if (!msg) {
        BT_ERR("%s, Out of memory", __func__);
        return -ENOMEM;
    }

    bt_mesh_model_msg_init(msg, OP_HEALTH_FAULT_STATUS);
    net_buf_simple_add_u8(msg, srv->test.prev_test_id);
    net_buf_simple_add_le16(msg, srv->test.company_id);
    if (ctx->recv_op != OP_HEALTH_FAULT_CLEAR) {
        /**
         * For Health Fault Clear, the FaultArray field in Health Fault Status
         * shall be empty.
         */
        health_get_fault_value(model, msg, false);
    }

    err = bt_mesh_model_send(model, ctx, msg, NULL, NULL);
    if (err) {
        BT_ERR("Failed to send Health Fault Status response");
    }

    bt_mesh_free_buf(msg);
    return err;
}

static void health_fault_get(struct bt_mesh_model *model,
                             struct bt_mesh_msg_ctx *ctx,
                             struct net_buf_simple *buf)
{
    struct bt_mesh_health_srv *srv = model->user_data;
    uint16_t company_id = 0U;

    if (!srv) {
        BT_ERR("No Health Server context provided");
        return;
    }

    company_id = net_buf_simple_pull_le16(buf);
    if (company_id != srv->test.company_id) {
        BT_ERR("Unknown Company ID 0x%04x", company_id);
        return;
    }

    BT_DBG("company_id 0x%04x", company_id);

    health_send_fault_status(model, ctx);
}

static void health_fault_clear(struct bt_mesh_model *model,
                               struct bt_mesh_msg_ctx *ctx,
                               struct net_buf_simple *buf)
{
    struct bt_mesh_health_srv *srv = model->user_data;
    uint16_t company_id = 0U;

    if (!srv) {
        BT_ERR("No Health Server context provided");
        return;
    }

    company_id = net_buf_simple_pull_le16(buf);
    if (company_id != srv->test.company_id) {
        BT_ERR("Unknown Company ID 0x%04x", company_id);
        return;
    }

    BT_DBG("company_id 0x%04x", company_id);

    memset(srv->test.reg_faults, HEALTH_NO_FAULT, ARRAY_SIZE(srv->test.reg_faults));

    if (srv->cb.fault_clear) {
        srv->cb.fault_clear(model, company_id);
    }

    if (ctx->recv_op == OP_HEALTH_FAULT_CLEAR) {
        health_send_fault_status(model, ctx);
    }
}

static void health_fault_test(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    struct bt_mesh_health_srv *srv = model->user_data;
    uint16_t company_id = 0U;
    uint8_t test_id = 0U;

    BT_DBG("%s", __func__);

    if (!srv) {
        BT_ERR("No Health Server context provided");
        return;
    }

    test_id = net_buf_simple_pull_u8(buf);
    if (health_is_test_id_exist(model, test_id) == false) {
        BT_ERR("Unknown Test ID 0x%02x", test_id);
        return;
    }

    company_id = net_buf_simple_pull_le16(buf);
    if (company_id != srv->test.company_id) {
        BT_ERR("Unknown Company ID 0x%04x", company_id);
        return;
    }

    BT_DBG("test 0x%02x company 0x%04x", test_id, company_id);

    srv->test.prev_test_id = test_id;

    if (srv->cb.fault_test) {
        srv->cb.fault_test(model, test_id, company_id);
    }

    if (ctx->recv_op == OP_HEALTH_FAULT_TEST) {
        health_send_fault_status(model, ctx);
    }
}

static void send_attention_status(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_ATTENTION_STATUS, 1);
    struct bt_mesh_health_srv *srv = model->user_data;
    uint8_t time = 0U;

    if (!srv) {
        BT_ERR("No Health Server context provided");
        return;
    }

    time = k_delayed_work_remaining_get(&srv->attn_timer) / 1000;
    BT_DBG("%u second%s", time, (time == 1U) ? "" : "s");

    bt_mesh_model_msg_init(&msg, OP_ATTENTION_STATUS);
    net_buf_simple_add_u8(&msg, time);

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("Unable to send Health Attention Status");
    }
}

static void attention_get(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    BT_DBG("%s", __func__);

    send_attention_status(model, ctx);
}

static void health_set_attention(struct bt_mesh_model *model,
                                 struct bt_mesh_msg_ctx *ctx,
                                 struct net_buf_simple *buf)
{
    uint8_t time = 0U;

    time = net_buf_simple_pull_u8(buf);

    BT_DBG("%u second%s", time, (time == 1U) ? "" : "s");

    bt_mesh_attention(model, time);
}

static void attention_set(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    BT_DBG("%s", __func__);

    health_set_attention(model, ctx, buf);

    if (ctx->recv_op == OP_ATTENTION_SET) {
        send_attention_status(model, ctx);
    }
}

static void send_health_period_status(struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_HEALTH_PERIOD_STATUS, 1);

    bt_mesh_model_msg_init(&msg, OP_HEALTH_PERIOD_STATUS);
    net_buf_simple_add_u8(&msg, model->pub->period_div);

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("Unable to send Health Period Status");
    }
}

static void health_period_get(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    BT_DBG("%s", __func__);

    send_health_period_status(model, ctx);
}

static void health_set_period(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    uint8_t period = 0U;

    period = net_buf_simple_pull_u8(buf);
    if (period > 15) {
        BT_WARN("Prohibited period value %u", period);
        return;
    }

    BT_DBG("period %u", period);

    model->pub->period_div = period;
}

static void health_period_set(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    BT_DBG("%s", __func__);

    health_set_period(model, ctx, buf);

    if (ctx->recv_op == OP_HEALTH_PERIOD_SET) {
        send_health_period_status(model, ctx);
    }
}

const struct bt_mesh_model_op bt_mesh_health_srv_op[] = {
    { OP_HEALTH_FAULT_GET,         2,   health_fault_get   },
    { OP_HEALTH_FAULT_CLEAR,       2,   health_fault_clear },
    { OP_HEALTH_FAULT_CLEAR_UNREL, 2,   health_fault_clear },
    { OP_HEALTH_FAULT_TEST,        3,   health_fault_test  },
    { OP_HEALTH_FAULT_TEST_UNREL,  3,   health_fault_test  },
    { OP_HEALTH_PERIOD_GET,        0,   health_period_get  },
    { OP_HEALTH_PERIOD_SET,        1,   health_period_set  },
    { OP_HEALTH_PERIOD_SET_UNREL,  1,   health_period_set  },
    { OP_ATTENTION_GET,            0,   attention_get      },
    { OP_ATTENTION_SET,            1,   attention_set      },
    { OP_ATTENTION_SET_UNREL,      1,   attention_set      },
    BLE_MESH_MODEL_OP_END,
};

static size_t health_get_current(struct bt_mesh_model *model,
                                 struct net_buf_simple *msg)
{
    struct bt_mesh_health_srv *srv = model->user_data;

    if (!srv) {
        BT_ERR("No Health Server context provided");
        return 0;
    }

    if (msg->size < 4) {
        BT_ERR("Too small health publication msg size %d", msg->size);
        return 0;
    }

    bt_mesh_model_msg_init(msg, OP_HEALTH_CURRENT_STATUS);
    net_buf_simple_add_u8(msg, srv->test.prev_test_id);
    net_buf_simple_add_le16(msg, srv->test.company_id);
    health_get_fault_value(model, msg, true);

    return health_get_curr_fault_count(model);
}

static int health_pub_update(struct bt_mesh_model *model)
{
    struct bt_mesh_model_pub *pub = model->pub;
    size_t count = 0U;

    BT_DBG("%s", __func__);

    if (!pub || !pub->msg) {
        BT_ERR("Invalid health publication context");
        return -EINVAL;
    }

    count = health_get_current(model, pub->msg);
    if (count) {
        pub->fast_period = 1U;
    } else {
        pub->fast_period = 0U;
    }

    return 0;
}

int bt_mesh_fault_update(struct bt_mesh_elem *elem)
{
    struct bt_mesh_model *model = NULL;

    model = bt_mesh_model_find(elem, BLE_MESH_MODEL_ID_HEALTH_SRV);
    if (!model) {
        BT_ERR("Health Server not exists");
        return -EINVAL;
    }

    if (!model->pub) {
        BT_ERR("Health Server has no publication support");
        return -EINVAL;
    }

    /* Let periodic publishing, if enabled, take care of sending the
     * Health Current Status.
     */
    if (bt_mesh_model_pub_period_get(model)) {
        return 0;
    }

    health_pub_update(model);

    return bt_mesh_model_publish(model);
}

static void attention_off(struct k_work *work)
{
    struct bt_mesh_health_srv *srv = CONTAINER_OF(work,
                                     struct bt_mesh_health_srv,
                                     attn_timer.work);
    BT_DBG("%s", __func__);

    if (!srv) {
        BT_ERR("No Health Server context provided");
        return;
    }

    if (srv->cb.attn_off) {
        srv->cb.attn_off(srv->model);
    }
    srv->attn_timer_start = false;
}

static int health_srv_init(struct bt_mesh_model *model)
{
    struct bt_mesh_health_srv *srv = model->user_data;

    /* Health Server Model shall be supported by a primary element and may be
     * supported by any secondary elements.
     */

    if (!srv) {
        BT_ERR("No Health Server context provided");
        return -EINVAL;
    }

    if (srv->test.id_count == 0 || !srv->test.test_ids) {
        BT_ERR("No Health Test ID provided");
        return -EINVAL;
    }

    if (!model->pub) {
        BT_ERR("Health Server has no publication support");
        return -EINVAL;
    }

    model->pub->update = health_pub_update;

    k_delayed_work_init(&srv->attn_timer, attention_off);

    srv->model = model;
    srv->attn_timer_start = false;

    memset(srv->test.curr_faults, HEALTH_NO_FAULT, ARRAY_SIZE(srv->test.curr_faults));
    memset(srv->test.reg_faults, HEALTH_NO_FAULT, ARRAY_SIZE(srv->test.reg_faults));

    if (bt_mesh_model_in_primary(model)) {
        health_srv = srv;
    }

    return 0;
}

#if CONFIG_BLE_MESH_DEINIT
static int health_srv_deinit(struct bt_mesh_model *model)
{
    struct bt_mesh_health_srv *srv = model->user_data;

    if (!srv) {
        BT_ERR("No Health Server context provided");
        return -EINVAL;
    }

    if (srv->test.id_count == 0 || !srv->test.test_ids) {
        BT_ERR("No Health Test ID provided");
        return -EINVAL;
    }

    if (!model->pub) {
        BT_ERR("Health Server has no publication support");
        return -EINVAL;
    }

    model->pub->addr = BLE_MESH_ADDR_UNASSIGNED;
    model->pub->update = NULL;

    k_delayed_work_free(&srv->attn_timer);

    if (bt_mesh_model_in_primary(model)) {
        health_srv = NULL;
    }

    return 0;
}
#endif /* CONFIG_BLE_MESH_DEINIT */

const struct bt_mesh_model_cb bt_mesh_health_srv_cb = {
    .init = health_srv_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = health_srv_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

void bt_mesh_attention(struct bt_mesh_model *model, uint8_t time)
{
    struct bt_mesh_health_srv *srv = NULL;

    if (!model) {
        srv = health_srv;
        if (!srv) {
            BT_WARN("No Health Server context provided");
            return;
        }

        model = srv->model;
    } else {
        srv = model->user_data;
        if (!srv) {
            BT_WARN("No Health Server context provided");
            return;
        }
    }

    if (time) {
        if (srv->cb.attn_on) {
            srv->cb.attn_on(model, time);
        }

        k_delayed_work_submit(&srv->attn_timer, time * 1000U);
        srv->attn_timer_start = true;
    } else {
        k_delayed_work_cancel(&srv->attn_timer);

        if (srv->attn_timer_start == true) {
            if (srv->cb.attn_off) {
                srv->cb.attn_off(model);
            }
            srv->attn_timer_start = false;
        }
    }
}
#else /* CONFIG_BLE_MESH_HEALTH_SRV */
void bt_mesh_attention(struct bt_mesh_model *model, uint8_t time)
{
    return;
}
#endif /* CONFIG_BLE_MESH_HEALTH_SRV */

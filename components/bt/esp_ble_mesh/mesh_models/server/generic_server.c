/* Bluetooth: Mesh Generic Server Models
 *
 * Copyright (c) 2018 Vikrant More
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#include "osi/mutex.h"

#include "mesh_types.h"
#include "mesh_kernel.h"
#include "mesh_trace.h"
#include "mesh_common.h"
#include "mesh.h"
#include "access.h"
#include "model_opcode.h"
#include "transport.h"

#include "server_common.h"
#include "state_binding.h"
#include "state_transition.h"
#include "device_property.h"

#include "btc_ble_mesh_generic_model.h"

static osi_mutex_t generic_server_mutex;

static void bt_mesh_generic_server_mutex_new(void)
{
    if (!generic_server_mutex) {
        osi_mutex_new(&generic_server_mutex);
        __ASSERT(generic_server_mutex, "%s, fail", __func__);
    }
}

void bt_mesh_generic_server_lock(void)
{
    osi_mutex_lock(&generic_server_mutex, OSI_MUTEX_MAX_TIMEOUT);
}

void bt_mesh_generic_server_unlock(void)
{
    osi_mutex_unlock(&generic_server_mutex);
}

/* message handlers (Start) */

/* Generic OnOff Server message handlers */
static void send_gen_onoff_status(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  bool publish)
{
    struct bt_mesh_gen_onoff_srv *srv = model->user_data;
    struct net_buf_simple *msg = NULL;
    u8_t length = 2 + 3;

    if (ctx == NULL && publish == false) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    if (publish == false) {
        msg = bt_mesh_alloc_buf(length + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        if (msg == NULL) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
    } else {
        msg = bt_mesh_server_get_pub_msg(model, length);
        if (msg == NULL) {
            return;
        }
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS);
    net_buf_simple_add_u8(msg, srv->state.onoff);
    if (srv->transition.counter) {
        bt_mesh_server_calc_remain_time(&srv->transition);
        net_buf_simple_add_u8(msg, srv->state.target_onoff);
        net_buf_simple_add_u8(msg, srv->transition.remain_time);
    }

    if (publish == false) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        bt_mesh_free_buf(msg);
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    }
    return;
}

static void gen_onoff_get(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    struct bt_mesh_gen_onoff_srv *srv = model->user_data;

    if (srv == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_GET_MSG, model, ctx, NULL, 0);
        return;
    }

    send_gen_onoff_status(model, ctx, false);
    return;
}

void gen_onoff_publish(struct bt_mesh_model *model)
{
    if (model->user_data == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    send_gen_onoff_status(model, NULL, true);
    return;
}

static void gen_onoff_set(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    struct bt_mesh_gen_onoff_srv *srv = model->user_data;
    u8_t tid, onoff, trans_time, delay;
    bool optional;
    s64_t now;

    if (srv == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    onoff = net_buf_simple_pull_u8(buf);
    if (onoff > BLE_MESH_STATE_ON) {
        BT_ERR("%s, Invalid OnOff value 0x%02x", __func__, onoff);
        return;
    }
    tid = net_buf_simple_pull_u8(buf);

    if (bt_mesh_server_get_optional(model, ctx, buf, &trans_time, &delay, &optional)) {
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_gen_server_recv_set_msg_t set = {
            .onoff_set.op_en = optional,
            .onoff_set.onoff = onoff,
            .onoff_set.tid = tid,
            .onoff_set.trans_time = trans_time,
            .onoff_set.delay = delay,
        };
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_SET_MSG, model, ctx, (const u8_t *)&set, sizeof(set));
        return;
    }

    if (bt_mesh_is_server_recv_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now)) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_ONOFF_SET) {
            send_gen_onoff_status(model, ctx, false);
        }
        send_gen_onoff_status(model, NULL, true);
        /* In this condition, no event will be callback to application layer */
        return;
    }

    bt_mesh_generic_server_lock();

    bt_mesh_server_stop_transition(&srv->transition);
    bt_mesh_server_update_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now);

    srv->state.target_onoff = onoff;

    if (srv->state.target_onoff != srv->state.onoff) {
        generic_onoff_tt_values(srv, trans_time, delay);
    } else {
        bt_mesh_gen_server_state_change_t change = {
            .gen_onoff_set.onoff = srv->state.onoff,
        };
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_STATE_CHANGE, model, ctx, (const u8_t *)&change, sizeof(change));

        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_ONOFF_SET) {
            send_gen_onoff_status(model, ctx, false);
        }
        send_gen_onoff_status(model, NULL, true);

        bt_mesh_generic_server_unlock();
        return;
    }

    /* Copy the ctx of the received message */
    if (srv->transition.timer.work._reserved) {
        memcpy(srv->transition.timer.work._reserved, ctx, sizeof(struct bt_mesh_msg_ctx));
    }

    /* For Instantaneous Transition */
    if (srv->transition.counter == 0U) {
        srv->state.onoff = srv->state.target_onoff;
    }

    srv->transition.just_started = true;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_ONOFF_SET) {
        send_gen_onoff_status(model, ctx, false);
    }
    send_gen_onoff_status(model, NULL, true);

    bt_mesh_generic_server_unlock();

    bt_mesh_server_start_transition(&srv->transition);
    return;
}

/* Generic Level Server message handlers */
static void send_gen_level_status(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  bool publish)
{
    struct bt_mesh_gen_level_srv *srv = model->user_data;
    struct net_buf_simple *msg = NULL;
    u8_t length = 2 + 5;

    if (ctx == NULL && publish == false) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    if (publish == false) {
        msg = bt_mesh_alloc_buf(length + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        if (msg == NULL) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
    } else {
        msg = bt_mesh_server_get_pub_msg(model, length);
        if (msg == NULL) {
            return;
        }
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS);
    net_buf_simple_add_le16(msg, srv->state.level);
    if (srv->transition.counter) {
        if (srv->state.move_start) {
            if (srv->state.positive) {
                net_buf_simple_add_le16(msg, INT16_MAX);
            } else { /* 0 should not be possible */
                net_buf_simple_add_le16(msg, INT16_MIN);
            }
            net_buf_simple_add_u8(msg, BLE_MESH_UNKNOWN_REMAIN_TIME);
        } else {
            bt_mesh_server_calc_remain_time(&srv->transition);
            net_buf_simple_add_le16(msg, srv->state.target_level);
            net_buf_simple_add_u8(msg, srv->transition.remain_time);
        }
    }

    if (publish == false) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        bt_mesh_free_buf(msg);
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    }
    return;
}

static void gen_level_get(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    struct bt_mesh_gen_level_srv *srv = model->user_data;

    if (srv == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_GET_MSG, model, ctx, NULL, 0);
        return;
    }

    send_gen_level_status(model, ctx, false);
    return;
}

void gen_level_publish(struct bt_mesh_model *model)
{
    if (model->user_data == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    send_gen_level_status(model, NULL, true);
    return;
}

static void gen_level_set(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    struct bt_mesh_gen_level_srv *srv = model->user_data;
    u8_t tid, trans_time, delay;
    bool optional;
    s16_t level;
    s64_t now;

    if (srv == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    level = (s16_t) net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    if (bt_mesh_server_get_optional(model, ctx, buf, &trans_time, &delay, &optional)) {
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_gen_server_recv_set_msg_t set = {
            .level_set.op_en = optional,
            .level_set.level = level,
            .level_set.tid = tid,
            .level_set.trans_time = trans_time,
            .level_set.delay = delay,
        };
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_SET_MSG, model, ctx, (const u8_t *)&set, sizeof(set));
        return;
    }

    if (bt_mesh_is_server_recv_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now)) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_LEVEL_SET) {
            send_gen_level_status(model, ctx, false);
        }
        send_gen_level_status(model, NULL, true);
        /* In this condition, no event will be callback to application layer */
        return;
    }

    bt_mesh_generic_server_lock();

    bt_mesh_server_stop_transition(&srv->transition);
    bt_mesh_server_update_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now);

    srv->state.target_level = level;

    /**
     * If the target state is equal to the current state, the transition
     * shall not be started and is considered complete.
     */
    if (srv->state.target_level != srv->state.level) {
        generic_level_tt_values(srv, trans_time, delay);
    } else {
        bt_mesh_gen_server_state_change_t change = {
            .gen_level_set.level = srv->state.level,
        };
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_STATE_CHANGE, model, ctx, (const u8_t *)&change, sizeof(change));

        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_LEVEL_SET) {
            send_gen_level_status(model, ctx, false);
        }
        send_gen_level_status(model, NULL, true);

        bt_mesh_generic_server_unlock();
        return;
    }

    /* Copy the ctx of the received message */
    if (srv->transition.timer.work._reserved) {
        memcpy(srv->transition.timer.work._reserved, ctx, sizeof(struct bt_mesh_msg_ctx));
    }

    /* For Instantaneous Transition */
    if (srv->transition.counter == 0U) {
        srv->state.level = srv->state.target_level;
    }

    srv->transition.just_started = true;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_LEVEL_SET) {
        send_gen_level_status(model, ctx, false);
    }
    send_gen_level_status(model, NULL, true);

    bt_mesh_generic_server_unlock();

    bt_mesh_server_start_transition(&srv->transition);
    return;
}

static void gen_delta_set(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    struct bt_mesh_gen_level_srv *srv = model->user_data;
    u8_t tid, trans_time, delay;
    s32_t tmp32, delta;
    bool optional;
    s64_t now;

    if (srv == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    delta = (s32_t)net_buf_simple_pull_le32(buf);
    tid = net_buf_simple_pull_u8(buf);

    if (bt_mesh_server_get_optional(model, ctx, buf, &trans_time, &delay, &optional)) {
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_gen_server_recv_set_msg_t set = {
            .delta_set.op_en = optional,
            .delta_set.delta_level = delta,
            .delta_set.tid = tid,
            .delta_set.trans_time = trans_time,
            .delta_set.delay = delay,
        };
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_SET_MSG, model, ctx, (const u8_t *)&set, sizeof(set));
        return;
    }

    /**
     * A number of Generic Delta Set and Generic Delta Set Unacknowledged
     * messages with the same transaction identifier set in the TID field
     * may be sent.
     *
     * A new transaction starts when the TID field value in the received
     * message is different from the TID field value in the previously
     * received message that was using the same source and destination
     * addresses or from the most recently received message with the same
     * TID field value that was received 6 or more seconds earlier.
     */
    if (bt_mesh_is_server_recv_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now)) {
        if (srv->state.last_delta == delta) {
            if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_DELTA_SET) {
                send_gen_level_status(model, ctx, false);
            }
            send_gen_level_status(model, NULL, true);
            /* In this condition, no event will be callback to application layer */
            return;
        }

        tmp32 = srv->state.last_level + delta;
    } else {
        /* Starts a new transaction */
        srv->state.last_level = srv->state.level;
        tmp32 = srv->state.level + delta;
    }

    bt_mesh_generic_server_lock();

    bt_mesh_server_stop_transition(&srv->transition);
    bt_mesh_server_update_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now);

    srv->state.last_delta = delta;
    if (tmp32 < INT16_MIN) {
        tmp32 = INT16_MIN;
    } else if (tmp32 > INT16_MAX) {
        tmp32 = INT16_MAX;
    }
    srv->state.target_level = tmp32;

    /**
     * If the target state is equal to the current state, the transition
     * shall not be started and is considered complete.
     */
    if (srv->state.target_level != srv->state.level) {
        generic_level_tt_values(srv, trans_time, delay);
    } else {
        bt_mesh_gen_server_state_change_t change = {
            .gen_delta_set.level = srv->state.level,
        };
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_STATE_CHANGE, model, ctx, (const u8_t *)&change, sizeof(change));

        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_DELTA_SET) {
            send_gen_level_status(model, ctx, false);
        }
        send_gen_level_status(model, NULL, true);

        bt_mesh_generic_server_unlock();
        return;
    }

    /* Copy the ctx of the received message */
    if (srv->transition.timer.work._reserved) {
        memcpy(srv->transition.timer.work._reserved, ctx, sizeof(struct bt_mesh_msg_ctx));
    }

    /* For Instantaneous Transition */
    if (srv->transition.counter == 0U) {
        srv->state.level = srv->state.target_level;
    }

    srv->transition.just_started = true;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_DELTA_SET) {
        send_gen_level_status(model, ctx, false);
    }
    send_gen_level_status(model, NULL, true);

    bt_mesh_generic_server_unlock();

    bt_mesh_server_start_transition(&srv->transition);
    return;
}

static void gen_move_set(struct bt_mesh_model *model,
                         struct bt_mesh_msg_ctx *ctx,
                         struct net_buf_simple *buf)
{
    struct bt_mesh_gen_level_srv *srv = model->user_data;
    u8_t tid, trans_time, delay;
    bool optional;
    s16_t delta;
    s32_t tmp32;
    s64_t now;

    if (srv == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    delta = (s16_t) net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    if (bt_mesh_server_get_optional(model, ctx, buf, &trans_time, &delay, &optional)) {
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_gen_server_recv_set_msg_t set = {
            .move_set.op_en = optional,
            .move_set.delta_level = delta,
            .move_set.tid = tid,
            .move_set.trans_time = trans_time,
            .move_set.delay = delay,
        };
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_SET_MSG, model, ctx, (const u8_t *)&set, sizeof(set));
        return;
    }

    if (bt_mesh_is_server_recv_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now)) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_MOVE_SET) {
            send_gen_level_status(model, ctx, false);
        }
        send_gen_level_status(model, NULL, true);
        /* In this condition, no event will be callback to application layer */
        return;
    }

    bt_mesh_generic_server_lock();

    bt_mesh_server_stop_transition(&srv->transition);
    bt_mesh_server_update_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now);

    srv->state.last_delta = delta;

    tmp32 = srv->state.level + delta;
    if (tmp32 < INT16_MIN) {
        tmp32 = INT16_MIN;
    } else if (tmp32 > INT16_MAX) {
        tmp32 = INT16_MAX;
    }
    srv->state.target_level = tmp32;

    /**
     * If the target state is equal to the current state, the transition
     * shall not be started and is considered complete.
     */
    if (srv->state.target_level != srv->state.level) {
        generic_level_tt_values(srv, trans_time, delay);
    } else {
        bt_mesh_gen_server_state_change_t change = {
            .gen_move_set.level = srv->state.level,
        };
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_STATE_CHANGE, model, ctx, (const u8_t *)&change, sizeof(change));

        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_MOVE_SET) {
            send_gen_level_status(model, ctx, false);
        }
        send_gen_level_status(model, NULL, true);
        srv->state.move_start = false;

        bt_mesh_generic_server_unlock();
        return;
    }

    /* Copy the ctx of the received message */
    if (srv->transition.timer.work._reserved) {
        memcpy(srv->transition.timer.work._reserved, ctx, sizeof(struct bt_mesh_msg_ctx));
    }

    if (delta) {
        srv->state.move_start = true;
        srv->state.positive = (delta > 0) ? true : false;
    }

    srv->transition.just_started = true;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_MOVE_SET) {
        send_gen_level_status(model, ctx, false);
    }
    send_gen_level_status(model, NULL, true);

    bt_mesh_generic_server_unlock();

    /**
     * If (trans_time == 0) OR (delta == 0)
     * 1. If the resulting Transition Time is equal to 0 or is undefined,
     *    the Generic Move Set command will not initiate any Generic Level
     *    state change.
     * 2. When a Generic Level Server receives the message with a value of
     *    the Delta Level field equal to 0, it shall stop changing the
     *    Generic Level state. (if delta == 0, srv->state.target_level will
     *    equal to srv->state.level)
     */
    if (srv->transition.counter == 0U) {
        srv->state.move_start = false;
        bt_mesh_gen_server_state_change_t change = {
            .gen_move_set.level = srv->state.level,
        };
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_STATE_CHANGE, model, ctx, (const u8_t *)&change, sizeof(change));
        return;
    }

    bt_mesh_server_start_transition(&srv->transition);
    return;
}

/* Generic Default Transition Time Server message handlers */
static void send_gen_def_trans_time_status(struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        bool publish)
{
    struct bt_mesh_gen_def_trans_time_srv *srv = model->user_data;
    struct net_buf_simple *msg = NULL;
    u8_t length = 2 + 1;

    if (ctx == NULL && publish == false) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    if (publish == false) {
        msg = bt_mesh_alloc_buf(length + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        if (msg == NULL) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
    } else {
        msg = bt_mesh_server_get_pub_msg(model, length);
        if (msg == NULL) {
            return;
        }
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_STATUS);
    net_buf_simple_add_u8(msg, srv->state.trans_time);

    if (publish == false) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        bt_mesh_free_buf(msg);
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    }
    return;
}

static void gen_def_trans_time_get(struct bt_mesh_model *model,
                                   struct bt_mesh_msg_ctx *ctx,
                                   struct net_buf_simple *buf)
{
    struct bt_mesh_gen_def_trans_time_srv *srv = model->user_data;

    if (srv == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_GET_MSG, model, ctx, NULL, 0);
        return;
    }

    send_gen_def_trans_time_status(model, ctx, false);
    return;
}

static void gen_def_trans_time_set(struct bt_mesh_model *model,
                                   struct bt_mesh_msg_ctx *ctx,
                                   struct net_buf_simple *buf)
{
    struct bt_mesh_gen_def_trans_time_srv *srv = model->user_data;
    u8_t trans_time;

    if (srv == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    trans_time = net_buf_simple_pull_u8(buf);
    if ((trans_time & 0x3F) == 0x3F) {
        BT_WARN("%s, Invalid Transaction Number of Steps 0x3F", __func__);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_gen_server_recv_set_msg_t set = {
            .def_trans_time_set.trans_time = trans_time,
        };
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_SET_MSG, model, ctx, (const u8_t *)&set, sizeof(set));
        return;
    }

    if (srv->state.trans_time != trans_time) {
        srv->state.trans_time = trans_time;
    }

    bt_mesh_gen_server_state_change_t change = {
        .gen_def_trans_time_set.trans_time = trans_time,
    };
    bt_mesh_generic_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_GENERIC_SERVER_STATE_CHANGE, model, ctx, (const u8_t *)&change, sizeof(change));

    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET) {
        send_gen_def_trans_time_status(model, ctx, false);
    }
    send_gen_def_trans_time_status(model, NULL, true);

    return;
}

/* Generic Power OnOff Server message handlers */
static void send_gen_onpowerup_status(struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx,
                                      bool publish)
{
    struct net_buf_simple *msg = NULL;
    u8_t length = 2 + 1;

    if (ctx == NULL && publish == false) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    if (publish == false) {
        msg = bt_mesh_alloc_buf(length + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        if (msg == NULL) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
    } else {
        msg = bt_mesh_server_get_pub_msg(model, length);
        if (msg == NULL) {
            return;
        }
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_ONPOWERUP_STATUS);
    switch (model->id) {
    case BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV: {
        struct bt_mesh_gen_power_onoff_srv *srv = model->user_data;
        net_buf_simple_add_u8(msg, srv->state->onpowerup);
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SETUP_SRV: {
        struct bt_mesh_gen_power_onoff_setup_srv *srv = model->user_data;
        net_buf_simple_add_u8(msg, srv->state->onpowerup);
        break;
    }
    default:
        BT_ERR("%s, Invalid Generic Power OnOff Server 0x%04x", __func__, model->id);
        if (publish == false) {
            bt_mesh_free_buf(msg);
        }
        return;
    }

    if (publish == false) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        bt_mesh_free_buf(msg);
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    }
    return;
}

static void gen_onpowerup_get(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    struct bt_mesh_gen_power_onoff_srv *srv = model->user_data;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_GET_MSG, model, ctx, NULL, 0);
        return;
    }

    send_gen_onpowerup_status(model, ctx, false);
    return;
}

/* Generic Power OnOff Setup Server message handlers */
void gen_onpowerup_publish(struct bt_mesh_model *model)
{
    if (model->user_data == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    switch (model->id) {
    case BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV: {
        struct bt_mesh_gen_power_onoff_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("%s, Invalid Generic Power OnOff Server state", __func__);
            return;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SETUP_SRV: {
        struct bt_mesh_gen_power_onoff_setup_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("%s, Invalid Generic Power OnOff Setup Server state", __func__);
            return;
        }
        break;
    }
    default:
        BT_ERR("%s, Invalid Generic Power OnOff Server 0x%04x", __func__, model->id);
        return;
    }

    send_gen_onpowerup_status(model, NULL, true);
    return;
}

static void gen_onpowerup_set(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    struct bt_mesh_gen_power_onoff_setup_srv *srv = model->user_data;
    u8_t onpowerup;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    onpowerup = net_buf_simple_pull_u8(buf);
    if (onpowerup > BLE_MESH_STATE_RESTORE) {
        BT_WARN("%s, Invalid OnPowerUp value 0x%02x", __func__, onpowerup);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_gen_server_recv_set_msg_t set = {
            .onpowerup_set.onpowerup = onpowerup,
        };
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_SET_MSG, model, ctx, (const u8_t *)&set, sizeof(set));
        return;
    }

    if (srv->state->onpowerup != onpowerup) {
        srv->state->onpowerup = onpowerup;
    }

    bt_mesh_gen_server_state_change_t change = {
        .gen_onpowerup_set.onpowerup = onpowerup,
    };
    bt_mesh_generic_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_GENERIC_SERVER_STATE_CHANGE, model, ctx, (const u8_t *)&change, sizeof(change));

    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET) {
        send_gen_onpowerup_status(model, ctx, false);
    }
    send_gen_onpowerup_status(model, NULL, true);

    return;
}

/* Generic Power Level Server message handlers */
static void send_gen_power_level_status(struct bt_mesh_model *model,
                                        struct bt_mesh_msg_ctx *ctx,
                                        bool publish, u16_t opcode)
{
    struct net_buf_simple *msg = NULL;
    u8_t length = 2 + 5;

    if (ctx == NULL && publish == false) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    if (publish == false) {
        msg = bt_mesh_alloc_buf(length + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        if (msg == NULL) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
    } else {
        msg = bt_mesh_server_get_pub_msg(model, length);
        if (msg == NULL) {
            return;
        }
    }

    bt_mesh_model_msg_init(msg, opcode);
    switch (opcode) {
    case BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS:
    case BLE_MESH_MODEL_OP_GEN_POWER_LAST_STATUS: {
        struct bt_mesh_gen_power_level_srv *srv = model->user_data;
        if (opcode == BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS) {
            net_buf_simple_add_le16(msg, srv->state->power_actual);
            if (srv->transition.counter) {
                bt_mesh_server_calc_remain_time(&srv->transition);
                net_buf_simple_add_le16(msg, srv->state->target_power_actual);
                net_buf_simple_add_u8(msg, srv->transition.remain_time);
            }
        } else if (opcode == BLE_MESH_MODEL_OP_GEN_POWER_LAST_STATUS) {
            net_buf_simple_add_le16(msg, srv->state->power_last);
        }
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS:
        if (model->id == BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV) {
            struct bt_mesh_gen_power_level_srv *srv = model->user_data;
            net_buf_simple_add_le16(msg, srv->state->power_default);
        } else if (model->id == BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SETUP_SRV) {
            struct bt_mesh_gen_power_level_setup_srv *srv = model->user_data;
            net_buf_simple_add_le16(msg, srv->state->power_default);
        }
        break;
    case BLE_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS:
        if (model->id == BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV) {
            struct bt_mesh_gen_power_level_srv *srv = model->user_data;
            net_buf_simple_add_u8(msg, srv->state->status_code);
            net_buf_simple_add_le16(msg, srv->state->power_range_min);
            net_buf_simple_add_le16(msg, srv->state->power_range_max);
        } else if (model->id == BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SETUP_SRV) {
            struct bt_mesh_gen_power_level_setup_srv *srv = model->user_data;
            net_buf_simple_add_u8(msg, srv->state->status_code);
            net_buf_simple_add_le16(msg, srv->state->power_range_min);
            net_buf_simple_add_le16(msg, srv->state->power_range_max);
        }
        break;
    default:
        BT_WARN("%s, Unknown Generic Power status opcode 0x%04x", __func__, opcode);
        if (publish == false) {
            bt_mesh_free_buf(msg);
        }
        return;
    }

    if (publish == false) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        bt_mesh_free_buf(msg);
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    }
    return;
}

static void gen_power_level_get(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf)
{
    struct bt_mesh_gen_power_level_srv *srv = model->user_data;
    u16_t opcode;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_GET_MSG, model, ctx, NULL, 0);
        return;
    }

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET:
        opcode = BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS;
        break;
    case BLE_MESH_MODEL_OP_GEN_POWER_LAST_GET:
        opcode = BLE_MESH_MODEL_OP_GEN_POWER_LAST_STATUS;
        break;
    case BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_GET:
        opcode = BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS;
        break;
    case BLE_MESH_MODEL_OP_GEN_POWER_RANGE_GET:
        opcode = BLE_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS;
        break;
    default:
        BT_WARN("%s, Unknown Generic Power Get opcode 0x%04x", __func__, ctx->recv_op);
        return;
    }

    send_gen_power_level_status(model, ctx, false, opcode);
    return;
}

void gen_power_level_publish(struct bt_mesh_model *model, u16_t opcode)
{
    if (model->user_data == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    switch (model->id) {
    case BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV: {
        struct bt_mesh_gen_power_level_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("%s, Invalid Generic Power Level Server state", __func__);
            return;
        }
        break;
    }
    case ESP_BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SETUP_SRV: {
        struct bt_mesh_gen_power_level_setup_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("%s, Invalid Generic Power Level Setup Server state", __func__);
            return;
        }
        break;
    }
    default:
        BT_ERR("%s, Invalid Generic Power Level Server 0x%04x", __func__, model->id);
        return;
    }

    send_gen_power_level_status(model, NULL, true, opcode);
    return;
}

static void gen_power_level_set(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf)
{
    struct bt_mesh_gen_power_level_srv *srv = model->user_data;
    u8_t tid, trans_time, delay;
    bool optional;
    u16_t power;
    s64_t now;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    power = net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    if (bt_mesh_server_get_optional(model, ctx, buf, &trans_time, &delay, &optional)) {
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_gen_server_recv_set_msg_t set = {
            .power_level_set.op_en = optional,
            .power_level_set.power = power,
            .power_level_set.tid = tid,
            .power_level_set.trans_time = trans_time,
            .power_level_set.delay = delay,
        };
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_SET_MSG, model, ctx, (const u8_t *)&set, sizeof(set));
        return;
    }

    if (bt_mesh_is_server_recv_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now)) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET) {
            send_gen_power_level_status(model, ctx, false, BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS);
        }
        send_gen_power_level_status(model, NULL, true, BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS);
        /* In this condition, no event will be callback to application layer */
        return;
    }

    bt_mesh_generic_server_lock();

    bt_mesh_server_stop_transition(&srv->transition);
    bt_mesh_server_update_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now);

    if (power) {
        if (srv->state->power_range_min && power < srv->state->power_range_min) {
            power = srv->state->power_range_min;
        } else if (srv->state->power_range_max && power > srv->state->power_range_max) {
            power = srv->state->power_range_max;
        }
    }
    srv->state->target_power_actual = power;

    /* If the target state is equal to the current state, the transition
     * shall not be started and is considered complete.
     */
    if (srv->state->target_power_actual != srv->state->power_actual) {
        generic_power_level_tt_values(srv, trans_time, delay);
    } else {
        bt_mesh_gen_server_state_change_t change = {
            .gen_power_level_set.power = srv->state->power_actual,
        };
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_STATE_CHANGE, model, ctx, (const u8_t *)&change, sizeof(change));

        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET) {
            send_gen_power_level_status(model, ctx, false, BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS);
        }
        send_gen_power_level_status(model, NULL, true, BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS);

        bt_mesh_generic_server_unlock();
        return;
    }

    /* Copy the ctx of the received message */
    if (srv->transition.timer.work._reserved) {
        memcpy(srv->transition.timer.work._reserved, ctx, sizeof(struct bt_mesh_msg_ctx));
    }

    /* For Instantaneous Transition */
    if (srv->transition.counter == 0U) {
        srv->state->power_actual = srv->state->target_power_actual;
        /* Whenever the Generic Power Actual state is changed to a non-zero value
         * as a result of a non-transactional message or a completed sequence of
         * transactional messages, the value of the Generic Power Last state shall
         * be set to the value of the Generic Power Actual state.
         */
        if (srv->state->power_actual) {
            srv->state->power_last = srv->state->power_actual;
        }
    }

    srv->transition.just_started = true;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET) {
        send_gen_power_level_status(model, ctx, false, BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS);
    }
    send_gen_power_level_status(model, NULL, true, BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS);

    bt_mesh_generic_server_unlock();

    bt_mesh_server_start_transition(&srv->transition);
    return;
}

/* Generic Power Level Setup Server message handlers */
static void gen_power_default_set(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf)
{
    struct bt_mesh_gen_power_level_setup_srv *srv = model->user_data;
    u16_t power;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    power = net_buf_simple_pull_le16(buf);

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_gen_server_recv_set_msg_t set = {
            .power_default_set.power = power, /* Just callback the actual recived value */
        };
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_SET_MSG, model, ctx, (const u8_t *)&set, sizeof(set));
        return;
    }

    /**
     * Value 0x0000 has a special meaning defined: use the value of the
     * Generic Power Last state as the default value.
     */
    if (power == 0x0000) {
        power = srv->state->power_last;
    }

    if (srv->state->power_default != power) {
        srv->state->power_default = power;
    }

    bt_mesh_gen_server_state_change_t change = {
        .gen_power_default_set.power = power,
    };
    bt_mesh_generic_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_GENERIC_SERVER_STATE_CHANGE, model, ctx, (const u8_t *)&change, sizeof(change));

    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET) {
        send_gen_power_level_status(model, ctx, false, BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS);
    }
    send_gen_power_level_status(model, NULL, true, BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS);

    return;
}

static void gen_power_range_set(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf)
{
    struct bt_mesh_gen_power_level_setup_srv *srv = model->user_data;
    u16_t range_min, range_max;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    range_min = net_buf_simple_pull_le16(buf);
    range_max = net_buf_simple_pull_le16(buf);

    if (range_min > range_max) {
        BT_ERR("%s, Range Min 0x%04x is greater than Range Max 0x%04x",
               __func__, range_min, range_max);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_gen_server_recv_set_msg_t set = {
            .power_range_set.range_min = range_min,
            .power_range_set.range_max = range_max,
        };
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_SET_MSG, model, ctx, (const u8_t *)&set, sizeof(set));
        return;
    }

    if (range_min == 0x0000) {
        srv->state->status_code = BLE_MESH_CANNOT_SET_RANGE_MIN;
    } else if (range_max == 0x0000) {
        srv->state->status_code = BLE_MESH_CANNOT_SET_RANGE_MAX;
    } else {
        srv->state->status_code = BLE_MESH_RANGE_UPDATE_SUCCESS;
    }

    if (range_min && srv->state->power_range_min != range_min) {
        srv->state->power_range_min = range_min;
    }

    if (range_max && srv->state->power_range_max != range_max) {
        srv->state->power_range_max = range_max;
    }

    bt_mesh_gen_server_state_change_t change = {
        .gen_power_range_set.range_min = srv->state->power_range_min,
        .gen_power_range_set.range_max = srv->state->power_range_max,
    };
    bt_mesh_generic_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_GENERIC_SERVER_STATE_CHANGE, model, ctx, (const u8_t *)&change, sizeof(change));

    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET) {
        send_gen_power_level_status(model, ctx, false, BLE_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS);
    }
    send_gen_power_level_status(model, NULL, true, BLE_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS);

    return;
}

/* Generic Battery Server message handlers */
static void gen_battery_get(struct bt_mesh_model *model,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *buf)
{
    struct bt_mesh_gen_battery_srv *srv = model->user_data;
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 8 + BLE_MESH_SERVER_TRANS_MIC_SIZE);

    if (srv == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_GET_MSG, model, ctx, NULL, 0);
        return;
    }

    bt_mesh_model_msg_init(&msg, BLE_MESH_MODEL_OP_GEN_BATTERY_STATUS);
    net_buf_simple_add_le32(&msg, srv->state.time_to_discharge << 8 | srv->state.battery_level);
    net_buf_simple_add_le32(&msg, srv->state.battery_flags << 24 | srv->state.time_to_charge);

    BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, &msg, NULL, NULL));
    return;
}

/* Generic Location Server message handlers */
static void send_gen_location_status(struct bt_mesh_model *model,
                                     struct bt_mesh_msg_ctx *ctx,
                                     bool publish, u16_t opcode)
{
    struct net_buf_simple *msg = NULL;
    u8_t length = 1 + 10;

    if (ctx == NULL && publish == false) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    if (publish == false) {
        msg = bt_mesh_alloc_buf(length + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        if (msg == NULL) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
    } else {
        msg = bt_mesh_server_get_pub_msg(model, length);
        if (msg == NULL) {
            return;
        }
    }

    bt_mesh_model_msg_init(msg, opcode);
    switch (opcode) {
    case BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_STATUS:
        if (model->id == BLE_MESH_MODEL_ID_GEN_LOCATION_SRV) {
            struct bt_mesh_gen_location_srv *srv = model->user_data;
            net_buf_simple_add_le32(msg, srv->state->global_latitude);
            net_buf_simple_add_le32(msg, srv->state->global_longitude);
            net_buf_simple_add_le16(msg, srv->state->global_altitude);
        } else if (model->id == BLE_MESH_MODEL_ID_GEN_LOCATION_SETUP_SRV) {
            struct bt_mesh_gen_location_setup_srv *srv = model->user_data;
            net_buf_simple_add_le32(msg, srv->state->global_latitude);
            net_buf_simple_add_le32(msg, srv->state->global_longitude);
            net_buf_simple_add_le16(msg, srv->state->global_altitude);
        }
        break;
    case BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_STATUS:
        if (model->id == BLE_MESH_MODEL_ID_GEN_LOCATION_SRV) {
            struct bt_mesh_gen_location_srv *srv = model->user_data;
            net_buf_simple_add_le16(msg, srv->state->local_north);
            net_buf_simple_add_le16(msg, srv->state->local_east);
            net_buf_simple_add_le16(msg, srv->state->local_altitude);
            net_buf_simple_add_u8(msg, srv->state->floor_number);
            net_buf_simple_add_le16(msg, srv->state->uncertainty);
        } else if (model->id == BLE_MESH_MODEL_ID_GEN_LOCATION_SETUP_SRV) {
            struct bt_mesh_gen_location_setup_srv *srv = model->user_data;
            net_buf_simple_add_le16(msg, srv->state->local_north);
            net_buf_simple_add_le16(msg, srv->state->local_east);
            net_buf_simple_add_le16(msg, srv->state->local_altitude);
            net_buf_simple_add_u8(msg, srv->state->floor_number);
            net_buf_simple_add_le16(msg, srv->state->uncertainty);
        }
        break;
    default:
        BT_WARN("%s, Unknown Generic Location status opcode 0x%04x", __func__, opcode);
        if (publish == false) {
            bt_mesh_free_buf(msg);
        }
        return;
    }

    if (publish == false) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        bt_mesh_free_buf(msg);
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    }
    return;
}

static void gen_location_get(struct bt_mesh_model *model,
                             struct bt_mesh_msg_ctx *ctx,
                             struct net_buf_simple *buf)
{
    struct bt_mesh_gen_location_srv *srv = model->user_data;
    u16_t opcode;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_GET_MSG, model, ctx, NULL, 0);
        return;
    }

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_GET:
        opcode = BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_STATUS;
        break;
    case BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_GET:
        opcode = BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_STATUS;
        break;
    default:
        BT_WARN("%s, Unknown Generic Location Get opcode 0x%04x", __func__, ctx->recv_op);
        return;
    }

    send_gen_location_status(model, ctx, false, opcode);
    return;
}

/* Generic Location Setup Server message handlers */
static void gen_location_set(struct bt_mesh_model *model,
                             struct bt_mesh_msg_ctx *ctx,
                             struct net_buf_simple *buf)
{
    struct bt_mesh_gen_location_setup_srv *srv = model->user_data;
    u16_t opcode;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET:
    case BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET_UNACK: {
        opcode = BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_STATUS;
        s32_t latitude = net_buf_simple_pull_le32(buf);
        s32_t longitude = net_buf_simple_pull_le32(buf);
        s16_t altitude = net_buf_simple_pull_le16(buf);

        /* Callback the received message to the application layer */
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
            bt_mesh_gen_server_recv_set_msg_t set = {
                .loc_global_set.latitude = latitude,
                .loc_global_set.longitude = longitude,
                .loc_global_set.altitude = altitude,
            };
            bt_mesh_generic_server_cb_evt_to_btc(
                BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_SET_MSG, model, ctx, (const u8_t *)&set, sizeof(set));
            return;
        }

        if (latitude != 0x80000000) {
            srv->state->global_latitude = latitude;
        }
        if (longitude != 0x80000000) {
            srv->state->global_longitude = longitude;
        }
        if (altitude != 0x7FFF) {
            srv->state->global_altitude = altitude;
        }

        bt_mesh_gen_server_state_change_t change = {
            .gen_loc_global_set.latitude = srv->state->global_latitude,
            .gen_loc_global_set.longitude = srv->state->global_longitude,
            .gen_loc_global_set.altitude = srv->state->global_altitude,
        };
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_STATE_CHANGE, model, ctx, (const u8_t *)&change, sizeof(change));
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET:
    case BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET_UNACK: {
        opcode = BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_STATUS;
        u16_t north = net_buf_simple_pull_le16(buf);
        u16_t east = net_buf_simple_pull_le16(buf);
        u16_t altitude = net_buf_simple_pull_le16(buf);
        u8_t floor = net_buf_simple_pull_u8(buf);
        u16_t uncertainty = net_buf_simple_pull_le16(buf);

        /* Callback the received message to the application layer */
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
            bt_mesh_gen_server_recv_set_msg_t set = {
                .loc_local_set.north = north,
                .loc_local_set.east = east,
                .loc_local_set.altitude = altitude,
                .loc_local_set.floor_number = floor,
                .loc_local_set.uncertainty = uncertainty,
            };
            bt_mesh_generic_server_cb_evt_to_btc(
                BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_SET_MSG, model, ctx, (const u8_t *)&set, sizeof(set));
            return;
        }

        if (north != 0x8000) {
            srv->state->local_north = north;
        }
        if (east != 0x8000) {
            srv->state->local_east = east;
        }
        if (altitude != 0x7FFF) {
            srv->state->local_altitude = altitude;
        }
        if (floor != 0xFF) {
            srv->state->floor_number = floor;
        }
        srv->state->uncertainty = uncertainty;

        bt_mesh_gen_server_state_change_t change = {
            .gen_loc_local_set.north = srv->state->local_north,
            .gen_loc_local_set.east = srv->state->local_east,
            .gen_loc_local_set.altitude = srv->state->local_altitude,
            .gen_loc_local_set.floor_number = srv->state->floor_number,
            .gen_loc_local_set.uncertainty = srv->state->uncertainty,
        };
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_STATE_CHANGE, model, ctx, (const u8_t *)&change, sizeof(change));
        break;
    }
    default:
        BT_WARN("%s, Unknown Generic Location Set opcode 0x%04x", __func__, ctx->recv_op);
        return;
    }

    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET ||
            ctx->recv_op == BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET) {
        send_gen_location_status(model, ctx, false, opcode);
    }
    send_gen_location_status(model, NULL, true, opcode);

    return;
}

/* Generic User Property Server message handlers */
struct bt_mesh_generic_property *gen_get_user_property(struct bt_mesh_model *model,
        u16_t property_id)
{
    struct bt_mesh_gen_user_prop_srv *srv = model->user_data;
    u8_t i;

    for (i = 0U; i < srv->property_count; i++) {
        if (srv->properties[i].id == property_id) {
            return &srv->properties[i];
        }
    }

    return NULL;
}

static void send_gen_user_prop_status(struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx,
                                      u16_t property_id, bool publish)
{
    struct bt_mesh_generic_property *property = NULL;
    struct net_buf_simple *msg = NULL;
    u16_t length;

    if (property_id == BLE_MESH_INVALID_DEVICE_PROPERTY_ID) {
        BT_ERR("%s, Invalid User Property ID 0x%04x", __func__, property_id);
        return;
    }

    property = gen_get_user_property(model, property_id);
    if (property == NULL) {
        BT_WARN("%s, User property 0x%04x not exist", __func__, property_id);
        length = sizeof(property_id);
    } else {
        length = sizeof(property->id) + sizeof(property->user_access) + property->val->len;
    }

    if (publish == false) {
        msg = bt_mesh_alloc_buf(1 + length + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        if (msg == NULL) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
    } else {
        msg = bt_mesh_server_get_pub_msg(model, 1 + length);
        if (msg == NULL) {
            return;
        }
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_STATUS);
    if (property == NULL) {
        net_buf_simple_add_le16(msg, property_id);
    } else {
        net_buf_simple_add_le16(msg, property->id);
        net_buf_simple_add_u8(msg, property->user_access);
        if ((ctx->recv_op == BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET &&
                property->user_access != USER_ACCESS_PROHIBIT &&
                property->user_access != USER_ACCESS_WRITE) ||
                ((ctx->recv_op == BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET ||
                  ctx->recv_op == BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET_UNACK) &&
                 property->user_access != USER_ACCESS_PROHIBIT &&
                 property->user_access != USER_ACCESS_READ)) {
            net_buf_simple_add_mem(msg, property->val->data, property->val->len);
        }
    }

    if (publish == false) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        bt_mesh_free_buf(msg);
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    }
    return;
}

static void gen_user_prop_get(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    struct bt_mesh_gen_user_prop_srv *srv = model->user_data;

    if (srv == NULL || srv->property_count == 0U || srv->properties == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        /**
         * Also we can use __packed for esp_ble_mesh_gen_user_property_get_t,
         * and directly callback with buf->data & buf->len.
         */
        bt_mesh_gen_server_recv_get_msg_t get = {0};
        const u8_t *param = NULL;
        size_t len = 0;
        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET) {
            get.user_property_get.id = net_buf_simple_pull_le16(buf);
            param = (const u8_t *)&get;
            len = sizeof(get);
        }
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_GET_MSG, model, ctx, param, len);
        return;
    }

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_GET: {
        struct net_buf_simple *msg = NULL;
        u8_t i;
        msg = bt_mesh_alloc_buf(1 + srv->property_count * 2 + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        if (msg == NULL) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_STATUS);
        for (i = 0U; i < srv->property_count; i++) {
            if (srv->properties[i].admin_access != ADMIN_NOT_USER_PROP &&
                    srv->properties[i].manu_access != MANU_NOT_USER_PROP) {
                net_buf_simple_add_le16(msg, srv->properties[i].id);
            }
        }
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        bt_mesh_free_buf(msg);
        return;
    }
    case BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET: {
        u16_t property_id = net_buf_simple_pull_le16(buf);
        send_gen_user_prop_status(model, ctx, property_id, false);
        return;
    }
    default:
        BT_WARN("%s, Unknown Generic User Property Get opcode 0x%04x", __func__, ctx->recv_op);
        return;
    }
}

static void gen_user_prop_set(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    struct bt_mesh_gen_user_prop_srv *srv = model->user_data;
    struct bt_mesh_generic_property *property = NULL;
    u16_t property_id;
    u8_t expect_len;

    if (srv == NULL || srv->property_count == 0U || srv->properties == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    property_id = net_buf_simple_pull_le16(buf);

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_gen_server_recv_set_msg_t set = {
            .user_property_set.id = property_id,
            .user_property_set.value = buf,
        };
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_SET_MSG, model, ctx, (const u8_t *)&set, sizeof(set));
        return;
    }

    property = gen_get_user_property(model, property_id);
    if (property == NULL || property->user_access == USER_ACCESS_PROHIBIT ||
            property->user_access == USER_ACCESS_READ) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET) {
            send_gen_user_prop_status(model, ctx, property_id, false);
        }
        send_gen_user_prop_status(model, ctx, property_id, true);
        return;
    }

    /* For BLE Mesh Model BQB test:
     * PTS will send Generic User Property Set/Set Unack messages with
     * invalid device property value length, these messages need to be
     * ignored, otherwise the test case will fail.
     */
    expect_len = bt_mesh_get_dev_prop_len(property_id);
    if (buf->len != expect_len) {
        BT_ERR("%s, Invalid User Property length, ID 0x%04x, expect %d, actual %d",
               __func__, property_id, expect_len, buf->len);
        return;
    }

    net_buf_simple_reset(property->val);
    net_buf_simple_add_mem(property->val, buf->data, MIN(buf->len, property->val->size));

    bt_mesh_gen_server_state_change_t change = {
        .gen_user_prop_set.id = property_id,
        .gen_user_prop_set.value = property->val,
    };
    bt_mesh_generic_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_GENERIC_SERVER_STATE_CHANGE, model, ctx, (const u8_t *)&change, sizeof(change));

    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET) {
        send_gen_user_prop_status(model, ctx, property_id, false);
    }
    send_gen_user_prop_status(model, ctx, property_id, true);

    return;
}

/* Generic Admin Property Server message handlers */
struct bt_mesh_generic_property *gen_get_admin_property(struct bt_mesh_model *model,
        u16_t property_id)
{
    struct bt_mesh_gen_admin_prop_srv *srv = model->user_data;
    u8_t i;

    for (i = 0U; i < srv->property_count; i++) {
        if (srv->properties[i].id == property_id) {
            return &srv->properties[i];
        }
    }

    return NULL;
}

static void send_gen_admin_prop_status(struct bt_mesh_model *model,
                                       struct bt_mesh_msg_ctx *ctx,
                                       u16_t property_id, bool publish)
{
    struct bt_mesh_generic_property *property = NULL;
    struct net_buf_simple *msg = NULL;
    u16_t length;

    if (property_id == BLE_MESH_INVALID_DEVICE_PROPERTY_ID) {
        BT_ERR("%s, Invalid User Property ID 0x%04x", __func__, property_id);
        return;
    }

    property = gen_get_admin_property(model, property_id);
    if (property == NULL) {
        BT_WARN("%s, Admin property 0x%04x not exist", __func__, property_id);
        length = sizeof(property_id);
    } else {
        length = sizeof(property->id) + sizeof(property->admin_access) + property->val->len;
    }

    if (publish == false) {
        msg = bt_mesh_alloc_buf(1 + length + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        if (msg == NULL) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
    } else {
        msg = bt_mesh_server_get_pub_msg(model, 1 + length);
        if (msg == NULL) {
            return;
        }
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_STATUS);
    if (property == NULL) {
        net_buf_simple_add_le16(msg, property_id);
    } else {
        net_buf_simple_add_le16(msg, property->id);
        net_buf_simple_add_u8(msg, property->admin_access);
        if (ctx->recv_op != BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_GET ||
                property->admin_access != ADMIN_ACCESS_WRITE) {
            net_buf_simple_add_mem(msg, property->val->data, property->val->len);
        }
    }

    if (publish == false) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        bt_mesh_free_buf(msg);
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    }
    return;
}

static void gen_admin_prop_get(struct bt_mesh_model *model,
                               struct bt_mesh_msg_ctx *ctx,
                               struct net_buf_simple *buf)
{
    struct bt_mesh_gen_admin_prop_srv *srv = model->user_data;

    if (srv == NULL || srv->property_count == 0U || srv->properties == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_gen_server_recv_get_msg_t get = {0};
        const u8_t *param = NULL;
        size_t len = 0;
        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_GET) {
            get.admin_property_get.id = net_buf_simple_pull_le16(buf);
            param = (const u8_t *)&get;
            len = sizeof(get);
        }
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_GET_MSG, model, ctx, param, len);
        return;
    }

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_GET: {
        struct net_buf_simple *msg = NULL;
        u8_t i;
        msg = bt_mesh_alloc_buf(1 + srv->property_count * 2 + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        if (msg == NULL) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_STATUS);
        for (i = 0U; i < srv->property_count; i++) {
            net_buf_simple_add_le16(msg, srv->properties[i].id);
        }
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        bt_mesh_free_buf(msg);
        return;
    }
    case BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_GET: {
        u16_t property_id = net_buf_simple_pull_le16(buf);
        send_gen_admin_prop_status(model, ctx, property_id, false);
        return;
    }
    default:
        BT_WARN("%s, Unknown Generic Admin Property Get opcode 0x%04x", __func__, ctx->recv_op);
        return;
    }
}

static void gen_admin_prop_set(struct bt_mesh_model *model,
                               struct bt_mesh_msg_ctx *ctx,
                               struct net_buf_simple *buf)
{
    struct bt_mesh_gen_admin_prop_srv *srv = model->user_data;
    struct bt_mesh_generic_property *property = NULL;
    u16_t property_id;
    u8_t access;

    if (srv == NULL || srv->property_count == 0U || srv->properties == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    property_id = net_buf_simple_pull_le16(buf);
    access = net_buf_simple_pull_u8(buf);
    if (access > ADMIN_ACCESS_READ_WRITE) {
        BT_ERR("%s, Invalid Admin Access 0x%02x", __func__, access);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_gen_server_recv_set_msg_t set = {
            .admin_property_set.id = property_id,
            .admin_property_set.access = access,
            .admin_property_set.value = buf,
        };
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_SET_MSG, model, ctx, (const u8_t *)&set, sizeof(set));
        return;
    }

    property = gen_get_admin_property(model, property_id);
    if (property == NULL) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET) {
            send_gen_admin_prop_status(model, ctx, property_id, false);
        }
        send_gen_admin_prop_status(model, ctx, property_id, true);
        return;
    }

    property->admin_access = access;

    net_buf_simple_reset(property->val);
    net_buf_simple_add_mem(property->val, buf->data, MIN(buf->len, property->val->size));

    bt_mesh_gen_server_state_change_t change = {
        .gen_admin_prop_set.id = property_id,
        .gen_admin_prop_set.access = property->admin_access,
        .gen_admin_prop_set.value = property->val,
    };
    bt_mesh_generic_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_GENERIC_SERVER_STATE_CHANGE, model, ctx, (const u8_t *)&change, sizeof(change));

    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET) {
        send_gen_admin_prop_status(model, ctx, property_id, false);
    }
    send_gen_admin_prop_status(model, ctx, property_id, true);

    return;
}

/* Generic Manufacturer Property Server message handlers */
struct bt_mesh_generic_property *gen_get_manu_property(struct bt_mesh_model *model,
        u16_t property_id)
{
    struct bt_mesh_gen_manu_prop_srv *srv = model->user_data;
    u8_t i;

    for (i = 0U; i < srv->property_count; i++) {
        if (srv->properties[i].id == property_id) {
            return &srv->properties[i];
        }
    }

    return NULL;
}

static void send_gen_manu_prop_status(struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx,
                                      u16_t property_id, bool publish)
{
    struct bt_mesh_generic_property *property = NULL;
    struct net_buf_simple *msg = NULL;
    u16_t length;

    if (property_id == BLE_MESH_INVALID_DEVICE_PROPERTY_ID) {
        BT_ERR("%s, Invalid User Property ID 0x%04x", __func__, property_id);
        return;
    }

    property = gen_get_manu_property(model, property_id);
    if (property == NULL) {
        BT_WARN("%s, Manufacturer property 0x%04x not exist", __func__, property_id);
        length = sizeof(property_id);
    } else {
        length = sizeof(property->id) + sizeof(property->manu_access) + property->val->len;
    }

    if (publish == false) {
        msg = bt_mesh_alloc_buf(1 + length + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        if (msg == NULL) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
    } else {
        msg = bt_mesh_server_get_pub_msg(model, 1 + length);
        if (msg == NULL) {
            return;
        }
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_STATUS);
    if (property == NULL) {
        net_buf_simple_add_le16(msg, property_id);
    } else {
        net_buf_simple_add_le16(msg, property->id);
        net_buf_simple_add_u8(msg, property->manu_access);
        net_buf_simple_add_mem(msg, property->val->data, property->val->len);
    }

    if (publish == false) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        bt_mesh_free_buf(msg);
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    }
    return;
}

static void gen_manu_prop_get(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    struct bt_mesh_gen_manu_prop_srv *srv = model->user_data;

    if (srv == NULL || srv->property_count == 0U || srv->properties == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_gen_server_recv_get_msg_t get = {0};
        const u8_t *param = NULL;
        size_t len = 0;
        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_GET) {
            get.manu_property_get.id = net_buf_simple_pull_le16(buf);
            param = (const u8_t *)&get;
            len = sizeof(get);
        }
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_GET_MSG, model, ctx, param, len);
        return;
    }

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_GET: {
        struct net_buf_simple *msg = NULL;
        u8_t i;
        msg = bt_mesh_alloc_buf(1 + srv->property_count * 2 + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        if (msg == NULL) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_STATUS);
        for (i = 0U; i < srv->property_count; i++) {
            net_buf_simple_add_le16(msg, srv->properties[i].id);
        }
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        bt_mesh_free_buf(msg);
        return;
    }
    case BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_GET: {
        u16_t property_id = net_buf_simple_pull_le16(buf);
        send_gen_manu_prop_status(model, ctx, property_id, false);
        return;
    }
    default:
        BT_WARN("%s, Unknown Generic Manufacturer Property Get opcode 0x%04x", __func__, ctx->recv_op);
        return;
    }
}

static void gen_manu_prop_set(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    struct bt_mesh_gen_manu_prop_srv *srv = model->user_data;
    struct bt_mesh_generic_property *property = NULL;
    u16_t property_id;
    u8_t access;

    if (srv == NULL || srv->property_count == 0U || srv->properties == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    property_id = net_buf_simple_pull_le16(buf);
    access = net_buf_simple_pull_u8(buf);
    if (access > MANU_ACCESS_READ) {
        BT_ERR("%s, Invalid Manufacturer Access 0x%02x", __func__, access);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_gen_server_recv_set_msg_t set = {
            .manu_property_set.id = property_id,
            .manu_property_set.access = access,
        };
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_SET_MSG, model, ctx, (const u8_t *)&set, sizeof(set));
        return;
    }

    property = gen_get_manu_property(model, property_id);
    if (property == NULL) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET) {
            send_gen_manu_prop_status(model, ctx, property_id, false);
        }
        send_gen_manu_prop_status(model, ctx, property_id, true);
        return;
    }

    property->manu_access = access;

    bt_mesh_gen_server_state_change_t change = {
        .gen_manu_prop_set.id = property_id,
        .gen_manu_prop_set.access = property->manu_access,
    };
    bt_mesh_generic_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_GENERIC_SERVER_STATE_CHANGE, model, ctx, (const u8_t *)&change, sizeof(change));

    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET) {
        send_gen_manu_prop_status(model, ctx, property_id, false);
    }
    send_gen_manu_prop_status(model, ctx, property_id, true);

    return;
}

/* Generic Client Property Server message handlers */
static int search_prop_id_index(const u16_t *array, u8_t array_idx, u16_t id)
{
    static const u16_t *start = NULL;
    u8_t index;
    u16_t temp;

    if (start == NULL) {
        start = array;
    }

    if (array_idx == 0U) {
        if (*array >= id) {
            return array - start;
        } else {
            return -1;
        }
    }

    index = array_idx / 2;
    temp = array[index];

    if (temp == id) {
        return array + index - start;
    } else if (temp > id) {
        return search_prop_id_index(array, index, id);
    } else {
        return search_prop_id_index(array + index + 1, array_idx - 1 - index, id);
    }
}

static void gen_client_prop_get(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf)
{
    struct bt_mesh_gen_client_prop_srv *srv = model->user_data;
    struct net_buf_simple *sdu = NULL;
    u16_t total_len = 5;
    u16_t property_id;
    int i, index;

    if (srv == NULL || srv->id_count == 0U || srv->property_ids == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_gen_server_recv_get_msg_t get = {
            .client_properties_get.id = net_buf_simple_pull_le16(buf),
        };
        bt_mesh_generic_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_GENERIC_SERVER_RECV_GET_MSG, model, ctx, (const u8_t *)&get, sizeof(get));
        return;
    }

    /* The sequence shall be in an ascending order of Property ID values and shall
     * start with a smallest Property ID that is greater than or equal to the value
     * of the Generic Client Property field of the Generic Client Properities Get
     * message that it is responding to.
     */

    property_id = net_buf_simple_pull_le16(buf);
    index = search_prop_id_index(srv->property_ids, srv->id_count - 1, property_id);
    if (index < 0) {
        NET_BUF_SIMPLE_DEFINE(msg, 1 + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        bt_mesh_model_msg_init(&msg, BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_STATUS);
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, &msg, NULL, NULL));
        return;
    }

    sdu = bt_mesh_alloc_buf(MIN(BLE_MESH_TX_SDU_MAX, BLE_MESH_SERVER_RSP_MAX_LEN));
    if (sdu == NULL) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        return;
    }

    bt_mesh_model_msg_init(sdu, BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_STATUS);
    for (i = index; i < srv->id_count; i++) {
        total_len += sizeof(u16_t);
        if (total_len > MIN(BLE_MESH_TX_SDU_MAX, BLE_MESH_SERVER_RSP_MAX_LEN)) {
            /* Add this in case the message is too long */
            break;
        }
        net_buf_simple_add_le16(sdu, srv->property_ids[i]);
    }

    BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, sdu, NULL, NULL));
    bt_mesh_free_buf(sdu);
    return;
}

/* message handlers (End) */

/* Mapping of message handlers for Generic OnOff Server (0x1000) */
const struct bt_mesh_model_op gen_onoff_srv_op[] = {
    { BLE_MESH_MODEL_OP_GEN_ONOFF_GET,       0, gen_onoff_get },
    { BLE_MESH_MODEL_OP_GEN_ONOFF_SET,       2, gen_onoff_set },
    { BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK, 2, gen_onoff_set },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Generic Levl Server (0x1002) */
const struct bt_mesh_model_op gen_level_srv_op[] = {
    { BLE_MESH_MODEL_OP_GEN_LEVEL_GET,       0, gen_level_get },
    { BLE_MESH_MODEL_OP_GEN_LEVEL_SET,       3, gen_level_set },
    { BLE_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK, 3, gen_level_set },
    { BLE_MESH_MODEL_OP_GEN_DELTA_SET,       5, gen_delta_set },
    { BLE_MESH_MODEL_OP_GEN_DELTA_SET_UNACK, 5, gen_delta_set },
    { BLE_MESH_MODEL_OP_GEN_MOVE_SET,        3, gen_move_set  },
    { BLE_MESH_MODEL_OP_GEN_MOVE_SET_UNACK,  3, gen_move_set  },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Generic Default TT Server (0x1004) */
const struct bt_mesh_model_op gen_def_trans_time_srv_op[] = {
    { BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_GET,       0, gen_def_trans_time_get },
    { BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET,       1, gen_def_trans_time_set },
    { BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET_UNACK, 1, gen_def_trans_time_set },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Generic Power OnOff Server (0x1006) */
const struct bt_mesh_model_op gen_power_onoff_srv_op[] = {
    { BLE_MESH_MODEL_OP_GEN_ONPOWERUP_GET, 0, gen_onpowerup_get },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Generic Power OnOff Setup Server (0x1007) */
const struct bt_mesh_model_op gen_power_onoff_setup_srv_op[] = {
    { BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET,       1, gen_onpowerup_set },
    { BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET_UNACK, 1, gen_onpowerup_set },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Generic Power Level Server (0x1009) */
const struct bt_mesh_model_op gen_power_level_srv_op[] = {
    { BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET,       0, gen_power_level_get },
    { BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET,       3, gen_power_level_set },
    { BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET_UNACK, 3, gen_power_level_set },
    { BLE_MESH_MODEL_OP_GEN_POWER_LAST_GET,        0, gen_power_level_get },
    { BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_GET,     0, gen_power_level_get },
    { BLE_MESH_MODEL_OP_GEN_POWER_RANGE_GET,       0, gen_power_level_get },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Generic Power Level Setup Server (0x100A) */
const struct bt_mesh_model_op gen_power_level_setup_srv_op[] = {
    { BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET,       2, gen_power_default_set },
    { BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET_UNACK, 2, gen_power_default_set },
    { BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET,         4, gen_power_range_set   },
    { BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET_UNACK,   4, gen_power_range_set   },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Generic Battery Server (0x100C) */
const struct bt_mesh_model_op gen_battery_srv_op[] = {
    { BLE_MESH_MODEL_OP_GEN_BATTERY_GET, 0, gen_battery_get },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Generic Location Server (0x100E) */
const struct bt_mesh_model_op gen_location_srv_op[] = {
    { BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_GET, 0, gen_location_get },
    { BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_GET,  0, gen_location_get },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Generic Location Setup Server (0x100F) */
const struct bt_mesh_model_op gen_location_setup_srv_op[] = {
    { BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET,       10, gen_location_set },
    { BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET_UNACK, 10, gen_location_set },
    { BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET,         9, gen_location_set },
    { BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET_UNACK,   9, gen_location_set },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Generic User Property Server (0x1013) */
const struct bt_mesh_model_op gen_user_prop_srv_op[] = {
    { BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_GET,     0, gen_user_prop_get },
    { BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET,       2, gen_user_prop_get },
    { BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET,       3, gen_user_prop_set },
    { BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET_UNACK, 3, gen_user_prop_set },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Generic Admin Property Server (0x1011) */
const struct bt_mesh_model_op gen_admin_prop_srv_op[] = {
    { BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_GET,     0, gen_admin_prop_get },
    { BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_GET,       2, gen_admin_prop_get },
    { BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET,       4, gen_admin_prop_set },
    { BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET_UNACK, 4, gen_admin_prop_set },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Generic Manufacturer Property Server (0x1012) */
const struct bt_mesh_model_op gen_manu_prop_srv_op[] = {
    { BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_GET,     0, gen_manu_prop_get },
    { BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_GET,       2, gen_manu_prop_get },
    { BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET,       3, gen_manu_prop_set },
    { BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET_UNACK, 3, gen_manu_prop_set },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Generic Client Property Server (0x1014) */
const struct bt_mesh_model_op gen_client_prop_srv_op[] = {
    { BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_GET, 2, gen_client_prop_get },
    BLE_MESH_MODEL_OP_END,
};

static inline int property_id_compare(const void *p1, const void *p2)
{
    if (*(u16_t *)p1 < * (u16_t *)p2) {
        return -1;
    }
    if (*(u16_t *)p1 > *(u16_t *)p2) {
        return 1;
    }
    return 0;
}

static int generic_server_init(struct bt_mesh_model *model)
{
    if (model->user_data == NULL) {
        BT_ERR("%s, No Generic Server context provided, model_id 0x%04x", __func__, model->id);
        return -EINVAL;
    }

    switch (model->id) {
    case BLE_MESH_MODEL_ID_GEN_ONOFF_SRV: {
        struct bt_mesh_gen_onoff_srv *srv = model->user_data;
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_AUTO_RSP) {
            bt_mesh_server_alloc_ctx(&srv->transition.timer.work);
            k_delayed_work_init(&srv->transition.timer, generic_onoff_work_handler);
        }
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_LEVEL_SRV: {
        struct bt_mesh_gen_level_srv *srv = model->user_data;
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_AUTO_RSP) {
            bt_mesh_server_alloc_ctx(&srv->transition.timer.work);
            k_delayed_work_init(&srv->transition.timer, generic_level_work_handler);
        }
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_SRV: {
        struct bt_mesh_gen_def_trans_time_srv *srv = model->user_data;
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV: {
        struct bt_mesh_gen_power_onoff_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("%s, NULL Generic OnPowerUp State", __func__);
            return -EINVAL;
        }
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SETUP_SRV: {
        struct bt_mesh_gen_power_onoff_setup_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("%s, NULL Generic OnPowerUp State", __func__);
            return -EINVAL;
        }
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV: {
        struct bt_mesh_gen_power_level_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("%s, NULL Generic Power Level State", __func__);
            return -EINVAL;
        }
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_AUTO_RSP) {
            bt_mesh_server_alloc_ctx(&srv->transition.timer.work);
            k_delayed_work_init(&srv->transition.timer, generic_power_level_work_handler);
        }
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SETUP_SRV: {
        struct bt_mesh_gen_power_level_setup_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("%s, NULL Generic Power Level State", __func__);
            return -EINVAL;
        }
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_BATTERY_SRV: {
        struct bt_mesh_gen_battery_srv *srv = model->user_data;
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_LOCATION_SRV: {
        struct bt_mesh_gen_location_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("%s, NULL Generic Location State", __func__);
            return -EINVAL;
        }
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_LOCATION_SETUP_SRV: {
        struct bt_mesh_gen_location_setup_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("%s, NULL Generic Location State", __func__);
            return -EINVAL;
        }
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_USER_PROP_SRV: {
        struct bt_mesh_gen_user_prop_srv *srv = model->user_data;
        if (srv->property_count == 0U || srv->properties == NULL) {
            BT_ERR("%s, NULL Generic User Property State", __func__);
            return -EINVAL;
        }
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_ADMIN_PROP_SRV: {
        struct bt_mesh_gen_admin_prop_srv *srv = model->user_data;
        if (srv->property_count == 0U || srv->properties == NULL) {
            BT_ERR("%s, NULL Generic Admin Property State", __func__);
            return -EINVAL;
        }
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_MANUFACTURER_PROP_SRV: {
        struct bt_mesh_gen_manu_prop_srv *srv = model->user_data;
        if (srv->property_count == 0U || srv->properties == NULL) {
            BT_ERR("%s, NULL Generic Manufacturer Property State", __func__);
            return -EINVAL;
        }
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_GEN_CLIENT_PROP_SRV: {
        struct bt_mesh_gen_client_prop_srv *srv = model->user_data;
        if (srv->id_count == 0U || srv->property_ids == NULL) {
            BT_ERR("%s, NULL Generic Client Property State", __func__);
            return -EINVAL;
        }
        /* Quick sort the Client Property IDs in ascending order */
        qsort(srv->property_ids, srv->id_count, sizeof(u16_t), property_id_compare);
        srv->model = model;
        break;
    }
    default:
        BT_WARN("%s, Unknown Generic Server Model, model_id 0x%04x", __func__, model->id);
        return -EINVAL;
    }

    bt_mesh_generic_server_mutex_new();

    return 0;
}

int bt_mesh_gen_onoff_srv_init(struct bt_mesh_model *model, bool primary)
{
    if (model->pub == NULL) {
        BT_ERR("%s, Generic OnOff Server has no publication support", __func__);
        return -EINVAL;
    }

    return generic_server_init(model);
}

int bt_mesh_gen_level_srv_init(struct bt_mesh_model *model, bool primary)
{
    if (model->pub == NULL) {
        BT_ERR("%s, Generic Level Server has no publication support", __func__);
        return -EINVAL;
    }

    return generic_server_init(model);
}

int bt_mesh_gen_def_trans_time_srv_init(struct bt_mesh_model *model, bool primary)
{
    if (model->pub == NULL) {
        BT_ERR("%s, Generic Default Trans Time Server has no publication support", __func__);
        return -EINVAL;
    }

    return generic_server_init(model);
}

int bt_mesh_gen_power_onoff_srv_init(struct bt_mesh_model *model, bool primary)
{
    if (model->pub == NULL) {
        BT_ERR("%s, Generic Power OnOff Server has no publication support", __func__);
        return -EINVAL;
    }

    /* When this model is present on an element, the corresponding Generic
     * Power OnOff Setup Server model shall also be present.
     */
    struct bt_mesh_elem *element = bt_mesh_model_elem(model);
    if (bt_mesh_model_find(element, BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SETUP_SRV) == NULL) {
        BT_WARN("%s, Generic Power OnOff Setup Server is not present", __func__);
        /* Just give a warning here, continue with the initialization */
    }
    return generic_server_init(model);
}

int bt_mesh_gen_power_onoff_setup_srv_init(struct bt_mesh_model *model, bool primary)
{
    return generic_server_init(model);
}

int bt_mesh_gen_power_level_srv_init(struct bt_mesh_model *model, bool primary)
{
    if (model->pub == NULL) {
        BT_ERR("%s, Generic Power Level Server has no publication support", __func__);
        return -EINVAL;
    }

    /* When this model is present on an Element, the corresponding Generic
     * Power Level Setup Server model shall also be present.
     */
    struct bt_mesh_elem *element = bt_mesh_model_elem(model);
    if (bt_mesh_model_find(element, BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SETUP_SRV) == NULL) {
        BT_WARN("%s, Generic Power Level Setup Server is not present", __func__);
        /* Just give a warning here, continue with the initialization */
    }
    return generic_server_init(model);
}

int bt_mesh_gen_power_level_setup_srv_init(struct bt_mesh_model *model, bool primary)
{
    return generic_server_init(model);
}

int bt_mesh_gen_battery_srv_init(struct bt_mesh_model *model, bool primary)
{
    if (model->pub == NULL) {
        BT_ERR("%s, Generic Battery Server has no publication support", __func__);
        return -EINVAL;
    }

    return generic_server_init(model);
}

int bt_mesh_gen_location_srv_init(struct bt_mesh_model *model, bool primary)
{
    if (model->pub == NULL) {
        BT_ERR("%s, Generic Location Server has no publication support", __func__);
        return -EINVAL;
    }

    return generic_server_init(model);
}

int bt_mesh_gen_location_setup_srv_init(struct bt_mesh_model *model, bool primary)
{
    /* When this model is present on an Element, the corresponding Generic
     * Location Setup Server model shall also be present.
     */
    struct bt_mesh_elem *element = bt_mesh_model_elem(model);
    if (bt_mesh_model_find(element, BLE_MESH_MODEL_ID_GEN_LOCATION_SETUP_SRV) == NULL) {
        BT_WARN("%s, Generic Location Setup Server is not present", __func__);
        /* Just give a warning here, continue with the initialization */
    }
    return generic_server_init(model);
}

int bt_mesh_gen_user_prop_srv_init(struct bt_mesh_model *model, bool primary)
{
    if (model->pub == NULL) {
        BT_ERR("%s, Generic User Property has no publication support", __func__);
        return -EINVAL;
    }

    return generic_server_init(model);
}

int bt_mesh_gen_admin_prop_srv_init(struct bt_mesh_model *model, bool primary)
{
    if (model->pub == NULL) {
        BT_ERR("%s, Generic Admin Property has no publication support", __func__);
        return -EINVAL;
    }

    return generic_server_init(model);
}

int bt_mesh_gen_manu_prop_srv_init(struct bt_mesh_model *model, bool primary)
{
    if (model->pub == NULL) {
        BT_ERR("%s, Generic Manufacturer Property has no publication support", __func__);
        return -EINVAL;
    }

    return generic_server_init(model);
}

int bt_mesh_gen_client_prop_srv_init(struct bt_mesh_model *model, bool primary)
{
    if (model->pub == NULL) {
        BT_ERR("%s, Generic Client Property has no publication support", __func__);
        return -EINVAL;
    }

    return generic_server_init(model);
}
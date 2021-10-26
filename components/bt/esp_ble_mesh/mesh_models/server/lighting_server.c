/* Bluetooth: Mesh Lighting Server Models
 *
 * SPDX-FileCopyrightText: 2018 Vikrant More
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include "btc_ble_mesh_lighting_model.h"

#include "mesh_config.h"
#include "access.h"
#include "transport.h"
#include "model_opcode.h"
#include "state_transition.h"
#include "device_property.h"

#if CONFIG_BLE_MESH_LIGHTING_SERVER

static bt_mesh_mutex_t light_server_lock;

static inline void bt_mesh_light_server_mutex_new(void)
{
    if (!light_server_lock.mutex) {
        bt_mesh_mutex_create(&light_server_lock);
    }
}

#if CONFIG_BLE_MESH_DEINIT
static inline void bt_mesh_light_server_mutex_free(void)
{
    bt_mesh_mutex_free(&light_server_lock);
}
#endif /* CONFIG_BLE_MESH_DEINIT */

void bt_mesh_light_server_lock(void)
{
    bt_mesh_mutex_lock(&light_server_lock);
}

void bt_mesh_light_server_unlock(void)
{
    bt_mesh_mutex_unlock(&light_server_lock);
}

/* message handlers (Start) */

/* Light Lightness Server/Setup Server message handlers */

static void send_light_lightness_status(struct bt_mesh_model *model,
                                        struct bt_mesh_msg_ctx *ctx,
                                        bool publish, uint16_t opcode)
{
    struct net_buf_simple *msg = NULL;
    uint8_t length = 2 + 5;

    if (ctx == NULL && publish == false) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    if (publish == false) {
        msg = bt_mesh_alloc_buf(length + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        if (msg == NULL) {
            BT_ERR("%s, Out of memory", __func__);
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
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS: {
        struct bt_mesh_light_lightness_srv *srv = model->user_data;
        net_buf_simple_add_le16(msg, srv->state->lightness_actual);
        if (srv->actual_transition.counter) {
            bt_mesh_server_calc_remain_time(&srv->actual_transition);
            net_buf_simple_add_le16(msg, srv->state->target_lightness_actual);
            net_buf_simple_add_u8(msg, srv->actual_transition.remain_time);
        }
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS: {
        struct bt_mesh_light_lightness_srv *srv = model->user_data;
        net_buf_simple_add_le16(msg, srv->state->lightness_linear);
        if (srv->linear_transition.counter) {
            bt_mesh_server_calc_remain_time(&srv->linear_transition);
            net_buf_simple_add_le16(msg, srv->state->target_lightness_linear);
            net_buf_simple_add_u8(msg, srv->linear_transition.remain_time);
        }
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_STATUS: {
        struct bt_mesh_light_lightness_srv *srv = model->user_data;
        net_buf_simple_add_le16(msg, srv->state->lightness_last);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS:
        if (model->id == BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV) {
            struct bt_mesh_light_lightness_srv *srv = model->user_data;
            net_buf_simple_add_le16(msg, srv->state->lightness_default);
        } else if (model->id == BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV) {
            struct bt_mesh_light_lightness_setup_srv *srv = model->user_data;
            net_buf_simple_add_le16(msg, srv->state->lightness_default);
        }
        break;
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS:
        if (model->id == BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV) {
            struct bt_mesh_light_lightness_srv *srv = model->user_data;
            net_buf_simple_add_u8(msg, srv->state->status_code);
            net_buf_simple_add_le16(msg, srv->state->lightness_range_min);
            net_buf_simple_add_le16(msg, srv->state->lightness_range_max);
        } else if (model->id == BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV) {
            struct bt_mesh_light_lightness_setup_srv *srv = model->user_data;
            net_buf_simple_add_u8(msg, srv->state->status_code);
            net_buf_simple_add_le16(msg, srv->state->lightness_range_min);
            net_buf_simple_add_le16(msg, srv->state->lightness_range_max);
        }
        break;
    default:
        BT_WARN("Unknown Light Lightness status opcode 0x%04x", opcode);
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

static void light_lightness_get(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf)
{
    struct bt_mesh_light_lightness_srv *srv = model->user_data;
    uint16_t opcode = 0U;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_GET_MSG, model, ctx, NULL, 0);
        return;
    }

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET:
        opcode = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS;
        break;
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_GET:
        opcode = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS;
        break;
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_GET:
        opcode = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_STATUS;
        break;
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_GET:
        opcode = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS;
        break;
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_GET:
        opcode = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS;
        break;
    default:
        BT_WARN("Unknown Light Lightness Get opcode 0x%04x", ctx->recv_op);
        return;
    }

    send_light_lightness_status(model, ctx, false, opcode);
    return;
}

void light_lightness_publish(struct bt_mesh_model *model, uint16_t opcode)
{
    if (model->user_data == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    switch (model->id) {
    case BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV: {
        struct bt_mesh_light_lightness_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light Lightness Server state");
            return;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV: {
        struct bt_mesh_light_lightness_setup_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light Lightness Setup Server state");
            return;
        }
        break;
    }
    default:
        BT_ERR("Invalid Light Lightness Server model 0x%04x", model->id);
        return;
    }

    send_light_lightness_status(model, NULL, true, opcode);
    return;
}

static void light_lightness_set(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf)
{
    struct bt_mesh_light_lightness_srv *srv = model->user_data;
    uint8_t tid = 0U, trans_time = 0U, delay = 0U;
    bool optional = false;
    uint16_t actual = 0U;
    int64_t now = 0;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    actual = net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    if (bt_mesh_server_get_optional(model, ctx, buf, &trans_time, &delay, &optional)) {
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_light_server_recv_set_msg_t set = {
            .lightness_set.op_en = optional,
            .lightness_set.lightness = actual,
            .lightness_set.tid = tid,
            .lightness_set.trans_time = trans_time,
            .lightness_set.delay = delay,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_SET_MSG, model, ctx, (const uint8_t *)&set, sizeof(set));
        return;
    }

    if (bt_mesh_is_server_recv_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now)) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET) {
            send_light_lightness_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS);
        }
        send_light_lightness_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS);
        /* In this condition, no event will be callback to application layer */
        return;
    }

    bt_mesh_light_server_lock();

    bt_mesh_server_stop_transition(&srv->actual_transition);
    bt_mesh_server_update_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now);

    if (actual) {
        if (srv->state->lightness_range_min && actual < srv->state->lightness_range_min) {
            actual = srv->state->lightness_range_min;
        } else if (srv->state->lightness_range_max && actual > srv->state->lightness_range_max) {
            actual = srv->state->lightness_range_max;
        }
    }
    srv->state->target_lightness_actual = actual;

    /**
     * If the target state is equal to the current state, the transition shall not be
     * started and is considered complete.
     */
    if (srv->state->target_lightness_actual != srv->state->lightness_actual) {
        light_lightness_actual_tt_values(srv, trans_time, delay);
    } else {
        bt_mesh_light_server_state_change_t change = {
            .lightness_set.lightness = srv->state->lightness_actual,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));

        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET) {
            send_light_lightness_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS);
        }
        send_light_lightness_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS);

        bt_mesh_light_server_unlock();
        return;
    }

    /* Copy the ctx of the received message */
    if (srv->actual_transition.timer.work._reserved) {
        memcpy(srv->actual_transition.timer.work._reserved, ctx, sizeof(struct bt_mesh_msg_ctx));
    }

    /* For Instantaneous Transition */
    if (srv->actual_transition.counter == 0U) {
        srv->state->lightness_actual = srv->state->target_lightness_actual;
        /**
         * Whenever the Light Lightness Actual state is changed with a non-transactional
         * message or a completed sequence of transactional messages to a non-zero value,
         * the value of the Light Lightness Last shall be set to the value of the Light
         * Lightness Actual.
         */
        if (srv->state->lightness_actual) {
            srv->state->lightness_last = srv->state->lightness_actual;
        }
    }

    srv->actual_transition.just_started = true;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET) {
        send_light_lightness_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS);
    }
    send_light_lightness_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS);

    bt_mesh_light_server_unlock();

    bt_mesh_server_start_transition(&srv->actual_transition);
    return;
}

static void light_lightness_linear_set(struct bt_mesh_model *model,
                                       struct bt_mesh_msg_ctx *ctx,
                                       struct net_buf_simple *buf)
{
    struct bt_mesh_light_lightness_srv *srv = model->user_data;
    uint8_t tid = 0U, trans_time = 0U, delay = 0U;
    bool optional = false;
    uint16_t linear = 0U;
    int64_t now = 0;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    linear = net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    if (bt_mesh_server_get_optional(model, ctx, buf, &trans_time, &delay, &optional)) {
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_light_server_recv_set_msg_t set = {
            .lightness_linear_set.op_en = optional,
            .lightness_linear_set.lightness = linear,
            .lightness_linear_set.tid = tid,
            .lightness_linear_set.trans_time = trans_time,
            .lightness_linear_set.delay = delay,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_SET_MSG, model, ctx, (const uint8_t *)&set, sizeof(set));
        return;
    }

    if (bt_mesh_is_server_recv_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now)) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET) {
            send_light_lightness_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS);
        }
        send_light_lightness_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS);
        /* In this condition, no event will be callback to application layer */
        return;
    }

    bt_mesh_light_server_lock();

    bt_mesh_server_stop_transition(&srv->linear_transition);
    bt_mesh_server_update_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now);

    srv->state->target_lightness_linear = linear;

    /**
     * If the target state is equal to the current state, the transition shall not
     * be started and is considered complete.
     */
    if (srv->state->target_lightness_linear != srv->state->lightness_linear) {
        light_lightness_linear_tt_values(srv, trans_time, delay);
    } else {
        bt_mesh_light_server_state_change_t change = {
            .lightness_linear_set.lightness = srv->state->lightness_actual,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));

        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET) {
            send_light_lightness_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS);
        }
        send_light_lightness_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS);

        bt_mesh_light_server_unlock();
        return;
    }

    /* Copy the ctx of the received message */
    if (srv->linear_transition.timer.work._reserved) {
        memcpy(srv->linear_transition.timer.work._reserved, ctx, sizeof(struct bt_mesh_msg_ctx));
    }

    /* For Instantaneous Transition */
    if (srv->linear_transition.counter == 0U) {
        srv->state->lightness_linear = srv->state->target_lightness_linear;
    }

    srv->linear_transition.just_started = true;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET) {
        send_light_lightness_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS);
    }
    send_light_lightness_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS);

    bt_mesh_light_server_unlock();

    bt_mesh_server_start_transition(&srv->linear_transition);
    return;
}

static void light_lightness_default_set(struct bt_mesh_model *model,
                                        struct bt_mesh_msg_ctx *ctx,
                                        struct net_buf_simple *buf)
{
    struct bt_mesh_light_lightness_setup_srv *srv = model->user_data;
    uint16_t lightness = 0U;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    lightness = net_buf_simple_pull_le16(buf);

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_light_server_recv_set_msg_t set = {
            .lightness_default_set.lightness = lightness,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_SET_MSG, model, ctx, (const uint8_t *)&set, sizeof(set));
        return;
    }

    if (srv->state->lightness_default != lightness) {
        srv->state->lightness_default = lightness;

        bt_mesh_light_server_state_change_t change = {
            .lightness_default_set.lightness = lightness,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));
    }

    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET) {
        send_light_lightness_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS);
    }
    send_light_lightness_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS);

    return;
}

static void light_lightness_range_set(struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx,
                                      struct net_buf_simple *buf)
{
    struct bt_mesh_light_lightness_setup_srv *srv = model->user_data;
    uint16_t range_min = 0U, range_max = 0U;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    range_min = net_buf_simple_pull_le16(buf);
    range_max = net_buf_simple_pull_le16(buf);

    if (range_min > range_max) {
        BT_ERR("Range min 0x%04x is greater than range max 0x%04x",
                range_min, range_max);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_light_server_recv_set_msg_t set = {
            .lightness_range_set.range_min = range_min,
            .lightness_range_set.range_max = range_max,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_SET_MSG, model, ctx, (const uint8_t *)&set, sizeof(set));
        return;
    }

    /**
     * When a Light Lightness Setup Server receives a Light Lightness Range Set
     * message or a Light Lightness Range Set Unacknowledged message with values
     * that cannot be accepted, it shall set the status of the operation to a
     * value representing the reason why the values cannot be accepted.
     *
     * TODO: 0x0000 for Light Range Min/Max is prohibited, but BQB test case
     * MMDL/SR/LLNS/BI-01-C requires 'SUCCESS' when it sends a set message with
     * Light Range Min set to 0x0000.
     */
#if 0
    srv->state->status_code = BLE_MESH_RANGE_UPDATE_SUCCESS;
#else
    if (range_min == 0x0000) {
        srv->state->status_code = BLE_MESH_CANNOT_SET_RANGE_MIN;
    } else if (range_max == 0x0000) {
        srv->state->status_code = BLE_MESH_CANNOT_SET_RANGE_MAX;
    } else {
        srv->state->status_code = BLE_MESH_RANGE_UPDATE_SUCCESS;
    }
#endif

    if (range_min && srv->state->lightness_range_min != range_min) {
        srv->state->lightness_range_min = range_min;
    }

    if (range_max && srv->state->lightness_range_max != range_max) {
        srv->state->lightness_range_max = range_max;
    }

    bt_mesh_light_server_state_change_t change = {
        .lightness_range_set.range_min = srv->state->lightness_range_min,
        .lightness_range_set.range_max = srv->state->lightness_range_max,
    };
    bt_mesh_lighting_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));

    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET) {
        send_light_lightness_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS);
    }
    send_light_lightness_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS);

    return;
}

/* Light CTL Server/Temperature Server/Setup Server message handlers */

static void send_light_ctl_status(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  bool publish, uint16_t opcode)
{
    struct net_buf_simple *msg = NULL;
    uint8_t length = 2 + 9;

    if (ctx == NULL && publish == false) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    if (publish == false) {
        msg = bt_mesh_alloc_buf(length + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        if (msg == NULL) {
            BT_ERR("%s, Out of memory", __func__);
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
    case BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS: {
        struct bt_mesh_light_ctl_srv *srv = model->user_data;
        net_buf_simple_add_le16(msg, srv->state->lightness);
        net_buf_simple_add_le16(msg, srv->state->temperature);
        if (srv->transition.counter) {
            bt_mesh_server_calc_remain_time(&srv->transition);
            net_buf_simple_add_le16(msg, srv->state->target_lightness);
            net_buf_simple_add_le16(msg, srv->state->target_temperature);
            net_buf_simple_add_u8(msg, srv->transition.remain_time);
        }
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS:
        if (model->id == BLE_MESH_MODEL_ID_LIGHT_CTL_SRV) {
            struct bt_mesh_light_ctl_srv *srv = model->user_data;
            net_buf_simple_add_u8(msg, srv->state->status_code);
            net_buf_simple_add_le16(msg, srv->state->temperature_range_min);
            net_buf_simple_add_le16(msg, srv->state->temperature_range_max);
        } else if (model->id == BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV) {
            struct bt_mesh_light_ctl_setup_srv *srv = model->user_data;
            net_buf_simple_add_u8(msg, srv->state->status_code);
            net_buf_simple_add_le16(msg, srv->state->temperature_range_min);
            net_buf_simple_add_le16(msg, srv->state->temperature_range_max);
        }
        break;
    case BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS: {
        if (model->id == BLE_MESH_MODEL_ID_LIGHT_CTL_SRV) {
            struct bt_mesh_light_ctl_srv *srv = model->user_data;
            net_buf_simple_add_le16(msg, srv->state->lightness_default);
            net_buf_simple_add_le16(msg, srv->state->temperature_default);
            net_buf_simple_add_le16(msg, srv->state->delta_uv_default);
        } else if (model->id == BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV) {
            struct bt_mesh_light_ctl_setup_srv *srv = model->user_data;
            net_buf_simple_add_le16(msg, srv->state->lightness_default);
            net_buf_simple_add_le16(msg, srv->state->temperature_default);
            net_buf_simple_add_le16(msg, srv->state->delta_uv_default);
        }
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS: {
        struct bt_mesh_light_ctl_temp_srv *srv = model->user_data;
        net_buf_simple_add_le16(msg, srv->state->temperature);
        net_buf_simple_add_le16(msg, srv->state->delta_uv);
        if (srv->transition.counter) {
            bt_mesh_server_calc_remain_time(&srv->transition);
            net_buf_simple_add_le16(msg, srv->state->target_temperature);
            net_buf_simple_add_le16(msg, srv->state->target_delta_uv);
            net_buf_simple_add_u8(msg, srv->transition.remain_time);
        }
        break;
    }
    default:
        BT_WARN("Unknown Light CTL status opcode 0x%04x", opcode);
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

static void light_ctl_get(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    struct bt_mesh_server_rsp_ctrl *rsp_ctrl = NULL;
    uint16_t opcode = 0U;

    if (model->user_data == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    switch (model->id) {
    case BLE_MESH_MODEL_ID_LIGHT_CTL_SRV: {
        struct bt_mesh_light_ctl_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light CTL Server state");
            return;
        }
        rsp_ctrl = &srv->rsp_ctrl;
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV: {
        struct bt_mesh_light_ctl_temp_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light CTL Temperature Server state");
            return;
        }
        rsp_ctrl = &srv->rsp_ctrl;
        break;
    }
    default:
        BT_ERR("Invalid Light CTL Server model 0x%04x", model->id);
        return;
    }

    /* Callback the received message to the application layer */
    if (rsp_ctrl->get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_GET_MSG, model, ctx, NULL, 0);
        return;
    }

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_LIGHT_CTL_GET:
        opcode = BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS;
        break;
    case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_GET:
        opcode = BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS;
        break;
    case BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_GET:
        opcode = BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS;
        break;
    case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_GET:
        opcode = BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS;
        break;
    default:
        BT_WARN("Unknown Light CTL Get opcode 0x%04x", ctx->recv_op);
        return;
    }

    send_light_ctl_status(model, ctx, false, opcode);
    return;
}

void light_ctl_publish(struct bt_mesh_model *model, uint16_t opcode)
{
    if (model->user_data == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    switch (model->id) {
    case BLE_MESH_MODEL_ID_LIGHT_CTL_SRV: {
        struct bt_mesh_light_ctl_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light CTL Server state");
            return;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV: {
        struct bt_mesh_light_ctl_temp_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light CTL Temperature Server state");
            return;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV: {
        struct bt_mesh_light_ctl_setup_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light CTL Setup Server state");
            return;
        }
        break;
    }
    default:
        BT_ERR("Invalid Light CTL Server model 0x%04x", model->id);
        return;
    }

    send_light_ctl_status(model, NULL, true, opcode);
    return;
}

static void light_ctl_set(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    struct bt_mesh_light_ctl_srv *srv = model->user_data;
    uint16_t lightness = 0U, temperature = 0U;
    uint8_t tid = 0U, trans_time = 0U, delay = 0U;
    int16_t delta_uv = 0;
    bool optional = false;
    int64_t now = 0;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    lightness = net_buf_simple_pull_le16(buf);
    temperature = net_buf_simple_pull_le16(buf);
    delta_uv = (int16_t) net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    if (temperature < BLE_MESH_TEMPERATURE_MIN || temperature > BLE_MESH_TEMPERATURE_MAX) {
        BT_ERR("Invalid temperature 0x%04x", temperature);
        return;
    }

    if (bt_mesh_server_get_optional(model, ctx, buf, &trans_time, &delay, &optional)) {
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_light_server_recv_set_msg_t set = {
            .ctl_set.op_en = optional,
            .ctl_set.lightness = lightness,
            .ctl_set.temperature = temperature,
            .ctl_set.delta_uv = delta_uv,
            .ctl_set.tid = tid,
            .ctl_set.trans_time = trans_time,
            .ctl_set.delay = delay,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_SET_MSG, model, ctx, (const uint8_t *)&set, sizeof(set));
        return;
    }

    if (bt_mesh_is_server_recv_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now)) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_CTL_SET) {
            send_light_ctl_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS);
        }
        send_light_ctl_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS);
        /* In this condition, no event will be callback to application layer */
        return;
    }

    bt_mesh_light_server_lock();

    bt_mesh_server_stop_transition(&srv->transition);
    bt_mesh_server_update_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now);

    srv->state->target_lightness = lightness;
    if (srv->state->temperature_range_min &&
            srv->state->temperature_range_min != BLE_MESH_TEMPERATURE_UNKNOWN &&
            temperature < srv->state->temperature_range_min) {
        temperature = srv->state->temperature_range_min;
    } else if (srv->state->temperature_range_max &&
               srv->state->temperature_range_max != BLE_MESH_TEMPERATURE_UNKNOWN &&
               temperature > srv->state->temperature_range_max) {
        temperature = srv->state->temperature_range_max;
    }
    srv->state->target_temperature = temperature;
    srv->state->target_delta_uv = delta_uv;

    if (srv->state->target_lightness != srv->state->lightness ||
            srv->state->target_temperature != srv->state->temperature ||
            srv->state->target_delta_uv != srv->state->delta_uv) {
        light_ctl_tt_values(srv, trans_time, delay);
    } else {
        bt_mesh_light_server_state_change_t change = {
            .ctl_set.lightness = srv->state->lightness,
            .ctl_set.temperature = srv->state->temperature,
            .ctl_set.delta_uv = srv->state->delta_uv,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));

        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_CTL_SET) {
            send_light_ctl_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS);
        }
        send_light_ctl_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS);

        bt_mesh_light_server_unlock();
        return;
    }

    /* Copy the ctx of the received message */
    if (srv->transition.timer.work._reserved) {
        memcpy(srv->transition.timer.work._reserved, ctx, sizeof(struct bt_mesh_msg_ctx));
    }

    /* For Instantaneous Transition */
    if (srv->transition.counter == 0U) {
        srv->state->lightness = srv->state->target_lightness;
        srv->state->temperature = srv->state->target_temperature;
        srv->state->delta_uv = srv->state->target_delta_uv;
    }

    srv->transition.just_started = true;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_CTL_SET) {
        send_light_ctl_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS);
    }
    send_light_ctl_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS);

    bt_mesh_light_server_unlock();

    bt_mesh_server_start_transition(&srv->transition);
    return;
}

static void light_ctl_default_set(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf)
{
    struct bt_mesh_light_ctl_setup_srv *srv = model->user_data;
    uint16_t lightness = 0U, temperature = 0U;
    int16_t delta_uv = 0;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    lightness = net_buf_simple_pull_le16(buf);
    temperature = net_buf_simple_pull_le16(buf);
    delta_uv = (int16_t) net_buf_simple_pull_le16(buf);

    if (temperature < BLE_MESH_TEMPERATURE_MIN || temperature > BLE_MESH_TEMPERATURE_MAX) {
        BT_ERR("Invalid temperature 0x%04x", temperature);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_light_server_recv_set_msg_t set = {
            .ctl_default_set.lightness = lightness,
            .ctl_default_set.temperature = temperature,
            .ctl_default_set.delta_uv = delta_uv,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_SET_MSG, model, ctx, (const uint8_t *)&set, sizeof(set));
        return;
    }

    if (srv->state->temperature_range_min &&
            srv->state->temperature_range_min != BLE_MESH_TEMPERATURE_UNKNOWN &&
            temperature < srv->state->temperature_range_min) {
        temperature = srv->state->temperature_range_min;
    } else if (srv->state->temperature_range_max &&
               srv->state->temperature_range_max != BLE_MESH_TEMPERATURE_UNKNOWN &&
               temperature > srv->state->temperature_range_max) {
        temperature = srv->state->temperature_range_max;
    }

    srv->state->lightness_default = lightness;
    srv->state->temperature_default = temperature;
    srv->state->delta_uv_default = delta_uv;

    bt_mesh_light_server_state_change_t change = {
        .ctl_default_set.lightness = srv->state->lightness_default,
        .ctl_default_set.temperature = srv->state->temperature_default,
        .ctl_default_set.delta_uv = srv->state->delta_uv_default,
    };
    bt_mesh_lighting_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));

    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET) {
        send_light_ctl_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS);
    }
    send_light_ctl_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS);

    return;
}

static void light_ctl_temp_range_set(struct bt_mesh_model *model,
                                     struct bt_mesh_msg_ctx *ctx,
                                     struct net_buf_simple *buf)
{
    struct bt_mesh_light_ctl_setup_srv *srv = model->user_data;
    uint16_t min = 0U, max = 0U;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    min = net_buf_simple_pull_le16(buf);
    max = net_buf_simple_pull_le16(buf);

    /* This is as per 6.1.3.1 in Mesh Model Specification */
    if (min > max ||
            min < BLE_MESH_TEMPERATURE_MIN || (min != BLE_MESH_TEMPERATURE_UNKNOWN && min > BLE_MESH_TEMPERATURE_MAX) ||
            max < BLE_MESH_TEMPERATURE_MIN || (max != BLE_MESH_TEMPERATURE_UNKNOWN && max > BLE_MESH_TEMPERATURE_MAX)) {
        BT_ERR("Invalid parameter, range min 0x%04x, range max 0x%04x",
                min, max);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_light_server_recv_set_msg_t set = {
            .ctl_temp_range_set.range_min = min,
            .ctl_temp_range_set.range_max = max,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_SET_MSG, model, ctx, (const uint8_t *)&set, sizeof(set));
        return;
    }

    if (min == BLE_MESH_TEMPERATURE_UNKNOWN) {
        srv->state->status_code = BLE_MESH_CANNOT_SET_RANGE_MIN;
    } else if (max == BLE_MESH_TEMPERATURE_UNKNOWN ) {
        srv->state->status_code = BLE_MESH_CANNOT_SET_RANGE_MAX;
    } else {
        srv->state->status_code = BLE_MESH_RANGE_UPDATE_SUCCESS;
    }

    if (min != BLE_MESH_TEMPERATURE_UNKNOWN && srv->state->temperature_range_min != min) {
        srv->state->temperature_range_min = min;
    }

    if (max != BLE_MESH_TEMPERATURE_UNKNOWN && srv->state->temperature_range_max != max) {
        srv->state->temperature_range_max = max;
    }

    bt_mesh_light_server_state_change_t change = {
        .ctl_temp_range_set.range_min = srv->state->temperature_range_min,
        .ctl_temp_range_set.range_max = srv->state->temperature_range_max,
    };
    bt_mesh_lighting_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));

    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET) {
        send_light_ctl_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS);
    }
    send_light_ctl_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS);

    return;
}

static void light_ctl_temp_set(struct bt_mesh_model *model,
                               struct bt_mesh_msg_ctx *ctx,
                               struct net_buf_simple *buf)
{
    struct bt_mesh_light_ctl_temp_srv *srv = model->user_data;
    uint8_t tid = 0U, trans_time = 0U, delay = 0U;
    uint16_t temperature = 0U;
    int16_t delta_uv = 0;
    bool optional = false;
    int64_t now = 0;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    temperature = net_buf_simple_pull_le16(buf);
    delta_uv = (int16_t) net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    if (temperature < BLE_MESH_TEMPERATURE_MIN || temperature > BLE_MESH_TEMPERATURE_MAX) {
        BT_ERR("Invalid temperature 0x%04x", temperature);
        return;
    }

    if (bt_mesh_server_get_optional(model, ctx, buf, &trans_time, &delay, &optional)) {
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_light_server_recv_set_msg_t set = {
            .ctl_temp_set.op_en = optional,
            .ctl_temp_set.temperature = temperature,
            .ctl_temp_set.delta_uv = delta_uv,
            .ctl_temp_set.tid = tid,
            .ctl_temp_set.trans_time = trans_time,
            .ctl_temp_set.delay = delay,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_SET_MSG, model, ctx, (const uint8_t *)&set, sizeof(set));
        return;
    }

    if (bt_mesh_is_server_recv_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now)) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET) {
            send_light_ctl_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS);
        }
        send_light_ctl_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS);
        /* In this condition, no event will be callback to application layer */
        return;
    }

    bt_mesh_light_server_lock();

    bt_mesh_server_stop_transition(&srv->transition);
    bt_mesh_server_update_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now);

    if (srv->state->temperature_range_min &&
            srv->state->temperature_range_min != BLE_MESH_TEMPERATURE_UNKNOWN &&
            temperature < srv->state->temperature_range_min) {
        temperature = srv->state->temperature_range_min;
    } else if (srv->state->temperature_range_max &&
               srv->state->temperature_range_max != BLE_MESH_TEMPERATURE_UNKNOWN &&
               temperature > srv->state->temperature_range_max) {
        temperature = srv->state->temperature_range_max;
    }
    srv->state->target_temperature = temperature;
    srv->state->target_delta_uv = delta_uv;

    if (srv->state->target_temperature != srv->state->temperature ||
            srv->state->target_delta_uv != srv->state->delta_uv) {
        light_ctl_temp_tt_values(srv, trans_time, delay);
    } else {
        bt_mesh_light_server_state_change_t change = {
            .ctl_temp_set.temperature = srv->state->temperature,
            .ctl_temp_set.delta_uv = srv->state->delta_uv,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));

        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET) {
            send_light_ctl_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS);
        }
        send_light_ctl_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS);

        bt_mesh_light_server_unlock();
        return;
    }

    /* Copy the ctx of the received message */
    if (srv->transition.timer.work._reserved) {
        memcpy(srv->transition.timer.work._reserved, ctx, sizeof(struct bt_mesh_msg_ctx));
    }

    /* For Instantaneous Transition */
    if (srv->transition.counter == 0U) {
        srv->state->temperature = srv->state->target_temperature;
        srv->state->delta_uv = srv->state->target_delta_uv;
    }

    srv->transition.just_started = true;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET) {
        send_light_ctl_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS);
    }
    send_light_ctl_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS);

    bt_mesh_light_server_unlock();

    bt_mesh_server_start_transition(&srv->transition);
    return;
}

/* Light HSL Server/Hue Server/Saturation Server/Setup Server message handlers */

static void send_light_hsl_status(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  bool publish, uint16_t opcode)
{
    struct net_buf_simple *msg = NULL;
    uint8_t length = 2 + 9;

    if (ctx == NULL && publish == false) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    if (publish == false) {
        msg = bt_mesh_alloc_buf(length + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        if (msg == NULL) {
            BT_ERR("%s, Out of memory", __func__);
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
    case BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS:
    case BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_STATUS: {
        struct bt_mesh_light_hsl_srv *srv = model->user_data;
        if (opcode == BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS) {
            net_buf_simple_add_le16(msg, srv->state->lightness);
            net_buf_simple_add_le16(msg, srv->state->hue);
            net_buf_simple_add_le16(msg, srv->state->saturation);
            if (srv->transition.counter) {
                bt_mesh_server_calc_remain_time(&srv->transition);
                net_buf_simple_add_u8(msg, srv->transition.remain_time);
            }
        } else if (opcode == BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_STATUS) {
            net_buf_simple_add_le16(msg, srv->state->target_lightness);
            net_buf_simple_add_le16(msg, srv->state->target_hue);
            net_buf_simple_add_le16(msg, srv->state->target_saturation);
            if (srv->transition.counter) {
                bt_mesh_server_calc_remain_time(&srv->transition);
                net_buf_simple_add_u8(msg, srv->transition.remain_time);
            }
        }
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_STATUS:
        if (model->id == BLE_MESH_MODEL_ID_LIGHT_HSL_SRV) {
            struct bt_mesh_light_hsl_srv *srv = model->user_data;
            net_buf_simple_add_le16(msg, srv->state->lightness_default);
            net_buf_simple_add_le16(msg, srv->state->hue_default);
            net_buf_simple_add_le16(msg, srv->state->saturation_default);
        } else if (model->id == BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV) {
            struct bt_mesh_light_hsl_setup_srv *srv = model->user_data;
            net_buf_simple_add_le16(msg, srv->state->lightness_default);
            net_buf_simple_add_le16(msg, srv->state->hue_default);
            net_buf_simple_add_le16(msg, srv->state->saturation_default);
        }
        break;
    case BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_STATUS:
        if (model->id == BLE_MESH_MODEL_ID_LIGHT_HSL_SRV) {
            struct bt_mesh_light_hsl_srv *srv = model->user_data;
            net_buf_simple_add_u8(msg, srv->state->status_code);
            net_buf_simple_add_le16(msg, srv->state->hue_range_min);
            net_buf_simple_add_le16(msg, srv->state->hue_range_max);
            net_buf_simple_add_le16(msg, srv->state->saturation_range_min);
            net_buf_simple_add_le16(msg, srv->state->saturation_range_max);
        } else if (model->id == BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV) {
            struct bt_mesh_light_hsl_setup_srv *srv = model->user_data;
            net_buf_simple_add_u8(msg, srv->state->status_code);
            net_buf_simple_add_le16(msg, srv->state->hue_range_min);
            net_buf_simple_add_le16(msg, srv->state->hue_range_max);
            net_buf_simple_add_le16(msg, srv->state->saturation_range_min);
            net_buf_simple_add_le16(msg, srv->state->saturation_range_max);
        }
        break;
    case BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS: {
        struct bt_mesh_light_hsl_hue_srv *srv = model->user_data;
        net_buf_simple_add_le16(msg, srv->state->hue);
        if (srv->transition.counter) {
            bt_mesh_server_calc_remain_time(&srv->transition);
            net_buf_simple_add_le16(msg, srv->state->target_hue);
            net_buf_simple_add_u8(msg, srv->transition.remain_time);
        }
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS: {
        struct bt_mesh_light_hsl_sat_srv *srv = model->user_data;
        net_buf_simple_add_le16(msg, srv->state->saturation);
        if (srv->transition.counter) {
            bt_mesh_server_calc_remain_time(&srv->transition);
            net_buf_simple_add_le16(msg, srv->state->target_saturation);
            net_buf_simple_add_u8(msg, srv->transition.remain_time);
        }
        break;
    }
    default:
        BT_WARN("Unknown Light HSL status opcode 0x%04x", opcode);
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

static void light_hsl_get(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    struct bt_mesh_server_rsp_ctrl *rsp_ctrl = NULL;
    uint16_t opcode = 0U;

    if (model->user_data == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    switch (model->id) {
    case BLE_MESH_MODEL_ID_LIGHT_HSL_SRV: {
        struct bt_mesh_light_hsl_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light HSL Server state");
            return;
        }
        rsp_ctrl = &srv->rsp_ctrl;
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_HSL_HUE_SRV: {
        struct bt_mesh_light_hsl_hue_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light HSL Hue Server state");
            return;
        }
        rsp_ctrl = &srv->rsp_ctrl;
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_HSL_SAT_SRV: {
        struct bt_mesh_light_hsl_sat_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light HSL Saturation Server state");
            return;
        }
        rsp_ctrl = &srv->rsp_ctrl;
        break;
    }
    default:
        BT_ERR("Invalid Light HSL Server model 0x%04x", model->id);
        return;
    }

    /* Callback the received message to the application layer */
    if (rsp_ctrl->get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_GET_MSG, model, ctx, NULL, 0);
        return;
    }

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_LIGHT_HSL_GET:
        opcode = BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS;
        break;
    case BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_GET:
        opcode = BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_STATUS;
        break;
    case BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_GET:
        opcode = BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_STATUS;
        break;
    case BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_GET:
        opcode = BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_STATUS;
        break;
    case BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_GET:
        opcode = BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS;
        break;
    case BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_GET:
        opcode = BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS;
        break;
    default:
        BT_WARN("Unknown Light HSL Get opcode 0x%04x", ctx->recv_op);
        return;
    }

    send_light_hsl_status(model, ctx, false, opcode);
    return;
}

void light_hsl_publish(struct bt_mesh_model *model, uint16_t opcode)
{
    if (model->user_data == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    switch (model->id) {
    case BLE_MESH_MODEL_ID_LIGHT_HSL_SRV: {
        struct bt_mesh_light_hsl_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light HSL Server state");
            return;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_HSL_HUE_SRV: {
        struct bt_mesh_light_hsl_hue_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light HSL Hue Server state");
            return;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_HSL_SAT_SRV: {
        struct bt_mesh_light_hsl_sat_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light HSL Saturation Server state");
            return;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV: {
        struct bt_mesh_light_hsl_setup_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light HSL Setup Server state");
            return;
        }
        break;
    }
    default:
        BT_ERR("Invalid Light HSL Server model 0x%04x", model->id);
        return;
    }

    send_light_hsl_status(model, NULL, true, opcode);
    return;
}

static void light_hsl_set(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    struct bt_mesh_light_hsl_srv *srv = model->user_data;
    uint16_t lightness = 0U, hue = 0U, saturation = 0U;
    uint8_t tid = 0U, trans_time = 0U, delay = 0U;
    bool optional = false;
    int64_t now = 0;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    lightness = net_buf_simple_pull_le16(buf);
    hue = net_buf_simple_pull_le16(buf);
    saturation = net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    if (bt_mesh_server_get_optional(model, ctx, buf, &trans_time, &delay, &optional)) {
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_light_server_recv_set_msg_t set = {
            .hsl_set.op_en = optional,
            .hsl_set.lightness = lightness,
            .hsl_set.hue = hue,
            .hsl_set.saturation = saturation,
            .hsl_set.tid = tid,
            .hsl_set.trans_time = trans_time,
            .hsl_set.delay = delay,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_SET_MSG, model, ctx, (const uint8_t *)&set, sizeof(set));
        return;
    }

    if (bt_mesh_is_server_recv_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now)) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_HSL_SET) {
            send_light_hsl_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS);
        }
        send_light_hsl_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS);
        /* In this condition, no event will be callback to application layer */
        return;
    }

    bt_mesh_light_server_lock();

    bt_mesh_server_stop_transition(&srv->transition);
    bt_mesh_server_update_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now);

    srv->state->target_lightness = lightness;
    if (srv->state->hue_range_min && hue < srv->state->hue_range_min) {
        hue = srv->state->hue_range_min;
    } else if (srv->state->hue_range_max && hue > srv->state->hue_range_max) {
        hue = srv->state->hue_range_max;
    }
    srv->state->target_hue = hue;
    if (srv->state->saturation_range_min && saturation < srv->state->saturation_range_min) {
        saturation = srv->state->saturation_range_min;
    } else if (srv->state->saturation_range_max && saturation > srv->state->saturation_range_max) {
        saturation = srv->state->saturation_range_max;
    }
    srv->state->target_saturation = saturation;

    /**
     * If the target state is equal to the current state, the transition shall not
     * be started and is considered complete.
     */
    if (srv->state->target_lightness != srv->state->lightness ||
            srv->state->target_hue != srv->state->hue ||
            srv->state->target_saturation != srv->state->saturation) {
        light_hsl_tt_values(srv, trans_time, delay);
    } else {
        bt_mesh_light_server_state_change_t change = {
            .hsl_set.lightness = srv->state->lightness,
            .hsl_set.hue = srv->state->hue,
            .hsl_set.saturation = srv->state->saturation,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));

        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_HSL_SET) {
            send_light_hsl_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS);
        }
        send_light_hsl_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS);

        bt_mesh_light_server_unlock();
        return;
    }

    /* Copy the ctx of the received message */
    if (srv->transition.timer.work._reserved) {
        memcpy(srv->transition.timer.work._reserved, ctx, sizeof(struct bt_mesh_msg_ctx));
    }

    /* For Instantaneous Transition */
    if (srv->transition.counter == 0U) {
        srv->state->lightness = srv->state->target_lightness;
        srv->state->hue = srv->state->target_hue;
        srv->state->saturation = srv->state->target_saturation;
    }

    srv->transition.just_started = true;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_HSL_SET) {
        send_light_hsl_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS);
    }
    send_light_hsl_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS);

    bt_mesh_light_server_unlock();

    bt_mesh_server_start_transition(&srv->transition);
    return;
}

static void light_hsl_default_set(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf)
{
    struct bt_mesh_light_hsl_setup_srv *srv = model->user_data;
    uint16_t lightness = 0U, hue = 0U, saturation = 0U;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    lightness = net_buf_simple_pull_le16(buf);
    hue = net_buf_simple_pull_le16(buf);
    saturation = net_buf_simple_pull_le16(buf);

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_light_server_recv_set_msg_t set = {
            .hsl_default_set.lightness = lightness,
            .hsl_default_set.hue = hue,
            .hsl_default_set.saturation = saturation,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_SET_MSG, model, ctx, (const uint8_t *)&set, sizeof(set));
        return;
    }

    if (srv->state->hue_range_min && hue < srv->state->hue_range_min) {
        hue = srv->state->hue_range_min;
    } else if (srv->state->hue_range_max && hue > srv->state->hue_range_max) {
        hue = srv->state->hue_range_max;
    }

    if (srv->state->saturation_range_min && saturation < srv->state->saturation_range_min) {
        saturation = srv->state->saturation_range_min;
    } else if (srv->state->saturation_range_max && saturation > srv->state->saturation_range_max) {
        saturation = srv->state->saturation_range_max;
    }

    srv->state->lightness_default = lightness;
    srv->state->hue_default = hue;
    srv->state->saturation_default = saturation;

    bt_mesh_light_server_state_change_t change = {
        .hsl_default_set.lightness = srv->state->lightness_default,
        .hsl_default_set.hue = srv->state->hue_default,
        .hsl_default_set.saturation = srv->state->saturation_default,
    };
    bt_mesh_lighting_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));

    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET) {
        send_light_hsl_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_STATUS);
    }
    send_light_hsl_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_STATUS);

    return;
}

static void light_hsl_range_set(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf)
{
    struct bt_mesh_light_hsl_setup_srv *srv = model->user_data;
    uint16_t hue_min = 0U, hue_max = 0U, saturation_min = 0U, saturation_max = 0U;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    hue_min = net_buf_simple_pull_le16(buf);
    hue_max = net_buf_simple_pull_le16(buf);
    saturation_min = net_buf_simple_pull_le16(buf);
    saturation_max = net_buf_simple_pull_le16(buf);

    if (hue_min > hue_max) {
        BT_ERR("Invalid parameter, hue min 0x%04x, hue max 0x%04x",
                hue_min, hue_max);
        return;
    }

    if (saturation_min > saturation_max) {
        BT_ERR("Invalid parameter, saturation min 0x%04x, saturation max 0x%04x",
                saturation_min, saturation_max);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_light_server_recv_set_msg_t set = {
            .hsl_range_set.hue_range_min = hue_min,
            .hsl_range_set.hue_range_max = hue_max,
            .hsl_range_set.sat_range_min = saturation_min,
            .hsl_range_set.sat_range_max = saturation_max,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_SET_MSG, model, ctx, (const uint8_t *)&set, sizeof(set));
        return;
    }

    srv->state->status_code = BLE_MESH_RANGE_UPDATE_SUCCESS;
    srv->state->hue_range_min = hue_min;
    srv->state->hue_range_max = hue_max;
    srv->state->saturation_range_min = saturation_min;
    srv->state->saturation_range_max = saturation_max;

    bt_mesh_light_server_state_change_t change = {
        .hsl_range_set.hue_range_min = srv->state->hue_range_min,
        .hsl_range_set.hue_range_max = srv->state->hue_range_max,
        .hsl_range_set.sat_range_min = srv->state->saturation_range_min,
        .hsl_range_set.sat_range_max = srv->state->saturation_range_max,
    };
    bt_mesh_lighting_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));

    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET) {
        send_light_hsl_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_STATUS);
    }
    send_light_hsl_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_STATUS);

    return;
}

static void light_hsl_hue_set(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    struct bt_mesh_light_hsl_hue_srv *srv = model->user_data;
    uint8_t tid = 0U, trans_time = 0U, delay = 0U;
    bool optional = false;
    uint16_t hue = 0U;
    int64_t now = 0;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    hue = net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    if (bt_mesh_server_get_optional(model, ctx, buf, &trans_time, &delay, &optional)) {
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_light_server_recv_set_msg_t set = {
            .hsl_hue_set.op_en = optional,
            .hsl_hue_set.hue = hue,
            .hsl_hue_set.tid = tid,
            .hsl_hue_set.trans_time = trans_time,
            .hsl_hue_set.delay = delay,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_SET_MSG, model, ctx, (const uint8_t *)&set, sizeof(set));
        return;
    }

    if (bt_mesh_is_server_recv_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now)) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET) {
            send_light_hsl_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS);
        }
        send_light_hsl_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS);
        /* In this condition, no event will be callback to application layer */
        return;
    }

    bt_mesh_light_server_lock();

    bt_mesh_server_stop_transition(&srv->transition);
    bt_mesh_server_update_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now);

    if (srv->state->hue_range_min && hue < srv->state->hue_range_min) {
        hue = srv->state->hue_range_min;
    } else if (srv->state->hue_range_max && hue > srv->state->hue_range_max) {
        hue = srv->state->hue_range_max;
    }
    srv->state->target_hue = hue;

    /**
     * If the target state is equal to the current state, the transition shall not
     * be started and is considered complete.
     */
    if (srv->state->target_hue != srv->state->hue) {
        light_hsl_hue_tt_values(srv, trans_time, delay);
    } else {
        bt_mesh_light_server_state_change_t change = {
            .hsl_hue_set.hue = srv->state->hue,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));

        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET) {
            send_light_hsl_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS);
        }
        send_light_hsl_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS);

        bt_mesh_light_server_unlock();
        return;
    }

    /* Copy the ctx of the received message */
    if (srv->transition.timer.work._reserved) {
        memcpy(srv->transition.timer.work._reserved, ctx, sizeof(struct bt_mesh_msg_ctx));
    }

    /* For Instantaneous Transition */
    if (srv->transition.counter == 0U) {
        srv->state->hue = srv->state->target_hue;
    }

    srv->transition.just_started = true;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET) {
        send_light_hsl_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS);
    }
    send_light_hsl_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS);

    bt_mesh_light_server_unlock();

    bt_mesh_server_start_transition(&srv->transition);
    return;
}

static void light_hsl_sat_set(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    struct bt_mesh_light_hsl_sat_srv *srv = model->user_data;
    uint8_t tid = 0U, trans_time = 0U, delay = 0U;
    uint16_t saturation = 0U;
    bool optional = false;
    int64_t now = 0;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    saturation = net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    if (bt_mesh_server_get_optional(model, ctx, buf, &trans_time, &delay, &optional)) {
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_light_server_recv_set_msg_t set = {
            .hsl_saturation_set.op_en = optional,
            .hsl_saturation_set.saturation = saturation,
            .hsl_saturation_set.tid = tid,
            .hsl_saturation_set.trans_time = trans_time,
            .hsl_saturation_set.delay = delay,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_SET_MSG, model, ctx, (const uint8_t *)&set, sizeof(set));
        return;
    }

    if (bt_mesh_is_server_recv_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now)) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET) {
            send_light_hsl_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS);
        }
        send_light_hsl_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS);
        /* In this condition, no event will be callback to application layer */
        return;
    }

    bt_mesh_light_server_lock();

    bt_mesh_server_stop_transition(&srv->transition);
    bt_mesh_server_update_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now);

    if (srv->state->saturation_range_min && saturation < srv->state->saturation_range_min) {
        saturation = srv->state->saturation_range_min;
    } else if (srv->state->saturation_range_max && saturation > srv->state->saturation_range_max) {
        saturation = srv->state->saturation_range_max;
    }
    srv->state->target_saturation = saturation;

    /**
     * If the target state is equal to the current state, the transition shall not
     * be started and is considered complete.
     */
    if (srv->state->target_saturation != srv->state->saturation) {
        light_hsl_sat_tt_values(srv, trans_time, delay);
    } else {
        bt_mesh_light_server_state_change_t change = {
            .hsl_saturation_set.saturation = srv->state->saturation,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));

        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET) {
            send_light_hsl_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS);
        }
        send_light_hsl_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS);

        bt_mesh_light_server_unlock();
        return;
    }

    /* Copy the ctx of the received message */
    if (srv->transition.timer.work._reserved) {
        memcpy(srv->transition.timer.work._reserved, ctx, sizeof(struct bt_mesh_msg_ctx));
    }

    /* For Instantaneous Transition */
    if (srv->transition.counter == 0U) {
        srv->state->saturation = srv->state->target_saturation;
    }

    srv->transition.just_started = true;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET) {
        send_light_hsl_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS);
    }
    send_light_hsl_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS);

    bt_mesh_light_server_unlock();

    bt_mesh_server_start_transition(&srv->transition);
    return;
}

/* Light xyL Server/Setup Server message handlers */

static void send_light_xyl_status(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  bool publish, uint16_t opcode)
{
    struct net_buf_simple *msg = NULL;
    uint8_t length = 2 + 9;

    if (ctx == NULL && publish == false) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    if (publish == false) {
        msg = bt_mesh_alloc_buf(length + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        if (msg == NULL) {
            BT_ERR("%s, Out of memory", __func__);
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
    case BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS:
    case BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_STATUS: {
        struct bt_mesh_light_xyl_srv *srv = model->user_data;
        if (opcode == BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS) {
            net_buf_simple_add_le16(msg, srv->state->lightness);
            net_buf_simple_add_le16(msg, srv->state->x);
            net_buf_simple_add_le16(msg, srv->state->y);
            if (srv->transition.counter) {
                bt_mesh_server_calc_remain_time(&srv->transition);
                net_buf_simple_add_u8(msg, srv->transition.remain_time);
            }
        } else if (opcode == BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_STATUS) {
            net_buf_simple_add_le16(msg, srv->state->target_lightness);
            net_buf_simple_add_le16(msg, srv->state->target_x);
            net_buf_simple_add_le16(msg, srv->state->target_y);
            if (srv->transition.counter) {
                bt_mesh_server_calc_remain_time(&srv->transition);
                net_buf_simple_add_u8(msg, srv->transition.remain_time);
            }
        }
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_STATUS:
        if (model->id == BLE_MESH_MODEL_ID_LIGHT_XYL_SRV) {
            struct bt_mesh_light_xyl_srv *srv = model->user_data;
            net_buf_simple_add_le16(msg, srv->state->lightness_default);
            net_buf_simple_add_le16(msg, srv->state->x_default);
            net_buf_simple_add_le16(msg, srv->state->y_default);
        } else if (model->id == BLE_MESH_MODEL_ID_LIGHT_XYL_SETUP_SRV) {
            struct bt_mesh_light_xyl_setup_srv *srv = model->user_data;
            net_buf_simple_add_le16(msg, srv->state->lightness_default);
            net_buf_simple_add_le16(msg, srv->state->x_default);
            net_buf_simple_add_le16(msg, srv->state->y_default);
        }
        break;
    case BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_STATUS:
        if (model->id == BLE_MESH_MODEL_ID_LIGHT_XYL_SRV) {
            struct bt_mesh_light_xyl_srv *srv = model->user_data;
            net_buf_simple_add_u8(msg, srv->state->status_code);
            net_buf_simple_add_le16(msg, srv->state->x_range_min);
            net_buf_simple_add_le16(msg, srv->state->x_range_max);
            net_buf_simple_add_le16(msg, srv->state->y_range_min);
            net_buf_simple_add_le16(msg, srv->state->y_range_max);
        } else if (model->id == BLE_MESH_MODEL_ID_LIGHT_XYL_SETUP_SRV) {
            struct bt_mesh_light_xyl_setup_srv *srv = model->user_data;
            net_buf_simple_add_u8(msg, srv->state->status_code);
            net_buf_simple_add_le16(msg, srv->state->x_range_min);
            net_buf_simple_add_le16(msg, srv->state->x_range_max);
            net_buf_simple_add_le16(msg, srv->state->y_range_min);
            net_buf_simple_add_le16(msg, srv->state->y_range_max);
        }
        break;
    default:
        BT_WARN("Unknown Light xyL status opcode 0x%04x", opcode);
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

static void light_xyl_get(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    struct bt_mesh_light_xyl_srv *srv = model->user_data;
    uint16_t opcode = 0U;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_GET_MSG, model, ctx, NULL, 0);
        return;
    }

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_LIGHT_XYL_GET:
        opcode = BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS;
        break;
    case BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_GET:
        opcode = BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_STATUS;
        break;
    case BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_GET:
        opcode = BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_STATUS;
        break;
    case BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_GET:
        opcode = BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_STATUS;
        break;
    default:
        BT_WARN("Unknown Light xyL Get opcode 0x%04x", ctx->recv_op);
        return;
    }

    send_light_xyl_status(model, ctx, false, opcode);
    return;
}

void light_xyl_publish(struct bt_mesh_model *model, uint16_t opcode)
{
    if (model->user_data == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    switch (model->id) {
    case BLE_MESH_MODEL_ID_LIGHT_XYL_SRV: {
        struct bt_mesh_light_xyl_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light xyL Server state");
            return;
        }
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_XYL_SETUP_SRV: {
        struct bt_mesh_light_xyl_setup_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light xyL Setup Server state");
            return;
        }
        break;
    }
    default:
        BT_ERR("Invalid Light xyL Server model 0x%04x", model->id);
        return;
    }

    send_light_xyl_status(model, NULL, true, opcode);
    return;
}

static void light_xyl_set(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    struct bt_mesh_light_xyl_srv *srv = model->user_data;
    uint8_t tid = 0U, trans_time = 0U, delay = 0U;
    uint16_t lightness = 0U, x = 0U, y = 0U;
    bool optional = false;
    int64_t now = 0;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    lightness = net_buf_simple_pull_le16(buf);
    x = net_buf_simple_pull_le16(buf);
    y = net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    if (bt_mesh_server_get_optional(model, ctx, buf, &trans_time, &delay, &optional)) {
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_light_server_recv_set_msg_t set = {
            .xyl_set.op_en = optional,
            .xyl_set.lightness = lightness,
            .xyl_set.x = x,
            .xyl_set.y = y,
            .xyl_set.tid = tid,
            .xyl_set.trans_time = trans_time,
            .xyl_set.delay = delay,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_SET_MSG, model, ctx, (const uint8_t *)&set, sizeof(set));
        return;
    }

    if (bt_mesh_is_server_recv_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now)) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_XYL_SET) {
            send_light_xyl_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS);
        }
        send_light_xyl_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS);
        /* In this condition, no event will be callback to application layer */
        return;
    }

    bt_mesh_light_server_lock();

    bt_mesh_server_stop_transition(&srv->transition);
    bt_mesh_server_update_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now);

    srv->state->target_lightness = lightness;
    if (srv->state->x_range_min && x < srv->state->x_range_min) {
        x = srv->state->x_range_min;
    } else if (srv->state->x_range_max && x > srv->state->x_range_max) {
        x = srv->state->x_range_max;
    }
    srv->state->target_x = x;
    if (srv->state->y_range_min && y < srv->state->y_range_min) {
        y = srv->state->y_range_min;
    } else if (srv->state->y_range_max && y > srv->state->y_range_max) {
        y = srv->state->y_range_max;
    }
    srv->state->target_y = y;

    /**
     * If the target state is equal to the current state, the transition shall not
     * be started and is considered complete.
     */
    if (srv->state->target_lightness != srv->state->lightness ||
            srv->state->target_x != srv->state->x ||
            srv->state->target_y != srv->state->y) {
        light_xyl_tt_values(srv, trans_time, delay);
    } else {
        bt_mesh_light_server_state_change_t change = {
            .xyl_set.lightness = srv->state->lightness,
            .xyl_set.x = srv->state->x,
            .xyl_set.y = srv->state->y,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));

        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_XYL_SET) {
            send_light_xyl_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS);
        }
        send_light_xyl_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS);

        bt_mesh_light_server_unlock();
        return;
    }

    /* Copy the ctx of the received message */
    if (srv->transition.timer.work._reserved) {
        memcpy(srv->transition.timer.work._reserved, ctx, sizeof(struct bt_mesh_msg_ctx));
    }

    /* For Instantaneous Transition */
    if (srv->transition.counter == 0U) {
        srv->state->lightness = srv->state->target_lightness;
        srv->state->x = srv->state->target_x;
        srv->state->y = srv->state->target_y;
    }

    srv->transition.just_started = true;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_XYL_SET) {
        send_light_xyl_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS);
    }
    send_light_xyl_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS);

    bt_mesh_light_server_unlock();

    bt_mesh_server_start_transition(&srv->transition);
    return;
}

static void light_xyl_default_set(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf)
{
    struct bt_mesh_light_xyl_setup_srv *srv = model->user_data;
    uint16_t lightness = 0U, x = 0U, y = 0U;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    lightness = net_buf_simple_pull_le16(buf);
    x = net_buf_simple_pull_le16(buf);
    y = net_buf_simple_pull_le16(buf);

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_light_server_recv_set_msg_t set = {
            .xyl_default_set.lightness = lightness,
            .xyl_default_set.x = x,
            .xyl_default_set.y = y,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_SET_MSG, model, ctx, (const uint8_t *)&set, sizeof(set));
        return;
    }

    if (srv->state->x_range_min && x < srv->state->x_range_min) {
        x = srv->state->x_range_min;
    } else if (srv->state->x_range_max && x > srv->state->x_range_max) {
        x = srv->state->x_range_max;
    }

    if (srv->state->y_range_min && y < srv->state->y_range_min) {
        y = srv->state->y_range_min;
    } else if (srv->state->y_range_max && y > srv->state->y_range_max) {
        y = srv->state->y_range_max;
    }

    srv->state->lightness_default = lightness;
    srv->state->x_default = x;
    srv->state->y_default = y;

    bt_mesh_light_server_state_change_t change = {
        .xyl_default_set.lightness = srv->state->lightness_default,
        .xyl_default_set.x = srv->state->x_default,
        .xyl_default_set.y = srv->state->y_default,
    };
    bt_mesh_lighting_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));

    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET) {
        send_light_xyl_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_STATUS);
    }
    send_light_xyl_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_STATUS);

    return;
}

static void light_xyl_range_set(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf)
{
    struct bt_mesh_light_xyl_setup_srv *srv = model->user_data;
    uint16_t x_min = 0U, x_max = 0U, y_min = 0U, y_max = 0U;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    x_min = net_buf_simple_pull_le16(buf);
    x_max = net_buf_simple_pull_le16(buf);
    y_min = net_buf_simple_pull_le16(buf);
    y_max = net_buf_simple_pull_le16(buf);

    if (x_min > x_max) {
        BT_ERR("Invalid parameter, x min 0x%04x, x max 0x%04x",
                x_min, x_max);
        return;
    }

    if (y_min > y_max) {
        BT_ERR("Invalid parameter, y min 0x%04x, y max 0x%04x",
                y_min, y_max);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_light_server_recv_set_msg_t set = {
            .xyl_range_set.x_range_min = x_min,
            .xyl_range_set.x_range_max = x_max,
            .xyl_range_set.y_range_min = y_min,
            .xyl_range_set.y_range_max = y_max,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_SET_MSG, model, ctx, (const uint8_t *)&set, sizeof(set));
        return;
    }

    srv->state->status_code = BLE_MESH_RANGE_UPDATE_SUCCESS;
    srv->state->x_range_min = x_min;
    srv->state->x_range_max = x_max;
    srv->state->y_range_min = y_min;
    srv->state->y_range_max = y_max;

    bt_mesh_light_server_state_change_t change = {
        .xyl_range_set.x_range_min = srv->state->x_range_min,
        .xyl_range_set.x_range_max = srv->state->x_range_max,
        .xyl_range_set.y_range_min = srv->state->y_range_min,
        .xyl_range_set.y_range_max = srv->state->y_range_max,
    };
    bt_mesh_lighting_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));

    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET) {
        send_light_xyl_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_STATUS);
    }
    send_light_xyl_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_STATUS);

    return;
}

/* Light LC Server/Setup Server message handlers */
static void send_light_lc_status(struct bt_mesh_model *model,
                                 struct bt_mesh_msg_ctx *ctx,
                                 bool publish, uint16_t opcode)
{
    struct bt_mesh_light_lc_srv *srv = model->user_data;
    struct net_buf_simple *msg = NULL;
    uint8_t length = 2 + 3;

    if (ctx == NULL && publish == false) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    if (publish == false) {
        msg = bt_mesh_alloc_buf(length + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        if (msg == NULL) {
            BT_ERR("%s, Out of memory", __func__);
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
    case BLE_MESH_MODEL_OP_LIGHT_LC_MODE_STATUS:
        net_buf_simple_add_u8(msg, srv->lc->state.mode);
        break;
    case BLE_MESH_MODEL_OP_LIGHT_LC_OM_STATUS:
        net_buf_simple_add_u8(msg, srv->lc->state.occupancy_mode);
        break;
    case BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_STATUS:
        net_buf_simple_add_u8(msg, srv->lc->state.light_onoff);
        if (srv->transition.counter) {
            bt_mesh_server_calc_remain_time(&srv->transition);
            net_buf_simple_add_u8(msg, srv->lc->state.target_light_onoff);
            net_buf_simple_add_u8(msg, srv->transition.remain_time);
        }
        break;
    default:
        BT_WARN("Unknown Light LC status opcode 0x%04x", opcode);
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

static void light_lc_get(struct bt_mesh_model *model,
                         struct bt_mesh_msg_ctx *ctx,
                         struct net_buf_simple *buf)
{
    struct bt_mesh_light_lc_srv *srv = model->user_data;
    uint16_t opcode = 0U;

    if (srv == NULL || srv->lc == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_GET_MSG, model, ctx, NULL, 0);
        return;
    }

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_LIGHT_LC_MODE_GET:
        opcode = BLE_MESH_MODEL_OP_LIGHT_LC_MODE_STATUS;
        break;
    case BLE_MESH_MODEL_OP_LIGHT_LC_OM_GET:
        opcode = BLE_MESH_MODEL_OP_LIGHT_LC_OM_STATUS;
        break;
    case BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_GET:
        opcode = BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_STATUS;
        break;
    default:
        BT_WARN("Unknown Light LC Get opcode 0x%04x", ctx->recv_op);
        return;
    }

    send_light_lc_status(model, ctx, false, opcode);
    return;
}

void light_lc_publish(struct bt_mesh_model *model, uint16_t opcode)
{
    struct bt_mesh_light_lc_srv *srv = model->user_data;

    if (srv == NULL || srv->lc == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    send_light_lc_status(model, NULL, true, opcode);
    return;
}

static void light_lc_mode_set(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    struct bt_mesh_light_lc_srv *srv = model->user_data;
    uint8_t mode = 0U;

    if (srv == NULL || srv->lc == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    mode = net_buf_simple_pull_u8(buf);
    if (mode > BLE_MESH_STATE_ON) {
        BT_ERR("Invalid LC Mode 0x%02x", mode);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_light_server_recv_set_msg_t set = {
            .lc_mode_set.mode = mode,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_SET_MSG, model, ctx, (const uint8_t *)&set, sizeof(set));
        return;
    }

    srv->lc->state.mode = mode;

    bt_mesh_light_server_state_change_t change = {
        .lc_mode_set.mode = srv->lc->state.mode,
    };
    bt_mesh_lighting_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));

    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET) {
        send_light_lc_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_LC_MODE_STATUS);
    }
    send_light_lc_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_LC_MODE_STATUS);

    return;
}

static void light_lc_om_set(struct bt_mesh_model *model,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *buf)
{
    struct bt_mesh_light_lc_srv *srv = model->user_data;
    uint8_t om = 0U;

    if (srv == NULL || srv->lc == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    om = net_buf_simple_pull_u8(buf);
    if (om > BLE_MESH_STATE_ON) {
        BT_ERR("Invalid LC Occupancy Mode 0x%02x", om);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_light_server_recv_set_msg_t set = {
            .lc_om_set.mode = om,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_SET_MSG, model, ctx, (const uint8_t *)&set, sizeof(set));
        return;
    }

    srv->lc->state.occupancy_mode = om;

    bt_mesh_light_server_state_change_t change = {
        .lc_om_set.mode = srv->lc->state.occupancy_mode,
    };
    bt_mesh_lighting_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));

    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET) {
        send_light_lc_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_LC_OM_STATUS);
    }
    send_light_lc_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_LC_OM_STATUS);

    return;
}

static void light_lc_light_onoff_set(struct bt_mesh_model *model,
                                     struct bt_mesh_msg_ctx *ctx,
                                     struct net_buf_simple *buf)
{
    struct bt_mesh_light_lc_srv *srv = model->user_data;
    uint8_t tid = 0U, trans_time = 0U, delay = 0U;
    bool optional = false;
    uint8_t onoff = 0U;
    int64_t now = 0;

    if (srv == NULL || srv->lc == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    onoff = net_buf_simple_pull_u8(buf);
    tid = net_buf_simple_pull_u8(buf);

    if (bt_mesh_server_get_optional(model, ctx, buf, &trans_time, &delay, &optional)) {
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_light_server_recv_set_msg_t set = {
            .lc_light_onoff_set.op_en = optional,
            .lc_light_onoff_set.light_onoff = onoff,
            .lc_light_onoff_set.tid = tid,
            .lc_light_onoff_set.trans_time = trans_time,
            .lc_light_onoff_set.delay = delay,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_SET_MSG, model, ctx, (const uint8_t *)&set, sizeof(set));
        return;
    }

    if (bt_mesh_is_server_recv_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now)) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET) {
            send_light_lc_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_STATUS);
        }
        send_light_lc_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_STATUS);
        /* In this condition, no event will be callback to application layer */
        return;
    }

    bt_mesh_light_server_lock();

    bt_mesh_server_stop_transition(&srv->transition);
    bt_mesh_server_update_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now);

    srv->lc->state.target_light_onoff = onoff;

    if (srv->lc->state.target_light_onoff != srv->lc->state.light_onoff) {
        light_lc_tt_values(srv, trans_time, delay);
    } else {
        bt_mesh_light_server_state_change_t change = {
            .lc_light_onoff_set.onoff = srv->lc->state.light_onoff,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));

        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET) {
            send_light_lc_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_STATUS);
        }
        send_light_lc_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_STATUS);

        bt_mesh_light_server_unlock();
        return;
    }

    /* Copy the ctx of the received message */
    if (srv->transition.timer.work._reserved) {
        memcpy(srv->transition.timer.work._reserved, ctx, sizeof(struct bt_mesh_msg_ctx));
    }

    /* For Instantaneous Transition */
    if (srv->transition.counter == 0U) {
        srv->lc->state.light_onoff = srv->lc->state.target_light_onoff;
    }

    srv->transition.just_started = true;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET) {
        send_light_lc_status(model, ctx, false, BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_STATUS);
    }
    send_light_lc_status(model, NULL, true, BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_STATUS);

    bt_mesh_light_server_unlock();

    bt_mesh_server_start_transition(&srv->transition);
    return;
}

static void light_lc_sensor_status(struct bt_mesh_model *model,
                                   struct bt_mesh_msg_ctx *ctx,
                                   struct net_buf_simple *buf)
{
    /**
     * When a Light LC Server receives a Sensor Status message, and if the message
     * Raw field contains a Raw Value for the Motion Sensed Property, and the value
     * is greater than 0, or a Raw Value for the People Count Property, and the
     * value is greater than 0, or a Raw Value for the Presence Detected Property,
     * and the value is greater than 0, then it shall set the Light LC Occupancy
     * state to 0b1.
     * If the message Raw field contains a Raw Value for the Time Since Motion Sensed
     * device property, which represents a value less than or equal to the value of
     * the Light LC Occupancy Delay state, it shall delay setting the Light LC Occupancy
     * state to 0b1 by the difference between the value of the Light LC Occupancy Delay
     * state and the received Time Since Motion value.
     * When a Light LC Server receives a Sensor Status message, and if the message Raw
     * field contains a Raw Value for the Present Ambient Light Level device property,
     * it shall set the Light LC Ambient LuxLevel state to the Represented Value of the
     * received Present Ambient Light Level.
     *
     * Motion Sensed: 1 octet, 0x0042
     * People Count: 2 octets, 0x004C
     * Presence Detected: 1 octet, 0x004D
     *
     * Time Since Motion Sensed: 2 octets, 0x0068
     *
     * Present Ambient Light Level: 4 octets, 0x004E
     */
    struct bt_mesh_light_lc_srv *srv = model->user_data;
    bt_mesh_light_server_state_change_t change = {0};
    uint16_t mpid = 0U, prop_id = 0U;
    uint8_t length = 0U;

    if (srv == NULL || srv->lc == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    if (srv->rsp_ctrl.status_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_light_server_recv_status_msg_t status = {
            .sensor_status.data = buf,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_STATUS_MSG, model, ctx, (const uint8_t *)&status, sizeof(status));
        return;
    }

    mpid = net_buf_simple_pull_le16(buf);
    if (mpid & BIT(0)) {
        length = (uint8_t)((mpid & 0xff) >> 1);
        uint8_t msb = net_buf_simple_pull_u8(buf);
        prop_id = (uint16_t)(msb << 8) | (uint16_t)(mpid >> 8);
    } else {
        length = (uint8_t)((mpid & 0x1f) >> 1);
        prop_id = (uint16_t)(mpid >> 5);
    }

    change.sensor_status.property_id = prop_id;

    switch (prop_id) {
    case BLE_MESH_MOTION_SENSED: {
        if (length != BLE_MESH_MOTION_SENSED_LEN || length != buf->len) {
            BT_WARN("Invalid Motion Sensed Property length %d", length);
            return;
        }
        uint8_t val = net_buf_simple_pull_u8(buf);
        if (val > 0) {
            srv->lc->state.occupancy = BLE_MESH_STATE_ON;

            change.sensor_status.state.occupancy = srv->lc->state.occupancy;
            bt_mesh_lighting_server_cb_evt_to_btc(
                BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));
        }
        break;
    }
    case BLE_MESH_PEOPLE_COUNT: {
        if (length != BLE_MESH_PEOPLE_COUNT_LEN || length != buf->len) {
            BT_WARN("Invalid Motion Sensed Property length %d", length);
            return;
        }
        uint16_t val = net_buf_simple_pull_le16(buf);
        if (val > 0) {
            srv->lc->state.occupancy = BLE_MESH_STATE_ON;

            change.sensor_status.state.occupancy = srv->lc->state.occupancy;
            bt_mesh_lighting_server_cb_evt_to_btc(
                BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));
        }
        break;
    }
    case BLE_MESH_PRESENCE_DETECTED: {
        if (length != BLE_MESH_PRESENCE_DETECTED_LEN || length != buf->len) {
            BT_WARN("Invalid Motion Sensed Property length %d", length);
            return;
        }
        uint8_t val = net_buf_simple_pull_u8(buf);
        if (val > 0) {
            srv->lc->state.occupancy = BLE_MESH_STATE_ON;

            change.sensor_status.state.occupancy = srv->lc->state.occupancy;
            bt_mesh_lighting_server_cb_evt_to_btc(
                BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));
        }
        break;
    }
    case BLE_MESH_TIME_SINCE_MOTION_SENSED: {
        if (length != BLE_MESH_TIME_SINCE_MOTION_SENSED_LEN || length != buf->len) {
            BT_WARN("Invalid Motion Sensed Property length %d", length);
            return;
        }
        uint16_t val = net_buf_simple_pull_le16(buf);
        if (val <= srv->lc->prop_state.time_occupancy_delay) {
            srv->lc->prop_state.set_occupancy_to_1_delay =
                srv->lc->prop_state.time_occupancy_delay - val;

            change.sensor_status.state.set_occupancy_to_1_delay = srv->lc->prop_state.set_occupancy_to_1_delay;
            bt_mesh_lighting_server_cb_evt_to_btc(
                BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));
        }
        break;
    }
    case BLE_MESH_PRESENT_AMBIENT_LIGHT_LEVEL: {
        /**
         * Present Ambient Light Level device property is 4 octets, but ambient
         * luxlevel length is 3 octets, and other devices may send Sensor Status
         * which only contains 3 octets just for Light LC Server.
         * Here we just check if the length is larger than 3.
         */
        if (buf->len < 3) {
            BT_WARN("Invalid Motion Sensed Property length %d", buf->len);
            return;
        }
        uint16_t lsb = net_buf_simple_pull_le16(buf);
        uint8_t msb = net_buf_simple_pull_u8(buf);
        srv->lc->state.ambient_luxlevel = (msb << 16) | lsb;

        change.sensor_status.state.ambient_luxlevel = srv->lc->state.ambient_luxlevel;
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));
        break;
    }
    default:
        break;
    }
}

static uint8_t *get_light_lc_prop_val(struct bt_mesh_model *model, uint16_t prop_id)
{
    struct bt_mesh_light_lc_setup_srv *srv = model->user_data;
    uint8_t *val = NULL;

    switch (prop_id) {
    case BLE_MESH_LIGHT_CONTROL_TIME_OCCUPANCY_DELAY:
        val = (uint8_t *)&srv->lc->prop_state.time_occupancy_delay;
        break;
    case BLE_MESH_LIGHT_CONTROL_TIME_FADE_ON:
        val = (uint8_t *)&srv->lc->prop_state.time_fade_on;
        break;
    case BLE_MESH_LIGHT_CONTROL_TIME_RUN_ON:
        val = (uint8_t *)&srv->lc->prop_state.time_run_on;
        break;
    case BLE_MESH_LIGHT_CONTROL_TIME_FADE:
        val = (uint8_t *)&srv->lc->prop_state.time_fade;
        break;
    case BLE_MESH_LIGHT_CONTROL_TIME_PROLONG:
        val = (uint8_t *)&srv->lc->prop_state.time_prolong;
        break;
    case BLE_MESH_LIGHT_CONTROL_TIME_FADE_STANDBY_AUTO:
        val = (uint8_t *)&srv->lc->prop_state.time_fade_standby_auto;
        break;
    case BLE_MESH_LIGHT_CONTROL_TIME_FADE_STANDBY_MANUAL:
        val = (uint8_t *)&srv->lc->prop_state.time_fade_standby_manual;
        break;
    case BLE_MESH_LIGHT_CONTROL_LIGHTNESS_ON:
        val = (uint8_t *)&srv->lc->prop_state.lightness_on;
        break;
    case BLE_MESH_LIGHT_CONTROL_LIGHTNESS_PROLONG:
        val = (uint8_t *)&srv->lc->prop_state.lightness_prolong;
        break;
    case BLE_MESH_LIGHT_CONTROL_LIGHTNESS_STANDBY:
        val = (uint8_t *)&srv->lc->prop_state.lightness_standby;
        break;
    case BLE_MESH_LIGHT_CONTROL_AMBIENT_LUXLEVEL_ON:
        val = (uint8_t *)&srv->lc->prop_state.ambient_luxlevel_on;
        break;
    case BLE_MESH_LIGHT_CONTROL_AMBIENT_LUXLEVEL_PROLONG:
        val = (uint8_t *)&srv->lc->prop_state.ambient_luxlevel_prolong;
        break;
    case BLE_MESH_LIGHT_CONTROL_AMBIENT_LUXLEVEL_STANDBY:
        val = (uint8_t *)&srv->lc->prop_state.ambient_luxlevel_standby;
        break;
    case BLE_MESH_LIGHT_CONTROL_REGULATOR_KIU:
        val = (uint8_t *)&srv->lc->prop_state.regulator_kiu;
        break;
    case BLE_MESH_LIGHT_CONTROL_REGULATOR_KID:
        val = (uint8_t *)&srv->lc->prop_state.regulator_kid;
        break;
    case BLE_MESH_LIGHT_CONTROL_REGULATOR_KPU:
        val = (uint8_t *)&srv->lc->prop_state.regulator_kpu;
        break;
    case BLE_MESH_LIGHT_CONTROL_REGULATOR_KPD:
        val = (uint8_t *)&srv->lc->prop_state.regulator_kpd;
        break;
    case BLE_MESH_LIGHT_CONTROL_REGULATOR_ACCURACY:
        val = (uint8_t *)&srv->lc->prop_state.regulator_accuracy;
        break;
    }

    return val;
}

uint8_t *bt_mesh_get_lc_prop_value(struct bt_mesh_model *model, uint16_t prop_id)
{
    if (model == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return NULL;
    }

    return get_light_lc_prop_val(model, prop_id);
}

static void send_light_lc_prop_status(struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx,
                                      uint16_t prop_id, bool publish)
{
    struct net_buf_simple *msg = NULL;
    uint8_t length = 1 + 2 + 4;
    uint8_t *prop_val = NULL;

    prop_val = get_light_lc_prop_val(model, prop_id);
    if (prop_val == NULL) {
        BT_ERR("Failed to get Light LC Property value");
        return;
    }

    if (publish == false) {
        msg = bt_mesh_alloc_buf(length + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        if (msg == NULL) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
    } else {
        msg = bt_mesh_server_get_pub_msg(model, length);
        if (msg == NULL) {
            return;
        }
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_STATUS);
    net_buf_simple_add_le16(msg, prop_id);
    net_buf_simple_add_mem(msg, prop_val, bt_mesh_get_dev_prop_len(prop_id));

    if (publish == false) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        bt_mesh_free_buf(msg);
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    }
    return;
}

static void light_lc_prop_get(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    struct bt_mesh_light_lc_setup_srv *srv = model->user_data;
    uint16_t prop_id = 0U;

    if (srv == NULL || srv->lc == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    prop_id = net_buf_simple_pull_le16(buf);
    if (prop_id < 0x002B || prop_id > 0x003C) {
        BT_ERR("Invalid Light LC Property ID 0x%04x", prop_id);
        return;
    }

    /* Callback the received message to the application layer */
    if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_light_server_recv_get_msg_t get = {
            .lc_property_get.id = net_buf_simple_pull_le16(buf),
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_GET_MSG, model, ctx, (const uint8_t *)&get, sizeof(get));
        return;
    }

    send_light_lc_prop_status(model, ctx, prop_id, false);
    return;
}

static void light_lc_prop_set(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    struct bt_mesh_light_lc_setup_srv *srv = model->user_data;
    uint8_t *prop_val = NULL, expect_len = 0U;
    uint16_t prop_id = 0U;

    if (srv == NULL || srv->lc == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    prop_id = net_buf_simple_pull_le16(buf);
    if (prop_id < 0x002B || prop_id > 0x003C) {
        BT_ERR("Invalid Light LC Property ID 0x%04x", prop_id);
        return;
    }

    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_light_server_recv_set_msg_t set = {
            .lc_property_set.id = net_buf_simple_pull_le16(buf),
            .lc_property_set.value = buf,
        };
        bt_mesh_lighting_server_cb_evt_to_btc(
            BTC_BLE_MESH_EVT_LIGHTING_SERVER_RECV_SET_MSG, model, ctx, (const uint8_t *)&set, sizeof(set));
        return;
    }

    expect_len = bt_mesh_get_dev_prop_len(prop_id);
    if (buf->len != expect_len) {
        BT_ERR("Invalid Light LC Property 0x%04x length, expect %d, actual %d",
                prop_id, expect_len, buf->len);
        return;
    }

    prop_val = get_light_lc_prop_val(model, prop_id);
    if (prop_val == NULL) {
        BT_ERR("Failed to get Light LC Property value");
        return;
    }

    memcpy(prop_val, buf->data, buf->len);

    bt_mesh_light_server_state_change_t change = {
        .lc_property_set.id = prop_id,
        .lc_property_set.value = buf,
    };
    bt_mesh_lighting_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, model, ctx, (const uint8_t *)&change, sizeof(change));

    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET) {
        send_light_lc_prop_status(model, ctx, prop_id, false);
    }
    send_light_lc_prop_status(model, ctx, prop_id, true);

    return;
}

/* message handlers (End) */

/* Mapping of message handlers for Light Lightness Server (0x1300) */
const struct bt_mesh_model_op bt_mesh_light_lightness_srv_op[] = {
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET,              0, light_lightness_get        },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET,              3, light_lightness_set        },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK,        3, light_lightness_set        },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_GET,       0, light_lightness_get        },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET,       3, light_lightness_linear_set },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK, 3, light_lightness_linear_set },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_GET,         0, light_lightness_get        },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_GET,      0, light_lightness_get        },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_GET,        0, light_lightness_get        },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Light Lightness Setup Server (0x1301) */
const struct bt_mesh_model_op bt_mesh_light_lightness_setup_srv_op[] = {
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET,       2, light_lightness_default_set },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET_UNACK, 2, light_lightness_default_set },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET,         4, light_lightness_range_set   },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET_UNACK,   4, light_lightness_range_set   },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Light CTL Server (0x1303) */
const struct bt_mesh_model_op bt_mesh_light_ctl_srv_op[] = {
    { BLE_MESH_MODEL_OP_LIGHT_CTL_GET,                   0, light_ctl_get },
    { BLE_MESH_MODEL_OP_LIGHT_CTL_SET,                   7, light_ctl_set },
    { BLE_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK,             7, light_ctl_set },
    { BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_GET, 0, light_ctl_get },
    { BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_GET,           0, light_ctl_get },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Light CTL Setup Server (0x1304) */
const struct bt_mesh_model_op bt_mesh_light_ctl_setup_srv_op[] = {
    { BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET,                 6, light_ctl_default_set    },
    { BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET_UNACK,           6, light_ctl_default_set    },
    { BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET,       4, light_ctl_temp_range_set },
    { BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACK, 4, light_ctl_temp_range_set },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Light CTL Temperature Server (0x1306) */
const struct bt_mesh_model_op bt_mesh_light_ctl_temp_srv_op[] = {
    { BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_GET,       0, light_ctl_get      },
    { BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET,       5, light_ctl_temp_set },
    { BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET_UNACK, 5, light_ctl_temp_set },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Light HSL Server (0x1307) */
const struct bt_mesh_model_op bt_mesh_light_hsl_srv_op[] = {
    { BLE_MESH_MODEL_OP_LIGHT_HSL_GET,         0, light_hsl_get },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_SET,         7, light_hsl_set },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK,   7, light_hsl_set },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_GET,  0, light_hsl_get },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_GET, 0, light_hsl_get },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_GET,   0, light_hsl_get },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Light HSL Setup Server (0x1308) */
const struct bt_mesh_model_op bt_mesh_light_hsl_setup_srv_op[] = {
    { BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET,       6, light_hsl_default_set },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET_UNACK, 6, light_hsl_default_set },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET,         8, light_hsl_range_set   },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET_UNACK,   8, light_hsl_range_set   },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Light HSL Hue Server (0x130A) */
const struct bt_mesh_model_op bt_mesh_light_hsl_hue_srv_op[] = {
    { BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_GET,       0, light_hsl_get     },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET,       3, light_hsl_hue_set },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET_UNACK, 3, light_hsl_hue_set },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Light HSL Saturation Server (0x130B) */
const struct bt_mesh_model_op bt_mesh_light_hsl_sat_srv_op[] = {
    { BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_GET,       0, light_hsl_get     },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET,       3, light_hsl_sat_set },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET_UNACK, 3, light_hsl_sat_set },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Light xyL Server (0x130C) */
const struct bt_mesh_model_op bt_mesh_light_xyl_srv_op[] = {
    { BLE_MESH_MODEL_OP_LIGHT_XYL_GET,         0, light_xyl_get },
    { BLE_MESH_MODEL_OP_LIGHT_XYL_SET,         7, light_xyl_set },
    { BLE_MESH_MODEL_OP_LIGHT_XYL_SET_UNACK,   7, light_xyl_set },
    { BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_GET,  0, light_xyl_get },
    { BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_GET, 0, light_xyl_get },
    { BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_GET,   0, light_xyl_get },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Light xyL Setup Server (0x130D) */
const struct bt_mesh_model_op bt_mesh_light_xyl_setup_srv_op[] = {
    { BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET,       6, light_xyl_default_set },
    { BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET_UNACK, 6, light_xyl_default_set },
    { BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET,         8, light_xyl_range_set   },
    { BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET_UNACK,   8, light_xyl_range_set   },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Light LC Server (0x130F) */
const struct bt_mesh_model_op bt_mesh_light_lc_srv_op[] = {
    { BLE_MESH_MODEL_OP_LIGHT_LC_MODE_GET,              0, light_lc_get             },
    { BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET,              1, light_lc_mode_set        },
    { BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET_UNACK,        1, light_lc_mode_set        },
    { BLE_MESH_MODEL_OP_LIGHT_LC_OM_GET,                0, light_lc_get             },
    { BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET,                1, light_lc_om_set          },
    { BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET_UNACK,          1, light_lc_om_set          },
    { BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_GET,       0, light_lc_get             },
    { BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET,       2, light_lc_light_onoff_set },
    { BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET_UNACK, 2, light_lc_light_onoff_set },
    { BLE_MESH_MODEL_OP_SENSOR_STATUS,                  3, light_lc_sensor_status   },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Light LC Setup Server (0x1310) */
const struct bt_mesh_model_op bt_mesh_light_lc_setup_srv_op[] = {
    { BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_GET,       2, light_lc_prop_get },
    { BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET,       3, light_lc_prop_set },
    { BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET_UNACK, 3, light_lc_prop_set },
    BLE_MESH_MODEL_OP_END,
};

static int light_server_init(struct bt_mesh_model *model)
{
    if (model->user_data == NULL) {
        BT_ERR("Invalid Lighting Server user data, model id 0x%04x", model->id);
        return -EINVAL;
    }

    switch (model->id) {
    case BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV: {
        struct bt_mesh_light_lightness_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light Lightness State");
            return -EINVAL;
        }
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_AUTO_RSP) {
            bt_mesh_server_alloc_ctx(&srv->actual_transition.timer.work);
            bt_mesh_server_alloc_ctx(&srv->linear_transition.timer.work);
            k_delayed_work_init(&srv->actual_transition.timer, light_lightness_actual_work_handler);
            k_delayed_work_init(&srv->linear_transition.timer, light_lightness_linear_work_handler);
        }
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV: {
        struct bt_mesh_light_lightness_setup_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light Lightness State");
            return -EINVAL;
        }
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_CTL_SRV: {
        struct bt_mesh_light_ctl_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light CTL State");
            return -EINVAL;
        }
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_AUTO_RSP) {
            bt_mesh_server_alloc_ctx(&srv->transition.timer.work);
            k_delayed_work_init(&srv->transition.timer, light_ctl_work_handler);
        }
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV: {
        struct bt_mesh_light_ctl_setup_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light CTL State");
            return -EINVAL;
        }
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV: {
        struct bt_mesh_light_ctl_temp_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light CTL State");
            return -EINVAL;
        }
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_AUTO_RSP) {
            bt_mesh_server_alloc_ctx(&srv->transition.timer.work);
            k_delayed_work_init(&srv->transition.timer, light_ctl_temp_work_handler);
        }
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_HSL_SRV: {
        struct bt_mesh_light_hsl_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light HSL State");
            return -EINVAL;
        }
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_AUTO_RSP) {
            bt_mesh_server_alloc_ctx(&srv->transition.timer.work);
            k_delayed_work_init(&srv->transition.timer, light_hsl_work_handler);
        }
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV: {
        struct bt_mesh_light_hsl_setup_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light HSL State");
            return -EINVAL;
        }
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_HSL_HUE_SRV: {
        struct bt_mesh_light_hsl_hue_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light HSL State");
            return -EINVAL;
        }
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_AUTO_RSP) {
            bt_mesh_server_alloc_ctx(&srv->transition.timer.work);
            k_delayed_work_init(&srv->transition.timer, light_hsl_hue_work_handler);
        }
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_HSL_SAT_SRV: {
        struct bt_mesh_light_hsl_sat_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light HSL State");
            return -EINVAL;
        }
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_AUTO_RSP) {
            bt_mesh_server_alloc_ctx(&srv->transition.timer.work);
            k_delayed_work_init(&srv->transition.timer, light_hsl_sat_work_handler);
        }
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_XYL_SRV: {
        struct bt_mesh_light_xyl_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light xyL State");
            return -EINVAL;
        }
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_AUTO_RSP) {
            bt_mesh_server_alloc_ctx(&srv->transition.timer.work);
            k_delayed_work_init(&srv->transition.timer, light_xyl_work_handler);
        }
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_XYL_SETUP_SRV: {
        struct bt_mesh_light_xyl_setup_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light xyL State");
            return -EINVAL;
        }
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_LC_SRV: {
        struct bt_mesh_light_lc_srv *srv = model->user_data;
        if (srv->lc == NULL) {
            BT_ERR("Invalid Light LC State");
            return -EINVAL;
        }
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_AUTO_RSP) {
            bt_mesh_server_alloc_ctx(&srv->transition.timer.work);
            k_delayed_work_init(&srv->transition.timer, light_lc_work_handler);
        }
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_LC_SETUP_SRV: {
        struct bt_mesh_light_lc_setup_srv *srv = model->user_data;
        if (srv->lc == NULL) {
            BT_ERR("Invalid Light LC State");
            return -EINVAL;
        }
        srv->model = model;
        break;
    }
    default:
        BT_WARN("Unknown Light Server, model id 0x%04x", model->id);
        return -EINVAL;
    }

    bt_mesh_light_server_mutex_new();

    return 0;
}

static int light_lightness_srv_init(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Light Lightness Server has no publication support");
        return -EINVAL;
    }

    /* When this model is present on an Element, the corresponding Light Lightness
     * Setup Server model shall also be present.
     */
    struct bt_mesh_elem *element = bt_mesh_model_elem(model);
    if (bt_mesh_model_find(element, BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV) == NULL) {
        BT_WARN("Light Lightness Setup Server not present");
        /* Just give a warning here, continue with the initialization */
    }
    return light_server_init(model);
}

static int light_lightness_setup_srv_init(struct bt_mesh_model *model)
{
    return light_server_init(model);
}

static int light_ctl_srv_init(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Light CTL Server has no publication support");
        return -EINVAL;
    }

    /**
     * When this model is present on an Element, the corresponding Light CTL
     * Temperature Server model and the corresponding Light CTL Setup Server
     * model shall also be present.
     * The model requires two elements: the main element and the Temperature
     * element. The Temperature element contains the corresponding Light CTL
     * Temperature Server model.
     */
    struct bt_mesh_elem *element = bt_mesh_model_elem(model);
    if (bt_mesh_model_find(element, BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV) == NULL) {
        BT_WARN("Light CTL Setup Server not present");
        /* Just give a warning here, continue with the initialization */
    }
    if (bt_mesh_elem_count() < 2) {
        BT_WARN("Light CTL Server requires two elements");
        /* Just give a warning here, continue with the initialization */
    }
    return light_server_init(model);
}

static int light_ctl_setup_srv_init(struct bt_mesh_model *model)
{
    return light_server_init(model);
}

static int light_ctl_temp_srv_init(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Light CTL Temperature Server has no publication support");
        return -EINVAL;
    }

    return light_server_init(model);
}

static int light_hsl_srv_init(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Light HSL Server has no publication support");
        return -EINVAL;
    }

    /**
     * When this model is present on an Element, the corresponding Light HSL Hue
     * Server model and the corresponding Light HSL Saturation Server model and
     * the corresponding Light HSL Setup Server model shall also be present.
     * The model requires three elements: the main element and the Hue element
     * and the Saturation element. The Hue element contains the corresponding
     * Light HSL Hue Server model, and the Saturation element contains the corr-
     * esponding Light HSL Saturation Server model.
     */
    struct bt_mesh_elem *element = bt_mesh_model_elem(model);
    if (bt_mesh_model_find(element, BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV) == NULL) {
        BT_WARN("Light HSL Setup Server not present");
        /* Just give a warning here, continue with the initialization */
    }
    if (bt_mesh_elem_count() < 3) {
        BT_WARN("Light HSL Server requires three elements");
        /* Just give a warning here, continue with the initialization */
    }
    return light_server_init(model);
}

static int light_hsl_setup_srv_init(struct bt_mesh_model *model)
{
    return light_server_init(model);
}

static int light_hsl_hue_srv_init(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Light HSL Hue Server has no publication support");
        return -EINVAL;
    }

    return light_server_init(model);
}

static int light_hsl_sat_srv_init(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Light HSL Saturation Server has no publication support");
        return -EINVAL;
    }

    return light_server_init(model);
}

static int light_xyl_srv_init(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Light xyL Server has no publication support");
        return -EINVAL;
    }

    /**
     * When this model is present on an Element, the corresponding Light xyL
     * Setup Server model shall also be present.
     */
    struct bt_mesh_elem *element = bt_mesh_model_elem(model);
    if (bt_mesh_model_find(element, BLE_MESH_MODEL_ID_LIGHT_XYL_SETUP_SRV) == NULL) {
        BT_WARN("Light xyL Setup Server not present");
        /* Just give a warning here, continue with the initialization */
    }
    return light_server_init(model);
}

static int light_xyl_setup_srv_init(struct bt_mesh_model *model)
{
    return light_server_init(model);
}

static int light_lc_srv_init(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Light LC Server has no publication support");
        return -EINVAL;
    }

    return light_server_init(model);
}

static int light_lc_setup_srv_init(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Light LC Setup Server has no publication support");
        return -EINVAL;
    }

    /**
     * When this model is present on an Element, the corresponding Light LC
     * Setup Server model shall also be present.
     */
    struct bt_mesh_elem *element = bt_mesh_model_elem(model);
    if (bt_mesh_model_find(element, BLE_MESH_MODEL_ID_LIGHT_LC_SETUP_SRV) == NULL) {
        BT_WARN("Light LC Setup Server not present");
        /* Just give a warning here, continue with the initialization */
    }
    return light_server_init(model);
}

#if CONFIG_BLE_MESH_DEINIT
static int light_server_deinit(struct bt_mesh_model *model)
{
    if (model->user_data == NULL) {
        BT_ERR("Invalid Lighting Server user data, model id 0x%04x", model->id);
        return -EINVAL;
    }

    switch (model->id) {
    case BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV: {
        struct bt_mesh_light_lightness_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light Lightness State");
            return -EINVAL;
        }
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_AUTO_RSP) {
            bt_mesh_server_free_ctx(&srv->actual_transition.timer.work);
            bt_mesh_server_free_ctx(&srv->linear_transition.timer.work);
            k_delayed_work_free(&srv->actual_transition.timer);
            k_delayed_work_free(&srv->linear_transition.timer);
        }
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_CTL_SRV: {
        struct bt_mesh_light_ctl_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light CTL State");
            return -EINVAL;
        }
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_AUTO_RSP) {
            bt_mesh_server_free_ctx(&srv->transition.timer.work);
            k_delayed_work_free(&srv->transition.timer);
        }
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV: {
        struct bt_mesh_light_ctl_temp_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light CTL State");
            return -EINVAL;
        }
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_AUTO_RSP) {
            bt_mesh_server_free_ctx(&srv->transition.timer.work);
            k_delayed_work_free(&srv->transition.timer);
        }
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_HSL_SRV: {
        struct bt_mesh_light_hsl_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light HSL State");
            return -EINVAL;
        }
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_AUTO_RSP) {
            bt_mesh_server_free_ctx(&srv->transition.timer.work);
            k_delayed_work_free(&srv->transition.timer);
        }
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_HSL_HUE_SRV: {
        struct bt_mesh_light_hsl_hue_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light HSL State");
            return -EINVAL;
        }
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_AUTO_RSP) {
            bt_mesh_server_free_ctx(&srv->transition.timer.work);
            k_delayed_work_free(&srv->transition.timer);
        }
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_HSL_SAT_SRV: {
        struct bt_mesh_light_hsl_sat_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light HSL State");
            return -EINVAL;
        }
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_AUTO_RSP) {
            bt_mesh_server_free_ctx(&srv->transition.timer.work);
            k_delayed_work_free(&srv->transition.timer);
        }
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_XYL_SRV: {
        struct bt_mesh_light_xyl_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light xyL State");
            return -EINVAL;
        }
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_AUTO_RSP) {
            bt_mesh_server_free_ctx(&srv->transition.timer.work);
            k_delayed_work_free(&srv->transition.timer);
        }
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_LC_SRV: {
        struct bt_mesh_light_lc_srv *srv = model->user_data;
        if (srv->lc == NULL) {
            BT_ERR("Invalid Light LC State");
            return -EINVAL;
        }
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_AUTO_RSP) {
            bt_mesh_server_free_ctx(&srv->transition.timer.work);
            k_delayed_work_free(&srv->transition.timer);
        }
        break;
    }
    case BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV:
    case BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV:
    case BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV:
    case BLE_MESH_MODEL_ID_LIGHT_XYL_SETUP_SRV:
    case BLE_MESH_MODEL_ID_LIGHT_LC_SETUP_SRV:
        break;
    default:
        BT_WARN("Unknown Light Server, model id 0x%04x", model->id);
        return -EINVAL;
    }

    bt_mesh_light_server_mutex_free();

    return 0;
}

static int light_lightness_srv_deinit(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Light Lightness Server has no publication support");
        return -EINVAL;
    }

    return light_server_deinit(model);
}

static int light_lightness_setup_srv_deinit(struct bt_mesh_model *model)
{
    return light_server_deinit(model);
}

static int light_ctl_srv_deinit(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Light CTL Server has no publication support");
        return -EINVAL;
    }

    return light_server_deinit(model);
}

static int light_ctl_setup_srv_deinit(struct bt_mesh_model *model)
{
    return light_server_deinit(model);
}

static int light_ctl_temp_srv_deinit(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Light CTL Temperature Server has no publication support");
        return -EINVAL;
    }

    return light_server_deinit(model);
}

static int light_hsl_srv_deinit(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Light HSL Server has no publication support");
        return -EINVAL;
    }

    return light_server_deinit(model);
}

static int light_hsl_setup_srv_deinit(struct bt_mesh_model *model)
{
    return light_server_deinit(model);
}

static int light_hsl_hue_srv_deinit(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Light HSL Hue Server has no publication support");
        return -EINVAL;
    }

    return light_server_deinit(model);
}

static int light_hsl_sat_srv_deinit(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Light HSL Saturation Server has no publication support");
        return -EINVAL;
    }

    return light_server_deinit(model);
}

static int light_xyl_srv_deinit(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Light xyL Server has no publication support");
        return -EINVAL;
    }

    return light_server_deinit(model);
}

static int light_xyl_setup_srv_deinit(struct bt_mesh_model *model)
{
    return light_server_deinit(model);
}

static int light_lc_srv_deinit(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Light LC Server has no publication support");
        return -EINVAL;
    }

    return light_server_deinit(model);
}

static int light_lc_setup_srv_deinit(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Light LC Setup Server has no publication support");
        return -EINVAL;
    }

    return light_server_deinit(model);
}
#endif /* CONFIG_BLE_MESH_DEINIT */

const struct bt_mesh_model_cb bt_mesh_light_lightness_srv_cb = {
    .init = light_lightness_srv_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = light_lightness_srv_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

const struct bt_mesh_model_cb bt_mesh_light_lightness_setup_srv_cb = {
    .init = light_lightness_setup_srv_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = light_lightness_setup_srv_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

const struct bt_mesh_model_cb bt_mesh_light_ctl_srv_cb = {
    .init = light_ctl_srv_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = light_ctl_srv_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

const struct bt_mesh_model_cb bt_mesh_light_ctl_setup_srv_cb = {
    .init = light_ctl_setup_srv_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = light_ctl_setup_srv_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

const struct bt_mesh_model_cb bt_mesh_light_ctl_temp_srv_cb = {
    .init = light_ctl_temp_srv_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = light_ctl_temp_srv_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

const struct bt_mesh_model_cb bt_mesh_light_hsl_srv_cb = {
    .init = light_hsl_srv_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = light_hsl_srv_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

const struct bt_mesh_model_cb bt_mesh_light_hsl_setup_srv_cb = {
    .init = light_hsl_setup_srv_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = light_hsl_setup_srv_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

const struct bt_mesh_model_cb bt_mesh_light_hsl_hue_srv_cb = {
    .init = light_hsl_hue_srv_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = light_hsl_hue_srv_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

const struct bt_mesh_model_cb bt_mesh_light_hsl_sat_srv_cb = {
    .init = light_hsl_sat_srv_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = light_hsl_sat_srv_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

const struct bt_mesh_model_cb bt_mesh_light_xyl_srv_cb = {
    .init = light_xyl_srv_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = light_xyl_srv_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

const struct bt_mesh_model_cb bt_mesh_light_xyl_setup_srv_cb = {
    .init = light_xyl_setup_srv_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = light_xyl_setup_srv_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

const struct bt_mesh_model_cb bt_mesh_light_lc_srv_cb = {
    .init = light_lc_srv_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = light_lc_srv_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

const struct bt_mesh_model_cb bt_mesh_light_lc_setup_srv_cb = {
    .init = light_lc_setup_srv_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = light_lc_setup_srv_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

#endif /* CONFIG_BLE_MESH_LIGHTING_SERVER */

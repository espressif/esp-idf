/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include "btc_ble_mesh_time_scene_model.h"

#include "mesh/config.h"
#include "access.h"
#include "transport.h"
#include "mesh/model_opcode.h"
#include "mesh/state_transition.h"

#if CONFIG_BLE_MESH_TIME_SCENE_SERVER

static bt_mesh_mutex_t time_scene_server_lock;

void bt_mesh_time_scene_server_lock(void)
{
    bt_mesh_mutex_lock(&time_scene_server_lock);
}

void bt_mesh_time_scene_server_unlock(void)
{
    bt_mesh_mutex_unlock(&time_scene_server_lock);
}

/* message handlers (Start) */

/* Time Server & Time Setup Server message handlers */
static void send_time_status(struct bt_mesh_model *model,
                             struct bt_mesh_msg_ctx *ctx,
                             bool publish, uint16_t opcode)
{
    struct net_buf_simple *msg = NULL;
    uint8_t zero[5] = {0};
    uint8_t length = 1 + 10;

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
    case BLE_MESH_MODEL_OP_TIME_STATUS:
        if (model->id == BLE_MESH_MODEL_ID_TIME_SRV) {
            struct bt_mesh_time_srv *srv = model->user_data;
            net_buf_simple_add_mem(msg, srv->state->time.tai_seconds, TAI_SECONDS_LEN);
            if (memcmp(srv->state->time.tai_seconds, zero, TAI_SECONDS_LEN)) {
                net_buf_simple_add_u8(msg, srv->state->time.subsecond);
                /**
                 * Set the Uncertainty field to a value that is a sum of the value of
                 * the Uncertainty state and an estimated time it will take the message
                 * to be processed before being sent on the radio interface.
                 *
                 * TODO: how to estimate the processing time?
                 */
                net_buf_simple_add_u8(msg, srv->state->time.uncertainty);
                net_buf_simple_add_le16(msg,
                                        (srv->state->time.tai_utc_delta_curr << 1) | srv->state->time.time_authority);
                net_buf_simple_add_u8(msg, srv->state->time.time_zone_offset_curr);
            }
        } else if (model->id == BLE_MESH_MODEL_ID_TIME_SETUP_SRV) {
            struct bt_mesh_time_setup_srv *srv = model->user_data;
            net_buf_simple_add_mem(msg, srv->state->time.tai_seconds, TAI_SECONDS_LEN);
            if (memcmp(srv->state->time.tai_seconds, zero, TAI_SECONDS_LEN)) {
                net_buf_simple_add_u8(msg, srv->state->time.subsecond);
                net_buf_simple_add_u8(msg, srv->state->time.uncertainty);
                net_buf_simple_add_le16(msg,
                                        (srv->state->time.tai_utc_delta_curr << 1) | srv->state->time.time_authority);
                net_buf_simple_add_u8(msg, srv->state->time.time_zone_offset_curr);
            }
        }
        break;
    case BLE_MESH_MODEL_OP_TIME_ZONE_STATUS:
        if (model->id == BLE_MESH_MODEL_ID_TIME_SRV) {
            struct bt_mesh_time_srv *srv = model->user_data;
            net_buf_simple_add_u8(msg, srv->state->time.time_zone_offset_curr);
            net_buf_simple_add_u8(msg, srv->state->time.time_zone_offset_new);
            net_buf_simple_add_mem(msg, srv->state->time.tai_zone_change, TAI_OF_ZONE_CHANGE_LEN);
        } else if (model->id == BLE_MESH_MODEL_ID_TIME_SETUP_SRV) {
            struct bt_mesh_time_setup_srv *srv = model->user_data;
            net_buf_simple_add_u8(msg, srv->state->time.time_zone_offset_curr);
            net_buf_simple_add_u8(msg, srv->state->time.time_zone_offset_new);
            net_buf_simple_add_mem(msg, srv->state->time.tai_zone_change, TAI_OF_ZONE_CHANGE_LEN);
        }
        break;
    case BLE_MESH_MODEL_OP_TAI_UTC_DELTA_STATUS:
        if (model->id == BLE_MESH_MODEL_ID_TIME_SRV) {
            struct bt_mesh_time_srv *srv = model->user_data;
            net_buf_simple_add_le16(msg, srv->state->time.tai_utc_delta_curr);
            net_buf_simple_add_le16(msg, srv->state->time.tai_utc_delta_new);
            net_buf_simple_add_mem(msg, srv->state->time.tai_delta_change, TAI_OF_DELTA_CHANGE_LEN);
        } else if (model->id == BLE_MESH_MODEL_ID_TIME_SETUP_SRV) {
            struct bt_mesh_time_setup_srv *srv = model->user_data;
            net_buf_simple_add_le16(msg, srv->state->time.tai_utc_delta_curr);
            net_buf_simple_add_le16(msg, srv->state->time.tai_utc_delta_new);
            net_buf_simple_add_mem(msg, srv->state->time.tai_delta_change, TAI_OF_DELTA_CHANGE_LEN);
        }
        break;
    case BLE_MESH_MODEL_OP_TIME_ROLE_STATUS: {
        struct bt_mesh_time_setup_srv *srv = model->user_data;
        net_buf_simple_add_u8(msg, srv->state->time_role);
        break;
    }
    default:
        BT_WARN("Unknown Time status opcode 0x%04x", opcode);
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
}

static void time_get(struct bt_mesh_model *model,
                     struct bt_mesh_msg_ctx *ctx,
                     struct net_buf_simple *buf)
{
    struct bt_mesh_server_rsp_ctrl *rsp_ctrl = NULL;
    uint8_t zero[5] = {0};
    uint16_t opcode = 0U, val = 0U;
    uint8_t prev_ttl = 0U;

    if (model->user_data == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    switch (model->id) {
    case BLE_MESH_MODEL_ID_TIME_SRV: {
        struct bt_mesh_time_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Time Server state");
            return;
        }
        rsp_ctrl = &srv->rsp_ctrl;
        break;
    }
    case BLE_MESH_MODEL_ID_TIME_SETUP_SRV: {
        struct bt_mesh_time_setup_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Time Setup Server state");
            return;
        }
        rsp_ctrl = &srv->rsp_ctrl;
        break;
    }
    default:
        BT_ERR("Invalid Time Server, model id 0x%04x", model->id);
        return;
    }

    if (rsp_ctrl->get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        if (ctx->recv_op != BLE_MESH_MODEL_OP_TIME_STATUS) {
            bt_mesh_time_scene_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_RECV_GET_MSG,
                                                    model, ctx, NULL, 0);
            return;
        }
    }

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_TIME_GET:
        opcode = BLE_MESH_MODEL_OP_TIME_STATUS;
        break;
    case BLE_MESH_MODEL_OP_TIME_STATUS: {
        struct bt_mesh_time_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Time Server state");
            return;
        }
        if (srv->state->time_role != TIME_RELAY &&
                srv->state->time_role != TIME_CLIENT) {
            /**
             * If the value of the Time Role state of the element is 0x00 (None) or
             * 0x01 (Time Authority), the message shall be ignored.
             */
            return;
        }
        if (rsp_ctrl->status_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
            bt_mesh_time_scene_server_recv_status_msg_t status = {0};
            memcpy(status.time_status.tai_seconds, buf->data, TAI_SECONDS_LEN);
            net_buf_simple_pull(buf, TAI_SECONDS_LEN);
            if (memcmp(status.time_status.tai_seconds, zero, TAI_SECONDS_LEN)) {
                if (buf->len != TAI_SECONDS_LEN) {
                    BT_ERR("Invalid Time Status length %d", buf->len + TAI_SECONDS_LEN);
                    return;
                }
                status.time_status.subsecond = net_buf_simple_pull_u8(buf);
                status.time_status.uncertainty = net_buf_simple_pull_u8(buf);
                val = net_buf_simple_pull_le16(buf);
                status.time_status.time_authority = val & BIT(0);
                status.time_status.tai_utc_delta = (val >> 1) & BIT_MASK(15);
                status.time_status.time_zone_offset = net_buf_simple_pull_u8(buf);
            }
            bt_mesh_time_scene_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_RECV_STATUS_MSG,
                                                    model, ctx, (const uint8_t *)&status, sizeof(status));
            return;
        }
        memcpy(srv->state->time.tai_seconds, buf->data, TAI_SECONDS_LEN);
        net_buf_simple_pull(buf, TAI_SECONDS_LEN);
        /**
         * If the TAI Seconds field is 0x0000000000 the Subsecond, Uncertainty,
         * Time Authority, TAI-UTC Delta and Time Zone Offset fields shall be
         * omitted; otherwise these fields shall be present.
         */
        if (memcmp(srv->state->time.tai_seconds, zero, TAI_SECONDS_LEN)) {
            if (buf->len != TAI_SECONDS_LEN) {
                BT_ERR("Invalid Time Status length %d", buf->len + TAI_SECONDS_LEN);
                return;
            }
            srv->state->time.subsecond = net_buf_simple_pull_u8(buf);
            srv->state->time.uncertainty = net_buf_simple_pull_u8(buf);
            val = net_buf_simple_pull_le16(buf);
            srv->state->time.tai_utc_delta_curr = (val >> 1) & BIT_MASK(15);
            srv->state->time.time_zone_offset_curr = net_buf_simple_pull_u8(buf);
        }

        bt_mesh_time_scene_server_state_change_t change = {0};
        memcpy(change.time_status.tai_seconds, srv->state->time.tai_seconds, TAI_SECONDS_LEN);
        change.time_status.subsecond = srv->state->time.subsecond;
        change.time_status.uncertainty = srv->state->time.uncertainty;
        change.time_status.time_authority = srv->state->time.time_authority;
        change.time_status.tai_utc_delta_curr = srv->state->time.subsecond;
        bt_mesh_time_scene_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_STATE_CHANGE,
                                                model, ctx, (const uint8_t *)&change, sizeof(change));

        if (model->pub == NULL || model->pub->msg == NULL ||
                model->pub->addr == BLE_MESH_ADDR_UNASSIGNED) {
            return;
        }
        prev_ttl = model->pub->ttl;
        if (srv->state->time_role == TIME_RELAY) {
            /**
             * Shall publish a Time Status message using TTL = 0 if the value of the
             * Time Role state is 0x02 (Time Relay) and the Publish Address for the
             * Time Server model is not set to unassigned address.
             */
            model->pub->ttl = 0U;
        }
        send_time_status(model, NULL, true, BLE_MESH_MODEL_OP_TIME_STATUS);
        /* Restore model publication ttl value */
        model->pub->ttl = prev_ttl;
        return;
    }
    case BLE_MESH_MODEL_OP_TIME_ZONE_GET:
        opcode = BLE_MESH_MODEL_OP_TIME_ZONE_STATUS;
        break;
    case BLE_MESH_MODEL_OP_TAI_UTC_DELTA_GET:
        opcode = BLE_MESH_MODEL_OP_TAI_UTC_DELTA_STATUS;
        break;
    case BLE_MESH_MODEL_OP_TIME_ROLE_GET:
        opcode = BLE_MESH_MODEL_OP_TIME_ROLE_STATUS;
        break;
    default:
        BT_WARN("Unknown Time Get opcode 0x%04x", ctx->recv_op);
        return;
    }

    send_time_status(model, ctx, false, opcode);
}

static void time_set(struct bt_mesh_model *model,
                     struct bt_mesh_msg_ctx *ctx,
                     struct net_buf_simple *buf)
{
    struct bt_mesh_time_setup_srv *srv = model->user_data;
    bt_mesh_time_scene_server_state_change_t change = {0};
    uint16_t opcode = 0U, val = 0U;
    uint8_t role = 0U;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_TIME_SET:
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
            bt_mesh_time_scene_server_recv_set_msg_t set = {0};
            memcpy(set.time_set.tai_seconds, buf->data, TAI_SECONDS_LEN);
            net_buf_simple_pull(buf, TAI_SECONDS_LEN);
            set.time_set.subsecond = net_buf_simple_pull_u8(buf);
            set.time_set.uncertainty = net_buf_simple_pull_u8(buf);
            val = net_buf_simple_pull_le16(buf);
            set.time_set.time_authority = val & BIT(0);
            set.time_set.tai_utc_delta = (val >> 1) & BIT_MASK(15);
            set.time_set.time_zone_offset = net_buf_simple_pull_u8(buf);
            bt_mesh_time_scene_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_RECV_SET_MSG,
                                                    model, ctx, (const uint8_t *)&set, sizeof(set));
            return;
        }
        memcpy(srv->state->time.tai_seconds, buf->data, TAI_SECONDS_LEN);
        net_buf_simple_pull(buf, TAI_SECONDS_LEN);
        srv->state->time.subsecond = net_buf_simple_pull_u8(buf);
        srv->state->time.uncertainty = net_buf_simple_pull_u8(buf);
        val = net_buf_simple_pull_le16(buf);
        srv->state->time.time_authority = val & BIT(0);
        srv->state->time.tai_utc_delta_curr = (val >> 1) & BIT_MASK(15);
        srv->state->time.time_zone_offset_curr = net_buf_simple_pull_u8(buf);
        opcode = BLE_MESH_MODEL_OP_TIME_STATUS;
        break;
    case BLE_MESH_MODEL_OP_TIME_ZONE_SET:
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
            bt_mesh_time_scene_server_recv_set_msg_t set = {0};
            set.time_zone_set.time_zone_offset_new = net_buf_simple_pull_u8(buf);
            memcpy(set.time_zone_set.tai_zone_change, buf->data, TAI_OF_ZONE_CHANGE_LEN);
            net_buf_simple_pull(buf, TAI_OF_ZONE_CHANGE_LEN);
            bt_mesh_time_scene_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_RECV_SET_MSG,
                                                    model, ctx, (const uint8_t *)&set, sizeof(set));
            return;
        }
        srv->state->time.time_zone_offset_new = net_buf_simple_pull_u8(buf);
        memcpy(srv->state->time.tai_zone_change, buf->data, TAI_OF_ZONE_CHANGE_LEN);
        net_buf_simple_pull(buf, TAI_OF_ZONE_CHANGE_LEN);
        opcode = BLE_MESH_MODEL_OP_TIME_ZONE_STATUS;
        break;
    case BLE_MESH_MODEL_OP_TAI_UTC_DELTA_SET:
        val = net_buf_simple_pull_le16(buf);
        if ((val >> 15) & BIT(0)) {
            BT_ERR("Invalid Padding value 1");
            return;
        }
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
            bt_mesh_time_scene_server_recv_set_msg_t set = {0};
            set.tai_utc_delta_set.tai_utc_delta_new = val & BIT_MASK(15);
            memcpy(set.tai_utc_delta_set.tai_delta_change, buf->data, TAI_OF_DELTA_CHANGE_LEN);
            net_buf_simple_pull(buf, TAI_OF_DELTA_CHANGE_LEN);
            bt_mesh_time_scene_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_RECV_SET_MSG,
                                                    model, ctx, (const uint8_t *)&set, sizeof(set));
            return;
        }
        srv->state->time.tai_utc_delta_new = val & BIT_MASK(15);
        memcpy(srv->state->time.tai_delta_change, buf->data, TAI_OF_DELTA_CHANGE_LEN);
        net_buf_simple_pull(buf, TAI_OF_DELTA_CHANGE_LEN);
        opcode = BLE_MESH_MODEL_OP_TAI_UTC_DELTA_STATUS;
        break;
    case BLE_MESH_MODEL_OP_TIME_ROLE_SET:
        role = net_buf_simple_pull_u8(buf);
        if (role > TIME_CLIENT) {
            BT_ERR("Invalid Time Role 0x%02x", role);
            return;
        }
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
            bt_mesh_time_scene_server_recv_set_msg_t set = {
                .time_role_set.time_role = role,
            };
            bt_mesh_time_scene_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_RECV_SET_MSG,
                                                    model, ctx, (const uint8_t *)&set, sizeof(set));
            return;
        }
        srv->state->time_role = role;
        opcode = BLE_MESH_MODEL_OP_TIME_ROLE_STATUS;
        break;
    default:
        BT_ERR("Unknown Time Set opcode 0x%04x", ctx->recv_op);
        return;
    }

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_TIME_SET:
        memcpy(change.time_set.tai_seconds, srv->state->time.tai_seconds, TAI_SECONDS_LEN);
        change.time_set.subsecond = srv->state->time.subsecond;
        change.time_set.uncertainty = srv->state->time.uncertainty;
        change.time_set.time_authority = srv->state->time.time_authority;
        change.time_set.tai_utc_delta_curr = srv->state->time.subsecond;
        break;
    case BLE_MESH_MODEL_OP_TIME_ZONE_SET:
        change.time_zone_set.time_zone_offset_new = srv->state->time.time_zone_offset_new;
        memcpy(change.time_zone_set.tai_zone_change, srv->state->time.tai_zone_change, TAI_OF_ZONE_CHANGE_LEN);
        break;
    case BLE_MESH_MODEL_OP_TAI_UTC_DELTA_SET:
        change.tai_utc_delta_set.tai_utc_delta_new = srv->state->time.tai_utc_delta_new;
        memcpy(change.tai_utc_delta_set.tai_delta_change, srv->state->time.tai_delta_change, TAI_OF_DELTA_CHANGE_LEN);
        break;
    case BLE_MESH_MODEL_OP_TIME_ROLE_SET:
        change.time_role_set.role = srv->state->time_role;
        break;
    default:
        return;
    }

    bt_mesh_time_scene_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_STATE_CHANGE,
                                            model, ctx, (const uint8_t *)&change, sizeof(change));

    /* Send corresponding time status message */
    send_time_status(model, ctx, false, opcode);
}

/* Scene Server & Scene Setup Server message handlers */
static void send_scene_status(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              bool publish)
{
    struct bt_mesh_scene_srv *srv = model->user_data;
    struct net_buf_simple *msg = NULL;
    uint8_t length = 1 + 6;

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

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_SCENE_STATUS);
    /**
     * If the message is sent as a reply to the Scene Recall message, the
     * Status Code field identifies the result of the related operation;
     * otherwise, the Status Code field shall be set to Success.
     */
    if (ctx->recv_op == BLE_MESH_MODEL_OP_SCENE_GET) {
        net_buf_simple_add_u8(msg, SCENE_SUCCESS);
    } else {
        net_buf_simple_add_u8(msg, srv->state->status_code);
    }
    net_buf_simple_add_le16(msg, srv->state->current_scene);
    /**
     * When an element is in the process of changing the Scene state, the
     * Target Scene field identifies the target Scene Number of the target
     * Scene state the element is to reach.
     * When an element is not in the process of changing the Scene state,
     * the Target Scene field shall be omitted.
     */
    if (srv->transition.counter) {
        bt_mesh_server_calc_remain_time(&srv->transition);
        net_buf_simple_add_le16(msg, srv->state->target_scene);
        net_buf_simple_add_u8(msg, srv->transition.remain_time);
    }

    if (publish == false) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        bt_mesh_free_buf(msg);
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    }
}

static void send_scene_register_status(struct bt_mesh_model *model,
                                       struct bt_mesh_msg_ctx *ctx,
                                       uint8_t status_code, bool publish)
{
    struct bt_mesh_scene_setup_srv *srv = model->user_data;
    struct scene_register *scene = NULL;
    struct net_buf_simple *msg = NULL;
    uint16_t total_len = 9U;
    int i;

    if (ctx == NULL && publish == false) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    if (publish == false) {
        msg = bt_mesh_alloc_buf(MIN(BLE_MESH_TX_SDU_MAX, BLE_MESH_SERVER_RSP_MAX_LEN));
        if (msg == NULL) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
    } else {
        msg = bt_mesh_server_get_pub_msg(model, 5);
        if (msg == NULL) {
            return;
        }
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_SCENE_REGISTER_STATUS);
    net_buf_simple_add_u8(msg, status_code);
    net_buf_simple_add_le16(msg, srv->state->current_scene);

    for (i = 0; i < srv->state->scene_count; i++) {
        scene = &srv->state->scenes[i];
        if (scene->scene_number != INVALID_SCENE_NUMBER) {
            total_len += SCENE_NUMBER_LEN;
            if ((publish == false && total_len > MIN(BLE_MESH_TX_SDU_MAX, BLE_MESH_SERVER_RSP_MAX_LEN)) ||
                (publish == true && total_len > msg->size + BLE_MESH_SERVER_TRANS_MIC_SIZE)) {
                /* Add this in case the message is too long */
                BT_WARN("Too large scene register status");
                break;
            }
            net_buf_simple_add_le16(msg, scene->scene_number);
        }
    }

    if (publish == false) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        bt_mesh_free_buf(msg);
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    }
}

static void scene_get(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct net_buf_simple *buf)
{
    struct bt_mesh_scene_srv *srv = model->user_data;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_time_scene_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_RECV_GET_MSG,
                                                model, ctx, NULL, 0);
        return;
    }

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_SCENE_GET:
        send_scene_status(model, ctx, false);
        return;
    case BLE_MESH_MODEL_OP_SCENE_REGISTER_GET:
        /**
         * When a Scene Server receives a Scene Register Get message, it shall
         * respond with a Scene Register Status message, setting the Status
         * Code field to Success.
         */
        send_scene_register_status(model, ctx, SCENE_SUCCESS, false);
        return;
    default:
        BT_WARN("Unknown Scene Get opcode 0x%04x", ctx->recv_op);
        return;
    }
}

void scene_publish(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t opcode)
{
    struct bt_mesh_scene_srv *srv = model->user_data;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    send_scene_status(model, ctx, true);
}

static void scene_recall(struct bt_mesh_model *model,
                         struct bt_mesh_msg_ctx *ctx,
                         struct net_buf_simple *buf)
{
    struct bt_mesh_scene_srv *srv = model->user_data;
    struct scene_register *scene = NULL;
    uint8_t tid = 0U, trans_time = 0U, delay = 0U;
    uint16_t scene_number = 0U;
    bool optional = false;
    int64_t now = 0;
    int i;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    scene_number = net_buf_simple_pull_le16(buf);
    if (scene_number == INVALID_SCENE_NUMBER) {
        BT_ERR("Invalid Scene Number 0x0000");
        return;
    }
    tid = net_buf_simple_pull_u8(buf);

    if (bt_mesh_server_get_optional(model, ctx, buf, &trans_time, &delay, &optional)) {
        return;
    }

    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_time_scene_server_recv_set_msg_t set = {
            .scene_recall.op_en = optional,
            .scene_recall.scene_number = scene_number,
            .scene_recall.tid = tid,
            .scene_recall.trans_time = trans_time,
            .scene_recall.delay = delay,
        };
        bt_mesh_time_scene_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_RECV_SET_MSG,
                                                model, ctx, (const uint8_t *)&set, sizeof(set));
        return;
    }

    for (i = 0; i < srv->state->scene_count; i++) {
        scene = &srv->state->scenes[i];
        if (scene->scene_number == scene_number) {
            break;
        }
    }
    if (i == srv->state->scene_count) {
        BT_WARN("Scene Number 0x%04x not exists", scene_number);
        srv->state->status_code = SCENE_NOT_FOUND;
        if (ctx->recv_op == BLE_MESH_MODEL_OP_SCENE_RECALL) {
            send_scene_status(model, ctx, false);
        }
        send_scene_status(model, ctx, true);
        return;
    }
    srv->state->status_code = SCENE_SUCCESS;

    /* Mesh Model Spec doesn't mention about this operation. */
    if (bt_mesh_is_server_recv_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now)) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_SCENE_RECALL) {
            send_scene_status(model, ctx, false);
        }
        send_scene_status(model, ctx, true);
        /* In this condition, no event will be callback to application layer */
        return;
    }

    bt_mesh_time_scene_server_lock();

    bt_mesh_server_stop_transition(&srv->transition);
    bt_mesh_server_update_last_msg(&srv->last, tid, ctx->addr, ctx->recv_dst, &now);

    srv->state->in_progress = false;
    /**
     * When the scene transition is not in progress, the value of the Target
     * Scene state shall be set to 0x0000.
     */
    srv->state->target_scene = INVALID_SCENE_NUMBER;

    /**
     * If the target state is equal to the current state, the transition
     * shall not be started and is considered complete.
     */
    if (srv->state->current_scene != scene_number) {
        scene_tt_values(srv, trans_time, delay);
    } else {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_SCENE_RECALL) {
            send_scene_status(model, ctx, false);
        }
        send_scene_status(model, ctx, true);

        bt_mesh_time_scene_server_state_change_t change = {
            .scene_recall.scene_number = scene_number,
        };
        bt_mesh_time_scene_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_STATE_CHANGE,
                                                model, ctx, (const uint8_t *)&change, sizeof(change));

        bt_mesh_time_scene_server_unlock();
        return;
    }

    /* Copy the ctx of the received message */
    if (srv->transition.timer.work.user_data) {
        memcpy(srv->transition.timer.work.user_data, ctx, sizeof(struct bt_mesh_msg_ctx));
    }

    /* For Instantaneous Transition */
    if (srv->transition.counter == 0U) {
        srv->state->current_scene = scene_number;
    } else {
        /**
         * When a scene transition is in progress, the value of the Current
         * Scene state shall be set to 0x0000.
         */
        srv->state->in_progress = true;
        srv->state->current_scene = INVALID_SCENE_NUMBER;
        srv->state->target_scene = scene_number;
    }

    srv->transition.just_started = true;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_SCENE_RECALL) {
        send_scene_status(model, ctx, false);
    }
    send_scene_status(model, ctx, true);

    bt_mesh_time_scene_server_unlock();

    bt_mesh_server_start_transition(&srv->transition);
}

static void scene_action(struct bt_mesh_model *model,
                         struct bt_mesh_msg_ctx *ctx,
                         struct net_buf_simple *buf)
{
    struct bt_mesh_scene_setup_srv *srv = model->user_data;
    struct scene_register *scene = NULL;
    uint16_t scene_number = 0U;
    int i;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    scene_number = net_buf_simple_pull_le16(buf);
    if (scene_number == INVALID_SCENE_NUMBER) {
        BT_ERR("Invalid Scene number 0x0000");
        return;
    }

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_SCENE_STORE:
    case BLE_MESH_MODEL_OP_SCENE_STORE_UNACK: {
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
            bt_mesh_time_scene_server_recv_set_msg_t set = {
                .scene_store.scene_number = scene_number,
            };
            bt_mesh_time_scene_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_RECV_SET_MSG,
                                                    model, ctx, (const uint8_t *)&set, sizeof(set));
            return;
        }
        /* Try to find a matching Scene Number */
        for (i = 0; i < srv->state->scene_count; i++) {
            scene = &srv->state->scenes[i];
            if (scene->scene_number == scene_number) {
                srv->state->status_code = SCENE_SUCCESS;
                srv->state->current_scene = scene_number;
                break;
            }
        }
        /* Try to find a unset entry if no matching Scene Number is found */
        if (i == srv->state->scene_count) {
            BT_DBG("No matching Scene Number 0x%04x found", scene_number);
            for (i = 0; i < srv->state->scene_count; i++) {
                scene = &srv->state->scenes[i];
                if (scene->scene_number == INVALID_SCENE_NUMBER) {
                    scene->scene_number = scene_number;
                    srv->state->status_code = SCENE_SUCCESS;
                    srv->state->current_scene = scene_number;
                    break;
                }
            }
            if (i == srv->state->scene_count) {
                BT_WARN("Scene Register is full!");
                srv->state->status_code = SCENE_REG_FULL;
                /* Get the Scene Number of the currently active scene */
                for (i = 0; i < srv->state->scene_count; i++) {
                    scene = &srv->state->scenes[i];
                    if (scene->scene_number != INVALID_SCENE_NUMBER) {
                        srv->state->current_scene = scene->scene_number;
                        break;
                    }
                }
                if (i == srv->state->scene_count) {
                    /* A value of 0x0000 when no scene is active */
                    srv->state->current_scene = INVALID_SCENE_NUMBER;
                }
            }
        }

        if (srv->state->in_progress == true) {
            /**
             * When the scene transition is in progress and a new Scene Number is
             * stored in the Scene Register as a result of Scene Store operation,
             * the Target Scene state shall be set to the new Scene Number.
             */
            srv->state->target_scene = scene_number;
        }
        if (srv->state->status_code == SCENE_SUCCESS) {
            bt_mesh_time_scene_server_state_change_t change = {
                .scene_store.scene_number = scene_number,
            };
            bt_mesh_time_scene_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_STATE_CHANGE,
                                                    model, ctx, (const uint8_t *)&change, sizeof(change));
        }
        break;
    }
    case BLE_MESH_MODEL_OP_SCENE_DELETE:
    case BLE_MESH_MODEL_OP_SCENE_DELETE_UNACK: {
        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
            bt_mesh_time_scene_server_recv_set_msg_t set = {
                .scene_delete.scene_number = scene_number,
            };
            bt_mesh_time_scene_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_RECV_SET_MSG,
                                                    model, ctx, (const uint8_t *)&set, sizeof(set));
            return;
        }
        for (i = 0; i < srv->state->scene_count; i++) {
            scene = &srv->state->scenes[i];
            if (scene->scene_number == scene_number) {
                scene->scene_number = INVALID_SCENE_NUMBER;
                break;
            }
        }
        if (i == srv->state->scene_count) {
            BT_WARN("Scene Number 0x%04x not exists", scene_number);
            /**
             * When a Scene Server receives a Scene Delete message with the Scene
             * Number value that does not match a Scene Number stored within the
             * Scene Register state, it shall respond with the Scene Register
             * Status message, setting the Status Code field to Success.
             */
        }
        srv->state->status_code = SCENE_SUCCESS;
        if (srv->state->current_scene == scene_number) {
            /**
             * When the Current Scene Number is deleted from a Scene Register state
             * as a result of Scene Delete operation, the Current Scene state shall
             * be set to 0x0000.
             */
            srv->state->current_scene = INVALID_SCENE_NUMBER;
        } else {
            /**
             * MMDL/SR/SCES/BV-02-C requires response with Current Scene set to the
             * latest Scene Number, but this is not mentioned in the spec.
             *
             * TODO: Do we need a timestamp for each newly added scene?
             */
            for (i = srv->state->scene_count; i > 0; i--) {
                scene = &srv->state->scenes[i - 1];
                if (scene->scene_number != INVALID_SCENE_NUMBER) {
                    srv->state->current_scene = scene->scene_number;
                    break;
                }
            }
            if (i == 0) {
                /* A value of 0x0000 when no scene is active */
                srv->state->current_scene = INVALID_SCENE_NUMBER;
            }
        }

        if (srv->state->target_scene == scene_number &&
                srv->state->in_progress == true) {
            /**
             * When the scene transition is in progress and the target Scene Number
             * is deleted from a Scene Register state as a result of Scene Delete
             * operation, the Target Scene state shall be set to 0x0000.
             */
            srv->state->target_scene = INVALID_SCENE_NUMBER;

            /**
             * When a scene is deleted when a scene transition to the deleted Scene
             * Number is in progress, the scene transition shall be terminated, but
             * individual model transitions shall not be terminated.
             */
            struct bt_mesh_scene_srv *scene_srv = NULL;
            struct bt_mesh_model *scene_model = NULL;

            scene_model = bt_mesh_model_find(bt_mesh_model_elem(model), BLE_MESH_MODEL_ID_SCENE_SRV);
            if (scene_model == NULL) {
                BT_ERR("Scene Server not present in the element");
                break;
            }

            scene_srv = scene_model->user_data;
            if (scene_srv == NULL || scene_srv->state == NULL) {
                BT_ERR("Invalid Scene Server user data");
                break;
            }

            if (srv->state != scene_srv->state) {
                /**
                 * Add this in case the Scene Setup Server is extending the Scene
                 * Server in another element.
                 */
                BT_WARN("Different Scene state in Scene Server & Scene Setup Server");
                break;
            }

            scene_srv->state->in_progress = false;
            bt_mesh_server_stop_transition(&scene_srv->transition);
        }

        bt_mesh_time_scene_server_state_change_t change = {
            .scene_delete.scene_number = scene_number,
        };
        bt_mesh_time_scene_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_STATE_CHANGE,
                                                model, ctx, (const uint8_t *)&change, sizeof(change));
        break;
    }
    default:
        BT_ERR("Unknown Scene setup action opcode 0x%04x", ctx->recv_op);
        return;
    }

    if (ctx->recv_op == BLE_MESH_MODEL_OP_SCENE_STORE ||
            ctx->recv_op == BLE_MESH_MODEL_OP_SCENE_DELETE) {
        send_scene_register_status(model, ctx, srv->state->status_code, false);
    }
    send_scene_register_status(model, NULL, srv->state->status_code, true);
}

static uint16_t get_schedule_reg_bit(struct bt_mesh_scheduler_state *state)
{
    uint16_t val = 0U;
    int i;

    for (i = 0; i < state->schedule_count; i++) {
        if (state->schedules[i].in_use) {
            val |= (1 << i);
        }
    }

    return val;
}

static uint64_t get_schedule_reg_state(struct bt_mesh_scheduler_state *state, uint8_t index)
{
    struct schedule_register *reg = &state->schedules[index];
    uint64_t val = 0U;

    val  = ((uint64_t)(reg->year) << 4) | index;
    val |= ((uint64_t)(reg->day) << 23) | ((uint64_t)(reg->month) << 11);
    val |= ((uint64_t)(reg->minute) << 33) | ((uint64_t)(reg->hour) << 28);
    val |= ((uint64_t)(reg->day_of_week) << 45) | ((uint64_t)(reg->second) << 39);
    val |= ((uint64_t)(reg->trans_time) << 56) | ((uint64_t)(reg->action) << 52);

    return val;
}

static void send_scheduler_act_status(struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx,
                                      uint8_t index)
{
    NET_BUF_SIMPLE_DEFINE(msg, 1 + 10 + BLE_MESH_SERVER_TRANS_MIC_SIZE);
    uint64_t value = 0U;

    bt_mesh_model_msg_init(&msg, BLE_MESH_MODEL_OP_SCHEDULER_ACT_STATUS);
    switch (model->id) {
    case BLE_MESH_MODEL_ID_SCHEDULER_SRV: {
        struct bt_mesh_scheduler_srv *srv = model->user_data;
        value = get_schedule_reg_state(srv->state, index);
        net_buf_simple_add_le32(&msg, (uint32_t)value);
        net_buf_simple_add_le32(&msg, (uint32_t)(value >> 32));
        net_buf_simple_add_le16(&msg, srv->state->schedules[index].scene_number);
        break;
    }
    case BLE_MESH_MODEL_ID_SCHEDULER_SETUP_SRV: {
        struct bt_mesh_scheduler_setup_srv *srv = model->user_data;
        value = get_schedule_reg_state(srv->state, index);
        net_buf_simple_add_le32(&msg, (uint32_t)value);
        net_buf_simple_add_le32(&msg, (uint32_t)(value >> 32));
        net_buf_simple_add_le16(&msg, srv->state->schedules[index].scene_number);
        break;
    }
    default:
        BT_ERR("Invalid Scheduler Server, model id 0x%04x", model->id);
        return;
    }

    BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, &msg, NULL, NULL));
}

static void scheduler_get(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    struct bt_mesh_scheduler_srv *srv = model->user_data;
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 2 + BLE_MESH_SERVER_TRANS_MIC_SIZE);

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_SCHEDULER_GET: {
        if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
            bt_mesh_time_scene_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_RECV_GET_MSG,
                                                    model, ctx, NULL, 0);
            return;
        }

        bt_mesh_model_msg_init(&msg, BLE_MESH_MODEL_OP_SCHEDULER_STATUS);
        net_buf_simple_add_le16(&msg, get_schedule_reg_bit(srv->state));
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, &msg, NULL, NULL));
        return;
    }
    case BLE_MESH_MODEL_OP_SCHEDULER_ACT_GET: {
        uint8_t index = net_buf_simple_pull_u8(buf);
        if (index > SCHEDULE_ENTRY_MAX_INDEX) {
            BT_ERR("Invalid Scheduler Register Entry index 0x%02x", index);
            return;
        }

        if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
            bt_mesh_time_scene_server_recv_get_msg_t get = {
                .scheduler_act_get.index = index,
            };
            bt_mesh_time_scene_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_RECV_GET_MSG,
                                                    model, ctx, (const uint8_t *)&get, sizeof(get));
            return;
        }

        send_scheduler_act_status(model, ctx, index);
        return;
    }
    default:
        BT_WARN("Unknown Scheduler Get opcode 0x%04x", ctx->recv_op);
        return;
    }
}

static void scheduler_act_set(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    /**
     * A recommended implementation of the Scheduler should calculate the value
     * of the TAI Seconds of the next scheduled event and put it in a queue of
     * scheduled events sorted by time. Every second, the first event in the
     * queue is compared with the value of the Time state. The first event is
     * executed if it is less than or equal to the Time state and then removed
     * from the queue. After execution, the Repeat Flag shall be checked, and
     * the next occurrence of the scheduled event is calculated and put in the
     * queue.
     */
    struct bt_mesh_scheduler_setup_srv *srv = model->user_data;
    uint8_t index = 0U, year = 0U, day = 0U, hour = 0U, minute = 0U,
         second = 0U, day_of_week = 0U, action = 0U, trans_time = 0U;
    uint16_t month = 0U, scene_number = 0U;
    uint64_t value = 0U;

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    value  = net_buf_simple_pull_le32(buf);
    value |= ((uint64_t)net_buf_simple_pull_le32(buf) << 32);

    index = value & BIT_MASK(4);
    year = (value >> 4) & BIT_MASK(7);
    month = (value >> 11) & BIT_MASK(12);
    day = (value >> 23) & BIT_MASK(5);
    hour = (value >> 28) & BIT_MASK(5);
    minute = (value >> 33) & BIT_MASK(6);
    second = (value >> 39) & BIT_MASK(6);
    day_of_week = (value >> 45) & BIT_MASK(7);
    action = (value >> 52) & BIT_MASK(4);
    trans_time = (value >> 56) & BIT_MASK(8);

    if (index > SCHEDULE_ENTRY_MAX_INDEX) {
        BT_ERR("Invalid Scheduler Register Entry index 0x%02x", index);
        return;
    }

    if (year > SCHEDULE_YEAR_ANY_YEAR) {
        BT_ERR("Invalid Scheduler Register year 0x%02x", year);
        return;
    }

    if (hour > SCHEDULE_HOUR_ONCE_A_DAY) {
        BT_ERR("Invalid Scheduler Register hour 0x%02x", hour);
        return;
    }

    if (action > SCHEDULE_ACT_SCENE_RECALL && action != SCHEDULE_ACT_NO_ACTION) {
        BT_ERR("Invalid Scheduler Register action 0x%02x", action);
        return;
    }

    scene_number = net_buf_simple_pull_le16(buf);

    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_time_scene_server_recv_set_msg_t set = {
            .scheduler_act_set.index = index,
            .scheduler_act_set.year = year,
            .scheduler_act_set.month = month,
            .scheduler_act_set.day = day,
            .scheduler_act_set.hour = hour,
            .scheduler_act_set.minute = minute,
            .scheduler_act_set.second = second,
            .scheduler_act_set.day_of_week = day_of_week,
            .scheduler_act_set.action = action,
            .scheduler_act_set.trans_time = trans_time,
            .scheduler_act_set.scene_number = scene_number,
        };
        bt_mesh_time_scene_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_RECV_SET_MSG,
                                                model, ctx, (const uint8_t *)&set, sizeof(set));
        return;
    }

    srv->state->schedules[index].in_use = true;
    srv->state->schedules[index].year = year;
    srv->state->schedules[index].month = month;
    srv->state->schedules[index].day = day;
    srv->state->schedules[index].hour = hour;
    srv->state->schedules[index].minute = minute;
    srv->state->schedules[index].second = second;
    srv->state->schedules[index].day_of_week = day_of_week;
    srv->state->schedules[index].action = action;
    srv->state->schedules[index].trans_time = trans_time;
    srv->state->schedules[index].scene_number = scene_number;

    bt_mesh_time_scene_server_state_change_t change = {
        .scheduler_act_set.index = index,
        .scheduler_act_set.year = year,
        .scheduler_act_set.month = month,
        .scheduler_act_set.day = day,
        .scheduler_act_set.hour = hour,
        .scheduler_act_set.minute = minute,
        .scheduler_act_set.second = second,
        .scheduler_act_set.day_of_week = day_of_week,
        .scheduler_act_set.action = action,
        .scheduler_act_set.trans_time = trans_time,
        .scheduler_act_set.scene_number = scene_number,
    };
    bt_mesh_time_scene_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_STATE_CHANGE,
                                            model, ctx, (const uint8_t *)&change, sizeof(change));

    if (ctx->recv_op == BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET) {
        send_scheduler_act_status(model, ctx, index);
    }
}

/* message handlers (End) */

/* Mapping of message handlers for Time Server (0x1200) */
const struct bt_mesh_model_op bt_mesh_time_srv_op[] = {
    { BLE_MESH_MODEL_OP_TIME_GET,          0, time_get },
    { BLE_MESH_MODEL_OP_TIME_STATUS,       5, time_get },
    { BLE_MESH_MODEL_OP_TIME_ZONE_GET,     0, time_get },
    { BLE_MESH_MODEL_OP_TAI_UTC_DELTA_GET, 0, time_get },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Time Setup Server (0x1201) */
const struct bt_mesh_model_op bt_mesh_time_setup_srv_op[] = {
    { BLE_MESH_MODEL_OP_TIME_SET,          10, time_set },
    { BLE_MESH_MODEL_OP_TIME_ZONE_SET,      6, time_set },
    { BLE_MESH_MODEL_OP_TAI_UTC_DELTA_SET,  7, time_set },
    { BLE_MESH_MODEL_OP_TIME_ROLE_GET,      0, time_get },
    { BLE_MESH_MODEL_OP_TIME_ROLE_SET,      1, time_set },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Scene Server (0x1203) */
const struct bt_mesh_model_op bt_mesh_scene_srv_op[] = {
    { BLE_MESH_MODEL_OP_SCENE_GET,          0, scene_get    },
    { BLE_MESH_MODEL_OP_SCENE_RECALL,       3, scene_recall },
    { BLE_MESH_MODEL_OP_SCENE_RECALL_UNACK, 3, scene_recall },
    { BLE_MESH_MODEL_OP_SCENE_REGISTER_GET, 0, scene_get    },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Scene Setup Server (0x1204) */
const struct bt_mesh_model_op bt_mesh_scene_setup_srv_op[] = {
    { BLE_MESH_MODEL_OP_SCENE_STORE,        2, scene_action },
    { BLE_MESH_MODEL_OP_SCENE_STORE_UNACK,  2, scene_action },
    { BLE_MESH_MODEL_OP_SCENE_DELETE,       2, scene_action },
    { BLE_MESH_MODEL_OP_SCENE_DELETE_UNACK, 2, scene_action },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Scheduler Server (0x1206) */
const struct bt_mesh_model_op bt_mesh_scheduler_srv_op[] = {
    { BLE_MESH_MODEL_OP_SCHEDULER_GET,     0, scheduler_get },
    { BLE_MESH_MODEL_OP_SCHEDULER_ACT_GET, 1, scheduler_get },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Scheduler Setup Server (0x1207) */
const struct bt_mesh_model_op bt_mesh_scheduler_setup_srv_op[] = {
    { BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET,       10, scheduler_act_set },
    { BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET_UNACK, 10, scheduler_act_set },
    BLE_MESH_MODEL_OP_END,
};

static int check_scene_server_init(struct bt_mesh_scenes_state *state)
{
    int i;

    if (state->scene_count == 0U || state->scenes == NULL) {
        BT_ERR("Invalid Scene state");
        return -EINVAL;
    }

    for (i = 0; i < state->scene_count; i++) {
        if (state->scenes[i].scene_value == NULL) {
            BT_ERR("Invalid Scene value, index %d", i);
            return -EINVAL;
        }
    }

    return 0;
}

static int time_scene_server_init(struct bt_mesh_model *model)
{
    if (model->user_data == NULL) {
        BT_ERR("Invalid Time Scene Server user data, model id 0x%04x", model->id);
        return -EINVAL;
    }

    switch (model->id) {
    case BLE_MESH_MODEL_ID_TIME_SRV: {
        struct bt_mesh_time_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Time State");
            return -EINVAL;
        }

        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_TIME_SETUP_SRV: {
        struct bt_mesh_time_setup_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Time State");
            return -EINVAL;
        }

        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_SCENE_SRV: {
        struct bt_mesh_scene_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Scene State");
            return -EINVAL;
        }

        if (check_scene_server_init(srv->state)) {
            return -EINVAL;
        }

        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_AUTO_RSP) {
            bt_mesh_server_alloc_ctx(&srv->transition.timer.work);
            k_delayed_work_init(&srv->transition.timer, scene_recall_work_handler);
        }
        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_SCENE_SETUP_SRV: {
        struct bt_mesh_scene_setup_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Scene State");
            return -EINVAL;
        }

        if (check_scene_server_init(srv->state)) {
            return -EINVAL;
        }

        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_SCHEDULER_SRV: {
        struct bt_mesh_scheduler_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Scheduler State");
            return -EINVAL;
        }

        if (srv->state->schedule_count == 0U || srv->state->schedules == NULL) {
            BT_ERR("Invalid Register Schedule");
            return -EINVAL;
        }

        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_SCHEDULER_SETUP_SRV: {
        struct bt_mesh_scheduler_setup_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Scheduler State");
            return -EINVAL;
        }

        if (srv->state->schedule_count == 0U || srv->state->schedules == NULL) {
            BT_ERR("Invalid Register Schedule");
            return -EINVAL;
        }

        srv->model = model;
        break;
    }
    default:
        BT_WARN("Unknown Time Scene Server, model id 0x%04x", model->id);
        return -EINVAL;
    }

    bt_mesh_mutex_create(&time_scene_server_lock);

    return 0;
}

static int time_srv_init(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Time Server has no publication support");
        return -EINVAL;
    }

    /**
     * When this model is present on an Element, the corresponding Time Setup
     * Server model shall also be present.
     */
    struct bt_mesh_elem *element = bt_mesh_model_elem(model);
    if (bt_mesh_model_find(element, BLE_MESH_MODEL_ID_TIME_SETUP_SRV) == NULL) {
        BT_WARN("Time Setup Server not present");
        /* Just give a warning here, continue with the initialization */
    }
    return time_scene_server_init(model);
}

static int time_setup_srv_init(struct bt_mesh_model *model)
{
    /* This model does not support subscribing nor publishing */
    if (model->pub) {
        BT_ERR("Time Setup Server shall not support publication");
        return -EINVAL;
    }

    return time_scene_server_init(model);
}

static int scene_srv_init(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Scene Server has no publication support");
        return -EINVAL;
    }

    /* The model may be present only on the Primary element of a node. */
    if (!bt_mesh_model_in_primary(model)) {
        BT_WARN("Scene Server not on the Primary element");
        /* Just give a warning here, continue with the initialization */
    }
    /**
     * When this model is present on an Element, the corresponding Scene Setup
     * Server model shall also be present.
     */
    struct bt_mesh_elem *element = bt_mesh_model_elem(model);
    if (bt_mesh_model_find(element, BLE_MESH_MODEL_ID_SCENE_SETUP_SRV) == NULL) {
        BT_WARN("Scene Setup Server not present");
        /* Just give a warning here, continue with the initialization */
    }
    return time_scene_server_init(model);
}

static int scene_setup_srv_init(struct bt_mesh_model *model)
{
    /* The model may be present only on the Primary element of a node. */
    if (!bt_mesh_model_in_primary(model)) {
        BT_WARN("Scene Setup Server not on the Primary element");
        /* Just give a warning here, continue with the initialization */
    }
    return time_scene_server_init(model);
}

static int scheduler_srv_init(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Scheduler Server has no publication support");
        return -EINVAL;
    }

    /* The model may be present only on the Primary element of a node. */
    if (!bt_mesh_model_in_primary(model)) {
        BT_WARN("Scheduler Server not on the Primary element");
        /* Just give a warning here, continue with the initialization */
    }
    /**
     * When this model is present on an Element, the corresponding Scheduler
     * Setup Server model shall also be present. The model requires the Time
     * Server model shall be present on the element.
     */
    struct bt_mesh_elem *element = bt_mesh_model_elem(model);
    if (bt_mesh_model_find(element, BLE_MESH_MODEL_ID_SCHEDULER_SETUP_SRV) == NULL) {
        BT_WARN("Scheduler Setup Server not present");
        /* Just give a warning here, continue with the initialization */
    }
    if (bt_mesh_model_find(element, BLE_MESH_MODEL_ID_TIME_SRV) == NULL) {
        BT_WARN("Time Server not present");
        /* Just give a warning here, continue with the initialization */
    }
    return time_scene_server_init(model);
}

static int scheduler_setup_srv_init(struct bt_mesh_model *model)
{
    /* The model may be present only on the Primary element of a node. */
    if (!bt_mesh_model_in_primary(model)) {
        BT_WARN("Scheduler Setup Server not on the Primary element");
        /* Just give a warning here, continue with the initialization */
    }
    return time_scene_server_init(model);
}

#if CONFIG_BLE_MESH_DEINIT
static int time_scene_server_deinit(struct bt_mesh_model *model)
{
    if (model->user_data == NULL) {
        BT_ERR("Invalid Time Scene Server user data, model id 0x%04x", model->id);
        return -EINVAL;
    }

    switch (model->id) {
    case BLE_MESH_MODEL_ID_SCENE_SRV: {
        struct bt_mesh_scene_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Scene State");
            return -EINVAL;
        }

        if (check_scene_server_init(srv->state)) {
            return -EINVAL;
        }

        if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_AUTO_RSP) {
            bt_mesh_server_free_ctx(&srv->transition.timer.work);
            k_delayed_work_free(&srv->transition.timer);
        }
        break;
    }
    case BLE_MESH_MODEL_ID_TIME_SRV:
    case BLE_MESH_MODEL_ID_TIME_SETUP_SRV:
    case BLE_MESH_MODEL_ID_SCENE_SETUP_SRV:
    case BLE_MESH_MODEL_ID_SCHEDULER_SRV:
    case BLE_MESH_MODEL_ID_SCHEDULER_SETUP_SRV:
        break;
    default:
        BT_WARN("Unknown Time Scene Server, model id 0x%04x", model->id);
        return -EINVAL;
    }

    bt_mesh_mutex_free(&time_scene_server_lock);

    return 0;
}

static int time_srv_deinit(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Time Server has no publication support");
        return -EINVAL;
    }

    return time_scene_server_deinit(model);
}

static int time_setup_srv_deinit(struct bt_mesh_model *model)
{
    if (model->pub) {
        BT_ERR("Time Setup Server shall not support publication");
        return -EINVAL;
    }

    return time_scene_server_deinit(model);
}

static int scene_srv_deinit(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Scene Server has no publication support");
        return -EINVAL;
    }

    return time_scene_server_deinit(model);
}

static int scene_setup_srv_deinit(struct bt_mesh_model *model)
{
    return time_scene_server_deinit(model);
}

static int scheduler_srv_deinit(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Scheduler Server has no publication support");
        return -EINVAL;
    }

    return time_scene_server_deinit(model);
}

static int scheduler_setup_srv_deinit(struct bt_mesh_model *model)
{
    return time_scene_server_deinit(model);
}
#endif /* CONFIG_BLE_MESH_DEINIT */

const struct bt_mesh_model_cb bt_mesh_time_srv_cb = {
    .init = time_srv_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = time_srv_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

const struct bt_mesh_model_cb bt_mesh_time_setup_srv_cb = {
    .init = time_setup_srv_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = time_setup_srv_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

const struct bt_mesh_model_cb bt_mesh_scene_srv_cb = {
    .init = scene_srv_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = scene_srv_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

const struct bt_mesh_model_cb bt_mesh_scene_setup_srv_cb = {
    .init = scene_setup_srv_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = scene_setup_srv_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

const struct bt_mesh_model_cb bt_mesh_scheduler_srv_cb = {
    .init = scheduler_srv_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = scheduler_srv_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

const struct bt_mesh_model_cb bt_mesh_scheduler_setup_srv_cb = {
    .init = scheduler_setup_srv_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = scheduler_setup_srv_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

#endif /* CONFIG_BLE_MESH_TIME_SCENE_SERVER */

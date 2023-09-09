/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include "btc_ble_mesh_sensor_model.h"

#include "mesh/config.h"
#include "access.h"
#include "transport.h"
#include "mesh/model_opcode.h"
#include "mesh/state_transition.h"
#include "mesh/device_property.h"

#if CONFIG_BLE_MESH_SENSOR_SERVER

static void update_sensor_periodic_pub(struct bt_mesh_model *model, uint16_t prop_id);

/* message handlers (Start) */

/* Sensor Server & Sensor Setup Server message handlers */
static void send_sensor_descriptor_status(struct bt_mesh_model *model,
                                          struct bt_mesh_msg_ctx *ctx,
                                          uint16_t prop_id, bool get_all)
{
    struct bt_mesh_sensor_srv *srv = model->user_data;
    struct bt_mesh_sensor_state *state = NULL;
    struct net_buf_simple *msg = NULL;
    uint16_t total_len = 5U;
    int i;

    msg = bt_mesh_alloc_buf(MIN(BLE_MESH_TX_SDU_MAX, BLE_MESH_SERVER_RSP_MAX_LEN));
    if (msg == NULL) {
        BT_ERR("%s, Out of memory", __func__);
        return;
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_STATUS);

    if (get_all == true) {
        for (i = 0; i < srv->state_count; i++) {
            state = &srv->states[i];
            if (state->sensor_property_id != INVALID_SENSOR_PROPERTY_ID) {
                total_len += SENSOR_DESCRIPTOR_LEN;
                if (total_len > MIN(BLE_MESH_TX_SDU_MAX, BLE_MESH_SERVER_RSP_MAX_LEN)) {
                    /* Add this in case the message is too long */
                    BT_WARN("Too large sensor descriptor status");
                    break;
                }

                net_buf_simple_add_le16(msg, state->sensor_property_id);
                net_buf_simple_add_le32(msg, (state->descriptor.sample_function << 24) |
                                        (state->descriptor.negative_tolerance << 12) |
                                        (state->descriptor.positive_tolerance));
                net_buf_simple_add_u8(msg, state->descriptor.measure_period);
                net_buf_simple_add_u8(msg, state->descriptor.update_interval);
            }
        }
    } else {
        for (i = 0; i < srv->state_count; i++) {
            state = &srv->states[i];
            if (state->sensor_property_id != INVALID_SENSOR_PROPERTY_ID &&
                state->sensor_property_id == prop_id) {
                total_len += SENSOR_DESCRIPTOR_LEN;
                if (total_len > MIN(BLE_MESH_TX_SDU_MAX, BLE_MESH_SERVER_RSP_MAX_LEN)) {
                    /* Add this in case the message is too long */
                    BT_WARN("Too large sensor descriptor status");
                    break;
                }
                net_buf_simple_add_le16(msg, state->sensor_property_id);
                net_buf_simple_add_le32(msg, (state->descriptor.sample_function << 24) |
                                             (state->descriptor.negative_tolerance << 12) |
                                             (state->descriptor.positive_tolerance));
                net_buf_simple_add_u8(msg, state->descriptor.measure_period);
                net_buf_simple_add_u8(msg, state->descriptor.update_interval);
                break;
            }
        }
        if (i == srv->state_count) {
            BT_WARN("Sensor Property ID 0x%04x not exists", prop_id);
            net_buf_simple_add_le16(msg, prop_id);
        }
    }

    BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));

    bt_mesh_free_buf(msg);
}

static void send_sensor_data_status(struct bt_mesh_model *model,
                                    struct bt_mesh_msg_ctx *ctx,
                                    uint16_t prop_id, bool get_all)
{
    struct bt_mesh_sensor_srv *srv = model->user_data;
    struct bt_mesh_sensor_state *state = NULL;
    struct net_buf_simple *msg = NULL;
    uint16_t total_len = 5U;
    int i;

    msg = bt_mesh_alloc_buf(MIN(BLE_MESH_TX_SDU_MAX, BLE_MESH_SERVER_RSP_MAX_LEN));
    if (msg == NULL) {
        BT_ERR("%s, Out of memory", __func__);
        return;
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_SENSOR_STATUS);

    if (get_all == true) {
        for (i = 0; i < srv->state_count; i++) {
            state = &srv->states[i];
            if (state->sensor_property_id != INVALID_SENSOR_PROPERTY_ID) {
                uint8_t mpid_len = (state->sensor_data.format == SENSOR_DATA_FORMAT_A) ?
                                    SENSOR_DATA_FORMAT_A_MPID_LEN : SENSOR_DATA_FORMAT_B_MPID_LEN;
                total_len += (mpid_len + (state->sensor_data.raw_value ?
                                          state->sensor_data.raw_value->len : 0));
                if (total_len > MIN(BLE_MESH_TX_SDU_MAX, BLE_MESH_SERVER_RSP_MAX_LEN)) {
                    /* Add this in case the message is too long */
                    BT_WARN("Too large sensor status");
                    break;
                }

                if (state->sensor_data.format == SENSOR_DATA_FORMAT_A) {
                    uint16_t mpid = ((state->sensor_property_id & BIT_MASK(11)) << 5) |
                                    ((state->sensor_data.length & BIT_MASK(4)) << 1) |
                                    state->sensor_data.format;
                    net_buf_simple_add_le16(msg, mpid);
                } else if (state->sensor_data.format == SENSOR_DATA_FORMAT_B) {
                    uint8_t mpid = (state->sensor_data.length << 1) | state->sensor_data.format;
                    net_buf_simple_add_u8(msg, mpid);
                    net_buf_simple_add_le16(msg, state->sensor_property_id);
                }
                if (state->sensor_data.raw_value) {
                    net_buf_simple_add_mem(msg, state->sensor_data.raw_value->data, state->sensor_data.raw_value->len);
                }
            }
        }
    } else {
        for (i = 0; i < srv->state_count; i++) {
            state = &srv->states[i];
            if (state->sensor_property_id != INVALID_SENSOR_PROPERTY_ID &&
                state->sensor_property_id == prop_id) {
                uint8_t mpid_len = (state->sensor_data.format == SENSOR_DATA_FORMAT_A) ?
                                    SENSOR_DATA_FORMAT_A_MPID_LEN : SENSOR_DATA_FORMAT_B_MPID_LEN;
                total_len += (mpid_len + (state->sensor_data.raw_value ?
                                          state->sensor_data.raw_value->len : 0));
                if (total_len > MIN(BLE_MESH_TX_SDU_MAX, BLE_MESH_SERVER_RSP_MAX_LEN)) {
                    /* Add this in case the message is too long */
                    BT_WARN("Too large sensor status");
                    break;
                }
                if (state->sensor_data.format == SENSOR_DATA_FORMAT_A) {
                    uint16_t mpid = ((state->sensor_property_id & BIT_MASK(11)) << 5) |
                                    ((state->sensor_data.length & BIT_MASK(4)) << 1) |
                                    state->sensor_data.format;
                    net_buf_simple_add_le16(msg, mpid);
                } else if (state->sensor_data.format == SENSOR_DATA_FORMAT_B) {
                    uint8_t mpid = (state->sensor_data.length << 1) | state->sensor_data.format;
                    net_buf_simple_add_u8(msg, mpid);
                    net_buf_simple_add_le16(msg, state->sensor_property_id);
                }
                if (state->sensor_data.raw_value) {
                    net_buf_simple_add_mem(msg, state->sensor_data.raw_value->data,
                                           state->sensor_data.raw_value->len);
                }
                break;
            }
        }
        if (i == srv->state_count) {
            BT_WARN("Sensor Property ID 0x%04x not exists", prop_id);
            uint8_t mpid = (SENSOR_DATA_ZERO_LEN << 1) | SENSOR_DATA_FORMAT_B;
            net_buf_simple_add_u8(msg, mpid);
            net_buf_simple_add_le16(msg, prop_id);
        }
    }

    BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));

    bt_mesh_free_buf(msg);
}

static void send_sensor_cadence_status(struct bt_mesh_model *model,
                                       struct bt_mesh_msg_ctx *ctx,
                                       uint16_t prop_id, bool publish)
{
    struct bt_mesh_sensor_setup_srv *srv = model->user_data;
    struct bt_mesh_sensor_state *state = NULL;
    struct net_buf_simple *msg = NULL;
    uint16_t length = 0U;
    int i;

    for (i = 0; i < srv->state_count; i++) {
        state = &srv->states[i];
        if (state->sensor_property_id != INVALID_SENSOR_PROPERTY_ID &&
            state->sensor_property_id == prop_id && state->cadence) {
            length = SENSOR_PROPERTY_ID_LEN + 1 + 1;
            if (state->cadence->trigger_delta_down) {
                if (state->cadence->trigger_type == SENSOR_STATUS_TRIGGER_TYPE_CHAR) {
                    length += state->cadence->trigger_delta_down->len;
                } else {
                    length += SENSOR_STATUS_TRIGGER_UINT16_LEN;
                }
            }
            if (state->cadence->trigger_delta_up) {
                if (state->cadence->trigger_type == SENSOR_STATUS_TRIGGER_TYPE_CHAR) {
                    length += state->cadence->trigger_delta_up->len;
                } else {
                    length += SENSOR_STATUS_TRIGGER_UINT16_LEN;
                }
            }
            if (state->cadence->fast_cadence_low) {
                length += state->cadence->fast_cadence_low->len;
            }
            if (state->cadence->fast_cadence_high) {
                length += state->cadence->fast_cadence_high->len;
            }
            break;
        }
    }
    if (i == srv->state_count) {
        BT_WARN("Sensor Property ID 0x%04x not exists", prop_id);
        length = SENSOR_PROPERTY_ID_LEN;
    }

    if (publish == false) {
        msg = bt_mesh_alloc_buf(1 + length + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        if (msg == NULL) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
    } else {
        msg = bt_mesh_server_get_pub_msg(model, 1 + length);
        if (msg == NULL) {
            return;
        }
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_SENSOR_CADENCE_STATUS);
    net_buf_simple_add_le16(msg, prop_id);
    if (i != srv->state_count) {
        if (state->cadence) {
            net_buf_simple_add_u8(msg, (state->cadence->trigger_type << 7) |
                                        state->cadence->period_divisor);
            if (state->cadence->trigger_delta_down) {
                if (state->cadence->trigger_type == SENSOR_STATUS_TRIGGER_TYPE_CHAR) {
                    net_buf_simple_add_mem(msg, state->cadence->trigger_delta_down->data,
                                           state->cadence->trigger_delta_down->len);
                } else {
                    net_buf_simple_add_mem(msg, state->cadence->trigger_delta_down->data,
                                           SENSOR_STATUS_TRIGGER_UINT16_LEN);
                }
            }
            if (state->cadence->trigger_delta_up) {
                if (state->cadence->trigger_type == SENSOR_STATUS_TRIGGER_TYPE_CHAR) {
                    net_buf_simple_add_mem(msg, state->cadence->trigger_delta_up->data,
                                           state->cadence->trigger_delta_up->len);
                } else {
                    net_buf_simple_add_mem(msg, state->cadence->trigger_delta_up->data,
                                           SENSOR_STATUS_TRIGGER_UINT16_LEN);
                }
            }
            net_buf_simple_add_u8(msg, state->cadence->min_interval);
            if (state->cadence->fast_cadence_low) {
                net_buf_simple_add_mem(msg, state->cadence->fast_cadence_low->data,
                                       state->cadence->fast_cadence_low->len);
            }
            if (state->cadence->fast_cadence_high) {
                net_buf_simple_add_mem(msg, state->cadence->fast_cadence_high->data,
                                       state->cadence->fast_cadence_high->len);
            }
        }
    }

    if (publish == false) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        bt_mesh_free_buf(msg);
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    }
}

static void send_sensor_settings_status(struct bt_mesh_model *model,
                                        struct bt_mesh_msg_ctx *ctx,
                                        uint16_t prop_id)
{
    struct bt_mesh_sensor_setup_srv *srv = model->user_data;
    struct bt_mesh_sensor_state *state = NULL;
    struct sensor_setting *item = NULL;
    struct net_buf_simple *msg = NULL;
    uint16_t total_len = 7U;
    int i, j;

    msg = bt_mesh_alloc_buf(MIN(BLE_MESH_TX_SDU_MAX, BLE_MESH_SERVER_RSP_MAX_LEN));
    if (msg == NULL) {
        BT_ERR("%s, Out of memory", __func__);
        return;
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_SENSOR_SETTINGS_STATUS);
    net_buf_simple_add_le16(msg, prop_id);

    for (i = 0; i < srv->state_count; i++) {
        state = &srv->states[i];
        if (state->sensor_property_id != INVALID_SENSOR_PROPERTY_ID &&
                state->sensor_property_id == prop_id &&
                state->setting_count && state->settings) {
            for (j = 0; j < state->setting_count; j++) {
                item = &state->settings[j];
                if (item->property_id != INVALID_SENSOR_SETTING_PROPERTY_ID) {
                    total_len += SENSOR_SETTING_PROPERTY_ID_LEN;
                    if (total_len > MIN(BLE_MESH_TX_SDU_MAX, BLE_MESH_SERVER_RSP_MAX_LEN)) {
                        /* Add this in case the message is too long */
                        BT_WARN("Too large sensor settings status");
                        break;
                    }
                    net_buf_simple_add_le16(msg, item->property_id);
                }
            }
            break;
        }
    }
    if (i == srv->state_count) {
        BT_WARN("Sensor Property ID 0x%04x not exists", prop_id);
    }

    BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));

    bt_mesh_free_buf(msg);
}

static struct sensor_setting *find_sensor_setting(struct bt_mesh_model *model,
                                                  uint16_t prop_id, uint16_t set_prop_id)
{
    struct bt_mesh_sensor_setup_srv *srv = model->user_data;
    struct bt_mesh_sensor_state *state = NULL;
    struct sensor_setting *item = NULL;
    int i, j;

    for (i = 0; i < srv->state_count; i++) {
        state = &srv->states[i];
        if (state->sensor_property_id != INVALID_SENSOR_PROPERTY_ID &&
            state->sensor_property_id == prop_id &&
            state->setting_count && state->settings) {
            for (j = 0; j < state->setting_count; j++) {
                item = &state->settings[j];
                if (item->property_id != INVALID_SENSOR_SETTING_PROPERTY_ID &&
                    item->property_id == set_prop_id) {
                    return item;
                }
            }
        }
    }

    return NULL;
}

static void send_sensor_setting_status(struct bt_mesh_model *model,
                                       struct bt_mesh_msg_ctx *ctx, uint16_t prop_id,
                                       uint16_t set_prop_id, bool publish)
{
    struct sensor_setting *item = NULL;
    struct net_buf_simple *msg = NULL;
    uint16_t length = 0U;

    item = find_sensor_setting(model, prop_id, set_prop_id);
    if (item) {
        length = SENSOR_PROPERTY_ID_LEN + SENSOR_SETTING_PROPERTY_ID_LEN +
                 SENSOR_SETTING_ACCESS_LEN + (item->raw ? item->raw->len : 0);
    } else {
        /* If the message is sent as a response to the Sensor Setting Get message or
         * a Sensor Setting Set message with an unknown Sensor Property ID field or
         * an unknown Sensor Setting Property ID field, the Sensor Setting Access
         * field and the Sensor Setting Raw field shall be omitted.
         */
        BT_WARN("Sensor Setting not found, 0x%04x, 0x%04x", prop_id, set_prop_id);
        length = SENSOR_PROPERTY_ID_LEN + SENSOR_SETTING_PROPERTY_ID_LEN;
    }

    if (publish == false) {
        msg = bt_mesh_alloc_buf(1 + length + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        if (msg == NULL) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
    } else {
        msg = bt_mesh_server_get_pub_msg(model, 1 + length);
        if (msg == NULL) {
            return;
        }
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_SENSOR_SETTING_STATUS);
    net_buf_simple_add_le16(msg, prop_id);
    net_buf_simple_add_le16(msg, set_prop_id);
    if (item) {
        /**
         * If the message is sent as a response to the Sensor Setting Set message with
         * a Sensor Setting Property ID field that identifies an existing Sensor Setting,
         * and the value of the Sensor Setting Access state is 0x01 (can be read), the
         * Sensor Setting Property ID field shall be set to the value of the Sensor
         * Setting Property ID field of the incoming message, the Sensor Setting Access
         * field shall be set to the value of the Sensor Setting Access state field, and
         * the Sensor Setting Raw field shall be omitted.
         *
         * TODO: What if the Sensor Setting Access is Prohibited?
         */
        net_buf_simple_add_u8(msg, item->access);
        if (ctx->recv_op != BLE_MESH_MODEL_OP_SENSOR_SETTING_SET ||
                item->access == SENSOR_SETTING_ACCESS_READ_WRITE) {
            if (item->raw) {
                net_buf_simple_add_mem(msg, item->raw->data, item->raw->len);
            }
        }
    }

    if (publish == false) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        bt_mesh_free_buf(msg);
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    }
}

static void send_sensor_column_status(struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx,
                                      struct net_buf_simple *buf, uint16_t prop_id)
{
    struct bt_mesh_sensor_srv *srv = model->user_data;
    struct bt_mesh_sensor_state *state = NULL;
    struct net_buf_simple *msg = NULL;
    bool optional = false;
    uint16_t length = 0U;
    int i;

    for (i = 0; i < srv->state_count; i++) {
        state = &srv->states[i];
        if (state->sensor_property_id != INVALID_SENSOR_PROPERTY_ID &&
            state->sensor_property_id == prop_id) {
            length = SENSOR_PROPERTY_ID_LEN;
            if (state->series_column.raw_value_x) {
                length += state->series_column.raw_value_x->len;
            }
            /**
             * TODO: column width & raw value y in Sensor Column Status are optional,
             * here we need to add some conditions to decide whether put these two
             * in the status message.
             */
            if (optional) {
                if (state->series_column.column_width) {
                    length += state->series_column.column_width->len;
                }
                if (state->series_column.raw_value_y) {
                    length += state->series_column.raw_value_y->len;
                }
            }
            break;
        }
    }
    if (i == srv->state_count) {
        BT_WARN("Sensor Property ID 0x%04x not exists", prop_id);
        length = SENSOR_PROPERTY_ID_LEN;
    }

    msg = bt_mesh_alloc_buf(1 + length + BLE_MESH_SERVER_TRANS_MIC_SIZE);
    if (msg == NULL) {
        BT_ERR("%s, Out of memory", __func__);
        return;
    }

    /**
     * TODO: Sensor Column Get contains Raw Value X which identifies a column,
     * we need to use this value to decide the column.
     */

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_SENSOR_COLUMN_STATUS);
    net_buf_simple_add_le16(msg, prop_id);
    if (i != srv->state_count) {
        if (state->series_column.raw_value_x) {
            net_buf_simple_add_mem(msg, state->series_column.raw_value_x->data,
                                   state->series_column.raw_value_x->len);
        }
        if (optional) {
            if (state->series_column.column_width) {
                net_buf_simple_add_mem(msg, state->series_column.column_width->data,
                                       state->series_column.column_width->len);
            }
            if (state->series_column.raw_value_y) {
                net_buf_simple_add_mem(msg, state->series_column.raw_value_y->data,
                                       state->series_column.raw_value_y->len);
            }
        }
    }

    BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));

    bt_mesh_free_buf(msg);
}

static void send_sensor_series_status(struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx,
                                      struct net_buf_simple *buf, uint16_t prop_id)
{
    struct bt_mesh_sensor_srv *srv = model->user_data;
    struct bt_mesh_sensor_state *state = NULL;
    struct net_buf_simple *msg = NULL;
    bool optional = false;
    uint16_t length = 0U;
    int i;

    for (i = 0; i < srv->state_count; i++) {
        state = &srv->states[i];
        if (state->sensor_property_id != INVALID_SENSOR_PROPERTY_ID &&
            state->sensor_property_id == prop_id) {
            length = SENSOR_PROPERTY_ID_LEN;
            /* TODO: raw value x, column width & raw value y in Sensor Series
             * Status are optional, here we need to add some conditions to
             * decide whether put these three in the status message.
             */
            if (optional) {
                if (state->series_column.raw_value_x) {
                    length += state->series_column.raw_value_x->len;
                }
                if (state->series_column.column_width) {
                    length += state->series_column.column_width->len;
                }
                if (state->series_column.raw_value_y) {
                    length += state->series_column.raw_value_y->len;
                }
            }
            break;
        }
    }
    if (i == srv->state_count) {
        BT_WARN("Sensor Property ID 0x%04x not exists", prop_id);
        length = SENSOR_PROPERTY_ID_LEN;
    }

    msg = bt_mesh_alloc_buf(1 + length + BLE_MESH_SERVER_TRANS_MIC_SIZE);
    if (msg == NULL) {
        BT_ERR("%s, Out of memory", __func__);
        return;
    }

    /**
     * TODO: Sensor Series Get may contain Raw Value X1 and Raw Value X2 which
     * identifies a starting column and a ending column, we need to use these
     * values to decide the columns.
     */

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_SENSOR_SERIES_STATUS);
    net_buf_simple_add_le16(msg, prop_id);
    if (i != srv->state_count) {
        if (optional) {
            if (state->series_column.raw_value_x) {
                net_buf_simple_add_mem(msg, state->series_column.raw_value_x->data,
                                       state->series_column.raw_value_x->len);
            }
            if (state->series_column.column_width) {
                net_buf_simple_add_mem(msg, state->series_column.column_width->data,
                                       state->series_column.column_width->len);
            }
            if (state->series_column.raw_value_y) {
                net_buf_simple_add_mem(msg, state->series_column.raw_value_y->data,
                                       state->series_column.raw_value_y->len);
            }
        }
    }

    BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));

    bt_mesh_free_buf(msg);
}

static void sensor_get(struct bt_mesh_model *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct net_buf_simple *buf)
{
    uint16_t set_prop_id = INVALID_SENSOR_PROPERTY_ID;
    uint16_t prop_id = INVALID_SENSOR_PROPERTY_ID;

    if (model->user_data == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET:
    case BLE_MESH_MODEL_OP_SENSOR_GET:
    case BLE_MESH_MODEL_OP_SENSOR_COLUMN_GET:
    case BLE_MESH_MODEL_OP_SENSOR_SERIES_GET: {
        struct bt_mesh_sensor_srv *srv = model->user_data;
        if (srv->state_count == 0U || srv->states == NULL) {
            BT_ERR("Invalid Sensor Server state");
            return;
        }
        if (ctx->recv_op == BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET ||
                ctx->recv_op == BLE_MESH_MODEL_OP_SENSOR_GET) {
            bool get_all = buf->len ? false : true;
            if (buf->len) {
                prop_id = net_buf_simple_pull_le16(buf);
                if (prop_id == INVALID_SENSOR_PROPERTY_ID) {
                    BT_ERR("Prohibited Sensor Property ID 0x0000");
                    return;
                }
            }
            if (ctx->recv_op == BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET) {
                if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
                    bt_mesh_sensor_server_recv_get_msg_t get = {
                        .sensor_descriptor_get.op_en = !get_all,
                        .sensor_descriptor_get.id = prop_id,
                    };
                    bt_mesh_sensor_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_SENSOR_SERVER_RECV_GET_MSG,
                                                        model, ctx, (const uint8_t *)&get, sizeof(get));
                } else {
                    send_sensor_descriptor_status(model, ctx, prop_id, get_all);
                }
            } else {
                if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
                    bt_mesh_sensor_server_recv_get_msg_t get = {
                        .sensor_get.op_en = !get_all,
                        .sensor_get.id = prop_id,
                    };
                    bt_mesh_sensor_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_SENSOR_SERVER_RECV_GET_MSG,
                                                        model, ctx, (const uint8_t *)&get, sizeof(get));
                } else {
                    send_sensor_data_status(model, ctx, prop_id, get_all);
                }
            }
        } else {
            prop_id = net_buf_simple_pull_le16(buf);
            if (prop_id == INVALID_SENSOR_PROPERTY_ID) {
                BT_ERR("Prohibited Sensor Property ID 0x0000");
                return;
            }
            if (ctx->recv_op == BLE_MESH_MODEL_OP_SENSOR_COLUMN_GET) {
                if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
                    bt_mesh_sensor_server_recv_get_msg_t get = {
                        .sensor_column_get.id = prop_id,
                        .sensor_column_get.raw_x = buf,
                    };
                    bt_mesh_sensor_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_SENSOR_SERVER_RECV_GET_MSG,
                                                        model, ctx, (const uint8_t *)&get, sizeof(get));
                } else {
                    send_sensor_column_status(model, ctx, buf, prop_id);
                }
            } else {
                if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
                    bt_mesh_sensor_server_recv_get_msg_t get = {
                        .sensor_series_get.id = prop_id,
                        .sensor_series_get.raw = buf,
                    };
                    bt_mesh_sensor_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_SENSOR_SERVER_RECV_GET_MSG,
                                                        model, ctx, (const uint8_t *)&get, sizeof(get));
                } else {
                    send_sensor_series_status(model, ctx, buf, prop_id);
                }
            }
        }
        return;
    }
    case BLE_MESH_MODEL_OP_SENSOR_CADENCE_GET:
    case BLE_MESH_MODEL_OP_SENSOR_SETTINGS_GET:
    case BLE_MESH_MODEL_OP_SENSOR_SETTING_GET: {
        struct bt_mesh_sensor_setup_srv *srv = model->user_data;
        if (srv->state_count == 0U || srv->states == NULL) {
            BT_ERR("Invalid Sensor Setup Server state");
            return;
        }
        if (ctx->recv_op == BLE_MESH_MODEL_OP_SENSOR_CADENCE_GET ||
                ctx->recv_op == BLE_MESH_MODEL_OP_SENSOR_SETTINGS_GET) {
            prop_id = net_buf_simple_pull_le16(buf);
            if (prop_id == INVALID_SENSOR_PROPERTY_ID) {
                BT_ERR("Prohibited Sensor Property ID 0x0000");
                return;
            }
            if (ctx->recv_op == BLE_MESH_MODEL_OP_SENSOR_CADENCE_GET) {
                if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
                    bt_mesh_sensor_server_recv_get_msg_t get = {
                        .sensor_cadence_get.id = prop_id,
                    };
                    bt_mesh_sensor_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_SENSOR_SERVER_RECV_GET_MSG,
                                                        model, ctx, (const uint8_t *)&get, sizeof(get));
                } else {
                    send_sensor_cadence_status(model, ctx, prop_id, false);
                }
            } else {
                if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
                    bt_mesh_sensor_server_recv_get_msg_t get = {
                        .sensor_settings_get.id = prop_id,
                    };
                    bt_mesh_sensor_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_SENSOR_SERVER_RECV_GET_MSG,
                                                        model, ctx, (const uint8_t *)&get, sizeof(get));
                } else {
                    send_sensor_settings_status(model, ctx, prop_id);
                }
            }
        } else {
            prop_id = net_buf_simple_pull_le16(buf);
            if (prop_id == INVALID_SENSOR_PROPERTY_ID) {
                BT_ERR("Prohibited Sensor Property ID 0x0000");
                return;
            }
            set_prop_id = net_buf_simple_pull_le16(buf);
            if (set_prop_id == INVALID_SENSOR_PROPERTY_ID) {
                BT_ERR("Prohibited Sensor Setting Property ID 0x0000");
                return;
            }

            if (srv->rsp_ctrl.get_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
                bt_mesh_sensor_server_recv_get_msg_t get = {
                    .sensor_setting_get.id = prop_id,
                    .sensor_setting_get.setting_id = set_prop_id,
                };
                bt_mesh_sensor_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_SENSOR_SERVER_RECV_GET_MSG,
                                                    model, ctx, (const uint8_t *)&get, sizeof(get));
            } else {
                send_sensor_setting_status(model, ctx, prop_id, set_prop_id, false);
            }
        }
        return;
    }
    default:
        BT_WARN("Unknown Sensor Get opcode 0x%04x", ctx->recv_op);
        return;
    }
}

static void sensor_cadence_set(struct bt_mesh_model *model,
                               struct bt_mesh_msg_ctx *ctx,
                               struct net_buf_simple *buf)
{
    struct bt_mesh_sensor_setup_srv *srv = model->user_data;
    bt_mesh_sensor_server_state_change_t change = {0};
    struct bt_mesh_sensor_state *state = NULL;
    struct bt_mesh_model *sensor_model = NULL;
    struct bt_mesh_elem *element = NULL;
    uint16_t prop_id = 0U, trigger_len = 0U;
    uint8_t val = 0U, divisor = 0U;
    int i;

    if (srv == NULL || srv->state_count == 0U || srv->states == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    prop_id = net_buf_simple_pull_le16(buf);
    if (prop_id == INVALID_SENSOR_PROPERTY_ID) {
        BT_ERR("Prohibited Sensor Property ID 0x0000");
        return;
    }

    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_sensor_server_recv_set_msg_t set = {
            .sensor_cadence_set.id = prop_id,
            .sensor_cadence_set.cadence = buf,
        };
        bt_mesh_sensor_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_SENSOR_SERVER_RECV_SET_MSG,
                                            model, ctx, (const uint8_t *)&set, sizeof(set));
        return;
    }

    for (i = 0; i < srv->state_count; i++) {
        state = &srv->states[i];
        if (state->sensor_property_id != INVALID_SENSOR_PROPERTY_ID &&
            state->sensor_property_id == prop_id) {
            break;
        }
    }
    if (i == srv->state_count || state->cadence == NULL) {
        /* When the message is sent as a response to the Sensor Cadence Get message or
         * a Sensor Cadence Set message with an unknown Property ID field or the Sensor
         * Server does not support the Sensor Cadence state for the sensor referred by
         * the Property ID, the following fields shall be omitted:
         * • Fast Cadence Period Divisor
         * • Status Trigger Type
         * • Status Trigger Delta Down
         * • Status Trigger Delta Up
         * • Status Min Interval
         * • Fast Cadence Low
         * • Fast Cadence High
         */
        send_sensor_cadence_status(model, ctx, prop_id, false);
        return;
    }

    val = net_buf_simple_pull_u8(buf);
    divisor = val & BIT_MASK(7);
    if (divisor > SENSOR_PERIOD_DIVISOR_MAX_VALUE) {
        BT_ERR("Prohibited Fast Cadence Period Divisor 0x%02x", divisor);
        return;
    }
    state->cadence->period_divisor = divisor;
    state->cadence->trigger_type = (val >> 7) & BIT_MASK(1);

    if (state->cadence->trigger_type == SENSOR_STATUS_TRIGGER_TYPE_CHAR) {
        trigger_len = bt_mesh_get_dev_prop_len(prop_id);
    } else {
        trigger_len = SENSOR_STATUS_TRIGGER_UINT16_LEN;
    }
    if (buf->len < (trigger_len << 1) + SENSOR_STATUS_MIN_INTERVAL_LEN) {
        BT_ERR("Invalid Sensor Cadence Set length %d, trigger type %d",
                buf->len + 3, state->cadence->trigger_type);
        return;
    }

    if (state->cadence->trigger_delta_down) {
        net_buf_simple_reset(state->cadence->trigger_delta_down);
        net_buf_simple_add_mem(state->cadence->trigger_delta_down, buf->data, trigger_len);
        net_buf_simple_pull_mem(buf, trigger_len);
    }
    if (state->cadence->trigger_delta_up) {
        net_buf_simple_reset(state->cadence->trigger_delta_up);
        net_buf_simple_add_mem(state->cadence->trigger_delta_up, buf->data, trigger_len);
        net_buf_simple_pull_mem(buf, trigger_len);
    }

    /* The valid range for the Status Min Interval is 0–26 and other values are Prohibited. */
    val = net_buf_simple_pull_u8(buf);
    if (val > SENSOR_STATUS_MIN_INTERVAL_MAX) {
        BT_ERR("Invalid Status Min Interval %d", val);
        return;
    }
    state->cadence->min_interval = val;

    if (buf->len % 2) {
        BT_ERR("Different length of Fast Cadence Low & High, length %d", buf->len);
        return;
    }
    if (buf->len) {
        uint8_t range_len = buf->len / 2;
        if (state->cadence->fast_cadence_low) {
            net_buf_simple_reset(state->cadence->fast_cadence_low);
            net_buf_simple_add_mem(state->cadence->fast_cadence_low, buf->data, range_len);
            net_buf_simple_pull_mem(buf, range_len);
        }
        if (state->cadence->fast_cadence_high) {
            net_buf_simple_reset(state->cadence->fast_cadence_high);
            net_buf_simple_add_mem(state->cadence->fast_cadence_high, buf->data, range_len);
            net_buf_simple_pull_mem(buf, range_len);
        }
    }

    change.sensor_cadence_set.id = prop_id;
    change.sensor_cadence_set.period_divisor = state->cadence->period_divisor;
    change.sensor_cadence_set.trigger_type = state->cadence->trigger_type;
    change.sensor_cadence_set.trigger_delta_down = state->cadence->trigger_delta_down;
    change.sensor_cadence_set.trigger_delta_up = state->cadence->trigger_delta_up;
    change.sensor_cadence_set.min_interval = state->cadence->min_interval;
    change.sensor_cadence_set.fast_cadence_low = state->cadence->fast_cadence_low;
    change.sensor_cadence_set.fast_cadence_high = state->cadence->fast_cadence_high;
    bt_mesh_sensor_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_SENSOR_SERVER_STATE_CHANGE,
                                        model, ctx, (const uint8_t *)&change, sizeof(change));

    if (ctx->recv_op == BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET) {
        send_sensor_cadence_status(model, ctx, prop_id, false);
    }
    send_sensor_cadence_status(model, ctx, prop_id, true);

    /* Try to find the corresponding Sensor Server Model */
    element = bt_mesh_model_elem(model);
    sensor_model = bt_mesh_model_find(element, BLE_MESH_MODEL_ID_SENSOR_SRV);
    if (sensor_model == NULL) {
        BT_WARN("Sensor Server model not exists in the element");
        return;
    }

    /**
     * Based on the configured Sensor Cadence state, change Periodic Sensor
     * status publication mechanism.
     */
    update_sensor_periodic_pub(sensor_model, prop_id);
}

static void update_sensor_periodic_pub(struct bt_mesh_model *model, uint16_t prop_id)
{
    struct bt_mesh_sensor_state *state = NULL;
    struct bt_mesh_sensor_srv *srv = NULL;
    int i;

    if (model->id != BLE_MESH_MODEL_ID_SENSOR_SRV) {
        BT_ERR("Invalid Sensor Server model 0x%04x", model->id);
        return;
    }

    srv = (struct bt_mesh_sensor_srv *)model->user_data;
    if (srv == NULL || srv->state_count == 0U || srv->states == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    for (i = 0; i < srv->state_count; i++) {
        state = &srv->states[i];
        if (state->sensor_property_id != INVALID_SENSOR_PROPERTY_ID &&
            state->sensor_property_id == prop_id) {
            break;
        }
    }
    if (i == srv->state_count) {
        BT_ERR("Sensor Property ID 0x%04x not exists", prop_id);
        return;
    }

    if (state->cadence == NULL) {
        BT_WARN("Sensor Cadence state not exists");
        return;
    }

    /**
     * Currently when the device receives a Sensor Cadence Set message,
     * a event will be callback to the application layer, and users can
     * change the Sensor Data publication period in the event. And this
     * is exactly what we do for the BQB test.
     */
}

static void sensor_setting_set(struct bt_mesh_model *model,
                               struct bt_mesh_msg_ctx *ctx,
                               struct net_buf_simple *buf)
{
    struct bt_mesh_sensor_setup_srv *srv = model->user_data;
    bt_mesh_sensor_server_state_change_t change = {0};
    struct sensor_setting *item = NULL;
    uint16_t prop_id = 0U, set_prop_id = 0U;

    if (srv == NULL || srv->state_count == 0U || srv->states == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return;
    }

    prop_id = net_buf_simple_pull_le16(buf);
    if (prop_id == INVALID_SENSOR_PROPERTY_ID) {
        BT_ERR("Prohibited Sensor Property ID 0x0000");
        return;
    }

    set_prop_id = net_buf_simple_pull_le16(buf);
    if (set_prop_id == INVALID_SENSOR_PROPERTY_ID) {
        BT_ERR("Prohibited Sensor Setting Property ID 0x0000");
        return;
    }

    if (srv->rsp_ctrl.set_auto_rsp == BLE_MESH_SERVER_RSP_BY_APP) {
        bt_mesh_sensor_server_recv_set_msg_t set = {
            .sensor_setting_set.id = prop_id,
            .sensor_setting_set.setting_id = set_prop_id,
            .sensor_setting_set.raw = buf,
        };
        bt_mesh_sensor_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_SENSOR_SERVER_RECV_SET_MSG,
                                            model, ctx, (const uint8_t *)&set, sizeof(set));
        return;
    }

    item = find_sensor_setting(model, prop_id, set_prop_id);
    if (item) {
        if (item->access == SENSOR_SETTING_ACCESS_READ_WRITE && item->raw) {
            net_buf_simple_reset(item->raw);
            net_buf_simple_add_mem(item->raw, buf->data,
                                   MIN(buf->len, item->raw->size));

            change.sensor_setting_set.id = prop_id;
            change.sensor_setting_set.setting_id = set_prop_id;
            change.sensor_setting_set.value = item->raw;
            bt_mesh_sensor_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_SENSOR_SERVER_STATE_CHANGE,
                                                model, ctx, (const uint8_t *)&change, sizeof(change));
        }
    }

    if (ctx->recv_op == BLE_MESH_MODEL_OP_SENSOR_SETTING_SET) {
        send_sensor_setting_status(model, ctx, prop_id, set_prop_id, false);
    }
    if (item) {
        send_sensor_setting_status(model, ctx, prop_id, set_prop_id, true);
    }
}

/* message handlers (End) */

/* Mapping of message handlers for Sensor Server (0x1100) */
const struct bt_mesh_model_op bt_mesh_sensor_srv_op[] = {
    { BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET, 0, sensor_get },
    { BLE_MESH_MODEL_OP_SENSOR_GET,            0, sensor_get },
    { BLE_MESH_MODEL_OP_SENSOR_COLUMN_GET,     2, sensor_get },
    { BLE_MESH_MODEL_OP_SENSOR_SERIES_GET,     2, sensor_get },
    BLE_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Sensor Setup Server (0x1101) */
const struct bt_mesh_model_op bt_mesh_sensor_setup_srv_op[] = {
    { BLE_MESH_MODEL_OP_SENSOR_CADENCE_GET,       2, sensor_get         },
    { BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET,       4, sensor_cadence_set },
    { BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET_UNACK, 4, sensor_cadence_set },
    { BLE_MESH_MODEL_OP_SENSOR_SETTINGS_GET,      2, sensor_get         },
    { BLE_MESH_MODEL_OP_SENSOR_SETTING_GET,       4, sensor_get         },
    { BLE_MESH_MODEL_OP_SENSOR_SETTING_SET,       4, sensor_setting_set },
    { BLE_MESH_MODEL_OP_SENSOR_SETTING_SET_UNACK, 4, sensor_setting_set },
    BLE_MESH_MODEL_OP_END,
};

static int check_sensor_server_init(struct bt_mesh_sensor_state *state_start,
                                    const uint8_t state_count)
{
    struct bt_mesh_sensor_state *state = NULL;
    struct sensor_setting *setting = NULL;
    int i, j, k;

    for (i = 0; i < state_count; i++) {
        state = &state_start[i];
        if (state->sensor_property_id == INVALID_SENSOR_PROPERTY_ID) {
            BT_ERR("Invalid Sensor Property ID 0x%04x", state->sensor_property_id);
            return -EINVAL;
        }
        /* Check if the same Sensor Property ID exists */
        for (k = i + 1; k < state_count; k++) {
            if (state->sensor_property_id == state_start[k].sensor_property_id) {
                BT_ERR("Same Sensor Property ID 0x%04x exists", state->sensor_property_id);
                return -EINVAL;
            }
        }
        if (state->setting_count && state->settings) {
            for (j = 0; j < state->setting_count; j++) {
                setting = &state->settings[j];
                if (setting->property_id == INVALID_SENSOR_SETTING_PROPERTY_ID || setting->raw == NULL) {
                    BT_ERR("Invalid Sensor Setting state");
                    return -EINVAL;
                }
                /* Check if the same Sensor Setting Property ID exists */
                for (k = j + 1; k < state->setting_count; k++) {
                    if (setting->property_id == state->settings[k].property_id) {
                        BT_ERR("Same Sensor Setting Property ID 0x%04x exists", setting->property_id);
                        return -EINVAL;
                    }
                }
            }
        }
        if (state->cadence) {
            if (state->cadence->trigger_delta_down == NULL ||
                state->cadence->trigger_delta_up == NULL ||
                state->cadence->fast_cadence_low == NULL ||
                state->cadence->fast_cadence_high == NULL) {
                BT_ERR("Invalid Sensor Cadence state");
                return -EINVAL;
            }
        }
        if (state->sensor_data.raw_value == NULL) {
            BT_ERR("Invalid Sensor Data state");
            return -EINVAL;
        }
    }

    return 0;
}

static int sensor_server_init(struct bt_mesh_model *model)
{
    if (model->user_data == NULL) {
        BT_ERR("Invalid Sensor Server user data, model id 0x%04x", model->id);
        return -EINVAL;
    }

    switch (model->id) {
    case BLE_MESH_MODEL_ID_SENSOR_SRV: {
        struct bt_mesh_sensor_srv *srv = model->user_data;
        if (srv->state_count == 0U || srv->states == NULL) {
            BT_ERR("Invalid Sensor state, model id 0x%04x", model->id);
            return -EINVAL;
        }

        if (check_sensor_server_init(srv->states, srv->state_count)) {
            return -EINVAL;
        }

        srv->model = model;
        break;
    }
    case BLE_MESH_MODEL_ID_SENSOR_SETUP_SRV: {
        struct bt_mesh_sensor_setup_srv *srv = model->user_data;
        if (srv->state_count == 0U || srv->states == NULL) {
            BT_ERR("Invalid Sensor state, model id 0x%04x", model->id);
            return -EINVAL;
        }

        if (check_sensor_server_init(srv->states, srv->state_count)) {
            return -EINVAL;
        }

        srv->model = model;
        break;
    }
    default:
        BT_WARN("Unknown Sensor Server, model id 0x%04x", model->id);
        return -EINVAL;
    }

    return 0;
}

static int sensor_srv_init(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Sensor Server has no publication support");
        return -EINVAL;
    }

    /* When this model is present on an element, the corresponding Sensor Setup
     * Server model shall also be present.
     */
    struct bt_mesh_elem *element = bt_mesh_model_elem(model);
    if (bt_mesh_model_find(element, BLE_MESH_MODEL_ID_SENSOR_SETUP_SRV) == NULL) {
        BT_WARN("Sensor Setup Server not present");
        /* Just give a warning here, continue with the initialization */
    }
    return sensor_server_init(model);
}

static int sensor_setup_srv_init(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Sensor Setup Server has no publication support");
        return -EINVAL;
    }

    return sensor_server_init(model);
}

#if CONFIG_BLE_MESH_DEINIT
static int sensor_server_deinit(struct bt_mesh_model *model)
{
    if (model->user_data == NULL) {
        BT_ERR("Invalid Sensor Server user, model id 0x%04x", model->id);
        return -EINVAL;
    }

    return 0;
}

static int sensor_srv_deinit(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Sensor Server has no publication support");
        return -EINVAL;
    }

    return sensor_server_deinit(model);
}

static int sensor_setup_srv_deinit(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Sensor Setup Server has no publication support");
        return -EINVAL;
    }

    return sensor_server_deinit(model);
}
#endif /* CONFIG_BLE_MESH_DEINIT */

const struct bt_mesh_model_cb bt_mesh_sensor_srv_cb = {
    .init = sensor_srv_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = sensor_srv_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

const struct bt_mesh_model_cb bt_mesh_sensor_setup_srv_cb = {
    .init = sensor_setup_srv_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = sensor_setup_srv_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

#endif /* CONFIG_BLE_MESH_SENSOR_SERVER */

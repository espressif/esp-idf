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

#include "btc_ble_mesh_sensor_model.h"

#include "mesh_config.h"
#include "model_opcode.h"

#if CONFIG_BLE_MESH_SENSOR_CLI
#include "sensor_client.h"

/* The followings are the macro definitions of Sensor client
 * model message length, and a message is composed of 3 parts:
 * Opcode + Payload + MIC
 */
/* Sensor client messages length */
#define BLE_MESH_SENSOR_DESCRIPTOR_GET_MSG_LEN (2 + 2 + 4)
#define BLE_MESH_SENSOR_CADENCE_GET_MSG_LEN    (2 + 2 + 4)
#define BLE_MESH_SENSOR_CADENCE_SET_MSG_LEN    /* variable */
#define BLE_MESH_SENSOR_SETTINGS_GET_MSG_LEN   (2 + 2 + 4)
#define BLE_MESH_SENSOR_SETTING_GET_MSG_LEN    (2 + 4 + 4)
#define BLE_MESH_SENSOR_SETTING_SET_MSG_LEN    /* variable */
#define BLE_MESH_SENSOR_GET_MSG_LEN            (2 + 2 + 4)
#define BLE_MESH_SENSOR_COLUMN_GET_MSG_LEN     /* variable */
#define BLE_MESH_SENSOR_SERIES_GET_MSG_LEN     /* variable */

static const bt_mesh_client_op_pair_t sensor_op_pair[] = {
    { BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET, BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_STATUS },
    { BLE_MESH_MODEL_OP_SENSOR_CADENCE_GET,    BLE_MESH_MODEL_OP_SENSOR_CADENCE_STATUS    },
    { BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET,    BLE_MESH_MODEL_OP_SENSOR_CADENCE_STATUS    },
    { BLE_MESH_MODEL_OP_SENSOR_SETTINGS_GET,   BLE_MESH_MODEL_OP_SENSOR_SETTINGS_STATUS   },
    { BLE_MESH_MODEL_OP_SENSOR_SETTING_GET,    BLE_MESH_MODEL_OP_SENSOR_SETTING_STATUS    },
    { BLE_MESH_MODEL_OP_SENSOR_SETTING_SET,    BLE_MESH_MODEL_OP_SENSOR_SETTING_STATUS    },
    { BLE_MESH_MODEL_OP_SENSOR_GET,            BLE_MESH_MODEL_OP_SENSOR_STATUS            },
    { BLE_MESH_MODEL_OP_SENSOR_COLUMN_GET,     BLE_MESH_MODEL_OP_SENSOR_COLUMN_STATUS     },
    { BLE_MESH_MODEL_OP_SENSOR_SERIES_GET,     BLE_MESH_MODEL_OP_SENSOR_SERIES_STATUS     },
};

static bt_mesh_mutex_t sensor_client_lock;

static inline void bt_mesh_sensor_client_mutex_new(void)
{
    if (!sensor_client_lock.mutex) {
        bt_mesh_mutex_create(&sensor_client_lock);
    }
}

#if CONFIG_BLE_MESH_DEINIT
static inline void bt_mesh_sensor_client_mutex_free(void)
{
    bt_mesh_mutex_free(&sensor_client_lock);
}
#endif /* CONFIG_BLE_MESH_DEINIT */

static inline void bt_mesh_sensor_client_lock(void)
{
    bt_mesh_mutex_lock(&sensor_client_lock);
}

static inline void bt_mesh_sensor_client_unlock(void)
{
    bt_mesh_mutex_unlock(&sensor_client_lock);
}

static void timeout_handler(struct k_work *work)
{
    struct k_delayed_work *timer = NULL;
    bt_mesh_client_node_t *node = NULL;
    struct bt_mesh_msg_ctx ctx = {0};
    uint32_t opcode = 0U;

    BT_WARN("Receive sensor status message timeout");

    bt_mesh_sensor_client_lock();

    timer = CONTAINER_OF(work, struct k_delayed_work, work);

    if (timer && !k_delayed_work_free(timer)) {
        node = CONTAINER_OF(work, bt_mesh_client_node_t, timer.work);
        if (node) {
            memcpy(&ctx, &node->ctx, sizeof(ctx));
            opcode = node->opcode;
            bt_mesh_client_free_node(node);
            bt_mesh_sensor_client_cb_evt_to_btc(
                opcode, BTC_BLE_MESH_EVT_SENSOR_CLIENT_TIMEOUT, ctx.model, &ctx, NULL, 0);
        }
    }

    bt_mesh_sensor_client_unlock();

    return;
}

static void sensor_status(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    bt_mesh_client_node_t *node = NULL;
    uint8_t *val = NULL;
    uint8_t evt = 0xFF;
    size_t len = 0U;

    BT_DBG("len %d, bytes %s", buf->len, bt_hex(buf->data, buf->len));

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_STATUS: {
        struct bt_mesh_sensor_descriptor_status *status = NULL;
        status = bt_mesh_calloc(sizeof(struct bt_mesh_sensor_descriptor_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->descriptor = bt_mesh_alloc_buf(buf->len);
        if (!status->descriptor) {
            BT_ERR("%s, Out of memory", __func__);
            bt_mesh_free(status);
            return;
        }
        net_buf_simple_add_mem(status->descriptor, buf->data, buf->len);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_sensor_descriptor_status);
        break;
    }
    case BLE_MESH_MODEL_OP_SENSOR_CADENCE_STATUS: {
        struct bt_mesh_sensor_cadence_status *status = NULL;
        status = bt_mesh_calloc(sizeof(struct bt_mesh_sensor_cadence_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->property_id = net_buf_simple_pull_le16(buf);
        status->sensor_cadence_value = bt_mesh_alloc_buf(buf->len);
        if (!status->sensor_cadence_value) {
            BT_ERR("%s, Out of memory", __func__);
            bt_mesh_free(status);
            return;
        }
        net_buf_simple_add_mem(status->sensor_cadence_value, buf->data, buf->len);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_sensor_cadence_status);
        break;
    }
    case BLE_MESH_MODEL_OP_SENSOR_SETTINGS_STATUS: {
        struct bt_mesh_sensor_settings_status *status = NULL;
        status = bt_mesh_calloc(sizeof(struct bt_mesh_sensor_settings_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->sensor_property_id = net_buf_simple_pull_le16(buf);
        status->sensor_setting_property_ids = bt_mesh_alloc_buf(buf->len);
        if (!status->sensor_setting_property_ids) {
            BT_ERR("%s, Out of memory", __func__);
            bt_mesh_free(status);
            return;
        }
        net_buf_simple_add_mem(status->sensor_setting_property_ids, buf->data, buf->len);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_sensor_settings_status);
        break;
    }
    case BLE_MESH_MODEL_OP_SENSOR_SETTING_STATUS: {
        struct bt_mesh_sensor_setting_status *status = NULL;
        status = bt_mesh_calloc(sizeof(struct bt_mesh_sensor_setting_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->sensor_property_id = net_buf_simple_pull_le16(buf);
        status->sensor_setting_property_id = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en = true;
            status->sensor_setting_access = net_buf_simple_pull_u8(buf);
            status->sensor_setting_raw = bt_mesh_alloc_buf(buf->len);
            if (!status->sensor_setting_raw) {
                BT_ERR("%s, Out of memory", __func__);
                bt_mesh_free(status);
                return;
            }
            net_buf_simple_add_mem(status->sensor_setting_raw, buf->data, buf->len);
        }
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_sensor_setting_status);
        break;
    }
    case BLE_MESH_MODEL_OP_SENSOR_STATUS: {
        struct bt_mesh_sensor_status *status = NULL;
        status = bt_mesh_calloc(sizeof(struct bt_mesh_sensor_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->marshalled_sensor_data = bt_mesh_alloc_buf(buf->len);
        if (!status->marshalled_sensor_data) {
            BT_ERR("%s, Out of memory", __func__);
            bt_mesh_free(status);
            return;
        }
        net_buf_simple_add_mem(status->marshalled_sensor_data, buf->data, buf->len);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_sensor_status);
        break;
    }
    case BLE_MESH_MODEL_OP_SENSOR_COLUMN_STATUS: {
        struct bt_mesh_sensor_column_status *status = NULL;
        status = bt_mesh_calloc(sizeof(struct bt_mesh_sensor_column_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->property_id = net_buf_simple_pull_le16(buf);
        status->sensor_column_value = bt_mesh_alloc_buf(buf->len);
        if (!status->sensor_column_value) {
            BT_ERR("%s, Out of memory", __func__);
            bt_mesh_free(status);
            return;
        }
        net_buf_simple_add_mem(status->sensor_column_value, buf->data, buf->len);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_sensor_column_status);
        break;
    }
    case BLE_MESH_MODEL_OP_SENSOR_SERIES_STATUS: {
        struct bt_mesh_sensor_series_status *status = NULL;
        status = bt_mesh_calloc(sizeof(struct bt_mesh_sensor_series_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->property_id = net_buf_simple_pull_le16(buf);
        status->sensor_series_value = bt_mesh_alloc_buf(buf->len);
        if (!status->sensor_series_value) {
            BT_ERR("%s, Out of memory", __func__);
            bt_mesh_free(status);
            return;
        }
        net_buf_simple_add_mem(status->sensor_series_value, buf->data, buf->len);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_sensor_series_status);
        break;
    }
    default:
        BT_ERR("Invalid Sensor Status opcode 0x%04x", ctx->recv_op);
        return;
    }

    buf->data = val;
    buf->len  = len;

    bt_mesh_sensor_client_lock();

    node = bt_mesh_is_client_recv_publish_msg(model, ctx, buf, true);
    if (!node) {
        BT_DBG("Unexpected Sensor Status 0x%04x", ctx->recv_op);
    } else {
        switch (node->opcode) {
        case BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET:
        case BLE_MESH_MODEL_OP_SENSOR_CADENCE_GET:
        case BLE_MESH_MODEL_OP_SENSOR_SETTINGS_GET:
        case BLE_MESH_MODEL_OP_SENSOR_SETTING_GET:
        case BLE_MESH_MODEL_OP_SENSOR_GET:
        case BLE_MESH_MODEL_OP_SENSOR_COLUMN_GET:
        case BLE_MESH_MODEL_OP_SENSOR_SERIES_GET:
            evt = BTC_BLE_MESH_EVT_SENSOR_CLIENT_GET_STATE;
            break;
        case BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET:
        case BLE_MESH_MODEL_OP_SENSOR_SETTING_SET:
            evt = BTC_BLE_MESH_EVT_SENSOR_CLIENT_SET_STATE;
            break;
        default:
            break;
        }

        if (!k_delayed_work_free(&node->timer)) {
            uint32_t opcode = node->opcode;
            bt_mesh_client_free_node(node);
            bt_mesh_sensor_client_cb_evt_to_btc(opcode, evt, model, ctx, val, len);
        }
    }

    bt_mesh_sensor_client_unlock();

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_STATUS: {
        struct bt_mesh_sensor_descriptor_status *status;
        status = (struct bt_mesh_sensor_descriptor_status *)val;
        bt_mesh_free_buf(status->descriptor);
        break;
    }
    case BLE_MESH_MODEL_OP_SENSOR_CADENCE_STATUS: {
        struct bt_mesh_sensor_cadence_status *status;
        status = (struct bt_mesh_sensor_cadence_status *)val;
        bt_mesh_free_buf(status->sensor_cadence_value);
        break;
    }
    case BLE_MESH_MODEL_OP_SENSOR_SETTINGS_STATUS: {
        struct bt_mesh_sensor_settings_status *status;
        status = (struct bt_mesh_sensor_settings_status *)val;
        bt_mesh_free_buf(status->sensor_setting_property_ids);
        break;
    }
    case BLE_MESH_MODEL_OP_SENSOR_SETTING_STATUS: {
        struct bt_mesh_sensor_setting_status *status;
        status = (struct bt_mesh_sensor_setting_status *)val;
        bt_mesh_free_buf(status->sensor_setting_raw);
        break;
    }
    case BLE_MESH_MODEL_OP_SENSOR_STATUS: {
        struct bt_mesh_sensor_status *status;
        status = (struct bt_mesh_sensor_status *)val;
        bt_mesh_free_buf(status->marshalled_sensor_data);
        break;
    }
    case BLE_MESH_MODEL_OP_SENSOR_COLUMN_STATUS: {
        struct bt_mesh_sensor_column_status *status;
        status = (struct bt_mesh_sensor_column_status *)val;
        bt_mesh_free_buf(status->sensor_column_value);
        break;
    }
    case BLE_MESH_MODEL_OP_SENSOR_SERIES_STATUS: {
        struct bt_mesh_sensor_series_status *status;
        status = (struct bt_mesh_sensor_series_status *)val;
        bt_mesh_free_buf(status->sensor_series_value);
        break;
    }
    default:
        break;
    }

    bt_mesh_free(val);

    return;
}

const struct bt_mesh_model_op bt_mesh_sensor_cli_op[] = {
    { BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_STATUS, 0, sensor_status },
    { BLE_MESH_MODEL_OP_SENSOR_CADENCE_STATUS,    2, sensor_status },
    { BLE_MESH_MODEL_OP_SENSOR_SETTINGS_STATUS,   2, sensor_status },
    { BLE_MESH_MODEL_OP_SENSOR_SETTING_STATUS,    4, sensor_status },
    { BLE_MESH_MODEL_OP_SENSOR_STATUS,            0, sensor_status },
    { BLE_MESH_MODEL_OP_SENSOR_COLUMN_STATUS,     2, sensor_status },
    { BLE_MESH_MODEL_OP_SENSOR_SERIES_STATUS,     2, sensor_status },
    BLE_MESH_MODEL_OP_END,
};

static int sensor_act_state(bt_mesh_client_common_param_t *common,
                            void *value, uint16_t value_len, bool need_ack)
{
    struct net_buf_simple *msg = NULL;
    int err = 0;

    msg = bt_mesh_alloc_buf(value_len);
    if (!msg) {
        BT_ERR("%s, Out of memory", __func__);
        return -ENOMEM;
    }

    bt_mesh_model_msg_init(msg, common->opcode);

    switch (common->opcode) {
    case BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET: {
        struct bt_mesh_sensor_descriptor_get *act;
        act = (struct bt_mesh_sensor_descriptor_get *)value;
        if (act->op_en) {
            net_buf_simple_add_le16(msg, act->property_id);
        }
        break;
    }
    case BLE_MESH_MODEL_OP_SENSOR_CADENCE_GET: {
        struct bt_mesh_sensor_cadence_get *act;
        act = (struct bt_mesh_sensor_cadence_get *)value;
        net_buf_simple_add_le16(msg, act->property_id);
        break;
    }
    case BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET:
    case BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET_UNACK: {
        struct bt_mesh_sensor_cadence_set *act;
        act = (struct bt_mesh_sensor_cadence_set *)value;
        net_buf_simple_add_le16(msg, act->property_id);
        net_buf_simple_add_u8(msg,   act->status_trigger_type << 7 | act->fast_cadence_period_divisor);
        net_buf_simple_add_mem(msg,  act->status_trigger_delta_down->data, act->status_trigger_delta_down->len);
        net_buf_simple_add_mem(msg,  act->status_trigger_delta_up->data, act->status_trigger_delta_up->len);
        net_buf_simple_add_u8(msg,   act->status_min_interval);
        net_buf_simple_add_mem(msg,  act->fast_cadence_low->data, act->fast_cadence_low->len);
        net_buf_simple_add_mem(msg,  act->fast_cadence_high->data, act->fast_cadence_high->len);
        break;
    }
    case BLE_MESH_MODEL_OP_SENSOR_SETTINGS_GET: {
        struct bt_mesh_sensor_settings_get *act;
        act = (struct bt_mesh_sensor_settings_get *)value;
        net_buf_simple_add_le16(msg, act->sensor_property_id);
        break;
    }
    case BLE_MESH_MODEL_OP_SENSOR_SETTING_GET: {
        struct bt_mesh_sensor_setting_get *act;
        act = (struct bt_mesh_sensor_setting_get *)value;
        net_buf_simple_add_le16(msg, act->sensor_property_id);
        net_buf_simple_add_le16(msg, act->sensor_setting_property_id);
        break;
    }
    case BLE_MESH_MODEL_OP_SENSOR_SETTING_SET:
    case BLE_MESH_MODEL_OP_SENSOR_SETTING_SET_UNACK: {
        struct bt_mesh_sensor_setting_set *act;
        act = (struct bt_mesh_sensor_setting_set *)value;
        net_buf_simple_add_le16(msg, act->sensor_property_id);
        net_buf_simple_add_le16(msg, act->sensor_setting_property_id);
        net_buf_simple_add_mem(msg,  act->sensor_setting_raw->data, act->sensor_setting_raw->len);
        break;
    }
    case BLE_MESH_MODEL_OP_SENSOR_GET: {
        struct bt_mesh_sensor_get *act;
        act = (struct bt_mesh_sensor_get *)value;
        if (act->op_en) {
            net_buf_simple_add_le16(msg, act->property_id);
        }
        break;
    }
    case BLE_MESH_MODEL_OP_SENSOR_COLUMN_GET: {
        struct bt_mesh_sensor_column_get *act;
        act = (struct bt_mesh_sensor_column_get *)value;
        net_buf_simple_add_le16(msg, act->property_id);
        net_buf_simple_add_mem(msg, act->raw_value_x->data, act->raw_value_x->len);
        break;
    }
    case BLE_MESH_MODEL_OP_SENSOR_SERIES_GET: {
        struct bt_mesh_sensor_series_get *act;
        act = (struct bt_mesh_sensor_series_get *)value;
        net_buf_simple_add_le16(msg, act->property_id);
        if (act->op_en) {
            net_buf_simple_add_mem(msg, act->raw_value_x1->data, act->raw_value_x1->len);
            net_buf_simple_add_mem(msg, act->raw_value_x2->data, act->raw_value_x2->len);
        }
        break;
    }
    default:
        BT_ERR("Invalid Sensor client opcode 0x%04x", common->opcode);
        err = -EINVAL;
        goto end;
    }

    err = bt_mesh_client_send_msg(common, msg, need_ack, timeout_handler);

end:
    bt_mesh_free_buf(msg);
    return err;
}

int bt_mesh_sensor_client_get_state(bt_mesh_client_common_param_t *common, void *get)
{
    bt_mesh_sensor_client_t *client = NULL;
    uint16_t length = 0U;

    if (!common || !common->model || !get) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    client = (bt_mesh_sensor_client_t *)common->model->user_data;
    if (!client || !client->internal_data) {
        BT_ERR("Invalid Sensor client data");
        return -EINVAL;
    }

    switch (common->opcode) {
    case BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET:
        length = BLE_MESH_SENSOR_DESCRIPTOR_GET_MSG_LEN;
        break;
    case BLE_MESH_MODEL_OP_SENSOR_CADENCE_GET:
        length = BLE_MESH_SENSOR_CADENCE_GET_MSG_LEN;
        break;
    case BLE_MESH_MODEL_OP_SENSOR_SETTINGS_GET:
        length = BLE_MESH_SENSOR_SETTINGS_GET_MSG_LEN;
        break;
    case BLE_MESH_MODEL_OP_SENSOR_SETTING_GET:
        length = BLE_MESH_SENSOR_SETTING_GET_MSG_LEN;
        break;
    case BLE_MESH_MODEL_OP_SENSOR_GET:
        length = BLE_MESH_SENSOR_GET_MSG_LEN;
        break;
    case BLE_MESH_MODEL_OP_SENSOR_COLUMN_GET: {
        struct bt_mesh_sensor_column_get *value;
        value = (struct bt_mesh_sensor_column_get *)get;
        if (!value->raw_value_x) {
            BT_ERR("Invalid Sensor Column Get");
            return -EINVAL;
        }
        length = (2 + 2 + value->raw_value_x->len + 4);
        break;
    }
    case BLE_MESH_MODEL_OP_SENSOR_SERIES_GET: {
        struct bt_mesh_sensor_series_get *value;
        value = (struct bt_mesh_sensor_series_get *)get;
        if (value->op_en) {
            if (!value->raw_value_x1 || !value->raw_value_x2) {
                BT_ERR("Invalid Sensor Series Get");
                return -EINVAL;
            }
        }
        if (value->op_en) {
            length = value->raw_value_x1->len + value->raw_value_x2->len;
        }
        length += (2 + 2 + 4);
        break;
    }
    default:
        BT_ERR("Invalid Sensor Get opcode 0x%04x", common->opcode);
        return -EINVAL;
    }

    return sensor_act_state(common, get, length, true);
}

int bt_mesh_sensor_client_set_state(bt_mesh_client_common_param_t *common, void *set)
{
    bt_mesh_sensor_client_t *client = NULL;
    uint16_t length = 0U;
    bool need_ack = false;

    if (!common || !common->model || !set) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    client = (bt_mesh_sensor_client_t *)common->model->user_data;
    if (!client || !client->internal_data) {
        BT_ERR("Invalid Sensor client data");
        return -EINVAL;
    }

    switch (common->opcode) {
    case BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET_UNACK: {
        struct bt_mesh_sensor_cadence_set *value;
        value = (struct bt_mesh_sensor_cadence_set *)set;
        if (!value->status_trigger_delta_down || !value->status_trigger_delta_up ||
                !value->fast_cadence_low || !value->fast_cadence_high) {
            BT_ERR("Invalid Sensor Cadence Set");
            return -EINVAL;
        }
        length = value->status_trigger_delta_down->len + \
                 value->status_trigger_delta_up->len + \
                 value->fast_cadence_low->len + \
                 value->fast_cadence_high->len;
        length += (1 + 2 + 1 + 1 + 4);
        break;
    }
    case BLE_MESH_MODEL_OP_SENSOR_SETTING_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_SENSOR_SETTING_SET_UNACK: {
        struct bt_mesh_sensor_setting_set *value;
        value = (struct bt_mesh_sensor_setting_set *)set;
        if (!value->sensor_setting_raw) {
            BT_ERR("Invalid Sensor Setting Raw value");
            return -EINVAL;
        }
        length = (1 + 2 + 2 + value->sensor_setting_raw->len + 4);
        break;
    }
    default:
        BT_ERR("Invalid Sensor Set opcode 0x%04x", common->opcode);
        return -EINVAL;
    }

    return sensor_act_state(common, set, length, need_ack);
}

static int sensor_client_init(struct bt_mesh_model *model)
{
    sensor_internal_data_t *internal = NULL;
    bt_mesh_sensor_client_t *client = NULL;

    if (!model) {
        BT_ERR("Invalid Sensor client model");
        return -EINVAL;
    }

    client = (bt_mesh_sensor_client_t *)model->user_data;
    if (!client) {
        BT_ERR("No Sensor client context provided");
        return -EINVAL;
    }

    if (!client->internal_data) {
        internal = bt_mesh_calloc(sizeof(sensor_internal_data_t));
        if (!internal) {
            BT_ERR("%s, Out of memory", __func__);
            return -ENOMEM;
        }

        sys_slist_init(&internal->queue);

        client->model = model;
        client->op_pair_size = ARRAY_SIZE(sensor_op_pair);
        client->op_pair = sensor_op_pair;
        client->internal_data = internal;
    } else {
        bt_mesh_client_clear_list(client->internal_data);
    }

    bt_mesh_sensor_client_mutex_new();

    return 0;
}

#if CONFIG_BLE_MESH_DEINIT
static int sensor_client_deinit(struct bt_mesh_model *model)
{
    bt_mesh_sensor_client_t *client = NULL;

    if (!model) {
        BT_ERR("Invalid Sensor client model");
        return -EINVAL;
    }

    client = (bt_mesh_sensor_client_t *)model->user_data;
    if (!client) {
        BT_ERR("No Sensor client context provided");
        return -EINVAL;
    }

    if (client->internal_data) {
        /* Remove items from the list */
        bt_mesh_client_clear_list(client->internal_data);

        /* Free the allocated internal data */
        bt_mesh_free(client->internal_data);
        client->internal_data = NULL;
    }

    bt_mesh_sensor_client_mutex_free();

    return 0;
}
#endif /* CONFIG_BLE_MESH_DEINIT */

const struct bt_mesh_model_cb bt_mesh_sensor_client_cb = {
    .init = sensor_client_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = sensor_client_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

#endif /* CONFIG_BLE_MESH_SENSOR_CLI */

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
#include <stdbool.h>

#include "osi/allocator.h"
#include "sdkconfig.h"

#include "mesh_types.h"
#include "mesh_kernel.h"
#include "mesh_trace.h"

#include "mesh.h"
#include "model_opcode.h"
#include "mesh_common.h"
#include "time_scene_client.h"

#include "btc_ble_mesh_time_scene_model.h"

/** The following are the macro definitions of time and client
 *  scene model messages length, and a message is composed of
 *  three parts: Opcode + msg_value + MIC
 */
/* Time client messages length */
#define BLE_MESH_TIME_SET_MSG_LEN            (1 + 10 + 4)
#define BLE_MESH_TIME_ZONE_SET_MSG_LEN       (2 + 6 + 4)
#define BLE_MESH_TAI_UTC_DELTA_SET_MSG_LEN   (2 + 7 + 4)
#define BLE_MESH_TIME_ROLE_SET_MSG_LEN       (2 + 1 + 4)

/* Scene client messages length */
#define BLE_MESH_SCENE_STORE_MSG_LEN         (2 + 2 + 4)
#define BLE_MESH_SCENE_RECALL_MSG_LEN        (2 + 5 + 4)
#define BLE_MESH_SCENE_GET_MSG_LEN           (2 + 0 + 4)
#define BLE_MESH_SCENE_REGISTER_GET_MSG_LEN  (2 + 0 + 4)
#define BLE_MESH_SCENE_DELETE_MSG_LEN        (2 + 2 + 4)

/* Scheduler client messages length */
#define BLE_MESH_SCHEDULER_ACT_GET_MSG_LEN   (2 + 1 + 4)
#define BLE_MESH_SCHEDULER_ACT_SET_MSG_LEN   (1 + 10 + 4)

#define BLE_MESH_SCENE_GET_STATE_MSG_LEN     (2 + 1 + 4)
#define BLE_MESH_SCENE_ACT_STATE_MSG_LEN     (2 + 2 + 4)

static const bt_mesh_client_op_pair_t time_scene_op_pair[] = {
    { BLE_MESH_MODEL_OP_TIME_GET,           BLE_MESH_MODEL_OP_TIME_STATUS           },
    { BLE_MESH_MODEL_OP_TIME_SET,           BLE_MESH_MODEL_OP_TIME_STATUS           },
    { BLE_MESH_MODEL_OP_TIME_ZONE_GET,      BLE_MESH_MODEL_OP_TIME_ZONE_STATUS      },
    { BLE_MESH_MODEL_OP_TIME_ZONE_SET,      BLE_MESH_MODEL_OP_TIME_ZONE_STATUS      },
    { BLE_MESH_MODEL_OP_TAI_UTC_DELTA_GET,  BLE_MESH_MODEL_OP_TAI_UTC_DELTA_STATUS  },
    { BLE_MESH_MODEL_OP_TAI_UTC_DELTA_SET,  BLE_MESH_MODEL_OP_TAI_UTC_DELTA_STATUS  },
    { BLE_MESH_MODEL_OP_TIME_ROLE_GET,      BLE_MESH_MODEL_OP_TIME_ROLE_STATUS      },
    { BLE_MESH_MODEL_OP_TIME_ROLE_SET,      BLE_MESH_MODEL_OP_TIME_ROLE_STATUS      },
    { BLE_MESH_MODEL_OP_SCENE_STORE,        BLE_MESH_MODEL_OP_SCENE_REGISTER_STATUS },
    { BLE_MESH_MODEL_OP_SCENE_RECALL,       BLE_MESH_MODEL_OP_SCENE_STATUS          },
    { BLE_MESH_MODEL_OP_SCENE_GET,          BLE_MESH_MODEL_OP_SCENE_STATUS          },
    { BLE_MESH_MODEL_OP_SCENE_REGISTER_GET, BLE_MESH_MODEL_OP_SCENE_REGISTER_STATUS },
    { BLE_MESH_MODEL_OP_SCENE_DELETE,       BLE_MESH_MODEL_OP_SCENE_REGISTER_STATUS },
    { BLE_MESH_MODEL_OP_SCHEDULER_GET,      BLE_MESH_MODEL_OP_SCHEDULER_STATUS      },
    { BLE_MESH_MODEL_OP_SCHEDULER_ACT_GET,  BLE_MESH_MODEL_OP_SCHEDULER_ACT_STATUS  },
    { BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET,  BLE_MESH_MODEL_OP_SCHEDULER_ACT_STATUS  },
};

static void timeout_handler(struct k_work *work)
{
    time_scene_internal_data_t *internal = NULL;
    bt_mesh_time_scene_client_t *client = NULL;
    bt_mesh_client_node_t *node = NULL;

    BT_WARN("Receive time scene status message timeout");

    node = CONTAINER_OF(work, bt_mesh_client_node_t, timer.work);
    if (!node || !node->ctx.model) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    client = (bt_mesh_time_scene_client_t *)node->ctx.model->user_data;
    if (!client) {
        BT_ERR("%s, Time Scene Client user_data is NULL", __func__);
        return;
    }

    internal = (time_scene_internal_data_t *)client->internal_data;
    if (!internal) {
        BT_ERR("%s, Time Scene Client internal_data is NULL", __func__);
        return;
    }

    bt_mesh_time_scene_client_cb_evt_to_btc(node->opcode,
        BTC_BLE_MESH_EVT_TIME_SCENE_CLIENT_TIMEOUT, node->ctx.model, &node->ctx, NULL, 0);

    bt_mesh_client_free_node(&internal->queue, node);

    return;
}

static void time_scene_status(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    time_scene_internal_data_t *internal = NULL;
    bt_mesh_time_scene_client_t *client = NULL;
    bt_mesh_client_node_t *node = NULL;
    u8_t  *val = NULL;
    u8_t   evt = 0xFF;
    u32_t  rsp = 0;
    size_t len = 0;

    BT_DBG("%s, len %d, bytes %s", __func__, buf->len, bt_hex(buf->data, buf->len));

    client = (bt_mesh_time_scene_client_t *)model->user_data;
    if (!client) {
        BT_ERR("%s, Time Scene Client user_data is NULL", __func__);
        return;
    }

    internal = (time_scene_internal_data_t *)client->internal_data;
    if (!internal) {
        BT_ERR("%s, Time Scene Client internal_data is NULL", __func__);
        return;
    }

    rsp = ctx->recv_op;
    switch (rsp) {
    case BLE_MESH_MODEL_OP_TIME_STATUS: {
        struct bt_mesh_time_status *status = NULL;
        if (buf->len != 5 && buf->len != 10) {
            BT_ERR("%s, Invalid Time Status length %d", __func__, buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_time_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        memcpy(status->tai_seconds, buf->data, 5);
        net_buf_simple_pull(buf, 5);
        status->sub_second = net_buf_simple_pull_u8(buf);
        status->uncertainty = net_buf_simple_pull_u8(buf);
        u16_t temp = net_buf_simple_pull_le16(buf);
        status->time_authority = temp & BIT(0);
        status->tai_utc_delta = temp >> 15;
        status->time_zone_offset = net_buf_simple_pull_u8(buf);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_time_status);
        break;
    }
    case BLE_MESH_MODEL_OP_TIME_ZONE_STATUS: {
        struct bt_mesh_time_zone_status *status = NULL;
        if (buf->len != 7) {
            BT_ERR("%s, Invalid Time Zone Status length %d", __func__, buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_time_zone_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        status->time_zone_offset_curr = net_buf_simple_pull_u8(buf);
        status->time_zone_offset_new = net_buf_simple_pull_u8(buf);
        memcpy(status->tai_zone_change, buf->data, 5);
        net_buf_simple_pull(buf, 5);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_time_zone_status);
        break;
    }
    case BLE_MESH_MODEL_OP_TAI_UTC_DELTA_STATUS: {
        struct bt_mesh_tai_utc_delta_status *status = NULL;
        if (buf->len != 9) {
            BT_ERR("%s, Invalid TAI UTC Delta Status length %d", __func__, buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_tai_utc_delta_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        u16_t temp = net_buf_simple_pull_le16(buf);
        status->tai_utc_delta_curr = temp & BIT_MASK(15);
        status->padding_1 = (temp >> 15) & BIT(0);
        temp = net_buf_simple_pull_le16(buf);
        status->tai_utc_delta_new = temp & BIT_MASK(15);
        status->padding_2 = (temp >> 15) & BIT(0);
        memcpy(status->tai_delta_change, buf->data, 5);
        net_buf_simple_pull(buf, 5);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_tai_utc_delta_status);
        break;
    }
    case BLE_MESH_MODEL_OP_TIME_ROLE_STATUS: {
        struct bt_mesh_time_role_status *status = NULL;
        if (buf->len != 1) {
            BT_ERR("%s, Invalid Time Role Status length %d", __func__, buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_time_role_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        status->time_role = net_buf_simple_pull_u8(buf);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_time_role_status);
        break;
    }
    case BLE_MESH_MODEL_OP_SCENE_STATUS: {
        struct bt_mesh_scene_status *status = NULL;
        if (buf->len != 3 && buf->len != 6) {
            BT_ERR("%s, Invalid Scene Status length %d", __func__, buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_scene_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        status->status_code = net_buf_simple_pull_u8(buf);
        status->current_scene = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en = true;
            status->target_scene = net_buf_simple_pull_le16(buf);
            status->remain_time = net_buf_simple_pull_u8(buf);
        }
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_scene_status);
        break;
    }
    case BLE_MESH_MODEL_OP_SCENE_REGISTER_STATUS: {
        struct bt_mesh_scene_register_status *status = NULL;
        status = osi_calloc(sizeof(struct bt_mesh_scene_register_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        status->status_code = net_buf_simple_pull_u8(buf);
        status->current_scene = net_buf_simple_pull_le16(buf);
        status->scenes = bt_mesh_alloc_buf(buf->len);
        if (!status->scenes) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            osi_free(status);
            return;
        }
        net_buf_simple_add_mem(status->scenes, buf->data, buf->len);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_scene_register_status);
        break;
    }
    case BLE_MESH_MODEL_OP_SCHEDULER_STATUS: {
        struct bt_mesh_scheduler_status *status = NULL;
        if (buf->len != 2) {
            BT_ERR("%s, Invalid Scheduler Status length %d", __func__, buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_scheduler_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        status->schedules = net_buf_simple_pull_le16(buf);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_scheduler_status);
        break;
    }
    case BLE_MESH_MODEL_OP_SCHEDULER_ACT_STATUS: {
        struct bt_mesh_scheduler_act_status *status = NULL;
        if (buf->len != 10) {
            BT_ERR("%s, Invalid Scheduler Action Status length %d", __func__, buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_scheduler_act_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        memcpy(status, buf->data, offsetof(struct bt_mesh_scheduler_act_status, scene_number));
        net_buf_simple_pull(buf, offsetof(struct bt_mesh_scheduler_act_status, scene_number));
        status->scene_number = net_buf_simple_pull_le16(buf);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_scheduler_act_status);
        break;
    }
    default:
        BT_ERR("%s, Not a Time Scene Status message opcode", __func__);
        return;
    }

    buf->data = val;
    buf->len = len;
    node = bt_mesh_is_client_recv_publish_msg(model, ctx, buf, true);
    if (!node) {
        BT_DBG("Unexpected time scene status message 0x%x", rsp);
    } else {
        switch (node->opcode) {
        case BLE_MESH_MODEL_OP_TIME_GET:
        case BLE_MESH_MODEL_OP_TIME_ZONE_GET:
        case BLE_MESH_MODEL_OP_TAI_UTC_DELTA_GET:
        case BLE_MESH_MODEL_OP_TIME_ROLE_GET:
        case BLE_MESH_MODEL_OP_SCENE_GET:
        case BLE_MESH_MODEL_OP_SCENE_REGISTER_GET:
        case BLE_MESH_MODEL_OP_SCHEDULER_GET:
        case BLE_MESH_MODEL_OP_SCHEDULER_ACT_GET:
            evt = BTC_BLE_MESH_EVT_TIME_SCENE_CLIENT_GET_STATE;
            break;
        case BLE_MESH_MODEL_OP_TIME_SET:
        case BLE_MESH_MODEL_OP_TIME_ZONE_SET:
        case BLE_MESH_MODEL_OP_TAI_UTC_DELTA_SET:
        case BLE_MESH_MODEL_OP_TIME_ROLE_SET:
        case BLE_MESH_MODEL_OP_SCENE_STORE:
        case BLE_MESH_MODEL_OP_SCENE_RECALL:
        case BLE_MESH_MODEL_OP_SCENE_DELETE:
        case BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET:
            evt = BTC_BLE_MESH_EVT_TIME_SCENE_CLIENT_SET_STATE;
            break;
        default:
            break;
        }

        bt_mesh_time_scene_client_cb_evt_to_btc(node->opcode, evt, model, ctx, val, len);
        // Don't forget to release the node at the end.
        bt_mesh_client_free_node(&internal->queue, node);
    }

    switch (rsp) {
    case BLE_MESH_MODEL_OP_SCENE_REGISTER_STATUS: {
        struct bt_mesh_scene_register_status *status;
        status = (struct bt_mesh_scene_register_status *)val;
        bt_mesh_free_buf(status->scenes);
        break;
    }
    default:
        break;
    }

    osi_free(val);

    return;
}

const struct bt_mesh_model_op time_cli_op[] = {
    { BLE_MESH_MODEL_OP_TIME_STATUS,          5, time_scene_status },
    { BLE_MESH_MODEL_OP_TIME_ZONE_STATUS,     7, time_scene_status },
    { BLE_MESH_MODEL_OP_TAI_UTC_DELTA_STATUS, 9, time_scene_status },
    { BLE_MESH_MODEL_OP_TIME_ROLE_STATUS,     1, time_scene_status },
    BLE_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op scene_cli_op[] = {
    { BLE_MESH_MODEL_OP_SCENE_STATUS,          3, time_scene_status },
    { BLE_MESH_MODEL_OP_SCENE_REGISTER_STATUS, 3, time_scene_status },
    BLE_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op scheduler_cli_op[] = {
    { BLE_MESH_MODEL_OP_SCHEDULER_STATUS,     2,  time_scene_status },
    { BLE_MESH_MODEL_OP_SCHEDULER_ACT_STATUS, 10, time_scene_status },
    BLE_MESH_MODEL_OP_END,
};

static int time_scene_get_state(bt_mesh_client_common_param_t *common, void *value)
{
    NET_BUF_SIMPLE_DEFINE(msg, BLE_MESH_SCENE_GET_STATE_MSG_LEN);
    int err;

    bt_mesh_model_msg_init(&msg, common->opcode);

    if (value) {
        switch (common->opcode) {
        case BLE_MESH_MODEL_OP_SCHEDULER_ACT_GET: {
            struct bt_mesh_scheduler_act_get *get;
            get = (struct bt_mesh_scheduler_act_get *)value;
            net_buf_simple_add_u8(&msg, get->index);
            break;
        }
        default:
            BT_DBG("This time scene message should be sent with NULL get pointer");
            break;
        }
    }

    err = bt_mesh_client_send_msg(common->model, common->opcode, &common->ctx, &msg,
                                  timeout_handler, common->msg_timeout, true,
                                  common->cb, common->cb_data);
    if (err) {
        BT_ERR("%s, Failed to send Time Scene Get message (err %d)", __func__, err);
    }

    return err;
}

static int time_scene_set_state(bt_mesh_client_common_param_t *common,
                                void *value, u16_t value_len, bool need_ack)
{
    struct net_buf_simple *msg = NULL;
    int err;

    msg = bt_mesh_alloc_buf(value_len);
    if (!msg) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        return -ENOMEM;
    }

    bt_mesh_model_msg_init(msg, common->opcode);

    switch (common->opcode) {
    case BLE_MESH_MODEL_OP_TIME_SET: {
        struct bt_mesh_time_set *set;
        set = (struct bt_mesh_time_set *)value;
        net_buf_simple_add_mem(msg, set->tai_seconds, 5);
        net_buf_simple_add_u8(msg, set->sub_second);
        net_buf_simple_add_u8(msg, set->uncertainty);
        net_buf_simple_add_le16(msg, set->tai_utc_delta << 1 | set->time_authority);
        net_buf_simple_add_u8(msg, set->time_zone_offset);
        break;
    }
    case BLE_MESH_MODEL_OP_TIME_ZONE_SET: {
        struct bt_mesh_time_zone_set *set;
        set = (struct bt_mesh_time_zone_set *)value;
        net_buf_simple_add_u8(msg, set->time_zone_offset_new);
        net_buf_simple_add_mem(msg, set->tai_zone_change, 5);
        break;
    }
    case BLE_MESH_MODEL_OP_TAI_UTC_DELTA_SET: {
        struct bt_mesh_tai_utc_delta_set *set;
        set = (struct bt_mesh_tai_utc_delta_set *)value;
        net_buf_simple_add_le16(msg, set->padding << 15 | set->tai_utc_delta_new);
        net_buf_simple_add_mem(msg, set->tai_delta_change, 5);
        break;
    }
    case BLE_MESH_MODEL_OP_TIME_ROLE_SET: {
        struct bt_mesh_time_role_set *set;
        set = (struct bt_mesh_time_role_set *)value;
        net_buf_simple_add_u8(msg, set->time_role);
        break;
    }
    case BLE_MESH_MODEL_OP_SCENE_STORE:
    case BLE_MESH_MODEL_OP_SCENE_STORE_UNACK: {
        struct bt_mesh_scene_store *set;
        set = (struct bt_mesh_scene_store *)value;
        net_buf_simple_add_le16(msg, set->scene_number);
        break;
    }
    case BLE_MESH_MODEL_OP_SCENE_RECALL:
    case BLE_MESH_MODEL_OP_SCENE_RECALL_UNACK: {
        struct bt_mesh_scene_recall *set;
        set = (struct bt_mesh_scene_recall *)value;
        net_buf_simple_add_le16(msg, set->scene_number);
        net_buf_simple_add_u8(msg,   set->tid);
        if (set->op_en) {
            net_buf_simple_add_u8(msg, set->trans_time);
            net_buf_simple_add_u8(msg, set->delay);
        }
        break;
    }
    case BLE_MESH_MODEL_OP_SCENE_DELETE:
    case BLE_MESH_MODEL_OP_SCENE_DELETE_UNACK: {
        struct bt_mesh_scene_delete *set;
        set = (struct bt_mesh_scene_delete *)value;
        net_buf_simple_add_le16(msg, set->scene_number);
        break;
    }
    case BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET:
    case BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET_UNACK: {
        struct bt_mesh_scheduler_act_set *set;
        set = (struct bt_mesh_scheduler_act_set *)value;
        net_buf_simple_add_mem(msg, set, offsetof(struct bt_mesh_scheduler_act_set, scene_number));
        net_buf_simple_add_le16(msg, set->scene_number);
        break;
    }
    default:
        BT_ERR("%s, Not a Time Scene Client set message opcode", __func__);
        err = -EINVAL;
        goto end;
    }

    err = bt_mesh_client_send_msg(common->model, common->opcode, &common->ctx, msg,
                                  timeout_handler, common->msg_timeout, need_ack,
                                  common->cb, common->cb_data);
    if (err) {
        BT_ERR("%s, Failed to send Time Scene Set message (err %d)", __func__, err);
    }

end:
    bt_mesh_free_buf(msg);
    return err;
}

int bt_mesh_time_scene_client_get_state(bt_mesh_client_common_param_t *common, void *get, void *status)
{
    bt_mesh_time_scene_client_t *client = NULL;

    if (!common || !common->model) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    client = (bt_mesh_time_scene_client_t *)common->model->user_data;
    if (!client || !client->internal_data) {
        BT_ERR("%s, Time Scene Client user data is NULL", __func__);
        return -EINVAL;
    }

    switch (common->opcode) {
    case BLE_MESH_MODEL_OP_TIME_GET:
    case BLE_MESH_MODEL_OP_TIME_ZONE_GET:
    case BLE_MESH_MODEL_OP_TAI_UTC_DELTA_GET:
    case BLE_MESH_MODEL_OP_TIME_ROLE_GET:
    case BLE_MESH_MODEL_OP_SCENE_GET:
    case BLE_MESH_MODEL_OP_SCENE_REGISTER_GET:
    case BLE_MESH_MODEL_OP_SCHEDULER_GET:
        break;
    case BLE_MESH_MODEL_OP_SCHEDULER_ACT_GET:
        if (!get) {
            BT_ERR("%s, Scheduler action index is NULL", __func__);
            return -EINVAL;
        }
        break;
    default:
        BT_ERR("%s, Not a Time Scene Client Get message opcode", __func__);
        return -EINVAL;
    }

    return time_scene_get_state(common, get);
}

int bt_mesh_time_scene_client_set_state(bt_mesh_client_common_param_t *common, void *set, void *status)
{
    bt_mesh_time_scene_client_t *client = NULL;
    u16_t length = 0;
    bool need_ack = false;

    if (!common || !common->model || !set) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    client = (bt_mesh_time_scene_client_t *)common->model->user_data;
    if (!client || !client->internal_data) {
        BT_ERR("%s, Time Scene Client user data is NULL", __func__);
        return -EINVAL;
    }

    switch (common->opcode) {
    case BLE_MESH_MODEL_OP_TIME_SET:
        need_ack = true;
        length = BLE_MESH_TIME_SET_MSG_LEN;
        break;
    case BLE_MESH_MODEL_OP_TIME_ZONE_SET:
        need_ack = true;
        length = BLE_MESH_TIME_ZONE_SET_MSG_LEN;
        break;
    case BLE_MESH_MODEL_OP_TAI_UTC_DELTA_SET: {
        struct bt_mesh_tai_utc_delta_set *value;
        value = (struct bt_mesh_tai_utc_delta_set *)set;
        if (value->padding) {
            BT_ERR("%s, Non-zero padding value is prohibited", __func__);
            return -EINVAL;
        }
        need_ack = true;
        length = BLE_MESH_TAI_UTC_DELTA_SET_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_TIME_ROLE_SET: {
        struct bt_mesh_time_role_set *value;
        value = (struct bt_mesh_time_role_set *)set;
        if (value->time_role > 0x03) {
            BT_ERR("%s, Time role value is prohibited", __func__);
            return -EINVAL;
        }
        need_ack = true;
        length = BLE_MESH_TIME_ROLE_SET_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_SCENE_STORE:
        need_ack = true;
    case BLE_MESH_MODEL_OP_SCENE_STORE_UNACK: {
        struct bt_mesh_scene_store *value;
        value = (struct bt_mesh_scene_store *)set;
        if (!value->scene_number) {
            BT_ERR("%s, Scene store scene_number 0x0000 is prohibited", __func__);
            return -EINVAL;
        }
        length = BLE_MESH_SCENE_STORE_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_SCENE_RECALL:
        need_ack = true;
    case BLE_MESH_MODEL_OP_SCENE_RECALL_UNACK: {
        struct bt_mesh_scene_recall *value;
        value = (struct bt_mesh_scene_recall *)set;
        if (!value->scene_number) {
            BT_ERR("%s, Scene recall scene_number 0x0000 is prohibited", __func__);
            return -EINVAL;
        }
        if (value->op_en) {
            if ((value->trans_time & 0x3F) > 0x3E) {
                BT_ERR("%s, Invalid Scene Recall transition time", __func__);
                return -EINVAL;
            }
        }
        length = BLE_MESH_SCENE_RECALL_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_SCENE_DELETE:
        need_ack = true;
    case BLE_MESH_MODEL_OP_SCENE_DELETE_UNACK: {
        length = BLE_MESH_SCENE_DELETE_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET_UNACK: {
        struct bt_mesh_scheduler_act_set *value;
        value = (struct bt_mesh_scheduler_act_set *)set;
        if (value->year > 0x64) {
            BT_ERR("%s, Scheduler register year value is prohibited", __func__);
            return -EINVAL;
        }
        if (value->hour > 0x19) {
            BT_ERR("%s, Scheduler register hour value is prohibited", __func__);
            return -EINVAL;
        }
        length = BLE_MESH_SCHEDULER_ACT_SET_MSG_LEN;
        break;
    }
    default:
        BT_ERR("%s, Not a Time Scene Set message opcode", __func__);
        return -EINVAL;
    }

    return time_scene_set_state(common, set, length, need_ack);
}

static int time_scene_client_init(struct bt_mesh_model *model, bool primary)
{
    time_scene_internal_data_t *internal = NULL;
    bt_mesh_time_scene_client_t *client = NULL;

    BT_DBG("primary %u", primary);

    if (!model) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    client = (bt_mesh_time_scene_client_t *)model->user_data;
    if (!client) {
        BT_ERR("%s, Time Scene Client user_data is NULL", __func__);
        return -EINVAL;
    }

    /* TODO: call osi_free() when deinit function is invoked*/
    internal = osi_calloc(sizeof(time_scene_internal_data_t));
    if (!internal) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        return -ENOMEM;
    }

    sys_slist_init(&internal->queue);

    client->model = model;
    client->op_pair_size = ARRAY_SIZE(time_scene_op_pair);
    client->op_pair = time_scene_op_pair;
    client->internal_data = internal;

    return 0;
}

int bt_mesh_time_cli_init(struct bt_mesh_model *model, bool primary)
{
    return time_scene_client_init(model, primary);
}

int bt_mesh_scene_cli_init(struct bt_mesh_model *model, bool primary)
{
    return time_scene_client_init(model, primary);
}

int bt_mesh_scheduler_cli_init(struct bt_mesh_model *model, bool primary)
{
    return time_scene_client_init(model, primary);
}
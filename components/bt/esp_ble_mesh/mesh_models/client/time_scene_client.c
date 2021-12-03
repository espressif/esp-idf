/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "btc_ble_mesh_time_scene_model.h"

#include "mesh_config.h"
#include "model_opcode.h"

#if CONFIG_BLE_MESH_TIME_SCENE_CLIENT
#include "time_scene_client.h"

/* The followings are the macro definitions of Time Scene client
 * model message length, and a message is composed of 3 parts:
 * Opcode + Payload + MIC
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

static bt_mesh_mutex_t time_scene_client_lock;

static inline void bt_mesh_time_scene_client_mutex_new(void)
{
    if (!time_scene_client_lock.mutex) {
        bt_mesh_mutex_create(&time_scene_client_lock);
    }
}

#if CONFIG_BLE_MESH_DEINIT
static inline void bt_mesh_time_scene_client_mutex_free(void)
{
    bt_mesh_mutex_free(&time_scene_client_lock);
}
#endif /* CONFIG_BLE_MESH_DEINIT */

static inline void bt_mesh_time_scene_client_lock(void)
{
    bt_mesh_mutex_lock(&time_scene_client_lock);
}

static inline void bt_mesh_time_scene_client_unlock(void)
{
    bt_mesh_mutex_unlock(&time_scene_client_lock);
}

static void timeout_handler(struct k_work *work)
{
    struct k_delayed_work *timer = NULL;
    bt_mesh_client_node_t *node = NULL;
    struct bt_mesh_msg_ctx ctx = {0};
    uint32_t opcode = 0U;

    BT_WARN("Receive time scene status message timeout");

    bt_mesh_time_scene_client_lock();

    timer = CONTAINER_OF(work, struct k_delayed_work, work);

    if (timer && !k_delayed_work_free(timer)) {
        node = CONTAINER_OF(work, bt_mesh_client_node_t, timer.work);
        if (node) {
            memcpy(&ctx, &node->ctx, sizeof(ctx));
            opcode = node->opcode;
            bt_mesh_client_free_node(node);
            bt_mesh_time_scene_client_cb_evt_to_btc(
                opcode, BTC_BLE_MESH_EVT_TIME_SCENE_CLIENT_TIMEOUT, ctx.model, &ctx, NULL, 0);
        }
    }

    bt_mesh_time_scene_client_unlock();

    return;
}

static void time_scene_status(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    bt_mesh_client_node_t *node = NULL;
    uint8_t *val = NULL;
    uint8_t evt = 0xFF;
    size_t len = 0U;

    BT_DBG("len %d, bytes %s", buf->len, bt_hex(buf->data, buf->len));

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_TIME_STATUS: {
        struct bt_mesh_time_status *status = NULL;
        if (buf->len != 5 && buf->len != 10) {
            BT_ERR("Invalid Time Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_time_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        memcpy(status->tai_seconds, buf->data, 5);
        net_buf_simple_pull(buf, 5);
        status->sub_second = net_buf_simple_pull_u8(buf);
        status->uncertainty = net_buf_simple_pull_u8(buf);
        uint16_t temp = net_buf_simple_pull_le16(buf);
        status->time_authority = temp & BIT(0);
        status->tai_utc_delta = temp >> 15;
        status->time_zone_offset = net_buf_simple_pull_u8(buf);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_time_status);
        break;
    }
    case BLE_MESH_MODEL_OP_TIME_ZONE_STATUS: {
        struct bt_mesh_time_zone_status *status = NULL;
        if (buf->len != 7) {
            BT_ERR("Invalid Time Zone Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_time_zone_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->time_zone_offset_curr = net_buf_simple_pull_u8(buf);
        status->time_zone_offset_new = net_buf_simple_pull_u8(buf);
        memcpy(status->tai_zone_change, buf->data, 5);
        net_buf_simple_pull(buf, 5);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_time_zone_status);
        break;
    }
    case BLE_MESH_MODEL_OP_TAI_UTC_DELTA_STATUS: {
        struct bt_mesh_tai_utc_delta_status *status = NULL;
        if (buf->len != 9) {
            BT_ERR("Invalid TAI UTC Delta Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_tai_utc_delta_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        uint16_t temp = net_buf_simple_pull_le16(buf);
        status->tai_utc_delta_curr = temp & BIT_MASK(15);
        status->padding_1 = (temp >> 15) & BIT(0);
        temp = net_buf_simple_pull_le16(buf);
        status->tai_utc_delta_new = temp & BIT_MASK(15);
        status->padding_2 = (temp >> 15) & BIT(0);
        memcpy(status->tai_delta_change, buf->data, 5);
        net_buf_simple_pull(buf, 5);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_tai_utc_delta_status);
        break;
    }
    case BLE_MESH_MODEL_OP_TIME_ROLE_STATUS: {
        struct bt_mesh_time_role_status *status = NULL;
        if (buf->len != 1) {
            BT_ERR("Invalid Time Role Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_time_role_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->time_role = net_buf_simple_pull_u8(buf);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_time_role_status);
        break;
    }
    case BLE_MESH_MODEL_OP_SCENE_STATUS: {
        struct bt_mesh_scene_status *status = NULL;
        if (buf->len != 3 && buf->len != 6) {
            BT_ERR("Invalid Scene Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_scene_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->status_code = net_buf_simple_pull_u8(buf);
        status->current_scene = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en = true;
            status->target_scene = net_buf_simple_pull_le16(buf);
            status->remain_time = net_buf_simple_pull_u8(buf);
        }
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_scene_status);
        break;
    }
    case BLE_MESH_MODEL_OP_SCENE_REGISTER_STATUS: {
        struct bt_mesh_scene_register_status *status = NULL;
        status = bt_mesh_calloc(sizeof(struct bt_mesh_scene_register_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->status_code = net_buf_simple_pull_u8(buf);
        status->current_scene = net_buf_simple_pull_le16(buf);
        status->scenes = bt_mesh_alloc_buf(buf->len);
        if (!status->scenes) {
            BT_ERR("%s, Out of memory", __func__);
            bt_mesh_free(status);
            return;
        }
        net_buf_simple_add_mem(status->scenes, buf->data, buf->len);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_scene_register_status);
        break;
    }
    case BLE_MESH_MODEL_OP_SCHEDULER_STATUS: {
        struct bt_mesh_scheduler_status *status = NULL;
        if (buf->len != 2) {
            BT_ERR("Invalid Scheduler Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_scheduler_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->schedules = net_buf_simple_pull_le16(buf);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_scheduler_status);
        break;
    }
    case BLE_MESH_MODEL_OP_SCHEDULER_ACT_STATUS: {
        struct bt_mesh_scheduler_act_status *status = NULL;
        if (buf->len != 10) {
            BT_ERR("Invalid Scheduler Action Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_scheduler_act_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        memcpy(status, buf->data, offsetof(struct bt_mesh_scheduler_act_status, scene_number));
        net_buf_simple_pull(buf, offsetof(struct bt_mesh_scheduler_act_status, scene_number));
        status->scene_number = net_buf_simple_pull_le16(buf);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_scheduler_act_status);
        break;
    }
    default:
        BT_ERR("Invalid Time Scene Status opcode 0x%04x", ctx->recv_op);
        return;
    }

    buf->data = val;
    buf->len = len;

    bt_mesh_time_scene_client_lock();

    node = bt_mesh_is_client_recv_publish_msg(model, ctx, buf, true);
    if (!node) {
        BT_DBG("Unexpected Time Scene Status 0x%04x", ctx->recv_op);
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

        if (!k_delayed_work_free(&node->timer)) {
            uint32_t opcode = node->opcode;
            bt_mesh_client_free_node(node);
            bt_mesh_time_scene_client_cb_evt_to_btc(opcode, evt, model, ctx, val, len);
        }
    }

    bt_mesh_time_scene_client_unlock();

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_SCENE_REGISTER_STATUS: {
        struct bt_mesh_scene_register_status *status;
        status = (struct bt_mesh_scene_register_status *)val;
        bt_mesh_free_buf(status->scenes);
        break;
    }
    default:
        break;
    }

    bt_mesh_free(val);

    return;
}

const struct bt_mesh_model_op bt_mesh_time_cli_op[] = {
    { BLE_MESH_MODEL_OP_TIME_STATUS,          5, time_scene_status },
    { BLE_MESH_MODEL_OP_TIME_ZONE_STATUS,     7, time_scene_status },
    { BLE_MESH_MODEL_OP_TAI_UTC_DELTA_STATUS, 9, time_scene_status },
    { BLE_MESH_MODEL_OP_TIME_ROLE_STATUS,     1, time_scene_status },
    BLE_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op bt_mesh_scene_cli_op[] = {
    { BLE_MESH_MODEL_OP_SCENE_STATUS,          3, time_scene_status },
    { BLE_MESH_MODEL_OP_SCENE_REGISTER_STATUS, 3, time_scene_status },
    BLE_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op bt_mesh_scheduler_cli_op[] = {
    { BLE_MESH_MODEL_OP_SCHEDULER_STATUS,     2,  time_scene_status },
    { BLE_MESH_MODEL_OP_SCHEDULER_ACT_STATUS, 10, time_scene_status },
    BLE_MESH_MODEL_OP_END,
};

static int time_scene_get_state(bt_mesh_client_common_param_t *common, void *value)
{
    NET_BUF_SIMPLE_DEFINE(msg, BLE_MESH_SCENE_GET_STATE_MSG_LEN);

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
            BT_DBG("No parameters for Time Scene Get 0x%04x", common->opcode);
            break;
        }
    }

    return bt_mesh_client_send_msg(common, &msg, true, timeout_handler);
}

static int time_scene_set_state(bt_mesh_client_common_param_t *common,
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
        BT_ERR("Invalid Time Scene Set opcode 0x%04x", common->opcode);
        err = -EINVAL;
        goto end;
    }

    err = bt_mesh_client_send_msg(common, msg, need_ack, timeout_handler);

end:
    bt_mesh_free_buf(msg);
    return err;
}

int bt_mesh_time_scene_client_get_state(bt_mesh_client_common_param_t *common, void *get)
{
    bt_mesh_time_scene_client_t *client = NULL;

    if (!common || !common->model) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    client = (bt_mesh_time_scene_client_t *)common->model->user_data;
    if (!client || !client->internal_data) {
        BT_ERR("Invalid Time Scene client data");
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
            BT_ERR("Invalid Scheduler Action Get");
            return -EINVAL;
        }
        break;
    default:
        BT_ERR("Invalid Time Scene Get opcode 0x%04x", common->opcode);
        return -EINVAL;
    }

    return time_scene_get_state(common, get);
}

int bt_mesh_time_scene_client_set_state(bt_mesh_client_common_param_t *common, void *set)
{
    bt_mesh_time_scene_client_t *client = NULL;
    uint16_t length = 0U;
    bool need_ack = false;

    if (!common || !common->model || !set) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    client = (bt_mesh_time_scene_client_t *)common->model->user_data;
    if (!client || !client->internal_data) {
        BT_ERR("Invalid Time Scene client data");
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
            BT_ERR("Non-zero padding value is prohibited");
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
            BT_ERR("Time role 0x%02x is prohibited", value->time_role);
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
            BT_ERR("Scene Store scene number 0x0000 is prohibited");
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
            BT_ERR("Scene Recall scene number 0x0000 is prohibited");
            return -EINVAL;
        }
        if (value->op_en) {
            if ((value->trans_time & 0x3F) > 0x3E) {
                BT_ERR("Invalid Scene Recall transition time");
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
            BT_ERR("Scheduler Register year 0x%02x is prohibited", value->year);
            return -EINVAL;
        }
        if (value->hour > 0x19) {
            BT_ERR("Scheduler Register hour 0x%02x is prohibited", value->hour);
            return -EINVAL;
        }
        length = BLE_MESH_SCHEDULER_ACT_SET_MSG_LEN;
        break;
    }
    default:
        BT_ERR("Invalid Time Scene Set opcode 0x%04x", common->opcode);
        return -EINVAL;
    }

    return time_scene_set_state(common, set, length, need_ack);
}

static int time_scene_client_init(struct bt_mesh_model *model)
{
    time_scene_internal_data_t *internal = NULL;
    bt_mesh_time_scene_client_t *client = NULL;

    if (!model) {
        BT_ERR("Invalid Time Scene client model");
        return -EINVAL;
    }

    client = (bt_mesh_time_scene_client_t *)model->user_data;
    if (!client) {
        BT_ERR("No Time Scene client context provided");
        return -EINVAL;
    }

    if (!client->internal_data) {
        internal = bt_mesh_calloc(sizeof(time_scene_internal_data_t));
        if (!internal) {
            BT_ERR("%s, Out of memory", __func__);
            return -ENOMEM;
        }

        sys_slist_init(&internal->queue);

        client->model = model;
        client->op_pair_size = ARRAY_SIZE(time_scene_op_pair);
        client->op_pair = time_scene_op_pair;
        client->internal_data = internal;
    } else {
        bt_mesh_client_clear_list(client->internal_data);
    }

    bt_mesh_time_scene_client_mutex_new();

    return 0;
}

#if CONFIG_BLE_MESH_DEINIT
static int time_scene_client_deinit(struct bt_mesh_model *model)
{
    bt_mesh_time_scene_client_t *client = NULL;

    if (!model) {
        BT_ERR("Invalid Time Scene client model");
        return -EINVAL;
    }

    client = (bt_mesh_time_scene_client_t *)model->user_data;
    if (!client) {
        BT_ERR("No Time Scene client context provided");
        return -EINVAL;
    }

    if (client->internal_data) {
        /* Remove items from the list */
        bt_mesh_client_clear_list(client->internal_data);

        /* Free the allocated internal data */
        bt_mesh_free(client->internal_data);
        client->internal_data = NULL;
    }

    bt_mesh_time_scene_client_mutex_free();

    return 0;
}
#endif /* CONFIG_BLE_MESH_DEINIT */

const struct bt_mesh_model_cb bt_mesh_time_scene_client_cb = {
    .init = time_scene_client_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = time_scene_client_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

#endif /* CONFIG_BLE_MESH_TIME_SCENE_CLIENT */

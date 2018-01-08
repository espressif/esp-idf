// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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

#include "mesh_types.h"
#include "mesh_kernel.h"
#include "mesh_trace.h"
#include "mesh.h"

#include "model_op.h"
#include "common.h"
#include "time_scene_client.h"

#include "sdkconfig.h"

#include "btc_ble_mesh_time_scene_client.h"

/** The following are the macro definitions of time and client
 *  scene model messages length, and a message is composed of
 *  three parts: Opcode + msg_value + MIC
 */
/* Time client messages length */

/* Scene client messages length */
#define BT_MESH_SCENE_STORE_MSG_LEN         (2 + 2 + 4)
#define BT_MESH_SCENE_RECALL_MSG_LEN        (2 + 5 + 4)
#define BT_MESH_SCENE_GET_MSG_LEN           (2 + 0 + 4)
#define BT_MESH_SCENE_REGISTER_GET_MSG_LEN  (2 + 0 + 4)
#define BT_MESH_SCENE_DELETE_MSG_LEN        (2 + 2 + 4)

/* Scheduler client messages length */

#define BT_MESH_SCENE_GET_STATE_MSG_LEN     (2 + 0 + 4)
#define BT_MESH_SCENE_ACT_STATE_MSG_LEN     (2 + 2 + 4)

static const bt_mesh_client_op_pair_t time_scene_op_pair[] = {
    { BT_MESH_MODEL_OP_SCENE_STORE,        BT_MESH_MODEL_OP_SCENE_REGISTER_STATUS },
    { BT_MESH_MODEL_OP_SCENE_RECALL,       BT_MESH_MODEL_OP_SCENE_STATUS          },
    { BT_MESH_MODEL_OP_SCENE_GET,          BT_MESH_MODEL_OP_SCENE_STATUS          },
    { BT_MESH_MODEL_OP_SCENE_REGISTER_GET, BT_MESH_MODEL_OP_SCENE_REGISTER_STATUS },
    { BT_MESH_MODEL_OP_SCENE_DELETE,       BT_MESH_MODEL_OP_SCENE_REGISTER_STATUS },
};

static void timeout_handler(struct k_work *work)
{
    bt_mesh_time_scene_client_t *client   = NULL;
    time_scene_internal_data_t  *internal = NULL;
    bt_mesh_client_node_t       *node     = NULL;

    BT_WARN("Receive time scene status message timeout");

    node = CONTAINER_OF(work, bt_mesh_client_node_t, timer.work);
    if (!node || !node->ctx.model) {
        BT_ERR("%s: node parameter is NULL", __func__);
        return;
    }

    client = (bt_mesh_time_scene_client_t *)node->ctx.model->user_data;
    if (!client) {
        BT_ERR("%s: model user_data is NULL", __func__);
        return;
    }

    internal = (time_scene_internal_data_t *)client->internal_data;
    if (!internal) {
        BT_ERR("%s: internal_data is NULL", __func__);
        return;
    }

    bt_mesh_callback_time_scene_status_to_btc(node->opcode, 0x03, node->ctx.model,
            &node->ctx, NULL, 0);

    sys_slist_find_and_remove(&internal->queue, &node->client_node);
    osi_free(node);

    return;
}

static void time_scene_status(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    bt_mesh_time_scene_client_t *client   = NULL;
    time_scene_internal_data_t  *internal = NULL;
    bt_mesh_client_node_t       *node     = NULL;
    u8_t  *val = NULL;
    u8_t   evt = 0xFF;
    u32_t  rsp = 0;
    size_t len = 0;

    BT_DBG("%s: len %d, bytes %s", __func__, buf->len, bt_hex(buf->data, buf->len));

    client = (bt_mesh_time_scene_client_t *)model->user_data;
    if (!client) {
        BT_ERR("%s: model user_data is NULL", __func__);
        return;
    }

    internal = (time_scene_internal_data_t *)client->internal_data;
    if (!internal) {
        BT_ERR("%s: model internal_data is NULL", __func__);
        return;
    }

    rsp = ctx->recv_op;
    switch (rsp) {
    case BT_MESH_MODEL_OP_SCENE_STATUS: {
        struct bt_mesh_scene_status *status = NULL;
        if (buf->len != 3 && buf->len != 6) {
            BT_ERR("Wrong Scene status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_scene_status));
        if (!status) {
            BT_ERR("%s: allocate memory fail", __func__);
            return;
        }
        status->status_code   = net_buf_simple_pull_u8(buf);
        status->current_scene = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en        = true;
            status->target_scene = net_buf_simple_pull_le16(buf);
            status->remain_time  = net_buf_simple_pull_u8(buf);
        }
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_scene_status);
        break;
    }
    case BT_MESH_MODEL_OP_SCENE_REGISTER_STATUS: {
        struct bt_mesh_scene_register_status *status = NULL;
        status = osi_calloc(sizeof(struct bt_mesh_scene_register_status));
        if (!status) {
            BT_ERR("%s: allocate memory fail", __func__);
            return;
        }
        status->status_code   = net_buf_simple_pull_u8(buf);
        status->current_scene = net_buf_simple_pull_le16(buf);
        status->scenes = bt_mesh_alloc_buf(buf->len);
        if (!status->scenes) {
            BT_ERR("%s: allocate memory fail", __func__);
            osi_free(status);
            return;
        }
        net_buf_simple_init(status->scenes, 0);
        net_buf_simple_add_mem(status->scenes, buf->data, buf->len);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_scene_register_status);
        break;
    }
    default:
        BT_ERR("Not a time scene status message opcode");
        return;
    }

    buf->data = val;
    buf->len  = len;
    node = bt_mesh_is_model_message_publish(model, ctx, buf, true);
    if (!node) {
        BT_DBG("Unexpected time scene status message 0x%x", rsp);
    } else {
        switch (node->opcode) {
        case BT_MESH_MODEL_OP_SCENE_GET:
        case BT_MESH_MODEL_OP_SCENE_REGISTER_GET:
            evt = 0x00;
            break;
        case BT_MESH_MODEL_OP_SCENE_STORE:
        case BT_MESH_MODEL_OP_SCENE_RECALL:
        case BT_MESH_MODEL_OP_SCENE_DELETE:
            evt = 0x01;
            break;
        default:
            break;
        }

        bt_mesh_callback_time_scene_status_to_btc(node->opcode, evt, model, ctx, val, len);
        // Don't forget to release the node at the end.
        bt_mesh_client_free_node(&internal->queue, node);
    }

    switch (rsp) {
    case BT_MESH_MODEL_OP_SCENE_REGISTER_STATUS: {
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

const struct bt_mesh_model_op scene_cli_op[] = {
    { BT_MESH_MODEL_OP_SCENE_STATUS,          3, time_scene_status },
    { BT_MESH_MODEL_OP_SCENE_REGISTER_STATUS, 3, time_scene_status },
    BT_MESH_MODEL_OP_END,
};

static int time_scene_get_state(struct bt_mesh_common_param *common, void *value)
{
    struct net_buf_simple *msg = NET_BUF_SIMPLE(BT_MESH_SCENE_GET_STATE_MSG_LEN);
    int err;

    bt_mesh_model_msg_init(msg, common->opcode);

    if (value) {
        /* TODO: if other time scene client model get messages contain parameters */
    }

    err = bt_mesh_client_send_msg(common->model, common->opcode, &common->ctx, msg,
                                  timeout_handler, common->msg_timeout, true,
                                  common->cb, common->cb_data);
    if (err) {
        BT_ERR("Time scene get message send failed (err %d)", err);
    }

    return err;
}

static int time_scene_set_state(struct bt_mesh_common_param *common,
                                void *value, u16_t value_len, bool need_ack)
{
    struct net_buf_simple *msg = NULL;
    int err;

    msg = bt_mesh_alloc_buf(value_len);
    if (!msg) {
        BT_ERR("Time scene allocate memory fail");
        return -ENOMEM;
    }

    bt_mesh_model_msg_init(msg, common->opcode);

    switch (common->opcode) {
    case BT_MESH_MODEL_OP_SCENE_STORE:
    case BT_MESH_MODEL_OP_SCENE_STORE_UNACK: {
        struct bt_mesh_scene_store *set;
        set = (struct bt_mesh_scene_store *)value;
        net_buf_simple_add_le16(msg, set->scene_number);
        break;
    }
    case BT_MESH_MODEL_OP_SCENE_RECALL:
    case BT_MESH_MODEL_OP_SCENE_RECALL_UNACK: {
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
    case BT_MESH_MODEL_OP_SCENE_DELETE:
    case BT_MESH_MODEL_OP_SCENE_DELETE_UNACK: {
        struct bt_mesh_scene_delete *set;
        set = (struct bt_mesh_scene_delete *)value;
        net_buf_simple_add_le16(msg, set->scene_number);
        break;
    }
    default:
        BT_ERR("Not a generic client model set message opcode");
        err = -EINVAL;
        goto end;
    }

    err = bt_mesh_client_send_msg(common->model, common->opcode, &common->ctx, msg,
                                  timeout_handler, common->msg_timeout, need_ack,
                                  common->cb, common->cb_data);
    if (err) {
        BT_ERR("Time scene set message send failed (err %d)", err);
    }

end:
    bt_mesh_free_buf(msg);
    return err;
}

int bt_mesh_time_scene_client_get_state(struct bt_mesh_common_param *common, void *get, void *status)
{
    bt_mesh_time_scene_client_t *client = NULL;

    if (!common || !common->model) {
        BT_ERR("%s: common parameter is NULL", __func__);
        return -EINVAL;
    }

    client = (bt_mesh_time_scene_client_t *)common->model->user_data;
    if (!client || !client->internal_data) {
        BT_ERR("%s: client parameter is NULL", __func__);
        return -EINVAL;
    }

    switch (common->opcode) {
    case BT_MESH_MODEL_OP_SCENE_GET:
    case BT_MESH_MODEL_OP_SCENE_REGISTER_GET:
        break;
    default:
        BT_ERR("Not a time scene get message opcode");
        return -EINVAL;
    }

    return time_scene_get_state(common, get);
}

int bt_mesh_time_scene_client_set_state(struct bt_mesh_common_param *common, void *set, void *status)
{
    bt_mesh_time_scene_client_t *client = NULL;
    u16_t length   = 0;
    bool  need_ack = false;

    if (!common || !common->model || !set) {
        BT_ERR("%s: common parameter is NULL", __func__);
        return -EINVAL;
    }

    client = (bt_mesh_time_scene_client_t *)common->model->user_data;
    if (!client || !client->internal_data) {
        BT_ERR("%s: client parameter is NULL", __func__);
        return -EINVAL;
    }

    switch (common->opcode) {
    case BT_MESH_MODEL_OP_SCENE_STORE:
        need_ack = true;
    case BT_MESH_MODEL_OP_SCENE_STORE_UNACK: {
        struct bt_mesh_scene_store *value;
        value = (struct bt_mesh_scene_store *)set;
        if (!value->scene_number) {
            BT_ERR("Scene store scene_number 0x0000 is prohibited");
            return -EINVAL;
        }
        length = BT_MESH_SCENE_STORE_MSG_LEN;
        break;
    }
    case BT_MESH_MODEL_OP_SCENE_RECALL:
        need_ack = true;
    case BT_MESH_MODEL_OP_SCENE_RECALL_UNACK: {
        struct bt_mesh_scene_recall *value;
        value = (struct bt_mesh_scene_recall *)set;
        if (!value->scene_number) {
            BT_ERR("Scene recall scene_number 0x0000 is prohibited");
            return -EINVAL;
        }
        if (value->op_en) {
            if ((value->trans_time & 0x3F) > 0x3E) {
                BT_ERR("Scene recall transition time is bigger than 0x3E");
                return -EINVAL;
            }
        }
        length = BT_MESH_SCENE_RECALL_MSG_LEN;
        break;
    }
    case BT_MESH_MODEL_OP_SCENE_DELETE:
        need_ack = true;
    case BT_MESH_MODEL_OP_SCENE_DELETE_UNACK: {
        length = BT_MESH_SCENE_DELETE_MSG_LEN;
        break;
    }
    default:
        BT_ERR("Not a time scene set message opcode");
        return -EINVAL;
    }

    return time_scene_set_state(common, set, length, need_ack);
}

static int time_scene_client_init(struct bt_mesh_model *model, bool primary)
{
    bt_mesh_time_scene_client_t *client   = NULL;
    time_scene_internal_data_t  *internal = NULL;

    BT_DBG("primary %u", primary);

    if (!model) {
        BT_ERR("Time scene client model is NULL");
        return -EINVAL;
    }

    client = (bt_mesh_time_scene_client_t *)model->user_data;
    if (!client) {
        BT_ERR("Time scene client model user_data is NULL");
        return -EINVAL;
    }

    /* TODO: call osi_free() when deinit function is invoked*/
    internal = osi_calloc(sizeof(time_scene_internal_data_t));
    if (!internal) {
        BT_ERR("Allocate memory for time scene internal data fail");
        return -ENOMEM;
    }

    sys_slist_init(&internal->queue);

    client->model         = model;
    client->op_pair_size  = ARRAY_SIZE(time_scene_op_pair);
    client->op_pair       = time_scene_op_pair;
    client->internal_data = internal;

    return 0;
}

int bt_mesh_scene_cli_init(struct bt_mesh_model *model, bool primary)
{
    return time_scene_client_init(model, primary);
}
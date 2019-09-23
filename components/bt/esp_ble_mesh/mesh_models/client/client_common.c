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

#include "osi/allocator.h"

#include "mesh_access.h"
#include "mesh_buf.h"
#include "mesh_slist.h"
#include "mesh_main.h"

#include "mesh.h"
#include "client_common.h"

bt_mesh_client_node_t *bt_mesh_is_client_recv_publish_msg(
        struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        struct net_buf_simple *buf, bool need_pub)
{
    bt_mesh_client_internal_data_t *data = NULL;
    bt_mesh_client_user_data_t *cli = NULL;
    bt_mesh_client_node_t *node = NULL;
    u32_t rsp;

    if (!model || !ctx || !buf) {
        BT_ERR("%s, Invalid parameter", __func__);
        return NULL;
    }

    cli = (bt_mesh_client_user_data_t *)model->user_data;
    if (!cli) {
        BT_ERR("%s, Clinet user_data is NULL", __func__);
        return NULL;
    }

    rsp = ctx->recv_op;

    /** If the received message address is not a unicast address,
     *  the address may be a group/virtual address, and we push
     *  this message to the application layer.
     */
    if (!BLE_MESH_ADDR_IS_UNICAST(ctx->recv_dst)) {
        BT_DBG("Unexpected status message 0x%x", rsp);
        if (cli->publish_status && need_pub) {
            cli->publish_status(rsp, model, ctx, buf);
        }
        return NULL;
    }

    /** If the source address of the received status message is
     *  different with the destination address of the sending
     *  message, then the message is from another element and
     *  push it to application layer.
     */
    data = (bt_mesh_client_internal_data_t *)cli->internal_data;
    if (!data) {
        BT_ERR("%s, Client internal_data is NULL", __func__);
        return NULL;
    }

    if ((node = bt_mesh_client_pick_node(&data->queue, ctx->addr)) == NULL) {
        BT_DBG("Unexpected status message 0x%x", rsp);
        if (cli->publish_status && need_pub) {
            cli->publish_status(rsp, model, ctx, buf);
        }
        return NULL;
    }

    if (node->op_pending != rsp) {
        BT_DBG("Unexpected status message 0x%x", rsp);
        if (cli->publish_status && need_pub) {
            cli->publish_status(rsp, model, ctx, buf);
        }
        return NULL;
    }

    return node;
}

bool bt_mesh_client_find_opcode_in_list(sys_slist_t *list, u32_t opcode)
{
    if (sys_slist_is_empty(list)) {
        return false;
    }

    sys_snode_t *cur = NULL; bt_mesh_client_node_t *node = NULL;
    for (cur = sys_slist_peek_head(list);
            cur != NULL; cur = sys_slist_peek_next(cur)) {
        node = (bt_mesh_client_node_t *)cur;
        if (node->op_pending == opcode) {
            return true;
        }
        return NULL;
    }

    return node;
}

bool bt_mesh_client_check_node_in_list(sys_slist_t *list, u16_t tx_dst)
{
    if (sys_slist_is_empty(list)) {
        return false;
    }

    sys_snode_t *cur = NULL; bt_mesh_client_node_t *node = NULL;
    for (cur = sys_slist_peek_head(list);
            cur != NULL; cur = sys_slist_peek_next(cur)) {
        node = (bt_mesh_client_node_t *)cur;
        if (node->ctx.addr == tx_dst) {
            return true;
        }
    }

    return false;
}

bt_mesh_client_node_t *bt_mesh_client_pick_node(sys_slist_t *list, u16_t tx_dst)
{
    if (sys_slist_is_empty(list)) {
        return NULL;
    }

    sys_snode_t *cur = NULL; bt_mesh_client_node_t *node = NULL;
    for (cur = sys_slist_peek_head(list);
            cur != NULL; cur = sys_slist_peek_next(cur)) {
        node = (bt_mesh_client_node_t *)cur;
        if (node->ctx.addr == tx_dst) {
            return node;
        }
    }

    return NULL;
}

static u32_t bt_mesh_client_get_status_op(const bt_mesh_client_op_pair_t *op_pair,
        int size, u32_t opcode)
{
    if (!op_pair || size == 0) {
        return 0;
    }

    const bt_mesh_client_op_pair_t *op = op_pair;
    for (int i = 0; i < size; i++) {
        if (op->cli_op == opcode) {
            return op->status_op;
        }
        op++;
    }

    return 0;
}

int bt_mesh_client_send_msg(struct bt_mesh_model *model,
                            u32_t opcode,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *msg,
                            k_work_handler_t timer_handler,
                            s32_t timeout, bool need_ack,
                            const struct bt_mesh_send_cb *cb,
                            void *cb_data)
{
    bt_mesh_client_internal_data_t *internal = NULL;
    bt_mesh_client_user_data_t *cli = NULL;
    bt_mesh_client_node_t *node = NULL;
    int err;

    if (!model || !ctx || !msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    cli = (bt_mesh_client_user_data_t *)model->user_data;
    __ASSERT(cli, "Invalid client value when sent client msg.");
    internal = (bt_mesh_client_internal_data_t *)cli->internal_data;
    __ASSERT(internal, "Invalid internal value when sent client msg.");

    if (!need_ack) {
        /* If this is an unack message, send it directly. */
        return bt_mesh_model_send(model, ctx, msg, cb, cb_data);
    }

    if (bt_mesh_client_check_node_in_list(&internal->queue, ctx->addr)) {
        BT_ERR("%s, Busy sending message to DST 0x%04x", __func__, ctx->addr);
        err = -EBUSY;
    } else {
        /* Don't forget to free the node in the timeout (timer_handler) function. */
        node = (bt_mesh_client_node_t *)osi_calloc(sizeof(bt_mesh_client_node_t));
        if (!node) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return -ENOMEM;
        }
        memcpy(&node->ctx, ctx, sizeof(struct bt_mesh_msg_ctx));
        node->ctx.model = model;
        node->opcode = opcode;
        if ((node->op_pending = bt_mesh_client_get_status_op(cli->op_pair, cli->op_pair_size, opcode)) == 0) {
            BT_ERR("%s, Not found the status opcode in the op_pair list", __func__);
            osi_free(node);
            return -EINVAL;
        }
        if ((err = bt_mesh_model_send(model, ctx, msg, cb, cb_data)) != 0) {
            osi_free(node);
        } else {
            sys_slist_append(&internal->queue, &node->client_node);
            k_delayed_work_init(&node->timer, timer_handler);
            k_delayed_work_submit(&node->timer, timeout ? timeout : CONFIG_BLE_MESH_CLIENT_MSG_TIMEOUT);
        }
    }

    return err;
}

int bt_mesh_client_init(struct bt_mesh_model *model)
{
    bt_mesh_client_internal_data_t *data = NULL;
    bt_mesh_client_user_data_t *cli = NULL;

    if (!model) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    if (!model->op) {
        BT_ERR("%s, Client model op is NULL", __func__);
        return -EINVAL;
    }

    cli = model->user_data;
    if (!cli) {
        BT_ERR("%s, Client user_data is NULL", __func__);
        return -EINVAL;
    }

    /* TODO: call osi_free() when deinit function is invoked */
    data = osi_calloc(sizeof(bt_mesh_client_internal_data_t));
    if (!data) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        return -ENOMEM;
    }

    /* Init the client data queue */
    sys_slist_init(&data->queue);

    cli->model = model;
    cli->internal_data = data;

    return 0;
}

int bt_mesh_client_free_node(sys_slist_t *queue, bt_mesh_client_node_t *node)
{
    if (!queue || !node) {
        return -EINVAL;
    }

    // Free the node timer
    k_delayed_work_free(&node->timer);
    // Release the client node from the queue
    sys_slist_find_and_remove(queue, &node->client_node);
    // Free the node
    osi_free(node);

    return 0;
}

int bt_mesh_set_client_model_role(bt_mesh_role_param_t *common)
{
    bt_mesh_client_user_data_t *client = NULL;

    if (!common || !common->model || !common->model->user_data) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    client = (bt_mesh_client_user_data_t *)common->model->user_data;

    switch (common->role) {
#if CONFIG_BLE_MESH_NODE
    case NODE:
        /* no matter if provisioner is enabled/disabled , node role can be used to send messages */
        client->msg_role = NODE;
        break;
#endif
#if CONFIG_BLE_MESH_PROVISIONER
    case PROVISIONER:
        /* if provisioner is not enabled, provisioner role can't be used to send messages */
        if (!bt_mesh_is_provisioner_en()) {
            BT_ERR("%s, Provisioner is disabled", __func__);
            return -EINVAL;
        }
        client->msg_role = PROVISIONER;
        break;
#endif
#if CONFIG_BLE_MESH_FAST_PROV
    case FAST_PROV:
        client->msg_role = FAST_PROV;
        break;
#endif
    default:
        BT_WARN("%s, Unknown model role %x", __func__, common->role);
        return -EINVAL;
    }

    return 0;
}

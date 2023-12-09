/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "mesh.h"
#include "mesh/main.h"
#include "transport.h"
#include "foundation.h"
#include "mesh/client_common.h"
#include "mesh/common.h"

#include "mesh_v1.1/utils.h"

#define HCI_TIME_FOR_START_ADV  K_MSEC(5)   /* Three adv related hci commands may take 4 ~ 5ms */

static bt_mesh_client_node_t *bt_mesh_client_pick_node(sys_slist_t *list, uint16_t tx_dst)
{
    bt_mesh_client_node_t *node = NULL;
    sys_snode_t *cur = NULL;

    bt_mesh_list_lock();
    if (sys_slist_is_empty(list)) {
        bt_mesh_list_unlock();
        return NULL;
    }

    for (cur = sys_slist_peek_head(list);
         cur != NULL; cur = sys_slist_peek_next(cur)) {
        node = (bt_mesh_client_node_t *)cur;
        if (node->ctx.addr == tx_dst) {
            bt_mesh_list_unlock();
            return node;
        }
    }

    bt_mesh_list_unlock();
    return NULL;
}

bt_mesh_client_node_t *bt_mesh_is_client_recv_publish_msg(struct bt_mesh_model *model,
                                                          struct bt_mesh_msg_ctx *ctx,
                                                          struct net_buf_simple *buf, bool need_pub)
{
    bt_mesh_client_internal_data_t *data = NULL;
    bt_mesh_client_user_data_t *cli = NULL;
    bt_mesh_client_node_t *node = NULL;

    if (!model || !ctx || !buf) {
        BT_ERR("%s, Invalid parameter", __func__);
        return NULL;
    }

    cli = (bt_mesh_client_user_data_t *)model->user_data;
    if (!cli) {
        BT_ERR("Invalid client user data");
        return NULL;
    }

    /** If the received message address is not a unicast address,
     *  the address may be a group/virtual address, and we push
     *  this message to the application layer.
     */
    if (!BLE_MESH_ADDR_IS_UNICAST(ctx->recv_dst)) {
        BT_DBG("Unexpected status message 0x%08x", ctx->recv_op);
        if (cli->publish_status && need_pub) {
            cli->publish_status(ctx->recv_op, model, ctx, buf);
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
        BT_ERR("Invalid client internal data");
        return NULL;
    }

    if ((node = bt_mesh_client_pick_node(&data->queue, ctx->addr)) == NULL) {
        BT_DBG("Unexpected status message 0x%08x", ctx->recv_op);
        if (cli->publish_status && need_pub) {
            cli->publish_status(ctx->recv_op, model, ctx, buf);
        }
        return NULL;
    }

    if (node->op_pending != ctx->recv_op) {
        BT_DBG("Unexpected status message 0x%08x", ctx->recv_op);
        if (cli->publish_status && need_pub) {
            cli->publish_status(ctx->recv_op, model, ctx, buf);
        }
        return NULL;
    }

    if (k_delayed_work_remaining_get(&node->timer) == 0) {
        BT_DBG("Unexpected status message 0x%08x", ctx->recv_op);
        if (cli->publish_status && need_pub) {
            cli->publish_status(ctx->recv_op, model, ctx, buf);
        }
        return NULL;
    }

    return node;
}

static bool bt_mesh_client_check_node_in_list(sys_slist_t *list, uint16_t tx_dst)
{
    bt_mesh_client_node_t *node = NULL;
    sys_snode_t *cur = NULL;

    bt_mesh_list_lock();
    if (sys_slist_is_empty(list)) {
        bt_mesh_list_unlock();
        return false;
    }

    for (cur = sys_slist_peek_head(list);
         cur != NULL; cur = sys_slist_peek_next(cur)) {
        node = (bt_mesh_client_node_t *)cur;
        if (node->ctx.addr == tx_dst) {
            bt_mesh_list_unlock();
            return true;
        }
    }

    bt_mesh_list_unlock();
    return false;
}

static uint32_t bt_mesh_client_get_status_op(const bt_mesh_client_op_pair_t *op_pair,
                                             int size, uint32_t opcode)
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

static int32_t bt_mesh_get_adv_duration(struct bt_mesh_msg_ctx *ctx)
{
    uint16_t duration = 0, adv_int = 0;
    uint8_t xmit = 0;

    /* Initialize with network transmission */
    xmit = bt_mesh_net_transmit_get();

    if (bt_mesh_tag_immutable_cred(ctx->send_tag)) {
#if CONFIG_BLE_MESH_DF_SRV
        if (ctx->send_cred == BLE_MESH_DIRECTED_CRED) {
            xmit = bt_mesh_direct_net_transmit_get();   /* Directed network transmission */
        }
#endif
    }

    adv_int = BLE_MESH_TRANSMIT_INT(xmit);
    duration = (BLE_MESH_TRANSMIT_COUNT(xmit) + 1) * (adv_int + 10);

    return (int32_t)duration;
}

static int32_t bt_mesh_client_calc_timeout(struct bt_mesh_msg_ctx *ctx,
                                           struct net_buf_simple *msg,
                                           uint32_t opcode, int32_t timeout)
{
    int32_t seg_rtx_to = 0, duration = 0, time = 0;
    uint8_t seg_count = 0, seg_rtx_num = 0;
    bool need_seg = false;
    uint8_t mic_size = 0;

    if (msg->len > BLE_MESH_SDU_UNSEG_MAX ||
        bt_mesh_tag_send_segmented(ctx->send_tag)) {
        need_seg = true;    /* Needs segmentation */
    }

    mic_size = (need_seg && ctx->send_szmic == BLE_MESH_SEG_SZMIC_LONG &&
                net_buf_simple_tailroom(msg) >= BLE_MESH_MIC_LONG) ?
                BLE_MESH_MIC_LONG : BLE_MESH_MIC_SHORT;

    if (need_seg) {
        /* Based on the message length, calculate how many segments are needed.
         * All the messages sent from here are access messages.
         */
        seg_rtx_num = bt_mesh_get_seg_rtx_num();
        seg_rtx_to = bt_mesh_get_seg_rtx_timeout(ctx->addr, ctx->send_ttl);
        seg_count = (msg->len + mic_size - 1) / 12U + 1U;

        duration = bt_mesh_get_adv_duration(ctx);

        /* Currently only consider the time consumption of the same segmented
         * messages, but if there are other messages between any two retrans-
         * missions of the same segmented messages, then the whole time will
         * be longer.
         *
         * Since the transport behavior has been changed, i.e. start retransmit
         * timer after the last segment is sent, so we can simplify the timeout
         * calculation here. And the retransmit timer will be started event if
         * the attempts reaches ZERO when the dst is a unicast address.
         */
        int32_t seg_duration = seg_count * (duration + HCI_TIME_FOR_START_ADV);
        time = (seg_duration + seg_rtx_to) * seg_rtx_num;

        BT_INFO("Original timeout %dms, calculated timeout %dms", timeout, time);

        if (time < timeout) {
            /* If the calculated time is smaller than the input timeout value,
             * then use the original timeout value.
             */
            time = timeout;
        }
    } else {
        /* For unsegmented access messages, directly use the timeout
         * value from the application layer.
         */
        time = timeout;
    }

    BT_INFO("Client message 0x%08x with timeout %dms", opcode, time);

    return time;
}

static void msg_send_start(uint16_t duration, int err, void *cb_data)
{
    bt_mesh_client_node_t *node = cb_data;

    BT_DBG("%s, duration %ums", __func__, duration);

    if (err) {
        if (!k_delayed_work_free(&node->timer)) {
            bt_mesh_client_free_node(node);
        }
        return;
    }

    k_delayed_work_submit(&node->timer, node->timeout);
}

static const struct bt_mesh_send_cb send_cb = {
    .start = msg_send_start,
    .end = NULL,
};

int bt_mesh_client_send_msg(bt_mesh_client_common_param_t *param,
                            struct net_buf_simple *msg, bool need_ack,
                            k_work_handler_t timer_handler)
{
    bt_mesh_client_internal_data_t *internal = NULL;
    bt_mesh_client_user_data_t *client = NULL;
    bt_mesh_client_node_t *node = NULL;
    int err = 0;

    if (!param || !param->model || !msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    client = (bt_mesh_client_user_data_t *)param->model->user_data;
    if (!client) {
        BT_ERR("Invalid client user data");
        return -EINVAL;
    }

    internal = (bt_mesh_client_internal_data_t *)client->internal_data;
    if (!internal) {
        BT_ERR("Invalid client internal data");
        return -EINVAL;
    }

    if (param->ctx.addr == BLE_MESH_ADDR_UNASSIGNED) {
        BT_ERR("Invalid DST 0x%04x", param->ctx.addr);
        return -EINVAL;
    }

    if (need_ack == false || !BLE_MESH_ADDR_IS_UNICAST(param->ctx.addr)) {
        /* 1. If this is an unacknowledged message, send it directly.
         * 2. If this is an acknowledged message, but the destination
         *    is not a unicast address, e.g. a group/virtual address,
         *    then all the corresponding responses will be treated as
         *    publish messages, and no timeout will be used.
         */
        err = bt_mesh_model_send(param->model, &param->ctx, msg, param->cb, param->cb_data);
        if (err) {
            BT_ERR("Failed to send client message 0x%08x", param->opcode);
        }
        return err;
    }

    if (!timer_handler) {
        BT_ERR("Invalid timeout handler");
        return -EINVAL;
    }

    if (bt_mesh_client_check_node_in_list(&internal->queue, param->ctx.addr)) {
        BT_ERR("Busy sending message to DST 0x%04x", param->ctx.addr);
        return -EBUSY;
    }

    /* Don't forget to free the node in the timeout (timer_handler) function. */
    node = (bt_mesh_client_node_t *)bt_mesh_calloc(sizeof(bt_mesh_client_node_t));
    if (!node) {
        BT_ERR("%s, Out of memory", __func__);
        return -ENOMEM;
    }

    memcpy(&node->ctx, &param->ctx, sizeof(struct bt_mesh_msg_ctx));
    node->model = param->model;
    node->opcode = param->opcode;
    node->op_pending = bt_mesh_client_get_status_op(client->op_pair, client->op_pair_size, param->opcode);
    if (node->op_pending == 0U) {
        BT_ERR("Status opcode not found in op_pair list, opcode 0x%08x", param->opcode);
        bt_mesh_free(node);
        return -EINVAL;
    }
    node->timeout = bt_mesh_client_calc_timeout(&param->ctx, msg, param->opcode,
                        param->msg_timeout ? param->msg_timeout : CONFIG_BLE_MESH_CLIENT_MSG_TIMEOUT);

    if (k_delayed_work_init(&node->timer, timer_handler)) {
        BT_ERR("Failed to create a timer");
        bt_mesh_free(node);
        return -EIO;
    }

    bt_mesh_list_lock();
    sys_slist_append(&internal->queue, &node->client_node);
    bt_mesh_list_unlock();

    /* "bt_mesh_model_send" will post the mesh packet to the mesh adv queue.
     * Due to the higher priority of adv_thread (than btc task), we need to
     * send the packet after the list item "node" is initialized properly.
     */
    err = bt_mesh_model_send(param->model, &param->ctx, msg, &send_cb, node);
    if (err) {
        BT_ERR("Failed to send client message 0x%08x", node->opcode);
        k_delayed_work_free(&node->timer);
        bt_mesh_client_free_node(node);
    }

    return err;
}

static bt_mesh_mutex_t client_model_lock;

void bt_mesh_client_model_lock(void)
{
    bt_mesh_mutex_lock(&client_model_lock);
}

void bt_mesh_client_model_unlock(void)
{
    bt_mesh_mutex_unlock(&client_model_lock);
}

int bt_mesh_client_init(struct bt_mesh_model *model)
{
    bt_mesh_client_internal_data_t *internal = NULL;
    bt_mesh_client_user_data_t *client = NULL;

    if (!model || !model->op) {
        BT_ERR("Invalid vendor client model");
        return -EINVAL;
    }

    client = (bt_mesh_client_user_data_t *)model->user_data;
    if (!client) {
        BT_ERR("No vendor client context provided");
        return -EINVAL;
    }

    if (client->internal_data) {
        BT_WARN("%s, Already", __func__);
        return -EALREADY;
    }

    internal = bt_mesh_calloc(sizeof(bt_mesh_client_internal_data_t));
    if (!internal) {
        BT_ERR("%s, Out of memory", __func__);
        return -ENOMEM;
    }

    sys_slist_init(&internal->queue);

    client->model = model;
    client->internal_data = internal;

    bt_mesh_mutex_create(&client_model_lock);

    return 0;
}

#if CONFIG_BLE_MESH_DEINIT
int bt_mesh_client_deinit(struct bt_mesh_model *model)
{
    bt_mesh_client_user_data_t *client = NULL;

    if (!model) {
        BT_ERR("Invalid vendor client model");
        return -EINVAL;
    }

    client = (bt_mesh_client_user_data_t *)model->user_data;
    if (!client) {
        BT_ERR("No vendor client context provided");
        return -EINVAL;
    }

    if (client->internal_data) {
        /* Remove items from the list */
        bt_mesh_client_clear_list(client->internal_data);

        /* Free the allocated internal data */
        bt_mesh_free(client->internal_data);
        client->internal_data = NULL;
    }

    bt_mesh_mutex_free(&client_model_lock);

    return 0;
}
#endif /* CONFIG_BLE_MESH_DEINIT */

int bt_mesh_client_free_node(bt_mesh_client_node_t *node)
{
    bt_mesh_client_internal_data_t *internal = NULL;
    bt_mesh_client_user_data_t *client = NULL;

    if (!node || !node->model) {
        BT_ERR("Invalid client list item");
        return -EINVAL;
    }

    client = (bt_mesh_client_user_data_t *)node->model->user_data;
    if (!client) {
        BT_ERR("Invalid client user data");
        return -EINVAL;
    }

    internal = (bt_mesh_client_internal_data_t *)client->internal_data;
    if (!internal) {
        BT_ERR("Invalid client internal data");
        return -EINVAL;
    }

    // Release the client node from the queue
    bt_mesh_list_lock();
    sys_slist_find_and_remove(&internal->queue, &node->client_node);
    bt_mesh_list_unlock();

    // Free the node
    bt_mesh_free(node);

    return 0;
}

int bt_mesh_client_clear_list(void *data)
{
    bt_mesh_client_internal_data_t *internal = NULL;
    bt_mesh_client_node_t *node = NULL;

    if (!data) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    internal = (bt_mesh_client_internal_data_t *)data;

    bt_mesh_list_lock();
    while (!sys_slist_is_empty(&internal->queue)) {
        node = (void *)sys_slist_get_not_empty(&internal->queue);
        k_delayed_work_free(&node->timer);
        bt_mesh_free(node);
    }
    bt_mesh_list_unlock();

    return 0;
}

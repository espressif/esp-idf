/*
 * SPDX-FileCopyrightText: 2020-2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>

#include <zephyr/net_buf.h>

#include "ots_l2cap_internal.h"

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(LEA_OTS, CONFIG_BT_ISO_LOG_LEVEL);

/* According to Bluetooth specification Assigned Numbers that are used in the
 * Logical Link Control for protocol/service multiplexers.
 */
#define BT_GATT_OTS_L2CAP_PSM   0x0025

NET_BUF_POOL_FIXED_DEFINE(ot_chan_tx_pool, 1,
                          BT_L2CAP_SDU_BUF_SIZE(CONFIG_BT_OTS_L2CAP_CHAN_TX_MTU),
                          CONFIG_BT_CONN_TX_USER_DATA_SIZE, NULL);

#if (CONFIG_BT_OTS_L2CAP_CHAN_RX_MTU > BT_L2CAP_SDU_RX_MTU)
NET_BUF_POOL_FIXED_DEFINE(ot_chan_rx_pool, 1, CONFIG_BT_OTS_L2CAP_CHAN_RX_MTU, 8,
                          NULL);
#endif

/* List of Object Transfer Channels. */
static sys_slist_t channels;

static int ots_l2cap_send(struct bt_gatt_ots_l2cap *l2cap_ctx)
{
    int ret;
    struct net_buf *buf;
    uint32_t len;

    /* Calculate maximum length of data chunk. */
    len = MIN(l2cap_ctx->ot_chan.tx.mtu, CONFIG_BT_OTS_L2CAP_CHAN_TX_MTU);
    len = MIN(len, l2cap_ctx->tx.len - l2cap_ctx->tx.len_sent);

    /* Prepare buffer for sending. */
    buf = net_buf_alloc(&ot_chan_tx_pool, K_FOREVER);
    net_buf_reserve(buf, BT_L2CAP_SDU_CHAN_SEND_RESERVE);
    net_buf_add_mem(buf, &l2cap_ctx->tx.data[l2cap_ctx->tx.len_sent], len);

    ret = bt_l2cap_chan_send(&l2cap_ctx->ot_chan.chan, buf);
    if (ret < 0) {
        LOG_ERR("Unable to send data over CoC: %d", ret);
        net_buf_unref(buf);

        return -ENOEXEC;
    }

    /* Mark that L2CAP TX was accepted. */
    l2cap_ctx->tx.len_sent += len;

    LOG_DBG("Sending TX chunk with %d bytes on L2CAP CoC", len);

    return 0;
}

#if (CONFIG_BT_OTS_L2CAP_CHAN_RX_MTU > BT_L2CAP_SDU_RX_MTU)
static struct net_buf *l2cap_alloc_buf(struct bt_l2cap_chan *chan)
{
    LOG_DBG("Channel %p allocating buffer", chan);

    return net_buf_alloc(&ot_chan_rx_pool, K_FOREVER);
}
#endif

static void l2cap_sent(struct bt_l2cap_chan *chan)
{
    struct bt_l2cap_le_chan *l2chan = CONTAINER_OF(chan, struct bt_l2cap_le_chan, chan);
    struct bt_gatt_ots_l2cap *l2cap_ctx;

    LOG_DBG("Outgoing data channel %p transmitted", chan);

    l2cap_ctx = CONTAINER_OF(l2chan, struct bt_gatt_ots_l2cap, ot_chan);

    /* Ongoing TX - sending next chunk. */
    if (l2cap_ctx->tx.len != l2cap_ctx->tx.len_sent) {
        if (ots_l2cap_send(l2cap_ctx)) {
            /* Send failed - clean up TX state to unblock channel. */
            LOG_ERR("Failed to send next chunk, aborting TX");
            memset(&l2cap_ctx->tx, 0, sizeof(l2cap_ctx->tx));
            if (l2cap_ctx->tx_done) {
                l2cap_ctx->tx_done(l2cap_ctx, chan->conn);
            }
        }

        return;
    }

    /* TX completed - notify upper layers and clean up. */
    memset(&l2cap_ctx->tx, 0, sizeof(l2cap_ctx->tx));

    LOG_DBG("Scheduled TX on L2CAP CoC is complete");

    if (l2cap_ctx->tx_done) {
        l2cap_ctx->tx_done(l2cap_ctx, chan->conn);
    }
}

static int l2cap_recv(struct bt_l2cap_chan *chan, struct net_buf *buf)
{
    struct bt_l2cap_le_chan *l2chan = CONTAINER_OF(chan, struct bt_l2cap_le_chan, chan);
    struct bt_gatt_ots_l2cap *l2cap_ctx;

    LOG_DBG("Incoming data channel %p received", chan);

    l2cap_ctx = CONTAINER_OF(l2chan, struct bt_gatt_ots_l2cap, ot_chan);

    if (!l2cap_ctx->rx_done) {
        return -ENODEV;
    }

    return l2cap_ctx->rx_done(l2cap_ctx, chan->conn, buf);
}

static void l2cap_status(struct bt_l2cap_chan *chan, atomic_t *status)
{
    LOG_DBG("Channel %p status %lu", chan, atomic_get(status));
}

static void l2cap_connected(struct bt_l2cap_chan *chan)
{
    LOG_DBG("Channel %p connected", chan);
}

static void l2cap_disconnected(struct bt_l2cap_chan *chan)
{
    struct bt_l2cap_le_chan *l2chan = CONTAINER_OF(chan, struct bt_l2cap_le_chan, chan);
    struct bt_gatt_ots_l2cap *l2cap_ctx;

    LOG_DBG("Channel %p disconnected", chan);

    l2cap_ctx = CONTAINER_OF(l2chan, struct bt_gatt_ots_l2cap, ot_chan);

    /* Clean up any in-progress TX */
    memset(&l2cap_ctx->tx, 0, sizeof(l2cap_ctx->tx));

    if (l2cap_ctx->closed) {
        l2cap_ctx->closed(l2cap_ctx, chan->conn);
    }
}

static const struct bt_l2cap_chan_ops l2cap_ops = {
#if (CONFIG_BT_OTS_L2CAP_CHAN_RX_MTU > BT_L2CAP_SDU_RX_MTU)
    .alloc_buf  = l2cap_alloc_buf,
#endif
    .sent       = l2cap_sent,
    .recv       = l2cap_recv,
    .status     = l2cap_status,
    .connected  = l2cap_connected,
    .disconnected   = l2cap_disconnected,
};

static inline void l2cap_chan_init(struct bt_l2cap_le_chan *chan)
{
    chan->rx.mtu = CONFIG_BT_OTS_L2CAP_CHAN_RX_MTU;
    chan->chan.ops = &l2cap_ops;

    LOG_DBG("RX MTU set to %u", chan->rx.mtu);
}

static struct bt_gatt_ots_l2cap *find_free_l2cap_ctx(void)
{
    struct bt_gatt_ots_l2cap *l2cap_ctx;

    SYS_SLIST_FOR_EACH_CONTAINER(&channels, l2cap_ctx, node) {
        if (l2cap_ctx->ot_chan.chan.conn) {
            continue;
        }

        return l2cap_ctx;
    }

    return NULL;
}

static int l2cap_accept(struct bt_conn *conn, struct bt_l2cap_server *server,
                        struct bt_l2cap_chan **chan)
{
    struct bt_gatt_ots_l2cap *l2cap_ctx;

    LOG_DBG("Incoming conn %p", (void *)conn);

    l2cap_ctx = find_free_l2cap_ctx();
    if (l2cap_ctx) {
        l2cap_chan_init(&l2cap_ctx->ot_chan);
        memset(&l2cap_ctx->tx, 0, sizeof(l2cap_ctx->tx));

        *chan = &l2cap_ctx->ot_chan.chan;

        return 0;
    }

    return -ENOMEM;
}

static struct bt_l2cap_server l2cap_server = {
    .psm = BT_GATT_OTS_L2CAP_PSM,
    .accept = l2cap_accept,
};

static int bt_gatt_ots_l2cap_init(void)
{
    int err;

    sys_slist_init(&channels);

    err = bt_l2cap_server_register(&l2cap_server);
    if (err) {
        LOG_ERR("Unable to register OTS PSM");
        return err;
    }

    LOG_DBG("Initialized OTS L2CAP");

    return 0;
}

bool bt_gatt_ots_l2cap_is_open(struct bt_gatt_ots_l2cap *l2cap_ctx,
                               struct bt_conn *conn)
{
    return (l2cap_ctx->ot_chan.chan.conn == conn);
}

int bt_gatt_ots_l2cap_send(struct bt_gatt_ots_l2cap *l2cap_ctx,
                           uint8_t *data, uint32_t len)
{
    int err;

    if (l2cap_ctx->tx.len != 0) {
        LOG_ERR("L2CAP TX in progress");

        return -EAGAIN;
    }

    l2cap_ctx->tx.data = data;
    l2cap_ctx->tx.len = len;

    LOG_DBG("Starting TX on L2CAP CoC with %d byte packet", len);

    err = ots_l2cap_send(l2cap_ctx);
    if (err) {
        LOG_ERR("Unable to send data over CoC: %d", err);
        memset(&l2cap_ctx->tx, 0, sizeof(l2cap_ctx->tx));

        return err;
    }

    return 0;
}

int bt_gatt_ots_l2cap_register(struct bt_gatt_ots_l2cap *l2cap_ctx)
{
    sys_slist_append(&channels, &l2cap_ctx->node);

    return 0;
}

int bt_gatt_ots_l2cap_unregister(struct bt_gatt_ots_l2cap *l2cap_ctx)
{
    sys_slist_find_and_remove(&channels, &l2cap_ctx->node);

    return 0;
}

/* Similar to l2cap_accept(), but for the client side */
int bt_gatt_ots_l2cap_connect(struct bt_conn *conn,
                              struct bt_gatt_ots_l2cap **l2cap_ctx)
{
    int err;
    struct bt_gatt_ots_l2cap *ctx;

    if (!conn) {
        LOG_WRN("Invalid Connection");
        return -ENOTCONN;
    }

    if (!l2cap_ctx) {
        LOG_WRN("Invalid context");
        return -EINVAL;
    }

    *l2cap_ctx = NULL;

    ctx = find_free_l2cap_ctx();
    if (!ctx) {
        return -ENOMEM;
    }

    l2cap_chan_init(&ctx->ot_chan);
    (void)memset(&ctx->tx, 0, sizeof(ctx->tx));

    LOG_DBG("Connecting L2CAP CoC");
    err = bt_l2cap_chan_connect(conn, &ctx->ot_chan.chan, BT_GATT_OTS_L2CAP_PSM);
    if (err) {
        LOG_WRN("Unable to connect to psm %u (err %d)", BT_GATT_OTS_L2CAP_PSM, err);
    } else {
        LOG_DBG("L2CAP connection pending");
        *l2cap_ctx = ctx;
    }

    return err;
}

int bt_gatt_ots_l2cap_disconnect(struct bt_gatt_ots_l2cap *l2cap_ctx)
{
    return bt_l2cap_chan_disconnect(&l2cap_ctx->ot_chan.chan);
}

SYS_INIT(bt_gatt_ots_l2cap_init, APPLICATION,
         CONFIG_APPLICATION_INIT_PRIORITY);

/*
 * SPDX-FileCopyrightText: 2020-2022 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
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

    /* Single-buffer pool, and iso_task is both the only allocator and the only
     * path that releases it, so waiting here would self-deadlock. */
    buf = net_buf_alloc(&ot_chan_tx_pool, K_NO_WAIT);
    if (buf == NULL) {
        LOG_ERR("OtsL2capTxBufAllocFail");
        return -ENOMEM;
    }

    net_buf_reserve(buf, BT_L2CAP_SDU_CHAN_SEND_RESERVE);
    net_buf_add_mem(buf, &l2cap_ctx->tx.data[l2cap_ctx->tx.len_sent], len);

    ret = bt_l2cap_chan_send(&l2cap_ctx->ot_chan.chan, buf);
    if (ret < 0) {
        LOG_WRN("OtsL2capChanSendFail[%d]", ret);
        net_buf_unref(buf);

        return -ENOEXEC;
    }

    /* Mark that L2CAP TX was accepted. */
    l2cap_ctx->tx.len_sent += len;

    LOG_DBG("OtsL2capTxChunk[%u]", len);

    return 0;
}

#if (CONFIG_BT_OTS_L2CAP_CHAN_RX_MTU > BT_L2CAP_SDU_RX_MTU)
static struct net_buf *l2cap_alloc_buf(struct bt_l2cap_chan *chan)
{
    LOG_DBG("OtsL2capAllocBuf");

    return net_buf_alloc(&ot_chan_rx_pool, K_NO_WAIT);
}
#endif

static void l2cap_sent(struct bt_l2cap_chan *chan)
{
    struct bt_l2cap_le_chan *l2chan = CONTAINER_OF(chan, struct bt_l2cap_le_chan, chan);
    struct bt_gatt_ots_l2cap *l2cap_ctx;

    LOG_DBG("OtsL2capSent");

    l2cap_ctx = CONTAINER_OF(l2chan, struct bt_gatt_ots_l2cap, ot_chan);

    /* Ongoing TX - sending next chunk. */
    if (l2cap_ctx->tx.len != l2cap_ctx->tx.len_sent) {
        if (ots_l2cap_send(l2cap_ctx)) {
            /* Do not call tx_done: it means success to upper layers
             * (oacp_read would skip unread bytes; write_obj_tx_done
             * would report a full write). Abort via disconnect → closed. */
            LOG_WRN("OtsL2capTxAbort");
            (void)bt_l2cap_chan_disconnect(chan);
        }

        return;
    }

    /* TX completed - notify upper layers and clean up. */
    memset(&l2cap_ctx->tx, 0, sizeof(l2cap_ctx->tx));

    LOG_DBG("OtsL2capTxComp");

    if (l2cap_ctx->tx_done) {
        l2cap_ctx->tx_done(l2cap_ctx, chan->conn);
    }
}

static int l2cap_recv(struct bt_l2cap_chan *chan, struct net_buf *buf)
{
    struct bt_l2cap_le_chan *l2chan = CONTAINER_OF(chan, struct bt_l2cap_le_chan, chan);
    struct bt_gatt_ots_l2cap *l2cap_ctx;

    LOG_DBG("OtsL2capRecv");

    l2cap_ctx = CONTAINER_OF(l2chan, struct bt_gatt_ots_l2cap, ot_chan);

    if (!l2cap_ctx->rx_done) {
        return -ENODEV;
    }

    return l2cap_ctx->rx_done(l2cap_ctx, chan->conn, buf);
}

static void l2cap_status(struct bt_l2cap_chan *chan, atomic_t *status)
{
    LOG_DBG("OtsL2capStatus[%u]", atomic_get(status));
}

static void l2cap_connected(struct bt_l2cap_chan *chan)
{
    LOG_DBG("OtsL2capConnected");
}

static void l2cap_disconnected(struct bt_l2cap_chan *chan)
{
    struct bt_l2cap_le_chan *l2chan = CONTAINER_OF(chan, struct bt_l2cap_le_chan, chan);
    struct bt_gatt_ots_l2cap *l2cap_ctx;

    LOG_DBG("OtsL2capDisconnected");

    l2cap_ctx = CONTAINER_OF(l2chan, struct bt_gatt_ots_l2cap, ot_chan);

    /* Clean up any in-progress TX */
    memset(&l2cap_ctx->tx, 0, sizeof(l2cap_ctx->tx));

    if (l2cap_ctx->closed) {
        l2cap_ctx->closed(l2cap_ctx, chan->conn);
    }

    /* Contexts are reused via find_free_l2cap_ctx; drop procedure hooks
     * so a later accept/connect cannot invoke stale callbacks. */
    l2cap_ctx->closed = NULL;
    l2cap_ctx->rx_done = NULL;
    l2cap_ctx->tx_done = NULL;
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

    LOG_DBG("OtsL2capRxMtu[%u]", chan->rx.mtu);
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

    LOG_DBG("OtsL2capIncomingConn");

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

int bt_gatt_ots_l2cap_init(void)
{
    int err;

    sys_slist_init(&channels);

    err = bt_l2cap_server_register(&l2cap_server);
    if (err) {
        LOG_ERR("OtsL2capPsmRegFail");
        return err;
    }

    LOG_DBG("OtsL2capInit");

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
        LOG_WRN("OtsL2capTxInProgress");

        return -EAGAIN;
    }

    l2cap_ctx->tx.data = data;
    l2cap_ctx->tx.len = len;

    LOG_DBG("OtsL2capTxStart[%u]", len);

    err = ots_l2cap_send(l2cap_ctx);
    if (err) {
        LOG_WRN("OtsL2capSendFail[%d]", err);
        memset(&l2cap_ctx->tx, 0, sizeof(l2cap_ctx->tx));

        return err;
    }

    return 0;
}

int bt_gatt_ots_l2cap_register(struct bt_gatt_ots_l2cap *l2cap_ctx)
{
    if (sys_slist_find(&channels, &l2cap_ctx->node, NULL)) {
        return -EALREADY;
    }

    sys_slist_append(&channels, &l2cap_ctx->node);

    return 0;
}

int bt_gatt_ots_l2cap_unregister(struct bt_gatt_ots_l2cap *l2cap_ctx)
{
    if (l2cap_ctx->ot_chan.chan.conn) {
        LOG_WRN("OtsL2capUnregBusy");
        return -EBUSY;
    }

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
        LOG_WRN("OtsL2capConnNull");
        return -ENOTCONN;
    }

    if (!l2cap_ctx) {
        LOG_WRN("OtsL2capCtxNull");
        return -EINVAL;
    }

    /* Callers owning a context pass it in, so the context they later use in
     * their callbacks is the one actually connected. The global free-list
     * lookup would otherwise hand out another instance's (or the server's)
     * context whenever more than one is registered. */
    ctx = *l2cap_ctx;
    *l2cap_ctx = NULL;

    if (!ctx) {
        ctx = find_free_l2cap_ctx();
        if (!ctx) {
            return -ENOMEM;
        }
    } else if (ctx->ot_chan.chan.conn) {
        LOG_WRN("OtsL2capCtxAlreadyConnected");
        return -EBUSY;
    }

    l2cap_chan_init(&ctx->ot_chan);
    (void)memset(&ctx->tx, 0, sizeof(ctx->tx));

    LOG_DBG("OtsL2capConnecting");
    err = bt_l2cap_chan_connect(conn, &ctx->ot_chan.chan, BT_GATT_OTS_L2CAP_PSM);
    if (err) {
        LOG_WRN("OtsL2capConnFail[%u][%d]", BT_GATT_OTS_L2CAP_PSM, err);
    } else {
        LOG_DBG("OtsL2capConnPending");
        *l2cap_ctx = ctx;
    }

    return err;
}

int bt_gatt_ots_l2cap_disconnect(struct bt_gatt_ots_l2cap *l2cap_ctx)
{
    return bt_l2cap_chan_disconnect(&l2cap_ctx->ot_chan.chan);
}

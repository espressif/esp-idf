/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/l2cap.h>

#include <../host/conn_internal.h>

#include "host/ble_hs.h"
#include "host/ble_l2cap.h"
#include "host/ble_hs_mbuf.h"
#include "../../../nimble/host/src/ble_l2cap_priv.h"

#include "common/host.h"

#include "nimble/hs_error.h"

LOG_MODULE_REGISTER(ISO_N2CAP, CONFIG_BT_ISO_LOG_LEVEL);

_Static_assert(CONFIG_BT_NIMBLE_L2CAP_COC_MAX_NUM && "At least one L2CAP coc shall be supported");

#define OTS_L2CAP_BUF_COUNT     (3 * CONFIG_BT_NIMBLE_L2CAP_COC_MAX_NUM)
#define OTS_L2CAP_MEM_SIZE      OS_MEMPOOL_SIZE(OTS_L2CAP_BUF_COUNT, L2CAP_LE_OTS_MTU * 2)

static os_membuf_t ots_mem[OTS_L2CAP_MEM_SIZE];
static struct os_mempool ots_mbuf_mempool;
static struct os_mbuf_pool ots_mbuf_pool;

static struct ble_l2cap_chan *ots_chan;

static int ots_l2cap_recv_ready(struct ble_l2cap_chan *chan)
{
    struct os_mbuf *sdu_rx;
    int rc;

    LOG_DBG("[N]OtsRecvReady");

    sdu_rx = os_mbuf_get_pkthdr(&ots_mbuf_pool, 0);
    if (sdu_rx == NULL) {
        LOG_ERR("[N]NoBufForL2capRecv");
        return -ENOMEM;
    }

    rc = ble_l2cap_recv_ready(chan, sdu_rx);
    if (rc) {
        LOG_ERR("[N]L2capRecvFail[%d]", rc);

        os_mbuf_free_chain(sdu_rx);
        return rc;
    }

    return 0;
}

static int ots_l2cap_event_cb(struct ble_l2cap_event *event, void *arg)
{
    struct ble_l2cap_chan_info chan_info;
    uint16_t result = 0;
    size_t sdu_len;
    uint8_t *sdu;
    int err;

    LOG_DBG("[N]OtsEvtCb[%u]", event->type);

    switch (event->type) {
    case BLE_L2CAP_EVENT_COC_CONNECTED:
        if (event->connect.status) {
            LOG_ERR("[N]CocConnectFail[%d]", event->connect.status);
            return 0;
        }

        if (ots_chan) {
            LOG_ERR("[N]CocChanExist");
            return 0;
        }

        ots_chan = event->connect.chan;

        if (ble_l2cap_get_chan_info(event->connect.chan, &chan_info)) {
            assert(0);
        }

        LOG_DBG("[N]CocConnect[%u][%04x][%04x][%04x][%u][%u][%u][%u]",
                event->connect.conn_handle, chan_info.scid, chan_info.dcid,
                chan_info.psm, chan_info.our_l2cap_mtu, chan_info.peer_l2cap_mtu,
                chan_info.our_coc_mtu, chan_info.peer_coc_mtu);

        bt_le_l2cap_connected(event->connect.conn_handle, chan_info.psm,
                              chan_info.dcid, chan_info.peer_coc_mtu,
                              chan_info.scid, chan_info.our_coc_mtu);
        return 0;

    case BLE_L2CAP_EVENT_COC_DISCONNECTED:
        if (ots_chan != event->disconnect.chan) {
            LOG_ERR("[N]DisconnectInvCocChan");
            return 0;
        }

        LOG_DBG("[N]CocDisconnect[%u][%04x]",
                event->disconnect.conn_handle, event->disconnect.chan->psm);

        bt_le_l2cap_disconnected(event->disconnect.conn_handle, event->disconnect.chan->psm);

        ots_chan = NULL;
        return 0;

    case BLE_L2CAP_EVENT_COC_ACCEPT:
        if (event->accept.peer_sdu_size > L2CAP_LE_OTS_MTU) {
            LOG_ERR("[N]InvAcceptMtu[%u][%u]",
                    event->accept.peer_sdu_size, L2CAP_LE_OTS_MTU);
            return -EINVAL;
        }

        LOG_DBG("[N]CocAccept[%u][%04x][%04x][%u][%u][%u][%u]",
                event->accept.conn_handle, event->accept.chan->psm,
                event->accept.chan->dcid, event->accept.chan->coc_tx.mtu,
                event->accept.chan->peer_coc_mps, event->accept.peer_sdu_size,
                event->accept.chan->coc_tx.credits);

        err = bt_le_l2cap_accept(event->accept.conn_handle,
                                 event->accept.chan->psm,
                                 event->accept.chan->dcid,
                                 event->accept.chan->coc_tx.mtu,
                                 event->accept.chan->peer_coc_mps,
                                 event->accept.chan->coc_tx.credits,
                                 &result);
        if (err) {
            return err;
        }

        ARG_UNUSED(result);

        return ots_l2cap_recv_ready(event->accept.chan);

    case BLE_L2CAP_EVENT_COC_DATA_RECEIVED:
        if (ots_chan != event->receive.chan) {
            LOG_ERR("[N]RecvOnInvCocChan");
            return 0;
        }

        assert(event->receive.sdu_rx);

        LOG_DBG("[N]CocReceive[%u][%04x][%u]",
                event->receive.conn_handle, event->receive.chan->psm,
                event->receive.sdu_rx->om_len);

        sdu_len = OS_MBUF_PKTLEN(event->receive.sdu_rx);

        sdu = calloc(1, sdu_len);
        assert(sdu);

        err = os_mbuf_copydata(event->receive.sdu_rx, 0, sdu_len, sdu);
        assert(err == 0);

        bt_le_l2cap_received(event->receive.conn_handle,
                             event->receive.chan->psm,
                             sdu, sdu_len);

        os_mbuf_free_chain(event->receive.sdu_rx);
        free(sdu);

        return ots_l2cap_recv_ready(event->receive.chan);

    case BLE_L2CAP_EVENT_COC_TX_UNSTALLED:
        if (ots_chan != event->tx_unstalled.chan) {
            LOG_ERR("[N]TxUnstalledOnInvCocChan");
            return 0;
        }

        LOG_DBG("[N]CocTxUnstalled[%u][%d]",
                event->tx_unstalled.conn_handle, event->tx_unstalled.status);

        /* TODO: transmit the remaining data */
        return 0;

    default:
        return 0;
    }
}

int bt_le_nimble_l2cap_chan_connect(uint16_t conn_handle)
{
    struct os_mbuf *sdu_rx;
    int rc;

    if (ots_chan) {
        LOG_WRN("[N]OtsChanExist");
        return -EALREADY;
    }

    sdu_rx = os_mbuf_get_pkthdr(&ots_mbuf_pool, 0);
    if (sdu_rx == NULL) {
        LOG_ERR("[N]NoBufForL2capConnect");
        return -ENOMEM;
    }

    rc = ble_l2cap_connect(conn_handle, L2CAP_LE_OTS_PSM, L2CAP_LE_OTS_MTU,
                           sdu_rx, ots_l2cap_event_cb, NULL);
    if (rc) {
        LOG_ERR("[N]L2capConnectFail[%d]", rc);

        os_mbuf_free_chain(sdu_rx);
        return nimble_err_to_errno(rc);
    }

    return 0;
}

int bt_le_nimble_l2cap_chan_disconnect(struct bt_l2cap_chan *chan)
{
    int rc;

    if (ots_chan == NULL) {
        LOG_WRN("[N]NoOtsChan");
        return -ENOTCONN;
    }

    if (ble_l2cap_get_conn_handle(ots_chan) != chan->conn->handle) {
        LOG_ERR("[N]UnexpOtsChan[%u][%u]",
                ble_l2cap_get_conn_handle(ots_chan), chan->conn->handle);
        return -EINVAL;
    }

    rc = ble_l2cap_disconnect(ots_chan);
    if (rc) {
        LOG_ERR("[N]L2capDisconnectFail[%d]", rc);
        return nimble_err_to_errno(rc);
    }

    return 0;
}

int bt_le_nimble_l2cap_chan_send(struct bt_l2cap_chan *chan, struct net_buf *buf)
{
    struct os_mbuf *sdu_tx;
    int rc;

    if (ots_chan == NULL) {
        LOG_WRN("[N]NoOtsChan");
        return -ENOTCONN;
    }

    if (ble_l2cap_get_conn_handle(ots_chan) != chan->conn->handle) {
        LOG_ERR("[N]UnexpOtsChan[%u][%u]",
                ble_l2cap_get_conn_handle(ots_chan), chan->conn->handle);
        return -EINVAL;
    }

    sdu_tx = os_mbuf_get_pkthdr(&ots_mbuf_pool, 0);
    if (sdu_tx == NULL) {
        LOG_ERR("[N]NoBufForL2capSend");
        return -ENOMEM;
    }

    rc = os_mbuf_append(sdu_tx, buf->data, buf->len);
    if (rc) {
        LOG_ERR("[N]AppendBufFail[%d]", rc);
        os_mbuf_free_chain(sdu_tx);
        return -EIO;
    }

    rc = ble_l2cap_send(ots_chan, sdu_tx);
    if (rc) {
        if (rc == BLE_HS_ESTALLED) {
            /* sdu is queued in tx->sdus[0]; NimBLE will continue on
             * BLE_L2CAP_EVENT_COC_TX_UNSTALLED. Do NOT free here.
             */
            LOG_WRN("[N]MoreCreditsForL2capSend");
        } else if (rc == BLE_HS_EBADDATA || rc == BLE_HS_EBUSY) {
            /* sdu was rejected before being queued; caller still owns it. */
            LOG_ERR("[N]L2capSendFail[%d]", rc);
            os_mbuf_free_chain(sdu_tx);
        } else {
            /* Internal error inside continue_tx; NimBLE already freed sdu. */
            LOG_ERR("[N]L2capSendFail[%d]", rc);
        }

        return nimble_err_to_errno(rc);
    }

    return 0;
}

int bt_le_nimble_l2cap_init(void)
{
    int rc;

    rc = os_mempool_init(&ots_mbuf_mempool, OTS_L2CAP_BUF_COUNT, L2CAP_LE_OTS_MTU * 2, ots_mem, "ots_pool");
    if (rc) {
        LOG_ERR("[N]InitOtsMempoolFail[%d]", rc);
        return rc;
    }

    rc = os_mbuf_pool_init(&ots_mbuf_pool, &ots_mbuf_mempool, L2CAP_LE_OTS_MTU, OTS_L2CAP_BUF_COUNT);
    if (rc) {
        LOG_ERR("[N]InitOtsMbufPoolFail[%d]", rc);
        return rc;
    }

#if CONFIG_BT_OTS
    rc = ble_l2cap_create_server(L2CAP_LE_OTS_PSM, L2CAP_LE_OTS_MTU, ots_l2cap_event_cb, NULL);
    if (rc) {
        LOG_ERR("[N]CreateL2capSrvFail[%d]", rc);
        return rc;
    }
#endif /* CONFIG_BT_OTS */

    return 0;
}

void bt_le_nimble_l2cap_deinit(void)
{
    LOG_DBG("NimbleL2capDeinit");

    /* TODO: free the ots_mbuf_pool and ots_mbuf_mempool */

#if CONFIG_BT_OTS
    /* TODO: destroy the server */
#endif /* CONFIG_BT_OTS */
}

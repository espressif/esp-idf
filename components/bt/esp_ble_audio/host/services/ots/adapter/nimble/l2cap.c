/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/l2cap.h>

#include <../host/conn_internal.h>

#include "host/ble_hs.h"
#include "host/ble_l2cap.h"
#include "host/ble_hs_mbuf.h"
#include "host/ble_gap.h"

#include "../../../nimble/host/src/ble_l2cap_priv.h"

#include "common/host.h"
#include "common/audio_attr.h"
#include "nimble/hs_error.h"

#include "ots/adapter/l2cap.h"

LOG_MODULE_REGISTER(ISO_N2CAP, CONFIG_BT_ISO_LOG_LEVEL);

_Static_assert(CONFIG_BT_NIMBLE_L2CAP_COC_MAX_NUM && "At least one L2CAP coc shall be supported");

#define OTS_L2CAP_BUF_COUNT     (3 * CONFIG_BT_NIMBLE_L2CAP_COC_MAX_NUM)
#define OTS_L2CAP_MEM_SIZE      OS_MEMPOOL_SIZE(OTS_L2CAP_BUF_COUNT, L2CAP_LE_OTS_MTU * 2)

/* Staging only - both directions copy through msys mbufs, so nothing here is
 * DMA-reached and PSRAM is fine. */
static BT_AUDIO_EXT_RAM_BSS_ATTR os_membuf_t ots_mem[OTS_L2CAP_MEM_SIZE];
static BT_AUDIO_EXT_RAM_BSS_ATTR struct os_mempool ots_mbuf_mempool;
static BT_AUDIO_EXT_RAM_BSS_ATTR struct os_mbuf_pool ots_mbuf_pool;

/* One slot per connection (OTS is single-PSM); pending bridges the async gap
 * between ble_l2cap_connect and COC_CONNECTED.
 *
 * Read without host_lock on both sides: the event callback must not take it
 * (NimBLE emits COC events under ble_hs_lock, so that would invert the ISO
 * path's host_lock -> ble_hs_lock order). psm is cached so a stale slot never
 * has to dereference chan; see chan_send for the residual window. */
struct ots_chan_slot {
    struct ble_l2cap_chan *chan;
    uint16_t conn_handle;
    uint16_t psm;
    bool used;
    bool pending;
};

static BT_AUDIO_EXT_RAM_BSS_ATTR struct ots_chan_slot ots_chans[L2CAP_OTS_MAX_CHAN];

static struct ots_chan_slot *ots_chan_find_used_by_chan(struct ble_l2cap_chan *chan)
{
    for (int i = 0; i < L2CAP_OTS_MAX_CHAN; i++) {
        if (ots_chans[i].used && ots_chans[i].chan == chan) {
            return &ots_chans[i];
        }
    }
    return NULL;
}

static struct ots_chan_slot *ots_chan_find_used_by_conn(uint16_t conn_handle)
{
    for (int i = 0; i < L2CAP_OTS_MAX_CHAN; i++) {
        if (ots_chans[i].used && ots_chans[i].conn_handle == conn_handle) {
            return &ots_chans[i];
        }
    }
    return NULL;
}

static struct ots_chan_slot *ots_chan_find_pending_by_conn(uint16_t conn_handle)
{
    for (int i = 0; i < L2CAP_OTS_MAX_CHAN; i++) {
        if (ots_chans[i].pending && ots_chans[i].conn_handle == conn_handle) {
            return &ots_chans[i];
        }
    }
    return NULL;
}

static struct ots_chan_slot *ots_chan_alloc_pending(uint16_t conn_handle)
{
    for (int i = 0; i < L2CAP_OTS_MAX_CHAN; i++) {
        if (!ots_chans[i].used && !ots_chans[i].pending) {
            memset(&ots_chans[i], 0, sizeof(ots_chans[i]));
            ots_chans[i].conn_handle = conn_handle;
            ots_chans[i].pending = true;
            return &ots_chans[i];
        }
    }
    return NULL;
}

static struct ots_chan_slot *ots_chan_alloc_used(struct ble_l2cap_chan *chan, uint16_t conn_handle)
{
    for (int i = 0; i < L2CAP_OTS_MAX_CHAN; i++) {
        if (!ots_chans[i].used && !ots_chans[i].pending) {
            memset(&ots_chans[i], 0, sizeof(ots_chans[i]));
            ots_chans[i].chan = chan;
            ots_chans[i].conn_handle = conn_handle;
            ots_chans[i].psm = chan->psm;
            ots_chans[i].used = true;
            return &ots_chans[i];
        }
    }
    return NULL;
}

static void ots_chan_free(struct ots_chan_slot *slot)
{
    if (slot != NULL) {
        memset(slot, 0, sizeof(*slot));
    }
}

static int ots_l2cap_recv_ready(struct ble_l2cap_chan *chan)
{
    struct os_mbuf *sdu_rx;
    int rc;

    LOG_DBG("[N]L2capOtsRecvReady");

    sdu_rx = os_mbuf_get_pkthdr(&ots_mbuf_pool, 0);
    if (sdu_rx == NULL) {
        LOG_ERR("[N]L2capNoBufForL2capRecv");
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
    uint16_t psm;
    uint8_t *sdu;
    int err;

    LOG_DBG("[N]L2capOtsEvtCb[%u]", event->type);

    switch (event->type) {
    case BLE_L2CAP_EVENT_COC_CONNECTED: {
        struct ots_chan_slot *slot;

        if (event->connect.status) {
            LOG_ERR("[N]L2capCocConnectFail[%d]", event->connect.status);
            ots_chan_free(ots_chan_find_pending_by_conn(event->connect.conn_handle));
            /* Unwind the optimistic chan_add done by bt_l2cap_chan_connect()/
             * l2cap_accept(), else the upper layer waits for a result forever. */
            bt_le_l2cap_post_disconnected(event->connect.conn_handle, L2CAP_LE_OTS_PSM);
            return 0;
        }

        if (ots_chan_find_used_by_chan(event->connect.chan) != NULL) {
            LOG_ERR("[N]L2capCocChanExist");
            return 0;
        }

        /* Outgoing connect: reuse the pending slot reserved at chan_connect.
         * Incoming accept: no pending slot, allocate a fresh one. */
        slot = ots_chan_find_pending_by_conn(event->connect.conn_handle);
        if (slot != NULL) {
            slot->pending = false;
            slot->chan = event->connect.chan;
            slot->psm = event->connect.chan->psm;
            slot->used = true;
        } else {
            slot = ots_chan_alloc_used(event->connect.chan, event->connect.conn_handle);
            if (slot == NULL) {
                LOG_ERR("[N]L2capCocNoSlot");
                bt_le_l2cap_post_disconnected(event->connect.conn_handle, L2CAP_LE_OTS_PSM);
                return -EIO;
            }
        }

        if (ble_l2cap_get_chan_info(event->connect.chan, &chan_info)) {
            LOG_ERR("[N]L2capCocGetChanInfoFail");
            /* Roll back the slot so the next COC_CONNECTED isn't refused. */
            ots_chan_free(ots_chan_find_used_by_chan(event->connect.chan));
            bt_le_l2cap_post_disconnected(event->connect.conn_handle, L2CAP_LE_OTS_PSM);
            return -EIO;
        }

        LOG_INF("[N]L2capCocConnect[%u][%04x][%04x][%04x][%u][%u][%u][%u]",
                event->connect.conn_handle, chan_info.scid, chan_info.dcid,
                chan_info.psm, chan_info.our_l2cap_mtu, chan_info.peer_l2cap_mtu,
                chan_info.our_coc_mtu, chan_info.peer_coc_mtu);

        bt_le_l2cap_post_connected(event->connect.conn_handle, chan_info.psm,
                                   chan_info.dcid, chan_info.peer_coc_mtu,
                                   chan_info.scid, chan_info.our_coc_mtu);
        return 0;
    }

    case BLE_L2CAP_EVENT_COC_DISCONNECTED: {
        struct ots_chan_slot *slot;

        slot = ots_chan_find_used_by_chan(event->disconnect.chan);
        if (slot == NULL) {
            LOG_ERR("[N]L2capDisconnectInvCocChan");
            return 0;
        }

        psm = slot->psm;
        /* Drop the slot before posting: NimBLE frees the chan once this returns,
         * and the queue pairing then makes iso_task not find a dead chan. */
        ots_chan_free(slot);

        LOG_INF("[N]L2capCocDisconnect[%u][%04x]", event->disconnect.conn_handle, psm);

        bt_le_l2cap_post_disconnected(event->disconnect.conn_handle, psm);
        return 0;
    }

    case BLE_L2CAP_EVENT_COC_ACCEPT:
        /* LE CoC allows asymmetric MTUs; peer_sdu_size is the peer's RX MTU
         * (our TX ceiling) — harmless. (PTS uses 1024 > our 256; rejecting broke SCP.) */

        LOG_DBG("[N]L2capCocAccept[%u][%04x][%04x][%u][%u][%u][%u]",
                event->accept.conn_handle, event->accept.chan->psm,
                event->accept.chan->dcid, event->accept.chan->coc_tx.mtu,
                event->accept.chan->peer_coc_mps, event->accept.peer_sdu_size,
                event->accept.chan->coc_tx.credits);

        err = bt_le_l2cap_accept_safe(event->accept.conn_handle,
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

    case BLE_L2CAP_EVENT_COC_DATA_RECEIVED: {
        if (event->receive.sdu_rx == NULL) {
            LOG_ERR("[N]L2capRecvNullSdu");
            return 0;
        }

        LOG_DBG("[N]L2capCocRecv[%u][%04x][%u]",
                event->receive.conn_handle, event->receive.chan->psm,
                event->receive.sdu_rx->om_len);

        sdu_len = OS_MBUF_PKTLEN(event->receive.sdu_rx);

        sdu = bt_le_ext_calloc(1, sdu_len);
        if (sdu == NULL) {
            LOG_ERR("[N]L2capRecvNoMem[%u]", (unsigned)sdu_len);
            /* Drop the SDU but keep the channel usable: hand a fresh RX buffer
             * back so the peer can retry rather than stalling on credits. */
            os_mbuf_free_chain(event->receive.sdu_rx);
            return ots_l2cap_recv_ready(event->receive.chan);
        }

        err = os_mbuf_copydata(event->receive.sdu_rx, 0, sdu_len, sdu);
        if (err) {
            LOG_ERR("[N]L2capRecvCopyFail[%d]", err);
        } else {
            bt_le_l2cap_post_received(event->receive.conn_handle,
                                      event->receive.chan->psm, sdu, sdu_len);
        }

        os_mbuf_free_chain(event->receive.sdu_rx);
        free(sdu);

        return ots_l2cap_recv_ready(event->receive.chan);
    }

    case BLE_L2CAP_EVENT_COC_TX_UNSTALLED: {
        LOG_DBG("[N]L2capCocTxUnstalled[%u][%d]",
                event->tx_unstalled.conn_handle, event->tx_unstalled.status);

        /* Report completion even on failure: ops->sent carries no status, and
         * silence would leave the upper layer's TX pending forever. */
        if (event->tx_unstalled.status) {
            LOG_ERR("[N]L2capCocTxFail[%d]", event->tx_unstalled.status);
        }

        bt_le_l2cap_post_sent(event->tx_unstalled.conn_handle,
                              event->tx_unstalled.chan->psm);
        return 0;
    }

    default:
        return 0;
    }
}

int bt_le_nimble_l2cap_chan_connect(uint16_t conn_handle)
{
    struct os_mbuf *sdu_rx;
    struct ots_chan_slot *slot;
    int rc;

    /* Reject if this connection already has an OTS channel. ots_chans is a
     * cache under ble_hs_lock, so no host_lock here. */
    for (int i = 0; i < L2CAP_OTS_MAX_CHAN; i++) {
        if ((ots_chans[i].used || ots_chans[i].pending) &&
                ots_chans[i].conn_handle == conn_handle) {
            LOG_WRN("[N]L2capOtsChanExist[%u]", conn_handle);
            return -EALREADY;
        }
    }
    slot = ots_chan_alloc_pending(conn_handle);
    if (slot == NULL) {
        LOG_ERR("[N]L2capOtsNoSlot[%u]", conn_handle);
        return -ENOMEM;
    }

    sdu_rx = os_mbuf_get_pkthdr(&ots_mbuf_pool, 0);
    if (sdu_rx == NULL) {
        ots_chan_free(slot);
        LOG_ERR("[N]L2capNoBufForConnect");
        return -ENOMEM;
    }

    rc = ble_l2cap_connect(conn_handle, L2CAP_LE_OTS_PSM, L2CAP_LE_OTS_MTU,
                           sdu_rx, ots_l2cap_event_cb, NULL);
    if (rc) {
        /* NimBLE takes ownership of sdu_rx and frees it on every failure path */
        ots_chan_free(slot);
        LOG_ERR("[N]L2capConnectFail[%d]", rc);
        return nimble_err_to_errno(rc);
    }

    return 0;
}

int bt_le_nimble_l2cap_chan_disconnect(struct bt_l2cap_chan *chan)
{
    struct ots_chan_slot *slot;
    struct ble_l2cap_chan *ble_chan = NULL;
    uint16_t conn_handle_h;
    struct ble_gap_conn_desc desc;
    int rc;

    slot = ots_chan_find_used_by_conn(chan->conn->handle);
    if (slot == NULL) {
        LOG_WRN("[N]L2capNoOtsChan");
        return -ENOTCONN;
    }
    ble_chan = slot->chan;
    conn_handle_h = slot->conn_handle;

    /* Best-effort liveness check: narrows the window where ble_chan could be
     * freed between this lookup and ble_l2cap_disconnect acquiring ble_hs_lock. */
    if (ble_gap_conn_find(conn_handle_h, &desc) != 0) {
        LOG_WRN("[N]L2capDiscConnGone[%u]", conn_handle_h);
        return -ENOTCONN;
    }

    rc = ble_l2cap_disconnect(ble_chan);
    if (rc) {
        LOG_ERR("[N]L2capDisconnectFail[%d]", rc);
        return nimble_err_to_errno(rc);
    }

    return 0;
}

int bt_le_nimble_l2cap_chan_send(struct bt_l2cap_chan *chan, struct net_buf *buf)
{
    struct ots_chan_slot *slot;
    struct ble_l2cap_chan *ble_chan = NULL;
    uint16_t conn_handle_h, psm_h;
    struct os_mbuf *sdu_tx;
    struct ble_gap_conn_desc desc;
    int rc;

    slot = ots_chan_find_used_by_conn(chan->conn->handle);
    if (slot == NULL) {
        LOG_WRN("[N]L2capNoOtsChan");
        return -ENOTCONN;
    }
    ble_chan = slot->chan;
    conn_handle_h = slot->conn_handle;
    psm_h = slot->psm;

    /* Best-effort liveness check: ble_gap_conn_find takes ble_hs_lock, so the
     * conn can't be torn down while it runs. It narrows but does not close the
     * window before ble_l2cap_send — that reads chan->coc_tx.mtu before taking
     * ble_hs_lock, so a concurrently freed chan is still dereferenced there. */
    if (ble_gap_conn_find(conn_handle_h, &desc) != 0) {
        LOG_WRN("[N]L2capSendConnGone[%u]", conn_handle_h);
        return -ENOTCONN;
    }

    sdu_tx = os_mbuf_get_pkthdr(&ots_mbuf_pool, 0);
    if (sdu_tx == NULL) {
        LOG_ERR("[N]L2capNoBufForSend");
        return -ENOMEM;
    }

    rc = os_mbuf_append(sdu_tx, buf->data, buf->len);
    if (rc) {
        LOG_ERR("[N]L2capAppendBufFail[%d]", rc);
        os_mbuf_free_chain(sdu_tx);
        return -EIO;
    }

    rc = ble_l2cap_send(ble_chan, sdu_tx);
    if (rc && rc != BLE_HS_ESTALLED) {
        if (rc == BLE_HS_EBADDATA || rc == BLE_HS_EBUSY) {
            /* sdu was rejected before being queued; caller still owns it. */
            LOG_ERR("[N]L2capSendFail[%d]", rc);
            os_mbuf_free_chain(sdu_tx);
        } else {
            /* NimBLE only consumes the mbuf on success; free it on any failure. */
            LOG_ERR("[N]L2capSendInternalFail[%d]", rc);
            os_mbuf_free_chain(sdu_tx);
        }

        return nimble_err_to_errno(rc);
    }

    /* Payload now in sdu_tx; release caller's buf else ot_chan_tx_pool
     * (1 entry) leaks. */
    net_buf_unref(buf);

    if (rc == BLE_HS_ESTALLED) {
        /* Out of credits: queued, reported by COC_TX_UNSTALLED. If the conn
         * drops while stalled, NimBLE fires only COC_DISCONNECTED; the OTS
         * disconnected callback cleans up tx state, so the missing sent is
         * harmless. */
        LOG_DBG("[N]L2capMoreCreditsForSend");
        return 0;
    }

    /* Fully transmitted; NimBLE raises COC_TX_UNSTALLED only for a stalled SDU,
     * so this is the sole completion signal. */
    bt_le_l2cap_post_sent(conn_handle_h, psm_h);

    return 0;
}

int bt_le_nimble_l2cap_init(void)
{
    int rc;

    memset(ots_chans, 0, sizeof(ots_chans));

    rc = os_mempool_init(&ots_mbuf_mempool, OTS_L2CAP_BUF_COUNT, L2CAP_LE_OTS_MTU * 2, ots_mem, "ots_pool");
    if (rc) {
        LOG_ERR("[N]L2capInitOtsMempoolFail[%d]", rc);
        return rc;
    }

    rc = os_mbuf_pool_init(&ots_mbuf_pool, &ots_mbuf_mempool, L2CAP_LE_OTS_MTU, OTS_L2CAP_BUF_COUNT);
    if (rc) {
        LOG_ERR("[N]L2capInitOtsMbufPoolFail[%d]", rc);
        return rc;
    }

#if CONFIG_BT_OTS
    rc = ble_l2cap_create_server(L2CAP_LE_OTS_PSM, L2CAP_LE_OTS_MTU, ots_l2cap_event_cb, NULL);
    /* NimBLE has no delete-server API, so the registration from a previous cycle
     * survives deinit. It still points at ots_l2cap_event_cb, so treat EALREADY
     * as success - failing here would make any re-init impossible. */
    if (rc == BLE_HS_EALREADY) {
        LOG_WRN("[N]L2capL2capSrvExist");
    } else if (rc) {
        LOG_ERR("[N]L2capCreateL2capSrvFail[%d]", rc);
        return rc;
    }
#endif /* CONFIG_BT_OTS */

    return 0;
}

void bt_le_nimble_l2cap_deinit(void)
{
    LOG_DBG("[N]L2capDeinit");

    /* Nothing to release: ots_mem is static BSS and os_mempool_init re-registers
     * an existing pool by name, so re-init needs no teardown here. The CoC server
     * cannot be dropped either - NimBLE exposes no delete-server API - which is
     * why init tolerates BLE_HS_EALREADY. */

    memset(ots_chans, 0, sizeof(ots_chans));
}

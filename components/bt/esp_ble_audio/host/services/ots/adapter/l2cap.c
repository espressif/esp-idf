/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileCopyrightText: 2023 Nordic Semiconductor
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/l2cap.h>

#include <../host/conn_internal.h>

#include "common/host.h"
#include "common/audio_attr.h"
#include "common/app/gap.h"

#include "ots/adapter/l2cap.h"

LOG_MODULE_REGISTER(ISO_L2CAP, CONFIG_BT_ISO_LOG_LEVEL);

#define L2CAP_LE_MIN_MTU                23
#define L2CAP_ECRED_MIN_MTU             64

#define L2CAP_LE_CID_DYN_START          0x0040
#define L2CAP_LE_CID_DYN_END            0x007F
#define L2CAP_LE_CID_IS_DYN(_cid)       (_cid >= L2CAP_LE_CID_DYN_START && _cid <= L2CAP_LE_CID_DYN_END)

#define L2CAP_LE_PSM_FIXED_START        0x0001
#define L2CAP_LE_PSM_FIXED_END          0x007F
#define L2CAP_LE_PSM_DYN_START          0x0080
#define L2CAP_LE_PSM_DYN_END            0x00FF
#define L2CAP_LE_PSM_IS_DYN(_psm)       (_psm >= L2CAP_LE_PSM_DYN_START && _psm <= L2CAP_LE_PSM_DYN_END)

static BT_AUDIO_EXT_RAM_BSS_ATTR sys_slist_t l2cap_servers;

/* OTS internals, declared here rather than including the OTS private headers. */
#if CONFIG_BT_OTS
extern int bt_gatt_ots_conn_cb_register(void);
extern void bt_gatt_ots_conn_cb_unregister(void);
extern int bt_gatt_ots_instances_prepare(void);
extern void bt_ots_instances_rewind(void);
#endif /* CONFIG_BT_OTS */
#if CONFIG_BT_OTS_CLIENT
extern int bt_gatt_ots_client_conn_cb_register(void);
extern void bt_gatt_ots_client_conn_cb_unregister(void);
extern void bt_ots_client_unregister_all(void);
#endif /* CONFIG_BT_OTS_CLIENT */
extern int bt_gatt_ots_l2cap_init(void);

static struct bt_l2cap_chan *l2cap_lookup_tx_cid(struct bt_conn *conn, uint16_t cid)
{
    struct bt_l2cap_chan *chan;

    SYS_SLIST_FOR_EACH_CONTAINER(&conn->channels, chan, node) {
        if (BT_L2CAP_LE_CHAN(chan)->tx.cid == cid) {
            return chan;
        }
    }

    return NULL;
}

__attribute__((unused))
static struct bt_l2cap_chan *l2cap_lookup_rx_cid(struct bt_conn *conn, uint16_t cid)
{
    struct bt_l2cap_chan *chan;

    SYS_SLIST_FOR_EACH_CONTAINER(&conn->channels, chan, node) {
        if (BT_L2CAP_LE_CHAN(chan)->rx.cid == cid) {
            return chan;
        }
    }

    return NULL;
}

static struct bt_l2cap_chan *l2cap_lookup_psm(struct bt_conn *conn, uint16_t psm)
{
    struct bt_l2cap_chan *chan;

    SYS_SLIST_FOR_EACH_CONTAINER(&conn->channels, chan, node) {
        if (BT_L2CAP_LE_CHAN(chan)->psm == psm) {
            return chan;
        }
    }

    return NULL;
}

static bool l2cap_chan_add(struct bt_conn *conn, struct bt_l2cap_chan *chan, uint16_t psm)
{
    LOG_DBG("L2capChanAdd[%04x]", psm);

    /* Attach channel to the connection */
    if (sys_slist_find(&conn->channels, &chan->node, NULL)) {
        LOG_WRN("L2capChanExists[%04x]", psm);
        return false;
    }

    sys_slist_append(&conn->channels, &chan->node);
    chan->conn = conn;

    /* Set channel PSM */
    BT_L2CAP_LE_CHAN(chan)->psm = psm;

    return true;
}

static struct bt_l2cap_server *l2cap_server_lookup_psm(uint16_t psm)
{
    struct bt_l2cap_server *server = NULL;

    SYS_SLIST_FOR_EACH_CONTAINER(&l2cap_servers, server, node) {
        if (server->psm == psm) {
            break;
        }
    }

    return server;
}

static inline uint16_t err_to_result(int err)
{
    switch (err) {
    case -ENOMEM:
        return L2CAP_LE_ERR_NO_RESOURCES;
    case -EACCES:
        return L2CAP_LE_ERR_AUTHORIZATION;
    case -EPERM:
        return L2CAP_LE_ERR_KEY_SIZE;
    case -ENOTSUP:
        /* This handle the cases where a fixed channel is registered but
         * for some reason (e.g. controller not supporting a feature)
         * cannot be used.
         */
        return L2CAP_LE_ERR_PSM_NOT_SUPP;
    default:
        return L2CAP_LE_ERR_UNACCEPT_PARAMS;
    }
}

static int l2cap_accept(uint16_t conn_handle, uint16_t psm,
                        uint16_t scid, uint16_t mtu,
                        uint16_t mps, uint16_t credits,
                        uint16_t *result)
{
    struct bt_l2cap_server *server;
    struct bt_l2cap_chan *chan;
    struct bt_conn *conn;
    int err;

    ARG_UNUSED(credits);

    LOG_DBG("L2capAccept[%u][%04x][%04x][%u][%u]", conn_handle, psm, scid, mtu, mps);

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_INF("L2capAcceptNotConn[%u][%u]", conn_handle, BT_CONN_STATE_GET(conn));

        *result = L2CAP_LE_ERR_INVALID_PARAMS;
        return -ENOTCONN;
    }

    /* Check if there is a server registered */
    server = l2cap_server_lookup_psm(psm);
    if (server == NULL) {
        LOG_ERR("L2capSrvNotReg[%04x]", psm);

        *result = L2CAP_LE_ERR_PSM_NOT_SUPP;
        return -ENOTSUP;
    }

    if (!L2CAP_LE_CID_IS_DYN(scid)) {
        LOG_ERR("L2capNotDynScid[%04x]", scid);

        *result = L2CAP_LE_ERR_INVALID_SCID;
        return -EINVAL;
    }

    chan = l2cap_lookup_tx_cid(conn, scid);
    if (chan) {
        LOG_WRN("L2capScidUsed[%04x]", scid);

        *result = L2CAP_LE_ERR_SCID_IN_USE;
        return -EALREADY;
    }

    /* Every event is routed back by (conn, psm), so a second channel for the
     * same pair would deliver to whichever chan l2cap_lookup_psm() hits first. */
    if (l2cap_lookup_psm(conn, psm) != NULL) {
        LOG_WRN("L2capPsmChanExists[%u][%04x]", conn_handle, psm);

        *result = L2CAP_LE_ERR_NO_RESOURCES;
        return -EALREADY;
    }

    if (server->accept == NULL) {
        LOG_ERR("L2capSrvAcceptNull");

        *result = L2CAP_LE_ERR_INVALID_PARAMS;
        return -EIO;
    }

    err = server->accept(conn, server, &chan);
    if (err) {
        LOG_ERR("L2capSrvAcceptFail[%d]", err);

        *result = err_to_result(err);
        return -EIO;
    }

    if (chan == NULL) {
        LOG_ERR("L2capSrvAcceptNullChan");
        *result = L2CAP_LE_ERR_NO_RESOURCES;
        return -ENOMEM;
    }

    if (l2cap_chan_add(conn, chan, psm) == false) {
        *result = L2CAP_LE_ERR_NO_RESOURCES;
        return -ENOMEM;
    }

    *result = L2CAP_LE_SUCCESS;
    return 0;
}

#if CONFIG_BT_BLUEDROID_ENABLED
static void l2cap_handle_accept(const struct bt_le_l2cap_event *qev)
{
    uint16_t result = L2CAP_LE_ERR_NO_RESOURCES;

    /* chan_handle is our local CID; the peer's is unknown until the channel is
     * up, so it stands in for scid (dynamic-CID check and duplicate detection).
     * mtu/mps are logging-only in l2cap_accept(). */
    (void)l2cap_accept(qev->conn_handle, qev->psm, qev->accept.chan_handle,
                       0, 0, 0, &result);

    /* Forward result, not a bool: l2cap_accept() distinguishes authentication,
     * key size and SCID-in-use, and the peer is entitled to the exact reason. */
    bt_le_bluedroid_l2cap_accept_result(qev->conn_handle, qev->accept.l2cap_id,
                                        qev->accept.chan_handle, result);
}
#endif /* CONFIG_BT_BLUEDROID_ENABLED */

static void l2cap_connected(uint16_t conn_handle, uint16_t psm,
                            uint16_t tx_cid, uint16_t tx_mtu,
                            uint16_t rx_cid, uint16_t rx_mtu)
{
    struct bt_l2cap_chan *chan;
    struct bt_conn *conn;

    LOG_DBG("L2capConnected[%u][%04x][%04x][%u][%04x][%u]",
            conn_handle, psm, tx_cid, tx_mtu, rx_cid, rx_mtu);

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_INF("L2capConnectedNotConn[%u][%u]", conn_handle, BT_CONN_STATE_GET(conn));
        return;
    }

    chan = l2cap_lookup_psm(conn, psm);
    if (chan == NULL) {
        LOG_ERR("L2capPsmNotFound[%04x]", psm);
        return;
    }

    BT_L2CAP_LE_CHAN(chan)->tx.cid = tx_cid;
    BT_L2CAP_LE_CHAN(chan)->tx.mtu = tx_mtu;
    BT_L2CAP_LE_CHAN(chan)->rx.cid = rx_cid;
    BT_L2CAP_LE_CHAN(chan)->rx.mtu = rx_mtu;

    if (chan->ops->connected) {
        chan->ops->connected(chan);
    }
}

static void l2cap_disconnected(uint16_t conn_handle, uint16_t psm)
{
    struct bt_l2cap_chan *chan;
    struct bt_conn *conn;

    LOG_DBG("L2capDisconnected[%u][%04x]", conn_handle, psm);

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_INF("L2capDisconnectedNotConn[%u][%u]", conn_handle, BT_CONN_STATE_GET(conn));
        return;
    }

    chan = l2cap_lookup_psm(conn, psm);
    if (chan == NULL) {
        LOG_ERR("L2capPsmNotFound[%04x]", psm);
        return;
    }

    /* Unlink first so no later event rediscovers it by PSM. The callback must not
     * free chan: it reads chan->conn, and clearing conn after returns the pool ctx. */
    sys_slist_find_and_remove(&conn->channels, &chan->node);

    if (chan->ops->disconnected) {
        chan->ops->disconnected(chan);
    }

    chan->conn = NULL;
}

static void l2cap_received(uint16_t conn_handle, uint16_t psm,
                           uint8_t *data, uint16_t len)
{
    struct bt_l2cap_chan *chan;
    struct net_buf buf = {0};
    struct bt_conn *conn;

    LOG_DBG("L2capReceived[%u][%04x][%u]", conn_handle, psm, len);

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_INF("L2capReceivedNotConn[%u][%u]", conn_handle, BT_CONN_STATE_GET(conn));
        return;
    }

    chan = l2cap_lookup_psm(conn, psm);
    if (chan == NULL) {
        LOG_ERR("L2capPsmNotFound[%04x]", psm);
        return;
    }

    buf.data = data;
    buf.len = len;

    if (chan->ops->recv) {
        chan->ops->recv(chan, &buf);
    }
}

static void l2cap_sent(uint16_t conn_handle, uint16_t psm)
{
    struct bt_l2cap_chan *chan;
    struct bt_conn *conn;

    LOG_DBG("L2capSent[%u][%04x]", conn_handle, psm);

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_INF("L2capSentNotConn[%u][%u]", conn_handle, BT_CONN_STATE_GET(conn));
        return;
    }

    chan = l2cap_lookup_psm(conn, psm);
    if (chan == NULL) {
        LOG_ERR("L2capPsmNotFound[%04x]", psm);
        return;
    }

    if (chan->ops->sent) {
        chan->ops->sent(chan);
    }
}

_IDF_ONLY
void bt_le_l2cap_event_free(void *data)
{
    struct bt_le_l2cap_event *qev = data;

    if (qev == NULL) {
        return;
    }

    /* Only RECEIVED carries a buffer of its own; the rest are header-only. */
    if (qev->type == BT_LE_L2CAP_EVENT_RECEIVED) {
        free(qev->received.data);
    }

    free(qev);
}

static int l2cap_post_event(struct bt_le_l2cap_event *qev)
{
    int err;

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_L2CAP_EVENT, qev, sizeof(*qev));
    if (err) {
        ISO_POST_FAIL_LOG(err, "L2capPostFail[%d][%u]", err, qev->type);
        bt_le_l2cap_event_free(qev);
    }

    return err;
}

/* Alloc + stamp the common header. NULL on OOM rather than abort: L2CAP is off
 * the ISO data path, so losing one event beats taking the whole stack down. */
static struct bt_le_l2cap_event *l2cap_event_alloc(uint8_t type, uint16_t conn_handle,
                                                   uint16_t psm)
{
    struct bt_le_l2cap_event *qev;

    qev = bt_le_ext_calloc(1, sizeof(*qev));
    if (qev == NULL) {
        return NULL;
    }

    qev->type = type;
    qev->conn_handle = conn_handle;
    qev->psm = psm;

    return qev;
}

_IDF_ONLY
int bt_le_l2cap_post_connected(uint16_t conn_handle, uint16_t psm,
                               uint16_t tx_cid, uint16_t tx_mtu,
                               uint16_t rx_cid, uint16_t rx_mtu)
{
    struct bt_le_l2cap_event *qev;

    qev = l2cap_event_alloc(BT_LE_L2CAP_EVENT_CONNECTED, conn_handle, psm);
    if (qev == NULL) {
        LOG_ERR("L2capConnectedNoMem[%u][%04x]", conn_handle, psm);
        return -ENOMEM;
    }

    qev->connected.tx_cid = tx_cid;
    qev->connected.tx_mtu = tx_mtu;
    qev->connected.rx_cid = rx_cid;
    qev->connected.rx_mtu = rx_mtu;

    return l2cap_post_event(qev);
}

_IDF_ONLY
int bt_le_l2cap_post_disconnected(uint16_t conn_handle, uint16_t psm)
{
    struct bt_le_l2cap_event *qev;

    qev = l2cap_event_alloc(BT_LE_L2CAP_EVENT_DISCONNECTED, conn_handle, psm);
    if (qev == NULL) {
        LOG_ERR("L2capDisconnectedNoMem[%u][%04x]", conn_handle, psm);
        return -ENOMEM;
    }

    return l2cap_post_event(qev);
}

_IDF_ONLY
int bt_le_l2cap_post_received(uint16_t conn_handle, uint16_t psm,
                              const uint8_t *data, uint16_t len)
{
    struct bt_le_l2cap_event *qev;

    qev = l2cap_event_alloc(BT_LE_L2CAP_EVENT_RECEIVED, conn_handle, psm);
    if (qev == NULL) {
        LOG_ERR("L2capReceivedNoMem[%u][%04x][%u]", conn_handle, psm, len);
        return -ENOMEM;
    }

    qev->received.len = len;

    if (len) {
        qev->received.data = bt_le_ext_calloc(1, len);
        if (qev->received.data == NULL) {
            LOG_ERR("L2capReceivedNoDataMem[%u][%04x][%u]", conn_handle, psm, len);
            free(qev);
            return -ENOMEM;
        }
        memcpy(qev->received.data, data, len);
    }

    return l2cap_post_event(qev);
}

_IDF_ONLY
int bt_le_l2cap_post_sent(uint16_t conn_handle, uint16_t psm)
{
    struct bt_le_l2cap_event *qev;

    qev = l2cap_event_alloc(BT_LE_L2CAP_EVENT_SENT, conn_handle, psm);
    if (qev == NULL) {
        LOG_ERR("L2capSentNoMem[%u][%04x]", conn_handle, psm);
        return -ENOMEM;
    }

    return l2cap_post_event(qev);
}

#if CONFIG_BT_BLUEDROID_ENABLED
_IDF_ONLY
int bt_le_l2cap_post_accept(uint16_t conn_handle, uint16_t psm,
                            uint16_t chan_handle, uint8_t l2cap_id)
{
    struct bt_le_l2cap_event *qev;

    qev = l2cap_event_alloc(BT_LE_L2CAP_EVENT_ACCEPT, conn_handle, psm);
    if (qev == NULL) {
        LOG_ERR("L2capAcceptNoMem[%u][%04x][%04x]", conn_handle, psm, chan_handle);
        return -ENOMEM;
    }

    qev->accept.chan_handle = chan_handle;
    qev->accept.l2cap_id = l2cap_id;

    return l2cap_post_event(qev);
}
#else /* !CONFIG_BT_BLUEDROID_ENABLED */
_IDF_ONLY
int bt_le_l2cap_accept_safe(uint16_t conn_handle, uint16_t psm,
                            uint16_t scid, uint16_t mtu,
                            uint16_t mps, uint16_t credits,
                            uint16_t *result)
{
    int err;
    bt_le_host_lock();
    err = l2cap_accept(conn_handle, psm, scid, mtu, mps, credits, result);
    bt_le_host_unlock();
    return err;
}
#endif /* !CONFIG_BT_BLUEDROID_ENABLED */

_IDF_ONLY
void bt_le_l2cap_handle_event(void *data, size_t data_len)
{
    struct bt_le_l2cap_event *qev = data;

    if (qev == NULL) {
        LOG_ERR("L2capEvtNull");
        return;
    }

    if (data_len != sizeof(*qev)) {
        LOG_ERR("L2capEvtBadLen[%u]", (unsigned)data_len);
        free(qev);
        return;
    }

    bt_le_host_lock();

    switch (qev->type) {
    case BT_LE_L2CAP_EVENT_CONNECTED:
        l2cap_connected(qev->conn_handle, qev->psm,
                        qev->connected.tx_cid, qev->connected.tx_mtu,
                        qev->connected.rx_cid, qev->connected.rx_mtu);
        break;
    case BT_LE_L2CAP_EVENT_DISCONNECTED:
        l2cap_disconnected(qev->conn_handle, qev->psm);
        break;
    case BT_LE_L2CAP_EVENT_RECEIVED:
        l2cap_received(qev->conn_handle, qev->psm,
                       qev->received.data, qev->received.len);
        free(qev->received.data);
        break;
    case BT_LE_L2CAP_EVENT_SENT:
        l2cap_sent(qev->conn_handle, qev->psm);
        break;
#if CONFIG_BT_BLUEDROID_ENABLED
    case BT_LE_L2CAP_EVENT_ACCEPT:
        l2cap_handle_accept(qev);
        break;
#endif /* CONFIG_BT_BLUEDROID_ENABLED */
    default:
        LOG_ERR("L2capEvtUnknown[%u]", qev->type);
        break;
    }

    bt_le_host_unlock();

    free(qev);
}

_IDF_ONLY
int bt_l2cap_chan_connect(struct bt_conn *conn, struct bt_l2cap_chan *chan, uint16_t psm)
{
    int err;

    LOG_DBG("L2capChanConnect[%04x]", psm);

    if (chan == NULL) {
        LOG_ERR("L2capChanNull");
        return -EINVAL;
    }

    if (psm < L2CAP_LE_PSM_FIXED_START || psm > L2CAP_LE_PSM_DYN_END) {
        LOG_ERR("L2capInvPsm[%04x]", psm);
        return -EINVAL;
    }

    /* Caller holds bt_le_host_lock. chan_add is done BEFORE the adapter call
     * (optimistic): the Bluedroid adapter posts fire-and-forget, so the
     * CONNECTED/DISCONNECTED event needs the chan already in conn->channels.
     * On failure the adapter posts DISCONNECTED, matching the Zephyr
     * connect-fail contract. */
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("L2capChanNotConn[%p]", conn);
        return -ENOTCONN;
    }

    /* Same (conn, psm) uniqueness as l2cap_accept(): event routing keys on it.
     * l2cap_chan_add only rejects re-adding this very chan object. */
    if (l2cap_lookup_psm(conn, psm) != NULL) {
        LOG_WRN("L2capPsmChanExists[%u][%04x]", conn->handle, psm);
        return -EALREADY;
    }

    if (!l2cap_chan_add(conn, chan, psm)) {
        return -EALREADY;
    }

#if CONFIG_BT_BLUEDROID_ENABLED
    err = bt_le_bluedroid_l2cap_chan_connect(conn->handle);
#else
    err = bt_le_nimble_l2cap_chan_connect(conn->handle);
#endif
    if (err) {
        /* Roll back the optimistic chan_add. */
        sys_slist_find_and_remove(&conn->channels, &chan->node);
        chan->conn = NULL;
        return err;
    }

    return 0;
}

_IDF_ONLY
int bt_l2cap_chan_disconnect(struct bt_l2cap_chan *chan)
{
    int err;

    LOG_DBG("L2capChanDisconnect");

    if (chan == NULL) {
        LOG_ERR("L2capChanNull");
        return -EINVAL;
    }

    if (chan->conn == NULL || chan->conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("L2capChanNotConn[%p]", chan->conn);
        return -ENOTCONN;
    }

#if CONFIG_BT_BLUEDROID_ENABLED
    err = bt_le_bluedroid_l2cap_chan_disconnect(chan);
#else
    err = bt_le_nimble_l2cap_chan_disconnect(chan);
#endif
    if (err) {
        /* If the disconnect failed, remove the channel from the connection.
         * Otherwise the removal will be handled by the disconnect callback. */
        if (chan->conn != NULL) {
            sys_slist_find_and_remove(&chan->conn->channels, &chan->node);
            chan->conn = NULL;
        }
    }

    return err;
}

_IDF_ONLY
int bt_l2cap_chan_send(struct bt_l2cap_chan *chan, struct net_buf *buf)
{
    int err;

    if (chan == NULL || buf == NULL) {
        LOG_ERR("L2capChanBufNull[%p][%p]", chan, buf);
        return -EINVAL;
    }

    LOG_DBG("L2capChanSend[%u]", buf->len);

    /* Caller holds bt_le_host_lock. The Bluedroid adapter posts fire-and-forget;
     * returns 0 (buf ownership transferred) or error (buf stays with caller).
     * TX completion via ops->sent. */
    if (chan->conn == NULL || chan->conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("L2capChanNotConn[%p]", chan->conn);
        return -ENOTCONN;
    }

    if (buf->len > BT_L2CAP_LE_CHAN(chan)->tx.mtu) {
        LOG_ERR("L2capTooLargeBufToSend[%u][%u]", buf->len,
                BT_L2CAP_LE_CHAN(chan)->tx.mtu);
        return -EMSGSIZE;
    }

#if CONFIG_BT_BLUEDROID_ENABLED
    err = bt_le_bluedroid_l2cap_chan_send(chan, buf);
#else
    err = bt_le_nimble_l2cap_chan_send(chan, buf);
#endif

    return err;
}

_IDF_ONLY
int bt_l2cap_server_register(struct bt_l2cap_server *server)
{
    LOG_DBG("L2capSrvReg");

    if (server == NULL) {
        LOG_ERR("L2capSrvNull");
        return -EINVAL;
    }

    if (server->accept == NULL) {
        LOG_ERR("L2capSrvAcceptNull");
        return -EINVAL;
    }

    if (server->sec_level > BT_SECURITY_L4) {
        LOG_ERR("L2capInvSecLevel[%u]", server->sec_level);
        return -EINVAL;
    }

    /* Init path (no concurrency). l2cap_servers is shared with the accept
     * lookup in the event handler (iso_task). */
    if (server->psm) {
        if (server->psm < L2CAP_LE_PSM_FIXED_START ||
                server->psm > L2CAP_LE_PSM_DYN_END) {
            LOG_ERR("L2capInvPsm[%04x]", server->psm);
            return -EINVAL;
        }

        /* Check if given PSM is already in use */
        if (l2cap_server_lookup_psm(server->psm)) {
            LOG_WRN("L2capPsmReg");
            return -EADDRINUSE;
        }

        LOG_DBG("L2capSrvPsm[%04x]", server->psm);
    } else {
        uint16_t psm;

        for (psm = L2CAP_LE_PSM_DYN_START;
                psm <= L2CAP_LE_PSM_DYN_END; psm++) {
            if (l2cap_server_lookup_psm(psm) == NULL) {
                break;
            }
        }

        if (psm > L2CAP_LE_PSM_DYN_END) {
            LOG_ERR("L2capNoFreeDynPsm");
            return -EADDRNOTAVAIL;
        }

        LOG_DBG("L2capPsmNew[%04x]", psm);

        server->psm = psm;
    }

    if (server->sec_level < BT_SECURITY_L1) {
        server->sec_level = BT_SECURITY_L1;
    }

    sys_slist_append(&l2cap_servers, &server->node);

    return 0;
}

int bt_le_l2cap_ots_init(void)
{
    int err;

    LOG_DBG("L2capOtsInit");

    /* No unregister API, so a re-init would otherwise find the stale entry and
     * fail bt_l2cap_server_register() with -EADDRINUSE. */
    sys_slist_init(&l2cap_servers);

#if CONFIG_BT_OTS
    err = bt_gatt_ots_conn_cb_register();
    if (err) {
        LOG_ERR("L2capOtsConnCbRegFail[%d]", err);
        return err;
    }

    err = bt_gatt_ots_instances_prepare();
    if (err) {
        LOG_ERR("L2capPrepOtsInstsFail[%d]", err);
        goto unreg_conn_cb;
    }
#endif /* CONFIG_BT_OTS */

#if CONFIG_BT_OTS_CLIENT
    err = bt_gatt_ots_client_conn_cb_register();
    if (err) {
        LOG_ERR("L2capOtsCliConnCbRegFail[%d]", err);
        goto unreg_conn_cb;
    }
#endif /* CONFIG_BT_OTS_CLIENT */

    err = bt_gatt_ots_l2cap_init();
    if (err) {
        LOG_ERR("L2capOtsInitFail[%d]", err);
        goto unreg_client_conn_cb;
    }

#if CONFIG_BT_BLUEDROID_ENABLED
    err = bt_le_bluedroid_l2cap_init();
#else
    err = bt_le_nimble_l2cap_init();
#endif
    if (err) {
        LOG_ERR("L2capOtsAdapterInitFail[%d]", err);
        /* bt_l2cap_server_register() has no unregister, so drop the whole list
         * rather than leave the OTS PSM claimed by a half-initialised service. */
        sys_slist_init(&l2cap_servers);
        goto unreg_client_conn_cb;
    }

    return 0;

unreg_client_conn_cb:
#if CONFIG_BT_OTS_CLIENT
    bt_gatt_ots_client_conn_cb_unregister();
#endif /* CONFIG_BT_OTS_CLIENT */
unreg_conn_cb:
#if CONFIG_BT_OTS
    /* Leaving it registered would fail every later retry with -EEXIST. */
    bt_gatt_ots_conn_cb_unregister();
#endif /* CONFIG_BT_OTS */
    return err;
}

void bt_le_l2cap_ots_deinit(void)
{
    LOG_DBG("L2capOtsDeinit");

    /* Symmetric with the registers in _init: conn_cbs is never reset elsewhere. */
#if CONFIG_BT_OTS
    bt_gatt_ots_conn_cb_unregister();

    /* Give the instances handed out by bt_ots_free_instance_get() back to the
     * pool; the profiles that took them are released right after this. */
    bt_ots_instances_rewind();
#endif /* CONFIG_BT_OTS */
#if CONFIG_BT_OTS_CLIENT
    bt_gatt_ots_client_conn_cb_unregister();

    /* Before lib_audio_resources_deinit() frees the bt_ots_client instances
     * the table points at - this runs first in bt_le_audio_deinit(). */
    bt_ots_client_unregister_all();
#endif /* CONFIG_BT_OTS_CLIENT */

#if CONFIG_BT_BLUEDROID_ENABLED
    bt_le_bluedroid_l2cap_deinit();
#else
    bt_le_nimble_l2cap_deinit();
#endif
}

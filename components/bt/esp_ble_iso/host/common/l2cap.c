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
#include <ots_l2cap_internal.h>

#include "common/host.h"
#include "common/app/gap.h"

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

static sys_slist_t l2cap_servers = SYS_SLIST_STATIC_INIT(&l2cap_servers);

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

_IDF_ONLY
int bt_le_l2cap_accept(uint16_t conn_handle, uint16_t psm,
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
        LOG_ERR("NotConn[%d]", __LINE__);

        *result = L2CAP_LE_ERR_INVALID_PARAMS;
        return -ENOTCONN;
    }

    /* Check if there is a server registered */
    server = l2cap_server_lookup_psm(psm);
    if (server == NULL) {
        LOG_ERR("SrvNotReg[%04x]", psm);

        *result = L2CAP_LE_ERR_PSM_NOT_SUPP;
        return -ENOTSUP;
    }

    if (!L2CAP_LE_CID_IS_DYN(scid)) {
        LOG_ERR("NotDynScid[%04x]", scid);

        *result = L2CAP_LE_ERR_INVALID_SCID;
        return -EINVAL;
    }

    chan = l2cap_lookup_tx_cid(conn, scid);
    if (chan) {
        LOG_WRN("ScidUsed[%04x]", scid);

        *result = L2CAP_LE_ERR_SCID_IN_USE;
        return -EALREADY;
    }

    if (server->accept == NULL) {
        LOG_ERR("SrvAcceptNull");

        *result = L2CAP_LE_ERR_INVALID_PARAMS;
        return -EIO;
    }

    err = server->accept(conn, server, &chan);
    if (err) {
        LOG_ERR("SrvAcceptFail[%d]", err);

        *result = err_to_result(err);
        return -EIO;
    }

    if (chan == NULL) {
        LOG_ERR("SrvAcceptNullChan");
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

_IDF_ONLY
void bt_le_l2cap_connected(uint16_t conn_handle, uint16_t psm,
                           uint16_t tx_cid, uint16_t tx_mtu,
                           uint16_t rx_cid, uint16_t rx_mtu)
{
    struct bt_l2cap_chan *chan;
    struct bt_conn *conn;

    LOG_DBG("L2capConnected[%u][%04x][%04x][%u][%04x][%u]",
            conn_handle, psm, tx_cid, tx_mtu, rx_cid, rx_mtu);

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("NotConn[%d]", __LINE__);
        return;
    }

    chan = l2cap_lookup_psm(conn, psm);
    if (chan == NULL) {
        LOG_ERR("PsmNotFound[%04x]", psm);
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

#define L2CAP_UGLY_TX_DONE_WORKAROUND   1

_IDF_ONLY
void bt_le_l2cap_disconnected(uint16_t conn_handle, uint16_t psm)
{
#if L2CAP_UGLY_TX_DONE_WORKAROUND
    struct bt_gatt_ots_l2cap *l2cap_ctx;
    struct bt_l2cap_le_chan *l2chan;
#endif /* L2CAP_UGLY_TX_DONE_WORKAROUND */
    struct bt_l2cap_chan *chan;
    struct bt_conn *conn;

    LOG_DBG("L2capDisconnected[%u][%04x]", conn_handle, psm);

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("NotConn[%d]", __LINE__);
        return;
    }

    chan = l2cap_lookup_psm(conn, psm);
    if (chan == NULL) {
        LOG_ERR("PsmNotFound[%04x]", psm);
        return;
    }

#if L2CAP_UGLY_TX_DONE_WORKAROUND
    l2chan = CONTAINER_OF(chan, struct bt_l2cap_le_chan, chan);
    l2cap_ctx = CONTAINER_OF(l2chan, struct bt_gatt_ots_l2cap, ot_chan);

    if (chan->ops->sent && l2cap_ctx->tx_done) {
        chan->ops->sent(chan);
    }
#endif /* L2CAP_UGLY_TX_DONE_WORKAROUND */

    if (chan->ops->disconnected) {
        chan->ops->disconnected(chan);
    }

    sys_slist_find_and_remove(&conn->channels, &chan->node);
    chan->conn = NULL;
}

_IDF_ONLY
void bt_le_l2cap_received(uint16_t conn_handle, uint16_t psm,
                          uint8_t *data, uint16_t len)
{
    struct bt_l2cap_chan *chan;
    struct net_buf buf = {0};
    struct bt_conn *conn;

    LOG_DBG("L2capReceived[%u][%04x][%u]", conn_handle, psm, len);

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("NotConn[%d]", __LINE__);
        return;
    }

    chan = l2cap_lookup_psm(conn, psm);
    if (chan == NULL) {
        LOG_ERR("PsmNotFound[%04x]", psm);
        return;
    }

    buf.data = data;
    buf.len = len;

    if (chan->ops->recv) {
        chan->ops->recv(chan, &buf);
    }
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

    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("L2capChanNotConn[%p]", conn);
        return -ENOTCONN;
    }

    if (psm < L2CAP_LE_PSM_FIXED_START || psm > L2CAP_LE_PSM_DYN_END) {
        LOG_ERR("InvPsm[%04x]", psm);
        return -EINVAL;
    }

    err = bt_le_nimble_l2cap_chan_connect(conn->handle);
    if (err) {
        return err;
    }

    l2cap_chan_add(conn, chan, psm);

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

    err = bt_le_nimble_l2cap_chan_disconnect(chan);
    if (err) {
        /* If the disconnect failed, remove the channel from the connection.
         * Otherwise the removal will be handled by the disconnect callback.
         */
        sys_slist_find_and_remove(&chan->conn->channels, &chan->node);
        chan->conn = NULL;
    }

    return err;
}

_IDF_ONLY
int bt_l2cap_chan_send(struct bt_l2cap_chan *chan, struct net_buf *buf)
{
    if (chan == NULL || buf == NULL) {
        LOG_ERR("L2capChanBufNull[%p][%p]", chan, buf);
        return -EINVAL;
    }

    LOG_DBG("L2capChanSend[%u]", buf->len);

    if (chan->conn == NULL || chan->conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("L2capChanNotConn[%p]", chan->conn);
        return -ENOTCONN;
    }

    if (buf->len > L2CAP_LE_OTS_MTU) {
        LOG_ERR("L2capTooLargeBufToSend[%u][%u]", buf->len, L2CAP_LE_OTS_MTU);
        return -EMSGSIZE;
    }

    return bt_le_nimble_l2cap_chan_send(chan, buf);
}

_IDF_ONLY
int bt_l2cap_server_register(struct bt_l2cap_server *server)
{
    LOG_DBG("L2capSrvReg");

    if (server == NULL) {
        LOG_ERR("SrvNull");
        return -EINVAL;
    }

    if (server->accept == NULL) {
        LOG_ERR("SrvAcceptNull");
        return -EINVAL;
    }

    if (server->sec_level > BT_SECURITY_L4) {
        LOG_ERR("InvSecLevel[%u]", server->sec_level);
        return -EINVAL;
    }

    if (server->psm) {
        if (server->psm < L2CAP_LE_PSM_FIXED_START ||
                server->psm > L2CAP_LE_PSM_DYN_END) {
            LOG_ERR("InvPsm[%04x]", server->psm);
            return -EINVAL;
        }

        /* Check if given PSM is already in use */
        if (l2cap_server_lookup_psm(server->psm)) {
            LOG_WRN("PsmReg");
            return -EADDRINUSE;
        }

        LOG_DBG("SrvPsm[%04x]", server->psm);
    } else {
        uint16_t psm;

        for (psm = L2CAP_LE_PSM_DYN_START;
                psm <= L2CAP_LE_PSM_DYN_END; psm++) {
            if (l2cap_server_lookup_psm(psm) == NULL) {
                break;
            }
        }

        if (psm > L2CAP_LE_PSM_DYN_END) {
            LOG_ERR("NoFreeDynPsm");
            return -EADDRNOTAVAIL;
        }

        LOG_DBG("PsmNew[%04x]", psm);

        server->psm = psm;
    }

    if (server->sec_level < BT_SECURITY_L1) {
        server->sec_level = BT_SECURITY_L1;
    }

    sys_slist_append(&l2cap_servers, &server->node);

    return 0;
}

int bt_le_l2cap_init(void)
{
    int err;

    LOG_DBG("L2capInit");

#if CONFIG_BT_OTS
    extern int bt_gatt_ots_conn_cb_register(void);
    err = bt_gatt_ots_conn_cb_register();
    if (err) {
        LOG_ERR("OtsConnCbRegFail[%d]", err);
        return err;
    }

    extern int bt_gatt_ots_instances_prepare_v2(void);
    err = bt_gatt_ots_instances_prepare_v2();
    if (err) {
        LOG_ERR("PrepOtsInstsFail[%d]", err);
        return err;
    }
#endif /* CONFIG_BT_OTS */

#if CONFIG_BT_OTS || CONFIG_BT_OTS_CLIENT
    extern int bt_gatt_ots_l2cap_init_v2(void);
    err = bt_gatt_ots_l2cap_init_v2();
    if (err) {
        LOG_ERR("OtsL2capInitFail[%d]", err);
        return err;
    }
#endif /* CONFIG_BT_OTS || CONFIG_BT_OTS_CLIENT */

    err = bt_le_nimble_l2cap_init();
    if (err) {
        return err;
    }

    return 0;
}

void bt_le_l2cap_deinit(void)
{
    LOG_DBG("L2capDeinit");

    /* TODO: L2CAP server deinit */
}

/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/l2cap.h>

#include <../host/conn_internal.h>

#include "osi/allocator.h"
#include "osi/thread.h"
#include "stack/l2c_api.h"
#include "stack/btm_api.h"
#include "stack/btu.h"
#include "l2c_int.h"

#include "common/host.h"
#include "common/audio_attr.h"

#include "ots/adapter/l2cap.h"

LOG_MODULE_REGISTER(ISO_B2CAP, CONFIG_BT_ISO_LOG_LEVEL);

/* BT_BLE_L2CAP_COC_MAX_CHAN sizes ble_rcb_pool[] (registered LE PSMs, not
 * channels); EATT holds one slot for PSM 0x0027. */
#if CONFIG_BT_BLE_EATT_ENABLE
_Static_assert(CONFIG_BT_BLE_L2CAP_COC_MAX_CHAN >= 2,
               "OTS and EATT each need an LE CoC PSM slot");
#endif /* CONFIG_BT_BLE_EATT_ENABLE */

/* L2CA callbacks identify a channel by lcid only; this table maps lcid back to
 * (conn_handle, psm). One slot per connection (OTS is single-PSM). */
struct ots_chan_slot {
    uint16_t conn_handle;
    uint16_t lcid;
    uint16_t psm;
    BD_ADDR  peer_addr;
    bool used;
    bool pending;           /* connect request sent, awaiting cfm */
    bool tx_pending;        /* SDU in flight, awaiting decongestion */
};

static BT_AUDIO_EXT_RAM_BSS_ATTR struct ots_chan_slot ots_chans[L2CAP_OTS_MAX_CHAN];

static BT_AUDIO_EXT_RAM_BSS_ATTR uint16_t ots_reg_psm;

/* ---- BTU executor types & state -------------------------------------------
 * All L2CA_* must run on BTU (single-threaded CCB). ISO entry points marshal
 * via l2cap_btu_post (fire-and-forget) or l2cap_btu_invoke (sync, INIT/DEINIT
 * only). ots_chans is BTU-only, so no host_lock. */
enum l2cap_btu_cmd_type {
    BTU_L2CAP_CMD_INIT,
    BTU_L2CAP_CMD_DEINIT,
    BTU_L2CAP_CMD_CONNECT,
    BTU_L2CAP_CMD_DISCONNECT,
    BTU_L2CAP_CMD_SEND,
    BTU_L2CAP_CMD_ACCEPT_RSP,
};

struct l2cap_btu_cmd {
    enum l2cap_btu_cmd_type type;
    union {
        struct {
            uint16_t conn_handle;
        } connect;
        struct {
            uint16_t conn_handle;
        } disconnect;
        struct {
            uint16_t conn_handle;
            BT_HDR *p_buf;
        } send;
        struct {
            BD_ADDR bd_addr;
            uint8_t id;
            uint16_t lcid;
            uint16_t result;
        } accept_rsp;
    };
    uint32_t gen;   /* bumped per cycle so a late completion can be rejected */
    bool sync;      /* true: give the sem on completion (INIT/DEINIT only) */
};

/* Completion handoff for the sync invoke. Single set of globals, so it only works
 * because INIT/DEINIT are the sole users and never overlap. */
static BT_AUDIO_CTRL_BSS_ATTR struct k_sem btu_cmd_done;
static BT_AUDIO_CTRL_BSS_ATTR bool btu_cmd_done_init;
static BT_AUDIO_CTRL_BSS_ATTR uint32_t btu_done_gen;
static BT_AUDIO_CTRL_BSS_ATTR int btu_done_ret;

static struct ots_chan_slot *ots_chan_find_used_by_lcid(uint16_t lcid)
{
    for (int i = 0; i < L2CAP_OTS_MAX_CHAN; i++) {
        if (ots_chans[i].used && ots_chans[i].lcid == lcid) {
            return &ots_chans[i];
        }
    }

    return NULL;
}

static struct ots_chan_slot *ots_chan_find_pending_by_addr(const BD_ADDR addr)
{
    for (int i = 0; i < L2CAP_OTS_MAX_CHAN; i++) {
        if (ots_chans[i].pending &&
                memcmp(ots_chans[i].peer_addr, addr, sizeof(BD_ADDR)) == 0) {
            return &ots_chans[i];
        }
    }

    return NULL;
}

/* psm is stamped here, not at cfm: a pending slot torn down by a link loss still
 * has to post DISCONNECTED, and the upper layer keys that on the wire PSM. */
static struct ots_chan_slot *ots_chan_alloc_pending(uint16_t conn_handle, const BD_ADDR addr)
{
    for (int i = 0; i < L2CAP_OTS_MAX_CHAN; i++) {
        if (!ots_chans[i].used && !ots_chans[i].pending) {
            memset(&ots_chans[i], 0, sizeof(ots_chans[i]));
            ots_chans[i].conn_handle = conn_handle;
            ots_chans[i].psm = L2CAP_LE_OTS_PSM;
            memcpy(ots_chans[i].peer_addr, addr, sizeof(BD_ADDR));
            ots_chans[i].pending = true;

            return &ots_chans[i];
        }
    }

    return NULL;
}

static struct ots_chan_slot *ots_chan_find_pending_by_lcid(uint16_t lcid)
{
    for (int i = 0; i < L2CAP_OTS_MAX_CHAN; i++) {
        if (ots_chans[i].pending && ots_chans[i].lcid == lcid) {
            return &ots_chans[i];
        }
    }

    return NULL;
}

static struct ots_chan_slot *ots_chan_alloc_used(uint16_t conn_handle, uint16_t lcid, uint16_t psm)
{
    for (int i = 0; i < L2CAP_OTS_MAX_CHAN; i++) {
        if (!ots_chans[i].used && !ots_chans[i].pending) {
            memset(&ots_chans[i], 0, sizeof(ots_chans[i]));
            ots_chans[i].conn_handle = conn_handle;
            ots_chans[i].lcid = lcid;
            ots_chans[i].psm = psm;
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

static void ots_cfg_init(tL2CAP_LE_CFG_INFO *cfg)
{
    /* mps/credits left at 0: the stack substitutes its CoC Kconfig defaults. */
    memset(cfg, 0, sizeof(*cfg));
    cfg->mtu = L2CAP_LE_OTS_MTU;
}

/* All callbacks below run on BTU. */

#if CONFIG_BT_OTS
static void l2cap_connect_ind_cb(BD_ADDR bd_addr, UINT16 lcid, UINT16 psm, UINT8 id)
{
    struct gatt_conn *gatt_conn;
    tL2CAP_LE_CFG_INFO cfg;
    int err = -ENOTCONN;

    LOG_DBG("[B]L2capCocConnectInd[%04x][%04x][%u]", lcid, psm, id);

    gatt_conn = bt_le_bluedroid_find_gatt_conn_with_addr(0, bd_addr, true);
    if (gatt_conn != NULL) {
        err = bt_le_l2cap_post_accept(gatt_conn->conn_handle, psm, lcid, id);
    }

    if (err) {
        /* Nothing will answer on iso_task, so reject here. */
        LOG_ERR("[B]L2capIndRefuse[%04x][%p][%d]", lcid, gatt_conn, err);

        ots_cfg_init(&cfg);

        L2CA_ConnectLECocRsp(bd_addr, id, lcid, L2CAP_LE_ERR_NO_RESOURCES, 0, &cfg);
    }
}
#endif /* CONFIG_BT_OTS */

/* Unwind the optimistic chan_add done by bt_l2cap_chan_connect()/l2cap_accept():
 * without a DISCONNECTED the upper layer waits for a result that never comes. */
static void l2cap_cfm_fail(uint16_t conn_handle, const BD_ADDR addr)
{
    ots_chan_free(ots_chan_find_pending_by_addr(addr));

    bt_le_l2cap_post_disconnected(conn_handle, L2CAP_LE_OTS_PSM);
}

/* Fires for both roles. remote_cid has no public L2CA getter, and peer_addr
 * is needed to match a pending slot, so the CCB is touched for those two only. */
static void l2cap_connect_cfm_cb(UINT16 lcid, UINT16 result)
{
    struct gatt_conn *gatt_conn;
    struct ots_chan_slot *slot;
    tL2C_CCB *p_ccb;
    tL2CAP_LE_CFG_INFO peer_cfg;
    uint16_t conn_handle_h, psm_h;

    LOG_DBG("[B]L2capCocConnectCfm[%04x][%u]", lcid, result);

    p_ccb = l2cu_find_ccb_by_cid(NULL, lcid);
    if (p_ccb == NULL || p_ccb->p_lcb == NULL) {
        LOG_ERR("[B]L2capCfmNoCcb[%04x][%p]", lcid, p_ccb);
        return;
    }

    gatt_conn = bt_le_bluedroid_find_gatt_conn_with_addr(0, p_ccb->p_lcb->remote_bd_addr, true);
    if (gatt_conn == NULL) {
        LOG_ERR("[B]L2capCfmNoConn[%04x]", lcid);

        /* No gatt_conn, so the pending slot is the only source of conn_handle. */
        slot = ots_chan_find_pending_by_addr(p_ccb->p_lcb->remote_bd_addr);
        if (slot != NULL) {
            l2cap_cfm_fail(slot->conn_handle, p_ccb->p_lcb->remote_bd_addr);
        }
        return;
    }

    if (result != L2CAP_CONN_OK) {
        LOG_ERR("[B]L2capCocConnectFail[%04x][%u]", lcid, result);
        l2cap_cfm_fail(gatt_conn->conn_handle, p_ccb->p_lcb->remote_bd_addr);
        return;
    }

    if (ots_chan_find_used_by_lcid(lcid) != NULL) {
        /* Duplicate cfm for a live channel: the first one already reported it. */
        LOG_ERR("[B]L2capCocChanExist[%04x]", lcid);
        return;
    }

    if (!L2CA_GetPeerLECocConfig(lcid, &peer_cfg)) {
        LOG_ERR("[B]L2capCfmNoPeerCfg[%04x]", lcid);
        l2cap_cfm_fail(gatt_conn->conn_handle, p_ccb->p_lcb->remote_bd_addr);
        return;
    }

    /* Outgoing connect: reuse the pending slot reserved at chan_connect.
     * Incoming accept: no pending slot, allocate a fresh one. */
    slot = ots_chan_find_pending_by_addr(p_ccb->p_lcb->remote_bd_addr);
    if (slot != NULL) {
        slot->pending = false;
        slot->lcid = lcid;
        slot->psm = L2CAP_LE_OTS_PSM;
        slot->used = true;
    } else {
        slot = ots_chan_alloc_used(gatt_conn->conn_handle, lcid, L2CAP_LE_OTS_PSM);
        if (slot == NULL) {
            LOG_ERR("[B]L2capCocNoSlot[%04x]", lcid);
            l2cap_cfm_fail(gatt_conn->conn_handle, p_ccb->p_lcb->remote_bd_addr);
            return;
        }
    }

    conn_handle_h = slot->conn_handle;
    psm_h = slot->psm;

    LOG_INF("[B]L2capCocConnect[%u][%04x][%04x][%04x][%u][%u]",
            conn_handle_h, psm_h,
            lcid, p_ccb->remote_cid,
            L2CAP_LE_OTS_MTU, peer_cfg.mtu);

    bt_le_l2cap_post_connected(conn_handle_h, psm_h,
                               p_ccb->remote_cid, peer_cfg.mtu,
                               lcid, L2CAP_LE_OTS_MTU);
}

/* Also fires for local disconnect, so DisconnectCfm needs no handler. */
static void l2cap_disconnect_ind_cb(UINT16 lcid, BOOLEAN local_init)
{
    struct ots_chan_slot *slot;
    uint16_t conn_handle_h, psm_h;

    LOG_DBG("[B]L2capCocDisconnectInd[%04x][%u]", lcid, local_init);

    slot = ots_chan_find_used_by_lcid(lcid);
    if (slot == NULL) {
        /* A link lost while the connect was still outstanding arrives here, not
         * as a failed cfm (see the LP_DISCONNECT_IND case of l2c_csm's
         * CST_W4_L2CAP_CONNECT_RSP). Without this the pending slot would leak and
         * block every later OTS connect on that conn_handle. */
        slot = ots_chan_find_pending_by_lcid(lcid);
        if (slot == NULL) {
            LOG_ERR("[B]L2capDisconnectInvCocChan[%04x]", lcid);
            return;
        }
    }

    conn_handle_h = slot->conn_handle;
    psm_h = slot->psm;
    ots_chan_free(slot);

    LOG_INF("[B]L2capCocDisconnect[%u][%04x]", conn_handle_h, psm_h);

    bt_le_l2cap_post_disconnected(conn_handle_h, psm_h);
}

static void l2cap_data_ind_cb(UINT16 lcid, BT_HDR *p_buf)
{
    struct ots_chan_slot *slot;
    uint16_t conn_handle_h, psm_h;

    if (p_buf == NULL) {
        LOG_ERR("[B]L2capRecvNullBuf[%04x]", lcid);
        return;
    }

    LOG_DBG("[B]L2capCocRecv[%04x][%u]", lcid, p_buf->len);

    slot = ots_chan_find_used_by_lcid(lcid);
    if (slot == NULL) {
        LOG_ERR("[B]L2capRecvOnInvCocChan[%04x]", lcid);
        osi_free(p_buf);
        return;
    }

    conn_handle_h = slot->conn_handle;
    psm_h = slot->psm;

    bt_le_l2cap_post_received(conn_handle_h, psm_h,
                              p_buf->data + p_buf->offset, p_buf->len);

    /* SDU ownership was handed to this callback; the stack osi_malloc'd it. */
    osi_free(p_buf);
}

/* Decongestion doubles as TX-done but can fire twice per SDU; tx_pending
 * collapses the pair into one ops->sent. */
static void l2cap_congestion_cb(UINT16 lcid, BOOLEAN congested)
{
    struct ots_chan_slot *slot;
    uint16_t conn_handle_h, psm_h;

    LOG_DBG("[B]L2capCocCongestion[%04x][%u]", lcid, congested);

    slot = ots_chan_find_used_by_lcid(lcid);
    if (slot == NULL) {
        LOG_ERR("[B]L2capCongestionInvCocChan[%04x]", lcid);
        return;
    }

    if (congested || !slot->tx_pending) {
        return;
    }

    slot->tx_pending = false;
    conn_handle_h = slot->conn_handle;
    psm_h = slot->psm;

    bt_le_l2cap_post_sent(conn_handle_h, psm_h);
}

/* ---- BTU executor functions ---------------------------------------------- */

static int l2cap_btu_do_accept_rsp(struct l2cap_btu_cmd *cmd)
{
    tL2CAP_LE_CFG_INFO cfg;

    ots_cfg_init(&cfg);

    if (!L2CA_ConnectLECocRsp(cmd->accept_rsp.bd_addr, cmd->accept_rsp.id,
                              cmd->accept_rsp.lcid, cmd->accept_rsp.result, 0, &cfg)) {
        LOG_ERR("[B]L2capAcceptFail[%04x]", cmd->accept_rsp.lcid);
        return -EIO;
    }

    return 0;
}

static void l2cap_btu_exec(void *ctx)
{
    struct l2cap_btu_cmd *cmd = ctx;
    tL2CAP_LE_CFG_INFO cfg;
    uint16_t lcid;
    int ret = -EINVAL;

    switch (cmd->type) {
    case BTU_L2CAP_CMD_INIT: {
        tL2CAP_APPL_INFO appl = {0};
#if CONFIG_BT_OTS
        appl.pL2CA_ConnectInd_Cb = l2cap_connect_ind_cb;
#endif /* CONFIG_BT_OTS */
        appl.pL2CA_ConnectCfm_Cb = l2cap_connect_cfm_cb;
        appl.pL2CA_DisconnectInd_Cb = l2cap_disconnect_ind_cb;
        appl.pL2CA_DataInd_Cb = l2cap_data_ind_cb;
        appl.pL2CA_CongestionStatus_Cb = l2cap_congestion_cb;

        ots_reg_psm = L2CA_RegisterLECoc(L2CAP_LE_OTS_PSM, &appl);
        if (ots_reg_psm == 0) {
            ret = -EIO;
            break;
        }

        BTM_SetSecurityLevel(TRUE, "BLE_L2CAP_OTS", BTM_SEC_SERVICE_GEN_NET,
                             BTM_SEC_NONE, ots_reg_psm, BTM_SEC_PROTO_L2CAP, 0);
        BTM_SetSecurityLevel(FALSE, "BLE_L2CAP_OTS", BTM_SEC_SERVICE_GEN_NET,
                             BTM_SEC_NONE, ots_reg_psm, BTM_SEC_PROTO_L2CAP, 0);
        ret = 0;
        break;
    }
    case BTU_L2CAP_CMD_DEINIT:
        /* Tear our channels down first: L2CA_DeregisterLECoc only inspects each
         * link's first CCB, so a CoC CCB sitting behind another one survives and
         * is left pointing at the released RCB (p_ccb->p_rcb->api use-after-free). */
        for (int i = 0; i < L2CAP_OTS_MAX_CHAN; i++) {
            /* pending too: its CCB is live from ConnectLECocReq onwards, and
               LECocDisconnect cancels a not-yet-open channel locally. */
            if (ots_chans[i].used || ots_chans[i].pending) {
                L2CA_LECocDisconnect(ots_chans[i].lcid);
            }
        }

        if (ots_reg_psm) {
            L2CA_DeregisterLECoc(ots_reg_psm);
            ots_reg_psm = 0;
        }
        ret = 0;
        break;
    case BTU_L2CAP_CMD_CONNECT: {
        struct gatt_conn *gatt_conn;
        struct ots_chan_slot *slot;

        gatt_conn = bt_le_bluedroid_find_gatt_conn_with_handle(cmd->connect.conn_handle);
        if (gatt_conn == NULL) {
            LOG_ERR("[B]L2capNoConnInfo[%u]", cmd->connect.conn_handle);
            ret = -ENOTCONN;
            break;
        }

        /* Reject if this connection already has an OTS channel. */
        for (int i = 0; i < L2CAP_OTS_MAX_CHAN; i++) {
            if ((ots_chans[i].used || ots_chans[i].pending) &&
                    ots_chans[i].conn_handle == cmd->connect.conn_handle) {
                LOG_WRN("[B]L2capOtsChanExist[%u]", cmd->connect.conn_handle);
                ret = -EALREADY;
                break;
            }
        }
        if (ret == -EALREADY) {
            break;
        }

        slot = ots_chan_alloc_pending(cmd->connect.conn_handle, gatt_conn->peer.val);
        if (slot == NULL) {
            LOG_ERR("[B]L2capOtsNoSlot[%u]", cmd->connect.conn_handle);
            ret = -ENOMEM;
            break;
        }

        ots_cfg_init(&cfg);

        /* Returns 0 on failure, LCID on success. Keep the LCID: if the link dies
         * before the cfm the stack reports DisconnectInd, and by then the CCB is
         * released so the LCID is the only way back to this slot. */
        lcid = L2CA_ConnectLECocReq(ots_reg_psm, gatt_conn->peer.val, &cfg);
        if (lcid == 0) {
            ret = -EIO;
            ots_chan_free(slot);
            /* Post DISCONNECTED to clean up the optimistic chan_add. */
            bt_le_l2cap_post_disconnected(cmd->connect.conn_handle, L2CAP_LE_OTS_PSM);
            break;
        }

        slot->lcid = lcid;
        ret = 0;
        break;
    }
    case BTU_L2CAP_CMD_DISCONNECT: {
        struct ots_chan_slot *slot = NULL;

        for (int i = 0; i < L2CAP_OTS_MAX_CHAN; i++) {
            if (ots_chans[i].used && ots_chans[i].conn_handle == cmd->disconnect.conn_handle) {
                slot = &ots_chans[i];
                break;
            }
        }
        if (slot == NULL) {
            LOG_WRN("[B]L2capNoOtsChan");
            ret = -ENOTCONN;
            break;
        }

        ret = L2CA_LECocDisconnect(slot->lcid) ? 0 : -EIO;
        break;
    }
    case BTU_L2CAP_CMD_SEND: {
        struct ots_chan_slot *slot = NULL;

        for (int i = 0; i < L2CAP_OTS_MAX_CHAN; i++) {
            if (ots_chans[i].used && ots_chans[i].conn_handle == cmd->send.conn_handle) {
                slot = &ots_chans[i];
                break;
            }
        }
        if (slot == NULL) {
            LOG_WRN("[B]L2capNoOtsChan");
            /* Slot gone (disconnect won the race); free p_buf ourselves. */
            osi_free(cmd->send.p_buf);
            ret = -ENOTCONN;
            break;
        }

        /* Armed before the write: a non-congested SDU completes inside it and
         * raises the decongestion cb synchronously (same BTU task). */
        slot->tx_pending = true;
        /* On DW_FAILED the stack frees p_buf; on SUCCESS/CONGESTED it owns it. */
        ret = (L2CA_LECocDataWrite(slot->lcid, cmd->send.p_buf) == L2CAP_DW_FAILED)
              ? -EIO : 0;
        if (ret != 0) {
            slot->tx_pending = false;
        }
        break;
    }
    case BTU_L2CAP_CMD_ACCEPT_RSP:
        ret = l2cap_btu_do_accept_rsp(cmd);
        break;
    default:
        ret = -EINVAL;
        break;
    }

    if (cmd->sync) {
        btu_done_ret = ret;
        btu_done_gen = cmd->gen;
        k_sem_give(&btu_cmd_done);
    }

    free(cmd);
}

/* Post cmd to BTU fire-and-forget. Returns 0 on success, -EIO if BTU is
 * unavailable (cmd is freed on failure). Never blocks. No self-delivery check is
 * needed - unlike the sync invoke, posting to our own queue cannot deadlock. */
static int l2cap_btu_post(struct l2cap_btu_cmd *cmd)
{
    osi_thread_t *btu = btu_get_current_thread();

    cmd->sync = false;

    if (btu == NULL || !osi_thread_post(btu, l2cap_btu_exec, cmd, 0, 0)) {
        LOG_ERR("[B]L2capBtuPostFail[%u]", cmd->type);
        free(cmd);
        return -EIO;
    }

    return 0;
}

void bt_le_bluedroid_l2cap_accept_result(uint16_t conn_handle, uint8_t l2cap_id,
                                         uint16_t chan_handle, uint16_t result)
{
    struct gatt_conn *gatt_conn;
    struct l2cap_btu_cmd *cmd;

    LOG_DBG("[B]L2capAcceptResult[%04x][%04x]", chan_handle, result);

    gatt_conn = bt_le_bluedroid_find_gatt_conn_with_handle(conn_handle);
    if (gatt_conn == NULL) {
        LOG_ERR("[B]L2capAcceptNoConn[%u]", conn_handle);
        return;
    }

    cmd = bt_le_ext_calloc(1, sizeof(*cmd));
    if (cmd == NULL) {
        LOG_ERR("[B]L2capAcceptNoCmd");
        return;
    }

    cmd->type = BTU_L2CAP_CMD_ACCEPT_RSP;
    cmd->accept_rsp.id = l2cap_id;
    cmd->accept_rsp.lcid = chan_handle;
    cmd->accept_rsp.result = result;
    memcpy(cmd->accept_rsp.bd_addr, gatt_conn->peer.val, sizeof(BD_ADDR));

    l2cap_btu_post(cmd);
}

int bt_le_bluedroid_l2cap_chan_connect(uint16_t conn_handle)
{
    struct l2cap_btu_cmd *cmd;

    cmd = bt_le_ext_calloc(1, sizeof(*cmd));
    if (cmd == NULL) {
        LOG_ERR("[B]L2capConnectNoCmd");
        return -ENOMEM;
    }

    cmd->type = BTU_L2CAP_CMD_CONNECT;
    cmd->connect.conn_handle = conn_handle;

    /* Result comes back as CONNECTED, or DISCONNECTED if BTU's connect fails. */
    return l2cap_btu_post(cmd);
}

int bt_le_bluedroid_l2cap_chan_disconnect(struct bt_l2cap_chan *chan)
{
    struct l2cap_btu_cmd *cmd;

    cmd = bt_le_ext_calloc(1, sizeof(*cmd));
    if (cmd == NULL) {
        LOG_ERR("[B]L2capDisconnectNoCmd");
        return -ENOMEM;
    }

    cmd->type = BTU_L2CAP_CMD_DISCONNECT;
    cmd->disconnect.conn_handle = chan->conn->handle;

    /* Result comes back as DISCONNECTED via l2cap_disconnect_ind_cb. */
    return l2cap_btu_post(cmd);
}

int bt_le_bluedroid_l2cap_chan_send(struct bt_l2cap_chan *chan, struct net_buf *buf)
{
    struct l2cap_btu_cmd *cmd;
    BT_HDR *p_buf;
    int ret;

    /* osi_malloc, not bt_le_int_malloc: the stack frees this SDU with osi_free,
     * and it never reaches DMA - the TX path copies it into a K-frame buffer. */
    p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR) + buf->len);
    if (p_buf == NULL) {
        LOG_ERR("[B]L2capNoBufForSend[%u]", buf->len);
        return -ENOMEM;
    }

    /* offset stays 0: the CoC TX path builds its own K-frames, no headroom here. */
    memset(p_buf, 0, sizeof(*p_buf));
    p_buf->len = buf->len;
    memcpy(p_buf->data, buf->data, buf->len);

    cmd = bt_le_ext_calloc(1, sizeof(*cmd));
    if (cmd == NULL) {
        LOG_ERR("[B]L2capSendNoCmd");
        osi_free(p_buf);
        return -ENOMEM;
    }

    cmd->type = BTU_L2CAP_CMD_SEND;
    cmd->send.conn_handle = chan->conn->handle;
    cmd->send.p_buf = p_buf;

    /* Completion comes back as ops->sent; on DW_FAILED nothing does, and the
     * eventual DISCONNECTED aborts the transfer. */
    ret = l2cap_btu_post(cmd);
    if (ret != 0) {
        /* BTU unavailable: p_buf not queued, free it. buf stays with caller. */
        osi_free(p_buf);
        return ret;
    }

    /* Posted: payload is in p_buf, release caller's buf else ot_chan_tx_pool
     * (1 entry) leaks. */
    net_buf_unref(buf);

    return 0;
}

/* Run cmd on BTU and wait — INIT/DEINIT only. Hot-path ops use
 * l2cap_btu_post (fire-and-forget). cmd is freed by l2cap_btu_exec. */
static int l2cap_btu_invoke(struct l2cap_btu_cmd *cmd)
{
    osi_thread_t *btu = btu_get_current_thread();
    uint32_t gen = ++btu_done_gen;  /* unique per cycle; stale gives are ignored */

    cmd->gen = gen;
    cmd->sync = true;

    /* Self-delivery would deadlock: run inline and drain the give. */
    if (btu != NULL && strcmp(osi_thread_name(btu), pcTaskGetName(NULL)) == 0) {
        l2cap_btu_exec(cmd);
        k_sem_take(&btu_cmd_done, 0);
        return btu_done_ret;
    }

    k_sem_reset(&btu_cmd_done);

    if (btu == NULL || !osi_thread_post(btu, l2cap_btu_exec, cmd, 0, 0)) {
        LOG_WRN("[B]L2capBtuUnavailable[%u]", cmd->type);
        l2cap_btu_exec(cmd);
        k_sem_take(&btu_cmd_done, 0);
        return btu_done_ret;
    }

    if (k_sem_take(&btu_cmd_done, K_SEM_SHORT) != 0) {
        LOG_ERR("[B]L2capBtuTimeout[%u]", cmd->type);
        /* cmd still on the BTU queue; bump gen so a late completion is rejected. */
        btu_done_gen++;
        return -ETIMEDOUT;
    }

    if (btu_done_gen != gen) {
        LOG_ERR("[B]L2capBtuStaleGen[%u][%u]", gen, btu_done_gen);
        return -ETIMEDOUT;
    }

    return btu_done_ret;
}

int bt_le_bluedroid_l2cap_init(void)
{
    struct l2cap_btu_cmd *cmd;
    int ret;

    LOG_DBG("[B]L2capInit");

    memset(ots_chans, 0, sizeof(ots_chans));

    if (!btu_cmd_done_init) {
        k_sem_create(&btu_cmd_done);
        btu_cmd_done_init = true;
    }

    /* Drop any stale give so the first invoke doesn't match an old completion. */
    k_sem_reset(&btu_cmd_done);

    cmd = bt_le_ext_calloc(1, sizeof(*cmd));
    if (cmd == NULL) {
        LOG_ERR("[B]L2capInitNoCmd");
        return -ENOMEM;
    }

    cmd->type = BTU_L2CAP_CMD_INIT;

    ret = l2cap_btu_invoke(cmd);
    if (ret == -ETIMEDOUT) {
        /* The command is still queued and will register the PSM once BTU drains
         * it, while we report failure. Queue a DEINIT behind it (the BTU queue
         * is FIFO) so the registration cannot outlive this failed init. */
        cmd = bt_le_ext_calloc(1, sizeof(*cmd));
        if (cmd == NULL) {
            LOG_ERR("[B]L2capInitNoUndoCmd");
            return ret;
        }

        cmd->type = BTU_L2CAP_CMD_DEINIT;
        l2cap_btu_post(cmd);
    }

    return ret;
}

void bt_le_bluedroid_l2cap_deinit(void)
{
    struct l2cap_btu_cmd *cmd;
    int ret;

    LOG_DBG("[B]L2capDeinit");

    /* If init never ran, nothing to tear down and the sem is unsafe to touch. */
    if (!btu_cmd_done_init || ots_reg_psm == 0) {
        memset(ots_chans, 0, sizeof(ots_chans));
        return;
    }

    cmd = bt_le_ext_calloc(1, sizeof(*cmd));
    if (cmd == NULL) {
        LOG_ERR("[B]L2capDeinitNoCmd");
        memset(ots_chans, 0, sizeof(ots_chans));
        return;
    }

    cmd->type = BTU_L2CAP_CMD_DEINIT;

    ret = l2cap_btu_invoke(cmd);
    if (ret == 0) {
        memset(ots_chans, 0, sizeof(ots_chans));
    } else {
        /* Timed out: DEINIT still queued. Leave ots_chans so the BTU handler's
         * disconnect loop runs before PSM deregister. */
        LOG_ERR("[B]L2capDeinitTimeout");
    }
}

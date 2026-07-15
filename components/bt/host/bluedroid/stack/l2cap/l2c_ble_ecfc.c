/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* LE L2CAP Enhanced Credit Based Flow Control (ECFC) - signaling 0x17/0x18/0x19/0x1A. */

#include <string.h>
#include "device/controller.h"
#include "stack/bt_types.h"
#include "stack/l2cdefs.h"
#include "l2c_int.h"
#include "stack/l2c_api.h"
#include "stack/btu.h"
#if (BLE_EATT_INCLUDED == TRUE)
#include "gatt_eatt_int.h"
#endif

#if (BLE_L2CAP_ENHANCED_COC_INCLUDED == TRUE)

#define L2C_BLE_ECFC_TRACE_API(fmt, ...)    L2CAP_TRACE_API("LE_ECFC: " fmt, ##__VA_ARGS__)
#define L2C_BLE_ECFC_TRACE_DEBUG(fmt, ...)  L2CAP_TRACE_DEBUG("LE_ECFC: " fmt, ##__VA_ARGS__)
#define L2C_BLE_ECFC_TRACE_WARN(fmt, ...)   L2CAP_TRACE_WARNING("LE_ECFC: " fmt, ##__VA_ARGS__)
#define L2C_BLE_ECFC_TRACE_ERROR(fmt, ...)  L2CAP_TRACE_ERROR("LE_ECFC: " fmt, ##__VA_ARGS__)

/* Core Spec v6.2 Vol 3 Part A 4.25/4.27: a single enhanced credit based
 * connection or reconfiguration request may target at most five channels. This
 * is independent of BLE_MAX_L2CAP_CLIENTS, which sizes the local channel
 * pool and can be configured up to 15. */
#define L2C_BLE_ECFC_MAX_REQ_CHANS  5

typedef struct {
    BOOLEAN     in_use;
    BOOLEAN     pending_link;
    UINT8       sig_id;
    UINT8       num_chan;
    tL2C_LCB    *p_lcb;
    tL2C_CCB    *ccbs[BLE_MAX_L2CAP_CLIENTS];
} tL2C_BLE_ECFC_CL_TXN;

typedef struct {
    BOOLEAN     in_use;
    UINT8       rem_id;
    tL2C_LCB    *p_lcb;
    UINT8       num_total;
    UINT8       num_done;
    BOOLEAN     rsp_recorded;   /* canonical rsp_* captured from first accepted chan */
    UINT16      reject_result;  /* app-supplied reject reason (security codes prioritized) */
    UINT16      rsp_mtu;
    UINT16      rsp_mps;
    UINT16      rsp_credits;
    tL2C_CCB    *ccbs[BLE_MAX_L2CAP_CLIENTS];
    BOOLEAN     accepted[BLE_MAX_L2CAP_CLIENTS];
} tL2C_BLE_ECFC_SRV_TXN;

typedef struct {
    BOOLEAN     in_use;
    UINT8       sig_id;
    UINT8       num_chan;
    UINT16      new_mtu;
    UINT16      new_mps;
    tL2C_LCB    *p_lcb;
    tL2C_CCB    *ccbs[BLE_MAX_L2CAP_CLIENTS];
} tL2C_BLE_ECFC_RECONFIG_TXN;

#if (BLE_L2CAP_COC_CLIENT_INCLUDED == TRUE)
static tL2C_BLE_ECFC_CL_TXN s_ecfc_cl_txn[MAX_L2CAP_LINKS];
#endif
#if (BLE_L2CAP_COC_SERVER_INCLUDED == TRUE)
static tL2C_BLE_ECFC_SRV_TXN s_ecfc_srv_txn[MAX_L2CAP_LINKS];
#endif
static tL2C_BLE_ECFC_RECONFIG_TXN s_ecfc_reconfig_txn[MAX_L2CAP_LINKS];

static void l2c_ble_ecfc_apply_default_cfg(tL2CAP_LE_CFG_INFO *cfg);
static void l2c_ble_ecfc_open_ccb(tL2C_CCB *p_ccb, UINT16 result);
#if (BLE_L2CAP_COC_CLIENT_INCLUDED == TRUE)
static tL2C_BLE_ECFC_CL_TXN *l2c_ble_ecfc_find_cl_txn(tL2C_LCB *p_lcb, UINT8 sig_id);
static void l2c_ble_ecfc_cl_txn_free(tL2C_BLE_ECFC_CL_TXN *txn);
static BOOLEAN l2c_ble_ecfc_cl_txn_has_ccb(tL2C_BLE_ECFC_CL_TXN *txn);
static BOOLEAN l2c_ble_ecfc_cl_send_connect(tL2C_BLE_ECFC_CL_TXN *txn);
#endif
static tL2C_BLE_ECFC_RECONFIG_TXN *l2c_ble_ecfc_find_reconfig_txn(tL2C_LCB *p_lcb, UINT8 sig_id);
#if (BLE_L2CAP_COC_SERVER_INCLUDED == TRUE)
static void l2c_ble_ecfc_srv_txn_free(tL2C_BLE_ECFC_SRV_TXN *txn);
static void l2c_ble_ecfc_srv_txn_try_complete(tL2C_BLE_ECFC_SRV_TXN *txn);
static void l2c_ble_ecfc_sec_cback(BD_ADDR bd_addr, tBT_TRANSPORT transport,
                                   void *p_ref_data, tBTM_STATUS result);
static void l2c_ble_ecfc_fire_connect_ind(tL2C_BLE_ECFC_SRV_TXN *txn);
static BOOLEAN l2c_ble_ecfc_srv_txn_has_ccb(tL2C_BLE_ECFC_SRV_TXN *txn);
#endif
static BOOLEAN l2c_ble_ecfc_reconfig_txn_has_ccb(tL2C_BLE_ECFC_RECONFIG_TXN *txn);

static void l2c_ble_ecfc_apply_default_cfg(tL2CAP_LE_CFG_INFO *cfg)
{
    if (cfg->mtu == 0) {
        cfg->mtu = L2CAP_LE_DEFAULT_MTU;
    }
    if (cfg->mps == 0) {
        cfg->mps = L2CAP_LE_COC_MPS;
    }
    cfg->mps = L2CAP_LE_CLAMP_MPS(cfg->mps);
    if (cfg->mps < L2CAP_LE_ECFC_MIN_MPS) {
        cfg->mps = L2CAP_LE_ECFC_MIN_MPS;
    }
    if (cfg->mtu < L2CAP_LE_ECFC_MIN_MTU) {
        cfg->mtu = L2CAP_LE_ECFC_MIN_MTU;
    }
    if (cfg->credits == 0) {
        cfg->credits = L2CAP_LE_INIT_CREDITS;
    }
}

#if (BLE_L2CAP_COC_CLIENT_INCLUDED == TRUE)
static tL2C_BLE_ECFC_CL_TXN *l2c_ble_ecfc_alloc_cl_txn(tL2C_LCB *p_lcb, UINT8 sig_id)
{
    for (int i = 0; i < MAX_L2CAP_LINKS; i++) {
        if (!s_ecfc_cl_txn[i].in_use) {
            memset(&s_ecfc_cl_txn[i], 0, sizeof(s_ecfc_cl_txn[i]));
            s_ecfc_cl_txn[i].in_use = TRUE;
            s_ecfc_cl_txn[i].p_lcb = p_lcb;
            s_ecfc_cl_txn[i].sig_id = sig_id;
            return &s_ecfc_cl_txn[i];
        }
    }
    L2C_BLE_ECFC_TRACE_WARN("alloc cl txn failed sig_id=%u", sig_id);
    return NULL;
}

static tL2C_BLE_ECFC_CL_TXN *l2c_ble_ecfc_find_cl_txn(tL2C_LCB *p_lcb, UINT8 sig_id)
{
    for (int i = 0; i < MAX_L2CAP_LINKS; i++) {
        if (s_ecfc_cl_txn[i].in_use && s_ecfc_cl_txn[i].p_lcb == p_lcb &&
            s_ecfc_cl_txn[i].sig_id == sig_id) {
            return &s_ecfc_cl_txn[i];
        }
    }
    return NULL;
}

static void l2c_ble_ecfc_cl_txn_free(tL2C_BLE_ECFC_CL_TXN *txn)
{
    if (txn) {
        memset(txn, 0, sizeof(*txn));
    }
}

static BOOLEAN l2c_ble_ecfc_cl_txn_has_ccb(tL2C_BLE_ECFC_CL_TXN *txn)
{
    for (int i = 0; i < txn->num_chan; i++) {
        if (txn->ccbs[i] != NULL) {
            return TRUE;
        }
    }
    return FALSE;
}

static BOOLEAN l2c_ble_ecfc_cl_send_connect(tL2C_BLE_ECFC_CL_TXN *txn)
{
    tL2C_LCB *p_lcb;
    UINT16 scids[BLE_MAX_L2CAP_CLIENTS];
    int i;

    if (txn == NULL || txn->num_chan == 0) {
        return FALSE;
    }

    p_lcb = txn->p_lcb;
    if (p_lcb == NULL || p_lcb->link_state != LST_CONNECTED) {
        return FALSE;
    }

    /* Validate every CCB before mutating any state: a NULL entry mid-array (a CCB
     * released via on_ccb_release while the txn waited for the link) must not
     * leave earlier CCBs stuck in CST_W4_L2CAP_CONNECT_RSP with no rollback. */
    for (i = 0; i < txn->num_chan; i++) {
        if (txn->ccbs[i] == NULL || txn->ccbs[i]->p_rcb == NULL) {
            return FALSE;
        }
    }
    for (i = 0; i < txn->num_chan; i++) {
        scids[i] = txn->ccbs[i]->local_cid;
        txn->ccbs[i]->local_id = txn->sig_id;
        txn->ccbs[i]->chnl_state = CST_W4_L2CAP_CONNECT_RSP;
    }

    /* If the request could not even be built/queued (e.g. buffer OOM), the peer
     * will never respond, so the CCBs would stay stuck in
     * CST_W4_L2CAP_CONNECT_RSP forever. Roll the state back and report failure so
     * the caller can clean up. */
    if (!l2cu_send_peer_ble_enhanced_credit_conn_req(p_lcb, txn->sig_id,
            txn->ccbs[0]->p_rcb->real_psm,
            txn->ccbs[0]->local_conn_cfg.mtu,
            txn->ccbs[0]->local_conn_cfg.mps,
            txn->ccbs[0]->local_conn_cfg.credits,
            txn->num_chan, scids)) {
        L2C_BLE_ECFC_TRACE_ERROR("0x17 send failed sig_id=%u", txn->sig_id);
        for (i = 0; i < txn->num_chan; i++) {
            if (txn->ccbs[i]) {
                txn->ccbs[i]->chnl_state = CST_CLOSED;
            }
        }
        return FALSE;
    }

    /* Guard against a peer that never sends the 0x18 response: one timer per
     * transaction (on ccbs[0]) aborts the whole batch on expiry. */
    l2c_ble_le_coc_start_rsp_timer(txn->ccbs[0], L2CAP_CHNL_CONNECT_TOUT);
    return TRUE;
}

void l2c_ble_ecfc_on_link_up(tL2C_LCB *p_lcb)
{
    if (p_lcb == NULL) {
        return;
    }

    for (int i = 0; i < MAX_L2CAP_LINKS; i++) {
        tL2C_BLE_ECFC_CL_TXN *txn = &s_ecfc_cl_txn[i];

        if (!txn->in_use || !txn->pending_link || txn->p_lcb != p_lcb) {
            continue;
        }

        txn->pending_link = FALSE;
        if (!l2c_ble_ecfc_cl_send_connect(txn)) {
            L2C_BLE_ECFC_TRACE_ERROR("deferred ConnectLEEcocReq send failed");
            /* Fail every channel so the app is notified and the CCBs are
             * released (open_ccb releases on non-OK), then free the txn to
             * avoid leaking it and leaving CCBs stuck in W4_CONNECT_RSP.
             * Snapshot then free before opening (see l2c_ble_ecfc_abort_cl_txn):
             * a non-OK open re-enters on_ccb_release and may free/realloc this
             * slot during the callback chain. */
            tL2C_CCB *ccbs[BLE_MAX_L2CAP_CLIENTS];
            UINT8 num_chan = txn->num_chan;
            if (num_chan > BLE_MAX_L2CAP_CLIENTS) {
                num_chan = BLE_MAX_L2CAP_CLIENTS;
            }
            memcpy(ccbs, txn->ccbs, num_chan * sizeof(tL2C_CCB *));
            l2c_ble_ecfc_cl_txn_free(txn);

            for (int j = 0; j < num_chan; j++) {
                if (ccbs[j]) {
                    l2c_ble_ecfc_open_ccb(ccbs[j], L2CAP_CONN_NO_PSM);
                }
            }
        }
    }
}
#endif /* BLE_L2CAP_COC_CLIENT_INCLUDED */

#if (BLE_L2CAP_COC_SERVER_INCLUDED == TRUE)
static tL2C_BLE_ECFC_SRV_TXN *l2c_ble_ecfc_alloc_srv_txn(tL2C_LCB *p_lcb, UINT8 rem_id)
{
    for (int i = 0; i < MAX_L2CAP_LINKS; i++) {
        if (!s_ecfc_srv_txn[i].in_use) {
            memset(&s_ecfc_srv_txn[i], 0, sizeof(s_ecfc_srv_txn[i]));
            s_ecfc_srv_txn[i].in_use = TRUE;
            s_ecfc_srv_txn[i].p_lcb = p_lcb;
            s_ecfc_srv_txn[i].rem_id = rem_id;
            return &s_ecfc_srv_txn[i];
        }
    }
    L2C_BLE_ECFC_TRACE_WARN("alloc srv txn failed rem_id=%u", rem_id);
    return NULL;
}

static tL2C_BLE_ECFC_SRV_TXN *l2c_ble_ecfc_find_srv_txn_by_ccb(tL2C_CCB *p_ccb)
{
    for (int i = 0; i < MAX_L2CAP_LINKS; i++) {
        if (!s_ecfc_srv_txn[i].in_use) {
            continue;
        }
        for (int j = 0; j < s_ecfc_srv_txn[i].num_total; j++) {
            if (s_ecfc_srv_txn[i].ccbs[j] == p_ccb) {
                return &s_ecfc_srv_txn[i];
            }
        }
    }
    return NULL;
}

static void l2c_ble_ecfc_srv_txn_free(tL2C_BLE_ECFC_SRV_TXN *txn)
{
    if (txn) {
#if (SMP_INCLUDED == TRUE)
        /* The server security check (l2ble_sec_access_req in
         * l2c_ble_ecfc_handle_conn_req) is queued with p_ref_data == txn, not a
         * CCB, so the CCB-based flush in l2c_ble_le_coc_cleanup_ccb never matches
         * it. If the txn is torn down (e.g. link loss) while that check is still
         * outstanding, drop the pending request here so the deferred SMP callback
         * cannot later fire l2c_ble_ecfc_sec_cback on a reused txn slot. Must run
         * before the memset since p_lcb is needed for the queue lookup. */
        l2ble_sec_flush_pending_req(txn->p_lcb, txn);
#endif
        memset(txn, 0, sizeof(*txn));
    }
}

static BOOLEAN l2c_ble_ecfc_srv_txn_has_ccb(tL2C_BLE_ECFC_SRV_TXN *txn)
{
    for (int i = 0; i < txn->num_total; i++) {
        if (txn->ccbs[i] != NULL) {
            return TRUE;
        }
    }
    return FALSE;
}
#endif /* BLE_L2CAP_COC_SERVER_INCLUDED */

static tL2C_BLE_ECFC_RECONFIG_TXN *l2c_ble_ecfc_alloc_reconfig_txn(tL2C_LCB *p_lcb, UINT8 sig_id)
{
    for (int i = 0; i < MAX_L2CAP_LINKS; i++) {
        if (!s_ecfc_reconfig_txn[i].in_use) {
            memset(&s_ecfc_reconfig_txn[i], 0, sizeof(s_ecfc_reconfig_txn[i]));
            s_ecfc_reconfig_txn[i].in_use = TRUE;
            s_ecfc_reconfig_txn[i].p_lcb = p_lcb;
            s_ecfc_reconfig_txn[i].sig_id = sig_id;
            return &s_ecfc_reconfig_txn[i];
        }
    }
    L2C_BLE_ECFC_TRACE_WARN("alloc reconfig txn failed sig_id=%u", sig_id);
    return NULL;
}

static tL2C_BLE_ECFC_RECONFIG_TXN *l2c_ble_ecfc_find_reconfig_txn(tL2C_LCB *p_lcb, UINT8 sig_id)
{
    for (int i = 0; i < MAX_L2CAP_LINKS; i++) {
        if (s_ecfc_reconfig_txn[i].in_use && s_ecfc_reconfig_txn[i].p_lcb == p_lcb &&
            s_ecfc_reconfig_txn[i].sig_id == sig_id) {
            return &s_ecfc_reconfig_txn[i];
        }
    }
    return NULL;
}

static BOOLEAN l2c_ble_ecfc_reconfig_txn_has_ccb(tL2C_BLE_ECFC_RECONFIG_TXN *txn)
{
    for (int i = 0; i < txn->num_chan; i++) {
        if (txn->ccbs[i] != NULL) {
            return TRUE;
        }
    }
    return FALSE;
}

void l2c_ble_ecfc_on_ccb_release(tL2C_CCB *p_ccb)
{
    int t, i;

    if (p_ccb == NULL || p_ccb->p_lcb == NULL ||
        p_ccb->p_lcb->transport != BT_TRANSPORT_LE) {
        return;
    }

#if (BLE_L2CAP_COC_CLIENT_INCLUDED == TRUE)
    for (t = 0; t < MAX_L2CAP_LINKS; t++) {
        tL2C_BLE_ECFC_CL_TXN *cl = &s_ecfc_cl_txn[t];

        if (!cl->in_use) {
            continue;
        }
        for (i = 0; i < cl->num_chan; i++) {
            if (cl->ccbs[i] == p_ccb) {
                cl->ccbs[i] = NULL;
            }
        }
        if (!l2c_ble_ecfc_cl_txn_has_ccb(cl)) {
            l2c_ble_ecfc_cl_txn_free(cl);
        }
    }
#endif

#if (BLE_L2CAP_COC_SERVER_INCLUDED == TRUE)
    for (t = 0; t < MAX_L2CAP_LINKS; t++) {
        tL2C_BLE_ECFC_SRV_TXN *srv = &s_ecfc_srv_txn[t];

        if (!srv->in_use) {
            continue;
        }
        for (i = 0; i < srv->num_total; i++) {
            if (srv->ccbs[i] == p_ccb) {
                srv->ccbs[i] = NULL;
                if (!srv->accepted[i]) {
                    srv->num_done++;
                }
            }
        }
        if (!srv->in_use) {
            continue;
        }
        if (srv->num_done >= srv->num_total) {
            l2c_ble_ecfc_srv_txn_try_complete(srv);
        } else if (!l2c_ble_ecfc_srv_txn_has_ccb(srv) && srv->num_done == 0) {
            /* Abandon a txn whose CCBs were torn down before any connect_rsp
             * (e.g. link loss). Do not free when num_done > 0: connect_rsp may
             * still be aggregating rejects and must send the 0x18 response. */
            l2c_ble_ecfc_srv_txn_free(srv);
        }
    }
#endif

    for (t = 0; t < MAX_L2CAP_LINKS; t++) {
        tL2C_BLE_ECFC_RECONFIG_TXN *rc = &s_ecfc_reconfig_txn[t];

        if (!rc->in_use) {
            continue;
        }
        for (i = 0; i < rc->num_chan; i++) {
            if (rc->ccbs[i] == p_ccb) {
                rc->ccbs[i] = NULL;
            }
        }
        if (!l2c_ble_ecfc_reconfig_txn_has_ccb(rc)) {
            memset(rc, 0, sizeof(*rc));
        }
    }
}

static void l2c_ble_ecfc_open_ccb(tL2C_CCB *p_ccb, UINT16 result)
{
    l2c_ble_le_coc_open_channel(p_ccb, result);
}

#if (BLE_L2CAP_COC_SERVER_INCLUDED == TRUE)
static void l2c_ble_ecfc_srv_txn_try_complete(tL2C_BLE_ECFC_SRV_TXN *txn)
{
    UINT16 dcids[BLE_MAX_L2CAP_CLIENTS];
    UINT16 result = L2CAP_LE_RESULT_CONN_OK;
    UINT8 created = 0;
    int i;

    if (txn == NULL || !txn->in_use || txn->num_done < txn->num_total) {
        return;
    }

    /* Once num_done == num_total, every channel was either accepted (accepted[i]
     * set with the CCB opened) or already released (ccbs[i] set NULL at the point
     * num_done was incremented). So a not-accepted, still-attached CCB cannot
     * exist here: reporting the accepted ones and zeroing the rest is enough,
     * and we must not call l2cu_release_ccb() from this path (it would re-enter
     * l2c_ble_ecfc_on_ccb_release() -> try_complete() and send a duplicate res). */
    for (i = 0; i < txn->num_total; i++) {
        if (txn->accepted[i] && txn->ccbs[i] && txn->ccbs[i]->chnl_state == CST_OPEN) {
            dcids[i] = txn->ccbs[i]->local_cid;
            created++;
        } else {
            dcids[i] = 0;
        }
    }

    /* Application/resource level rejection (all DCIDs 0). Prefer the specific
     * app-supplied reason (e.g. a security code 0x0005-0x0008, mandated by Core
     * Spec v6.2 Vol 3 Part A 10.2) so the peer learns to encrypt/authenticate
     * instead of treating it as a transient resource shortage. Fall back to
     * "insufficient resources" (0x0004) when no specific reason was recorded. */
    if (created == 0) {
        result = (txn->reject_result != L2CAP_LE_RESULT_CONN_OK) ?
                 txn->reject_result : L2CAP_LE_RESULT_NO_RESOURCES;
    } else if (created < txn->num_total) {
        result = L2CAP_LE_RESULT_NO_RESOURCES;
    }

    l2cu_send_peer_ble_enhanced_credit_conn_res(txn->p_lcb, txn->rem_id,
            txn->rsp_mtu, txn->rsp_mps, txn->rsp_credits, result,
            txn->num_total, dcids);
    l2c_ble_ecfc_srv_txn_free(txn);
}

static void l2c_ble_ecfc_fire_connect_ind(tL2C_BLE_ECFC_SRV_TXN *txn)
{
    tL2CA_CONNECT_IND_CB *ind_cb;
    int i;

    for (i = 0; i < txn->num_total; i++) {
        tL2C_CCB *p_ccb = txn->ccbs[i];
        if (p_ccb == NULL || p_ccb->p_rcb == NULL) {
            continue;
        }
        /* Skip a channel already accepted by a connect_rsp re-entered from an
         * earlier iteration's ind_cb (batch accept sharing rem_id); firing ind_cb
         * again would deliver a duplicate ConnectInd for an already-open channel.
         * Mirrors the accepted[] duplicate guard in l2c_ble_ecfc_connect_rsp. */
        if (txn->accepted[i]) {
            continue;
        }
        ind_cb = p_ccb->p_rcb->api.pL2CA_ConnectInd_Cb;
        if (ind_cb) {
            L2C_BLE_ECFC_TRACE_API("ConnectInd lcid=0x%04x psm=0x%04x id=%u",
                                   p_ccb->local_cid, p_ccb->p_rcb->real_psm, txn->rem_id);
            if (txn->in_use) {
                (*ind_cb)(p_ccb->p_lcb->remote_bd_addr, p_ccb->local_cid,
                          p_ccb->p_rcb->real_psm, txn->rem_id);
            }
        } else {
            /* Increment num_done BEFORE open_ccb: open_ccb fires ConnectCfm,
             * whose handler may synchronously release this CCB and re-enter
             * l2c_ble_ecfc_on_ccb_release(). If num_done were still 0 there, the
             * abandon guard (!has_ccb && num_done == 0) would free the txn out
             * from under us and the 0x18 response would never be sent. */
            txn->accepted[i] = TRUE;
            txn->num_done++;
            l2c_ble_ecfc_open_ccb(p_ccb, L2CAP_CONN_OK);
        }
    }
    if (txn->in_use) {
        l2c_ble_ecfc_srv_txn_try_complete(txn);
    }
}

static void l2c_ble_ecfc_sec_cback(BD_ADDR bd_addr, tBT_TRANSPORT transport,
                                   void *p_ref_data, tBTM_STATUS result)
{
    tL2C_BLE_ECFC_SRV_TXN *txn = (tL2C_BLE_ECFC_SRV_TXN *)p_ref_data;
    UNUSED(transport);

    L2C_BLE_ECFC_TRACE_DEBUG("sec_cback bda=%02x:%02x:%02x:%02x:%02x:%02x result=%u rem_id=%u num_total=%u",
                             bd_addr[0], bd_addr[1], bd_addr[2], bd_addr[3], bd_addr[4], bd_addr[5],
                             result, txn ? txn->rem_id : 0, txn ? txn->num_total : 0);

    if (txn == NULL || !txn->in_use) {
        L2C_BLE_ECFC_TRACE_WARN("sec_cback txn invalid");
        return;
    }

    if (result != BTM_SUCCESS) {
        L2C_BLE_ECFC_TRACE_WARN("sec_cback security failed result=%u rem_id=%u", result, txn->rem_id);
        l2cu_reject_ble_enhanced_connection(txn->p_lcb, txn->rem_id,
                                          l2c_ble_coc_sec_status_to_result(bd_addr, result),
                                          txn->num_total);
        for (int i = 0; i < txn->num_total; i++) {
            if (txn->ccbs[i]) {
                tL2C_CCB *p_rel = txn->ccbs[i];
                txn->ccbs[i] = NULL;
                l2cu_release_ccb(p_rel);
            }
        }
        /* Releasing the last CCB above can reach l2c_ble_ecfc_on_ccb_release,
         * whose abandon guard (all ccbs[] NULL && num_done == 0) may already have
         * freed this txn. Only free again if it is still in use, mirroring the
         * guard in l2c_ble_ecfc_fire_connect_ind, to avoid a double free. */
        if (txn->in_use) {
            l2c_ble_ecfc_srv_txn_free(txn);
        }
        return;
    }

    L2C_BLE_ECFC_TRACE_DEBUG("sec_cback security ok rem_id=%u, fire connect ind", txn->rem_id);
    l2c_ble_ecfc_fire_connect_ind(txn);
}
#endif /* BLE_L2CAP_COC_SERVER_INCLUDED */

#if (BLE_L2CAP_COC_SERVER_INCLUDED == TRUE)
void l2c_ble_ecfc_connect_rsp(tL2C_CCB *p_ccb, UINT16 result)
{
    tL2C_BLE_ECFC_SRV_TXN *txn;
    int idx;

    L2C_BLE_ECFC_TRACE_DEBUG("connect_rsp local_cid=0x%04x result=%u",
                           p_ccb ? p_ccb->local_cid : 0, result);

    if (p_ccb == NULL || !p_ccb->le_ecfc_channel) {
        L2C_BLE_ECFC_TRACE_WARN("connect_rsp not an ECFC channel");
        return;
    }

    txn = l2c_ble_ecfc_find_srv_txn_by_ccb(p_ccb);
    if (txn == NULL) {
        L2C_BLE_ECFC_TRACE_WARN("connect_rsp srv txn not found lcid=0x%04x", p_ccb->local_cid);
        return;
    }

    for (idx = 0; idx < txn->num_total; idx++) {
        if (txn->ccbs[idx] == p_ccb) {
            break;
        }
    }
    if (idx >= txn->num_total) {
        L2C_BLE_ECFC_TRACE_WARN("connect_rsp ccb not in txn lcid=0x%04x", p_ccb->local_cid);
        return;
    }

    /* Guard against a duplicate response for the same channel (e.g. the app
     * calls accept twice for one chan_handle). Re-processing would inflate
     * num_done, fire a second ConnectCfm and re-seed the RX window. */
    if (txn->accepted[idx]) {
        L2C_BLE_ECFC_TRACE_WARN("connect_rsp duplicate response lcid=0x%04x", p_ccb->local_cid);
        return;
    }

    if (result == L2CAP_CONN_OK) {
        l2c_ble_ecfc_apply_default_cfg(&p_ccb->local_conn_cfg);
        /* The 0x18 response carries a single MTU/MPS/credits set that the peer
         * applies uniformly to every accepted channel (Core Spec v6.2 Vol 3
         * Part A 4.26). Record the canonical values from the first accepted
         * channel and force every subsequent channel's local config to match, so
         * the RX window (le_coc_rx_avail) seeded in l2c_ble_le_coc_open_channel
         * stays consistent with what the peer is told. */
        if (!txn->rsp_recorded) {
            txn->rsp_mtu = p_ccb->local_conn_cfg.mtu;
            txn->rsp_mps = p_ccb->local_conn_cfg.mps;
            txn->rsp_credits = p_ccb->local_conn_cfg.credits;
            txn->rsp_recorded = TRUE;
        } else {
            p_ccb->local_conn_cfg.mtu = txn->rsp_mtu;
            p_ccb->local_conn_cfg.mps = txn->rsp_mps;
            p_ccb->local_conn_cfg.credits = txn->rsp_credits;
        }
        /* Increment num_done BEFORE open_ccb so a synchronous CCB release from
         * within the ConnectCfm callback (re-entering on_ccb_release) cannot hit
         * the abandon guard (!has_ccb && num_done == 0) and free the txn before
         * the 0x18 response is sent. */
        txn->accepted[idx] = TRUE;
        txn->num_done++;
        l2c_ble_ecfc_open_ccb(p_ccb, L2CAP_CONN_OK);
        /* open_ccb fires ConnectCfm synchronously; its handler may release this
         * CCB and re-enter on_ccb_release -> try_complete, which frees the txn.
         * Skip the second try_complete on a freed txn (mirrors the in_use guard
         * in l2c_ble_ecfc_fire_connect_ind and l2c_ble_ecfc_sec_cback). */
        if (txn->in_use) {
            l2c_ble_ecfc_srv_txn_try_complete(txn);
        }
        return;
    }

    /* Record the app-supplied reject reason. A security-related code
     * (0x0005-0x0008) takes precedence over a previously recorded generic code
     * so the aggregate response conveys the strongest security requirement. */
    if (result != L2CAP_CONN_OK) {
        BOOLEAN is_sec = (result >= L2CAP_LE_RESULT_INSUFFICIENT_AUTHENTICATION &&
                          result <= L2CAP_LE_RESULT_INSUFFICIENT_ENCRY);
        if (txn->reject_result == L2CAP_LE_RESULT_CONN_OK || is_sec) {
            txn->reject_result = result;
        }
    }

    /* Reject: aggregate the 0x18 response before releasing the CCB. Pre-nulling
     * ccbs[idx] and calling l2cu_release_ccb() first lets on_ccb_release() free the
     * txn while num_done is still short, so try_complete() never reaches the peer. */
    txn->num_done++;
    if (txn->num_done >= txn->num_total) {
        l2c_ble_ecfc_srv_txn_try_complete(txn);
        l2cu_release_ccb(p_ccb);
        return;
    }

    txn->ccbs[idx] = NULL;
    l2cu_release_ccb(p_ccb);
}
#endif /* BLE_L2CAP_COC_SERVER_INCLUDED */

#if (BLE_L2CAP_COC_CLIENT_INCLUDED == TRUE)
UINT8 L2CA_ConnectLEEcocReq(UINT16 psm, BD_ADDR p_bd_addr, tL2CAP_LE_CFG_INFO *p_cfg,
                            UINT8 num_chan, UINT16 *p_lcids)
{
    tL2C_RCB *p_rcb;
    tL2C_LCB *p_lcb;
    tL2C_CCB *p_ccb;
    tL2C_BLE_ECFC_CL_TXN *txn;
    UINT16 scids[BLE_MAX_L2CAP_CLIENTS];
    UINT8 sig_id;
    int i;

    if (num_chan == 0 || num_chan > BLE_MAX_L2CAP_CLIENTS ||
        num_chan > L2C_BLE_ECFC_MAX_REQ_CHANS) {
        L2C_BLE_ECFC_TRACE_WARN("ConnectLEEcocReq invalid num_chan=%u", num_chan);
        return 0;
    }

    L2C_BLE_ECFC_TRACE_API("ConnectLEEcocReq psm=0x%04x num=%u", psm, num_chan);

    if (!BTM_IsDeviceUp()) {
        L2C_BLE_ECFC_TRACE_WARN("ConnectLEEcocReq BTM not up psm=0x%04x", psm);
        return 0;
    }

    /* Bail out before allocating an LCB if BLE is unsupported: l2cu_create_conn()'s
     * !supports_ble() path returns FALSE without releasing the LCB, so relying on
     * it below would leak the freshly-allocated LCB (mirrors L2CA_ConnectLECocReq). */
    if (!controller_get_interface()->supports_ble()) {
        L2C_BLE_ECFC_TRACE_WARN("ConnectLEEcocReq BLE not supported");
        return 0;
    }

    p_rcb = l2cu_find_ble_rcb_by_psm(psm);
    if (p_rcb == NULL) {
        L2C_BLE_ECFC_TRACE_WARN("ConnectLEEcocReq no RCB for psm=0x%04x", psm);
        return 0;
    }

    p_lcb = l2cu_find_lcb_by_bd_addr(p_bd_addr, BT_TRANSPORT_LE);
    if (p_lcb == NULL) {
        p_lcb = l2cu_allocate_lcb(p_bd_addr, FALSE, BT_TRANSPORT_LE);
        if (p_lcb == NULL || !l2cu_create_conn(p_lcb, BT_TRANSPORT_LE)) {
            L2C_BLE_ECFC_TRACE_ERROR("ConnectLEEcocReq LCB alloc/create_conn failed");
            return 0;
        }
    }

    for (i = 0; i < num_chan; i++) {
        p_ccb = l2cu_allocate_ccb(p_lcb, 0);
        if (p_ccb == NULL) {
            L2C_BLE_ECFC_TRACE_ERROR("ConnectLEEcocReq CCB alloc failed at chan %d", i);
            while (--i >= 0) {
                p_ccb = l2cu_find_ccb_by_cid(p_lcb, scids[i]);
                if (p_ccb) {
                    l2cu_release_ccb(p_ccb);
                }
            }
            return 0;
        }
        p_ccb->p_rcb = p_rcb;
        p_ccb->le_coc_active = TRUE;
        p_ccb->le_ecfc_channel = TRUE;
        /* A pooled CCB reused from a released non-CoC channel keeps its stale
         * remote_cid; clear it so the DCID/SCID dedup checks in
         * l2c_ble_ecfc_handle_conn_res/handle_conn_req cannot false-match this
         * channel-in-setup before its real remote_cid is assigned (mirrors
         * L2CA_ConnectLECocReq). */
        p_ccb->remote_cid = 0;
        if (p_cfg) {
            memcpy(&p_ccb->local_conn_cfg, p_cfg, sizeof(tL2CAP_LE_CFG_INFO));
        }
        l2c_ble_ecfc_apply_default_cfg(&p_ccb->local_conn_cfg);
        scids[i] = p_ccb->local_cid;
        if (p_lcids) {
            p_lcids[i] = p_ccb->local_cid;
        }
    }

    p_lcb->id++;
    l2cu_adj_id(p_lcb, L2CAP_ADJ_ID);
    sig_id = p_lcb->id;

    txn = l2c_ble_ecfc_alloc_cl_txn(p_lcb, sig_id);
    if (txn == NULL) {
        L2C_BLE_ECFC_TRACE_WARN("ConnectLEEcocReq cl txn alloc failed sig_id=%u", sig_id);
        for (i = 0; i < num_chan; i++) {
            p_ccb = l2cu_find_ccb_by_cid(p_lcb, scids[i]);
            if (p_ccb) {
                l2cu_release_ccb(p_ccb);
            }
        }
        return 0;
    }

    txn->num_chan = num_chan;
    for (i = 0; i < num_chan; i++) {
        txn->ccbs[i] = l2cu_find_ccb_by_cid(p_lcb, scids[i]);
    }

    if (p_lcb->link_state != LST_CONNECTED) {
        if (p_lcb->link_state == LST_DISCONNECTING) {
            L2C_BLE_ECFC_TRACE_WARN("ConnectLEEcocReq link disconnecting, abort sig_id=%u", sig_id);
            l2c_ble_ecfc_cl_txn_free(txn);
            for (i = 0; i < num_chan; i++) {
                p_ccb = l2cu_find_ccb_by_cid(p_lcb, scids[i]);
                if (p_ccb) {
                    l2cu_release_ccb(p_ccb);
                }
            }
            return 0;
        }

        txn->pending_link = TRUE;
        L2C_BLE_ECFC_TRACE_DEBUG("ConnectLEEcocReq deferred until link up");
        return num_chan;
    }

    if (!l2c_ble_ecfc_cl_send_connect(txn)) {
        L2C_BLE_ECFC_TRACE_ERROR("ConnectLEEcocReq send_connect failed sig_id=%u", sig_id);
        l2c_ble_ecfc_cl_txn_free(txn);
        for (i = 0; i < num_chan; i++) {
            p_ccb = l2cu_find_ccb_by_cid(p_lcb, scids[i]);
            if (p_ccb) {
                l2cu_release_ccb(p_ccb);
            }
        }
        return 0;
    }

    return num_chan;
}
#endif /* BLE_L2CAP_COC_CLIENT_INCLUDED */

BOOLEAN L2CA_LEEcocReconfig(UINT16 lcids[], UINT8 num, UINT16 new_mtu, UINT16 new_mps)
{
    tL2C_CCB *p_ccb;
    tL2C_LCB *p_lcb = NULL;
    tL2C_BLE_ECFC_RECONFIG_TXN *txn;
    UINT16 dcids[BLE_MAX_L2CAP_CLIENTS];
    UINT8 sig_id;
    int i;

    L2C_BLE_ECFC_TRACE_DEBUG("LEEcocReconfig num=%u new_mtu=%u new_mps=%u", num, new_mtu, new_mps);

    if (lcids == NULL || num == 0 || num > BLE_MAX_L2CAP_CLIENTS ||
        num > L2C_BLE_ECFC_MAX_REQ_CHANS ||
        new_mtu < L2CAP_LE_ECFC_MIN_MTU || new_mps < L2CAP_LE_ECFC_MIN_MPS ||
        new_mps > L2CAP_LE_MAX_MPS) {
        L2C_BLE_ECFC_TRACE_WARN("LEEcocReconfig invalid params num=%u new_mtu=%u new_mps=%u",
                                num, new_mtu, new_mps);
        return FALSE;
    }

    for (i = 0; i < num; i++) {
        /* Reject a request that lists the same LCID more than once: a duplicate
         * resolves to the same CCB, so txn->ccbs[] would store it twice and the
         * 0x1A response would apply/notify the reconfigure on one channel twice.
         * Mirrors the seen_dcids[] dedup in l2c_ble_ecfc_handle_reconfig_req.
         * num <= 5, so the O(n^2) scan is cheap. */
        for (int j = 0; j < i; j++) {
            if (lcids[j] == lcids[i]) {
                L2C_BLE_ECFC_TRACE_WARN("LEEcocReconfig duplicate lcid=0x%04x", lcids[i]);
                return FALSE;
            }
        }
        p_ccb = l2cu_find_ccb_by_cid(NULL, lcids[i]);
        if (p_ccb == NULL || !p_ccb->le_coc_active || p_ccb->chnl_state != CST_OPEN) {
            L2C_BLE_ECFC_TRACE_WARN("LEEcocReconfig invalid ccb/not open lcid=0x%04x", lcids[i]);
            return FALSE;
        }
        /* Reconfiguration (0x19) is only defined for ECFC channels. Reject an
         * attempt to reconfigure a base LE CoC channel, which cannot carry the
         * enhanced reconfig request. */
        if (!p_ccb->le_ecfc_channel) {
            L2C_BLE_ECFC_TRACE_WARN("LEEcocReconfig not an ECFC channel lcid=0x%04x", lcids[i]);
            return FALSE;
        }
        if (p_lcb == NULL) {
            p_lcb = p_ccb->p_lcb;
        } else if (p_ccb->p_lcb != p_lcb) {
            L2C_BLE_ECFC_TRACE_WARN("LEEcocReconfig lcid=0x%04x on different link", lcids[i]);
            return FALSE;
        }
        if (p_ccb->local_conn_cfg.mtu > new_mtu) {
            L2C_BLE_ECFC_TRACE_WARN("LEEcocReconfig MTU reduction not allowed lcid=0x%04x cur=%u new=%u",
                                    lcids[i], p_ccb->local_conn_cfg.mtu, new_mtu);
            return FALSE;
        }
        /* Core Spec v6.2 Vol 3 Part A 4.27: when more than one channel is
         * reconfigured, the new MPS must be >= the current MPS of each channel.
         * Reject up front instead of sending a request the peer will refuse
         * (result 0x0002) while we wrongly report success. */
        if (num > 1 && p_ccb->local_conn_cfg.mps > new_mps) {
            L2C_BLE_ECFC_TRACE_WARN("LEEcocReconfig multi-chan MPS reduction not allowed lcid=0x%04x cur=%u new=%u",
                                    lcids[i], p_ccb->local_conn_cfg.mps, new_mps);
            return FALSE;
        }
        dcids[i] = p_ccb->local_cid;
    }

    p_lcb->id++;
    l2cu_adj_id(p_lcb, L2CAP_ADJ_ID);
    sig_id = p_lcb->id;

    txn = l2c_ble_ecfc_alloc_reconfig_txn(p_lcb, sig_id);
    if (txn == NULL) {
        L2C_BLE_ECFC_TRACE_WARN("LEEcocReconfig reconfig txn alloc failed sig_id=%u", sig_id);
        return FALSE;
    }

    txn->num_chan = num;
    txn->new_mtu = new_mtu;
    txn->new_mps = new_mps;
    for (i = 0; i < num; i++) {
        txn->ccbs[i] = l2cu_find_ccb_by_cid(p_lcb, lcids[i]);
    }

    /* If the request cannot be sent (e.g. buffer OOM) the peer never answers, so
     * release the txn slot instead of leaking it and reporting success. */
    if (!l2cu_send_peer_ble_credit_reconfig_req(p_lcb, sig_id, new_mtu, new_mps, num, dcids)) {
        L2C_BLE_ECFC_TRACE_ERROR("LEEcocReconfig 0x19 send failed sig_id=%u", sig_id);
        memset(txn, 0, sizeof(*txn));
        return FALSE;
    }

    /* Guard against a peer that never sends the 0x1A response: one timer per
     * transaction (on ccbs[0]) aborts the reconfigure on expiry. Channels stay
     * OPEN, so this timer is stopped explicitly on completion/abort. */
    if (txn->ccbs[0]) {
        l2c_ble_le_coc_start_rsp_timer(txn->ccbs[0], L2CAP_CHNL_CONNECT_TOUT);
    }
    L2C_BLE_ECFC_TRACE_DEBUG("LEEcocReconfig 0x19 sent sig_id=%u num=%u", sig_id, num);
    return TRUE;
}

#if (BLE_L2CAP_COC_SERVER_INCLUDED == TRUE)
void l2c_ble_ecfc_handle_conn_req(tL2C_LCB *p_lcb, UINT8 *p, UINT8 id, UINT16 cmd_len)
{
    tL2C_RCB *p_rcb;
    tL2C_CCB *p_ccb;
    tL2C_BLE_ECFC_SRV_TXN *txn;
    UINT16 spsm, mtu, mps, credits, scid;
    UINT16 n_scids;
    UINT8 num_scids;
    int i;

    if (cmd_len < L2CAP_CMD_BLE_ENHANCED_CONN_REQ_BASE_LEN + sizeof(UINT16)) {
        L2C_BLE_ECFC_TRACE_WARN("reject 0x17 short cmd len=%u id=%u", cmd_len, id);
        l2cu_reject_ble_enhanced_connection(p_lcb, id, L2CAP_LE_RESULT_INVALID_PARAMETERS, 1);
        return;
    }

    STREAM_TO_UINT16(spsm, p);
    STREAM_TO_UINT16(mtu, p);
    STREAM_TO_UINT16(mps, p);
    STREAM_TO_UINT16(credits, p);

    /* Compute the SCID count in a wide type first: (cmd_len - base)/2 can exceed
     * 255 for an oversized packet and would truncate if assigned to a UINT8
     * before the upper-bound check, letting a malformed request slip through. */
    n_scids = (UINT16)((cmd_len - L2CAP_CMD_BLE_ENHANCED_CONN_REQ_BASE_LEN) / sizeof(UINT16));
    /* Core Spec v6.2 Vol 3 Part A 4.25 caps a single request at 5 source CIDs.
     * BLE_MAX_L2CAP_CLIENTS may be configured up to 15, so enforce the 5
     * spec limit here too (matches handle_reconfig_req / L2CA_ConnectLEEcocReq). */
    /* The reject response must echo one DCID per requested SCID (Core Spec v6.2
     * Vol 3 Part A 4.26: 1:1 positional mapping), so pass the actual n_scids as
     * the count rather than clamping it to the local channel budget. Cap the
     * count at 255 only to fit the UINT8 API argument: n_scids = (cmd_len-8)/2
     * can exceed 255 for an oversized/malformed packet, and a bare (UINT8) cast
     * would truncate it (e.g. 256 -> 0, then promoted to 1). */
    if (n_scids == 0) {
        L2C_BLE_ECFC_TRACE_WARN("reject 0x17 n_scids=0 id=%u", id);
        l2cu_reject_ble_enhanced_connection(p_lcb, id, L2CAP_LE_RESULT_INVALID_PARAMETERS, 1);
        return;
    }
    UINT8 reject_scids = (n_scids > 255) ? 255 : (UINT8)n_scids;
    if (n_scids > L2C_BLE_ECFC_MAX_REQ_CHANS) {
        /* More SCIDs than the spec's per-request maximum of 5: malformed. */
        L2C_BLE_ECFC_TRACE_WARN("reject 0x17 n_scids=%u > max id=%u", n_scids, id);
        l2cu_reject_ble_enhanced_connection(p_lcb, id, L2CAP_LE_RESULT_INVALID_PARAMETERS,
                                            reject_scids);
        return;
    }
    if (n_scids > BLE_MAX_L2CAP_CLIENTS) {
        /* Spec-valid count but exceeds our local channel budget: resource limit. */
        L2C_BLE_ECFC_TRACE_WARN("reject 0x17 n_scids=%u over budget id=%u", n_scids, id);
        l2cu_reject_ble_enhanced_connection(p_lcb, id, L2CAP_LE_RESULT_NO_RESOURCES,
                                            reject_scids);
        return;
    }
    num_scids = (UINT8)n_scids;

    /* Defensive: reject an initial credit value of 0 for the enhanced
     * credit-based (0x17) request. NOTE: confirm against Core Spec v6.2 4.25
     * whether 0 is strictly illegal for 0x17; regardless, seeding a channel
     * with 0 TX credits leaves it unusable until the peer later grants credit,
     * so rejecting up front is the safer behaviour. */
    if (mtu < L2CAP_LE_ECFC_MIN_MTU || mps < L2CAP_LE_ECFC_MIN_MPS ||
        mps > L2CAP_LE_MAX_MPS || credits == 0) {
        L2C_BLE_ECFC_TRACE_WARN("reject 0x17 invalid mtu=%u mps=%u cred=%u id=%u", mtu, mps, credits, id);
        l2cu_reject_ble_enhanced_connection(p_lcb, id, L2CAP_LE_RESULT_INVALID_PARAMETERS, num_scids);
        return;
    }

    L2C_BLE_ECFC_TRACE_API("rx 0x17 spsm=0x%04x mtu=%u mps=%u cred=%u n=%u id=%u",
                           spsm, mtu, mps, credits, num_scids, id);

    p_rcb = l2cu_find_ble_rcb_by_psm(spsm);
    if (p_rcb == NULL) {
        L2C_BLE_ECFC_TRACE_WARN("reject 0x17 no PSM spsm=0x%04x id=%u", spsm, id);
        l2cu_reject_ble_enhanced_connection(p_lcb, id, L2CAP_LE_RESULT_NO_PSM, num_scids);
        return;
    }

    txn = l2c_ble_ecfc_alloc_srv_txn(p_lcb, id);
    if (txn == NULL) {
        L2C_BLE_ECFC_TRACE_ERROR("reject 0x17 srv txn alloc failed id=%u", id);
        l2cu_reject_ble_enhanced_connection(p_lcb, id, L2CAP_LE_RESULT_NO_RESOURCES, num_scids);
        return;
    }

    txn->num_total = num_scids;

    for (i = 0; i < num_scids; i++) {
        STREAM_TO_UINT16(scid, p);
        /* Each SCID must be a peer dynamic LE-U CID (0x0040-0x007F). Reject a
         * fixed/invalid CID (e.g. 0x0004 ATT) so we never target it with
         * K-frames (Core Spec v6.2 Vol 3 Part A 4.25 / result 0x0009). */
        if (scid < L2CAP_BASE_APPL_CID || scid > L2CAP_BLE_CONN_MAX_CID) {
            L2C_BLE_ECFC_TRACE_WARN("reject 0x17 invalid scid=0x%04x id=%u", scid, id);
            l2cu_reject_ble_enhanced_connection(p_lcb, id, L2CAP_LE_RESULT_INVALID_SOURCE_CID, num_scids);
            for (int j = 0; j < i; j++) {
                if (txn->ccbs[j]) {
                    tL2C_CCB *p_rel = txn->ccbs[j];
                    txn->ccbs[j] = NULL;
                    l2cu_release_ccb(p_rel);
                }
            }
            if (txn->in_use) {
                l2c_ble_ecfc_srv_txn_free(txn);
            }
            return;
        }
        if (l2cu_find_ccb_by_remote_cid(p_lcb, scid)) {
            L2C_BLE_ECFC_TRACE_WARN("reject 0x17 duplicate scid=0x%04x id=%u", scid, id);
            l2cu_reject_ble_enhanced_connection(p_lcb, id, L2CAP_LE_RESULT_SOURCE_CID_ALREADY_ALLOCATED, num_scids);
            for (int j = 0; j < i; j++) {
                if (txn->ccbs[j]) {
                    tL2C_CCB *p_rel = txn->ccbs[j];
                    txn->ccbs[j] = NULL;
                    l2cu_release_ccb(p_rel);
                }
            }
            if (txn->in_use) {
                l2c_ble_ecfc_srv_txn_free(txn);
            }
            return;
        }

        p_ccb = l2cu_allocate_ccb(p_lcb, 0);
        if (p_ccb == NULL) {
            L2C_BLE_ECFC_TRACE_ERROR("reject 0x17 CCB alloc failed id=%u chan=%d", id, i);
            l2cu_reject_ble_enhanced_connection(p_lcb, id, L2CAP_LE_RESULT_NO_RESOURCES, num_scids);
            for (int j = 0; j < i; j++) {
                if (txn->ccbs[j]) {
                    tL2C_CCB *p_rel = txn->ccbs[j];
                    txn->ccbs[j] = NULL;
                    l2cu_release_ccb(p_rel);
                }
            }
            if (txn->in_use) {
                l2c_ble_ecfc_srv_txn_free(txn);
            }
            return;
        }

        p_ccb->le_coc_active = TRUE;
        p_ccb->le_ecfc_channel = TRUE;
        p_ccb->remote_id = id;
        p_ccb->p_rcb = p_rcb;
        p_ccb->remote_cid = scid;
        p_ccb->peer_conn_cfg.mtu = mtu;
        p_ccb->peer_conn_cfg.mps = mps;
        p_ccb->peer_conn_cfg.credits = credits;
        l2c_ble_ecfc_apply_default_cfg(&p_ccb->local_conn_cfg);
        txn->ccbs[i] = p_ccb;
    }

    txn->rsp_credits = txn->ccbs[0]->local_conn_cfg.credits;
    txn->rsp_mtu = txn->ccbs[0]->local_conn_cfg.mtu;
    txn->rsp_mps = txn->ccbs[0]->local_conn_cfg.mps;

    /* Move every channel in the batch (not just ccbs[0]) into the security-wait
     * state so link/CSM lookups and cleanup treat them consistently while the
     * single security check for this connection request is in flight. */
    for (i = 0; i < num_scids; i++) {
        txn->ccbs[i]->chnl_state = CST_TERM_W4_SEC_COMP;
    }

    p_ccb = txn->ccbs[0];
    (void)p_ccb;
#if (SMP_INCLUDED == TRUE)
    l2ble_sec_access_req(p_lcb->remote_bd_addr, p_rcb->real_psm, FALSE,
                         l2c_ble_ecfc_sec_cback, txn);
#else
    /* SMP disabled: no security procedure to run, so complete the access check
     * immediately (l2ble_sec_access_req is only compiled with SMP). */
    l2c_ble_ecfc_sec_cback(p_lcb->remote_bd_addr, BT_TRANSPORT_LE, txn, BTM_SUCCESS);
#endif
}
#endif /* BLE_L2CAP_COC_SERVER_INCLUDED */

#if (BLE_L2CAP_COC_CLIENT_INCLUDED == TRUE)
void l2c_ble_ecfc_abort_cl_txn(tL2C_LCB *p_lcb, UINT8 sig_id, UINT16 result)
{
    tL2C_BLE_ECFC_CL_TXN *txn = l2c_ble_ecfc_find_cl_txn(p_lcb, sig_id);

    if (txn == NULL) {
        /* Common no-op: a CMD_REJECT that does not target a pending ECFC client
         * transaction (it may be for a base CoC or reconfigure request). */
        return;
    }

    L2C_BLE_ECFC_TRACE_WARN("abort cl txn sig_id=%u result=%u", sig_id, result);

    /* Snapshot the CCBs and free the txn slot BEFORE opening any CCB. A non-OK
     * open releases the CCB, which re-enters l2c_ble_ecfc_on_ccb_release() and may
     * free this txn once no CCBs remain. Working from a local copy means neither
     * the loop condition nor the trailing free can read or corrupt a slot that
     * was reallocated during the callback chain. */
    tL2C_CCB *ccbs[BLE_MAX_L2CAP_CLIENTS];
    UINT8 num_chan = txn->num_chan;
    if (num_chan > BLE_MAX_L2CAP_CLIENTS) {
        num_chan = BLE_MAX_L2CAP_CLIENTS;
    }
    memcpy(ccbs, txn->ccbs, num_chan * sizeof(tL2C_CCB *));
    l2c_ble_ecfc_cl_txn_free(txn);

    for (int i = 0; i < num_chan; i++) {
        if (ccbs[i]) {
            l2c_ble_ecfc_open_ccb(ccbs[i], result);
        }
    }
}

void l2c_ble_ecfc_handle_conn_res(tL2C_LCB *p_lcb, UINT8 *p, UINT8 id, UINT16 cmd_len)
{
    tL2C_BLE_ECFC_CL_TXN *txn;
    UINT16 mtu, mps, credits, result, dcid;
    UINT16 n_dcids;
    int i;

    txn = l2c_ble_ecfc_find_cl_txn(p_lcb, id);
    if (txn == NULL) {
        L2C_BLE_ECFC_TRACE_WARN("0x18 unknown id=%u", id);
        return;
    }

    /* Response received: cancel the connect-response timeout (armed on ccbs[0]). */
    if (txn->ccbs[0]) {
        l2c_ble_le_coc_stop_rsp_timer(txn->ccbs[0]);
    }

    /* Snapshot the CCBs and free the txn BEFORE opening any channel. A non-OK
     * open releases the CCB and re-enters l2c_ble_ecfc_on_ccb_release(), which
     * frees this txn once its last CCB is gone; a synchronous ConnectCfm callback
     * (e.g. EATT) could then reallocate the same slot. Working from a local copy
     * keeps the loop bound, the stream parsing and the field writes from touching
     * a reused txn, matching l2c_ble_ecfc_abort_cl_txn / l2c_ble_ecfc_on_link_up. */
    tL2C_CCB *ccbs[BLE_MAX_L2CAP_CLIENTS];
    UINT8 num_chan = txn->num_chan;
    if (num_chan > BLE_MAX_L2CAP_CLIENTS) {
        num_chan = BLE_MAX_L2CAP_CLIENTS;
    }
    memcpy(ccbs, txn->ccbs, num_chan * sizeof(tL2C_CCB *));

    if (cmd_len < L2CAP_CMD_BLE_ENHANCED_CONN_RES_BASE_LEN) {
        L2C_BLE_ECFC_TRACE_WARN("0x18 short cmd len=%u", cmd_len);
        l2c_ble_ecfc_cl_txn_free(txn);
        for (i = 0; i < num_chan; i++) {
            if (ccbs[i]) {
                l2c_ble_ecfc_open_ccb(ccbs[i], L2CAP_CONN_NO_PSM);
            }
        }
        return;
    }

    STREAM_TO_UINT16(mtu, p);
    STREAM_TO_UINT16(mps, p);
    STREAM_TO_UINT16(credits, p);
    STREAM_TO_UINT16(result, p);

    /* Compute the DCID count in a wide type first: (cmd_len - base)/2 can exceed
     * 255 for an oversized packet and would truncate if assigned to a UINT8
     * before the count check, letting a malformed response slip through. Mirrors
     * the request handler (l2c_ble_ecfc_handle_conn_req). */
    n_dcids = (UINT16)((cmd_len - L2CAP_CMD_BLE_ENHANCED_CONN_RES_BASE_LEN) / sizeof(UINT16));
    L2C_BLE_ECFC_TRACE_API("rx 0x18 id=%u mtu=%u mps=%u cred=%u result=%u n=%u",
                           id, mtu, mps, credits, result, n_dcids);

    if (n_dcids != num_chan) {
        L2C_BLE_ECFC_TRACE_WARN("0x18 dcid count mismatch n=%u txn=%u", n_dcids, num_chan);
        l2c_ble_ecfc_cl_txn_free(txn);
        for (i = 0; i < num_chan; i++) {
            if (ccbs[i]) {
                l2c_ble_ecfc_open_ccb(ccbs[i], L2CAP_CONN_NO_PSM);
            }
        }
        return;
    }

    /* Validate the peer's common MTU/MPS/credits once. An mps of 0 would later
     * be used as a divisor when fragmenting SDUs (divide-by-zero); mtu/credits of
     * 0 leave the channel unusable while being reported as opened (Core Spec v6.2
     * Vol 3 Part A 4.26). The check is applied per accepted channel (dcid != 0)
     * inside the loop below rather than up front, so that on an "all connections
     * refused" result — where MTU/MPS/Credits shall be ignored and every DCID is
     * zero — the loop still forwards the real reject reason to the app instead of
     * masking every channel with L2CAP_CONN_NO_PSM. */
    BOOLEAN params_valid = !(mtu < L2CAP_LE_ECFC_MIN_MTU || mps < L2CAP_LE_ECFC_MIN_MPS ||
                             mps > L2CAP_LE_MAX_MPS || credits == 0);

    /* Everything needed from txn has been read; free it now so the open_ccb calls
     * below cannot trigger a re-entrant free of a slot we still read. */
    l2c_ble_ecfc_cl_txn_free(txn);

    for (i = 0; i < num_chan; i++) {
        tL2C_CCB *p_ccb;

        /* Each DCID in the response maps 1:1 to the SCID at the same index in
         * the request (Core Spec 4.26). Consume the stream entry before checking
         * ccbs[i]: a NULL slot (CCB released while the txn was pending) must
         * still advance p past its DCID. */
        STREAM_TO_UINT16(dcid, p);

        p_ccb = ccbs[i];
        if (p_ccb == NULL) {
            continue;
        }

        /* Per Core Spec v6.2 Vol 3 Part A 4.26, on a "some connections refused"
         * result each channel is accepted iff its DCID is non-zero; a zero DCID
         * marks that individual channel as refused. Do not tear down channels
         * with a valid DCID just because the aggregate result is not CONN_OK. */
        if (dcid == 0) {
            UINT16 fail_result = (result != L2CAP_LE_RESULT_CONN_OK) ? result : L2CAP_CONN_NO_PSM;
            l2c_ble_ecfc_open_ccb(p_ccb, fail_result);
            continue;
        }

        /* A non-zero DCID must be a peer dynamic LE-U CID (0x0040-0x007F) and
         * unique on this link; otherwise our outgoing K-frames would target an
         * invalid/duplicate peer CID (matches the 0x15 DCID check). Validate it
         * (and record remote_cid) BEFORE the params check so a channel the peer
         * accepted can be torn down on the air. An invalid/duplicate DCID cannot
         * be cleanly disconnected (no valid target, and a duplicate would drop
         * the wrong channel), so that case just drops our CCB. */
        if (dcid < L2CAP_BASE_APPL_CID || dcid > L2CAP_BLE_CONN_MAX_CID ||
            l2cu_find_ccb_by_remote_cid(p_lcb, dcid)) {
            L2C_BLE_ECFC_TRACE_WARN("0x18 invalid/duplicate dcid=0x%04x lcid=0x%04x",
                                    dcid, p_ccb->local_cid);
            l2c_ble_ecfc_open_ccb(p_ccb, L2CAP_CONN_NO_PSM);
            continue;
        }
        p_ccb->remote_cid = dcid;

        /* A channel the peer accepted (valid non-zero DCID) must carry usable
         * common parameters; if the shared MTU/MPS/credits are invalid the peer
         * still has an open channel, so tear it down on the air (best-effort
         * DISC_REQ) before dropping our CCB rather than leaving it orphaned. */
        if (!params_valid) {
            L2C_BLE_ECFC_TRACE_WARN("0x18 invalid peer params mtu=%u mps=%u cred=%u", mtu, mps, credits);
            l2cble_send_peer_disc_req(p_ccb);
            l2c_ble_ecfc_open_ccb(p_ccb, L2CAP_CONN_NO_PSM);
            continue;
        }

        p_ccb->peer_conn_cfg.mtu = mtu;
        p_ccb->peer_conn_cfg.mps = mps;
        p_ccb->peer_conn_cfg.credits = credits;
        l2c_ble_ecfc_open_ccb(p_ccb, L2CAP_CONN_OK);
    }
}
#endif /* BLE_L2CAP_COC_CLIENT_INCLUDED */

void l2c_ble_ecfc_handle_reconfig_req(tL2C_LCB *p_lcb, UINT8 *p, UINT8 id, UINT16 cmd_len)
{
    UINT8 *p_start = p;
    UINT16 mtu, mps, dcid;
    UINT16 n_dcids;
    UINT8 num_dcids;
    tL2C_CCB *p_ccb;
    UINT8 reduction_mps = 0;
    UINT16 seen_dcids[BLE_MAX_L2CAP_CLIENTS];
    int i;

    L2C_BLE_ECFC_TRACE_DEBUG("rx 0x19 id=%u cmd_len=%u", id, cmd_len);

    if (cmd_len < L2CAP_CMD_BLE_CREDIT_RECONFIG_REQ_BASE_LEN + sizeof(UINT16)) {
        L2C_BLE_ECFC_TRACE_WARN("reject 0x19 short cmd len=%u id=%u", cmd_len, id);
        l2cu_send_peer_ble_credit_reconfig_rsp(p_lcb, id, L2CAP_LE_RECONFIG_UNACCEPTED_PARAM);
        return;
    }

    STREAM_TO_UINT16(mtu, p);
    STREAM_TO_UINT16(mps, p);
    /* Wide-type count first to avoid UINT8 truncation on an oversized packet,
     * then enforce the spec upper bound of 5 DCIDs (Core Spec v6.2 Vol 3
     * Part A 4.27). */
    n_dcids = (UINT16)((cmd_len - L2CAP_CMD_BLE_CREDIT_RECONFIG_REQ_BASE_LEN) / sizeof(UINT16));

    if (mtu < L2CAP_LE_ECFC_MIN_MTU || mps < L2CAP_LE_ECFC_MIN_MPS ||
        mps > L2CAP_LE_MAX_MPS || n_dcids == 0 ||
        n_dcids > BLE_MAX_L2CAP_CLIENTS || n_dcids > L2C_BLE_ECFC_MAX_REQ_CHANS) {
        L2C_BLE_ECFC_TRACE_WARN("reject 0x19 invalid mtu=%u mps=%u n_dcids=%u id=%u",
                                mtu, mps, n_dcids, id);
        l2cu_send_peer_ble_credit_reconfig_rsp(p_lcb, id, L2CAP_LE_RECONFIG_UNACCEPTED_PARAM);
        return;
    }
    num_dcids = (UINT8)n_dcids;

    for (i = 0; i < num_dcids; i++) {
        STREAM_TO_UINT16(dcid, p);
        /* Reject a request that lists the same DCID more than once. Each DCID
         * "shall be non-zero and represent channel endpoints" (Core Spec v6.2
         * Vol 3 Part A 4.27); a duplicate is malformed and, if let through, would
         * update the same CCB and fire notify_reconfig twice. n <= 5, so an O(n^2)
         * scan of the DCIDs already parsed is cheap. */
        for (int j = 0; j < i; j++) {
            if (seen_dcids[j] == dcid) {
                L2C_BLE_ECFC_TRACE_WARN("reject 0x19 duplicate dcid=0x%04x id=%u", dcid, id);
                l2cu_send_peer_ble_credit_reconfig_rsp(p_lcb, id, L2CAP_LE_RECONFIG_INVALID_DCID);
                return;
            }
        }
        seen_dcids[i] = dcid;
        /* The Destination CID array in a reconfigure request holds the peer's
         * local CIDs (Core Spec v6.2 Vol 3 Part A 4.27), which map to our
         * remote_cid, not our local_cid. */
        p_ccb = l2cu_find_ccb_by_remote_cid(p_lcb, dcid);
        /* Reconfiguration (0x19) is only defined for an established ECFC channel
         * (Core Spec v6.2 Vol 3 Part A 4.27). Reject a request that targets a
         * base LE CoC channel (le_coc_active but !le_ecfc_channel) or a channel
         * not yet fully open, mirroring the local L2CA_LEEcocReconfig checks. */
        if (p_ccb == NULL || !p_ccb->le_coc_active || !p_ccb->le_ecfc_channel ||
            p_ccb->chnl_state != CST_OPEN) {
            L2C_BLE_ECFC_TRACE_WARN("reject 0x19 invalid dcid=0x%04x id=%u", dcid, id);
            l2cu_send_peer_ble_credit_reconfig_rsp(p_lcb, id, L2CAP_LE_RECONFIG_INVALID_DCID);
            return;
        }
        /* A reconfigure request changes the requester's (peer's) receive
         * MTU/MPS, i.e. our peer_conn_cfg. Per spec the new MTU must not be
         * smaller than the peer's previously agreed MTU. Compare against
         * peer_conn_cfg.mtu (matches the peer_conn_cfg.mps check below), not
         * our own local RX MTU. */
        if (p_ccb->peer_conn_cfg.mtu > mtu) {
            L2C_BLE_ECFC_TRACE_WARN("reject 0x19 MTU reduction not allowed dcid=0x%04x cur=%u new=%u id=%u",
                                    dcid, p_ccb->peer_conn_cfg.mtu, mtu, id);
            l2cu_send_peer_ble_credit_reconfig_rsp(p_lcb, id, L2CAP_LE_RECONFIG_REDUCTION_MTU_NOT_ALLOWED);
            return;
        }
        if (p_ccb->peer_conn_cfg.mps > mps) {
            reduction_mps++;
        }
    }

    if (reduction_mps > 0 && num_dcids > 1) {
        L2C_BLE_ECFC_TRACE_WARN("reject 0x19 multi-chan MPS reduction not allowed n=%u id=%u",
                                num_dcids, id);
        l2cu_send_peer_ble_credit_reconfig_rsp(p_lcb, id, L2CAP_LE_RECONFIG_REDUCTION_MPS_NOT_ALLOWED);
        return;
    }

    /* Core Spec v6.2 Vol 3 Part A 7.11 mandates the ordering: finish sending any
     * existing PDUs that need the old (larger) MPS, THEN send the
     * L2CAP_CREDIT_BASED_RECONFIGURE_RSP, and only afterwards adopt the new
     * MTU/MPS for subsequent SDUs. Send the response before updating the local
     * peer_conn_cfg / notifying the upper layer, otherwise a data write driven
     * synchronously from the notify callback would fragment SDUs with the new
     * (possibly smaller) MPS before the peer has seen the confirming response. */
    l2cu_send_peer_ble_credit_reconfig_rsp(p_lcb, id, L2CAP_LE_RECONFIG_OK);
    L2C_BLE_ECFC_TRACE_DEBUG("0x1A OK id=%u mtu=%u mps=%u num_dcids=%u", id, mtu, mps, num_dcids);

    p = p_start + sizeof(UINT16) + sizeof(UINT16);
    for (i = 0; i < num_dcids; i++) {
        STREAM_TO_UINT16(dcid, p);
        p_ccb = l2cu_find_ccb_by_remote_cid(p_lcb, dcid);
        if (p_ccb) {
            p_ccb->peer_conn_cfg.mtu = mtu;
            p_ccb->peer_conn_cfg.mps = mps;
            l2c_ble_le_coc_notify_reconfig(p_ccb, L2CAP_LE_RECONFIG_OK, TRUE);
#if (BLE_EATT_INCLUDED == TRUE)
            gatt_eatt_on_chan_mtu_changed(p_lcb->remote_bd_addr, p_ccb->local_cid);
#endif
        }
    }
}

void l2c_ble_ecfc_handle_reconfig_res(tL2C_LCB *p_lcb, UINT8 *p, UINT8 id, UINT16 cmd_len)
{
    tL2C_BLE_ECFC_RECONFIG_TXN *txn;
    UINT16 result;
    int i;

    tL2C_CCB *ccbs[BLE_MAX_L2CAP_CLIENTS];
    UINT8 num_chan;
    UINT16 new_mtu, new_mps;

    txn = l2c_ble_ecfc_find_reconfig_txn(p_lcb, id);
    if (txn == NULL) {
        L2C_BLE_ECFC_TRACE_WARN("0x1A unknown id=%u", id);
        return;
    }

    /* Response received: cancel the reconfigure-response timeout (armed on
     * ccbs[0]); channels remain OPEN so the timer is not freed by release. */
    if (txn->ccbs[0]) {
        l2c_ble_le_coc_stop_rsp_timer(txn->ccbs[0]);
    }

    /* Snapshot the CCBs and reconfigure params, then free the txn slot BEFORE
     * invoking any user callback. l2c_ble_le_coc_notify_reconfig() calls
     * pL2CA_LeReconfigInd_Cb, whose handler may synchronously disconnect a
     * channel; that re-enters l2c_ble_ecfc_on_ccb_release(), which memset-frees
     * this reconfig txn once its last CCB is gone, and a re-entrant reconfigure
     * could then reallocate the slot. Working from a local copy keeps the loop
     * and the trailing writes from reading/corrupting a reused slot (matches
     * l2c_ble_ecfc_abort_cl_txn / l2c_ble_ecfc_handle_conn_res).
     *
     * Residual (theoretical only): the snapshot could still hold a CCB that a
     * callback releases mid-loop, so a later apply/notify would touch freed
     * memory. This is the same edge already accepted for l2c_ble_ecfc_handle_conn_res
     * and is not reachable today: the only pL2CA_LeReconfigInd_Cb registrant is
     * btc_ble_l2cap_reconfig_ind(), which merely posts an async event (no
     * synchronous l2cu_release_ccb), and EATT registers no reconfig callback. */
    num_chan = txn->num_chan;
    if (num_chan > BLE_MAX_L2CAP_CLIENTS) {
        num_chan = BLE_MAX_L2CAP_CLIENTS;
    }
    memcpy(ccbs, txn->ccbs, num_chan * sizeof(tL2C_CCB *));
    new_mtu = txn->new_mtu;
    new_mps = txn->new_mps;

    /* A malformed/short response still terminates this pending reconfigure, so
     * the txn must be released here (it was already looked up) or it leaks. */
    if (cmd_len < L2CAP_CMD_BLE_CREDIT_RECONFIG_RSP_LEN) {
        L2C_BLE_ECFC_TRACE_WARN("0x1A short cmd len=%u id=%u, notify unaccepted", cmd_len, id);
        memset(txn, 0, sizeof(*txn));
        for (i = 0; i < num_chan; i++) {
            if (ccbs[i]) {
                l2c_ble_le_coc_notify_reconfig(ccbs[i], L2CAP_LE_RECONFIG_UNACCEPTED_PARAM, FALSE);
            }
        }
        return;
    }

    STREAM_TO_UINT16(result, p);

    memset(txn, 0, sizeof(*txn));

    if (result == L2CAP_LE_RECONFIG_OK) {
        L2C_BLE_ECFC_TRACE_DEBUG("0x1A reconfig ok id=%u num_chan=%u new_mtu=%u new_mps=%u",
                               id, num_chan, new_mtu, new_mps);
        for (i = 0; i < num_chan; i++) {
            if (ccbs[i]) {
                l2c_ble_le_coc_apply_reconfig(ccbs[i], new_mtu, new_mps);
                l2c_ble_le_coc_notify_reconfig(ccbs[i], L2CAP_LE_RECONFIG_OK, FALSE);
            }
        }
    } else {
        L2C_BLE_ECFC_TRACE_WARN("0x1A reconfig failed id=%u result=%u", id, result);
        for (i = 0; i < num_chan; i++) {
            if (ccbs[i]) {
                l2c_ble_le_coc_notify_reconfig(ccbs[i], result, FALSE);
            }
        }
    }
}

void l2c_ble_ecfc_abort_reconfig_txn(tL2C_LCB *p_lcb, UINT8 sig_id)
{
    tL2C_BLE_ECFC_RECONFIG_TXN *txn = l2c_ble_ecfc_find_reconfig_txn(p_lcb, sig_id);
    tL2C_CCB *ccbs[BLE_MAX_L2CAP_CLIENTS];
    UINT8 num_chan;
    int i;

    if (txn == NULL) {
        /* Common no-op: a CMD_REJECT that does not target a pending reconfigure. */
        return;
    }

    L2C_BLE_ECFC_TRACE_WARN("abort reconfig txn sig_id=%u", sig_id);

    /* Peer rejected the reconfigure request with L2CAP_CMD_REJECT instead of a
     * 0x1A response, so no reconfig response will ever arrive. Snapshot the
     * CCBs and free the txn slot BEFORE notifying: l2c_ble_le_coc_notify_reconfig
     * calls pL2CA_LeReconfigInd_Cb, whose handler may synchronously disconnect a
     * channel and re-enter l2c_ble_ecfc_on_ccb_release() (which memset-frees this
     * txn once its last CCB is gone), after which a re-entrant reconfigure could
     * reallocate the slot. Working from a local copy avoids corrupting a reused
     * slot (matches l2c_ble_ecfc_abort_cl_txn / l2c_ble_ecfc_handle_reconfig_res).
     *
     * Residual (theoretical only): the snapshot could still hold a CCB that a
     * notify callback releases mid-loop, so a later stop_timer/notify would touch
     * freed memory. Same edge already accepted for l2c_ble_ecfc_handle_conn_res,
     * and not reachable today: the only pL2CA_LeReconfigInd_Cb registrant
     * (btc_ble_l2cap_reconfig_ind) posts an async event without a synchronous
     * l2cu_release_ccb, and EATT registers no reconfig callback. */
    num_chan = txn->num_chan;
    if (num_chan > BLE_MAX_L2CAP_CLIENTS) {
        num_chan = BLE_MAX_L2CAP_CLIENTS;
    }
    memcpy(ccbs, txn->ccbs, num_chan * sizeof(tL2C_CCB *));
    memset(txn, 0, sizeof(*txn));

    /* The reconfigure-response timer is armed only on ccbs[0] (see
     * L2CA_LEEcocReconfig), so stop it there and nowhere else. Do NOT stop it
     * per-CCB: another channel in this txn may have independently armed a
     * disconnect RTX timer on the same timer_entry (l2c_ble_le_coc_initiate_disc);
     * cancelling that would strand the channel in CST_W4_L2CAP_DISCONNECT_RSP
     * with no timeout. Channels stay OPEN through a reconfigure, so release does
     * not run here. Mirrors l2c_ble_ecfc_handle_reconfig_res. */
    if (ccbs[0]) {
        l2c_ble_le_coc_stop_rsp_timer(ccbs[0]);
    }

    for (i = 0; i < num_chan; i++) {
        if (ccbs[i]) {
            l2c_ble_le_coc_notify_reconfig(ccbs[i], L2CAP_LE_RECONFIG_UNACCEPTED_PARAM, FALSE);
        }
    }
}

#if (BLE_L2CAP_COC_CLIENT_INCLUDED == TRUE)
BOOLEAN l2c_ble_ecfc_on_conn_timeout(tL2C_CCB *p_ccb)
{
    int i, j;

    for (i = 0; i < MAX_L2CAP_LINKS; i++) {
        tL2C_BLE_ECFC_CL_TXN *txn = &s_ecfc_cl_txn[i];
        if (!txn->in_use) {
            continue;
        }
        for (j = 0; j < txn->num_chan; j++) {
            if (txn->ccbs[j] == p_ccb) {
                L2C_BLE_ECFC_TRACE_WARN("0x18 timeout sig_id=%u", txn->sig_id);
                /* Fails every channel (open_ccb releases them, freeing timers)
                 * and releases the txn slot. Report L2CAP_CONN_TIMEOUT (not the
                 * hardcoded NO_PSM) so the app sees a retryable timeout, matching
                 * the base LE CoC timeout path (l2c_ble_le_coc_channel_timeout). */
                l2c_ble_ecfc_abort_cl_txn(txn->p_lcb, txn->sig_id, L2CAP_CONN_TIMEOUT);
                return TRUE;
            }
        }
    }
    return FALSE;
}
#endif /* BLE_L2CAP_COC_CLIENT_INCLUDED */

BOOLEAN l2c_ble_ecfc_on_reconfig_timeout(tL2C_CCB *p_ccb)
{
    int i, j;

    for (i = 0; i < MAX_L2CAP_LINKS; i++) {
        tL2C_BLE_ECFC_RECONFIG_TXN *txn = &s_ecfc_reconfig_txn[i];
        if (!txn->in_use) {
            continue;
        }
        for (j = 0; j < txn->num_chan; j++) {
            if (txn->ccbs[j] == p_ccb) {
                L2C_BLE_ECFC_TRACE_WARN("0x1A timeout sig_id=%u", txn->sig_id);
                /* Notifies each channel of the failed reconfigure and frees the
                 * txn (also stops the response timers). */
                l2c_ble_ecfc_abort_reconfig_txn(txn->p_lcb, txn->sig_id);
                return TRUE;
            }
        }
    }
    return FALSE;
}

#if (BLE_L2CAP_COC_CLIENT_INCLUDED != TRUE)
UINT8 L2CA_ConnectLEEcocReq(UINT16 psm, BD_ADDR p_bd_addr, tL2CAP_LE_CFG_INFO *p_cfg,
                            UINT8 num_chan, UINT16 *p_lcids)
{
    UNUSED(psm);
    UNUSED(p_bd_addr);
    UNUSED(p_cfg);
    UNUSED(num_chan);
    UNUSED(p_lcids);
    return 0;
}
#endif

#endif /* BLE_L2CAP_ENHANCED_COC_INCLUDED == TRUE */

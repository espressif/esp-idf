/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* LE L2CAP Connection-Oriented Channel (Credit Based Flow Control Mode). */
/* Independent from BR/EDR l2c_csm.c. */

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

#if (BLE_L2CAP_COC_INCLUDED == TRUE)

#define L2C_BLE_COC_DEFAULT_MTU         512
#define L2C_BLE_COC_DEFAULT_CREDITS     10
#define L2C_BLE_COC_SDU_LEN_SIZE          2
/* LE Credit Based Flow Control minimums (Core Spec Vol 3 Part A 4.22/4.23). */
#define L2C_BLE_COC_MIN_MTU              23
#define L2C_BLE_COC_MIN_MPS              23

/* RX credit window used for throughput. This is the number of K-frame credits we
 * keep granted to the peer. It must stay large enough to keep the sender's pipeline
 * fed (otherwise it ping-pongs at 1 credit), and is decoupled from the minimal
 * ceil(MTU/MPS) needed to reassemble a single SDU. Kept in sync with the ECFC
 * initial credits so the window is consistent before and after a reconfig. */
#define L2C_BLE_COC_RX_CREDIT_WINDOW    L2CAP_LE_INIT_CREDITS

#define L2C_BLE_COC_TRACE_DEBUG(fmt, ...)   L2CAP_TRACE_DEBUG("LE_COC: " fmt, ##__VA_ARGS__)
#define L2C_BLE_COC_TRACE_WARN(fmt, ...)    L2CAP_TRACE_WARNING("LE_COC: " fmt, ##__VA_ARGS__)
#define L2C_BLE_COC_TRACE_ERROR(fmt, ...)   L2CAP_TRACE_ERROR("LE_COC: " fmt, ##__VA_ARGS__)

static void l2c_ble_le_coc_sec_cback(BD_ADDR bd_addr, tBT_TRANSPORT transport,
                                      void *p_ref_data, tBTM_STATUS result);
static void l2c_ble_le_coc_notify_disconnect(tL2C_CCB *p_ccb, BOOLEAN local_init);
void l2c_ble_le_coc_open_channel(tL2C_CCB *p_ccb, UINT16 result);
static void l2c_ble_le_coc_try_xmit(tL2C_CCB *p_ccb);
static BOOLEAN l2c_ble_le_coc_send_frame(tL2C_CCB *p_ccb, const UINT8 *data, UINT16 len);
static void l2c_ble_le_coc_apply_default_cfg(tL2CAP_LE_CFG_INFO *cfg);
static UINT16 l2c_ble_le_coc_effective_mps(tL2C_CCB *p_ccb);
static UINT16 l2c_ble_le_coc_calc_rx_credits(UINT16 mtu, UINT16 mps);

#if (BLE_L2CAP_COC_SERVER_INCLUDED == TRUE)
/* Normalize an application/internal result code to a valid LE Credit Based
 * Connection Response result. L2CAP_CONN_{OK,NO_PSM,NO_RESOURCES} share values
 * with their L2CAP_LE_RESULT_* counterparts, and callers may also pass an
 * LE result code directly, so any valid LE result code is forwarded as-is.
 * Internal-only L2CAP_CONN_* codes with no LE encoding (e.g. NO_LINK, TIMEOUT)
 * fall back to UNACCEPTABLE_PARAMETERS. */
static UINT16 l2c_ble_le_coc_wire_result(UINT16 result)
{
    switch (result) {
    case L2CAP_LE_RESULT_CONN_OK:
    case L2CAP_LE_RESULT_NO_PSM:
    case L2CAP_LE_RESULT_NO_RESOURCES:
    case L2CAP_LE_RESULT_INSUFFICIENT_AUTHENTICATION:
    case L2CAP_LE_RESULT_INSUFFICIENT_AUTHORIZATION:
    case L2CAP_LE_RESULT_INSUFFICIENT_ENCRY_KEY_SIZE:
    case L2CAP_LE_RESULT_INSUFFICIENT_ENCRY:
    case L2CAP_LE_RESULT_INVALID_SOURCE_CID:
    case L2CAP_LE_RESULT_SOURCE_CID_ALREADY_ALLOCATED:
    case L2CAP_LE_RESULT_UNACCEPTABLE_PARAMETERS:
        return result;
    /* 0x0C (INVALID_PARAMETERS) is "Reserved for future use" for the LE Credit
     * Based Connection Response (code 0x15) per Core Spec v6.2 Vol 3 Part A
     * Table 4.16; fall through to the default so it maps to 0x0B, the last
     * valid result code for this packet type. */
    default:
        return L2CAP_LE_RESULT_UNACCEPTABLE_PARAMETERS;
    }
}
#endif /* BLE_L2CAP_COC_SERVER_INCLUDED */

BOOLEAN l2c_ble_le_coc_is_chan(tL2C_CCB *p_ccb)
{
    return (p_ccb != NULL) && p_ccb->le_coc_active && p_ccb->p_lcb != NULL &&
           p_ccb->p_lcb->transport == BT_TRANSPORT_LE;
}

static void l2c_ble_le_coc_apply_default_cfg(tL2CAP_LE_CFG_INFO *cfg)
{
    if (cfg->mtu == 0) {
        cfg->mtu = L2C_BLE_COC_DEFAULT_MTU;
    }
    /* Enforce the spec minimum MTU (Core Spec v6.2 Vol 3 Part A 4.22/4.23:
     * "shall support a minimum MTU size of 23 octets"). Mirrors the MPS clamp
     * below so a caller-supplied non-zero MTU < 23 is not sent on the wire. */
    if (cfg->mtu < L2C_BLE_COC_MIN_MTU) {
        cfg->mtu = L2C_BLE_COC_MIN_MTU;
    }
    if (cfg->mps == 0) {
        cfg->mps = L2CAP_LE_COC_MPS;
    }
    cfg->mps = L2CAP_LE_CLAMP_MPS(cfg->mps);
    if (cfg->credits == 0) {
        cfg->credits = L2C_BLE_COC_RX_CREDIT_WINDOW;
    }
}

static UINT16 l2c_ble_le_coc_effective_mps(tL2C_CCB *p_ccb)
{
    UINT16 mps = p_ccb->peer_conn_cfg.mps;
    UINT16 acl = controller_get_interface()->get_acl_data_size_ble();

    if (mps == 0) {
        mps = L2CAP_LE_COC_MPS;
    }
    if (acl > L2CAP_PKT_OVERHEAD && mps > (acl - L2CAP_PKT_OVERHEAD)) {
        mps = acl - L2CAP_PKT_OVERHEAD;
    }
    return mps;
}

static UINT16 l2c_ble_le_coc_calc_rx_credits(UINT16 mtu, UINT16 mps)
{
    UINT16 credits;
    UINT32 total;

    if (mps == 0) {
        mps = L2CAP_LE_COC_MPS;
    }
    /* The first K-frame of an SDU carries a 2-byte SDU Length header, so a full
     * SDU of `mtu` bytes spans ceil((mtu + 2) / mps) K-frames. Ignoring the
     * header under-counts by one credit whenever mtu is a multiple of mps. */
    total = (UINT32)mtu + L2C_BLE_COC_SDU_LEN_SIZE;
    credits = (UINT16)(total / mps);
    if (total % mps) {
        credits++;
    }
    return (credits > 0) ? credits : 1;
}

void l2c_ble_le_coc_apply_reconfig(tL2C_CCB *p_ccb, UINT16 new_mtu, UINT16 new_mps)
{
    UINT16 credits;

    if (p_ccb == NULL || !p_ccb->le_coc_active) {
        return;
    }

    p_ccb->local_conn_cfg.mtu = new_mtu;
    if (new_mps != 0) {
        p_ccb->local_conn_cfg.mps = L2CAP_LE_CLAMP_MPS(new_mps);
    }
    credits = l2c_ble_le_coc_calc_rx_credits(new_mtu, p_ccb->local_conn_cfg.mps);
    if (credits < L2C_BLE_COC_RX_CREDIT_WINDOW) {
        credits = L2C_BLE_COC_RX_CREDIT_WINDOW;
    }
    p_ccb->local_conn_cfg.credits = credits;

    /* Grant the peer the extra RX credits the enlarged window now allows,
     * otherwise the widened window never takes effect: the auto-credit path only
     * returns consumed credits and can never raise le_coc_rx_avail above the
     * previously outstanding count. */
    {
        /* Count every credit the peer will eventually hold again: what it still
         * holds (le_coc_rx_avail) plus credits already consumed but not yet
         * returned - batched in auto mode (le_coc_rx_credits_pending) or owed in
         * manual mode (le_coc_rx_manual_owed). Omitting manual_owed under-counts
         * the window, so the deficit sent here plus the same owed credits later
         * returned by recv_ready would double-grant and inflate the peer's TX
         * window beyond the configured window. Use a UINT32 sum and clamp to the
         * max credit window, matching give_credits / set_auto_credit. */
        UINT32 outstanding = (UINT32)p_ccb->le_coc_rx_avail +
                             p_ccb->le_coc_rx_credits_pending +
                             p_ccb->le_coc_rx_manual_owed;
        if (credits > outstanding && p_ccb->p_lcb != NULL) {
            UINT16 deficit = (UINT16)(credits - outstanding);
            if ((UINT32)p_ccb->le_coc_rx_avail + deficit > L2CAP_LE_MAX_CREDIT) {
                deficit = L2CAP_LE_MAX_CREDIT - p_ccb->le_coc_rx_avail;
            }
            if (deficit > 0) {
                p_ccb->le_coc_rx_avail += deficit;
                l2cble_send_flow_control_credit(p_ccb, deficit);
            }
        }
    }

    L2C_BLE_COC_TRACE_DEBUG("reconfig lcid=0x%04x mtu=%u mps=%u rx_cred=%u",
                            p_ccb->local_cid, new_mtu, p_ccb->local_conn_cfg.mps, credits);
#if (BLE_EATT_INCLUDED == TRUE)
    if (p_ccb->p_lcb != NULL) {
        gatt_eatt_on_chan_mtu_changed(p_ccb->p_lcb->remote_bd_addr, p_ccb->local_cid);
    }
#endif
}

#if (BLE_L2CAP_ENHANCED_COC_INCLUDED == TRUE)
void l2c_ble_le_coc_notify_reconfig(tL2C_CCB *p_ccb, UINT16 status, BOOLEAN peer_initiated)
{
    tL2CA_LE_RECONFIG_IND_CB *cb;

    if (p_ccb == NULL || p_ccb->p_rcb == NULL) {
        return;
    }

    cb = p_ccb->p_rcb->api.pL2CA_LeReconfigInd_Cb;
    if (cb) {
        (*cb)(p_ccb->local_cid, status, peer_initiated);
    }
}
#endif /* BLE_L2CAP_ENHANCED_COC_INCLUDED == TRUE */

BOOLEAN l2c_ble_le_coc_set_auto_credit(UINT16 lcid, BOOLEAN enable)
{
    tL2C_CCB *p_ccb;

    p_ccb = l2cu_find_ccb_by_cid(NULL, lcid);
    if (p_ccb == NULL || !p_ccb->le_coc_active) {
        return FALSE;
    }

    /* When leaving auto-credit mode, flush any RX credits batched by the
     * auto-credit path. Otherwise they are stranded (the auto path stops running
     * and give_credits only adds new credits), permanently shrinking the peer's
     * TX window. */
    if (!enable && !p_ccb->le_coc_no_auto_credit && p_ccb->le_coc_rx_credits_pending > 0 &&
        p_ccb->p_lcb != NULL) {
        UINT16 give = p_ccb->le_coc_rx_credits_pending;
        p_ccb->le_coc_rx_credits_pending = 0;
        p_ccb->le_coc_rx_avail += give;
        l2cble_send_flow_control_credit(p_ccb, give);
    }

    /* Symmetrically, when re-enabling auto-credit, flush any credits the manual
     * path consumed but has not returned yet (le_coc_rx_manual_owed). The auto
     * path only returns credits for frames it consumes from now on, so leftover
     * owed credits would otherwise be stranded and shrink the peer's TX window. */
    if (enable && p_ccb->le_coc_no_auto_credit && p_ccb->le_coc_rx_manual_owed > 0 &&
        p_ccb->p_lcb != NULL) {
        UINT16 give = p_ccb->le_coc_rx_manual_owed;
        if ((UINT32)p_ccb->le_coc_rx_avail + give > L2CAP_LE_MAX_CREDIT) {
            give = L2CAP_LE_MAX_CREDIT - p_ccb->le_coc_rx_avail;
        }
        if (give > 0) {
            p_ccb->le_coc_rx_manual_owed -= give;
            p_ccb->le_coc_rx_avail += give;
            l2cble_send_flow_control_credit(p_ccb, give);
        }
    }

    /* Manual mode returns RX credits only after a complete SDU is delivered (via
     * recv_ready). If a single SDU can span more K-frames than the whole RX
     * window, the peer runs out of TX credit mid-SDU and the SDU never completes
     * -> the app never calls recv_ready -> stall. Warn when entering manual mode
     * so the misconfiguration is visible; the data_ind deadlock breaker keeps it
     * working, but auto mode (or a larger MPS) is the proper fix. */
    if (!enable) {
        UINT16 need = l2c_ble_le_coc_calc_rx_credits(p_ccb->local_conn_cfg.mtu,
                                                     p_ccb->local_conn_cfg.mps);
        if (need > p_ccb->local_conn_cfg.credits) {
            L2C_BLE_COC_TRACE_WARN("manual mode: SDU up to %u K-frames > RX window %u lcid=0x%04x, prefer auto mode",
                                   need, p_ccb->local_conn_cfg.credits, lcid);
        }
    }

    p_ccb->le_coc_no_auto_credit = !enable;
    return TRUE;
}

/* Arm/disarm the per-CCB signalling response timer. Reuses p_ccb->timer_entry
 * (unused by LE CoC otherwise) with the classic per-channel BTU timer type; the
 * dispatcher in l2c_process_timeout() routes it to l2c_ble_le_coc_channel_timeout()
 * for LE CoC channels. l2cu_release_ccb() frees the timer, so no explicit stop is
 * needed on the teardown path. */
void l2c_ble_le_coc_start_rsp_timer(tL2C_CCB *p_ccb, UINT16 timeout_sec)
{
    if (p_ccb != NULL) {
        btu_start_timer(&p_ccb->timer_entry, BTU_TTYPE_L2CAP_CHNL, timeout_sec);
    }
}

void l2c_ble_le_coc_stop_rsp_timer(tL2C_CCB *p_ccb)
{
    if (p_ccb != NULL) {
        btu_stop_timer(&p_ccb->timer_entry);
    }
}

/* Send an LE Credit Based DISC_REQ, move the channel to
 * CST_W4_L2CAP_DISCONNECT_RSP and arm the RTX timer so an unresponsive peer
 * cannot leave the CCB (and its CID) leaked (Core Spec v6.2 Vol 3 Part A
 * 6.2.1). l2c_ble_le_coc_channel_timeout() releases the CCB on expiry. */
static void l2c_ble_le_coc_initiate_disc(tL2C_CCB *p_ccb)
{
    /* Already awaiting a DISC_RSP: a second DISC_REQ would bump p_lcb->id and
     * overwrite p_ccb->local_id (see l2cu_send_peer_ble_credit_based_disconn_req),
     * so the peer's DISC_RSP to the first request would be rejected on the id
     * mismatch in l2c_ble_le_coc_handle_disc_rsp and the channel would linger
     * (with the RTX timer restarted, up to 20s) instead of closing. Guard here so
     * every caller is safe, including l2c_ble_le_coc_handle_flow_ctrl_credit
     * which does not check chnl_state. Mirrors l2c_ble_le_coc_disconnect. */
    if (p_ccb->chnl_state == CST_W4_L2CAP_DISCONNECT_RSP) {
        L2C_BLE_COC_TRACE_DEBUG("DISC pending, skip dup DISC_REQ lcid=0x%04x", p_ccb->local_cid);
        return;
    }

    /* No NULL p_lcb guard is needed here even though the send path dereferences
     * p_ccb->p_lcb: p_lcb is only ever set to NULL in l2cu_release_ccb(), which
     * clears in_use in the same synchronous call immediately afterwards, and the
     * BT stack is single-threaded. l2cu_find_ccb_by_cid() (used by the callers)
     * only returns CCBs with in_use == TRUE, so a CCB reaching here always has a
     * live p_lcb; there is no window where p_lcb == NULL while in_use == TRUE. */
    L2C_BLE_COC_TRACE_DEBUG("send DISC_REQ lcid=0x%04x rcid=0x%04x", p_ccb->local_cid, p_ccb->remote_cid);
    l2cble_send_peer_disc_req(p_ccb);
    p_ccb->chnl_state = CST_W4_L2CAP_DISCONNECT_RSP;
    /* This is the disconnect RTX (waiting for DISC_RSP), not a connect timeout,
     * so use the shorter 10s disconnect timeout rather than the 60s connect one
     * (matches the classic BR/EDR disconnect path). */
    l2c_ble_le_coc_start_rsp_timer(p_ccb, L2CAP_CHNL_DISCONNECT_TOUT);
}

void l2c_ble_le_coc_channel_timeout(tL2C_CCB *p_ccb)
{
    if (p_ccb == NULL || !p_ccb->le_coc_active) {
        return;
    }

    L2C_BLE_COC_TRACE_WARN("rsp timeout lcid=0x%04x state=%d", p_ccb->local_cid, p_ccb->chnl_state);

#if (BLE_L2CAP_ENHANCED_COC_INCLUDED == TRUE)
    /* Reconfigure (0x19) whose 0x1A never arrived: channel stays OPEN. */
    if (p_ccb->chnl_state == CST_OPEN) {
        l2c_ble_ecfc_on_reconfig_timeout(p_ccb);
        return;
    }
#endif

    if (p_ccb->chnl_state == CST_W4_L2CAP_DISCONNECT_RSP) {
        /* Peer never answered our DISC_REQ (RTX timeout, Core Spec v6.2 Vol 3
         * Part A 6.2.1): notify the upper layer of a local disconnect and free
         * the CCB instead of leaking it and its CID. The DisconnectInd callback
         * runs synchronously and may re-enter L2CAP and release/reuse this CCB
         * slot; re-fetch by the saved lcid and only release if it is still the
         * same, in-use CoC CCB (mirrors l2c_ble_le_coc_open_channel). */
        UINT16 saved_lcid = p_ccb->local_cid;
        l2c_ble_le_coc_notify_disconnect(p_ccb, TRUE);
        tL2C_CCB *p_now = l2cu_find_ccb_by_cid(NULL, saved_lcid);
        if (p_now == p_ccb && p_ccb->in_use && p_ccb->le_coc_active) {
            l2cu_release_ccb(p_ccb);
        } else {
            L2C_BLE_COC_TRACE_DEBUG("disc timeout: lcid=0x%04x freed in cb, skip release", saved_lcid);
        }
        return;
    }

    if (p_ccb->chnl_state == CST_W4_L2CAP_CONNECT_RSP) {
#if (BLE_L2CAP_ENHANCED_COC_INCLUDED == TRUE)
        if (p_ccb->le_ecfc_channel) {
#if (BLE_L2CAP_COC_CLIENT_INCLUDED == TRUE)
            if (l2c_ble_ecfc_on_conn_timeout(p_ccb)) {
                return;
            }
#endif
            /* No owning txn found: release the orphaned CCB directly. */
            l2cu_release_ccb(p_ccb);
            return;
        }
#endif
        /* Basic LE CoC (0x14) with no 0x15: fail + release via open_channel. */
        l2c_ble_le_coc_open_channel(p_ccb, L2CAP_CONN_TIMEOUT);
    }
}

void l2c_ble_le_coc_cleanup_ccb(tL2C_CCB *p_ccb)
{
    if (p_ccb == NULL) {
        return;
    }

    L2C_BLE_COC_TRACE_DEBUG("cleanup lcid=0x%04x", p_ccb->local_cid);

#if (SMP_INCLUDED == TRUE)
    /* A CCB torn down while its LE security check is still outstanding would
     * leave a queued request pointing at this (soon reused) memory. Drop it so
     * the deferred SMP callback never lands on a stale/reused CCB. */
    l2ble_sec_flush_pending_req(p_ccb->p_lcb, p_ccb);
#endif

    if (p_ccb->le_coc_rx_sdu) {
        osi_free(p_ccb->le_coc_rx_sdu);
        p_ccb->le_coc_rx_sdu = NULL;
    }
    if (p_ccb->le_coc_tx_sdu) {
        osi_free(p_ccb->le_coc_tx_sdu);
        p_ccb->le_coc_tx_sdu = NULL;
    }

    p_ccb->le_coc_active = FALSE;
#if (BLE_L2CAP_ENHANCED_COC_INCLUDED == TRUE)
    p_ccb->le_ecfc_channel = FALSE;
#endif
    p_ccb->le_coc_no_auto_credit = FALSE;
    p_ccb->le_coc_rx_avail = 0;
    p_ccb->le_coc_rx_credits_pending = 0;
    p_ccb->le_coc_rx_manual_owed = 0;
    p_ccb->le_coc_rx_sdu_total = 0;
    p_ccb->le_coc_rx_sdu_rcvd = 0;
    p_ccb->le_coc_rx_have_len = FALSE;
    p_ccb->le_coc_tx_offset = 0;
    p_ccb->le_coc_tx_len_sent = FALSE;
    /* Reset the TX re-entrancy guard/flag too. l2cu_allocate_ccb does not memset
     * a CCB reused from the pool, so a stale le_coc_xmit_busy == TRUE (left by a
     * release that raced a try_xmit re-entrancy) would permanently deadlock TX on
     * the recycled channel. */
    p_ccb->le_coc_xmit_busy = FALSE;
    p_ccb->le_coc_xmit_rerun = FALSE;
    /* Clear the negotiated config so a recycled CCB never inherits the previous
     * channel's MTU/MPS/credits (apply_default_cfg only fills zero fields). */
    memset(&p_ccb->local_conn_cfg, 0, sizeof(p_ccb->local_conn_cfg));
    memset(&p_ccb->peer_conn_cfg, 0, sizeof(p_ccb->peer_conn_cfg));
    /* Clear the peer CID too. l2cu_allocate_ccb does not memset a CCB reused from
     * the pool, so a stale remote_cid would let the duplicate-DCID check in
     * l2c_ble_le_coc_handle_credit_conn_res (l2cu_find_ccb_by_remote_cid) match
     * this very CCB and wrongly reject a valid new outgoing connection. */
    p_ccb->remote_cid = 0;
}

static void l2c_ble_le_coc_notify_disconnect(tL2C_CCB *p_ccb, BOOLEAN local_init)
{
    tL2CA_DISCONNECT_IND_CB *cb;

    if (p_ccb == NULL || p_ccb->p_rcb == NULL) {
        return;
    }

    cb = p_ccb->p_rcb->api.pL2CA_DisconnectInd_Cb;
    if (cb) {
        L2C_BLE_COC_TRACE_DEBUG("DisconnectInd lcid=0x%04x local_init=%d",
                              p_ccb->local_cid, local_init);
        (*cb)(p_ccb->local_cid, local_init);
    }
}

void l2c_ble_le_coc_open_channel(tL2C_CCB *p_ccb, UINT16 result)
{
    tL2CA_CONNECT_CFM_CB *cb;

    if (p_ccb == NULL) {
        return;
    }

    if (result == L2CAP_CONN_OK) {
        p_ccb->chnl_state = CST_OPEN;
        p_ccb->le_coc_rx_avail = p_ccb->local_conn_cfg.credits;
        p_ccb->le_coc_rx_credits_pending = 0;
        p_ccb->le_coc_rx_manual_owed = 0;
        L2C_BLE_COC_TRACE_DEBUG("OPEN lcid=0x%04x rcid=0x%04x mtu=%u mps=%u tx_cred=%u rx_cred=%u",
                              p_ccb->local_cid, p_ccb->remote_cid,
                              p_ccb->peer_conn_cfg.mtu, p_ccb->peer_conn_cfg.mps,
                              p_ccb->peer_conn_cfg.credits, p_ccb->le_coc_rx_avail);
    } else {
        L2C_BLE_COC_TRACE_WARN("connect failed lcid=0x%04x result=%u", p_ccb->local_cid, result);
    }

    UINT16 saved_lcid = p_ccb->local_cid;

    if (p_ccb->p_rcb && p_ccb->p_rcb->api.pL2CA_ConnectCfm_Cb) {
        cb = p_ccb->p_rcb->api.pL2CA_ConnectCfm_Cb;
        (*cb)(p_ccb->local_cid, result);
    }

    if (result != L2CAP_CONN_OK) {
        /* The ConnectCfm callback runs synchronously and may re-enter L2CAP (e.g.
         * disconnect this channel and start a new connect), which could release
         * this CCB and reallocate the same pool slot for another channel. In that
         * case l2cu_release_ccb(p_ccb) would tear down the wrong (reused) CCB.
         * Re-fetch by the saved lcid and only release if it is still the same,
         * in-use, active CoC CCB. No registered ConnectCfm handler does this
         * synchronously today; this just closes the dangling-pointer window. */
        tL2C_CCB *p_now = l2cu_find_ccb_by_cid(NULL, saved_lcid);
        if (p_now == p_ccb && p_ccb->in_use && p_ccb->le_coc_active) {
            l2cu_release_ccb(p_ccb);
        }
    }
}

static void l2c_ble_le_coc_sec_cback(BD_ADDR bd_addr, tBT_TRANSPORT transport,
                                      void *p_ref_data, tBTM_STATUS result)
{
    tL2C_CCB *p_ccb = (tL2C_CCB *)p_ref_data;
    UNUSED(transport);

    if (p_ccb == NULL || !p_ccb->le_coc_active) {
        return;
    }

    L2C_BLE_COC_TRACE_DEBUG("sec_cback lcid=0x%04x status=%d state=%d",
                            p_ccb->local_cid, result, p_ccb->chnl_state);

    if (result != BTM_SUCCESS) {
#if (BLE_L2CAP_COC_SERVER_INCLUDED == TRUE)
        if (p_ccb->chnl_state == CST_TERM_W4_SEC_COMP && p_ccb->p_lcb != NULL) {
            l2cu_reject_ble_connection(p_ccb->p_lcb, p_ccb->remote_id,
                                        l2c_ble_coc_sec_status_to_result(bd_addr, result));
            l2cu_release_ccb(p_ccb);
            return;
        }
#endif
        l2c_ble_le_coc_open_channel(p_ccb, L2CAP_CONN_NO_LINK);
        return;
    }

    if (p_ccb->chnl_state == CST_ORIG_W4_SEC_COMP) {
#if (BLE_L2CAP_COC_CLIENT_INCLUDED == TRUE)
        l2cble_credit_based_conn_req(p_ccb);
        p_ccb->chnl_state = CST_W4_L2CAP_CONNECT_RSP;
        l2c_ble_le_coc_start_rsp_timer(p_ccb, L2CAP_CHNL_CONNECT_TOUT);
        L2C_BLE_COC_TRACE_DEBUG("sent 0x14 lcid=0x%04x", p_ccb->local_cid);
#else
        L2C_BLE_COC_TRACE_WARN("orig sec complete but client path disabled lcid=0x%04x", p_ccb->local_cid);
        l2c_ble_le_coc_open_channel(p_ccb, L2CAP_CONN_NO_LINK);
#endif
    } else if (p_ccb->chnl_state == CST_TERM_W4_SEC_COMP) {
#if (BLE_L2CAP_COC_SERVER_INCLUDED == TRUE)
        tL2CA_CONNECT_IND_CB *ind_cb;

        p_ccb->chnl_state = CST_W4_L2CA_CONNECT_RSP;
        if (p_ccb->p_rcb && p_ccb->p_rcb->api.pL2CA_ConnectInd_Cb) {
            ind_cb = p_ccb->p_rcb->api.pL2CA_ConnectInd_Cb;
            L2C_BLE_COC_TRACE_DEBUG("ConnectInd lcid=0x%04x psm=0x%04x id=%u",
                                  p_ccb->local_cid, p_ccb->p_rcb->real_psm, p_ccb->remote_id);
            /* Use the bd_addr parameter instead of dereferencing p_ccb->p_lcb:
             * it carries the same address (both callers pass p_lcb->remote_bd_addr,
             * and the BTM security callback delivers the peer address), and this
             * matches the defensive p_lcb-free failure path above. */
            (*ind_cb)(bd_addr, p_ccb->local_cid,
                      p_ccb->p_rcb->real_psm, p_ccb->remote_id);
        } else {
            L2C_BLE_COC_TRACE_WARN("no ConnectInd_Cb, auto-accept lcid=0x%04x", p_ccb->local_cid);
            l2c_ble_le_coc_connect_rsp(p_ccb, L2CAP_CONN_OK);
        }
#else
        L2C_BLE_COC_TRACE_WARN("term sec complete but server path disabled lcid=0x%04x", p_ccb->local_cid);
        l2c_ble_le_coc_open_channel(p_ccb, L2CAP_CONN_NO_LINK);
#endif
    }
}

#if (BLE_L2CAP_COC_CLIENT_INCLUDED == TRUE)
void l2c_ble_le_coc_connect_req(tL2C_CCB *p_ccb)
{
    if (p_ccb == NULL || p_ccb->p_lcb == NULL || p_ccb->p_rcb == NULL) {
        return;
    }

    l2c_ble_le_coc_apply_default_cfg(&p_ccb->local_conn_cfg);

    L2C_BLE_COC_TRACE_DEBUG("connect_req lcid=0x%04x psm=0x%04x mtu=%u mps=%u cred=%u",
                          p_ccb->local_cid, p_ccb->p_rcb->real_psm,
                          p_ccb->local_conn_cfg.mtu, p_ccb->local_conn_cfg.mps,
                          p_ccb->local_conn_cfg.credits);

    p_ccb->chnl_state = CST_ORIG_W4_SEC_COMP;
#if (SMP_INCLUDED == TRUE)
    l2ble_sec_access_req(p_ccb->p_lcb->remote_bd_addr, p_ccb->p_rcb->real_psm,
                         TRUE, l2c_ble_le_coc_sec_cback, p_ccb);
#else
    /* SMP disabled: there is no LE security procedure to run, so proceed as if
     * the access check passed (l2ble_sec_access_req is only compiled with SMP). */
    l2c_ble_le_coc_sec_cback(p_ccb->p_lcb->remote_bd_addr, BT_TRANSPORT_LE, p_ccb, BTM_SUCCESS);
#endif
}
#endif /* BLE_L2CAP_COC_CLIENT_INCLUDED */

#if (BLE_L2CAP_COC_SERVER_INCLUDED == TRUE)
void l2c_ble_le_coc_connect_rsp(tL2C_CCB *p_ccb, UINT16 result)
{
    if (p_ccb == NULL) {
        return;
    }

    result = l2c_ble_le_coc_wire_result(result);
    L2C_BLE_COC_TRACE_DEBUG("connect_rsp lcid=0x%04x result=%u", p_ccb->local_cid, result);

    if (result == L2CAP_LE_RESULT_CONN_OK) {
        l2c_ble_le_coc_apply_default_cfg(&p_ccb->local_conn_cfg);
        l2cble_credit_based_conn_res(p_ccb, L2CAP_LE_RESULT_CONN_OK);
        l2c_ble_le_coc_open_channel(p_ccb, L2CAP_CONN_OK);
    } else {
        /* Forward the specific reject reason to the peer instead of collapsing
         * every failure to UNACCEPTABLE_PARAMETERS. */
        l2cble_credit_based_conn_res(p_ccb, result);
        l2cu_release_ccb(p_ccb);
    }
}
#endif /* BLE_L2CAP_COC_SERVER_INCLUDED */

void l2c_ble_le_coc_on_link_up(tL2C_LCB *p_lcb)
{
    tL2C_CCB *p_ccb;

    if (p_lcb == NULL) {
        return;
    }

    for (p_ccb = p_lcb->ccb_queue.p_first_ccb; p_ccb; ) {
        tL2C_CCB *p_next = p_ccb->p_next_ccb;

        if (p_ccb->le_coc_active && p_ccb->chnl_state == CST_CLOSED) {
#if (BLE_L2CAP_ENHANCED_COC_INCLUDED == TRUE)
            if (!p_ccb->le_ecfc_channel)
#endif
            {
            L2C_BLE_COC_TRACE_DEBUG("link up, start pending lcid=0x%04x", p_ccb->local_cid);
#if (BLE_L2CAP_COC_CLIENT_INCLUDED == TRUE)
            l2c_ble_le_coc_connect_req(p_ccb);
#endif
            }
        }
        p_ccb = p_next;
    }

#if (BLE_L2CAP_ENHANCED_COC_INCLUDED == TRUE) && (BLE_L2CAP_COC_CLIENT_INCLUDED == TRUE)
    l2c_ble_ecfc_on_link_up(p_lcb);
#endif
}

#if (BLE_L2CAP_COC_SERVER_INCLUDED == TRUE)
void l2c_ble_le_coc_handle_credit_conn_req(tL2C_LCB *p_lcb, UINT8 *p, UINT8 id, UINT16 cmd_len)
{
    tL2C_CCB *p_ccb = NULL;
    tL2C_RCB *p_rcb = NULL;
    UINT16 spsm, scid, mtu, mps, credits;

    if (cmd_len < L2CAP_CMD_BLE_CREDIT_BASED_CONN_REQ_LEN) {
        /* Owe the peer a response even for a malformed request, otherwise it
         * waits out its RTX timer (Core Spec v6.2 Vol 3 Part A signalling
         * rules). ECFC 0x17 already rejects short packets the same way. */
        L2C_BLE_COC_TRACE_WARN("short 0x14 len=%u", cmd_len);
        l2cu_reject_ble_connection(p_lcb, id, L2CAP_LE_RESULT_UNACCEPTABLE_PARAMETERS);
        return;
    }

    STREAM_TO_UINT16(spsm, p);
    STREAM_TO_UINT16(scid, p);
    STREAM_TO_UINT16(mtu, p);
    STREAM_TO_UINT16(mps, p);
    STREAM_TO_UINT16(credits, p);

    L2C_BLE_COC_TRACE_DEBUG("rx 0x14 spsm=0x%04x scid=0x%04x mtu=%u mps=%u cred=%u id=%u",
                          spsm, scid, mtu, mps, credits, id);

    if (mtu < L2C_BLE_COC_MIN_MTU || mps < L2C_BLE_COC_MIN_MPS ||
        mps > L2CAP_LE_MAX_MPS) {
        L2C_BLE_COC_TRACE_WARN("bad params mtu=%u mps=%u", mtu, mps);
        l2cu_reject_ble_connection(p_lcb, id, L2CAP_LE_RESULT_UNACCEPTABLE_PARAMETERS);
        return;
    }

    /* The source CID must lie in the LE-U dynamically allocated range
     * (0x0040-0x007F, Core Spec v6.2 Vol 3 Part A Table 2.3). Reject values
     * such as 0x0000 or the fixed ATT CID 0x0004. */
    if (scid < L2CAP_BASE_APPL_CID || scid > L2CAP_BLE_CONN_MAX_CID) {
        L2C_BLE_COC_TRACE_WARN("0x14 invalid scid=0x%04x", scid);
        l2cu_reject_ble_connection(p_lcb, id, L2CAP_LE_RESULT_INVALID_SOURCE_CID);
        return;
    }

    if (l2cu_find_ccb_by_remote_cid(p_lcb, scid)) {
        l2cu_reject_ble_connection(p_lcb, id, L2CAP_LE_RESULT_SOURCE_CID_ALREADY_ALLOCATED);
        return;
    }

    p_rcb = l2cu_find_ble_rcb_by_psm(spsm);
    if (p_rcb == NULL) {
        L2C_BLE_COC_TRACE_WARN("no RCB for psm=0x%04x", spsm);
        l2cu_reject_ble_connection(p_lcb, id, L2CAP_LE_RESULT_NO_PSM);
        return;
    }

    p_ccb = l2cu_allocate_ccb(p_lcb, 0);
    if (p_ccb == NULL) {
        l2cu_reject_ble_connection(p_lcb, id, L2CAP_LE_RESULT_NO_RESOURCES);
        return;
    }

    p_ccb->le_coc_active = TRUE;
    p_ccb->remote_id = id;
    p_ccb->p_rcb = p_rcb;
    p_ccb->remote_cid = scid;
    p_ccb->peer_conn_cfg.mtu = mtu;
    p_ccb->peer_conn_cfg.mps = mps;
    p_ccb->peer_conn_cfg.credits = credits;
    l2c_ble_le_coc_apply_default_cfg(&p_ccb->local_conn_cfg);

    p_ccb->chnl_state = CST_TERM_W4_SEC_COMP;
#if (SMP_INCLUDED == TRUE)
    l2ble_sec_access_req(p_lcb->remote_bd_addr, p_rcb->real_psm, FALSE,
                         l2c_ble_le_coc_sec_cback, p_ccb);
#else
    /* SMP disabled: no security procedure, treat the access check as passed. */
    l2c_ble_le_coc_sec_cback(p_lcb->remote_bd_addr, BT_TRANSPORT_LE, p_ccb, BTM_SUCCESS);
#endif
}
#endif /* BLE_L2CAP_COC_SERVER_INCLUDED */

#if (BLE_L2CAP_COC_CLIENT_INCLUDED == TRUE)
static tL2C_CCB *l2c_ble_le_coc_find_ccb_by_sig_id(tL2C_LCB *p_lcb, UINT8 id)
{
    tL2C_CCB *p_ccb;

    if (p_lcb == NULL) {
        return NULL;
    }

    for (p_ccb = p_lcb->ccb_queue.p_first_ccb; p_ccb; p_ccb = p_ccb->p_next_ccb) {
        if (p_ccb->in_use && p_ccb->le_coc_active &&
#if (BLE_L2CAP_ENHANCED_COC_INCLUDED == TRUE)
            /* ECFC client CCBs also set le_coc_active and sit in
             * CST_W4_L2CAP_CONNECT_RSP with local_id == txn sig_id. Exclude them
             * so a base-CoC 0x15 response can never be applied to an ECFC channel
             * on a signalling-id collision (8-bit wrap or a malformed peer). */
            !p_ccb->le_ecfc_channel &&
#endif
            p_ccb->local_id == id &&
            p_ccb->chnl_state == CST_W4_L2CAP_CONNECT_RSP) {
            return p_ccb;
        }
    }
    return NULL;
}

void l2c_ble_le_coc_handle_credit_conn_res(tL2C_LCB *p_lcb, UINT8 *p, UINT8 id, UINT16 cmd_len)
{
    tL2C_CCB *p_ccb;
    UINT16 dcid, mtu, mps, credits, result;

    /* Find and de-arm the pending CCB before validating length, mirroring the
     * ECFC handler (l2c_ble_ecfc_handle_conn_res). The response id is consumed
     * either way; returning on a short packet without cleanup would leave the
     * CCB stuck in CST_W4_L2CAP_CONNECT_RSP until its response timer fires,
     * needlessly delaying error recovery. */
    p_ccb = l2c_ble_le_coc_find_ccb_by_sig_id(p_lcb, id);
    if (p_ccb == NULL || !p_ccb->le_coc_active) {
        L2C_BLE_COC_TRACE_WARN("0x15 unknown id=%u", id);
        return;
    }

    /* Response received: cancel the connect-response timeout. */
    l2c_ble_le_coc_stop_rsp_timer(p_ccb);

    if (cmd_len < L2CAP_CMD_BLE_CREDIT_BASED_CONN_RES_LEN) {
        L2C_BLE_COC_TRACE_WARN("short 0x15 len=%u", cmd_len);
        l2c_ble_le_coc_open_channel(p_ccb, L2CAP_CONN_NO_PSM);
        return;
    }

    STREAM_TO_UINT16(dcid, p);
    STREAM_TO_UINT16(mtu, p);
    STREAM_TO_UINT16(mps, p);
    STREAM_TO_UINT16(credits, p);
    STREAM_TO_UINT16(result, p);

    L2C_BLE_COC_TRACE_DEBUG("rx 0x15 dcid=0x%04x mtu=%u mps=%u cred=%u result=%u id=%u",
                          dcid, mtu, mps, credits, result, id);

    if (result != L2CAP_LE_RESULT_CONN_OK) {
        /* Forward the peer's specific reject reason (e.g. insufficient
         * authentication/encryption, no resources) instead of a hardcoded
         * L2CAP_CONN_NO_PSM, so the application can recover appropriately.
         * L2CAP_LE_RESULT_CONN_OK (0) == L2CAP_CONN_OK (0), so the success
         * check in l2c_ble_le_coc_open_channel still holds. Mirrors the ECFC
         * handler (l2c_ble_ecfc_handle_conn_res). */
        l2c_ble_le_coc_open_channel(p_ccb, result);
        return;
    }

    /* Destination CID must be from the LE-U dynamic range (Core Spec v6.2 Vol 3
     * Part A 4.23) and not already assigned on this link; otherwise outgoing
     * frames would target an invalid peer CID. Validate the DCID (and record
     * remote_cid) BEFORE the MTU/MPS check so a subsequent teardown can address
     * the peer's channel. An invalid/duplicate DCID cannot be cleanly torn down
     * (no valid target, and a duplicate would disconnect the wrong channel), so
     * that case still just drops our CCB. */
    if (dcid < L2CAP_BASE_APPL_CID || dcid > L2CAP_BLE_CONN_MAX_CID ||
        l2cu_find_ccb_by_remote_cid(p_lcb, dcid) != NULL) {
        L2C_BLE_COC_TRACE_WARN("0x15 invalid/duplicate dcid=0x%04x", dcid);
        l2c_ble_le_coc_open_channel(p_ccb, L2CAP_CONN_NO_PSM);
        return;
    }
    p_ccb->remote_cid = dcid;

    if (mtu < L2C_BLE_COC_MIN_MTU || mps < L2CAP_LE_MIN_MPS || mps > L2CAP_LE_MAX_MPS) {
        /* result==OK means the peer established the channel on its side; a bad
         * MTU/MPS makes it unusable for us, but merely dropping our CCB would
         * leave the peer's half orphaned until the ACL drops. Send a best-effort
         * DISC_REQ (remote_cid is set) to tear it down on the air, then report
         * the connect failure to the app (Core Spec v6.2 Vol 3 Part A 4.23
         * mandates a 23-byte minimum MTU/MPS). */
        L2C_BLE_COC_TRACE_WARN("0x15 bad mtu=%u mps=%u", mtu, mps);
        l2cble_send_peer_disc_req(p_ccb);
        l2c_ble_le_coc_open_channel(p_ccb, L2CAP_CONN_NO_PSM);
        return;
    }

    p_ccb->peer_conn_cfg.mtu = mtu;
    p_ccb->peer_conn_cfg.mps = mps;
    p_ccb->peer_conn_cfg.credits = credits;
    l2c_ble_le_coc_open_channel(p_ccb, L2CAP_CONN_OK);
}

BOOLEAN l2c_ble_le_coc_abort_conn_req(tL2C_LCB *p_lcb, UINT8 id, UINT16 result)
{
    /* A base LE CoC (0x14) client request whose signalling id was answered by a
     * CMD_REJECT (rather than a 0x15 response): fail it now so the app is told
     * immediately instead of waiting out the connect RTX timer. find_ccb_by_sig_id
     * excludes ECFC channels, so this never collides with the ECFC abort path
     * handling the same CMD_REJECT. */
    tL2C_CCB *p_ccb = l2c_ble_le_coc_find_ccb_by_sig_id(p_lcb, id);

    if (p_ccb == NULL) {
        return FALSE;
    }

    L2C_BLE_COC_TRACE_DEBUG("CMD_REJECT abort base CoC lcid=0x%04x id=%u result=%u", p_ccb->local_cid, id, result);
    l2c_ble_le_coc_stop_rsp_timer(p_ccb);
    l2c_ble_le_coc_open_channel(p_ccb, result); /* failure path releases the CCB */
    return TRUE;
}
#endif /* BLE_L2CAP_COC_CLIENT_INCLUDED */

void l2c_ble_le_coc_handle_flow_ctrl_credit(tL2C_LCB *p_lcb, UINT8 *p, UINT16 cmd_len)
{
    tL2C_CCB *p_ccb;
    UINT16 lcid, credit;

    if (cmd_len < L2CAP_CMD_BLE_FLOW_CTRL_CREDIT_LEN) {
        L2C_BLE_COC_TRACE_WARN("short 0x16 len=%u", cmd_len);
        return;
    }

    STREAM_TO_UINT16(lcid, p);
    STREAM_TO_UINT16(credit, p);

    /* Per Core Spec v6.2 Vol 3 Part A 4.24 the CID in L2CAP_FLOW_CONTROL_CREDIT_IND
     * is the sender's local (source) CID, i.e. our remote CID. Look up by remote
     * CID so credits are routed to the correct channel when several CoC channels
     * share the ACL link with non-matching CID values. */
    p_ccb = l2cu_find_ccb_by_remote_cid(p_lcb, lcid);
    if (p_ccb == NULL || !p_ccb->le_coc_active) {
        L2C_BLE_COC_TRACE_WARN("0x16 unknown lcid=0x%04x", lcid);
        return;
    }

    if (credit == 0) {
        /* Core Spec v6.2 Vol 3 Part A 10.1: a device receiving a
         * L2CAP_FLOW_CONTROL_CREDIT_IND with a credit value of zero shall ignore
         * the packet. Handling it would falsely toggle the congestion state. */
        L2C_BLE_COC_TRACE_WARN("0x16 zero credits lcid=0x%04x, ignoring", lcid);
        return;
    }

    if ((p_ccb->peer_conn_cfg.credits + credit) > L2CAP_LE_MAX_CREDIT) {
        L2C_BLE_COC_TRACE_ERROR("credit overflow lcid=0x%04x", lcid);
        l2c_ble_le_coc_initiate_disc(p_ccb);
        return;
    }

    p_ccb->peer_conn_cfg.credits += credit;
    L2C_BLE_COC_TRACE_DEBUG("0x16 lcid=0x%04x +%u tx_cred=%u", lcid, credit, p_ccb->peer_conn_cfg.credits);

    /* Only signal decongestion for a channel that is actually OPEN. Credits can
     * still arrive after l2c_ble_le_coc_initiate_disc moved the channel to
     * CST_W4_L2CAP_DISCONNECT_RSP; firing the callback then would tell the upper
     * layer the channel is ready to send while it is being torn down. Mirrors the
     * CST_OPEN guard in l2c_ble_le_coc_try_xmit. */
    if (p_ccb->chnl_state == CST_OPEN && p_ccb->p_rcb &&
        p_ccb->p_rcb->api.pL2CA_CongestionStatus_Cb && p_ccb->cong_sent) {
        UINT16 saved_lcid = p_ccb->local_cid;
        p_ccb->cong_sent = FALSE;
        (*p_ccb->p_rcb->api.pL2CA_CongestionStatus_Cb)(p_ccb->local_cid, FALSE);
        /* The callback runs synchronously and may re-enter L2CAP and release this
         * CCB. l2cu_release_ccb() clears in_use/p_lcb but leaves chnl_state intact,
         * so try_xmit's CST_OPEN check alone would not detect a freed slot; re-fetch
         * by the saved lcid and bail if it is gone, mirroring handle_disc_req /
         * handle_disc_rsp / open_channel / channel_timeout. */
        tL2C_CCB *p_now = l2cu_find_ccb_by_cid(NULL, saved_lcid);
        if (p_now != p_ccb || !p_ccb->in_use || !p_ccb->le_coc_active) {
            L2C_BLE_COC_TRACE_DEBUG("0x16: lcid=0x%04x freed in cong cb, skip xmit", saved_lcid);
            return;
        }
    }

    l2c_ble_le_coc_try_xmit(p_ccb);
    l2c_link_check_send_pkts(p_ccb->p_lcb, NULL, NULL);
}

void l2c_ble_le_coc_handle_disc_req(tL2C_CCB *p_ccb, tL2C_LCB *p_lcb, UINT8 id, UINT16 lcid, UINT16 rcid)
{
    if (p_ccb == NULL || p_lcb == NULL || !p_ccb->le_coc_active) {
        return;
    }

    /* Core Spec v6.2 Vol 3 Part A 4.6: if the DCID matches but the SCID does not,
     * silently discard the request. */
    if (p_ccb->remote_cid != 0 && rcid != p_ccb->remote_cid) {
        L2C_BLE_COC_TRACE_WARN("rx DISC_REQ scid mismatch lcid=0x%04x rcid=0x%04x expect=0x%04x",
                               lcid, rcid, p_ccb->remote_cid);
        return;
    }

    L2C_BLE_COC_TRACE_DEBUG("rx DISC_REQ lcid=0x%04x", lcid);
    p_ccb->remote_id = id;
    l2cu_send_peer_disc_rsp(p_lcb, id, lcid, rcid);
    /* The DisconnectInd callback runs synchronously and may re-enter L2CAP and
     * release/reuse this CCB slot; re-fetch by the saved lcid and only release if
     * it is still the same, in-use CoC CCB (mirrors l2c_ble_le_coc_open_channel). */
    UINT16 saved_lcid = p_ccb->local_cid;
    l2c_ble_le_coc_notify_disconnect(p_ccb, FALSE);
    tL2C_CCB *p_now = l2cu_find_ccb_by_cid(NULL, saved_lcid);
    if (p_now == p_ccb && p_ccb->in_use && p_ccb->le_coc_active) {
        l2cu_release_ccb(p_ccb);
    } else {
        L2C_BLE_COC_TRACE_DEBUG("DISC_REQ: lcid=0x%04x freed in cb, skip release", saved_lcid);
    }
}

void l2c_ble_le_coc_handle_disc_rsp(tL2C_LCB *p_lcb, UINT8 *p, UINT8 id, UINT16 cmd_len)
{
    tL2C_CCB *p_ccb;
    UINT16 lcid, rcid;

    if (cmd_len < L2CAP_DISC_REQ_LEN) {
        return;
    }

    STREAM_TO_UINT16(rcid, p);
    STREAM_TO_UINT16(lcid, p);

    p_ccb = l2cu_find_ccb_by_cid(p_lcb, lcid);
    if (p_ccb == NULL || !p_ccb->le_coc_active) {
        return;
    }

    /* Only accept a DISC_RSP that actually answers a DISC_REQ we sent: the
     * channel must be awaiting the response and the signalling Identifier must
     * match the one used for the request (Core Spec v6.2 Vol 3 Part A 4.7).
     * Otherwise an unsolicited/stale/cross DISC_RSP could tear down an OPEN
     * channel that reused the same CID. */
    if (p_ccb->chnl_state != CST_W4_L2CAP_DISCONNECT_RSP || p_ccb->local_id != id) {
        L2C_BLE_COC_TRACE_WARN("rx unexpected DISC_RSP lcid=0x%04x state=%d id=%u expect_id=%u",
                               lcid, p_ccb->chnl_state, id, p_ccb->local_id);
        return;
    }

    if (p_ccb->remote_cid != 0 && rcid != p_ccb->remote_cid) {
        /* The signalling Identifier already confirmed this DISC_RSP answers our
         * DISC_REQ, so no further response will arrive for this transaction.
         * Complete the teardown below even though the echoed DCID is malformed;
         * returning here would leave the CCB in CST_W4_L2CAP_DISCONNECT_RSP until
         * the 10s RTX timer fires. l2cu_release_ccb() below stops that timer. */
        L2C_BLE_COC_TRACE_WARN("rx DISC_RSP rcid mismatch lcid=0x%04x rcid=0x%04x expect=0x%04x",
                               lcid, rcid, p_ccb->remote_cid);
    }

    L2C_BLE_COC_TRACE_DEBUG("rx DISC_RSP lcid=0x%04x rcid=0x%04x", lcid, rcid);
    /* The DisconnectInd callback runs synchronously and may re-enter L2CAP and
     * release/reuse this CCB slot; re-fetch by the saved lcid and only release if
     * it is still the same, in-use CoC CCB (mirrors l2c_ble_le_coc_open_channel). */
    UINT16 saved_lcid = p_ccb->local_cid;
    l2c_ble_le_coc_notify_disconnect(p_ccb, TRUE);
    tL2C_CCB *p_now = l2cu_find_ccb_by_cid(NULL, saved_lcid);
    if (p_now == p_ccb && p_ccb->in_use && p_ccb->le_coc_active) {
        l2cu_release_ccb(p_ccb);
    } else {
        L2C_BLE_COC_TRACE_DEBUG("DISC_RSP: lcid=0x%04x freed in cb, skip release", saved_lcid);
    }
}

static BOOLEAN l2c_ble_le_coc_send_frame(tL2C_CCB *p_ccb, const UINT8 *data, UINT16 len)
{
    BT_HDR *p_buf;
    UINT8 *pkt;

    if (p_ccb->peer_conn_cfg.credits == 0) {
        return FALSE;
    }

    p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR) + HCI_DATA_PREAMBLE_SIZE +
                                 L2CAP_PKT_OVERHEAD + len);
    if (p_buf == NULL) {
        return FALSE;
    }

    p_buf->offset = HCI_DATA_PREAMBLE_SIZE;
    p_buf->len = L2CAP_PKT_OVERHEAD + len;
    p_buf->event = 0;
    p_buf->layer_specific = 0;
    pkt = (UINT8 *)(p_buf + 1) + p_buf->offset;
    UINT16_TO_STREAM(pkt, len);
    UINT16_TO_STREAM(pkt, p_ccb->remote_cid);
    memcpy(pkt, data, len);

    l2cu_set_acl_hci_header(p_buf, p_ccb);
    l2c_link_check_send_pkts(p_ccb->p_lcb, p_ccb, p_buf);
    p_ccb->peer_conn_cfg.credits--;

    L2C_BLE_COC_TRACE_DEBUG("tx frame lcid=0x%04x len=%u tx_cred=%u",
                            p_ccb->local_cid, len, p_ccb->peer_conn_cfg.credits);
    return TRUE;
}

static void l2c_ble_le_coc_try_xmit(tL2C_CCB *p_ccb)
{
    UINT16 mps, chunk, sdu_len, hdr_len;
    const UINT8 *src;
    BT_HDR *p_sdu;
    BOOLEAN completed_sdu = FALSE;

    if (p_ccb == NULL || p_ccb->chnl_state != CST_OPEN) {
        return;
    }

    /* Re-entrancy guard. The uncongested CongestionStatus_Cb(FALSE) below can be
     * invoked synchronously (e.g. the EATT congestion callback ->
     * gatt_cl_send_next_cmd_inq -> data_write -> try_xmit), which would otherwise
     * recurse once per credit and overflow the BTU task stack. Instead, a
     * re-entrant call just flags a rerun and returns; the outermost invocation
     * loops to drain the newly queued data. This preserves the self-clocked TX
     * (UNSTALLED-after-each-SDU) behaviour without unbounded recursion. */
    if (p_ccb->le_coc_xmit_busy) {
        p_ccb->le_coc_xmit_rerun = TRUE;
        return;
    }
    p_ccb->le_coc_xmit_busy = TRUE;

again:
    /* Re-validate the state on every (re)entry, not just at function entry. The
     * uncongested callback below can synchronously disconnect the channel
     * (chnl_state -> CST_W4_L2CAP_DISCONNECT_RSP after DISC_REQ is sent) and set
     * le_coc_xmit_rerun, so a plain "goto again" would otherwise resume the TX
     * loop on a non-OPEN channel and emit K-frames after the DISC_REQ. Reset the
     * busy flag so a recycled CCB is not left permanently blocked for TX. */
    if (p_ccb->chnl_state != CST_OPEN) {
        p_ccb->le_coc_xmit_busy = FALSE;
        return;
    }
    completed_sdu = FALSE;
    while (p_ccb->peer_conn_cfg.credits > 0) {
        if (p_ccb->le_coc_tx_sdu == NULL) {
            if (fixed_queue_is_empty(p_ccb->xmit_hold_q)) {
                break;
            }
            p_ccb->le_coc_tx_sdu = (BT_HDR *)fixed_queue_dequeue(p_ccb->xmit_hold_q, 0);
            p_ccb->le_coc_tx_offset = 0;
            p_ccb->le_coc_tx_len_sent = FALSE;
        }

        p_sdu = p_ccb->le_coc_tx_sdu;
        sdu_len = p_sdu->len;
        mps = l2c_ble_le_coc_effective_mps(p_ccb);
        src = (UINT8 *)(p_sdu + 1) + p_sdu->offset;

        if (!p_ccb->le_coc_tx_len_sent) {
            UINT8 *frame_buf;
            hdr_len = L2C_BLE_COC_SDU_LEN_SIZE;
            if (mps < hdr_len) {
                break;
            }
            frame_buf = (UINT8 *)osi_malloc(mps);
            if (frame_buf == NULL) {
                break;
            }
            frame_buf[0] = (UINT8)(sdu_len & 0xFF);
            frame_buf[1] = (UINT8)((sdu_len >> 8) & 0xFF);
            chunk = mps - hdr_len;
            if (chunk > sdu_len) {
                chunk = sdu_len;
            }
            memcpy(frame_buf + hdr_len, src, chunk);
            if (!l2c_ble_le_coc_send_frame(p_ccb, frame_buf, hdr_len + chunk)) {
                osi_free(frame_buf);
                break;
            }
            osi_free(frame_buf);
            p_ccb->le_coc_tx_offset += chunk;
            if (p_ccb->le_coc_tx_offset >= sdu_len) {
                osi_free(p_ccb->le_coc_tx_sdu);
                p_ccb->le_coc_tx_sdu = NULL;
                completed_sdu = TRUE;
            } else {
                p_ccb->le_coc_tx_len_sent = TRUE;
            }
        } else {
            chunk = sdu_len - p_ccb->le_coc_tx_offset;
            if (chunk > mps) {
                chunk = mps;
            }
            if (!l2c_ble_le_coc_send_frame(p_ccb, src + p_ccb->le_coc_tx_offset, chunk)) {
                break;
            }
            p_ccb->le_coc_tx_offset += chunk;
            if (p_ccb->le_coc_tx_offset >= sdu_len) {
                osi_free(p_ccb->le_coc_tx_sdu);
                p_ccb->le_coc_tx_sdu = NULL;
                completed_sdu = TRUE;
            }
        }
    }

    if (p_ccb->peer_conn_cfg.credits == 0 && p_ccb->p_rcb &&
        p_ccb->p_rcb->api.pL2CA_CongestionStatus_Cb && !p_ccb->cong_sent) {
        p_ccb->cong_sent = TRUE;
        (*p_ccb->p_rcb->api.pL2CA_CongestionStatus_Cb)(p_ccb->local_cid, TRUE);
        L2C_BLE_COC_TRACE_DEBUG("tx congested lcid=0x%04x", p_ccb->local_cid);
    } else if (completed_sdu && p_ccb->peer_conn_cfg.credits > 0 &&
               p_ccb->le_coc_tx_sdu == NULL &&
               fixed_queue_is_empty(p_ccb->xmit_hold_q) &&
               p_ccb->p_rcb && p_ccb->p_rcb->api.pL2CA_CongestionStatus_Cb) {
        /* SDU fully sent and pipeline drained with credits to spare: signal the
         * app it may send the next SDU. This self-clocks TX without polling. The
         * re-entrancy guard above turns any synchronous re-entry from this
         * callback into an iterative rerun instead of deep recursion. */
        p_ccb->cong_sent = FALSE;
        (*p_ccb->p_rcb->api.pL2CA_CongestionStatus_Cb)(p_ccb->local_cid, FALSE);
    }

    /* A re-entrant call (typically triggered by the callback above) queued more
     * data; drain it here in the outer frame rather than on a nested stack. */
    if (p_ccb->le_coc_xmit_rerun) {
        p_ccb->le_coc_xmit_rerun = FALSE;
        goto again;
    }
    p_ccb->le_coc_xmit_busy = FALSE;
}

UINT8 l2c_ble_le_coc_data_write(UINT16 lcid, BT_HDR *p_data)
{
    tL2C_CCB *p_ccb;

    if (p_data == NULL) {
        return L2CAP_DW_FAILED;
    }

    p_ccb = l2cu_find_ccb_by_cid(NULL, lcid);
    if (p_ccb == NULL || !p_ccb->le_coc_active || p_ccb->chnl_state != CST_OPEN) {
        osi_free(p_data);
        return L2CAP_DW_FAILED;
    }

    if (p_data->len > p_ccb->peer_conn_cfg.mtu) {
        L2C_BLE_COC_TRACE_WARN("SDU too large lcid=0x%04x len=%u mtu=%u",
                               lcid, p_data->len, p_ccb->peer_conn_cfg.mtu);
        osi_free(p_data);
        return L2CAP_DW_FAILED;
    }

    L2C_BLE_COC_TRACE_DEBUG("data_write lcid=0x%04x len=%u", lcid, p_data->len);
    /* fixed_queue_enqueue returns FALSE on OOM (list node alloc) without taking
     * ownership of p_data; free it and report failure instead of leaking + losing
     * the SDU while wrongly reporting success. */
    if (!fixed_queue_enqueue(p_ccb->xmit_hold_q, p_data, FIXED_QUEUE_MAX_TIMEOUT)) {
        L2C_BLE_COC_TRACE_ERROR("xmit enqueue failed lcid=0x%04x", lcid);
        osi_free(p_data);
        return L2CAP_DW_FAILED;
    }
    l2c_ble_le_coc_try_xmit(p_ccb);

    if (p_ccb->peer_conn_cfg.credits == 0 &&
        (p_ccb->le_coc_tx_sdu != NULL || !fixed_queue_is_empty(p_ccb->xmit_hold_q))) {
        return L2CAP_DW_CONGESTED;
    }
    return L2CAP_DW_SUCCESS;
}

BOOLEAN l2c_ble_le_coc_is_congested(UINT16 lcid)
{
    tL2C_CCB *p_ccb;

    p_ccb = l2cu_find_ccb_by_cid(NULL, lcid);
    if (p_ccb == NULL || !p_ccb->le_coc_active || p_ccb->chnl_state != CST_OPEN) {
        return TRUE;
    }
    if (p_ccb->cong_sent) {
        return TRUE;
    }
    if (p_ccb->le_coc_tx_sdu != NULL || !fixed_queue_is_empty(p_ccb->xmit_hold_q)) {
        return TRUE;
    }
    return FALSE;
}

BOOLEAN l2c_ble_le_coc_give_credits(UINT16 lcid, UINT16 credits)
{
    tL2C_CCB *p_ccb;

    if (credits == 0) {
        return FALSE;
    }

    p_ccb = l2cu_find_ccb_by_cid(NULL, lcid);
    if (p_ccb == NULL || !p_ccb->le_coc_active || p_ccb->chnl_state != CST_OPEN) {
        return FALSE;
    }

    /* In auto-credit mode the stack returns credits per consumed frame, so a
     * manual return (app recv_ready) would double-count and over-grant. Ignore. */
    if (!p_ccb->le_coc_no_auto_credit) {
        return TRUE;
    }

    if (p_ccb->p_lcb == NULL) {
        return FALSE;
    }

    /* Manual mode: return the credits actually consumed by the delivered SDU(s),
     * not the caller's nominal count. data_ind() tracked one credit per consumed
     * K-frame in le_coc_rx_manual_owed, so a multi-frame SDU returns >1 credit
     * even though the app calls recv_ready once. The caller's `credits` argument
     * is intentionally ignored here (recv_ready always means "return what the
     * processed SDU consumed"). Nothing owed (e.g. a pre-auth recv_ready before
     * any data) is a harmless no-op: the initial window was granted at open. */
    UINT16 give = p_ccb->le_coc_rx_manual_owed;
    if (give == 0) {
        return TRUE;
    }
    if ((UINT32)p_ccb->le_coc_rx_avail + give > L2CAP_LE_MAX_CREDIT) {
        /* Clamp so we never advertise more than the peer's max window; keep the
         * remainder owed to return on the next recv_ready. */
        give = L2CAP_LE_MAX_CREDIT - p_ccb->le_coc_rx_avail;
        if (give == 0) {
            L2C_BLE_COC_TRACE_ERROR("give_credits window full lcid=0x%04x", lcid);
            return FALSE;
        }
    }

    p_ccb->le_coc_rx_manual_owed -= give;
    p_ccb->le_coc_rx_avail += give;
    L2C_BLE_COC_TRACE_DEBUG("give_credits lcid=0x%04x +%u owed=%u", lcid, give, p_ccb->le_coc_rx_manual_owed);
    l2cble_send_flow_control_credit(p_ccb, give);
    return TRUE;
}

BOOLEAN l2c_ble_le_coc_disconnect(UINT16 lcid)
{
    tL2C_CCB *p_ccb;

    p_ccb = l2cu_find_ccb_by_cid(NULL, lcid);
    if (p_ccb == NULL || !p_ccb->le_coc_active) {
        return FALSE;
    }

    /* Already tearing down: don't emit a duplicate disconnect request. */
    if (p_ccb->chnl_state == CST_W4_L2CAP_DISCONNECT_RSP) {
        return TRUE;
    }

#if (BLE_L2CAP_COC_SERVER_INCLUDED == TRUE)
    /* Incoming connection still awaiting the app's accept/reject decision: the
     * peer sent an LE credit based connection request that we owe a response to.
     * Reject it (sends 0x15 with a failure result) instead of silently dropping
     * the CCB, which would leave the peer waiting for a response. */
    if (p_ccb->chnl_state == CST_W4_L2CA_CONNECT_RSP) {
        L2C_BLE_COC_TRACE_DEBUG("reject pending incoming coc lcid=0x%04x", lcid);
        l2c_ble_le_coc_connect_rsp(p_ccb, L2CAP_LE_RESULT_NO_RESOURCES);
        return TRUE;
    }
#endif

    /* A channel still connecting has remote_cid == 0. Emitting a disconnect
     * request with a 0x0000 destination CID violates the spec (Core Spec v6.2
     * Vol 3 Part A 2.1) and the peer will not answer, leaving the CCB stuck.
     * Cancel such a pending connection locally instead. */
    if (p_ccb->chnl_state != CST_OPEN || p_ccb->remote_cid == 0) {
        L2C_BLE_COC_TRACE_DEBUG("cancel pending coc lcid=0x%04x state=%d", lcid, p_ccb->chnl_state);
        l2cu_release_ccb(p_ccb);
        return TRUE;
    }

    L2C_BLE_COC_TRACE_DEBUG("disconnect lcid=0x%04x", lcid);
    l2c_ble_le_coc_initiate_disc(p_ccb);
    return TRUE;
}

void l2c_ble_le_coc_data_ind(tL2C_CCB *p_ccb, BT_HDR *p_msg)
{
    UINT8 *p;
    UINT16 frame_len, sdu_len, copy_len, hdr_need;
    tL2CA_DATA_IND_CB *cb;

    if (p_ccb == NULL || p_msg == NULL || !p_ccb->le_coc_active) {
        osi_free(p_msg);
        return;
    }

    if (p_ccb->chnl_state != CST_OPEN) {
        L2C_BLE_COC_TRACE_WARN("data on non-open lcid=0x%04x state=%d",
                              p_ccb->local_cid, p_ccb->chnl_state);
        osi_free(p_msg);
        return;
    }

    if (p_ccb->le_coc_rx_avail == 0) {
        L2C_BLE_COC_TRACE_ERROR("rx credit exhausted lcid=0x%04x", p_ccb->local_cid);
        l2c_ble_le_coc_initiate_disc(p_ccb);
        osi_free(p_msg);
        return;
    }

    frame_len = p_msg->len;
    p = (UINT8 *)(p_msg + 1) + p_msg->offset;
    p_ccb->le_coc_rx_avail--;

    /* Core Spec v6.2 Vol 3 Part A 3.4.3: "If the payload size of any K-frame
     * exceeds the receiver's MPS, the receiver shall disconnect the channel."
     * frame_len is the K-frame information payload (basic L2CAP header already
     * stripped), so it must not exceed the MPS we advertised for this channel. */
    if (frame_len > p_ccb->local_conn_cfg.mps) {
        L2C_BLE_COC_TRACE_ERROR("K-frame %u > mps %u lcid=0x%04x",
                                frame_len, p_ccb->local_conn_cfg.mps, p_ccb->local_cid);
        l2c_ble_le_coc_initiate_disc(p_ccb);
        osi_free(p_msg);
        return;
    }

    if (!p_ccb->le_coc_rx_have_len) {
        if (frame_len < L2C_BLE_COC_SDU_LEN_SIZE) {
            L2C_BLE_COC_TRACE_ERROR("short first frame lcid=0x%04x", p_ccb->local_cid);
            l2c_ble_le_coc_initiate_disc(p_ccb);
            osi_free(p_msg);
            return;
        }
        sdu_len = p[0] | (p[1] << 8);
        if (sdu_len > p_ccb->local_conn_cfg.mtu) {
            L2C_BLE_COC_TRACE_ERROR("SDU len %u > mtu %u", sdu_len, p_ccb->local_conn_cfg.mtu);
            l2c_ble_le_coc_initiate_disc(p_ccb);
            osi_free(p_msg);
            return;
        }

        p_ccb->le_coc_rx_sdu = (BT_HDR *)osi_malloc(sizeof(BT_HDR) + sdu_len);
        if (p_ccb->le_coc_rx_sdu == NULL) {
            /* Reassembly buffer OOM: disconnect instead of silently dropping.
             * le_coc_rx_have_len is still FALSE here, so keeping the channel up
             * would misparse the peer's subsequent continuation K-frames as new
             * first frames and desync reassembly. */
            L2C_BLE_COC_TRACE_ERROR("rx SDU alloc failed lcid=0x%04x len=%u", p_ccb->local_cid, sdu_len);
            l2c_ble_le_coc_initiate_disc(p_ccb);
            osi_free(p_msg);
            return;
        }
        p_ccb->le_coc_rx_sdu->offset = 0;
        p_ccb->le_coc_rx_sdu->len = 0;
        p_ccb->le_coc_rx_sdu_total = sdu_len;
        p_ccb->le_coc_rx_sdu_rcvd = 0;
        p_ccb->le_coc_rx_have_len = TRUE;

        copy_len = frame_len - L2C_BLE_COC_SDU_LEN_SIZE;
        /* Core Spec v6.2 Vol 3 Part A 3.4.3: "If the sum of the payload sizes
         * ... exceeds the specified SDU length, the receiver shall disconnect
         * the channel." A first frame carrying more data than the declared SDU
         * is malformed; disconnect instead of silently truncating (which would
         * misalign reassembly against the peer's intended SDU boundaries). */
        if (copy_len > sdu_len) {
            L2C_BLE_COC_TRACE_ERROR("first frame data %u > sdu %u lcid=0x%04x",
                                    copy_len, sdu_len, p_ccb->local_cid);
            l2c_ble_le_coc_initiate_disc(p_ccb);
            osi_free(p_msg);
            return;
        }
        memcpy((UINT8 *)(p_ccb->le_coc_rx_sdu + 1), p + L2C_BLE_COC_SDU_LEN_SIZE, copy_len);
        p_ccb->le_coc_rx_sdu->len = copy_len;
        p_ccb->le_coc_rx_sdu_rcvd = copy_len;
    } else {
        copy_len = frame_len;
        hdr_need = p_ccb->le_coc_rx_sdu_total - p_ccb->le_coc_rx_sdu_rcvd;
        /* Same 3.4.3 "shall disconnect" rule: a continuation frame that pushes
         * the running total past the declared SDU length is malformed. */
        if (copy_len > hdr_need) {
            L2C_BLE_COC_TRACE_ERROR("cont frame %u > remaining %u lcid=0x%04x",
                                    copy_len, hdr_need, p_ccb->local_cid);
            l2c_ble_le_coc_initiate_disc(p_ccb);
            osi_free(p_msg);
            return;
        }
        memcpy((UINT8 *)(p_ccb->le_coc_rx_sdu + 1) + p_ccb->le_coc_rx_sdu_rcvd, p, copy_len);
        p_ccb->le_coc_rx_sdu->len += copy_len;
        p_ccb->le_coc_rx_sdu_rcvd += copy_len;
    }

    osi_free(p_msg);

    /* Return one RX credit for the K-frame just consumed. In auto-credit mode we
     * batch the returns and flush when half the window has been used, or
     * immediately if the window is empty, so the peer keeps a healthy credit
     * pipeline and never ping-pongs at a single credit. */
    if (!p_ccb->le_coc_no_auto_credit) {
        UINT16 window = p_ccb->local_conn_cfg.credits;
        UINT16 flush_at = window ? ((window + 1) / 2) : 1;

        p_ccb->le_coc_rx_credits_pending++;
        if (p_ccb->le_coc_rx_credits_pending >= flush_at || p_ccb->le_coc_rx_avail == 0) {
            UINT16 give = p_ccb->le_coc_rx_credits_pending;
            p_ccb->le_coc_rx_credits_pending = 0;
            p_ccb->le_coc_rx_avail += give;
            l2cble_send_flow_control_credit(p_ccb, give);
        }
    } else {
        /* Manual mode: the stack does not return the credit now. Track each
         * consumed K-frame so recv_ready (l2c_ble_le_coc_give_credits) can return
         * the exact number of credits this SDU used. A multi-frame SDU consumes
         * >1 credit while the app calls recv_ready once per SDU; returning a fixed
         * 1 would leak (credit consumed per K-frame, returned per SDU). */
        p_ccb->le_coc_rx_manual_owed++;

        /* Deadlock breaker: if the peer's credit is now exhausted (avail == 0)
         * while the current SDU is still incomplete, the app can never receive
         * DATA_RECEIVED and thus never call recv_ready to replenish -> permanent
         * stall. Return the owed credits now so the peer can finish this SDU.
         * This only triggers for an SDU larger than the whole RX window; an SDU
         * that fits the window never reaches avail == 0 mid-reassembly (it hits 0
         * only on its final frame, when it is already complete), so normal
         * per-SDU backpressure is fully preserved for the supported range. */
        if (p_ccb->le_coc_rx_avail == 0 &&
            p_ccb->le_coc_rx_have_len &&
            p_ccb->le_coc_rx_sdu_rcvd < p_ccb->le_coc_rx_sdu_total) {
            UINT16 give = p_ccb->le_coc_rx_manual_owed;
            p_ccb->le_coc_rx_manual_owed = 0;
            p_ccb->le_coc_rx_avail += give;
            L2C_BLE_COC_TRACE_DEBUG("manual deadlock breaker lcid=0x%04x return %u", p_ccb->local_cid, give);
            l2cble_send_flow_control_credit(p_ccb, give);
        }
    }

    if (p_ccb->le_coc_rx_sdu_rcvd < p_ccb->le_coc_rx_sdu_total) {
        return;
    }

    L2C_BLE_COC_TRACE_DEBUG("SDU complete lcid=0x%04x len=%u", p_ccb->local_cid, p_ccb->le_coc_rx_sdu_total);

    p_ccb->le_coc_rx_have_len = FALSE;
    p_ccb->le_coc_rx_sdu_total = 0;
    p_ccb->le_coc_rx_sdu_rcvd = 0;

    /* Transfer ownership of the completed SDU before invoking the callback:
     * clear le_coc_rx_sdu first so that if the app synchronously tears the
     * channel down from within DataInd_Cb, cleanup_ccb() cannot free the same
     * buffer again (double free) and we never touch p_ccb after it may be gone. */
    BT_HDR *rx_sdu = p_ccb->le_coc_rx_sdu;
    p_ccb->le_coc_rx_sdu = NULL;
    if (p_ccb->p_rcb && p_ccb->p_rcb->api.pL2CA_DataInd_Cb) {
        cb = p_ccb->p_rcb->api.pL2CA_DataInd_Cb;
        (*cb)(p_ccb->local_cid, rx_sdu);
    } else {
        osi_free(rx_sdu);
    }
}

#endif /* BLE_L2CAP_COC_INCLUDED == TRUE */

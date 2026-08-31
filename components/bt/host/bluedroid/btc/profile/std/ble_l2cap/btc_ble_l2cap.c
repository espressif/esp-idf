/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "osi/allocator.h"
#include "btc/btc_task.h"
#include "btc/btc_manage.h"
#include "stack/l2c_api.h"
#include "l2c_int.h"
#include "stack/gatt_api.h"
#include "stack/btm_api.h"
#include "gatt_int.h"
#include "esp_err.h"
#include "btc_ble_l2cap.h"
#include "osi/list.h"

#if (BLE_L2CAP_COC_INCLUDED == TRUE)

#define BTC_BLE_L2CAP_TRACE_API(fmt, ...)   BTC_TRACE_API("BLE_L2CAP: " fmt, ##__VA_ARGS__)
#define BTC_BLE_L2CAP_TRACE_DEBUG(fmt, ...) BTC_TRACE_DEBUG("BLE_L2CAP: " fmt, ##__VA_ARGS__)
#define BTC_BLE_L2CAP_TRACE_ERROR(fmt, ...) BTC_TRACE_ERROR("BLE_L2CAP: " fmt, ##__VA_ARGS__)

typedef struct {
    bool in_use;
    uint16_t reg_psm;
    uint16_t real_psm;
    uint16_t mtu;
} btc_ble_l2cap_server_t;

typedef struct {
    bool initialized;
#if (BLE_L2CAP_COC_SERVER_INCLUDED == TRUE)
    btc_ble_l2cap_server_t servers[BLE_MAX_L2CAP_CLIENTS];
#endif
#if (BLE_L2CAP_COC_CLIENT_INCLUDED == TRUE)
    /* reg_psm values this module created (via L2CA_RegisterLECoc) for outgoing
     * client connections that have no server slot, so DEINIT can deregister them
     * instead of leaking BLE RCB pool entries. */
    uint16_t client_reg_psms[BLE_MAX_L2CAP_CLIENTS];
#endif
} btc_ble_l2cap_env_t;

typedef struct {
    bool in_use;
    uint16_t conn_id;
    BD_ADDR bda;
} btc_ble_l2cap_conn_bind_t;

/* Per-channel lcid->bda shadow. The disconnect_ind callback only receives the
 * lcid, and on some teardown paths (e.g. the classic L2CAP CSM link-loss path,
 * l2cu_disconnect_chnl) the CCB is released BEFORE the callback runs, so
 * l2cu_find_ccb_by_cid(lcid) returns NULL and the peer address can no longer be
 * derived from the stack. Recording lcid->bda when the channel opens lets us
 * still resolve the address at disconnect time and release the conn-bind slot. */
typedef struct {
    bool in_use;
    uint16_t lcid;
    BD_ADDR bda;
} btc_ble_l2cap_chan_bind_t;

static btc_ble_l2cap_env_t s_l2cap_env;
static tL2CAP_APPL_INFO s_l2cap_appl;
static btc_ble_l2cap_conn_bind_t s_conn_bind[BLE_MAX_L2CAP_CLIENTS];
static btc_ble_l2cap_chan_bind_t s_chan_bind[BLE_MAX_L2CAP_CLIENTS];

static void btc_ble_l2cap_post_event(esp_ble_l2cap_evt_t event, esp_ble_l2cap_cb_param_t *param);
static void btc_ble_l2cap_bind_conn(uint16_t conn_id, BD_ADDR bda);
static uint16_t btc_ble_l2cap_conn_id_from_bda(BD_ADDR bda);
static bool btc_ble_l2cap_bda_from_conn_id(uint16_t conn_id, BD_ADDR bda);
#if (BLE_L2CAP_COC_SERVER_INCLUDED == TRUE)
static btc_ble_l2cap_server_t *btc_ble_l2cap_find_server_by_real_psm(uint16_t psm);
#endif
static void btc_ble_l2cap_fill_chan_info_from_ccb(uint16_t chan_handle,
                                                 esp_ble_l2cap_chan_info_t *info);
#if (BLE_L2CAP_COC_CLIENT_INCLUDED == TRUE)
static uint16_t btc_ble_l2cap_get_or_register_psm(uint16_t real_psm, bool *newly_registered);
#endif

/* ESP GATTC/GATTS APIs expose conn_id as tcb_idx (see BTC_GATT_GET_CONN_ID). */
static uint16_t btc_ble_l2cap_app_conn_id_from_stack(UINT16 stack_conn_id)
{
    return (uint16_t)GATT_GET_TCB_IDX(stack_conn_id);
}

static void btc_ble_l2cap_bind_conn(uint16_t conn_id, BD_ADDR bda)
{
    int i;

    for (i = 0; i < BLE_MAX_L2CAP_CLIENTS; i++) {
        if (s_conn_bind[i].in_use && memcmp(s_conn_bind[i].bda, bda, BD_ADDR_LEN) == 0) {
            s_conn_bind[i].conn_id = conn_id;
            return;
        }
    }
    for (i = 0; i < BLE_MAX_L2CAP_CLIENTS; i++) {
        if (!s_conn_bind[i].in_use) {
            s_conn_bind[i].in_use = true;
            s_conn_bind[i].conn_id = conn_id;
            memcpy(s_conn_bind[i].bda, bda, BD_ADDR_LEN);
            return;
        }
    }
    BTC_BLE_L2CAP_TRACE_ERROR("%s: conn bind table full (max=%d), conn_id=%u not tracked",
                              __func__, BLE_MAX_L2CAP_CLIENTS, conn_id);
}

static void btc_ble_l2cap_unbind_bda(BD_ADDR bda)
{
    int i;

    for (i = 0; i < BLE_MAX_L2CAP_CLIENTS; i++) {
        if (s_conn_bind[i].in_use && memcmp(s_conn_bind[i].bda, bda, BD_ADDR_LEN) == 0) {
            memset(&s_conn_bind[i], 0, sizeof(s_conn_bind[i]));
            return;
        }
    }
}

/* Record (or refresh) the lcid->bda mapping for an opened CoC channel. */
static void btc_ble_l2cap_track_chan(uint16_t lcid, BD_ADDR bda)
{
    int i, free_idx = -1;

    for (i = 0; i < BLE_MAX_L2CAP_CLIENTS; i++) {
        if (s_chan_bind[i].in_use && s_chan_bind[i].lcid == lcid) {
            memcpy(s_chan_bind[i].bda, bda, BD_ADDR_LEN);
            return;
        }
        if (!s_chan_bind[i].in_use && free_idx < 0) {
            free_idx = i;
        }
    }
    if (free_idx >= 0) {
        s_chan_bind[free_idx].in_use = true;
        s_chan_bind[free_idx].lcid = lcid;
        memcpy(s_chan_bind[free_idx].bda, bda, BD_ADDR_LEN);
    } else {
        /* Table full: without a mapping, disconnect_ind cannot recover the BDA
         * once the CCB is gone, so the conn-bind slot for this peer would leak.
         * Log it so the (normally unreachable) exhaustion is diagnosable. */
        BTC_BLE_L2CAP_TRACE_ERROR("%s: chan bind table full (max=%d), lcid=0x%04x not tracked",
                                  __func__, BLE_MAX_L2CAP_CLIENTS, lcid);
    }
}

/* Look up the bda for an lcid and drop the entry. Returns true on success. */
static bool btc_ble_l2cap_untrack_chan(uint16_t lcid, BD_ADDR out_bda)
{
    int i;

    for (i = 0; i < BLE_MAX_L2CAP_CLIENTS; i++) {
        if (s_chan_bind[i].in_use && s_chan_bind[i].lcid == lcid) {
            if (out_bda != NULL) {
                memcpy(out_bda, s_chan_bind[i].bda, BD_ADDR_LEN);
            }
            memset(&s_chan_bind[i], 0, sizeof(s_chan_bind[i]));
            return true;
        }
    }
    return false;
}

/* Whether the shadow table still holds another channel to this peer. */
static bool btc_ble_l2cap_chan_has_other(BD_ADDR bda, uint16_t except_lcid)
{
    int i;

    for (i = 0; i < BLE_MAX_L2CAP_CLIENTS; i++) {
        if (s_chan_bind[i].in_use && s_chan_bind[i].lcid != except_lcid &&
            memcmp(s_chan_bind[i].bda, bda, BD_ADDR_LEN) == 0) {
            return true;
        }
    }
    return false;
}

static BOOLEAN btc_ble_l2cap_has_other_active_coc(BD_ADDR bda, UINT16 except_lcid)
{
    list_node_t *p_node;
    tL2C_LCB *p_lcb;
    tL2C_CCB *p_ccb;

    for (p_node = list_begin(l2cb.p_lcb_pool); p_node; p_node = list_next(p_node)) {
        p_lcb = list_node(p_node);
        /* The pool holds released LCBs too (in_use == FALSE, stale
         * remote_bd_addr); skip them and non-LE links to match the established
         * l2cu_find_lcb_by_bd_addr pattern. */
        if (!p_lcb->in_use || p_lcb->transport != BT_TRANSPORT_LE) {
            continue;
        }
        if (memcmp(p_lcb->remote_bd_addr, bda, BD_ADDR_LEN) != 0) {
            continue;
        }
        for (p_ccb = p_lcb->ccb_queue.p_first_ccb; p_ccb; p_ccb = p_ccb->p_next_ccb) {
            if (p_ccb->le_coc_active && p_ccb->local_cid != except_lcid) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

static uint16_t btc_ble_l2cap_conn_id_from_bda(BD_ADDR bda)
{
    int i;
    UINT16 conn_id = 0;

    for (i = 0; i < BLE_MAX_L2CAP_CLIENTS; i++) {
        if (s_conn_bind[i].in_use && memcmp(s_conn_bind[i].bda, bda, BD_ADDR_LEN) == 0) {
            return s_conn_bind[i].conn_id;
        }
    }
#if (GATTC_INCLUDED == TRUE)
    tGATT_REG *p_reg;

    for (i = 0, p_reg = gatt_cb.cl_rcb; i < GATT_MAX_APPS; i++, p_reg++) {
        if (p_reg->in_use &&
            GATT_GetConnIdIfConnected(p_reg->gatt_if, bda, &conn_id, BT_TRANSPORT_LE)) {
            return btc_ble_l2cap_app_conn_id_from_stack(conn_id);
        }
    }
#endif
#if (GATTS_INCLUDED == TRUE)
    tGATT_SR_REG *p_sr_reg;

    for (i = 0, p_sr_reg = gatt_cb.sr_reg; i < GATT_MAX_SR_PROFILES; i++, p_sr_reg++) {
        if (p_sr_reg->in_use &&
            GATT_GetConnIdIfConnected(p_sr_reg->gatt_if, bda, &conn_id, BT_TRANSPORT_LE)) {
            return btc_ble_l2cap_app_conn_id_from_stack(conn_id);
        }
    }
#endif
    return 0;
}

static bool btc_ble_l2cap_bda_from_conn_id(uint16_t conn_id, BD_ADDR bda)
{
    int i;
    tGATT_TCB *p_tcb;

    for (i = 0; i < BLE_MAX_L2CAP_CLIENTS; i++) {
        if (s_conn_bind[i].in_use && s_conn_bind[i].conn_id == conn_id) {
            memcpy(bda, s_conn_bind[i].bda, BD_ADDR_LEN);
            return true;
        }
    }

    p_tcb = gatt_get_tcb_by_idx((UINT8)conn_id);
    if (p_tcb != NULL && gatt_get_ch_state(p_tcb) >= GATT_CH_OPEN) {
        memcpy(bda, p_tcb->peer_bda, BD_ADDR_LEN);
        return true;
    }
    return false;
}

static void btc_ble_l2cap_fill_chan_info_from_ccb(uint16_t chan_handle,
                                                 esp_ble_l2cap_chan_info_t *info)
{
    tL2C_CCB *p_ccb;

    if (info == NULL) {
        return;
    }

    memset(info, 0, sizeof(*info));
    p_ccb = l2cu_find_ccb_by_cid(NULL, chan_handle);
    if (p_ccb == NULL || !p_ccb->le_coc_active) {
        return;
    }

    info->scid = p_ccb->local_cid;
    info->dcid = p_ccb->remote_cid;
    info->psm = p_ccb->p_rcb ? p_ccb->p_rcb->real_psm : 0;
    info->our_mtu = p_ccb->local_conn_cfg.mtu;
    info->peer_mtu = p_ccb->peer_conn_cfg.mtu;
    info->our_mps = p_ccb->local_conn_cfg.mps;
    info->peer_mps = p_ccb->peer_conn_cfg.mps;
}

#if (BLE_L2CAP_COC_SERVER_INCLUDED == TRUE)
static void btc_ble_l2cap_connect_ind(BD_ADDR bd_addr, UINT16 lcid, UINT16 psm, UINT8 id)
{
    esp_ble_l2cap_cb_param_t param = {0};

    param.coc_accept.conn_id = btc_ble_l2cap_conn_id_from_bda(bd_addr);
    param.coc_accept.chan_handle = lcid;
    param.coc_accept.l2cap_id = id;
    param.coc_accept.psm = psm;

    BTC_BLE_L2CAP_TRACE_API("ConnectInd conn_id=%u lcid=0x%04x psm=0x%04x id=%u",
                            param.coc_accept.conn_id, lcid, psm, id);
    btc_ble_l2cap_bind_conn(param.coc_accept.conn_id, bd_addr);
    btc_ble_l2cap_post_event(ESP_BLE_L2CAP_COC_ACCEPT_EVT, &param);
}
#endif /* BLE_L2CAP_COC_SERVER_INCLUDED */

static void btc_ble_l2cap_connect_cfm(UINT16 lcid, UINT16 result)
{
    esp_ble_l2cap_cb_param_t param = {0};
    tL2C_CCB *p_ccb = l2cu_find_ccb_by_cid(NULL, lcid);

    param.coc_connected.chan_handle = lcid;
    param.coc_connected.status = result;
    if (p_ccb && p_ccb->p_lcb) {
        param.coc_connected.conn_id = btc_ble_l2cap_conn_id_from_bda(p_ccb->p_lcb->remote_bd_addr);
    }

    BTC_BLE_L2CAP_TRACE_API("ConnectCfm conn_id=%u lcid=0x%04x status=%u",
                            param.coc_connected.conn_id, lcid, result);
    if (result == L2CAP_CONN_OK) {
        btc_ble_l2cap_fill_chan_info_from_ccb(lcid, &param.coc_connected.chan_info);
        /* Shadow the lcid->bda mapping so disconnect_ind can still resolve the
         * peer address (and release the conn-bind slot) even if the CCB is freed
         * before the DisconnectInd callback fires. */
        if (p_ccb && p_ccb->p_lcb) {
            btc_ble_l2cap_track_chan(lcid, p_ccb->p_lcb->remote_bd_addr);
        }
    } else if (p_ccb && p_ccb->p_lcb &&
               !btc_ble_l2cap_has_other_active_coc(p_ccb->p_lcb->remote_bd_addr, lcid) &&
               !btc_ble_l2cap_chan_has_other(p_ccb->p_lcb->remote_bd_addr, lcid)) {
        /* A failed connection is torn down via l2cu_release_ccb without a
         * DisconnectInd callback, so the s_conn_bind entry created at connect
         * time would leak. Release it here, mirroring disconnect_ind (which also
         * consults the shadow table so we do not unbind while another channel to
         * this peer is still tracked with a pending DisconnectInd). */
        btc_ble_l2cap_unbind_bda(p_ccb->p_lcb->remote_bd_addr);
    }
    btc_ble_l2cap_post_event(ESP_BLE_L2CAP_COC_CONNECTED_EVT, &param);
}

#if (BLE_L2CAP_COC_CLIENT_INCLUDED == TRUE)
/* A connect/connect_ecoc request can be rejected synchronously by the stack
 * (e.g. PSM registration or CCB allocation failure) with no CCB created, so
 * neither ConnectCfm nor DisconnectInd will ever fire. Report the failure to the
 * application (so it does not wait forever) and release the conn-bind slot
 * created at request time. */
static void btc_ble_l2cap_report_connect_fail(uint16_t conn_id, BD_ADDR bda, uint16_t status)
{
    esp_ble_l2cap_cb_param_t param = {0};

    /* Consult both the live stack and the lcid->bda shadow before releasing the
     * conn-bind slot, mirroring disconnect_ind. Another channel to this peer may
     * have already released its CCB (so has_other_active_coc misses it) while its
     * DisconnectInd is still pending in the shadow; unbinding on has_other_active_coc
     * alone would drop the slot prematurely. */
    if (!btc_ble_l2cap_has_other_active_coc(bda, 0) &&
        !btc_ble_l2cap_chan_has_other(bda, 0)) {
        btc_ble_l2cap_unbind_bda(bda);
    }
    param.coc_connected.conn_id = conn_id;
    param.coc_connected.chan_handle = 0;
    param.coc_connected.status = status;
    BTC_BLE_L2CAP_TRACE_API("connect fail conn_id=%u status=%u", conn_id, status);
    btc_ble_l2cap_post_event(ESP_BLE_L2CAP_COC_CONNECTED_EVT, &param);
}
#endif /* BLE_L2CAP_COC_CLIENT_INCLUDED */

static void btc_ble_l2cap_disconnect_ind(UINT16 lcid, BOOLEAN local_init)
{
    esp_ble_l2cap_cb_param_t param = {0};
    tL2C_CCB *p_ccb = l2cu_find_ccb_by_cid(NULL, lcid);
    BD_ADDR bda;
    bool have_bda = false;

    UNUSED(local_init);

    /* Prefer the live CCB's address; fall back to the lcid->bda shadow because
     * some teardown paths release the CCB before invoking this callback, in
     * which case l2cu_find_ccb_by_cid() returns NULL and the address would
     * otherwise be lost, leaking the conn-bind slot. */
    if (p_ccb != NULL && p_ccb->p_lcb != NULL) {
        memcpy(bda, p_ccb->p_lcb->remote_bd_addr, BD_ADDR_LEN);
        have_bda = true;
        btc_ble_l2cap_untrack_chan(lcid, NULL);
    } else if (btc_ble_l2cap_untrack_chan(lcid, bda)) {
        have_bda = true;
    }

    /* Release the conn-bind slot once the last CoC channel to this peer is gone.
     * Consult both the live stack (other CCBs) and the shadow table so we do not
     * unbind while another channel to the same peer is still up. */
    if (have_bda &&
        !btc_ble_l2cap_has_other_active_coc(bda, lcid) &&
        !btc_ble_l2cap_chan_has_other(bda, lcid)) {
        btc_ble_l2cap_unbind_bda(bda);
    }

    param.coc_disconnected.chan_handle = lcid;
    BTC_BLE_L2CAP_TRACE_API("DisconnectInd lcid=0x%04x", lcid);
    btc_ble_l2cap_post_event(ESP_BLE_L2CAP_COC_DISCONNECTED_EVT, &param);
}

static void btc_ble_l2cap_data_ind(UINT16 lcid, BT_HDR *p_buf)
{
    esp_ble_l2cap_cb_param_t param = {0};

    if (p_buf == NULL) {
        BTC_BLE_L2CAP_TRACE_ERROR("data_ind NULL buffer lcid=0x%04x", lcid);
        return;
    }

    param.data_received.chan_handle = lcid;
    param.data_received.len = p_buf->len;
    param.data_received.data = (UINT8 *)(p_buf + 1) + p_buf->offset;

    BTC_BLE_L2CAP_TRACE_DEBUG("DataInd lcid=0x%04x len=%u", lcid, p_buf->len);
    /* p_buf ownership transfers to btc_ble_l2cap_cb_deep_copy() */
    btc_ble_l2cap_post_event(ESP_BLE_L2CAP_COC_DATA_RECEIVED_EVT, &param);
}

static void btc_ble_l2cap_congestion(UINT16 lcid, BOOLEAN congested)
{
    esp_ble_l2cap_cb_param_t param = {0};

    if (congested) {
        return;
    }

    param.tx_unstalled.chan_handle = lcid;
    BTC_BLE_L2CAP_TRACE_DEBUG("TxUnstalled lcid=0x%04x", lcid);
    btc_ble_l2cap_post_event(ESP_BLE_L2CAP_COC_TX_UNSTALLED_EVT, &param);
}

#if (BLE_L2CAP_ENHANCED_COC_INCLUDED == TRUE)
static void btc_ble_l2cap_reconfig_ind(UINT16 lcid, UINT16 result, BOOLEAN peer_initiated)
{
    esp_ble_l2cap_cb_param_t param = {0};

    if (peer_initiated) {
        param.peer_reconfigured.chan_handle = lcid;
        param.peer_reconfigured.status = result;
        BTC_BLE_L2CAP_TRACE_API("PeerReconfig lcid=0x%04x status=%u", lcid, result);
        if (result == L2CAP_LE_RECONFIG_OK) {
            btc_ble_l2cap_fill_chan_info_from_ccb(lcid, &param.peer_reconfigured.chan_info);
            BTC_BLE_L2CAP_TRACE_API("PeerReconfig mtu=%u mps=%u",
                                    param.peer_reconfigured.chan_info.peer_mtu,
                                    param.peer_reconfigured.chan_info.peer_mps);
        }
        btc_ble_l2cap_post_event(ESP_BLE_L2CAP_COC_PEER_RECONFIGURED_EVT, &param);
    } else {
        param.reconfig_completed.chan_handle = lcid;
        param.reconfig_completed.status = result;
        BTC_BLE_L2CAP_TRACE_API("ReconfigCompleted lcid=0x%04x status=%u", lcid, result);
        if (result == L2CAP_LE_RECONFIG_OK) {
            btc_ble_l2cap_fill_chan_info_from_ccb(lcid, &param.reconfig_completed.chan_info);
            BTC_BLE_L2CAP_TRACE_API("ReconfigCompleted mtu=%u/%u mps=%u/%u",
                                    param.reconfig_completed.chan_info.our_mtu,
                                    param.reconfig_completed.chan_info.peer_mtu,
                                    param.reconfig_completed.chan_info.our_mps,
                                    param.reconfig_completed.chan_info.peer_mps);
        }
        btc_ble_l2cap_post_event(ESP_BLE_L2CAP_COC_RECONFIG_COMPLETED_EVT, &param);
    }
}
#endif

static void btc_ble_l2cap_post_event(esp_ble_l2cap_evt_t event, esp_ble_l2cap_cb_param_t *param)
{
    btc_msg_t msg = {0};
    bt_status_t ret;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_BLE_L2CAP;
    msg.act = event;

    ret = btc_transfer_context(&msg, param, sizeof(esp_ble_l2cap_cb_param_t),
                               btc_ble_l2cap_cb_deep_copy, btc_ble_l2cap_cb_deep_free);
    if (ret != BT_STATUS_SUCCESS) {
        BTC_BLE_L2CAP_TRACE_ERROR("btc_transfer_context failed evt=%d", event);
        /* Free the original RX BT_HDR only if the deep-copy callback did not
         * already free it. When btc_transfer_context fails after running the
         * deep copy, that callback clears param->data_received.data to NULL, so
         * the check below skips the free and avoids a double-free. If it failed
         * before the deep copy (e.g. message alloc failure), the pointer is
         * still valid and we free it here to avoid a leak (LE CoC delivers SDUs
         * with offset 0, so the BT_HDR header sits right before data). */
        if (event == ESP_BLE_L2CAP_COC_DATA_RECEIVED_EVT && param->data_received.data != NULL) {
            osi_free((UINT8 *)param->data_received.data - sizeof(BT_HDR));
        }
    }
}

#if (BLE_L2CAP_COC_SERVER_INCLUDED == TRUE)
static btc_ble_l2cap_server_t *btc_ble_l2cap_find_server_by_real_psm(uint16_t psm)
{
    for (int i = 0; i < BLE_MAX_L2CAP_CLIENTS; i++) {
        if (s_l2cap_env.servers[i].in_use && s_l2cap_env.servers[i].real_psm == psm) {
            return &s_l2cap_env.servers[i];
        }
    }
    return NULL;
}

static btc_ble_l2cap_server_t *btc_ble_l2cap_alloc_server(uint16_t psm, uint16_t mtu)
{
    for (int i = 0; i < BLE_MAX_L2CAP_CLIENTS; i++) {
        if (!s_l2cap_env.servers[i].in_use) {
            s_l2cap_env.servers[i].in_use = true;
            s_l2cap_env.servers[i].real_psm = psm;
            s_l2cap_env.servers[i].mtu = mtu;
            s_l2cap_env.servers[i].reg_psm = 0;
            return &s_l2cap_env.servers[i];
        }
    }
    return NULL;
}
#endif /* BLE_L2CAP_COC_SERVER_INCLUDED */

static void btc_ble_l2cap_register_psm_security(uint16_t reg_psm)
{
    BTM_SetSecurityLevel(TRUE, "BLE_L2CAP_COC", BTM_SEC_SERVICE_GEN_NET,
                         BTM_SEC_NONE, reg_psm, BTM_SEC_PROTO_L2CAP, 0);
    BTM_SetSecurityLevel(FALSE, "BLE_L2CAP_COC", BTM_SEC_SERVICE_GEN_NET,
                         BTM_SEC_NONE, reg_psm, BTM_SEC_PROTO_L2CAP, 0);
}

#if (BLE_L2CAP_COC_CLIENT_INCLUDED == TRUE)
static bool btc_ble_l2cap_track_client_psm(uint16_t reg_psm)
{
    int free_slot = -1;
    for (int i = 0; i < BLE_MAX_L2CAP_CLIENTS; i++) {
        if (s_l2cap_env.client_reg_psms[i] == reg_psm) {
            return true; /* already tracked */
        }
        if (free_slot < 0 && s_l2cap_env.client_reg_psms[i] == 0) {
            free_slot = i;
        }
    }
    if (free_slot >= 0) {
        s_l2cap_env.client_reg_psms[free_slot] = reg_psm;
        return true;
    }
    /* Table full: report it (mirrors btc_ble_l2cap_track_chan/bind_conn) and let
     * the caller deregister the PSM. A silently untracked PSM would never be
     * deregistered at DEINIT, permanently leaking a BLE RCB pool entry. */
    BTC_BLE_L2CAP_TRACE_ERROR("%s client PSM track table full, psm=0x%04x", __func__, reg_psm);
    return false;
}

static void btc_ble_l2cap_untrack_client_psm(uint16_t reg_psm)
{
    for (int i = 0; i < BLE_MAX_L2CAP_CLIENTS; i++) {
        if (s_l2cap_env.client_reg_psms[i] == reg_psm) {
            s_l2cap_env.client_reg_psms[i] = 0;
            return;
        }
    }
}

/* newly_registered (optional out): set TRUE only when this call created a fresh
 * L2CAP registration, so a failed connect attempt can deregister its own PSM
 * without tearing down a registration shared with a server or a prior client
 * connection to the same PSM. */
static uint16_t btc_ble_l2cap_get_or_register_psm(uint16_t real_psm, bool *newly_registered)
{
    UINT16 reg_psm;
    tL2C_RCB *p_rcb;

    if (newly_registered != NULL) {
        *newly_registered = false;
    }

#if (BLE_L2CAP_COC_SERVER_INCLUDED == TRUE)
    btc_ble_l2cap_server_t *srv = btc_ble_l2cap_find_server_by_real_psm(real_psm);

    if (srv != NULL && srv->reg_psm != 0) {
        btc_ble_l2cap_register_psm_security(srv->reg_psm);
        return srv->reg_psm;
    }
#endif

    p_rcb = l2cu_find_ble_rcb_by_real_psm(real_psm);
    if (p_rcb != NULL) {
        btc_ble_l2cap_register_psm_security(p_rcb->psm);
        return p_rcb->psm;
    }

    reg_psm = L2CA_RegisterLECoc(real_psm, &s_l2cap_appl);
    if (reg_psm != 0) {
        /* Remember it so DEINIT can deregister this client-created PSM. If the
         * tracking table is full, roll back the registration right away instead
         * of leaving a PSM that DEINIT can never find and deregister (RCB leak).
         * If the connect attempt later fails the caller deregisters + untracks. */
        if (!btc_ble_l2cap_track_client_psm(reg_psm)) {
            L2CA_DeregisterLECoc(reg_psm);
            return 0;
        }
        btc_ble_l2cap_register_psm_security(reg_psm);
        if (newly_registered != NULL) {
            *newly_registered = true;
        }
    }
    return reg_psm;
}
#endif /* BLE_L2CAP_COC_CLIENT_INCLUDED */

static void btc_ble_l2cap_register_appl_cb(void)
{
    memset(&s_l2cap_appl, 0, sizeof(s_l2cap_appl));
#if (BLE_L2CAP_COC_SERVER_INCLUDED == TRUE)
    s_l2cap_appl.pL2CA_ConnectInd_Cb = btc_ble_l2cap_connect_ind;
#endif
    s_l2cap_appl.pL2CA_ConnectCfm_Cb = btc_ble_l2cap_connect_cfm;
    s_l2cap_appl.pL2CA_DisconnectInd_Cb = btc_ble_l2cap_disconnect_ind;
    s_l2cap_appl.pL2CA_DataInd_Cb = btc_ble_l2cap_data_ind;
    s_l2cap_appl.pL2CA_CongestionStatus_Cb = btc_ble_l2cap_congestion;
#if (BLE_L2CAP_ENHANCED_COC_INCLUDED == TRUE)
    s_l2cap_appl.pL2CA_LeReconfigInd_Cb = btc_ble_l2cap_reconfig_ind;
#endif
}

void btc_ble_l2cap_cb_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    esp_ble_l2cap_cb_param_t *dst = (esp_ble_l2cap_cb_param_t *)p_dest;
    esp_ble_l2cap_cb_param_t *src = (esp_ble_l2cap_cb_param_t *)p_src;

    if (!dst || !src) {
        return;
    }

    memcpy(dst, src, sizeof(esp_ble_l2cap_cb_param_t));

    /* Only the DATA_RECEIVED event carries a heap pointer that needs a deep copy.
     * Do NOT clear dst->data_received.data for other events: the param is a union,
     * so writing data_received.data would clobber overlapping scalar fields of
     * other events (e.g. coc_accept.l2cap_id / psm). */
    if (msg->act == ESP_BLE_L2CAP_COC_DATA_RECEIVED_EVT && src->data_received.data != NULL) {
        BT_HDR *p_rx;

        dst->data_received.data = NULL;

        /* LE CoC delivers SDUs with offset 0 (see l2c_ble_le_coc_data_ind). */
        p_rx = (BT_HDR *)((UINT8 *)src->data_received.data - sizeof(BT_HDR));

        if (src->data_received.len > 0) {
            dst->data_received.data = (uint8_t *)osi_malloc(src->data_received.len);
            if (dst->data_received.data) {
                memcpy(dst->data_received.data, src->data_received.data, src->data_received.len);
            } else {
                /* Deep-copy OOM: never hand the app a non-zero len with a NULL
                 * pointer, or it will dereference NULL. Report an empty SDU. */
                dst->data_received.len = 0;
                BTC_BLE_L2CAP_TRACE_ERROR("rx deep_copy malloc failed len=%u",
                                          src->data_received.len);
            }
        }
        osi_free(p_rx);
        /* The original RX BT_HDR is now freed. Clear the source pointer (safe:
         * this is the DATA_RECEIVED union member) so the caller's error path in
         * btc_ble_l2cap_post_event sees NULL and does not free it a second time
         * when btc_task_post fails after this deep-copy already ran. */
        src->data_received.data = NULL;
    }
}

void btc_ble_l2cap_cb_deep_free(btc_msg_t *msg)
{
    esp_ble_l2cap_cb_param_t *param = (esp_ble_l2cap_cb_param_t *)msg->arg;

    if (param && msg->act == ESP_BLE_L2CAP_COC_DATA_RECEIVED_EVT && param->data_received.data) {
        osi_free(param->data_received.data);
        param->data_received.data = NULL;
    }
}

void btc_ble_l2cap_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_ble_l2cap_args_t *dst = (btc_ble_l2cap_args_t *)p_dest;
    btc_ble_l2cap_args_t *src = (btc_ble_l2cap_args_t *)p_src;

    if (!dst || !src) {
        return;
    }

    memcpy(dst, src, sizeof(btc_ble_l2cap_args_t));

    /* Only the SEND act carries a heap pointer that needs a deep copy. Do NOT
     * clear dst->send.data for other acts: send/connect_ecoc/etc. share the same
     * union, so writing send.data would clobber overlapping scalar fields (e.g.
     * connect_ecoc.mtu / num_chan). */
    if (msg->act == BTC_BLE_L2CAP_ACT_SEND) {
        dst->send.data = NULL;
        if (src->send.len > 0 && src->send.data) {
            BT_HDR *p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR) + src->send.len);
            if (p_buf) {
                p_buf->offset = 0;
                p_buf->len = src->send.len;
                p_buf->event = 0;
                p_buf->layer_specific = 0;
                memcpy((UINT8 *)(p_buf + 1), src->send.data, src->send.len);
                dst->send.data = (uint8_t *)p_buf;
            } else {
                BTC_BLE_L2CAP_TRACE_ERROR("deep_copy malloc failed act=%d len=%u", msg->act, src->send.len);
            }
        }
    }
}

void btc_ble_l2cap_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_l2cap_args_t *arg = (btc_ble_l2cap_args_t *)msg->arg;

    if (arg && msg->act == BTC_BLE_L2CAP_ACT_SEND && arg->send.data) {
        osi_free(arg->send.data);
        arg->send.data = NULL;
    }
}

static void btc_ble_l2cap_cb_to_app(esp_ble_l2cap_evt_t event, esp_ble_l2cap_cb_param_t *param)
{
    esp_ble_l2cap_cb_t cb = (esp_ble_l2cap_cb_t)btc_profile_cb_get(BTC_PID_BLE_L2CAP);
    if (cb) {
        cb(event, param);
    }
}

void btc_ble_l2cap_cb_handler(btc_msg_t *msg)
{
    esp_ble_l2cap_cb_param_t *param = (esp_ble_l2cap_cb_param_t *)msg->arg;

    if (msg->act < ESP_BLE_L2CAP_COC_EVT_MAX) {
        btc_ble_l2cap_cb_to_app((esp_ble_l2cap_evt_t)msg->act, param);
    } else {
        BTC_BLE_L2CAP_TRACE_ERROR("cb_handler invalid event act=%d", msg->act);
    }
    btc_ble_l2cap_cb_deep_free(msg);
}

void btc_ble_l2cap_call_handler(btc_msg_t *msg)
{
    btc_ble_l2cap_args_t *arg = (btc_ble_l2cap_args_t *)msg->arg;

    BTC_BLE_L2CAP_TRACE_DEBUG("%s act=%d", __func__, msg->act);

    switch (msg->act) {
    case BTC_BLE_L2CAP_ACT_INIT:
        BTC_BLE_L2CAP_TRACE_DEBUG("INIT");
        /* Mirror DEINIT: on a re-init without a prior DEINIT, deregister any PSMs
         * this module still tracks before wiping s_l2cap_env. Otherwise the
         * corresponding tL2C_RCB entries leak from the limited BLE RCB pool and
         * eventually make new L2CA_RegisterLECoc calls fail. */
        if (s_l2cap_env.initialized) {
#if (BLE_L2CAP_COC_SERVER_INCLUDED == TRUE)
            for (int i = 0; i < BLE_MAX_L2CAP_CLIENTS; i++) {
                if (s_l2cap_env.servers[i].in_use && s_l2cap_env.servers[i].reg_psm) {
                    L2CA_DeregisterLECoc(s_l2cap_env.servers[i].reg_psm);
                }
            }
#endif
#if (BLE_L2CAP_COC_CLIENT_INCLUDED == TRUE)
            for (int i = 0; i < BLE_MAX_L2CAP_CLIENTS; i++) {
                if (s_l2cap_env.client_reg_psms[i] != 0) {
                    L2CA_DeregisterLECoc(s_l2cap_env.client_reg_psms[i]);
                }
            }
#endif
        }
        memset(&s_l2cap_env, 0, sizeof(s_l2cap_env));
        memset(s_conn_bind, 0, sizeof(s_conn_bind));
        /* Mirror DEINIT: clear the channel-bind shadow table so a re-init without
         * a prior DEINIT does not inherit stale entries, which would make
         * btc_ble_l2cap_chan_has_other() wrongly hold conn-bind slots. */
        memset(s_chan_bind, 0, sizeof(s_chan_bind));
        s_l2cap_env.initialized = true;
        btc_ble_l2cap_register_appl_cb();
        break;

    case BTC_BLE_L2CAP_ACT_DEINIT:
        BTC_BLE_L2CAP_TRACE_DEBUG("DEINIT");
#if (BLE_L2CAP_COC_SERVER_INCLUDED == TRUE)
        for (int i = 0; i < BLE_MAX_L2CAP_CLIENTS; i++) {
            if (s_l2cap_env.servers[i].in_use && s_l2cap_env.servers[i].reg_psm) {
                BTC_BLE_L2CAP_TRACE_DEBUG("DEINIT deregister server reg_psm=0x%04x",
                                          s_l2cap_env.servers[i].reg_psm);
                L2CA_DeregisterLECoc(s_l2cap_env.servers[i].reg_psm);
            }
        }
#endif
#if (BLE_L2CAP_COC_CLIENT_INCLUDED == TRUE)
        /* Deregister PSMs this module registered for outgoing connections that
         * had no server slot, otherwise they leak BLE RCB pool entries across
         * an init/deinit cycle. */
        for (int i = 0; i < BLE_MAX_L2CAP_CLIENTS; i++) {
            if (s_l2cap_env.client_reg_psms[i] != 0) {
                BTC_BLE_L2CAP_TRACE_DEBUG("DEINIT deregister client reg_psm=0x%04x",
                                          s_l2cap_env.client_reg_psms[i]);
                L2CA_DeregisterLECoc(s_l2cap_env.client_reg_psms[i]);
            }
        }
#endif
        memset(&s_l2cap_env, 0, sizeof(s_l2cap_env));
        memset(s_conn_bind, 0, sizeof(s_conn_bind));
        memset(s_chan_bind, 0, sizeof(s_chan_bind));
        break;

#if (BLE_L2CAP_COC_SERVER_INCLUDED == TRUE)
    case BTC_BLE_L2CAP_ACT_CREATE_SERVER: {
        btc_ble_l2cap_server_t *srv = btc_ble_l2cap_find_server_by_real_psm(arg->create_server.psm);
        UINT16 reg_psm;
        if (srv != NULL) {
            /* Already registered for this PSM: refresh MTU instead of allocating
             * another slot. A second L2CA_RegisterLECoc would overwrite the RCB
             * and leak the previous slot. */
            srv->mtu = arg->create_server.mtu;
            BTC_BLE_L2CAP_TRACE_API("create_server psm=0x%04x already registered", srv->real_psm);
            break;
        }
        srv = btc_ble_l2cap_alloc_server(arg->create_server.psm, arg->create_server.mtu);
        if (srv == NULL) {
            BTC_BLE_L2CAP_TRACE_ERROR("no server slot psm=0x%04x", arg->create_server.psm);
            break;
        }
        reg_psm = L2CA_RegisterLECoc(arg->create_server.psm, &s_l2cap_appl);
        if (reg_psm == 0) {
            srv->in_use = false;
            BTC_BLE_L2CAP_TRACE_ERROR("RegisterLECoc failed psm=0x%04x", arg->create_server.psm);
            break;
        }
        srv->reg_psm = reg_psm;
        btc_ble_l2cap_register_psm_security(reg_psm);
        BTC_BLE_L2CAP_TRACE_API("create_server real_psm=0x%04x reg_psm=0x%04x mtu=%u",
                                srv->real_psm, srv->reg_psm, srv->mtu);
        break;
    }

    case BTC_BLE_L2CAP_ACT_DELETE_SERVER: {
        btc_ble_l2cap_server_t *srv = btc_ble_l2cap_find_server_by_real_psm(arg->delete_server.psm);
        if (srv && srv->reg_psm) {
            L2CA_DeregisterLECoc(srv->reg_psm);
            memset(srv, 0, sizeof(*srv));
            BTC_BLE_L2CAP_TRACE_DEBUG("delete_server psm=0x%04x", arg->delete_server.psm);
        } else {
            BTC_BLE_L2CAP_TRACE_ERROR("delete_server psm=0x%04x not found", arg->delete_server.psm);
        }
        break;
    }
#endif /* BLE_L2CAP_COC_SERVER_INCLUDED */

#if (BLE_L2CAP_COC_CLIENT_INCLUDED == TRUE)
    case BTC_BLE_L2CAP_ACT_CONNECT: {
        BD_ADDR bda;
        tL2CAP_LE_CFG_INFO cfg = {0};
        UINT16 reg_psm;
        UINT16 lcid;

        memset(bda, 0, BD_ADDR_LEN);
        if (!btc_ble_l2cap_bda_from_conn_id(arg->connect.conn_id, bda)) {
            BTC_BLE_L2CAP_TRACE_ERROR("invalid conn_id=%u", arg->connect.conn_id);
            btc_ble_l2cap_report_connect_fail(arg->connect.conn_id, bda, L2CAP_CONN_NO_LINK);
            break;
        }
        btc_ble_l2cap_bind_conn(arg->connect.conn_id, bda);
        cfg.mtu = arg->connect.mtu;
        bool newly_registered = false;
        reg_psm = btc_ble_l2cap_get_or_register_psm(arg->connect.psm, &newly_registered);
        if (reg_psm == 0) {
            BTC_BLE_L2CAP_TRACE_ERROR("client RegisterLECoc failed psm=0x%04x", arg->connect.psm);
            btc_ble_l2cap_report_connect_fail(arg->connect.conn_id, bda, L2CAP_CONN_NO_RESOURCES);
            break;
        }
        lcid = L2CA_ConnectLECocReq(reg_psm, bda, &cfg);
        if (lcid == 0) {
            BTC_BLE_L2CAP_TRACE_ERROR("ConnectLECocReq failed conn_id=%u psm=0x%04x",
                                      arg->connect.conn_id, arg->connect.psm);
            /* Only deregister a PSM this attempt registered. A shared PSM (server
             * or a prior client connection) must survive, or deregistering would
             * tear down other active channels using the same RCB. */
            if (newly_registered && l2cu_find_ble_rcb_by_psm(reg_psm) != NULL) {
                L2CA_DeregisterLECoc(reg_psm);
                btc_ble_l2cap_untrack_client_psm(reg_psm);
            }
            /* No CCB was created, so ConnectCfm will never fire: report the
             * failure and drop the conn-bind slot instead of leaking it. */
            btc_ble_l2cap_report_connect_fail(arg->connect.conn_id, bda, L2CAP_CONN_NO_RESOURCES);
            break;
        }
        BTC_BLE_L2CAP_TRACE_API("connect conn_id=%u lcid=0x%04x", arg->connect.conn_id, lcid);
        break;
    }
#endif /* BLE_L2CAP_COC_CLIENT_INCLUDED */

#if (BLE_L2CAP_COC_SERVER_INCLUDED == TRUE)
    case BTC_BLE_L2CAP_ACT_ACCEPT: {
        BD_ADDR bda;
        tL2CAP_LE_CFG_INFO cfg = {0};
        UINT16 result = arg->accept.accept ? L2CAP_CONN_OK : L2CAP_CONN_NO_PSM;
        tL2C_CCB *p_ccb = l2cu_find_ccb_by_cid(NULL, arg->accept.chan_handle);

        /* Resolve the peer address from the (always unique) channel handle
         * instead of conn_id. For GATT-less LE CoC peers conn_id is 0 for every
         * such peer, so btc_ble_l2cap_bda_from_conn_id could return the wrong
         * address and make L2CA_ConnectLECocRsp fail. Fall back to the conn_id
         * lookup only if the CCB is no longer available. */
        if (p_ccb != NULL && p_ccb->p_lcb != NULL) {
            memcpy(bda, p_ccb->p_lcb->remote_bd_addr, BD_ADDR_LEN);
        } else if (!btc_ble_l2cap_bda_from_conn_id(arg->accept.conn_id, bda)) {
            BTC_BLE_L2CAP_TRACE_ERROR("accept invalid conn_id=%u lcid=0x%04x",
                                      arg->accept.conn_id, arg->accept.chan_handle);
            /* Link torn down between ACCEPT_EVT and the app response: neither the
             * CCB nor conn_id resolves, so L2CA_ConnectLECocRsp cannot run and no
             * ConnectCfm will follow. For an accept, notify the app of the failure
             * so its pending esp_ble_l2cap_accept() does not hang forever (mirrors
             * the accept-fail path below and the CONNECT handler). No conn-bind was
             * created yet, so nothing to unbind. Reject needs no event: the reject
             * path never posts CONNECTED_EVT. */
            if (arg->accept.accept) {
                esp_ble_l2cap_cb_param_t param = {0};
                param.coc_connected.conn_id = arg->accept.conn_id;
                param.coc_connected.chan_handle = 0;
                param.coc_connected.status = L2CAP_CONN_NO_LINK;
                btc_ble_l2cap_post_event(ESP_BLE_L2CAP_COC_CONNECTED_EVT, &param);
            }
            break;
        }
        btc_ble_l2cap_bind_conn(arg->accept.conn_id, bda);
        cfg.mtu = arg->accept.mtu;
        if (!L2CA_ConnectLECocRsp(bda, arg->accept.l2cap_id, arg->accept.chan_handle,
                                  result, 0, &cfg)) {
            BTC_BLE_L2CAP_TRACE_ERROR("ConnectLECocRsp failed lcid=0x%04x", arg->accept.chan_handle);
            if (arg->accept.accept) {
                /* Accept failed at the stack API: no CCB and thus no ConnectCfm/
                 * DisconnectInd will follow. Notify the app of the failure and drop
                 * the conn-bind slot created above, mirroring the CONNECT failure
                 * path (btc_ble_l2cap_report_connect_fail, which is client-only, so
                 * the equivalent is inlined here). Consult the shadow table as the
                 * reject path does so we do not unbind while another channel to this
                 * peer is still tracked with a pending DisconnectInd. */
                esp_ble_l2cap_cb_param_t param = {0};
                param.coc_connected.conn_id = arg->accept.conn_id;
                param.coc_connected.chan_handle = 0;
                param.coc_connected.status = L2CAP_CONN_NO_RESOURCES;
                btc_ble_l2cap_post_event(ESP_BLE_L2CAP_COC_CONNECTED_EVT, &param);
                if (!btc_ble_l2cap_has_other_active_coc(bda, arg->accept.chan_handle) &&
                    !btc_ble_l2cap_chan_has_other(bda, arg->accept.chan_handle)) {
                    btc_ble_l2cap_unbind_bda(bda);
                }
                break;
            }
        }
        /* On reject the stack releases the CCB directly (no DisconnectInd
         * callback), so release the binding entry here to avoid leaking a slot.
         * Consult the shadow table too (as disconnect_ind does) so we do not
         * unbind while another channel to this peer is still tracked with a
         * pending DisconnectInd. */
        if (!arg->accept.accept &&
            !btc_ble_l2cap_has_other_active_coc(bda, arg->accept.chan_handle) &&
            !btc_ble_l2cap_chan_has_other(bda, arg->accept.chan_handle)) {
            btc_ble_l2cap_unbind_bda(bda);
        }
        break;
    }
#endif /* BLE_L2CAP_COC_SERVER_INCLUDED */

    case BTC_BLE_L2CAP_ACT_DISCONNECT:
        if (!L2CA_LECocDisconnect(arg->disconnect.chan_handle)) {
            BTC_BLE_L2CAP_TRACE_ERROR("disconnect failed lcid=0x%04x", arg->disconnect.chan_handle);
        }
        break;

    case BTC_BLE_L2CAP_ACT_SEND: {
        BT_HDR *p_buf;

        if (arg->send.len == 0 || arg->send.data == NULL) {
            BTC_BLE_L2CAP_TRACE_ERROR("send invalid lcid=0x%04x len=%u",
                                      arg->send.chan_handle, arg->send.len);
            break;
        }
        p_buf = (BT_HDR *)arg->send.data;
        arg->send.data = NULL;
        if (L2CA_LECocIsCongested(arg->send.chan_handle)) {
            BTC_BLE_L2CAP_TRACE_DEBUG("send dropped, congested lcid=0x%04x", arg->send.chan_handle);
            osi_free(p_buf);
            break;
        }
        if (L2CA_LECocDataWrite(arg->send.chan_handle, p_buf) == L2CAP_DW_FAILED) {
            /* L2CA_LECocDataWrite() already released p_buf on every DW_FAILED
             * path; freeing it here would be a double free. */
            BTC_BLE_L2CAP_TRACE_ERROR("send failed lcid=0x%04x", arg->send.chan_handle);
        }
        break;
    }

    case BTC_BLE_L2CAP_ACT_RECV_READY:
        if (!L2CA_LECocGiveCredits(arg->recv_ready.chan_handle, 1)) {
            BTC_BLE_L2CAP_TRACE_ERROR("recv_ready failed lcid=0x%04x", arg->recv_ready.chan_handle);
        }
        break;

    case BTC_BLE_L2CAP_ACT_SET_AUTO_CREDIT:
        if (!L2CA_LECocSetAutoCredit(arg->set_auto_credit.chan_handle,
                                     arg->set_auto_credit.enable ? TRUE : FALSE)) {
            BTC_BLE_L2CAP_TRACE_ERROR("set_auto_credit failed lcid=0x%04x",
                                      arg->set_auto_credit.chan_handle);
        }
        break;

#if (BLE_L2CAP_ENHANCED_COC_INCLUDED == TRUE)
#if (BLE_L2CAP_COC_CLIENT_INCLUDED == TRUE)
    case BTC_BLE_L2CAP_ACT_CONNECT_ECOC: {
        BD_ADDR bda;
        tL2CAP_LE_CFG_INFO cfg = {0};
        UINT16 lcids[BLE_MAX_L2CAP_CLIENTS];
        UINT16 reg_psm;
        UINT8 num_started;

        memset(bda, 0, BD_ADDR_LEN);
        if (!btc_ble_l2cap_bda_from_conn_id(arg->connect_ecoc.conn_id, bda)) {
            BTC_BLE_L2CAP_TRACE_ERROR("ecoc connect invalid conn_id=%u", arg->connect_ecoc.conn_id);
            btc_ble_l2cap_report_connect_fail(arg->connect_ecoc.conn_id, bda, L2CAP_CONN_NO_LINK);
            break;
        }
        cfg.mtu = arg->connect_ecoc.mtu;
        btc_ble_l2cap_bind_conn(arg->connect_ecoc.conn_id, bda);
        bool ecoc_newly_registered = false;
        reg_psm = btc_ble_l2cap_get_or_register_psm(arg->connect_ecoc.psm, &ecoc_newly_registered);
        if (reg_psm == 0) {
            BTC_BLE_L2CAP_TRACE_ERROR("client RegisterLECoc failed psm=0x%04x", arg->connect_ecoc.psm);
            btc_ble_l2cap_report_connect_fail(arg->connect_ecoc.conn_id, bda, L2CAP_CONN_NO_RESOURCES);
            break;
        }
        num_started = L2CA_ConnectLEEcocReq(reg_psm, bda, &cfg,
                                            arg->connect_ecoc.num_chan, lcids);
        if (num_started == 0) {
            BTC_BLE_L2CAP_TRACE_ERROR("ConnectLEEcocReq failed");
            /* Only deregister a PSM this attempt registered; a shared PSM must
             * survive so other active channels are not torn down. */
            if (ecoc_newly_registered && l2cu_find_ble_rcb_by_psm(reg_psm) != NULL) {
                L2CA_DeregisterLECoc(reg_psm);
                btc_ble_l2cap_untrack_client_psm(reg_psm);
            }
            /* No CCB created: report failure and release the conn-bind slot. */
            btc_ble_l2cap_report_connect_fail(arg->connect_ecoc.conn_id, bda, L2CAP_CONN_NO_RESOURCES);
        } else {
            BTC_BLE_L2CAP_TRACE_API("ecoc connect started n=%u", num_started);
        }
        break;
    }
#endif /* BLE_L2CAP_COC_CLIENT_INCLUDED */

    case BTC_BLE_L2CAP_ACT_RECONFIG:
        if (!L2CA_LEEcocReconfig(arg->reconfig.chan_handles, arg->reconfig.num_chan,
                                 arg->reconfig.mtu, arg->reconfig.mps)) {
            BTC_BLE_L2CAP_TRACE_ERROR("LEEcocReconfig failed");
        }
        break;
#endif

    default:
        BTC_BLE_L2CAP_TRACE_ERROR("unknown act=%d", msg->act);
        break;
    }

    btc_ble_l2cap_arg_deep_free(msg);
}

#endif /* BLE_L2CAP_COC_INCLUDED == TRUE */

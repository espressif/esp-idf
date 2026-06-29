/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* EATT (Enhanced ATT) over LE Enhanced CoC (PSM 0x0027). */

#include <string.h>
#include "common/bt_target.h"
#include "stack/bt_types.h"
#include "stack/l2c_api.h"
#include "stack/l2cdefs.h"
#include "stack/gatt_api.h"
#include "stack/gattdefs.h"
#include "stack/sdpdefs.h"
#include "stack/btm_ble_api.h"
#include "stack/btm_api.h"
#include "btm_int.h"
#include "l2c_int.h"
#include "gatt_int.h"
#include "gatt_eatt_int.h"
#include "osi/allocator.h"

#if (BLE_EATT_INCLUDED == TRUE)

#define GATT_EATT_TRACE_API(fmt, ...)    GATT_TRACE_API("EATT: " fmt, ##__VA_ARGS__)
#define GATT_EATT_TRACE_DEBUG(fmt, ...)  GATT_TRACE_DEBUG("EATT: " fmt, ##__VA_ARGS__)
#define GATT_EATT_TRACE_ERROR(fmt, ...)  GATT_TRACE_ERROR("EATT: " fmt, ##__VA_ARGS__)

#define BLE_GATT_SR_SUPP_FEAT_EATT_BITMASK 0x01
#define BLE_GATT_CL_SUPP_FEAT_EATT_BITMASK 0x02

typedef enum {
    GATT_EATT_ST_IDLE = 0,
    GATT_EATT_ST_READ_SR_FEAT,
    GATT_EATT_ST_READ_CL_FEAT,
    GATT_EATT_ST_WRITE_CL_FEAT,
    GATT_EATT_ST_CONNECTING,
} tGATT_EATT_SETUP_ST;

typedef struct {
    BOOLEAN in_use;
    BOOLEAN reported;   /* TRUE once a connect event (status=0) was delivered for
                         * this bearer, so free_bearer only reports a symmetric
                         * disconnect for bearers the app actually saw connect. */
    UINT16  lcid;
    UINT8   client_op;
    UINT16  clcb_idx;
} tGATT_EATT_BEARER;

typedef struct {
    BOOLEAN in_use;
    BD_ADDR peer_bda;
    UINT16  conn_id;
    UINT8   bearer_count;
    UINT8   setup_target;       /* number of bearers requested in the active setup */
    UINT8   setup_done;         /* number of setup responses (ok or fail) received */
    UINT16  default_lcid;
    tGATT_EATT_SETUP_ST setup_st;
    UINT16  peer_cl_feat_handle;
    UINT8   peer_cl_feat_val;   /* current Client Supported Features value read back */
    UINT8   tx_rr;              /* round-robin cursor for server-initiated PDUs */
    tGATT_EATT_BEARER bearers[GATT_EATT_MAX_CHAN];
} tGATT_EATT_CONN;

static tGATT_EATT_CONN s_eatt_conn[MAX_L2CAP_LINKS];
static tL2CAP_APPL_INFO s_eatt_l2cap_appl;
static UINT16 s_eatt_reg_psm;
static UINT8 s_eatt_chan_num = GATT_EATT_MAX_CHAN;
static tGATT_EATT_EVT_CBACK *s_eatt_evt_cback;
static tGATT_IF s_eatt_gatt_if;

static void gatt_eatt_connect_cfm(UINT16 lcid, UINT16 result);
static void gatt_eatt_disconnect_ind(UINT16 lcid, BOOLEAN local_init);
static void gatt_eatt_data_ind_l2c(UINT16 lcid, BT_HDR *p_buf);
static void gatt_eatt_congestion(UINT16 lcid, BOOLEAN congested);

static void gatt_eatt_gatt_conn_cback(tGATT_IF gatt_if, BD_ADDR bda, UINT16 conn_id,
                                      BOOLEAN connected, tGATT_DISCONN_REASON reason,
                                      tBT_TRANSPORT transport);
#if (BLE_EATT_CLIENT_INCLUDED == TRUE)
static void gatt_eatt_gatt_cmpl_cback(UINT16 conn_id, tGATTC_OPTYPE op, tGATT_STATUS status,
                                       tGATT_CL_COMPLETE *p_data);
#endif
#if (BLE_EATT_SERVER_INCLUDED == TRUE)
static void gatt_eatt_connect_ind(BD_ADDR bd_addr, UINT16 lcid, UINT16 psm, UINT8 id);
#endif

static tGATT_EATT_CONN *gatt_eatt_find_conn_by_bda(BD_ADDR bd_addr)
{
    for (int i = 0; i < MAX_L2CAP_LINKS; i++) {
        if (s_eatt_conn[i].in_use &&
            memcmp(s_eatt_conn[i].peer_bda, bd_addr, BD_ADDR_LEN) == 0) {
            return &s_eatt_conn[i];
        }
    }
    return NULL;
}

#if (BLE_EATT_CLIENT_INCLUDED == TRUE)
static tGATT_EATT_CONN *gatt_eatt_find_conn_for_setup(UINT16 stack_conn_id)
{
    UINT8 tcb_idx = GATT_GET_TCB_IDX(stack_conn_id);
    tGATT_TCB *p_tcb = gatt_get_tcb_by_idx(tcb_idx);

    if (p_tcb == NULL) {
        return NULL;
    }
    return gatt_eatt_find_conn_by_bda(p_tcb->peer_bda);
}
#endif

static tGATT_EATT_CONN *gatt_eatt_alloc_conn(BD_ADDR bd_addr)
{
    tGATT_EATT_CONN *ec;

    ec = gatt_eatt_find_conn_by_bda(bd_addr);
    if (ec != NULL) {
        return ec;
    }

    for (int i = 0; i < MAX_L2CAP_LINKS; i++) {
        if (!s_eatt_conn[i].in_use) {
            memset(&s_eatt_conn[i], 0, sizeof(s_eatt_conn[i]));
            s_eatt_conn[i].in_use = TRUE;
            memcpy(s_eatt_conn[i].peer_bda, bd_addr, BD_ADDR_LEN);
            return &s_eatt_conn[i];
        }
    }
    GATT_EATT_TRACE_ERROR("alloc_conn failed: conn table full");
    return NULL;
}

static tGATT_EATT_BEARER *gatt_eatt_find_bearer(tGATT_EATT_CONN *ec, UINT16 lcid)
{
    if (ec == NULL) {
        return NULL;
    }
    for (int i = 0; i < GATT_EATT_MAX_CHAN; i++) {
        if (ec->bearers[i].in_use && ec->bearers[i].lcid == lcid) {
            return &ec->bearers[i];
        }
    }
    return NULL;
}

static tGATT_EATT_BEARER *gatt_eatt_alloc_bearer(tGATT_EATT_CONN *ec, UINT16 lcid)
{
    tGATT_EATT_BEARER *b;

    b = gatt_eatt_find_bearer(ec, lcid);
    if (b != NULL) {
        return b;
    }

    for (int i = 0; i < GATT_EATT_MAX_CHAN; i++) {
        if (!ec->bearers[i].in_use) {
            memset(&ec->bearers[i], 0, sizeof(ec->bearers[i]));
            ec->bearers[i].in_use = TRUE;
            ec->bearers[i].lcid = lcid;
            ec->bearer_count++;
            return &ec->bearers[i];
        }
    }
    GATT_EATT_TRACE_ERROR("alloc_bearer failed: slots full lcid=0x%04x bearer_count=%u",
                          lcid, ec->bearer_count);
    return NULL;
}

static void gatt_eatt_free_bearer(tGATT_EATT_CONN *ec, tGATT_EATT_BEARER *b)
{
    BD_ADDR peer_bda;
    UINT16  freed_lcid;
    tGATT_TCB *p_tcb;

    if (ec == NULL || b == NULL || !b->in_use) {
        return;
    }
    memcpy(peer_bda, ec->peer_bda, BD_ADDR_LEN);
    freed_lcid = b->lcid;
    if (ec->default_lcid == b->lcid) {
        ec->default_lcid = 0;
    }
    /* Do not guard on ec->conn_id: an app conn_id of 0 is the valid tcb_idx 0
     * (the first BLE connection), not a "not found" sentinel. Guarding on it
     * would suppress the disconnect event for tcb_idx 0 while its connect event
     * was reported, causing an asymmetry.
     *
     * Only report the disconnect if a matching connect event was delivered. A
     * bearer allocated in connect_ind but torn down before connect_cfm succeeds
     * (e.g. L2CA_ConnectLECocRsp failed, or the link dropped mid-setup) was never
     * reported as connected, so emitting a disconnect for it would be spurious. */
    if (s_eatt_evt_cback && b->reported) {
        GATT_EATT_TRACE_DEBUG("free_bearer report disconnect conn_id=%u lcid=0x%04x",
                            ec->conn_id, b->lcid);
        (*s_eatt_evt_cback)(ec->conn_id, 1, b->lcid);
    }
    memset(b, 0, sizeof(*b));
    if (ec->bearer_count > 0) {
        ec->bearer_count--;
    }
    if (ec->bearer_count == 0) {
        GATT_EATT_TRACE_DEBUG("free_bearer last bearer gone conn_id=%u", ec->conn_id);
        ec->setup_st = GATT_EATT_ST_IDLE;
    }
    p_tcb = gatt_find_tcb_by_addr(peer_bda, BT_TRANSPORT_LE);
    if (p_tcb != NULL) {
        if (p_tcb->eatt_ind_bearer == freed_lcid) {
            p_tcb->eatt_ind_bearer = 0;
        }
#if (GATTS_INCLUDED == TRUE)
        if (p_tcb->sr_cmd.eatt_lcid == freed_lcid) {
            /* The freed bearer owns the pending server command. Fully clear the
             * sr_cmd slot (op_code, p_rsp_msg, multi_rsp_q, eatt_lcid) via
             * gatt_dequeue_sr_cmd. Clearing only eatt_lcid would leave op_code
             * non-zero, so gatt_sr_cmd_empty stays FALSE and every later server
             * request from this peer is silently discarded until the ACL drops;
             * any pending response buffer would also leak. */
            gatt_dequeue_sr_cmd(p_tcb);
        }
#endif
        if (ec->bearer_count == 0) {
            p_tcb->eatt_att_mtu = 0;
        }
    }
}

static void gatt_eatt_free_conn(tGATT_EATT_CONN *ec)
{
    if (ec == NULL) {
        return;
    }

    GATT_EATT_TRACE_DEBUG("free_conn conn_id=%u bearer_count=%u", ec->conn_id, ec->bearer_count);
    for (int i = 0; i < GATT_EATT_MAX_CHAN; i++) {
        if (ec->bearers[i].in_use) {
            gatt_eatt_free_bearer(ec, &ec->bearers[i]);
        }
    }
    memset(ec, 0, sizeof(*ec));
}

static UINT16 gatt_eatt_chan_mtu(UINT16 lcid)
{
    tL2C_CCB *p_ccb = l2cu_find_ccb_by_cid(NULL, lcid);

    if (p_ccb == NULL) {
        return 0;
    }
    return MIN(p_ccb->local_conn_cfg.mtu, p_ccb->peer_conn_cfg.mtu);
}

static void gatt_eatt_update_tcb_mtu(BD_ADDR bd_addr, UINT16 lcid)
{
    tGATT_TCB *p_tcb = gatt_find_tcb_by_addr(bd_addr, BT_TRANSPORT_LE);
    UINT16 mtu;

    if (p_tcb == NULL) {
        return;
    }
    mtu = gatt_eatt_chan_mtu(lcid);
    if (mtu >= GATT_DEF_BLE_MTU_SIZE) {
        p_tcb->eatt_att_mtu = mtu;
        GATT_EATT_TRACE_API("sync att mtu=%u lcid=0x%04x", mtu, lcid);
    }
}

void gatt_eatt_on_chan_mtu_changed(BD_ADDR bd_addr, UINT16 lcid)
{
    if (!gatt_eatt_is_bearer(lcid)) {
        return;
    }
    gatt_eatt_update_tcb_mtu(bd_addr, lcid);
}

UINT16 gatt_get_att_mtu(tGATT_TCB *p_tcb)
{
    UINT16 mtu;

    if (p_tcb == NULL) {
        return GATT_DEF_BLE_MTU_SIZE;
    }
    if (p_tcb->eatt_tx_bearer != 0) {
        mtu = gatt_eatt_chan_mtu(p_tcb->eatt_tx_bearer);
        if (mtu >= GATT_DEF_BLE_MTU_SIZE) {
            return mtu;
        }
    }
    if (p_tcb->eatt_rx_bearer != 0) {
        mtu = gatt_eatt_chan_mtu(p_tcb->eatt_rx_bearer);
        if (mtu >= GATT_DEF_BLE_MTU_SIZE) {
            return mtu;
        }
    }
    return p_tcb->payload_size;
}

UINT16 gatt_eatt_mtu_for_client_op(BD_ADDR bd_addr, UINT8 op_code, UINT16 legacy_mtu)
{
#if (BLE_EATT_CLIENT_INCLUDED == TRUE)
    UINT16 lcid;
    UINT16 mtu;

    if (op_code == GATT_REQ_MTU || op_code == GATT_CMD_WRITE ||
        op_code == GATT_SIGN_CMD_WRITE || op_code == GATT_HANDLE_VALUE_CONF) {
        return legacy_mtu;
    }

    lcid = gatt_eatt_get_available_bearer(bd_addr, op_code);
    if (lcid == L2CAP_ATT_CID) {
        return legacy_mtu;
    }

    mtu = gatt_eatt_chan_mtu(lcid);
    if (mtu >= GATT_DEF_BLE_MTU_SIZE) {
        return mtu;
    }
    return legacy_mtu;
#else
    UNUSED(bd_addr);
    UNUSED(op_code);
    return legacy_mtu;
#endif
}

static UINT16 gatt_eatt_app_conn_id_from_bda(BD_ADDR bd_addr)
{
    tGATT_TCB *p_tcb = gatt_find_tcb_by_addr(bd_addr, BT_TRANSPORT_LE);

    /* ESP exposes conn_id as tcb_idx (see BTC_GATT_GET_CONN_ID / btc_gattc.c),
     * so report tcb_idx here to stay consistent with esp_ble_gattc_* events and
     * with esp_ble_eatt_set_default_bearer(). */
    if (p_tcb != NULL && gatt_get_ch_state(p_tcb) == GATT_CH_OPEN) {
        return p_tcb->tcb_idx;
    }
    /* tcb_idx is a UINT8 starting at 0, so the first BLE connection legitimately
     * owns tcb_idx 0. Return the dedicated invalid sentinel (0xFFFF) for the
     * not-found/not-open case so a raced setup does not report an EATT event with
     * conn_id 0 that the app would misroute to the real first connection. */
    return GATT_INVALID_CONN_ID;
}

static UINT16 gatt_eatt_stack_conn_id_from_bda(BD_ADDR bd_addr)
{
    tGATT_TCB *p_tcb = gatt_find_tcb_by_addr(bd_addr, BT_TRANSPORT_LE);

    if (p_tcb != NULL && gatt_get_ch_state(p_tcb) == GATT_CH_OPEN && s_eatt_gatt_if != 0) {
        return GATT_CREATE_CONN_ID(p_tcb->tcb_idx, s_eatt_gatt_if);
    }
    if (p_tcb != NULL) {
        GATT_EATT_TRACE_DEBUG("stack_conn_id: tcb ch_state=%u eatt_gatt_if=%u",
                              gatt_get_ch_state(p_tcb), s_eatt_gatt_if);
    }
    return 0;
}

static void gatt_eatt_log_peer(const char *tag, BD_ADDR bd_addr)
{
    GATT_EATT_TRACE_API("%s peer %02x:%02x:%02x:%02x:%02x:%02x",
                        tag,
                        bd_addr[0], bd_addr[1], bd_addr[2],
                        bd_addr[3], bd_addr[4], bd_addr[5]);
}

static void gatt_eatt_report_evt(UINT16 conn_id, UINT8 status, UINT16 cid)
{
    if (s_eatt_evt_cback) {
        GATT_EATT_TRACE_DEBUG("report evt conn_id=%u status=%u cid=0x%04x", conn_id, status, cid);
        (*s_eatt_evt_cback)(conn_id, status, cid);
    } else {
        GATT_EATT_TRACE_DEBUG("report evt dropped (no cback) conn_id=%u status=%u cid=0x%04x",
                              conn_id, status, cid);
    }
}

static BOOLEAN gatt_eatt_is_central(BD_ADDR bd_addr)
{
    tL2C_LCB *p_lcb = l2cu_find_lcb_by_bd_addr(bd_addr, BT_TRANSPORT_LE);

    if (p_lcb != NULL && p_lcb->link_state == LST_CONNECTED) {
        return (p_lcb->link_role == HCI_ROLE_MASTER);
    }

    tBTM_SEC_DEV_REC *p_dev = btm_find_dev(bd_addr);
    return (p_dev != NULL && p_dev->role_master);
}

static BOOLEAN gatt_eatt_is_encrypted(BD_ADDR bd_addr)
{
    UINT8 sec_flag = 0;

    if (!BTM_GetSecurityFlagsByTransport(bd_addr, &sec_flag, BT_TRANSPORT_LE)) {
        return FALSE;
    }
    return (sec_flag & BTM_SEC_FLAG_ENCRYPTED) != 0;
}

#if (BLE_EATT_CLIENT_INCLUDED == TRUE)
static void gatt_eatt_fallback_legacy(tGATT_EATT_CONN *ec)
{
    if (ec == NULL) {
        return;
    }
    GATT_EATT_TRACE_DEBUG("EATT setup aborted, fallback to Legacy ATT (CID 4)");
    gatt_eatt_free_conn(ec);
}

static void gatt_eatt_start_ecoc_connect(BD_ADDR bd_addr)
{
    tGATT_EATT_CONN *ec = gatt_eatt_alloc_conn(bd_addr);
    tL2CAP_LE_CFG_INFO cfg = {0};
    UINT16 lcids[GATT_EATT_MAX_CHAN];

    if (ec == NULL) {
        return;
    }

    ec->conn_id = gatt_eatt_app_conn_id_from_bda(bd_addr);
    ec->setup_st = GATT_EATT_ST_CONNECTING;
    ec->setup_target = s_eatt_chan_num;
    ec->setup_done = 0;

    cfg.mtu = GATT_EATT_MTU;
    cfg.mps = L2CAP_LE_COC_MPS;

    GATT_EATT_TRACE_API("ConnectLEEcocReq psm=0x%04x n=%u", GATT_EATT_PSM, s_eatt_chan_num);
    {
        UINT8 num_started = L2CA_ConnectLEEcocReq(GATT_EATT_PSM, bd_addr, &cfg, s_eatt_chan_num, lcids);
        if (num_started == 0) {
            GATT_EATT_TRACE_ERROR("ConnectLEEcocReq failed (no RCB or link not ready?)");
            /* Release the conn slot so a stale in-use ec does not linger and
             * occupy a MAX_L2CAP_LINKS entry (fall back to Legacy ATT). */
            gatt_eatt_free_conn(ec);
        } else {
            GATT_EATT_TRACE_API("ConnectLEEcocReq started n=%u lcids=0x%04x 0x%04x 0x%04x",
                                num_started, lcids[0],
                                s_eatt_chan_num > 1 ? lcids[1] : 0,
                                s_eatt_chan_num > 2 ? lcids[2] : 0);
        }
    }
}

static void gatt_eatt_central_write_cl_feat(UINT16 conn_id, tGATT_EATT_CONN *ec)
{
    tGATT_VALUE wr;
    /* OR the EATT bit into the previously-read value instead of overwriting it,
     * so bits the client already set are preserved (Core Spec v6.2 Vol 3 Part G
     * 7.2 forbids clearing set bits). */
    UINT8 feat = ec->peer_cl_feat_val | BLE_GATT_CL_SUPP_FEAT_EATT_BITMASK;

    if (ec->peer_cl_feat_handle == 0) {
        gatt_eatt_start_ecoc_connect(ec->peer_bda);
        return;
    }

    memset(&wr, 0, sizeof(wr));
    wr.handle = ec->peer_cl_feat_handle;
    wr.len = 1;
    wr.value[0] = feat;
    ec->setup_st = GATT_EATT_ST_WRITE_CL_FEAT;

    if (GATTC_Write(conn_id, GATT_WRITE, &wr) != GATT_SUCCESS) {
        GATT_EATT_TRACE_ERROR("write cl supp feat failed");
        /* Writing Client Supported Features is not a prerequisite for the ECOC
         * bearers. Fall back to the connect phase (matching the read paths and
         * the async WRITE_CL_FEAT error handler) instead of silently abandoning
         * setup with no completion callback to advance the state machine. */
        gatt_eatt_start_ecoc_connect(ec->peer_bda);
    }
}

static void gatt_eatt_central_read_cl_feat(UINT16 conn_id, tGATT_EATT_CONN *ec)
{
    tGATT_READ_PARAM rd;
    tBT_UUID uuid;

    memset(&rd, 0, sizeof(rd));
    uuid.len = LEN_UUID_16;
    uuid.uu.uuid16 = GATT_UUID_CLIENT_SUP_FEAT;
    rd.char_type.s_handle = 0x0001;
    rd.char_type.e_handle = 0xFFFF;
    rd.char_type.uuid = uuid;
    rd.char_type.auth_req = GATT_AUTH_REQ_NONE;
    ec->setup_st = GATT_EATT_ST_READ_CL_FEAT;

    if (GATTC_Read(conn_id, GATT_READ_BY_TYPE, &rd) != GATT_SUCCESS) {
        GATT_EATT_TRACE_ERROR("read cl supp feat failed, try ecoc");
        gatt_eatt_start_ecoc_connect(ec->peer_bda);
    }
}

#if GATTS_ROBUST_CACHING_ENABLED
static void gatt_eatt_central_read_sr_feat(UINT16 conn_id, tGATT_EATT_CONN *ec)
{
    tGATT_READ_PARAM rd;
    tBT_UUID uuid;

    memset(&rd, 0, sizeof(rd));
    uuid.len = LEN_UUID_16;
    uuid.uu.uuid16 = GATT_UUID_SERVER_SUP_FEAT;
    rd.char_type.s_handle = 0x0001;
    rd.char_type.e_handle = 0xFFFF;
    rd.char_type.uuid = uuid;
    rd.char_type.auth_req = GATT_AUTH_REQ_NONE;
    ec->setup_st = GATT_EATT_ST_READ_SR_FEAT;

    {
        tGATT_STATUS st = GATTC_Read(conn_id, GATT_READ_BY_TYPE, &rd);
        if (st != GATT_SUCCESS) {
            GATT_EATT_TRACE_API("read sr feat queue failed status=0x%x, fallback legacy", st);
            gatt_eatt_fallback_legacy(ec);
        } else {
            GATT_EATT_TRACE_API("read sr feat queued conn_id=0x%04x", conn_id);
        }
    }
}
#endif /* GATTS_ROBUST_CACHING_ENABLED */

static void gatt_eatt_gatt_cmpl_cback(UINT16 conn_id, tGATTC_OPTYPE op, tGATT_STATUS status,
                                       tGATT_CL_COMPLETE *p_data)
{
    tGATT_EATT_CONN *ec;
    UINT8 sr_feat = 0;

    if (op != GATTC_OPTYPE_READ && op != GATTC_OPTYPE_WRITE) {
        return;
    }

    ec = gatt_eatt_find_conn_for_setup(conn_id);
    if (ec == NULL || ec->setup_st == GATT_EATT_ST_IDLE) {
        GATT_EATT_TRACE_DEBUG("gatt_cmpl: no active setup for conn_id=0x%04x", conn_id);
        return;
    }

    GATT_EATT_TRACE_API("gatt_cmpl op=%u status=0x%x setup_st=%u conn_id=0x%04x",
                        op, status, ec->setup_st, conn_id);

    if (status != GATT_SUCCESS) {
        if (ec->setup_st == GATT_EATT_ST_READ_SR_FEAT) {
            GATT_EATT_TRACE_API("read sr feat failed status=0x%x, fallback legacy", status);
            gatt_eatt_fallback_legacy(ec);
        } else if (ec->setup_st != GATT_EATT_ST_CONNECTING) {
            GATT_EATT_TRACE_DEBUG("setup GATT op=%u st=%u, try ecoc", op, ec->setup_st);
            gatt_eatt_start_ecoc_connect(ec->peer_bda);
        }
        return;
    }

    switch (ec->setup_st) {
    case GATT_EATT_ST_READ_SR_FEAT:
        if (p_data && p_data->att_value.len >= 1) {
            sr_feat = p_data->att_value.value[0];
        }
        if (sr_feat & BLE_GATT_SR_SUPP_FEAT_EATT_BITMASK) {
            GATT_EATT_TRACE_API("peer supports EATT (sr_feat=0x%02x), read cl feat", sr_feat);
            gatt_eatt_central_read_cl_feat(conn_id, ec);
        } else {
            GATT_EATT_TRACE_API("peer does not support EATT (sr_feat=0x%02x), legacy ATT", sr_feat);
            gatt_eatt_free_conn(ec);
        }
        break;

    case GATT_EATT_ST_READ_CL_FEAT:
        if (p_data) {
            ec->peer_cl_feat_handle = p_data->att_value.handle;
            /* Preserve any bits already set in the Client Supported Features
             * value so the subsequent write does not clear them (Core Spec v6.2
             * Vol 3 Part G 7.2: a client shall not clear bits it has set, else
             * the server rejects the write with Value Not Allowed 0x13). */
            if (p_data->att_value.len >= 1) {
                ec->peer_cl_feat_val = p_data->att_value.value[0];
            }
        }
        gatt_eatt_central_write_cl_feat(conn_id, ec);
        break;

    case GATT_EATT_ST_WRITE_CL_FEAT:
        gatt_eatt_start_ecoc_connect(ec->peer_bda);
        break;

    default:
        break;
    }
}
#endif /* BLE_EATT_CLIENT_INCLUDED */

/* The EATT module registers a GATT interface only to receive connection /
 * operation-complete callbacks for its own bearer setup; it is not a GATT
 * server application. Server request indications (e.g. the legacy ATT Exchange
 * MTU) are fanned out to every registered interface, so provide a no-op
 * request callback to absorb them instead of tripping the
 * "Call back not found for application" warning in gatt_sr_send_req_callback().
 * Mirrors bta_gattc_req_cback(). */
static void gatt_eatt_gatt_req_cback(UINT16 conn_id, UINT32 trans_id,
                                     tGATTS_REQ_TYPE type, tGATTS_DATA *p_data)
{
    UNUSED(conn_id);
    UNUSED(trans_id);
    UNUSED(type);
    UNUSED(p_data);
}

static void gatt_eatt_gatt_conn_cback(tGATT_IF gatt_if, BD_ADDR bda, UINT16 conn_id,
                                      BOOLEAN connected, tGATT_DISCONN_REASON reason,
                                      tBT_TRANSPORT transport)
{
    UNUSED(gatt_if);
    UNUSED(conn_id);
    UNUSED(reason);

    /* EATT state is BLE-only. A BR/EDR GATT disconnect for the same BDA must not
     * tear down the BLE EATT connection (find_conn_by_bda matches on BDA only). */
    if (transport != BT_TRANSPORT_LE) {
        return;
    }

    if (!connected) {
        tGATT_EATT_CONN *ec = gatt_eatt_find_conn_by_bda(bda);
        if (ec) {
            GATT_EATT_TRACE_DEBUG("gatt disconnect, free_conn conn_id=%u reason=0x%x", conn_id, reason);
            gatt_eatt_free_conn(ec);
        }
    }
}

#if (BLE_EATT_SERVER_INCLUDED == TRUE)
static void gatt_eatt_release_empty_conn(tGATT_EATT_CONN *ec)
{
    if (ec != NULL && ec->bearer_count == 0) {
        gatt_eatt_free_conn(ec);
    }
}

static void gatt_eatt_connect_ind(BD_ADDR bd_addr, UINT16 lcid, UINT16 psm, UINT8 id)
{
    tL2CAP_LE_CFG_INFO cfg = {0};
    tGATT_EATT_CONN *ec;
    tGATT_EATT_BEARER *bearer;

    UNUSED(psm);

    if (!gatt_eatt_is_encrypted(bd_addr)) {
        GATT_EATT_TRACE_API("connect_ind: reject unencrypted lcid=0x%04x", lcid);
        /* Per Core Spec v6.2 10.1/10.2, reject due to missing security with the
         * insufficient-encryption result, not a generic no-resources code, so
         * the peer knows to encrypt/pair rather than retry. */
        L2CA_ConnectLECocRsp(bd_addr, id, lcid, L2CAP_LE_RESULT_INSUFFICIENT_ENCRY, 0, NULL);
        return;
    }

    gatt_eatt_log_peer("connect_ind accept", bd_addr);

    ec = gatt_eatt_alloc_conn(bd_addr);
    if (ec == NULL) {
        GATT_EATT_TRACE_ERROR("connect_ind: no conn slot, reject lcid=0x%04x", lcid);
        L2CA_ConnectLECocRsp(bd_addr, id, lcid, L2CAP_LE_RESULT_NO_RESOURCES, 0, NULL);
        return;
    }

    ec->conn_id = gatt_eatt_app_conn_id_from_bda(bd_addr);
    bearer = gatt_eatt_alloc_bearer(ec, lcid);
    if (bearer == NULL) {
        /* No free bearer slot: reject so the L2CAP channel is not left up while
         * GATT does not track it (which would break TX routing and reporting). */
        GATT_EATT_TRACE_ERROR("connect_ind: no bearer slot lcid=0x%04x", lcid);
        L2CA_ConnectLECocRsp(bd_addr, id, lcid, L2CAP_LE_RESULT_NO_RESOURCES, 0, NULL);
        gatt_eatt_release_empty_conn(ec);
        return;
    }

    cfg.mtu = GATT_EATT_MTU;
    cfg.mps = L2CAP_LE_COC_MPS;
    /* Grant the full initial RX credit window (L2CAP_LE_INIT_CREDITS) instead of
     * a single credit. A window of 1 forces the peer into a one-K-frame-at-a-time
     * ping-pong (send frame -> wait for credit return), which throttles the
     * central -> peripheral GATT request throughput on EATT bearers. Leaving
     * credits at 0 would let l2c_ble_ecfc_apply_default_cfg() fill the same
     * value; set it explicitly for clarity and symmetry with the central path. */
    cfg.credits = L2CAP_LE_INIT_CREDITS;
    if (!L2CA_ConnectLECocRsp(bd_addr, id, lcid, L2CAP_CONN_OK, 0, &cfg)) {
        GATT_EATT_TRACE_ERROR("connect_ind: ConnectLECocRsp failed lcid=0x%04x", lcid);
        gatt_eatt_free_bearer(ec, bearer);
        gatt_eatt_release_empty_conn(ec);
        (void)L2CA_LECocDisconnect(lcid);
        return;
    }
    GATT_EATT_TRACE_DEBUG("connect_ind accepted lcid=0x%04x conn_id=%u", lcid, ec->conn_id);
    gatt_eatt_update_tcb_mtu(bd_addr, lcid);
}
#endif /* BLE_EATT_SERVER_INCLUDED */

/* Account for one completed EATT setup response (success or failure) on a
 * client-initiated setup, and release the setup state once every requested
 * bearer has been answered. Without this, a partial failure (peer accepts
 * fewer bearers than requested) would wedge setup_st at CONNECTING and block
 * any future EATT setup on this connection. Server-side (setup_st != CONNECTING)
 * is unaffected. */
static void gatt_eatt_setup_mark_done(tGATT_EATT_CONN *ec)
{
    if (ec == NULL || ec->setup_st != GATT_EATT_ST_CONNECTING) {
        return;
    }
    if (ec->setup_done < 0xFF) {
        ec->setup_done++;
    }
    if (ec->bearer_count >= s_eatt_chan_num ||
        (ec->setup_target != 0 && ec->setup_done >= ec->setup_target)) {
        ec->setup_st = GATT_EATT_ST_IDLE;
    }
}

/* Invoked from l2c_ble_le_coc_open_channel() for both originator and acceptor
 * paths; do not trim with BLE_EATT_CLIENT_INCLUDED. */
static void gatt_eatt_connect_cfm(UINT16 lcid, UINT16 result)
{
    tL2C_CCB *p_ccb = l2cu_find_ccb_by_cid(NULL, lcid);
    tGATT_EATT_CONN *ec;
    tGATT_EATT_BEARER *b;

    if (p_ccb == NULL || p_ccb->p_lcb == NULL) {
        GATT_EATT_TRACE_ERROR("connect_cfm: no ccb/lcb lcid=0x%04x result=%u", lcid, result);
        return;
    }

    if (result != L2CAP_LE_RESULT_CONN_OK) {
        /* Failure path: only clean up an EXISTING conn. Do not allocate one here.
         * If the original EATT conn was already freed (e.g. a GATT disconnect
         * raced ahead of this CoC failure callback), gatt_eatt_alloc_conn would
         * create a fresh empty conn (no bearers, setup_st IDLE) that nothing ever
         * frees, leaking a MAX_L2CAP_LINKS slot. */
        ec = gatt_eatt_find_conn_by_bda(p_ccb->p_lcb->remote_bd_addr);
        if (ec == NULL) {
            GATT_EATT_TRACE_DEBUG("connect_cfm fail, conn gone lcid=0x%04x result=%u", lcid, result);
            return;
        }
        GATT_EATT_TRACE_ERROR("bearer connect failed lcid=0x%04x result=%u", lcid, result);
        b = gatt_eatt_find_bearer(ec, lcid);
        if (b) {
            gatt_eatt_free_bearer(ec, b);
        }
        gatt_eatt_setup_mark_done(ec);
        return;
    }

    /* Only look up the EXISTING conn; do not allocate here. In both the client
     * (gatt_eatt_start_ecoc_connect) and server (gatt_eatt_connect_ind) flows the
     * conn is allocated before connect_cfm arrives, so find_conn_by_bda succeeds.
     * If it is NULL the original conn was already freed by a racing GATT
     * disconnect; allocating a fresh one here would create an orphan conn (no
     * GATT conn cback to ever free it) and leak a MAX_L2CAP_LINKS slot. Tear the
     * L2CAP channel down instead, mirroring the failure path (696-712) and the
     * no-bearer-slot path below. */
    ec = gatt_eatt_find_conn_by_bda(p_ccb->p_lcb->remote_bd_addr);
    if (ec == NULL) {
        GATT_EATT_TRACE_ERROR("connect_cfm ok but conn gone, disconnect lcid=0x%04x", lcid);
        L2CA_LECocDisconnect(lcid);
        return;
    }
    ec->conn_id = gatt_eatt_app_conn_id_from_bda(p_ccb->p_lcb->remote_bd_addr);

    b = gatt_eatt_alloc_bearer(ec, lcid);
    if (b == NULL) {
        /* No free bearer slot: tear down the L2CAP channel instead of leaving it
         * up untracked by GATT. */
        GATT_EATT_TRACE_ERROR("connect_cfm: no bearer slot lcid=0x%04x, disconnecting", lcid);
        gatt_eatt_setup_mark_done(ec);
        L2CA_LECocDisconnect(lcid);
        return;
    }

    GATT_EATT_TRACE_API("bearer connected lcid=0x%04x conn_id=%u (%u/%u)",
                        lcid, ec->conn_id, ec->bearer_count, s_eatt_chan_num);
    gatt_eatt_update_tcb_mtu(p_ccb->p_lcb->remote_bd_addr, lcid);
    b->reported = TRUE;
    gatt_eatt_report_evt(ec->conn_id, 0, lcid);

    gatt_eatt_setup_mark_done(ec);
}

static void gatt_eatt_disconnect_ind(UINT16 lcid, BOOLEAN local_init)
{
    tL2C_CCB *p_ccb = l2cu_find_ccb_by_cid(NULL, lcid);
    tGATT_EATT_CONN *ec;
    tGATT_EATT_BEARER *b;

    UNUSED(local_init);

    GATT_EATT_TRACE_DEBUG("disconnect_ind lcid=0x%04x local_init=%u", lcid, local_init);

    if (p_ccb == NULL || p_ccb->p_lcb == NULL) {
        GATT_EATT_TRACE_ERROR("disconnect_ind: no ccb/lcb lcid=0x%04x", lcid);
        return;
    }

    ec = gatt_eatt_find_conn_by_bda(p_ccb->p_lcb->remote_bd_addr);
    if (ec == NULL) {
        GATT_EATT_TRACE_DEBUG("disconnect_ind: no conn for lcid=0x%04x", lcid);
        return;
    }

    b = gatt_eatt_find_bearer(ec, lcid);
    if (b == NULL) {
        GATT_EATT_TRACE_DEBUG("disconnect_ind: no bearer for lcid=0x%04x", lcid);
    }
    if (b) {
#if (GATTC_INCLUDED == TRUE)
        if (b->client_op != 0 && b->clcb_idx != 0) {
            tGATT_CLCB *p_clcb = gatt_clcb_find_by_idx(b->clcb_idx);

            if (p_clcb != NULL) {
                btu_stop_timer(&p_clcb->rsp_timer_ent);
                gatt_end_operation(p_clcb, GATT_ERROR, NULL);
            }
        }
#endif
        gatt_eatt_free_bearer(ec, b);
    }
}

static void gatt_eatt_congestion(UINT16 lcid, BOOLEAN congested)
{
    /* Deliberately a no-op on EATT decongestion. EATT client commands are never
     * queued in cl_cmd_q: attp_cl_send_cmd() sends them straight to L2CAP, which
     * owns their credit-based flow control. cl_cmd_q only holds legacy commands
     * bound to the ATT fixed channel, and its resend is driven solely by the ATT
     * fixed-channel congestion path (gatt_channel_congestion) and the response
     * handler (gatt_client_handle_server_rsp). Calling gatt_cl_send_next_cmd_inq
     * here would flush that legacy queue onto the ATT fixed channel with
     * eatt_tx/rx_bearer == 0; if that channel were still congested the commands
     * would be dropped as GATT_BUSY (permanently dequeued + failed). */
    UNUSED(lcid);
    UNUSED(congested);
}

static void gatt_eatt_data_ind_l2c(UINT16 lcid, BT_HDR *p_buf)
{
    gatt_eatt_data_ind(lcid, p_buf);
}

void gatt_eatt_init(void)
{
    tBT_UUID app_uuid = {LEN_UUID_16, {UUID_SERVCLASS_GATT_SERVER}};
    static const tGATT_CBACK s_eatt_gatt_cback = {
        gatt_eatt_gatt_conn_cback,
#if (BLE_EATT_CLIENT_INCLUDED == TRUE)
        gatt_eatt_gatt_cmpl_cback,
#else
        NULL,
#endif
        NULL,                       /* p_disc_res_cb  */
        NULL,                       /* p_disc_cmpl_cb */
        gatt_eatt_gatt_req_cback,   /* p_req_cb: no-op, absorbs server req fan-out */
        NULL,                       /* p_enc_cmpl_cb  */
        NULL,                       /* p_congestion_cb */
    };

    memset(s_eatt_conn, 0, sizeof(s_eatt_conn));
    memset(&s_eatt_l2cap_appl, 0, sizeof(s_eatt_l2cap_appl));

#if (BLE_EATT_SERVER_INCLUDED == TRUE)
    s_eatt_l2cap_appl.pL2CA_ConnectInd_Cb = gatt_eatt_connect_ind;
#endif
    s_eatt_l2cap_appl.pL2CA_ConnectCfm_Cb = gatt_eatt_connect_cfm;
    s_eatt_l2cap_appl.pL2CA_DisconnectInd_Cb = gatt_eatt_disconnect_ind;
    s_eatt_l2cap_appl.pL2CA_DataInd_Cb = gatt_eatt_data_ind_l2c;
    s_eatt_l2cap_appl.pL2CA_CongestionStatus_Cb = gatt_eatt_congestion;

    s_eatt_reg_psm = L2CA_RegisterLECoc(GATT_EATT_PSM, &s_eatt_l2cap_appl);
    if (s_eatt_reg_psm == 0) {
        /* Bail out before GATT_Register so a failed EATT init does not consume a
         * scarce GATT_MAX_APPS slot. With no PSM, gatt_eatt_on_encrypted() also
         * returns early, so nothing allocates an EATT conn we could not free. */
        GATT_EATT_TRACE_ERROR("RegisterLECoc PSM 0x%04x failed, EATT disabled", GATT_EATT_PSM);
        return;
    }
    GATT_EATT_TRACE_DEBUG("RegisterLECoc success reg_psm=0x%04x", s_eatt_reg_psm);

    BTM_SetSecurityLevel(TRUE, "GATT_EATT", BTM_SEC_SERVICE_GEN_NET,
                         BTM_SEC_NONE, s_eatt_reg_psm, BTM_SEC_PROTO_L2CAP, 0);
    BTM_SetSecurityLevel(FALSE, "GATT_EATT", BTM_SEC_SERVICE_GEN_NET,
                         BTM_SEC_NONE, s_eatt_reg_psm, BTM_SEC_PROTO_L2CAP, 0);
    GATT_EATT_TRACE_API("init BTM_SetSecurityLevel for PSM 0x%04x", s_eatt_reg_psm);

    s_eatt_gatt_if = GATT_Register(&app_uuid, &s_eatt_gatt_cback);
    if (s_eatt_gatt_if != 0) {
        GATT_EATT_TRACE_DEBUG("GATT_Register success gatt_if=%u", s_eatt_gatt_if);
        GATT_StartIf(s_eatt_gatt_if);
    } else {
        GATT_EATT_TRACE_ERROR("GATT_Register failed for EATT module");
        /* Roll back the L2CAP PSM registration. Without a GATT interface the
         * gatt_eatt_gatt_conn_cback that frees tGATT_EATT_CONN on BLE disconnect
         * is never registered, so leaving the PSM (and its still-live callbacks)
         * up would let the server accept EATT connections it can never clean up,
         * leaking an s_eatt_conn[] slot per peer. */
        L2CA_DeregisterLECoc(s_eatt_reg_psm);
        s_eatt_reg_psm = 0;
    }

#if GATTS_ROBUST_CACHING_ENABLED
    if (s_eatt_reg_psm != 0) {
        gatt_cb.gatt_sr_supported_feat_mask |= BLE_GATT_SR_SUPP_FEAT_EATT_BITMASK;
    }
#endif

    GATT_EATT_TRACE_API("init reg_psm=0x%04x eatt_gatt_if=%u chan_num=%u mtu=%u robust_caching=%d",
                        s_eatt_reg_psm, s_eatt_gatt_if, s_eatt_chan_num, GATT_EATT_MTU,
                        GATTS_ROBUST_CACHING_ENABLED);
}

void gatt_eatt_deinit(void)
{
    tGATT_EATT_EVT_CBACK *saved_cback = s_eatt_evt_cback;

    GATT_EATT_TRACE_DEBUG("deinit start reg_psm=0x%04x gatt_if=%u", s_eatt_reg_psm, s_eatt_gatt_if);

    /* Suppress app callbacks while tearing down bearers during stack disable. */
    s_eatt_evt_cback = NULL;

    /* Explicitly disconnect every EATT L2CAP channel first. gatt_eatt_free_bearer
     * only clears internal state, and the L2CA_DeregisterLECoc call below walks
     * each link but disconnects only the first CCB in its queue. With multiple
     * bearers per link the rest would be orphaned (left open until the ACL drops).
     * Send the disconnect here, before free_conn wipes the bearer LCIDs. */
    for (int i = 0; i < MAX_L2CAP_LINKS; i++) {
        if (!s_eatt_conn[i].in_use) {
            continue;
        }
        GATT_EATT_TRACE_DEBUG("deinit disconnect bearers for conn_id=%u count=%u",
                              s_eatt_conn[i].conn_id, s_eatt_conn[i].bearer_count);
        for (int j = 0; j < GATT_EATT_MAX_CHAN; j++) {
            if (s_eatt_conn[i].bearers[j].in_use) {
                L2CA_LECocDisconnect(s_eatt_conn[i].bearers[j].lcid);
            }
        }
    }

    for (int i = 0; i < MAX_L2CAP_LINKS; i++) {
        if (s_eatt_conn[i].in_use) {
            gatt_eatt_free_conn(&s_eatt_conn[i]);
        }
    }

    if (s_eatt_reg_psm != 0) {
        GATT_EATT_TRACE_DEBUG("deinit DeregisterLECoc reg_psm=0x%04x", s_eatt_reg_psm);
        L2CA_DeregisterLECoc(s_eatt_reg_psm);
        s_eatt_reg_psm = 0;
    }

    /* Mirror the GATT_Register/GATT_StartIf done in init: GATT_Deregister stops
     * CLCB response timers, frees CLCBs, updates link-use flags and releases the
     * cl_rcb slot. Skipping it would leak the registration slot and could leave a
     * timer referencing a CLCB later freed by gatt_free(). */
    if (s_eatt_gatt_if != 0) {
        GATT_EATT_TRACE_DEBUG("deinit GATT_Deregister gatt_if=%u", s_eatt_gatt_if);
        GATT_Deregister(s_eatt_gatt_if);
        s_eatt_gatt_if = 0;
    }

#if GATTS_ROBUST_CACHING_ENABLED
    gatt_cb.gatt_sr_supported_feat_mask &= ~BLE_GATT_SR_SUPP_FEAT_EATT_BITMASK;
#endif

    UNUSED(saved_cback);
    memset(&s_eatt_l2cap_appl, 0, sizeof(s_eatt_l2cap_appl));
    GATT_EATT_TRACE_DEBUG("deinit done");
}

void gatt_eatt_register_evt_cback(tGATT_EATT_EVT_CBACK *p_cback)
{
    s_eatt_evt_cback = p_cback;
}

void gatt_eatt_set_chan_num(UINT8 num_chan)
{
    if (num_chan > 0 && num_chan <= GATT_EATT_MAX_CHAN) {
        s_eatt_chan_num = num_chan;
    }
}

void gatt_eatt_on_encrypted(BD_ADDR bd_addr)
{
    tGATT_EATT_CONN *ec;
    UINT16 stack_conn_id;
    tL2C_LCB *p_lcb;
    tBTM_SEC_DEV_REC *p_dev;
    BOOLEAN is_central;

    gatt_eatt_log_peer("on_encrypted", bd_addr);

    if (!gatt_eatt_is_encrypted(bd_addr)) {
        GATT_EATT_TRACE_API("on_encrypted: link not encrypted yet, skip");
        return;
    }

    /* EATT is not operational unless its L2CAP PSM was registered in
     * gatt_eatt_init(). Without it no bearer can ever be set up, and the GATT
     * conn cback that frees tGATT_EATT_CONN on disconnect may not be registered
     * either, so allocating a conn here would only leak an s_eatt_conn[] slot. */
    if (s_eatt_reg_psm == 0) {
        GATT_EATT_TRACE_API("on_encrypted: EATT PSM not registered, skip");
        return;
    }

    ec = gatt_eatt_find_conn_by_bda(bd_addr);
    if (ec != NULL && (ec->bearer_count > 0 || ec->setup_st != GATT_EATT_ST_IDLE)) {
        /* Skip if bearers are already up OR a setup is in progress, so a repeated
         * encryption-complete notification cannot start a duplicate EATT setup. */
        GATT_EATT_TRACE_API("on_encrypted: setup in progress or %u bearers up, skip",
                            ec->bearer_count);
        return;
    }

    ec = gatt_eatt_alloc_conn(bd_addr);
    if (ec == NULL) {
        GATT_EATT_TRACE_ERROR("on_encrypted: alloc ec failed");
        return;
    }

    ec->conn_id = gatt_eatt_app_conn_id_from_bda(bd_addr);
    if (ec->conn_id == GATT_INVALID_CONN_ID) {
        GATT_EATT_TRACE_ERROR("on_encrypted: conn_id invalid conn_id=0x%04x", ec->conn_id);
    }
    stack_conn_id = gatt_eatt_stack_conn_id_from_bda(bd_addr);
    is_central = gatt_eatt_is_central(bd_addr);

    p_lcb = l2cu_find_lcb_by_bd_addr(bd_addr, BT_TRANSPORT_LE);
    p_dev = btm_find_dev(bd_addr);
    GATT_EATT_TRACE_DEBUG("on_encrypted: central=%u app_conn_id=%u stack_conn_id=0x%04x "
                          "eatt_gatt_if=%u lcb=%p state=%u role=%u btm_role_master=%u",
                        is_central, ec->conn_id, stack_conn_id, s_eatt_gatt_if,
                        (void *)p_lcb,
                        p_lcb ? p_lcb->link_state : 0xFF,
                        p_lcb ? p_lcb->link_role : 0xFF,
                        (p_dev && p_dev->role_master) ? 1 : 0);

    if (is_central) {
#if (BLE_EATT_CLIENT_INCLUDED == TRUE)
#if GATTS_ROBUST_CACHING_ENABLED
        if (stack_conn_id != 0) {
            gatt_eatt_central_read_sr_feat(stack_conn_id, ec);
        } else {
            GATT_EATT_TRACE_ERROR("on_encrypted: no stack conn_id, fallback legacy");
            gatt_eatt_fallback_legacy(ec);
        }
#else
        GATT_EATT_TRACE_API("on_encrypted: robust caching off, start ecoc directly");
        gatt_eatt_start_ecoc_connect(bd_addr);
#endif
#else
        GATT_EATT_TRACE_API("on_encrypted: central but GATTC disabled, skip EATT setup");
        gatt_eatt_free_conn(ec);
#endif
    } else {
#if (BLE_EATT_SERVER_INCLUDED == TRUE)
        GATT_EATT_TRACE_API("on_encrypted: peripheral, wait central ECFC on PSM 0x%04x",
                            GATT_EATT_PSM);
#else
        /* No ConnectInd cb is registered when the EATT server is disabled, so this
         * pre-allocated conn can never receive an incoming bearer; free it instead
         * of holding an s_eatt_conn[] slot until GATT disconnect. */
        gatt_eatt_free_conn(ec);
#endif
    }
}

BOOLEAN gatt_eatt_is_bearer(UINT16 lcid)
{
    tL2C_CCB *p_ccb = l2cu_find_ccb_by_cid(NULL, lcid);
    if (p_ccb == NULL || p_ccb->p_lcb == NULL || p_ccb->p_rcb == NULL) {
        return FALSE;
    }
    return (p_ccb->p_rcb->real_psm == GATT_EATT_PSM && p_ccb->le_coc_active);
}

UINT16 gatt_eatt_get_available_bearer(BD_ADDR bd_addr, UINT8 op)
{
#if (BLE_EATT_CLIENT_INCLUDED == TRUE)
    tGATT_EATT_CONN *ec = gatt_eatt_find_conn_by_bda(bd_addr);
    tGATT_EATT_BEARER *b;

    UNUSED(op);

    if (ec == NULL || ec->bearer_count == 0) {
        return L2CAP_ATT_CID;
    }

    if (ec->default_lcid != 0) {
        b = gatt_eatt_find_bearer(ec, ec->default_lcid);
        if (b != NULL && b->client_op == 0) {
            GATT_EATT_TRACE_DEBUG("get_available_bearer op=0x%02x -> default lcid=0x%04x", op, ec->default_lcid);
            return ec->default_lcid;
        }
    }

    for (int i = 0; i < GATT_EATT_MAX_CHAN; i++) {
        if (ec->bearers[i].in_use && ec->bearers[i].client_op == 0) {
            GATT_EATT_TRACE_DEBUG("get_available_bearer op=0x%02x -> lcid=0x%04x", op, ec->bearers[i].lcid);
            return ec->bearers[i].lcid;
        }
    }
    GATT_EATT_TRACE_DEBUG("no free EATT bearer op=0x%02x (%u busy), use ATT CID", op, ec->bearer_count);
#endif
    UNUSED(bd_addr);
    UNUSED(op);
    return L2CAP_ATT_CID;
}

#if (BLE_EATT_SERVER_INCLUDED == TRUE)
UINT16 gatt_eatt_get_server_tx_bearer(BD_ADDR bd_addr)
{
    tGATT_EATT_CONN *ec = gatt_eatt_find_conn_by_bda(bd_addr);

    if (ec == NULL || ec->bearer_count == 0) {
        return L2CAP_ATT_CID;
    }

    /* Honor an app-selected default bearer when present. */
    if (ec->default_lcid != 0 &&
        gatt_eatt_find_bearer(ec, ec->default_lcid) != NULL) {
        GATT_EATT_TRACE_DEBUG("server_tx_bearer -> default lcid=0x%04x", ec->default_lcid);
        return ec->default_lcid;
    }

    /* Round-robin across in-use bearers so server-initiated notifications and
     * indications are spread over all EATT channels rather than serialized on
     * a single one. */
    for (int n = 0; n < GATT_EATT_MAX_CHAN; n++) {
        int i = (ec->tx_rr + n) % GATT_EATT_MAX_CHAN;
        if (ec->bearers[i].in_use) {
            ec->tx_rr = (UINT8)((i + 1) % GATT_EATT_MAX_CHAN);
            GATT_EATT_TRACE_DEBUG("server_tx_bearer -> rr lcid=0x%04x", ec->bearers[i].lcid);
            return ec->bearers[i].lcid;
        }
    }

    return L2CAP_ATT_CID;
}
#endif /* BLE_EATT_SERVER_INCLUDED */

BOOLEAN gatt_eatt_set_default_bearer(UINT16 conn_id, UINT16 lcid)
{
#if (BLE_EATT_CLIENT_INCLUDED == TRUE)
    /* conn_id here is the application-level id, which ESP exposes as the raw
     * tcb_idx (see gatt_eatt_app_conn_id_from_bda / esp_ble_eatt_set_default_bearer).
     * Do NOT apply GATT_GET_TCB_IDX (a >>8 for stack-layer conn_ids), which would
     * evaluate to 0 for every application conn_id. */
    tGATT_TCB *p_tcb = gatt_get_tcb_by_idx((UINT8)conn_id);
    tGATT_EATT_CONN *ec;
    tGATT_EATT_BEARER *b;

    if (p_tcb == NULL) {
        GATT_EATT_TRACE_ERROR("set_default_bearer: no tcb for conn_id=%u", conn_id);
        return FALSE;
    }

    ec = gatt_eatt_find_conn_by_bda(p_tcb->peer_bda);
    if (ec == NULL) {
        GATT_EATT_TRACE_ERROR("set_default_bearer: no EATT conn for conn_id=%u", conn_id);
        return FALSE;
    }

    if (lcid == 0 || lcid == L2CAP_ATT_CID) {
        GATT_EATT_TRACE_DEBUG("set_default_bearer: clear default for conn_id=%u", conn_id);
        ec->default_lcid = 0;
        return TRUE;
    }

    b = gatt_eatt_find_bearer(ec, lcid);
    if (b == NULL) {
        GATT_EATT_TRACE_ERROR("set_default_bearer: lcid=0x%04x not an EATT bearer of conn_id=%u", lcid, conn_id);
        return FALSE;
    }

    GATT_EATT_TRACE_DEBUG("set_default_bearer conn_id=%u lcid=0x%04x", conn_id, lcid);
    ec->default_lcid = lcid;
    return TRUE;
#else
    UNUSED(conn_id);
    UNUSED(lcid);
    return FALSE;
#endif
}

BOOLEAN gatt_eatt_mark_busy(BD_ADDR bd_addr, UINT16 lcid, UINT8 op, UINT16 clcb_idx)
{
#if (BLE_EATT_CLIENT_INCLUDED == TRUE)
    tGATT_EATT_BEARER *b;
    tGATT_EATT_CONN *ec = gatt_eatt_find_conn_by_bda(bd_addr);

    if (ec == NULL) {
        return FALSE;
    }
    b = gatt_eatt_find_bearer(ec, lcid);
    if (b == NULL || b->client_op != 0) {
        return FALSE;
    }
    b->client_op = op;
    b->clcb_idx = clcb_idx;
    return TRUE;
#else
    UNUSED(bd_addr);
    UNUSED(lcid);
    UNUSED(op);
    UNUSED(clcb_idx);
    return FALSE;
#endif
}

BOOLEAN gatt_eatt_release_bearer(BD_ADDR bd_addr, UINT16 lcid, UINT8 *p_op, UINT16 *p_clcb_idx)
{
#if (BLE_EATT_CLIENT_INCLUDED == TRUE)
    tGATT_EATT_BEARER *b;
    tGATT_EATT_CONN *ec = gatt_eatt_find_conn_by_bda(bd_addr);

    if (ec == NULL) {
        return FALSE;
    }
    b = gatt_eatt_find_bearer(ec, lcid);
    if (b == NULL || b->client_op == 0) {
        return FALSE;
    }
    if (p_op) {
        *p_op = b->client_op;
    }
    if (p_clcb_idx) {
        *p_clcb_idx = b->clcb_idx;
    }
    b->client_op = 0;
    b->clcb_idx = 0;
    return TRUE;
#else
    UNUSED(bd_addr);
    UNUSED(lcid);
    UNUSED(p_op);
    UNUSED(p_clcb_idx);
    return FALSE;
#endif
}

BOOLEAN gatt_eatt_release_bearer_by_clcb(BD_ADDR bd_addr, UINT16 clcb_idx)
{
#if (BLE_EATT_CLIENT_INCLUDED == TRUE)
    tGATT_EATT_CONN *ec = gatt_eatt_find_conn_by_bda(bd_addr);
    int i;

    if (ec == NULL || clcb_idx == 0) {
        return FALSE;
    }
    /* bearers[] is a sparse slot array: freeing a bearer only decrements
     * bearer_count without compacting, so an in-use bearer may live at any
     * index. Iterate the full slot range (matches every other loop here). */
    for (i = 0; i < GATT_EATT_MAX_CHAN; i++) {
        tGATT_EATT_BEARER *b = &ec->bearers[i];
        if (b->in_use && b->client_op != 0 && b->clcb_idx == clcb_idx) {
            b->client_op = 0;
            b->clcb_idx = 0;
            return TRUE;
        }
    }
    return FALSE;
#else
    UNUSED(bd_addr);
    UNUSED(clcb_idx);
    return FALSE;
#endif
}

void gatt_eatt_data_ind(UINT16 lcid, BT_HDR *p_buf)
{
    tL2C_CCB *p_ccb;
    tGATT_TCB *p_tcb;
    UINT8 *p;
    UINT8 op_code;

    if (p_buf == NULL) {
        return;
    }

    p_ccb = l2cu_find_ccb_by_cid(NULL, lcid);
    if (p_ccb == NULL || p_ccb->p_lcb == NULL) {
        osi_free(p_buf);
        return;
    }

    if (!gatt_eatt_is_encrypted(p_ccb->p_lcb->remote_bd_addr)) {
        GATT_EATT_TRACE_ERROR("ATT PDU on EATT before encryption, disconnect");
        L2CA_LECocDisconnect(lcid);
        osi_free(p_buf);
        return;
    }

    if (p_buf->len < 1) {
        L2CA_LECocDisconnect(lcid);
        osi_free(p_buf);
        return;
    }

    p = (UINT8 *)(p_buf + 1) + p_buf->offset;
    STREAM_TO_UINT8(op_code, p);
    if (!gatt_is_valid_att_opcode(op_code)) {
        GATT_EATT_TRACE_ERROR("invalid ATT opcode 0x%02x on EATT, disconnect", op_code);
        L2CA_LECocDisconnect(lcid);
        osi_free(p_buf);
        return;
    }

    if (op_code == GATT_SIGN_CMD_WRITE) {
        GATT_EATT_TRACE_ERROR("signed write on EATT bearer, disconnect lcid=0x%04x", lcid);
        L2CA_LECocDisconnect(lcid);
        osi_free(p_buf);
        return;
    }

    p_tcb = gatt_find_tcb_by_addr(p_ccb->p_lcb->remote_bd_addr, BT_TRANSPORT_LE);
    if (p_tcb == NULL || gatt_get_ch_state(p_tcb) < GATT_CH_OPEN) {
        osi_free(p_buf);
        return;
    }

    p_tcb->eatt_rx_bearer = lcid;

    if ((op_code % 2) != 0) {
#if (GATTC_INCLUDED == TRUE)
        /* ATT response on client bearer */
        gatt_client_handle_server_rsp(p_tcb, op_code, p_buf->len - 1, p, lcid);
        osi_free(p_buf);
#else
        GATT_EATT_TRACE_ERROR("ATT response on EATT but GATTC disabled, disconnect");
        L2CA_LECocDisconnect(lcid);
        osi_free(p_buf);
        p_tcb->eatt_rx_bearer = 0;
        return;
#endif
    } else {
        /* Client->server request on an EATT bearer. Record the bearer so a
         * deferred (GATT_PENDING) app response is routed back to it once
         * eatt_rx_bearer is cleared below. Cleared on gatt_dequeue_sr_cmd. */
#if (GATTS_INCLUDED == TRUE)
        /* Only record the routing hint when the sr_cmd slot is free. If a prior
         * request is still pending (op_code != 0), gatt_server_handle_client_req
         * discards this PDU without touching sr_cmd, so overwriting eatt_lcid here
         * would misroute the already-pending response to this bearer. */
        BOOLEAN sr_cmd_was_empty = (p_tcb->sr_cmd.op_code == 0);
        if (sr_cmd_was_empty) {
            p_tcb->sr_cmd.eatt_lcid = lcid;
        }
        gatt_data_process(p_tcb, p_buf);
        /* If the request was fully handled synchronously (or needed no response,
         * e.g. a write command), no sr_cmd is pending (op_code == 0). Clear the
         * routing hint so it cannot misroute a later, unrelated response. */
        if (sr_cmd_was_empty && p_tcb->sr_cmd.op_code == 0) {
            p_tcb->sr_cmd.eatt_lcid = 0;
        }
#else
        gatt_data_process(p_tcb, p_buf);
#endif
    }

    p_tcb->eatt_rx_bearer = 0;
    /* EATT bearers run in auto-credit mode, where l2c_ble_le_coc_data_ind() has
     * already returned the RX credit for this K-frame. This call is therefore a
     * no-op today (l2c_ble_le_coc_give_credits() ignores it while auto-credit is
     * on); it is kept only as a safety net should EATT ever switch to manual
     * credit management. */
    L2CA_LECocGiveCredits(lcid, 1);
}

#endif /* BLE_EATT_INCLUDED == TRUE */

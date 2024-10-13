/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "osi/osi.h"
#include "osi/allocator.h"
#include "common/bt_target.h"

#include "stack/l2c_api.h"
#include "stack/l2cdefs.h"
#include "stack/btm_api.h"
#include "btm_int.h"
#include "obex_tl.h"
#include "obex_tl_l2cap.h"

#if (OBEX_INCLUDED == TRUE)

#define OBEX_TL_L2CAP_NUM_CONN          4
#define OBEX_TL_L2CAP_NUM_SERVER        2

#define OBEX_TL_L2CAP_STATUS_FLAG_CFG_DONE          0x01
#define OBEX_TL_L2CAP_STATUS_FLAG_PEER_CFG_DONE     0x02
#define OBEX_TL_L2CAP_STATUS_FLAG_CONNECTED         0x80

/* ERTM Tx window size */
#define OBEX_TL_L2CAP_FCR_OPT_TX_WINDOW_SIZE    10

/* ERTM Maximum transmissions before disconnecting */
#define OBEX_TL_L2CAP_FCR_OPT_MAX_TX_B4_DISCNT  20

/* ERTM Retransmission timeout (2 secs) */
#define OBEX_TL_L2CAP_FCR_OPT_RETX_TOUT         2000

/* ERTM Monitor timeout (12 secs) */
#define OBEX_TL_L2CAP_FCR_OPT_MONITOR_TOUT      12000

/* ERTM ERTM MPS segment size */
#define OBEX_TL_L2CAP_FCR_OPT_MAX_PDU_SIZE      1010

typedef struct {
    UINT16      vpsm;               /* psm in our side */
    UINT16      lcid;               /* local channel id */
    UINT16      peer_mtu;           /* MTU that peer device set */
    UINT16      our_mtu;            /* MTU that we set to peer device */
    BOOLEAN     initiator;          /* TRUE if is initiator, otherwise FALSE */
    UINT8       id;                 /* remote channel id */
    UINT8       status_flag;        /* status flag used in config procedure */
    UINT8       allocated;          /* 0 if not allocated, otherwise, index + 1, equal to handle */
    BD_ADDR     addr;               /* peer bluetooth device address */
} tOBEX_TL_L2CAP_CCB;

typedef struct {
    UINT16      psm;                /* psm in our side */
    UINT16      pref_mtu;           /* preferred mtu */
    UINT8       allocated;          /* 0 if not allocated, otherwise, index + 1, handle of server will left shift 8 bits */
} tOBEX_TL_L2CAP_SCB;

typedef struct {
    tOBEX_TL_CBACK      *callback;                          /* Upper layer callback */
    tOBEX_TL_L2CAP_CCB  ccb[OBEX_TL_L2CAP_NUM_CONN];
    tOBEX_TL_L2CAP_SCB  scb[OBEX_TL_L2CAP_NUM_SERVER];
    tL2CAP_APPL_INFO    l2cap_reg_info;                     /* Default L2CAP Registration info */
    UINT8               trace_level;                        /* trace level */
} tOBEX_TL_L2CAP_CB;

#if OBEX_DYNAMIC_MEMORY == FALSE
static tOBEX_TL_L2CAP_CB obex_tl_l2cap_cb;
#else
static tOBEX_TL_L2CAP_CB *obex_tl_l2cap_cb_ptr = NULL;
#define obex_tl_l2cap_cb (*obex_tl_l2cap_cb_ptr)
#endif

static tL2CAP_ERTM_INFO obex_tl_l2cap_etm_opts =
{
    L2CAP_FCR_ERTM_MODE,
    L2CAP_FCR_CHAN_OPT_ERTM | L2CAP_FCR_CHAN_OPT_BASIC,     /* Some devices do not support ERTM */
    L2CAP_USER_RX_BUF_SIZE,
    L2CAP_USER_TX_BUF_SIZE,
    L2CAP_FCR_RX_BUF_SIZE,
    L2CAP_FCR_TX_BUF_SIZE
};

static tL2CAP_FCR_OPTS obex_tl_l2cap_fcr_opts =
{
    L2CAP_FCR_ERTM_MODE,
    OBEX_TL_L2CAP_FCR_OPT_TX_WINDOW_SIZE,     /* Tx window size */
    OBEX_TL_L2CAP_FCR_OPT_MAX_TX_B4_DISCNT,   /* Maximum transmissions before disconnecting */
    OBEX_TL_L2CAP_FCR_OPT_RETX_TOUT,          /* Retransmission timeout (2 secs) */
    OBEX_TL_L2CAP_FCR_OPT_MONITOR_TOUT,       /* Monitor timeout (12 secs) */
    OBEX_TL_L2CAP_FCR_OPT_MAX_PDU_SIZE        /* MPS segment size */
};

static tOBEX_TL_L2CAP_CCB *allocate_ccb(void)
{
    tOBEX_TL_L2CAP_CCB *p_ccb = NULL;
    for(int i = 0; i < OBEX_TL_L2CAP_NUM_CONN; ++i) {
        if (obex_tl_l2cap_cb.ccb[i].allocated == 0) {
            obex_tl_l2cap_cb.ccb[i].allocated = i + 1;
            p_ccb = &obex_tl_l2cap_cb.ccb[i];
            break;
        }
    }
    return p_ccb;
}

static void free_ccb(tOBEX_TL_L2CAP_CCB *p_ccb)
{
    memset(p_ccb, 0, sizeof(tOBEX_TL_L2CAP_CCB));
}

static tOBEX_TL_L2CAP_CCB *find_ccb_by_lcid(UINT16 lcid)
{
    tOBEX_TL_L2CAP_CCB *p_ccb = NULL;
    for (int i = 0; i < OBEX_TL_L2CAP_NUM_CONN; ++i) {
        if (obex_tl_l2cap_cb.ccb[i].allocated && obex_tl_l2cap_cb.ccb[i].lcid == lcid) {
            p_ccb = &obex_tl_l2cap_cb.ccb[i];
            break;
        }
    }
    return p_ccb;
}

static tOBEX_TL_L2CAP_CCB *find_ccb_by_hdl(UINT16 hdl)
{
    tOBEX_TL_L2CAP_CCB *p_ccb = NULL;
    if (hdl > 0 && hdl <= OBEX_TL_L2CAP_NUM_CONN) {
        if (obex_tl_l2cap_cb.ccb[hdl-1].allocated == hdl) {
            p_ccb = &obex_tl_l2cap_cb.ccb[hdl-1];
        }
    }
    return p_ccb;
}

static tOBEX_TL_L2CAP_CCB *find_ccb_by_psm(UINT16 psm)
{
    tOBEX_TL_L2CAP_CCB *p_ccb = NULL;
    for(int i = 0; i < OBEX_TL_L2CAP_NUM_CONN; ++i) {
        if (obex_tl_l2cap_cb.ccb[i].allocated && obex_tl_l2cap_cb.ccb[i].vpsm == psm) {
            p_ccb = &obex_tl_l2cap_cb.ccb[i];
            break;
        }
    }
    return p_ccb;
}

static tOBEX_TL_L2CAP_SCB *allocate_scb(void)
{
    tOBEX_TL_L2CAP_SCB *p_scb = NULL;
    for(int i = 0; i < OBEX_TL_L2CAP_NUM_SERVER; ++i) {
        if (obex_tl_l2cap_cb.scb[i].allocated == 0) {
            obex_tl_l2cap_cb.scb[i].allocated = i + 1;
            p_scb = &obex_tl_l2cap_cb.scb[i];
            break;
        }
    }
    return p_scb;
}

static tOBEX_TL_L2CAP_SCB *find_scb_by_psm(UINT16 psm)
{
    tOBEX_TL_L2CAP_SCB *p_scb = NULL;
    for(int i = 0; i < OBEX_TL_L2CAP_NUM_SERVER; ++i) {
        if (obex_tl_l2cap_cb.scb[i].allocated && obex_tl_l2cap_cb.scb[i].psm == psm) {
            p_scb = &obex_tl_l2cap_cb.scb[i];
            break;
        }
    }
    return p_scb;
}

static void free_scb(tOBEX_TL_L2CAP_SCB *p_scb)
{
    memset(p_scb, 0, sizeof(tOBEX_TL_L2CAP_SCB));
}

static tOBEX_TL_L2CAP_SCB *find_scb_by_hdl(UINT16 hdl)
{
    tOBEX_TL_L2CAP_SCB *p_scb = NULL;
    hdl = hdl >> 8;
    if (hdl > 0 && hdl <= OBEX_TL_L2CAP_NUM_SERVER) {
        if (obex_tl_l2cap_cb.scb[hdl-1].allocated == hdl) {
            p_scb = &obex_tl_l2cap_cb.scb[hdl-1];
        }
    }
    return p_scb;
}

/*******************************************************************************
 *
 * Function         obex_tl_l2cap_sec_check_complete_term
 *
 * Description      OBEX over L2CAP security check complete callback function
 *                  (terminated).
 *
 ******************************************************************************/
static void l2cap_sec_check_complete_term(BD_ADDR bd_addr, tBT_TRANSPORT transport, void *p_ref_data, UINT8 res)
{
    tOBEX_TL_L2CAP_CCB *p_ccb = (tOBEX_TL_L2CAP_CCB *)p_ref_data;

    if (res == BTM_SUCCESS) {
        L2CA_ErtmConnectRsp(p_ccb->addr, p_ccb->id, p_ccb->lcid, L2CAP_CONN_OK, 0, &obex_tl_l2cap_etm_opts);
        tL2CAP_CFG_INFO cfg = {0};
        cfg.mtu_present = TRUE;
        cfg.mtu = p_ccb->our_mtu;
        cfg.fcr_present = TRUE;
        cfg.fcr = obex_tl_l2cap_fcr_opts;
        L2CA_ConfigReq(p_ccb->lcid, &cfg);
    }
    else{
        L2CA_ErtmConnectRsp(p_ccb->addr, p_ccb->id, p_ccb->lcid, L2CAP_CONN_SECURITY_BLOCK, 0, &obex_tl_l2cap_etm_opts);
        tOBEX_TL_MSG msg = {0};
        msg.any.hdl = p_ccb->allocated;
        obex_tl_l2cap_cb.callback(OBEX_TL_DIS_CONN_EVT, &msg);
        free_ccb(p_ccb);
    }
}

/*******************************************************************************
 *
 * Function         obex_tl_l2cap_sec_check_complete_orig
 *
 * Description      OBEX over L2CAP security check complete callback function
 *                  (originated).
 *
 ******************************************************************************/
static void l2cap_sec_check_complete_orig(BD_ADDR bd_addr, tBT_TRANSPORT transport, void *p_ref_data, UINT8 res)
{
    tOBEX_TL_L2CAP_CCB *p_ccb = (tOBEX_TL_L2CAP_CCB *)p_ref_data;

    if (res == BTM_SUCCESS) {
        tL2CAP_CFG_INFO cfg = {0};
        cfg.mtu_present = TRUE;
        cfg.mtu = p_ccb->our_mtu;
        cfg.fcr_present = TRUE;
        cfg.fcr = obex_tl_l2cap_fcr_opts;
        L2CA_ConfigReq(p_ccb->lcid, &cfg);
    } else {
        L2CA_DisconnectReq(p_ccb->lcid);
        tOBEX_TL_MSG msg = {0};
        msg.any.hdl = p_ccb->allocated;
        obex_tl_l2cap_cb.callback(OBEX_TL_DIS_CONN_EVT, &msg);
        free_ccb(p_ccb);
    }
}

/*******************************************************************************
**
** Function         obex_tl_l2cap_connect_ind
**
** Description      This is a callback function called by L2CAP when
**                  L2CA_ConnectInd received.
**
*******************************************************************************/
void obex_tl_l2cap_connect_ind(BD_ADDR addr, UINT16 lcid, UINT16 psm, UINT8 id)
{
    tOBEX_TL_L2CAP_SCB *p_scb = find_scb_by_psm(psm);
    if (p_scb == NULL) {
        /* An incoming connection, but no corresponding server found, reject */
        L2CA_ErtmConnectRsp (addr, id, lcid, L2CAP_CONN_NO_PSM, 0, &obex_tl_l2cap_etm_opts);
        OBEX_TL_L2CAP_TRACE_WARNING("No corresponding server found, reject incoming connection\n");
        return;
    }

    tOBEX_TL_L2CAP_CCB *p_ccb = allocate_ccb();
    if (p_ccb == NULL) {
        /* No resource, can not allocate ccb, reject */
        L2CA_ErtmConnectRsp (addr, id, lcid, L2CAP_CONN_NO_RESOURCES, 0, &obex_tl_l2cap_etm_opts);
        OBEX_TL_L2CAP_TRACE_WARNING("Can not allocate a ccb for new connection\n");
        return;
    }

    bdcpy(p_ccb->addr, addr);
    p_ccb->initiator = FALSE;
    p_ccb->lcid = lcid;
    p_ccb->id = id;
    p_ccb->vpsm = psm;
    p_ccb->our_mtu = p_scb->pref_mtu;
    if (btm_sec_mx_access_request(p_ccb->addr, p_ccb->vpsm,
                              FALSE, BTM_SEC_PROTO_OBEX,
                              OBEX_TL_L2CAP_NUM_CONN + p_scb->allocated - 1,
                              &l2cap_sec_check_complete_term, p_ccb) == BTM_CMD_STARTED) {
        L2CA_ErtmConnectRsp(addr, id, lcid, L2CAP_CONN_PENDING, 0, &obex_tl_l2cap_etm_opts);
    }
}

/*******************************************************************************
**
** Function         obex_tl_l2cap_connect_cfm
**
** Description      This is a callback function called by L2CAP when
**                  ConnectCfm received.
**
*******************************************************************************/
void obex_tl_l2cap_connect_cfm(UINT16 lcid, UINT16 result)
{
    tOBEX_TL_L2CAP_CCB *p_ccb = find_ccb_by_lcid(lcid);
    if (p_ccb == NULL) {
        OBEX_TL_L2CAP_TRACE_ERROR("l2cap_connect_cfm but no ccb found\n");
        return;
    }

    if (result == L2CAP_CONN_OK) {
        btm_sec_mx_access_request(p_ccb->addr, p_ccb->vpsm,
                                  TRUE, BTM_SEC_PROTO_OBEX,
                                  p_ccb->allocated - 1,
                                  &l2cap_sec_check_complete_orig, p_ccb);
    } else {
        OBEX_TL_L2CAP_TRACE_WARNING("l2cap_connect_cfm result != L2CAP_CONN_OK: result: 0x%x\n", result);
        tOBEX_TL_MSG msg = {0};
        msg.any.hdl = p_ccb->allocated;
        obex_tl_l2cap_cb.callback(OBEX_TL_DIS_CONN_EVT, &msg);
        free_ccb(p_ccb);
    }
}


/*******************************************************************************
**
** Function         obex_tl_l2cap_config_ind
**
** Description      This is a callback function called by L2CAP when
**                  L2CA_ConfigInd received.
**
*******************************************************************************/
void  obex_tl_l2cap_config_ind(UINT16 lcid, tL2CAP_CFG_INFO *p_cfg)
{
    tOBEX_TL_L2CAP_CCB *p_ccb = find_ccb_by_lcid(lcid);
    if (p_ccb == NULL) {
        OBEX_TL_L2CAP_TRACE_ERROR("l2cap_config_ind but no ccb found\n");
        return;
    }

    tOBEX_TL_L2CAP_SCB *p_scb = find_scb_by_psm(p_ccb->vpsm);
    if (p_ccb->initiator == FALSE && p_scb == NULL) {
        /* not a initiator, but can not find corresponding server */
        OBEX_TL_L2CAP_TRACE_ERROR("l2cap_config_ind, not a initiator, but can not find corresponding server\n");
        return;
    }

    /* save peer mtu if present */
    UINT16 peer_mtu = L2CAP_DEFAULT_MTU;
    if (p_cfg->mtu_present) {
        peer_mtu = p_cfg->mtu;
    }

    p_cfg->mtu_present      = FALSE;
    p_cfg->flush_to_present = FALSE;
    p_cfg->qos_present      = FALSE;
    p_cfg->result = L2CAP_CFG_OK;
    L2CA_ConfigRsp(p_ccb->lcid, p_cfg);

    if (p_ccb->status_flag & OBEX_TL_L2CAP_STATUS_FLAG_CONNECTED) {
        /* reconfig l2cap channel */
        if (p_ccb->peer_mtu != peer_mtu) {
            /* only report to upper if mtu change */
            p_ccb->peer_mtu = peer_mtu;
            tOBEX_TL_MSG msg = {0};
            msg.mtu_chg.hdl = p_ccb->allocated;
            msg.mtu_chg.peer_mtu = peer_mtu;
            msg.mtu_chg.our_mtu = p_ccb->our_mtu;
            obex_tl_l2cap_cb.callback(OBEX_TL_MTU_CHANGE_EVT, &msg);
        }
        return;
    }

    p_ccb->peer_mtu = peer_mtu;
    p_ccb->status_flag |= OBEX_TL_L2CAP_STATUS_FLAG_PEER_CFG_DONE;
    if (p_ccb->status_flag & OBEX_TL_L2CAP_STATUS_FLAG_CFG_DONE) {
        p_ccb->status_flag |= OBEX_TL_L2CAP_STATUS_FLAG_CONNECTED;
    }

    if ((p_ccb->status_flag & OBEX_TL_L2CAP_STATUS_FLAG_CONNECTED)) {
        tOBEX_TL_MSG msg = {0};
        if (p_ccb->initiator) {
            msg.conn_open.hdl = p_ccb->allocated;
            msg.conn_open.peer_mtu = p_ccb->peer_mtu;
            msg.conn_open.our_mtu = p_ccb->our_mtu;
            obex_tl_l2cap_cb.callback(OBEX_TL_CONN_OPEN_EVT, &msg);
        }
        else {
            msg.conn_income.hdl = p_ccb->allocated;
            msg.conn_income.peer_mtu = p_ccb->peer_mtu;
            msg.conn_income.our_mtu = p_ccb->our_mtu;
            msg.conn_income.svr_hdl = p_scb->allocated << 8;
            obex_tl_l2cap_cb.callback(OBEX_TL_CONN_INCOME_EVT, &msg);
        }
    }
}


/*******************************************************************************
**
** Function         obex_tl_l2cap_config_cfm
**
** Description      This is a callback function called by L2CAP when
**                  L2CA_ConfigCnf received.
**
*******************************************************************************/
void obex_tl_l2cap_config_cfm(UINT16 lcid, tL2CAP_CFG_INFO *p_cfg)
{
    tOBEX_TL_L2CAP_CCB *p_ccb = find_ccb_by_lcid(lcid);
    if (p_ccb == NULL) {
        OBEX_TL_L2CAP_TRACE_ERROR("l2cap_config_cfm but no ccb found\n");
        return;
    }

    tOBEX_TL_L2CAP_SCB *p_scb = find_scb_by_psm(p_ccb->vpsm);
    if (p_ccb->initiator == FALSE && p_scb == NULL) {
        /* not a initiator, but can not find corresponding server */
        OBEX_TL_L2CAP_TRACE_ERROR("l2cap_config_cfm, not a initiator, but can not find corresponding server\n");
        return;
    }

    if (p_cfg->result != L2CAP_CFG_OK) {
        OBEX_TL_L2CAP_TRACE_WARNING("l2cap_config_cfm result != L2CAP_CFG_OK: result: 0x%x\n", p_cfg->result);
        L2CA_DisconnectReq(p_ccb->lcid);
        tOBEX_TL_MSG msg = {0};
        msg.any.hdl = p_ccb->allocated;
        obex_tl_l2cap_cb.callback(OBEX_TL_DIS_CONN_EVT, &msg);
        free_ccb(p_ccb);
        return;
    }

    p_ccb->status_flag |= OBEX_TL_L2CAP_STATUS_FLAG_CFG_DONE;
    if (p_ccb->status_flag & OBEX_TL_L2CAP_STATUS_FLAG_PEER_CFG_DONE) {
        p_ccb->status_flag |= OBEX_TL_L2CAP_STATUS_FLAG_CONNECTED;
    }

    if (p_ccb->status_flag & OBEX_TL_L2CAP_STATUS_FLAG_CONNECTED) {
        tOBEX_TL_MSG msg = {0};
        if (p_ccb->initiator) {
            msg.conn_open.hdl = p_ccb->allocated;
            msg.conn_open.peer_mtu = p_ccb->peer_mtu;
            msg.conn_open.our_mtu = p_ccb->our_mtu;
            obex_tl_l2cap_cb.callback(OBEX_TL_CONN_OPEN_EVT, &msg);
        }
        else {
            msg.conn_income.hdl = p_ccb->allocated;
            msg.conn_income.peer_mtu = p_ccb->peer_mtu;
            msg.conn_income.our_mtu = p_ccb->our_mtu;
            msg.conn_income.svr_hdl = p_scb->allocated << 8;
            obex_tl_l2cap_cb.callback(OBEX_TL_CONN_INCOME_EVT, &msg);
        }
    }
}


/*******************************************************************************
**
** Function         obex_tl_l2cap_qos_violation_ind
**
** Description      This is a callback function called by L2CAP when
**                  L2CA_QoSViolationIndInd received.
**
*******************************************************************************/
void obex_tl_l2cap_qos_violation_ind(BD_ADDR addr)
{
    UNUSED(addr);
}


/*******************************************************************************
**
** Function         obex_tl_l2cap_disconnect_ind
**
** Description      This is a callback function called by L2CAP when
**                  L2CA_DisconnectInd received.
**
*******************************************************************************/
void obex_tl_l2cap_disconnect_ind(UINT16 lcid, BOOLEAN is_conf_needed)
{
    tOBEX_TL_L2CAP_CCB *p_ccb = find_ccb_by_lcid(lcid);

    if (is_conf_needed) {
        L2CA_DisconnectRsp(lcid);
    }

    if (p_ccb == NULL) {
        OBEX_TL_L2CAP_TRACE_ERROR("l2cap_disconnect_ind but no ccb found\n");
        return;
    }

    tOBEX_TL_MSG msg = {0};
    msg.any.hdl = p_ccb->allocated;
    obex_tl_l2cap_cb.callback(OBEX_TL_DIS_CONN_EVT, &msg);
    free_ccb(p_ccb);
}


/*******************************************************************************
**
** Function         obex_tl_l2cap_buf_data_ind
**
** Description      This is a callback function called by L2CAP when
**                  data frame is received.
**
*******************************************************************************/
void obex_tl_l2cap_buf_data_ind(UINT16 lcid, BT_HDR *p_buf)
{
    tOBEX_TL_L2CAP_CCB *p_ccb = find_ccb_by_lcid(lcid);
    if (p_ccb == NULL) {
        OBEX_TL_L2CAP_TRACE_ERROR("l2cap_buf_data_ind but no ccb found\n");
        osi_free(p_buf);
        return;
    }

    tOBEX_TL_MSG msg = {0};
    msg.data.hdl = p_ccb->allocated;
    msg.data.p_buf = p_buf;
    obex_tl_l2cap_cb.callback(OBEX_TL_DATA_EVT, &msg);
}

/*******************************************************************************
**
** Function         obex_tl_l2cap_congestion_status_ind
**
** Description      This is a callback function called by L2CAP when
**                  L2CAP channel congestion status changes
**
*******************************************************************************/
void obex_tl_l2cap_congestion_status_ind(UINT16 lcid, BOOLEAN is_congested)
{
    tOBEX_TL_L2CAP_CCB *p_ccb = find_ccb_by_lcid(lcid);

    if (p_ccb == NULL) {
        OBEX_TL_L2CAP_TRACE_ERROR("l2cap_congestion_status_ind but no ccb found\n");
        return;
    }

    tOBEX_TL_MSG msg = {0};
    msg.any.hdl = p_ccb->allocated;
    if (is_congested) {
        obex_tl_l2cap_cb.callback(OBEX_TL_CONGEST_EVT, &msg);
    }
    else {
        obex_tl_l2cap_cb.callback(OBEX_TL_UNCONGEST_EVT, &msg);
    }
}

/*******************************************************************************
**
** Function         obex_tl_l2cap_init
**
** Description      Initialize OBEX over L2CAP transport layer, callback
**                  can not be NULL, must be called once before using any
**                  other APIs
**
*******************************************************************************/
void obex_tl_l2cap_init(tOBEX_TL_CBACK callback)
{
    assert(callback != NULL);
#if (OBEX_DYNAMIC_MEMORY)
    if (!obex_tl_l2cap_cb_ptr) {
        obex_tl_l2cap_cb_ptr = (tOBEX_TL_L2CAP_CB *)osi_malloc(sizeof(tOBEX_TL_L2CAP_CB));
        if (!obex_tl_l2cap_cb_ptr) {
            OBEX_TL_L2CAP_TRACE_ERROR("OBEX over L2CAP transport layer initialize failed, no memory\n");
            assert(0);
        }
    }
#endif /* #if (OBEX_DYNAMIC_MEMORY) */
    memset(&obex_tl_l2cap_cb, 0, sizeof(tOBEX_TL_L2CAP_CB));
    obex_tl_l2cap_cb.callback = callback;
    obex_tl_l2cap_cb.trace_level = BT_TRACE_LEVEL_ERROR;

    tL2CAP_APPL_INFO *p_reg_info = &obex_tl_l2cap_cb.l2cap_reg_info;

    p_reg_info->pL2CA_ConnectInd_Cb         = NULL;         /* obex_tl_l2cap_connect_ind or NULL, depend on server or not */
    p_reg_info->pL2CA_ConnectCfm_Cb         = obex_tl_l2cap_connect_cfm;
    p_reg_info->pL2CA_ConnectPnd_Cb         = NULL;
    p_reg_info->pL2CA_ConfigInd_Cb          = obex_tl_l2cap_config_ind;
    p_reg_info->pL2CA_ConfigCfm_Cb          = obex_tl_l2cap_config_cfm;
    p_reg_info->pL2CA_DisconnectInd_Cb      = obex_tl_l2cap_disconnect_ind;
    p_reg_info->pL2CA_DisconnectCfm_Cb      = NULL;
    p_reg_info->pL2CA_QoSViolationInd_Cb    = obex_tl_l2cap_qos_violation_ind;
    p_reg_info->pL2CA_DataInd_Cb            = obex_tl_l2cap_buf_data_ind;
    p_reg_info->pL2CA_CongestionStatus_Cb   = obex_tl_l2cap_congestion_status_ind;
    p_reg_info->pL2CA_TxComplete_Cb         = NULL;
}

/*******************************************************************************
**
** Function         obex_tl_l2cap_init
**
** Description      Deinitialize OBEX over L2CAP transport layer
**
*******************************************************************************/
void obex_tl_l2cap_deinit(void)
{
#if (OBEX_DYNAMIC_MEMORY)
    if (obex_tl_l2cap_cb_ptr) {
        osi_free(obex_tl_l2cap_cb_ptr);
        obex_tl_l2cap_cb_ptr = NULL;
    }
#endif /* #if (OBEX_DYNAMIC_MEMORY) */
}

/*******************************************************************************
**
** Function         obex_tl_l2cap_connect
**
** Description      Start the process of establishing a L2CAP connection
**
** Returns          Non-zeros handle, 0 while failed
**
*******************************************************************************/
UINT16 obex_tl_l2cap_connect(tOBEX_TL_SVR_INFO *server)
{
    tOBEX_TL_L2CAP_CCB *p_ccb = allocate_ccb();
    if (p_ccb == NULL) {
        /* can not allocate a ccb */
        return 0;
    }

    if (find_scb_by_psm(server->l2cap.psm) == NULL) {
        /* if the psm not register by a server, we register it as outgoing only record */
        tL2CAP_APPL_INFO *p_reg_info = &obex_tl_l2cap_cb.l2cap_reg_info;
        p_reg_info->pL2CA_ConnectInd_Cb = NULL;
        p_ccb->vpsm = L2CA_Register(server->l2cap.psm, p_reg_info);
        if (p_ccb->vpsm == 0) {
            free_ccb(p_ccb);
            return 0;
        }
        /* set security level */
        BTM_SetSecurityLevel(TRUE, "", BTM_SEC_SERVICE_OBEX, server->l2cap.sec_mask, p_ccb->vpsm, BTM_SEC_PROTO_OBEX, p_ccb->allocated - 1);
    }
    else {
        p_ccb->vpsm = server->l2cap.psm;
    }

    if (server->l2cap.pref_mtu == 0 || server->l2cap.pref_mtu > L2CAP_MTU_SIZE) {
        p_ccb->our_mtu = L2CAP_MTU_SIZE;
    }
    else {
        p_ccb->our_mtu = server->l2cap.pref_mtu;
    }
    p_ccb->initiator = TRUE;
    p_ccb->lcid = L2CA_ErtmConnectReq(p_ccb->vpsm, server->l2cap.addr, &obex_tl_l2cap_etm_opts);
    if (p_ccb->lcid == 0) {
        free_ccb(p_ccb);
        return 0;
    }

    return p_ccb->allocated;
}

/*******************************************************************************
**
** Function         obex_tl_l2cap_disconnect
**
** Description      Disconnect a L2CAP connection
**
*******************************************************************************/
void obex_tl_l2cap_disconnect(UINT16 hdl)
{
    tOBEX_TL_L2CAP_CCB *p_ccb = find_ccb_by_hdl(hdl);
    if (p_ccb != NULL) {
        L2CA_DisconnectReq(p_ccb->lcid);
        if (p_ccb->initiator && find_scb_by_psm(p_ccb->vpsm) == NULL) {
            L2CA_Deregister(p_ccb->vpsm);
        }
        free_ccb(p_ccb);
    }
}

/*******************************************************************************
**
** Function         obex_tl_l2cap_send_data
**
** Description      Start the process of establishing a L2CAP connection
**
** Returns          OBEX_TL_SUCCESS, if data accepted
**                  OBEX_TL_CONGESTED, if data accepted and the channel is congested
**                  OBEX_TL_FAILED, if error
**
*******************************************************************************/
UINT16 obex_tl_l2cap_send_data(UINT16 hdl, BT_HDR *p_buf)
{
    UINT16 ret = OBEX_TL_FAILED;
    tOBEX_TL_L2CAP_CCB *p_ccb = find_ccb_by_hdl(hdl);
    if (p_ccb == NULL) {
        osi_free(p_buf);
        return ret;
    }

    /* Can not send data size larger than peer MTU */
    /* Offset should not smaller than L2CAP_MIN_OFFSET */
    if (p_buf->len > p_ccb->peer_mtu || p_buf->offset < L2CAP_MIN_OFFSET) {
        osi_free(p_buf);
        return ret;
    }

    UINT16 status = L2CA_DataWrite(p_ccb->lcid, p_buf);
    switch (status)
    {
    case L2CAP_DW_SUCCESS:
        ret = OBEX_TL_SUCCESS;
        break;
    case L2CAP_DW_CONGESTED:
        ret = OBEX_TL_CONGESTED;
        break;
    default:
        ret = OBEX_TL_FAILED;
        break;
    }
    return ret;
}

/*******************************************************************************
**
** Function         obex_tl_l2cap_bind
**
** Description      Register a server in L2CAP module
**
** Returns          Non-zeros handle, 0 while failed
**
*******************************************************************************/
UINT16 obex_tl_l2cap_bind(tOBEX_TL_SVR_INFO *server)
{
    tOBEX_TL_L2CAP_SCB *p_scb = find_scb_by_psm(server->l2cap.psm);

    if (p_scb != NULL) {
        /* psm already used */
        return 0;
    }

    p_scb = allocate_scb();
    if (p_scb == NULL) {
        /* can not allocate a new scb */
        return 0;
    }

    if (server->l2cap.pref_mtu == 0 || server->l2cap.pref_mtu > L2CAP_MTU_SIZE) {
        p_scb->pref_mtu= L2CAP_MTU_SIZE;
    }
    else {
        p_scb->pref_mtu = server->l2cap.pref_mtu;
    }

    tL2CAP_APPL_INFO *p_reg_info = &obex_tl_l2cap_cb.l2cap_reg_info;
    p_reg_info->pL2CA_ConnectInd_Cb = obex_tl_l2cap_connect_ind;
    /* Register a l2cap server, in this case, L2CA_Register always return the same psm as input */
    p_scb->psm = L2CA_Register(server->l2cap.psm, p_reg_info);
    if (p_scb->psm == 0) {
        free_scb(p_scb);
        return 0;
    }
    /* set security level */
    BTM_SetSecurityLevel(TRUE, "", BTM_SEC_SERVICE_OBEX, server->l2cap.sec_mask, p_scb->psm, BTM_SEC_PROTO_OBEX, OBEX_TL_L2CAP_NUM_CONN + p_scb->allocated - 1);
    BTM_SetSecurityLevel(FALSE, "", BTM_SEC_SERVICE_OBEX, server->l2cap.sec_mask, p_scb->psm, BTM_SEC_PROTO_OBEX, OBEX_TL_L2CAP_NUM_CONN + p_scb->allocated - 1);
    /* left shift 8 bits to avoid confuse with connection handle */
    return p_scb->allocated << 8;
}

/*******************************************************************************
**
** Function         obex_tl_l2cap_unbind
**
** Description      Deregister a server in L2CAP module
**
*******************************************************************************/
void obex_tl_l2cap_unbind(UINT16 tl_hdl)
{
    tOBEX_TL_L2CAP_SCB *p_scb = find_scb_by_hdl(tl_hdl);
    if (p_scb) {
        tOBEX_TL_L2CAP_CCB *p_ccb = NULL;
        while ((p_ccb = find_ccb_by_psm(p_scb->psm)) != NULL) {
            L2CA_DisconnectReq(p_ccb->lcid);
            tOBEX_TL_MSG msg = {0};
            msg.any.hdl = p_ccb->allocated;
            obex_tl_l2cap_cb.callback(OBEX_TL_DIS_CONN_EVT, &msg);
            free_ccb(p_ccb);
        }
        L2CA_Deregister(p_scb->psm);
        free_scb(p_scb);
    }
}

static tOBEX_TL_OPS obex_tl_l2cap_ops = {
    .init = obex_tl_l2cap_init,
    .deinit = obex_tl_l2cap_deinit,
    .connect = obex_tl_l2cap_connect,
    .disconnect = obex_tl_l2cap_disconnect,
    .bind = obex_tl_l2cap_bind,
    .unbind = obex_tl_l2cap_unbind,
    .send = obex_tl_l2cap_send_data
};

/*******************************************************************************
**
** Function         obex_tl_l2cap_ops_get
**
** Description      Get the operation function structure pointer of OBEX over
**                  L2CAP transport layer
**
** Returns          Pointer to operation function structure
**
*******************************************************************************/
tOBEX_TL_OPS *obex_tl_l2cap_ops_get(void)
{
    return &obex_tl_l2cap_ops;
}

#endif /* #if (OBEX_INCLUDED == TRUE) */

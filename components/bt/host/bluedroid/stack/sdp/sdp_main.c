/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  This file contains the main SDP functions
 *
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
//#include <stdio.h>

#include "common/bt_target.h"
#include "osi/allocator.h"
#include "stack/hcidefs.h"
#include "stack/hcimsgs.h"

#include "stack/l2c_api.h"
#include "stack/l2cdefs.h"

#include "stack/btu.h"
#include "stack/btm_api.h"

#include "stack/sdp_api.h"
#include "sdpint.h"

#include "osi/list.h"

#if (SDP_INCLUDED == TRUE)
/********************************************************************************/
/*                       G L O B A L      S D P       D A T A                   */
/********************************************************************************/
#if SDP_DYNAMIC_MEMORY == FALSE
tSDP_CB  sdp_cb;
#else
tSDP_CB  *sdp_cb_ptr;
#endif

/********************************************************************************/
/*              L O C A L    F U N C T I O N     P R O T O T Y P E S            */
/********************************************************************************/
static void sdp_connect_ind (BD_ADDR  bd_addr, UINT16 l2cap_cid, UINT16 psm,
                             UINT8 l2cap_id);
static void sdp_config_ind (UINT16 l2cap_cid, tL2CAP_CFG_INFO *p_cfg);
static void sdp_config_cfm (UINT16 l2cap_cid, tL2CAP_CFG_INFO *p_cfg);
static void sdp_disconnect_ind (UINT16 l2cap_cid, BOOLEAN ack_needed);
static void sdp_data_ind (UINT16 l2cap_cid, BT_HDR *p_msg);

#if SDP_CLIENT_ENABLED == TRUE
static void sdp_connect_cfm (UINT16 l2cap_cid, UINT16 result);
static void sdp_disconnect_cfm (UINT16 l2cap_cid, UINT16 result);
#else
#define sdp_connect_cfm     NULL
#define sdp_disconnect_cfm  NULL
#endif

#if BT_SDP_BQB_INCLUDED
/* Defaults are off. Each field is one PTS case and one ctrl function. */
struct sdp_bqb_params {
    BOOLEAN disable_flag;
    BOOLEAN inact_timeout_flag;
    /* L2CAP ERTM BQB flag. Also read by l2c_fcr.c and l2c_utils.c. */
    BOOLEAN ertm_mode_included_flag;
    /* L2CAP/COS/CFD/BV-09-C: first SDP L2CAP payload is at least 48 bytes.
     * Consumed by the next Service Search Attribute Request. */
    BOOLEAN data_len_min_48_flag;
};

static struct sdp_bqb_params s_sdp_bqb;

BOOLEAN l2cap_bqb_ertm_mode_included_get(void)
{
    return s_sdp_bqb.ertm_mode_included_flag;
}

BOOLEAN sdp_bqb_take_data_len_min_48(void)
{
    BOOLEAN take = s_sdp_bqb.data_len_min_48_flag;

    s_sdp_bqb.data_len_min_48_flag = FALSE;
    return take;
}
#endif /* BT_SDP_BQB_INCLUDED */

/*******************************************************************************
**
** Function     sdp_bqb_disable_ctrl
**
** Description  Control the disable of bqb for SDP BQB test
**
** Returns      void
**
*******************************************************************************/
#if BT_SDP_BQB_INCLUDED
void sdp_bqb_disable_ctrl(BOOLEAN enable)
{
    s_sdp_bqb.disable_flag = enable;
}

/*******************************************************************************
**
** Function     sdp_bqb_inact_timeout_ctrl
**
** Description  Control the inactivity timeout for SDP BQB test
**
** Returns      void
**
*******************************************************************************/
void sdp_bqb_inact_timeout_ctrl(BOOLEAN enable)
{
    s_sdp_bqb.inact_timeout_flag = enable;
}

/* Local ConfigReq options while the ERTM flag is set. */
static void sdp_bqb_fill_ertm_cfg(tL2CAP_CFG_INFO *cfg)
{
    cfg->fcr_present = TRUE;
    cfg->fcr.mode = L2CAP_FCR_ERTM_MODE;
    cfg->fcr.tx_win_sz = 8;
    cfg->fcr.max_transmit = 0xff;
    cfg->fcr.rtrans_tout = 2000;
    cfg->fcr.mon_tout = 12000;
    cfg->fcr.mps = 672;
}

/* Applied on the BTU task. Flag off forces Basic on this copy. */
static void sdp_bqb_prepare_cfg(tL2CAP_CFG_INFO *cfg)
{
    if (s_sdp_bqb.ertm_mode_included_flag) {
        sdp_bqb_fill_ertm_cfg(cfg);
        return;
    }
    memset(&cfg->fcr, 0, sizeof(cfg->fcr));
    cfg->fcr_present = FALSE;
    cfg->fcr.mode = L2CAP_FCR_BASIC_MODE;
}

/*******************************************************************************
**
** Function     l2cap_bqb_ertm_mode_included_ctrl
**
** Description  Enable ERTM on the next SDP connection. Only the flag is stored
**              here. sdp_init and the connect paths apply it on the BTU task.
**
** Returns      void
**
*******************************************************************************/
void l2cap_bqb_ertm_mode_included_ctrl(BOOLEAN enable)
{
    s_sdp_bqb.ertm_mode_included_flag = enable;
}

/*******************************************************************************
**
** Function     sdp_bqb_data_len_min_48_ctrl
**
** Description  Pad a short Service Search Attribute Request so the first
**              SDP L2CAP payload is at least 48 bytes. A longer request is
**              not truncated. Used by L2CAP/COS/CFD/BV-09-C.
**
** Returns      void
**
*******************************************************************************/
void sdp_bqb_data_len_min_48_ctrl(BOOLEAN enable)
{
    s_sdp_bqb.data_len_min_48_flag = enable;
}

#if SDP_CLIENT_ENABLED == TRUE
#ifndef SDP_BQB_DB_SIZE
#define SDP_BQB_DB_SIZE  1500
#endif

#define SDP_BQB_OP_NONE  0
#define SDP_BQB_OP_SS    1
#define SDP_BQB_OP_SA    2

/* One SS or SA at a time. The discovery DB stays outside: it is the
 * buffer SDP parses, not the result handed to the callback. */
struct sdp_bqb_disc {
    UINT32 handles[SDP_MAX_DISC_SERVER_RECS];
    UINT16 num_handles;
    UINT32 sa_handle;
    sdp_bqb_ss_cb_t ss_cb;
    sdp_bqb_sa_cb_t sa_cb;
    BOOLEAN busy;
    UINT8 op;
};

static UINT8 __attribute__((aligned(4))) s_bqb_db_buf[SDP_BQB_DB_SIZE];
static struct sdp_bqb_disc s_sdp_bqb_disc;

static void sdp_bqb_disc_reset(void)
{
    memset(&s_sdp_bqb_disc, 0, sizeof(s_sdp_bqb_disc));
}

void sdp_bqb_save_search_handles(const UINT32 *handles, UINT16 num)
{
    if (handles == NULL) {
        s_sdp_bqb_disc.num_handles = 0;
        return;
    }
    if (num > SDP_MAX_DISC_SERVER_RECS) {
        num = SDP_MAX_DISC_SERVER_RECS;
    }
    s_sdp_bqb_disc.num_handles = num;
    memcpy(s_sdp_bqb_disc.handles, handles, (size_t)num * sizeof(UINT32));
}

static void sdp_bqb_disc_cmpl(UINT16 status, void *user_data)
{
    UINT8 op = s_sdp_bqb_disc.op;
    sdp_bqb_ss_cb_t ss_cb = s_sdp_bqb_disc.ss_cb;
    sdp_bqb_sa_cb_t sa_cb = s_sdp_bqb_disc.sa_cb;

    UNUSED(user_data);
    s_sdp_bqb_disc.busy = FALSE;
    s_sdp_bqb_disc.op = SDP_BQB_OP_NONE;
    s_sdp_bqb_disc.ss_cb = NULL;
    s_sdp_bqb_disc.sa_cb = NULL;

    if (op == SDP_BQB_OP_SS && ss_cb) {
        ss_cb(status, s_sdp_bqb_disc.num_handles, s_sdp_bqb_disc.handles);
    } else if (op == SDP_BQB_OP_SA && sa_cb) {
        sa_cb(status, s_sdp_bqb_disc.sa_handle);
    }
}

/* Caller task. busy is checked before SDP_InitDiscoveryDb() zeroes the
 * shared buffer, and is set before the request is handed to L2CAP, so a
 * concurrent call cannot clear a discovery that is already in flight. */
BOOLEAN sdp_bqb_service_search(UINT8 *bd_addr, UINT16 uuid16, sdp_bqb_ss_cb_t cb)
{
    tCONN_CB *p_ccb;
    tSDP_UUID uuid;

    if (bd_addr == NULL || cb == NULL || s_sdp_bqb_disc.busy) {
        return FALSE;
    }

    memset(&uuid, 0, sizeof(uuid));
    uuid.len = 2;
    uuid.uu.uuid16 = uuid16;

    if (!SDP_InitDiscoveryDb((tSDP_DISCOVERY_DB *)s_bqb_db_buf, SDP_BQB_DB_SIZE,
                             1, &uuid, 0, NULL)) {
        return FALSE;
    }

    p_ccb = sdp_conn_originate(bd_addr);
    if (!p_ccb) {
        return FALSE;
    }

    p_ccb->disc_state = SDP_DISC_WAIT_CONN;
    p_ccb->p_db = (tSDP_DISCOVERY_DB *)s_bqb_db_buf;
    p_ccb->p_cb = NULL;
    p_ccb->p_cb2 = sdp_bqb_disc_cmpl;
    p_ccb->user_data = NULL;
    p_ccb->is_attr_search = FALSE;
    p_ccb->is_search_only = TRUE;
    p_ccb->is_attr_only = FALSE;

    s_sdp_bqb_disc.num_handles = 0;
    memset(s_sdp_bqb_disc.handles, 0, sizeof(s_sdp_bqb_disc.handles));
    s_sdp_bqb_disc.ss_cb = cb;
    s_sdp_bqb_disc.op = SDP_BQB_OP_SS;
    s_sdp_bqb_disc.busy = TRUE;
    return TRUE;
}

/* Same single-discovery rule as sdp_bqb_service_search(). */
BOOLEAN sdp_bqb_service_attr(UINT8 *bd_addr, UINT32 handle, sdp_bqb_sa_cb_t cb)
{
    tCONN_CB *p_ccb;

    if (bd_addr == NULL || cb == NULL || s_sdp_bqb_disc.busy) {
        return FALSE;
    }

    if (!SDP_InitDiscoveryDb((tSDP_DISCOVERY_DB *)s_bqb_db_buf, SDP_BQB_DB_SIZE,
                             0, NULL, 0, NULL)) {
        return FALSE;
    }

    p_ccb = sdp_conn_originate(bd_addr);
    if (!p_ccb) {
        return FALSE;
    }

    p_ccb->disc_state = SDP_DISC_WAIT_CONN;
    p_ccb->p_db = (tSDP_DISCOVERY_DB *)s_bqb_db_buf;
    p_ccb->p_cb = NULL;
    p_ccb->p_cb2 = sdp_bqb_disc_cmpl;
    p_ccb->user_data = NULL;
    p_ccb->is_attr_search = FALSE;
    p_ccb->is_search_only = FALSE;
    p_ccb->is_attr_only = TRUE;
    p_ccb->handles[0] = handle;
    p_ccb->num_handles = 1;
    p_ccb->cur_handle = 0;

    s_sdp_bqb_disc.sa_handle = handle;
    s_sdp_bqb_disc.sa_cb = cb;
    s_sdp_bqb_disc.op = SDP_BQB_OP_SA;
    s_sdp_bqb_disc.busy = TRUE;
    return TRUE;
}
#endif /* SDP_CLIENT_ENABLED == TRUE */
#endif /* BT_SDP_BQB_INCLUDED */

/*******************************************************************************
**
** Function         sdp_init
**
** Description      This function initializes the SDP unit.
**
** Returns          void
**
*******************************************************************************/
void sdp_init (void)
{
#if SDP_DYNAMIC_MEMORY
    sdp_cb_ptr = (tSDP_CB *)osi_malloc(sizeof(tSDP_CB));
    if (!sdp_cb_ptr) {
        ESP_LOGE("BT_SDP", "SDP control block malloc failed\n");
        return;
    }
#endif /* #if SDP_DYNAMIC_MEMORY */
    /* Clears all structures and local SDP database (if Server is enabled) */
    memset (&sdp_cb, 0, sizeof (tSDP_CB));

    sdp_cb.server_db.p_record_list  = list_new(osi_free_func);
    /* Initialize the L2CAP configuration. We only care about MTU and flush */
    sdp_cb.l2cap_my_cfg.mtu_present       = TRUE;
    sdp_cb.l2cap_my_cfg.mtu               = SDP_MTU_SIZE;
    sdp_cb.l2cap_my_cfg.flush_to_present  = TRUE;
    sdp_cb.l2cap_my_cfg.flush_to          = SDP_FLUSH_TO;
#if BT_SDP_BQB_INCLUDED
    sdp_bqb_prepare_cfg(&sdp_cb.l2cap_my_cfg);
#if (SDP_CLIENT_ENABLED == TRUE)
    sdp_bqb_disc_reset();
#endif
#endif /* BT_SDP_BQB_INCLUDED */
    sdp_cb.max_attr_list_size             = SDP_MTU_SIZE - 16;
    sdp_cb.max_recs_per_search            = SDP_MAX_DISC_SERVER_RECS;

#if SDP_SERVER_ENABLED == TRUE
    /* Register with Security Manager for the specific security level */
    if (!BTM_SetSecurityLevel (FALSE, SDP_SERVICE_NAME, BTM_SEC_SERVICE_SDP_SERVER,
                               SDP_SECURITY_LEVEL, SDP_PSM, 0, 0)) {
        SDP_TRACE_ERROR ("Security Registration Server failed\n");
        return;
    }
#endif

#if SDP_CLIENT_ENABLED == TRUE
    /* Register with Security Manager for the specific security level */
    if (!BTM_SetSecurityLevel (TRUE, SDP_SERVICE_NAME, BTM_SEC_SERVICE_SDP_SERVER,
                               SDP_SECURITY_LEVEL, SDP_PSM, 0, 0)) {
        SDP_TRACE_ERROR ("Security Registration for Client failed\n");
        return;
    }
#endif

#if defined(SDP_INITIAL_TRACE_LEVEL)
    sdp_cb.trace_level = SDP_INITIAL_TRACE_LEVEL;
#else
    sdp_cb.trace_level = BT_TRACE_LEVEL_NONE;    /* No traces */
#endif

    sdp_cb.reg_info.pL2CA_ConnectInd_Cb = sdp_connect_ind;
    sdp_cb.reg_info.pL2CA_ConnectCfm_Cb = sdp_connect_cfm;
    sdp_cb.reg_info.pL2CA_ConnectPnd_Cb = NULL;
    sdp_cb.reg_info.pL2CA_ConfigInd_Cb  = sdp_config_ind;
    sdp_cb.reg_info.pL2CA_ConfigCfm_Cb  = sdp_config_cfm;
    sdp_cb.reg_info.pL2CA_DisconnectInd_Cb = sdp_disconnect_ind;
    sdp_cb.reg_info.pL2CA_DisconnectCfm_Cb = sdp_disconnect_cfm;
    sdp_cb.reg_info.pL2CA_QoSViolationInd_Cb = NULL;
    sdp_cb.reg_info.pL2CA_DataInd_Cb = sdp_data_ind;
    sdp_cb.reg_info.pL2CA_CongestionStatus_Cb = NULL;
    sdp_cb.reg_info.pL2CA_TxComplete_Cb       = NULL;

    /* Now, register with L2CAP */
    if (!L2CA_Register (SDP_PSM, &sdp_cb.reg_info)) {
        SDP_TRACE_ERROR ("SDP Registration failed\n");
    }
}

void sdp_deinit (void)
{
#if BT_SDP_BQB_INCLUDED && (SDP_CLIENT_ENABLED == TRUE)
    sdp_bqb_disc_reset();
#endif
#if SDP_DYNAMIC_MEMORY == FALSE
    list_free(sdp_cb.server_db.p_record_list);
#else
    if (sdp_cb_ptr) {
        list_free(sdp_cb_ptr->server_db.p_record_list);
        osi_free(sdp_cb_ptr);
        sdp_cb_ptr = NULL;
    }
#endif /* #if SDP_DYNAMIC_MEMORY */
}

#if (defined(SDP_DEBUG) && SDP_DEBUG == TRUE)
/*******************************************************************************
**
** Function         sdp_set_max_attr_list_size
**
** Description      This function sets the max attribute list size to use
**
** Returns          void
**
*******************************************************************************/
UINT16 sdp_set_max_attr_list_size (UINT16 max_size)
{
    if (max_size > (sdp_cb.l2cap_my_cfg.mtu - 16) ) {
        max_size = sdp_cb.l2cap_my_cfg.mtu - 16;
    }

    sdp_cb.max_attr_list_size  = max_size;

    return sdp_cb.max_attr_list_size;
}
#endif

/*******************************************************************************
**
** Function         sdp_connect_ind
**
** Description      This function handles an inbound connection indication
**                  from L2CAP. This is the case where we are acting as a
**                  server.
**
** Returns          void
**
*******************************************************************************/
static void sdp_connect_ind (BD_ADDR  bd_addr, UINT16 l2cap_cid, UINT16 psm, UINT8 l2cap_id)
{
    UNUSED(psm);
#if SDP_SERVER_ENABLED == TRUE
    tCONN_CB    *p_ccb;

    /* Allocate a new CCB. Return if none available. */
    if ((p_ccb = sdpu_allocate_ccb()) == NULL) {
        return;
    }

    /* Transition to the next appropriate state, waiting for config setup. */
    p_ccb->con_state = SDP_STATE_CFG_SETUP;

    /* Save the BD Address and Channel ID. */
    memcpy (&p_ccb->device_address[0], bd_addr, sizeof (BD_ADDR));
    p_ccb->connection_id = l2cap_cid;

    /* Send response to the L2CAP layer. */
    L2CA_ConnectRsp (bd_addr, l2cap_id, l2cap_cid, L2CAP_CONN_OK, L2CAP_CONN_OK);
    {
        tL2CAP_CFG_INFO cfg = sdp_cb.l2cap_my_cfg;

#if BT_SDP_BQB_INCLUDED
        /* Flag on asks for ERTM. Flag off forces Basic on this copy. */
        sdp_bqb_prepare_cfg(&cfg);
#endif /* BT_SDP_BQB_INCLUDED */

        if (cfg.fcr_present) {
            SDP_TRACE_DEBUG("sdp_connect_ind:  mode %u, txwinsz %u, max_trans %u, rtrans_tout %u, mon_tout %u, mps %u\n",
                            cfg.fcr.mode, cfg.fcr.tx_win_sz, cfg.fcr.max_transmit,
                            cfg.fcr.rtrans_tout, cfg.fcr.mon_tout, cfg.fcr.mps);
        }

        if ((!L2CA_ConfigReq (l2cap_cid, &cfg)) && cfg.fcr_present
                && cfg.fcr.mode != L2CAP_FCR_BASIC_MODE) {
#if BT_SDP_BQB_INCLUDED
            if (s_sdp_bqb.ertm_mode_included_flag) {
                /* Sending the same options again cannot succeed, and the
                 * incompatible-mode path has already disconnected. */
                SDP_TRACE_WARNING ("SDP - ERTM ConfigReq failed, CID 0x%x\n", l2cap_cid);
                return;
            } else
#endif /* BT_SDP_BQB_INCLUDED */
            {
                /* FCR not desired; try again in basic mode */
                cfg.fcr.mode = L2CAP_FCR_BASIC_MODE;
                cfg.fcr_present = FALSE;
                L2CA_ConfigReq (l2cap_cid, &cfg);
            }
        }
    }

    SDP_TRACE_EVENT ("SDP - Rcvd L2CAP conn ind, sent config req, CID 0x%x\n", p_ccb->connection_id);
#else   /* No server */
    /* Reject the connection */
    L2CA_ConnectRsp (bd_addr, l2cap_id, l2cap_cid, L2CAP_CONN_NO_PSM, 0);
#endif
}

#if SDP_CLIENT_ENABLED == TRUE
/*******************************************************************************
**
** Function         sdp_connect_cfm
**
** Description      This function handles the connect confirm events
**                  from L2CAP. This is the case when we are acting as a
**                  client and have sent a connect request.
**
** Returns          void
**
*******************************************************************************/
static void sdp_connect_cfm (UINT16 l2cap_cid, UINT16 result)
{
    tCONN_CB    *p_ccb;
    tL2CAP_CFG_INFO cfg;

    /* Find CCB based on CID */
    if ((p_ccb = sdpu_find_ccb_by_cid (l2cap_cid)) == NULL) {
        SDP_TRACE_WARNING ("SDP - Rcvd conn cnf for unknown CID 0x%x\n", l2cap_cid);
        return;
    }

    /* If the connection response contains success status, then */
    /* Transition to the next state and startup the timer.      */
    if ((result == L2CAP_CONN_OK) && (p_ccb->con_state == SDP_STATE_CONN_SETUP)) {
        p_ccb->con_state = SDP_STATE_CFG_SETUP;

        cfg = sdp_cb.l2cap_my_cfg;

#if BT_SDP_BQB_INCLUDED
        /* Same ERTM request as the acceptor. Flag off forces Basic. */
        sdp_bqb_prepare_cfg(&cfg);
#endif /* BT_SDP_BQB_INCLUDED */

        if (cfg.fcr_present) {
            SDP_TRACE_DEBUG("sdp_connect_cfm:  mode %u, txwinsz %u, max_trans %u, rtrans_tout %u, mon_tout %u, mps %u\n",
                            cfg.fcr.mode, cfg.fcr.tx_win_sz, cfg.fcr.max_transmit,
                            cfg.fcr.rtrans_tout, cfg.fcr.mon_tout, cfg.fcr.mps);
        }

        if ((!L2CA_ConfigReq (l2cap_cid, &cfg)) && cfg.fcr_present
                && cfg.fcr.mode != L2CAP_FCR_BASIC_MODE) {
#if BT_SDP_BQB_INCLUDED
            if (s_sdp_bqb.ertm_mode_included_flag) {
                SDP_TRACE_WARNING ("SDP - ERTM ConfigReq failed, CID 0x%x\n", l2cap_cid);
                return;
            } else
#endif /* BT_SDP_BQB_INCLUDED */
            {
                /* FCR not desired; try again in basic mode */
                cfg.fcr_present = FALSE;
                cfg.fcr.mode = L2CAP_FCR_BASIC_MODE;
                L2CA_ConfigReq (l2cap_cid, &cfg);
            }
        }

        SDP_TRACE_EVENT ("SDP - got conn cnf, sent cfg req, CID: 0x%x\n", p_ccb->connection_id);
    } else {
        SDP_TRACE_WARNING ("SDP - Rcvd conn cnf with error: 0x%x  CID 0x%x\n", result, p_ccb->connection_id);

        /* Tell the user if he has a callback */
        if (p_ccb->p_cb || p_ccb->p_cb2) {
            UINT16 err = -1;
            if ((result == HCI_ERR_HOST_REJECT_SECURITY)
                    || (result == HCI_ERR_AUTH_FAILURE)
                    || (result == HCI_ERR_PAIRING_NOT_ALLOWED)
                    || (result == HCI_ERR_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED)
                    || (result == HCI_ERR_KEY_MISSING)) {
                err = SDP_SECURITY_ERR;
            } else if (result == HCI_ERR_HOST_REJECT_DEVICE) {
                err = SDP_CONN_REJECTED;
            } else {
                err = SDP_CONN_FAILED;
            }
            if (p_ccb->p_cb) {
                (*p_ccb->p_cb)(err);
            } else if (p_ccb->p_cb2) {
                (*p_ccb->p_cb2)(err, p_ccb->user_data);
            }

        }
        sdpu_release_ccb (p_ccb);
    }
}
#endif  /* SDP_CLIENT_ENABLED == TRUE */


/*******************************************************************************
**
** Function         sdp_config_ind
**
** Description      This function processes the L2CAP configuration indication
**                  event.
**
** Returns          void
**
*******************************************************************************/
static void sdp_config_ind (UINT16 l2cap_cid, tL2CAP_CFG_INFO *p_cfg)
{
    tCONN_CB    *p_ccb;

    /* Find CCB based on CID */
    if ((p_ccb = sdpu_find_ccb_by_cid (l2cap_cid)) == NULL) {
        SDP_TRACE_WARNING ("SDP - Rcvd L2CAP cfg ind, unknown CID: 0x%x\n", l2cap_cid);
        return;
    }

    /* Remember the remote MTU size */
    if (!p_cfg->mtu_present) {
        /* use min(L2CAP_DEFAULT_MTU,SDP_MTU_SIZE) for GKI buffer size reasons */
        p_ccb->rem_mtu_size = (L2CAP_DEFAULT_MTU > SDP_MTU_SIZE) ? SDP_MTU_SIZE : L2CAP_DEFAULT_MTU;
    } else {
        if (p_cfg->mtu > SDP_MTU_SIZE) {
            p_ccb->rem_mtu_size = SDP_MTU_SIZE;
        } else {
            p_ccb->rem_mtu_size = p_cfg->mtu;
        }
    }

    /* For now, always accept configuration from the other side */
    p_cfg->flush_to_present = FALSE;
    p_cfg->mtu_present      = FALSE;
    p_cfg->result           = L2CAP_CFG_OK;

    /* Check peer config request against our sdp configuration */
    if (p_cfg->fcr_present) {
        /* Reject the window size if it is bigger than we want it to be */
        if (p_cfg->fcr.mode != L2CAP_FCR_BASIC_MODE) {
            BOOLEAN want_ertm = (sdp_cb.l2cap_my_cfg.fcr.mode != L2CAP_FCR_BASIC_MODE);
            UINT8 max_tx_win = sdp_cb.l2cap_my_cfg.fcr.tx_win_sz;
#if BT_SDP_BQB_INCLUDED
            /* Flag on keeps ERTM and caps the window at the value we request.
             * Flag off asks for Basic, including a cfg stored while the flag was on. */
            if (s_sdp_bqb.ertm_mode_included_flag) {
                want_ertm = TRUE;
                max_tx_win = 8;
            } else {
                want_ertm = FALSE;
            }
#endif /* BT_SDP_BQB_INCLUDED */
            if (want_ertm && p_cfg->fcr.tx_win_sz > max_tx_win) {
                p_cfg->fcr.tx_win_sz = max_tx_win;
                p_cfg->result = L2CAP_CFG_UNACCEPTABLE_PARAMS;
                SDP_TRACE_DEBUG("sdp_config_ind(CONFIG) -> Please try again with SMALLER TX WINDOW\n");
            }

            /* Reject if locally we want basic and they don't. */
            if (!want_ertm) {
                /* Ask for a new setup */
                p_cfg->fcr.mode = L2CAP_FCR_BASIC_MODE;
                p_cfg->result = L2CAP_CFG_UNACCEPTABLE_PARAMS;
                SDP_TRACE_DEBUG("sdp_config_ind(CONFIG) -> Please try again with BASIC mode\n");
            }
            /* Remain in configure state and give the peer our desired configuration */
            if (p_cfg->result != L2CAP_CFG_OK) {
                SDP_TRACE_WARNING ("SDP - Rcvd cfg ind, Unacceptable Parameters sent cfg cfm, CID: 0x%x\n", l2cap_cid);
                L2CA_ConfigRsp (l2cap_cid, p_cfg);
                return;
            }
        } else { /* We agree with peer's request */
            p_cfg->fcr_present = FALSE;
        }
    }

    L2CA_ConfigRsp (l2cap_cid, p_cfg);

    SDP_TRACE_EVENT ("SDP - Rcvd cfg ind, sent cfg cfm, CID: 0x%x\n", l2cap_cid);

    p_ccb->con_flags |= SDP_FLAGS_HIS_CFG_DONE;

    if (p_ccb->con_flags & SDP_FLAGS_MY_CFG_DONE) {
        p_ccb->con_state = SDP_STATE_CONNECTED;

        if (p_ccb->con_flags & SDP_FLAGS_IS_ORIG) {
            sdp_disc_connected (p_ccb);
        } else
            /* Start inactivity timer */
        {
            btu_start_timer (&p_ccb->timer_entry, BTU_TTYPE_SDP, SDP_INACT_TIMEOUT);
        }
    }

}


/*******************************************************************************
**
** Function         sdp_config_cfm
**
** Description      This function processes the L2CAP configuration confirmation
**                  event.
**
** Returns          void
**
*******************************************************************************/
static void sdp_config_cfm (UINT16 l2cap_cid, tL2CAP_CFG_INFO *p_cfg)
{
    tCONN_CB    *p_ccb;

    SDP_TRACE_EVENT ("SDP - Rcvd cfg cfm, CID: 0x%x  Result: %d\n", l2cap_cid, p_cfg->result);

    /* Find CCB based on CID */
    if ((p_ccb = sdpu_find_ccb_by_cid (l2cap_cid)) == NULL) {
        SDP_TRACE_WARNING ("SDP - Rcvd L2CAP cfg ind, unknown CID: 0x%x\n", l2cap_cid);
        return;
    }

    /* For now, always accept configuration from the other side */
    if (p_cfg->result == L2CAP_CFG_OK) {
        p_ccb->con_flags |= SDP_FLAGS_MY_CFG_DONE;

        if (p_ccb->con_flags & SDP_FLAGS_HIS_CFG_DONE) {
            p_ccb->con_state = SDP_STATE_CONNECTED;

            if (p_ccb->con_flags & SDP_FLAGS_IS_ORIG) {
                sdp_disc_connected (p_ccb);
            } else
                /* Start inactivity timer */
            {
#if BT_SDP_BQB_INCLUDED
                /* PTS leaves this channel idle longer than SDP_INACT_TIMEOUT. */
                if (s_sdp_bqb.inact_timeout_flag) {
                    btu_start_timer(&p_ccb->timer_entry, BTU_TTYPE_SDP, SDP_BQB_INACT_TIMEOUT);
                } else
#endif /* BT_SDP_BQB_INCLUDED */
                {
                    btu_start_timer(&p_ccb->timer_entry, BTU_TTYPE_SDP, SDP_INACT_TIMEOUT);
                }
            }
        }
    } else {
#if BT_SDP_BQB_INCLUDED
        /* Peer rejected ERTM. Do not retry in Basic while the flag is set. */
        if (s_sdp_bqb.ertm_mode_included_flag && p_cfg->fcr_present) {
            SDP_TRACE_WARNING("SDP - peer rejected ERTM, CID 0x%x\n", l2cap_cid);
        } else
#endif
        /* If peer has rejected FCR and suggested basic then try basic */
        if (p_cfg->fcr_present) {
            tL2CAP_CFG_INFO cfg = sdp_cb.l2cap_my_cfg;
            cfg.fcr_present = FALSE;
            L2CA_ConfigReq (l2cap_cid, &cfg);

            /* Remain in configure state */
            return;
        }

#if SDP_CLIENT_ENABLED == TRUE
        sdp_disconnect(p_ccb, SDP_CFG_FAILED);
#endif
    }
}

/*******************************************************************************
**
** Function         sdp_disconnect_ind
**
** Description      This function handles a disconnect event from L2CAP. If
**                  requested to, we ack the disconnect before dropping the CCB
**
** Returns          void
**
*******************************************************************************/
static void sdp_disconnect_ind (UINT16 l2cap_cid, BOOLEAN ack_needed)
{
    tCONN_CB    *p_ccb;

    /* Find CCB based on CID */
    if ((p_ccb = sdpu_find_ccb_by_cid (l2cap_cid)) == NULL) {
        SDP_TRACE_WARNING ("SDP - Rcvd L2CAP disc, unknown CID: 0x%x\n", l2cap_cid);
        /* The SDP CCB may already have been released by the inactivity
         * timeout. L2CAP still waits for Disconnect Response; without it
         * the request is left unanswered (L2CAP/ERM/BV-02-C, L2CAP/ERM/BV-19-C). */
        if (ack_needed) {
            L2CA_DisconnectRsp (l2cap_cid);
        }
        return;
    }

    if (ack_needed) {
        L2CA_DisconnectRsp (l2cap_cid);
    }

    SDP_TRACE_EVENT ("SDP - Rcvd L2CAP disc, CID: 0x%x\n", l2cap_cid);
#if SDP_CLIENT_ENABLED == TRUE
    /* Tell the user if he has a callback */
    if (p_ccb->p_cb) {
        (*p_ccb->p_cb) ((UINT16) ((p_ccb->con_state == SDP_STATE_CONNECTED) ?
                                  SDP_SUCCESS : SDP_CONN_FAILED));
    } else if (p_ccb->p_cb2) {
        (*p_ccb->p_cb2) ((UINT16) ((p_ccb->con_state == SDP_STATE_CONNECTED) ?
                                   SDP_SUCCESS : SDP_CONN_FAILED), p_ccb->user_data);
    }

#endif
    sdpu_release_ccb (p_ccb);
}

/*******************************************************************************
**
** Function         sdp_data_ind
**
** Description      This function is called when data is received from L2CAP.
**                  if we are the originator of the connection, we are the SDP
**                  client, and the received message is queued up for the client.
**
**                  If we are the destination of the connection, we are the SDP
**                  server, so the message is passed to the server processing
**                  function.
**
** Returns          void
**
*******************************************************************************/
static void sdp_data_ind (UINT16 l2cap_cid, BT_HDR *p_msg)
{
    tCONN_CB    *p_ccb;

    /* Find CCB based on CID */
    if ((p_ccb = sdpu_find_ccb_by_cid (l2cap_cid)) != NULL) {
        if (p_ccb->con_state == SDP_STATE_CONNECTED) {
#if BT_SDP_BQB_INCLUDED
            /* Skip the following code in BQB test when the flag is true, since the PDU is reserved and
               function sdp_server_handle_client_req will return error (sdpu_build_n_send_error) */
            if (!s_sdp_bqb.disable_flag)
#endif /* BT_SDP_BQB_INCLUDED */
            {
                if (p_ccb->con_flags & SDP_FLAGS_IS_ORIG) {
                    sdp_disc_server_rsp(p_ccb, p_msg);
                } else {
                    sdp_server_handle_client_req(p_ccb, p_msg);
                }
            }
        } else {
            SDP_TRACE_WARNING ("SDP - Ignored L2CAP data while in state: %d, CID: 0x%x\n",
                               p_ccb->con_state, l2cap_cid);
        }
    } else {
        SDP_TRACE_WARNING ("SDP - Rcvd L2CAP data, unknown CID: 0x%x\n", l2cap_cid);
    }

    osi_free (p_msg);
}


#if SDP_CLIENT_ENABLED == TRUE
/*******************************************************************************
**
** Function         sdp_conn_originate
**
** Description      This function is called from the API to originate a
**                  connection.
**
** Returns          void
**
*******************************************************************************/
tCONN_CB *sdp_conn_originate (UINT8 *p_bd_addr)
{
    tCONN_CB              *p_ccb;
    UINT16                cid;

    /* Allocate a new CCB. Return if none available. */
    if ((p_ccb = sdpu_allocate_ccb()) == NULL) {
        SDP_TRACE_WARNING ("SDP - no spare CCB for orig\n");
        return (NULL);
    }

    SDP_TRACE_EVENT ("SDP - Originate started\n");

    /* We are the originator of this connection */
    p_ccb->con_flags |= SDP_FLAGS_IS_ORIG;

    /* Save the BD Address and Channel ID. */
    memcpy (&p_ccb->device_address[0], p_bd_addr, sizeof (BD_ADDR));

    /* Transition to the next appropriate state, waiting for connection confirm. */
    p_ccb->con_state = SDP_STATE_CONN_SETUP;

    cid = L2CA_ConnectReq (SDP_PSM, p_bd_addr);

    /* Check if L2CAP started the connection process */
    if (cid != 0) {
        p_ccb->connection_id = cid;

        return (p_ccb);
    } else {
        SDP_TRACE_WARNING ("SDP - Originate failed\n");
        sdpu_release_ccb (p_ccb);
        return (NULL);
    }
}

/*******************************************************************************
**
** Function         sdp_disconnect
**
** Description      This function disconnects a connection.
**
** Returns          void
**
*******************************************************************************/
void sdp_disconnect (tCONN_CB *p_ccb, UINT16 reason)
{
#if (defined(SDP_BROWSE_PLUS) && SDP_BROWSE_PLUS == TRUE)
    /* If we are browsing for multiple UUIDs ... */
    if ((p_ccb->con_state == SDP_STATE_CONNECTED)
            && (p_ccb->con_flags & SDP_FLAGS_IS_ORIG)
            && ((reason == SDP_SUCCESS) || (reason == SDP_NO_RECS_MATCH))) {
        /* If the browse found something, do no more searching */
        if ((p_ccb->cur_uuid_idx == 0) && (p_ccb->p_db->p_first_rec)) {
            p_ccb->cur_uuid_idx = p_ccb->p_db->num_uuid_filters;
        }

        while (++p_ccb->cur_uuid_idx < p_ccb->p_db->num_uuid_filters) {
            /* Check we have not already found the UUID (maybe through browse) */
            if ((p_ccb->p_db->uuid_filters[p_ccb->cur_uuid_idx].len == 2)
                    && (SDP_FindServiceInDb (p_ccb->p_db,
                                             p_ccb->p_db->uuid_filters[p_ccb->cur_uuid_idx].uu.uuid16,
                                             NULL))) {
                continue;
            }

            if ((p_ccb->p_db->uuid_filters[p_ccb->cur_uuid_idx].len > 2)
                    && (SDP_FindServiceUUIDInDb (p_ccb->p_db,
                                                 &p_ccb->p_db->uuid_filters[p_ccb->cur_uuid_idx], NULL))) {
                continue;
            }

            p_ccb->cur_handle = 0;

            SDP_TRACE_EVENT ("SDP - looking for for more,  CID: 0x%x\n",
                             p_ccb->connection_id);

            sdp_disc_connected (p_ccb);
            return;
        }
    }

    if ((reason == SDP_NO_RECS_MATCH) && (p_ccb->p_db->p_first_rec)) {
        reason = SDP_SUCCESS;
    }

#endif

    SDP_TRACE_EVENT ("SDP - disconnect  CID: 0x%x\n", p_ccb->connection_id);

    /* Check if we have a connection ID */
    if (p_ccb->connection_id != 0) {
        L2CA_DisconnectReq (p_ccb->connection_id);
        p_ccb->disconnect_reason = reason;
    }

    /* If at setup state, we may not get callback ind from L2CAP */
    /* Call user callback immediately */
    if (p_ccb->con_state == SDP_STATE_CONN_SETUP) {
        /* Tell the user if he has a callback */
        if (p_ccb->p_cb) {
            (*p_ccb->p_cb) (reason);
        } else if (p_ccb->p_cb2) {
            (*p_ccb->p_cb2) (reason, p_ccb->user_data);
        }

        sdpu_release_ccb (p_ccb);
    }

}

/*******************************************************************************
**
** Function         sdp_disconnect_cfm
**
** Description      This function handles a disconnect confirm event from L2CAP.
**
** Returns          void
**
*******************************************************************************/
static void sdp_disconnect_cfm (UINT16 l2cap_cid, UINT16 result)
{
    tCONN_CB    *p_ccb;
    UNUSED(result);

    /* Find CCB based on CID */
    if ((p_ccb = sdpu_find_ccb_by_cid (l2cap_cid)) == NULL) {
        SDP_TRACE_WARNING ("SDP - Rcvd L2CAP disc cfm, unknown CID: 0x%x\n", l2cap_cid);
        return;
    }

    SDP_TRACE_EVENT ("SDP - Rcvd L2CAP disc cfm, CID: 0x%x, rsn %d\n", l2cap_cid, p_ccb->disconnect_reason);
    /* Tell the user if he has a callback */
    if (p_ccb->p_cb) {
        (*p_ccb->p_cb) (p_ccb->disconnect_reason);
    } else if (p_ccb->p_cb2) {
        (*p_ccb->p_cb2) (p_ccb->disconnect_reason, p_ccb->user_data);
    }


    sdpu_release_ccb (p_ccb);
}

#endif  /* SDP_CLIENT_ENABLED == TRUE */

/*******************************************************************************
**
** Function         sdp_conn_timeout
**
** Description      This function processes a timeout. Currently, we simply send
**                  a disconnect request to L2CAP.
**
** Returns          void
**
*******************************************************************************/
void sdp_conn_timeout (tCONN_CB *p_ccb)
{
    SDP_TRACE_EVENT ("SDP - CCB timeout in state: %d  CID: 0x%x\n",
                     p_ccb->con_state, p_ccb->connection_id);

    L2CA_DisconnectReq (p_ccb->connection_id);
#if SDP_CLIENT_ENABLED == TRUE
    /* Tell the user if he has a callback */
    if (p_ccb->p_cb) {
        (*p_ccb->p_cb) (SDP_CONN_FAILED);
    } else if (p_ccb->p_cb2) {
        (*p_ccb->p_cb2) (SDP_CONN_FAILED, p_ccb->user_data);
    }
#endif
    sdpu_release_ccb (p_ccb);
}

#endif  ///SDP_INCLUDED == TRUE

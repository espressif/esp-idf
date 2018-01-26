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

#include "bt_target.h"
#include "allocator.h"
#include "l2cdefs.h"
#include "hcidefs.h"
#include "hcimsgs.h"

#include "l2c_api.h"
#include "l2cdefs.h"

#include "btu.h"
#include "btm_api.h"

#include "sdp_api.h"
#include "sdpint.h"

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
#endif /* #if SDP_DYNAMIC_MEMORY */
    /* Clears all structures and local SDP database (if Server is enabled) */
    memset (&sdp_cb, 0, sizeof (tSDP_CB));

    /* Initialize the L2CAP configuration. We only care about MTU and flush */
    sdp_cb.l2cap_my_cfg.mtu_present       = TRUE;
    sdp_cb.l2cap_my_cfg.mtu               = SDP_MTU_SIZE;
    sdp_cb.l2cap_my_cfg.flush_to_present  = TRUE;
    sdp_cb.l2cap_my_cfg.flush_to          = SDP_FLUSH_TO;

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

        if (cfg.fcr_present) {
            SDP_TRACE_DEBUG("sdp_connect_ind:  mode %u, txwinsz %u, max_trans %u, rtrans_tout %u, mon_tout %u, mps %u\n",
                            cfg.fcr.mode, cfg.fcr.tx_win_sz, cfg.fcr.max_transmit,
                            cfg.fcr.rtrans_tout, cfg.fcr.mon_tout, cfg.fcr.mps);
        }

        if ((!L2CA_ConfigReq (l2cap_cid, &cfg)) && cfg.fcr_present
                && cfg.fcr.mode != L2CAP_FCR_BASIC_MODE) {
            /* FCR not desired; try again in basic mode */
            cfg.fcr.mode = L2CAP_FCR_BASIC_MODE;
            cfg.fcr_present = FALSE;
            L2CA_ConfigReq (l2cap_cid, &cfg);
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

        if (cfg.fcr_present) {
            SDP_TRACE_DEBUG("sdp_connect_cfm:  mode %u, txwinsz %u, max_trans %u, rtrans_tout %u, mon_tout %u, mps %u\n",
                            cfg.fcr.mode, cfg.fcr.tx_win_sz, cfg.fcr.max_transmit,
                            cfg.fcr.rtrans_tout, cfg.fcr.mon_tout, cfg.fcr.mps);
        }

        if ((!L2CA_ConfigReq (l2cap_cid, &cfg)) && cfg.fcr_present
                && cfg.fcr.mode != L2CAP_FCR_BASIC_MODE) {
            /* FCR not desired; try again in basic mode */
            cfg.fcr_present = FALSE;
            cfg.fcr.mode = L2CAP_FCR_BASIC_MODE;
            L2CA_ConfigReq (l2cap_cid, &cfg);
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

    /* Check peer config request against our rfcomm configuration */
    if (p_cfg->fcr_present) {
        /* Reject the window size if it is bigger than we want it to be */
        if (p_cfg->fcr.mode != L2CAP_FCR_BASIC_MODE) {
            if (sdp_cb.l2cap_my_cfg.fcr.mode != L2CAP_FCR_BASIC_MODE
                    && p_cfg->fcr.tx_win_sz > sdp_cb.l2cap_my_cfg.fcr.tx_win_sz) {
                p_cfg->fcr.tx_win_sz = sdp_cb.l2cap_my_cfg.fcr.tx_win_sz;
                p_cfg->result = L2CAP_CFG_UNACCEPTABLE_PARAMS;
                SDP_TRACE_DEBUG("sdp_config_ind(CONFIG) -> Please try again with SMALLER TX WINDOW\n");
            }

            /* Reject if locally we want basic and they don't */
            if (sdp_cb.l2cap_my_cfg.fcr.mode == L2CAP_FCR_BASIC_MODE) {
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
                btu_start_timer (&p_ccb->timer_entry, BTU_TTYPE_SDP, SDP_INACT_TIMEOUT);
            }
        }
    } else {
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
        return;
    }

    if (ack_needed) {
        L2CA_DisconnectRsp (l2cap_cid);
    }

    SDP_TRACE_EVENT ("SDP - Rcvd L2CAP disc, CID: 0x%x\n", l2cap_cid);
#if SDP_CLIENT_ENABLED == TRUE
    /* Tell the user if he has a callback */
    if (p_ccb->p_cb)
        (*p_ccb->p_cb) ((UINT16) ((p_ccb->con_state == SDP_STATE_CONNECTED) ?
                                  SDP_SUCCESS : SDP_CONN_FAILED));
    else if (p_ccb->p_cb2)
        (*p_ccb->p_cb2) ((UINT16) ((p_ccb->con_state == SDP_STATE_CONNECTED) ?
                                   SDP_SUCCESS : SDP_CONN_FAILED), p_ccb->user_data);

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
            if (p_ccb->con_flags & SDP_FLAGS_IS_ORIG) {
                sdp_disc_server_rsp (p_ccb, p_msg);
            } else {
                sdp_server_handle_client_req (p_ccb, p_msg);
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




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
 *  This file contains the L2CAP API code
 *
 ******************************************************************************/

//#define LOG_TAG "bt_l2cap"

//#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "common/bt_trace.h"
#include "stack/bt_types.h"
#include "stack/hcidefs.h"
#include "stack/hcimsgs.h"
#include "stack/l2cdefs.h"
#include "l2c_int.h"
#include "stack/btu.h"
#include "stack/btm_api.h"
#include "osi/allocator.h"
#include "gatt_int.h"
#if (CLASSIC_BT_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         L2CA_Register
**
** Description      Other layers call this function to register for L2CAP
**                  services.
**
** Returns          PSM to use or zero if error. Typically, the PSM returned
**                  is the same as was passed in, but for an outgoing-only
**                  connection to a dynamic PSM, a "virtual" PSM is returned
**                  and should be used in the calls to L2CA_ConnectReq(),
**                  L2CA_ErtmConnectReq() and L2CA_Deregister()
**
*******************************************************************************/
UINT16 L2CA_Register (UINT16 psm, tL2CAP_APPL_INFO *p_cb_info)
{
    tL2C_RCB    *p_rcb;
    UINT16      vpsm = psm;


    /* Verify that the required callback info has been filled in
    **      Note:  Connection callbacks are required but not checked
    **             for here because it is possible to be only a client
    **             or only a server.
    */
    if ((!p_cb_info->pL2CA_ConfigCfm_Cb)
            || (!p_cb_info->pL2CA_ConfigInd_Cb)
            || (!p_cb_info->pL2CA_DataInd_Cb)
            || (!p_cb_info->pL2CA_DisconnectInd_Cb)) {
        L2CAP_TRACE_ERROR ("L2CAP - no cb registering PSM: 0x%04x", psm);
        return (0);
    }

    /* Verify PSM is valid */
    if (L2C_INVALID_PSM(psm)) {
        L2CAP_TRACE_ERROR ("L2CAP - invalid PSM value, PSM: 0x%04x", psm);
        return (0);
    }

    /* Check if this is a registration for an outgoing-only connection to */
    /* a dynamic PSM. If so, allocate a "virtual" PSM for the app to use. */
    if ( (psm >= 0x1001) && (p_cb_info->pL2CA_ConnectInd_Cb == NULL) ) {
        for (vpsm = 0x1002; vpsm < 0x8000; vpsm += 2) {
            if ((p_rcb = l2cu_find_rcb_by_psm (vpsm)) == NULL) {
                break;
            }
        }

        //L2CAP_TRACE_API ("L2CA_Register - Real PSM: 0x%04x  Virtual PSM: 0x%04x", psm, vpsm);
    }

    /* If registration block already there, just overwrite it */
    if ((p_rcb = l2cu_find_rcb_by_psm (vpsm)) == NULL) {
        if ((p_rcb = l2cu_allocate_rcb (vpsm)) == NULL) {
            L2CAP_TRACE_WARNING ("L2CAP - no RCB available, PSM: 0x%04x  vPSM: 0x%04x", psm, vpsm);
            return (0);
        }
    }

    p_rcb->api      = *p_cb_info;
    p_rcb->real_psm = psm;

    return (vpsm);
}



/*******************************************************************************
**
** Function         L2CA_Deregister
**
** Description      Other layers call this function to de-register for L2CAP
**                  services.
**
** Returns          void
**
*******************************************************************************/
void L2CA_Deregister (UINT16 psm)
{
    tL2C_RCB    *p_rcb;
    tL2C_CCB    *p_ccb;
    tL2C_LCB    *p_lcb;
    int         ii;


    if ((p_rcb = l2cu_find_rcb_by_psm (psm)) != NULL) {
        p_lcb = &l2cb.lcb_pool[0];
        for (ii = 0; ii < MAX_L2CAP_LINKS; ii++, p_lcb++) {
            if (p_lcb->in_use) {
                if (((p_ccb = p_lcb->ccb_queue.p_first_ccb) == NULL)
                        || (p_lcb->link_state == LST_DISCONNECTING)) {
                    continue;
                }

                if ((p_ccb->in_use) &&
                        ((p_ccb->chnl_state == CST_W4_L2CAP_DISCONNECT_RSP) ||
                         (p_ccb->chnl_state == CST_W4_L2CA_DISCONNECT_RSP))) {
                    continue;
                }

                if (p_ccb->p_rcb == p_rcb) {
                    l2c_csm_execute (p_ccb, L2CEVT_L2CA_DISCONNECT_REQ, NULL);
                }
            }
        }
        l2cu_release_rcb (p_rcb);
    } else {
        L2CAP_TRACE_WARNING ("L2CAP - PSM: 0x%04x not found for deregistration", psm);
    }
}

/*******************************************************************************
**
** Function         L2CA_AllocatePSM
**
** Description      Other layers call this function to find an unused PSM for L2CAP
**                  services.
**
** Returns          PSM to use.
**
*******************************************************************************/
UINT16 L2CA_AllocatePSM(void)
{
    BOOLEAN done = FALSE;
    UINT16 psm = l2cb.dyn_psm;

    while (!done) {
        psm += 2;
        if (psm > 0xfeff) {
            psm = 0x1001;
        } else if (psm & 0x0100) {
            /* the upper byte must be even */
            psm += 0x0100;
        }

        /* if psm is in range of reserved BRCM Aware features */
        if ((BRCM_RESERVED_PSM_START <= psm) && (psm <= BRCM_RESERVED_PSM_END)) {
            continue;
        }

        /* make sure the newlly allocated psm is not used right now */
        if ((l2cu_find_rcb_by_psm (psm)) == NULL) {
            done = TRUE;
        }
    }
    l2cb.dyn_psm = psm;

    return (psm);
}

/*******************************************************************************
**
** Function         L2CA_ConnectReq
**
** Description      Higher layers call this function to create an L2CAP connection.
**                  Note that the connection is not established at this time, but
**                  connection establishment gets started. The callback function
**                  will be invoked when connection establishes or fails.
**
** Returns          the CID of the connection, or 0 if it failed to start
**
*******************************************************************************/
UINT16 L2CA_ConnectReq (UINT16 psm, BD_ADDR p_bd_addr)
{
    return L2CA_ErtmConnectReq (psm, p_bd_addr, NULL);
}

/*******************************************************************************
**
** Function         L2CA_ErtmConnectReq
**
** Description      Higher layers call this function to create an L2CAP connection.
**                  Note that the connection is not established at this time, but
**                  connection establishment gets started. The callback function
**                  will be invoked when connection establishes or fails.
**
**  Parameters:       PSM: L2CAP PSM for the connection
**                    BD address of the peer
**                   Enhaced retransmission mode configurations

** Returns          the CID of the connection, or 0 if it failed to start
**
*******************************************************************************/
UINT16 L2CA_ErtmConnectReq (UINT16 psm, BD_ADDR p_bd_addr, tL2CAP_ERTM_INFO *p_ertm_info)
{
    tL2C_LCB        *p_lcb;
    tL2C_CCB        *p_ccb;
    tL2C_RCB        *p_rcb;

    //counter_add("l2cap.conn.req", 1);
    L2CAP_TRACE_API ("L2CA_ErtmConnectReq()  PSM: 0x%04x  BDA: %08x%04x  p_ertm_info: %p allowed:0x%x preferred:%d", psm,
                     (p_bd_addr[0] << 24) + (p_bd_addr[1] << 16) + (p_bd_addr[2] << 8) + p_bd_addr[3],
                     (p_bd_addr[4] << 8) + p_bd_addr[5], p_ertm_info,
                     (p_ertm_info) ? p_ertm_info->allowed_modes : 0,
                     (p_ertm_info) ? p_ertm_info->preferred_mode : 0);

    /* Fail if we have not established communications with the controller */
    if (!BTM_IsDeviceUp()) {
        L2CAP_TRACE_WARNING ("L2CAP connect req - BTU not ready");
        return (0);
    }
    /* Fail if the PSM is not registered */
    if ((p_rcb = l2cu_find_rcb_by_psm (psm)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no RCB for L2CA_conn_req, PSM: 0x%04x", psm);
        return (0);
    }

    /* First, see if we already have a link to the remote */
    /* assume all ERTM l2cap connection is going over BR/EDR for now */
    if ((p_lcb = l2cu_find_lcb_by_bd_addr (p_bd_addr, BT_TRANSPORT_BR_EDR)) == NULL) {
        /* No link. Get an LCB and start link establishment */
        if ( ((p_lcb = l2cu_allocate_lcb (p_bd_addr, FALSE, BT_TRANSPORT_BR_EDR)) == NULL)
                /* currently use BR/EDR for ERTM mode l2cap connection */
                ||  (l2cu_create_conn(p_lcb, BT_TRANSPORT_BR_EDR) == FALSE) ) {
            L2CAP_TRACE_WARNING ("L2CAP - conn not started for PSM: 0x%04x  p_lcb: %p", psm, p_lcb);
            return (0);
        }
    }

    /* Allocate a channel control block */
    if ((p_ccb = l2cu_allocate_ccb (p_lcb, 0)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no CCB for L2CA_conn_req, PSM: 0x%04x", psm);
        return (0);
    }

    /* Save registration info */
    p_ccb->p_rcb = p_rcb;

    if (p_ertm_info) {
        p_ccb->ertm_info  = *p_ertm_info;

        /* Replace default indicators with the actual default pool */
        if (p_ccb->ertm_info.fcr_rx_buf_size == L2CAP_INVALID_ERM_BUF_SIZE) {
            p_ccb->ertm_info.fcr_rx_buf_size = L2CAP_FCR_RX_BUF_SIZE;
        }

        if (p_ccb->ertm_info.fcr_tx_buf_size == L2CAP_INVALID_ERM_BUF_SIZE) {
            p_ccb->ertm_info.fcr_tx_buf_size = L2CAP_FCR_TX_BUF_SIZE;
        }

        if (p_ccb->ertm_info.user_rx_buf_size == L2CAP_INVALID_ERM_BUF_SIZE) {
            p_ccb->ertm_info.user_rx_buf_size = L2CAP_USER_RX_BUF_SIZE;
        }

        if (p_ccb->ertm_info.user_tx_buf_size == L2CAP_INVALID_ERM_BUF_SIZE) {
            p_ccb->ertm_info.user_tx_buf_size = L2CAP_USER_TX_BUF_SIZE;
        }

        p_ccb->max_rx_mtu = p_ertm_info->user_rx_buf_size -
            (L2CAP_MIN_OFFSET + L2CAP_SDU_LEN_OFFSET + L2CAP_FCS_LEN);
    }



    /* If link is up, start the L2CAP connection */
    if (p_lcb->link_state == LST_CONNECTED) {
        l2c_csm_execute (p_ccb, L2CEVT_L2CA_CONNECT_REQ, NULL);
    }

    /* If link is disconnecting, save link info to retry after disconnect
     * Possible Race condition when a reconnect occurs
     * on the channel during a disconnect of link. This
     * ccb will be automatically retried after link disconnect
     * arrives
     */
    else if (p_lcb->link_state == LST_DISCONNECTING) {
        L2CAP_TRACE_DEBUG ("L2CAP API - link disconnecting: RETRY LATER");

        /* Save ccb so it can be started after disconnect is finished */
        p_lcb->p_pending_ccb = p_ccb;
    }

    L2CAP_TRACE_API ("L2CAP - L2CA_conn_req(psm: 0x%04x) returned CID: 0x%04x", psm, p_ccb->local_cid);

    /* Return the local CID as our handle */
    return (p_ccb->local_cid);
}

bool L2CA_SetConnectionCallbacks(uint16_t local_cid, const tL2CAP_APPL_INFO *callbacks)
{
    assert(callbacks != NULL);
    assert(callbacks->pL2CA_ConnectInd_Cb == NULL);
    assert(callbacks->pL2CA_ConnectCfm_Cb != NULL);
    assert(callbacks->pL2CA_ConfigInd_Cb != NULL);
    assert(callbacks->pL2CA_ConfigCfm_Cb != NULL);
    assert(callbacks->pL2CA_DisconnectInd_Cb != NULL);
    assert(callbacks->pL2CA_DisconnectCfm_Cb != NULL);
    assert(callbacks->pL2CA_CongestionStatus_Cb != NULL);
    assert(callbacks->pL2CA_DataInd_Cb != NULL);
    assert(callbacks->pL2CA_TxComplete_Cb != NULL);

    tL2C_CCB *channel_control_block = l2cu_find_ccb_by_cid(NULL, local_cid);
    if (!channel_control_block) {
        L2CAP_TRACE_ERROR("%s no channel control block found for L2CAP LCID=0x%04x.", __func__, local_cid);
        return false;
    }

    // We're making a connection-specific registration control block so we check if
    // we already have a private one allocated to us on the heap. If not, we make a
    // new allocation, mark it as heap-allocated, and inherit the fields from the old
    // control block.
    tL2C_RCB *registration_control_block = channel_control_block->p_rcb;
    if (!channel_control_block->should_free_rcb) {
        registration_control_block = (tL2C_RCB *)osi_calloc(sizeof(tL2C_RCB));
        if (!registration_control_block) {
            L2CAP_TRACE_ERROR("%s unable to allocate registration control block.", __func__);
            return false;
        }

        *registration_control_block = *channel_control_block->p_rcb;
        channel_control_block->p_rcb = registration_control_block;
        channel_control_block->should_free_rcb = true;
    }

    registration_control_block->api = *callbacks;
    return true;
}

/*******************************************************************************
**
** Function         L2CA_ConnectRsp
**
** Description      Higher layers call this function to accept an incoming
**                  L2CAP connection, for which they had gotten an connect
**                  indication callback.
**
** Returns          TRUE for success, FALSE for failure
**
*******************************************************************************/
BOOLEAN L2CA_ConnectRsp (BD_ADDR p_bd_addr, UINT8 id, UINT16 lcid,
                         UINT16 result, UINT16 status)
{
    return L2CA_ErtmConnectRsp (p_bd_addr, id, lcid, result, status, NULL);
}


/*******************************************************************************
**
** Function         L2CA_ErtmConnectRsp
**
** Description      Higher layers call this function to accept an incoming
**                  L2CAP connection, for which they had gotten an connect
**                  indication callback.
**
** Returns          TRUE for success, FALSE for failure
**
*******************************************************************************/
BOOLEAN L2CA_ErtmConnectRsp (BD_ADDR p_bd_addr, UINT8 id, UINT16 lcid, UINT16 result,
                             UINT16 status, tL2CAP_ERTM_INFO *p_ertm_info)
{
    tL2C_LCB        *p_lcb;
    tL2C_CCB        *p_ccb;

    //counter_add("l2cap.conn.rsp", 1);
    L2CAP_TRACE_API ("L2CA_ErtmConnectRsp()  CID: 0x%04x  Result: %d  Status: %d  BDA: %08x%04x  p_ertm_info:%p",
                     lcid, result, status,
                     (p_bd_addr[0] << 24) + (p_bd_addr[1] << 16) + (p_bd_addr[2] << 8) + p_bd_addr[3],
                     (p_bd_addr[4] << 8) + p_bd_addr[5], p_ertm_info);

    /* First, find the link control block */
    if ((p_lcb = l2cu_find_lcb_by_bd_addr (p_bd_addr, BT_TRANSPORT_BR_EDR)) == NULL) {
        /* No link. Get an LCB and start link establishment */
        L2CAP_TRACE_WARNING ("L2CAP - no LCB for L2CA_conn_rsp");
        return (FALSE);
    }

    /* Now, find the channel control block */
    if ((p_ccb = l2cu_find_ccb_by_cid (p_lcb, lcid)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no CCB for L2CA_conn_rsp");
        return (FALSE);
    }

    /* The IDs must match */
    if (p_ccb->remote_id != id) {
        L2CAP_TRACE_WARNING ("L2CAP - bad id in L2CA_conn_rsp. Exp: %d  Got: %d", p_ccb->remote_id, id);
        return (FALSE);
    }

    if (p_ertm_info) {
        p_ccb->ertm_info  = *p_ertm_info;

        /* Replace default indicators with the actual default pool */
        if (p_ccb->ertm_info.fcr_rx_buf_size == L2CAP_INVALID_ERM_BUF_SIZE) {
            p_ccb->ertm_info.fcr_rx_buf_size = L2CAP_FCR_RX_BUF_SIZE;
        }

        if (p_ccb->ertm_info.fcr_tx_buf_size == L2CAP_INVALID_ERM_BUF_SIZE) {
            p_ccb->ertm_info.fcr_tx_buf_size = L2CAP_FCR_TX_BUF_SIZE;
        }

        if (p_ccb->ertm_info.user_rx_buf_size == L2CAP_INVALID_ERM_BUF_SIZE) {
            p_ccb->ertm_info.user_rx_buf_size = L2CAP_USER_RX_BUF_SIZE;
        }

        if (p_ccb->ertm_info.user_tx_buf_size == L2CAP_INVALID_ERM_BUF_SIZE) {
            p_ccb->ertm_info.user_tx_buf_size = L2CAP_USER_TX_BUF_SIZE;
        }

        p_ccb->max_rx_mtu = p_ertm_info->user_rx_buf_size -
            (L2CAP_MIN_OFFSET + L2CAP_SDU_LEN_OFFSET + L2CAP_FCS_LEN);
    }

    if (result == L2CAP_CONN_OK) {
        l2c_csm_execute (p_ccb, L2CEVT_L2CA_CONNECT_RSP, NULL);
    } else {
        tL2C_CONN_INFO   conn_info;

        conn_info.l2cap_result = result;
        conn_info.l2cap_status = status;

        if (result == L2CAP_CONN_PENDING) {
            l2c_csm_execute (p_ccb, L2CEVT_L2CA_CONNECT_RSP, &conn_info);
        } else {
            l2c_csm_execute (p_ccb, L2CEVT_L2CA_CONNECT_RSP_NEG, &conn_info);
        }
    }

    return (TRUE);
}


/*******************************************************************************
**
** Function         L2CA_ConfigReq
**
** Description      Higher layers call this function to send configuration.
**
**                  Note:  The FCR options of p_cfg are not used.
**
** Returns          TRUE if configuration sent, else FALSE
**
*******************************************************************************/
BOOLEAN L2CA_ConfigReq (UINT16 cid, tL2CAP_CFG_INFO *p_cfg)
{
    tL2C_CCB        *p_ccb;

    //counter_add("l2cap.cfg.req", 1);
    L2CAP_TRACE_API ("L2CA_ConfigReq()  CID 0x%04x: fcr_present:%d (mode %d) mtu_present:%d (%d)",
                     cid, p_cfg->fcr_present, p_cfg->fcr.mode, p_cfg->mtu_present, p_cfg->mtu);

    /* Find the channel control block. We don't know the link it is on. */
    if ((p_ccb = l2cu_find_ccb_by_cid (NULL, cid)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no CCB for L2CA_cfg_req, CID: %d", cid);
        return (FALSE);
    }

    /* We need to have at least one mode type common with the peer */
    if (!l2c_fcr_adj_our_req_options(p_ccb, p_cfg)) {
        return (FALSE);
    }

    /* Don't adjust FCR options if not used */
    if ((!p_cfg->fcr_present) || (p_cfg->fcr.mode == L2CAP_FCR_BASIC_MODE)) {
        /* FCR and FCS options are not used in basic mode */
        p_cfg->fcs_present = FALSE;
        p_cfg->ext_flow_spec_present = FALSE;

        if ( (p_cfg->mtu_present) && (p_cfg->mtu > L2CAP_MTU_SIZE) ) {
            L2CAP_TRACE_WARNING ("L2CAP - adjust MTU: %u too large", p_cfg->mtu);
            p_cfg->mtu = L2CAP_MTU_SIZE;
        }
    }

    /* Save the adjusted configuration in case it needs to be used for renegotiation */
    p_ccb->our_cfg = *p_cfg;

    l2c_csm_execute (p_ccb, L2CEVT_L2CA_CONFIG_REQ, p_cfg);

    return (TRUE);
}


/*******************************************************************************
**
** Function         L2CA_ConfigRsp
**
** Description      Higher layers call this function to send a configuration
**                  response.
**
** Returns          TRUE if configuration response sent, else FALSE
**
*******************************************************************************/
BOOLEAN L2CA_ConfigRsp (UINT16 cid, tL2CAP_CFG_INFO *p_cfg)
{
    tL2C_CCB        *p_ccb;

    //counter_add("l2cap.cfg.rsp", 1);
    L2CAP_TRACE_API ("L2CA_ConfigRsp()  CID: 0x%04x  Result: %d MTU present:%d Flush TO:%d FCR:%d FCS:%d",
                     cid, p_cfg->result, p_cfg->mtu_present, p_cfg->flush_to_present, p_cfg->fcr_present, p_cfg->fcs_present);

    /* Find the channel control block. We don't know the link it is on. */
    if ((p_ccb = l2cu_find_ccb_by_cid (NULL, cid)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no CCB for L2CA_cfg_rsp, CID: %d", cid);
        return (FALSE);
    }

    if ( (p_cfg->result == L2CAP_CFG_OK) || (p_cfg->result == L2CAP_CFG_PENDING) ) {
        l2c_csm_execute (p_ccb, L2CEVT_L2CA_CONFIG_RSP, p_cfg);
    } else {
        p_cfg->fcr_present = FALSE; /* FCR options already negotiated before this point */

        /* Clear out any cached options that are being returned as an error (excluding FCR) */
        if (p_cfg->mtu_present) {
            p_ccb->peer_cfg.mtu_present = FALSE;
        }
        if (p_cfg->flush_to_present) {
            p_ccb->peer_cfg.flush_to_present = FALSE;
        }
        if (p_cfg->qos_present) {
            p_ccb->peer_cfg.qos_present = FALSE;
        }

        l2c_csm_execute (p_ccb, L2CEVT_L2CA_CONFIG_RSP_NEG, p_cfg);
    }

    return (TRUE);
}


/*******************************************************************************
**
** Function         L2CA_DisconnectReq
**
** Description      Higher layers call this function to disconnect a channel.
**
** Returns          TRUE if disconnect sent, else FALSE
**
*******************************************************************************/
BOOLEAN L2CA_DisconnectReq (UINT16 cid)
{
    tL2C_CCB        *p_ccb;

    //counter_add("l2cap.disconn.req", 1);
    L2CAP_TRACE_API ("L2CA_DisconnectReq()  CID: 0x%04x", cid);

    /* Find the channel control block. We don't know the link it is on. */
    if ((p_ccb = l2cu_find_ccb_by_cid (NULL, cid)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no CCB for L2CA_disc_req, CID: %d", cid);
        return (FALSE);
    }

    l2c_csm_execute (p_ccb, L2CEVT_L2CA_DISCONNECT_REQ, NULL);

    return (TRUE);
}

/*******************************************************************************
**
** Function         L2CA_DisconnectRsp
**
** Description      Higher layers call this function to acknowledge the
**                  disconnection of a channel.
**
** Returns          void
**
*******************************************************************************/
BOOLEAN L2CA_DisconnectRsp (UINT16 cid)
{
    tL2C_CCB        *p_ccb;

    //counter_add("l2cap.disconn.rsp", 1);
    L2CAP_TRACE_API ("L2CA_DisconnectRsp()  CID: 0x%04x", cid);

    /* Find the channel control block. We don't know the link it is on. */
    if ((p_ccb = l2cu_find_ccb_by_cid (NULL, cid)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no CCB for L2CA_disc_rsp, CID: %d", cid);
        return (FALSE);
    }

    l2c_csm_execute (p_ccb, L2CEVT_L2CA_DISCONNECT_RSP, NULL);

    return (TRUE);
}

/*******************************************************************************
**
** Function         L2CA_Ping
**
** Description      Higher layers call this function to send an echo request.
**
** Returns          TRUE if echo request sent, else FALSE.
**
*******************************************************************************/
BOOLEAN  L2CA_Ping (BD_ADDR p_bd_addr, tL2CA_ECHO_RSP_CB *p_callback)
{
    tL2C_LCB        *p_lcb;

    L2CAP_TRACE_API ("L2CA_Ping()  BDA: %02x-%02x-%02x-%02x-%02x-%02x",
                     p_bd_addr[0], p_bd_addr[1], p_bd_addr[2], p_bd_addr[3], p_bd_addr[4], p_bd_addr[5]);

    /* Fail if we have not established communications with the controller */
    if (!BTM_IsDeviceUp()) {
        return (FALSE);
    }

    /* First, see if we already have a link to the remote */
    if ((p_lcb = l2cu_find_lcb_by_bd_addr (p_bd_addr, BT_TRANSPORT_BR_EDR)) == NULL) {
        /* No link. Get an LCB and start link establishment */
        if ((p_lcb = l2cu_allocate_lcb (p_bd_addr, FALSE, BT_TRANSPORT_BR_EDR)) == NULL) {
            L2CAP_TRACE_WARNING ("L2CAP - no LCB for L2CA_ping");
            return (FALSE);
        }
        if (l2cu_create_conn(p_lcb, BT_TRANSPORT_BR_EDR) == FALSE) {
            return (FALSE);
        }

        p_lcb->p_echo_rsp_cb = p_callback;

        return (TRUE);
    }

    /* We only allow 1 ping outstanding at a time */
    if (p_lcb->p_echo_rsp_cb != NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - rejected second L2CA_ping");
        return (FALSE);
    }

    /* Have a link control block. If link is disconnecting, tell user to retry later */
    if (p_lcb->link_state == LST_DISCONNECTING) {
        L2CAP_TRACE_WARNING ("L2CAP - L2CA_ping rejected - link disconnecting");
        return (FALSE);
    }

    /* Save address of callback */
    p_lcb->p_echo_rsp_cb = p_callback;

    if (p_lcb->link_state == LST_CONNECTED) {
        l2cu_adj_id(p_lcb, L2CAP_ADJ_BRCM_ID);  /* Make sure not using Broadcom ID */
        l2cu_send_peer_echo_req (p_lcb, NULL, 0);
        btu_start_timer (&p_lcb->timer_entry, BTU_TTYPE_L2CAP_LINK, L2CAP_ECHO_RSP_TOUT);
    }

    return (TRUE);
}

/*******************************************************************************
**
** Function         L2CA_Echo
**
** Description      Higher layers call this function to send an echo request
**                  with application-specific data.
**
** Returns          TRUE if echo request sent, else FALSE.
**
*******************************************************************************/
BOOLEAN  L2CA_Echo (BD_ADDR p_bd_addr, BT_HDR *p_data, tL2CA_ECHO_DATA_CB *p_callback)
{
    tL2C_LCB    *p_lcb;
    UINT8       *pp;

    L2CAP_TRACE_API ("L2CA_Echo() BDA: %08X%04X",
                     ((p_bd_addr[0] << 24) + (p_bd_addr[1] << 16) + (p_bd_addr[2] <<  8) + (p_bd_addr[3])),
                     ((p_bd_addr[4] <<  8) + (p_bd_addr[5])));

    /* Fail if we have not established communications with the controller */
    if (!BTM_IsDeviceUp()) {
        return (FALSE);
    }

    if ((memcmp(BT_BD_ANY, p_bd_addr, BD_ADDR_LEN) == 0) && (p_data == NULL)) {
        /* Only register callback without sending message. */
        l2cb.p_echo_data_cb = p_callback;
        return TRUE;
    }

    /* We assume the upper layer will call this function only when the link is established. */
    if ((p_lcb = l2cu_find_lcb_by_bd_addr (p_bd_addr, BT_TRANSPORT_BR_EDR)) == NULL) {
        L2CAP_TRACE_ERROR ("L2CA_Echo ERROR : link not established");
        return FALSE;
    }

    if (p_lcb->link_state != LST_CONNECTED) {
        L2CAP_TRACE_ERROR ("L2CA_Echo ERROR : link is not connected");
        return FALSE;
    }

    /* Save address of callback */
    l2cb.p_echo_data_cb = p_callback;

    /* Set the pointer to the beginning of the data */
    pp = (UINT8 *)(p_data + 1) + p_data->offset;
    l2cu_adj_id(p_lcb, L2CAP_ADJ_BRCM_ID);  /* Make sure not using Broadcom ID */
    l2cu_send_peer_echo_req (p_lcb, pp, p_data->len);

    return (TRUE);

}

#endif ///CLASSIC_BT_INCLUDED == TRUE


bool L2CA_GetIdentifiers(uint16_t lcid, uint16_t *rcid, uint16_t *handle)
{
    tL2C_CCB *control_block = l2cu_find_ccb_by_cid(NULL, lcid);
    if (!control_block) {
        return false;
    }

    if (rcid) {
        *rcid = control_block->remote_cid;
    }
    if (handle) {
        *handle = control_block->p_lcb->handle;
    }

    return true;
}

/*******************************************************************************
**
** Function         L2CA_SetIdleTimeout
**
** Description      Higher layers call this function to set the idle timeout for
**                  a connection, or for all future connections. The "idle timeout"
**                  is the amount of time that a connection can remain up with
**                  no L2CAP channels on it. A timeout of zero means that the
**                  connection will be torn down immediately when the last channel
**                  is removed. A timeout of 0xFFFF means no timeout. Values are
**                  in seconds.
**
** Returns          TRUE if command succeeded, FALSE if failed
**
** NOTE             This timeout takes effect after at least 1 channel has been
**                  established and removed. L2CAP maintains its own timer from
**                  whan a connection is established till the first channel is
**                  set up.
*******************************************************************************/
BOOLEAN L2CA_SetIdleTimeout (UINT16 cid, UINT16 timeout, BOOLEAN is_global)
{
    tL2C_CCB        *p_ccb;
    tL2C_LCB        *p_lcb;

    if (is_global) {
        l2cb.idle_timeout = timeout;
    } else {
        /* Find the channel control block. We don't know the link it is on. */
        if ((p_ccb = l2cu_find_ccb_by_cid (NULL, cid)) == NULL) {
            L2CAP_TRACE_WARNING ("L2CAP - no CCB for L2CA_SetIdleTimeout, CID: %d", cid);
            return (FALSE);
        }

        p_lcb = p_ccb->p_lcb;

        if ((p_lcb) && (p_lcb->in_use) && (p_lcb->link_state == LST_CONNECTED)) {
            p_lcb->idle_timeout = timeout;
        } else {
            return (FALSE);
        }
    }

    return (TRUE);
}



/*******************************************************************************
**
** Function         L2CA_SetIdleTimeoutByBdAddr
**
** Description      Higher layers call this function to set the idle timeout for
**                  a connection. The "idle timeout" is the amount of time that
**                  a connection can remain up with no L2CAP channels on it.
**                  A timeout of zero means that the connection will be torn
**                  down immediately when the last channel is removed.
**                  A timeout of 0xFFFF means no timeout. Values are in seconds.
**                  A bd_addr is the remote BD address. If bd_addr = BT_BD_ANY,
**                  then the idle timeouts for all active l2cap links will be
**                  changed.
**
** Returns          TRUE if command succeeded, FALSE if failed
**
** NOTE             This timeout applies to all logical channels active on the
**                  ACL link.
*******************************************************************************/
BOOLEAN L2CA_SetIdleTimeoutByBdAddr(BD_ADDR bd_addr, UINT16 timeout, tBT_TRANSPORT transport)
{
    tL2C_LCB        *p_lcb;

    if (memcmp (BT_BD_ANY, bd_addr, BD_ADDR_LEN)) {
        p_lcb = l2cu_find_lcb_by_bd_addr( bd_addr, transport);
        if ((p_lcb) && (p_lcb->in_use) && (p_lcb->link_state == LST_CONNECTED)) {
            p_lcb->idle_timeout = timeout;

            if (!p_lcb->ccb_queue.p_first_ccb) {
                l2cu_no_dynamic_ccbs (p_lcb);
            }
        } else {
            return FALSE;
        }
    } else {
        int         xx;
        tL2C_LCB    *p_lcb = &l2cb.lcb_pool[0];

        for (xx = 0; xx < MAX_L2CAP_LINKS; xx++, p_lcb++) {
            if ((p_lcb->in_use) && (p_lcb->link_state == LST_CONNECTED)) {
                p_lcb->idle_timeout = timeout;

                if (!p_lcb->ccb_queue.p_first_ccb) {
                    l2cu_no_dynamic_ccbs (p_lcb);
                }
            }
        }
    }

    return TRUE;
}



/*******************************************************************************
**
** Function         L2CA_SetTraceLevel
**
** Description      This function sets the trace level for L2CAP. If called with
**                  a value of 0xFF, it simply reads the current trace level.
**
** Returns          the new (current) trace level
**
*******************************************************************************/
UINT8 L2CA_SetTraceLevel (UINT8 new_level)
{
    if (new_level != 0xFF) {
        l2cb.l2cap_trace_level = new_level;
    }

    return (l2cb.l2cap_trace_level);
}


/*******************************************************************************
**
** Function     L2CA_SetDesireRole
**
** Description  This function sets the desire role for L2CAP.
**              If the new role is L2CAP_ROLE_ALLOW_SWITCH, allow switch on
**              HciCreateConnection.
**              If the new role is L2CAP_ROLE_DISALLOW_SWITCH, do not allow switch on
**              HciCreateConnection.
**
**              If the new role is a valid role (HCI_ROLE_MASTER or HCI_ROLE_SLAVE),
**              the desire role is set to the new value. Otherwise, it is not changed.
**
** Returns      the new (current) role
**
*******************************************************************************/
UINT8 L2CA_SetDesireRole (UINT8 new_role)
{
    L2CAP_TRACE_API ("L2CA_SetDesireRole() new:x%x, disallow_switch:%d",
                     new_role, l2cb.disallow_switch);

    if (L2CAP_ROLE_CHECK_SWITCH != (L2CAP_ROLE_CHECK_SWITCH & new_role)) {
        /* do not process the allow_switch when both bits are set */
        if (new_role & L2CAP_ROLE_ALLOW_SWITCH) {
            l2cb.disallow_switch = FALSE;
        }
        if (new_role & L2CAP_ROLE_DISALLOW_SWITCH) {
            l2cb.disallow_switch = TRUE;
        }
    }

    if (new_role == HCI_ROLE_MASTER || new_role == HCI_ROLE_SLAVE) {
        l2cb.desire_role = new_role;
    }

    return (l2cb.desire_role);
}

#if (CLASSIC_BT_INCLUDED == TRUE)

/*******************************************************************************
**
** Function     L2CA_LocalLoopbackReq
**
** Description  This function sets up a CID for local loopback
**
** Returns      CID of 0 if none.
**
*******************************************************************************/
UINT16 L2CA_LocalLoopbackReq (UINT16 psm, UINT16 handle, BD_ADDR p_bd_addr)
{
    tL2C_LCB        *p_lcb;
    tL2C_CCB        *p_ccb;
    tL2C_RCB        *p_rcb;

    L2CAP_TRACE_API ("L2CA_LocalLoopbackReq()  PSM: %d  Handle: 0x%04x", psm, handle);

    /* Fail if we have not established communications with the controller */
    if (!BTM_IsDeviceUp()) {
        L2CAP_TRACE_WARNING ("L2CAP loop req - BTU not ready");
        return (0);
    }

    /* Fail if the PSM is not registered */
    if ((p_rcb = l2cu_find_rcb_by_psm (psm)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no RCB for L2CA_conn_req, PSM: %d", psm);
        return (0);
    }

    if ((p_lcb = l2cu_allocate_lcb (p_bd_addr, FALSE, BT_TRANSPORT_BR_EDR)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no LCB for L2CA_conn_req");
        return (0);
    }

    p_lcb->link_state = LST_CONNECTED;
    p_lcb->handle     = handle;

    /* Allocate a channel control block */
    if ((p_ccb = l2cu_allocate_ccb (p_lcb, 0)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no CCB for L2CA_conn_req");
        return (0);
    }

    /* Save registration info */
    p_ccb->p_rcb        = p_rcb;
    p_ccb->chnl_state   = CST_OPEN;
    p_ccb->remote_cid   = p_ccb->local_cid;
    p_ccb->config_done  = CFG_DONE_MASK;

    /* Return the local CID as our handle */
    return (p_ccb->local_cid);
}

/*******************************************************************************
**
** Function         L2CA_SetAclPriority
**
** Description      Sets the transmission priority for a channel.
**                  (For initial implementation only two values are valid.
**                  L2CAP_PRIORITY_NORMAL and L2CAP_PRIORITY_HIGH).
**
** Returns          TRUE if a valid channel, else FALSE
**
*******************************************************************************/
BOOLEAN L2CA_SetAclPriority (BD_ADDR bd_addr, UINT8 priority)
{
    L2CAP_TRACE_API ("L2CA_SetAclPriority()  bdaddr: %02x%02x%02x%02x%04x, priority:%d",
                     bd_addr[0], bd_addr[1], bd_addr[2],
                     bd_addr[3], (bd_addr[4] << 8) + bd_addr[5], priority);

    return (l2cu_set_acl_priority(bd_addr, priority, FALSE));
}

/*******************************************************************************
**
** Function         L2CA_FlowControl
**
** Description      Higher layers call this function to flow control a channel.
**
**                  data_enabled - TRUE data flows, FALSE data is stopped
**
** Returns          TRUE if valid channel, else FALSE
**
*******************************************************************************/
BOOLEAN L2CA_FlowControl (UINT16 cid, BOOLEAN data_enabled)
{
    tL2C_CCB  *p_ccb;
    BOOLEAN   on_off = !data_enabled;

    L2CAP_TRACE_API ("L2CA_FlowControl(%d)  CID: 0x%04x", on_off, cid);

    /* Find the channel control block. We don't know the link it is on. */
    if ((p_ccb = l2cu_find_ccb_by_cid (NULL, cid)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no CCB for L2CA_FlowControl, CID: 0x%04x  data_enabled: %d", cid, data_enabled);
        return (FALSE);
    }

    if (p_ccb->peer_cfg.fcr.mode != L2CAP_FCR_ERTM_MODE) {
        L2CAP_TRACE_EVENT ("L2CA_FlowControl()  invalid mode:%d", p_ccb->peer_cfg.fcr.mode);
        return (FALSE);
    }
    if (p_ccb->fcrb.local_busy != on_off) {
        p_ccb->fcrb.local_busy = on_off;

        if ( (p_ccb->chnl_state == CST_OPEN) && (!p_ccb->fcrb.wait_ack) ) {
            if (on_off) {
                l2c_fcr_send_S_frame (p_ccb, L2CAP_FCR_SUP_RNR, 0);
            } else {
                l2c_fcr_send_S_frame (p_ccb, L2CAP_FCR_SUP_RR, L2CAP_FCR_P_BIT);
            }
        }
    }

    return (TRUE);
}

/*******************************************************************************
**
** Function         L2CA_SendTestSFrame
**
** Description      Higher layers call this function to send a test S-frame.
**
** Returns          TRUE if valid Channel, else FALSE
**
*******************************************************************************/
BOOLEAN L2CA_SendTestSFrame (UINT16 cid, UINT8 sup_type, UINT8 back_track)
{
    tL2C_CCB        *p_ccb;

    L2CAP_TRACE_API ("L2CA_SendTestSFrame()  CID: 0x%04x  Type: 0x%02x  back_track: %u", cid, sup_type, back_track);

    /* Find the channel control block. We don't know the link it is on. */
    if ((p_ccb = l2cu_find_ccb_by_cid (NULL, cid)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no CCB for L2CA_SendTestSFrame, CID: %d", cid);
        return (FALSE);
    }

    if ( (p_ccb->chnl_state != CST_OPEN) || (p_ccb->peer_cfg.fcr.mode != L2CAP_FCR_ERTM_MODE) ) {
        return (FALSE);
    }

    p_ccb->fcrb.next_seq_expected -= back_track;

    l2c_fcr_send_S_frame (p_ccb, (UINT16)(sup_type & 3), (UINT16)(sup_type & (L2CAP_FCR_P_BIT | L2CAP_FCR_F_BIT)));

    return (TRUE);
}


/*******************************************************************************
**
** Function         L2CA_SetTxPriority
**
** Description      Sets the transmission priority for a channel.
**
** Returns          TRUE if a valid channel, else FALSE
**
*******************************************************************************/
BOOLEAN L2CA_SetTxPriority (UINT16 cid, tL2CAP_CHNL_PRIORITY priority)
{
    tL2C_CCB        *p_ccb;

    L2CAP_TRACE_API ("L2CA_SetTxPriority()  CID: 0x%04x, priority:%d", cid, priority);

    /* Find the channel control block. We don't know the link it is on. */
    if ((p_ccb = l2cu_find_ccb_by_cid (NULL, cid)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no CCB for L2CA_SetTxPriority, CID: %d", cid);
        return (FALSE);
    }

    /* it will update the order of CCB in LCB by priority and update round robin service variables */
    l2cu_change_pri_ccb (p_ccb, priority);

    return (TRUE);
}

/*******************************************************************************
**
** Function         L2CA_SetChnlDataRate
**
** Description      Sets the tx/rx data rate for a channel.
**
** Returns          TRUE if a valid channel, else FALSE
**
*******************************************************************************/
BOOLEAN L2CA_SetChnlDataRate (UINT16 cid, tL2CAP_CHNL_DATA_RATE tx, tL2CAP_CHNL_DATA_RATE rx)
{
    tL2C_CCB        *p_ccb;

    L2CAP_TRACE_API ("L2CA_SetChnlDataRate()  CID: 0x%04x, tx:%d, rx:%d", cid, tx, rx);

    /* Find the channel control block. We don't know the link it is on. */
    if ((p_ccb = l2cu_find_ccb_by_cid (NULL, cid)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no CCB for L2CA_SetChnlDataRate, CID: %d", cid);
        return (FALSE);
    }

    p_ccb->tx_data_rate = tx;
    p_ccb->rx_data_rate = rx;

    /* Adjust channel buffer allocation */
    l2c_link_adjust_chnl_allocation ();

    return (TRUE);
}

/*******************************************************************************
**
** Function         L2CA_SetFlushTimeout
**
** Description      This function set the automatic flush time out in Baseband
**                  for ACL-U packets.
**                  BdAddr : the remote BD address of ACL link. If it is BT_DB_ANY
**                           then the flush time out will be applied to all ACL link.
**                  FlushTimeout: flush time out in ms
**                           0x0000 : No automatic flush
**                           L2CAP_NO_RETRANSMISSION : No retransmission
**                           0x0002 - 0xFFFE : flush time out, if (flush_tout*8)+3/5)
**                                    <= HCI_MAX_AUTO_FLUSH_TOUT (in 625us slot).
**                                    Otherwise, return FALSE.
**                           L2CAP_NO_AUTOMATIC_FLUSH : No automatic flush
**
** Returns          TRUE if command succeeded, FALSE if failed
**
** NOTE             This flush timeout applies to all logical channels active on the
**                  ACL link.
*******************************************************************************/
BOOLEAN L2CA_SetFlushTimeout (BD_ADDR bd_addr, UINT16 flush_tout)
{
    tL2C_LCB    *p_lcb;
    UINT16      hci_flush_to;
    UINT32      temp;

    /* no automatic flush (infinite timeout) */
    if (flush_tout == 0x0000) {
        hci_flush_to = flush_tout;
        flush_tout   = L2CAP_NO_AUTOMATIC_FLUSH;
    }
    /* no retransmission */
    else if (flush_tout == L2CAP_NO_RETRANSMISSION) {
        /* not mandatory range for controller */
        /* Packet is flushed before getting any ACK/NACK */
        /* To do this, flush timeout should be 1 baseband slot */
        hci_flush_to = flush_tout;
    }
    /* no automatic flush (infinite timeout) */
    else if (flush_tout == L2CAP_NO_AUTOMATIC_FLUSH) {
        hci_flush_to = 0x0000;
    } else {
        /* convert L2CAP flush_to to 0.625 ms units, with round */
        temp = (((UINT32)flush_tout * 8) + 3) / 5;

        /* if L2CAP flush_to within range of HCI, set HCI flush timeout */
        if (temp > HCI_MAX_AUTO_FLUSH_TOUT) {
            L2CAP_TRACE_WARNING("WARNING L2CA_SetFlushTimeout timeout(0x%x) is out of range", flush_tout);
            return FALSE;
        } else {
            hci_flush_to = (UINT16)temp;
        }
    }

    if (memcmp (BT_BD_ANY, bd_addr, BD_ADDR_LEN)) {
        p_lcb = l2cu_find_lcb_by_bd_addr (bd_addr, BT_TRANSPORT_BR_EDR);

        if ((p_lcb) && (p_lcb->in_use) && (p_lcb->link_state == LST_CONNECTED)) {
            if (p_lcb->link_flush_tout != flush_tout) {
                p_lcb->link_flush_tout = flush_tout;

                L2CAP_TRACE_API ("L2CA_SetFlushTimeout 0x%04x ms for bd_addr [...;%02x%02x%02x]",
                                 flush_tout, bd_addr[3], bd_addr[4], bd_addr[5]);

                if (!btsnd_hcic_write_auto_flush_tout (p_lcb->handle, hci_flush_to)) {
                    return (FALSE);
                }
            }
        } else {
            L2CAP_TRACE_WARNING ("WARNING L2CA_SetFlushTimeout No lcb for bd_addr [...;%02x%02x%02x]",
                                 bd_addr[3], bd_addr[4], bd_addr[5]);
            return (FALSE);
        }
    } else {
        int   xx;
        p_lcb = &l2cb.lcb_pool[0];

        for (xx = 0; xx < MAX_L2CAP_LINKS; xx++, p_lcb++) {
            if ((p_lcb->in_use) && (p_lcb->link_state == LST_CONNECTED)) {
                if (p_lcb->link_flush_tout != flush_tout) {
                    p_lcb->link_flush_tout = flush_tout;

                    L2CAP_TRACE_API ("L2CA_SetFlushTimeout 0x%04x ms for bd_addr [...;%02x%02x%02x]",
                                     flush_tout, p_lcb->remote_bd_addr[3],
                                     p_lcb->remote_bd_addr[4], p_lcb->remote_bd_addr[5]);

                    if (!btsnd_hcic_write_auto_flush_tout(p_lcb->handle, hci_flush_to)) {
                        return (FALSE);
                    }
                }
            }
        }
    }

    return (TRUE);
}
#endif  ///CLASSIC_BT_INCLUDED == TRUE


/*******************************************************************************
**
**  Function         L2CA_GetPeerFeatures
**
**  Description      Get a peers features and fixed channel map
**
**  Parameters:      BD address of the peer
**                   Pointers to features and channel mask storage area
**
**  Return value:    TRUE if peer is connected
**
*******************************************************************************/
BOOLEAN L2CA_GetPeerFeatures (BD_ADDR bd_addr, UINT32 *p_ext_feat, UINT8 *p_chnl_mask)
{
    tL2C_LCB        *p_lcb;

    /* We must already have a link to the remote */
    if ((p_lcb = l2cu_find_lcb_by_bd_addr (bd_addr, BT_TRANSPORT_BR_EDR)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CA_GetPeerFeatures() No BDA: %08x%04x",
                             (bd_addr[0] << 24) + (bd_addr[1] << 16) + (bd_addr[2] << 8) + bd_addr[3],
                             (bd_addr[4] << 8) + bd_addr[5]);
        return (FALSE);
    }

    L2CAP_TRACE_API ("L2CA_GetPeerFeatures() BDA: %08x%04x  ExtFea: 0x%08x  Chnl_Mask[0]: 0x%02x",
                     (bd_addr[0] << 24) + (bd_addr[1] << 16) + (bd_addr[2] << 8) + bd_addr[3],
                     (bd_addr[4] << 8) + bd_addr[5], p_lcb->peer_ext_fea, p_lcb->peer_chnl_mask[0]);

    *p_ext_feat = p_lcb->peer_ext_fea;

    memcpy (p_chnl_mask, p_lcb->peer_chnl_mask, L2CAP_FIXED_CHNL_ARRAY_SIZE);

    return (TRUE);
}

/*******************************************************************************
**
**  Function         L2CA_GetBDAddrbyHandle
**
**  Description      Get BD address for the given HCI handle
**
**  Parameters:      HCI handle
**                   BD address of the peer
**
**  Return value:    TRUE if found lcb for the given handle, FALSE otherwise
**
*******************************************************************************/
BOOLEAN L2CA_GetBDAddrbyHandle (UINT16 handle, BD_ADDR bd_addr)
{
    tL2C_LCB *p_lcb = NULL;
    BOOLEAN found_dev = FALSE;

    p_lcb = l2cu_find_lcb_by_handle (handle);
    if (p_lcb) {
        found_dev = TRUE;
        memcpy (bd_addr, p_lcb->remote_bd_addr, BD_ADDR_LEN);
    }

    return found_dev;
}

#if (CLASSIC_BT_INCLUDED == TRUE)
/*******************************************************************************
**
**  Function         L2CA_GetChnlFcrMode
**
**  Description      Get the channel FCR mode
**
**  Parameters:      Local CID
**
**  Return value:    Channel mode
**
*******************************************************************************/
UINT8 L2CA_GetChnlFcrMode (UINT16 lcid)
{
    tL2C_CCB    *p_ccb = l2cu_find_ccb_by_cid (NULL, lcid);

    if (p_ccb) {
        L2CAP_TRACE_API ("L2CA_GetChnlFcrMode() returns mode %d", p_ccb->peer_cfg.fcr.mode);
        return (p_ccb->peer_cfg.fcr.mode);
    }

    L2CAP_TRACE_API ("L2CA_GetChnlFcrMode() returns mode L2CAP_FCR_BASIC_MODE");
    return (L2CAP_FCR_BASIC_MODE);
}

#endif  ///CLASSIC_BT_INCLUDED == TRUE

#if (BLE_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         L2CA_RegisterLECoc
**
** Description      Other layers call this function to register for L2CAP
**                  Connection Oriented Channel.
**
** Returns          PSM to use or zero if error. Typically, the PSM returned
**                  is the same as was passed in, but for an outgoing-only
**                  connection to a dynamic PSM, a "virtual" PSM is returned
**                  and should be used in the calls to L2CA_ConnectLECocReq()
**                  and L2CA_DeregisterLECoc()
**
*******************************************************************************/
UINT16 L2CA_RegisterLECoc(UINT16 psm, tL2CAP_APPL_INFO *p_cb_info)
{
    L2CAP_TRACE_API("%s called for LE PSM: 0x%04x", __func__, psm);

    /* Verify that the required callback info has been filled in
    **      Note:  Connection callbacks are required but not checked
    **             for here because it is possible to be only a client
    **             or only a server.
    */
    if ((!p_cb_info->pL2CA_DataInd_Cb)
     || (!p_cb_info->pL2CA_DisconnectInd_Cb))
    {
        L2CAP_TRACE_ERROR("%s No cb registering BLE PSM: 0x%04x", __func__, psm);
        return 0;
    }

    /* Verify PSM is valid */
    if (!L2C_IS_VALID_LE_PSM(psm))
    {
        L2CAP_TRACE_ERROR("%s Invalid BLE PSM value, PSM: 0x%04x", __func__, psm);
        return 0;
    }

    tL2C_RCB    *p_rcb;
    UINT16      vpsm = psm;

    /* Check if this is a registration for an outgoing-only connection to */
    /* a dynamic PSM. If so, allocate a "virtual" PSM for the app to use. */
    if ((psm >= 0x0080) && (p_cb_info->pL2CA_ConnectInd_Cb == NULL))
    {
        for (vpsm = 0x0080; vpsm < 0x0100; vpsm++)
        {
            p_rcb = l2cu_find_ble_rcb_by_psm(vpsm);
            if (p_rcb == NULL) {
                break;
            }
        }

        L2CAP_TRACE_API("%s Real PSM: 0x%04x  Virtual PSM: 0x%04x", __func__, psm, vpsm);
    }

    /* If registration block already there, just overwrite it */
    p_rcb = l2cu_find_ble_rcb_by_psm(vpsm);
    if (p_rcb == NULL)
    {
        p_rcb = l2cu_allocate_ble_rcb(vpsm);
        if (p_rcb == NULL)
        {
            L2CAP_TRACE_WARNING("%s No BLE RCB available, PSM: 0x%04x  vPSM: 0x%04x",
                  __func__, psm, vpsm);
            return 0;
        }
    }

    p_rcb->api      = *p_cb_info;
    p_rcb->real_psm = psm;

    return vpsm;
}

/*******************************************************************************
**
** Function         L2CA_DeregisterLECoc
**
** Description      Other layers call this function to de-register for L2CAP
**                  Connection Oriented Channel.
**
** Returns          void
**
*******************************************************************************/
void L2CA_DeregisterLECoc(UINT16 psm)
{
    L2CAP_TRACE_API("%s called for PSM: 0x%04x", __func__, psm);

    tL2C_RCB *p_rcb = l2cu_find_ble_rcb_by_psm(psm);
    if (p_rcb == NULL)
    {
        L2CAP_TRACE_WARNING("%s PSM: 0x%04x not found for deregistration", __func__, psm);
        return;
    }

    tL2C_LCB *p_lcb = &l2cb.lcb_pool[0];
    for (int i = 0; i < MAX_L2CAP_LINKS; i++, p_lcb++)
    {
        if (!p_lcb->in_use || p_lcb->transport != BT_TRANSPORT_LE) {
            continue;
        }

        tL2C_CCB *p_ccb = p_lcb->ccb_queue.p_first_ccb;
        if ((p_ccb == NULL) || (p_lcb->link_state == LST_DISCONNECTING)) {
            continue;
        }

        if (p_ccb->in_use &&
           (p_ccb->chnl_state == CST_W4_L2CAP_DISCONNECT_RSP ||
            p_ccb->chnl_state == CST_W4_L2CA_DISCONNECT_RSP)) {
            continue;
        }

        if (p_ccb->p_rcb == p_rcb) {
            l2c_csm_execute(p_ccb, L2CEVT_L2CA_DISCONNECT_REQ, NULL);
        }
    }

    l2cu_release_rcb (p_rcb);
}

/*******************************************************************************
**
** Function         L2CA_ConnectLECocReq
**
** Description      Higher layers call this function to create an L2CAP connection.
**                  Note that the connection is not established at this time, but
**                  connection establishment gets started. The callback function
**                  will be invoked when connection establishes or fails.
**
**  Parameters:     PSM: L2CAP PSM for the connection
**                  BD address of the peer
**                  Local Coc configurations

** Returns          the CID of the connection, or 0 if it failed to start
**
*******************************************************************************/
UINT16 L2CA_ConnectLECocReq(UINT16 psm, BD_ADDR p_bd_addr, tL2CAP_LE_CFG_INFO *p_cfg)
{
    L2CAP_TRACE_API("%s PSM: 0x%04x BDA: %02x:%02x:%02x:%02x:%02x:%02x", __func__, psm,
        p_bd_addr[0], p_bd_addr[1], p_bd_addr[2], p_bd_addr[3], p_bd_addr[4], p_bd_addr[5]);

    /* Fail if we have not established communications with the controller */
    if (!BTM_IsDeviceUp())
    {
        L2CAP_TRACE_WARNING("%s BTU not ready", __func__);
        return 0;
    }

    /* Fail if the PSM is not registered */
    tL2C_RCB *p_rcb = l2cu_find_ble_rcb_by_psm(psm);
    if (p_rcb == NULL)
    {
        L2CAP_TRACE_WARNING("%s No BLE RCB, PSM: 0x%04x", __func__, psm);
        return 0;
    }

    /* First, see if we already have a le link to the remote */
    tL2C_LCB *p_lcb = l2cu_find_lcb_by_bd_addr(p_bd_addr, BT_TRANSPORT_LE);
    if (p_lcb == NULL)
    {
        /* No link. Get an LCB and start link establishment */
        p_lcb = l2cu_allocate_lcb(p_bd_addr, FALSE, BT_TRANSPORT_LE);
        if ((p_lcb == NULL)
             /* currently use BR/EDR for ERTM mode l2cap connection */
         || (l2cu_create_conn(p_lcb, BT_TRANSPORT_LE) == FALSE) )
        {
            L2CAP_TRACE_WARNING("%s conn not started for PSM: 0x%04x  p_lcb: 0x%p",
                __func__, psm, p_lcb);
            return 0;
        }
    }

    /* Allocate a channel control block */
    tL2C_CCB *p_ccb = l2cu_allocate_ccb(p_lcb, 0);
    if (p_ccb == NULL)
    {
        L2CAP_TRACE_WARNING("%s no CCB, PSM: 0x%04x", __func__, psm);
        return 0;
    }

    /* Save registration info */
    p_ccb->p_rcb = p_rcb;

    /* Save the configuration */
    if (p_cfg) {
        memcpy(&p_ccb->local_conn_cfg, p_cfg, sizeof(tL2CAP_LE_CFG_INFO));
    }

    /* If link is up, start the L2CAP connection */
    if (p_lcb->link_state == LST_CONNECTED)
    {
        if (p_ccb->p_lcb->transport == BT_TRANSPORT_LE)
        {
            L2CAP_TRACE_DEBUG("%s LE Link is up", __func__);
            l2c_csm_execute(p_ccb, L2CEVT_L2CA_CONNECT_REQ, NULL);
        }
    }

    /* If link is disconnecting, save link info to retry after disconnect
     * Possible Race condition when a reconnect occurs
     * on the channel during a disconnect of link. This
     * ccb will be automatically retried after link disconnect
     * arrives
     */
    else if (p_lcb->link_state == LST_DISCONNECTING)
    {
        L2CAP_TRACE_DEBUG("%s link disconnecting: RETRY LATER", __func__);

        /* Save ccb so it can be started after disconnect is finished */
        p_lcb->p_pending_ccb = p_ccb;
    }

    L2CAP_TRACE_API("%s(psm: 0x%04x) returned CID: 0x%04x", __func__, psm, p_ccb->local_cid);

    /* Return the local CID as our handle */
    return p_ccb->local_cid;
}

/*******************************************************************************
**
** Function         L2CA_ConnectLECocRsp
**
** Description      Higher layers call this function to accept an incoming
**                  L2CAP COC connection, for which they had gotten an connect
**                  indication callback.
**
** Returns          TRUE for success, FALSE for failure
**
*******************************************************************************/
BOOLEAN L2CA_ConnectLECocRsp (BD_ADDR p_bd_addr, UINT8 id, UINT16 lcid, UINT16 result,
                             UINT16 status, tL2CAP_LE_CFG_INFO *p_cfg)
{
    L2CAP_TRACE_API("%s CID: 0x%04x Result: %d Status: %d BDA: %02x:%02x:%02x:%02x:%02x:%02x",
        __func__, lcid, result, status,
        p_bd_addr[0], p_bd_addr[1], p_bd_addr[2], p_bd_addr[3], p_bd_addr[4], p_bd_addr[5]);


    /* First, find the link control block */
    tL2C_LCB *p_lcb = l2cu_find_lcb_by_bd_addr(p_bd_addr, BT_TRANSPORT_LE);
    if (p_lcb == NULL)
    {
        /* No link. Get an LCB and start link establishment */
        L2CAP_TRACE_WARNING("%s no LCB", __func__);
        return FALSE;
    }

    /* Now, find the channel control block */
    tL2C_CCB *p_ccb = l2cu_find_ccb_by_cid(p_lcb, lcid);
    if (p_ccb == NULL)
    {
        L2CAP_TRACE_WARNING("%s no CCB", __func__);
        return FALSE;
    }

    /* The IDs must match */
    if (p_ccb->remote_id != id)
    {
        L2CAP_TRACE_WARNING("%s bad id. Expected: %d  Got: %d", __func__, p_ccb->remote_id, id);
        return FALSE;
    }

    if (p_cfg) {
        memcpy(&p_ccb->local_conn_cfg, p_cfg, sizeof(tL2CAP_LE_CFG_INFO));
    }

    if (result == L2CAP_CONN_OK)
        l2c_csm_execute (p_ccb, L2CEVT_L2CA_CONNECT_RSP, NULL);
    else
    {
        tL2C_CONN_INFO conn_info;
        memcpy(conn_info.bd_addr, p_bd_addr, BD_ADDR_LEN);
        conn_info.l2cap_result = result;
        conn_info.l2cap_status = status;
        l2c_csm_execute(p_ccb, L2CEVT_L2CA_CONNECT_RSP_NEG, &conn_info);
    }

    return TRUE;
}

/*******************************************************************************
**
**  Function         L2CA_GetPeerLECocConfig
**
**  Description      Get a peers configuration for LE Connection Oriented Channel.
**
**  Parameters:      local channel id
**                   Pointers to peers configuration storage area
**
**  Return value:    TRUE if peer is connected
**
*******************************************************************************/
BOOLEAN L2CA_GetPeerLECocConfig (UINT16 lcid, tL2CAP_LE_CFG_INFO* peer_cfg)
{
    L2CAP_TRACE_API ("%s CID: 0x%04x", __func__, lcid);

    tL2C_CCB *p_ccb = l2cu_find_ccb_by_cid(NULL, lcid);
    if (p_ccb == NULL)
    {
        L2CAP_TRACE_ERROR("%s No CCB for CID:0x%04x", __func__, lcid);
        return FALSE;
    }

    if (peer_cfg != NULL) {
        memcpy(peer_cfg, &p_ccb->peer_conn_cfg, sizeof(tL2CAP_LE_CFG_INFO));
    }

    return TRUE;
}
#endif  ///BLE_INCLUDED == TRUE

#if (L2CAP_NUM_FIXED_CHNLS > 0)
/*******************************************************************************
**
**  Function        L2CA_RegisterFixedChannel
**
**  Description     Register a fixed channel.
**
**  Parameters:     Fixed Channel #
**                  Channel Callbacks and config
**
**  Return value:   -
**
*******************************************************************************/
BOOLEAN  L2CA_RegisterFixedChannel (UINT16 fixed_cid, tL2CAP_FIXED_CHNL_REG *p_freg)
{
    L2CAP_TRACE_DEBUG ("L2CA_RegisterFixedChannel()  CID: 0x%04x, %p", fixed_cid,p_freg);
    if ( (fixed_cid < L2CAP_FIRST_FIXED_CHNL) || (fixed_cid > L2CAP_LAST_FIXED_CHNL) ) {
        L2CAP_TRACE_ERROR ("L2CA_RegisterFixedChannel()  Invalid CID: 0x%04x", fixed_cid);

        return (FALSE);
    }

    l2cb.fixed_reg[fixed_cid - L2CAP_FIRST_FIXED_CHNL] = *p_freg;
    return (TRUE);
}

/*******************************************************************************
**
**  Function        L2CA_ConnectFixedChnl
**
**  Description     Connect an fixed signalling channel to a remote device.
**
**  Parameters:     Fixed CID
**                  BD Address of remote
**                  BD Address type
**
**  Return value:   TRUE if connection started
**
*******************************************************************************/
BOOLEAN L2CA_ConnectFixedChnl (UINT16 fixed_cid, BD_ADDR rem_bda, tBLE_ADDR_TYPE bd_addr_type)
{
    tL2C_LCB *p_lcb;
    tBT_TRANSPORT transport = BT_TRANSPORT_BR_EDR;

    L2CAP_TRACE_API  ("%s() CID: 0x%04x  BDA: %08x%04x", __func__, fixed_cid,
                      (rem_bda[0] << 24) + (rem_bda[1] << 16) + (rem_bda[2] << 8) + rem_bda[3], (rem_bda[4] << 8) + rem_bda[5]);

    // Check CID is valid and registered
    if ( (fixed_cid < L2CAP_FIRST_FIXED_CHNL) || (fixed_cid > L2CAP_LAST_FIXED_CHNL)
            ||  (l2cb.fixed_reg[fixed_cid - L2CAP_FIRST_FIXED_CHNL].pL2CA_FixedData_Cb == NULL) ) {
        L2CAP_TRACE_ERROR ("%s() Invalid CID: 0x%04x", __func__, fixed_cid);
        return (FALSE);
    }

    // Fail if BT is not yet up
    if (!BTM_IsDeviceUp()) {
        L2CAP_TRACE_WARNING ("%s(0x%04x) - BTU not ready", __func__, fixed_cid);
        return (FALSE);
    }

#if BLE_INCLUDED == TRUE
    if (fixed_cid >= L2CAP_ATT_CID && fixed_cid <= L2CAP_SMP_CID) {
        transport = BT_TRANSPORT_LE;
    }
#endif

    tL2C_BLE_FIXED_CHNLS_MASK peer_channel_mask;

    // If we already have a link to the remote, check if it supports that CID
    if ((p_lcb = l2cu_find_lcb_by_bd_addr (rem_bda, transport)) != NULL) {
        // Fixed channels are mandatory on LE transports so ignore the received
        // channel mask and use the locally cached LE channel mask.

#if BLE_INCLUDED == TRUE
        if (transport == BT_TRANSPORT_LE) {
            peer_channel_mask = l2cb.l2c_ble_fixed_chnls_mask;
        } else
#endif
        {
            peer_channel_mask = p_lcb->peer_chnl_mask[0];
        }

        // Check for supported channel
        if (!(peer_channel_mask & (1 << fixed_cid))) {
            L2CAP_TRACE_EVENT  ("%s() CID:0x%04x  BDA: %08x%04x not supported", __func__,
                                fixed_cid, (rem_bda[0] << 24) + (rem_bda[1] << 16) + (rem_bda[2] << 8) + rem_bda[3],
                                (rem_bda[4] << 8) + rem_bda[5]);
            return FALSE;
        }

        // Get a CCB and link the lcb to it
        if (!l2cu_initialize_fixed_ccb (p_lcb, fixed_cid,
                                        &l2cb.fixed_reg[fixed_cid - L2CAP_FIRST_FIXED_CHNL].fixed_chnl_opts)) {
            L2CAP_TRACE_WARNING ("%s(0x%04x) - LCB but no CCB", __func__, fixed_cid);
            return FALSE;
        }

        // racing with disconnecting, queue the connection request
        if (p_lcb->link_state == LST_DISCONNECTING) {
            L2CAP_TRACE_DEBUG ("%s() - link disconnecting: RETRY LATER", __func__);
            /* Save ccb so it can be started after disconnect is finished */
            p_lcb->p_pending_ccb = p_lcb->p_fixed_ccbs[fixed_cid - L2CAP_FIRST_FIXED_CHNL];
            return TRUE;
        }

        (*l2cb.fixed_reg[fixed_cid - L2CAP_FIRST_FIXED_CHNL].pL2CA_FixedConn_Cb)
        (fixed_cid, p_lcb->remote_bd_addr, TRUE, 0, transport);

        return TRUE;
    }

    // No link. Get an LCB and start link establishment
    if ((p_lcb = l2cu_allocate_lcb (rem_bda, FALSE, transport)) == NULL) {
        L2CAP_TRACE_WARNING ("%s(0x%04x) - no LCB", __func__, fixed_cid);
        return FALSE;
    }

    // Get a CCB and link the lcb to it
    if (!l2cu_initialize_fixed_ccb (p_lcb, fixed_cid,
                                    &l2cb.fixed_reg[fixed_cid - L2CAP_FIRST_FIXED_CHNL].fixed_chnl_opts)) {
        p_lcb->disc_reason = L2CAP_CONN_NO_RESOURCES;
        L2CAP_TRACE_WARNING ("%s(0x%04x) - no CCB", __func__, fixed_cid);
        l2cu_release_lcb (p_lcb);
        return FALSE;
    }
#if (BLE_INCLUDED == TRUE)
    p_lcb->open_addr_type = bd_addr_type;
#endif
    if (!l2cu_create_conn(p_lcb, transport)) {
        L2CAP_TRACE_WARNING ("%s() - create_conn failed", __func__);
        l2cu_release_lcb (p_lcb);
        return FALSE;
    }
    return TRUE;
}

/*******************************************************************************
**
**  Function        L2CA_SendFixedChnlData
**
**  Description     Write data on a fixed channel.
**
**  Parameters:     Fixed CID
**                  BD Address of remote
**                  Pointer to buffer of type BT_HDR
**
** Return value     L2CAP_DW_SUCCESS, if data accepted
**                  L2CAP_DW_FAILED,  if error
**
*******************************************************************************/
UINT16 L2CA_SendFixedChnlData (UINT16 fixed_cid, BD_ADDR rem_bda, BT_HDR *p_buf)
{
    tL2C_LCB        *p_lcb;
    tBT_TRANSPORT   transport = BT_TRANSPORT_BR_EDR;

    L2CAP_TRACE_API ("L2CA_SendFixedChnlData()  CID: 0x%04x  BDA: %08x%04x", fixed_cid,
                     (rem_bda[0] << 24) + (rem_bda[1] << 16) + (rem_bda[2] << 8) + rem_bda[3], (rem_bda[4] << 8) + rem_bda[5]);

#if BLE_INCLUDED == TRUE
    if (fixed_cid >= L2CAP_ATT_CID && fixed_cid <= L2CAP_SMP_CID) {
        transport = BT_TRANSPORT_LE;
    }
#endif

    // Check CID is valid and registered
    if ( (fixed_cid < L2CAP_FIRST_FIXED_CHNL) || (fixed_cid > L2CAP_LAST_FIXED_CHNL)
            ||  (l2cb.fixed_reg[fixed_cid - L2CAP_FIRST_FIXED_CHNL].pL2CA_FixedData_Cb == NULL) ) {
        L2CAP_TRACE_ERROR ("L2CA_SendFixedChnlData()  Invalid CID: 0x%04x", fixed_cid);
        osi_free (p_buf);
        return (L2CAP_DW_FAILED);
    }

    // Fail if BT is not yet up
    if (!BTM_IsDeviceUp()) {
        L2CAP_TRACE_WARNING ("L2CA_SendFixedChnlData(0x%04x) - BTU not ready", fixed_cid);
        osi_free (p_buf);
        return (L2CAP_DW_FAILED);
    }

    // We need to have a link up
    if ((p_lcb = l2cu_find_lcb_by_bd_addr (rem_bda, transport)) == NULL ||
            /* if link is disconnecting, also report data sending failure */
            p_lcb->link_state == LST_DISCONNECTING) {
        L2CAP_TRACE_WARNING ("L2CA_SendFixedChnlData(0x%04x) - no LCB", fixed_cid);
        osi_free (p_buf);
        return (L2CAP_DW_FAILED);
    }

    tL2C_BLE_FIXED_CHNLS_MASK peer_channel_mask;

    // Select peer channels mask to use depending on transport
#if BLE_INCLUDED == TRUE
    if (transport == BT_TRANSPORT_LE) {
        peer_channel_mask = l2cb.l2c_ble_fixed_chnls_mask;
    } else
#endif
    {
        peer_channel_mask = p_lcb->peer_chnl_mask[0];
    }

    if ((peer_channel_mask & (1 << fixed_cid)) == 0) {
        L2CAP_TRACE_WARNING ("L2CA_SendFixedChnlData() - peer does not support fixed chnl: 0x%04x", fixed_cid);
        osi_free (p_buf);
        return (L2CAP_DW_FAILED);
    }

    p_buf->event = 0;
    p_buf->layer_specific = L2CAP_FLUSHABLE_CH_BASED;

    if (!p_lcb->p_fixed_ccbs[fixed_cid - L2CAP_FIRST_FIXED_CHNL]) {
        if (!l2cu_initialize_fixed_ccb (p_lcb, fixed_cid, &l2cb.fixed_reg[fixed_cid - L2CAP_FIRST_FIXED_CHNL].fixed_chnl_opts)) {
            L2CAP_TRACE_WARNING ("L2CA_SendFixedChnlData() - no CCB for chnl: 0x%4x", fixed_cid);
            osi_free (p_buf);
            return (L2CAP_DW_FAILED);
        }
    }

    // If already congested, do not accept any more packets
    if (p_lcb->p_fixed_ccbs[fixed_cid - L2CAP_FIRST_FIXED_CHNL]->cong_sent && fixed_cid != L2CAP_SMP_CID) {
        L2CAP_TRACE_DEBUG ("L2CAP - CID: 0x%04x cannot send, already congested\
            xmit_hold_q.count: %u buff_quota: %u", fixed_cid,
            fixed_queue_length(p_lcb->p_fixed_ccbs[fixed_cid - L2CAP_FIRST_FIXED_CHNL]->xmit_hold_q),
            p_lcb->p_fixed_ccbs[fixed_cid - L2CAP_FIRST_FIXED_CHNL]->buff_quota);
        osi_free(p_buf);
        return (L2CAP_DW_CONGESTED);
    }

    l2c_enqueue_peer_data (p_lcb->p_fixed_ccbs[fixed_cid - L2CAP_FIRST_FIXED_CHNL], p_buf);

    l2c_link_check_send_pkts (p_lcb, NULL, NULL);

    // If there is no dynamic CCB on the link, restart the idle timer each time something is sent
    if (p_lcb->in_use && p_lcb->link_state == LST_CONNECTED && !p_lcb->ccb_queue.p_first_ccb) {
        l2cu_no_dynamic_ccbs (p_lcb);
    }

    if (p_lcb->p_fixed_ccbs[fixed_cid - L2CAP_FIRST_FIXED_CHNL]->cong_sent) {
        return (L2CAP_DW_CONGESTED);
    }

    return (L2CAP_DW_SUCCESS);
}

BOOLEAN L2CA_CheckIsCongest(UINT16 fixed_cid, UINT16 handle)
{
    tL2C_LCB *p_lcb;
    p_lcb = l2cu_find_lcb_by_handle(handle);

    if (p_lcb != NULL && p_lcb->p_fixed_ccbs[fixed_cid - L2CAP_FIRST_FIXED_CHNL] != NULL) {
        return p_lcb->p_fixed_ccbs[fixed_cid - L2CAP_FIRST_FIXED_CHNL]->cong_sent;
    }

    return TRUE;
}

#if (BLE_INCLUDED == TRUE)
UINT16 L2CA_GetFreePktBufferNum_LE(void)
{
    return l2cb.controller_le_xmit_window;
}
UINT16 L2CA_GetCurFreePktBufferNum_LE(UINT16 conn_id)
{
    uint16_t num = 0;
    tl2c_buff_param_t param;
    param.conn_id = conn_id;
    param.get_num = &num;
    l2ble_update_att_acl_pkt_num(L2CA_GET_ATT_NUM, &param);
    return num;
}
#endif

/*******************************************************************************
**
**  Function        L2CA_RemoveFixedChnl
**
**  Description     Remove a fixed channel to a remote device.
**
**  Parameters:     Fixed CID
**                  BD Address of remote
**                  Idle timeout to use (or 0xFFFF if don't care)
**
**  Return value:   TRUE if channel removed
**
*******************************************************************************/
BOOLEAN L2CA_RemoveFixedChnl (UINT16 fixed_cid, BD_ADDR rem_bda)
{
    tL2C_LCB    *p_lcb;
    tL2C_CCB    *p_ccb;
    tBT_TRANSPORT   transport = BT_TRANSPORT_BR_EDR;

    /* Check CID is valid and registered */
    if ( (fixed_cid < L2CAP_FIRST_FIXED_CHNL) || (fixed_cid > L2CAP_LAST_FIXED_CHNL)
            ||  (l2cb.fixed_reg[fixed_cid - L2CAP_FIRST_FIXED_CHNL].pL2CA_FixedData_Cb == NULL) ) {
        L2CAP_TRACE_ERROR ("L2CA_RemoveFixedChnl()  Invalid CID: 0x%04x", fixed_cid);
        return (FALSE);
    }

#if BLE_INCLUDED == TRUE
    if (fixed_cid >= L2CAP_ATT_CID && fixed_cid <= L2CAP_SMP_CID) {
        transport = BT_TRANSPORT_LE;
    }
#endif

    /* Is a fixed channel connected to the remote BDA ?*/
    p_lcb = l2cu_find_lcb_by_bd_addr (rem_bda, transport);

    if ( ((p_lcb) == NULL) || (!p_lcb->p_fixed_ccbs[fixed_cid - L2CAP_FIRST_FIXED_CHNL]) ) {
        L2CAP_TRACE_DEBUG ("L2CA_RemoveFixedChnl()  CID: 0x%04x  BDA: %08x%04x not connected", fixed_cid,
                             (rem_bda[0] << 24) + (rem_bda[1] << 16) + (rem_bda[2] << 8) + rem_bda[3], (rem_bda[4] << 8) + rem_bda[5]);
        return (FALSE);
    }

    L2CAP_TRACE_API ("L2CA_RemoveFixedChnl()  CID: 0x%04x  BDA: %08x%04x", fixed_cid,
                     (rem_bda[0] << 24) + (rem_bda[1] << 16) + (rem_bda[2] << 8) + rem_bda[3], (rem_bda[4] << 8) + rem_bda[5]);

    /* Release the CCB, starting an inactivity timeout on the LCB if no other CCBs exist */
    p_ccb = p_lcb->p_fixed_ccbs[fixed_cid - L2CAP_FIRST_FIXED_CHNL];

    p_lcb->p_fixed_ccbs[fixed_cid - L2CAP_FIRST_FIXED_CHNL] = NULL;
    p_lcb->disc_reason = HCI_ERR_CONN_CAUSE_LOCAL_HOST;

#if BLE_INCLUDED == TRUE
    // Retain the link for a few more seconds after SMP pairing is done, since the Android
    // platform always does service discovery after pairing is complete. This will avoid
    // the link down (pairing is complete) and an immediate re-connection for service
    // discovery.
    // Some devices do not do auto advertising when link is dropped, thus fail the second
    // connection and service discovery.
    if ((fixed_cid == L2CAP_ATT_CID ) && !p_lcb->ccb_queue.p_first_ccb) {
        p_lcb->idle_timeout = 0;
    }
#endif

    l2cu_release_ccb (p_ccb);

    return (TRUE);
}

#if BLE_INCLUDED == TRUE
BOOLEAN L2CA_BleDisconnect (BD_ADDR rem_bda)
{
    tL2C_LCB    *p_lcb;
    tGATT_TCB   *p_tcb;

    p_lcb = l2cu_find_lcb_by_bd_addr (rem_bda, BT_TRANSPORT_LE);
    if (p_lcb == NULL) {
        return FALSE;
    }

    if (p_lcb->link_state != LST_CONNECTED) {
        return FALSE;
    }

    p_lcb->disc_reason = HCI_ERR_CONN_CAUSE_LOCAL_HOST;
    p_lcb->link_state = LST_DISCONNECTING;
    btsnd_hcic_disconnect (p_lcb->handle, HCI_ERR_PEER_USER);

    p_tcb = gatt_find_tcb_by_addr(rem_bda, BT_TRANSPORT_LE);
    if (p_tcb == NULL) {
        return FALSE;
    }

    gatt_set_ch_state(p_tcb, GATT_CH_CLOSING);

    return TRUE;
}
#endif

/*******************************************************************************
**
** Function         L2CA_SetFixedChannelTout
**
** Description      Higher layers call this function to set the idle timeout for
**                  a fixed channel. The "idle timeout" is the amount of time that
**                  a connection can remain up with no L2CAP channels on it.
**                  A timeout of zero means that the connection will be torn
**                  down immediately when the last channel is removed.
**                  A timeout of 0xFFFF means no timeout. Values are in seconds.
**                  A bd_addr is the remote BD address. If bd_addr = BT_BD_ANY,
**                  then the idle timeouts for all active l2cap links will be
**                  changed.
**
** Returns          TRUE if command succeeded, FALSE if failed
**
*******************************************************************************/
BOOLEAN L2CA_SetFixedChannelTout (BD_ADDR rem_bda, UINT16 fixed_cid, UINT16 idle_tout)
{
    tL2C_LCB        *p_lcb;
    tBT_TRANSPORT   transport = BT_TRANSPORT_BR_EDR;

#if BLE_INCLUDED == TRUE
    if (fixed_cid >= L2CAP_ATT_CID && fixed_cid <= L2CAP_SMP_CID) {
        transport = BT_TRANSPORT_LE;
    }
#endif
    if (fixed_cid<L2CAP_FIRST_FIXED_CHNL) {
        return (FALSE);
    }

    /* Is a fixed channel connected to the remote BDA ?*/
    p_lcb = l2cu_find_lcb_by_bd_addr (rem_bda, transport);
    if ( ((p_lcb) == NULL) || (!p_lcb->p_fixed_ccbs[fixed_cid - L2CAP_FIRST_FIXED_CHNL]) ) {
        L2CAP_TRACE_WARNING ("L2CA_SetFixedChannelTout()  CID: 0x%04x  BDA: %08x%04x not connected", fixed_cid,
                             (rem_bda[0] << 24) + (rem_bda[1] << 16) + (rem_bda[2] << 8) + rem_bda[3], (rem_bda[4] << 8) + rem_bda[5]);
        return (FALSE);
    }

    p_lcb->p_fixed_ccbs[fixed_cid - L2CAP_FIRST_FIXED_CHNL]->fixed_chnl_idle_tout = idle_tout;

    if (p_lcb->in_use && p_lcb->link_state == LST_CONNECTED && !p_lcb->ccb_queue.p_first_ccb) {
        /* If there are no dynamic CCBs, (re)start the idle timer in case we changed it */
        l2cu_no_dynamic_ccbs (p_lcb);
    }

    return TRUE;
}

#endif /* #if (L2CAP_NUM_FIXED_CHNLS > 0) */

#if (CLASSIC_BT_INCLUDED == TRUE)
/*******************************************************************************
**
** Function     L2CA_GetCurrentConfig
**
** Description  This function returns configurations of L2CAP channel
**              pp_our_cfg : pointer of our saved configuration options
**              p_our_cfg_bits : valid config in bitmap
**              pp_peer_cfg: pointer of peer's saved configuration options
**              p_peer_cfg_bits : valid config in bitmap
**
** Returns      TRUE if successful
**
*******************************************************************************/
BOOLEAN L2CA_GetCurrentConfig (UINT16 lcid,
                               tL2CAP_CFG_INFO **pp_our_cfg,  tL2CAP_CH_CFG_BITS *p_our_cfg_bits,
                               tL2CAP_CFG_INFO **pp_peer_cfg, tL2CAP_CH_CFG_BITS *p_peer_cfg_bits)
{
    tL2C_CCB    *p_ccb;

    L2CAP_TRACE_API ("L2CA_GetCurrentConfig()  CID: 0x%04x", lcid);

    p_ccb = l2cu_find_ccb_by_cid(NULL, lcid);

    if (p_ccb) {
        *pp_our_cfg  = &(p_ccb->our_cfg);

        /* convert valid config items into bitmap */
        *p_our_cfg_bits = 0;
        if (p_ccb->our_cfg.mtu_present) {
            *p_our_cfg_bits |= L2CAP_CH_CFG_MASK_MTU;
        }
        if (p_ccb->our_cfg.qos_present) {
            *p_our_cfg_bits |= L2CAP_CH_CFG_MASK_QOS;
        }
        if (p_ccb->our_cfg.flush_to_present) {
            *p_our_cfg_bits |= L2CAP_CH_CFG_MASK_FLUSH_TO;
        }
        if (p_ccb->our_cfg.fcr_present) {
            *p_our_cfg_bits |= L2CAP_CH_CFG_MASK_FCR;
        }
        if (p_ccb->our_cfg.fcs_present) {
            *p_our_cfg_bits |= L2CAP_CH_CFG_MASK_FCS;
        }
        if (p_ccb->our_cfg.ext_flow_spec_present) {
            *p_our_cfg_bits |= L2CAP_CH_CFG_MASK_EXT_FLOW_SPEC;
        }

        *pp_peer_cfg = &(p_ccb->peer_cfg);
        *p_peer_cfg_bits = p_ccb->peer_cfg_bits;

        return TRUE;
    } else {
        L2CAP_TRACE_ERROR ("No CCB for CID:0x%04x", lcid);
        return FALSE;
    }
}

/*******************************************************************************
**
** Function         L2CA_RegForNoCPEvt
**
** Description      Register callback for Number of Completed Packets event.
**
** Input Param      p_cb - callback for Number of completed packets event
**                  p_bda - BT address of remote device
**
** Returns          TRUE if registered OK, else FALSE
**
*******************************************************************************/
BOOLEAN L2CA_RegForNoCPEvt(tL2CA_NOCP_CB *p_cb, BD_ADDR p_bda)
{
    tL2C_LCB        *p_lcb;

    /* Find the link that is associated with this remote bdaddr */
    p_lcb = l2cu_find_lcb_by_bd_addr (p_bda, BT_TRANSPORT_BR_EDR);

    /* If no link for this handle, nothing to do. */
    if (!p_lcb) {
        return FALSE;
    }

    p_lcb->p_nocp_cb = p_cb;

    return TRUE;
}
#endif  ///CLASSIC_BT_INCLUDED == TRUE

/*******************************************************************************
**
** Function         L2CA_DataWrite
**
** Description      Higher layers call this function to write data.
**
** Returns          L2CAP_DW_SUCCESS, if data accepted, else FALSE
**                  L2CAP_DW_CONGESTED, if data accepted and the channel is congested
**                  L2CAP_DW_FAILED, if error
**
*******************************************************************************/
#if (CLASSIC_BT_INCLUDED == TRUE)
UINT8 L2CA_DataWrite (UINT16 cid, BT_HDR *p_data)
{
    L2CAP_TRACE_API ("L2CA_DataWrite()  CID: 0x%04x  Len: %d", cid, p_data->len);
    return l2c_data_write (cid, p_data, L2CAP_FLUSHABLE_CH_BASED);
}
#endif  ///CLASSIC_BT_INCLUDED == TRUE

/*******************************************************************************
**
** Function         L2CA_SetChnlFlushability
**
** Description      Higher layers call this function to set a channels
**                  flushability flags
**
** Returns          TRUE if CID found, else FALSE
**
*******************************************************************************/
BOOLEAN L2CA_SetChnlFlushability (UINT16 cid, BOOLEAN is_flushable)
{
#if (L2CAP_NON_FLUSHABLE_PB_INCLUDED == TRUE)

    tL2C_CCB        *p_ccb;

    /* Find the channel control block. We don't know the link it is on. */
    if ((p_ccb = l2cu_find_ccb_by_cid (NULL, cid)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no CCB for L2CA_SetChnlFlushability, CID: %d", cid);
        return (FALSE);
    }

    p_ccb->is_flushable = is_flushable;

    L2CAP_TRACE_API ("L2CA_SetChnlFlushability()  CID: 0x%04x  is_flushable: %d", cid, is_flushable);

#endif

    return (TRUE);
}

/*******************************************************************************
**
** Function         L2CA_DataWriteEx
**
** Description      Higher layers call this function to write data with extended
**                  flags.
**                  flags : L2CAP_FLUSHABLE_CH_BASED
**                          L2CAP_FLUSHABLE_PKT
**                          L2CAP_NON_FLUSHABLE_PKT
**
** Returns          L2CAP_DW_SUCCESS, if data accepted, else FALSE
**                  L2CAP_DW_CONGESTED, if data accepted and the channel is congested
**                  L2CAP_DW_FAILED, if error
**
*******************************************************************************/
#if (CLASSIC_BT_INCLUDED == TRUE)
UINT8 L2CA_DataWriteEx (UINT16 cid, BT_HDR *p_data, UINT16 flags)
{
    L2CAP_TRACE_API ("L2CA_DataWriteEx()  CID: 0x%04x  Len: %d Flags:0x%04X",
                     cid, p_data->len, flags);
    return l2c_data_write (cid, p_data, flags);
}
#endif  ///CLASSIC_BT_INCLUDED == TRUE

/*******************************************************************************
**
** Function     L2CA_FlushChannel
**
** Description  This function flushes none, some or all buffers queued up
**              for xmission for a particular CID. If called with
**              L2CAP_FLUSH_CHANS_GET (0), it simply returns the number
**              of buffers queued for that CID L2CAP_FLUSH_CHANS_ALL (0xffff)
**              flushes all buffers.  All other values specifies the maximum
**              buffers to flush.
**
** Returns      Number of buffers left queued for that CID
**
*******************************************************************************/
UINT16 L2CA_FlushChannel (UINT16 lcid, UINT16 num_to_flush)
{
    tL2C_CCB        *p_ccb;
    tL2C_LCB        *p_lcb;
    UINT16          num_left = 0,
                    num_flushed1 = 0,
                    num_flushed2 = 0;

    p_ccb = l2cu_find_ccb_by_cid(NULL, lcid);

    if ( !p_ccb || ((p_lcb = p_ccb->p_lcb) == NULL) ) {
        L2CAP_TRACE_WARNING ("L2CA_FlushChannel()  abnormally returning 0  CID: 0x%04x", lcid);
        return (0);
    }

    if (num_to_flush != L2CAP_FLUSH_CHANS_GET) {
        L2CAP_TRACE_API ("L2CA_FlushChannel (FLUSH)  CID: 0x%04x  NumToFlush: %d  QC: %u  pFirst: %p",
                         lcid, num_to_flush,
                         fixed_queue_length(p_ccb->xmit_hold_q),
                         fixed_queue_try_peek_first(p_ccb->xmit_hold_q));
    } else {
        L2CAP_TRACE_API ("L2CA_FlushChannel (QUERY)  CID: 0x%04x", lcid);
    }

    /* Cannot flush eRTM buffers once they have a sequence number */
    if (p_ccb->peer_cfg.fcr.mode != L2CAP_FCR_ERTM_MODE) {
#if L2CAP_NON_FLUSHABLE_PB_INCLUDED == TRUE
        if (num_to_flush != L2CAP_FLUSH_CHANS_GET) {
            /* If the controller supports enhanced flush, flush the data queued at the controller */
            if ( (HCI_NON_FLUSHABLE_PB_SUPPORTED(BTM_ReadLocalFeatures ()))
                    && (BTM_GetNumScoLinks() == 0) ) {
                if ( l2cb.is_flush_active == FALSE ) {
                    l2cb.is_flush_active = TRUE;

                    /* The only packet type defined - 0 - Automatically-Flushable Only */
                    btsnd_hcic_enhanced_flush (p_lcb->handle, 0);
                }
            }
        }
#endif

        // Iterate though list and flush the amount requested from
        // the transmit data queue that satisfy the layer and event conditions.
        for (const list_node_t *node = list_begin(p_lcb->link_xmit_data_q);
                (num_to_flush > 0) && node != list_end(p_lcb->link_xmit_data_q);) {
            BT_HDR *p_buf = (BT_HDR *)list_node(node);
            node = list_next(node);
            if ((p_buf->layer_specific == 0) && (p_buf->event == lcid)) {
                num_to_flush--;
                num_flushed1++;

                list_remove(p_lcb->link_xmit_data_q, p_buf);
                osi_free(p_buf);
            }
        }
    }

    /* If needed, flush buffers in the CCB xmit hold queue */
    while ( (num_to_flush != 0) && (!fixed_queue_is_empty(p_ccb->xmit_hold_q))) {
        BT_HDR *p_buf = (BT_HDR *)fixed_queue_dequeue(p_ccb->xmit_hold_q, 0);
        if (p_buf) {
            osi_free (p_buf);
        }
        num_to_flush--;
        num_flushed2++;
    }

    /* If app needs to track all packets, call him */
    if ( (p_ccb->p_rcb) && (p_ccb->p_rcb->api.pL2CA_TxComplete_Cb) && (num_flushed2) ) {
        (*p_ccb->p_rcb->api.pL2CA_TxComplete_Cb)(p_ccb->local_cid, num_flushed2);
    }

    /* Now count how many are left */
    for (const list_node_t *node = list_begin(p_lcb->link_xmit_data_q);
            node != list_end(p_lcb->link_xmit_data_q);
            node = list_next(node)) {

        BT_HDR *p_buf = (BT_HDR *)list_node(node);
        if (p_buf->event == lcid) {
            num_left++;
        }
    }

    /* Add in the number in the CCB xmit queue */
    num_left += fixed_queue_length(p_ccb->xmit_hold_q);

    /* Return the local number of buffers left for the CID */
    L2CAP_TRACE_DEBUG ("L2CA_FlushChannel()  flushed: %u + %u,  num_left: %u", num_flushed1, num_flushed2, num_left);

    /* If we were congested, and now we are not, tell the app */
    l2cu_check_channel_congestion (p_ccb);

    return (num_left);
}

/******************************************************************************
**
** Function         update_acl_pkt_num
**
** Description      Update the number of att acl packets to be sent in xmit_hold_q.
**
** Returns          None
**
*******************************************************************************/
#if BLE_INCLUDED == TRUE
void l2ble_update_att_acl_pkt_num(UINT8 type, tl2c_buff_param_t *param)
{
    static SemaphoreHandle_t buff_semaphore = NULL ;
    static INT16 btc_buf;
    static INT16 btu_buf;

    if(buff_semaphore == NULL && type != L2CA_BUFF_INI){
        L2CAP_TRACE_ERROR("%s buff_semaphore not init", __func__);
        return;
    }
    switch (type)
    {
    case L2CA_ADD_BTC_NUM:{
        xSemaphoreTake(buff_semaphore, portMAX_DELAY);
        btc_buf ++;
        xSemaphoreGive(buff_semaphore);
        break;
    }
    case L2CA_DECREASE_BTC_NUM:{
        xSemaphoreTake(buff_semaphore, portMAX_DELAY);
        btc_buf --;
        xSemaphoreGive(buff_semaphore);
        break;
    }
    case L2CA_ADD_BTU_NUM:{
        xSemaphoreTake(buff_semaphore, portMAX_DELAY);
        btu_buf ++;
        xSemaphoreGive(buff_semaphore);
        break;
    }
    case L2CA_DECREASE_BTU_NUM:{
        xSemaphoreTake(buff_semaphore, portMAX_DELAY);
        btu_buf --;
        xSemaphoreGive(buff_semaphore);
        break;
    }
    case L2CA_GET_ATT_NUM:{
        xSemaphoreTake(buff_semaphore, portMAX_DELAY);
        INT16 att_acl_pkt_num = 0;
        INT16 att_max_num = 0;
        *(param->get_num) = 0;
        UINT8 tcb_idx = param->conn_id;
        tGATT_TCB * p_tcb = gatt_get_tcb_by_idx(tcb_idx);
        if (p_tcb == NULL){
            L2CAP_TRACE_ERROR("%s not found p_tcb", __func__);
            xSemaphoreGive(buff_semaphore);
            break;
        }

        if (!gatt_check_connection_state_by_tcb(p_tcb)) {
            L2CAP_TRACE_ERROR("connection not established\n");
            xSemaphoreGive(buff_semaphore);
            break;
        }

        tL2C_LCB * p_lcb = l2cu_find_lcb_by_bd_addr (p_tcb->peer_bda, BT_TRANSPORT_LE);
        if (p_lcb == NULL){
            L2CAP_TRACE_ERROR("%s not found p_lcb", __func__);
            xSemaphoreGive(buff_semaphore);
            break;
        }

        tL2C_CCB *p_ccb = p_lcb->p_fixed_ccbs[L2CAP_ATT_CID - L2CAP_FIRST_FIXED_CHNL];
        if(p_ccb == NULL) {
            L2CAP_TRACE_ERROR("%s not found p_ccb", __func__);
            xSemaphoreGive(buff_semaphore);
            break;
        }

        fixed_queue_t * queue = p_ccb->xmit_hold_q;
        att_max_num = MIN(p_lcb->link_xmit_quota, L2CAP_CACHE_ATT_ACL_NUM);
        if (queue == NULL){
            L2CAP_TRACE_ERROR("%s not found queue", __func__);
            xSemaphoreGive(buff_semaphore);
            break;
        }
        att_acl_pkt_num = fixed_queue_length(queue);
        if(att_acl_pkt_num < att_max_num){
            if(btc_buf + btu_buf < att_max_num - att_acl_pkt_num){
                *(param->get_num) = att_max_num - att_acl_pkt_num - (btc_buf + btu_buf);
            }
        }
        xSemaphoreGive(buff_semaphore);
        break;
    }
    case L2CA_BUFF_INI:{
        btc_buf = 0;
        btu_buf = 0;
        buff_semaphore = xSemaphoreCreateBinary();
        if (buff_semaphore == NULL) {
            L2CAP_TRACE_ERROR("%s NO MEMORY", __func__);
            break;
        }
        xSemaphoreGive(buff_semaphore);
        break;
    }
    case L2CA_BUFF_DEINIT:{
        xSemaphoreTake(buff_semaphore, portMAX_DELAY);
        btc_buf = 0;
        btu_buf = 0;
        xSemaphoreGive(buff_semaphore);
        vSemaphoreDelete(buff_semaphore);
        buff_semaphore = NULL;
        break;
    }
    default:
        break;
    }
}
#endif
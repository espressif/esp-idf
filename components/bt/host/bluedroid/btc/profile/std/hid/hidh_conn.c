/******************************************************************************
 *
 *  Copyright (C) 2002-2012 Broadcom Corporation
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
 *  This file contains HID HOST internal definitions
 *
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#include "common/bt_target.h"
#include "osi/allocator.h"
#include "stack/bt_types.h"

#include "stack/l2cdefs.h"
#include "stack/l2c_api.h"

#include "stack/btu.h"
#include "stack/btm_api.h"
#include "btm_int.h"

#include "stack/hiddefs.h"

#include "stack/hidh_api.h"
#include "hidh_int.h"
#include "osi/osi.h"

#if (HID_HOST_INCLUDED == TRUE)

static UINT8 find_conn_by_cid (UINT16 cid);
static void hidh_conn_retry (UINT8 dhandle);

/********************************************************************************/
/*              L O C A L    F U N C T I O N     P R O T O T Y P E S            */
/********************************************************************************/
static void hidh_l2cif_connect_ind (BD_ADDR  bd_addr, UINT16 l2cap_cid,
                                    UINT16 psm, UINT8 l2cap_id);
static void hidh_l2cif_connect_cfm (UINT16 l2cap_cid, UINT16 result);
static void hidh_l2cif_config_ind (UINT16 l2cap_cid, tL2CAP_CFG_INFO *p_cfg);
static void hidh_l2cif_config_cfm (UINT16 l2cap_cid, tL2CAP_CFG_INFO *p_cfg);
static void hidh_l2cif_disconnect_ind (UINT16 l2cap_cid, BOOLEAN ack_needed);
static void hidh_l2cif_data_ind (UINT16 l2cap_cid, BT_HDR *p_msg);
static void hidh_l2cif_disconnect_cfm (UINT16 l2cap_cid, UINT16 result);
static void hidh_l2cif_cong_ind (UINT16 l2cap_cid, BOOLEAN congested);

static const tL2CAP_APPL_INFO hst_reg_info = {
    hidh_l2cif_connect_ind,
    hidh_l2cif_connect_cfm,
    NULL,
    hidh_l2cif_config_ind,
    hidh_l2cif_config_cfm,
    hidh_l2cif_disconnect_ind,
    hidh_l2cif_disconnect_cfm,
    NULL,
    hidh_l2cif_data_ind,
    hidh_l2cif_cong_ind,
    NULL                        /* tL2CA_TX_COMPLETE_CB */
};

/*******************************************************************************
**
** Function         hidh_l2cif_reg
**
** Description      This function initializes the SDP unit.
**
** Returns          void
**
*******************************************************************************/
tHID_STATUS hidh_conn_reg (void)
{
    int xx;

    /* Initialize the L2CAP configuration. We only care about MTU and flush */
    memset(&hh_cb.l2cap_cfg, 0, sizeof(tL2CAP_CFG_INFO));

    hh_cb.l2cap_cfg.mtu_present          = TRUE;
    hh_cb.l2cap_cfg.mtu                  = HID_HOST_MTU;
    hh_cb.l2cap_cfg.flush_to_present     = TRUE;
    hh_cb.l2cap_cfg.flush_to             = HID_HOST_FLUSH_TO;

    /* Now, register with L2CAP */
    if (!L2CA_Register (HID_PSM_CONTROL, (tL2CAP_APPL_INFO *) &hst_reg_info)) {
        HIDH_TRACE_ERROR ("HID-Host Control Registration failed");
        return (HID_ERR_L2CAP_FAILED) ;
    }
    if (!L2CA_Register (HID_PSM_INTERRUPT, (tL2CAP_APPL_INFO *) &hst_reg_info)) {
        L2CA_Deregister( HID_PSM_CONTROL ) ;
        HIDH_TRACE_ERROR ("HID-Host Interrupt Registration failed");
        return (HID_ERR_L2CAP_FAILED) ;
    }

    for (xx = 0; xx < HID_HOST_MAX_DEVICES; xx++) {
        hh_cb.devices[xx].in_use = FALSE ;
        hh_cb.devices[xx].conn.conn_state = HID_CONN_STATE_UNUSED;
    }

    return (HID_SUCCESS);
}

/*******************************************************************************
**
** Function         hidh_conn_disconnect
**
** Description      This function disconnects a connection.
**
** Returns          TRUE if disconnect started, FALSE if already disconnected
**
*******************************************************************************/
tHID_STATUS hidh_conn_disconnect (UINT8 dhandle)
{
    tHID_CONN *p_hcon = &hh_cb.devices[dhandle].conn;

    HIDH_TRACE_EVENT ("HID-Host disconnect");

    if ((p_hcon->ctrl_cid != 0) || (p_hcon->intr_cid != 0)) {
        p_hcon->conn_state = HID_CONN_STATE_DISCONNECTING;

        /* Set l2cap idle timeout to 0 (so ACL link is disconnected
         * immediately after last channel is closed) */
        L2CA_SetIdleTimeoutByBdAddr(hh_cb.devices[dhandle].addr, 0, BT_TRANSPORT_BR_EDR);
        /* Disconnect both interrupt and control channels */
        if (p_hcon->intr_cid) {
            L2CA_DisconnectReq (p_hcon->intr_cid);
        } else if (p_hcon->ctrl_cid) {
            L2CA_DisconnectReq (p_hcon->ctrl_cid);
        }
    } else {
        p_hcon->conn_state = HID_CONN_STATE_UNUSED;
    }

    return (HID_SUCCESS);
}

/*******************************************************************************
**
** Function         hidh_sec_check_complete_term
**
** Description      HID security check complete callback function.
**
** Returns          Send L2CA_ConnectRsp OK if secutiry check succeed; otherwise
**                  send security block L2C connection response.
**
*******************************************************************************/
void hidh_sec_check_complete_term (BD_ADDR bd_addr, tBT_TRANSPORT transport, void *p_ref_data, UINT8 res)
{
    tHID_HOST_DEV_CTB *p_dev = (tHID_HOST_DEV_CTB *) p_ref_data;
    UNUSED(bd_addr);
    UNUSED (transport);

    if ( res == BTM_SUCCESS && p_dev->conn.conn_state == HID_CONN_STATE_SECURITY ) {
        p_dev->conn.disc_reason = HID_SUCCESS;  /* Authentication passed. Reset disc_reason (from HID_ERR_AUTH_FAILED) */

        p_dev->conn.conn_state = HID_CONN_STATE_CONNECTING_INTR;

        /* Send response to the L2CAP layer. */
        L2CA_ConnectRsp (p_dev->addr, p_dev->conn.ctrl_id, p_dev->conn.ctrl_cid, L2CAP_CONN_OK, L2CAP_CONN_OK);

        /* Send a Configuration Request. */
        L2CA_ConfigReq (p_dev->conn.ctrl_cid, &hh_cb.l2cap_cfg);

    }
    /* security check fail */
    else if (res != BTM_SUCCESS) {
        p_dev->conn.disc_reason = HID_ERR_AUTH_FAILED;      /* Save reason for disconnecting */
        p_dev->conn.conn_state = HID_CONN_STATE_UNUSED;
        L2CA_ConnectRsp (p_dev->addr, p_dev->conn.ctrl_id, p_dev->conn.ctrl_cid, L2CAP_CONN_SECURITY_BLOCK, L2CAP_CONN_OK);
    }
}

/*******************************************************************************
**
** Function         hidh_l2cif_connect_ind
**
** Description      This function handles an inbound connection indication
**                  from L2CAP. This is the case where we are acting as a
**                  server.
**
** Returns          void
**
*******************************************************************************/
static void hidh_l2cif_connect_ind (BD_ADDR  bd_addr, UINT16 l2cap_cid, UINT16 psm, UINT8 l2cap_id)
{
    tHID_CONN    *p_hcon;
    BOOLEAN      bAccept = TRUE;
    UINT8        i = HID_HOST_MAX_DEVICES;
    tHID_HOST_DEV_CTB *p_dev;

    HIDH_TRACE_EVENT ("HID-Host Rcvd L2CAP conn ind, PSM: 0x%04x  CID 0x%x", psm, l2cap_cid);

    /* always add incoming connection device into HID database by default */
    if (HID_HostAddDev(bd_addr, HID_SEC_REQUIRED, &i) != HID_SUCCESS) {
        L2CA_ConnectRsp (bd_addr, l2cap_id, l2cap_cid, L2CAP_CONN_SECURITY_BLOCK, 0);
        return;
    }

    p_hcon = &hh_cb.devices[i].conn;
    p_dev  = &hh_cb.devices[i];

    /* Check we are in the correct state for this */
    if (psm == HID_PSM_INTERRUPT) {
        if (p_hcon->ctrl_cid == 0) {
            HIDH_TRACE_WARNING ("HID-Host Rcvd INTR L2CAP conn ind, but no CTL channel");
            bAccept = FALSE;
        }
        if (p_hcon->conn_state != HID_CONN_STATE_CONNECTING_INTR) {
            HIDH_TRACE_WARNING ("HID-Host Rcvd INTR L2CAP conn ind, wrong state: %d",
                                p_hcon->conn_state);
            bAccept = FALSE;
        }
    } else { /* CTRL channel */
#if defined(HID_HOST_ACPT_NEW_CONN) && (HID_HOST_ACPT_NEW_CONN == TRUE)
        p_hcon->ctrl_cid = p_hcon->intr_cid = 0;
        p_hcon->conn_state = HID_CONN_STATE_UNUSED;
#else
        if (p_hcon->conn_state != HID_CONN_STATE_UNUSED) {
            HIDH_TRACE_WARNING ("HID-Host - Rcvd CTL L2CAP conn ind, wrong state: %d",
                                p_hcon->conn_state);
            bAccept = FALSE;
        }
#endif
    }

    if (!bAccept) {
        L2CA_ConnectRsp (bd_addr, l2cap_id, l2cap_cid, L2CAP_CONN_NO_RESOURCES, 0);
        return;
    }

    if (psm == HID_PSM_CONTROL) {
        p_hcon->conn_flags = 0;
        p_hcon->ctrl_cid   = l2cap_cid;
        p_hcon->ctrl_id    = l2cap_id;
        p_hcon->disc_reason = HID_L2CAP_CONN_FAIL;  /* In case disconnection occurs before security is completed, then set CLOSE_EVT reason code to 'connection failure' */

        p_hcon->conn_state = HID_CONN_STATE_SECURITY;
        if (btm_sec_mx_access_request (p_dev->addr, HID_PSM_CONTROL,
                                       FALSE, BTM_SEC_PROTO_HID,
                                       (p_dev->attr_mask & HID_SEC_REQUIRED) ? HID_SEC_CHN : HID_NOSEC_CHN,
                                       &hidh_sec_check_complete_term, p_dev) == BTM_CMD_STARTED) {
            L2CA_ConnectRsp (bd_addr, l2cap_id, l2cap_cid, L2CAP_CONN_PENDING, L2CAP_CONN_OK);
        }

        return;
    }

    /* Transition to the next appropriate state, configuration */
    p_hcon->conn_state = HID_CONN_STATE_CONFIG;
    p_hcon->intr_cid   = l2cap_cid;

    /* Send response to the L2CAP layer. */
    L2CA_ConnectRsp (bd_addr, l2cap_id, l2cap_cid, L2CAP_CONN_OK, L2CAP_CONN_OK);

    /* Send a Configuration Request. */
    L2CA_ConfigReq (l2cap_cid, &hh_cb.l2cap_cfg);

    HIDH_TRACE_EVENT ("HID-Host Rcvd L2CAP conn ind, sent config req, PSM: 0x%04x  CID 0x%x",
                      psm, l2cap_cid);
}

/*******************************************************************************
**
** Function         hidh_proc_repage_timeout
**
** Description      This function handles timeout (to page device).
**
** Returns          void
**
*******************************************************************************/
void hidh_proc_repage_timeout (TIMER_LIST_ENT *p_tle)
{
    hidh_conn_initiate( (UINT8) p_tle->param ) ;
    hh_cb.devices[p_tle->param].conn_tries++;
    hh_cb.callback( (UINT8) p_tle->param, hh_cb.devices[p_tle->param].addr,
                    HID_HDEV_EVT_RETRYING, hh_cb.devices[p_tle->param].conn_tries, NULL ) ;
}

/*******************************************************************************
**
** Function         hidh_sec_check_complete_orig
**
** Description      This function checks to see if security procedures are being
**                  carried out or not..
**
** Returns          void
**
*******************************************************************************/
void hidh_sec_check_complete_orig (BD_ADDR bd_addr, tBT_TRANSPORT transport, void *p_ref_data, UINT8 res)
{
    tHID_HOST_DEV_CTB *p_dev = (tHID_HOST_DEV_CTB *) p_ref_data;
    UINT8 dhandle;
    UNUSED(bd_addr);
    UNUSED (transport);

    dhandle = ((UINT32)p_dev - (UINT32) & (hh_cb.devices[0])) / sizeof(tHID_HOST_DEV_CTB);
    if ( res == BTM_SUCCESS && p_dev->conn.conn_state == HID_CONN_STATE_SECURITY ) {
        HIDH_TRACE_EVENT ("HID-Host Originator security pass.");
        p_dev->conn.disc_reason = HID_SUCCESS;  /* Authentication passed. Reset disc_reason (from HID_ERR_AUTH_FAILED) */

        /* Transition to the next appropriate state, configuration */
        p_dev->conn.conn_state = HID_CONN_STATE_CONFIG;
        L2CA_ConfigReq (p_dev->conn.ctrl_cid, &hh_cb.l2cap_cfg);
        HIDH_TRACE_EVENT ("HID-Host Got Control conn cnf, sent cfg req, CID: 0x%x", p_dev->conn.ctrl_cid);

    }

    if ( res != BTM_SUCCESS && p_dev->conn.conn_state == HID_CONN_STATE_SECURITY ) {
#if (HID_HOST_MAX_CONN_RETRY > 0)
        if ( res == BTM_DEVICE_TIMEOUT ) {
            if ( p_dev->conn_tries <= HID_HOST_MAX_CONN_RETRY ) {
                hidh_conn_retry (dhandle);
                return;
            }
        }
#endif
        p_dev->conn.disc_reason = HID_ERR_AUTH_FAILED;      /* Save reason for disconnecting */
        hidh_conn_disconnect(dhandle);
    }

}

/*******************************************************************************
**
** Function         hidh_l2cif_connect_cfm
**
** Description      This function handles the connect confirm events
**                  from L2CAP. This is the case when we are acting as a
**                  client and have sent a connect request.
**
** Returns          void
**
*******************************************************************************/
static void hidh_l2cif_connect_cfm (UINT16 l2cap_cid, UINT16 result)
{
    UINT8 dhandle;
    tHID_CONN    *p_hcon = NULL;
    UINT32  reason;
    tHID_HOST_DEV_CTB *p_dev = NULL;

    /* Find CCB based on CID, and verify we are in a state to accept this message */
    if ( (dhandle = find_conn_by_cid(l2cap_cid)) < HID_HOST_MAX_DEVICES ) {
        p_dev = &hh_cb.devices[dhandle];
        p_hcon = &hh_cb.devices[dhandle].conn;
    }

    if ((p_hcon == NULL)
            || (!(p_hcon->conn_flags & HID_CONN_FLAGS_IS_ORIG))
            || ((l2cap_cid == p_hcon->ctrl_cid) && (p_hcon->conn_state != HID_CONN_STATE_CONNECTING_CTRL))
            || ((l2cap_cid == p_hcon->intr_cid) && (p_hcon->conn_state != HID_CONN_STATE_CONNECTING_INTR)
                && (p_hcon->conn_state != HID_CONN_STATE_DISCONNECTING))) {
        HIDH_TRACE_WARNING ("HID-Host Rcvd unexpected conn cnf, CID 0x%x ", l2cap_cid);
        return;
    }

    if (result != L2CAP_CONN_OK) {
        if (l2cap_cid == p_hcon->ctrl_cid) {
            p_hcon->ctrl_cid = 0;
        } else {
            p_hcon->intr_cid = 0;
        }

        hidh_conn_disconnect(dhandle);

#if (HID_HOST_MAX_CONN_RETRY > 0)
        if ( (hh_cb.devices[dhandle].conn_tries <= HID_HOST_MAX_CONN_RETRY) &&
                (result == HCI_ERR_CONNECTION_TOUT || result == HCI_ERR_UNSPECIFIED ||
                 result == HCI_ERR_PAGE_TIMEOUT) ) {
            hidh_conn_retry(dhandle);
        } else
#endif
        {
            reason = HID_L2CAP_CONN_FAIL | (UINT32) result ;
            hh_cb.callback( dhandle, hh_cb.devices[dhandle].addr, HID_HDEV_EVT_CLOSE, reason, NULL ) ;
        }
        return;
    }
    /* receive Control Channel connect confirmation */
    if (l2cap_cid == p_hcon->ctrl_cid) {
        /* check security requirement */
        p_hcon->conn_state = HID_CONN_STATE_SECURITY;
        p_hcon->disc_reason = HID_L2CAP_CONN_FAIL;  /* In case disconnection occurs before security is completed, then set CLOSE_EVT reason code to "connection failure" */

        btm_sec_mx_access_request (p_dev->addr, HID_PSM_CONTROL,
                                   TRUE, BTM_SEC_PROTO_HID,
                                   (p_dev->attr_mask & HID_SEC_REQUIRED) ? HID_SEC_CHN : HID_NOSEC_CHN,
                                   &hidh_sec_check_complete_orig, p_dev);
    } else {
        p_hcon->conn_state = HID_CONN_STATE_CONFIG;
        /* Send a Configuration Request. */
        L2CA_ConfigReq (l2cap_cid, &hh_cb.l2cap_cfg);
        HIDH_TRACE_EVENT ("HID-Host got Interrupt conn cnf, sent cfg req, CID: 0x%x", l2cap_cid);
    }

    return;
}

/*******************************************************************************
**
** Function         hidh_l2cif_config_ind
**
** Description      This function processes the L2CAP configuration indication
**                  event.
**
** Returns          void
**
*******************************************************************************/
static void hidh_l2cif_config_ind (UINT16 l2cap_cid, tL2CAP_CFG_INFO *p_cfg)
{
    UINT8 dhandle;
    tHID_CONN    *p_hcon = NULL;
    UINT32  reason;

    /* Find CCB based on CID */
    if ( (dhandle = find_conn_by_cid(l2cap_cid)) < HID_HOST_MAX_DEVICES ) {
        p_hcon = &hh_cb.devices[dhandle].conn;
    }

    if (p_hcon == NULL) {
        HIDH_TRACE_WARNING ("HID-Host Rcvd L2CAP cfg ind, unknown CID: 0x%x", l2cap_cid);
        return;
    }

    HIDH_TRACE_EVENT ("HID-Host Rcvd cfg ind, sent cfg cfm, CID: 0x%x", l2cap_cid);

    /* Remember the remote MTU size */
    if ((!p_cfg->mtu_present) || (p_cfg->mtu > HID_HOST_MTU)) {
        p_hcon->rem_mtu_size = HID_HOST_MTU;
    } else {
        p_hcon->rem_mtu_size = p_cfg->mtu;
    }

    /* For now, always accept configuration from the other side */
    p_cfg->flush_to_present = FALSE;
    p_cfg->mtu_present      = FALSE;
    p_cfg->result           = L2CAP_CFG_OK;

    L2CA_ConfigRsp (l2cap_cid, p_cfg);

    if (l2cap_cid == p_hcon->ctrl_cid) {
        p_hcon->conn_flags |= HID_CONN_FLAGS_HIS_CTRL_CFG_DONE;
        if ((p_hcon->conn_flags & HID_CONN_FLAGS_IS_ORIG) &&
                (p_hcon->conn_flags & HID_CONN_FLAGS_MY_CTRL_CFG_DONE)) {
            /* Connect interrupt channel */
            p_hcon->disc_reason = HID_L2CAP_CONN_FAIL;  /* Reset initial reason for CLOSE_EVT: Connection Attempt was made but failed */
            if ((p_hcon->intr_cid = L2CA_ConnectReq (HID_PSM_INTERRUPT, hh_cb.devices[dhandle].addr)) == 0) {
                HIDH_TRACE_WARNING ("HID-Host INTR Originate failed");
                reason = HID_L2CAP_REQ_FAIL ;
                p_hcon->conn_state = HID_CONN_STATE_UNUSED;
                hidh_conn_disconnect (dhandle);
                hh_cb.callback( dhandle, hh_cb.devices[dhandle].addr, HID_HDEV_EVT_CLOSE, reason, NULL ) ;
                return;
            } else {
                /* Transition to the next appropriate state, waiting for connection confirm on interrupt channel. */
                p_hcon->conn_state = HID_CONN_STATE_CONNECTING_INTR;
            }
        }
    } else {
        p_hcon->conn_flags |= HID_CONN_FLAGS_HIS_INTR_CFG_DONE;
    }

    /* If all configuration is complete, change state and tell management we are up */
    if (((p_hcon->conn_flags & HID_CONN_FLAGS_ALL_CONFIGURED) == HID_CONN_FLAGS_ALL_CONFIGURED)
            && (p_hcon->conn_state == HID_CONN_STATE_CONFIG)) {
        p_hcon->conn_state = HID_CONN_STATE_CONNECTED;
        /* Reset disconnect reason to success, as connection successful */
        p_hcon->disc_reason = HID_SUCCESS;

        hh_cb.devices[dhandle].state = HID_DEV_CONNECTED;
        hh_cb.callback( dhandle,  hh_cb.devices[dhandle].addr, HID_HDEV_EVT_OPEN, 0, NULL ) ;
    }
}


/*******************************************************************************
**
** Function         hidh_l2cif_config_cfm
**
** Description      This function processes the L2CAP configuration confirmation
**                  event.
**
** Returns          void
**
*******************************************************************************/
static void hidh_l2cif_config_cfm (UINT16 l2cap_cid, tL2CAP_CFG_INFO *p_cfg)
{
    UINT8 dhandle;
    tHID_CONN    *p_hcon = NULL;
    UINT32  reason;

    HIDH_TRACE_EVENT ("HID-Host Rcvd cfg cfm, CID: 0x%x  Result: %d", l2cap_cid, p_cfg->result);

    /* Find CCB based on CID */
    if ( (dhandle = find_conn_by_cid(l2cap_cid)) < HID_HOST_MAX_DEVICES ) {
        p_hcon = &hh_cb.devices[dhandle].conn;
    }

    if (p_hcon == NULL) {
        HIDH_TRACE_WARNING ("HID-Host Rcvd L2CAP cfg ind, unknown CID: 0x%x", l2cap_cid);
        return;
    }

    /* If configuration failed, disconnect the channel(s) */
    if (p_cfg->result != L2CAP_CFG_OK) {
        hidh_conn_disconnect (dhandle);
        reason = HID_L2CAP_CFG_FAIL | (UINT32) p_cfg->result ;
        hh_cb.callback( dhandle, hh_cb.devices[dhandle].addr, HID_HDEV_EVT_CLOSE, reason, NULL ) ;
        return;
    }

    if (l2cap_cid == p_hcon->ctrl_cid) {
        p_hcon->conn_flags |= HID_CONN_FLAGS_MY_CTRL_CFG_DONE;
        if ((p_hcon->conn_flags & HID_CONN_FLAGS_IS_ORIG) &&
                (p_hcon->conn_flags & HID_CONN_FLAGS_HIS_CTRL_CFG_DONE)) {
            /* Connect interrupt channel */
            p_hcon->disc_reason = HID_L2CAP_CONN_FAIL;  /* Reset initial reason for CLOSE_EVT: Connection Attempt was made but failed */
            if ((p_hcon->intr_cid = L2CA_ConnectReq (HID_PSM_INTERRUPT, hh_cb.devices[dhandle].addr)) == 0) {
                HIDH_TRACE_WARNING ("HID-Host INTR Originate failed");
                reason = HID_L2CAP_REQ_FAIL ;
                p_hcon->conn_state = HID_CONN_STATE_UNUSED;
                hidh_conn_disconnect (dhandle);
                hh_cb.callback( dhandle, hh_cb.devices[dhandle].addr, HID_HDEV_EVT_CLOSE, reason, NULL ) ;
                return;
            } else {
                /* Transition to the next appropriate state, waiting for connection confirm on interrupt channel. */
                p_hcon->conn_state = HID_CONN_STATE_CONNECTING_INTR;
            }
        }
    } else {
        p_hcon->conn_flags |= HID_CONN_FLAGS_MY_INTR_CFG_DONE;
    }

    /* If all configuration is complete, change state and tell management we are up */
    if (((p_hcon->conn_flags & HID_CONN_FLAGS_ALL_CONFIGURED) == HID_CONN_FLAGS_ALL_CONFIGURED)
            && (p_hcon->conn_state == HID_CONN_STATE_CONFIG)) {
        p_hcon->conn_state = HID_CONN_STATE_CONNECTED;
        /* Reset disconnect reason to success, as connection successful */
        p_hcon->disc_reason = HID_SUCCESS;

        hh_cb.devices[dhandle].state = HID_DEV_CONNECTED;
        hh_cb.callback( dhandle, hh_cb.devices[dhandle].addr, HID_HDEV_EVT_OPEN, 0, NULL ) ;
    }
}


/*******************************************************************************
**
** Function         hidh_l2cif_disconnect_ind
**
** Description      This function handles a disconnect event from L2CAP. If
**                  requested to, we ack the disconnect before dropping the CCB
**
** Returns          void
**
*******************************************************************************/
static void hidh_l2cif_disconnect_ind (UINT16 l2cap_cid, BOOLEAN ack_needed)
{
    UINT8 dhandle;
    tHID_CONN    *p_hcon = NULL;
    UINT16 disc_res = HCI_SUCCESS;
    UINT16 hid_close_evt_reason;

    /* Find CCB based on CID */
    if ( (dhandle = find_conn_by_cid(l2cap_cid)) < HID_HOST_MAX_DEVICES ) {
        p_hcon = &hh_cb.devices[dhandle].conn;
    }

    if (p_hcon == NULL) {
        HIDH_TRACE_WARNING ("HID-Host Rcvd L2CAP disc, unknown CID: 0x%x", l2cap_cid);
        return;
    }

    if (ack_needed) {
        L2CA_DisconnectRsp (l2cap_cid);
    }

    HIDH_TRACE_EVENT ("HID-Host Rcvd L2CAP disc, CID: 0x%x", l2cap_cid);

    p_hcon->conn_state = HID_CONN_STATE_DISCONNECTING;

    if (l2cap_cid == p_hcon->ctrl_cid) {
        p_hcon->ctrl_cid = 0;
    } else {
        p_hcon->intr_cid = 0;
    }

    if ((p_hcon->ctrl_cid == 0) && (p_hcon->intr_cid == 0)) {
        hh_cb.devices[dhandle].state = HID_DEV_NO_CONN;
        p_hcon->conn_state = HID_CONN_STATE_UNUSED;

        if ( !ack_needed ) {
            disc_res = btm_get_acl_disc_reason_code();
        }

#if (HID_HOST_MAX_CONN_RETRY > 0)
        if ( (disc_res == HCI_ERR_CONNECTION_TOUT || disc_res == HCI_ERR_UNSPECIFIED) &&
                (!(hh_cb.devices[dhandle].attr_mask & HID_RECONN_INIT)) &&
                (hh_cb.devices[dhandle].attr_mask & HID_NORMALLY_CONNECTABLE)) {
            hh_cb.devices[dhandle].conn_tries = 0;
            hh_cb.devices[dhandle].conn.timer_entry.param = (UINT32) dhandle;
            btu_start_timer (&(hh_cb.devices[dhandle].conn.timer_entry), BTU_TTYPE_HID_HOST_REPAGE_TO, HID_HOST_REPAGE_WIN);
            hh_cb.callback( dhandle,  hh_cb.devices[dhandle].addr, HID_HDEV_EVT_CLOSE, disc_res, NULL);
        } else
#endif
        {
            /* Set reason code for HID_HDEV_EVT_CLOSE */
            hid_close_evt_reason = p_hcon->disc_reason;

            /* If we got baseband sent HCI_DISCONNECT_COMPLETE_EVT due to security failure, then set reason to HID_ERR_AUTH_FAILED */
            if ((disc_res == HCI_ERR_AUTH_FAILURE)                        ||
                    (disc_res == HCI_ERR_KEY_MISSING)                         ||
                    (disc_res == HCI_ERR_HOST_REJECT_SECURITY)                ||
                    (disc_res == HCI_ERR_PAIRING_NOT_ALLOWED)                 ||
                    (disc_res == HCI_ERR_UNIT_KEY_USED)                       ||
                    (disc_res == HCI_ERR_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED) ||
                    (disc_res == HCI_ERR_ENCRY_MODE_NOT_ACCEPTABLE)           ||
                    (disc_res == HCI_ERR_REPEATED_ATTEMPTS)) {
                hid_close_evt_reason = HID_ERR_AUTH_FAILED;
            }

            hh_cb.callback( dhandle,  hh_cb.devices[dhandle].addr, HID_HDEV_EVT_CLOSE, hid_close_evt_reason, NULL ) ;
        }
    }
}


/*******************************************************************************
**
** Function         hidh_l2cif_disconnect_cfm
**
** Description      This function handles a disconnect confirm event from L2CAP.
**
** Returns          void
**
*******************************************************************************/
static void hidh_l2cif_disconnect_cfm (UINT16 l2cap_cid, UINT16 result)
{
    UINT8 dhandle;
    tHID_CONN    *p_hcon = NULL;
    UNUSED(result);

    /* Find CCB based on CID */
    if ( (dhandle = find_conn_by_cid(l2cap_cid)) < HID_HOST_MAX_DEVICES ) {
        p_hcon = &hh_cb.devices[dhandle].conn;
    }

    if (p_hcon == NULL) {
        HIDH_TRACE_WARNING ("HID-Host Rcvd L2CAP disc cfm, unknown CID: 0x%x", l2cap_cid);
        return;
    }

    HIDH_TRACE_EVENT ("HID-Host Rcvd L2CAP disc cfm, CID: 0x%x", l2cap_cid);

    if (l2cap_cid == p_hcon->ctrl_cid) {
        p_hcon->ctrl_cid = 0;
    } else {
        p_hcon->intr_cid = 0;
        if (p_hcon->ctrl_cid) {
            HIDH_TRACE_EVENT ("HID-Host Initiating L2CAP Ctrl disconnection");
            L2CA_DisconnectReq (p_hcon->ctrl_cid);
        }
    }

    if ((p_hcon->ctrl_cid == 0) && (p_hcon->intr_cid == 0)) {
        hh_cb.devices[dhandle].state = HID_DEV_NO_CONN;
        p_hcon->conn_state = HID_CONN_STATE_UNUSED;
        hh_cb.callback( dhandle, hh_cb.devices[dhandle].addr, HID_HDEV_EVT_CLOSE, p_hcon->disc_reason, NULL ) ;
    }
}


/*******************************************************************************
**
** Function         hidh_l2cif_cong_ind
**
** Description      This function handles a congestion status event from L2CAP.
**
** Returns          void
**
*******************************************************************************/
static void hidh_l2cif_cong_ind (UINT16 l2cap_cid, BOOLEAN congested)
{
    UINT8 dhandle;
    tHID_CONN    *p_hcon = NULL;

    /* Find CCB based on CID */
    if ( (dhandle = find_conn_by_cid(l2cap_cid)) < HID_HOST_MAX_DEVICES ) {
        p_hcon = &hh_cb.devices[dhandle].conn;
    }

    if (p_hcon == NULL) {
        HIDH_TRACE_WARNING ("HID-Host Rcvd L2CAP congestion status, unknown CID: 0x%x", l2cap_cid);
        return;
    }

    HIDH_TRACE_EVENT ("HID-Host Rcvd L2CAP congestion status, CID: 0x%x  Cong: %d", l2cap_cid, congested);

    if (congested) {
        p_hcon->conn_flags |= HID_CONN_FLAGS_CONGESTED;
    } else {
        p_hcon->conn_flags &= ~HID_CONN_FLAGS_CONGESTED;

    }
}


/*******************************************************************************
**
** Function         hidh_l2cif_data_ind
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
static void hidh_l2cif_data_ind (UINT16 l2cap_cid, BT_HDR *p_msg)
{
    UINT8           *p_data = (UINT8 *)(p_msg + 1) + p_msg->offset;
    UINT8           ttype, param, rep_type, evt;
    UINT8 dhandle;
    tHID_CONN    *p_hcon = NULL;

    HIDH_TRACE_DEBUG ("HID-Host hidh_l2cif_data_ind [l2cap_cid=0x%04x]", l2cap_cid);

    /* Find CCB based on CID */
    if ((dhandle = find_conn_by_cid(l2cap_cid)) < HID_HOST_MAX_DEVICES) {
        p_hcon = &hh_cb.devices[dhandle].conn;
    }

    if (p_hcon == NULL) {
        HIDH_TRACE_WARNING ("HID-Host Rcvd L2CAP data, unknown CID: 0x%x", l2cap_cid);
        osi_free (p_msg);
        return;
    }


    ttype    = HID_GET_TRANS_FROM_HDR(*p_data);
    param    = HID_GET_PARAM_FROM_HDR(*p_data);
    rep_type = param & HID_PAR_REP_TYPE_MASK;
    p_data++;

    /* Get rid of the data type */
    p_msg->len--;
    p_msg->offset++;

    switch (ttype) {
    case HID_TRANS_HANDSHAKE:
        hh_cb.callback(dhandle,  hh_cb.devices[dhandle].addr, HID_HDEV_EVT_HANDSHAKE, param, NULL);
        osi_free (p_msg);
        break;

    case HID_TRANS_CONTROL:
        switch (param) {
        case HID_PAR_CONTROL_VIRTUAL_CABLE_UNPLUG:
            hidh_conn_disconnect( dhandle ) ;
            /* Device is unplugging from us. Tell USB */
            hh_cb.callback(dhandle,  hh_cb.devices[dhandle].addr, HID_HDEV_EVT_VC_UNPLUG, 0, NULL);
            break;

        default:
            break;
        }
        osi_free (p_msg);
        break;


    case HID_TRANS_DATA:
        evt = (hh_cb.devices[dhandle].conn.intr_cid == l2cap_cid) ?
              HID_HDEV_EVT_INTR_DATA : HID_HDEV_EVT_CTRL_DATA;
        hh_cb.callback(dhandle, hh_cb.devices[dhandle].addr, evt, rep_type, p_msg);
        break;

    case HID_TRANS_DATAC:
        evt = (hh_cb.devices[dhandle].conn.intr_cid == l2cap_cid) ?
              HID_HDEV_EVT_INTR_DATC : HID_HDEV_EVT_CTRL_DATC;
        hh_cb.callback(dhandle,  hh_cb.devices[dhandle].addr, evt, rep_type, p_msg);
        break;

    default:
        osi_free (p_msg);
        break;
    }

}

/*******************************************************************************
**
** Function         hidh_conn_snd_data
**
** Description      This function is sends out data.
**
** Returns          tHID_STATUS
**
*******************************************************************************/
tHID_STATUS hidh_conn_snd_data (UINT8 dhandle, UINT8 trans_type, UINT8 param,
                                UINT16 data, UINT8 report_id, BT_HDR *buf)
{
    tHID_CONN   *p_hcon = &hh_cb.devices[dhandle].conn;
    BT_HDR      *p_buf;
    UINT8       *p_out;
    UINT16      bytes_copied;
    BOOLEAN     seg_req = FALSE;
    UINT16      data_size;
    UINT16      cid;
    UINT16      buf_size;
    UINT8       use_data = 0 ;
    BOOLEAN     blank_datc = FALSE;

    if (!BTM_IsAclConnectionUp(hh_cb.devices[dhandle].addr, BT_TRANSPORT_BR_EDR)) {
        if (buf) {
            osi_free ((void *)buf);
        }
        return ( HID_ERR_NO_CONNECTION );
    }

    if (p_hcon->conn_flags & HID_CONN_FLAGS_CONGESTED) {
        if (buf) {
            osi_free ((void *)buf);
        }
        return ( HID_ERR_CONGESTED );
    }

    switch ( trans_type ) {
    case HID_TRANS_CONTROL:
    case HID_TRANS_GET_REPORT:
    case HID_TRANS_SET_REPORT:
    case HID_TRANS_GET_PROTOCOL:
    case HID_TRANS_SET_PROTOCOL:
    case HID_TRANS_GET_IDLE:
    case HID_TRANS_SET_IDLE:
        cid = p_hcon->ctrl_cid;
        buf_size = HID_CONTROL_BUF_SIZE;
        break;
    case HID_TRANS_DATA:
        cid = p_hcon->intr_cid;
        buf_size = HID_INTERRUPT_BUF_SIZE;
        break;
    default:
        return (HID_ERR_INVALID_PARAM) ;
    }

    if ( trans_type == HID_TRANS_SET_IDLE ) {
        use_data = 1;
    } else if ( (trans_type == HID_TRANS_GET_REPORT) && (param & 0x08) ) {
        use_data = 2;
    }

    do {
        if ( buf == NULL || blank_datc ) {
            if ((p_buf = (BT_HDR *)osi_malloc(buf_size)) == NULL) {
                return (HID_ERR_NO_RESOURCES);
            }

            p_buf->offset = L2CAP_MIN_OFFSET;
            seg_req = FALSE;
            data_size = 0;
            bytes_copied = 0;
            blank_datc = FALSE;
        } else if ( (buf->len > (p_hcon->rem_mtu_size - 1))) {
            if ((p_buf = (BT_HDR *)osi_malloc(buf_size)) == NULL) {
                return (HID_ERR_NO_RESOURCES);
            }

            p_buf->offset = L2CAP_MIN_OFFSET;
            seg_req = TRUE;
            data_size = buf->len;
            bytes_copied = p_hcon->rem_mtu_size - 1;
        } else {
            p_buf = buf ;
            p_buf->offset -= 1;
            seg_req = FALSE;
            data_size = buf->len;
            bytes_copied = buf->len;
        }

        p_out         = (UINT8 *)(p_buf + 1) + p_buf->offset;
        *p_out++      = HID_BUILD_HDR(trans_type, param);

        /* If report ID required for this device */
        if ( (trans_type == HID_TRANS_GET_REPORT) && (report_id != 0) ) {
            *p_out = report_id;
            data_size = bytes_copied = 1;
        }


        if (seg_req) {
            memcpy (p_out, (((UINT8 *)(buf + 1)) + buf->offset), bytes_copied);
            buf->offset += bytes_copied;
            buf->len -= bytes_copied;
        } else if ( use_data == 1) {
            *(p_out + bytes_copied) = data & 0xff;
        } else if ( use_data == 2 ) {
            *(p_out + bytes_copied) = data & 0xff;
            *(p_out + bytes_copied + 1) = (data >> 8) & 0xff ;
        }

        p_buf->len   = bytes_copied + 1 + use_data;
        data_size    -= bytes_copied;

        /* Send the buffer through L2CAP */
        if ((p_hcon->conn_flags & HID_CONN_FLAGS_CONGESTED) || (!L2CA_DataWrite (cid, p_buf))) {
            return (HID_ERR_CONGESTED);
        }

        if (data_size) {
            trans_type = HID_TRANS_DATAC;
        } else if ( bytes_copied == (p_hcon->rem_mtu_size - 1) ) {
            trans_type = HID_TRANS_DATAC;
            blank_datc = TRUE;
        }

    } while ((data_size != 0) || blank_datc ) ;

    return (HID_SUCCESS);
}
/*******************************************************************************
**
** Function         hidh_conn_initiate
**
** Description      This function is called by the management to create a connection.
**
** Returns          void
**
*******************************************************************************/
tHID_STATUS hidh_conn_initiate (UINT8 dhandle)
{
    UINT8   service_id = BTM_SEC_SERVICE_HIDH_NOSEC_CTRL;
    UINT32  mx_chan_id = HID_NOSEC_CHN;

    tHID_HOST_DEV_CTB *p_dev = &hh_cb.devices[dhandle];

    if ( p_dev->conn.conn_state != HID_CONN_STATE_UNUSED ) {
        return ( HID_ERR_CONN_IN_PROCESS );
    }

    p_dev->conn.ctrl_cid = 0;
    p_dev->conn.intr_cid = 0;
    p_dev->conn.disc_reason = HID_L2CAP_CONN_FAIL;  /* Reset initial reason for CLOSE_EVT: Connection Attempt was made but failed */

    /* We are the originator of this connection */
    p_dev->conn.conn_flags = HID_CONN_FLAGS_IS_ORIG;

    if (p_dev->attr_mask & HID_SEC_REQUIRED) {
        service_id = BTM_SEC_SERVICE_HIDH_SEC_CTRL;
        mx_chan_id = HID_SEC_CHN;
    }
    BTM_SetOutService (p_dev->addr, service_id, mx_chan_id);

    /* Check if L2CAP started the connection process */
    if ((p_dev->conn.ctrl_cid = L2CA_ConnectReq (HID_PSM_CONTROL, p_dev->addr)) == 0) {
        HIDH_TRACE_WARNING ("HID-Host Originate failed");
        hh_cb.callback( dhandle,  hh_cb.devices[dhandle].addr, HID_HDEV_EVT_CLOSE,
                        HID_ERR_L2CAP_FAILED, NULL ) ;
    } else {
        /* Transition to the next appropriate state, waiting for connection confirm on control channel. */
        p_dev->conn.conn_state = HID_CONN_STATE_CONNECTING_CTRL;
    }

    return ( HID_SUCCESS );
}


/*******************************************************************************
**
** Function         find_conn_by_cid
**
** Description      This function finds a connection control block based on CID
**
** Returns          address of control block, or NULL if not found
**
*******************************************************************************/
static UINT8 find_conn_by_cid (UINT16 cid)
{
    UINT8      xx;

    for (xx = 0; xx < HID_HOST_MAX_DEVICES; xx++) {
        if ((hh_cb.devices[xx].in_use) && (hh_cb.devices[xx].conn.conn_state != HID_CONN_STATE_UNUSED)
                && ((hh_cb.devices[xx].conn.ctrl_cid == cid) || (hh_cb.devices[xx].conn.intr_cid == cid))) {
            break;
        }
    }

    return (xx);
}

void hidh_conn_dereg( void )
{
    L2CA_Deregister (HID_PSM_CONTROL);
    L2CA_Deregister (HID_PSM_INTERRUPT);
}

/*******************************************************************************
**
** Function         hidh_conn_retry
**
** Description      This function is called to retry a failed connection.
**
** Returns          void
**
*******************************************************************************/
static void hidh_conn_retry(  UINT8 dhandle )
{
    tHID_HOST_DEV_CTB *p_dev = &hh_cb.devices[dhandle];

    p_dev->conn.conn_state = HID_CONN_STATE_UNUSED;
    p_dev->conn.timer_entry.param = (UINT32) dhandle;
#if (HID_HOST_REPAGE_WIN > 0)
    btu_start_timer (&(p_dev->conn.timer_entry), BTU_TTYPE_HID_HOST_REPAGE_TO, HID_HOST_REPAGE_WIN);
#else
    hidh_proc_repage_timeout( &(p_dev->conn.timer_entry) );
#endif
}

#endif // HID_HOST_INCLUDED

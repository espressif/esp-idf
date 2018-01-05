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
 *  This file contains state machine and action routines for multiplexer
 *  channel of the RFCOMM unit
 *
 ******************************************************************************/
#include <string.h>
#include "bt_types.h"
#include "rfcdefs.h"
#include "l2cdefs.h"
#include "port_api.h"
#include "port_int.h"
#include "l2c_api.h"
#include "rfc_int.h"
#include "bt_defs.h"
#include "allocator.h"
#include "mutex.h"
#include "bt_target.h"
#if (defined RFCOMM_INCLUDED && RFCOMM_INCLUDED == TRUE)

#define L2CAP_SUCCESS   0
#define L2CAP_ERROR     1


/********************************************************************************/
/*              L O C A L    F U N C T I O N     P R O T O T Y P E S            */
/********************************************************************************/
static void rfc_mx_sm_state_idle (tRFC_MCB *p_mcb, UINT16 event, void *p_data);
static void rfc_mx_sm_state_wait_conn_cnf (tRFC_MCB *p_mcb, UINT16 event, void *p_data);
static void rfc_mx_sm_state_configure (tRFC_MCB *p_mcb, UINT16 event, void *p_data);
static void rfc_mx_sm_sabme_wait_ua (tRFC_MCB *p_mcb, UINT16 event, void *p_data);
static void rfc_mx_sm_state_wait_sabme (tRFC_MCB *p_mcb, UINT16 event, void *p_data);
static void rfc_mx_sm_state_connected (tRFC_MCB *p_mcb, UINT16 event, void *p_data);
static void rfc_mx_sm_state_disc_wait_ua (tRFC_MCB *p_mcb, UINT16 event, void *p_data);

static void rfc_mx_send_config_req (tRFC_MCB *p_mcb);
static void rfc_mx_conf_ind (tRFC_MCB *p_mcb, tL2CAP_CFG_INFO *p_cfg);
static void rfc_mx_conf_cnf (tRFC_MCB *p_mcb, tL2CAP_CFG_INFO *p_cfg);



/*******************************************************************************
**
** Function         rfc_mx_sm_execute
**
** Description      This function sends multiplexor events through the state
**                  machine.
**
** Returns          void
**
*******************************************************************************/
void rfc_mx_sm_execute (tRFC_MCB *p_mcb, UINT16 event, void *p_data)
{
    switch (p_mcb->state) {
    case RFC_MX_STATE_IDLE:
        rfc_mx_sm_state_idle (p_mcb, event, p_data);
        break;

    case RFC_MX_STATE_WAIT_CONN_CNF:
        rfc_mx_sm_state_wait_conn_cnf (p_mcb, event, p_data);
        break;

    case RFC_MX_STATE_CONFIGURE:
        rfc_mx_sm_state_configure (p_mcb, event, p_data);
        break;

    case RFC_MX_STATE_SABME_WAIT_UA:
        rfc_mx_sm_sabme_wait_ua (p_mcb, event, p_data);
        break;

    case RFC_MX_STATE_WAIT_SABME:
        rfc_mx_sm_state_wait_sabme (p_mcb, event, p_data);
        break;

    case RFC_MX_STATE_CONNECTED:
        rfc_mx_sm_state_connected (p_mcb, event, p_data);
        break;

    case RFC_MX_STATE_DISC_WAIT_UA:
        rfc_mx_sm_state_disc_wait_ua (p_mcb, event, p_data);
        break;

    }
}


/*******************************************************************************
**
** Function         rfc_mx_sm_state_idle
**
** Description      This function handles events when the multiplexer is in
**                  IDLE state. This state exists when connection is being
**                  initially established.
**
** Returns          void
**
*******************************************************************************/
void rfc_mx_sm_state_idle (tRFC_MCB *p_mcb, UINT16 event, void *p_data)
{
    RFCOMM_TRACE_EVENT ("rfc_mx_sm_state_idle - evt:%d", event);
    switch (event) {
    case RFC_MX_EVENT_START_REQ:

        /* Initialize L2CAP MTU */
        p_mcb->peer_l2cap_mtu = L2CAP_DEFAULT_MTU - RFCOMM_MIN_OFFSET - 1;

        if ((p_mcb->lcid = L2CA_ConnectReq (BT_PSM_RFCOMM, p_mcb->bd_addr)) == 0) {
            PORT_StartCnf (p_mcb, RFCOMM_ERROR);
            return;
        }
        /* Save entry for quicker access to mcb based on the LCID */
        rfc_save_lcid_mcb (p_mcb, p_mcb->lcid);

        p_mcb->state = RFC_MX_STATE_WAIT_CONN_CNF;
        return;

    case RFC_MX_EVENT_START_RSP:
    case RFC_MX_EVENT_CONN_CNF:
    case RFC_MX_EVENT_CONF_IND:
    case RFC_MX_EVENT_CONF_CNF:
        RFCOMM_TRACE_ERROR ("Mx error state %d event %d", p_mcb->state, event);
        return;

    case RFC_MX_EVENT_CONN_IND:

        rfc_timer_start (p_mcb, RFCOMM_CONN_TIMEOUT);
        L2CA_ConnectRsp (p_mcb->bd_addr, *((UINT8 *)p_data), p_mcb->lcid, L2CAP_CONN_OK, 0);

        rfc_mx_send_config_req (p_mcb);

        p_mcb->state = RFC_MX_STATE_CONFIGURE;
        return;

    case RFC_EVENT_SABME:
        break;

    case RFC_EVENT_UA:
    case RFC_EVENT_DM:
        return;

    case RFC_EVENT_DISC:
        rfc_send_dm (p_mcb, RFCOMM_MX_DLCI, TRUE);
        return;

    case RFC_EVENT_UIH:
        rfc_send_dm (p_mcb, RFCOMM_MX_DLCI, FALSE);
        return;
    }
    RFCOMM_TRACE_EVENT ("RFCOMM MX ignored - evt:%d in state:%d", event, p_mcb->state);
}


/*******************************************************************************
**
** Function         rfc_mx_sm_state_wait_conn_cnf
**
** Description      This function handles events when the multiplexer is
**                  waiting for Connection Confirm from L2CAP.
**
** Returns          void
**
*******************************************************************************/
void rfc_mx_sm_state_wait_conn_cnf (tRFC_MCB *p_mcb, UINT16 event, void *p_data)
{
    RFCOMM_TRACE_EVENT ("rfc_mx_sm_state_wait_conn_cnf - evt:%d", event);
    switch (event) {
    case RFC_MX_EVENT_START_REQ:
        RFCOMM_TRACE_ERROR ("Mx error state %d event %d", p_mcb->state, event);
        return;

    /* There is some new timing so that Config Ind comes before security is completed
       so we are still waiting fo the confirmation. */
    case RFC_MX_EVENT_CONF_IND:
        rfc_mx_conf_ind (p_mcb, (tL2CAP_CFG_INFO *)p_data);
        return;

    case RFC_MX_EVENT_CONN_CNF:
        if (*((UINT16 *)p_data) != L2CAP_SUCCESS) {
            p_mcb->state = RFC_MX_STATE_IDLE;

            PORT_StartCnf (p_mcb, *((UINT16 *)p_data));
            return;
        }
        p_mcb->state = RFC_MX_STATE_CONFIGURE;
        rfc_mx_send_config_req (p_mcb);
        return;

    case RFC_MX_EVENT_DISC_IND:
        p_mcb->state = RFC_MX_STATE_IDLE;
        PORT_CloseInd (p_mcb);
        return;

    case RFC_EVENT_TIMEOUT:
        p_mcb->state = RFC_MX_STATE_IDLE;
        L2CA_DisconnectReq (p_mcb->lcid);

        /* we gave up outgoing connection request then try peer's request */
        if (p_mcb->pending_lcid) {
            UINT16 i;
            UINT8  idx;

            RFCOMM_TRACE_DEBUG ("RFCOMM MX retry as acceptor in collision case - evt:%d in state:%d", event, p_mcb->state);

            rfc_save_lcid_mcb (NULL, p_mcb->lcid);
            p_mcb->lcid = p_mcb->pending_lcid;
            rfc_save_lcid_mcb (p_mcb, p_mcb->lcid);

            p_mcb->is_initiator = FALSE;

            /* update direction bit */
            for (i = 0; i < RFCOMM_MAX_DLCI; i += 2) {
                if ((idx = p_mcb->port_inx[i]) != 0) {
                    p_mcb->port_inx[i] = 0;
                    p_mcb->port_inx[i + 1] = idx;
                    rfc_cb.port.port[idx - 1].dlci += 1;
                    RFCOMM_TRACE_DEBUG ("RFCOMM MX - DLCI:%d -> %d", i, rfc_cb.port.port[idx - 1].dlci);
                }
            }

            rfc_mx_sm_execute (p_mcb, RFC_MX_EVENT_CONN_IND, &(p_mcb->pending_id));
        } else {
            PORT_CloseInd (p_mcb);
        }
        return;
    }
    RFCOMM_TRACE_EVENT ("RFCOMM MX ignored - evt:%d in state:%d", event, p_mcb->state);
}


/*******************************************************************************
**
** Function         rfc_mx_sm_state_configure
**
** Description      This function handles events when the multiplexer in the
**                  configuration state.
**
** Returns          void
**
*******************************************************************************/
void rfc_mx_sm_state_configure (tRFC_MCB *p_mcb, UINT16 event, void *p_data)
{
    RFCOMM_TRACE_EVENT ("rfc_mx_sm_state_configure - evt:%d", event);
    switch (event) {
    case RFC_MX_EVENT_START_REQ:
    case RFC_MX_EVENT_CONN_CNF:

        RFCOMM_TRACE_ERROR ("Mx error state %d event %d", p_mcb->state, event);
        return;

    case RFC_MX_EVENT_CONF_IND:
        rfc_mx_conf_ind (p_mcb, (tL2CAP_CFG_INFO *)p_data);
        return;

    case RFC_MX_EVENT_CONF_CNF:
        rfc_mx_conf_cnf (p_mcb, (tL2CAP_CFG_INFO *)p_data);
        return;

    case RFC_MX_EVENT_DISC_IND:
        p_mcb->state = RFC_MX_STATE_IDLE;
        PORT_CloseInd (p_mcb);
        return;

    case RFC_EVENT_TIMEOUT:
        p_mcb->state = RFC_MX_STATE_IDLE;
        L2CA_DisconnectReq (p_mcb->lcid);

        PORT_StartCnf (p_mcb, RFCOMM_ERROR);
        return;
    }
    RFCOMM_TRACE_EVENT ("RFCOMM MX ignored - evt:%d in state:%d", event, p_mcb->state);
}


/*******************************************************************************
**
** Function         rfc_mx_sm_sabme_wait_ua
**
** Description      This function handles events when the multiplexer sent
**                  SABME and is waiting for UA reply.
**
** Returns          void
**
*******************************************************************************/
void rfc_mx_sm_sabme_wait_ua (tRFC_MCB *p_mcb, UINT16 event, void *p_data)
{
    UNUSED(p_data);

    RFCOMM_TRACE_EVENT ("rfc_mx_sm_sabme_wait_ua - evt:%d", event);
    switch (event) {
    case RFC_MX_EVENT_START_REQ:
    case RFC_MX_EVENT_CONN_CNF:
        RFCOMM_TRACE_ERROR ("Mx error state %d event %d", p_mcb->state, event);
        return;

    /* workaround: we don't support reconfig */
    /* commented out until we support reconfig
    case RFC_MX_EVENT_CONF_IND:
        rfc_mx_conf_ind (p_mcb, (tL2CAP_CFG_INFO *)p_data);
        return;

    case RFC_MX_EVENT_CONF_CNF:
        rfc_mx_conf_cnf (p_mcb, (tL2CAP_CFG_INFO *)p_data);
        return;
    */

    case RFC_MX_EVENT_DISC_IND:
        p_mcb->state = RFC_MX_STATE_IDLE;
        PORT_CloseInd (p_mcb);
        return;

    case RFC_EVENT_UA:
        rfc_timer_stop (p_mcb);

        p_mcb->state      = RFC_MX_STATE_CONNECTED;
        p_mcb->peer_ready = TRUE;

        PORT_StartCnf (p_mcb, RFCOMM_SUCCESS);
        return;

    case RFC_EVENT_DM:
        rfc_timer_stop (p_mcb);
    /* Case falls through */

    case RFC_MX_EVENT_CONF_IND: /* workaround: we don't support reconfig */
    case RFC_MX_EVENT_CONF_CNF: /* workaround: we don't support reconfig */
    case RFC_EVENT_TIMEOUT:
        p_mcb->state = RFC_MX_STATE_IDLE;
        L2CA_DisconnectReq (p_mcb->lcid);

        PORT_StartCnf (p_mcb, RFCOMM_ERROR);
        return;
    }
    RFCOMM_TRACE_EVENT ("RFCOMM MX ignored - evt:%d in state:%d", event, p_mcb->state);
}

/*******************************************************************************
**
** Function         rfc_mx_sm_state_wait_sabme
**
** Description      This function handles events when the multiplexer is
**                  waiting for SABME on the acceptor side after configuration
**
** Returns          void
**
*******************************************************************************/
void rfc_mx_sm_state_wait_sabme (tRFC_MCB *p_mcb, UINT16 event, void *p_data)
{
    RFCOMM_TRACE_EVENT ("rfc_mx_sm_state_wait_sabme - evt:%d", event);
    switch (event) {
    case RFC_MX_EVENT_DISC_IND:
        p_mcb->state = RFC_MX_STATE_IDLE;
        PORT_CloseInd (p_mcb);
        return;

    case RFC_EVENT_SABME:
        /* if we gave up outgoing connection request */
        if (p_mcb->pending_lcid) {
            p_mcb->pending_lcid = 0;

            rfc_send_ua (p_mcb, RFCOMM_MX_DLCI);

            rfc_timer_stop (p_mcb);
            p_mcb->state      = RFC_MX_STATE_CONNECTED;
            p_mcb->peer_ready = TRUE;

            /* MX channel collision has been resolved, continue to open ports */
            PORT_StartCnf (p_mcb, RFCOMM_SUCCESS);
        } else {
            rfc_timer_stop (p_mcb);
            PORT_StartInd (p_mcb);
        }
        return;

    case RFC_MX_EVENT_START_RSP:
        if (*((UINT16 *)p_data) != RFCOMM_SUCCESS) {
            rfc_send_dm (p_mcb, RFCOMM_MX_DLCI, TRUE);
        } else {
            rfc_send_ua (p_mcb, RFCOMM_MX_DLCI);

            p_mcb->state      = RFC_MX_STATE_CONNECTED;
            p_mcb->peer_ready = TRUE;
        }
        return;

    case RFC_MX_EVENT_CONF_IND: /* workaround: we don't support reconfig */
    case RFC_MX_EVENT_CONF_CNF: /* workaround: we don't support reconfig */
    case RFC_EVENT_TIMEOUT:
        p_mcb->state = RFC_MX_STATE_IDLE;
        L2CA_DisconnectReq (p_mcb->lcid);

        PORT_CloseInd (p_mcb);
        return;
    }
    RFCOMM_TRACE_EVENT ("RFCOMM MX ignored - evt:%d in state:%d", event, p_mcb->state);
}


/*******************************************************************************
**
** Function         rfc_mx_sm_state_connected
**
** Description      This function handles events when the multiplexer is
**                  in the CONNECTED state
**
** Returns          void
**
*******************************************************************************/
void rfc_mx_sm_state_connected (tRFC_MCB *p_mcb, UINT16 event, void *p_data)
{
    UNUSED(p_data);

    RFCOMM_TRACE_EVENT ("rfc_mx_sm_state_connected - evt:%d", event);

    switch (event) {
    case RFC_EVENT_TIMEOUT:
    case RFC_MX_EVENT_CLOSE_REQ:
        rfc_timer_start (p_mcb, RFC_DISC_TIMEOUT);
        p_mcb->state = RFC_MX_STATE_DISC_WAIT_UA;
        rfc_send_disc (p_mcb, RFCOMM_MX_DLCI);
        return;

    case RFC_MX_EVENT_DISC_IND:
        p_mcb->state = RFC_MX_STATE_IDLE;
        PORT_CloseInd (p_mcb);
        return;

    case RFC_EVENT_DISC:
        /* Reply with UA.  If initiator bring down L2CAP connection */
        /* If server wait for some time if client decide to reinitiate channel */
        rfc_send_ua (p_mcb, RFCOMM_MX_DLCI);
        if (p_mcb->is_initiator) {
            L2CA_DisconnectReq (p_mcb->lcid);
        }
        /* notify all ports that connection is gone */
        PORT_CloseInd (p_mcb);
        return;
    }
    RFCOMM_TRACE_EVENT ("RFCOMM MX ignored - evt:%d in state:%d", event, p_mcb->state);
}


/*******************************************************************************
**
** Function         rfc_mx_sm_state_disc_wait_ua
**
** Description      This function handles events when the multiplexer sent
**                  DISC and is waiting for UA reply.
**
** Returns          void
**
*******************************************************************************/
void rfc_mx_sm_state_disc_wait_ua (tRFC_MCB *p_mcb, UINT16 event, void *p_data)
{
    BT_HDR *p_buf;

    RFCOMM_TRACE_EVENT ("rfc_mx_sm_state_disc_wait_ua - evt:%d", event);
    switch (event) {
    case RFC_EVENT_UA:
    case RFC_EVENT_DM:
    case RFC_EVENT_TIMEOUT:
        L2CA_DisconnectReq (p_mcb->lcid);

        if (p_mcb->restart_required) {
            /* Start Request was received while disconnecting.  Execute it again */
            if ((p_mcb->lcid = L2CA_ConnectReq (BT_PSM_RFCOMM, p_mcb->bd_addr)) == 0) {
                PORT_StartCnf (p_mcb, RFCOMM_ERROR);
                return;
            }
            /* Save entry for quicker access to mcb based on the LCID */
            rfc_save_lcid_mcb (p_mcb, p_mcb->lcid);

            /* clean up before reuse it */
            while ((p_buf = (BT_HDR *)fixed_queue_try_dequeue(p_mcb->cmd_q)) != NULL) {
                osi_free(p_buf);
            }

            rfc_timer_start (p_mcb, RFC_MCB_INIT_INACT_TIMER);

            p_mcb->is_initiator     = TRUE;
            p_mcb->restart_required = FALSE;
            p_mcb->local_cfg_sent   = FALSE;
            p_mcb->peer_cfg_rcvd    = FALSE;

            p_mcb->state = RFC_MX_STATE_WAIT_CONN_CNF;
            return;
        }
        rfc_release_multiplexer_channel (p_mcb);
        return;

    case RFC_EVENT_DISC:
        rfc_send_ua (p_mcb, RFCOMM_MX_DLCI);
        return;

    case RFC_EVENT_UIH:
        osi_free (p_data);
        rfc_send_dm (p_mcb, RFCOMM_MX_DLCI, FALSE);
        return;

    case RFC_MX_EVENT_START_REQ:
        p_mcb->restart_required = TRUE;
        return;

    case RFC_MX_EVENT_DISC_IND:
        p_mcb->state = RFC_MX_STATE_IDLE;
        PORT_CloseInd (p_mcb);
        return;

    case RFC_MX_EVENT_CLOSE_REQ:
        return;

    case RFC_MX_EVENT_QOS_VIOLATION_IND:
        break;
    }
    RFCOMM_TRACE_EVENT ("RFCOMM MX ignored - evt:%d in state:%d", event, p_mcb->state);
}


/*******************************************************************************
**
** Function         rfc_mx_send_config_req
**
** Description      This function handles L2CA_ConnectInd message from the
**                  L2CAP.  Accept connection.
**
*******************************************************************************/
static void rfc_mx_send_config_req (tRFC_MCB *p_mcb)
{
    tL2CAP_CFG_INFO cfg;

    RFCOMM_TRACE_EVENT ("rfc_mx_send_config_req");

    memset (&cfg, 0, sizeof (tL2CAP_CFG_INFO));

    cfg.mtu_present      = TRUE;
    cfg.mtu              = L2CAP_MTU_SIZE;

    /* Defaults set by memset
        cfg.flush_to_present = FALSE;
        cfg.qos_present      = FALSE;
        cfg.fcr_present      = FALSE;
        cfg.fcr.mode         = L2CAP_FCR_BASIC_MODE;
        cfg.fcs_present      = FALSE;
        cfg.fcs              = N/A when fcs_present is FALSE;
    */
    L2CA_ConfigReq (p_mcb->lcid, &cfg);
}


/*******************************************************************************
**
** Function         rfc_mx_conf_cnf
**
** Description      This function handles L2CA_ConfigCnf message from the
**                  L2CAP.  If result is not success tell upper layer that
**                  start has not been accepted.  If initiator send SABME
**                  on DLCI 0.  T1 is still running.
**
*******************************************************************************/
static void rfc_mx_conf_cnf (tRFC_MCB *p_mcb, tL2CAP_CFG_INFO *p_cfg)
{
    // RFCOMM_TRACE_EVENT ("rfc_mx_conf_cnf p_cfg:%08x res:%d ", p_cfg, (p_cfg) ? p_cfg->result : 0);

    if (p_cfg->result != L2CAP_CFG_OK) {
        if (p_mcb->is_initiator) {
            PORT_StartCnf (p_mcb, p_cfg->result);
            L2CA_DisconnectReq (p_mcb->lcid);
        }
        rfc_release_multiplexer_channel (p_mcb);
        return;
    }

    p_mcb->local_cfg_sent = TRUE;
    if ((p_mcb->state == RFC_MX_STATE_CONFIGURE) && p_mcb->peer_cfg_rcvd) {
        if (p_mcb->is_initiator) {
            p_mcb->state = RFC_MX_STATE_SABME_WAIT_UA;
            rfc_send_sabme (p_mcb, RFCOMM_MX_DLCI);
            rfc_timer_start (p_mcb, RFC_T1_TIMEOUT);
        } else {
            p_mcb->state = RFC_MX_STATE_WAIT_SABME;
            rfc_timer_start (p_mcb, RFCOMM_CONN_TIMEOUT); /* - increased from T2=20 to CONN=120
                                                to allow the user more than 10 sec to type in the
                                                pin which can be e.g. 16 digits */
        }
    }
}


/*******************************************************************************
**
** Function         rfc_mx_conf_ind
**
** Description      This function handles L2CA_ConfigInd message from the
**                  L2CAP.  Send the L2CA_ConfigRsp message.
**
*******************************************************************************/
static void rfc_mx_conf_ind (tRFC_MCB *p_mcb, tL2CAP_CFG_INFO *p_cfg)
{
    /* Save peer L2CAP MTU if present */
    /* RFCOMM adds 3-4 bytes in the beginning and 1 bytes FCS */
    if (p_cfg->mtu_present) {
        p_mcb->peer_l2cap_mtu = p_cfg->mtu - RFCOMM_MIN_OFFSET - 1;
    } else {
        p_mcb->peer_l2cap_mtu = L2CAP_DEFAULT_MTU - RFCOMM_MIN_OFFSET - 1;
    }

    p_cfg->mtu_present      = FALSE;
    p_cfg->flush_to_present = FALSE;
    p_cfg->qos_present      = FALSE;

    p_cfg->result = L2CAP_CFG_OK;

    L2CA_ConfigRsp (p_mcb->lcid, p_cfg);

    p_mcb->peer_cfg_rcvd = TRUE;
    if ((p_mcb->state == RFC_MX_STATE_CONFIGURE) && p_mcb->local_cfg_sent) {
        if (p_mcb->is_initiator) {
            p_mcb->state = RFC_MX_STATE_SABME_WAIT_UA;
            rfc_send_sabme (p_mcb, RFCOMM_MX_DLCI);
            rfc_timer_start (p_mcb, RFC_T1_TIMEOUT);
        } else {
            p_mcb->state = RFC_MX_STATE_WAIT_SABME;
            rfc_timer_start (p_mcb, RFCOMM_CONN_TIMEOUT); /* - increased from T2=20 to CONN=120
                                                to allow the user more than 10 sec to type in the
                                                pin which can be e.g. 16 digits */
        }
    }
}

#endif ///(defined RFCOMM_INCLUDED && RFCOMM_INCLUDED == TRUE)
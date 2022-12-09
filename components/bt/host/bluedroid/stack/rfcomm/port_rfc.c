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
 *  This module contains functions for port emulation entity and RFCOMM
 *  communications
 *
 ******************************************************************************/
#include <string.h>

#include "common/bt_target.h"
#include "stack/rfcdefs.h"
#include "stack/port_api.h"
#include "btm_int.h"
#include "stack/btm_api.h"
#include "port_int.h"
#include "rfc_int.h"
#include "common/bt_defs.h"
#include "osi/mutex.h"
#include "osi/allocator.h"
#if (defined RFCOMM_INCLUDED && RFCOMM_INCLUDED == TRUE)
/*
** Local function definitions
*/
UINT32 port_rfc_send_tx_data (tPORT *p_port);
void   port_rfc_closed (tPORT *p_port, UINT8 res);
void   port_get_credits (tPORT *p_port, UINT8 k);


/*******************************************************************************
**
** Function         port_open_continue
**
** Description      This function is called after security manager completes
**                  required security checks.
**
** Returns          void
**
*******************************************************************************/
int port_open_continue (tPORT *p_port)
{
    tRFC_MCB *p_mcb;

    RFCOMM_TRACE_EVENT ("port_open_continue, p_port:%p", p_port);

    /* Check if multiplexer channel has already been established */
    if ((p_mcb = rfc_alloc_multiplexer_channel (p_port->bd_addr, TRUE)) == NULL) {
        RFCOMM_TRACE_WARNING ("port_open_continue no mx channel");
        port_release_port (p_port);
        return (PORT_NO_RESOURCES);
    }

    p_port->rfc.p_mcb = p_mcb;

    p_mcb->port_inx[p_port->dlci] = p_port->inx;

    /* Connection is up and we know local and remote features, select MTU */
    port_select_mtu (p_port);

    if (p_mcb->state == RFC_MX_STATE_CONNECTED) {
        RFCOMM_ParNegReq (p_mcb, p_port->dlci, p_port->mtu);
    } else if ((p_mcb->state == RFC_MX_STATE_IDLE)
               || (p_mcb->state == RFC_MX_STATE_DISC_WAIT_UA)) {
        /* In RFC_MX_STATE_IDLE state, MX state machine will create connection           */
        /* In RFC_MX_STATE_DISC_WAIT_UA state, MX state machine will recreate connection */
        /*    after disconnecting is completed                                           */
        RFCOMM_StartReq (p_mcb);
    } else {
        /* MX state machine ignores RFC_MX_EVENT_START_REQ in these states */
        /* When it enters RFC_MX_STATE_CONNECTED, it will check any openning ports */
        RFCOMM_TRACE_DEBUG ("port_open_continue: mx state(%d) mx channel is openning", p_mcb->state);
    }
    return (PORT_SUCCESS);
}


/*******************************************************************************
**
** Function         port_start_control
**
** Description      This function is called in the BTU_TASK context to
**                  send control information
**
** Returns          void
**
*******************************************************************************/
void port_start_control (tPORT *p_port)
{
    tRFC_MCB *p_mcb = p_port->rfc.p_mcb;

    if (p_mcb == NULL) {
        return;
    }

    RFCOMM_ControlReq (p_mcb, p_port->dlci, &p_port->local_ctrl);
}


/*******************************************************************************
**
** Function         port_start_par_neg
**
** Description      This function is called in the BTU_TASK context to
**                  send configuration information
**
** Returns          void
**
*******************************************************************************/
void port_start_par_neg (tPORT *p_port)
{
    tRFC_MCB *p_mcb = p_port->rfc.p_mcb;

    if (p_mcb == NULL) {
        return;
    }

    RFCOMM_PortNegReq (p_mcb, p_port->dlci, &p_port->user_port_pars);
}


/*******************************************************************************
**
** Function         port_start_close
**
** Description      This function is called in the BTU_TASK context to
**                  release DLC
**
** Returns          void
**
*******************************************************************************/
void port_start_close (tPORT *p_port)
{
    tRFC_MCB *p_mcb = p_port->rfc.p_mcb;
    UINT8  old_signals;
    UINT32 events = 0;

    /* At first indicate to the user that signals on the connection were dropped */
    p_port->line_status |= LINE_STATUS_FAILED;
    old_signals = p_port->peer_ctrl.modem_signal;

    p_port->peer_ctrl.modem_signal &= ~(PORT_DTRDSR_ON | PORT_CTSRTS_ON | PORT_DCD_ON);

    events |= port_get_signal_changes (p_port, old_signals, p_port->peer_ctrl.modem_signal);

    if (p_port->ev_mask & PORT_EV_CONNECT_ERR) {
        events |= PORT_EV_CONNECT_ERR;
    }

    if (p_port->ev_mask & PORT_EV_ERR) {
        events |= PORT_EV_ERR;
    }

    if ((p_port->p_callback != NULL) && events) {
        p_port->p_callback (events, p_port->inx);
    }


    /* Check if RFCOMM side has been closed while the message was queued */
    if ((p_mcb == NULL) || (p_port->rfc.state == RFC_STATE_CLOSED)) {
        /* Call management callback function before calling port_release_port() to clear tPort */
        if (p_port->p_mgmt_callback) {
            p_port->p_mgmt_callback (PORT_CLOSED, p_port->inx);
        }

        port_release_port (p_port);
    } else {
        RFCOMM_DlcReleaseReq (p_mcb, p_port->dlci);
    }
}


/*******************************************************************************
**
** Function         PORT_StartCnf
**
** Description      This function is called from the RFCOMM layer when
**                  establishing of the multiplexer channel is completed.
**                  Continue establishing of the connection for all ports that
**                  are in the OPENING state
**
*******************************************************************************/
void PORT_StartCnf (tRFC_MCB *p_mcb, UINT16 result)
{
    tPORT   *p_port;
    int     i;
    BOOLEAN no_ports_up = TRUE;

    RFCOMM_TRACE_EVENT ("PORT_StartCnf result:%d", result);

    p_port = &rfc_cb.port.port[0];
    for (i = 0; i < MAX_RFC_PORTS; i++, p_port++) {
        if (p_port->rfc.p_mcb == p_mcb) {
            no_ports_up = FALSE;

            if (result == RFCOMM_SUCCESS) {
                RFCOMM_ParNegReq (p_mcb, p_port->dlci, p_port->mtu);
            } else {
                RFCOMM_TRACE_WARNING ("PORT_StartCnf failed result:%d", result);

                /* Warning: result is also set to 4 when l2cap connection
                   fails due to l2cap connect cnf (no_resources) */
                if ( result == HCI_ERR_PAGE_TIMEOUT ) {
                    p_port->error = PORT_PAGE_TIMEOUT;
                } else {
                    p_port->error = PORT_START_FAILED;
                }

                rfc_release_multiplexer_channel (p_mcb);
                p_port->rfc.p_mcb = NULL;

                /* Send event to the application */
                if (p_port->p_callback && (p_port->ev_mask & PORT_EV_CONNECT_ERR)) {
                    (p_port->p_callback)(PORT_EV_CONNECT_ERR, p_port->inx);
                }

                if (p_port->p_mgmt_callback) {
                    p_port->p_mgmt_callback (PORT_START_FAILED, p_port->inx);
                }

                port_release_port (p_port);
            }
        }
    }

    /* There can be a situation when after starting connection, user closes the */
    /* port, we can catch it here to close multiplexor channel */
    if (no_ports_up) {
        rfc_check_mcb_active (p_mcb);
    }
}


/*******************************************************************************
**
** Function         PORT_StartInd
**
** Description      This function is called from the RFCOMM layer when
**                  some peer device wants to establish a multiplexer
**                  connection.  Check if there are any ports open with this
**                  or not assigned multiplexer.
**
*******************************************************************************/
void PORT_StartInd (tRFC_MCB *p_mcb)
{
    tPORT *p_port;
    int   i;

    RFCOMM_TRACE_EVENT ("PORT_StartInd");

    p_port = &rfc_cb.port.port[0];
    for (i = 0; i < MAX_RFC_PORTS; i++, p_port++) {
        if ((p_port->rfc.p_mcb == NULL)
                || (p_port->rfc.p_mcb == p_mcb)) {
            RFCOMM_TRACE_DEBUG("PORT_StartInd, RFCOMM_StartRsp RFCOMM_SUCCESS: p_mcb:%p", p_mcb);
            RFCOMM_StartRsp (p_mcb, RFCOMM_SUCCESS);
            return;
        }
    }
    RFCOMM_StartRsp (p_mcb, RFCOMM_ERROR);
}


/*******************************************************************************
**
** Function         PORT_ParNegInd
**
** Description      This function is called from the RFCOMM layer to change
**                  DLCI parameters (currently only MTU is negotiated).
**                  If can not find the port do not accept the request.
**                  Otherwise save the MTU size supported by the peer.
**
*******************************************************************************/
void PORT_ParNegInd (tRFC_MCB *p_mcb, UINT8 dlci, UINT16 mtu, UINT8 cl, UINT8 k)
{
    tPORT *p_port = port_find_mcb_dlci_port (p_mcb, dlci);
    UINT8 our_cl;
    UINT8 our_k;

    RFCOMM_TRACE_EVENT ("PORT_ParNegInd dlci:%d mtu:%d", dlci, mtu);

    if (!p_port) {
        /* This can be a first request for this port */
        p_port = port_find_dlci_port (dlci);
        if (!p_port) {
            /* If the port cannot be opened, send a DM.  Per Errata 1205 */
            rfc_send_dm(p_mcb, dlci, FALSE);
            /* check if this is the last port open, some headsets have
            problem, they don't disconnect if we send DM */
            rfc_check_mcb_active( p_mcb );
            RFCOMM_TRACE_EVENT( "PORT_ParNegInd: port not found" );
            return;
        }
        p_mcb->port_inx[dlci] = p_port->inx;
    }

    memcpy (p_port->bd_addr, p_mcb->bd_addr, BD_ADDR_LEN);

    /* Connection is up and we know local and remote features, select MTU */
    port_select_mtu (p_port);

    p_port->rfc.p_mcb   = p_mcb;
    p_port->mtu         = (p_port->mtu < mtu) ? p_port->mtu : mtu;
    p_port->peer_mtu    = p_port->mtu;

    /* Negotiate the flow control mechanism.  If flow control mechanism for */
    /* mux has not been set yet, set it now.  If either we or peer wants TS 07.10, */
    /* use that.  Otherwise both must want credit based, so use that. If flow is */
    /* already defined for this mux, we respond with that value. */
    if (p_mcb->flow == PORT_FC_UNDEFINED) {
        if ((PORT_FC_DEFAULT == PORT_FC_TS710) || (cl == RFCOMM_PN_CONV_LAYER_TYPE_1)) {
            p_mcb->flow = PORT_FC_TS710;
        } else {
            p_mcb->flow = PORT_FC_CREDIT;
        }
    }

    /* Regardless of our flow control mechanism, if the PN cl is zero, we must */
    /* respond with zero.  "A responding implementation must set this field to 14 */
    /* if (and only if) the PN request was 15."  This could happen if a PN is sent */
    /* after the DLCI is already established-- the PN in that case must have cl = 0. */
    /* See RFCOMM spec 5.5.3 */
    if (cl == RFCOMM_PN_CONV_LAYER_TYPE_1) {
        our_cl = RFCOMM_PN_CONV_LAYER_TYPE_1;
        our_k = 0;
    } else if (p_mcb->flow == PORT_FC_CREDIT) {
        /* get credits */
        port_get_credits (p_port, k);

        /* Set convergence layer and number of credits (k) */
        our_cl = RFCOMM_PN_CONV_LAYER_CBFC_R;
        our_k = (p_port->credit_rx_max < RFCOMM_K_MAX) ? p_port->credit_rx_max : RFCOMM_K_MAX;
        p_port->credit_rx = our_k;
    } else {
        /* must not be using credit based flow control; use TS 7.10 */
        our_cl = RFCOMM_PN_CONV_LAYER_TYPE_1;
        our_k = 0;
    }
    RFCOMM_ParNegRsp (p_mcb, dlci, p_port->mtu, our_cl, our_k);
}


/*******************************************************************************
**
** Function         PORT_ParNegCnf
**
** Description      This function is called from the RFCOMM layer to change
**                  DLCI parameters (currently only MTU is negotiated).
**                  Save the MTU size supported by the peer.
**                  If the confirmation is received during the port opening
**                  procedure send EstablishRequest to continue.
**
*******************************************************************************/
void PORT_ParNegCnf (tRFC_MCB *p_mcb, UINT8 dlci, UINT16 mtu, UINT8 cl, UINT8 k)
{
    tPORT   *p_port = port_find_mcb_dlci_port (p_mcb, dlci);

    RFCOMM_TRACE_EVENT ("PORT_ParNegCnf dlci:%d mtu:%d cl: %d k: %d", dlci, mtu, cl, k);

    if (!p_port) {
        return;
    }

    /* Flow control mechanism not set yet.  Negotiate flow control mechanism. */
    if (p_mcb->flow == PORT_FC_UNDEFINED) {
        /* Our stack is configured for TS07.10 and they responded with credit-based. */
        /* This is illegal-- negotiation fails. */
        if ((PORT_FC_DEFAULT == PORT_FC_TS710) && (cl == RFCOMM_PN_CONV_LAYER_CBFC_R)) {
            rfc_send_disc (p_mcb, p_port->dlci);
            rfc_port_closed (p_port);
            return;
        }
        /* Our stack is configured for credit-based and they responded with credit-based. */
        else if (cl == RFCOMM_PN_CONV_LAYER_CBFC_R) {
            p_mcb->flow = PORT_FC_CREDIT;
        }
        /* They responded with any other value.  Treat this as negotiation to TS07.10. */
        else {
            p_mcb->flow = PORT_FC_TS710;
        }
    }
    /* If mux flow control mechanism set, we honor that setting regardless of */
    /* the CL value in their response.  This allows us to gracefully accept any */
    /* illegal PN negotiation scenarios. */

    p_port->mtu         = (p_port->mtu < mtu) ? p_port->mtu : mtu;
    p_port->peer_mtu    = p_port->mtu;

    if (p_mcb->flow == PORT_FC_CREDIT) {
        port_get_credits (p_port, k);
    }

    if (p_port->state == PORT_STATE_OPENING) {
        RFCOMM_DlcEstablishReq (p_mcb, p_port->dlci, p_port->mtu);
    }
}


/*******************************************************************************
**
** Function         PORT_DlcEstablishInd
**
** Description      This function is called from the RFCOMM layer when peer
**                  device wants to establish a new DLC.  If this is not the
**                  first message in the establishment procedure port_handle
**                  has a handle to the port control block otherwise the control
**                  block should be found based on the muliplexer channel and
**                  dlci.  The block should be allocated allocated before
**                  meaning that application already made open.
**
*******************************************************************************/
void PORT_DlcEstablishInd (tRFC_MCB *p_mcb, UINT8 dlci, UINT16 mtu)
{
    tPORT *p_port = port_find_mcb_dlci_port (p_mcb, dlci);

    RFCOMM_TRACE_DEBUG ("PORT_DlcEstablishInd p_mcb:%p, dlci:%d mtu:%di, p_port:%p", p_mcb, dlci, mtu, p_port);
    RFCOMM_TRACE_DEBUG ("PORT_DlcEstablishInd p_mcb addr:%02x:%02x:%02x:%02x:%02x:%02x",
                        p_mcb->bd_addr[0], p_mcb->bd_addr[1], p_mcb->bd_addr[2],
                        p_mcb->bd_addr[3], p_mcb->bd_addr[4], p_mcb->bd_addr[5]);

    if (!p_port) {
        /* This can be a first request for this port */
        p_port = port_find_dlci_port (dlci);
        if (!p_port) {
            RFCOMM_DlcEstablishRsp (p_mcb, dlci, 0, RFCOMM_ERROR);
            return;
        }
        p_mcb->port_inx[dlci] = p_port->inx;
    }

    /* If L2CAP's mtu less then RFCOMM's take it */
    if (mtu && (mtu < p_port->peer_mtu)) {
        p_port->peer_mtu = mtu;
    }

    /* If there was an inactivity timer running for MCB stop it */
    rfc_timer_stop (p_mcb);

    RFCOMM_DlcEstablishRsp (p_mcb, dlci, p_port->mtu, RFCOMM_SUCCESS);

    /* This is the server side.  If application wants to know when connection */
    /* is established, thats the place */
    if (p_port->p_callback && (p_port->ev_mask & PORT_EV_CONNECTED)) {
        (p_port->p_callback)(PORT_EV_CONNECTED, p_port->inx);
    }

    if (p_port->p_mgmt_callback) {
        p_port->p_mgmt_callback (PORT_SUCCESS, p_port->inx);
    }

    p_port->state = PORT_STATE_OPENED;
}


/*******************************************************************************
**
** Function         PORT_DlcEstablishCnf
**
** Description      This function is called from the RFCOMM layer when peer
**                  acknowledges establish procedure (SABME/UA).  Send reply
**                  to the user and set state to OPENED if result was
**                  successfull.
**
*******************************************************************************/
void PORT_DlcEstablishCnf (tRFC_MCB *p_mcb, UINT8 dlci, UINT16 mtu, UINT16 result)
{
    tPORT  *p_port = port_find_mcb_dlci_port (p_mcb, dlci);

    RFCOMM_TRACE_EVENT ("PORT_DlcEstablishCnf dlci:%d mtu:%d result:%d", dlci, mtu, result);

    if (!p_port) {
        return;
    }

    if (result != RFCOMM_SUCCESS) {
        p_port->error = PORT_START_FAILED;
        port_rfc_closed (p_port, PORT_START_FAILED);
        return;
    }

    /* If L2CAP's mtu less then RFCOMM's take it */
    if (mtu && (mtu < p_port->peer_mtu)) {
        p_port->peer_mtu = mtu;
    }

    /* If there was an inactivity timer running for MCB stop it */
    rfc_timer_stop (p_mcb);

    if (p_port->p_callback && (p_port->ev_mask & PORT_EV_CONNECTED)) {
        (p_port->p_callback)(PORT_EV_CONNECTED, p_port->inx);
    }

    if (p_port->p_mgmt_callback) {
        p_port->p_mgmt_callback (PORT_SUCCESS, p_port->inx);
    }

    p_port->state = PORT_STATE_OPENED;

    /* RPN is required only if we want to tell DTE how the port should be opened */
    if ((p_port->uuid == UUID_SERVCLASS_DIALUP_NETWORKING)
            || (p_port->uuid == UUID_SERVCLASS_FAX)) {
        RFCOMM_PortNegReq (p_port->rfc.p_mcb, p_port->dlci, NULL);
    } else {
        RFCOMM_ControlReq (p_port->rfc.p_mcb, p_port->dlci, &p_port->local_ctrl);
    }
}


/*******************************************************************************
**
** Function         PORT_PortNegInd
**
** Description      This function is called from the RFCOMM layer when peer
**                  device wants to set parameters of the port.  As per the spec
**                  this message has to be sent before the first data packet
**                  and can be sent before establish.  The block should be
**                  allocated before meaning that application already made open.
**
*******************************************************************************/
void PORT_PortNegInd (tRFC_MCB *p_mcb, UINT8 dlci, tPORT_STATE *p_pars,
                      UINT16 param_mask)
{
    tPORT *p_port = port_find_mcb_dlci_port (p_mcb, dlci);

    RFCOMM_TRACE_EVENT ("PORT_PortNegInd");

    if (!p_port) {
        /* This can be a first request for this port */
        p_port = port_find_dlci_port (dlci);
        if (!p_port) {
            RFCOMM_PortNegRsp (p_mcb, dlci, p_pars, 0);
            return;
        }
        p_mcb->port_inx[dlci] = p_port->inx;
    }

    /* Check if the flow control is acceptable on local side */
    p_port->peer_port_pars = *p_pars;
    RFCOMM_PortNegRsp (p_mcb, dlci, p_pars, param_mask);
}


/*******************************************************************************
**
** Function         PORT_PortNegCnf
**
** Description      This function is called from the RFCOMM layer to change
**                  state for the port.  Propagate change to the user.
**
*******************************************************************************/
void PORT_PortNegCnf (tRFC_MCB *p_mcb, UINT8 dlci, tPORT_STATE *p_pars, UINT16 result)
{
    tPORT  *p_port = port_find_mcb_dlci_port (p_mcb, dlci);
    UNUSED(p_pars);

    RFCOMM_TRACE_EVENT ("PORT_PortNegCnf");

    if (!p_port) {
        RFCOMM_TRACE_WARNING ("PORT_PortNegCnf no port");
        return;
    }
    /* Port negotiation failed. Drop the connection */
    if (result != RFCOMM_SUCCESS) {
        p_port->error = PORT_PORT_NEG_FAILED;

        RFCOMM_DlcReleaseReq (p_mcb, p_port->dlci);

        port_rfc_closed (p_port, PORT_PORT_NEG_FAILED);
        return;
    }

    if (!(p_port->port_ctrl & PORT_CTRL_REQ_SENT)) {
        RFCOMM_ControlReq (p_port->rfc.p_mcb, p_port->dlci, &p_port->local_ctrl);
    } else {
        RFCOMM_TRACE_WARNING ("PORT_PortNegCnf Control Already sent");
    }
}


/*******************************************************************************
**
** Function         PORT_ControlInd
**
** Description      This function is called from the RFCOMM layer on the modem
**                  signal change.  Propagate change to the user.
**
*******************************************************************************/
void PORT_ControlInd (tRFC_MCB *p_mcb, UINT8 dlci, tPORT_CTRL *p_pars)
{
    tPORT  *p_port = port_find_mcb_dlci_port (p_mcb, dlci);
    UINT32 event;
    UINT8  old_signals;

    RFCOMM_TRACE_EVENT ("PORT_ControlInd");

    if (!p_port) {
        return;
    }

    old_signals = p_port->peer_ctrl.modem_signal;

    event = port_get_signal_changes (p_port, old_signals, p_pars->modem_signal);

    p_port->peer_ctrl = *p_pars;

    if (!(p_port->port_ctrl & PORT_CTRL_REQ_SENT)) {
        RFCOMM_ControlReq (p_port->rfc.p_mcb, p_port->dlci, &p_port->local_ctrl);
    } else {
        /* If this is the first time we received control RFCOMM is connected */
        if (!(p_port->port_ctrl & PORT_CTRL_IND_RECEIVED)) {
            event |= (PORT_EV_CONNECTED & p_port->ev_mask);
        }

        if (p_port->port_ctrl & PORT_CTRL_REQ_CONFIRMED) {
            event |= port_rfc_send_tx_data(p_port);
        }
    }

    p_port->port_ctrl |= (PORT_CTRL_IND_RECEIVED | PORT_CTRL_IND_RESPONDED);

    if (p_pars->break_signal) {
        event |= (PORT_EV_BREAK & p_port->ev_mask);
    }

    /* execute call back function only if the application is registered for events */
    if (event && p_port->p_callback) {
        (p_port->p_callback)(event, p_port->inx);
    }

    RFCOMM_TRACE_EVENT ("PORT_ControlInd DTR_DSR : %d, RTS_CTS : %d, RI : %d, DCD : %d",
                        ((p_port->peer_ctrl.modem_signal & MODEM_SIGNAL_DTRDSR) ? 1 : 0),
                        ((p_port->peer_ctrl.modem_signal & MODEM_SIGNAL_RTSCTS) ? 1 : 0),
                        ((p_port->peer_ctrl.modem_signal & MODEM_SIGNAL_RI) ? 1 : 0),
                        ((p_port->peer_ctrl.modem_signal & MODEM_SIGNAL_DCD) ? 1 : 0));

}


/*******************************************************************************
**
** Function         PORT_ControlCnf
**
** Description      This function is called from the RFCOMM layer when
**                  peer acknowleges change of the modem signals.
**
*******************************************************************************/
void PORT_ControlCnf (tRFC_MCB *p_mcb, UINT8 dlci, tPORT_CTRL *p_pars)
{
    tPORT *p_port = port_find_mcb_dlci_port (p_mcb, dlci);
    UINT32 event = 0;
    UNUSED(p_pars);

    RFCOMM_TRACE_EVENT ("PORT_ControlCnf");

    if (!p_port) {
        return;
    }

    if (!(p_port->port_ctrl & PORT_CTRL_REQ_CONFIRMED)) {
        p_port->port_ctrl |= PORT_CTRL_REQ_CONFIRMED;

        if (p_port->port_ctrl & PORT_CTRL_IND_RECEIVED) {
            event = (p_port->ev_mask & PORT_EV_CONNECTED);
        }
    }

    if (p_port->port_ctrl & PORT_CTRL_IND_RECEIVED) {
        event |= port_rfc_send_tx_data(p_port);
    }

    /* execute call back function only if the application is registered for events */
    if (event && p_port->p_callback) {
        (p_port->p_callback)(event, p_port->inx);
    }
}


/*******************************************************************************
**
** Function         PORT_LineStatusInd
**
** Description      This function is called from the RFCOMM layer when
**                  peer indicates change in the line status
**
*******************************************************************************/
void PORT_LineStatusInd (tRFC_MCB *p_mcb, UINT8 dlci, UINT8 line_status)
{
    tPORT  *p_port = port_find_mcb_dlci_port (p_mcb, dlci);
    UINT32 event = 0;

    RFCOMM_TRACE_EVENT ("PORT_LineStatusInd");

    if (!p_port) {
        return;
    }

    p_port->line_status |= line_status;

    if (line_status & PORT_ERR_OVERRUN) {
        event |= PORT_EV_OVERRUN;
    }

    if (line_status & PORT_ERR_BREAK) {
        event |= PORT_EV_BREAK;
    }

    if (line_status & ~(PORT_ERR_OVERRUN | PORT_ERR_BREAK)) {
        event |= PORT_EV_ERR;
    }

    if ((p_port->p_callback != NULL) && (p_port->ev_mask & event)) {
        p_port->p_callback ((p_port->ev_mask & event), p_port->inx);
    }
}


/*******************************************************************************
**
** Function         PORT_DlcReleaseInd
**
** Description      This function is called from the RFCOMM layer when
**                  DLC connection is released.
**
*******************************************************************************/
void PORT_DlcReleaseInd (tRFC_MCB *p_mcb, UINT8 dlci)
{
    tPORT  *p_port = port_find_mcb_dlci_port (p_mcb, dlci);

    RFCOMM_TRACE_EVENT ("PORT_DlcReleaseInd");

    if (!p_port) {
        return;
    }

    port_rfc_closed (p_port, PORT_CLOSED);
}


/*******************************************************************************
**
** Function         PORT_CloseInd
**
** Description      This function is called from the RFCOMM layer when
**                  multiplexer connection is released.
**
*******************************************************************************/
void PORT_CloseInd (tRFC_MCB *p_mcb)
{
    tPORT  *p_port;
    int    i;

    RFCOMM_TRACE_EVENT ("PORT_CloseInd");

    p_port = &rfc_cb.port.port[0];
    for (i = 0; i < MAX_RFC_PORTS; i++, p_port++) {
        if (p_port->rfc.p_mcb == p_mcb) {
            port_rfc_closed (p_port, PORT_PEER_CONNECTION_FAILED);
        }
    }
    rfc_release_multiplexer_channel (p_mcb);
}

/*******************************************************************************
**
** Function         Port_TimeOutCloseMux
**
** Description      This function is called when RFCOMM timesout on a command
**                  as a result multiplexer connection is closed.
**
*******************************************************************************/
void Port_TimeOutCloseMux (tRFC_MCB *p_mcb)
{
    tPORT  *p_port;
    int    i;

    RFCOMM_TRACE_EVENT ("Port_TimeOutCloseMux");

    p_port = &rfc_cb.port.port[0];
    for (i = 0; i < MAX_RFC_PORTS; i++, p_port++) {
        if (p_port->rfc.p_mcb == p_mcb) {
            port_rfc_closed (p_port, PORT_PEER_TIMEOUT);
        }
    }
}


/*******************************************************************************
**
** Function         PORT_DataInd
**
** Description      This function is called from the RFCOMM layer when data
**                  buffer is received from the peer.
**
*******************************************************************************/
void PORT_DataInd (tRFC_MCB *p_mcb, UINT8 dlci, BT_HDR *p_buf)
{
    tPORT  *p_port = port_find_mcb_dlci_port (p_mcb, dlci);
    UINT8  rx_char1;
    UINT32 events = 0;
    UINT8  *p;
    int    i;

    RFCOMM_TRACE_EVENT("PORT_DataInd with data length %d, p_mcb:%p,p_port:%p,dlci:%d",
                       p_buf->len, p_mcb, p_port, dlci);
    if (!p_port) {
        osi_free (p_buf);
        return;
    }
    /* If client registered callout callback with flow control we can just deliver receive data */
    if (p_port->p_data_co_callback) {
        /* Another packet is delivered to user.  Send credits to peer if required */

        if (p_port->p_data_co_callback(p_port->inx, (UINT8 *)p_buf, -1, DATA_CO_CALLBACK_TYPE_INCOMING)) {
            // do nothing, flow control credits will be given upon upper-layer request;
            // port_flow_control_peer(p_port, TRUE, 1);
        } else {
            port_flow_control_peer(p_port, FALSE, 0);
        }
        //osi_free (p_buf);
        return;
    } else {
        RFCOMM_TRACE_DEBUG("PORT_DataInd, p_port:%p, p_data_co_callback is null", p_port);
    }
    /* If client registered callback we can just deliver receive data */
    if (p_port->p_data_callback) {
        /* Another packet is delivered to user.  Send credits to peer if required */
        port_flow_control_peer(p_port, TRUE, 1);

        p_port->p_data_callback (p_port->inx, (UINT8 *)(p_buf + 1) + p_buf->offset, p_buf->len);
        osi_free (p_buf);
        return;
    }

    /* Check if rx queue exceeds the limit */
    if ((p_port->rx.queue_size + p_buf->len > PORT_RX_CRITICAL_WM)
     || (fixed_queue_length(p_port->rx.queue) + 1 > p_port->rx_buf_critical)) {
        RFCOMM_TRACE_EVENT ("PORT_DataInd. Buffer over run. Dropping the buffer");
        osi_free (p_buf);

        RFCOMM_LineStatusReq (p_mcb, dlci, LINE_STATUS_OVERRUN);
        return;
    }

    /* If user registered to receive notification when a particular byte is */
    /* received we mast check all received bytes */
    if (((rx_char1 = p_port->user_port_pars.rx_char1) != 0)
            && (p_port->ev_mask & PORT_EV_RXFLAG)) {
        for (i = 0, p = (UINT8 *)(p_buf + 1) + p_buf->offset; i < p_buf->len; i++) {
            if (*p++ == rx_char1) {
                events |= PORT_EV_RXFLAG;
                break;
            }
        }
    }

    osi_mutex_global_lock();

    fixed_queue_enqueue(p_port->rx.queue, p_buf, FIXED_QUEUE_MAX_TIMEOUT);
    p_port->rx.queue_size += p_buf->len;

    osi_mutex_global_unlock();

    /* perform flow control procedures if necessary */
    port_flow_control_peer(p_port, FALSE, 0);

    /* If user indicated flow control can not deliver any notifications to him */
    if (p_port->rx.user_fc) {
        if (events & PORT_EV_RXFLAG) {
            p_port->rx_flag_ev_pending = TRUE;
        }

        return;
    }

    events |= PORT_EV_RXCHAR;

    /* Mask out all events that are not of interest to user */
    events &= p_port->ev_mask;

    if (p_port->p_callback && events) {
        p_port->p_callback (events, p_port->inx);
    }
}


/*******************************************************************************
**
** Function         PORT_FlowInd
**
** Description      This function is called from the RFCOMM layer on the flow
**                  control signal change.  Propagate change to the user.
**
*******************************************************************************/
void PORT_FlowInd (tRFC_MCB *p_mcb, UINT8 dlci, BOOLEAN enable_data)
{
    tPORT  *p_port = (tPORT *)NULL;
    UINT32 events = 0;
    int    i;

    RFCOMM_TRACE_EVENT ("PORT_FlowInd fc:%d", enable_data);

    if (dlci == 0) {
        p_mcb->peer_ready = enable_data;
    } else {
        if ((p_port = port_find_mcb_dlci_port (p_mcb, dlci)) == NULL) {
            return;
        }

        p_port->tx.peer_fc = !enable_data;
    }

    for (i = 0; i < MAX_RFC_PORTS; i++) {
        /* If DLCI is 0 event applies to all ports */
        if (dlci == 0) {
            p_port = &rfc_cb.port.port[i];
            if (!p_port->in_use
                    || (p_port->rfc.p_mcb != p_mcb)
                    || (p_port->rfc.state != RFC_STATE_OPENED)) {
                continue;
            }
        }
        events = 0;

        /* Check if flow of data is still enabled */
        events |= port_flow_control_user (p_port);

        /* Check if data can be sent and send it */
        events |= port_rfc_send_tx_data (p_port);

        /* Mask out all events that are not of interest to user */
        events &= p_port->ev_mask;

        /* Send event to the application */
        if (p_port->p_callback && events) {
            (p_port->p_callback)(events, p_port->inx);
        }

        /* If DLCI is not 0 event applies to one port only */
        if (dlci != 0) {
            break;
        }
    }
}


/*******************************************************************************
**
** Function         port_rfc_send_tx_data
**
** Description      This function is when forward data can be sent to the peer
**
*******************************************************************************/
UINT32 port_rfc_send_tx_data (tPORT *p_port)
{
    UINT32 events = 0;
    BT_HDR *p_buf;

    /* if there is data to be sent */
    if (p_port->tx.queue_size > 0) {
        /* while the rfcomm peer is not flow controlling us, and peer is ready */
        while (!p_port->tx.peer_fc && p_port->rfc.p_mcb && p_port->rfc.p_mcb->peer_ready) {
            /* get data from tx queue and send it */
            osi_mutex_global_lock();

            if ((p_buf = (BT_HDR *)fixed_queue_dequeue(p_port->tx.queue, 0)) != NULL) {
                p_port->tx.queue_size -= p_buf->len;

                osi_mutex_global_unlock();

                RFCOMM_TRACE_DEBUG ("Sending RFCOMM_DataReq tx.queue_size=%d", p_port->tx.queue_size);

                RFCOMM_DataReq (p_port->rfc.p_mcb, p_port->dlci, p_buf);

                events |= PORT_EV_TXCHAR;

                if (p_port->tx.queue_size == 0) {
                    events |= PORT_EV_TXEMPTY;
                    break;
                }
            }
            /* queue is empty-- all data sent */
            else {
                osi_mutex_global_unlock();

                events |= PORT_EV_TXEMPTY;
                break;
            }
        }
        /* If we flow controlled user based on the queue size enable data again */
        events |= port_flow_control_user (p_port);
    }
    return (events & p_port->ev_mask);
}


/*******************************************************************************
**
** Function         port_rfc_closed
**
** Description      This function when RFCOMM side of port is closed
**
*******************************************************************************/
void port_rfc_closed (tPORT *p_port, UINT8 res)
{
    UINT8     old_signals;
    UINT32    events = 0;
    tRFC_MCB *p_mcb = p_port->rfc.p_mcb;

    if ((p_port->state == PORT_STATE_OPENING) && (p_port->is_server)) {
        /* The servr side has not been informed that connection is up, ignore */
        RFCOMM_TRACE_EVENT ("port_rfc_closed in OPENING state ignored");

        rfc_port_timer_stop (p_port);
        p_port->rfc.state = RFC_STATE_CLOSED;

        if (p_mcb) {
            p_mcb->port_inx[p_port->dlci] = 0;

            /* If there are no more ports opened on this MCB release it */
            rfc_check_mcb_active (p_mcb);
            p_port->rfc.p_mcb = NULL;
        }

        /* Need to restore DLCI to listening state
         * if the server was on the initiating RFC
         */
        p_port->dlci &= 0xfe;

        return;
    }

    if ((p_port->state != PORT_STATE_CLOSING) && (p_port->state != PORT_STATE_CLOSED)) {
        p_port->line_status |= LINE_STATUS_FAILED;

        old_signals = p_port->peer_ctrl.modem_signal;

        p_port->peer_ctrl.modem_signal &= ~(PORT_DTRDSR_ON | PORT_CTSRTS_ON | PORT_DCD_ON);

        events |= port_get_signal_changes (p_port, old_signals, p_port->peer_ctrl.modem_signal);

        if (p_port->ev_mask & PORT_EV_CONNECT_ERR) {
            events |= PORT_EV_CONNECT_ERR;
        }
    }
    RFCOMM_TRACE_EVENT ("port_rfc_closed state:%d sending events:%x", p_port->state, events);

    if ((p_port->p_callback != NULL) && events) {
        p_port->p_callback (events, p_port->inx);
    }

    if (p_port->p_mgmt_callback) {
        p_port->p_mgmt_callback (res, p_port->inx);
    }

    p_port->rfc.state = RFC_STATE_CLOSED;

    RFCOMM_TRACE_WARNING ("%s RFCOMM connection in state %d closed: %s (res: %d)",
                          __func__, p_port->state, PORT_GetResultString(res), res);

    port_release_port (p_port);
}


/*******************************************************************************
**
** Function         port_get_credits
**
** Description      Set initial values for credits.
**                  Adjust max number of rx credits based on negotiated MTU.
**                  Check max allowed num of bytes, max allowed num buffers,
**                  should be less then 255
**
*******************************************************************************/
void port_get_credits (tPORT *p_port, UINT8 k)
{
    p_port->credit_tx = k;
    if (p_port->credit_tx == 0) {
        p_port->tx.peer_fc = TRUE;
    }
}


#endif ///(defined RFCOMM_INCLUDED && RFCOMM_INCLUDED == TRUE)

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

/*****************************************************************************
 *
 * This file contains functions callable by an application
 * running on top of RFCOMM
 *
 *****************************************************************************/

#include <string.h>
#include "bt_target.h"
#include "rfcdefs.h"
#include "port_api.h"
#include "l2c_api.h"
#include "port_int.h"
#include "rfc_int.h"
#include "bt_defs.h"

#if (defined RFCOMM_INCLUDED && RFCOMM_INCLUDED == TRUE)

#if RFC_DYNAMIC_MEMORY == FALSE
tRFC_CB rfc_cb;
#endif

/*******************************************************************************
**
** Function         RFCOMM_StartReq
**
** Description      This function handles Start Request from the upper layer.
**                  If RFCOMM multiplexer channel can not be allocated
**                  send start not accepted confirmation.  Otherwise dispatch
**                  start event to the state machine.
**
*******************************************************************************/
void RFCOMM_StartReq (tRFC_MCB *p_mcb)
{
    rfc_mx_sm_execute (p_mcb, RFC_MX_EVENT_START_REQ, NULL);
}


/*******************************************************************************
**
** Function         RFCOMM_StartRsp
**
** Description      This function handles Start Response from the upper layer.
**                  Save upper layer handle and result of the Start Indication
**                  in the control block and dispatch event to the FSM.
**
*******************************************************************************/
void RFCOMM_StartRsp (tRFC_MCB *p_mcb, UINT16 result)
{
    rfc_mx_sm_execute (p_mcb, RFC_MX_EVENT_START_RSP, &result);
}


/*******************************************************************************
**
** Function         RFCOMM_DlcEstablishReq
**
** Description      This function is called by the user app to establish
**                  connection with the specific dlci on a specific bd device.
**                  It will allocate RFCOMM connection control block if not
**                  allocated before and dispatch open event to the state
**                  machine.
**
*******************************************************************************/
void RFCOMM_DlcEstablishReq (tRFC_MCB *p_mcb, UINT8 dlci, UINT16 mtu)
{
    UNUSED(mtu);
    if (p_mcb->state != RFC_MX_STATE_CONNECTED) {
        PORT_DlcEstablishCnf (p_mcb, dlci, 0, RFCOMM_ERROR);
        return;
    }

    tPORT *p_port = port_find_mcb_dlci_port(p_mcb, dlci);
    if (p_port == NULL) {
        RFCOMM_TRACE_WARNING("%s Unable to find DLCI port dlci:%d", __func__,
                             dlci);
        return;
    }

    rfc_port_sm_execute(p_port, RFC_EVENT_OPEN, NULL);
}


/*******************************************************************************
**
** Function         RFCOMM_DlcEstablishRsp
**
** Description      This function is called by the port emulation entity
**                  acks Establish Indication.
**
*******************************************************************************/
void RFCOMM_DlcEstablishRsp (tRFC_MCB *p_mcb, UINT8 dlci, UINT16 mtu, UINT16 result)
{
    UNUSED(mtu);
    if ((p_mcb->state != RFC_MX_STATE_CONNECTED) && (result == RFCOMM_SUCCESS)) {
        PORT_DlcReleaseInd (p_mcb, dlci);
        return;
    }

    tPORT *p_port = port_find_mcb_dlci_port (p_mcb, dlci);
    if (p_port == NULL) {
        RFCOMM_TRACE_WARNING("%s Unable to find DLCI port dlci:%d", __func__,
                             dlci);
        return;
    }
    rfc_port_sm_execute(p_port, RFC_EVENT_ESTABLISH_RSP, &result);
}


/*******************************************************************************
**
** Function         RFCOMM_ParNegReq
**
** Description      This function is called by the user app to start
**                  DLC parameter negotiation.  Port emulation can send this
**                  request before actually establishing the DLC.  In this
**                  case the function will allocate RFCOMM connection control
**                  block.
**
*******************************************************************************/
void RFCOMM_ParNegReq (tRFC_MCB *p_mcb, UINT8 dlci, UINT16 mtu)
{
    UINT8 flow;
    UINT8 cl;
    UINT8 k;

    tPORT *p_port = port_find_mcb_dlci_port(p_mcb, dlci);
    if (p_port == NULL) {
        RFCOMM_TRACE_WARNING("%s Unable to find DLCI port dlci:%d", __func__,
                             dlci);
        return;
    }

    if (p_mcb->state != RFC_MX_STATE_CONNECTED) {
        p_port->error = PORT_PAR_NEG_FAILED;
        return;
    }

    /* Negotiate the flow control mechanism.  If flow control mechanism for */
    /* mux has not been set yet, use our default value.  If it has been set, */
    /* use that value. */
    flow = (p_mcb->flow == PORT_FC_UNDEFINED) ? PORT_FC_DEFAULT : p_mcb->flow;

    /* Set convergence layer and number of credits (k) */
    if (flow == PORT_FC_CREDIT) {
        cl = RFCOMM_PN_CONV_LAYER_CBFC_I;
        k = (p_port->credit_rx_max < RFCOMM_K_MAX) ? p_port->credit_rx_max : RFCOMM_K_MAX;
        p_port->credit_rx = k;
    } else {
        cl = RFCOMM_PN_CONV_LAYER_TYPE_1;
        k = 0;
    }

    /* Send Parameter Negotiation Command UIH frame */
    p_port->rfc.expected_rsp |= RFC_RSP_PN;

    rfc_send_pn (p_mcb, dlci, TRUE, mtu, cl, k);

    rfc_port_timer_start (p_port, RFC_T2_TIMEOUT) ;
}


/*******************************************************************************
**
** Function         RFCOMM_ParNegRsp
**
** Description      This function is called by the user app to acknowledge
**                  DLC parameter negotiation.
**
*******************************************************************************/
void RFCOMM_ParNegRsp (tRFC_MCB *p_mcb, UINT8 dlci, UINT16 mtu, UINT8 cl, UINT8 k)
{
    if (p_mcb->state != RFC_MX_STATE_CONNECTED) {
        return;
    }

    /* Send Parameter Negotiation Response UIH frame */
    rfc_send_pn (p_mcb, dlci, FALSE, mtu, cl, k);
}


/*******************************************************************************
**
** Function         RFCOMM_PortNegReq
**
** Description      This function is called by the user app to start
**                  Remote Port parameter negotiation.  Port emulation can
**                  send this request before actually establishing the DLC.
**                  In this case the function will allocate RFCOMM connection
**                  control block.
**
*******************************************************************************/
void RFCOMM_PortNegReq (tRFC_MCB *p_mcb, UINT8 dlci, tPORT_STATE *p_pars)
{
    if (p_mcb->state != RFC_MX_STATE_CONNECTED) {
        PORT_PortNegCnf (p_mcb, dlci, NULL, RFCOMM_ERROR);
        return;
    }

    tPORT *p_port = port_find_mcb_dlci_port(p_mcb, dlci);
    if (p_port == NULL) {
        RFCOMM_TRACE_WARNING("%s Unable to find DLCI port dlci:%d", __func__,
                             dlci);
        return;
    }

    /* Send Parameter Negotiation Command UIH frame */
    if (!p_pars) {
        p_port->rfc.expected_rsp |= RFC_RSP_RPN_REPLY;
    } else {
        p_port->rfc.expected_rsp |= RFC_RSP_RPN;
    }

    rfc_send_rpn (p_mcb, dlci, TRUE, p_pars, RFCOMM_RPN_PM_MASK);
    rfc_port_timer_start (p_port, RFC_T2_TIMEOUT) ;

}


/*******************************************************************************
**
** Function         RFCOMM_PortNegRsp
**
** Description      This function is called by the user app to acknowledge
**                  Port parameters negotiation.
**
*******************************************************************************/
void RFCOMM_PortNegRsp (tRFC_MCB *p_mcb, UINT8 dlci, tPORT_STATE *p_pars,
                        UINT16 param_mask)
{
    if (p_mcb->state != RFC_MX_STATE_CONNECTED) {
        return;
    }

    rfc_send_rpn (p_mcb, dlci, FALSE, p_pars, param_mask);
}


/*******************************************************************************
**
** Function         RFCOMM_ControlReq
**
** Description      This function is called by the port entity to send control
**                  parameters to remote port emulation entity.
**
*******************************************************************************/
void RFCOMM_ControlReq (tRFC_MCB *p_mcb, UINT8 dlci, tPORT_CTRL *p_pars)
{
    tPORT *p_port = port_find_mcb_dlci_port(p_mcb, dlci);
    if (p_port == NULL) {
        RFCOMM_TRACE_WARNING("%s Unable to find DLCI port dlci:%d", __func__,
                             dlci);
        return;
    }

    if ((p_port->state != PORT_STATE_OPENED)
            || (p_port->rfc.state  != RFC_STATE_OPENED)) {
        return;
    }

    p_port->port_ctrl |= PORT_CTRL_REQ_SENT;

    p_port->rfc.expected_rsp |= RFC_RSP_MSC;

    rfc_send_msc (p_mcb, dlci, TRUE, p_pars);
    rfc_port_timer_start (p_port, RFC_T2_TIMEOUT) ;

}


/*******************************************************************************
**
** Function         RFCOMM_FlowReq
**
** Description      This function is called by the port entity when flow
**                  control state has changed.  Enable flag passed shows if
**                  port can accept more data.
**
*******************************************************************************/
void RFCOMM_FlowReq (tRFC_MCB *p_mcb, UINT8 dlci, UINT8 enable)
{
    tPORT *p_port = port_find_mcb_dlci_port(p_mcb, dlci);
    if (p_port == NULL) {
        RFCOMM_TRACE_WARNING("%s Unable to find DLCI port dlci:%d", __func__,
                             dlci);
        return;
    }

    if ((p_port->state != PORT_STATE_OPENED)
            || (p_port->rfc.state  != RFC_STATE_OPENED)) {
        return;
    }

    p_port->local_ctrl.fc = !enable;

    p_port->rfc.expected_rsp |= RFC_RSP_MSC;

    rfc_send_msc (p_mcb, dlci, TRUE, &p_port->local_ctrl);
    rfc_port_timer_start (p_port, RFC_T2_TIMEOUT) ;

}


/*******************************************************************************
**
** Function         RFCOMM_LineStatusReq
**
** Description      This function is called by the port entity when line
**                  status should be delivered to the peer.
**
*******************************************************************************/
void RFCOMM_LineStatusReq (tRFC_MCB *p_mcb, UINT8 dlci, UINT8 status)
{
    tPORT *p_port = port_find_mcb_dlci_port(p_mcb, dlci);
    if (p_port == NULL) {
        RFCOMM_TRACE_WARNING("%s Unable to find DLCI port dlci:%d", __func__,
                             dlci);
        return;
    }

    if ((p_port->state != PORT_STATE_OPENED)
            || (p_port->rfc.state  != RFC_STATE_OPENED)) {
        return;
    }

    p_port->rfc.expected_rsp |= RFC_RSP_RLS;

    rfc_send_rls (p_mcb, dlci, TRUE, status);
    rfc_port_timer_start (p_port, RFC_T2_TIMEOUT);
}


/*******************************************************************************
**
** Function         RFCOMM_DlcReleaseReq
**
** Description      This function is called by the PORT unit to close DLC
**
*******************************************************************************/
void RFCOMM_DlcReleaseReq (tRFC_MCB *p_mcb, UINT8 dlci)
{
    rfc_port_sm_execute(port_find_mcb_dlci_port (p_mcb, dlci), RFC_EVENT_CLOSE, 0);
}


/*******************************************************************************
**
** Function         RFCOMM_DataReq
**
** Description      This function is called by the user app to send data buffer
**
*******************************************************************************/
void RFCOMM_DataReq (tRFC_MCB *p_mcb, UINT8 dlci, BT_HDR *p_buf)
{
    rfc_port_sm_execute(port_find_mcb_dlci_port (p_mcb, dlci), RFC_EVENT_DATA, p_buf);
}


#endif ///(defined RFCOMM_INCLUDED && RFCOMM_INCLUDED == TRUE)
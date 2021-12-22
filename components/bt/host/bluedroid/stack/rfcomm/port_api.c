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
 *  this file contains the Serial Port API code
 *
 ******************************************************************************/

#include <string.h>
#include "common/bt_target.h"
#include "stack/rfcdefs.h"
#include "stack/port_api.h"
#include "port_int.h"
#include "btm_int.h"
#include "stack/btm_api.h"
#include "rfc_int.h"
#include "stack/l2c_api.h"
#include "stack/sdp_api.h"
#include "osi/allocator.h"
#include "osi/mutex.h"

#if (defined RFCOMM_INCLUDED && RFCOMM_INCLUDED == TRUE)
/* duration of break in 200ms units */
#define PORT_BREAK_DURATION     1

/* Mapping from PORT_* result codes to human readable strings. */
static const char *result_code_strings[] = {
    "Success",
    "Unknown error",
    "Already opened",
    "Command pending",
    "App not registered",
    "No memory",
    "No resources",
    "Bad BD address",
    "Unspecified error",
    "Bad handle",
    "Not opened",
    "Line error",
    "Start failed",
    "Parameter negotiation failed",
    "Port negotiation failed",
    "Sec failed",
    "Peer connection failed",
    "Peer failed",
    "Peer timeout",
    "Closed",
    "TX full",
    "Local closed",
    "Local timeout",
    "TX queue disabled",
    "Page timeout",
    "Invalid SCN",
    "Unknown result code"
};

/*******************************************************************************
**
** Function         RFCOMM_CreateConnection
**
** Description      RFCOMM_CreateConnection function is used from the application
**                  to establish serial port connection to the peer device,
**                  or allow RFCOMM to accept a connection from the peer
**                  application.
**
** Parameters:      scn          - Service Channel Number as registered with
**                                 the SDP (server) or obtained using SDP from
**                                 the peer device (client).
**                  is_server    - TRUE if requesting application is a server
**                  mtu          - Maximum frame size the application can accept
**                  bd_addr      - BD_ADDR of the peer (client)
**                  mask         - specifies events to be enabled.  A value
**                                 of zero disables all events.
**                  p_handle     - OUT pointer to the handle.
**                  p_mgmt_cb    - pointer to callback function to receive
**                                 connection up/down events.
** Notes:
**
** Server can call this function with the same scn parameter multiple times if
** it is ready to accept multiple simulteneous connections.
**
** DLCI for the connection is (scn * 2 + 1) if client originates connection on
** existing none initiator multiplexer channel.  Otherwise it is (scn * 2).
** For the server DLCI can be changed later if client will be calling it using
** (scn * 2 + 1) dlci.
**
*******************************************************************************/
int RFCOMM_CreateConnection (UINT16 uuid, UINT8 scn, BOOLEAN is_server,
                             UINT16 mtu, BD_ADDR bd_addr, UINT16 *p_handle,
                             tPORT_MGMT_CALLBACK *p_mgmt_cb)
{
    tPORT      *p_port;
    int        i;
    UINT8      dlci;
    tRFC_MCB   *p_mcb = port_find_mcb (bd_addr);
    UINT16     rfcomm_mtu;

    RFCOMM_TRACE_API ("RFCOMM_CreateConnection()  BDA: %02x-%02x-%02x-%02x-%02x-%02x",
                      bd_addr[0], bd_addr[1], bd_addr[2], bd_addr[3], bd_addr[4], bd_addr[5]);

    *p_handle = 0;

    if (( scn == 0 ) || (scn >= PORT_MAX_RFC_PORTS )) {
        /* Server Channel Number(SCN) should be in range 1...30 */
        RFCOMM_TRACE_ERROR ("RFCOMM_CreateConnection - invalid SCN");
        return (PORT_INVALID_SCN);
    }

    /* For client that originate connection on the existing none initiator */
    /* multiplexer channel DLCI should be odd */
    if (p_mcb && !p_mcb->is_initiator && !is_server) {
        dlci = (scn << 1) + 1;
    } else {
        dlci = (scn << 1);
    }
    RFCOMM_TRACE_API("RFCOMM_CreateConnection(): scn:%d, dlci:%d, is_server:%d mtu:%d, p_mcb:%p",
                     scn, dlci, is_server, mtu, p_mcb);

    /* For the server side always allocate a new port.  On the client side */
    /* do not allow the same (dlci, bd_addr) to be opened twice by application */
    if (!is_server && ((p_port = port_find_port (dlci, bd_addr)) != NULL)) {
        /* if existing port is also a client port */
        if (p_port->is_server == FALSE) {
            RFCOMM_TRACE_ERROR ("RFCOMM_CreateConnection - already opened state:%d, RFC state:%d, MCB state:%d",
                                p_port->state, p_port->rfc.state, p_port->rfc.p_mcb ? p_port->rfc.p_mcb->state : 0);
            return (PORT_ALREADY_OPENED);
        }
    }

    if ((p_port = port_allocate_port (dlci, bd_addr)) == NULL) {
        RFCOMM_TRACE_WARNING ("RFCOMM_CreateConnection - no resources");
        return (PORT_NO_RESOURCES);
    }
    RFCOMM_TRACE_API("RFCOMM_CreateConnection(): scn:%d, dlci:%d, is_server:%d mtu:%d, p_mcb:%p, p_port:%p",
                     scn, dlci, is_server, mtu, p_mcb, p_port);

    p_port->default_signal_state = (PORT_DTRDSR_ON | PORT_CTSRTS_ON | PORT_DCD_ON);

    switch (uuid) {
    case UUID_PROTOCOL_OBEX:
        p_port->default_signal_state = PORT_OBEX_DEFAULT_SIGNAL_STATE;
        break;
    case UUID_SERVCLASS_SERIAL_PORT:
        p_port->default_signal_state = PORT_SPP_DEFAULT_SIGNAL_STATE;
        break;
    case UUID_SERVCLASS_LAN_ACCESS_USING_PPP:
        p_port->default_signal_state = PORT_PPP_DEFAULT_SIGNAL_STATE;
        break;
    case UUID_SERVCLASS_DIALUP_NETWORKING:
    case UUID_SERVCLASS_FAX:
        p_port->default_signal_state = PORT_DUN_DEFAULT_SIGNAL_STATE;
        break;
    }

    RFCOMM_TRACE_EVENT ("RFCOMM_CreateConnection dlci:%d signal state:0x%x", dlci, p_port->default_signal_state);

    *p_handle = p_port->inx;

    p_port->state        = PORT_STATE_OPENING;
    p_port->uuid         = uuid;
    p_port->is_server    = is_server;
    p_port->scn          = scn;
    p_port->ev_mask      = 0;

    /* If the MTU is not specified (0), keep MTU decision until the
     * PN frame has to be send
     * at that time connection should be established and we
     * will know for sure our prefered MTU
     */

    rfcomm_mtu = L2CAP_MTU_SIZE - RFCOMM_DATA_OVERHEAD;

    if (mtu) {
        p_port->mtu      = (mtu < rfcomm_mtu) ? mtu : rfcomm_mtu;
    } else {
        p_port->mtu      = rfcomm_mtu;
    }

    /* server doesn't need to release port when closing */
    if ( is_server ) {
        p_port->keep_port_handle = TRUE;

        /* keep mtu that user asked, p_port->mtu could be updated during param negotiation */
        p_port->keep_mtu         = p_port->mtu;
    }

    p_port->local_ctrl.modem_signal = p_port->default_signal_state;
    p_port->local_ctrl.fc           = FALSE;

    p_port->p_mgmt_callback = p_mgmt_cb;

    for (i = 0; i < BD_ADDR_LEN; i++) {
        p_port->bd_addr[i] = bd_addr[i];
    }

    /* If this is not initiator of the connection need to just wait */
    if (p_port->is_server) {
        return (PORT_SUCCESS);
    }

    /* Open will be continued after security checks are passed */
    return port_open_continue (p_port);
}


/*******************************************************************************
**
** Function         RFCOMM_RemoveConnection
**
** Description      This function is called to close the specified connection.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**
*******************************************************************************/
int RFCOMM_RemoveConnection (UINT16 handle)
{
    tPORT      *p_port;

    RFCOMM_TRACE_API ("RFCOMM_RemoveConnection() handle:%d", handle);

    /* Check if handle is valid to avoid crashing */
    if ((handle == 0) || (handle > MAX_RFC_PORTS)) {
        RFCOMM_TRACE_ERROR ("RFCOMM_RemoveConnection() BAD handle:%d", handle);
        return (PORT_BAD_HANDLE);
    }
    p_port = &rfc_cb.port.port[handle - 1];

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        RFCOMM_TRACE_EVENT ("RFCOMM_RemoveConnection() Not opened:%d", handle);
        return (PORT_SUCCESS);
    }

    p_port->state = PORT_STATE_CLOSING;

    port_start_close (p_port);

    return (PORT_SUCCESS);
}

/*******************************************************************************
**
** Function         RFCOMM_RemoveServer
**
** Description      This function is called to close the server port.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**
*******************************************************************************/
int RFCOMM_RemoveServer (UINT16 handle)
{
    tPORT      *p_port;

    RFCOMM_TRACE_API ("RFCOMM_RemoveServer() handle:%d", handle);

    /* Check if handle is valid to avoid crashing */
    if ((handle == 0) || (handle > MAX_RFC_PORTS)) {
        RFCOMM_TRACE_ERROR ("RFCOMM_RemoveServer() BAD handle:%d", handle);
        return (PORT_BAD_HANDLE);
    }
    p_port = &rfc_cb.port.port[handle - 1];

    /* Do not report any events to the client any more. */
    p_port->p_mgmt_callback = NULL;

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        RFCOMM_TRACE_EVENT ("RFCOMM_RemoveServer() Not opened:%d", handle);
        return (PORT_SUCCESS);
    }

    /* this port will be deallocated after closing */
    p_port->keep_port_handle = FALSE;
    p_port->state = PORT_STATE_CLOSING;

    port_start_close (p_port);

    return (PORT_SUCCESS);
}

/*******************************************************************************
**
** Function         PORT_SetEventCallback
**
** Description      This function is called to provide an address of the
**                  function which will be called when one of the events
**                  specified in the mask occures.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  p_callback - address of the callback function which should
**                               be called from the RFCOMM when an event
**                               specified in the mask occures.
**
**
*******************************************************************************/
int PORT_SetEventCallback (UINT16 port_handle, tPORT_CALLBACK *p_port_cb)
{
    tPORT  *p_port;

    /* Check if handle is valid to avoid crashing */
    if ((port_handle == 0) || (port_handle > MAX_RFC_PORTS)) {
        return (PORT_BAD_HANDLE);
    }

    p_port = &rfc_cb.port.port[port_handle - 1];

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        return (PORT_NOT_OPENED);
    }

    RFCOMM_TRACE_API ("PORT_SetEventCallback() handle:%d", port_handle);

    p_port->p_callback = p_port_cb;

    return (PORT_SUCCESS);
}
/*******************************************************************************
**
** Function         PORT_ClearKeepHandleFlag
**
** Description      This function is called to clear the keep handle flag
**                  which will cause not to keep the port handle open when closed
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**
*******************************************************************************/

int PORT_ClearKeepHandleFlag (UINT16 port_handle)
{
    tPORT  *p_port;

    /* Check if handle is valid to avoid crashing */
    if ((port_handle == 0) || (port_handle > MAX_RFC_PORTS)) {
        return (PORT_BAD_HANDLE);
    }

    p_port = &rfc_cb.port.port[port_handle - 1];
    p_port->keep_port_handle = 0;
    return (PORT_SUCCESS);
}

/*******************************************************************************
**
** Function         PORT_SetDataCallback
**
** Description      This function is when a data packet is received
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  p_callback - address of the callback function which should
**                               be called from the RFCOMM when data packet
**                               is received.
**
**
*******************************************************************************/
int PORT_SetDataCallback (UINT16 port_handle, tPORT_DATA_CALLBACK *p_port_cb)
{
    tPORT  *p_port;

    // RFCOMM_TRACE_API ("PORT_SetDataCallback() handle:%d cb 0x%x", port_handle, p_port_cb);

    /* Check if handle is valid to avoid crashing */
    if ((port_handle == 0) || (port_handle > MAX_RFC_PORTS)) {
        return (PORT_BAD_HANDLE);
    }

    p_port = &rfc_cb.port.port[port_handle - 1];

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        return (PORT_NOT_OPENED);
    }

    p_port->p_data_callback = p_port_cb;

    return (PORT_SUCCESS);
}
/*******************************************************************************
**
** Function         PORT_SetCODataCallback
**
** Description      This function is when a data packet is received
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  p_callback - address of the callback function which should
**                               be called from the RFCOMM when data packet
**                               is received.
**
**
*******************************************************************************/
int PORT_SetDataCOCallback (UINT16 port_handle, tPORT_DATA_CO_CALLBACK *p_port_cb)
{
    tPORT  *p_port;

    // RFCOMM_TRACE_API ("PORT_SetDataCOCallback() handle:%d cb 0x%x", port_handle, p_port_cb);

    /* Check if handle is valid to avoid crashing */
    if ((port_handle == 0) || (port_handle > MAX_RFC_PORTS)) {
        return (PORT_BAD_HANDLE);
    }

    p_port = &rfc_cb.port.port[port_handle - 1];

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        return (PORT_NOT_OPENED);
    }

    p_port->p_data_co_callback = p_port_cb;

    return (PORT_SUCCESS);
}



/*******************************************************************************
**
** Function         PORT_SetEventMask
**
** Description      This function is called to close the specified connection.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  mask   - Bitmask of the events the host is interested in
**
*******************************************************************************/
int PORT_SetEventMask (UINT16 port_handle, UINT32 mask)
{
    tPORT  *p_port;

    RFCOMM_TRACE_API ("PORT_SetEventMask() handle:%d mask:0x%x", port_handle, mask);

    /* Check if handle is valid to avoid crashing */
    if ((port_handle == 0) || (port_handle > MAX_RFC_PORTS)) {
        return (PORT_BAD_HANDLE);
    }

    p_port = &rfc_cb.port.port[port_handle - 1];

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        return (PORT_NOT_OPENED);
    }

    p_port->ev_mask = mask;

    return (PORT_SUCCESS);
}


/*******************************************************************************
**
** Function         PORT_CheckConnection
**
** Description      This function returns PORT_SUCCESS if connection referenced
**                  by handle is up and running
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  ignore_rfc_state - If need to ignore rfc state
**                  bd_addr    - OUT bd_addr of the peer
**                  p_lcid     - OUT L2CAP's LCID
**
*******************************************************************************/
int PORT_CheckConnection(UINT16 handle, BOOLEAN ignore_rfc_state, BD_ADDR bd_addr, UINT16 *p_lcid)
{
    tPORT      *p_port;

    RFCOMM_TRACE_API ("PORT_CheckConnection() handle:%d", handle);

    /* Check if handle is valid to avoid crashing */
    if ((handle == 0) || (handle > MAX_RFC_PORTS)) {
        return (PORT_BAD_HANDLE);
    }

    p_port = &rfc_cb.port.port[handle - 1];

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        return (PORT_NOT_OPENED);
    }

    if (!p_port->rfc.p_mcb || !p_port->rfc.p_mcb->peer_ready ||
        (!ignore_rfc_state ? (p_port->rfc.state != RFC_STATE_OPENED) : false)) {
        return (PORT_LINE_ERR);
    }

    memcpy (bd_addr, p_port->rfc.p_mcb->bd_addr, BD_ADDR_LEN);
    if (p_lcid) {
        *p_lcid = p_port->rfc.p_mcb->lcid;
    }

    return (PORT_SUCCESS);
}

/*******************************************************************************
**
** Function         PORT_IsOpening
**
** Description      This function returns TRUE if there is any RFCOMM connection
**                  opening in process.
**
** Parameters:      TRUE if any connection opening is found
**                  bd_addr    - bd_addr of the peer
**
*******************************************************************************/
BOOLEAN PORT_IsOpening (BD_ADDR bd_addr)
{
    UINT8   xx, yy;
    tRFC_MCB *p_mcb = NULL;
    tPORT  *p_port;
    BOOLEAN found_port;

    /* Check for any rfc_mcb which is in the middle of opening. */
    for (xx = 0; xx < MAX_BD_CONNECTIONS; xx++) {
        if ((rfc_cb.port.rfc_mcb[xx].state > RFC_MX_STATE_IDLE) &&
                (rfc_cb.port.rfc_mcb[xx].state < RFC_MX_STATE_CONNECTED)) {
            memcpy (bd_addr, rfc_cb.port.rfc_mcb[xx].bd_addr, BD_ADDR_LEN);
            return TRUE;
        }

        if (rfc_cb.port.rfc_mcb[xx].state == RFC_MX_STATE_CONNECTED) {
            found_port = FALSE;
            p_mcb = &rfc_cb.port.rfc_mcb[xx];
            p_port = &rfc_cb.port.port[0];

            for (yy = 0; yy < MAX_RFC_PORTS; yy++, p_port++) {
                if (p_port->rfc.p_mcb == p_mcb) {
                    found_port = TRUE;
                    break;
                }
            }

            if ((!found_port) ||
                    (found_port && (p_port->rfc.state < RFC_STATE_OPENED))) {
                /* Port is not established yet. */
                memcpy (bd_addr, rfc_cb.port.rfc_mcb[xx].bd_addr, BD_ADDR_LEN);
                return TRUE;
            }
        }
    }

    return FALSE;
}

/*******************************************************************************
**
** Function         PORT_SetState
**
** Description      This function configures connection according to the
**                  specifications in the tPORT_STATE structure.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  p_settings - Pointer to a tPORT_STATE structure containing
**                               configuration information for the connection.
**
**
*******************************************************************************/
int PORT_SetState (UINT16 handle, tPORT_STATE *p_settings)
{
    tPORT      *p_port;
    UINT8       baud_rate;

    RFCOMM_TRACE_API ("PORT_SetState() handle:%d", handle);

    /* Check if handle is valid to avoid crashing */
    if ((handle == 0) || (handle > MAX_RFC_PORTS)) {
        return (PORT_BAD_HANDLE);
    }

    p_port = &rfc_cb.port.port[handle - 1];

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        return (PORT_NOT_OPENED);
    }

    if (p_port->line_status) {
        return (PORT_LINE_ERR);
    }

    RFCOMM_TRACE_API ("PORT_SetState() handle:%d FC_TYPE:0x%x", handle,
                      p_settings->fc_type);

    baud_rate = p_port->user_port_pars.baud_rate;
    p_port->user_port_pars = *p_settings;

    /* for now we've been asked to pass only baud rate */
    if (baud_rate != p_settings->baud_rate) {
        port_start_par_neg (p_port);
    }
    return (PORT_SUCCESS);
}

/*******************************************************************************
**
** Function         PORT_GetRxQueueCnt
**
** Description      This function return number of buffers on the rx queue.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  p_rx_queue_count - Pointer to return queue count in.
**
*******************************************************************************/
int PORT_GetRxQueueCnt (UINT16 handle, UINT16 *p_rx_queue_count)
{
    tPORT      *p_port;

    RFCOMM_TRACE_API ("PORT_GetRxQueueCnt() handle:%d", handle);

    /* Check if handle is valid to avoid crashing */
    if ((handle == 0) || (handle > MAX_RFC_PORTS)) {
        return (PORT_BAD_HANDLE);
    }

    p_port = &rfc_cb.port.port[handle - 1];

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        return (PORT_NOT_OPENED);
    }

    if (p_port->line_status) {
        return (PORT_LINE_ERR);
    }

    *p_rx_queue_count = p_port->rx.queue_size;

    RFCOMM_TRACE_API ("PORT_GetRxQueueCnt() p_rx_queue_count:%d, p_port->rx.queue.count = %d",
                      *p_rx_queue_count, p_port->rx.queue_size);

    return (PORT_SUCCESS);
}

/*******************************************************************************
**
** Function         PORT_GetState
**
** Description      This function is called to fill tPORT_STATE structure
**                  with the curremt control settings for the port
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  p_settings - Pointer to a tPORT_STATE structure in which
**                               configuration information is returned.
**
*******************************************************************************/
int PORT_GetState (UINT16 handle, tPORT_STATE *p_settings)
{
    tPORT      *p_port;

    RFCOMM_TRACE_API ("PORT_GetState() handle:%d", handle);

    /* Check if handle is valid to avoid crashing */
    if ((handle == 0) || (handle > MAX_RFC_PORTS)) {
        return (PORT_BAD_HANDLE);
    }

    p_port = &rfc_cb.port.port[handle - 1];

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        return (PORT_NOT_OPENED);
    }

    if (p_port->line_status) {
        return (PORT_LINE_ERR);
    }

    *p_settings = p_port->user_port_pars;
    return (PORT_SUCCESS);
}


/*******************************************************************************
**
** Function         PORT_Control
**
** Description      This function directs a specified connection to pass control
**                  control information to the peer device.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  signal     = specify the function to be passed
**
*******************************************************************************/
int PORT_Control (UINT16 handle, UINT8 signal)
{
    tPORT      *p_port;
    UINT8      old_modem_signal;

    RFCOMM_TRACE_API ("PORT_Control() handle:%d signal:0x%x", handle, signal);

    /* Check if handle is valid to avoid crashing */
    if ((handle == 0) || (handle > MAX_RFC_PORTS)) {
        return (PORT_BAD_HANDLE);
    }

    p_port = &rfc_cb.port.port[handle - 1];

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        return (PORT_NOT_OPENED);
    }

    old_modem_signal = p_port->local_ctrl.modem_signal;
    p_port->local_ctrl.break_signal = 0;

    switch (signal) {
    case PORT_SET_CTSRTS:
        p_port->local_ctrl.modem_signal |= PORT_CTSRTS_ON;
        break;

    case PORT_CLR_CTSRTS:
        p_port->local_ctrl.modem_signal &= ~PORT_CTSRTS_ON;
        break;

    case PORT_SET_DTRDSR:
        p_port->local_ctrl.modem_signal |= PORT_DTRDSR_ON;
        break;

    case PORT_CLR_DTRDSR:
        p_port->local_ctrl.modem_signal &= ~PORT_DTRDSR_ON;
        break;

    case PORT_SET_RI:
        p_port->local_ctrl.modem_signal |= PORT_RING_ON;
        break;

    case PORT_CLR_RI:
        p_port->local_ctrl.modem_signal &= ~PORT_RING_ON;
        break;

    case PORT_SET_DCD:
        p_port->local_ctrl.modem_signal |= PORT_DCD_ON;
        break;

    case PORT_CLR_DCD:
        p_port->local_ctrl.modem_signal &= ~PORT_DCD_ON;
        break;
    }

    if (signal == PORT_BREAK) {
        p_port->local_ctrl.break_signal = PORT_BREAK_DURATION;
    } else if (p_port->local_ctrl.modem_signal == old_modem_signal) {
        return (PORT_SUCCESS);
    }

    port_start_control (p_port);

    RFCOMM_TRACE_EVENT ("PORT_Control DTR_DSR : %d, RTS_CTS : %d, RI : %d, DCD : %d",
                        ((p_port->local_ctrl.modem_signal & MODEM_SIGNAL_DTRDSR) ? 1 : 0),
                        ((p_port->local_ctrl.modem_signal & MODEM_SIGNAL_RTSCTS) ? 1 : 0),
                        ((p_port->local_ctrl.modem_signal & MODEM_SIGNAL_RI) ? 1 : 0),
                        ((p_port->local_ctrl.modem_signal & MODEM_SIGNAL_DCD) ? 1 : 0));

    return (PORT_SUCCESS);
}


/*******************************************************************************
**
** Function         PORT_FlowControl
**
** Description      This function directs a specified connection to pass
**                  flow control message to the peer device.  Enable flag passed
**                  shows if port can accept more data.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  enable     - enables data flow
**
*******************************************************************************/
int PORT_FlowControl (UINT16 handle, BOOLEAN enable)
{
    tPORT      *p_port;
    BOOLEAN    old_fc;
    UINT32     events;

    RFCOMM_TRACE_API ("PORT_FlowControl() handle:%d enable: %d", handle, enable);

    /* Check if handle is valid to avoid crashing */
    if ((handle == 0) || (handle > MAX_RFC_PORTS)) {
        return (PORT_BAD_HANDLE);
    }

    p_port = &rfc_cb.port.port[handle - 1];

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        return (PORT_NOT_OPENED);
    }

    if (!p_port->rfc.p_mcb) {
        return (PORT_NOT_OPENED);
    }

    p_port->rx.user_fc = !enable;

    if (p_port->rfc.p_mcb->flow == PORT_FC_CREDIT) {
        if (!p_port->rx.user_fc) {
            port_flow_control_peer(p_port, TRUE, 0);
        }
    } else {
        old_fc = p_port->local_ctrl.fc;

        /* FC is set if user is set or peer is set */
        p_port->local_ctrl.fc = (p_port->rx.user_fc | p_port->rx.peer_fc);

        if (p_port->local_ctrl.fc != old_fc) {
            port_start_control (p_port);
        }
    }

    /* Need to take care of the case when we could not deliver events */
    /* to the application because we were flow controlled */
    if (enable && (p_port->rx.queue_size != 0)) {
        events = PORT_EV_RXCHAR;
        if (p_port->rx_flag_ev_pending) {
            p_port->rx_flag_ev_pending = FALSE;
            events |= PORT_EV_RXFLAG;
        }

        events &= p_port->ev_mask;
        if (p_port->p_callback && events) {
            p_port->p_callback (events, p_port->inx);
        }
    }
    return (PORT_SUCCESS);
}

#if 0 //Unused
/*******************************************************************************
**
** Function         PORT_FlowControl_MaxCredit
**
** Description      This function directs a specified connection to pass
**                  flow control message to the peer device.  Enable flag passed
**                  shows if port can accept more data. It also sends max credit
**                  when data flow enabled
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  enable     - enables data flow
**
*******************************************************************************/
int PORT_FlowControl_MaxCredit (UINT16 handle, BOOLEAN enable)
{
    tPORT      *p_port;
    BOOLEAN    old_fc;
    UINT32     events;

    RFCOMM_TRACE_API ("PORT_FlowControl() handle:%d enable: %d", handle, enable);

    /* Check if handle is valid to avoid crashing */
    if ((handle == 0) || (handle > MAX_RFC_PORTS)) {
        return (PORT_BAD_HANDLE);
    }

    p_port = &rfc_cb.port.port[handle - 1];

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        return (PORT_NOT_OPENED);
    }

    if (!p_port->rfc.p_mcb) {
        return (PORT_NOT_OPENED);
    }

    p_port->rx.user_fc = !enable;

    if (p_port->rfc.p_mcb->flow == PORT_FC_CREDIT) {
        if (!p_port->rx.user_fc) {
            port_flow_control_peer(p_port, TRUE, p_port->credit_rx);
        }
    } else {
        old_fc = p_port->local_ctrl.fc;

        /* FC is set if user is set or peer is set */
        p_port->local_ctrl.fc = (p_port->rx.user_fc | p_port->rx.peer_fc);

        if (p_port->local_ctrl.fc != old_fc) {
            port_start_control (p_port);
        }
    }

    /* Need to take care of the case when we could not deliver events */
    /* to the application because we were flow controlled */
    if (enable && (p_port->rx.queue_size != 0)) {
        events = PORT_EV_RXCHAR;
        if (p_port->rx_flag_ev_pending) {
            p_port->rx_flag_ev_pending = FALSE;
            events |= PORT_EV_RXFLAG;
        }

        events &= p_port->ev_mask;
        if (p_port->p_callback && events) {
            p_port->p_callback (events, p_port->inx);
        }
    }
    return (PORT_SUCCESS);
}
#endif

/*******************************************************************************
**
** Function         PORT_FlowControl_GiveCredit
**
** Description      This function gives specified credits to the peer
**
** Parameters:      handle        - Handle returned in the RFCOMM_CreateConnection
**                  enable        - enables data flow
**                  credits_given - credits to give
**
*******************************************************************************/
int PORT_FlowControl_GiveCredit (UINT16 handle, BOOLEAN enable, UINT16 credits_given)
{
    tPORT      *p_port;
    BOOLEAN    old_fc;
    UINT32     events;

    RFCOMM_TRACE_DEBUG("%s handle:%d enable: %d, cred %d", __func__, handle, enable, credits_given);

    /* Check if handle is valid to avoid crashing */
    if ((handle == 0) || (handle > MAX_RFC_PORTS)) {
        return (PORT_BAD_HANDLE);
    }

    p_port = &rfc_cb.port.port[handle - 1];

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        return (PORT_NOT_OPENED);
    }

    if (!p_port->rfc.p_mcb) {
        return (PORT_NOT_OPENED);
    }

    p_port->rx.user_fc = !enable;

    if (p_port->rfc.p_mcb->flow == PORT_FC_CREDIT) {
        if (!p_port->rx.user_fc) {
            port_flow_control_peer(p_port, TRUE, credits_given);
        }
    } else {
        assert(0); // karl: temporarily not allowed
        old_fc = p_port->local_ctrl.fc;

        /* FC is set if user is set or peer is set */
        p_port->local_ctrl.fc = (p_port->rx.user_fc | p_port->rx.peer_fc);

        if (p_port->local_ctrl.fc != old_fc) {
            port_start_control (p_port);
        }
    }

    /* Need to take care of the case when we could not deliver events */
    /* to the application because we were flow controlled */
    if (enable && (p_port->rx.queue_size != 0)) {
        assert(0); // karl: temporarily not allowed
        events = PORT_EV_RXCHAR;
        if (p_port->rx_flag_ev_pending) {
            p_port->rx_flag_ev_pending = FALSE;
            events |= PORT_EV_RXFLAG;
        }

        events &= p_port->ev_mask;
        if (p_port->p_callback && events) {
            p_port->p_callback (events, p_port->inx);
        }
    }
    return (PORT_SUCCESS);
}


/*******************************************************************************
**
** Function         PORT_GetModemStatus
**
** Description      This function retrieves modem control signals.  Normally
**                  application will call this function after a callback
**                  function is called with notification that one of signals
**                  has been changed.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  p_signal   - specify the pointer to control signals info
**
*******************************************************************************/
int PORT_GetModemStatus (UINT16 handle, UINT8 *p_signal)
{
    tPORT      *p_port;

    if ((handle == 0) || (handle > MAX_RFC_PORTS)) {
        return (PORT_BAD_HANDLE);
    }

    p_port = &rfc_cb.port.port[handle - 1];

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        return (PORT_NOT_OPENED);
    }

    *p_signal = p_port->peer_ctrl.modem_signal;

    RFCOMM_TRACE_API ("PORT_GetModemStatus() handle:%d signal:%x", handle, *p_signal);

    return (PORT_SUCCESS);
}


/*******************************************************************************
**
** Function         PORT_ClearError
**
** Description      This function retreives information about a communications
**                  error and reports current status of a connection.  The
**                  function should be called when an error occures to clear
**                  the connection error flag and to enable additional read
**                  and write operations.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  p_errors   - pointer of the variable to receive error codes
**                  p_status   - pointer to the tPORT_STATUS structur to receive
**                               connection status
**
*******************************************************************************/
int PORT_ClearError (UINT16 handle, UINT16 *p_errors, tPORT_STATUS *p_status)
{
    tPORT  *p_port;

    RFCOMM_TRACE_API ("PORT_ClearError() handle:%d", handle);

    if ((handle == 0) || (handle > MAX_RFC_PORTS)) {
        return (PORT_BAD_HANDLE);
    }

    p_port = &rfc_cb.port.port[handle - 1];

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        return (PORT_NOT_OPENED);
    }

    *p_errors = p_port->line_status;

    /* This is the only call to clear error status.  We can not clear */
    /* connection failed status.  To clean it port should be closed and reopened */
    p_port->line_status = (p_port->line_status & LINE_STATUS_FAILED);

    PORT_GetQueueStatus (handle, p_status);
    return (PORT_SUCCESS);
}


/*******************************************************************************
**
** Function         PORT_SendError
**
** Description      This function send a communications error to the peer device
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  errors     - receive error codes
**
*******************************************************************************/
int PORT_SendError (UINT16 handle, UINT8 errors)
{
    tPORT      *p_port;

    RFCOMM_TRACE_API ("PORT_SendError() handle:%d errors:0x%x", handle, errors);

    if ((handle == 0) || (handle > MAX_RFC_PORTS)) {
        return (PORT_BAD_HANDLE);
    }

    p_port = &rfc_cb.port.port[handle - 1];

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        return (PORT_NOT_OPENED);
    }

    if (!p_port->rfc.p_mcb) {
        return (PORT_NOT_OPENED);
    }

    RFCOMM_LineStatusReq (p_port->rfc.p_mcb, p_port->dlci, errors);
    return (PORT_SUCCESS);
}


/*******************************************************************************
**
** Function         PORT_GetQueueStatus
**
** Description      This function reports current status of a connection.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  p_status   - pointer to the tPORT_STATUS structur to receive
**                               connection status
**
*******************************************************************************/
int PORT_GetQueueStatus (UINT16 handle, tPORT_STATUS *p_status)
{
    tPORT      *p_port;

    /* RFCOMM_TRACE_API ("PORT_GetQueueStatus() handle:%d", handle); */

    if ((handle == 0) || (handle > MAX_RFC_PORTS)) {
        return (PORT_BAD_HANDLE);
    }

    p_port = &rfc_cb.port.port[handle - 1];

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        return (PORT_NOT_OPENED);
    }

    p_status->in_queue_size  = (UINT16) p_port->rx.queue_size;
    p_status->out_queue_size = (UINT16) p_port->tx.queue_size;

    p_status->mtu_size = (UINT16) p_port->peer_mtu;

    p_status->flags = 0;

    if (!(p_port->peer_ctrl.modem_signal & PORT_CTSRTS_ON)) {
        p_status->flags |= PORT_FLAG_CTS_HOLD;
    }

    if (!(p_port->peer_ctrl.modem_signal & PORT_DTRDSR_ON)) {
        p_status->flags |= PORT_FLAG_DSR_HOLD;
    }

    if (!(p_port->peer_ctrl.modem_signal & PORT_DCD_ON)) {
        p_status->flags |= PORT_FLAG_RLSD_HOLD;
    }

    return (PORT_SUCCESS);
}


/*******************************************************************************
**
** Function         PORT_Purge
**
** Description      This function discards all the data from the output or
**                  input queues of the specified connection.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  purge_flags - specify the action to take.
**
*******************************************************************************/
int PORT_Purge (UINT16 handle, UINT8 purge_flags)
{
    tPORT      *p_port;
    BT_HDR     *p_buf;
    UINT16      count;
    UINT32     events;

    RFCOMM_TRACE_API ("PORT_Purge() handle:%d flags:0x%x", handle, purge_flags);

    /* Check if handle is valid to avoid crashing */
    if ((handle == 0) || (handle > MAX_RFC_PORTS)) {
        return (PORT_BAD_HANDLE);
    }

    p_port = &rfc_cb.port.port[handle - 1];

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        return (PORT_NOT_OPENED);
    }

    if (purge_flags & PORT_PURGE_RXCLEAR) {
        osi_mutex_global_lock();  /* to prevent missing credit */

        count = fixed_queue_length(p_port->rx.queue);

        while ((p_buf = (BT_HDR *)fixed_queue_dequeue(p_port->rx.queue, 0)) != NULL) {
            osi_free (p_buf);
        }

        p_port->rx.queue_size = 0;

        osi_mutex_global_unlock();

        /* If we flowed controlled peer based on rx_queue size enable data again */
        if (count) {
            port_flow_control_peer (p_port, TRUE, count);
        }
    }

    if (purge_flags & PORT_PURGE_TXCLEAR) {
        osi_mutex_global_lock();  /* to prevent tx.queue_size from being negative */

        while ((p_buf = (BT_HDR *)fixed_queue_dequeue(p_port->tx.queue, 0)) != NULL) {
            osi_free (p_buf);
        }

        p_port->tx.queue_size = 0;

        osi_mutex_global_unlock();

        events = PORT_EV_TXEMPTY;

        events |= port_flow_control_user (p_port);

        events &= p_port->ev_mask;

        if ((p_port->p_callback != NULL) && events) {
            (p_port->p_callback)(events, p_port->inx);
        }
    }

    return (PORT_SUCCESS);
}


/*******************************************************************************
**
** Function         PORT_ReadData
**
** Description      Normally not GKI aware application will call this function
**                  after receiving PORT_EV_RXCHAR event.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  p_data      - Data area
**                  max_len     - Byte count requested
**                  p_len       - Byte count received
**
*******************************************************************************/
int PORT_ReadData (UINT16 handle, char *p_data, UINT16 max_len, UINT16 *p_len)
{
    tPORT      *p_port;
    BT_HDR     *p_buf;
    UINT16      count;

    RFCOMM_TRACE_API ("PORT_ReadData() handle:%d max_len:%d", handle, max_len);

    /* Initialize this in case of an error */
    *p_len = 0;

    /* Check if handle is valid to avoid crashing */
    if ((handle == 0) || (handle > MAX_RFC_PORTS)) {
        return (PORT_BAD_HANDLE);
    }

    p_port = &rfc_cb.port.port[handle - 1];

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        return (PORT_NOT_OPENED);
    }

    if (p_port->line_status) {
        return (PORT_LINE_ERR);
    }

    if (fixed_queue_is_empty(p_port->rx.queue)){
        return (PORT_SUCCESS);
    }

    count = 0;

    while (max_len)
    {
        p_buf = (BT_HDR *)fixed_queue_try_peek_first(p_port->rx.queue);
        if (p_buf == NULL){
            break;
        }

        if (p_buf->len > max_len) {
            memcpy (p_data, (UINT8 *)(p_buf + 1) + p_buf->offset, max_len);
            p_buf->offset += max_len;
            p_buf->len    -= max_len;

            *p_len += max_len;

            osi_mutex_global_lock();

            p_port->rx.queue_size -= max_len;

            osi_mutex_global_unlock();

            break;
        } else {
            memcpy (p_data, (UINT8 *)(p_buf + 1) + p_buf->offset, p_buf->len);

            *p_len  += p_buf->len;
            max_len -= p_buf->len;

            osi_mutex_global_lock();

            p_port->rx.queue_size -= p_buf->len;

            if (max_len) {
                p_data  += p_buf->len;
            }

            osi_free(fixed_queue_dequeue(p_port->rx.queue, 0));

            osi_mutex_global_unlock();

            count++;
        }
    }

    if (*p_len == 1) {
        RFCOMM_TRACE_EVENT ("PORT_ReadData queue:%d returned:%d %x", p_port->rx.queue_size, *p_len, (p_data[0]));
    } else {
        RFCOMM_TRACE_EVENT ("PORT_ReadData queue:%d returned:%d", p_port->rx.queue_size, *p_len);
    }

    /* If rfcomm suspended traffic from the peer based on the rx_queue_size */
    /* check if it can be resumed now */
    port_flow_control_peer (p_port, TRUE, count);

    return (PORT_SUCCESS);
}


/*******************************************************************************
**
** Function         PORT_Read
**
** Description      Normally application will call this function after receiving
**                  PORT_EV_RXCHAR event.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  pp_buf      - pointer to address of buffer with data,
**
*******************************************************************************/
int PORT_Read (UINT16 handle, BT_HDR **pp_buf)
{
    tPORT      *p_port;
    BT_HDR     *p_buf;

    RFCOMM_TRACE_API ("PORT_Read() handle:%d", handle);

    /* Check if handle is valid to avoid crashing */
    if ((handle == 0) || (handle > MAX_RFC_PORTS)) {
        return (PORT_BAD_HANDLE);
    }
    p_port = &rfc_cb.port.port[handle - 1];

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        return (PORT_NOT_OPENED);
    }

    if (p_port->line_status) {
        return (PORT_LINE_ERR);
    }

    osi_mutex_global_lock();

    p_buf = (BT_HDR *)fixed_queue_dequeue(p_port->rx.queue, 0);
    if (p_buf) {
        p_port->rx.queue_size -= p_buf->len;

        osi_mutex_global_unlock();

        /* If rfcomm suspended traffic from the peer based on the rx_queue_size */
        /* check if it can be resumed now */
        port_flow_control_peer (p_port, TRUE, 1);
    } else {
        osi_mutex_global_unlock();
    }

    *pp_buf = p_buf;
    return (PORT_SUCCESS);
}


/*******************************************************************************
**
** Function         port_write
**
** Description      This function when a data packet is received from the apper
**                  layer task.
**
** Parameters:      p_port     - pointer to address of port control block
**                  p_buf      - pointer to address of buffer with data,
**
*******************************************************************************/
static int port_write (tPORT *p_port, BT_HDR *p_buf)
{
    /* We should not allow to write data in to server port when connection is not opened */
    if (p_port->is_server && (p_port->rfc.state != RFC_STATE_OPENED)) {
        osi_free (p_buf);
        return (PORT_CLOSED);
    }

    /* Keep the data in pending queue if peer does not allow data, or */
    /* Peer is not ready or Port is not yet opened or initial port control */
    /* command has not been sent */
    if (p_port->tx.peer_fc
            || !p_port->rfc.p_mcb
            || !p_port->rfc.p_mcb->peer_ready
            || (p_port->rfc.state != RFC_STATE_OPENED)
            || ((p_port->port_ctrl & (PORT_CTRL_REQ_SENT | PORT_CTRL_IND_RECEIVED)) !=
                (PORT_CTRL_REQ_SENT | PORT_CTRL_IND_RECEIVED))) {
        if ((p_port->tx.queue_size  > PORT_TX_CRITICAL_WM)
         || (fixed_queue_length(p_port->tx.queue) > PORT_TX_BUF_CRITICAL_WM)){
            RFCOMM_TRACE_WARNING ("PORT_Write: Queue size: %d",
                                  p_port->tx.queue_size);

            osi_free (p_buf);

            if ((p_port->p_callback != NULL) && (p_port->ev_mask & PORT_EV_ERR)) {
                p_port->p_callback (PORT_EV_ERR, p_port->inx);
            }

            return (PORT_TX_FULL);
        }

        RFCOMM_TRACE_EVENT ("PORT_Write : Data is enqued. flow disabled %d peer_ready %d state %d ctrl_state %x",
                            p_port->tx.peer_fc,
                            (p_port->rfc.p_mcb && p_port->rfc.p_mcb->peer_ready),
                            p_port->rfc.state,
                            p_port->port_ctrl);

        fixed_queue_enqueue(p_port->tx.queue, p_buf, FIXED_QUEUE_MAX_TIMEOUT);
        p_port->tx.queue_size += p_buf->len;

        return (PORT_CMD_PENDING);
    } else {
        RFCOMM_TRACE_EVENT ("PORT_Write : Data is being sent");

        RFCOMM_DataReq (p_port->rfc.p_mcb, p_port->dlci, p_buf);
        return (PORT_SUCCESS);
    }
}

/*******************************************************************************
**
** Function         PORT_Write
**
** Description      This function when a data packet is received from the apper
**                  layer task.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  pp_buf      - pointer to address of buffer with data,
**
*******************************************************************************/
int PORT_Write (UINT16 handle, BT_HDR *p_buf)
{
    tPORT  *p_port;
    UINT32 event = 0;
    int    rc;

    RFCOMM_TRACE_API ("PORT_Write() handle:%d", handle);

    /* Check if handle is valid to avoid crashing */
    if ((handle == 0) || (handle > MAX_RFC_PORTS)) {
        osi_free (p_buf);
        return (PORT_BAD_HANDLE);
    }

    p_port = &rfc_cb.port.port[handle - 1];

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        osi_free (p_buf);
        return (PORT_NOT_OPENED);
    }

    if (p_port->line_status) {
        RFCOMM_TRACE_WARNING ("PORT_Write: Data dropped line_status:0x%x",
                              p_port->line_status);
        osi_free (p_buf);
        return (PORT_LINE_ERR);
    }

    rc = port_write (p_port, p_buf);
    event |= port_flow_control_user (p_port);

    switch (rc) {
    case PORT_TX_FULL:
        event |= PORT_EV_ERR;
        break;

    case PORT_SUCCESS:
        event |= (PORT_EV_TXCHAR | PORT_EV_TXEMPTY);
        break;
    }
    /* Mask out all events that are not of interest to user */
    event &= p_port->ev_mask;

    /* Send event to the application */
    if (p_port->p_callback && event) {
        (p_port->p_callback)(event, p_port->inx);
    }

    return (PORT_SUCCESS);
}
/*******************************************************************************
**
** Function         PORT_WriteDataCO
**
** Description      Normally not GKI aware application will call this function
**                  to send data to the port by callout functions
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  fd         - socket fd
**                  p_len      - Byte count returned
**
*******************************************************************************/
int PORT_WriteDataCO (UINT16 handle, int *p_len, int len, UINT8 *p_data)
{

    tPORT      *p_port;
    BT_HDR     *p_buf;
    UINT32     event = 0;
    int        rc = 0;
    UINT16     length;

    RFCOMM_TRACE_API ("PORT_WriteDataCO() handle:%d", handle);
    *p_len = 0;

    /* Check if handle is valid to avoid crashing */
    if ((handle == 0) || (handle > MAX_RFC_PORTS)) {
        return (PORT_BAD_HANDLE);
    }
    p_port = &rfc_cb.port.port[handle - 1];

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        RFCOMM_TRACE_WARNING ("PORT_WriteDataByFd() no port state:%d", p_port->state);
        return (PORT_NOT_OPENED);
    }

    if (!p_port->peer_mtu) {
        RFCOMM_TRACE_ERROR ("PORT_WriteDataByFd() peer_mtu:%d", p_port->peer_mtu);
        return (PORT_UNKNOWN_ERROR);
    }
    int available = 0;
    available = len;
    if (available == 0) {
        return PORT_SUCCESS;
    }
    /* Length for each buffer is the smaller of GKI buffer, peer MTU, or max_len */
    length = RFCOMM_DATA_BUF_SIZE -
             (UINT16)(sizeof(BT_HDR) + L2CAP_MIN_OFFSET + RFCOMM_DATA_OVERHEAD);

    while (available) {
        /* if we're over buffer high water mark, we're done */
        if ((p_port->tx.queue_size  > PORT_TX_HIGH_WM)
         || (fixed_queue_length(p_port->tx.queue) > PORT_TX_BUF_HIGH_WM)) {
            port_flow_control_user(p_port);
            event |= PORT_EV_FC;
            RFCOMM_TRACE_EVENT ("tx queue is full,tx.queue_size:%d,tx.queue.count:%d,available:%d",
                    p_port->tx.queue_size, fixed_queue_length(p_port->tx.queue), available);
            break;
        }

        /* continue with rfcomm data write */
        if (p_port->peer_mtu < length) {
            length = p_port->peer_mtu;
        }

        if (available < (int)length) {
            length = (UINT16)available;
        }

        UINT16 alloc_size = (UINT16)(sizeof(BT_HDR) + L2CAP_MIN_OFFSET + RFCOMM_DATA_OVERHEAD+length);
        p_buf = (BT_HDR *)osi_malloc(alloc_size);
        if (!p_buf) {
            RFCOMM_TRACE_EVENT ("PORT_WriteDataCO: out of heap.");
            break;
        }

        p_buf->offset         = L2CAP_MIN_OFFSET + RFCOMM_MIN_OFFSET;
        p_buf->layer_specific = handle;

        p_buf->len = length;
        p_buf->event          = BT_EVT_TO_BTU_SP_DATA;

        memcpy ((UINT8 *)(p_buf + 1) + p_buf->offset, p_data, length);

        RFCOMM_TRACE_EVENT ("PORT_WriteData %d bytes", length);

        rc = port_write (p_port, p_buf);

        /* If queue went below the threshold need to send flow control */
        event |= port_flow_control_user (p_port);

        if (rc == PORT_SUCCESS) {
            event |= PORT_EV_TXCHAR;
        }

        if ((rc != PORT_SUCCESS) && (rc != PORT_CMD_PENDING)) {
            break;
        }

        *p_len  += length;
        available -= (int)length;
        p_data += length;
    }
    if (!available && (rc != PORT_CMD_PENDING) && (rc != PORT_TX_QUEUE_DISABLED)) {
        event |= PORT_EV_TXEMPTY;
    }

    /* Mask out all events that are not of interest to user */
    event &= p_port->ev_mask;

    /* Send event to the application */
    if (p_port->p_callback && event) {
        (p_port->p_callback)(event, p_port->inx);
    }

    return (PORT_SUCCESS);
}



/*******************************************************************************
**
** Function         PORT_WriteData
**
** Description      Normally not GKI aware application will call this function
**                  to send data to the port.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  p_data      - Data area
**                  max_len     - Byte count requested
**                  p_len       - Byte count received
**
*******************************************************************************/
int PORT_WriteData (UINT16 handle, char *p_data, UINT16 max_len, UINT16 *p_len)
{
    tPORT      *p_port;
    BT_HDR     *p_buf;
    UINT32     event = 0;
    int        rc = 0;
    UINT16     length;

    RFCOMM_TRACE_API ("PORT_WriteData() max_len:%d", max_len);

    *p_len = 0;

    /* Check if handle is valid to avoid crashing */
    if ((handle == 0) || (handle > MAX_RFC_PORTS)) {
        return (PORT_BAD_HANDLE);
    }
    p_port = &rfc_cb.port.port[handle - 1];

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        RFCOMM_TRACE_WARNING ("PORT_WriteData() no port state:%d", p_port->state);
        return (PORT_NOT_OPENED);
    }

    if (!max_len || !p_port->peer_mtu) {
        RFCOMM_TRACE_ERROR ("PORT_WriteData() peer_mtu:%d", p_port->peer_mtu);
        return (PORT_UNKNOWN_ERROR);
    }

    /* Length for each buffer is the smaller of GKI buffer, peer MTU, or max_len */
    length = RFCOMM_DATA_BUF_SIZE -
             (UINT16)(sizeof(BT_HDR) + L2CAP_MIN_OFFSET + RFCOMM_DATA_OVERHEAD);

    /* If there are buffers scheduled for transmission check if requested */
    /* data fits into the end of the queue */
    osi_mutex_global_lock();

    if (((p_buf = (BT_HDR *)fixed_queue_try_peek_last(p_port->tx.queue)) != NULL)
            && ((p_buf->len + max_len) <= p_port->peer_mtu)
            && ((p_buf->len + max_len) <= length)) {
        memcpy ((UINT8 *)(p_buf + 1) + p_buf->offset + p_buf->len, p_data, max_len);
        p_port->tx.queue_size += max_len;

        *p_len = max_len;
        p_buf->len += max_len;

        osi_mutex_global_unlock();

        return (PORT_SUCCESS);
    }

    osi_mutex_global_unlock();

    while (max_len) {
        /* if we're over buffer high water mark, we're done */
        if ((p_port->tx.queue_size  > PORT_TX_HIGH_WM)
         || (fixed_queue_length(p_port->tx.queue) > PORT_TX_BUF_HIGH_WM)) {
            break;
        }

        /* continue with rfcomm data write */
        p_buf = (BT_HDR *)osi_malloc(RFCOMM_DATA_BUF_SIZE);
        if (!p_buf) {
            break;
        }

        p_buf->offset         = L2CAP_MIN_OFFSET + RFCOMM_MIN_OFFSET;
        p_buf->layer_specific = handle;

        if (p_port->peer_mtu < length) {
            length = p_port->peer_mtu;
        }
        if (max_len < length) {
            length = max_len;
        }
        p_buf->len = length;
        p_buf->event          = BT_EVT_TO_BTU_SP_DATA;

        memcpy ((UINT8 *)(p_buf + 1) + p_buf->offset, p_data, length);

        RFCOMM_TRACE_EVENT ("PORT_WriteData %d bytes", length);

        rc = port_write (p_port, p_buf);

        /* If queue went below the threashold need to send flow control */
        event |= port_flow_control_user (p_port);

        if (rc == PORT_SUCCESS) {
            event |= PORT_EV_TXCHAR;
        }

        if ((rc != PORT_SUCCESS) && (rc != PORT_CMD_PENDING)) {
            break;
        }

        *p_len  += length;
        max_len -= length;
        p_data  += length;

    }
    if (!max_len && (rc != PORT_CMD_PENDING) && (rc != PORT_TX_QUEUE_DISABLED)) {
        event |= PORT_EV_TXEMPTY;
    }

    /* Mask out all events that are not of interest to user */
    event &= p_port->ev_mask;

    /* Send event to the application */
    if (p_port->p_callback && event) {
        (p_port->p_callback)(event, p_port->inx);
    }

    return (PORT_SUCCESS);
}


/*******************************************************************************
**
** Function         PORT_Test
**
** Description      Application can call this function to send RFCOMM Test frame
**
** Parameters:      handle      - Handle returned in the RFCOMM_CreateConnection
**                  p_data      - Data area
**                  max_len     - Byte count requested
**
*******************************************************************************/
int PORT_Test (UINT16 handle, UINT8 *p_data, UINT16 len)
{
    BT_HDR   *p_buf;
    tPORT    *p_port;

    RFCOMM_TRACE_API ("PORT_Test() len:%d", len);

    if ((handle == 0) || (handle > MAX_RFC_PORTS)) {
        return (PORT_BAD_HANDLE);
    }
    p_port = &rfc_cb.port.port[handle - 1];

    if (!p_port->in_use || (p_port->state == PORT_STATE_CLOSED)) {
        return (PORT_NOT_OPENED);
    }

    if (len > ((p_port->mtu == 0) ? RFCOMM_DEFAULT_MTU : p_port->mtu)) {
        return (PORT_UNKNOWN_ERROR);
    }

    if ((p_buf = (BT_HDR *)osi_malloc(RFCOMM_CMD_BUF_SIZE)) != NULL) {

        p_buf->offset  = L2CAP_MIN_OFFSET + RFCOMM_MIN_OFFSET + 2;
        p_buf->len = len;

        memcpy ((UINT8 *)(p_buf + 1) + p_buf->offset, p_data, p_buf->len);

        rfc_send_test (p_port->rfc.p_mcb, TRUE, p_buf);
        return (PORT_SUCCESS);
    } else {
        return (PORT_NO_MEM);
    }
}

/*******************************************************************************
**
** Function         RFCOMM_Init
**
** Description      This function is called to initialize RFCOMM layer
**
** Returns          status
**
*******************************************************************************/
bt_status_t RFCOMM_Init (void)
{
#if RFC_DYNAMIC_MEMORY == TRUE
    rfc_cb_ptr = (tRFC_CB *)osi_malloc(sizeof(tRFC_CB));
    if (rfc_cb_ptr == NULL) {
        return BT_STATUS_NOMEM;
    }
#endif /* #if (RFC_DYNAMIC_MEMORY) */
    memset (&rfc_cb, 0, sizeof (tRFC_CB));  /* Init RFCOMM control block */

    rfc_cb.rfc.last_mux = MAX_BD_CONNECTIONS;

#if defined(RFCOMM_INITIAL_TRACE_LEVEL)
    rfc_cb.trace_level = RFCOMM_INITIAL_TRACE_LEVEL;
#else
    rfc_cb.trace_level = BT_TRACE_LEVEL_NONE;    /* No traces */
#endif

    rfcomm_l2cap_if_init ();
    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         RFCOMM_Deinit
**
** Description      This function is called to deinitialize the control block
**                  for this layer.
**
** Returns          void
**
*******************************************************************************/
void RFCOMM_Deinit(void)
{
#if RFC_DYNAMIC_MEMORY == TRUE
    if (rfc_cb_ptr){
        osi_free(rfc_cb_ptr);
        rfc_cb_ptr = NULL;
    }
#endif
}

/*******************************************************************************
**
** Function         PORT_SetTraceLevel
**
** Description      This function sets the trace level for RFCOMM. If called with
**                  a value of 0xFF, it simply reads the current trace level.
**
** Returns          the new (current) trace level
**
*******************************************************************************/
UINT8 PORT_SetTraceLevel (UINT8 new_level)
{
    if (new_level != 0xFF) {
        rfc_cb.trace_level = new_level;
    }

    return (rfc_cb.trace_level);
}

/*******************************************************************************
**
** Function         PORT_GetResultString
**
** Description      This function returns the human-readable string for a given
**                  result code.
**
** Returns          a pointer to the human-readable string for the given result.
**
*******************************************************************************/
const char *PORT_GetResultString (const uint8_t result_code)
{
    if (result_code > PORT_ERR_MAX) {
        return result_code_strings[PORT_ERR_MAX];
    }

    return result_code_strings[result_code];
}

#endif ///(defined RFCOMM_INCLUDED && RFCOMM_INCLUDED == TRUE)

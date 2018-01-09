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
 *  Port Emulation entity utilities
 *
 ******************************************************************************/
#include <string.h>

#include "bt_target.h"
#include "rfcdefs.h"
#include "port_api.h"
#include "port_int.h"
#include "rfc_int.h"
#include "l2cdefs.h"
#include "btm_int.h"
#include "btu.h"
#include "mutex.h"
#include "allocator.h"
#if (defined RFCOMM_INCLUDED && RFCOMM_INCLUDED == TRUE)

static const tPORT_STATE default_port_pars = {
    PORT_BAUD_RATE_9600,
    PORT_8_BITS,
    PORT_ONESTOPBIT,
    PORT_PARITY_NO,
    PORT_ODD_PARITY,
    PORT_FC_OFF,
    0,                      /* No rx_char */
    PORT_XON_DC1,
    PORT_XOFF_DC3,
};



/*******************************************************************************
**
** Function         port_allocate_port
**
** Description      Look through the Port Control Blocks for a free one.  Note
**                  that one server can open several ports with the same SCN
**                  if it can support simulteneous requests from different
**                  clients.
**
** Returns          Pointer to the PORT or NULL if not found
**
*******************************************************************************/
tPORT *port_allocate_port (UINT8 dlci, BD_ADDR bd_addr)
{
    tPORT  *p_port = &rfc_cb.port.port[0];
    UINT8  xx, yy;

    for (xx = 0, yy = rfc_cb.rfc.last_port + 1; xx < MAX_RFC_PORTS; xx++, yy++) {
        if (yy >= MAX_RFC_PORTS) {
            yy = 0;
        }

        p_port = &rfc_cb.port.port[yy];
        if (!p_port->in_use) {
            memset (p_port, 0, sizeof (tPORT));

            p_port->in_use = TRUE;
            p_port->inx    = yy + 1;

            p_port->dlci   = dlci;
            memcpy (p_port->bd_addr, bd_addr, BD_ADDR_LEN);

            /* During the open set default state for the port connection */
            port_set_defaults (p_port);

            rfc_cb.rfc.last_port = yy;
            RFCOMM_TRACE_DEBUG("rfc_cb.port.port[%d]:%p allocated, last_port:%d", yy, p_port, rfc_cb.rfc.last_port);
            RFCOMM_TRACE_DEBUG("port_allocate_port:bd_addr:%02x:%02x:%02x:%02x:%02x:%02x",
                               bd_addr[0], bd_addr[1], bd_addr[2], bd_addr[3], bd_addr[4], bd_addr[5]);
            return (p_port);
        }
    }

    /* If here, no free PORT found */
    return (NULL);
}


/*******************************************************************************
**
** Function         port_set_defaults
**
** Description      Set defualt port parameters
**
**
*******************************************************************************/
void port_set_defaults (tPORT *p_port)
{
    p_port->ev_mask        = 0;
    p_port->p_callback     = NULL;
    p_port->port_ctrl      = 0;
    p_port->error          = 0;
    p_port->line_status    = 0;
    p_port->rx_flag_ev_pending = FALSE;
    p_port->peer_mtu       = RFCOMM_DEFAULT_MTU;

    p_port->user_port_pars = default_port_pars;
    p_port->peer_port_pars = default_port_pars;

    p_port->credit_tx      = 0;
    p_port->credit_rx      = 0;
    /*  p_port->credit_rx_max  = PORT_CREDIT_RX_MAX;            Determined later */
    /*  p_port->credit_rx_low  = PORT_CREDIT_RX_LOW;            Determined later */

    memset (&p_port->local_ctrl, 0, sizeof (p_port->local_ctrl));
    memset (&p_port->peer_ctrl, 0, sizeof (p_port->peer_ctrl));
    memset (&p_port->rx, 0, sizeof (p_port->rx));
    memset (&p_port->tx, 0, sizeof (p_port->tx));

    p_port->tx.queue = fixed_queue_new(SIZE_MAX);
    p_port->rx.queue = fixed_queue_new(SIZE_MAX);
}

/*******************************************************************************
**
** Function         port_select_mtu
**
** Description      Select MTU which will best serve connection from our
**                  point of view.
**                  If our device is 1.2 or lower we calculate how many DH5s
**                  fit into 1 RFCOMM buffer.
**
**
*******************************************************************************/
void port_select_mtu (tPORT *p_port)
{
    UINT16 packet_size;

    /* Will select MTU only if application did not setup something */
    if (p_port->mtu == 0) {
        /* find packet size which connection supports */
        packet_size = btm_get_max_packet_size (p_port->bd_addr);
        if (packet_size == 0) {
            /* something is very wrong */
            RFCOMM_TRACE_WARNING ("port_select_mtu bad packet size");
            p_port->mtu = RFCOMM_DEFAULT_MTU;
        } else {
            /* We try to negotiate MTU that each packet can be split into whole
            number of max packets.  For example if link is 1.2 max packet size is 339 bytes.
            At first calculate how many whole packets it is.  MAX L2CAP is 1691 + 4 overhead.
            1695, that will be 5 Dh5 packets.  Now maximum RFCOMM packet is
            5 * 339 = 1695. Minus 4 bytes L2CAP header 1691.  Minus RFCOMM 6 bytes header overhead 1685

            For EDR 2.0 packet size is 1027.  So we better send RFCOMM packet as 1 3DH5 packet
            1 * 1027 = 1027.  Minus 4 bytes L2CAP header 1023.  Minus RFCOMM 6 bytes header overhead 1017 */
            if ((L2CAP_MTU_SIZE + L2CAP_PKT_OVERHEAD) >= packet_size) {
                p_port->mtu = ((L2CAP_MTU_SIZE + L2CAP_PKT_OVERHEAD) / packet_size * packet_size) - RFCOMM_DATA_OVERHEAD - L2CAP_PKT_OVERHEAD;
                RFCOMM_TRACE_DEBUG ("port_select_mtu selected %d based on connection speed", p_port->mtu);
            } else {
                p_port->mtu = L2CAP_MTU_SIZE - RFCOMM_DATA_OVERHEAD;
                RFCOMM_TRACE_DEBUG ("port_select_mtu selected %d based on l2cap PDU size", p_port->mtu);
            }
        }
    } else {
        RFCOMM_TRACE_DEBUG ("port_select_mtu application selected %d", p_port->mtu);
    }
    p_port->credit_rx_max  = (PORT_RX_HIGH_WM / p_port->mtu);
    if ( p_port->credit_rx_max > PORT_RX_BUF_HIGH_WM ) {
        p_port->credit_rx_max = PORT_RX_BUF_HIGH_WM;
    }
    p_port->credit_rx_low  = (PORT_RX_LOW_WM / p_port->mtu);
    if ( p_port->credit_rx_low > PORT_RX_BUF_LOW_WM ) {
        p_port->credit_rx_low = PORT_RX_BUF_LOW_WM;
    }
    p_port->rx_buf_critical = (PORT_RX_CRITICAL_WM / p_port->mtu);
    if ( p_port->rx_buf_critical > PORT_RX_BUF_CRITICAL_WM ) {
        p_port->rx_buf_critical = PORT_RX_BUF_CRITICAL_WM;
    }
    RFCOMM_TRACE_DEBUG ("port_select_mtu credit_rx_max %d, credit_rx_low %d, rx_buf_critical %d",
                        p_port->credit_rx_max, p_port->credit_rx_low, p_port->rx_buf_critical);
}


/*******************************************************************************
**
** Function         port_release_port
**
** Description      Release port infor control block.
**
** Returns          Pointer to the PORT or NULL if not found
**
*******************************************************************************/
void port_release_port (tPORT *p_port)
{
    BT_HDR *p_buf;
    UINT32 mask;
    tPORT_CALLBACK *p_port_cb;
    tPORT_STATE user_port_pars;

    osi_mutex_global_lock();
    RFCOMM_TRACE_DEBUG("port_release_port, p_port:%p", p_port);
    while ((p_buf = (BT_HDR *)fixed_queue_try_dequeue(p_port->rx.queue)) != NULL) {
        osi_free (p_buf);
    }

    p_port->rx.queue_size = 0;

    while ((p_buf = (BT_HDR *)fixed_queue_try_dequeue(p_port->tx.queue)) != NULL) {
        osi_free (p_buf);
    }

    p_port->tx.queue_size = 0;

    osi_mutex_global_unlock();

    p_port->state = PORT_STATE_CLOSED;

    if (p_port->rfc.state == RFC_STATE_CLOSED) {
        RFCOMM_TRACE_DEBUG ("rfc_port_closed DONE");
        if (p_port->rfc.p_mcb) {
            p_port->rfc.p_mcb->port_inx[p_port->dlci] = 0;

            /* If there are no more ports opened on this MCB release it */
            rfc_check_mcb_active (p_port->rfc.p_mcb);
        }
        rfc_port_timer_stop (p_port);
        fixed_queue_free(p_port->tx.queue, NULL);
        p_port->tx.queue = NULL;
        fixed_queue_free(p_port->rx.queue, NULL);
        p_port->rx.queue = NULL;
		
        RFCOMM_TRACE_DEBUG ("port_release_port:p_port->keep_port_handle:%d", p_port->keep_port_handle);
        if ( p_port->keep_port_handle ) {
            RFCOMM_TRACE_DEBUG ("port_release_port:Initialize handle:%d", p_port->inx);
            /* save event mask and callback */
            mask = p_port->ev_mask;
            p_port_cb = p_port->p_callback;
            user_port_pars = p_port->user_port_pars;

            port_set_defaults(p_port);
            /* restore */
            p_port->ev_mask         = mask;
            p_port->p_callback      = p_port_cb;
            p_port->user_port_pars  = user_port_pars;
            p_port->mtu             = p_port->keep_mtu;

            p_port->state           = PORT_STATE_OPENING;
            p_port->rfc.p_mcb       = NULL;
            if (p_port->is_server) {
                p_port->dlci       &= 0xfe;
            }

            p_port->local_ctrl.modem_signal = p_port->default_signal_state;
            memcpy (p_port->bd_addr, BT_BD_ANY, BD_ADDR_LEN);
        } else {
            RFCOMM_TRACE_DEBUG ("port_release_port:Clean-up handle:%d", p_port->inx);
            memset (p_port, 0, sizeof (tPORT));
        }
    }
}


/*******************************************************************************
**
** Function         port_find_mcb
**
** Description      This function checks if connection exists to device with
**                  the BD_ADDR.
**
*******************************************************************************/
tRFC_MCB *port_find_mcb (BD_ADDR bd_addr)
{
    int      i;

    for (i = 0; i < MAX_BD_CONNECTIONS; i++) {
        if ((rfc_cb.port.rfc_mcb[i].state != RFC_MX_STATE_IDLE)
                && !memcmp (rfc_cb.port.rfc_mcb[i].bd_addr, bd_addr, BD_ADDR_LEN)) {
            /* Multiplexer channel found do not change anything */
            RFCOMM_TRACE_DEBUG("port_find_mcb: found  bd_addr:%02x:%02x:%02x:%02x:%02x:%02x",
                               bd_addr[0], bd_addr[1], bd_addr[2], bd_addr[3], bd_addr[4], bd_addr[5]);
            RFCOMM_TRACE_DEBUG("port_find_mcb: rfc_cb.port.rfc_mcb:index:%d, %p, lcid:%d",
                               i, &rfc_cb.port.rfc_mcb[i], rfc_cb.port.rfc_mcb[i].lcid);
            return (&rfc_cb.port.rfc_mcb[i]);
        }
    }
    RFCOMM_TRACE_DEBUG("port_find_mcb: not found, bd_addr:%02x:%02x:%02x:%02x:%02x:%02x",
                       bd_addr[0], bd_addr[1], bd_addr[2], bd_addr[3], bd_addr[4], bd_addr[5]);
    return (NULL);
}


/*******************************************************************************
**
** Function         port_find_mcb_dlci_port
**
** Description      Find port on the multiplexer channel based on DLCI.  If
**                  this port with DLCI not found try to use even DLCI.  This
**                  is for the case when client is establishing connection on
**                  none-initiator MCB.
**
** Returns          Pointer to the PORT or NULL if not found
**
*******************************************************************************/
tPORT *port_find_mcb_dlci_port (tRFC_MCB *p_mcb, UINT8 dlci)
{
    UINT8 inx;

    if (!p_mcb) {
        return (NULL);
    }

    if (dlci > RFCOMM_MAX_DLCI) {
        return (NULL);
    }

    inx = p_mcb->port_inx[dlci];
    if (inx == 0) {
        RFCOMM_TRACE_DEBUG("port_find_mcb_dlci_port: p_mcb:%p, port_inx[dlci:%d] is 0", p_mcb, dlci);
        return (NULL);
    } else {
        return (&rfc_cb.port.port[inx - 1]);
    }
}


/*******************************************************************************
**
** Function         port_find_dlci_port
**
** Description      Find port with DLCI not assigned to multiplexer channel
**
** Returns          Pointer to the PORT or NULL if not found
**
*******************************************************************************/
tPORT *port_find_dlci_port (UINT8 dlci)
{
    UINT16 i;
    tPORT  *p_port;

    for (i = 0; i < MAX_RFC_PORTS; i++) {
        p_port = &rfc_cb.port.port[i];

        if (p_port->in_use && (p_port->rfc.p_mcb == NULL)) {
            if (p_port->dlci == dlci) {
                return (p_port);
            } else if ((dlci & 0x01) && (p_port->dlci == (dlci - 1))) {
                p_port->dlci++;
                return (p_port);
            }
        }
    }
    return (NULL);
}


/*******************************************************************************
**
** Function         port_find_port
**
** Description      Find port with DLCI, BD_ADDR
**
** Returns          Pointer to the PORT or NULL if not found
**
*******************************************************************************/
tPORT *port_find_port (UINT8 dlci, BD_ADDR bd_addr)
{
    UINT16 i;
    tPORT  *p_port;

    for (i = 0; i < MAX_RFC_PORTS; i++) {
        p_port = &rfc_cb.port.port[i];
        if (p_port->in_use
                && (p_port->dlci == dlci)
                && !memcmp (p_port->bd_addr, bd_addr, BD_ADDR_LEN)) {
            return (p_port);
        }
    }
    return (NULL);
}


/*******************************************************************************
**
** Function         port_flow_control_user
**
** Description      Check the current user flow control and if necessary return
**                  events to be send to the user based on the user's specified
**                  flow control type.
**
** Returns          event mask to be returned to the application
**
*******************************************************************************/
UINT32 port_flow_control_user (tPORT *p_port)
{
    UINT32 event = 0;

    /* Flow control to the user can be caused by flow controlling by the peer */
    /* (FlowInd, or flow control by the peer RFCOMM (Fcon) or internally if */
    /* tx_queue is full */
    BOOLEAN fc = p_port->tx.peer_fc
                 || !p_port->rfc.p_mcb
                 || !p_port->rfc.p_mcb->peer_ready
                 || (p_port->tx.queue_size > PORT_TX_HIGH_WM)
                 || (fixed_queue_length(p_port->tx.queue) > PORT_TX_BUF_HIGH_WM);

    if (p_port->tx.user_fc == fc) {
        return (0);
    }

    p_port->tx.user_fc = fc;

    if (fc) {
        event = PORT_EV_FC;
    } else {
        event = PORT_EV_FC | PORT_EV_FCS;
    }

    return (event);
}


/*******************************************************************************
**
** Function         port_get_signal_changes
**
** Description      Check modem signals that has been changed
**
** Returns          event mask to be returned to the application
**
*******************************************************************************/
UINT32 port_get_signal_changes (tPORT *p_port, UINT8 old_signals, UINT8 signal)
{
    UINT8  changed_signals = (signal ^ old_signals);
    UINT32 events = 0;

    if (changed_signals & PORT_DTRDSR_ON) {
        events |= PORT_EV_DSR;

        if (signal & PORT_DTRDSR_ON) {
            events |= PORT_EV_DSRS;
        }
    }

    if (changed_signals & PORT_CTSRTS_ON) {
        events |= PORT_EV_CTS;

        if (signal & PORT_CTSRTS_ON) {
            events |= PORT_EV_CTSS;
        }
    }

    if (changed_signals & PORT_RING_ON) {
        events |= PORT_EV_RING;
    }

    if (changed_signals & PORT_DCD_ON) {
        events |= PORT_EV_RLSD;

        if (signal & PORT_DCD_ON) {
            events |= PORT_EV_RLSDS;
        }
    }

    return (p_port->ev_mask & events);
}

/*******************************************************************************
**
** Function         port_flow_control_peer
**
** Description      Send flow control messages to the peer for both enabling
**                  and disabling flow control, for both credit-based and
**                  TS 07.10 flow control mechanisms.
**
** Returns          nothing
**
*******************************************************************************/
void port_flow_control_peer(tPORT *p_port, BOOLEAN enable, UINT16 count)
{
    if (!p_port->rfc.p_mcb) {
        return;
    }

    /* If using credit based flow control */
    if (p_port->rfc.p_mcb->flow == PORT_FC_CREDIT) {
        /* if want to enable flow from peer */
        if (enable) {
            /* update rx credits */
            if (count > p_port->credit_rx) {
                p_port->credit_rx = 0;
            } else {
                p_port->credit_rx -= count;
            }

            /* If credit count is less than low credit watermark, and user */
            /* did not force flow control, send a credit update */
            /* There might be a special case when we just adjusted rx_max */
            if ((p_port->credit_rx <= p_port->credit_rx_low)
                    && !p_port->rx.user_fc
                    && (p_port->credit_rx_max > p_port->credit_rx)) {
                rfc_send_credit(p_port->rfc.p_mcb, p_port->dlci,
                                (UINT8) (p_port->credit_rx_max - p_port->credit_rx));

                p_port->credit_rx = p_port->credit_rx_max;

                p_port->rx.peer_fc = FALSE;
            }
        }
        /* else want to disable flow from peer */
        else {
            /* if client registered data callback, just do what they want */
            if (p_port->p_data_callback || p_port->p_data_co_callback) {
                p_port->rx.peer_fc = TRUE;
            }
            /* if queue count reached credit rx max, set peer fc */
            else if (fixed_queue_length(p_port->rx.queue) >= p_port->credit_rx_max) {
                p_port->rx.peer_fc = TRUE;
            }
        }
    }
    /* else using TS 07.10 flow control */
    else {
        /* if want to enable flow from peer */
        if (enable) {
            /* If rfcomm suspended traffic from the peer based on the rx_queue_size */
            /* check if it can be resumed now */
            if (p_port->rx.peer_fc
                    && (p_port->rx.queue_size < PORT_RX_LOW_WM)
                    && (fixed_queue_length(p_port->rx.queue) < PORT_RX_BUF_LOW_WM)) {
                p_port->rx.peer_fc = FALSE;

                /* If user did not force flow control allow traffic now */
                if (!p_port->rx.user_fc) {
                    RFCOMM_FlowReq (p_port->rfc.p_mcb, p_port->dlci, TRUE);
                }
            }
        }
        /* else want to disable flow from peer */
        else {
            /* if client registered data callback, just do what they want */
            if (p_port->p_data_callback || p_port->p_data_co_callback) {
                p_port->rx.peer_fc = TRUE;
                RFCOMM_FlowReq (p_port->rfc.p_mcb, p_port->dlci, FALSE);
            }
            /* Check the size of the rx queue.  If it exceeds certain */
            /* level and flow control has not been sent to the peer do it now */
            else if ( ((p_port->rx.queue_size > PORT_RX_HIGH_WM)
                      || (fixed_queue_length(p_port->rx.queue) > PORT_RX_BUF_HIGH_WM))
                      && !p_port->rx.peer_fc) {
                RFCOMM_TRACE_EVENT ("PORT_DataInd Data reached HW. Sending FC set.");

                p_port->rx.peer_fc = TRUE;
                RFCOMM_FlowReq (p_port->rfc.p_mcb, p_port->dlci, FALSE);
            }
        }
    }
}


#endif ///(defined RFCOMM_INCLUDED && RFCOMM_INCLUDED == TRUE)
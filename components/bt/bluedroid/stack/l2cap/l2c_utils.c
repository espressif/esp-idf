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
 *  This file contains L2CAP utility functions
 *
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "allocator.h"
#include "controller.h"
#include "bt_types.h"
#include "hcimsgs.h"
#include "l2cdefs.h"
#include "l2c_int.h"
#include "hcidefs.h"
#include "btu.h"
#include "btm_api.h"
#include "btm_int.h"
#include "hcidefs.h"
#include "allocator.h"

/*******************************************************************************
**
** Function         l2cu_allocate_lcb
**
** Description      Look for an unused LCB
**
** Returns          LCB address or NULL if none found
**
*******************************************************************************/
tL2C_LCB *l2cu_allocate_lcb (BD_ADDR p_bd_addr, BOOLEAN is_bonding, tBT_TRANSPORT transport)
{
    int         xx;
    tL2C_LCB    *p_lcb = &l2cb.lcb_pool[0];

    for (xx = 0; xx < MAX_L2CAP_LINKS; xx++, p_lcb++) {
        if (!p_lcb->in_use) {
            memset (p_lcb, 0, sizeof (tL2C_LCB));

            memcpy (p_lcb->remote_bd_addr, p_bd_addr, BD_ADDR_LEN);

            p_lcb->in_use          = TRUE;
            p_lcb->link_state      = LST_DISCONNECTED;
            p_lcb->handle          = HCI_INVALID_HANDLE;
            p_lcb->link_flush_tout = 0xFFFF;
            p_lcb->timer_entry.param = (TIMER_PARAM_TYPE)p_lcb;
            p_lcb->info_timer_entry.param = (TIMER_PARAM_TYPE)p_lcb;
            p_lcb->upda_con_timer.param = (TIMER_PARAM_TYPE)p_lcb;
            p_lcb->idle_timeout    = l2cb.idle_timeout;
            p_lcb->id              = 1;                     /* spec does not allow '0' */
            p_lcb->is_bonding      = is_bonding;
#if (BLE_INCLUDED == TRUE)
            p_lcb->transport       = transport;
            p_lcb->tx_data_len     = controller_get_interface()->get_ble_default_data_packet_length();
            p_lcb->le_sec_pending_q = fixed_queue_new(SIZE_MAX);

            if (transport == BT_TRANSPORT_LE) {
                l2cb.num_ble_links_active++;
                l2c_ble_link_adjust_allocation();
            } else
#endif
            {
                l2cb.num_links_active++;
                l2c_link_adjust_allocation();
            }
            p_lcb->link_xmit_data_q = list_new(NULL);
            return (p_lcb);
        }
    }

    /* If here, no free LCB found */
    return (NULL);
}

/*******************************************************************************
**
** Function         l2cu_update_lcb_4_bonding
**
** Description      Mark the lcb for bonding. Used when bonding takes place on
**                  an existing ACL connection.  (Pre-Lisbon devices)
**
** Returns          Nothing
**
*******************************************************************************/
void l2cu_update_lcb_4_bonding (BD_ADDR p_bd_addr, BOOLEAN is_bonding)
{
    tL2C_LCB    *p_lcb = l2cu_find_lcb_by_bd_addr (p_bd_addr, BT_TRANSPORT_BR_EDR);

    if (p_lcb) {
        p_lcb->is_bonding = is_bonding;
    }
}

/*******************************************************************************
**
** Function         l2cu_release_lcb
**
** Description      Release an LCB. All timers will be stopped, channels
**                  dropped, buffers returned etc.
**
** Returns          void
**
*******************************************************************************/
void l2cu_release_lcb (tL2C_LCB *p_lcb)
{
    tL2C_CCB    *p_ccb;

    p_lcb->in_use     = FALSE;
    p_lcb->is_bonding = FALSE;

    /* Stop timers */
    btu_stop_timer (&p_lcb->timer_entry);
    btu_stop_timer (&p_lcb->info_timer_entry);

    /* Release any unfinished L2CAP packet on this link */
    if (p_lcb->p_hcit_rcv_acl) {
        osi_free(p_lcb->p_hcit_rcv_acl);
        p_lcb->p_hcit_rcv_acl = NULL;
    }

#if BTM_SCO_INCLUDED == TRUE
#if (BLE_INCLUDED == TRUE)
    if (p_lcb->transport == BT_TRANSPORT_BR_EDR)
#endif
        /* Release all SCO links */
        btm_remove_sco_links(p_lcb->remote_bd_addr);
#endif

    if (p_lcb->sent_not_acked > 0) {
#if (BLE_INCLUDED == TRUE)
        if (p_lcb->transport == BT_TRANSPORT_LE) {
            l2cb.controller_le_xmit_window += p_lcb->sent_not_acked;
            if (l2cb.controller_le_xmit_window > l2cb.num_lm_ble_bufs) {
                l2cb.controller_le_xmit_window = l2cb.num_lm_ble_bufs;
            }
        } else
#endif
        {
            l2cb.controller_xmit_window += p_lcb->sent_not_acked;
            if (l2cb.controller_xmit_window > l2cb.num_lm_acl_bufs) {
                l2cb.controller_xmit_window = l2cb.num_lm_acl_bufs;
            }
        }
    }

#if (BLE_INCLUDED == TRUE)
    // Reset BLE connecting flag only if the address matches
    if (!memcmp(l2cb.ble_connecting_bda, p_lcb->remote_bd_addr, BD_ADDR_LEN)) {
        l2cb.is_ble_connecting = FALSE;
    }
#endif

#if (L2CAP_NUM_FIXED_CHNLS > 0)
    l2cu_process_fixed_disc_cback(p_lcb);
#endif

    /* Ensure no CCBs left on this LCB */
    for (p_ccb = p_lcb->ccb_queue.p_first_ccb; p_ccb; p_ccb = p_lcb->ccb_queue.p_first_ccb) {
        l2cu_release_ccb (p_ccb);
    }

    /* Tell BTM Acl management the link was removed */
    if ((p_lcb->link_state == LST_CONNECTED) || (p_lcb->link_state == LST_DISCONNECTING))
#if (BLE_INCLUDED == TRUE)
        btm_acl_removed (p_lcb->remote_bd_addr, p_lcb->transport);
#else
        btm_acl_removed (p_lcb->remote_bd_addr, BT_TRANSPORT_BR_EDR);
#endif

    /* Release any held buffers */
    if (p_lcb->link_xmit_data_q) {
        while (!list_is_empty(p_lcb->link_xmit_data_q)) {
            BT_HDR *p_buf = list_front(p_lcb->link_xmit_data_q);
            list_remove(p_lcb->link_xmit_data_q, p_buf);
            osi_free(p_buf);
        }
        list_free(p_lcb->link_xmit_data_q);
        p_lcb->link_xmit_data_q = NULL;
    }

#if (L2CAP_UCD_INCLUDED == TRUE)
    /* clean up any security pending UCD */
    l2c_ucd_delete_sec_pending_q(p_lcb);
#endif

#if BLE_INCLUDED == TRUE
    /* Re-adjust flow control windows make sure it does not go negative */
    if (p_lcb->transport == BT_TRANSPORT_LE) {
        if (l2cb.num_ble_links_active >= 1) {
            l2cb.num_ble_links_active--;
        }

        l2c_ble_link_adjust_allocation();
    } else
#endif
    {
        if (l2cb.num_links_active >= 1) {
            l2cb.num_links_active--;
        }

        l2c_link_adjust_allocation();
    }

    /* Check for ping outstanding */
    if (p_lcb->p_echo_rsp_cb) {
        tL2CA_ECHO_RSP_CB *p_cb = p_lcb->p_echo_rsp_cb;

        /* Zero out the callback in case app immediately calls us again */
        p_lcb->p_echo_rsp_cb = NULL;

        (*p_cb) (L2CAP_PING_RESULT_NO_LINK);
    }

	/* Check and release all the LE COC connections waiting for security */
    if (p_lcb->le_sec_pending_q)
    {
        while (!fixed_queue_is_empty(p_lcb->le_sec_pending_q))
        {
            tL2CAP_SEC_DATA *p_buf = (tL2CAP_SEC_DATA*) fixed_queue_dequeue(p_lcb->le_sec_pending_q);
            if (p_buf->p_callback)
                p_buf->p_callback(p_lcb->remote_bd_addr, p_lcb->transport, p_buf->p_ref_data, BTM_DEV_RESET);
            osi_free(p_buf);
        }
        fixed_queue_free(p_lcb->le_sec_pending_q, NULL);
        p_lcb->le_sec_pending_q = NULL;
    }
}


/*******************************************************************************
**
** Function         l2cu_find_lcb_by_bd_addr
**
** Description      Look through all active LCBs for a match based on the
**                  remote BD address.
**
** Returns          pointer to matched LCB, or NULL if no match
**
*******************************************************************************/
tL2C_LCB  *l2cu_find_lcb_by_bd_addr (BD_ADDR p_bd_addr, tBT_TRANSPORT transport)
{
    int         xx;
    tL2C_LCB    *p_lcb = &l2cb.lcb_pool[0];

    for (xx = 0; xx < MAX_L2CAP_LINKS; xx++, p_lcb++) {
        if ((p_lcb->in_use) &&
#if BLE_INCLUDED == TRUE
                p_lcb->transport == transport &&
#endif
                (!memcmp (p_lcb->remote_bd_addr, p_bd_addr, BD_ADDR_LEN))) {
            return (p_lcb);
        }
    }

    /* If here, no match found */
    return (NULL);
}

/*******************************************************************************
**
** Function         l2cu_get_conn_role
**
** Description      Determine the desired role (master or slave) of a link.
**                  If already got a slave link, this one must be a master. If
**                  already got at least 1 link where we are the master, make this
**                  also a master.
**
** Returns          HCI_ROLE_MASTER or HCI_ROLE_SLAVE
**
*******************************************************************************/
UINT8 l2cu_get_conn_role (tL2C_LCB *p_this_lcb)
{
    return l2cb.desire_role;
}

/*******************************************************************************
**
** Function         l2c_is_cmd_rejected
**
** Description      Checks if cmd_code is command or response
**                  If a command it will be rejected per spec.
**                  This function is used when a illegal packet length is detected
**
** Returns          BOOLEAN - TRUE if cmd_code is a command and it is rejected,
**                            FALSE if response code. (command not rejected)
**
*******************************************************************************/
BOOLEAN l2c_is_cmd_rejected (UINT8 cmd_code, UINT8 id, tL2C_LCB *p_lcb)
{
    switch (cmd_code) {
    case L2CAP_CMD_CONN_REQ:
    case L2CAP_CMD_CONFIG_REQ:
    case L2CAP_CMD_DISC_REQ:
    case L2CAP_CMD_ECHO_REQ:
    case L2CAP_CMD_INFO_REQ:
    case L2CAP_CMD_AMP_CONN_REQ:
    case L2CAP_CMD_AMP_MOVE_REQ:
    case L2CAP_CMD_BLE_UPDATE_REQ:
        l2cu_send_peer_cmd_reject (p_lcb, L2CAP_CMD_REJ_MTU_EXCEEDED, id, L2CAP_DEFAULT_MTU, 0);
        L2CAP_TRACE_WARNING ("Dumping first Command (%d)", cmd_code);
        return TRUE;

    default:    /* Otherwise a response */
        return FALSE;
    }
}

/*******************************************************************************
**
** Function         l2cu_build_header
**
** Description      Builds the L2CAP command packet header
**
** Returns          Pointer to allocated packet or NULL if no resources
**
*******************************************************************************/
BT_HDR *l2cu_build_header (tL2C_LCB *p_lcb, UINT16 len, UINT8 cmd, UINT8 id)
{
    BT_HDR  *p_buf = (BT_HDR *)osi_malloc(L2CAP_CMD_BUF_SIZE);
    UINT8   *p;

    if (!p_buf) {
        return (NULL);
    }

    p_buf->offset = L2CAP_SEND_CMD_OFFSET;
    p_buf->len = len + HCI_DATA_PREAMBLE_SIZE + L2CAP_PKT_OVERHEAD + L2CAP_CMD_OVERHEAD;
    p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET;

    /* Put in HCI header - handle + pkt boundary */
#if (BLE_INCLUDED == TRUE)
    if (p_lcb->transport == BT_TRANSPORT_LE) {
        UINT16_TO_STREAM (p, (p_lcb->handle | (L2CAP_PKT_START_NON_FLUSHABLE << L2CAP_PKT_TYPE_SHIFT)));
    } else
#endif
    {
#if (L2CAP_NON_FLUSHABLE_PB_INCLUDED == TRUE)
        UINT16_TO_STREAM (p, p_lcb->handle | l2cb.non_flushable_pbf);
#else
        UINT16_TO_STREAM (p, (p_lcb->handle | (L2CAP_PKT_START << L2CAP_PKT_TYPE_SHIFT)));
#endif
    }

    UINT16_TO_STREAM (p, len + L2CAP_PKT_OVERHEAD + L2CAP_CMD_OVERHEAD);
    UINT16_TO_STREAM (p, len + L2CAP_CMD_OVERHEAD);

#if (BLE_INCLUDED == TRUE)
    if (p_lcb->transport == BT_TRANSPORT_LE) {
        //counter_add("l2cap.ble.tx.bytes", p_buf->len);
        //counter_add("l2cap.ble.tx.pkts", 1);

        UINT16_TO_STREAM (p, L2CAP_BLE_SIGNALLING_CID);
    } else
#endif
    {
        //counter_add("l2cap.sig.tx.bytes", p_buf->len);
        //counter_add("l2cap.sig.tx.pkts", 1);
        UINT16_TO_STREAM (p, L2CAP_SIGNALLING_CID);
    }

    /* Put in L2CAP command header */
    UINT8_TO_STREAM  (p, cmd);
    UINT8_TO_STREAM  (p, id);
    UINT16_TO_STREAM (p, len);

    return (p_buf);
}

/*******************************************************************************
**
** Function         l2cu_adj_id
**
** Description      Checks for valid ID based on specified mask
**                  and adjusts the id if invalid.
**
** Returns          void
**
*******************************************************************************/
void l2cu_adj_id (tL2C_LCB *p_lcb, UINT8 adj_mask)
{
    if ((adj_mask & L2CAP_ADJ_ZERO_ID) && !p_lcb->id) {
        p_lcb->id++;
    }
}

/*******************************************************************************
**
** Function         l2cu_send_peer_cmd_reject
**
** Description      Build and send an L2CAP "command reject" message
**                  to the peer.
**
** Returns          void
**
*******************************************************************************/
void l2cu_send_peer_cmd_reject (tL2C_LCB *p_lcb, UINT16 reason, UINT8 rem_id,
                                UINT16 p1, UINT16 p2)
{
    UINT16  param_len;
    BT_HDR  *p_buf;
    UINT8   *p;

    /* Put in L2CAP packet header */
    if (reason == L2CAP_CMD_REJ_MTU_EXCEEDED) {
        param_len = 2;
    } else if (reason == L2CAP_CMD_REJ_INVALID_CID) {
        param_len = 4;
    } else {
        param_len = 0;
    }

    if ((p_buf = l2cu_build_header (p_lcb, (UINT16) (L2CAP_CMD_REJECT_LEN + param_len), L2CAP_CMD_REJECT, rem_id)) == NULL ) {
        L2CAP_TRACE_WARNING ("L2CAP - no buffer cmd_rej");
        return;
    }

    p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + HCI_DATA_PREAMBLE_SIZE +
        L2CAP_PKT_OVERHEAD + L2CAP_CMD_OVERHEAD;

    UINT16_TO_STREAM (p, reason);

    if (param_len >= 2) {
        UINT16_TO_STREAM (p, p1);
    }

    if (param_len >= 4) {
        UINT16_TO_STREAM (p, p2);
    }

    l2c_link_check_send_pkts (p_lcb, NULL, p_buf);
}


/*******************************************************************************
**
** Function         l2cu_send_peer_connect_req
**
** Description      Build and send an L2CAP "connection request" message
**                  to the peer.
**
** Returns          void
**
*******************************************************************************/
void l2cu_send_peer_connect_req (tL2C_CCB *p_ccb)
{
    BT_HDR  *p_buf;
    UINT8   *p;

    /* Create an identifier for this packet */
    p_ccb->p_lcb->id++;
    l2cu_adj_id(p_ccb->p_lcb, L2CAP_ADJ_ID);

    p_ccb->local_id = p_ccb->p_lcb->id;

    if ((p_buf = l2cu_build_header (p_ccb->p_lcb, L2CAP_CONN_REQ_LEN, L2CAP_CMD_CONN_REQ,
                                    p_ccb->local_id)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no buffer for conn_req");
        return;
    }

    p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + HCI_DATA_PREAMBLE_SIZE +
        L2CAP_PKT_OVERHEAD + L2CAP_CMD_OVERHEAD;

    UINT16_TO_STREAM (p, p_ccb->p_rcb->real_psm);
    UINT16_TO_STREAM (p, p_ccb->local_cid);

    l2c_link_check_send_pkts (p_ccb->p_lcb, NULL, p_buf);
}


/*******************************************************************************
**
** Function         l2cu_send_peer_connect_rsp
**
** Description      Build and send an L2CAP "connection response" message
**                  to the peer.
**
** Returns          void
**
*******************************************************************************/
void l2cu_send_peer_connect_rsp (tL2C_CCB *p_ccb, UINT16 result, UINT16 status)
{
    BT_HDR  *p_buf;
    UINT8   *p;

    if (result == L2CAP_CONN_PENDING) {
        /* if we already sent pending response */
        if (p_ccb->flags & CCB_FLAG_SENT_PENDING) {
            return;
        } else {
            p_ccb->flags |= CCB_FLAG_SENT_PENDING;
        }
    }

    if ((p_buf = l2cu_build_header(p_ccb->p_lcb, L2CAP_CONN_RSP_LEN, L2CAP_CMD_CONN_RSP, p_ccb->remote_id)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no buffer for conn_rsp");
        return;
    }

    p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + HCI_DATA_PREAMBLE_SIZE +
        L2CAP_PKT_OVERHEAD + L2CAP_CMD_OVERHEAD;

    UINT16_TO_STREAM (p, p_ccb->local_cid);
    UINT16_TO_STREAM (p, p_ccb->remote_cid);
    UINT16_TO_STREAM (p, result);
    UINT16_TO_STREAM (p, status);

    l2c_link_check_send_pkts (p_ccb->p_lcb, NULL, p_buf);
}


/*******************************************************************************
**
** Function         l2cu_reject_connection
**
** Description      Build and send an L2CAP "connection response neg" message
**                  to the peer. This function is called when there is no peer
**                  CCB (non-existant PSM or no resources).
**
** Returns          void
**
*******************************************************************************/
void l2cu_reject_connection (tL2C_LCB *p_lcb, UINT16 remote_cid, UINT8 rem_id, UINT16 result)
{
    BT_HDR  *p_buf;
    UINT8   *p;

    if ((p_buf = l2cu_build_header(p_lcb, L2CAP_CONN_RSP_LEN, L2CAP_CMD_CONN_RSP, rem_id)) == NULL ) {
        L2CAP_TRACE_WARNING ("L2CAP - no buffer for conn_req");
        return;
    }

    p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + HCI_DATA_PREAMBLE_SIZE + L2CAP_PKT_OVERHEAD + L2CAP_CMD_OVERHEAD;

    UINT16_TO_STREAM (p, 0);                    /* Local CID of 0   */
    UINT16_TO_STREAM (p, remote_cid);
    UINT16_TO_STREAM (p, result);
    UINT16_TO_STREAM (p, 0);                    /* Status of 0      */

    l2c_link_check_send_pkts (p_lcb, NULL, p_buf);
}

/*******************************************************************************
**
** Function         l2cu_send_peer_config_req
**
** Description      Build and send an L2CAP "configuration request" message
**                  to the peer.
**
** Returns          void
**
*******************************************************************************/
void l2cu_send_peer_config_req (tL2C_CCB *p_ccb, tL2CAP_CFG_INFO *p_cfg)
{
    BT_HDR  *p_buf;
    UINT16  cfg_len = 0;
    UINT8   *p;

    /* Create an identifier for this packet */
    p_ccb->p_lcb->id++;
    l2cu_adj_id(p_ccb->p_lcb, L2CAP_ADJ_ID);

    p_ccb->local_id = p_ccb->p_lcb->id;

    if (p_cfg->mtu_present) {
        cfg_len += L2CAP_CFG_MTU_OPTION_LEN + L2CAP_CFG_OPTION_OVERHEAD;
    }
    if (p_cfg->flush_to_present) {
        cfg_len += L2CAP_CFG_FLUSH_OPTION_LEN + L2CAP_CFG_OPTION_OVERHEAD;
    }
    if (p_cfg->qos_present) {
        cfg_len += L2CAP_CFG_QOS_OPTION_LEN + L2CAP_CFG_OPTION_OVERHEAD;
    }
    if (p_cfg->fcr_present) {
        cfg_len += L2CAP_CFG_FCR_OPTION_LEN + L2CAP_CFG_OPTION_OVERHEAD;
    }
    if (p_cfg->fcs_present) {
        cfg_len += L2CAP_CFG_FCS_OPTION_LEN + L2CAP_CFG_OPTION_OVERHEAD;
    }
    if (p_cfg->ext_flow_spec_present) {
        cfg_len += L2CAP_CFG_EXT_FLOW_OPTION_LEN + L2CAP_CFG_OPTION_OVERHEAD;
    }

    if ((p_buf = l2cu_build_header (p_ccb->p_lcb, (UINT16) (L2CAP_CONFIG_REQ_LEN + cfg_len),
                                    L2CAP_CMD_CONFIG_REQ, p_ccb->local_id)) == NULL ) {
        L2CAP_TRACE_WARNING ("L2CAP - no buffer for conn_req");
        return;
    }

    p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + HCI_DATA_PREAMBLE_SIZE +
        L2CAP_PKT_OVERHEAD + L2CAP_CMD_OVERHEAD;

    UINT16_TO_STREAM (p, p_ccb->remote_cid);
    UINT16_TO_STREAM (p, p_cfg->flags);                    /* Flags (continuation) */

    /* Now, put the options */
    if (p_cfg->mtu_present) {
        UINT8_TO_STREAM  (p, L2CAP_CFG_TYPE_MTU);
        UINT8_TO_STREAM  (p, L2CAP_CFG_MTU_OPTION_LEN);
        UINT16_TO_STREAM (p, p_cfg->mtu);
    }
    if (p_cfg->flush_to_present) {
        UINT8_TO_STREAM  (p, L2CAP_CFG_TYPE_FLUSH_TOUT);
        UINT8_TO_STREAM  (p, L2CAP_CFG_FLUSH_OPTION_LEN);
        UINT16_TO_STREAM (p, p_cfg->flush_to);
    }
    if (p_cfg->qos_present) {
        UINT8_TO_STREAM  (p, L2CAP_CFG_TYPE_QOS);
        UINT8_TO_STREAM  (p, L2CAP_CFG_QOS_OPTION_LEN);
        UINT8_TO_STREAM  (p, p_cfg->qos.qos_flags);
        UINT8_TO_STREAM  (p, p_cfg->qos.service_type);
        UINT32_TO_STREAM (p, p_cfg->qos.token_rate);
        UINT32_TO_STREAM (p, p_cfg->qos.token_bucket_size);
        UINT32_TO_STREAM (p, p_cfg->qos.peak_bandwidth);
        UINT32_TO_STREAM (p, p_cfg->qos.latency);
        UINT32_TO_STREAM (p, p_cfg->qos.delay_variation);
    }
    if (p_cfg->fcr_present) {
        UINT8_TO_STREAM  (p, L2CAP_CFG_TYPE_FCR);
        UINT8_TO_STREAM  (p, L2CAP_CFG_FCR_OPTION_LEN);
        UINT8_TO_STREAM  (p, p_cfg->fcr.mode);
        UINT8_TO_STREAM  (p, p_cfg->fcr.tx_win_sz);
        UINT8_TO_STREAM  (p, p_cfg->fcr.max_transmit);
        UINT16_TO_STREAM (p, p_cfg->fcr.rtrans_tout);
        UINT16_TO_STREAM (p, p_cfg->fcr.mon_tout);
        UINT16_TO_STREAM (p, p_cfg->fcr.mps);
    }

    if (p_cfg->fcs_present) {
        UINT8_TO_STREAM  (p, L2CAP_CFG_TYPE_FCS);
        UINT8_TO_STREAM  (p, L2CAP_CFG_FCS_OPTION_LEN);
        UINT8_TO_STREAM  (p, p_cfg->fcs);
    }

    if (p_cfg->ext_flow_spec_present) {
        UINT8_TO_STREAM  (p, L2CAP_CFG_TYPE_EXT_FLOW);
        UINT8_TO_STREAM  (p, L2CAP_CFG_EXT_FLOW_OPTION_LEN);
        UINT8_TO_STREAM  (p, p_cfg->ext_flow_spec.id);
        UINT8_TO_STREAM  (p, p_cfg->ext_flow_spec.stype);
        UINT16_TO_STREAM (p, p_cfg->ext_flow_spec.max_sdu_size);
        UINT32_TO_STREAM (p, p_cfg->ext_flow_spec.sdu_inter_time);
        UINT32_TO_STREAM (p, p_cfg->ext_flow_spec.access_latency);
        UINT32_TO_STREAM (p, p_cfg->ext_flow_spec.flush_timeout);
    }

    l2c_link_check_send_pkts (p_ccb->p_lcb, NULL, p_buf);
}

/*******************************************************************************
**
** Function         l2cu_send_peer_config_rsp
**
** Description      Build and send an L2CAP "configuration response" message
**                  to the peer.
**
** Returns          void
**
*******************************************************************************/
void l2cu_send_peer_config_rsp (tL2C_CCB *p_ccb, tL2CAP_CFG_INFO *p_cfg)
{
    BT_HDR  *p_buf;
    UINT16  cfg_len = 0;
    UINT8   *p;

    /* Create an identifier for this packet */
    if (p_cfg->mtu_present) {
        cfg_len += L2CAP_CFG_MTU_OPTION_LEN + L2CAP_CFG_OPTION_OVERHEAD;
    }
    if (p_cfg->flush_to_present) {
        cfg_len += L2CAP_CFG_FLUSH_OPTION_LEN + L2CAP_CFG_OPTION_OVERHEAD;
    }
    if (p_cfg->qos_present) {
        cfg_len += L2CAP_CFG_QOS_OPTION_LEN + L2CAP_CFG_OPTION_OVERHEAD;
    }
    if (p_cfg->fcr_present) {
        cfg_len += L2CAP_CFG_FCR_OPTION_LEN + L2CAP_CFG_OPTION_OVERHEAD;
    }
    if (p_cfg->ext_flow_spec_present) {
        cfg_len += L2CAP_CFG_EXT_FLOW_OPTION_LEN + L2CAP_CFG_OPTION_OVERHEAD;
    }

    if ((p_buf = l2cu_build_header (p_ccb->p_lcb, (UINT16)(L2CAP_CONFIG_RSP_LEN + cfg_len),
                                    L2CAP_CMD_CONFIG_RSP, p_ccb->remote_id)) == NULL ) {
        L2CAP_TRACE_WARNING ("L2CAP - no buffer for conn_req");
        return;
    }

    p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + HCI_DATA_PREAMBLE_SIZE + L2CAP_PKT_OVERHEAD + L2CAP_CMD_OVERHEAD;

    UINT16_TO_STREAM (p, p_ccb->remote_cid);
    UINT16_TO_STREAM (p, p_cfg->flags);           /* Flags (continuation) Must match request */
    UINT16_TO_STREAM (p, p_cfg->result);

    /* Now, put the options */
    if (p_cfg->mtu_present) {
        UINT8_TO_STREAM  (p, L2CAP_CFG_TYPE_MTU);
        UINT8_TO_STREAM  (p, L2CAP_CFG_MTU_OPTION_LEN);
        UINT16_TO_STREAM (p, p_cfg->mtu);
    }
    if (p_cfg->flush_to_present) {
        UINT8_TO_STREAM  (p, L2CAP_CFG_TYPE_FLUSH_TOUT);
        UINT8_TO_STREAM  (p, L2CAP_CFG_FLUSH_OPTION_LEN);
        UINT16_TO_STREAM (p, p_cfg->flush_to);
    }
    if (p_cfg->qos_present) {
        UINT8_TO_STREAM  (p, L2CAP_CFG_TYPE_QOS);
        UINT8_TO_STREAM  (p, L2CAP_CFG_QOS_OPTION_LEN);
        UINT8_TO_STREAM  (p, p_cfg->qos.qos_flags);
        UINT8_TO_STREAM  (p, p_cfg->qos.service_type);
        UINT32_TO_STREAM (p, p_cfg->qos.token_rate);
        UINT32_TO_STREAM (p, p_cfg->qos.token_bucket_size);
        UINT32_TO_STREAM (p, p_cfg->qos.peak_bandwidth);
        UINT32_TO_STREAM (p, p_cfg->qos.latency);
        UINT32_TO_STREAM (p, p_cfg->qos.delay_variation);
    }
    if (p_cfg->fcr_present) {
        UINT8_TO_STREAM  (p, L2CAP_CFG_TYPE_FCR);
        UINT8_TO_STREAM  (p, L2CAP_CFG_FCR_OPTION_LEN);
        UINT8_TO_STREAM  (p, p_cfg->fcr.mode);
        UINT8_TO_STREAM  (p, p_cfg->fcr.tx_win_sz);
        UINT8_TO_STREAM  (p, p_cfg->fcr.max_transmit);
        UINT16_TO_STREAM (p, p_ccb->our_cfg.fcr.rtrans_tout);
        UINT16_TO_STREAM (p, p_ccb->our_cfg.fcr.mon_tout);
        UINT16_TO_STREAM (p, p_cfg->fcr.mps);
    }

    if (p_cfg->ext_flow_spec_present) {
        UINT8_TO_STREAM  (p, L2CAP_CFG_TYPE_EXT_FLOW);
        UINT8_TO_STREAM  (p, L2CAP_CFG_EXT_FLOW_OPTION_LEN);
        UINT8_TO_STREAM  (p, p_cfg->ext_flow_spec.id);
        UINT8_TO_STREAM  (p, p_cfg->ext_flow_spec.stype);
        UINT16_TO_STREAM (p, p_cfg->ext_flow_spec.max_sdu_size);
        UINT32_TO_STREAM (p, p_cfg->ext_flow_spec.sdu_inter_time);
        UINT32_TO_STREAM (p, p_cfg->ext_flow_spec.access_latency);
        UINT32_TO_STREAM (p, p_cfg->ext_flow_spec.flush_timeout);
    }

    l2c_link_check_send_pkts (p_ccb->p_lcb, NULL, p_buf);
}

/*******************************************************************************
**
** Function         l2cu_send_peer_config_rej
**
** Description      Build and send an L2CAP "configuration reject" message
**                  to the peer.
**
** Returns          void
**
*******************************************************************************/
void l2cu_send_peer_config_rej (tL2C_CCB *p_ccb, UINT8 *p_data, UINT16 data_len, UINT16 rej_len)
{
    BT_HDR  *p_buf;
    UINT16  len, cfg_len, buf_space, len1;
    UINT8   *p, *p_hci_len, *p_data_end;
    UINT8   cfg_code;

    L2CAP_TRACE_DEBUG("l2cu_send_peer_config_rej: data_len=%d, rej_len=%d", data_len, rej_len);


    len = BT_HDR_SIZE + HCI_DATA_PREAMBLE_SIZE + L2CAP_PKT_OVERHEAD + L2CAP_CMD_OVERHEAD + L2CAP_CONFIG_RSP_LEN;
    len1 = 0xFFFF - len;
    if (rej_len > len1) {
        L2CAP_TRACE_ERROR ("L2CAP - cfg_rej pkt size exceeds buffer design max limit.");
        return;
    }

    p_buf = (BT_HDR *)osi_malloc (len + rej_len);

    if (!p_buf) {
        L2CAP_TRACE_ERROR ("L2CAP - no buffer for cfg_rej");
        return;
    }

    p_buf->offset = L2CAP_SEND_CMD_OFFSET;
    p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET;

    /* Put in HCI header - handle + pkt boundary */
#if (L2CAP_NON_FLUSHABLE_PB_INCLUDED == TRUE)
    if (HCI_NON_FLUSHABLE_PB_SUPPORTED(BTM_ReadLocalFeatures ())) {
        UINT16_TO_STREAM (p, (p_ccb->p_lcb->handle | (L2CAP_PKT_START_NON_FLUSHABLE << L2CAP_PKT_TYPE_SHIFT)));
    } else
#endif
    {
        UINT16_TO_STREAM (p, (p_ccb->p_lcb->handle | (L2CAP_PKT_START << L2CAP_PKT_TYPE_SHIFT)));
    }

    /* Remember the HCI header length position, and save space for it */
    p_hci_len = p;
    p += 2;

    /* Put in L2CAP packet header */
    UINT16_TO_STREAM (p, L2CAP_CMD_OVERHEAD + L2CAP_CONFIG_RSP_LEN + rej_len);
    UINT16_TO_STREAM (p, L2CAP_SIGNALLING_CID);

    /* Put in L2CAP command header */
    UINT8_TO_STREAM  (p, L2CAP_CMD_CONFIG_RSP);
    UINT8_TO_STREAM  (p, p_ccb->remote_id);

    UINT16_TO_STREAM (p, L2CAP_CONFIG_RSP_LEN + rej_len);

    UINT16_TO_STREAM (p, p_ccb->remote_cid);
    UINT16_TO_STREAM (p, 0);                    /* Flags = 0 (no continuation) */
    UINT16_TO_STREAM (p, L2CAP_CFG_UNKNOWN_OPTIONS);

    buf_space = rej_len;

    /* Now, put the rejected options */
    p_data_end = p_data + data_len;
    while (p_data < p_data_end) {
        cfg_code = *p_data;
        cfg_len = *(p_data + 1);

        switch (cfg_code & 0x7F) {
        /* skip known options */
        case L2CAP_CFG_TYPE_MTU:
        case L2CAP_CFG_TYPE_FLUSH_TOUT:
        case L2CAP_CFG_TYPE_QOS:
            p_data += cfg_len + L2CAP_CFG_OPTION_OVERHEAD;
            break;

        /* unknown options; copy into rsp if not hints */
        default:
            /* sanity check option length */
            if ((cfg_len + L2CAP_CFG_OPTION_OVERHEAD) <= data_len) {
                if ((cfg_code & 0x80) == 0) {
                    if (buf_space >= (cfg_len + L2CAP_CFG_OPTION_OVERHEAD)) {
                        memcpy(p, p_data, cfg_len + L2CAP_CFG_OPTION_OVERHEAD);
                        p += cfg_len + L2CAP_CFG_OPTION_OVERHEAD;
                        buf_space -= (cfg_len + L2CAP_CFG_OPTION_OVERHEAD);
                    } else {
                        L2CAP_TRACE_WARNING("L2CAP - cfg_rej exceeds allocated buffer");
                        p_data = p_data_end; /* force loop exit */
                        break;
                    }
                }
                p_data += cfg_len + L2CAP_CFG_OPTION_OVERHEAD;
            }
            /* bad length; force loop exit */
            else {
                p_data = p_data_end;
            }
            break;
        }
    }

    len = (UINT16) (p - p_hci_len - 2);
    UINT16_TO_STREAM (p_hci_len, len);

    p_buf->len = len + 4;

    L2CAP_TRACE_DEBUG ("L2CAP - cfg_rej pkt hci_len=%d, l2cap_len=%d",
                       len, (L2CAP_CMD_OVERHEAD + L2CAP_CONFIG_RSP_LEN + rej_len));

    l2c_link_check_send_pkts (p_ccb->p_lcb, NULL, p_buf);
}

/*******************************************************************************
**
** Function         l2cu_send_peer_disc_req
**
** Description      Build and send an L2CAP "disconnect request" message
**                  to the peer.
**
** Returns          void
**
*******************************************************************************/
void l2cu_send_peer_disc_req (tL2C_CCB *p_ccb)
{
    BT_HDR  *p_buf, *p_buf2;
    UINT8   *p;

    /* Create an identifier for this packet */
    p_ccb->p_lcb->id++;
    l2cu_adj_id(p_ccb->p_lcb, L2CAP_ADJ_ID);

    p_ccb->local_id = p_ccb->p_lcb->id;

    if ((p_buf = l2cu_build_header(p_ccb->p_lcb, L2CAP_DISC_REQ_LEN, L2CAP_CMD_DISC_REQ, p_ccb->local_id)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no buffer for disc_req");
        return;
    }

    p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + HCI_DATA_PREAMBLE_SIZE + L2CAP_PKT_OVERHEAD + L2CAP_CMD_OVERHEAD;

    UINT16_TO_STREAM (p, p_ccb->remote_cid);
    UINT16_TO_STREAM (p, p_ccb->local_cid);

    /* Move all queued data packets to the LCB. In FCR mode, assume the higher
       layer checks that all buffers are sent before disconnecting.
    */
    if (p_ccb->peer_cfg.fcr.mode == L2CAP_FCR_BASIC_MODE) {
        while ((p_buf2 = (BT_HDR *)fixed_queue_try_dequeue(p_ccb->xmit_hold_q)) != NULL) {
            l2cu_set_acl_hci_header (p_buf2, p_ccb);
            l2c_link_check_send_pkts (p_ccb->p_lcb, p_ccb, p_buf2);
        }
    }

    l2c_link_check_send_pkts (p_ccb->p_lcb, NULL, p_buf);
}


/*******************************************************************************
**
** Function         l2cu_send_peer_disc_rsp
**
** Description      Build and send an L2CAP "disconnect response" message
**                  to the peer.
**
**                  This function is passed the parameters for the disconnect
**                  response instead of the CCB address, as it may be called
**                  to send a disconnect response when there is no CCB.
**
** Returns          void
**
*******************************************************************************/
void l2cu_send_peer_disc_rsp (tL2C_LCB *p_lcb, UINT8 remote_id, UINT16 local_cid,
                              UINT16 remote_cid)
{
    BT_HDR  *p_buf;
    UINT8   *p;

    if (!p_lcb) {
        L2CAP_TRACE_WARNING("lcb already released\n");
        return;
    }
    
    if ((p_buf = l2cu_build_header(p_lcb, L2CAP_DISC_RSP_LEN, L2CAP_CMD_DISC_RSP, remote_id)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no buffer for disc_rsp");
        return;
    }

    p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + HCI_DATA_PREAMBLE_SIZE + L2CAP_PKT_OVERHEAD + L2CAP_CMD_OVERHEAD;

    UINT16_TO_STREAM (p, local_cid);
    UINT16_TO_STREAM (p, remote_cid);

    l2c_link_check_send_pkts (p_lcb, NULL, p_buf);
}


/*******************************************************************************
**
** Function         l2cu_send_peer_echo_req
**
** Description      Build and send an L2CAP "echo request" message
**                  to the peer. Note that we do not currently allow
**                  data in the echo request.
**
** Returns          void
**
*******************************************************************************/
void l2cu_send_peer_echo_req (tL2C_LCB *p_lcb, UINT8 *p_data, UINT16 data_len)
{
    BT_HDR  *p_buf;
    UINT8   *p;

    p_lcb->id++;
    l2cu_adj_id(p_lcb, L2CAP_ADJ_ZERO_ID);  /* check for wrap to '0' */

    if ((p_buf = l2cu_build_header(p_lcb, (UINT16) (L2CAP_ECHO_REQ_LEN + data_len), L2CAP_CMD_ECHO_REQ, p_lcb->id)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no buffer for echo_req");
        return;
    }

    p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + HCI_DATA_PREAMBLE_SIZE + L2CAP_PKT_OVERHEAD + L2CAP_CMD_OVERHEAD;

    if (data_len) {
        ARRAY_TO_STREAM  (p, p_data, data_len);
    }

    l2c_link_check_send_pkts (p_lcb, NULL, p_buf);
}


/*******************************************************************************
**
** Function         l2cu_send_peer_echo_rsp
**
** Description      Build and send an L2CAP "echo response" message
**                  to the peer.
**
** Returns          void
**
*******************************************************************************/
void l2cu_send_peer_echo_rsp (tL2C_LCB *p_lcb, UINT8 id, UINT8 *p_data, UINT16 data_len)
{
    BT_HDR  *p_buf;
    UINT8   *p;
    UINT16   maxlen;
    /* Filter out duplicate IDs or if available buffers are low (intruder checking) */
    if (!id || id == p_lcb->cur_echo_id) {
        /* Dump this request since it is illegal */
        L2CAP_TRACE_WARNING ("L2CAP ignoring duplicate echo request (%d)", id);
        return;
    } else {
        p_lcb->cur_echo_id = id;
    }

    uint16_t acl_data_size = controller_get_interface()->get_acl_data_size_classic();
    uint16_t acl_packet_size = controller_get_interface()->get_acl_packet_size_classic();
    /* Don't return data if it does not fit in ACL and L2CAP MTU */
    maxlen = (L2CAP_CMD_BUF_SIZE > acl_packet_size) ?
               acl_data_size : (UINT16)L2CAP_CMD_BUF_SIZE;
    maxlen -= (UINT16)(BT_HDR_SIZE + HCI_DATA_PREAMBLE_SIZE + L2CAP_PKT_OVERHEAD +
                       L2CAP_CMD_OVERHEAD + L2CAP_ECHO_RSP_LEN);

    if (data_len > maxlen) {
        data_len = 0;
    }

    if ((p_buf = l2cu_build_header (p_lcb, (UINT16)(L2CAP_ECHO_RSP_LEN + data_len), L2CAP_CMD_ECHO_RSP, id)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no buffer for echo_rsp");
        return;
    }

    p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + HCI_DATA_PREAMBLE_SIZE +
        L2CAP_PKT_OVERHEAD + L2CAP_CMD_OVERHEAD;

    if (data_len) {
        ARRAY_TO_STREAM  (p, p_data, data_len);
    }

    l2c_link_check_send_pkts (p_lcb, NULL, p_buf);
}

/*******************************************************************************
**
** Function         l2cu_send_peer_info_req
**
** Description      Build and send an L2CAP "info request" message
**                  to the peer.
** Returns          void
**
*******************************************************************************/
void l2cu_send_peer_info_req (tL2C_LCB *p_lcb, UINT16 info_type)
{
    BT_HDR  *p_buf;
    UINT8   *p;

    /* check for wrap and/or BRCM ID */
    p_lcb->id++;
    l2cu_adj_id(p_lcb, L2CAP_ADJ_ID);

    if ((p_buf = l2cu_build_header(p_lcb, 2, L2CAP_CMD_INFO_REQ, p_lcb->id)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no buffer for info_req");
        return;
    }

    L2CAP_TRACE_EVENT ("l2cu_send_peer_info_req: type 0x%04x", info_type);

    p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + HCI_DATA_PREAMBLE_SIZE +
        L2CAP_PKT_OVERHEAD + L2CAP_CMD_OVERHEAD;

    UINT16_TO_STREAM (p, info_type);

    p_lcb->w4_info_rsp = TRUE;
    btu_start_timer (&p_lcb->info_timer_entry, BTU_TTYPE_L2CAP_INFO, L2CAP_WAIT_INFO_RSP_TOUT);

    l2c_link_check_send_pkts (p_lcb, NULL, p_buf);
}


/*******************************************************************************
**
** Function         l2cu_send_peer_info_rsp
**
** Description      Build and send an L2CAP "info response" message
**                  to the peer.
**
** Returns          void
**
*******************************************************************************/
void l2cu_send_peer_info_rsp (tL2C_LCB *p_lcb, UINT8 remote_id, UINT16 info_type)
{
    BT_HDR  *p_buf;
    UINT8   *p;
    UINT16   len = L2CAP_INFO_RSP_LEN;

#if (L2CAP_CONFORMANCE_TESTING == TRUE)
    if ((info_type == L2CAP_EXTENDED_FEATURES_INFO_TYPE)
            && (l2cb.test_info_resp & (L2CAP_EXTFEA_ENH_RETRANS   | L2CAP_EXTFEA_STREAM_MODE |
                                       L2CAP_EXTFEA_NO_CRC | L2CAP_EXTFEA_EXT_FLOW_SPEC |
                                       L2CAP_EXTFEA_FIXED_CHNLS | L2CAP_EXTFEA_EXT_WINDOW |
                                       L2CAP_EXTFEA_UCD_RECEPTION )) )
#else
    if ((info_type == L2CAP_EXTENDED_FEATURES_INFO_TYPE)
            && (L2CAP_EXTFEA_SUPPORTED_MASK & (L2CAP_EXTFEA_ENH_RETRANS | L2CAP_EXTFEA_STREAM_MODE |
                    L2CAP_EXTFEA_NO_CRC | L2CAP_EXTFEA_FIXED_CHNLS |
                    L2CAP_EXTFEA_UCD_RECEPTION )) )
#endif
    {
        len += L2CAP_EXTENDED_FEATURES_ARRAY_SIZE;
    } else if (info_type == L2CAP_FIXED_CHANNELS_INFO_TYPE) {
        len += L2CAP_FIXED_CHNL_ARRAY_SIZE;
    } else if (info_type == L2CAP_CONNLESS_MTU_INFO_TYPE) {
        len += L2CAP_CONNLESS_MTU_INFO_SIZE;
    }

    if ((p_buf = l2cu_build_header(p_lcb, len, L2CAP_CMD_INFO_RSP, remote_id)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no buffer for info_rsp");
        return;
    }

    p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + HCI_DATA_PREAMBLE_SIZE +
        L2CAP_PKT_OVERHEAD + L2CAP_CMD_OVERHEAD;

    UINT16_TO_STREAM (p, info_type);

#if (L2CAP_CONFORMANCE_TESTING == TRUE)
    if ((info_type == L2CAP_EXTENDED_FEATURES_INFO_TYPE)
            && (l2cb.test_info_resp & ( L2CAP_EXTFEA_ENH_RETRANS | L2CAP_EXTFEA_STREAM_MODE
                                        | L2CAP_EXTFEA_UCD_RECEPTION )) )
#else
    if ((info_type == L2CAP_EXTENDED_FEATURES_INFO_TYPE)
            && (L2CAP_EXTFEA_SUPPORTED_MASK & ( L2CAP_EXTFEA_ENH_RETRANS | L2CAP_EXTFEA_STREAM_MODE
                    | L2CAP_EXTFEA_UCD_RECEPTION )) )
#endif
    {
        UINT16_TO_STREAM (p, L2CAP_INFO_RESP_RESULT_SUCCESS);
#if (BLE_INCLUDED == TRUE)
        if (p_lcb->transport == BT_TRANSPORT_LE) {
            /* optional data are not added for now */
            UINT32_TO_STREAM (p, L2CAP_BLE_EXTFEA_MASK);
        } else
#endif
        {
#if L2CAP_CONFORMANCE_TESTING == TRUE
            UINT32_TO_STREAM (p, l2cb.test_info_resp);
#else
#if (L2CAP_NUM_FIXED_CHNLS > 0)
            UINT32_TO_STREAM (p, L2CAP_EXTFEA_SUPPORTED_MASK | L2CAP_EXTFEA_FIXED_CHNLS);
#else
            UINT32_TO_STREAM (p, L2CAP_EXTFEA_SUPPORTED_MASK);
#endif
#endif
        }
    } else if (info_type == L2CAP_FIXED_CHANNELS_INFO_TYPE) {
        UINT16_TO_STREAM (p, L2CAP_INFO_RESP_RESULT_SUCCESS);
        memset (p, 0, L2CAP_FIXED_CHNL_ARRAY_SIZE);

        p[0] = L2CAP_FIXED_CHNL_SIG_BIT;

        if ( L2CAP_EXTFEA_SUPPORTED_MASK & L2CAP_EXTFEA_UCD_RECEPTION ) {
            p[0] |= L2CAP_FIXED_CHNL_CNCTLESS_BIT;
        }

#if (L2CAP_NUM_FIXED_CHNLS > 0)
        {
            int xx;

            for (xx = 0; xx < L2CAP_NUM_FIXED_CHNLS; xx++)
                if (l2cb.fixed_reg[xx].pL2CA_FixedConn_Cb != NULL) {
                    p[(xx + L2CAP_FIRST_FIXED_CHNL) / 8] |= 1 << ((xx + L2CAP_FIRST_FIXED_CHNL) % 8);
                }
        }
#endif
    } else if (info_type == L2CAP_CONNLESS_MTU_INFO_TYPE) {
        UINT16_TO_STREAM (p, L2CAP_INFO_RESP_RESULT_SUCCESS);
        UINT16_TO_STREAM (p, L2CAP_UCD_MTU);
    } else {
        UINT16_TO_STREAM (p, L2CAP_INFO_RESP_RESULT_NOT_SUPPORTED);  /* 'not supported' */
    }

    l2c_link_check_send_pkts (p_lcb, NULL, p_buf);
}

/******************************************************************************
**
** Function         l2cu_enqueue_ccb
**
** Description      queue CCB by priority. The first CCB is highest priority and
**                  is served at first. The CCB is queued to an LLCB or an LCB.
**
** Returns          None
**
*******************************************************************************/
void l2cu_enqueue_ccb (tL2C_CCB *p_ccb)
{
    tL2C_CCB        *p_ccb1;
    tL2C_CCB_Q      *p_q = NULL;

    /* Find out which queue the channel is on
    */
    if (p_ccb->p_lcb != NULL) {
        p_q = &p_ccb->p_lcb->ccb_queue;
    }

    if ( (!p_ccb->in_use) || (p_q == NULL) ) {
        L2CAP_TRACE_ERROR ("l2cu_enqueue_ccb  CID: 0x%04x ERROR in_use: %u  p_lcb: %p",
                           p_ccb->local_cid, p_ccb->in_use, p_ccb->p_lcb);
        return;
    }

    L2CAP_TRACE_DEBUG ("l2cu_enqueue_ccb CID: 0x%04x  priority: %d",
                       p_ccb->local_cid, p_ccb->ccb_priority);

    /* If the queue is empty, we go at the front */
    if (!p_q->p_first_ccb) {
        p_q->p_first_ccb  = p_q->p_last_ccb   = p_ccb;
        p_ccb->p_next_ccb = p_ccb->p_prev_ccb = NULL;
    } else {
        p_ccb1 = p_q->p_first_ccb;

        while (p_ccb1 != NULL) {
            /* Insert new ccb at the end of the same priority. Lower number, higher priority */
            if (p_ccb->ccb_priority < p_ccb1->ccb_priority) {
                /* Are we at the head of the queue ? */
                if (p_ccb1 == p_q->p_first_ccb) {
                    p_q->p_first_ccb = p_ccb;
                } else {
                    p_ccb1->p_prev_ccb->p_next_ccb  = p_ccb;
                }

                p_ccb->p_next_ccb  = p_ccb1;
                p_ccb->p_prev_ccb  = p_ccb1->p_prev_ccb;
                p_ccb1->p_prev_ccb = p_ccb;
                break;
            }

            p_ccb1 = p_ccb1->p_next_ccb;
        }

        /* If we are lower then anyone in the list, we go at the end */
        if (!p_ccb1) {
            /* add new ccb at the end of the list */
            p_q->p_last_ccb->p_next_ccb = p_ccb;

            p_ccb->p_next_ccb   = NULL;
            p_ccb->p_prev_ccb   = p_q->p_last_ccb;
            p_q->p_last_ccb = p_ccb;
        }
    }

#if (L2CAP_ROUND_ROBIN_CHANNEL_SERVICE == TRUE)
    /* Adding CCB into round robin service table of its LCB */
    if (p_ccb->p_lcb != NULL) {
        /* if this is the first channel in this priority group */
        if (p_ccb->p_lcb->rr_serv[p_ccb->ccb_priority].num_ccb == 0 ) {
            /* Set the first channel to this CCB */
            p_ccb->p_lcb->rr_serv[p_ccb->ccb_priority].p_first_ccb = p_ccb;
            /* Set the next serving channel in this group to this CCB */
            p_ccb->p_lcb->rr_serv[p_ccb->ccb_priority].p_serve_ccb = p_ccb;
            /* Initialize quota of this priority group based on its priority */
            p_ccb->p_lcb->rr_serv[p_ccb->ccb_priority].quota = L2CAP_GET_PRIORITY_QUOTA(p_ccb->ccb_priority);
        }
        /* increase number of channels in this group */
        p_ccb->p_lcb->rr_serv[p_ccb->ccb_priority].num_ccb++;
    }
#endif

}

/******************************************************************************
**
** Function         l2cu_dequeue_ccb
**
** Description      dequeue CCB from a queue
**
** Returns          -
**
*******************************************************************************/
void l2cu_dequeue_ccb (tL2C_CCB *p_ccb)
{
    tL2C_CCB_Q      *p_q = NULL;

    L2CAP_TRACE_DEBUG ("l2cu_dequeue_ccb  CID: 0x%04x", p_ccb->local_cid);

    /* Find out which queue the channel is on
    */
    if (p_ccb->p_lcb != NULL) {
        p_q = &p_ccb->p_lcb->ccb_queue;
    }

    if ( (!p_ccb->in_use) || (p_q == NULL) || (p_q->p_first_ccb == NULL) ) {
        L2CAP_TRACE_ERROR ("l2cu_dequeue_ccb  CID: 0x%04x ERROR in_use: %u  p_lcb: %p  p_q: %p  p_q->p_first_ccb: %p",
                           p_ccb->local_cid, p_ccb->in_use, p_ccb->p_lcb, p_q, p_q ? p_q->p_first_ccb : 0);
        return;
    }

#if (L2CAP_ROUND_ROBIN_CHANNEL_SERVICE == TRUE)
    /* Removing CCB from round robin service table of its LCB */
    if (p_ccb->p_lcb != NULL) {
        /* decrease number of channels in this priority group */
        p_ccb->p_lcb->rr_serv[p_ccb->ccb_priority].num_ccb--;

        /* if it was the last channel in the priority group */
        if (p_ccb->p_lcb->rr_serv[p_ccb->ccb_priority].num_ccb == 0 ) {
            p_ccb->p_lcb->rr_serv[p_ccb->ccb_priority].p_first_ccb = NULL;
            p_ccb->p_lcb->rr_serv[p_ccb->ccb_priority].p_serve_ccb = NULL;
        } else {
            /* if it is the first channel of this group */
            if ( p_ccb->p_lcb->rr_serv[p_ccb->ccb_priority].p_first_ccb == p_ccb ) {
                p_ccb->p_lcb->rr_serv[p_ccb->ccb_priority].p_first_ccb = p_ccb->p_next_ccb;
            }
            /* if it is the next serving channel of this group */
            if ( p_ccb->p_lcb->rr_serv[p_ccb->ccb_priority].p_serve_ccb == p_ccb ) {
                /* simply, start serving from the first channel */
                p_ccb->p_lcb->rr_serv[p_ccb->ccb_priority].p_serve_ccb
                    = p_ccb->p_lcb->rr_serv[p_ccb->ccb_priority].p_first_ccb;
            }
        }
    }
#endif

    if (p_ccb == p_q->p_first_ccb) {
        /* We are removing the first in a queue */
        p_q->p_first_ccb = p_ccb->p_next_ccb;

        if (p_q->p_first_ccb) {
            p_q->p_first_ccb->p_prev_ccb = NULL;
        } else {
            p_q->p_last_ccb = NULL;
        }
    } else if (p_ccb == p_q->p_last_ccb) {
        /* We are removing the last in a queue */
        p_q->p_last_ccb = p_ccb->p_prev_ccb;
        p_q->p_last_ccb->p_next_ccb = NULL;
    } else {
        /* In the middle of a chain. */
        p_ccb->p_prev_ccb->p_next_ccb = p_ccb->p_next_ccb;
        p_ccb->p_next_ccb->p_prev_ccb = p_ccb->p_prev_ccb;
    }

    p_ccb->p_next_ccb = p_ccb->p_prev_ccb = NULL;
}

/******************************************************************************
**
** Function         l2cu_change_pri_ccb
**
** Description
**
** Returns          -
**
*******************************************************************************/
void l2cu_change_pri_ccb (tL2C_CCB *p_ccb, tL2CAP_CHNL_PRIORITY priority)
{
    if (p_ccb->ccb_priority != priority) {
        /* If CCB is not the only guy on the queue */
        if ( (p_ccb->p_next_ccb != NULL) || (p_ccb->p_prev_ccb != NULL) ) {
            L2CAP_TRACE_DEBUG ("Update CCB list in logical link");

            /* Remove CCB from queue and re-queue it at new priority */
            l2cu_dequeue_ccb (p_ccb);

            p_ccb->ccb_priority = priority;
            l2cu_enqueue_ccb (p_ccb);
        }
#if (L2CAP_ROUND_ROBIN_CHANNEL_SERVICE == TRUE)
        else {
            /* If CCB is the only guy on the queue, no need to re-enqueue */
            /* update only round robin service data */
            p_ccb->p_lcb->rr_serv[p_ccb->ccb_priority].num_ccb = 0;
            p_ccb->p_lcb->rr_serv[p_ccb->ccb_priority].p_first_ccb = NULL;
            p_ccb->p_lcb->rr_serv[p_ccb->ccb_priority].p_serve_ccb = NULL;

            p_ccb->ccb_priority = priority;

            p_ccb->p_lcb->rr_serv[p_ccb->ccb_priority].p_first_ccb = p_ccb;
            p_ccb->p_lcb->rr_serv[p_ccb->ccb_priority].p_serve_ccb = p_ccb;
            p_ccb->p_lcb->rr_serv[p_ccb->ccb_priority].quota = L2CAP_GET_PRIORITY_QUOTA(p_ccb->ccb_priority);
            p_ccb->p_lcb->rr_serv[p_ccb->ccb_priority].num_ccb = 1;
        }
#endif
    }
}

/*******************************************************************************
**
** Function         l2cu_allocate_ccb
**
** Description      This function allocates a Channel Control Block and
**                  attaches it to a link control block. The local CID
**                  is also assigned.
**
** Returns          pointer to CCB, or NULL if none
**
*******************************************************************************/
tL2C_CCB *l2cu_allocate_ccb (tL2C_LCB *p_lcb, UINT16 cid)
{
    tL2C_CCB    *p_ccb;
    tL2C_CCB    *p_prev;

    L2CAP_TRACE_DEBUG ("l2cu_allocate_ccb: cid 0x%04x", cid);

    if (!l2cb.p_free_ccb_first) {
        return (NULL);
    }

    /* If a CID was passed in, use that, else take the first free one */
    if (cid == 0) {
        p_ccb = l2cb.p_free_ccb_first;
        l2cb.p_free_ccb_first = p_ccb->p_next_ccb;
    } else {
        p_prev = NULL;

        p_ccb = &l2cb.ccb_pool[cid - L2CAP_BASE_APPL_CID];

        if (p_ccb == l2cb.p_free_ccb_first) {
            l2cb.p_free_ccb_first = p_ccb->p_next_ccb;
        } else {
            for (p_prev = l2cb.p_free_ccb_first; p_prev != NULL; p_prev = p_prev->p_next_ccb) {
                if (p_prev->p_next_ccb == p_ccb) {
                    p_prev->p_next_ccb = p_ccb->p_next_ccb;

                    if (p_ccb == l2cb.p_free_ccb_last) {
                        l2cb.p_free_ccb_last = p_prev;
                    }

                    break;
                }
            }
            if (p_prev == NULL) {
                L2CAP_TRACE_ERROR ("l2cu_allocate_ccb: could not find CCB for CID 0x%04x in the free list", cid);
                return NULL;
            }
        }
    }

    p_ccb->p_next_ccb = p_ccb->p_prev_ccb = NULL;

    p_ccb->in_use = TRUE;

    /* Get a CID for the connection */
    p_ccb->local_cid = L2CAP_BASE_APPL_CID + (UINT16)(p_ccb - l2cb.ccb_pool);

    p_ccb->p_lcb = p_lcb;
    p_ccb->p_rcb = NULL;
    p_ccb->should_free_rcb = false;

    /* Set priority then insert ccb into LCB queue (if we have an LCB) */
    p_ccb->ccb_priority = L2CAP_CHNL_PRIORITY_LOW;

    if (p_lcb) {
        l2cu_enqueue_ccb (p_ccb);
    }

    /* clear what peer wants to configure */
    p_ccb->peer_cfg_bits = 0;

    /* Put in default values for configuration */
    memset (&p_ccb->our_cfg, 0, sizeof(tL2CAP_CFG_INFO));
    memset (&p_ccb->peer_cfg, 0, sizeof(tL2CAP_CFG_INFO));

    /* Put in default values for local/peer configurations */
    p_ccb->our_cfg.flush_to              = p_ccb->peer_cfg.flush_to              = L2CAP_DEFAULT_FLUSH_TO;
    p_ccb->our_cfg.mtu                   = p_ccb->peer_cfg.mtu                   = L2CAP_DEFAULT_MTU;
    p_ccb->our_cfg.qos.service_type      = p_ccb->peer_cfg.qos.service_type      = L2CAP_DEFAULT_SERV_TYPE;
    p_ccb->our_cfg.qos.token_rate        = p_ccb->peer_cfg.qos.token_rate        = L2CAP_DEFAULT_TOKEN_RATE;
    p_ccb->our_cfg.qos.token_bucket_size = p_ccb->peer_cfg.qos.token_bucket_size = L2CAP_DEFAULT_BUCKET_SIZE;
    p_ccb->our_cfg.qos.peak_bandwidth    = p_ccb->peer_cfg.qos.peak_bandwidth    = L2CAP_DEFAULT_PEAK_BANDWIDTH;
    p_ccb->our_cfg.qos.latency           = p_ccb->peer_cfg.qos.latency           = L2CAP_DEFAULT_LATENCY;
    p_ccb->our_cfg.qos.delay_variation   = p_ccb->peer_cfg.qos.delay_variation   = L2CAP_DEFAULT_DELAY;

    p_ccb->bypass_fcs = 0;
    memset (&p_ccb->ertm_info, 0, sizeof(tL2CAP_ERTM_INFO));
    p_ccb->peer_cfg_already_rejected = FALSE;
    p_ccb->fcr_cfg_tries         = L2CAP_MAX_FCR_CFG_TRIES;
    p_ccb->fcrb.ack_timer.param  = (TIMER_PARAM_TYPE)p_ccb;

    /* if timer is running, remove it from timer list */
    if (p_ccb->fcrb.ack_timer.in_use) {
        btu_stop_quick_timer (&p_ccb->fcrb.ack_timer);
    }

    p_ccb->fcrb.mon_retrans_timer.param  = (TIMER_PARAM_TYPE)p_ccb;

// btla-specific ++
    /*  CSP408639 Fix: When L2CAP send amp move channel request or receive
      * L2CEVT_AMP_MOVE_REQ do following sequence. Send channel move
      * request -> Stop retrans/monitor timer -> Change channel state to CST_AMP_MOVING. */
    if (p_ccb->fcrb.mon_retrans_timer.in_use) {
        btu_stop_quick_timer (&p_ccb->fcrb.mon_retrans_timer);
    }
// btla-specific --
#if (CLASSIC_BT_INCLUDED == TRUE)
    l2c_fcr_stop_timer (p_ccb);
#endif  ///CLASSIC_BT_INCLUDED == TRUE
    p_ccb->ertm_info.preferred_mode  = L2CAP_FCR_BASIC_MODE;        /* Default mode for channel is basic mode */
    p_ccb->ertm_info.allowed_modes   = L2CAP_FCR_CHAN_OPT_BASIC;    /* Default mode for channel is basic mode */
    p_ccb->ertm_info.fcr_rx_buf_size = L2CAP_FCR_RX_BUF_SIZE;
    p_ccb->ertm_info.fcr_tx_buf_size = L2CAP_FCR_TX_BUF_SIZE;
    p_ccb->ertm_info.user_rx_buf_size = L2CAP_USER_RX_BUF_SIZE;
    p_ccb->ertm_info.user_tx_buf_size = L2CAP_USER_TX_BUF_SIZE;
    p_ccb->max_rx_mtu                = L2CAP_MTU_SIZE;
    p_ccb->tx_mps                    = L2CAP_FCR_TX_BUF_SIZE - 32;

    p_ccb->xmit_hold_q  = fixed_queue_new(SIZE_MAX);
    p_ccb->fcrb.srej_rcv_hold_q = fixed_queue_new(SIZE_MAX);
    p_ccb->fcrb.retrans_q = fixed_queue_new(SIZE_MAX);
    p_ccb->fcrb.waiting_for_ack_q = fixed_queue_new(SIZE_MAX);

    p_ccb->cong_sent    = FALSE;
    p_ccb->buff_quota   = 2;                /* This gets set after config */

    /* If CCB was reserved Config_Done can already have some value */
    if (cid == 0) {
        p_ccb->config_done  = 0;
    } else {
        L2CAP_TRACE_DEBUG ("l2cu_allocate_ccb: cid 0x%04x config_done:0x%x", cid, p_ccb->config_done);
    }

    p_ccb->chnl_state   = CST_CLOSED;
    p_ccb->flags        = 0;
    p_ccb->tx_data_rate = L2CAP_CHNL_DATA_RATE_LOW;
    p_ccb->rx_data_rate = L2CAP_CHNL_DATA_RATE_LOW;

#if (L2CAP_NON_FLUSHABLE_PB_INCLUDED == TRUE)
    p_ccb->is_flushable = FALSE;
#endif

    p_ccb->timer_entry.param = (TIMER_PARAM_TYPE)p_ccb;
    p_ccb->timer_entry.in_use = 0;

    l2c_link_adjust_chnl_allocation ();

    return (p_ccb);
}

/*******************************************************************************
**
** Function         l2cu_start_post_bond_timer
**
** Description      This function starts the ACL Link inactivity timer after
**                  dedicated bonding
**                  This timer can be longer than the normal link inactivity
**                  timer for some platforms.
**
** Returns          BOOLEAN  - TRUE if idle timer started or disconnect initiated
**                             FALSE if there's one or more pending CCB's exist
**
*******************************************************************************/
BOOLEAN l2cu_start_post_bond_timer (UINT16 handle)
{
    UINT16    timeout;
    tL2C_LCB *p_lcb = l2cu_find_lcb_by_handle(handle);

    if (!p_lcb) {
        return (TRUE);
    }

    p_lcb->is_bonding = FALSE;

    /* Only start timer if no control blocks allocated */
    if (p_lcb->ccb_queue.p_first_ccb != NULL) {
        return (FALSE);
    }

    /* If no channels on the connection, start idle timeout */
    if ( (p_lcb->link_state == LST_CONNECTED) || (p_lcb->link_state == LST_CONNECTING) || (p_lcb->link_state == LST_DISCONNECTING) ) {
        if (p_lcb->idle_timeout == 0) {
            if (btsnd_hcic_disconnect (p_lcb->handle, HCI_ERR_PEER_USER)) {
                p_lcb->link_state = LST_DISCONNECTING;
                timeout = L2CAP_LINK_DISCONNECT_TOUT;
            } else {
                timeout = BT_1SEC_TIMEOUT;
            }
        } else {
            timeout = L2CAP_BONDING_TIMEOUT;
        }

        if (timeout != 0xFFFF) {
            btu_start_timer (&p_lcb->timer_entry, BTU_TTYPE_L2CAP_LINK, timeout);
        }

        return (TRUE);
    }

    return (FALSE);
}

/*******************************************************************************
**
** Function         l2cu_release_ccb
**
** Description      This function releases a Channel Control Block. The timer
**                  is stopped, any attached buffers freed, and the CCB is removed
**                  from the link control block.
**
** Returns          void
**
*******************************************************************************/
void l2cu_release_ccb (tL2C_CCB *p_ccb)
{
    tL2C_LCB    *p_lcb = p_ccb->p_lcb;
    tL2C_RCB    *p_rcb = p_ccb->p_rcb;

    L2CAP_TRACE_DEBUG ("l2cu_release_ccb: cid 0x%04x  in_use: %u", p_ccb->local_cid, p_ccb->in_use);

    /* If already released, could be race condition */
    if (!p_ccb->in_use) {
        return;
    }
#if (SDP_INCLUDED == TRUE)
    if (p_rcb && (p_rcb->psm != p_rcb->real_psm)) {
        btm_sec_clr_service_by_psm(p_rcb->psm);
    }
#endif  ///SMP_INCLUDED == TRUE
    if (p_ccb->should_free_rcb) {
        osi_free(p_rcb);
        p_ccb->p_rcb = NULL;
        p_ccb->should_free_rcb = false;
    }

    if (p_lcb) {
        btm_sec_clr_temp_auth_service (p_lcb->remote_bd_addr);
    }

    /* Stop the timer */
    btu_stop_timer (&p_ccb->timer_entry);


    fixed_queue_free(p_ccb->xmit_hold_q, osi_free_func);
    p_ccb->xmit_hold_q = NULL;

#if (CLASSIC_BT_INCLUDED == TRUE)
    l2c_fcr_cleanup (p_ccb);
#endif  ///CLASSIC_BT_INCLUDED == TRUE
    /* Channel may not be assigned to any LCB if it was just pre-reserved */
    if ( (p_lcb) &&
            ( (p_ccb->local_cid >= L2CAP_BASE_APPL_CID)
#if (L2CAP_UCD_INCLUDED == TRUE)
              || (p_ccb->local_cid == L2CAP_CONNECTIONLESS_CID)
#endif
            )
       ) {
        l2cu_dequeue_ccb (p_ccb);

        /* Delink the CCB from the LCB */
        p_ccb->p_lcb = NULL;
    }

    /* Put the CCB back on the free pool */
    if (!l2cb.p_free_ccb_first) {
        l2cb.p_free_ccb_first = p_ccb;
        l2cb.p_free_ccb_last  = p_ccb;
        p_ccb->p_next_ccb     = NULL;
        p_ccb->p_prev_ccb     = NULL;
    } else {
        p_ccb->p_next_ccb  = NULL;
        p_ccb->p_prev_ccb  = l2cb.p_free_ccb_last;
        l2cb.p_free_ccb_last->p_next_ccb = p_ccb;
        l2cb.p_free_ccb_last  = p_ccb;
    }

    /* Flag as not in use */
    p_ccb->in_use = FALSE;

    /* If no channels on the connection, start idle timeout */
    if ((p_lcb) && p_lcb->in_use && (p_lcb->link_state == LST_CONNECTED)) {
        if (!p_lcb->ccb_queue.p_first_ccb) {
            l2cu_no_dynamic_ccbs (p_lcb);
        } else {
            /* Link is still active, adjust channel quotas. */
            l2c_link_adjust_chnl_allocation ();
        }
    }
}

/*******************************************************************************
**
** Function         l2cu_find_ccb_by_remote_cid
**
** Description      Look through all active CCBs on a link for a match based
**                  on the remote CID.
**
** Returns          pointer to matched CCB, or NULL if no match
**
*******************************************************************************/
tL2C_CCB *l2cu_find_ccb_by_remote_cid (tL2C_LCB *p_lcb, UINT16 remote_cid)
{
    tL2C_CCB    *p_ccb;

    /* If LCB is NULL, look through all active links */
    if (!p_lcb) {
        return NULL;
    } else {
        for (p_ccb = p_lcb->ccb_queue.p_first_ccb; p_ccb; p_ccb = p_ccb->p_next_ccb)
            if ((p_ccb->in_use) && (p_ccb->remote_cid == remote_cid)) {
                return (p_ccb);
            }
    }

    /* If here, no match found */
    return (NULL);
}

/*******************************************************************************
**
** Function         l2cu_allocate_rcb
**
** Description      Look through the Registration Control Blocks for a free
**                  one.
**
** Returns          Pointer to the RCB or NULL if not found
**
*******************************************************************************/
tL2C_RCB *l2cu_allocate_rcb (UINT16 psm)
{
    tL2C_RCB    *p_rcb = &l2cb.rcb_pool[0];
    UINT16      xx;

    for (xx = 0; xx < MAX_L2CAP_CLIENTS; xx++, p_rcb++) {
        if (!p_rcb->in_use) {
            p_rcb->in_use = TRUE;
            p_rcb->psm    = psm;
#if (L2CAP_UCD_INCLUDED == TRUE)
            p_rcb->ucd.state = L2C_UCD_STATE_UNUSED;
#endif
            return (p_rcb);
        }
    }

    /* If here, no free RCB found */
    return (NULL);
}

/*******************************************************************************
**
** Function         l2cu_allocate_ble_rcb
**
** Description      Look through the BLE Registration Control Blocks for a free
**                  one.
**
** Returns          Pointer to the BLE RCB or NULL if not found
**
*******************************************************************************/
tL2C_RCB *l2cu_allocate_ble_rcb (UINT16 psm)
{
    tL2C_RCB    *p_rcb = &l2cb.ble_rcb_pool[0];
    UINT16      xx;

    for (xx = 0; xx < BLE_MAX_L2CAP_CLIENTS; xx++, p_rcb++)
    {
        if (!p_rcb->in_use)
        {
            p_rcb->in_use = TRUE;
            p_rcb->psm    = psm;
#if (L2CAP_UCD_INCLUDED == TRUE)
            p_rcb->ucd.state = L2C_UCD_STATE_UNUSED;
#endif
            return (p_rcb);
        }
    }

    /* If here, no free RCB found */
    return (NULL);
}

/*******************************************************************************
**
** Function         l2cu_release_rcb
**
** Description      Mark an RCB as no longet in use
**
** Returns          void
**
*******************************************************************************/
void l2cu_release_rcb (tL2C_RCB *p_rcb)
{
    p_rcb->in_use = FALSE;
    p_rcb->psm    = 0;
}


/*******************************************************************************
**
** Function         l2cu_disconnect_chnl
**
** Description      Disconnect a channel. Typically, this is due to either
**                  receiving a bad configuration,  bad packet or max_retries expiring.
**
*******************************************************************************/
void l2cu_disconnect_chnl (tL2C_CCB *p_ccb)
{
    UINT16      local_cid = p_ccb->local_cid;

    if (local_cid >= L2CAP_BASE_APPL_CID) {
        tL2CA_DISCONNECT_IND_CB   *p_disc_cb = p_ccb->p_rcb->api.pL2CA_DisconnectInd_Cb;

        L2CAP_TRACE_WARNING ("L2CAP - disconnect_chnl CID: 0x%04x", local_cid);

        l2cu_send_peer_disc_req (p_ccb);

        l2cu_release_ccb (p_ccb);

        (*p_disc_cb)(local_cid, FALSE);
    } else {
        /* failure on the AMP channel, probably need to disconnect ACL */
        L2CAP_TRACE_ERROR ("L2CAP - disconnect_chnl CID: 0x%04x Ignored", local_cid);
    }
}


/*******************************************************************************
**
** Function         l2cu_find_rcb_by_psm
**
** Description      Look through the Registration Control Blocks to see if
**                  anyone registered to handle the PSM in question
**
** Returns          Pointer to the RCB or NULL if not found
**
*******************************************************************************/
tL2C_RCB *l2cu_find_rcb_by_psm (UINT16 psm)
{
    tL2C_RCB    *p_rcb = &l2cb.rcb_pool[0];
    UINT16      xx;

    for (xx = 0; xx < MAX_L2CAP_CLIENTS; xx++, p_rcb++) {
        if ((p_rcb->in_use) && (p_rcb->psm == psm)) {
            return (p_rcb);
        }
    }

    /* If here, no match found */
    return (NULL);
}

/*******************************************************************************
**
** Function         l2cu_find_ble_rcb_by_psm
**
** Description      Look through the BLE Registration Control Blocks to see if
**                  anyone registered to handle the PSM in question
**
** Returns          Pointer to the BLE RCB or NULL if not found
**
*******************************************************************************/
tL2C_RCB *l2cu_find_ble_rcb_by_psm (UINT16 psm)
{
    tL2C_RCB    *p_rcb = &l2cb.ble_rcb_pool[0];
    UINT16      xx;

    for (xx = 0; xx < BLE_MAX_L2CAP_CLIENTS; xx++, p_rcb++)
    {
        if ((p_rcb->in_use) && (p_rcb->psm == psm))
            return (p_rcb);
    }

    /* If here, no match found */
    return (NULL);
}



/*******************************************************************************
**
** Function         l2cu_process_peer_cfg_req
**
** Description      This function is called when the peer sends us a "config request"
**                  message. It extracts the configuration of interest and saves
**                  it in the CCB.
**
**                  Note:  Negotiation of the FCR channel type is handled internally,
**                         all others are passed to the upper layer.
**
** Returns          UINT8 - L2CAP_PEER_CFG_OK if passed to upper layer,
**                          L2CAP_PEER_CFG_UNACCEPTABLE if automatically responded to
**                              because parameters are unnacceptable from a specification
**                              point of view.
**                          L2CAP_PEER_CFG_DISCONNECT if no compatible channel modes
**                              between the two devices, and shall be closed.
**
*******************************************************************************/
UINT8 l2cu_process_peer_cfg_req (tL2C_CCB *p_ccb, tL2CAP_CFG_INFO *p_cfg)
{
    BOOLEAN  mtu_ok      = TRUE;
    BOOLEAN  qos_type_ok = TRUE;
    BOOLEAN  flush_to_ok = TRUE;
    BOOLEAN  fcr_ok      = TRUE;
#if (CLASSIC_BT_INCLUDED == TRUE)
    UINT8    fcr_status;
#endif  ///CLASSIC_BT_INCLUDED == TRUE
    /* Ignore FCR parameters for basic mode */
    if (!p_cfg->fcr_present) {
        p_cfg->fcr.mode = L2CAP_FCR_BASIC_MODE;
    }

    /* Save the MTU that our peer can receive */
    if (p_cfg->mtu_present) {
        /* Make sure MTU is at least the minimum */
        if (p_cfg->mtu >= L2CAP_MIN_MTU) {
            /* In basic mode, limit the MTU to our buffer size */
            if ( (p_cfg->fcr_present == FALSE) && (p_cfg->mtu > L2CAP_MTU_SIZE) ) {
                p_cfg->mtu = L2CAP_MTU_SIZE;
            }

            /* Save the accepted value in case of renegotiation */
            p_ccb->peer_cfg.mtu = p_cfg->mtu;
            p_ccb->peer_cfg.mtu_present = TRUE;
            p_ccb->peer_cfg_bits |= L2CAP_CH_CFG_MASK_MTU;
        } else { /* Illegal MTU value */
            p_cfg->mtu = L2CAP_MIN_MTU;
            mtu_ok     = FALSE;
        }
    }
    /* Reload mtu from a previously accepted config request */
    else if (p_ccb->peer_cfg.mtu_present) {
        p_cfg->mtu_present = TRUE;
        p_cfg->mtu = p_ccb->peer_cfg.mtu;
    }

    /* Verify that the flush timeout is a valid value (0 is illegal) */
    if (p_cfg->flush_to_present) {
        if (!p_cfg->flush_to) {
            p_cfg->flush_to = 0xFFFF;   /* Infinite retransmissions (spec default) */
            flush_to_ok     = FALSE;
        } else { /* Save the accepted value in case of renegotiation */
            p_ccb->peer_cfg.flush_to_present = TRUE;
            p_ccb->peer_cfg.flush_to = p_cfg->flush_to;
            p_ccb->peer_cfg_bits |= L2CAP_CH_CFG_MASK_FLUSH_TO;
        }
    }
    /* Reload flush_to from a previously accepted config request */
    else if (p_ccb->peer_cfg.flush_to_present) {
        p_cfg->flush_to_present = TRUE;
        p_cfg->flush_to = p_ccb->peer_cfg.flush_to;
    }

    /* Save the QOS settings the the peer is using */
    if (p_cfg->qos_present) {
        /* Make sure service type is not a reserved value; otherwise let upper
           layer decide if acceptable
        */
        if (p_cfg->qos.service_type <= GUARANTEED) {
            p_ccb->peer_cfg.qos         = p_cfg->qos;
            p_ccb->peer_cfg.qos_present = TRUE;
            p_ccb->peer_cfg_bits |= L2CAP_CH_CFG_MASK_QOS;
        } else { /* Illegal service type value */
            p_cfg->qos.service_type = BEST_EFFORT;
            qos_type_ok             = FALSE;
        }
    }
    /* Reload QOS from a previously accepted config request */
    else if (p_ccb->peer_cfg.qos_present) {
        p_cfg->qos_present = TRUE;
        p_cfg->qos         = p_ccb->peer_cfg.qos;
    }
#if (CLASSIC_BT_INCLUDED == TRUE)
    if ((fcr_status = l2c_fcr_process_peer_cfg_req (p_ccb, p_cfg)) == L2CAP_PEER_CFG_DISCONNECT) {
        /* Notify caller to disconnect the channel (incompatible modes) */
        p_cfg->result = L2CAP_CFG_FAILED_NO_REASON;
        p_cfg->mtu_present = p_cfg->qos_present = p_cfg->flush_to_present = 0;

        return (L2CAP_PEER_CFG_DISCONNECT);
    }

    fcr_ok = (fcr_status == L2CAP_PEER_CFG_OK);
#endif  ///CLASSIC_BT_INCLUDED == TRUE

    /* Return any unacceptable parameters */
    if (mtu_ok && flush_to_ok && qos_type_ok && fcr_ok) {
        l2cu_adjust_out_mps (p_ccb);
        return (L2CAP_PEER_CFG_OK);
    } else {
        p_cfg->result = L2CAP_CFG_UNACCEPTABLE_PARAMS;

        if (mtu_ok) {
            p_cfg->mtu_present = FALSE;
        }
        if (flush_to_ok) {
            p_cfg->flush_to_present = FALSE;
        }
        if (qos_type_ok) {
            p_cfg->qos_present = FALSE;
        }
        if (fcr_ok) {
            p_cfg->fcr_present = FALSE;
        }

        return (L2CAP_PEER_CFG_UNACCEPTABLE);
    }
}


/*******************************************************************************
**
** Function         l2cu_process_peer_cfg_rsp
**
** Description      This function is called when the peer sends us a "config response"
**                  message. It extracts the configuration of interest and saves
**                  it in the CCB.
**
** Returns          void
**
*******************************************************************************/
void l2cu_process_peer_cfg_rsp (tL2C_CCB *p_ccb, tL2CAP_CFG_INFO *p_cfg)
{
    /* If we wanted QoS and the peer sends us a positive response with QoS, use his values */
    if ( (p_cfg->qos_present) && (p_ccb->our_cfg.qos_present) ) {
        p_ccb->our_cfg.qos = p_cfg->qos;
    }

    if (p_cfg->fcr_present) {
        /* Save the retransmission and monitor timeout values */
        if (p_cfg->fcr.mode == L2CAP_FCR_ERTM_MODE) {
            p_ccb->peer_cfg.fcr.rtrans_tout = p_cfg->fcr.rtrans_tout;
            p_ccb->peer_cfg.fcr.mon_tout = p_cfg->fcr.mon_tout;
        }

        /* Calculate the max number of packets for which we can delay sending an ack */
        if (p_cfg->fcr.tx_win_sz < p_ccb->our_cfg.fcr.tx_win_sz) {
            p_ccb->fcrb.max_held_acks = p_cfg->fcr.tx_win_sz / 3;
        } else {
            p_ccb->fcrb.max_held_acks = p_ccb->our_cfg.fcr.tx_win_sz / 3;
        }

        L2CAP_TRACE_DEBUG ("l2cu_process_peer_cfg_rsp(): peer tx_win_sz: %d, our tx_win_sz: %d, max_held_acks: %d",
                           p_cfg->fcr.tx_win_sz, p_ccb->our_cfg.fcr.tx_win_sz, p_ccb->fcrb.max_held_acks);
    }
}

/*******************************************************************************
**
** Function         l2cu_process_our_cfg_req
**
** Description      This function is called when we send a "config request"
**                  message. It extracts the configuration of interest and saves
**                  it in the CCB.
**
** Returns          void
**
*******************************************************************************/
void l2cu_process_our_cfg_req (tL2C_CCB *p_ccb, tL2CAP_CFG_INFO *p_cfg)
{
    tL2C_LCB    *p_lcb;
    UINT16      hci_flush_to;

    /* Save the QOS settings we are using for transmit */
    if (p_cfg->qos_present) {
        p_ccb->our_cfg.qos_present = TRUE;
        p_ccb->our_cfg.qos         = p_cfg->qos;
    }

    if (p_cfg->fcr_present) {
        /* Override FCR options if attempting streaming or basic */
        if (p_cfg->fcr.mode == L2CAP_FCR_BASIC_MODE) {
            memset(&p_cfg->fcr, 0, sizeof(tL2CAP_FCR_OPTS));
        } else {
            /* On BR/EDR, timer values are zero in config request */
            /* On class 2 AMP, timer value in config request shall be non-0 processing time */
            /*                 timer value in config response shall be greater than received processing time */
            p_cfg->fcr.mon_tout = p_cfg->fcr.rtrans_tout = 0;

            if (p_cfg->fcr.mode == L2CAP_FCR_STREAM_MODE) {
                p_cfg->fcr.max_transmit = p_cfg->fcr.tx_win_sz = 0;
            }
        }

        /* Set the threshold to send acks (may be updated in the cfg response) */
        p_ccb->fcrb.max_held_acks = p_cfg->fcr.tx_win_sz / 3;

        /* Include FCS option only if peer can handle it */
        if (p_ccb->p_lcb->peer_ext_fea & L2CAP_EXTFEA_NO_CRC) {
            /* FCS check can be bypassed if peer also desires to bypass */
            if (p_cfg->fcs_present && p_cfg->fcs == L2CAP_CFG_FCS_BYPASS) {
                p_ccb->bypass_fcs |= L2CAP_CFG_FCS_OUR;
            }
        } else {
            p_cfg->fcs_present = FALSE;
        }
    } else {
        p_cfg->fcr.mode = L2CAP_FCR_BASIC_MODE;
    }

    p_ccb->our_cfg.fcr.mode    = p_cfg->fcr.mode;
    p_ccb->our_cfg.fcr_present = p_cfg->fcr_present;

    /* Check the flush timeout. If it is lower than the current one used */
    /* then we need to adjust the flush timeout sent to the controller   */
    if (p_cfg->flush_to_present) {
        if ((p_cfg->flush_to == 0) || (p_cfg->flush_to == L2CAP_NO_AUTOMATIC_FLUSH)) {
            /* don't send invalid flush timeout */
            /* SPEC: The sender of the Request shall specify its flush timeout value */
            /*       if it differs from the default value of 0xFFFF                  */
            p_cfg->flush_to_present = FALSE;
        } else {
            p_ccb->our_cfg.flush_to = p_cfg->flush_to;
            p_lcb = p_ccb->p_lcb;

            if (p_cfg->flush_to < p_lcb->link_flush_tout) {
                p_lcb->link_flush_tout = p_cfg->flush_to;

                /* If the timeout is within range of HCI, set the flush timeout */
                if (p_cfg->flush_to <= ((HCI_MAX_AUTO_FLUSH_TOUT * 5) / 8)) {
                    /* Convert flush timeout to 0.625 ms units, with round */
                    hci_flush_to = ((p_cfg->flush_to * 8) + 3) / 5;
                    btsnd_hcic_write_auto_flush_tout (p_lcb->handle, hci_flush_to);
                }
            }
        }
    }
}


/*******************************************************************************
**
** Function         l2cu_process_our_cfg_rsp
**
** Description      This function is called when we send the peer a "config response"
**                  message. It extracts the configuration of interest and saves
**                  it in the CCB.
**
** Returns          void
**
*******************************************************************************/
#if (CLASSIC_BT_INCLUDED == TRUE)
void l2cu_process_our_cfg_rsp (tL2C_CCB *p_ccb, tL2CAP_CFG_INFO *p_cfg)
{
    /* If peer wants QoS, we are allowed to change the values in a positive response */
    if ( (p_cfg->qos_present) && (p_ccb->peer_cfg.qos_present) ) {
        p_ccb->peer_cfg.qos = p_cfg->qos;
    } else {
        p_cfg->qos_present = FALSE;
    }

    l2c_fcr_adj_our_rsp_options (p_ccb, p_cfg);
}
#endif  ///CLASSIC_BT_INCLUDED == TRUE


/*******************************************************************************
**
** Function         l2cu_device_reset
**
** Description      This function is called when reset of the device is
**                  completed.  For all active connection simulate HCI_DISC
**
** Returns          void
**
*******************************************************************************/
void l2cu_device_reset (void)
{
    int         xx;
    tL2C_LCB    *p_lcb = &l2cb.lcb_pool[0];

    for (xx = 0; xx < MAX_L2CAP_LINKS; xx++, p_lcb++) {
        if ((p_lcb->in_use) && (p_lcb->handle != HCI_INVALID_HANDLE)) {
            l2c_link_hci_disc_comp (p_lcb->handle, (UINT8) - 1);
        }
    }
#if (BLE_INCLUDED == TRUE)
    l2cb.is_ble_connecting = FALSE;
#endif
}

/*******************************************************************************
**
** Function         l2cu_create_conn
**
** Description      This function initiates an acl connection via HCI
**
** Returns          TRUE if successful, FALSE if gki get buffer fails.
**
*******************************************************************************/
BOOLEAN l2cu_create_conn (tL2C_LCB *p_lcb, tBT_TRANSPORT transport)
{
    int             xx;
    tL2C_LCB        *p_lcb_cur = &l2cb.lcb_pool[0];
#if BTM_SCO_INCLUDED == TRUE
    BOOLEAN         is_sco_active;
#endif

#if (BLE_INCLUDED == TRUE)
    tBT_DEVICE_TYPE     dev_type;
    tBLE_ADDR_TYPE      addr_type;


    BTM_ReadDevInfo(p_lcb->remote_bd_addr, &dev_type, &addr_type);

    if (transport == BT_TRANSPORT_LE) {
        if (!controller_get_interface()->supports_ble()) {
            return FALSE;
        }

        p_lcb->ble_addr_type = addr_type;
        p_lcb->transport = BT_TRANSPORT_LE;

        return (l2cble_create_conn(p_lcb));
    }
#endif

    /* If there is a connection where we perform as a slave, try to switch roles
       for this connection */
    for (xx = 0, p_lcb_cur = &l2cb.lcb_pool[0]; xx < MAX_L2CAP_LINKS; xx++, p_lcb_cur++) {
        if (p_lcb_cur == p_lcb) {
            continue;
        }

        if ((p_lcb_cur->in_use) && (p_lcb_cur->link_role == HCI_ROLE_SLAVE)) {

#if BTM_SCO_INCLUDED == TRUE
            /* The LMP_switch_req shall be sent only if the ACL logical transport
            is in active mode, when encryption is disabled, and all synchronous
            logical transports on the same physical link are disabled." */

            /* Check if there is any SCO Active on this BD Address */
            is_sco_active = btm_is_sco_active_by_bdaddr(p_lcb_cur->remote_bd_addr);

            L2CAP_TRACE_API ("l2cu_create_conn - btm_is_sco_active_by_bdaddr() is_sco_active = %s", \
                             (is_sco_active == TRUE) ? "TRUE" : "FALSE");

            if (is_sco_active == TRUE) {
                continue;    /* No Master Slave switch not allowed when SCO Active */
            }
#endif
            /*4_1_TODO check  if btm_cb.devcb.local_features to be used instead */
            if (HCI_SWITCH_SUPPORTED(BTM_ReadLocalFeatures())) {
                /* mark this lcb waiting for switch to be completed and
                   start switch on the other one */
                p_lcb->link_state = LST_CONNECTING_WAIT_SWITCH;
                p_lcb->link_role  = HCI_ROLE_MASTER;

                if (BTM_SwitchRole (p_lcb_cur->remote_bd_addr, HCI_ROLE_MASTER, NULL) == BTM_CMD_STARTED) {
                    btu_start_timer (&p_lcb->timer_entry, BTU_TTYPE_L2CAP_LINK, L2CAP_LINK_ROLE_SWITCH_TOUT);
                    return (TRUE);
                }
            }
        }
    }

    p_lcb->link_state = LST_CONNECTING;

    return (l2cu_create_conn_after_switch (p_lcb));
}

/*******************************************************************************
**
** Function         l2cu_get_num_hi_priority
**
** Description      Gets the number of high priority channels.
**
** Returns
**
*******************************************************************************/
UINT8 l2cu_get_num_hi_priority (void)
{
    UINT8       no_hi = 0;
    int         xx;
    tL2C_LCB    *p_lcb = &l2cb.lcb_pool[0];

    for (xx = 0; xx < MAX_L2CAP_LINKS; xx++, p_lcb++) {
        if ((p_lcb->in_use) && (p_lcb->acl_priority == L2CAP_PRIORITY_HIGH)) {
            no_hi++;
        }
    }
    return no_hi;
}


/*******************************************************************************
**
** Function         l2cu_create_conn_after_switch
**
** Description      This function initiates an acl connection via HCI
**                  If switch required to create connection it is already done.
**
** Returns          TRUE if successful, FALSE if gki get buffer fails.
**
*******************************************************************************/

BOOLEAN l2cu_create_conn_after_switch (tL2C_LCB *p_lcb)
{
    UINT8            allow_switch = HCI_CR_CONN_ALLOW_SWITCH;
    tBTM_INQ_INFO    *p_inq_info;
    UINT8            page_scan_rep_mode;
    UINT8            page_scan_mode;
    UINT16           clock_offset;
    UINT8            *p_features;
    UINT16           num_acl = BTM_GetNumAclLinks();
    tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev (p_lcb->remote_bd_addr);
    UINT8            no_hi_prio_chs = l2cu_get_num_hi_priority();

    p_features = BTM_ReadLocalFeatures();

    L2CAP_TRACE_DEBUG ("l2cu_create_conn_after_switch :%d num_acl:%d no_hi: %d is_bonding:%d",
                       l2cb.disallow_switch, num_acl, no_hi_prio_chs, p_lcb->is_bonding);
    /* FW team says that we can participant in 4 piconets
     * typically 3 piconet + 1 for scanning.
     * We can enhance the code to count the number of piconets later. */
    if ( ((!l2cb.disallow_switch && (num_acl < 3)) || (p_lcb->is_bonding && (no_hi_prio_chs == 0)))
            && HCI_SWITCH_SUPPORTED(p_features)) {
        allow_switch = HCI_CR_CONN_ALLOW_SWITCH;
    } else {
        allow_switch = HCI_CR_CONN_NOT_ALLOW_SWITCH;
    }

    p_lcb->link_state = LST_CONNECTING;

    /* Check with the BT manager if details about remote device are known */
    if ((p_inq_info = BTM_InqDbRead(p_lcb->remote_bd_addr)) != NULL) {
        page_scan_rep_mode = p_inq_info->results.page_scan_rep_mode;
        page_scan_mode = p_inq_info->results.page_scan_mode;
        clock_offset = (UINT16)(p_inq_info->results.clock_offset);
    } else {
        /* No info known. Use default settings */
        page_scan_rep_mode = HCI_PAGE_SCAN_REP_MODE_R1;
        page_scan_mode = HCI_MANDATARY_PAGE_SCAN_MODE;

        clock_offset = (p_dev_rec) ? p_dev_rec->clock_offset : 0;
    }

    if (!btsnd_hcic_create_conn (p_lcb->remote_bd_addr,
                                 ( HCI_PKT_TYPES_MASK_DM1 | HCI_PKT_TYPES_MASK_DH1
                                   | HCI_PKT_TYPES_MASK_DM3 | HCI_PKT_TYPES_MASK_DH3
                                   | HCI_PKT_TYPES_MASK_DM5 | HCI_PKT_TYPES_MASK_DH5 ),
                                 page_scan_rep_mode,
                                 page_scan_mode,
                                 clock_offset,
                                 allow_switch))

    {
        L2CAP_TRACE_ERROR ("L2CAP - no buffer for l2cu_create_conn");
        l2cu_release_lcb (p_lcb);
        return (FALSE);
    }

    btm_acl_update_busy_level (BTM_BLI_PAGE_EVT);

    btu_start_timer (&p_lcb->timer_entry, BTU_TTYPE_L2CAP_LINK,
                     L2CAP_LINK_CONNECT_TOUT);

    return (TRUE);
}


/*******************************************************************************
**
** Function         l2cu_find_lcb_by_state
**
** Description      Look through all active LCBs for a match based on the
**                  LCB state.
**
** Returns          pointer to first matched LCB, or NULL if no match
**
*******************************************************************************/
tL2C_LCB *l2cu_find_lcb_by_state (tL2C_LINK_STATE state)
{
    UINT16      i;
    tL2C_LCB    *p_lcb = &l2cb.lcb_pool[0];

    for (i = 0; i < MAX_L2CAP_LINKS; i++, p_lcb++) {
        if ((p_lcb->in_use) && (p_lcb->link_state == state)) {
            return (p_lcb);
        }
    }

    /* If here, no match found */
    return (NULL);
}


/*******************************************************************************
**
** Function         l2cu_lcb_disconnecting
**
** Description      On each active lcb, check if the lcb is in disconnecting
**                  state, or if there are no ccb's on the lcb (implying
                    idle timeout is running), or if last ccb on the link
                    is in disconnecting state.
**
** Returns          TRUE if any of above conditions met, FALSE otherwise
**
*******************************************************************************/
BOOLEAN l2cu_lcb_disconnecting (void)
{
    tL2C_LCB    *p_lcb;
    tL2C_CCB    *p_ccb;
    UINT16      i;
    BOOLEAN     status = FALSE;

    p_lcb = &l2cb.lcb_pool[0];

    for (i = 0; i < MAX_L2CAP_LINKS; i++, p_lcb++) {
        if (p_lcb->in_use) {
            /* no ccbs on lcb, or lcb is in disconnecting state */
            if ((!p_lcb->ccb_queue.p_first_ccb) || (p_lcb->link_state == LST_DISCONNECTING)) {
                status = TRUE;
                break;
            }
            /* only one ccb left on lcb */
            else if (p_lcb->ccb_queue.p_first_ccb == p_lcb->ccb_queue.p_last_ccb) {
                p_ccb = p_lcb->ccb_queue.p_first_ccb;

                if ((p_ccb->in_use) &&
                        ((p_ccb->chnl_state == CST_W4_L2CAP_DISCONNECT_RSP) ||
                         (p_ccb->chnl_state == CST_W4_L2CA_DISCONNECT_RSP))) {
                    status = TRUE;
                    break;
                }
            }
        }
    }
    return status;
}


/*******************************************************************************
**
** Function         l2cu_set_acl_priority
**
** Description      Sets the transmission priority for a channel.
**                  (For initial implementation only two values are valid.
**                  L2CAP_PRIORITY_NORMAL and L2CAP_PRIORITY_HIGH).
**
** Returns          TRUE if a valid channel, else FALSE
**
*******************************************************************************/

BOOLEAN l2cu_set_acl_priority (BD_ADDR bd_addr, UINT8 priority, BOOLEAN reset_after_rs)
{
    tL2C_LCB            *p_lcb;
    UINT8               *pp;
    UINT8                command[HCI_BRCM_ACL_PRIORITY_PARAM_SIZE];
    UINT8                vs_param;

    //APPL_TRACE_EVENT("SET ACL PRIORITY %d", priority);

    /* Find the link control block for the acl channel */
    if ((p_lcb = l2cu_find_lcb_by_bd_addr(bd_addr, BT_TRANSPORT_BR_EDR)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no LCB for L2CA_SetAclPriority");
        return (FALSE);
    }

    if (BTM_IS_BRCM_CONTROLLER()) {
        /* Called from above L2CAP through API; send VSC if changed */
        if ((!reset_after_rs && (priority != p_lcb->acl_priority)) ||
                /* Called because of a master/slave role switch; if high resend VSC */
                ( reset_after_rs && p_lcb->acl_priority == L2CAP_PRIORITY_HIGH)) {
            pp = command;

            vs_param = (priority == L2CAP_PRIORITY_HIGH) ? HCI_BRCM_ACL_PRIORITY_HIGH : HCI_BRCM_ACL_PRIORITY_LOW;

            UINT16_TO_STREAM (pp, p_lcb->handle);
            UINT8_TO_STREAM  (pp, vs_param);

            BTM_VendorSpecificCommand (HCI_BRCM_SET_ACL_PRIORITY, HCI_BRCM_ACL_PRIORITY_PARAM_SIZE, command, NULL);

            /* Adjust lmp buffer allocation for this channel if priority changed */
            if (p_lcb->acl_priority != priority) {
                p_lcb->acl_priority = priority;
                l2c_link_adjust_allocation();
            }
        }
    }
    return (TRUE);
}

#if (L2CAP_NON_FLUSHABLE_PB_INCLUDED == TRUE)
/******************************************************************************
**
** Function         l2cu_set_non_flushable_pbf
**
** Description      set L2CAP_PKT_START_NON_FLUSHABLE if controller supoorts
**
** Returns          void
**
*******************************************************************************/
void l2cu_set_non_flushable_pbf (BOOLEAN is_supported)
{
    if (is_supported) {
        l2cb.non_flushable_pbf = (L2CAP_PKT_START_NON_FLUSHABLE << L2CAP_PKT_TYPE_SHIFT);
    } else {
        l2cb.non_flushable_pbf = (L2CAP_PKT_START << L2CAP_PKT_TYPE_SHIFT);
    }
}
#endif

/*******************************************************************************
**
** Function         l2cu_resubmit_pending_sec_req
**
** Description      This function is called when required security procedures
**                  are completed and any pending requests can be re-submitted.
**
** Returns          void
**
*******************************************************************************/
#if (CLASSIC_BT_INCLUDED == TRUE)
void l2cu_resubmit_pending_sec_req (BD_ADDR p_bda)
{
    tL2C_LCB        *p_lcb;
    tL2C_CCB        *p_ccb;
    tL2C_CCB        *p_next_ccb;
    int             xx;
    L2CAP_TRACE_DEBUG ("l2cu_resubmit_pending_sec_req  p_bda: %p", p_bda);

    /* If we are called with a BDA, only resubmit for that BDA */
    if (p_bda) {
        p_lcb = l2cu_find_lcb_by_bd_addr (p_bda, BT_TRANSPORT_BR_EDR);
        /* If we don't have one, this is an error */
        if (p_lcb) {
            /* For all channels, send the event through their FSMs */
            for (p_ccb = p_lcb->ccb_queue.p_first_ccb; p_ccb; p_ccb = p_next_ccb) {
                p_next_ccb = p_ccb->p_next_ccb;
                l2c_csm_execute (p_ccb, L2CEVT_SEC_RE_SEND_CMD, NULL);
            }
        } else {
            L2CAP_TRACE_WARNING ("l2cu_resubmit_pending_sec_req - unknown BD_ADDR");
        }
    } else {
        /* No BDA pasesed in, so check all links */
        for (xx = 0, p_lcb = &l2cb.lcb_pool[0]; xx < MAX_L2CAP_LINKS; xx++, p_lcb++) {
            if (p_lcb->in_use) {
                /* For all channels, send the event through their FSMs */
                for (p_ccb = p_lcb->ccb_queue.p_first_ccb; p_ccb; p_ccb = p_next_ccb) {
                    p_next_ccb = p_ccb->p_next_ccb;
                    l2c_csm_execute (p_ccb, L2CEVT_SEC_RE_SEND_CMD, NULL);
                }
            }
        }       
    }
}
#endif  ///CLASSIC_BT_INCLUDED == TRUE 

#if L2CAP_CONFORMANCE_TESTING == TRUE
/*******************************************************************************
**
** Function         l2cu_set_info_rsp_mask
**
** Description      This function allows the script wrapper to change the
**                  info resp mask for conformance testing.
**
** Returns          pointer to CCB, or NULL if none
**
*******************************************************************************/
void l2cu_set_info_rsp_mask (UINT32 mask)
{
    l2cb.test_info_resp = mask;
}
#endif  /* L2CAP_CONFORMANCE_TESTING */

/*******************************************************************************
**
** Function         l2cu_adjust_out_mps
**
** Description      Sets our MPS based on current controller capabilities
**
** Returns          void
**
*******************************************************************************/
void l2cu_adjust_out_mps (tL2C_CCB *p_ccb)
{
    UINT16 packet_size;

    /* on the tx side MTU is selected based on packet size of the controller */
    packet_size = btm_get_max_packet_size (p_ccb->p_lcb->remote_bd_addr);

    if (packet_size <= (L2CAP_PKT_OVERHEAD + L2CAP_FCR_OVERHEAD + L2CAP_SDU_LEN_OVERHEAD + L2CAP_FCS_LEN)) {
        /* something is very wrong */
        L2CAP_TRACE_ERROR ("l2cu_adjust_out_mps bad packet size: %u  will use MPS: %u", packet_size, p_ccb->peer_cfg.fcr.mps);
        p_ccb->tx_mps = p_ccb->peer_cfg.fcr.mps;
    } else {
        packet_size -= (L2CAP_PKT_OVERHEAD + L2CAP_FCR_OVERHEAD + L2CAP_SDU_LEN_OVERHEAD + L2CAP_FCS_LEN);

        /* We try to negotiate MTU that each packet can be split into whole
        number of max packets.  For example if link is 1.2 max packet size is 339 bytes.
        At first calculate how many whole packets it is.  MAX L2CAP is 1691 + 4 overhead.
        1695, that will be 5 Dh5 packets.  Now maximum L2CAP packet is
        5 * 339 = 1695. Minus 4 bytes L2CAP header 1691.

        For EDR 2.0 packet size is 1027.  So we better send RFCOMM packet as 1 3DH5 packet
        1 * 1027 = 1027.  Minus 4 bytes L2CAP header 1023.  */
        if (p_ccb->peer_cfg.fcr.mps >= packet_size) {
            p_ccb->tx_mps = p_ccb->peer_cfg.fcr.mps / packet_size * packet_size;
        } else {
            p_ccb->tx_mps = p_ccb->peer_cfg.fcr.mps;
        }

        L2CAP_TRACE_DEBUG ("l2cu_adjust_out_mps use %d   Based on peer_cfg.fcr.mps: %u  packet_size: %u",
                           p_ccb->tx_mps, p_ccb->peer_cfg.fcr.mps, packet_size);
    }
}


/*******************************************************************************
**
** Function         l2cu_initialize_fixed_ccb
**
** Description      Initialize a fixed channel's CCB
**
** Returns          TRUE or FALSE
**
*******************************************************************************/
BOOLEAN l2cu_initialize_fixed_ccb (tL2C_LCB *p_lcb, UINT16 fixed_cid, tL2CAP_FCR_OPTS *p_fcr)
{
#if (L2CAP_NUM_FIXED_CHNLS > 0)
    tL2C_CCB    *p_ccb;
    /* If we already have a CCB, then simply return */
    if (p_lcb->p_fixed_ccbs[fixed_cid - L2CAP_FIRST_FIXED_CHNL] != NULL) {
        return (TRUE);
    }

    if ((p_ccb = l2cu_allocate_ccb (NULL, 0)) == NULL) {
        return (FALSE);
    }

    btu_stop_timer(&p_lcb->timer_entry);

    /* Set CID for the connection */
    p_ccb->local_cid  = fixed_cid;
    p_ccb->remote_cid = fixed_cid;

    p_ccb->is_flushable = FALSE;

    p_ccb->timer_entry.param  = (TIMER_PARAM_TYPE)p_ccb;


    if (p_fcr) {
        /* Set the FCR parameters. For now, we will use default pools */
        p_ccb->our_cfg.fcr = p_ccb->peer_cfg.fcr = *p_fcr;

        p_ccb->ertm_info.fcr_rx_buf_size  = L2CAP_FCR_RX_BUF_SIZE;
        p_ccb->ertm_info.fcr_tx_buf_size  = L2CAP_FCR_TX_BUF_SIZE;
        p_ccb->ertm_info.user_rx_buf_size = L2CAP_USER_RX_BUF_SIZE;
        p_ccb->ertm_info.user_tx_buf_size = L2CAP_USER_TX_BUF_SIZE;

        p_ccb->fcrb.max_held_acks = p_fcr->tx_win_sz / 3;
    }

    /* Link ccb to lcb and lcb to ccb */
    p_lcb->p_fixed_ccbs[fixed_cid - L2CAP_FIRST_FIXED_CHNL] = p_ccb;
    p_ccb->p_lcb = p_lcb;

    /* There is no configuration, so if the link is up, the channel is up */
    if (p_lcb->link_state == LST_CONNECTED) {
        p_ccb->chnl_state = CST_OPEN;
    }

    /* Set the default idle timeout value to use */
    p_ccb->fixed_chnl_idle_tout = l2cb.fixed_reg[fixed_cid - L2CAP_FIRST_FIXED_CHNL].default_idle_tout;
#endif
    return (TRUE);
}

/*******************************************************************************
**
** Function         l2cu_no_dynamic_ccbs
**
** Description      Handles the case when there are no more dynamic CCBs. If there
**                  are any fixed CCBs, start the longest of the fixed CCB timeouts,
**                  otherwise start the default link idle timeout or disconnect.
**
** Returns          void
**
*******************************************************************************/
void l2cu_no_dynamic_ccbs (tL2C_LCB *p_lcb)
{
#if (SMP_INCLUDED == TRUE)
    tBTM_STATUS     rc;
#endif  ///SMP_INCLUDED == TRUE
    UINT16          timeout = p_lcb->idle_timeout;

#if (L2CAP_NUM_FIXED_CHNLS > 0)
    int         xx;

    for (xx = 0; xx < L2CAP_NUM_FIXED_CHNLS; xx++) {
        if ( (p_lcb->p_fixed_ccbs[xx] != NULL) && (p_lcb->p_fixed_ccbs[xx]->fixed_chnl_idle_tout > timeout) ) {
            timeout = p_lcb->p_fixed_ccbs[xx]->fixed_chnl_idle_tout;
        }
    }
#endif

    /* If the link is pairing, do not mess with the timeouts */
    if (p_lcb->is_bonding) {
        return;
    }

    if (timeout == 0) {
        L2CAP_TRACE_DEBUG ("l2cu_no_dynamic_ccbs() IDLE timer 0, disconnecting link");
#if (SMP_INCLUDED == TRUE)
        rc = btm_sec_disconnect (p_lcb->handle, HCI_ERR_PEER_USER);
        if (rc == BTM_CMD_STARTED) {
            l2cu_process_fixed_disc_cback(p_lcb);
            p_lcb->link_state = LST_DISCONNECTING;
            timeout = L2CAP_LINK_DISCONNECT_TOUT;
        } else if (rc == BTM_SUCCESS) {
            l2cu_process_fixed_disc_cback(p_lcb);
            /* BTM SEC will make sure that link is release (probably after pairing is done) */
            p_lcb->link_state = LST_DISCONNECTING;
            timeout = 0xFFFF;
        } else if ( (p_lcb->is_bonding)
                    &&   (btsnd_hcic_disconnect (p_lcb->handle, HCI_ERR_PEER_USER)) ) {
            l2cu_process_fixed_disc_cback(p_lcb);
            p_lcb->link_state = LST_DISCONNECTING;
            timeout = L2CAP_LINK_DISCONNECT_TOUT;
        } else {
            /* probably no buffer to send disconnect */
            timeout = BT_1SEC_TIMEOUT;
        }
#else
        if (btsnd_hcic_disconnect (p_lcb->handle, HCI_ERR_PEER_USER)) {
            l2cu_process_fixed_disc_cback(p_lcb);
            p_lcb->link_state = LST_DISCONNECTING;
            timeout = L2CAP_LINK_DISCONNECT_TOUT;
        } else {
            timeout = BT_1SEC_TIMEOUT;
        }
#endif  ///SMP_INCLUDED == TRUE

    }

    if (timeout != 0xFFFF) {
        L2CAP_TRACE_DEBUG ("l2cu_no_dynamic_ccbs() starting IDLE timeout: %d", timeout);
        btu_start_timer (&p_lcb->timer_entry, BTU_TTYPE_L2CAP_LINK, timeout);
    } else {
        btu_stop_timer(&p_lcb->timer_entry);
    }
}

#if (L2CAP_NUM_FIXED_CHNLS > 0)
/*******************************************************************************
**
** Function         l2cu_process_fixed_chnl_resp
**
** Description      handle a fixed channel response (or lack thereof)
**                  if the link failed, or a fixed channel response was
**                  not received, the bitfield is all zeros.
**
*******************************************************************************/
void l2cu_process_fixed_chnl_resp (tL2C_LCB *p_lcb)
{
    L2CAP_TRACE_DEBUG("%s",__func__);
#if (BLE_INCLUDED == TRUE)
    if (p_lcb->transport == BT_TRANSPORT_BR_EDR) {
        /* ignore all not assigned BR/EDR channels */
        p_lcb->peer_chnl_mask[0] &= (L2CAP_FIXED_CHNL_SIG_BIT | \
                                     L2CAP_FIXED_CHNL_CNCTLESS_BIT | \
                                     L2CAP_FIXED_CHNL_SMP_BR_BIT);
    } else {
        p_lcb->peer_chnl_mask[0] = l2cb.l2c_ble_fixed_chnls_mask;
    }
#endif

    /* Tell all registered fixed channels about the connection */
    for (int xx = 0; xx < L2CAP_NUM_FIXED_CHNLS; xx++) {
#if BLE_INCLUDED == TRUE
        /* skip sending LE fix channel callbacks on BR/EDR links */
        if (p_lcb->transport == BT_TRANSPORT_BR_EDR &&
                xx + L2CAP_FIRST_FIXED_CHNL >= L2CAP_ATT_CID &&
                xx + L2CAP_FIRST_FIXED_CHNL <= L2CAP_SMP_CID) {
            continue;
        }
#endif
        if (l2cb.fixed_reg[xx].pL2CA_FixedConn_Cb != NULL) {
            if (p_lcb->peer_chnl_mask[(xx + L2CAP_FIRST_FIXED_CHNL) / 8]
                    & (1 << ((xx + L2CAP_FIRST_FIXED_CHNL) % 8))) {
                if (p_lcb->p_fixed_ccbs[xx]) {
                    p_lcb->p_fixed_ccbs[xx]->chnl_state = CST_OPEN;
                }
#if BLE_INCLUDED == TRUE
                (*l2cb.fixed_reg[xx].pL2CA_FixedConn_Cb)(xx + L2CAP_FIRST_FIXED_CHNL,
                        p_lcb->remote_bd_addr, TRUE, 0, p_lcb->transport);
#else
                (*l2cb.fixed_reg[xx].pL2CA_FixedConn_Cb)(xx + L2CAP_FIRST_FIXED_CHNL,
                        p_lcb->remote_bd_addr, TRUE, 0, BT_TRANSPORT_BR_EDR);
#endif
            } else {
#if BLE_INCLUDED == TRUE
                (*l2cb.fixed_reg[xx].pL2CA_FixedConn_Cb)(xx + L2CAP_FIRST_FIXED_CHNL,
                        p_lcb->remote_bd_addr, FALSE, p_lcb->disc_reason, p_lcb->transport);
#else
                (*l2cb.fixed_reg[xx].pL2CA_FixedConn_Cb)(xx + L2CAP_FIRST_FIXED_CHNL,
                        p_lcb->remote_bd_addr, FALSE, p_lcb->disc_reason, BT_TRANSPORT_BR_EDR);
#endif

                if (p_lcb->p_fixed_ccbs[xx]) {
                    l2cu_release_ccb (p_lcb->p_fixed_ccbs[xx]);
                    p_lcb->p_fixed_ccbs[xx] = NULL;
                }
            }
        }
    }
}
#endif


/*******************************************************************************
**
** Function         l2cu_process_fixed_disc_cback
**
** Description      send l2cap fixed channel disconnection callback to application
**
**
** Returns          void
**
*******************************************************************************/
void l2cu_process_fixed_disc_cback (tL2C_LCB *p_lcb)
{
#if (L2CAP_NUM_FIXED_CHNLS > 0)

    /* Select peer channels mask to use depending on transport */
    UINT8 peer_channel_mask = p_lcb->peer_chnl_mask[0];

    // For LE, reset the stored peer channel mask
    if (p_lcb->transport == BT_TRANSPORT_LE) {
        p_lcb->peer_chnl_mask[0] = 0;
    }

    for (int xx = 0; xx < L2CAP_NUM_FIXED_CHNLS; xx++) {
        if (p_lcb->p_fixed_ccbs[xx]) {
            if (p_lcb->p_fixed_ccbs[xx] != p_lcb->p_pending_ccb) {
                tL2C_CCB *p_l2c_chnl_ctrl_block;
                p_l2c_chnl_ctrl_block = p_lcb->p_fixed_ccbs[xx];
                p_lcb->p_fixed_ccbs[xx] = NULL;
                l2cu_release_ccb(p_l2c_chnl_ctrl_block);
#if BLE_INCLUDED == TRUE
                (*l2cb.fixed_reg[xx].pL2CA_FixedConn_Cb)(xx + L2CAP_FIRST_FIXED_CHNL,
                        p_lcb->remote_bd_addr, FALSE, p_lcb->disc_reason, p_lcb->transport);
#else
                (*l2cb.fixed_reg[xx].pL2CA_FixedConn_Cb)(xx + L2CAP_FIRST_FIXED_CHNL,
                        p_lcb->remote_bd_addr, FALSE, p_lcb->disc_reason, BT_TRANSPORT_BR_EDR);
#endif
            }
        } else if ( (peer_channel_mask & (1 << (xx + L2CAP_FIRST_FIXED_CHNL)))
                    && (l2cb.fixed_reg[xx].pL2CA_FixedConn_Cb != NULL) )
#if BLE_INCLUDED == TRUE
            (*l2cb.fixed_reg[xx].pL2CA_FixedConn_Cb)(xx + L2CAP_FIRST_FIXED_CHNL,
                    p_lcb->remote_bd_addr, FALSE, p_lcb->disc_reason, p_lcb->transport);
#else
            (*l2cb.fixed_reg[xx].pL2CA_FixedConn_Cb)(xx + L2CAP_FIRST_FIXED_CHNL,
                    p_lcb->remote_bd_addr, FALSE, p_lcb->disc_reason, BT_TRANSPORT_BR_EDR);
#endif
    }
#endif
}

#if (BLE_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         l2cu_send_peer_ble_par_req
**
** Description      Build and send a BLE parameter update request message
**                  to the peer.
**
** Returns          void
**
*******************************************************************************/
void l2cu_send_peer_ble_par_req (tL2C_LCB *p_lcb, UINT16 min_int, UINT16 max_int,
                                 UINT16 latency, UINT16 timeout)
{
    BT_HDR  *p_buf;
    UINT8   *p;

    /* Create an identifier for this packet */
    p_lcb->id++;
    l2cu_adj_id (p_lcb, L2CAP_ADJ_ID);

    if ((p_buf = l2cu_build_header (p_lcb, L2CAP_CMD_BLE_UPD_REQ_LEN,
                                    L2CAP_CMD_BLE_UPDATE_REQ, p_lcb->id)) == NULL ) {
        L2CAP_TRACE_WARNING ("l2cu_send_peer_ble_par_req - no buffer");
        return;
    }

    p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + HCI_DATA_PREAMBLE_SIZE +
        L2CAP_PKT_OVERHEAD + L2CAP_CMD_OVERHEAD;

    UINT16_TO_STREAM (p, min_int);
    UINT16_TO_STREAM (p, max_int);
    UINT16_TO_STREAM (p, latency);
    UINT16_TO_STREAM (p, timeout);

    l2c_link_check_send_pkts (p_lcb, NULL, p_buf);
}

/*******************************************************************************
**
** Function         l2cu_send_peer_ble_par_rsp
**
** Description      Build and send a BLE parameter update response message
**                  to the peer.
**
** Returns          void
**
*******************************************************************************/
void l2cu_send_peer_ble_par_rsp (tL2C_LCB *p_lcb, UINT16 reason, UINT8 rem_id)
{
    BT_HDR  *p_buf;
    UINT8   *p;

    if ((p_buf = l2cu_build_header (p_lcb, L2CAP_CMD_BLE_UPD_RSP_LEN,
                                    L2CAP_CMD_BLE_UPDATE_RSP, rem_id)) == NULL ) {
        L2CAP_TRACE_WARNING ("l2cu_send_peer_ble_par_rsp - no buffer");
        return;
    }

    p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + HCI_DATA_PREAMBLE_SIZE +
        L2CAP_PKT_OVERHEAD + L2CAP_CMD_OVERHEAD;

    UINT16_TO_STREAM (p, reason);

    l2c_link_check_send_pkts (p_lcb, NULL, p_buf);
}

/*******************************************************************************
**
** Function         l2cu_send_peer_ble_credit_based_conn_req
**
** Description      Build and send a BLE packet to establish LE connection oriented
**                  L2CAP channel.
**
** Returns          void
**
*******************************************************************************/
void l2cu_send_peer_ble_credit_based_conn_req (tL2C_CCB *p_ccb)
{
    BT_HDR  *p_buf;
    UINT8   *p;
    tL2C_LCB *p_lcb = NULL;
    UINT16 mtu;
    UINT16 mps;
    UINT16 initial_credit;

    if (!p_ccb)
        return;
    p_lcb = p_ccb->p_lcb;

    /* Create an identifier for this packet */
    p_ccb->p_lcb->id++;
    l2cu_adj_id(p_ccb->p_lcb, L2CAP_ADJ_ID);

    p_ccb->local_id = p_ccb->p_lcb->id;

    if ((p_buf = l2cu_build_header (p_lcb, L2CAP_CMD_BLE_CREDIT_BASED_CONN_REQ_LEN,
                    L2CAP_CMD_BLE_CREDIT_BASED_CONN_REQ, p_lcb->id)) == NULL )
    {
        L2CAP_TRACE_WARNING ("l2cu_send_peer_ble_credit_based_conn_req - no buffer");
        return;
    }

    p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + HCI_DATA_PREAMBLE_SIZE +
                               L2CAP_PKT_OVERHEAD + L2CAP_CMD_OVERHEAD;

    mtu = p_ccb->local_conn_cfg.mtu;
    mps = p_ccb->local_conn_cfg.mps;
    initial_credit = p_ccb->local_conn_cfg.credits;

    L2CAP_TRACE_DEBUG ("l2cu_send_peer_ble_credit_based_conn_req PSM:0x%04x local_cid:%d\
                mtu:%d mps:%d initial_credit:%d", p_ccb->p_rcb->real_psm,\
                p_ccb->local_cid, mtu, mps, initial_credit);

    UINT16_TO_STREAM (p, p_ccb->p_rcb->real_psm);
    UINT16_TO_STREAM (p, p_ccb->local_cid);
    UINT16_TO_STREAM (p, mtu);
    UINT16_TO_STREAM (p, mps);
    UINT16_TO_STREAM (p, initial_credit);

    l2c_link_check_send_pkts (p_lcb, NULL, p_buf);
}

/*******************************************************************************
**
** Function         l2cu_reject_ble_connection
**
** Description      Build and send an L2CAP "Credit based connection res" message
**                  to the peer. This function is called for non-success cases.
**
** Returns          void
**
*******************************************************************************/
void l2cu_reject_ble_connection (tL2C_LCB *p_lcb, UINT8 rem_id, UINT16 result)
{
    BT_HDR  *p_buf;
    UINT8   *p;

    if ((p_buf = l2cu_build_header(p_lcb, L2CAP_CMD_BLE_CREDIT_BASED_CONN_RES_LEN,
                    L2CAP_CMD_BLE_CREDIT_BASED_CONN_RES, rem_id)) == NULL )
    {
        L2CAP_TRACE_WARNING ("l2cu_reject_ble_connection - no buffer");
        return;
    }

    p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + HCI_DATA_PREAMBLE_SIZE +
                               L2CAP_PKT_OVERHEAD + L2CAP_CMD_OVERHEAD;

    UINT16_TO_STREAM (p, 0);                    /* Local CID of 0   */
    UINT16_TO_STREAM (p, 0);                    /* MTU */
    UINT16_TO_STREAM (p, 0);                    /* MPS */
    UINT16_TO_STREAM (p, 0);                    /* initial credit */
    UINT16_TO_STREAM (p, result);

    l2c_link_check_send_pkts (p_lcb, NULL, p_buf);
}

/*******************************************************************************
**
** Function         l2cu_send_peer_ble_credit_based_conn_res
**
** Description      Build and send an L2CAP "Credit based connection res" message
**                  to the peer. This function is called in case of success.
**
** Returns          void
**
*******************************************************************************/
void l2cu_send_peer_ble_credit_based_conn_res (tL2C_CCB *p_ccb, UINT16 result)
{
    BT_HDR  *p_buf;
    UINT8   *p;

    L2CAP_TRACE_DEBUG ("l2cu_send_peer_ble_credit_based_conn_res");
    if ((p_buf = l2cu_build_header(p_ccb->p_lcb, L2CAP_CMD_BLE_CREDIT_BASED_CONN_RES_LEN,
                    L2CAP_CMD_BLE_CREDIT_BASED_CONN_RES, p_ccb->remote_id)) == NULL )
    {
        L2CAP_TRACE_WARNING ("l2cu_send_peer_ble_credit_based_conn_res - no buffer");
        return;
    }

    p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + HCI_DATA_PREAMBLE_SIZE +
                               L2CAP_PKT_OVERHEAD + L2CAP_CMD_OVERHEAD;

    UINT16_TO_STREAM (p, p_ccb->local_cid);                      /* Local CID */
    UINT16_TO_STREAM (p, p_ccb->local_conn_cfg.mtu);             /* MTU */
    UINT16_TO_STREAM (p, p_ccb->local_conn_cfg.mps);             /* MPS */
    UINT16_TO_STREAM (p, p_ccb->local_conn_cfg.credits);         /* initial credit */
    UINT16_TO_STREAM (p, result);

    l2c_link_check_send_pkts (p_ccb->p_lcb, NULL, p_buf);
}

/*******************************************************************************
**
** Function         l2cu_send_peer_ble_flow_control_credit
**
** Description      Build and send a BLE packet to give credits to peer device
**                  for LE connection oriented L2CAP channel.
**
** Returns          void
**
*******************************************************************************/
void l2cu_send_peer_ble_flow_control_credit(tL2C_CCB *p_ccb, UINT16 credit_value)
{
    BT_HDR  *p_buf;
    UINT8   *p;
    tL2C_LCB *p_lcb = NULL;

    if (!p_ccb)
        return;
    p_lcb = p_ccb->p_lcb;

    /* Create an identifier for this packet */
    p_ccb->p_lcb->id++;
    l2cu_adj_id(p_ccb->p_lcb, L2CAP_ADJ_ID);

    p_ccb->local_id = p_ccb->p_lcb->id;

    if ((p_buf = l2cu_build_header (p_lcb, L2CAP_CMD_BLE_FLOW_CTRL_CREDIT_LEN,
                    L2CAP_CMD_BLE_FLOW_CTRL_CREDIT, p_lcb->id)) == NULL )
    {
        L2CAP_TRACE_WARNING ("l2cu_send_peer_ble_credit_based_conn_req - no buffer");
        return;
    }

    p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + HCI_DATA_PREAMBLE_SIZE +
                               L2CAP_PKT_OVERHEAD + L2CAP_CMD_OVERHEAD;

    UINT16_TO_STREAM (p, p_ccb->local_cid);
    UINT16_TO_STREAM (p, credit_value);

    l2c_link_check_send_pkts (p_lcb, NULL, p_buf);
}

/*******************************************************************************
**
** Function         l2cu_send_peer_ble_credit_based_conn_req
**
** Description      Build and send a BLE packet to disconnect LE connection oriented
**                  L2CAP channel.
**
** Returns          void
**
*******************************************************************************/
void l2cu_send_peer_ble_credit_based_disconn_req(tL2C_CCB *p_ccb)
{
    BT_HDR  *p_buf;
    UINT8   *p;
    tL2C_LCB *p_lcb = NULL;
    L2CAP_TRACE_DEBUG ("%s",__func__);

    if (!p_ccb)
        return;
    p_lcb = p_ccb->p_lcb;

    /* Create an identifier for this packet */
    p_ccb->p_lcb->id++;
    l2cu_adj_id(p_ccb->p_lcb, L2CAP_ADJ_ID);

    p_ccb->local_id = p_ccb->p_lcb->id;
     if ((p_buf = l2cu_build_header (p_lcb, L2CAP_DISC_REQ_LEN,
                    L2CAP_CMD_DISC_REQ, p_lcb->id)) == NULL )
    {
        L2CAP_TRACE_WARNING ("l2cu_send_peer_ble_credit_based_disconn_req - no buffer");
        return;
    }

    p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + HCI_DATA_PREAMBLE_SIZE +
                               L2CAP_PKT_OVERHEAD + L2CAP_CMD_OVERHEAD;

    UINT16_TO_STREAM (p, p_ccb->remote_cid);
    UINT16_TO_STREAM (p,p_ccb->local_cid);

    l2c_link_check_send_pkts (p_lcb, NULL, p_buf);
}

#endif /* BLE_INCLUDED == TRUE */


/*******************************************************************************
** Functions used by both Full and Light Stack
********************************************************************************/

/*******************************************************************************
**
** Function         l2cu_find_lcb_by_handle
**
** Description      Look through all active LCBs for a match based on the
**                  HCI handle.
**
** Returns          pointer to matched LCB, or NULL if no match
**
*******************************************************************************/
tL2C_LCB  *l2cu_find_lcb_by_handle (UINT16 handle)
{
    int         xx;
    tL2C_LCB    *p_lcb = &l2cb.lcb_pool[0];

    for (xx = 0; xx < MAX_L2CAP_LINKS; xx++, p_lcb++) {
        if ((p_lcb->in_use) && (p_lcb->handle == handle)) {
            return (p_lcb);
        }
    }

    /* If here, no match found */
    return (NULL);
}

/*******************************************************************************
**
** Function         l2cu_find_ccb_by_cid
**
** Description      Look through all active CCBs on a link for a match based
**                  on the local CID. If passed the link pointer is NULL, all
**                  active links are searched.
**
** Returns          pointer to matched CCB, or NULL if no match
**
*******************************************************************************/
tL2C_CCB *l2cu_find_ccb_by_cid (tL2C_LCB *p_lcb, UINT16 local_cid)
{
    tL2C_CCB    *p_ccb = NULL;
#if (L2CAP_UCD_INCLUDED == TRUE)
    UINT8 xx;
#endif

    if (local_cid >= L2CAP_BASE_APPL_CID) {
        /* find the associated CCB by "index" */
        local_cid -= L2CAP_BASE_APPL_CID;

        if (local_cid >= MAX_L2CAP_CHANNELS) {
            return NULL;
        }

        p_ccb = l2cb.ccb_pool + local_cid;

        /* make sure the CCB is in use */
        if (!p_ccb->in_use) {
            p_ccb = NULL;
        }
        /* make sure it's for the same LCB */
        else if (p_lcb && p_lcb != p_ccb->p_lcb) {
            p_ccb = NULL;
        }
    }
#if (L2CAP_UCD_INCLUDED == TRUE)
    else {
        /* searching fixed channel */
        p_ccb = l2cb.ccb_pool;
        for ( xx = 0; xx < MAX_L2CAP_CHANNELS; xx++ ) {
            if ((p_ccb->local_cid == local_cid)
                    && (p_ccb->in_use)
                    && (p_lcb == p_ccb->p_lcb)) {
                break;
            } else {
                p_ccb++;
            }
        }
        if ( xx >= MAX_L2CAP_CHANNELS ) {
            return NULL;
        }
    }
#endif

    return (p_ccb);
}

#if (L2CAP_ROUND_ROBIN_CHANNEL_SERVICE == TRUE && CLASSIC_BT_INCLUDED == TRUE)

/******************************************************************************
**
** Function         l2cu_get_next_channel_in_rr
**
** Description      get the next channel to send on a link. It also adjusts the
**                  CCB queue to do a basic priority and round-robin scheduling.
**
** Returns          pointer to CCB or NULL
**
*******************************************************************************/
static tL2C_CCB *l2cu_get_next_channel_in_rr(tL2C_LCB *p_lcb)
{
    tL2C_CCB    *p_serve_ccb = NULL;
    tL2C_CCB    *p_ccb;

    int i, j;

    /* scan all of priority until finding a channel to serve */
    for ( i = 0; (i < L2CAP_NUM_CHNL_PRIORITY) && (!p_serve_ccb); i++ ) {
        /* scan all channel within serving priority group until finding a channel to serve */
        for ( j = 0; (j < p_lcb->rr_serv[p_lcb->rr_pri].num_ccb) && (!p_serve_ccb); j++) {
            /* scaning from next serving channel */
            p_ccb = p_lcb->rr_serv[p_lcb->rr_pri].p_serve_ccb;

            if (!p_ccb) {
                L2CAP_TRACE_ERROR("p_serve_ccb is NULL, rr_pri=%d", p_lcb->rr_pri);
                return NULL;
            }

            L2CAP_TRACE_DEBUG("RR scan pri=%d, lcid=0x%04x, q_cout=%d",
                              p_ccb->ccb_priority, p_ccb->local_cid,
                              fixed_queue_length(p_ccb->xmit_hold_q));

            /* store the next serving channel */
            /* this channel is the last channel of its priority group */
            if (( p_ccb->p_next_ccb == NULL )
                    || ( p_ccb->p_next_ccb->ccb_priority != p_ccb->ccb_priority )) {
                /* next serving channel is set to the first channel in the group */
                p_lcb->rr_serv[p_lcb->rr_pri].p_serve_ccb = p_lcb->rr_serv[p_lcb->rr_pri].p_first_ccb;
            } else {
                /* next serving channel is set to the next channel in the group */
                p_lcb->rr_serv[p_lcb->rr_pri].p_serve_ccb = p_ccb->p_next_ccb;
            }

            if (p_ccb->chnl_state != CST_OPEN) {
                continue;
            }

            /* eL2CAP option in use */
            if (p_ccb->peer_cfg.fcr.mode != L2CAP_FCR_BASIC_MODE) {
                if (p_ccb->fcrb.wait_ack || p_ccb->fcrb.remote_busy) {
                    continue;
                }

                if (fixed_queue_is_empty(p_ccb->fcrb.retrans_q)) {
                    if (fixed_queue_is_empty(p_ccb->xmit_hold_q)) {
                        continue;
                    }


#if (CLASSIC_BT_INCLUDED == TRUE)
                    /* If in eRTM mode, check for window closure */
                    if ( (p_ccb->peer_cfg.fcr.mode == L2CAP_FCR_ERTM_MODE) && (l2c_fcr_is_flow_controlled (p_ccb)) ) {
                        continue;
                    }
#endif  ///CLASSIC_BT_INCLUDED == TRUE
                }
            } else {
                if (fixed_queue_is_empty(p_ccb->xmit_hold_q)) {
                    continue;
                }
            }

            /* found a channel to serve */
            p_serve_ccb = p_ccb;
            /* decrease quota of its priority group */
            p_lcb->rr_serv[p_lcb->rr_pri].quota--;
        }

        /* if there is no more quota of the priority group or no channel to have data to send */
        if ((p_lcb->rr_serv[p_lcb->rr_pri].quota == 0) || (!p_serve_ccb)) {
            /* serve next priority group */
            p_lcb->rr_pri = (p_lcb->rr_pri + 1) % L2CAP_NUM_CHNL_PRIORITY;
            /* initialize its quota */
            p_lcb->rr_serv[p_lcb->rr_pri].quota = L2CAP_GET_PRIORITY_QUOTA(p_lcb->rr_pri);
        }
    }

    if (p_serve_ccb) {
        L2CAP_TRACE_DEBUG("RR service pri=%d, quota=%d, lcid=0x%04x",
                          p_serve_ccb->ccb_priority,
                          p_lcb->rr_serv[p_serve_ccb->ccb_priority].quota,
                          p_serve_ccb->local_cid );
    }

    return p_serve_ccb;
}

#else /* (L2CAP_ROUND_ROBIN_CHANNEL_SERVICE == TRUE) */

/******************************************************************************
**
** Function         l2cu_get_next_channel
**
** Description      get the next channel to send on a link bassed on priority
**                  scheduling.
**
** Returns          pointer to CCB or NULL
**
*******************************************************************************/
#if (CLASSIC_BT_INCLUDED == TRUE)
static tL2C_CCB *l2cu_get_next_channel(tL2C_LCB *p_lcb)
{
    tL2C_CCB    *p_ccb;

    /* Get the first CCB with data to send.
    */
    for (p_ccb = p_lcb->ccb_queue.p_first_ccb; p_ccb; p_ccb = p_ccb->p_next_ccb) {
        if (p_ccb->chnl_state != CST_OPEN) {
            continue;
        }

        if (p_ccb->fcrb.wait_ack || p_ccb->fcrb.remote_busy) {
            continue;
        }

        if (!fixed_queue_is_empty(p_ccb->fcrb.retrans_q))
            return p_ccb;
        }

        if (fixed_queue_is_empty(p_ccb->xmit_hold_q))
            continue;
        }

        /* If in eRTM mode, check for window closure */
        if ( (p_ccb->peer_cfg.fcr.mode == L2CAP_FCR_ERTM_MODE) && (l2c_fcr_is_flow_controlled (p_ccb)) ) {
            continue;
        }

        /* If here, we found someone */
        return p_ccb;
    }

    return NULL;
}
#endif  ///CLASSIC_BT_INCLUDED == TRUE

#endif /* (L2CAP_ROUND_ROBIN_CHANNEL_SERVICE == TRUE) */

/******************************************************************************
**
** Function         l2cu_get_next_buffer_to_send
**
** Description      get the next buffer to send on a link. It also adjusts the
**                  CCB queue to do a basic priority and round-robin scheduling.
**
** Returns          pointer to buffer or NULL
**
*******************************************************************************/
BT_HDR *l2cu_get_next_buffer_to_send (tL2C_LCB *p_lcb)
{
    tL2C_CCB    *p_ccb;
    BT_HDR      *p_buf = NULL;

    /* Highest priority are fixed channels */
#if (L2CAP_NUM_FIXED_CHNLS > 0)
    int         xx;

    for (xx = 0; xx < L2CAP_NUM_FIXED_CHNLS; xx++) {
        if ((p_ccb = p_lcb->p_fixed_ccbs[xx]) == NULL) {
            continue;
        }

        /* eL2CAP option in use */
        if (p_ccb->peer_cfg.fcr.mode != L2CAP_FCR_BASIC_MODE) {
#if (CLASSIC_BT_INCLUDED == TRUE)
            if (p_ccb->fcrb.wait_ack || p_ccb->fcrb.remote_busy) {
                continue;
            }

            /* No more checks needed if sending from the reatransmit queue */
            if (fixed_queue_is_empty(p_ccb->fcrb.retrans_q))
            {
                if (fixed_queue_is_empty(p_ccb->xmit_hold_q)) {
                    continue;
                }
                /* If in eRTM mode, check for window closure */
                if ( (p_ccb->peer_cfg.fcr.mode == L2CAP_FCR_ERTM_MODE) && (l2c_fcr_is_flow_controlled (p_ccb)) ) {
                    continue;
                }
            }
            if ((p_buf = l2c_fcr_get_next_xmit_sdu_seg(p_ccb, 0)) != NULL) {
                l2cu_check_channel_congestion (p_ccb);
                l2cu_set_acl_hci_header (p_buf, p_ccb);
                return (p_buf);
            }
#else
            continue;          
#endif  ///CLASSIC_BT_INCLUDED == TRUE

        } else {
            if (!fixed_queue_is_empty(p_ccb->xmit_hold_q)) {
                p_buf = (BT_HDR *)fixed_queue_try_dequeue(p_ccb->xmit_hold_q);
                if (NULL == p_buf) {
                    L2CAP_TRACE_ERROR("l2cu_get_buffer_to_send: No data to be sent");
                    return (NULL);
                }
                /* send tx complete */
                if (l2cb.fixed_reg[xx].pL2CA_FixedTxComplete_Cb) {
                    (*l2cb.fixed_reg[xx].pL2CA_FixedTxComplete_Cb)(p_ccb->local_cid, 1);
                }

                l2cu_check_channel_congestion (p_ccb);
                l2cu_set_acl_hci_header (p_buf, p_ccb);
                return (p_buf);
            }
        }
    }
#endif
#if (CLASSIC_BT_INCLUDED == TRUE)
#if (L2CAP_ROUND_ROBIN_CHANNEL_SERVICE == TRUE)
    /* get next serving channel in round-robin */
    p_ccb  = l2cu_get_next_channel_in_rr( p_lcb );
#else
    p_ccb  = l2cu_get_next_channel( p_lcb );
#endif

    /* Return if no buffer */
    if (p_ccb == NULL) {
        return (NULL);
    }

    if (p_ccb->peer_cfg.fcr.mode != L2CAP_FCR_BASIC_MODE) {

        if ((p_buf = l2c_fcr_get_next_xmit_sdu_seg(p_ccb, 0)) == NULL) {
            return (NULL);
        }

    } else {
        p_buf = (BT_HDR *)fixed_queue_try_dequeue(p_ccb->xmit_hold_q);
        if (NULL == p_buf) {
            L2CAP_TRACE_ERROR("l2cu_get_buffer_to_send() #2: No data to be sent");
            return (NULL);
        }
    }

    if ( p_ccb->p_rcb && p_ccb->p_rcb->api.pL2CA_TxComplete_Cb && (p_ccb->peer_cfg.fcr.mode != L2CAP_FCR_ERTM_MODE) ) {
        (*p_ccb->p_rcb->api.pL2CA_TxComplete_Cb)(p_ccb->local_cid, 1);
    }


    l2cu_check_channel_congestion (p_ccb);

    l2cu_set_acl_hci_header (p_buf, p_ccb);
#endif  ///CLASSIC_BT_INCLUDED == TRUE

    return (p_buf);
}

/******************************************************************************
**
** Function         l2cu_set_acl_hci_header
**
** Description      Set HCI handle for ACL packet
**
** Returns          None
**
*******************************************************************************/
void l2cu_set_acl_hci_header (BT_HDR *p_buf, tL2C_CCB *p_ccb)
{
    UINT8       *p;

    /* Set the pointer to the beginning of the data minus 4 bytes for the packet header */
    p = (UINT8 *)(p_buf + 1) + p_buf->offset - HCI_DATA_PREAMBLE_SIZE;

#if (BLE_INCLUDED == TRUE)
    if (p_ccb->p_lcb->transport == BT_TRANSPORT_LE) {
        UINT16_TO_STREAM (p, p_ccb->p_lcb->handle | (L2CAP_PKT_START_NON_FLUSHABLE << L2CAP_PKT_TYPE_SHIFT));

        uint16_t acl_data_size = controller_get_interface()->get_acl_data_size_ble();
        /* The HCI transport will segment the buffers. */
        if (p_buf->len > acl_data_size) {
            UINT16_TO_STREAM (p, acl_data_size);
        } else {
            UINT16_TO_STREAM (p, p_buf->len);
        }
    } /* (BLE_INCLUDED == TRUE) */
    else
#endif
    {
#if (L2CAP_NON_FLUSHABLE_PB_INCLUDED == TRUE)
        if ( (((p_buf->layer_specific & L2CAP_FLUSHABLE_MASK) == L2CAP_FLUSHABLE_CH_BASED) && (p_ccb->is_flushable))
                || ((p_buf->layer_specific & L2CAP_FLUSHABLE_MASK) == L2CAP_FLUSHABLE_PKT) ) {
            UINT16_TO_STREAM (p, p_ccb->p_lcb->handle | (L2CAP_PKT_START << L2CAP_PKT_TYPE_SHIFT));
        } else {
            UINT16_TO_STREAM (p, p_ccb->p_lcb->handle | l2cb.non_flushable_pbf);
        }
#else
        UINT16_TO_STREAM (p, p_ccb->p_lcb->handle | (L2CAP_PKT_START << L2CAP_PKT_TYPE_SHIFT));
#endif

        uint16_t acl_data_size = controller_get_interface()->get_acl_data_size_classic();
        /* The HCI transport will segment the buffers. */
        if (p_buf->len > acl_data_size) {
            UINT16_TO_STREAM (p, acl_data_size);
        } else {
            UINT16_TO_STREAM (p, p_buf->len);
        }
    }
    p_buf->offset -= HCI_DATA_PREAMBLE_SIZE;
    p_buf->len    += HCI_DATA_PREAMBLE_SIZE;
}

/******************************************************************************
**
** Function         l2cu_check_channel_congestion
**
** Description      check if any change in congestion status
**
** Returns          None
**
*******************************************************************************/
void l2cu_check_channel_congestion (tL2C_CCB *p_ccb)
{
    size_t q_count = fixed_queue_length(p_ccb->xmit_hold_q);

#if (L2CAP_UCD_INCLUDED == TRUE)
    if ( p_ccb->local_cid == L2CAP_CONNECTIONLESS_CID ) {
        q_count += fixed_queue_length(p_ccb->p_lcb->ucd_out_sec_pending_q);
    }
#endif
    /* If the CCB queue limit is subject to a quota, check for congestion */
    /* if this channel has outgoing traffic */
    if (p_ccb->buff_quota != 0) {
        /* If this channel was congested */
        if ( p_ccb->cong_sent ) {
            /* If the channel is not congested now, tell the app */
            if (q_count <= (p_ccb->buff_quota / 2)) {
                p_ccb->cong_sent = FALSE;
                if (p_ccb->p_rcb && p_ccb->p_rcb->api.pL2CA_CongestionStatus_Cb) {
                    L2CAP_TRACE_DEBUG ("L2CAP - Calling CongestionStatus_Cb (FALSE), CID: 0x%04x  xmit_hold_q.count: %u  buff_quota: %u",
                                       p_ccb->local_cid, q_count, p_ccb->buff_quota);

                    /* Prevent recursive calling */
                    l2cb.is_cong_cback_context = TRUE;
                    (*p_ccb->p_rcb->api.pL2CA_CongestionStatus_Cb)(p_ccb->local_cid, FALSE);
                    l2cb.is_cong_cback_context = FALSE;
                }
#if (L2CAP_UCD_INCLUDED == TRUE)
                else if ( p_ccb->p_rcb && p_ccb->local_cid == L2CAP_CONNECTIONLESS_CID ) {
                    if ( p_ccb->p_rcb->ucd.cb_info.pL2CA_UCD_Congestion_Status_Cb ) {
                        L2CAP_TRACE_DEBUG ("L2CAP - Calling UCD CongestionStatus_Cb (FALSE), SecPendingQ:%u,XmitQ:%u,Quota:%u",
                                           fixed_queue_length(p_ccb->p_lcb->ucd_out_sec_pending_q),
                                           fixed_queue_length(p_ccb->xmit_hold_q),
                                           p_ccb->buff_quota);
                        p_ccb->p_rcb->ucd.cb_info.pL2CA_UCD_Congestion_Status_Cb( p_ccb->p_lcb->remote_bd_addr, FALSE );
                    }
                }
#endif
#if (L2CAP_NUM_FIXED_CHNLS > 0)
                else {
                    UINT8 xx;
                    for (xx = 0; xx < L2CAP_NUM_FIXED_CHNLS; xx ++) {
                        if (p_ccb->p_lcb->p_fixed_ccbs[xx] == p_ccb) {
                            if (l2cb.fixed_reg[xx].pL2CA_FixedCong_Cb != NULL) {
                                (* l2cb.fixed_reg[xx].pL2CA_FixedCong_Cb)(p_ccb->p_lcb->remote_bd_addr, FALSE);
                            }
                            break;
                        }
                    }
                }
#endif
            }
        } else {
            /* If this channel was not congested but it is congested now, tell the app */
            if (q_count > p_ccb->buff_quota) {
                p_ccb->cong_sent = TRUE;
                if (p_ccb->p_rcb && p_ccb->p_rcb->api.pL2CA_CongestionStatus_Cb) {
                    L2CAP_TRACE_DEBUG ("L2CAP - Calling CongestionStatus_Cb (TRUE),CID:0x%04x,XmitQ:%u,Quota:%u",
                                       p_ccb->local_cid, q_count, p_ccb->buff_quota);

                    (*p_ccb->p_rcb->api.pL2CA_CongestionStatus_Cb)(p_ccb->local_cid, TRUE);
                }
#if (L2CAP_UCD_INCLUDED == TRUE)
                else if ( p_ccb->p_rcb && p_ccb->local_cid == L2CAP_CONNECTIONLESS_CID ) {
                    if ( p_ccb->p_rcb->ucd.cb_info.pL2CA_UCD_Congestion_Status_Cb ) {
                        L2CAP_TRACE_DEBUG ("L2CAP - Calling UCD CongestionStatus_Cb (TRUE), SecPendingQ:%u,XmitQ:%u,Quota:%u",
                                          fixed_queue_length(p_ccb->p_lcb->ucd_out_sec_pending_q),
                                          fixed_queue_length(p_ccb->xmit_hold_q),
                                          p_ccb->buff_quota);
                        p_ccb->p_rcb->ucd.cb_info.pL2CA_UCD_Congestion_Status_Cb( p_ccb->p_lcb->remote_bd_addr, TRUE );
                    }
                }
#endif
#if (L2CAP_NUM_FIXED_CHNLS > 0)
                else {
                    UINT8 xx;
                    for (xx = 0; xx < L2CAP_NUM_FIXED_CHNLS; xx ++) {
                        if (p_ccb->p_lcb->p_fixed_ccbs[xx] == p_ccb) {
                            if (l2cb.fixed_reg[xx].pL2CA_FixedCong_Cb != NULL) {
                                (* l2cb.fixed_reg[xx].pL2CA_FixedCong_Cb)(p_ccb->p_lcb->remote_bd_addr, TRUE);
                            }
                            break;
                        }
                    }
                }
#endif
            }
        }
    }
}


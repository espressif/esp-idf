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
 *  This file contains the main L2CAP entry points
 *
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
//#include <stdio.h>

#include "controller.h"
//#include "btcore/include/counter.h"
#include "bt_target.h"
#include "btm_int.h"
#include "btu.h"
#include "hcimsgs.h"
#include "l2c_api.h"
#include "l2c_int.h"
#include "l2cdefs.h"
//#include "osi/include/log.h"

/********************************************************************************/
/*              L O C A L    F U N C T I O N     P R O T O T Y P E S            */
/********************************************************************************/
#if (CLASSIC_BT_INCLUDED == TRUE)
static void process_l2cap_cmd (tL2C_LCB *p_lcb, UINT8 *p, UINT16 pkt_len);
#endif  ///CLASSIC_BT_INCLUDED == TRUE
/********************************************************************************/
/*                 G L O B A L      L 2 C A P       D A T A                     */
/********************************************************************************/
#if L2C_DYNAMIC_MEMORY == FALSE
tL2C_CB l2cb;
#endif

/*******************************************************************************
**
** Function         l2c_bcst_msg
**
** Description
**
** Returns          void
**
*******************************************************************************/
void l2c_bcst_msg( BT_HDR *p_buf, UINT16 psm )
{
    UINT8       *p;

    /* Ensure we have enough space in the buffer for the L2CAP and HCI headers */
    if (p_buf->offset < L2CAP_BCST_MIN_OFFSET) {
        L2CAP_TRACE_ERROR ("L2CAP - cannot send buffer, offset: %d", p_buf->offset);
        osi_free (p_buf);
        return;
    }

    /* Step back some bytes to add the headers */
    p_buf->offset -= (HCI_DATA_PREAMBLE_SIZE + L2CAP_PKT_OVERHEAD + L2CAP_BCST_OVERHEAD);
    p_buf->len    += L2CAP_PKT_OVERHEAD + L2CAP_BCST_OVERHEAD;

    /* Set the pointer to the beginning of the data */
    p = (UINT8 *)(p_buf + 1) + p_buf->offset;

    /* First, the HCI transport header */
    UINT16_TO_STREAM (p, 0x0050 | (L2CAP_PKT_START << 12) | (2 << 14));

    uint16_t acl_data_size = controller_get_interface()->get_acl_data_size_classic();
    /* The HCI transport will segment the buffers. */
    if (p_buf->len > acl_data_size) {
        UINT16_TO_STREAM (p, acl_data_size);
    } else {
        UINT16_TO_STREAM (p, p_buf->len);
    }

    /* Now the L2CAP header */
    UINT16_TO_STREAM (p, p_buf->len - L2CAP_PKT_OVERHEAD);
    UINT16_TO_STREAM (p, L2CAP_CONNECTIONLESS_CID);
    UINT16_TO_STREAM (p, psm);

    p_buf->len += HCI_DATA_PREAMBLE_SIZE;

    if (p_buf->len <= controller_get_interface()->get_acl_packet_size_classic()) {
        //counter_add("l2cap.ch2.tx.bytes", p_buf->len);
        //counter_add("l2cap.ch2.tx.pkts", 1);

        bte_main_hci_send(p_buf, BT_EVT_TO_LM_HCI_ACL);
    }
}


/*******************************************************************************
**
** Function         l2c_rcv_acl_data
**
** Description      This function is called from the HCI Interface when an ACL
**                  data packet is received.
**
** Returns          void
**
*******************************************************************************/
void l2c_rcv_acl_data (BT_HDR *p_msg)
{
    UINT8       *p = (UINT8 *)(p_msg + 1) + p_msg->offset;
    UINT16      handle, hci_len;
    UINT8       pkt_type;
    tL2C_LCB    *p_lcb;
    tL2C_CCB    *p_ccb = NULL;
    UINT16      l2cap_len, rcv_cid, psm;
    UINT16      credit;

    /* Extract the handle */
    STREAM_TO_UINT16 (handle, p);
    pkt_type = HCID_GET_EVENT (handle);
    handle   = HCID_GET_HANDLE (handle);

    /* Since the HCI Transport is putting segmented packets back together, we */
    /* should never get a valid packet with the type set to "continuation"    */
    if (pkt_type != L2CAP_PKT_CONTINUE) {
        /* Find the LCB based on the handle */
        if ((p_lcb = l2cu_find_lcb_by_handle (handle)) == NULL) {
            UINT8       cmd_code;

            /* There is a slight possibility (specifically with USB) that we get an */
            /* L2CAP connection request before we get the HCI connection complete.  */
            /* So for these types of messages, hold them for up to 2 seconds.       */
            STREAM_TO_UINT16 (hci_len, p);
            STREAM_TO_UINT16 (l2cap_len, p);
            STREAM_TO_UINT16 (rcv_cid, p);
            STREAM_TO_UINT8  (cmd_code, p);

            if ((p_msg->layer_specific == 0) && (rcv_cid == L2CAP_SIGNALLING_CID)
                    && (cmd_code == L2CAP_CMD_INFO_REQ || cmd_code == L2CAP_CMD_CONN_REQ)) {
                L2CAP_TRACE_WARNING ("L2CAP - holding ACL for unknown handle:%d ls:%d"
                                     "  cid:%d opcode:%d cur count:%d", handle, p_msg->layer_specific,
                                     rcv_cid, cmd_code, list_length(l2cb.rcv_pending_q));
                p_msg->layer_specific = 2;
                list_append(l2cb.rcv_pending_q, p_msg);

                if (list_length(l2cb.rcv_pending_q) == 1) {
                    btu_start_timer (&l2cb.rcv_hold_tle, BTU_TTYPE_L2CAP_HOLD, BT_1SEC_TIMEOUT);
                }

                return;
            } else {
                L2CAP_TRACE_ERROR ("L2CAP - rcvd ACL for unknown handle:%d ls:%d cid:%d"
                                   " opcode:%d cur count:%d", handle, p_msg->layer_specific, rcv_cid,
                                   cmd_code, list_length(l2cb.rcv_pending_q));
            }
            osi_free (p_msg);
            return;
        }
    } else {
        L2CAP_TRACE_WARNING ("L2CAP - expected pkt start or complete, got: %d", pkt_type);
        osi_free (p_msg);
        return;
    }

    /* Extract the length and update the buffer header */
    STREAM_TO_UINT16 (hci_len, p);
    p_msg->offset += 4;

    /* Extract the length and CID */
    STREAM_TO_UINT16 (l2cap_len, p);
    STREAM_TO_UINT16 (rcv_cid, p);

#if BLE_INCLUDED == TRUE
    /* for BLE channel, always notify connection when ACL data received on the link */
    if (p_lcb && p_lcb->transport == BT_TRANSPORT_LE && p_lcb->link_state != LST_DISCONNECTING)
        /* only process fixed channel data as channel open indication when link is not in disconnecting mode */
    {
        l2cble_notify_le_connection(p_lcb->remote_bd_addr);
    }
#endif
    L2CAP_TRACE_DEBUG ("L2CAP - rcv_cid CID: 0x%04x\n", rcv_cid);
    /* Find the CCB for this CID */
    if (rcv_cid >= L2CAP_BASE_APPL_CID) {
        if ((p_ccb = l2cu_find_ccb_by_cid (p_lcb, rcv_cid)) == NULL) {
            L2CAP_TRACE_WARNING ("L2CAP - unknown CID: 0x%04x", rcv_cid);
            osi_free (p_msg);
            return;
        }
    }

    if (hci_len >= L2CAP_PKT_OVERHEAD) { /* Must receive at least the L2CAP length and CID.*/
        p_msg->len    = hci_len - L2CAP_PKT_OVERHEAD;
        p_msg->offset += L2CAP_PKT_OVERHEAD;
    } else {
        L2CAP_TRACE_WARNING ("L2CAP - got incorrect hci header" );
        osi_free (p_msg);
        return;
    }

    if (l2cap_len != p_msg->len) {
        L2CAP_TRACE_WARNING ("L2CAP - bad length in pkt. Exp: %d  Act: %d",
                             l2cap_len, p_msg->len);

        osi_free (p_msg);
        return;
    }

    /* Send the data through the channel state machine */
    if (rcv_cid == L2CAP_SIGNALLING_CID) {
        //counter_add("l2cap.sig.rx.bytes", l2cap_len);
        //counter_add("l2cap.sig.rx.pkts", 1);
#if (CLASSIC_BT_INCLUDED == TRUE)
        process_l2cap_cmd (p_lcb, p, l2cap_len);
#endif  ///CLASSIC_BT_INCLUDED == TRUE
        osi_free (p_msg);
    } else if (rcv_cid == L2CAP_CONNECTIONLESS_CID) {
        //counter_add("l2cap.ch2.rx.bytes", l2cap_len);
        //counter_add("l2cap.ch2.rx.pkts", 1);
        /* process_connectionless_data (p_lcb); */
        STREAM_TO_UINT16 (psm, p);
        L2CAP_TRACE_DEBUG( "GOT CONNECTIONLESS DATA PSM:%d", psm ) ;

#if (L2CAP_UCD_INCLUDED == TRUE)
        /* if it is not broadcast, check UCD registration */
        if ( l2c_ucd_check_rx_pkts( p_lcb, p_msg ) ) {
            /* nothing to do */
        } else
#endif
            osi_free (p_msg);
    }
#if (BLE_INCLUDED == TRUE)
    else if (rcv_cid == L2CAP_BLE_SIGNALLING_CID) {
        //counter_add("l2cap.ble.rx.bytes", l2cap_len);
        //counter_add("l2cap.ble.rx.pkts", 1);
        l2cble_process_sig_cmd (p_lcb, p, l2cap_len);
        osi_free (p_msg);
    }
#endif
#if (L2CAP_NUM_FIXED_CHNLS > 0)
    else if ((rcv_cid >= L2CAP_FIRST_FIXED_CHNL) && (rcv_cid <= L2CAP_LAST_FIXED_CHNL) &&
             (l2cb.fixed_reg[rcv_cid - L2CAP_FIRST_FIXED_CHNL].pL2CA_FixedData_Cb != NULL) ) {
        //counter_add("l2cap.fix.rx.bytes", l2cap_len);
        //counter_add("l2cap.fix.rx.pkts", 1);
        /* If no CCB for this channel, allocate one */
        if (p_lcb &&
                /* only process fixed channel data when link is open or wait for data indication */
                (p_lcb->link_state != LST_DISCONNECTING) &&
                l2cu_initialize_fixed_ccb (p_lcb, rcv_cid, &l2cb.fixed_reg[rcv_cid - L2CAP_FIRST_FIXED_CHNL].fixed_chnl_opts)) {
            p_ccb = p_lcb->p_fixed_ccbs[rcv_cid - L2CAP_FIRST_FIXED_CHNL];

            if (p_ccb->peer_cfg.fcr.mode != L2CAP_FCR_BASIC_MODE) {
#if (CLASSIC_BT_INCLUDED == TRUE)
                l2c_fcr_proc_pdu (p_ccb, p_msg);
#endif  ///CLASSIC_BT_INCLUDED == TRUE
            } else
                (*l2cb.fixed_reg[rcv_cid - L2CAP_FIRST_FIXED_CHNL].pL2CA_FixedData_Cb)
                (rcv_cid, p_lcb->remote_bd_addr, p_msg);
        } else {
            osi_free (p_msg);
        }
    }
#endif

    else {
        //counter_add("l2cap.dyn.rx.bytes", l2cap_len);
        //counter_add("l2cap.dyn.rx.pkts", 1);
        if (p_ccb == NULL) {
            osi_free (p_msg);
        } else {
            if (p_lcb->transport == BT_TRANSPORT_LE) {
                // Got a pkt, valid send out credits to the peer device
                credit = L2CAP_LE_DEFAULT_CREDIT;
                L2CAP_TRACE_DEBUG("%s Credits received %d",__func__, credit);
                if((p_ccb->peer_conn_cfg.credits + credit) > L2CAP_LE_MAX_CREDIT) {
                    /* we have received credits more than max coc credits,
                     * so disconnecting the Le Coc Channel
                     */
                    l2cble_send_peer_disc_req (p_ccb);
                } else {
                    p_ccb->peer_conn_cfg.credits += credit;
                    l2c_link_check_send_pkts (p_ccb->p_lcb, NULL, NULL);
                }
            }
            /* Basic mode packets go straight to the state machine */
            if (p_ccb->peer_cfg.fcr.mode == L2CAP_FCR_BASIC_MODE) {
#if (CLASSIC_BT_INCLUDED == TRUE)
                l2c_csm_execute (p_ccb, L2CEVT_L2CAP_DATA, p_msg);
#endif  ///CLASSIC_BT_INCLUDED == TRUE
            } else {
                /* eRTM or streaming mode, so we need to validate states first */
                if ((p_ccb->chnl_state == CST_OPEN) || (p_ccb->chnl_state == CST_CONFIG)) {
#if (CLASSIC_BT_INCLUDED == TRUE)
                    l2c_fcr_proc_pdu (p_ccb, p_msg);
#endif  ///CLASSIC_BT_INCLUDED == TRUE
                } else {
                    osi_free (p_msg);
                }
            }
        }
    }
}

/*******************************************************************************
**
** Function         process_l2cap_cmd
**
** Description      This function is called when a packet is received on the
**                  L2CAP signalling CID
**
** Returns          void
**
*******************************************************************************/
#if (CLASSIC_BT_INCLUDED == TRUE)
static void process_l2cap_cmd (tL2C_LCB *p_lcb, UINT8 *p, UINT16 pkt_len)
{
    UINT8           *p_pkt_end, *p_next_cmd, *p_cfg_end, *p_cfg_start;
    UINT8           cmd_code, cfg_code, cfg_len, id;
    tL2C_CONN_INFO  con_info;
    tL2CAP_CFG_INFO cfg_info;
    UINT16          rej_reason, rej_mtu, lcid, rcid, info_type;
    tL2C_CCB        *p_ccb;
    tL2C_RCB        *p_rcb;
    BOOLEAN         cfg_rej, pkt_size_rej = FALSE;
    UINT16          cfg_rej_len, cmd_len;
    UINT16          result;
    tL2C_CONN_INFO  ci;

#if (defined BLE_INCLUDED && BLE_INCLUDED == TRUE)
    /* if l2cap command received in CID 1 on top of an LE link, ignore this command */
    if (p_lcb->transport == BT_TRANSPORT_LE) {
        return;
    }
#endif

    /* Reject the packet if it exceeds the default Signalling Channel MTU */
    if (pkt_len > L2CAP_DEFAULT_MTU) {
        /* Core Spec requires a single response to the first command found in a multi-command
        ** L2cap packet.  If only responses in the packet, then it will be ignored.
        ** Here we simply mark the bad packet and decide which cmd ID to reject later
        */
        pkt_size_rej = TRUE;
        L2CAP_TRACE_ERROR ("L2CAP SIG MTU Pkt Len Exceeded (672) -> pkt_len: %d", pkt_len);
    }

    p_next_cmd = p;
    p_pkt_end  = p + pkt_len;

    memset (&cfg_info, 0, sizeof(cfg_info));

    /* An L2CAP packet may contain multiple commands */
    while (TRUE) {
        /* Smallest command is 4 bytes */
        if ((p = p_next_cmd) > (p_pkt_end - 4)) {
            break;
        }

        STREAM_TO_UINT8  (cmd_code, p);
        STREAM_TO_UINT8  (id, p);
        STREAM_TO_UINT16 (cmd_len, p);

        /* Check command length does not exceed packet length */
        if ((p_next_cmd = p + cmd_len) > p_pkt_end) {
            L2CAP_TRACE_WARNING ("Command len bad  pkt_len: %d  cmd_len: %d  code: %d",
                                 pkt_len, cmd_len, cmd_code);
            break;
        }

        L2CAP_TRACE_DEBUG ("cmd_code: %d, id:%d, cmd_len:%d", cmd_code, id, cmd_len);

        /* Bad L2CAP packet length, look or cmd to reject */
        if (pkt_size_rej) {
            /* If command found rejected it and we're done, otherwise keep looking */
            if (l2c_is_cmd_rejected(cmd_code, id, p_lcb)) {
                return;
            } else {
                continue;    /* Look for next cmd/response in current packet */
            }
        }

        switch (cmd_code) {
        case L2CAP_CMD_REJECT:
            STREAM_TO_UINT16 (rej_reason, p);
            if (rej_reason == L2CAP_CMD_REJ_MTU_EXCEEDED) {
                STREAM_TO_UINT16 (rej_mtu, p);
                /* What to do with the MTU reject ? We have negotiated an MTU. For now */
                /* we will ignore it and let a higher protocol timeout take care of it */

                L2CAP_TRACE_WARNING ("L2CAP - MTU rej Handle: %d MTU: %d", p_lcb->handle, rej_mtu);
            }
            if (rej_reason == L2CAP_CMD_REJ_INVALID_CID) {
                STREAM_TO_UINT16 (rcid, p);
                STREAM_TO_UINT16 (lcid, p);

                L2CAP_TRACE_WARNING ("L2CAP - rej with CID invalid, LCID: 0x%04x RCID: 0x%04x", lcid, rcid);

                /* Remote CID invalid. Treat as a disconnect */
                if (((p_ccb = l2cu_find_ccb_by_cid (p_lcb, lcid)) != NULL)
                        && (p_ccb->remote_cid == rcid)) {
                    /* Fake link disconnect - no reply is generated */
                    l2c_csm_execute (p_ccb, L2CEVT_LP_DISCONNECT_IND, NULL);
                }
            }

            /* SonyEricsson Info request Bug workaround (Continue connection) */
            else if (rej_reason == L2CAP_CMD_REJ_NOT_UNDERSTOOD && p_lcb->w4_info_rsp) {
                btu_stop_timer (&p_lcb->info_timer_entry);

                p_lcb->w4_info_rsp = FALSE;
                ci.status = HCI_SUCCESS;
                memcpy (ci.bd_addr, p_lcb->remote_bd_addr, sizeof(BD_ADDR));

                /* For all channels, send the event through their FSMs */
                for (p_ccb = p_lcb->ccb_queue.p_first_ccb; p_ccb; p_ccb = p_ccb->p_next_ccb) {
                    l2c_csm_execute (p_ccb, L2CEVT_L2CAP_INFO_RSP, &ci);
                }
            }
            break;

        case L2CAP_CMD_CONN_REQ:
            STREAM_TO_UINT16 (con_info.psm, p);
            STREAM_TO_UINT16 (rcid, p);
            if ((p_rcb = l2cu_find_rcb_by_psm (con_info.psm)) == NULL) {
                L2CAP_TRACE_WARNING ("L2CAP - rcvd conn req for unknown PSM: %d", con_info.psm);
                l2cu_reject_connection (p_lcb, rcid, id, L2CAP_CONN_NO_PSM);
                break;
            } else {
                if (!p_rcb->api.pL2CA_ConnectInd_Cb) {
                    L2CAP_TRACE_WARNING ("L2CAP - rcvd conn req for outgoing-only connection PSM: %d", con_info.psm);
                    l2cu_reject_connection (p_lcb, rcid, id, L2CAP_CONN_NO_PSM);
                    break;
                }
            }
            if ((p_ccb = l2cu_allocate_ccb (p_lcb, 0)) == NULL) {
                L2CAP_TRACE_ERROR ("L2CAP - unable to allocate CCB");
                l2cu_reject_connection (p_lcb, rcid, id, L2CAP_CONN_NO_RESOURCES);
                break;
            }
            p_ccb->remote_id = id;
            p_ccb->p_rcb = p_rcb;
            p_ccb->remote_cid = rcid;

            l2c_csm_execute(p_ccb, L2CEVT_L2CAP_CONNECT_REQ, &con_info);
            break;

        case L2CAP_CMD_CONN_RSP:
            STREAM_TO_UINT16 (con_info.remote_cid, p);
            STREAM_TO_UINT16 (lcid, p);
            STREAM_TO_UINT16 (con_info.l2cap_result, p);
            STREAM_TO_UINT16 (con_info.l2cap_status, p);

            if ((p_ccb = l2cu_find_ccb_by_cid (p_lcb, lcid)) == NULL) {
                L2CAP_TRACE_WARNING ("L2CAP - no CCB for conn rsp, LCID: %d RCID: %d",
                                     lcid, con_info.remote_cid);
                break;
            }
            if (p_ccb->local_id != id) {
                L2CAP_TRACE_WARNING ("L2CAP - con rsp - bad ID. Exp: %d Got: %d",
                                     p_ccb->local_id, id);
                break;
            }

            if (con_info.l2cap_result == L2CAP_CONN_OK) {
                l2c_csm_execute(p_ccb, L2CEVT_L2CAP_CONNECT_RSP, &con_info);
            } else if (con_info.l2cap_result == L2CAP_CONN_PENDING) {
                l2c_csm_execute(p_ccb, L2CEVT_L2CAP_CONNECT_RSP_PND, &con_info);
            } else {
                l2c_csm_execute(p_ccb, L2CEVT_L2CAP_CONNECT_RSP_NEG, &con_info);
            }

            break;

        case L2CAP_CMD_CONFIG_REQ:
            p_cfg_end = p + cmd_len;
            cfg_rej = FALSE;
            cfg_rej_len = 0;

            STREAM_TO_UINT16 (lcid, p);
            STREAM_TO_UINT16 (cfg_info.flags, p);

            p_cfg_start = p;

            cfg_info.flush_to_present = cfg_info.mtu_present = cfg_info.qos_present =
                                            cfg_info.fcr_present = cfg_info.fcs_present = FALSE;

            while (p < p_cfg_end) {
                STREAM_TO_UINT8 (cfg_code, p);
                STREAM_TO_UINT8 (cfg_len, p);

                switch (cfg_code & 0x7F) {
                case L2CAP_CFG_TYPE_MTU:
                    cfg_info.mtu_present = TRUE;
                    STREAM_TO_UINT16 (cfg_info.mtu, p);
                    break;

                case L2CAP_CFG_TYPE_FLUSH_TOUT:
                    cfg_info.flush_to_present = TRUE;
                    STREAM_TO_UINT16 (cfg_info.flush_to, p);
                    break;

                case L2CAP_CFG_TYPE_QOS:
                    cfg_info.qos_present = TRUE;
                    STREAM_TO_UINT8  (cfg_info.qos.qos_flags, p);
                    STREAM_TO_UINT8  (cfg_info.qos.service_type, p);
                    STREAM_TO_UINT32 (cfg_info.qos.token_rate, p);
                    STREAM_TO_UINT32 (cfg_info.qos.token_bucket_size, p);
                    STREAM_TO_UINT32 (cfg_info.qos.peak_bandwidth, p);
                    STREAM_TO_UINT32 (cfg_info.qos.latency, p);
                    STREAM_TO_UINT32 (cfg_info.qos.delay_variation, p);
                    break;

                case L2CAP_CFG_TYPE_FCR:
                    cfg_info.fcr_present = TRUE;
                    STREAM_TO_UINT8 (cfg_info.fcr.mode, p);
                    STREAM_TO_UINT8 (cfg_info.fcr.tx_win_sz, p);
                    STREAM_TO_UINT8 (cfg_info.fcr.max_transmit, p);
                    STREAM_TO_UINT16 (cfg_info.fcr.rtrans_tout, p);
                    STREAM_TO_UINT16 (cfg_info.fcr.mon_tout, p);
                    STREAM_TO_UINT16 (cfg_info.fcr.mps, p);
                    break;

                case L2CAP_CFG_TYPE_FCS:
                    cfg_info.fcs_present = TRUE;
                    STREAM_TO_UINT8 (cfg_info.fcs, p);
                    break;

                case L2CAP_CFG_TYPE_EXT_FLOW:
                    cfg_info.ext_flow_spec_present = TRUE;
                    STREAM_TO_UINT8  (cfg_info.ext_flow_spec.id, p);
                    STREAM_TO_UINT8  (cfg_info.ext_flow_spec.stype, p);
                    STREAM_TO_UINT16 (cfg_info.ext_flow_spec.max_sdu_size, p);
                    STREAM_TO_UINT32 (cfg_info.ext_flow_spec.sdu_inter_time, p);
                    STREAM_TO_UINT32 (cfg_info.ext_flow_spec.access_latency, p);
                    STREAM_TO_UINT32 (cfg_info.ext_flow_spec.flush_timeout, p);
                    break;

                default:
                    /* sanity check option length */
                    if ((cfg_len + L2CAP_CFG_OPTION_OVERHEAD) <= cmd_len) {
                        p += cfg_len;
                        if ((cfg_code & 0x80) == 0) {
                            cfg_rej_len += cfg_len + L2CAP_CFG_OPTION_OVERHEAD;
                            cfg_rej = TRUE;
                        }
                    }
                    /* bad length; force loop exit */
                    else {
                        p = p_cfg_end;
                        cfg_rej = TRUE;
                    }
                    break;
                }
            }

            if ((p_ccb = l2cu_find_ccb_by_cid (p_lcb, lcid)) != NULL) {
                p_ccb->remote_id = id;
                if (cfg_rej) {
                    l2cu_send_peer_config_rej (p_ccb, p_cfg_start, (UINT16) (cmd_len - L2CAP_CONFIG_REQ_LEN), cfg_rej_len);
                } else {
                    l2c_csm_execute (p_ccb, L2CEVT_L2CAP_CONFIG_REQ, &cfg_info);
                }
            } else {
                /* updated spec says send command reject on invalid cid */
                l2cu_send_peer_cmd_reject (p_lcb, L2CAP_CMD_REJ_INVALID_CID, id, 0, 0);
            }
            break;

        case L2CAP_CMD_CONFIG_RSP:
            p_cfg_end = p + cmd_len;
            STREAM_TO_UINT16 (lcid, p);
            STREAM_TO_UINT16 (cfg_info.flags, p);
            STREAM_TO_UINT16 (cfg_info.result, p);

            cfg_info.flush_to_present = cfg_info.mtu_present = cfg_info.qos_present =
                                            cfg_info.fcr_present = cfg_info.fcs_present = FALSE;

            while (p < p_cfg_end) {
                STREAM_TO_UINT8 (cfg_code, p);
                STREAM_TO_UINT8 (cfg_len, p);

                switch (cfg_code & 0x7F) {
                case L2CAP_CFG_TYPE_MTU:
                    cfg_info.mtu_present = TRUE;
                    STREAM_TO_UINT16 (cfg_info.mtu, p);
                    break;

                case L2CAP_CFG_TYPE_FLUSH_TOUT:
                    cfg_info.flush_to_present = TRUE;
                    STREAM_TO_UINT16 (cfg_info.flush_to, p);
                    break;

                case L2CAP_CFG_TYPE_QOS:
                    cfg_info.qos_present = TRUE;
                    STREAM_TO_UINT8  (cfg_info.qos.qos_flags, p);
                    STREAM_TO_UINT8  (cfg_info.qos.service_type, p);
                    STREAM_TO_UINT32 (cfg_info.qos.token_rate, p);
                    STREAM_TO_UINT32 (cfg_info.qos.token_bucket_size, p);
                    STREAM_TO_UINT32 (cfg_info.qos.peak_bandwidth, p);
                    STREAM_TO_UINT32 (cfg_info.qos.latency, p);
                    STREAM_TO_UINT32 (cfg_info.qos.delay_variation, p);
                    break;

                case L2CAP_CFG_TYPE_FCR:
                    cfg_info.fcr_present = TRUE;
                    STREAM_TO_UINT8 (cfg_info.fcr.mode, p);
                    STREAM_TO_UINT8 (cfg_info.fcr.tx_win_sz, p);
                    STREAM_TO_UINT8 (cfg_info.fcr.max_transmit, p);
                    STREAM_TO_UINT16 (cfg_info.fcr.rtrans_tout, p);
                    STREAM_TO_UINT16 (cfg_info.fcr.mon_tout, p);
                    STREAM_TO_UINT16 (cfg_info.fcr.mps, p);
                    break;

                case L2CAP_CFG_TYPE_FCS:
                    cfg_info.fcs_present = TRUE;
                    STREAM_TO_UINT8 (cfg_info.fcs, p);
                    break;

                case L2CAP_CFG_TYPE_EXT_FLOW:
                    cfg_info.ext_flow_spec_present = TRUE;
                    STREAM_TO_UINT8  (cfg_info.ext_flow_spec.id, p);
                    STREAM_TO_UINT8  (cfg_info.ext_flow_spec.stype, p);
                    STREAM_TO_UINT16 (cfg_info.ext_flow_spec.max_sdu_size, p);
                    STREAM_TO_UINT32 (cfg_info.ext_flow_spec.sdu_inter_time, p);
                    STREAM_TO_UINT32 (cfg_info.ext_flow_spec.access_latency, p);
                    STREAM_TO_UINT32 (cfg_info.ext_flow_spec.flush_timeout, p);
                    break;
                }
            }

            if ((p_ccb = l2cu_find_ccb_by_cid (p_lcb, lcid)) != NULL) {
                if (p_ccb->local_id != id) {
                    L2CAP_TRACE_WARNING ("L2CAP - cfg rsp - bad ID. Exp: %d Got: %d",
                                         p_ccb->local_id, id);
                    break;
                }
                if ( (cfg_info.result == L2CAP_CFG_OK) || (cfg_info.result == L2CAP_CFG_PENDING) ) {
                    l2c_csm_execute (p_ccb, L2CEVT_L2CAP_CONFIG_RSP, &cfg_info);
                } else {
                    l2c_csm_execute (p_ccb, L2CEVT_L2CAP_CONFIG_RSP_NEG, &cfg_info);
                }
            } else {
                L2CAP_TRACE_WARNING ("L2CAP - rcvd cfg rsp for unknown CID: 0x%04x", lcid);
            }
            break;


        case L2CAP_CMD_DISC_REQ:
            STREAM_TO_UINT16 (lcid, p);
            STREAM_TO_UINT16 (rcid, p);

            if ((p_ccb = l2cu_find_ccb_by_cid (p_lcb, lcid)) != NULL) {
                if (p_ccb->remote_cid == rcid) {
                    p_ccb->remote_id = id;
                    l2c_csm_execute (p_ccb, L2CEVT_L2CAP_DISCONNECT_REQ, &con_info);
                }
            } else {
                l2cu_send_peer_disc_rsp (p_lcb, id, lcid, rcid);
            }

            break;

        case L2CAP_CMD_DISC_RSP:
            STREAM_TO_UINT16 (rcid, p);
            STREAM_TO_UINT16 (lcid, p);

            if ((p_ccb = l2cu_find_ccb_by_cid (p_lcb, lcid)) != NULL) {
                if ((p_ccb->remote_cid == rcid) && (p_ccb->local_id == id)) {
                    l2c_csm_execute (p_ccb, L2CEVT_L2CAP_DISCONNECT_RSP, &con_info);
                }
            }
            break;

        case L2CAP_CMD_ECHO_REQ:
            l2cu_send_peer_echo_rsp (p_lcb, id, NULL, 0);
            break;

        case L2CAP_CMD_ECHO_RSP:
            if (p_lcb->p_echo_rsp_cb) {
                tL2CA_ECHO_RSP_CB *p_cb = p_lcb->p_echo_rsp_cb;

                /* Zero out the callback in case app immediately calls us again */
                p_lcb->p_echo_rsp_cb = NULL;

                (*p_cb) (L2CAP_PING_RESULT_OK);
            }
            break;

        case L2CAP_CMD_INFO_REQ:
            STREAM_TO_UINT16 (info_type, p);
            l2cu_send_peer_info_rsp (p_lcb, id, info_type);
            break;

        case L2CAP_CMD_INFO_RSP:
            /* Stop the link connect timer if sent before L2CAP connection is up */
            if (p_lcb->w4_info_rsp) {
                btu_stop_timer (&p_lcb->info_timer_entry);
                p_lcb->w4_info_rsp = FALSE;
            }

            STREAM_TO_UINT16 (info_type, p);
            STREAM_TO_UINT16 (result, p);

            p_lcb->info_rx_bits |= (1 << info_type);

            if ( (info_type == L2CAP_EXTENDED_FEATURES_INFO_TYPE)
                    && (result == L2CAP_INFO_RESP_RESULT_SUCCESS) ) {
                STREAM_TO_UINT32( p_lcb->peer_ext_fea, p );

#if (L2CAP_NUM_FIXED_CHNLS > 0)
                if (p_lcb->peer_ext_fea & L2CAP_EXTFEA_FIXED_CHNLS) {
                    l2cu_send_peer_info_req (p_lcb, L2CAP_FIXED_CHANNELS_INFO_TYPE);
                    break;
                } else {
                    l2cu_process_fixed_chnl_resp (p_lcb);
                }
#endif
            }


#if (L2CAP_NUM_FIXED_CHNLS > 0)
            if (info_type == L2CAP_FIXED_CHANNELS_INFO_TYPE) {
                if (result == L2CAP_INFO_RESP_RESULT_SUCCESS) {
                    memcpy (p_lcb->peer_chnl_mask, p, L2CAP_FIXED_CHNL_ARRAY_SIZE);
                }

                l2cu_process_fixed_chnl_resp (p_lcb);
            }
#endif
#if (L2CAP_UCD_INCLUDED == TRUE)
            else if (info_type == L2CAP_CONNLESS_MTU_INFO_TYPE) {
                if (result == L2CAP_INFO_RESP_RESULT_SUCCESS) {
                    STREAM_TO_UINT16 (p_lcb->ucd_mtu, p);
                }
            }
#endif

            ci.status = HCI_SUCCESS;
            memcpy (ci.bd_addr, p_lcb->remote_bd_addr, sizeof(BD_ADDR));
            for (p_ccb = p_lcb->ccb_queue.p_first_ccb; p_ccb; p_ccb = p_ccb->p_next_ccb) {
                l2c_csm_execute (p_ccb, L2CEVT_L2CAP_INFO_RSP, &ci);
            }
            break;

        default:
            L2CAP_TRACE_WARNING ("L2CAP - bad cmd code: %d", cmd_code);
            l2cu_send_peer_cmd_reject (p_lcb, L2CAP_CMD_REJ_NOT_UNDERSTOOD, id, 0, 0);
            return;
        }
    }

}
#endif  ///CLASSIC_BT_INCLUDED == TRUE


/*******************************************************************************
**
** Function         l2c_process_held_packets
**
** Description      This function processes any L2CAP packets that arrived before
**                  the HCI connection complete arrived. It is a work around for
**                  badly behaved controllers.
**
** Returns          void
**
*******************************************************************************/
void l2c_process_held_packets(BOOLEAN timed_out)
{
    if (list_is_empty(l2cb.rcv_pending_q)) {
        return;
    }

    if (!timed_out) {
        btu_stop_timer(&l2cb.rcv_hold_tle);
        L2CAP_TRACE_WARNING("L2CAP HOLD CONTINUE");
    } else {
        L2CAP_TRACE_WARNING("L2CAP HOLD TIMEOUT");
    }

    for (const list_node_t *node = list_begin(l2cb.rcv_pending_q);
            node != list_end(l2cb.rcv_pending_q);)  {
        BT_HDR *p_buf = list_node(node);
        node = list_next(node);
        if (!timed_out || (!p_buf->layer_specific) || (--p_buf->layer_specific == 0)) {
            list_remove(l2cb.rcv_pending_q, p_buf);
            p_buf->layer_specific = 0xFFFF;
            l2c_rcv_acl_data(p_buf);
        }
    }

    /* If anyone still in the queue, restart the timeout */
    if (!list_is_empty(l2cb.rcv_pending_q)) {
        btu_start_timer (&l2cb.rcv_hold_tle, BTU_TTYPE_L2CAP_HOLD, BT_1SEC_TIMEOUT);
    }
}


/*******************************************************************************
**
** Function         l2c_init
**
** Description      This function is called once at startup to initialize
**                  all the L2CAP structures
**
** Returns          void
**
*******************************************************************************/
void l2c_init (void)
{
    INT16  xx;

    memset (&l2cb, 0, sizeof (tL2C_CB));
    /* the psm is increased by 2 before being used */
    l2cb.dyn_psm = 0xFFF;

    /* Put all the channel control blocks on the free queue */
    for (xx = 0; xx < MAX_L2CAP_CHANNELS - 1; xx++) {
        l2cb.ccb_pool[xx].p_next_ccb = &l2cb.ccb_pool[xx + 1];
    }

#if (L2CAP_NON_FLUSHABLE_PB_INCLUDED == TRUE)
    /* it will be set to L2CAP_PKT_START_NON_FLUSHABLE if controller supports */
    l2cb.non_flushable_pbf = L2CAP_PKT_START << L2CAP_PKT_TYPE_SHIFT;
#endif


    l2cb.p_free_ccb_first = &l2cb.ccb_pool[0];
    l2cb.p_free_ccb_last  = &l2cb.ccb_pool[MAX_L2CAP_CHANNELS - 1];

#ifdef L2CAP_DESIRED_LINK_ROLE
    l2cb.desire_role      = L2CAP_DESIRED_LINK_ROLE;
#else
    l2cb.desire_role      = HCI_ROLE_SLAVE;
#endif

    /* Set the default idle timeout */
    l2cb.idle_timeout = L2CAP_LINK_INACTIVITY_TOUT;

#if defined(L2CAP_INITIAL_TRACE_LEVEL)
    l2cb.l2cap_trace_level = L2CAP_INITIAL_TRACE_LEVEL;
#else
    l2cb.l2cap_trace_level = BT_TRACE_LEVEL_NONE;    /* No traces */
#endif

#if L2CAP_CONFORMANCE_TESTING == TRUE
    /* Conformance testing needs a dynamic response */
    l2cb.test_info_resp = L2CAP_EXTFEA_SUPPORTED_MASK;
#endif

    /* Number of ACL buffers to use for high priority channel */
#if (defined(L2CAP_HIGH_PRI_CHAN_QUOTA_IS_CONFIGURABLE) && (L2CAP_HIGH_PRI_CHAN_QUOTA_IS_CONFIGURABLE == TRUE))
    l2cb.high_pri_min_xmit_quota = L2CAP_HIGH_PRI_MIN_XMIT_QUOTA;
#endif

#if BLE_INCLUDED == TRUE
    l2cb.l2c_ble_fixed_chnls_mask =
        L2CAP_FIXED_CHNL_ATT_BIT | L2CAP_FIXED_CHNL_BLE_SIG_BIT | L2CAP_FIXED_CHNL_SMP_BIT;
#endif

    l2cb.rcv_pending_q = list_new(NULL);
    if (l2cb.rcv_pending_q == NULL) {
        LOG_ERROR("%s unable to allocate memory for link layer control block", __func__);
    }
}

void l2c_free(void)
{
    list_free(l2cb.rcv_pending_q);
    l2cb.rcv_pending_q = NULL;
}

/*******************************************************************************
**
** Function         l2c_process_timeout
**
** Description      This function is called when an L2CAP-related timeout occurs
**
** Returns          void
**
*******************************************************************************/
void l2c_process_timeout (TIMER_LIST_ENT *p_tle)
{
    /* What type of timeout ? */
    switch (p_tle->event) {
    case BTU_TTYPE_L2CAP_LINK:
        l2c_link_timeout ((tL2C_LCB *)p_tle->param);
        break;
#if (CLASSIC_BT_INCLUDED == TRUE)
    case BTU_TTYPE_L2CAP_CHNL:
        l2c_csm_execute (((tL2C_CCB *)p_tle->param), L2CEVT_TIMEOUT, NULL);
        break;

    case BTU_TTYPE_L2CAP_FCR_ACK:
        l2c_csm_execute (((tL2C_CCB *)p_tle->param), L2CEVT_ACK_TIMEOUT, NULL);
        break;
#endif  ///CLASSIC_BT_INCLUDED == TRUE
    case BTU_TTYPE_L2CAP_HOLD:
        /* Update the timeouts in the hold queue */
        l2c_process_held_packets(TRUE);
        break;

    case BTU_TTYPE_L2CAP_INFO:
        l2c_info_timeout((tL2C_LCB *)p_tle->param);
        break;
    case BTU_TTYPE_L2CAP_UPDA_CONN_PARAMS: {
        UINT8 status = HCI_ERR_HOST_TIMEOUT;
        tL2C_LCB *p_lcb = (tL2C_LCB *)p_tle->param;
        if (p_lcb){
            p_lcb->conn_update_mask &= ~L2C_BLE_UPDATE_PENDING;
            p_lcb->conn_update_mask &= ~L2C_BLE_UPDATE_PARAM_FULL;
        }
        l2c_send_update_conn_params_cb(p_lcb, status);
        break;
    }
    }
}

/*******************************************************************************
**
** Function         l2c_data_write
**
** Description      API functions call this function to write data.
**
** Returns          L2CAP_DW_SUCCESS, if data accepted, else FALSE
**                  L2CAP_DW_CONGESTED, if data accepted and the channel is congested
**                  L2CAP_DW_FAILED, if error
**
*******************************************************************************/
#if (CLASSIC_BT_INCLUDED == TRUE)
UINT8 l2c_data_write (UINT16 cid, BT_HDR *p_data, UINT16 flags)
{
    tL2C_CCB        *p_ccb;

    /* Find the channel control block. We don't know the link it is on. */
    if ((p_ccb = l2cu_find_ccb_by_cid (NULL, cid)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no CCB for L2CA_DataWrite, CID: %d", cid);
        osi_free (p_data);
        return (L2CAP_DW_FAILED);
    }

#ifndef TESTER /* Tester may send any amount of data. otherwise sending message
                  bigger than mtu size of peer is a violation of protocol */
    if (p_data->len > p_ccb->peer_cfg.mtu) {
        L2CAP_TRACE_WARNING ("L2CAP - CID: 0x%04x  cannot send message bigger than peer's mtu size", cid);
        osi_free (p_data);
        return (L2CAP_DW_FAILED);
    }
#endif

    /* channel based, packet based flushable or non-flushable */
    p_data->layer_specific = flags;

    /* If already congested, do not accept any more packets */
    if (p_ccb->cong_sent) {
        L2CAP_TRACE_ERROR ("L2CAP - CID: 0x%04x cannot send, already congested  xmit_hold_q.count: %u  buff_quota: %u",
                           p_ccb->local_cid,
                           fixed_queue_length(p_ccb->xmit_hold_q),
                           p_ccb->buff_quota);

        osi_free (p_data);
        return (L2CAP_DW_FAILED);
    }

    //counter_add("l2cap.dyn.tx.bytes", p_data->len);
    //counter_add("l2cap.dyn.tx.pkts", 1);

    l2c_csm_execute (p_ccb, L2CEVT_L2CA_DATA_WRITE, p_data);

    if (p_ccb->cong_sent) {
        return (L2CAP_DW_CONGESTED);
    }
    return (L2CAP_DW_SUCCESS);
}
#endif  ///CLASSIC_BT_INCLUDED == TRUE



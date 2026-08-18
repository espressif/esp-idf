/******************************************************************************
 *
 *  Copyright (C) 2001-2012 Broadcom Corporation
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
 *  this file contains the main BNEP functions
 *
 ******************************************************************************/

#include "common/bt_target.h"

#if (BNEP_INCLUDED == TRUE)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stack/bt_types.h"
#include "stack/hcidefs.h"
#include "stack/hcimsgs.h"

#include "stack/l2c_api.h"
#include "stack/l2cdefs.h"

#include "device/controller.h"

#include "osi/allocator.h"
#include "osi/fixed_queue.h"

#include "stack/btm_api.h"
#include "stack/btu.h"

#include "stack/bnep_api.h"
#include "bnep_int.h"

extern fixed_queue_t* btu_general_alarm_queue;

/******************************************************************************/
/*                     G L O B A L    B N E P       D A T A                   */
/******************************************************************************/
tBNEP_CB bnep_cb;

const UINT16 bnep_frame_hdr_sizes[] = {14, 1, 2, 8, 8};

/******************************************************************************/
/*            L O C A L    F U N C T I O N     P R O T O T Y P E S            */
/******************************************************************************/
static void bnep_connect_ind(BD_ADDR bd_addr, UINT16 l2cap_cid,
                             UINT16 psm, UINT8 l2cap_id);
static void bnep_connect_cfm(UINT16 l2cap_cid, UINT16 result);
static void bnep_config_ind(UINT16 l2cap_cid, tL2CAP_CFG_INFO* p_cfg);
static void bnep_config_cfm(UINT16 l2cap_cid, tL2CAP_CFG_INFO* p_cfg);
static void bnep_disconnect_ind(UINT16 l2cap_cid, BOOLEAN ack_needed);
static void bnep_disconnect_cfm(UINT16 l2cap_cid, UINT16 result);
static void bnep_data_ind(UINT16 l2cap_cid, BT_HDR * p_msg);
static void bnep_congestion_ind(UINT16 lcid, BOOLEAN is_congested);

/*******************************************************************************
 *
 * Function         bnep_register_with_l2cap
 *
 * Description      This function registers BNEP PSM with L2CAP
 *
 * Returns          void
 *
 ******************************************************************************/
tBNEP_RESULT bnep_register_with_l2cap(void)
{
    /* Initialize the L2CAP configuration. We only care about MTU and flush */
    memset(&bnep_cb.l2cap_my_cfg, 0, sizeof(tL2CAP_CFG_INFO));

    bnep_cb.l2cap_my_cfg.mtu_present = TRUE;
    bnep_cb.l2cap_my_cfg.mtu = BNEP_MTU_SIZE;
    bnep_cb.l2cap_my_cfg.flush_to_present = TRUE;
    bnep_cb.l2cap_my_cfg.flush_to = BNEP_FLUSH_TO;

    bnep_cb.reg_info.pL2CA_ConnectInd_Cb = bnep_connect_ind;
    bnep_cb.reg_info.pL2CA_ConnectCfm_Cb = bnep_connect_cfm;
    bnep_cb.reg_info.pL2CA_ConfigInd_Cb = bnep_config_ind;
    bnep_cb.reg_info.pL2CA_ConfigCfm_Cb = bnep_config_cfm;
    bnep_cb.reg_info.pL2CA_DisconnectInd_Cb = bnep_disconnect_ind;
    bnep_cb.reg_info.pL2CA_DisconnectCfm_Cb = bnep_disconnect_cfm;
    bnep_cb.reg_info.pL2CA_DataInd_Cb = bnep_data_ind;
    bnep_cb.reg_info.pL2CA_CongestionStatus_Cb = bnep_congestion_ind;

    /* Now, register with L2CAP */
    if (!L2CA_Register(BT_PSM_BNEP, &bnep_cb.reg_info)) {
        BNEP_TRACE_ERROR("BNEP - Registration failed");
        return BNEP_SECURITY_FAIL;
    }

    return BNEP_SUCCESS;
}

/*******************************************************************************
 *
 * Function         bnep_connect_ind
 *
 * Description      This function handles an inbound connection indication
 *                  from L2CAP. This is the case where we are acting as a
 *                  server.
 *
 * Returns          void
 *
 ******************************************************************************/
static void bnep_connect_ind(BD_ADDR bd_addr, UINT16 l2cap_cid,
                             UNUSED_ATTR UINT16 psm, UINT8 l2cap_id)
{
    tBNEP_CONN * p_bcb = bnepu_find_bcb_by_bd_addr(bd_addr);

    /* If we are not acting as server, or already have a connection, or have */
    /* no more resources to handle the connection, reject the connection.    */
    if (!(bnep_cb.profile_registered) || (p_bcb) ||
            ((p_bcb = bnepu_allocate_bcb(bd_addr)) == NULL)) {
        L2CA_ConnectRsp(bd_addr, l2cap_id, l2cap_cid, L2CAP_CONN_NO_PSM, 0);
        return;
    }

    /* Transition to the next appropriate state, waiting for config setup. */
    p_bcb->con_state = BNEP_STATE_CFG_SETUP;

    /* Save the L2CAP Channel ID. */
    p_bcb->l2cap_cid = l2cap_cid;

    /* Send response to the L2CAP layer. */
    L2CA_ConnectRsp(bd_addr, l2cap_id, l2cap_cid, L2CAP_CONN_OK, L2CAP_CONN_OK);

    /* Send a Configuration Request. */
    L2CA_ConfigReq(l2cap_cid, &bnep_cb.l2cap_my_cfg);

    /* Start timer waiting for config setup */
    btu_start_timer(&p_bcb->conn_timer, BTU_TTYPE_BNEP, BNEP_CONN_TIMEOUT_SEC);

    BNEP_TRACE_EVENT("BNEP - Rcvd L2CAP conn ind, CID: 0x%x", p_bcb->l2cap_cid);
}

/*******************************************************************************
 *
 * Function         bnep_connect_cfm
 *
 * Description      This function handles the connect confirm events
 *                  from L2CAP. This is the case when we are acting as a
 *                  client and have sent a connect request.
 *
 * Returns          void
 *
 ******************************************************************************/
static void bnep_connect_cfm(UINT16 l2cap_cid, UINT16 result)
{
    tBNEP_CONN * p_bcb;

    /* Find CCB based on CID */
    p_bcb = bnepu_find_bcb_by_cid(l2cap_cid);
    if (p_bcb == NULL) {
        BNEP_TRACE_WARNING("BNEP - Rcvd conn cnf for unknown CID 0x%x", l2cap_cid);
        return;
    }

    /* If the connection response contains success status, then */
    /* Transition to the next state and startup the timer.      */
    if ((result == L2CAP_CONN_OK) &&
            (p_bcb->con_state == BNEP_STATE_CONN_START)) {
        p_bcb->con_state = BNEP_STATE_CFG_SETUP;

        /* Send a Configuration Request. */
        L2CA_ConfigReq(l2cap_cid, &bnep_cb.l2cap_my_cfg);

        /* Start timer waiting for config results */
        btu_start_timer(&p_bcb->conn_timer, BTU_TTYPE_BNEP, BNEP_CONN_TIMEOUT_SEC);

        BNEP_TRACE_EVENT("BNEP - got conn cnf, sent cfg req, CID: 0x%x",
                         p_bcb->l2cap_cid);
    } else {
        BNEP_TRACE_WARNING("BNEP - Rcvd conn cnf with error: 0x%x  CID 0x%x",
                           result, p_bcb->l2cap_cid);

        /* Tell the upper layer, if he has a callback */
        if (bnep_cb.p_conn_state_cb && p_bcb->con_flags & BNEP_FLAGS_IS_ORIG) {
            (*bnep_cb.p_conn_state_cb)(p_bcb->handle, p_bcb->rem_bda,
                                       BNEP_CONN_FAILED, FALSE);
        }

        bnepu_release_bcb(p_bcb);
    }
}

/*******************************************************************************
 *
 * Function         bnep_config_ind
 *
 * Description      This function processes the L2CAP configuration indication
 *                  event.
 *
 * Returns          void
 *
 ******************************************************************************/
static void bnep_config_ind(UINT16 l2cap_cid, tL2CAP_CFG_INFO *p_cfg)
{
    tBNEP_CONN * p_bcb;
    UINT16 result, mtu = 0;

    /* Find CCB based on CID */
    p_bcb = bnepu_find_bcb_by_cid(l2cap_cid);
    if (p_bcb == NULL) {
        BNEP_TRACE_WARNING("BNEP - Rcvd L2CAP cfg ind, unknown CID: 0x%x",
                           l2cap_cid);
        return;
    }

    BNEP_TRACE_EVENT("BNEP - Rcvd cfg ind, CID: 0x%x", l2cap_cid);

    /* Remember the remote MTU size */
    if ((!p_cfg->mtu_present) || (p_cfg->mtu < BNEP_MIN_MTU_SIZE)) {
        mtu = p_cfg->mtu;
        p_cfg->flush_to_present = FALSE;
        p_cfg->mtu_present = TRUE;
        p_cfg->mtu = BNEP_MIN_MTU_SIZE;
        p_cfg->result = result = L2CAP_CFG_UNACCEPTABLE_PARAMS;
    } else {
        if (p_cfg->mtu > BNEP_MTU_SIZE) {
            p_bcb->rem_mtu_size = BNEP_MTU_SIZE;
        } else {
            p_bcb->rem_mtu_size = p_cfg->mtu;
        }

        /* For now, always accept configuration from the other side */
        p_cfg->flush_to_present = FALSE;
        p_cfg->mtu_present = FALSE;
        p_cfg->result = result = L2CAP_CFG_OK;
    }

    L2CA_ConfigRsp(l2cap_cid, p_cfg);

    if (result != L2CAP_CFG_OK) {
        BNEP_TRACE_EVENT("BNEP - Rcvd cfg ind with bad MTU %d, CID: 0x%x", mtu,
                         l2cap_cid);
        return;
    }

    p_bcb->con_flags |= BNEP_FLAGS_HIS_CFG_DONE;

    if (p_bcb->con_flags & BNEP_FLAGS_MY_CFG_DONE) {
        p_bcb->con_state = BNEP_STATE_SEC_CHECKING;

        /* Start timer waiting for setup or response */
        btu_start_timer(&p_bcb->conn_timer, BTU_TTYPE_BNEP, BNEP_HOST_TIMEOUT_SEC);

        if (p_bcb->con_flags & BNEP_FLAGS_IS_ORIG) {
            btm_sec_mx_access_request(
                p_bcb->rem_bda, BT_PSM_BNEP, TRUE, BTM_SEC_PROTO_BNEP,
                bnep_get_uuid32(&(p_bcb->src_uuid)), &bnep_sec_check_complete, p_bcb);
        }
    }
}

/*******************************************************************************
 *
 * Function         bnep_config_cfm
 *
 * Description      This function processes the L2CAP configuration confirmation
 *                  event.
 *
 * Returns          void
 *
 ******************************************************************************/
static void bnep_config_cfm(UINT16 l2cap_cid, tL2CAP_CFG_INFO *p_cfg)
{
    tBNEP_CONN * p_bcb;

    BNEP_TRACE_EVENT("BNEP - Rcvd cfg cfm, CID: 0x%x  Result: %d", l2cap_cid,
                     p_cfg->result);

    /* Find CCB based on CID */
    p_bcb = bnepu_find_bcb_by_cid(l2cap_cid);
    if (p_bcb == NULL) {
        BNEP_TRACE_WARNING("BNEP - Rcvd L2CAP cfg ind, unknown CID: 0x%x",
                           l2cap_cid);
        return;
    }

    /* For now, always accept configuration from the other side */
    if (p_cfg->result == L2CAP_CFG_OK) {
        p_bcb->con_flags |= BNEP_FLAGS_MY_CFG_DONE;

        if (p_bcb->con_flags & BNEP_FLAGS_HIS_CFG_DONE) {
            p_bcb->con_state = BNEP_STATE_SEC_CHECKING;

            /* Start timer waiting for setup or response */
            btu_start_timer(&p_bcb->conn_timer, BTU_TTYPE_BNEP, BNEP_HOST_TIMEOUT_SEC);

            if (p_bcb->con_flags & BNEP_FLAGS_IS_ORIG) {
                btm_sec_mx_access_request(p_bcb->rem_bda, BT_PSM_BNEP, TRUE,
                                          BTM_SEC_PROTO_BNEP,
                                          bnep_get_uuid32(&(p_bcb->src_uuid)),
                                          &bnep_sec_check_complete, p_bcb);
            }
        }
    } else {
        /* Tell the upper layer, if he has a callback */
        if ((p_bcb->con_flags & BNEP_FLAGS_IS_ORIG) && (bnep_cb.p_conn_state_cb)) {
            (*bnep_cb.p_conn_state_cb)(p_bcb->handle, p_bcb->rem_bda,
                                       BNEP_CONN_FAILED_CFG, FALSE);
        }

        L2CA_DisconnectReq(p_bcb->l2cap_cid);

        bnepu_release_bcb(p_bcb);
    }
}

/*******************************************************************************
 *
 * Function         bnep_disconnect_ind
 *
 * Description      This function handles a disconnect event from L2CAP. If
 *                  requested to, we ack the disconnect before dropping the CCB
 *
 * Returns          void
 *
 ******************************************************************************/
static void bnep_disconnect_ind(UINT16 l2cap_cid, BOOLEAN ack_needed)
{
    tBNEP_CONN * p_bcb;

    if (ack_needed) {
        L2CA_DisconnectRsp(l2cap_cid);
    }

    /* Find CCB based on CID */
    p_bcb = bnepu_find_bcb_by_cid(l2cap_cid);
    if (p_bcb == NULL) {
        BNEP_TRACE_WARNING("BNEP - Rcvd L2CAP disc, unknown CID: 0x%x", l2cap_cid);
        return;
    }

    BNEP_TRACE_EVENT("BNEP - Rcvd L2CAP disc, CID: 0x%x", l2cap_cid);

    /* Tell the user if he has a callback */
    if (p_bcb->con_state == BNEP_STATE_CONNECTED) {
        if (bnep_cb.p_conn_state_cb)
            (*bnep_cb.p_conn_state_cb)(p_bcb->handle, p_bcb->rem_bda,
                                       BNEP_CONN_DISCONNECTED, FALSE);
    } else {
        if ((bnep_cb.p_conn_state_cb) &&
                ((p_bcb->con_flags & BNEP_FLAGS_IS_ORIG) ||
                 (p_bcb->con_flags & BNEP_FLAGS_CONN_COMPLETED)))
            (*bnep_cb.p_conn_state_cb)(p_bcb->handle, p_bcb->rem_bda,
                                       BNEP_CONN_FAILED, FALSE);
    }

    bnepu_release_bcb(p_bcb);
}

/*******************************************************************************
 *
 * Function         bnep_disconnect_cfm
 *
 * Description      This function gets the disconnect confirm event from L2CAP
 *
 * Returns          void
 *
 ******************************************************************************/
static void bnep_disconnect_cfm(UINT16 l2cap_cid, UINT16 result)
{
    BNEP_TRACE_EVENT("BNEP - Rcvd L2CAP disc cfm, CID: 0x%x, Result 0x%x",
                     l2cap_cid, result);
}

/*******************************************************************************
 *
 * Function         bnep_congestion_ind
 *
 * Description      This is a callback function called by L2CAP when
 *                  congestion status changes
 *
 ******************************************************************************/
static void bnep_congestion_ind(UINT16 l2cap_cid, BOOLEAN is_congested)
{
    tBNEP_CONN * p_bcb;

    /* Find BCB based on CID */
    p_bcb = bnepu_find_bcb_by_cid(l2cap_cid);
    if (p_bcb == NULL) {
        BNEP_TRACE_WARNING("BNEP - Rcvd L2CAP cong, unknown CID: 0x%x", l2cap_cid);
        return;
    }

    if (is_congested) {
        p_bcb->con_flags |= BNEP_FLAGS_L2CAP_CONGESTED;
        if (bnep_cb.p_tx_data_flow_cb) {
            bnep_cb.p_tx_data_flow_cb(p_bcb->handle, BNEP_TX_FLOW_OFF);
        }
    } else {
        p_bcb->con_flags &= ~BNEP_FLAGS_L2CAP_CONGESTED;

        if (bnep_cb.p_tx_data_flow_cb) {
            bnep_cb.p_tx_data_flow_cb(p_bcb->handle, BNEP_TX_FLOW_ON);
        }

        /* While not congested, send as many buffers as we can */
        while (!(p_bcb->con_flags & BNEP_FLAGS_L2CAP_CONGESTED)) {
            BT_HDR * p_buf = (BT_HDR *)fixed_queue_dequeue(p_bcb->xmit_q, 0);

            if (!p_buf) {
                break;
            }

            L2CA_DataWrite(l2cap_cid, p_buf);
        }
    }
}

/*******************************************************************************
 *
 * Function         bnep_data_ind
 *
 * Description      This function is called when data is received from L2CAP.
 *                  if we are the originator of the connection, we are the SDP
 *                  client, and the received message is queued for the client.
 *
 *                  If we are the destination of the connection, we are the SDP
 *                  server, so the message is passed to the server processing
 *                  function.
 *
 * Returns          void
 *
 ******************************************************************************/
static void bnep_data_ind(UINT16 l2cap_cid, BT_HDR *p_buf)
{
    tBNEP_CONN *p_bcb;
    UINT8 *p = (UINT8 *)(p_buf + 1) + p_buf->offset;
    UINT16 rem_len = p_buf->len;
    UINT8 type, ctrl_type, ext_type = 0;
    BOOLEAN extension_present, fw_ext_present;
    UINT16 protocol = 0;

    /* Find CCB based on CID */
    p_bcb = bnepu_find_bcb_by_cid(l2cap_cid);
    if (p_bcb == NULL) {
        BNEP_TRACE_WARNING("BNEP - Rcvd L2CAP data, unknown CID: 0x%x", l2cap_cid);
        osi_free(p_buf);
        return;
    }

    /* Check rem_len before any reading from the buffer */
    if (rem_len == 0) {
        BNEP_TRACE_EVENT("BNEP - rcvd frame, bad len: %d", p_buf->len);
        osi_free(p_buf);
        return;
    }

    /* Get the type and extension bits */
    type = *p++;
    extension_present = type >> 7;
    type &= 0x7f;
    if (type > BNEP_FRAME_COMPRESSED_ETHERNET_DEST_ONLY ||
            (rem_len <= bnep_frame_hdr_sizes[type]) || (rem_len > BNEP_MTU_SIZE)) {
        BNEP_TRACE_EVENT("BNEP - rcvd frame, bad len: %d  type: 0x%02x", p_buf->len,
                         type);
        osi_free(p_buf);
        return;
    }

    rem_len--;

    if ((p_bcb->con_state != BNEP_STATE_CONNECTED) &&
            (!(p_bcb->con_flags & BNEP_FLAGS_CONN_COMPLETED)) &&
            (type != BNEP_FRAME_CONTROL)) {
        BNEP_TRACE_WARNING(
            "BNEP - Ignored L2CAP data while in state: %d, CID: 0x%x",
            p_bcb->con_state, l2cap_cid);

        if (extension_present) {
            /*
            ** When there is no connection if a data packet is received
            ** with unknown control extension headers then those should be processed
            ** according to complain/ignore law.
            ** Extensions follow the type-specific networking header (e.g. 14-byte
            ** Ethernet header for GENERAL_ETHERNET), not the BNEP type byte.
            */
            UINT8 ext, length;
            UINT16 org_len, new_len;
            UINT16 hdr_len = bnep_frame_hdr_sizes[type];

            if (rem_len < hdr_len) {
                osi_free(p_buf);
                return;
            }
            p += hdr_len;
            org_len = rem_len - hdr_len;
            do {
                if (org_len < 2) {
                    break;
                }
                ext = *p++;
                length = *p++;

                new_len = (length + 2);
                if (new_len > org_len) {
                    break;
                }

                if ((ext & 0x7F) == BNEP_EXTENSION_FILTER_CONTROL) {
                    if (length == 0) {
                        break;
                    }
                    if (*p > BNEP_FILTER_MULTI_ADDR_RESPONSE_MSG) {
                        bnep_send_command_not_understood(p_bcb, *p);
                    }
                }

                p += length;
                org_len -= new_len;

            } while (ext & 0x80);
        }

        osi_free(p_buf);
        return;
    }

    BNEP_TRACE_DEBUG("BNEP - rcv frame, type: %d len: %d Ext: %d", type,
                     p_buf->len, extension_present);

    /* Initialize addresses to 'not supplied' */
    UINT8 *p_src_addr, *p_dst_addr;
    p_src_addr = p_dst_addr = NULL;

    switch (type) {
    case BNEP_FRAME_GENERAL_ETHERNET:
        p_dst_addr = p;
        p += BD_ADDR_LEN;
        p_src_addr = p;
        p += BD_ADDR_LEN;
        BE_STREAM_TO_UINT16(protocol, p);
        rem_len -= 14;
        break;

    case BNEP_FRAME_CONTROL:
        ctrl_type = *p;
        p = bnep_process_control_packet(p_bcb, p, &rem_len, FALSE);

        /* Failed setup response releases the BCB; do not parse extensions. */
        if (p_bcb->con_state == BNEP_STATE_IDLE) {
            osi_free(p_buf);
            return;
        }

        if (ctrl_type == BNEP_SETUP_CONNECTION_REQUEST_MSG &&
                p_bcb->con_state != BNEP_STATE_CONNECTED && extension_present && p &&
                rem_len) {
            /* Allocate BT_HDR header plus payload; data is stored after the header. */
            if (p_bcb->p_pending_data) {
                osi_free(p_bcb->p_pending_data);
                p_bcb->p_pending_data = NULL;
            }
            p_bcb->p_pending_data = (BT_HDR *)osi_malloc(sizeof(BT_HDR) + rem_len);
            if (p_bcb->p_pending_data) {
                memcpy((UINT8 *)(p_bcb->p_pending_data + 1), p, rem_len);
                p_bcb->p_pending_data->len = rem_len;
                p_bcb->p_pending_data->offset = 0;
            }
        } else {
            while (extension_present && p && rem_len) {
                ext_type = *p++;
                rem_len--;
                extension_present = ext_type >> 7;
                ext_type &= 0x7F;

                /* if unknown extension present stop processing */
                if (ext_type) {
                    break;
                }

                p = bnep_process_control_packet(p_bcb, p, &rem_len, TRUE);
                if (p_bcb->con_state == BNEP_STATE_IDLE) {
                    break;
                }
            }
        }
        osi_free(p_buf);
        return;

    case BNEP_FRAME_COMPRESSED_ETHERNET:
        BE_STREAM_TO_UINT16(protocol, p);
        rem_len -= 2;
        break;

    case BNEP_FRAME_COMPRESSED_ETHERNET_SRC_ONLY:
        p_src_addr = p;
        p += BD_ADDR_LEN;
        BE_STREAM_TO_UINT16(protocol, p);
        rem_len -= 8;
        break;

    case BNEP_FRAME_COMPRESSED_ETHERNET_DEST_ONLY:
        p_dst_addr = p;
        p += BD_ADDR_LEN;
        BE_STREAM_TO_UINT16(protocol, p);
        rem_len -= 8;
        break;
    }

    /* Process the header extension if there is one */
    while (extension_present && p && rem_len) {
        ext_type = *p;
        extension_present = ext_type >> 7;
        ext_type &= 0x7F;

        /* if unknown extension present stop processing */
        if (ext_type) {
            BNEP_TRACE_EVENT("Data extension type 0x%x found", ext_type);
            break;
        }

        p++;
        rem_len--;
        p = bnep_process_control_packet(p_bcb, p, &rem_len, TRUE);
        if (p_bcb->con_state == BNEP_STATE_IDLE) {
            osi_free(p_buf);
            return;
        }
    }

    p_buf->offset += p_buf->len - rem_len;
    p_buf->len = rem_len;

    /* Always give the upper layer MAC addresses */
    if (!p_src_addr) {
        p_src_addr = (UINT8 *)p_bcb->rem_bda;
    }

    if (!p_dst_addr) {
        p_dst_addr = (UINT8 *)controller_get_interface()->get_address();
    }

    /* check whether there are any extensions to be forwarded */
    if (ext_type) {
        fw_ext_present = TRUE;
    } else {
        fw_ext_present = FALSE;
    }

    if (bnep_cb.p_data_buf_cb) {
        (*bnep_cb.p_data_buf_cb)(p_bcb->handle, p_src_addr, p_dst_addr, protocol,
                                 p_buf, fw_ext_present);
    } else if (bnep_cb.p_data_ind_cb) {
        (*bnep_cb.p_data_ind_cb)(p_bcb->handle, p_src_addr, p_dst_addr, protocol,
                                 p, rem_len, fw_ext_present);
        osi_free(p_buf);
    } else {
        osi_free(p_buf);
    }
}

/*******************************************************************************
 *
 * Function         bnep_conn_timer_timeout
 *
 * Description      This function processes a timeout. If it is a startup
 *                  timeout, we check for reading our BD address. If it
 *                  is an L2CAP timeout, we send a disconnect req to L2CAP.
 *
 * Returns          void
 *
 ******************************************************************************/
void bnep_conn_timer_timeout(void *data)
{
    tBNEP_CONN * p_bcb = (tBNEP_CONN *)data;

    BNEP_TRACE_EVENT(
        "BNEP - CCB timeout in state: %d  CID: 0x%x flags %x, re_transmit %d",
        p_bcb->con_state, p_bcb->l2cap_cid, p_bcb->con_flags,
        p_bcb->re_transmits);

    if (p_bcb->con_state == BNEP_STATE_CONN_SETUP) {
        BNEP_TRACE_EVENT("BNEP - CCB timeout in state: %d  CID: 0x%x",
                         p_bcb->con_state, p_bcb->l2cap_cid);

        if (!(p_bcb->con_flags & BNEP_FLAGS_IS_ORIG)) {
            L2CA_DisconnectReq(p_bcb->l2cap_cid);

            bnepu_release_bcb(p_bcb);
            return;
        }

        if (p_bcb->re_transmits++ != BNEP_MAX_RETRANSMITS) {
            bnep_send_conn_req(p_bcb);
            btu_start_timer(&p_bcb->conn_timer, BTU_TTYPE_BNEP, BNEP_CONN_TIMEOUT_SEC);
        } else {
            L2CA_DisconnectReq(p_bcb->l2cap_cid);

            if ((p_bcb->con_flags & BNEP_FLAGS_IS_ORIG) && (bnep_cb.p_conn_state_cb))
                (*bnep_cb.p_conn_state_cb)(p_bcb->handle, p_bcb->rem_bda,
                                           BNEP_CONN_FAILED, FALSE);

            bnepu_release_bcb(p_bcb);
            return;
        }
    } else if (p_bcb->con_state != BNEP_STATE_CONNECTED) {
        BNEP_TRACE_EVENT("BNEP - CCB timeout in state: %d  CID: 0x%x",
                         p_bcb->con_state, p_bcb->l2cap_cid);

        L2CA_DisconnectReq(p_bcb->l2cap_cid);

        /* Tell the user if he has a callback */
        if ((p_bcb->con_flags & BNEP_FLAGS_IS_ORIG) && (bnep_cb.p_conn_state_cb))
            (*bnep_cb.p_conn_state_cb)(p_bcb->handle, p_bcb->rem_bda,
                                       BNEP_CONN_FAILED, FALSE);

        bnepu_release_bcb(p_bcb);
    } else if (p_bcb->con_flags & BNEP_FLAGS_FILTER_RESP_PEND) {
        if (p_bcb->re_transmits++ != BNEP_MAX_RETRANSMITS) {
            bnepu_send_peer_our_filters(p_bcb);
            btu_start_timer(&p_bcb->conn_timer, BTU_TTYPE_BNEP, BNEP_FILTER_SET_TIMEOUT_SEC);
        } else {
            L2CA_DisconnectReq(p_bcb->l2cap_cid);

            /* Tell the user if he has a callback */
            if (bnep_cb.p_conn_state_cb)
                (*bnep_cb.p_conn_state_cb)(p_bcb->handle, p_bcb->rem_bda,
                                           BNEP_SET_FILTER_FAIL, FALSE);

            bnepu_release_bcb(p_bcb);
            return;
        }
    } else if (p_bcb->con_flags & BNEP_FLAGS_MULTI_RESP_PEND) {
        if (p_bcb->re_transmits++ != BNEP_MAX_RETRANSMITS) {
            bnepu_send_peer_our_multi_filters(p_bcb);
            btu_start_timer(&p_bcb->conn_timer, BTU_TTYPE_BNEP, BNEP_FILTER_SET_TIMEOUT_SEC);
        } else {
            L2CA_DisconnectReq(p_bcb->l2cap_cid);

            /* Tell the user if he has a callback */
            if (bnep_cb.p_conn_state_cb)
                (*bnep_cb.p_conn_state_cb)(p_bcb->handle, p_bcb->rem_bda,
                                           BNEP_SET_FILTER_FAIL, FALSE);

            bnepu_release_bcb(p_bcb);
            return;
        }
    }
}

/*******************************************************************************
 *
 * Function         bnep_connected
 *
 * Description      This function is called when a connection is established
 *                  (after config).
 *
 * Returns          void
 *
 ******************************************************************************/
void bnep_connected(tBNEP_CONN *p_bcb)
{
    BOOLEAN is_role_change;

    if (p_bcb->con_flags & BNEP_FLAGS_CONN_COMPLETED) {
        is_role_change = TRUE;
    } else {
        is_role_change = FALSE;
    }

    p_bcb->con_state = BNEP_STATE_CONNECTED;
    p_bcb->con_flags |= BNEP_FLAGS_CONN_COMPLETED;
    p_bcb->con_flags &= (~BNEP_FLAGS_SETUP_RCVD);

    /* Ensure timer is stopped */
    btu_stop_timer(&p_bcb->conn_timer);
    p_bcb->re_transmits = 0;

    /* Tell the upper layer, if he has a callback */
    if (bnep_cb.p_conn_state_cb)
        (*bnep_cb.p_conn_state_cb)(p_bcb->handle, p_bcb->rem_bda, BNEP_SUCCESS,
                                   is_role_change);
}

#endif /* (BNEP_INCLUDED == TRUE) */

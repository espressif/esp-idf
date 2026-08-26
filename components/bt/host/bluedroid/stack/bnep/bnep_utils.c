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
 *  This file contains BNEP utility functions
 *
 ******************************************************************************/

#include "common/bt_target.h"

#if (BNEP_INCLUDED == TRUE)

#include <stdio.h>
#include <string.h>

#include "device/controller.h"
#include "osi/allocator.h"
#include "osi/fixed_queue.h"

#include "bnep_int.h"
#include "stack/bt_types.h"

#include "btm_int.h"
#include "stack/btu.h"

extern fixed_queue_t* btu_general_alarm_queue;

/******************************************************************************/
/*            L O C A L    F U N C T I O N     P R O T O T Y P E S            */
/******************************************************************************/
static UINT8* bnepu_init_hdr(BT_HDR * p_buf, UINT16 hdr_len,
                             UINT8 pkt_type);

void bnepu_process_peer_multicast_filter_set(tBNEP_CONN * p_bcb,
                                             UINT8* p_filters, UINT16 len);
void bnepu_send_peer_multicast_filter_rsp(tBNEP_CONN * p_bcb,
                                          UINT16 response_code);

/*******************************************************************************
 *
 * Function         bnepu_find_bcb_by_cid
 *
 * Description      This function searches the bcb table for an entry with the
 *                  passed CID.
 *
 * Returns          the BCB address, or NULL if not found.
 *
 ******************************************************************************/
tBNEP_CONN *bnepu_find_bcb_by_cid(UINT16 cid)
{
    UINT16 xx;
    tBNEP_CONN * p_bcb;

    /* Look through each connection control block */
    for (xx = 0, p_bcb = bnep_cb.bcb; xx < BNEP_MAX_CONNECTIONS; xx++, p_bcb++) {
        if ((p_bcb->con_state != BNEP_STATE_IDLE) && (p_bcb->l2cap_cid == cid)) {
            return (p_bcb);
        }
    }

    /* If here, not found */
    return (NULL);
}

/*******************************************************************************
 *
 * Function         bnepu_find_bcb_by_bd_addr
 *
 * Description      This function searches the BCB table for an entry with the
 *                  passed Bluetooth Address.
 *
 * Returns          the BCB address, or NULL if not found.
 *
 ******************************************************************************/
tBNEP_CONN *bnepu_find_bcb_by_bd_addr(BD_ADDR p_bda)
{
    UINT16 xx;
    tBNEP_CONN * p_bcb;

    /* Look through each connection control block */
    for (xx = 0, p_bcb = bnep_cb.bcb; xx < BNEP_MAX_CONNECTIONS; xx++, p_bcb++) {
        if (p_bcb->con_state != BNEP_STATE_IDLE) {
            if (!bdcmp(p_bcb->rem_bda, p_bda)) {
                return (p_bcb);
            }
        }
    }

    /* If here, not found */
    return (NULL);
}

/*******************************************************************************
 *
 * Function         bnepu_allocate_bcb
 *
 * Description      This function allocates a new BCB.
 *
 * Returns          BCB address, or NULL if none available.
 *
 ******************************************************************************/
tBNEP_CONN *bnepu_allocate_bcb(BD_ADDR p_rem_bda)
{
    UINT16 xx;
    tBNEP_CONN * p_bcb;

    /* Look through each connection control block for a free one */
    for (xx = 0, p_bcb = bnep_cb.bcb; xx < BNEP_MAX_CONNECTIONS; xx++, p_bcb++) {
        if (p_bcb->con_state == BNEP_STATE_IDLE) {
            /* Preserve sec_gen across recycle so stale BTM callbacks that still
             * hold this BCB pointer cannot match a new connection's generation. */
            UINT32 prev_sec_gen = p_bcb->sec_gen;

            /* Todo: this free operation may be redundant */
            btu_free_timer(&p_bcb->conn_timer);
            memset((UINT8 *)p_bcb, 0, sizeof(tBNEP_CONN));
            p_bcb->sec_gen = prev_sec_gen;

            /* Initialize timer parameter */
            p_bcb->conn_timer.param = (uint32_t)p_bcb;
            bdcpy(p_bcb->rem_bda, p_rem_bda);
            p_bcb->handle = xx + 1;
            p_bcb->xmit_q = fixed_queue_new(SIZE_MAX);

            return (p_bcb);
        }
    }

    /* If here, no free BCB found */
    return (NULL);
}

/*******************************************************************************
 *
 * Function         bnepu_release_bcb
 *
 * Description      This function releases a BCB.
 *
 * Returns          void
 *
 ******************************************************************************/
void bnepu_release_bcb(tBNEP_CONN *p_bcb)
{
    /* Ensure timer is stopped */
    btu_free_timer(&p_bcb->conn_timer);

    /* Free any pending setup extension data we may be holding */
    p_bcb->con_state = BNEP_STATE_IDLE;
    p_bcb->con_flags = 0;
    p_bcb->l2cap_cid = 0;
    /* Invalidate outstanding BTM security callbacks bound to this BCB. */
    p_bcb->sec_gen++;
    memset(p_bcb->rem_bda, 0, BD_ADDR_LEN);
    if (p_bcb->p_pending_data) {
        osi_free(p_bcb->p_pending_data);
        p_bcb->p_pending_data = NULL;
    }

    /* Free transmit queue */
    if (p_bcb->xmit_q) {
        while (!fixed_queue_is_empty(p_bcb->xmit_q)) {
            osi_free(fixed_queue_dequeue(p_bcb->xmit_q, 0));
        }
        fixed_queue_free(p_bcb->xmit_q, NULL);
        p_bcb->xmit_q = NULL;
    }
}

/*******************************************************************************
 *
 * Function         bnep_send_conn_req
 *
 * Description      This function sends a BNEP connection request to peer
 *
 * Returns          void
 *
 ******************************************************************************/
void bnep_send_conn_req(tBNEP_CONN *p_bcb)
{
    BT_HDR * p_buf = (BT_HDR *)osi_malloc(BNEP_BUF_SIZE);
    UINT8 *p, *p_start;

    if (!p_buf) {
        BNEP_TRACE_ERROR("%s: no buffer", __func__);
        return;
    }

    BNEP_TRACE_DEBUG("%s: sending setup req with dst uuid %x", __func__,
                     p_bcb->dst_uuid.uu.uuid16);

    p_buf->offset = L2CAP_MIN_OFFSET;
    p = p_start = (UINT8*)(p_buf + 1) + L2CAP_MIN_OFFSET;

    /* Put in BNEP frame type - filter control */
    UINT8_TO_BE_STREAM(p, BNEP_FRAME_CONTROL);

    /* Put in filter message type - set filters */
    UINT8_TO_BE_STREAM(p, BNEP_SETUP_CONNECTION_REQUEST_MSG);

    UINT8_TO_BE_STREAM(p, p_bcb->dst_uuid.len);

    if (p_bcb->dst_uuid.len == 2) {
        UINT16_TO_BE_STREAM(p, p_bcb->dst_uuid.uu.uuid16);
        UINT16_TO_BE_STREAM(p, p_bcb->src_uuid.uu.uuid16);
    } else if (p_bcb->dst_uuid.len == 4) {
        UINT32_TO_BE_STREAM(p, p_bcb->dst_uuid.uu.uuid32);
        UINT32_TO_BE_STREAM(p, p_bcb->src_uuid.uu.uuid32);
    } else if (p_bcb->dst_uuid.len == 16) {
        memcpy(p, p_bcb->dst_uuid.uu.uuid128, p_bcb->dst_uuid.len);
        p += p_bcb->dst_uuid.len;
        memcpy(p, p_bcb->src_uuid.uu.uuid128, p_bcb->dst_uuid.len);
        p += p_bcb->dst_uuid.len;
    } else {
        BNEP_TRACE_ERROR("%s: uuid: %x, invalid length: %x", __func__,
                         p_bcb->dst_uuid.uu.uuid16, p_bcb->dst_uuid.len);
        osi_free(p_buf);
        return;
    }

    p_buf->len = (UINT16)(p - p_start);

    bnepu_check_send_packet(p_bcb, p_buf);
}

/*******************************************************************************
 *
 * Function         bnep_send_conn_responce
 *
 * Description      This function sends a BNEP setup response to peer
 *
 * Returns          void
 *
 ******************************************************************************/
void bnep_send_conn_responce(tBNEP_CONN *p_bcb, UINT16 resp_code)
{
    BT_HDR * p_buf = (BT_HDR *)osi_malloc(BNEP_BUF_SIZE);
    UINT8* p;

    if (!p_buf) {
        BNEP_TRACE_ERROR("%s: no buffer", __func__);
        return;
    }

    BNEP_TRACE_EVENT("BNEP - bnep_send_conn_responce for CID: 0x%x",
                     p_bcb->l2cap_cid);

    p_buf->offset = L2CAP_MIN_OFFSET;
    p = (UINT8*)(p_buf + 1) + L2CAP_MIN_OFFSET;

    /* Put in BNEP frame type - filter control */
    UINT8_TO_BE_STREAM(p, BNEP_FRAME_CONTROL);

    /* Put in filter message type - set filters */
    UINT8_TO_BE_STREAM(p, BNEP_SETUP_CONNECTION_RESPONSE_MSG);

    UINT16_TO_BE_STREAM(p, resp_code);

    p_buf->len = 4;

    bnepu_check_send_packet(p_bcb, p_buf);
}

/*******************************************************************************
 *
 * Function         bnepu_send_peer_our_filters
 *
 * Description      This function sends our filters to a peer
 *
 * Returns          void
 *
 ******************************************************************************/
void bnepu_send_peer_our_filters(tBNEP_CONN *p_bcb)
{
    BT_HDR * p_buf = (BT_HDR *)osi_malloc(BNEP_BUF_SIZE);
    UINT8* p;
    UINT16 xx;

    if (!p_buf) {
        BNEP_TRACE_ERROR("%s: no buffer", __func__);
        return;
    }

    BNEP_TRACE_DEBUG("BNEP sending peer our filters");

    p_buf->offset = L2CAP_MIN_OFFSET;
    p = (UINT8*)(p_buf + 1) + L2CAP_MIN_OFFSET;

    /* Put in BNEP frame type - filter control */
    UINT8_TO_BE_STREAM(p, BNEP_FRAME_CONTROL);

    /* Put in filter message type - set filters */
    UINT8_TO_BE_STREAM(p, BNEP_FILTER_NET_TYPE_SET_MSG);

    UINT16_TO_BE_STREAM(p, (4 * p_bcb->sent_num_filters));
    for (xx = 0; xx < p_bcb->sent_num_filters; xx++) {
        UINT16_TO_BE_STREAM(p, p_bcb->sent_prot_filter_start[xx]);
        UINT16_TO_BE_STREAM(p, p_bcb->sent_prot_filter_end[xx]);
    }

    p_buf->len = 4 + (4 * p_bcb->sent_num_filters);

    bnepu_check_send_packet(p_bcb, p_buf);

    p_bcb->con_flags |= BNEP_FLAGS_FILTER_RESP_PEND;

    /* Start timer waiting for setup response */
    btu_start_timer(&p_bcb->conn_timer, BTU_TTYPE_BNEP, BNEP_FILTER_SET_TIMEOUT_SEC);
}

/*******************************************************************************
 *
 * Function         bnepu_send_peer_our_multi_filters
 *
 * Description      This function sends our multicast filters to a peer
 *
 * Returns          void
 *
 ******************************************************************************/
void bnepu_send_peer_our_multi_filters(tBNEP_CONN *p_bcb)
{
    BT_HDR * p_buf = (BT_HDR *)osi_malloc(BNEP_BUF_SIZE);
    UINT8* p;
    UINT16 xx;

    if (!p_buf) {
        BNEP_TRACE_ERROR("%s: no buffer", __func__);
        return;
    }

    BNEP_TRACE_DEBUG("BNEP sending peer our multicast filters");

    p_buf->offset = L2CAP_MIN_OFFSET;
    p = (UINT8*)(p_buf + 1) + L2CAP_MIN_OFFSET;

    /* Put in BNEP frame type - filter control */
    UINT8_TO_BE_STREAM(p, BNEP_FRAME_CONTROL);

    /* Put in filter message type - set filters */
    UINT8_TO_BE_STREAM(p, BNEP_FILTER_MULTI_ADDR_SET_MSG);

    UINT16_TO_BE_STREAM(p, (2 * BD_ADDR_LEN * p_bcb->sent_mcast_filters));
    for (xx = 0; xx < p_bcb->sent_mcast_filters; xx++) {
        memcpy(p, p_bcb->sent_mcast_filter_start[xx], BD_ADDR_LEN);
        p += BD_ADDR_LEN;
        memcpy(p, p_bcb->sent_mcast_filter_end[xx], BD_ADDR_LEN);
        p += BD_ADDR_LEN;
    }

    p_buf->len = 4 + (2 * BD_ADDR_LEN * p_bcb->sent_mcast_filters);

    bnepu_check_send_packet(p_bcb, p_buf);

    p_bcb->con_flags |= BNEP_FLAGS_MULTI_RESP_PEND;

    /* Start timer waiting for setup response */
    btu_start_timer(&p_bcb->conn_timer, BTU_TTYPE_BNEP, BNEP_FILTER_SET_TIMEOUT_SEC);
}

/*******************************************************************************
 *
 * Function         bnepu_send_peer_filter_rsp
 *
 * Description      This function sends a filter response to a peer
 *
 * Returns          void
 *
 ******************************************************************************/
void bnepu_send_peer_filter_rsp(tBNEP_CONN *p_bcb, UINT16 response_code)
{
    BT_HDR * p_buf = (BT_HDR *)osi_malloc(BNEP_BUF_SIZE);
    UINT8* p;

    if (!p_buf) {
        BNEP_TRACE_ERROR("%s: no buffer", __func__);
        return;
    }

    BNEP_TRACE_DEBUG("BNEP sending filter response");

    p_buf->offset = L2CAP_MIN_OFFSET;
    p = (UINT8*)(p_buf + 1) + L2CAP_MIN_OFFSET;

    /* Put in BNEP frame type - filter control */
    UINT8_TO_BE_STREAM(p, BNEP_FRAME_CONTROL);

    /* Put in filter message type - set filters */
    UINT8_TO_BE_STREAM(p, BNEP_FILTER_NET_TYPE_RESPONSE_MSG);

    UINT16_TO_BE_STREAM(p, response_code);

    p_buf->len = 4;

    bnepu_check_send_packet(p_bcb, p_buf);
}

/*******************************************************************************
 *
 * Function         bnep_send_command_not_understood
 *
 * Description      This function sends a BNEP command not understood message
 *
 * Returns          void
 *
 ******************************************************************************/
void bnep_send_command_not_understood(tBNEP_CONN *p_bcb, UINT8 cmd_code)
{
    BT_HDR * p_buf = (BT_HDR *)osi_malloc(BNEP_BUF_SIZE);
    UINT8* p;

    if (!p_buf) {
        BNEP_TRACE_ERROR("%s: no buffer", __func__);
        return;
    }

    BNEP_TRACE_EVENT(
        "BNEP - bnep_send_command_not_understood for CID: 0x%x, cmd 0x%x",
        p_bcb->l2cap_cid, cmd_code);

    p_buf->offset = L2CAP_MIN_OFFSET;
    p = (UINT8*)(p_buf + 1) + L2CAP_MIN_OFFSET;

    /* Put in BNEP frame type - filter control */
    UINT8_TO_BE_STREAM(p, BNEP_FRAME_CONTROL);

    /* Put in filter message type - set filters */
    UINT8_TO_BE_STREAM(p, BNEP_CONTROL_COMMAND_NOT_UNDERSTOOD);

    UINT8_TO_BE_STREAM(p, cmd_code);

    p_buf->len = 3;

    bnepu_check_send_packet(p_bcb, p_buf);
}

/*******************************************************************************
 *
 * Function         bnepu_check_send_packet
 *
 * Description      This function tries to send a packet to L2CAP.
 *                  If L2CAP is flow controlled, it enqueues the
 *                  packet to the transmit queue
 *
 * Returns          void
 *
 ******************************************************************************/
void bnepu_check_send_packet(tBNEP_CONN *p_bcb, BT_HDR *p_buf)
{
    BNEP_TRACE_EVENT("BNEP - bnepu_check_send_packet for CID: 0x%x",
                     p_bcb->l2cap_cid);

    /* BCB may have been released while still processing the same RX frame
     * (e.g. failed setup response followed by extension headers). */
    if (p_bcb->con_state == BNEP_STATE_IDLE || p_bcb->xmit_q == NULL) {
        BNEP_TRACE_WARNING("BNEP - drop TX on idle/released BCB");
        osi_free(p_buf);
        return;
    }

    if (p_bcb->con_flags & BNEP_FLAGS_L2CAP_CONGESTED) {
        if (fixed_queue_length(p_bcb->xmit_q) >= BNEP_MAX_XMITQ_DEPTH) {
            BNEP_TRACE_EVENT("BNEP - congested, dropping buf, CID: 0x%x",
                             p_bcb->l2cap_cid);

            osi_free(p_buf);
        } else {
            fixed_queue_enqueue(p_bcb->xmit_q, p_buf, FIXED_QUEUE_MAX_TIMEOUT);
        }
    } else {
        L2CA_DataWrite(p_bcb->l2cap_cid, p_buf);
    }
}

/*******************************************************************************
 *
 * Function         bnepu_build_bnep_hdr
 *
 * Description      This function builds the BNEP header for a packet
 *                  Extension headers are not sent yet, so there is no
 *                  check for that.
 *
 * Returns          void
 *
 ******************************************************************************/
void bnepu_build_bnep_hdr(tBNEP_CONN *p_bcb, BT_HDR *p_buf, UINT16 protocol,
                          BD_ADDR p_src_addr, BD_ADDR p_dest_addr,
                          BOOLEAN fw_ext_present)
{
    const controller_t *controller = controller_get_interface();
    const UINT8 *local_addr = (const UINT8 *)controller->get_address();
    const UINT8 *src_addr = p_src_addr ? (const UINT8 *)p_src_addr : local_addr;
    UINT8 ext_bit, *p = (UINT8 *)NULL;
    UINT8 type = BNEP_FRAME_COMPRESSED_ETHERNET;

    ext_bit = fw_ext_present ? 0x80 : 0x00;

    if (bdcmp(src_addr, local_addr)) {
        type = BNEP_FRAME_COMPRESSED_ETHERNET_SRC_ONLY;
    }

    if (bdcmp(p_dest_addr, p_bcb->rem_bda))
        type = (type == BNEP_FRAME_COMPRESSED_ETHERNET)
               ? BNEP_FRAME_COMPRESSED_ETHERNET_DEST_ONLY
               : BNEP_FRAME_GENERAL_ETHERNET;

    switch (type) {
    case BNEP_FRAME_GENERAL_ETHERNET:
        p = bnepu_init_hdr(p_buf, 15,
                           (UINT8)(ext_bit | BNEP_FRAME_GENERAL_ETHERNET));
        if (!p) {
            return;
        }

        memcpy(p, p_dest_addr, BD_ADDR_LEN);
        p += BD_ADDR_LEN;

        memcpy(p, src_addr, BD_ADDR_LEN);
        p += BD_ADDR_LEN;
        break;

    case BNEP_FRAME_COMPRESSED_ETHERNET:
        p = bnepu_init_hdr(p_buf, 3,
                           (UINT8)(ext_bit | BNEP_FRAME_COMPRESSED_ETHERNET));
        break;

    case BNEP_FRAME_COMPRESSED_ETHERNET_SRC_ONLY:
        p = bnepu_init_hdr(
                p_buf, 9,
                (UINT8)(ext_bit | BNEP_FRAME_COMPRESSED_ETHERNET_SRC_ONLY));
        if (!p) {
            return;
        }

        memcpy(p, src_addr, BD_ADDR_LEN);
        p += BD_ADDR_LEN;
        break;

    case BNEP_FRAME_COMPRESSED_ETHERNET_DEST_ONLY:
        p = bnepu_init_hdr(
                p_buf, 9,
                (UINT8)(ext_bit | BNEP_FRAME_COMPRESSED_ETHERNET_DEST_ONLY));
        if (!p) {
            return;
        }

        memcpy(p, p_dest_addr, BD_ADDR_LEN);
        p += BD_ADDR_LEN;
        break;
    }

    if (!p) {
        return;
    }

    UINT16_TO_BE_STREAM(p, protocol);
}

/*******************************************************************************
 *
 * Function         bnepu_init_hdr
 *
 * Description      This function initializes the BNEP header
 *
 * Returns          pointer to header in buffer
 *
 ******************************************************************************/
static UINT8 *bnepu_init_hdr(BT_HDR *p_buf, UINT16 hdr_len,
                             UINT8 pkt_type)
{
    UINT8 *p = (UINT8 *)(p_buf + 1) + p_buf->offset;

    /* Headroom must already be present. Do not shift the payload: BT_HDR does
     * not record allocation size, so moving data forward can overrun
     * exact-sized heap buffers used in ESP-IDF. */
    if (p_buf->offset < (hdr_len + L2CAP_MIN_OFFSET)) {
        BNEP_TRACE_ERROR("BNEP init_hdr: offset %d too small for hdr_len %d",
                         p_buf->offset, hdr_len);
        return NULL;
    }

    p_buf->len += hdr_len;
    p_buf->offset -= hdr_len;
    p -= hdr_len;

    *p++ = pkt_type;

    return (p);
}

/*******************************************************************************
 *
 * Function         bnep_process_setup_conn_req
 *
 * Description      This function processes a peer's setup connection request
 *                  message. The destination UUID is verified and response sent
 *                  Connection open indication will be given to PAN profile
 *
 * Returns          void
 *
 ******************************************************************************/
void bnep_process_setup_conn_req(tBNEP_CONN *p_bcb, UINT8 *p_setup,
                                 UINT8 len)
{
    BNEP_TRACE_EVENT("BNEP - bnep_process_setup_conn_req for CID: 0x%x",
                     p_bcb->l2cap_cid);

    if (p_bcb->con_state != BNEP_STATE_CONN_SETUP &&
            p_bcb->con_state != BNEP_STATE_SEC_CHECKING &&
            p_bcb->con_state != BNEP_STATE_CONNECTED) {
        BNEP_TRACE_ERROR("BNEP - setup request in bad state %d", p_bcb->con_state);
        bnep_send_conn_responce(p_bcb, BNEP_SETUP_CONN_NOT_ALLOWED);
        return;
    }

    /* Check if we already initiated security check or if waiting for user
     * response */
    if (p_bcb->con_flags & BNEP_FLAGS_SETUP_RCVD) {
        BNEP_TRACE_EVENT(
            "BNEP - Duplicate Setup message received while doing security check");
        return;
    }

    /* Check if peer is the originator */
    if (p_bcb->con_state != BNEP_STATE_CONNECTED &&
            (!(p_bcb->con_flags & BNEP_FLAGS_SETUP_RCVD)) &&
            (p_bcb->con_flags & BNEP_FLAGS_IS_ORIG)) {
        BNEP_TRACE_ERROR("BNEP - setup request when we are originator",
                         p_bcb->con_state);
        bnep_send_conn_responce(p_bcb, BNEP_SETUP_CONN_NOT_ALLOWED);
        return;
    }

    if (p_bcb->con_state == BNEP_STATE_CONNECTED) {
        memcpy((UINT8 *) & (p_bcb->prv_src_uuid), (UINT8 *) & (p_bcb->src_uuid),
               sizeof(tBT_UUID));
        memcpy((UINT8 *) & (p_bcb->prv_dst_uuid), (UINT8 *) & (p_bcb->dst_uuid),
               sizeof(tBT_UUID));
    }

    p_bcb->dst_uuid.len = p_bcb->src_uuid.len = len;

    if (p_bcb->dst_uuid.len == 2) {
        /* because peer initiated connection keep src uuid as dst uuid */
        BE_STREAM_TO_UINT16(p_bcb->src_uuid.uu.uuid16, p_setup);
        BE_STREAM_TO_UINT16(p_bcb->dst_uuid.uu.uuid16, p_setup);

        /* If nothing has changed don't bother the profile */
        if (p_bcb->con_state == BNEP_STATE_CONNECTED &&
                p_bcb->prv_src_uuid.len == 2 &&
                p_bcb->prv_dst_uuid.len == 2 &&
                p_bcb->src_uuid.uu.uuid16 == p_bcb->prv_src_uuid.uu.uuid16 &&
                p_bcb->dst_uuid.uu.uuid16 == p_bcb->prv_dst_uuid.uu.uuid16) {
            bnep_send_conn_responce(p_bcb, BNEP_SETUP_CONN_OK);
            return;
        }
    } else if (p_bcb->dst_uuid.len == 4) {
        BE_STREAM_TO_UINT32(p_bcb->src_uuid.uu.uuid32, p_setup);
        BE_STREAM_TO_UINT32(p_bcb->dst_uuid.uu.uuid32, p_setup);

        if (p_bcb->con_state == BNEP_STATE_CONNECTED &&
                p_bcb->prv_src_uuid.len == 4 &&
                p_bcb->prv_dst_uuid.len == 4 &&
                p_bcb->src_uuid.uu.uuid32 == p_bcb->prv_src_uuid.uu.uuid32 &&
                p_bcb->dst_uuid.uu.uuid32 == p_bcb->prv_dst_uuid.uu.uuid32) {
            bnep_send_conn_responce(p_bcb, BNEP_SETUP_CONN_OK);
            return;
        }
    } else if (p_bcb->dst_uuid.len == 16) {
        memcpy(p_bcb->src_uuid.uu.uuid128, p_setup, p_bcb->src_uuid.len);
        p_setup += p_bcb->src_uuid.len;
        memcpy(p_bcb->dst_uuid.uu.uuid128, p_setup, p_bcb->dst_uuid.len);
        p_setup += p_bcb->dst_uuid.len;

        if (p_bcb->con_state == BNEP_STATE_CONNECTED &&
                p_bcb->prv_src_uuid.len == 16 &&
                p_bcb->prv_dst_uuid.len == 16 &&
                memcmp(p_bcb->src_uuid.uu.uuid128,
                       p_bcb->prv_src_uuid.uu.uuid128, 16) == 0 &&
                memcmp(p_bcb->dst_uuid.uu.uuid128,
                       p_bcb->prv_dst_uuid.uu.uuid128, 16) == 0) {
            bnep_send_conn_responce(p_bcb, BNEP_SETUP_CONN_OK);
            return;
        }
    } else {
        BNEP_TRACE_ERROR("BNEP - Bad UID len %d in ConnReq", p_bcb->dst_uuid.len);
        bnep_send_conn_responce(p_bcb, BNEP_SETUP_INVALID_UUID_SIZE);
        return;
    }

    p_bcb->con_state = BNEP_STATE_SEC_CHECKING;
    p_bcb->con_flags |= BNEP_FLAGS_SETUP_RCVD;

    BNEP_TRACE_EVENT(
        "BNEP initiating security check for incoming call for uuid 0x%x",
        p_bcb->src_uuid.uu.uuid16);
#if (BNEP_DO_AUTH_FOR_ROLE_SWITCH == FALSE)
    if (p_bcb->con_flags & BNEP_FLAGS_CONN_COMPLETED)
        bnep_sec_check_complete(p_bcb->rem_bda, BT_TRANSPORT_BR_EDR, p_bcb,
                                BTM_SUCCESS);
    else
#endif
        btm_sec_mx_access_request(
            p_bcb->rem_bda, BT_PSM_BNEP, FALSE, BTM_SEC_PROTO_BNEP,
            bnep_get_uuid32(&(p_bcb->src_uuid)), &bnep_sec_check_complete, p_bcb);

    return;
}

/*******************************************************************************
 *
 * Function         bnep_process_setup_conn_responce
 *
 * Description      This function processes a peer's setup connection response
 *                  message. The response code is verified and
 *                  Connection open indication will be given to PAN profile
 *
 * Returns          void
 *
 ******************************************************************************/
void bnep_process_setup_conn_responce(tBNEP_CONN *p_bcb, UINT8 *p_setup)
{
    tBNEP_RESULT resp;
    UINT16 resp_code;

    BNEP_TRACE_DEBUG("BNEP received setup response");
    /* The state should be either SETUP or CONNECTED */
    if (p_bcb->con_state != BNEP_STATE_CONN_SETUP) {
        /* Should we disconnect ? */
        BNEP_TRACE_ERROR("BNEP - setup response in bad state %d", p_bcb->con_state);
        return;
    }

    /* Check if we are the originator */
    if (!(p_bcb->con_flags & BNEP_FLAGS_IS_ORIG)) {
        BNEP_TRACE_ERROR("BNEP - setup response when we are not originator",
                         p_bcb->con_state);
        return;
    }

    BE_STREAM_TO_UINT16(resp_code, p_setup);

    switch (resp_code) {
    case BNEP_SETUP_INVALID_SRC_UUID:
        resp = BNEP_CONN_FAILED_SRC_UUID;
        break;

    case BNEP_SETUP_INVALID_DEST_UUID:
        resp = BNEP_CONN_FAILED_DST_UUID;
        break;

    case BNEP_SETUP_INVALID_UUID_SIZE:
        resp = BNEP_CONN_FAILED_UUID_SIZE;
        break;

    case BNEP_SETUP_CONN_NOT_ALLOWED:
    default:
        resp = BNEP_CONN_FAILED;
        break;
    }

    /* Check the response code */
    if (resp_code != BNEP_SETUP_CONN_OK) {
        if (p_bcb->con_flags & BNEP_FLAGS_CONN_COMPLETED) {
            BNEP_TRACE_EVENT("BNEP - role change response is %d", resp_code);

            /* Restore the earlier BNEP status */
            p_bcb->con_state = BNEP_STATE_CONNECTED;
            p_bcb->con_flags &= (~BNEP_FLAGS_SETUP_RCVD);
            memcpy((UINT8*) & (p_bcb->src_uuid), (UINT8*) & (p_bcb->prv_src_uuid),
                   sizeof(tBT_UUID));
            memcpy((UINT8*) & (p_bcb->dst_uuid), (UINT8*) & (p_bcb->prv_dst_uuid),
                   sizeof(tBT_UUID));

            /* Ensure timer is stopped */
            btu_stop_timer(&p_bcb->conn_timer);
            p_bcb->re_transmits = 0;

            /* Tell the user if he has a callback */
            if (bnep_cb.p_conn_state_cb) {
                (*bnep_cb.p_conn_state_cb)(p_bcb->handle, p_bcb->rem_bda, resp, TRUE);
            }

            return;
        } else {
            BNEP_TRACE_ERROR("BNEP - setup response %d is not OK", resp_code);

            L2CA_DisconnectReq(p_bcb->l2cap_cid);

            /* Tell the user if he has a callback */
            if ((p_bcb->con_flags & BNEP_FLAGS_IS_ORIG) && (bnep_cb.p_conn_state_cb)) {
                (*bnep_cb.p_conn_state_cb)(p_bcb->handle, p_bcb->rem_bda, resp, FALSE);
            }

            bnepu_release_bcb(p_bcb);
            return;
        }
    }

    /* Received successful response */
    bnep_connected(p_bcb);
}

/*******************************************************************************
 *
 * Function         bnep_process_control_packet
 *
 * Description      This function processes a peer's setup connection request
 *                  message. The destination UUID is verified and response sent
 *                  Connection open indication will be given to PAN profile
 *
 * Returns          void
 *
 ******************************************************************************/
UINT8 *bnep_process_control_packet(tBNEP_CONN *p_bcb, UINT8 *p,
                                   UINT16 *rem_len, BOOLEAN is_ext)
{
    UINT8 control_type;
    BOOLEAN bad_pkt = FALSE;
    UINT16 len, ext_len = 0;
    UINT16 rem_len_prev = *rem_len;
    /* When is_ext, rem_len is temporarily limited to this extension body. */
    UINT16 ext_body = 0;
    UINT16 pkt_rem_after_type = 0;

    /* is_ext: need extension length + control type (2 bytes);
     * otherwise need at least the control type (1 byte). */
    if (*rem_len < (is_ext ? 2 : 1)) {
        BNEP_TRACE_ERROR("BNEP control packet truncated: rem_len %d is_ext %d",
                         *rem_len, is_ext);
        *rem_len = 0;
        return NULL;
    }

    if (is_ext) {
        ext_len = *p++;
        *rem_len = *rem_len - 1;
        /* Length covers the control type and optional payload; zero is invalid
         * and would underflow (ext_len - 1) in the unknown-control path. */
        if (ext_len == 0 || *rem_len < ext_len) {
            BNEP_TRACE_ERROR(
                "BNEP control extension bad length: ext_len %d rem_len %d",
                ext_len, *rem_len);
            *rem_len = 0;
            return NULL;
        }
    }

    control_type = *p++;
    *rem_len = *rem_len - 1;

    if (is_ext) {
        /* Only parse within this extension; do not consume following
         * extensions or network payload as control fields. */
        ext_body = (UINT16)(ext_len - 1);
        pkt_rem_after_type = *rem_len;
        *rem_len = ext_body;
    }

    BNEP_TRACE_EVENT(
        "BNEP processing control packet rem_len %d, is_ext %d, ctrl_type %d",
        *rem_len, is_ext, control_type);

    switch (control_type) {
    case BNEP_CONTROL_COMMAND_NOT_UNDERSTOOD:
        if (*rem_len < 1) {
            bad_pkt = TRUE;
            BNEP_TRACE_ERROR(
                "BNEP Received Cmd not understood with bad length");
            break;
        }
        BNEP_TRACE_ERROR("BNEP Received Cmd not understood for ctl pkt type: %d",
                         *p);
        p++;
        *rem_len = *rem_len - 1;
        break;

    case BNEP_SETUP_CONNECTION_REQUEST_MSG:
        if (*rem_len < 1) {
            bad_pkt = TRUE;
            BNEP_TRACE_ERROR("BNEP Received Setup message with bad length");
            break;
        }
        len = *p++;
        if (*rem_len < ((2 * len) + 1)) {
            bad_pkt = TRUE;
            BNEP_TRACE_ERROR("BNEP Received Setup message with bad length");
            break;
        }
        if (!is_ext) {
            bnep_process_setup_conn_req(p_bcb, p, (UINT8)len);
        }
        p += (2 * len);
        *rem_len = *rem_len - (2 * len) - 1;
        break;

    case BNEP_SETUP_CONNECTION_RESPONSE_MSG:
        if (*rem_len < 2) {
            bad_pkt = TRUE;
            BNEP_TRACE_ERROR("BNEP Received Setup response with bad length");
            break;
        }
        if (!is_ext) {
            bnep_process_setup_conn_responce(p_bcb, p);
        }
        p += 2;
        *rem_len = *rem_len - 2;
        break;

    case BNEP_FILTER_NET_TYPE_SET_MSG:
        if (*rem_len < 2) {
            bad_pkt = TRUE;
            BNEP_TRACE_ERROR("BNEP Received Filter set message with bad length");
            break;
        }
        BE_STREAM_TO_UINT16(len, p);
        if (*rem_len < (len + 2)) {
            bad_pkt = TRUE;
            BNEP_TRACE_ERROR("BNEP Received Filter set message with bad length");
            break;
        }
        bnepu_process_peer_filter_set(p_bcb, p, len);
        p += len;
        *rem_len = *rem_len - len - 2;
        break;

    case BNEP_FILTER_NET_TYPE_RESPONSE_MSG:
        if (*rem_len < 2) {
            bad_pkt = TRUE;
            BNEP_TRACE_ERROR("BNEP Received Filter response with bad length");
            break;
        }
        bnepu_process_peer_filter_rsp(p_bcb, p);
        p += 2;
        *rem_len = *rem_len - 2;
        break;

    case BNEP_FILTER_MULTI_ADDR_SET_MSG:
        if (*rem_len < 2) {
            bad_pkt = TRUE;
            BNEP_TRACE_ERROR(
                "BNEP Received Multicast Filter Set message with bad length");
            break;
        }
        BE_STREAM_TO_UINT16(len, p);
        if (*rem_len < (len + 2)) {
            bad_pkt = TRUE;
            BNEP_TRACE_ERROR(
                "BNEP Received Multicast Filter Set message with bad length");
            break;
        }
        bnepu_process_peer_multicast_filter_set(p_bcb, p, len);
        p += len;
        *rem_len = *rem_len - len - 2;
        break;

    case BNEP_FILTER_MULTI_ADDR_RESPONSE_MSG:
        if (*rem_len < 2) {
            bad_pkt = TRUE;
            BNEP_TRACE_ERROR(
                "BNEP Received Multicast Filter response with bad length");
            break;
        }
        bnepu_process_multicast_filter_rsp(p_bcb, p);
        p += 2;
        *rem_len = *rem_len - 2;
        break;

    default:
        BNEP_TRACE_ERROR("BNEP - bad ctl pkt type: %d", control_type);
        bnep_send_command_not_understood(p_bcb, control_type);
        /* Remaining extension body is skipped in the is_ext epilogue below. */
        break;
    }

    if (bad_pkt ||
            (is_ext ? (*rem_len > ext_body) : (*rem_len > rem_len_prev))) {
        BNEP_TRACE_ERROR("BNEP - bad ctl pkt length: %d", *rem_len);
        *rem_len = 0;
        return NULL;
    }

    if (is_ext) {
        /* Consume any unused bytes in this extension, then restore rem_len for
         * the rest of the BNEP frame (next extension or network payload). */
        p += *rem_len;
        *rem_len = (UINT16)(pkt_rem_after_type - ext_body);
    }

    /* Setup failure (and similar) may release the BCB mid-packet. Signal the
     * caller to stop parsing further extension headers on this buffer. */
    if (p_bcb->con_state == BNEP_STATE_IDLE) {
        *rem_len = 0;
        return NULL;
    }

    return p;
}

/*******************************************************************************
 *
 * Function         bnepu_process_peer_filter_set
 *
 * Description      This function processes a peer's filter control
 *                  'set' message. The filters are stored in the BCB,
 *                  and an appropriate filter response message sent.
 *
 * Returns          void
 *
 ******************************************************************************/
void bnepu_process_peer_filter_set(tBNEP_CONN *p_bcb, UINT8 *p_filters,
                                   UINT16 len)
{
    UINT16 num_filters = 0;
    UINT16 xx, resp_code = BNEP_FILTER_CRL_OK;
    UINT16 start, end;
    UINT8* p_temp_filters;

    if ((p_bcb->con_state != BNEP_STATE_CONNECTED) &&
            (!(p_bcb->con_flags & BNEP_FLAGS_CONN_COMPLETED))) {
        BNEP_TRACE_DEBUG(
            "BNEP received filter set from peer when there is no connection");
        return;
    }

    BNEP_TRACE_DEBUG("BNEP received filter set from peer");
    /* Check for length not a multiple of 4 */
    if (len & 3) {
        BNEP_TRACE_EVENT("BNEP - bad filter len: %d", len);
        bnepu_send_peer_filter_rsp(p_bcb, BNEP_FILTER_CRL_BAD_RANGE);
        return;
    }

    if (len) {
        num_filters = (UINT16)(len >> 2);
    }

    /* Validate filter values */
    if (num_filters <= BNEP_MAX_PROT_FILTERS) {
        p_temp_filters = p_filters;
        for (xx = 0; xx < num_filters; xx++) {
            BE_STREAM_TO_UINT16(start, p_temp_filters);
            BE_STREAM_TO_UINT16(end, p_temp_filters);

            if (start > end) {
                resp_code = BNEP_FILTER_CRL_BAD_RANGE;
                break;
            }
        }
    } else {
        resp_code = BNEP_FILTER_CRL_MAX_REACHED;
    }

    if (resp_code != BNEP_FILTER_CRL_OK) {
        bnepu_send_peer_filter_rsp(p_bcb, resp_code);
        return;
    }

    if (bnep_cb.p_filter_ind_cb) {
        (*bnep_cb.p_filter_ind_cb)(p_bcb->handle, TRUE, 0, len, p_filters);
    }

    p_bcb->rcvd_num_filters = num_filters;
    for (xx = 0; xx < num_filters; xx++) {
        BE_STREAM_TO_UINT16(start, p_filters);
        BE_STREAM_TO_UINT16(end, p_filters);

        p_bcb->rcvd_prot_filter_start[xx] = start;
        p_bcb->rcvd_prot_filter_end[xx] = end;
    }

    bnepu_send_peer_filter_rsp(p_bcb, resp_code);
}

/*******************************************************************************
 *
 * Function         bnepu_process_peer_filter_rsp
 *
 * Description      This function processes a peer's filter control
 *                  'response' message.
 *
 * Returns          void
 *
 ******************************************************************************/
void bnepu_process_peer_filter_rsp(tBNEP_CONN *p_bcb, UINT8 *p_data)
{
    UINT16 resp_code;
    tBNEP_RESULT result;

    BNEP_TRACE_DEBUG("BNEP received filter response");
    /* The state should be  CONNECTED */
    if ((p_bcb->con_state != BNEP_STATE_CONNECTED) &&
            (!(p_bcb->con_flags & BNEP_FLAGS_CONN_COMPLETED))) {
        BNEP_TRACE_ERROR("BNEP - filter response in bad state %d",
                         p_bcb->con_state);
        return;
    }

    /* Check if we are the originator */
    if (!(p_bcb->con_flags & BNEP_FLAGS_FILTER_RESP_PEND)) {
        BNEP_TRACE_ERROR("BNEP - filter response when not expecting");
        return;
    }

    /* Ensure timer is stopped */
    btu_stop_timer(&p_bcb->conn_timer);
    p_bcb->con_flags &= ~BNEP_FLAGS_FILTER_RESP_PEND;
    p_bcb->re_transmits = 0;

    BE_STREAM_TO_UINT16(resp_code, p_data);

    result = BNEP_SUCCESS;
    if (resp_code != BNEP_FILTER_CRL_OK) {
        result = BNEP_SET_FILTER_FAIL;
    }

    if (bnep_cb.p_filter_ind_cb) {
        (*bnep_cb.p_filter_ind_cb)(p_bcb->handle, FALSE, result, 0, NULL);
    }
}

/*******************************************************************************
 *
 * Function         bnepu_process_multicast_filter_rsp
 *
 * Description      This function processes multicast filter control
 *                  'response' message.
 *
 * Returns          void
 *
 ******************************************************************************/
void bnepu_process_multicast_filter_rsp(tBNEP_CONN *p_bcb, UINT8 *p_data)
{
    UINT16 resp_code;
    tBNEP_RESULT result;

    BNEP_TRACE_DEBUG("BNEP received multicast filter response");
    /* The state should be  CONNECTED */
    if ((p_bcb->con_state != BNEP_STATE_CONNECTED) &&
            (!(p_bcb->con_flags & BNEP_FLAGS_CONN_COMPLETED))) {
        BNEP_TRACE_ERROR("BNEP - multicast filter response in bad state %d",
                         p_bcb->con_state);
        return;
    }

    /* Check if we are the originator */
    if (!(p_bcb->con_flags & BNEP_FLAGS_MULTI_RESP_PEND)) {
        BNEP_TRACE_ERROR("BNEP - multicast filter response when not expecting");
        return;
    }

    /* Ensure timer is stopped */
    btu_stop_timer(&p_bcb->conn_timer);
    p_bcb->con_flags &= ~BNEP_FLAGS_MULTI_RESP_PEND;
    p_bcb->re_transmits = 0;

    BE_STREAM_TO_UINT16(resp_code, p_data);

    result = BNEP_SUCCESS;
    if (resp_code != BNEP_FILTER_CRL_OK) {
        result = BNEP_SET_FILTER_FAIL;
    }

    if (bnep_cb.p_mfilter_ind_cb) {
        (*bnep_cb.p_mfilter_ind_cb)(p_bcb->handle, FALSE, result, 0, NULL);
    }
}

/*******************************************************************************
 *
 * Function         bnepu_process_peer_multicast_filter_set
 *
 * Description      This function processes a peer's filter control
 *                  'set' message. The filters are stored in the BCB,
 *                  and an appropriate filter response message sent.
 *
 * Returns          void
 *
 ******************************************************************************/
void bnepu_process_peer_multicast_filter_set(tBNEP_CONN *p_bcb,
                                             UINT8 *p_filters, UINT16 len)
{
    UINT16 resp_code = BNEP_FILTER_CRL_OK;
    UINT16 num_filters, xx;
    UINT8 *p_temp_filters, null_bda[BD_ADDR_LEN] = {0, 0, 0, 0, 0, 0};

    if ((p_bcb->con_state != BNEP_STATE_CONNECTED) &&
            (!(p_bcb->con_flags & BNEP_FLAGS_CONN_COMPLETED))) {
        BNEP_TRACE_DEBUG(
            "BNEP received multicast filter set from peer when there is no "
            "connection");
        return;
    }

    if (len % 12) {
        BNEP_TRACE_EVENT("BNEP - bad filter len: %d", len);
        bnepu_send_peer_multicast_filter_rsp(p_bcb, BNEP_FILTER_CRL_BAD_RANGE);
        return;
    }

    if (len > (BNEP_MAX_MULTI_FILTERS * 2 * BD_ADDR_LEN)) {
        BNEP_TRACE_EVENT("BNEP - Too many filters");
        bnepu_send_peer_multicast_filter_rsp(p_bcb, BNEP_FILTER_CRL_MAX_REACHED);
        return;
    }

    num_filters = 0;
    if (len) {
        num_filters = (UINT16)(len / 12);
    }

    /* Validate filter values */
    if (num_filters <= BNEP_MAX_MULTI_FILTERS) {
        p_temp_filters = p_filters;
        for (xx = 0; xx < num_filters; xx++) {
            if (memcmp(p_temp_filters, p_temp_filters + BD_ADDR_LEN, BD_ADDR_LEN) >
                    0) {
                bnepu_send_peer_multicast_filter_rsp(p_bcb, BNEP_FILTER_CRL_BAD_RANGE);
                return;
            }

            p_temp_filters += (BD_ADDR_LEN * 2);
        }
    }

    p_bcb->rcvd_mcast_filters = num_filters;
    p_temp_filters = p_filters;
    for (xx = 0; xx < num_filters; xx++) {
        memcpy(p_bcb->rcvd_mcast_filter_start[xx], p_temp_filters, BD_ADDR_LEN);
        memcpy(p_bcb->rcvd_mcast_filter_end[xx], p_temp_filters + BD_ADDR_LEN,
               BD_ADDR_LEN);
        p_temp_filters += (BD_ADDR_LEN * 2);

        /* Check if any of the ranges have all zeros as both starting and ending
         * addresses */
        if ((memcmp(null_bda, p_bcb->rcvd_mcast_filter_start[xx],
                    BD_ADDR_LEN) == 0) &&
                (memcmp(null_bda, p_bcb->rcvd_mcast_filter_end[xx],
                        BD_ADDR_LEN) == 0)) {
            p_bcb->rcvd_mcast_filters = 0xFFFF;
            break;
        }
    }

    BNEP_TRACE_EVENT("BNEP multicast filters %d", p_bcb->rcvd_mcast_filters);
    bnepu_send_peer_multicast_filter_rsp(p_bcb, resp_code);

    if (bnep_cb.p_mfilter_ind_cb) {
        (*bnep_cb.p_mfilter_ind_cb)(p_bcb->handle, TRUE, 0, len, p_filters);
    }
}

/*******************************************************************************
 *
 * Function         bnepu_send_peer_multicast_filter_rsp
 *
 * Description      This function sends a filter response to a peer
 *
 * Returns          void
 *
 ******************************************************************************/
void bnepu_send_peer_multicast_filter_rsp(tBNEP_CONN *p_bcb,
                                          UINT16 response_code)
{
    BT_HDR * p_buf = (BT_HDR *)osi_malloc(BNEP_BUF_SIZE);
    UINT8* p;

    if (!p_buf) {
        BNEP_TRACE_ERROR("%s: no buffer", __func__);
        return;
    }

    BNEP_TRACE_DEBUG("BNEP sending multicast filter response %d", response_code);

    p_buf->offset = L2CAP_MIN_OFFSET;
    p = (UINT8*)(p_buf + 1) + L2CAP_MIN_OFFSET;

    /* Put in BNEP frame type - filter control */
    UINT8_TO_BE_STREAM(p, BNEP_FRAME_CONTROL);

    /* Put in filter message type - set filters */
    UINT8_TO_BE_STREAM(p, BNEP_FILTER_MULTI_ADDR_RESPONSE_MSG);

    UINT16_TO_BE_STREAM(p, response_code);

    p_buf->len = 4;

    bnepu_check_send_packet(p_bcb, p_buf);
}

/*******************************************************************************
 *
 * Function         bnep_sec_check_complete
 *
 * Description      This function is registered with BTM and will be called
 *                  after completing the security procedures
 *
 * Returns          void
 *
 ******************************************************************************/
void bnep_sec_check_complete(BD_ADDR bd_addr,
                             UNUSED_ATTR tBT_TRANSPORT transport,
                             void *p_ref_data, UINT8 result)
{
    tBNEP_CONN * p_bcb = (tBNEP_CONN *)p_ref_data;
    UINT16 resp_code = BNEP_SETUP_CONN_OK;
    BOOLEAN is_role_change;

    if (!p_bcb) {
        return;
    }

    /* check if the port is still waiting for security to complete */
    if (p_bcb->con_state != BNEP_STATE_SEC_CHECKING) {
        BNEP_TRACE_ERROR(
            "BNEP Connection in wrong state %d when security is completed",
            p_bcb->con_state);
        return;
    }

    /*
     * After L2CAP drop, bnepu_release_bcb may recycle this BCB for another
     * peer while a BTM security request is still outstanding. Ignore callbacks
     * whose address does not match the connection currently using the BCB.
     */
    if (bd_addr == NULL || bdcmp(bd_addr, p_bcb->rem_bda)) {
        BNEP_TRACE_WARNING(
            "BNEP ignoring security callback (stale or mismatched address)");
        return;
    }

    BNEP_TRACE_EVENT("BNEP security callback returned result %d", result);
    if (p_bcb->con_flags & BNEP_FLAGS_CONN_COMPLETED) {
        is_role_change = TRUE;
    } else {
        is_role_change = FALSE;
    }

    /* if it is outgoing call and result is FAILURE return security fail error */
    if (!(p_bcb->con_flags & BNEP_FLAGS_SETUP_RCVD)) {
        if (result != BTM_SUCCESS) {
            if (p_bcb->con_flags & BNEP_FLAGS_CONN_COMPLETED) {
                /* Tell the user that role change is failed because of security */
                if (bnep_cb.p_conn_state_cb)
                    (*bnep_cb.p_conn_state_cb)(p_bcb->handle, p_bcb->rem_bda,
                                               BNEP_SECURITY_FAIL, is_role_change);

                p_bcb->con_state = BNEP_STATE_CONNECTED;
                memcpy((UINT8*) & (p_bcb->src_uuid), (UINT8*) & (p_bcb->prv_src_uuid),
                       sizeof(tBT_UUID));
                memcpy((UINT8*) & (p_bcb->dst_uuid), (UINT8*) & (p_bcb->prv_dst_uuid),
                       sizeof(tBT_UUID));
                return;
            }

            L2CA_DisconnectReq(p_bcb->l2cap_cid);

            /* Tell the user if he has a callback */
            if (bnep_cb.p_conn_state_cb)
                (*bnep_cb.p_conn_state_cb)(p_bcb->handle, p_bcb->rem_bda,
                                           BNEP_SECURITY_FAIL, is_role_change);

            bnepu_release_bcb(p_bcb);
            return;
        }

        /* Transition to the next appropriate state, waiting for connection confirm.
         */
        p_bcb->con_state = BNEP_STATE_CONN_SETUP;

        bnep_send_conn_req(p_bcb);
        btu_start_timer(&p_bcb->conn_timer, BTU_TTYPE_BNEP, BNEP_CONN_TIMEOUT_SEC);
        return;
    }

    /* it is an incoming call respond appropriately */
    if (result != BTM_SUCCESS) {
        bnep_send_conn_responce(p_bcb, BNEP_SETUP_CONN_NOT_ALLOWED);
        if (p_bcb->con_flags & BNEP_FLAGS_CONN_COMPLETED) {
            /* Role change is failed because of security. Revert back to connected
             * state */
            p_bcb->con_state = BNEP_STATE_CONNECTED;
            p_bcb->con_flags &= (~BNEP_FLAGS_SETUP_RCVD);
            memcpy((UINT8*) & (p_bcb->src_uuid), (UINT8*) & (p_bcb->prv_src_uuid),
                   sizeof(tBT_UUID));
            memcpy((UINT8*) & (p_bcb->dst_uuid), (UINT8*) & (p_bcb->prv_dst_uuid),
                   sizeof(tBT_UUID));
            return;
        }

        L2CA_DisconnectReq(p_bcb->l2cap_cid);

        bnepu_release_bcb(p_bcb);
        return;
    }

    if (bnep_cb.p_conn_ind_cb) {
        p_bcb->con_state = BNEP_STATE_CONN_SETUP;
        (*bnep_cb.p_conn_ind_cb)(p_bcb->handle, p_bcb->rem_bda, &p_bcb->dst_uuid,
                                 &p_bcb->src_uuid, is_role_change);
    } else {
        /* Profile didn't register connection indication call back */
        bnep_send_conn_responce(p_bcb, resp_code);
        bnep_connected(p_bcb);
    }

    return;
}

/*******************************************************************************
 *
 * Function         bnep_is_packet_allowed
 *
 * Description      This function verifies whether the protocol passes through
 *                  the protocol filters set by the peer
 *
 * Returns          BNEP_SUCCESS          - if the protocol is allowed
 *                  BNEP_IGNORE_CMD       - if the protocol is filtered out
 *
 ******************************************************************************/
tBNEP_RESULT bnep_is_packet_allowed(tBNEP_CONN *p_bcb,
                                    BD_ADDR p_dest_addr,
                                    UINT16 protocol, BOOLEAN fw_ext_present,
                                    UINT8 *p_data, UINT16 data_len)
{
    if (p_bcb->rcvd_num_filters) {
        UINT16 i, proto;

        /* Find out the actual protocol to check for the filtering */
        proto = protocol;
        if (proto == BNEP_802_1_P_PROTOCOL) {
            if (fw_ext_present) {
                UINT8 ext_len, ext;
                /* parse the extension headers and find out actual protocol */
                do {
                    if (data_len < 2) {
                        return BNEP_IGNORE_CMD;
                    }
                    ext = *p_data++;
                    ext_len = *p_data++;
                    data_len -= 2;
                    if (data_len < ext_len) {
                        return BNEP_IGNORE_CMD;
                    }
                    p_data += ext_len;
                    data_len -= ext_len;

                } while (ext & 0x80);
            }
            /* 802.1Q tag (2) + ethertype (2) */
            if (data_len < 4) {
                return BNEP_IGNORE_CMD;
            }
            p_data += 2;
            BE_STREAM_TO_UINT16(proto, p_data);
        }

        for (i = 0; i < p_bcb->rcvd_num_filters; i++) {
            if ((p_bcb->rcvd_prot_filter_start[i] <= proto) &&
                    (proto <= p_bcb->rcvd_prot_filter_end[i])) {
                break;
            }
        }

        if (i == p_bcb->rcvd_num_filters) {
            BNEP_TRACE_DEBUG("Ignoring protocol 0x%x in BNEP data write", proto);
            return BNEP_IGNORE_CMD;
        }
    }

    /* Check for multicast address filtering */
    if ((p_dest_addr[0] & 0x01) && p_bcb->rcvd_mcast_filters) {
        UINT16 i = 0;

        /* Check if every multicast should be filtered */
        if (p_bcb->rcvd_mcast_filters != 0xFFFF) {
            /* Check if the address is mentioned in the filter range */
            for (i = 0; i < p_bcb->rcvd_mcast_filters; i++) {
                if ((memcmp(p_bcb->rcvd_mcast_filter_start[i],
                            p_dest_addr, BD_ADDR_LEN) <= 0) &&
                        (memcmp(p_bcb->rcvd_mcast_filter_end[i],
                                p_dest_addr, BD_ADDR_LEN) >= 0)) {
                    break;
                }
            }
        }

        /*
        ** If every multicast should be filtered or the address is not in the filter
        *range
        ** drop the packet
        */
        if ((p_bcb->rcvd_mcast_filters == 0xFFFF) ||
            (i == p_bcb->rcvd_mcast_filters)) {
            BNEP_TRACE_DEBUG(
                "Ignoring multicast address %02x:%02x:%02x:%02x:%02x:%02x in BNEP data write",
                p_dest_addr[0], p_dest_addr[1], p_dest_addr[2], p_dest_addr[3],
                p_dest_addr[4], p_dest_addr[5]);
            return BNEP_IGNORE_CMD;
        }
    }

    return BNEP_SUCCESS;
}

/*******************************************************************************
 *
 * Function         bnep_get_uuid32
 *
 * Description      This function returns the 32-bit equivalent of the UUID
 *
 * Returns          UINT32 - 32-bit equivalent of the UUID
 *
 ******************************************************************************/
UINT32 bnep_get_uuid32(tBT_UUID *src_uuid)
{
    UINT32 result;

    if (src_uuid->len == 2) {
        return ((UINT32)src_uuid->uu.uuid16);
    } else if (src_uuid->len == 4) {
        return (src_uuid->uu.uuid32 & 0x0000FFFF);
    } else {
        result = src_uuid->uu.uuid128[2];
        result = (result << 8) | (src_uuid->uu.uuid128[3]);
        return result;
    }
}

#endif /* (BNEP_INCLUDED == TRUE) */

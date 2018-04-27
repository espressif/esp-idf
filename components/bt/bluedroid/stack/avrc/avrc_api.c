/******************************************************************************
 *
 *  Copyright (C) 2003-2012 Broadcom Corporation
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
 *  Interface to AVRCP mandatory commands
 *
 ******************************************************************************/
// #include <assert.h>
#include "common/bt_trace.h"
#include <string.h>
#include "common/bt_target.h"
#include "stack/avrc_api.h"
#include "avrc_int.h"
#include "osi/allocator.h"

#if (defined(AVRC_INCLUDED) && AVRC_INCLUDED == TRUE)

/*****************************************************************************
**  Global data
*****************************************************************************/


#define AVRC_MAX_RCV_CTRL_EVT   AVCT_BROWSE_UNCONG_IND_EVT

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

static const UINT8 avrc_ctrl_event_map[] = {
    AVRC_OPEN_IND_EVT,  /* AVCT_CONNECT_CFM_EVT */
    AVRC_OPEN_IND_EVT,  /* AVCT_CONNECT_IND_EVT */
    AVRC_CLOSE_IND_EVT, /* AVCT_DISCONNECT_CFM_EVT */
    AVRC_CLOSE_IND_EVT, /* AVCT_DISCONNECT_IND_EVT */
    AVRC_CONG_IND_EVT,  /* AVCT_CONG_IND_EVT */
    AVRC_UNCONG_IND_EVT,/* AVCT_UNCONG_IND_EVT */
    AVRC_BROWSE_OPEN_IND_EVT,  /* AVCT_BROWSE_CONN_CFM_EVT   */
    AVRC_BROWSE_OPEN_IND_EVT,  /* AVCT_BROWSE_CONN_IND_EVT   */
    AVRC_BROWSE_CLOSE_IND_EVT, /* AVCT_BROWSE_DISCONN_CFM_EVT */
    AVRC_BROWSE_CLOSE_IND_EVT, /* AVCT_BROWSE_DISCONN_IND_EVT */
    AVRC_BROWSE_CONG_IND_EVT,  /* AVCT_BROWSE_CONG_IND_EVT    */
    AVRC_BROWSE_UNCONG_IND_EVT /* AVCT_BROWSE_UNCONG_IND_EVT  */
};

#define AVRC_OP_DROP        0xFE    /* use this unused opcode to indication no need to call the callback function */
#define AVRC_OP_DROP_N_FREE 0xFD    /* use this unused opcode to indication no need to call the callback function & free buffer */

#define AVRC_OP_UNIT_INFO_RSP_LEN       8
#define AVRC_OP_SUB_UNIT_INFO_RSP_LEN   8
#define AVRC_OP_REJ_MSG_LEN            11

/******************************************************************************
**
** Function         avrc_ctrl_cback
**
** Description      This is the callback function used by AVCTP to report
**                  received link events.
**
** Returns          Nothing.
**
******************************************************************************/
static void avrc_ctrl_cback(UINT8 handle, UINT8 event, UINT16 result,
                            BD_ADDR peer_addr)
{
    UINT8   avrc_event;

    if (event <= AVRC_MAX_RCV_CTRL_EVT && avrc_cb.ccb[handle].p_ctrl_cback) {
        avrc_event = avrc_ctrl_event_map[event];
        if (event == AVCT_CONNECT_CFM_EVT) {
            if (result != 0) { /* failed */
                avrc_event = AVRC_CLOSE_IND_EVT;
            }
        }
        (*avrc_cb.ccb[handle].p_ctrl_cback)(handle, avrc_event, result, peer_addr);
    }
    /* else drop the unknown event*/
}

/******************************************************************************
**
** Function         avrc_get_data_ptr
**
** Description      Gets a pointer to the data payload in the packet.
**
** Returns          A pointer to the data payload.
**
******************************************************************************/
static UINT8 *avrc_get_data_ptr(BT_HDR *p_pkt)
{
    return (UINT8 *)(p_pkt + 1) + p_pkt->offset;
}

/******************************************************************************
**
** Function         avrc_copy_packet
**
** Description      Copies an AVRC packet to a new buffer. In the new buffer,
**                  the payload offset is at least AVCT_MSG_OFFSET octets.
**
** Returns          The buffer with the copied data.
**
******************************************************************************/
static BT_HDR *avrc_copy_packet(BT_HDR *p_pkt, int rsp_pkt_len)
{
    const int offset = MAX(AVCT_MSG_OFFSET, p_pkt->offset);
    const int pkt_len = MAX(rsp_pkt_len, p_pkt->len);
    BT_HDR *p_pkt_copy =
        (BT_HDR *)osi_malloc((UINT16)(BT_HDR_SIZE + offset + pkt_len));

    /* Copy the packet header, set the new offset, and copy the payload */
    if (p_pkt_copy != NULL) {
        memcpy(p_pkt_copy, p_pkt, BT_HDR_SIZE);
        p_pkt_copy->offset = offset;
        UINT8 *p_data = avrc_get_data_ptr(p_pkt);
        UINT8 *p_data_copy = avrc_get_data_ptr(p_pkt_copy);
        memcpy(p_data_copy, p_data, p_pkt->len);
    }

    return p_pkt_copy;
}

#if (AVRC_METADATA_INCLUDED == TRUE)
/******************************************************************************
**
** Function         avrc_prep_end_frag
**
** Description      This function prepares an end response fragment
**
** Returns          Nothing.
**
******************************************************************************/
static void avrc_prep_end_frag(UINT8 handle)
{
    tAVRC_FRAG_CB   *p_fcb;
    BT_HDR  *p_pkt_new;
    UINT8   *p_data, *p_orig_data;
    UINT8   rsp_type;

    AVRC_TRACE_DEBUG ("avrc_prep_end_frag" );
    p_fcb = &avrc_cb.fcb[handle];

    /* The response type of the end fragment should be the same as the the PDU of "End Fragment
    ** Response" Errata: https://www.bluetooth.org/errata/errata_view.cfm?errata_id=4383
    */
    p_orig_data = ((UINT8 *)(p_fcb->p_fmsg + 1) + p_fcb->p_fmsg->offset);
    rsp_type = ((*p_orig_data) & AVRC_CTYPE_MASK);

    p_pkt_new           = p_fcb->p_fmsg;
    p_pkt_new->len      -= (AVRC_MAX_CTRL_DATA_LEN - AVRC_VENDOR_HDR_SIZE - AVRC_MIN_META_HDR_SIZE);
    p_pkt_new->offset   += (AVRC_MAX_CTRL_DATA_LEN - AVRC_VENDOR_HDR_SIZE - AVRC_MIN_META_HDR_SIZE);
    p_data = (UINT8 *)(p_pkt_new + 1) + p_pkt_new->offset;
    *p_data++       = rsp_type;
    *p_data++       = (AVRC_SUB_PANEL << AVRC_SUBTYPE_SHIFT);
    *p_data++       = AVRC_OP_VENDOR;
    AVRC_CO_ID_TO_BE_STREAM(p_data, AVRC_CO_METADATA);
    *p_data++       = p_fcb->frag_pdu;
    *p_data++       = AVRC_PKT_END;

    /* 4=pdu, pkt_type & len */
    UINT16_TO_BE_STREAM(p_data, (p_pkt_new->len - AVRC_VENDOR_HDR_SIZE - AVRC_MIN_META_HDR_SIZE));
}

/******************************************************************************
**
** Function         avrc_send_continue_frag
**
** Description      This function sends a continue response fragment
**
** Returns          Nothing.
**
******************************************************************************/
static void avrc_send_continue_frag(UINT8 handle, UINT8 label)
{
    tAVRC_FRAG_CB   *p_fcb;
    BT_HDR  *p_pkt_old, *p_pkt;
    UINT8   *p_old, *p_data;
    UINT8   cr = AVCT_RSP;
    tAVRC_RSP   rej_rsp;

    p_fcb = &avrc_cb.fcb[handle];
    p_pkt = p_fcb->p_fmsg;

    AVRC_TRACE_DEBUG("%s handle = %u label = %u len = %d",
                     __func__, handle, label, p_pkt->len);
    if (p_pkt->len > AVRC_MAX_CTRL_DATA_LEN) {
        int offset_len = MAX(AVCT_MSG_OFFSET, p_pkt->offset);
        p_pkt_old = p_fcb->p_fmsg;
        p_pkt = (BT_HDR *)osi_malloc((UINT16)(AVRC_PACKET_LEN + offset_len + BT_HDR_SIZE));
        if (p_pkt) {
            p_pkt->len          = AVRC_MAX_CTRL_DATA_LEN;
            p_pkt->offset       = AVCT_MSG_OFFSET;
            p_pkt->layer_specific = p_pkt_old->layer_specific;
            p_pkt->event = p_pkt_old->event;
            p_old = (UINT8 *)(p_pkt_old + 1) + p_pkt_old->offset;
            p_data = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
            memcpy (p_data, p_old, AVRC_MAX_CTRL_DATA_LEN);
            /* use AVRC continue packet type */
            p_data += AVRC_VENDOR_HDR_SIZE;
            p_data++; /* pdu */
            *p_data++ = AVRC_PKT_CONTINUE;
            /* 4=pdu, pkt_type & len */
            UINT16_TO_BE_STREAM(p_data, (AVRC_MAX_CTRL_DATA_LEN - AVRC_VENDOR_HDR_SIZE - 4));

            /* prepare the left over for as an end fragment */
            avrc_prep_end_frag (handle);
        } else {
            /* use the current GKI buffer to send Internal error status */
            p_pkt = p_fcb->p_fmsg;
            p_fcb->p_fmsg = NULL;
            p_fcb->frag_enabled = FALSE;
            AVRC_TRACE_ERROR ("AVRC_MsgReq no buffers for fragmentation - send internal error" );
            p_data = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
            *p_data++ = AVRC_PDU_REQUEST_CONTINUATION_RSP;
            *p_data++ = 0;
            UINT16_TO_BE_STREAM(p_data, 0);
            p_pkt->len = 4;
            rej_rsp.pdu = AVRC_PDU_REQUEST_CONTINUATION_RSP;
            rej_rsp.status = AVRC_STS_INTERNAL_ERR;
            AVRC_BldResponse( handle, (tAVRC_RESPONSE *)&rej_rsp, &p_pkt);
            cr = AVCT_RSP;
        }
    } else {
        /* end fragment. clean the control block */
        p_fcb->frag_enabled = FALSE;
        p_fcb->p_fmsg       = NULL;
    }
    AVCT_MsgReq( handle, label, cr, p_pkt);
}

/******************************************************************************
**
** Function         avrc_proc_vendor_command
**
** Description      This function processes received vendor command.
**
** Returns          if not NULL, the response to send right away.
**
******************************************************************************/
static BT_HDR *avrc_proc_vendor_command(UINT8 handle, UINT8 label,
                                        BT_HDR *p_pkt, tAVRC_MSG_VENDOR *p_msg)
{
    BT_HDR      *p_rsp = NULL;
    UINT8       *p_data;
    UINT8       *p_begin;
    UINT8       pkt_type;
    BOOLEAN     abort_frag = FALSE;
    tAVRC_STS   status = AVRC_STS_NO_ERROR;
    tAVRC_FRAG_CB   *p_fcb;

    p_begin  = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
    p_data   = p_begin + AVRC_VENDOR_HDR_SIZE;
    pkt_type = *(p_data + 1) & AVRC_PKT_TYPE_MASK;

    if (pkt_type != AVRC_PKT_SINGLE) {
        /* reject - commands can only be in single packets at AVRCP level */
        AVRC_TRACE_ERROR ("commands must be in single packet pdu:0x%x", *p_data );
        /* use the current GKI buffer to send the reject */
        status = AVRC_STS_BAD_CMD;
    }
    /* check if there are fragments waiting to be sent */
    else if (avrc_cb.fcb[handle].frag_enabled) {
        p_fcb = &avrc_cb.fcb[handle];
        if (p_msg->company_id == AVRC_CO_METADATA) {
            switch (*p_data) {
            case AVRC_PDU_ABORT_CONTINUATION_RSP:
                /* aborted by CT - send accept response */
                abort_frag = TRUE;
                p_begin = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
                *p_begin = (AVRC_RSP_ACCEPT & AVRC_CTYPE_MASK);
                if (*(p_data + 4) != p_fcb->frag_pdu) {
                    *p_begin = (AVRC_RSP_REJ & AVRC_CTYPE_MASK);
                    *(p_data + 4) = AVRC_STS_BAD_PARAM;
                } else {
                    p_data = (p_begin + AVRC_VENDOR_HDR_SIZE + 2);
                    UINT16_TO_BE_STREAM(p_data, 0);
                    p_pkt->len = (p_data - p_begin);
                }
                AVCT_MsgReq( handle, label, AVCT_RSP, p_pkt);
                p_msg->hdr.opcode = AVRC_OP_DROP; /* used the p_pkt to send response */
                break;

            case AVRC_PDU_REQUEST_CONTINUATION_RSP:
                if (*(p_data + 4) == p_fcb->frag_pdu) {
                    avrc_send_continue_frag(handle, label);
                    p_msg->hdr.opcode = AVRC_OP_DROP_N_FREE;
                } else {
                    /* the pdu id does not match - reject the command using the current GKI buffer */
                    AVRC_TRACE_ERROR("avrc_proc_vendor_command continue pdu: 0x%x does not match \
                    current re-assembly pdu: 0x%x",
                                     *(p_data + 4), p_fcb->frag_pdu);
                    status = AVRC_STS_BAD_PARAM;
                    abort_frag = TRUE;
                }
                break;

            default:
                /* implicit abort */
                abort_frag = TRUE;
            }
        } else {
            abort_frag = TRUE;
            /* implicit abort */
        }

        if (abort_frag) {
            if (p_fcb->p_fmsg) {
                osi_free(p_fcb->p_fmsg);
                p_fcb->p_fmsg = NULL;
            }
            p_fcb->frag_enabled = FALSE;
        }
    }

    if (status != AVRC_STS_NO_ERROR) {
        /* use the current GKI buffer to build/send the reject message */
        p_data = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
        *p_data++ = AVRC_RSP_REJ;
        p_data += AVRC_VENDOR_HDR_SIZE; /* pdu */
        *p_data++ = 0;                  /* pkt_type */
        UINT16_TO_BE_STREAM(p_data, 1); /* len */
        *p_data++ = status;             /* error code */
        p_pkt->len = AVRC_VENDOR_HDR_SIZE + 5;
        p_rsp = p_pkt;
    }

    return p_rsp;
}

/******************************************************************************
**
** Function         avrc_proc_far_msg
**
** Description      This function processes metadata fragmenation
**                  and reassembly
**
** Returns          0, to report the message with msg_cback .
**
******************************************************************************/
static UINT8 avrc_proc_far_msg(UINT8 handle, UINT8 label, UINT8 cr, BT_HDR **pp_pkt,
                               tAVRC_MSG_VENDOR *p_msg)
{
    BT_HDR      *p_pkt = *pp_pkt;
    UINT8       *p_data;
    UINT8       drop_code = 0;
    BT_HDR      *p_rsp = NULL;
    BT_HDR      *p_cmd = NULL;
    BOOLEAN     req_continue = FALSE;
    BT_HDR      *p_pkt_new = NULL;
    UINT8       pkt_type;
    tAVRC_RASM_CB   *p_rcb;
    tAVRC_NEXT_CMD   avrc_cmd;

    p_data  = (UINT8 *)(p_pkt + 1) + p_pkt->offset;

    /* Skip over vendor header (ctype, subunit*, opcode, CO_ID) */
    p_data += AVRC_VENDOR_HDR_SIZE;

    pkt_type = *(p_data + 1) & AVRC_PKT_TYPE_MASK;
    AVRC_TRACE_DEBUG ("pkt_type %d", pkt_type );
    p_rcb = &avrc_cb.rcb[handle];
    if (p_msg->company_id == AVRC_CO_METADATA) {
        /* check if the message needs to be re-assembled */
        if (pkt_type == AVRC_PKT_SINGLE || pkt_type == AVRC_PKT_START) {
            /* previous fragments need to be dropped, when received another new message */
            p_rcb->rasm_offset = 0;
            if (p_rcb->p_rmsg) {
                osi_free(p_rcb->p_rmsg);
                p_rcb->p_rmsg = NULL;
            }
        }

        if (pkt_type != AVRC_PKT_SINGLE && cr == AVCT_RSP) {
            /* not a single response packet - need to re-assemble metadata messages */
            if (pkt_type == AVRC_PKT_START) {
                /* Allocate buffer for re-assembly */
                p_rcb->rasm_pdu = *p_data;
                if ((p_rcb->p_rmsg = (BT_HDR *)osi_malloc(BT_DEFAULT_BUFFER_SIZE)) != NULL) {
                    /* Copy START packet to buffer for re-assembling fragments*/
                    memcpy(p_rcb->p_rmsg, p_pkt, sizeof(BT_HDR));   /* Copy bt hdr */

                    /* Copy metadata message */
                    memcpy((UINT8 *)(p_rcb->p_rmsg + 1),
                           (UINT8 *)(p_pkt + 1) + p_pkt->offset, p_pkt->len);

                    /* offset of start of metadata response in reassembly buffer */
                    p_rcb->p_rmsg->offset = p_rcb->rasm_offset = 0;

                    /* Free original START packet, replace with pointer to reassembly buffer  */
                    osi_free(p_pkt);
                    *pp_pkt = p_rcb->p_rmsg;
                } else {
                    /* Unable to allocate buffer for fragmented avrc message. Reuse START
                                      buffer for reassembly (re-assembled message may fit into ACL buf) */
                    AVRC_TRACE_DEBUG ("Unable to allocate buffer for fragmented avrc message, \
                                       reusing START buffer for reassembly");
                    p_rcb->rasm_offset = p_pkt->offset;
                    p_rcb->p_rmsg = p_pkt;
                }

                /* set offset to point to where to copy next - use the same re-asm logic as AVCT */
                p_rcb->p_rmsg->offset += p_rcb->p_rmsg->len;
                req_continue = TRUE;
            } else if (p_rcb->p_rmsg == NULL) {
                /* Received a CONTINUE/END, but no corresponding START
                              (or previous fragmented response was dropped) */
                AVRC_TRACE_DEBUG ("Received a CONTINUE/END without no corresponding START \
                                   (or previous fragmented response was dropped)");
                drop_code = 5;
                osi_free(p_pkt);
                *pp_pkt = NULL;
            } else {
                /* get size of buffer holding assembled message */
                /*
                 * NOTE: The buffer is allocated above at the beginning of the
                 * reassembly, and is always of size BT_DEFAULT_BUFFER_SIZE.
                 */
                UINT16 buf_len = BT_DEFAULT_BUFFER_SIZE - sizeof(BT_HDR);
                /* adjust offset and len of fragment for header byte */
                p_pkt->offset += (AVRC_VENDOR_HDR_SIZE + AVRC_MIN_META_HDR_SIZE);
                p_pkt->len -= (AVRC_VENDOR_HDR_SIZE + AVRC_MIN_META_HDR_SIZE);
                /* verify length */
                if ((p_rcb->p_rmsg->offset + p_pkt->len) > buf_len) {
                    AVRC_TRACE_WARNING("Fragmented message too big! - report the partial message");
                    p_pkt->len = buf_len - p_rcb->p_rmsg->offset;
                    pkt_type = AVRC_PKT_END;
                }

                /* copy contents of p_pkt to p_rx_msg */
                memcpy((UINT8 *)(p_rcb->p_rmsg + 1) + p_rcb->p_rmsg->offset,
                       (UINT8 *)(p_pkt + 1) + p_pkt->offset, p_pkt->len);

                if (pkt_type == AVRC_PKT_END) {
                    p_rcb->p_rmsg->offset = p_rcb->rasm_offset;
                    p_rcb->p_rmsg->len += p_pkt->len;
                    p_pkt_new = p_rcb->p_rmsg;
                    p_rcb->rasm_offset = 0;
                    p_rcb->p_rmsg = NULL;
                    p_msg->p_vendor_data   = (UINT8 *)(p_pkt_new + 1) + p_pkt_new->offset;
                    p_msg->hdr.ctype       = p_msg->p_vendor_data[0] & AVRC_CTYPE_MASK;
                    /* 6 = ctype, subunit*, opcode & CO_ID */
                    p_msg->p_vendor_data  += AVRC_VENDOR_HDR_SIZE;
                    p_msg->vendor_len      = p_pkt_new->len - AVRC_VENDOR_HDR_SIZE;
                    p_data = p_msg->p_vendor_data + 1; /* skip pdu */
                    *p_data++ = AVRC_PKT_SINGLE;
                    UINT16_TO_BE_STREAM(p_data, (p_msg->vendor_len - AVRC_MIN_META_HDR_SIZE));
                    AVRC_TRACE_DEBUG("end frag:%d, total len:%d, offset:%d", p_pkt->len,
                                     p_pkt_new->len, p_pkt_new->offset);
                } else {
                    p_rcb->p_rmsg->offset += p_pkt->len;
                    p_rcb->p_rmsg->len += p_pkt->len;
                    p_pkt_new = NULL;
                    req_continue = TRUE;
                }
                osi_free(p_pkt);
                *pp_pkt = p_pkt_new;
            }
        }

        if (cr == AVCT_CMD) {
            p_rsp = avrc_proc_vendor_command(handle, label, *pp_pkt, p_msg);
            if (p_rsp) {
                AVCT_MsgReq( handle, label, AVCT_RSP, p_rsp);
                drop_code = 3;
            } else if (p_msg->hdr.opcode == AVRC_OP_DROP) {
                drop_code = 1;
            } else if (p_msg->hdr.opcode == AVRC_OP_DROP_N_FREE) {
                drop_code = 4;
            }

        } else if (cr == AVCT_RSP && req_continue == TRUE) {
            avrc_cmd.pdu    = AVRC_PDU_REQUEST_CONTINUATION_RSP;
            avrc_cmd.status = AVRC_STS_NO_ERROR;
            avrc_cmd.target_pdu = p_rcb->rasm_pdu;
            if (AVRC_BldCommand ((tAVRC_COMMAND *)&avrc_cmd, &p_cmd) == AVRC_STS_NO_ERROR) {
                drop_code = 2;
                AVRC_MsgReq (handle, (UINT8)(label), AVRC_CMD_CTRL, p_cmd);
            }
        }
    }

    return drop_code;
}
#endif /* (AVRC_METADATA_INCLUDED == TRUE) */

/******************************************************************************
**
** Function         avrc_msg_cback
**
** Description      This is the callback function used by AVCTP to report
**                  received AV control messages.
**
** Returns          Nothing.
**
******************************************************************************/
static void avrc_msg_cback(UINT8 handle, UINT8 label, UINT8 cr,
                           BT_HDR *p_pkt)
{
    UINT8       opcode;
    tAVRC_MSG   msg;
    UINT8       *p_data;
    UINT8       *p_begin;
    BOOLEAN     drop = FALSE;
    BOOLEAN     do_free = TRUE;
    BT_HDR      *p_rsp = NULL;
    UINT8       *p_rsp_data;
    int         xx;
    BOOLEAN     reject = FALSE;
#if (BT_USE_TRACES == TRUE)
    char        *p_drop_msg = "dropped";
#endif
    tAVRC_MSG_VENDOR *p_msg = &msg.vendor;

    if (cr == AVCT_CMD &&
            (p_pkt->layer_specific & AVCT_DATA_CTRL && AVRC_PACKET_LEN < sizeof(p_pkt->len))) {
        /* Ignore the invalid AV/C command frame */
#if (BT_USE_TRACES == TRUE)
        p_drop_msg = "dropped - too long AV/C cmd frame size";
#endif
        osi_free(p_pkt);
        return;
    }

    if (cr == AVCT_REJ) {
        /* The peer thinks that this PID is no longer open - remove this handle */
        /*  */
        osi_free(p_pkt);
        AVCT_RemoveConn(handle);
        return;
    }

    p_data  = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
    memset(&msg, 0, sizeof(tAVRC_MSG) );
    {
        msg.hdr.ctype           = p_data[0] & AVRC_CTYPE_MASK;
        AVRC_TRACE_DEBUG("avrc_msg_cback handle:%d, ctype:%d, offset:%d, len: %d",
                         handle, msg.hdr.ctype, p_pkt->offset, p_pkt->len);
        msg.hdr.subunit_type    = (p_data[1] & AVRC_SUBTYPE_MASK) >> AVRC_SUBTYPE_SHIFT;
        msg.hdr.subunit_id      = p_data[1] & AVRC_SUBID_MASK;
        opcode                  = p_data[2];
    }

    if ( ((avrc_cb.ccb[handle].control & AVRC_CT_TARGET) && (cr == AVCT_CMD)) ||
            ((avrc_cb.ccb[handle].control & AVRC_CT_CONTROL) && (cr == AVCT_RSP)) ) {

        switch (opcode) {
        case AVRC_OP_UNIT_INFO:
            if (cr == AVCT_CMD) {
                /* send the response to the peer */
                p_rsp = avrc_copy_packet(p_pkt, AVRC_OP_UNIT_INFO_RSP_LEN);
                p_rsp_data = avrc_get_data_ptr(p_rsp);
                *p_rsp_data = AVRC_RSP_IMPL_STBL;
                /* check & set the offset. set response code, set subunit_type & subunit_id,
                   set AVRC_OP_UNIT_INFO */
                /* 3 bytes: ctype, subunit*, opcode */
                p_rsp_data      += AVRC_AVC_HDR_SIZE;
                *p_rsp_data++   = 7;
                /* Panel subunit & id=0 */
                *p_rsp_data++   = (AVRC_SUB_PANEL << AVRC_SUBTYPE_SHIFT);
                AVRC_CO_ID_TO_BE_STREAM(p_rsp_data, avrc_cb.ccb[handle].company_id);
                p_rsp->len      = (UINT16) (p_rsp_data - (UINT8 *)(p_rsp + 1) - p_rsp->offset);
                cr = AVCT_RSP;
#if (BT_USE_TRACES == TRUE)
                p_drop_msg = "auto respond";
#endif
            } else {
                /* parse response */
                p_data += 4; /* 3 bytes: ctype, subunit*, opcode + octet 3 (is 7)*/
                msg.unit.unit_type  = (*p_data & AVRC_SUBTYPE_MASK) >> AVRC_SUBTYPE_SHIFT;
                msg.unit.unit       = *p_data & AVRC_SUBID_MASK;
                p_data++;
                AVRC_BE_STREAM_TO_CO_ID(msg.unit.company_id, p_data);
            }
            break;

        case AVRC_OP_SUB_INFO:
            if (cr == AVCT_CMD) {
                /* send the response to the peer */
                p_rsp = avrc_copy_packet(p_pkt, AVRC_OP_SUB_UNIT_INFO_RSP_LEN);
                p_rsp_data = avrc_get_data_ptr(p_rsp);
                *p_rsp_data = AVRC_RSP_IMPL_STBL;
                /* check & set the offset. set response code, set (subunit_type & subunit_id),
                   set AVRC_OP_SUB_INFO, set (page & extention code) */
                p_rsp_data      += 4;
                /* Panel subunit & id=0 */
                *p_rsp_data++   = (AVRC_SUB_PANEL << AVRC_SUBTYPE_SHIFT);
                memset(p_rsp_data, AVRC_CMD_OPRND_PAD, AVRC_SUBRSP_OPRND_BYTES);
                p_rsp_data      += AVRC_SUBRSP_OPRND_BYTES;
                p_rsp->len      = (UINT16) (p_rsp_data - (UINT8 *)(p_rsp + 1) - p_rsp->offset);
                cr = AVCT_RSP;
#if (BT_USE_TRACES == TRUE)
                p_drop_msg = "auto responded";
#endif
            } else {
                /* parse response */
                p_data += AVRC_AVC_HDR_SIZE; /* 3 bytes: ctype, subunit*, opcode */
                msg.sub.page    = (*p_data++ >> AVRC_SUB_PAGE_SHIFT) & AVRC_SUB_PAGE_MASK;
                xx      = 0;
                while (*p_data != AVRC_CMD_OPRND_PAD && xx < AVRC_SUB_TYPE_LEN) {
                    msg.sub.subunit_type[xx] = *p_data++ >> AVRC_SUBTYPE_SHIFT;
                    if (msg.sub.subunit_type[xx] == AVRC_SUB_PANEL) {
                        msg.sub.panel   = TRUE;
                    }
                    xx++;
                }
            }
            break;

        case AVRC_OP_VENDOR:
            p_data  = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
            p_begin = p_data;
            if (p_pkt->len < AVRC_VENDOR_HDR_SIZE) { /* 6 = ctype, subunit*, opcode & CO_ID */
                if (cr == AVCT_CMD) {
                    reject = TRUE;
                } else {
                    drop = TRUE;
                }
                break;
            }
            p_data += AVRC_AVC_HDR_SIZE; /* skip the first 3 bytes: ctype, subunit*, opcode */
            AVRC_BE_STREAM_TO_CO_ID(p_msg->company_id, p_data);
            p_msg->p_vendor_data   = p_data;
            p_msg->vendor_len      = p_pkt->len - (p_data - p_begin);

#if (AVRC_METADATA_INCLUDED == TRUE)
            UINT8 drop_code = 0;
            if (p_msg->company_id == AVRC_CO_METADATA) {
                /* Validate length for metadata message */
                if (p_pkt->len < (AVRC_VENDOR_HDR_SIZE + AVRC_MIN_META_HDR_SIZE)) {
                    if (cr == AVCT_CMD) {
                        reject = TRUE;
                    } else {
                        drop = TRUE;
                    }
                    break;
                }

                /* Check+handle fragmented messages */
                drop_code = avrc_proc_far_msg(handle, label, cr, &p_pkt, p_msg);
                if (drop_code > 0) {
                    drop = TRUE;
                }
            }
            if (drop_code > 0) {
                if (drop_code != 4) {
                    do_free = FALSE;
                }
#if (BT_USE_TRACES == TRUE)
                switch (drop_code) {
                case 1:
                    p_drop_msg = "sent_frag";
                    break;
                case 2:
                    p_drop_msg = "req_cont";
                    break;
                case 3:
                    p_drop_msg = "sent_frag3";
                    break;
                case 4:
                    p_drop_msg = "sent_frag_free";
                    break;
                default:
                    p_drop_msg = "sent_fragd";
                }
#endif
            }
#endif /* (AVRC_METADATA_INCLUDED == TRUE) */
            break;

        case AVRC_OP_PASS_THRU:
            if (p_pkt->len < 5) { /* 3 bytes: ctype, subunit*, opcode & op_id & len */
                if (cr == AVCT_CMD) {
                    reject = TRUE;
                } else {
                    drop = TRUE;
                }
                break;
            }
            p_data += AVRC_AVC_HDR_SIZE; /* skip the first 3 bytes: ctype, subunit*, opcode */
            msg.pass.op_id  = (AVRC_PASS_OP_ID_MASK & *p_data);
            if (AVRC_PASS_STATE_MASK & *p_data) {
                msg.pass.state  = TRUE;
            } else {
                msg.pass.state  = FALSE;
            }
            p_data++;
            msg.pass.pass_len    = *p_data++;
            if (msg.pass.pass_len != p_pkt->len - 5) {
                msg.pass.pass_len = p_pkt->len - 5;
            }
            if (msg.pass.pass_len) {
                msg.pass.p_pass_data = p_data;
            } else {
                msg.pass.p_pass_data = NULL;
            }
            break;


        default:
            if ((avrc_cb.ccb[handle].control & AVRC_CT_TARGET) && (cr == AVCT_CMD)) {
                /* reject unsupported opcode */
                reject = TRUE;
            }
            drop    = TRUE;
            break;
        }
    } else { /* drop the event */
        drop    = TRUE;
    }

    if (reject) {
        /* reject unsupported opcode */
        p_rsp = avrc_copy_packet(p_pkt, AVRC_OP_REJ_MSG_LEN);
        p_rsp_data = avrc_get_data_ptr(p_rsp);
        *p_rsp_data = AVRC_RSP_REJ;
#if (BT_USE_TRACES == TRUE)
        p_drop_msg = "rejected";
#endif
        cr      = AVCT_RSP;
        drop    = TRUE;
    }

    if (p_rsp) {
        /* set to send response right away */
        AVCT_MsgReq( handle, label, cr, p_rsp);
        drop = TRUE;
    }

    if (drop == FALSE) {
        msg.hdr.opcode = opcode;
        (*avrc_cb.ccb[handle].p_msg_cback)(handle, label, opcode, &msg);
    }
#if (BT_USE_TRACES == TRUE)
    else {
        AVRC_TRACE_WARNING("avrc_msg_cback %s msg handle:%d, control:%d, cr:%d, opcode:x%x",
                           p_drop_msg,
                           handle, avrc_cb.ccb[handle].control, cr, opcode);
    }
#endif


    if (do_free) {
        osi_free(p_pkt);
    }
}




/******************************************************************************
**
** Function         avrc_pass_msg
**
** Description      Compose a PASS THROUGH command according to p_msg
**
**                  Input Parameters:
**                      p_msg: Pointer to PASS THROUGH message structure.
**
**                  Output Parameters:
**                      None.
**
** Returns          pointer to a valid GKI buffer if successful.
**                  NULL if p_msg is NULL.
**
******************************************************************************/
static BT_HDR   *avrc_pass_msg(tAVRC_MSG_PASS *p_msg)
{
    BT_HDR  *p_cmd = NULL;
    UINT8   *p_data;

    assert(p_msg != NULL);
    assert(AVRC_CMD_BUF_SIZE > (AVRC_MIN_CMD_LEN+p_msg->pass_len));

    if ((p_cmd = (BT_HDR *) osi_malloc(AVRC_CMD_BUF_SIZE)) != NULL) {
        p_cmd->offset   = AVCT_MSG_OFFSET;
        p_cmd->layer_specific   = AVCT_DATA_CTRL;
        p_data          = (UINT8 *)(p_cmd + 1) + p_cmd->offset;
        *p_data++       = (p_msg->hdr.ctype & AVRC_CTYPE_MASK);
        *p_data++       = (AVRC_SUB_PANEL << AVRC_SUBTYPE_SHIFT); /* Panel subunit & id=0 */
        *p_data++       = AVRC_OP_PASS_THRU;
        *p_data         = (AVRC_PASS_OP_ID_MASK & p_msg->op_id);
        if (p_msg->state) {
            *p_data     |= AVRC_PASS_STATE_MASK;
        }
        p_data++;

        if (p_msg->op_id == AVRC_ID_VENDOR) {
            *p_data++       = p_msg->pass_len;
            if (p_msg->pass_len && p_msg->p_pass_data) {
                memcpy(p_data, p_msg->p_pass_data, p_msg->pass_len);
                p_data += p_msg->pass_len;
            }
        } else { /* set msg len to 0 for other op_id */
            /* set msg len to 0 for other op_id */
            *p_data++       = 0;
        }
        p_cmd->len      = (UINT16) (p_data - (UINT8 *)(p_cmd + 1) - p_cmd->offset);
    }
    return p_cmd;
}

/******************************************************************************
**
** Function         AVRC_Open
**
** Description      This function is called to open a connection to AVCTP.
**                  The connection can be either an initiator or acceptor, as
**                  determined by the p_ccb->stream parameter.
**                  The connection can be a target, a controller or for both role,
**                  as determined by the p_ccb->control parameter.
**                  By definition, a target connection is an acceptor connection
**                  that waits for an incoming AVCTP connection from the peer.
**                  The connection remains available to the application until
**                  the application closes it by calling AVRC_Close().  The
**                  application does not need to reopen the connection after an
**                  AVRC_CLOSE_IND_EVT is received.
**
**                  Input Parameters:
**                      p_ccb->company_id: Company Identifier.
**
**                      p_ccb->p_ctrl_cback:  Pointer to control callback function.
**
**                      p_ccb->p_msg_cback:  Pointer to message callback function.
**
**                      p_ccb->conn: AVCTP connection role.  This is set to
**                      AVCTP_INT for initiator connections and AVCTP_ACP
**                      for acceptor connections.
**
**                      p_ccb->control: Control role.  This is set to
**                      AVRC_CT_TARGET for target connections, AVRC_CT_CONTROL
**                      for control connections or (AVRC_CT_TARGET|AVRC_CT_CONTROL)
**                      for connections that support both roles.
**
**                      peer_addr: BD address of peer device.  This value is
**                      only used for initiator connections; for acceptor
**                      connections it can be set to NULL.
**
**                  Output Parameters:
**                      p_handle: Pointer to handle.  This parameter is only
**                                valid if AVRC_SUCCESS is returned.
**
** Returns          AVRC_SUCCESS if successful.
**                  AVRC_NO_RESOURCES if there are not enough resources to open
**                  the connection.
**
******************************************************************************/
UINT16 AVRC_Open(UINT8 *p_handle, tAVRC_CONN_CB *p_ccb, BD_ADDR_PTR peer_addr)
{
    UINT16      status;
    tAVCT_CC    cc;

    cc.p_ctrl_cback = avrc_ctrl_cback;      /* Control callback */
    cc.p_msg_cback  = avrc_msg_cback;       /* Message callback */
    cc.pid          = UUID_SERVCLASS_AV_REMOTE_CONTROL;  /* Profile ID */
    cc.role         = p_ccb->conn;          /* Initiator/acceptor role */
    cc.control      = p_ccb->control;       /* Control role (Control/Target) */

    status = AVCT_CreateConn(p_handle, &cc, peer_addr);
    if (status == AVCT_SUCCESS) {
        memcpy(&avrc_cb.ccb[*p_handle], p_ccb, sizeof(tAVRC_CONN_CB));
#if (AVRC_METADATA_INCLUDED == TRUE)
        memset(&avrc_cb.fcb[*p_handle], 0, sizeof(tAVRC_FRAG_CB));
        memset(&avrc_cb.rcb[*p_handle], 0, sizeof(tAVRC_RASM_CB));
#endif
    }
    AVRC_TRACE_DEBUG("AVRC_Open role: %d, control:%d status:%d, handle:%d", cc.role, cc.control,
                     status, *p_handle);

    return status;
}

/******************************************************************************
**
** Function         AVRC_Close
**
** Description      Close a connection opened with AVRC_Open().
**                  This function is called when the
**                  application is no longer using a connection.
**
**                  Input Parameters:
**                      handle: Handle of this connection.
**
**                  Output Parameters:
**                      None.
**
** Returns          AVRC_SUCCESS if successful.
**                  AVRC_BAD_HANDLE if handle is invalid.
**
******************************************************************************/
UINT16 AVRC_Close(UINT8 handle)
{
    AVRC_TRACE_DEBUG("AVRC_Close handle:%d", handle);
    return AVCT_RemoveConn(handle);
}


/******************************************************************************
**
** Function         AVRC_MsgReq
**
** Description      This function is used to send the AVRCP byte stream in p_pkt
**                  down to AVCTP.
**
**                  It is expected that p_pkt->offset is at least AVCT_MSG_OFFSET
**                  p_pkt->layer_specific is AVCT_DATA_CTRL or AVCT_DATA_BROWSE
**                  p_pkt->event is AVRC_OP_VENDOR, AVRC_OP_PASS_THRU or AVRC_OP_BROWSE
**                  The above BT_HDR settings are set by the AVRC_Bld* functions.
**
** Returns          AVRC_SUCCESS if successful.
**                  AVRC_BAD_HANDLE if handle is invalid.
**
******************************************************************************/
UINT16 AVRC_MsgReq (UINT8 handle, UINT8 label, UINT8 ctype, BT_HDR *p_pkt)
{
#if (AVRC_METADATA_INCLUDED == TRUE)
    UINT8   *p_data;
    UINT8   cr = AVCT_CMD;
    BOOLEAN chk_frag = TRUE;
    UINT8   *p_start = NULL;
    tAVRC_FRAG_CB   *p_fcb;
    UINT16  len;
    BT_HDR  *p_pkt_new;

    if (!p_pkt) {
        return AVRC_BAD_PARAM;
    }

    AVRC_TRACE_DEBUG("%s handle = %u label = %u ctype = %u len = %d",
                     __func__, handle, label, ctype, p_pkt->len);

    if (ctype >= AVRC_RSP_NOT_IMPL) {
        cr = AVCT_RSP;
    }

    if (p_pkt->event == AVRC_OP_VENDOR) {
        /* add AVRCP Vendor Dependent headers */
        p_start = ((UINT8 *)(p_pkt + 1) + p_pkt->offset);
        p_pkt->offset -= AVRC_VENDOR_HDR_SIZE;
        p_pkt->len += AVRC_VENDOR_HDR_SIZE;
        p_data = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
        *p_data++       = (ctype & AVRC_CTYPE_MASK);
        *p_data++       = (AVRC_SUB_PANEL << AVRC_SUBTYPE_SHIFT);
        *p_data++       = AVRC_OP_VENDOR;
        AVRC_CO_ID_TO_BE_STREAM(p_data, AVRC_CO_METADATA);
    } else if (p_pkt->event == AVRC_OP_PASS_THRU) {
        /* add AVRCP Pass Through headers */
        p_start = ((UINT8 *)(p_pkt + 1) + p_pkt->offset);
        p_pkt->offset -= AVRC_PASS_THRU_SIZE;
        p_pkt->len += AVRC_PASS_THRU_SIZE;
        p_data = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
        *p_data++       = (ctype & AVRC_CTYPE_MASK);
        *p_data++       = (AVRC_SUB_PANEL << AVRC_SUBTYPE_SHIFT);
        *p_data++       = AVRC_OP_PASS_THRU;/* opcode */
        *p_data++       = AVRC_ID_VENDOR;   /* operation id */
        *p_data++       = 5;                /* operation data len */
        AVRC_CO_ID_TO_BE_STREAM(p_data, AVRC_CO_METADATA);
    }

    /* abandon previous fragments */
    p_fcb = &avrc_cb.fcb[handle];
    if (p_fcb->frag_enabled) {
        p_fcb->frag_enabled = FALSE;
    }

    if (p_fcb->p_fmsg) {
        osi_free(p_fcb->p_fmsg);
        p_fcb->p_fmsg = NULL;
    }

    /* AVRCP spec has not defined any control channel commands that needs fragmentation at this level
     * check for fragmentation only on the response */
    if ((cr == AVCT_RSP) && (chk_frag == TRUE)) {
        if (p_pkt->len > AVRC_MAX_CTRL_DATA_LEN) {
            int offset_len = MAX(AVCT_MSG_OFFSET, p_pkt->offset);
            p_pkt_new = (BT_HDR *)osi_malloc((UINT16)(AVRC_PACKET_LEN + offset_len
                                             + BT_HDR_SIZE));
            if (p_pkt_new && (p_start != NULL)) {
                p_fcb->frag_enabled = TRUE;
                p_fcb->p_fmsg       = p_pkt;
                p_fcb->frag_pdu     = *p_start;
                p_pkt               = p_pkt_new;
                p_pkt_new           = p_fcb->p_fmsg;
                p_pkt->len          = AVRC_MAX_CTRL_DATA_LEN;
                p_pkt->offset       = p_pkt_new->offset;
                p_pkt->layer_specific = p_pkt_new->layer_specific;
                p_pkt->event = p_pkt_new->event;
                p_data = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
                p_start -= AVRC_VENDOR_HDR_SIZE;
                memcpy (p_data, p_start, AVRC_MAX_CTRL_DATA_LEN);
                /* use AVRC start packet type */
                p_data += AVRC_VENDOR_HDR_SIZE;
                p_data++; /* pdu */
                *p_data++ = AVRC_PKT_START;
                /* 4 pdu, pkt_type & len */
                len = (AVRC_MAX_CTRL_DATA_LEN - AVRC_VENDOR_HDR_SIZE - AVRC_MIN_META_HDR_SIZE);
                UINT16_TO_BE_STREAM(p_data, len);

                /* prepare the left over for as an end fragment */
                avrc_prep_end_frag (handle);
                AVRC_TRACE_DEBUG ("%s p_pkt len:%d/%d, next len:%d", __func__,
                                  p_pkt->len, len, p_fcb->p_fmsg->len );
            } else {
                AVRC_TRACE_ERROR ("AVRC_MsgReq no buffers for fragmentation" );
                osi_free(p_pkt);
                return AVRC_NO_RESOURCES;
            }
        }
    }

    return AVCT_MsgReq( handle, label, cr, p_pkt);
#else
    return AVRC_NO_RESOURCES;
#endif
}


/******************************************************************************
**
** Function         AVRC_PassCmd
**
** Description      Send a PASS THROUGH command to the peer device.  This
**                  function can only be called for controller role connections.
**                  Any response message from the peer is passed back through
**                  the tAVRC_MSG_CBACK callback function.
**
**                  Input Parameters:
**                      handle: Handle of this connection.
**
**                      label: Transaction label.
**
**                      p_msg: Pointer to PASS THROUGH message structure.
**
**                  Output Parameters:
**                      None.
**
** Returns          AVRC_SUCCESS if successful.
**                  AVRC_BAD_HANDLE if handle is invalid.
**
******************************************************************************/
UINT16 AVRC_PassCmd(UINT8 handle, UINT8 label, tAVRC_MSG_PASS *p_msg)
{
    BT_HDR *p_buf;
    assert(p_msg != NULL);
    if (p_msg) {
        p_msg->hdr.ctype    = AVRC_CMD_CTRL;
        p_buf = avrc_pass_msg(p_msg);
        if (p_buf) {
            return AVCT_MsgReq( handle, label, AVCT_CMD, p_buf);
        }
    }
    return AVRC_NO_RESOURCES;
}

/******************************************************************************
**
** Function         AVRC_PassRsp
**
** Description      Send a PASS THROUGH response to the peer device.  This
**                  function can only be called for target role connections.
**                  This function must be called when a PASS THROUGH command
**                  message is received from the peer through the
**                  tAVRC_MSG_CBACK callback function.
**
**                  Input Parameters:
**                      handle: Handle of this connection.
**
**                      label: Transaction label.  Must be the same value as
**                      passed with the command message in the callback function.
**
**                      p_msg: Pointer to PASS THROUGH message structure.
**
**                  Output Parameters:
**                      None.
**
** Returns          AVRC_SUCCESS if successful.
**                  AVRC_BAD_HANDLE if handle is invalid.
**
******************************************************************************/
UINT16 AVRC_PassRsp(UINT8 handle, UINT8 label, tAVRC_MSG_PASS *p_msg)
{
    BT_HDR *p_buf;
    assert(p_msg != NULL);
    if (p_msg) {
        p_buf = avrc_pass_msg(p_msg);
        if (p_buf) {
            return AVCT_MsgReq( handle, label, AVCT_RSP, p_buf);
        }
    }
    return AVRC_NO_RESOURCES;
}

#endif /* #if (defined(AVRC_INCLUDED) && AVRC_INCLUDED == TRUE) */

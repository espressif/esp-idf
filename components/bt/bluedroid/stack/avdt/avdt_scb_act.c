/******************************************************************************
 *
 *  Copyright (C) 2002-2012 Broadcom Corporation
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
 *  This module contains the action functions associated with the stream
 *  control block state machine.
 *
 ******************************************************************************/

#include <string.h>
#include "bt_types.h"
#include "bt_target.h"
#include "bt_utils.h"
#include "avdt_api.h"
#include "avdtc_api.h"
#include "avdt_int.h"
#include "gki.h"
#include "btu.h"

#if (defined(AVDT_INCLUDED) && AVDT_INCLUDED == TRUE)

/* This table is used to lookup the callback event that matches a particular
** state machine API request event.  Note that state machine API request
** events are at the beginning of the event list starting at zero, thus
** allowing for this table.
*/
const UINT8 avdt_scb_cback_evt[] = {
    0,                          /* API_REMOVE_EVT (no event) */
    AVDT_WRITE_CFM_EVT,         /* API_WRITE_REQ_EVT */
    0,                          /* API_GETCONFIG_REQ_EVT (no event) */
    0,                          /* API_DELAY_RPT_REQ_EVT (no event) */
    AVDT_OPEN_CFM_EVT,          /* API_SETCONFIG_REQ_EVT */
    AVDT_OPEN_CFM_EVT,          /* API_OPEN_REQ_EVT */
    AVDT_CLOSE_CFM_EVT,         /* API_CLOSE_REQ_EVT */
    AVDT_RECONFIG_CFM_EVT,      /* API_RECONFIG_REQ_EVT */
    AVDT_SECURITY_CFM_EVT,      /* API_SECURITY_REQ_EVT */
    0                           /* API_ABORT_REQ_EVT (no event) */
};

/* This table is used to look up the callback event based on the signaling
** role when the stream is closed.
*/
const UINT8 avdt_scb_role_evt[] = {
    AVDT_CLOSE_IND_EVT,         /* AVDT_CLOSE_ACP */
    AVDT_CLOSE_CFM_EVT,         /* AVDT_CLOSE_INT */
    AVDT_CLOSE_IND_EVT,         /* AVDT_OPEN_ACP */
    AVDT_OPEN_CFM_EVT           /* AVDT_OPEN_INT */
};

/*******************************************************************************
**
** Function         avdt_scb_gen_ssrc
**
** Description      This function generates a SSRC number unique to the stream.
**
** Returns          SSRC value.
**
*******************************************************************************/
UINT32 avdt_scb_gen_ssrc(tAVDT_SCB *p_scb)
{
    /* combine the value of the media type and codec type of the SCB */
    return ((UINT32)(p_scb->cs.cfg.codec_info[1] | p_scb->cs.cfg.codec_info[2]));
}

/*******************************************************************************
**
** Function         avdt_scb_hdl_abort_cmd
**
** Description      This function sends the SCB an AVDT_SCB_API_ABORT_RSP_EVT
**                  to initiate sending of an abort response message.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_abort_cmd(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    p_scb->role = AVDT_CLOSE_ACP;
    avdt_scb_event(p_scb, AVDT_SCB_API_ABORT_RSP_EVT, p_data);
}

/*******************************************************************************
**
** Function         avdt_scb_hdl_abort_rsp
**
** Description      This function is an empty function; it serves as a
**                  placeholder for a conformance API action function.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_abort_rsp(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    UNUSED(p_scb);
    UNUSED(p_data);
    return;
}

/*******************************************************************************
**
** Function         avdt_scb_hdl_close_cmd
**
** Description      This function sends the SCB an AVDT_SCB_API_CLOSE_RSP_EVT
**                  to initiate sending of a close response message.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_close_cmd(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    p_scb->role = AVDT_CLOSE_ACP;
    avdt_scb_event(p_scb, AVDT_SCB_API_CLOSE_RSP_EVT, p_data);
}

/*******************************************************************************
**
** Function         avdt_scb_hdl_close_rsp
**
** Description      This function sets the close_code variable to the error
**                  code returned in the close response.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_close_rsp(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    p_scb->close_code = p_data->msg.hdr.err_code;
}

/*******************************************************************************
**
** Function         avdt_scb_hdl_getconfig_cmd
**
** Description      This function retrieves the configuration parameters of
**                  the SCB and sends the SCB an AVDT_SCB_API_GETCONFIG_RSP_EVT
**                  to initiate sending of a get configuration response message.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_getconfig_cmd(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    p_data->msg.svccap.p_cfg = &p_scb->curr_cfg;

    avdt_scb_event(p_scb, AVDT_SCB_API_GETCONFIG_RSP_EVT, p_data);
}

/*******************************************************************************
**
** Function         avdt_scb_hdl_getconfig_rsp
**
** Description      This function is an empty function; it serves as a
**                  placeholder for a conformance API action function.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_getconfig_rsp(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    UNUSED(p_scb);
    UNUSED(p_data);
    return;
}

/*******************************************************************************
**
** Function         avdt_scb_hdl_open_cmd
**
** Description      This function sends the SCB an AVDT_SCB_API_OPEN_RSP_EVT
**                  to initiate sending of an open response message.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_open_cmd(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    avdt_scb_event(p_scb, AVDT_SCB_API_OPEN_RSP_EVT, p_data);
}

/*******************************************************************************
**
** Function         avdt_scb_hdl_open_rej
**
** Description      This function calls the application callback function
**                  indicating the open request has failed.  It initializes
**                  certain SCB variables and sends a AVDT_CCB_UL_CLOSE_EVT
**                  to the CCB.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_open_rej(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    /* do exactly same as setconfig reject */
    avdt_scb_hdl_setconfig_rej(p_scb, p_data);
}

/*******************************************************************************
**
** Function         avdt_scb_hdl_open_rsp
**
** Description      This function calls avdt_ad_open_req() to initiate
**                  connection of the transport channel for this stream.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_open_rsp(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    UNUSED(p_data);

    /* initiate opening of trans channels for this SEID */
    p_scb->role = AVDT_OPEN_INT;
    avdt_ad_open_req(AVDT_CHAN_MEDIA, p_scb->p_ccb, p_scb, AVDT_INT);

    /* start tc connect timer */
    btu_start_timer(&p_scb->timer_entry, BTU_TTYPE_AVDT_SCB_TC, AVDT_SCB_TC_CONN_TOUT);
}

/*******************************************************************************
**
** Function         avdt_scb_hdl_pkt_no_frag
**
** Description
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_pkt_no_frag(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    UINT8   *p, *p_start;
    UINT8   o_v, o_p, o_x, o_cc;
    UINT8   m_pt;
    UINT8   marker;
    UINT16  seq;
    UINT32  time_stamp;
    UINT16  offset;
    UINT16  ex_len;
    UINT8   pad_len = 0;

    p = p_start = (UINT8 *)(p_data->p_pkt + 1) + p_data->p_pkt->offset;

    /* parse media packet header */
    AVDT_MSG_PRS_OCTET1(p, o_v, o_p, o_x, o_cc);
    AVDT_MSG_PRS_M_PT(p, m_pt, marker);
    BE_STREAM_TO_UINT16(seq, p);
    BE_STREAM_TO_UINT32(time_stamp, p);
    p += 4;

    UNUSED(o_v);

    /* skip over any csrc's in packet */
    p += o_cc * 4;

    /* check for and skip over extension header */
    if (o_x) {
        p += 2;
        BE_STREAM_TO_UINT16(ex_len, p);
        p += ex_len * 4;
    }

    /* save our new offset */
    offset = (UINT16) (p - p_start);

    /* adjust length for any padding at end of packet */
    if (o_p) {
        /* padding length in last byte of packet */
        pad_len =  *(p_start + p_data->p_pkt->len);
    }

    /* do sanity check */
    if ((offset > p_data->p_pkt->len) || ((pad_len + offset) > p_data->p_pkt->len)) {
        AVDT_TRACE_WARNING("Got bad media packet");
        GKI_freebuf(p_data->p_pkt);
    }
    /* adjust offset and length and send it up */
    else {
        p_data->p_pkt->len -= (offset + pad_len);
        p_data->p_pkt->offset += offset;

        if (p_scb->cs.p_data_cback != NULL) {
            /* report sequence number */
            p_data->p_pkt->layer_specific = seq;
            (*p_scb->cs.p_data_cback)(avdt_scb_to_hdl(p_scb), p_data->p_pkt,
                                      time_stamp, (UINT8)(m_pt | (marker << 7)));
        } else {
#if AVDT_MULTIPLEXING == TRUE
            if ((p_scb->cs.p_media_cback != NULL)
                    && (p_scb->p_media_buf != NULL)
                    && (p_scb->media_buf_len > p_data->p_pkt->len)) {
                /* media buffer enough length is assigned by application. Lets use it*/
                memcpy(p_scb->p_media_buf, (UINT8 *)(p_data->p_pkt + 1) + p_data->p_pkt->offset,
                       p_data->p_pkt->len);
                (*p_scb->cs.p_media_cback)(avdt_scb_to_hdl(p_scb), p_scb->p_media_buf,
                                           p_scb->media_buf_len, time_stamp, seq, m_pt, marker);
            }
#endif
            GKI_freebuf(p_data->p_pkt);
        }
    }
}

#if AVDT_REPORTING == TRUE
/*******************************************************************************
**
** Function         avdt_scb_hdl_report
**
** Description
**
** Returns          Nothing.
**
*******************************************************************************/
UINT8 *avdt_scb_hdl_report(tAVDT_SCB *p_scb, UINT8 *p, UINT16 len)
{
    UINT16  result = AVDT_SUCCESS;
    UINT8   *p_start = p;
    UINT32  ssrc;
    UINT8   o_v, o_p, o_cc;
    AVDT_REPORT_TYPE    pt;
    tAVDT_REPORT_DATA   report, *p_rpt;

    AVDT_TRACE_DEBUG( "avdt_scb_hdl_report");
    if (p_scb->cs.p_report_cback) {
        p_rpt = &report;
        /* parse report packet header */
        AVDT_MSG_PRS_RPT_OCTET1(p, o_v, o_p, o_cc);
        pt = *p++;
        p += 2;
        BE_STREAM_TO_UINT32(ssrc, p);

        UNUSED(o_p);
        UNUSED(o_v);

        switch (pt) {
        case AVDT_RTCP_PT_SR:   /* the packet type - SR (Sender Report) */
            BE_STREAM_TO_UINT32(report.sr.ntp_sec, p);
            BE_STREAM_TO_UINT32(report.sr.ntp_frac, p);
            BE_STREAM_TO_UINT32(report.sr.rtp_time, p);
            BE_STREAM_TO_UINT32(report.sr.pkt_count, p);
            BE_STREAM_TO_UINT32(report.sr.octet_count, p);
            break;

        case AVDT_RTCP_PT_RR:   /* the packet type - RR (Receiver Report) */
            report.rr.frag_lost = *p;
            BE_STREAM_TO_UINT32(report.rr.packet_lost, p);
            report.rr.packet_lost &= 0xFFFFFF;
            BE_STREAM_TO_UINT32(report.rr.seq_num_rcvd, p);
            BE_STREAM_TO_UINT32(report.rr.jitter, p);
            BE_STREAM_TO_UINT32(report.rr.lsr, p);
            BE_STREAM_TO_UINT32(report.rr.dlsr, p);
            break;

        case AVDT_RTCP_PT_SDES: /* the packet type - SDES (Source Description) */
            if (*p == AVDT_RTCP_SDES_CNAME) {
                p_rpt = (tAVDT_REPORT_DATA *)(p + 2);
            } else {
                AVDT_TRACE_WARNING( " - SDES SSRC=0x%08x sc=%d %d len=%d %s\n",
                                    ssrc, o_cc, *p, *(p + 1), p + 2);
                result = AVDT_BUSY;
            }
            break;

        default:
            AVDT_TRACE_ERROR( "Bad Report pkt - packet type: %d\n", pt);
            result = AVDT_BAD_PARAMS;
        }

        if (result == AVDT_SUCCESS) {
            (*p_scb->cs.p_report_cback)(avdt_scb_to_hdl(p_scb), pt, p_rpt);
        }

    }
    p_start += len;
    return p_start;
}
#endif

#if AVDT_MULTIPLEXING == TRUE
/*******************************************************************************
**
** Function         avdt_scb_hdl_pkt_frag
**
** Description
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_pkt_frag(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    /* Fields of Adaptation Layer Header */
    UINT8   al_tsid, al_frag, al_lcode;
    UINT16  al_len;
    /* media header fields */
    UINT8   o_v, o_p, o_x, o_cc;
    UINT8   m_pt;
    UINT8   marker;
    UINT16  seq;
    UINT32  time_stamp;
    UINT32  ssrc;
    UINT16  ex_len;
    UINT8   pad_len;
    /* other variables */
    UINT8   *p; /* current pointer */
    UINT8   *p_end; /* end of all packet */
    UINT8   *p_payload; /* pointer to media fragment payload in the buffer */
    UINT32  payload_len; /* payload length */
    UINT16  frag_len; /* fragment length */

    p = (UINT8 *)(p_data->p_pkt + 1) + p_data->p_pkt->offset;
    p_end = p + p_data->p_pkt->len;
    /* parse all fragments */
    while (p < p_end) {
        if (p_end - p < 4) { /* length check. maximum length of AL header = 4 */
            AVDT_TRACE_WARNING("p_end: %p - p:%p < 4\n", p_end, p);
            break;
        }

        /* parse first byte */
        al_tsid = (*p) >> 3;
        al_frag = ( (*p) >> 2 ) & 0x01;
        al_lcode = (*p++) & AVDT_ALH_LCODE_MASK;

        /* in case of TSID=00000, a second AL header byte, before the length field,
        ** is expected and contains the actual TSID, aligned with MSB */
        if (al_tsid == 0) {
            al_tsid = *p++;
        }

        /* get remaining media length on base of lcode */
        switch (al_lcode) {
        case AVDT_ALH_LCODE_NONE:  /* No length field present. Take length from l2cap */
            al_len = (UINT16)(p_end - p);
            break;
        case AVDT_ALH_LCODE_16BIT:  /* 16 bit length field */
            BE_STREAM_TO_UINT16(al_len, p);
            break;
        case AVDT_ALH_LCODE_9BITM0:  /* 9 bit length field, MSB = 0, 8 LSBs in 1 octet following */
            al_len = *p++;
            break;
        default:    /* 9 bit length field, MSB = 1, 8 LSBs in 1 octet following */
            al_len = (UINT16) * p++ + 0x100;
        }

        /* max fragment length */
        frag_len = (UINT16)(p_end - p);
        /* if it isn't last fragment */
        if (frag_len >= al_len) {
            frag_len = al_len;
        }

        /* check TSID corresponds to config */
        if (al_tsid != p_scb->curr_cfg.mux_tsid_media) {
#if AVDT_REPORTING == TRUE
            if ((p_scb->curr_cfg.psc_mask & AVDT_PSC_REPORT) &&
                    (al_tsid == p_scb->curr_cfg.mux_tsid_report)) {
                /* parse reporting packet */
                p = avdt_scb_hdl_report(p_scb, p, frag_len);
                continue;
            } else
#endif
            {
                AVDT_TRACE_WARNING("bad tsid: %d, mux_tsid_media:%d\n", al_tsid, p_scb->curr_cfg.mux_tsid_media);
                break;
            }
        }
        /* check are buffer for assembling and related callback set */
        else if ((p_scb->p_media_buf == NULL) || (p_scb->cs.p_media_cback == NULL)) {
            AVDT_TRACE_WARNING("NULL p_media_buf or p_media_cback");
            break;
        }


        /* it is media fragment beginning */
        if (!al_frag) { /* is it first fragment of original media packet */
            AVDT_TRACE_DEBUG("al:%d media:%d\n",
                             al_len, p_scb->media_buf_len);

            p_scb->frag_off = 0;
            p_scb->frag_org_len = al_len; /* total length of original media packet */
            /* length check: minimum length of media header is 12 */
            if (p_scb->frag_org_len < 12) {
                AVDT_TRACE_WARNING("bad al_len: %d(<12)\n", al_len);
                break;
            }
            /* check that data fit into buffer */
            if (al_len > p_scb->media_buf_len) {
                AVDT_TRACE_WARNING("bad al_len: %d(>%d)\n", al_len, p_scb->media_buf_len);
                break;
            }
            /* make sure it is the last fragment in l2cap packet */
            if (p + al_len < p_end) {
                AVDT_TRACE_WARNING("bad al_len: %d(>%d)\n", al_len, p_scb->media_buf_len);
                break;
            }
        } else {
            AVDT_TRACE_DEBUG("al:%d media:%d frag_org_len:%d frag_off:%d\n",
                             al_len, p_scb->media_buf_len, p_scb->frag_org_len, p_scb->frag_off);

            /* check that remaining length from AL header equals to original len - length of already received fragments */
            if (al_len != p_scb->frag_org_len - p_scb->frag_off) {
                AVDT_TRACE_WARNING("al_len:%d != (frag_org_len:%d - frag_off:%d) %d\n",
                                   al_len, p_scb->frag_org_len, p_scb->frag_off,
                                   (p_scb->frag_org_len - p_scb->frag_off));
                break;
            }

            /* do sanity check */
            if (p_scb->frag_off == 0) {
                AVDT_TRACE_WARNING("frag_off=0");
                break;
            }
        }
        /* do common sanity check */
        if ((p_scb->frag_org_len <= p_scb->frag_off) || (p_scb->frag_org_len >= p_scb->media_buf_len)) {
            AVDT_TRACE_WARNING("common sanity frag_off:%d frag_org_len:%d media_buf_len:%d\n",
                               p_scb->frag_off, p_scb->frag_org_len, p_scb->media_buf_len);
            break;
        }

        AVDT_TRACE_DEBUG("Received fragment org_len=%d off=%d al_len=%d frag_len=%d\n",
                         p_scb->frag_org_len, p_scb->frag_off, al_len, frag_len);

        /* copy fragment into buffer */
        memcpy(p_scb->p_media_buf + p_scb->frag_off, p, frag_len);
        p_scb->frag_off += frag_len;
        /* move to the next fragment */
        p += frag_len;
        /* if it is last fragment in original media packet then process total media pocket */
        if (p_scb->frag_off == p_scb->frag_org_len) {
            p_payload = p_scb->p_media_buf;

            /* media header */
            AVDT_MSG_PRS_OCTET1(p_payload, o_v, o_p, o_x, o_cc);
            AVDT_MSG_PRS_M_PT(p_payload, m_pt, marker);
            BE_STREAM_TO_UINT16(seq, p_payload);
            BE_STREAM_TO_UINT32(time_stamp, p_payload);
            BE_STREAM_TO_UINT32(ssrc, p_payload);

            UNUSED(o_v);
            UNUSED(ssrc);

            /* skip over any csrc's in packet */
            p_payload += o_cc * 4;

            /* check for and skip over extension header */
            if (o_x) {
                if (p_scb->p_media_buf + p_scb->frag_off - p_payload < 4) {
                    AVDT_TRACE_WARNING("length check frag_off:%d p_media_buf:%p p_payload:%p\n",
                                       p_scb->frag_off, p_scb->p_media_buf, p_payload);
                    break;/* length check */
                }
                p_payload += 2;
                BE_STREAM_TO_UINT16(ex_len, p_payload);
                p_payload += ex_len * 4;
            }

            if (p_payload >= p_scb->p_media_buf + p_scb->frag_off) {
                AVDT_TRACE_WARNING("length check2 frag_off:%d p_media_buf:%p p_payload:%p\n",
                                   p_scb->frag_off, p_scb->p_media_buf, p_payload);
                break;/* length check */
            }

            /* adjust length for any padding at end of packet */
            if (o_p) {
                /* padding length in last byte of packet */
                pad_len =  *(p_scb->p_media_buf + p_scb->frag_off - 1);
            } else {
                pad_len =  0;
            }
            /* payload length */
            payload_len = (UINT32)(p_scb->p_media_buf + p_scb->frag_off - pad_len - p_payload);

            AVDT_TRACE_DEBUG("Received last fragment header=%d len=%d\n",
                             p_payload - p_scb->p_media_buf, payload_len);

            /* send total media packet up */
            if (p_scb->cs.p_media_cback != NULL) {
                (*p_scb->cs.p_media_cback)(avdt_scb_to_hdl(p_scb), p_payload,
                                           payload_len, time_stamp, seq, m_pt, marker);
            }
        }
    } /* while(p < p_end) */

    if (p < p_end) {
        AVDT_TRACE_WARNING("*** Got bad media packet");
    }
    GKI_freebuf(p_data->p_pkt);
}
#endif

/*******************************************************************************
**
** Function         avdt_scb_hdl_pkt
**
** Description
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_pkt(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
#if AVDT_REPORTING == TRUE
    UINT8 *p;
#endif

#if AVDT_MULTIPLEXING == TRUE
    /* select right function in dependance of is fragmentation supported or not */
    if ( 0 != (p_scb->curr_cfg.psc_mask & AVDT_PSC_MUX)) {
        avdt_scb_hdl_pkt_frag(p_scb, p_data);
    } else
#endif
#if AVDT_REPORTING == TRUE
        if (p_data->p_pkt->layer_specific == AVDT_CHAN_REPORT) {
            p = (UINT8 *)(p_data->p_pkt + 1) + p_data->p_pkt->offset;
            avdt_scb_hdl_report(p_scb, p, p_data->p_pkt->len);
            GKI_freebuf(p_data->p_pkt);
        } else
#endif
            avdt_scb_hdl_pkt_no_frag(p_scb, p_data);
}

/*******************************************************************************
**
** Function         avdt_scb_drop_pkt
**
** Description      Drop an incoming media packet.  This function is called if
**                  a media packet is received in any state besides streaming.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_drop_pkt(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    UNUSED(p_scb);

    GKI_freebuf(p_data->p_pkt);
    AVDT_TRACE_ERROR(" avdt_scb_drop_pkt Dropped incoming media packet");
}

/*******************************************************************************
**
** Function         avdt_scb_hdl_reconfig_cmd
**
** Description      This function calls the application callback function
**                  with a reconfiguration indication.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_reconfig_cmd(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    /* if command not supported */
    if (p_scb->cs.nsc_mask & AVDT_NSC_RECONFIG) {
        /* send reject */
        p_data->msg.hdr.err_code = AVDT_ERR_NSC;
        p_data->msg.hdr.err_param = 0;
        avdt_scb_event(p_scb, AVDT_SCB_API_RECONFIG_RSP_EVT, p_data);
    } else {
        /* store requested configuration */
        memcpy(&p_scb->req_cfg, p_data->msg.reconfig_cmd.p_cfg, sizeof(tAVDT_CFG));

        /* call application callback */
        (*p_scb->cs.p_ctrl_cback)(avdt_scb_to_hdl(p_scb),
                                  NULL,
                                  AVDT_RECONFIG_IND_EVT,
                                  (tAVDT_CTRL *) &p_data->msg.reconfig_cmd);
    }
}

/*******************************************************************************
**
** Function         avdt_scb_hdl_reconfig_rsp
**
** Description      This function calls the application callback function
**                  with a reconfiguration confirm.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_reconfig_rsp(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    if (p_data->msg.hdr.err_code == 0) {
        /* store new configuration */
        if (p_scb->req_cfg.num_codec > 0) {
            p_scb->curr_cfg.num_codec = p_scb->req_cfg.num_codec;
            memcpy(p_scb->curr_cfg.codec_info, p_scb->req_cfg.codec_info, AVDT_CODEC_SIZE);
        }
        if (p_scb->req_cfg.num_protect > 0) {
            p_scb->curr_cfg.num_protect = p_scb->req_cfg.num_protect;
            memcpy(p_scb->curr_cfg.protect_info, p_scb->req_cfg.protect_info, AVDT_PROTECT_SIZE);
        }
    }

    p_data->msg.svccap.p_cfg = &p_scb->curr_cfg;

    /* call application callback */
    (*p_scb->cs.p_ctrl_cback)(avdt_scb_to_hdl(p_scb),
                              NULL,
                              AVDT_RECONFIG_CFM_EVT,
                              (tAVDT_CTRL *) &p_data->msg.svccap);
}

/*******************************************************************************
**
** Function         avdt_scb_hdl_security_cmd
**
** Description      This function calls the application callback with a
**                  security indication.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_security_cmd(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    /* if command not supported */
    if (p_scb->cs.nsc_mask & AVDT_NSC_SECURITY) {
        /* send reject */
        p_data->msg.hdr.err_code = AVDT_ERR_NSC;
        avdt_scb_event(p_scb, AVDT_SCB_API_SECURITY_RSP_EVT, p_data);
    } else {
        /* call application callback */
        (*p_scb->cs.p_ctrl_cback)(avdt_scb_to_hdl(p_scb),
                                  NULL,
                                  AVDT_SECURITY_IND_EVT,
                                  (tAVDT_CTRL *) &p_data->msg.security_cmd);
    }
}

/*******************************************************************************
**
** Function         avdt_scb_hdl_security_rsp
**
** Description      This function calls the application callback with a
**                  security confirm.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_security_rsp(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    /* call application callback */
    (*p_scb->cs.p_ctrl_cback)(avdt_scb_to_hdl(p_scb),
                              NULL,
                              AVDT_SECURITY_CFM_EVT,
                              (tAVDT_CTRL *) &p_data->msg.security_cmd);
}

/*******************************************************************************
**
** Function         avdt_scb_hdl_setconfig_cmd
**
** Description      This function marks the SCB as in use and copies the
**                  configuration and peer SEID to the SCB.  It then calls
**                  the application callback with a configuration indication.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_setconfig_cmd(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    tAVDT_CFG *p_cfg;

    if (!p_scb->in_use) {
        p_cfg = p_data->msg.config_cmd.p_cfg;
        if (p_scb->cs.cfg.codec_info[AVDT_CODEC_TYPE_INDEX] == p_cfg->codec_info[AVDT_CODEC_TYPE_INDEX]) {
            /* set sep as in use */
            p_scb->in_use = TRUE;

            /* copy info to scb */
            p_scb->p_ccb = avdt_ccb_by_idx(p_data->msg.config_cmd.hdr.ccb_idx);
            p_scb->peer_seid = p_data->msg.config_cmd.int_seid;
            memcpy(&p_scb->req_cfg, p_cfg, sizeof(tAVDT_CFG));
            /* call app callback */
            (*p_scb->cs.p_ctrl_cback)(avdt_scb_to_hdl(p_scb), /* handle of scb- which is same as sep handle of bta_av_cb.p_scb*/
                                      p_scb->p_ccb ? p_scb->p_ccb->peer_addr : NULL,
                                      AVDT_CONFIG_IND_EVT,
                                      (tAVDT_CTRL *) &p_data->msg.config_cmd);
        } else {
            p_data->msg.hdr.err_code = AVDT_ERR_UNSUP_CFG;
            p_data->msg.hdr.err_param = 0;
            avdt_msg_send_rej(avdt_ccb_by_idx(p_data->msg.hdr.ccb_idx),
                              p_data->msg.hdr.sig_id, &p_data->msg);
        }
    } else {
        avdt_scb_rej_in_use(p_scb, p_data);
    }
}

/*******************************************************************************
**
** Function         avdt_scb_hdl_setconfig_rej
**
** Description      This function marks the SCB as not in use and calls the
**                  application callback with an open confirm indicating failure.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_setconfig_rej(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    /* clear scb variables */
    avdt_scb_clr_vars(p_scb, p_data);

    /* tell ccb we're done with signaling channel */
    avdt_ccb_event(avdt_ccb_by_idx(p_data->msg.hdr.ccb_idx), AVDT_CCB_UL_CLOSE_EVT, NULL);

    /* call application callback */
    (*p_scb->cs.p_ctrl_cback)(avdt_scb_to_hdl(p_scb),
                              NULL,
                              AVDT_OPEN_CFM_EVT,
                              (tAVDT_CTRL *) &p_data->msg.hdr);
}

/*******************************************************************************
**
** Function         avdt_scb_hdl_setconfig_rsp
**
** Description      This function sends the SCB an AVDT_SCB_API_OPEN_REQ_EVT
**                  to initiate sending of an open command message.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_setconfig_rsp(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    tAVDT_EVT_HDR   single;
    UNUSED(p_data);

    if (p_scb->p_ccb != NULL) {
        /* save configuration */
        memcpy(&p_scb->curr_cfg, &p_scb->req_cfg, sizeof(tAVDT_CFG));

        /* initiate open */
        single.seid = p_scb->peer_seid;
        avdt_scb_event(p_scb, AVDT_SCB_API_OPEN_REQ_EVT, (tAVDT_SCB_EVT *) &single);
    }
}

/*******************************************************************************
**
** Function         avdt_scb_hdl_start_cmd
**
** Description      This function calls the application callback with a
**                  start indication.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_start_cmd(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    UNUSED(p_data);

    (*p_scb->cs.p_ctrl_cback)(avdt_scb_to_hdl(p_scb),
                              p_scb->p_ccb ? p_scb->p_ccb->peer_addr : NULL,
                              AVDT_START_IND_EVT,
                              NULL);
}

/*******************************************************************************
**
** Function         avdt_scb_hdl_start_rsp
**
** Description      This function calls the application callback with a
**                  start confirm.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_start_rsp(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    (*p_scb->cs.p_ctrl_cback)(avdt_scb_to_hdl(p_scb),
                              p_scb->p_ccb ? p_scb->p_ccb->peer_addr : NULL,
                              AVDT_START_CFM_EVT,
                              (tAVDT_CTRL *) &p_data->msg.hdr);
}

/*******************************************************************************
**
** Function         avdt_scb_hdl_suspend_cmd
**
** Description      This function calls the application callback with a suspend
**                  indication.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_suspend_cmd(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    UNUSED(p_data);

    (*p_scb->cs.p_ctrl_cback)(avdt_scb_to_hdl(p_scb),
                              p_scb->p_ccb ? p_scb->p_ccb->peer_addr : NULL,
                              AVDT_SUSPEND_IND_EVT,
                              NULL);
}

/*******************************************************************************
**
** Function         avdt_scb_hdl_suspend_rsp
**
** Description      This function calls the application callback with a suspend
**                  confirm.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_suspend_rsp(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    (*p_scb->cs.p_ctrl_cback)(avdt_scb_to_hdl(p_scb),
                              p_scb->p_ccb ? p_scb->p_ccb->peer_addr : NULL,
                              AVDT_SUSPEND_CFM_EVT,
                              (tAVDT_CTRL *) &p_data->msg.hdr);
}

/*******************************************************************************
**
** Function         avdt_scb_hdl_tc_close
**
** Description      This function is called when the transport channel is
**                  closed.  It marks the SCB as not in use and
**                  initializes certain SCB parameters.  It then sends
**                  an AVDT_CCB_UL_CLOSE_EVT to the CCB if the SCB
**                  initiated the close.  It then checks to see if the SCB
**                  is to be removed.  If it is it deallocates the SCB.  Finally,
**                  it calls the application callback with a close indication.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_tc_close(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    UINT8               hdl = avdt_scb_to_hdl(p_scb);
    tAVDT_CTRL_CBACK    *p_ctrl_cback = p_scb->cs.p_ctrl_cback;
    tAVDT_CTRL          avdt_ctrl;
    UINT8               event;
    tAVDT_CCB           *p_ccb = p_scb->p_ccb;
    BD_ADDR remote_addr;


    memcpy (remote_addr, p_ccb->peer_addr, BD_ADDR_LEN);

    /* set up hdr */
    avdt_ctrl.hdr.err_code = p_scb->close_code;
    if (p_data) {
        avdt_ctrl.hdr.err_param = p_data->close.disc_rsn;
    } else {
        avdt_ctrl.hdr.err_param = AVDT_DISC_RSN_NORMAL;
    }

    /* clear sep variables */
    avdt_scb_clr_vars(p_scb, p_data);
    p_scb->media_seq = 0;
    p_scb->cong = FALSE;

    /* free pkt we're holding, if any */
    if (p_scb->p_pkt != NULL) {
        GKI_freebuf(p_scb->p_pkt);
        p_scb->p_pkt = NULL;
    }

    /* stop transport channel timer */
    btu_stop_timer(&p_scb->timer_entry);

    if ((p_scb->role == AVDT_CLOSE_INT) || (p_scb->role == AVDT_OPEN_INT)) {
        /* tell ccb we're done with signaling channel */
        avdt_ccb_event(p_ccb, AVDT_CCB_UL_CLOSE_EVT, NULL);
    }
    event = (p_scb->role == AVDT_CLOSE_INT) ? AVDT_CLOSE_CFM_EVT : AVDT_CLOSE_IND_EVT;
    p_scb->role = AVDT_CLOSE_ACP;

    if (p_scb->remove) {
        avdt_scb_dealloc(p_scb, NULL);
    }

    /* call app callback */
    (*p_ctrl_cback)(hdl, remote_addr, event, &avdt_ctrl);
}

/*******************************************************************************
**
** Function         avdt_scb_snd_delay_rpt_req
**
** Description      This function calls the application callback with a delay
**                  report.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_snd_delay_rpt_req (tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    avdt_msg_send_cmd(p_scb->p_ccb, p_scb, AVDT_SIG_DELAY_RPT, (tAVDT_MSG *) &p_data->apidelay);
}

/*******************************************************************************
**
** Function         avdt_scb_hdl_delay_rpt_cmd
**
** Description      This function calls the application callback with a delay
**                  report.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_delay_rpt_cmd (tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    (*p_scb->cs.p_ctrl_cback)(avdt_scb_to_hdl(p_scb),
                              p_scb->p_ccb ? p_scb->p_ccb->peer_addr : NULL,
                              AVDT_DELAY_REPORT_EVT,
                              (tAVDT_CTRL *) &p_data->msg.hdr);

    if (p_scb->p_ccb) {
        avdt_msg_send_rsp(p_scb->p_ccb, AVDT_SIG_DELAY_RPT, &p_data->msg);
    } else {
        avdt_scb_rej_not_in_use(p_scb, p_data);
    }
}

/*******************************************************************************
**
** Function         avdt_scb_hdl_delay_rpt_rsp
**
** Description      This function calls the application callback with a delay
**                  report.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_delay_rpt_rsp (tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    (*p_scb->cs.p_ctrl_cback)(avdt_scb_to_hdl(p_scb),
                              p_scb->p_ccb ? p_scb->p_ccb->peer_addr : NULL,
                              AVDT_DELAY_REPORT_CFM_EVT,
                              (tAVDT_CTRL *) &p_data->msg.hdr);
}

#if AVDT_REPORTING == TRUE
/*******************************************************************************
**
** Function         avdt_scb_hdl_tc_close_sto
**
** Description      This function is called when a channel is closed in OPEN
**                  state.  Check the channel type and process accordingly.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_tc_close_sto(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    tAVDT_CTRL          avdt_ctrl;
    /* AVDT_CHAN_SIG does not visit this action */
    if (p_data && p_data->close.type != AVDT_CHAN_MEDIA) {
        /* it's reporting or recovery channel,
         * the channel close in open state means the peer does not support it */
        if (p_data->close.old_tc_state == AVDT_AD_ST_OPEN) {
            avdt_ctrl.hdr.err_code = 0;
            avdt_ctrl.hdr.err_param = 0;
            /* call app callback */
            (*p_scb->cs.p_ctrl_cback)(avdt_scb_to_hdl(p_scb),
                                      p_scb->p_ccb ? p_scb->p_ccb->peer_addr : NULL,
                                      AVDT_REPORT_DISCONN_EVT, &avdt_ctrl);
        }
    } else {
        /* must be in OPEN state. need to go back to idle */
        avdt_scb_event(p_scb, AVDT_SCB_MSG_ABORT_RSP_EVT, NULL);
        avdt_scb_hdl_tc_close(p_scb, p_data);
    }
}
#endif

/*******************************************************************************
**
** Function         avdt_scb_hdl_tc_open
**
** Description      This function is called when the transport channel is
**                  opened while in the opening state.  It calls the
**                  application callback with an open indication or open
**                  confirm depending on who initiated the open procedure.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_tc_open(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    UINT8   event;
#if AVDT_REPORTING == TRUE
    UINT8   role;
#endif

    /* stop transport channel connect timer */
    btu_stop_timer(&p_scb->timer_entry);

    event = (p_scb->role == AVDT_OPEN_INT) ? AVDT_OPEN_CFM_EVT : AVDT_OPEN_IND_EVT;
    p_data->open.hdr.err_code = 0;

    AVDT_TRACE_DEBUG("psc_mask: cfg: 0x%x, req:0x%x, cur: 0x%x\n",
                     p_scb->cs.cfg.psc_mask, p_scb->req_cfg.psc_mask, p_scb->curr_cfg.psc_mask);
#if AVDT_REPORTING == TRUE
    if (p_scb->curr_cfg.psc_mask & AVDT_PSC_REPORT) {
        /* open the reporting channel, if both devices support it */
        role = (p_scb->role == AVDT_OPEN_INT) ? AVDT_INT : AVDT_ACP;
        avdt_ad_open_req(AVDT_CHAN_REPORT, p_scb->p_ccb, p_scb, role);
    }
#endif

    /* call app callback */
    (*p_scb->cs.p_ctrl_cback)(avdt_scb_to_hdl(p_scb),
                              p_scb->p_ccb ? p_scb->p_ccb->peer_addr : NULL,
                              event,
                              (tAVDT_CTRL *) &p_data->open);
}

#if AVDT_REPORTING == TRUE
/*******************************************************************************
**
** Function         avdt_scb_hdl_tc_open_sto
**
** Description      This function is called when the transport channel is
**                  opened while in the opening state.  It calls the
**                  application callback with an open indication or open
**                  confirm depending on who initiated the open procedure.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_tc_open_sto(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    tAVDT_CTRL          avdt_ctrl;
    /* open reporting channel here, when it is implemented */

    /* call app callback */
    if (p_data->open.hdr.err_code == AVDT_CHAN_REPORT) {
        avdt_ctrl.hdr.err_code = 0;
        avdt_ctrl.hdr.err_param = 1;
        (*p_scb->cs.p_ctrl_cback)(avdt_scb_to_hdl(p_scb),
                                  p_scb->p_ccb ? p_scb->p_ccb->peer_addr : NULL,
                                  AVDT_REPORT_CONN_EVT, &avdt_ctrl);
    }
}
#endif

/*******************************************************************************
**
** Function         avdt_scb_hdl_write_req_no_frag
**
** Description      This function frees the media packet currently stored in
**                  the SCB, if any.  Then it builds a new media packet from
**                  with the passed in buffer and stores it in the SCB.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_write_req_no_frag(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    UINT8   *p;
    UINT32  ssrc;

    /* free packet we're holding, if any; to be replaced with new */
    if (p_scb->p_pkt != NULL) {
        GKI_freebuf(p_scb->p_pkt);

        /* this shouldn't be happening */
        AVDT_TRACE_WARNING("Dropped media packet; congested");
    }

    /* build a media packet */
    /* Add RTP header if required */
    if ( !(p_data->apiwrite.opt & AVDT_DATA_OPT_NO_RTP) ) {
        ssrc = avdt_scb_gen_ssrc(p_scb);

        p_data->apiwrite.p_buf->len += AVDT_MEDIA_HDR_SIZE;
        p_data->apiwrite.p_buf->offset -= AVDT_MEDIA_HDR_SIZE;
        p_scb->media_seq++;
        p = (UINT8 *)(p_data->apiwrite.p_buf + 1) + p_data->apiwrite.p_buf->offset;

        UINT8_TO_BE_STREAM(p, AVDT_MEDIA_OCTET1);
        UINT8_TO_BE_STREAM(p, p_data->apiwrite.m_pt);
        UINT16_TO_BE_STREAM(p, p_scb->media_seq);
        UINT32_TO_BE_STREAM(p, p_data->apiwrite.time_stamp);
        UINT32_TO_BE_STREAM(p, ssrc);
    }

    /* store it */
    p_scb->p_pkt = p_data->apiwrite.p_buf;
}

#if AVDT_MULTIPLEXING == TRUE
/*******************************************************************************
**
** Function         avdt_scb_hdl_write_req_frag
**
** Description      This function builds a new fragments of media packet from
**                  the passed in buffers and stores them in the SCB.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_write_req_frag(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    UINT8   *p;
    UINT32  ssrc;
    BT_HDR  *p_frag;

    /* free fragments we're holding, if any; it shouldn't happen */
    if (!GKI_queue_is_empty(&p_scb->frag_q)) {
        while ((p_frag = (BT_HDR *)GKI_dequeue (&p_scb->frag_q)) != NULL) {
            GKI_freebuf(p_frag);
        }

        /* this shouldn't be happening */
        AVDT_TRACE_WARNING("*** Dropped media packet; congested");
    }

    /* build a media fragments */
    p_scb->frag_off = p_data->apiwrite.data_len;
    p_scb->p_next_frag = p_data->apiwrite.p_data;

    ssrc = avdt_scb_gen_ssrc(p_scb);

    /* get first packet */
    p_frag = (BT_HDR *)GKI_getfirst (&p_data->apiwrite.frag_q);
    /* posit on Adaptation Layer header */
    p_frag->len += AVDT_AL_HDR_SIZE + AVDT_MEDIA_HDR_SIZE;
    p_frag->offset -= AVDT_AL_HDR_SIZE + AVDT_MEDIA_HDR_SIZE;
    p = (UINT8 *)(p_frag + 1) + p_frag->offset;

    /* Adaptation Layer header */
    /* TSID, no-fragment bit and coding of length(in 2 length octets following) */
    *p++ = (p_scb->curr_cfg.mux_tsid_media << 3) | AVDT_ALH_LCODE_16BIT;

    /* length of all remaining transport packet */
    UINT16_TO_BE_STREAM(p, p_frag->layer_specific + AVDT_MEDIA_HDR_SIZE );
    /* media header */
    UINT8_TO_BE_STREAM(p, AVDT_MEDIA_OCTET1);
    UINT8_TO_BE_STREAM(p, p_data->apiwrite.m_pt);
    UINT16_TO_BE_STREAM(p, p_scb->media_seq);
    UINT32_TO_BE_STREAM(p, p_data->apiwrite.time_stamp);
    UINT32_TO_BE_STREAM(p, ssrc);
    p_scb->media_seq++;

    while ((p_frag = (BT_HDR *)GKI_getnext (p_frag)) != NULL) {
        /* posit on Adaptation Layer header */
        p_frag->len += AVDT_AL_HDR_SIZE;
        p_frag->offset -= AVDT_AL_HDR_SIZE;
        p = (UINT8 *)(p_frag + 1) + p_frag->offset;
        /* Adaptation Layer header */
        /* TSID, fragment bit and coding of length(in 2 length octets following) */
        *p++ = (p_scb->curr_cfg.mux_tsid_media << 3) | (AVDT_ALH_FRAG_MASK | AVDT_ALH_LCODE_16BIT);

        /* length of all remaining transport packet */
        UINT16_TO_BE_STREAM(p, p_frag->layer_specific );
    }

    /* store it */
    p_scb->frag_q = p_data->apiwrite.frag_q;
}
#endif


/*******************************************************************************
**
** Function         avdt_scb_hdl_write_req
**
** Description      This function calls one of the two versions of building functions
**                  for case with and without fragmentation
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_hdl_write_req(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
#if AVDT_MULTIPLEXING == TRUE
    if (GKI_queue_is_empty(&p_data->apiwrite.frag_q))
#endif
        avdt_scb_hdl_write_req_no_frag(p_scb, p_data);
#if AVDT_MULTIPLEXING == TRUE
    else {
        avdt_scb_hdl_write_req_frag(p_scb, p_data);
    }
#endif
}

/*******************************************************************************
**
** Function         avdt_scb_snd_abort_req
**
** Description      This function sends an abort command message.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_snd_abort_req(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    tAVDT_EVT_HDR   hdr;
    UNUSED(p_data);

    if (p_scb->p_ccb != NULL) {
        p_scb->role = AVDT_CLOSE_INT;

        hdr.seid = p_scb->peer_seid;

        avdt_msg_send_cmd(p_scb->p_ccb, p_scb, AVDT_SIG_ABORT, (tAVDT_MSG *) &hdr);
    }
}

/*******************************************************************************
**
** Function         avdt_scb_snd_abort_rsp
**
** Description      This function sends an abort response message.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_snd_abort_rsp(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    UNUSED(p_scb);

    avdt_msg_send_rsp(avdt_ccb_by_idx(p_data->msg.hdr.ccb_idx), AVDT_SIG_ABORT,
                      &p_data->msg);
}

/*******************************************************************************
**
** Function         avdt_scb_snd_close_req
**
** Description      This function sends a close command message.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_snd_close_req(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    tAVDT_EVT_HDR   hdr;
    UNUSED(p_data);

    p_scb->role = AVDT_CLOSE_INT;

    hdr.seid = p_scb->peer_seid;

    avdt_msg_send_cmd(p_scb->p_ccb, p_scb, AVDT_SIG_CLOSE, (tAVDT_MSG *) &hdr);
}

/*******************************************************************************
**
** Function         avdt_scb_snd_stream_close
**
** Description      This function sends a close command message.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_snd_stream_close(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
#if AVDT_MULTIPLEXING == TRUE
    BT_HDR          *p_frag;

    AVDT_TRACE_WARNING("avdt_scb_snd_stream_close c:%d, off:%d\n",
                       GKI_queue_length(&p_scb->frag_q), p_scb->frag_off);
    /* clean fragments queue */
    while ((p_frag = (BT_HDR *)GKI_dequeue (&p_scb->frag_q)) != NULL) {
        GKI_freebuf(p_frag);
    }
    p_scb->frag_off = 0;
#endif
    if (p_scb->p_pkt) {
        GKI_freebuf(p_scb->p_pkt);
        p_scb->p_pkt = NULL;
    }

#if 0
    if (p_scb->cong) {
        p_scb->cong = FALSE;
    }

    /* p_scb->curr_cfg.mux_tsid_media == 0 */
#endif
    avdt_scb_snd_close_req(p_scb, p_data);
}

/*******************************************************************************
**
** Function         avdt_scb_snd_close_rsp
**
** Description      This function sends a close response message.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_snd_close_rsp(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    avdt_msg_send_rsp(p_scb->p_ccb, AVDT_SIG_CLOSE, &p_data->msg);
}

/*******************************************************************************
**
** Function         avdt_scb_snd_getconfig_req
**
** Description      This function sends a get configuration command message.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_snd_getconfig_req(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    tAVDT_EVT_HDR   hdr;
    UNUSED(p_data);

    hdr.seid = p_scb->peer_seid;

    avdt_msg_send_cmd(p_scb->p_ccb, p_scb, AVDT_SIG_GETCONFIG, (tAVDT_MSG *) &hdr);
}

/*******************************************************************************
**
** Function         avdt_scb_snd_getconfig_rsp
**
** Description      This function sends a get configuration response message.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_snd_getconfig_rsp(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    avdt_msg_send_rsp(p_scb->p_ccb, AVDT_SIG_GETCONFIG, &p_data->msg);
}

/*******************************************************************************
**
** Function         avdt_scb_snd_open_req
**
** Description      This function sends an open command message.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_snd_open_req(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    tAVDT_EVT_HDR   hdr;
    UNUSED(p_data);

    hdr.seid = p_scb->peer_seid;

    avdt_msg_send_cmd(p_scb->p_ccb, p_scb, AVDT_SIG_OPEN, (tAVDT_MSG *) &hdr);
}

/*******************************************************************************
**
** Function         avdt_scb_snd_open_rsp
**
** Description      This function sends an open response message.  It also
**                  calls avdt_ad_open_req() to accept a transport channel
**                  connection.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_snd_open_rsp(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    /* notify adaption that we're waiting for transport channel open */
    p_scb->role = AVDT_OPEN_ACP;
    avdt_ad_open_req(AVDT_CHAN_MEDIA, p_scb->p_ccb, p_scb, AVDT_ACP);

    /* send response */
    avdt_msg_send_rsp(p_scb->p_ccb, AVDT_SIG_OPEN, &p_data->msg);

    /* start tc connect timer */
    btu_start_timer(&p_scb->timer_entry, BTU_TTYPE_AVDT_SCB_TC, AVDT_SCB_TC_CONN_TOUT);
}

/*******************************************************************************
**
** Function         avdt_scb_snd_reconfig_req
**
** Description      This function stores the configuration parameters in the
**                  SCB and sends a reconfiguration command message.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_snd_reconfig_req(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    memcpy(&p_scb->req_cfg, p_data->msg.config_cmd.p_cfg, sizeof(tAVDT_CFG));
    p_data->msg.hdr.seid = p_scb->peer_seid;
    avdt_msg_send_cmd(p_scb->p_ccb, p_scb, AVDT_SIG_RECONFIG, &p_data->msg);
}

/*******************************************************************************
**
** Function         avdt_scb_snd_reconfig_rsp
**
** Description      This function stores the configuration parameters in the
**                  SCB and sends a reconfiguration response message.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_snd_reconfig_rsp(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    if (p_data->msg.hdr.err_code == 0) {
        /* store new configuration */
        if (p_scb->req_cfg.num_codec > 0) {
            p_scb->curr_cfg.num_codec = p_scb->req_cfg.num_codec;
            memcpy(p_scb->curr_cfg.codec_info, p_scb->req_cfg.codec_info, AVDT_CODEC_SIZE);
        }
        if (p_scb->req_cfg.num_protect > 0) {
            p_scb->curr_cfg.num_protect = p_scb->req_cfg.num_protect;
            memcpy(p_scb->curr_cfg.protect_info, p_scb->req_cfg.protect_info, AVDT_PROTECT_SIZE);
        }

        /* send response */
        avdt_msg_send_rsp(p_scb->p_ccb, AVDT_SIG_RECONFIG, &p_data->msg);
    } else {
        /* send reject */
        avdt_msg_send_rej(p_scb->p_ccb, AVDT_SIG_RECONFIG, &p_data->msg);
    }
}

/*******************************************************************************
**
** Function         avdt_scb_snd_security_req
**
** Description      This function sends a security command message.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_snd_security_req(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    p_data->msg.hdr.seid = p_scb->peer_seid;
    avdt_msg_send_cmd(p_scb->p_ccb, p_scb, AVDT_SIG_SECURITY, &p_data->msg);
}

/*******************************************************************************
**
** Function         avdt_scb_snd_security_rsp
**
** Description      This function sends a security response message.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_snd_security_rsp(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    if (p_data->msg.hdr.err_code == 0) {
        avdt_msg_send_rsp(p_scb->p_ccb, AVDT_SIG_SECURITY, &p_data->msg);
    } else {
        avdt_msg_send_rej(p_scb->p_ccb, AVDT_SIG_SECURITY, &p_data->msg);
    }
}

/*******************************************************************************
**
** Function         avdt_scb_snd_setconfig_rej
**
** Description      This function marks the SCB as not in use and sends a
**                  set configuration reject message.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_snd_setconfig_rej(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    if (p_scb->p_ccb != NULL) {
        avdt_msg_send_rej(p_scb->p_ccb, AVDT_SIG_SETCONFIG, &p_data->msg);

        /* clear scb variables */
        avdt_scb_clr_vars(p_scb, p_data);
    }
}

/*******************************************************************************
**
** Function         avdt_scb_snd_setconfig_req
**
** Description      This function marks the SCB as in use and copies the
**                  configuration parameters to the SCB.  Then the function
**                  sends a set configuration command message and initiates
**                  opening of the signaling channel.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_snd_setconfig_req(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    tAVDT_CFG *p_req, *p_cfg;

    /* copy API parameters to scb, set scb as in use */
    p_scb->in_use = TRUE;
    p_scb->p_ccb = avdt_ccb_by_idx(p_data->msg.config_cmd.hdr.ccb_idx);
    p_scb->peer_seid = p_data->msg.config_cmd.hdr.seid;
    p_req = p_data->msg.config_cmd.p_cfg;
    p_cfg = &p_scb->cs.cfg;
#if AVDT_MULTIPLEXING == TRUE
    p_req->mux_tsid_media = p_cfg->mux_tsid_media;
    p_req->mux_tcid_media = p_cfg->mux_tcid_media;
    if (p_req->psc_mask & AVDT_PSC_REPORT) {
        p_req->mux_tsid_report = p_cfg->mux_tsid_report;
        p_req->mux_tcid_report = p_cfg->mux_tcid_report;
    }
#endif
    memcpy(&p_scb->req_cfg, p_data->msg.config_cmd.p_cfg, sizeof(tAVDT_CFG));

    avdt_msg_send_cmd(p_scb->p_ccb, p_scb, AVDT_SIG_SETCONFIG, &p_data->msg);

    /* tell ccb to open channel */
    avdt_ccb_event(p_scb->p_ccb, AVDT_CCB_UL_OPEN_EVT, NULL);
}

/*******************************************************************************
**
** Function         avdt_scb_snd_setconfig_rsp
**
** Description      This function copies the requested configuration into the
**                  current configuration and sends a set configuration
**                  response message.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_snd_setconfig_rsp(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    if (p_scb->p_ccb != NULL) {
        memcpy(&p_scb->curr_cfg, &p_scb->req_cfg, sizeof(tAVDT_CFG));

        avdt_msg_send_rsp(p_scb->p_ccb, AVDT_SIG_SETCONFIG, &p_data->msg);
    }
}

/*******************************************************************************
**
** Function         avdt_scb_snd_tc_close
**
** Description      This function calls avdt_ad_close_req() to close the
**                  transport channel for this SCB.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_snd_tc_close(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    UNUSED(p_data);

#if AVDT_REPORTING == TRUE
    if (p_scb->curr_cfg.psc_mask & AVDT_PSC_REPORT) {
        avdt_ad_close_req(AVDT_CHAN_REPORT, p_scb->p_ccb, p_scb);
    }
#endif
    avdt_ad_close_req(AVDT_CHAN_MEDIA, p_scb->p_ccb, p_scb);
}

/*******************************************************************************
**
** Function         avdt_scb_cb_err
**
** Description      This function calls the application callback function
**                  indicating an error.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_cb_err(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    tAVDT_CTRL          avdt_ctrl;
    UNUSED(p_data);

    /* set error code and parameter */
    avdt_ctrl.hdr.err_code = AVDT_ERR_BAD_STATE;
    avdt_ctrl.hdr.err_param = 0;

    /* call callback, using lookup table to get callback event */
    (*p_scb->cs.p_ctrl_cback)(avdt_scb_to_hdl(p_scb),
                              NULL,
                              avdt_scb_cback_evt[p_scb->curr_evt],
                              &avdt_ctrl);
}

/*******************************************************************************
**
** Function         avdt_scb_cong_state
**
** Description      This function sets the congestion state of the SCB media
**                  transport channel.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_cong_state(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    p_scb->cong = p_data->llcong;
}

/*******************************************************************************
**
** Function         avdt_scb_rej_state
**
** Description      This function sends a reject message to the peer indicating
**                  incorrect state for the received command message.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_rej_state(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    UNUSED(p_scb);

    p_data->msg.hdr.err_code = AVDT_ERR_BAD_STATE;
    p_data->msg.hdr.err_param = 0;
    avdt_msg_send_rej(avdt_ccb_by_idx(p_data->msg.hdr.ccb_idx),
                      p_data->msg.hdr.sig_id, &p_data->msg);
}

/*******************************************************************************
**
** Function         avdt_scb_rej_in_use
**
** Description      This function sends a reject message to the peer indicating
**                  the stream is in use.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_rej_in_use(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    UNUSED(p_scb);

    p_data->msg.hdr.err_code = AVDT_ERR_IN_USE;
    p_data->msg.hdr.err_param = 0;
    avdt_msg_send_rej(avdt_ccb_by_idx(p_data->msg.hdr.ccb_idx),
                      p_data->msg.hdr.sig_id, &p_data->msg);
}

/*******************************************************************************
**
** Function         avdt_scb_rej_not_in_use
**
** Description      This function sends a reject message to the peer indicating
**                  the stream is in use.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_rej_not_in_use(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    UNUSED(p_scb);

    p_data->msg.hdr.err_code = AVDT_ERR_NOT_IN_USE;
    p_data->msg.hdr.err_param = 0;
    avdt_msg_send_rej(avdt_ccb_by_idx(p_data->msg.hdr.ccb_idx),
                      p_data->msg.hdr.sig_id, &p_data->msg);
}

/*******************************************************************************
**
** Function         avdt_scb_set_remove
**
** Description      This function marks an SCB to be removed.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_set_remove(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    UNUSED(p_data);

    p_scb->remove = TRUE;
}

/*******************************************************************************
**
** Function         avdt_scb_free_pkt
**
** Description      This function frees the media packet passed in.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_free_pkt(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    tAVDT_CTRL      avdt_ctrl;
#if AVDT_MULTIPLEXING == TRUE
    BT_HDR          *p_frag;
#endif

    /* set error code and parameter */
    avdt_ctrl.hdr.err_code = AVDT_ERR_BAD_STATE;
    avdt_ctrl.hdr.err_param = 0;

    /* p_buf can be NULL in case using of fragments queue frag_q */
    if (p_data->apiwrite.p_buf) {
        GKI_freebuf(p_data->apiwrite.p_buf);
    }

#if AVDT_MULTIPLEXING == TRUE
    /* clean fragments queue */
    while ((p_frag = (BT_HDR *)GKI_dequeue (&p_data->apiwrite.frag_q)) != NULL) {
        GKI_freebuf(p_frag);
    }
#endif

    AVDT_TRACE_WARNING("Dropped media packet");

    /* we need to call callback to keep data flow going */
    (*p_scb->cs.p_ctrl_cback)(avdt_scb_to_hdl(p_scb), NULL, AVDT_WRITE_CFM_EVT,
                              &avdt_ctrl);
}

/*******************************************************************************
**
** Function         avdt_scb_clr_pkt
**
** Description      This function frees the media packet stored in the SCB.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_clr_pkt(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    tAVDT_CTRL      avdt_ctrl;
    tAVDT_CCB       *p_ccb;
    UINT8           tcid;
    UINT16          lcid;
#if AVDT_MULTIPLEXING == TRUE
    BT_HDR          *p_frag;
#endif
    UNUSED(p_data);

    /* set error code and parameter */
    avdt_ctrl.hdr.err_code = AVDT_ERR_BAD_STATE;
    avdt_ctrl.hdr.err_param = 0;
    /* flush the media data queued at L2CAP */
    if ((p_ccb = p_scb->p_ccb) != NULL) {
        /* get tcid from type, scb */
        tcid = avdt_ad_type_to_tcid(AVDT_CHAN_MEDIA, p_scb);

        lcid = avdt_cb.ad.rt_tbl[avdt_ccb_to_idx(p_ccb)][tcid].lcid;
        L2CA_FlushChannel (lcid, L2CAP_FLUSH_CHANS_ALL);
    }

    if (p_scb->p_pkt != NULL) {
        GKI_freebuf(p_scb->p_pkt);
        p_scb->p_pkt = NULL;

        AVDT_TRACE_DEBUG("Dropped stored media packet");

        /* we need to call callback to keep data flow going */
        (*p_scb->cs.p_ctrl_cback)(avdt_scb_to_hdl(p_scb), NULL, AVDT_WRITE_CFM_EVT,
                                  &avdt_ctrl);
    }
#if AVDT_MULTIPLEXING == TRUE
    else if (!GKI_queue_is_empty (&p_scb->frag_q)) {
        AVDT_TRACE_DEBUG("Dropped fragments queue");
        /* clean fragments queue */
        while ((p_frag = (BT_HDR *)GKI_dequeue (&p_scb->frag_q)) != NULL) {
            GKI_freebuf(p_frag);
        }

        p_scb->frag_off = 0;

        /* we need to call callback to keep data flow going */
        (*p_scb->cs.p_ctrl_cback)(avdt_scb_to_hdl(p_scb), NULL, AVDT_WRITE_CFM_EVT,
                                  &avdt_ctrl);
    }
#endif
}


/*******************************************************************************
**
** Function         avdt_scb_chk_snd_pkt
**
** Description      This function checks if the SCB is congested, and if not
**                  congested it sends a stored media packet, if any.  After it
**                  sends the packet it calls the application callback function
**                  with a write confirm.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_chk_snd_pkt(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    tAVDT_CTRL      avdt_ctrl;
    BT_HDR          *p_pkt;
#if AVDT_MULTIPLEXING == TRUE
    BOOLEAN         sent = FALSE;
    UINT8   res = AVDT_AD_SUCCESS;
    tAVDT_SCB_EVT data;
#endif
    UNUSED(p_data);

    avdt_ctrl.hdr.err_code = 0;

    if (!p_scb->cong) {
        if (p_scb->p_pkt != NULL) {
            p_pkt = p_scb->p_pkt;
            p_scb->p_pkt = NULL;
            avdt_ad_write_req(AVDT_CHAN_MEDIA, p_scb->p_ccb, p_scb, p_pkt);

            (*p_scb->cs.p_ctrl_cback)(avdt_scb_to_hdl(p_scb), NULL, AVDT_WRITE_CFM_EVT, &avdt_ctrl);
        }
#if AVDT_MULTIPLEXING == TRUE
        else {
#if 0
            AVDT_TRACE_DEBUG("num_q=%d\n",
                             L2CA_FlushChannel(avdt_cb.ad.rt_tbl[avdt_ccb_to_idx(p_scb->p_ccb)][avdt_ad_type_to_tcid(AVDT_CHAN_MEDIA, p_scb)].lcid),
                             L2CAP_FLUSH_CHANS_GET);
#endif
            while ((p_pkt = (BT_HDR *)GKI_dequeue (&p_scb->frag_q)) != NULL) {
                sent = TRUE;
                AVDT_TRACE_DEBUG("Send fragment len=%d\n", p_pkt->len);
                /* fragments queue contains fragment to send */
                res = avdt_ad_write_req(AVDT_CHAN_MEDIA, p_scb->p_ccb, p_scb, p_pkt);
                if (AVDT_AD_CONGESTED == res) {
                    p_scb->cong = TRUE;
                    AVDT_TRACE_DEBUG("avdt/l2c congested!!");
                    break;/* exit loop if channel became congested */
                }
            }
            AVDT_TRACE_DEBUG("res=%d left=%d\n", res, p_scb->frag_off);

            if (p_scb->frag_off) {
                if (AVDT_AD_SUCCESS == res || GKI_queue_is_empty (&p_scb->frag_q)) {
                    /* all buffers were sent to L2CAP, compose more to queue */
                    avdt_scb_queue_frags(p_scb, &p_scb->p_next_frag, &p_scb->frag_off, &p_scb->frag_q);
                    if (!GKI_queue_is_empty (&p_scb->frag_q)) {
                        data.llcong = p_scb->cong;
                        avdt_scb_event(p_scb, AVDT_SCB_TC_CONG_EVT, &data);
                    }
                }
            }

            /* Send event AVDT_WRITE_CFM_EVT if it was last fragment */
            else if (sent && GKI_queue_is_empty (&p_scb->frag_q)) {
                (*p_scb->cs.p_ctrl_cback)(avdt_scb_to_hdl(p_scb), NULL, AVDT_WRITE_CFM_EVT, &avdt_ctrl);
            }
        }
#endif
    }
}

/*******************************************************************************
**
** Function         avdt_scb_tc_timer
**
** Description      This function is called to start a timer when the peer
**                  initiates closing of the stream.  The timer verifies that
**                  the peer disconnects the transport channel.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_tc_timer(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    UNUSED(p_data);

    btu_start_timer(&p_scb->timer_entry, BTU_TTYPE_AVDT_SCB_TC, AVDT_SCB_TC_DISC_TOUT);
}

/*******************************************************************************
**
** Function         avdt_scb_clr_vars
**
** Description      This function initializes certain SCB variables.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_clr_vars(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
    UNUSED(p_data);

    if ((p_scb->cs.tsep == AVDT_TSEP_SNK) && (!p_scb->sink_activated)) {
        p_scb->in_use = TRUE;
    } else {
        p_scb->in_use = FALSE;
    }
    p_scb->p_ccb = NULL;
    p_scb->peer_seid = 0;
}

#if AVDT_MULTIPLEXING == TRUE
/*******************************************************************************
**
** Function         avdt_scb_queue_frags
**
** Description      This function breaks media payload into fragments
**                  and put the fragments in the given queue.
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_queue_frags(tAVDT_SCB *p_scb, UINT8 **pp_data, UINT32 *p_data_len, BUFFER_Q *pq)
{
    UINT16  lcid;
    UINT16  num_frag;
    UINT16  mtu_used;
    UINT8   *p;
    BOOLEAN al_hdr = FALSE;
    UINT8   tcid;
    tAVDT_TC_TBL    *p_tbl;
    UINT16          buf_size;
    UINT16          offset = AVDT_MEDIA_OFFSET + AVDT_AL_HDR_SIZE;
    UINT16          cont_offset = offset - AVDT_MEDIA_HDR_SIZE;
    BT_HDR          *p_frag;

    tcid = avdt_ad_type_to_tcid(AVDT_CHAN_MEDIA, p_scb);
    lcid = avdt_cb.ad.rt_tbl[avdt_ccb_to_idx(p_scb->p_ccb)][tcid].lcid;

    if ( p_scb->frag_off != 0) {
        /* continuing process is usually triggered by un-congest event.
         * the number of buffers at L2CAP is very small (if not 0).
         * we do not need to L2CA_FlushChannel() */
        offset = cont_offset;
        al_hdr = TRUE;
        num_frag = AVDT_MAX_FRAG_COUNT;
    } else {
        num_frag = L2CA_FlushChannel(lcid, L2CAP_FLUSH_CHANS_GET);
        AVDT_TRACE_DEBUG("num_q=%d lcid=%d\n", num_frag, lcid);
        if (num_frag >= AVDT_MAX_FRAG_COUNT) {
            num_frag = 0;
        } else {
            num_frag = AVDT_MAX_FRAG_COUNT - num_frag;
        }
    }

    /* look up transport channel table entry to get peer mtu */
    p_tbl = avdt_ad_tc_tbl_by_type(AVDT_CHAN_MEDIA, p_scb->p_ccb, p_scb);
    buf_size = p_tbl->peer_mtu + BT_HDR_SIZE;
    AVDT_TRACE_DEBUG("peer_mtu: %d, buf_size: %d num_frag=%d\n",
                     p_tbl->peer_mtu, buf_size, num_frag);

    if (buf_size > AVDT_DATA_POOL_SIZE) {
        buf_size = AVDT_DATA_POOL_SIZE;
    }

    mtu_used = buf_size - BT_HDR_SIZE;

    while (*p_data_len && num_frag) {
        /* allocate buffer for fragment */
        if (NULL == (p_frag = (BT_HDR *)GKI_getbuf(buf_size))) {
            AVDT_TRACE_WARNING("avdt_scb_queue_frags len=%d(out of GKI buffers)\n", *p_data_len);
            break;
        }
        /* fill fragment by chunk of media payload */
        p_frag->layer_specific = *p_data_len;/* length of all remaining transport packet */
        p_frag->offset = offset;
        /* adjust packet offset for continuing packets */
        offset = cont_offset;

        p_frag->len = mtu_used - p_frag->offset;
        if (p_frag->len > *p_data_len) {
            p_frag->len = *p_data_len;
        }
        memcpy((UINT8 *)(p_frag + 1) + p_frag->offset, *pp_data, p_frag->len);
        *pp_data += p_frag->len;
        *p_data_len -= p_frag->len;
        AVDT_TRACE_DEBUG("Prepared fragment len=%d\n", p_frag->len);

        if (al_hdr) {
            /* Adaptation Layer header */
            p_frag->len += AVDT_AL_HDR_SIZE;
            p_frag->offset -= AVDT_AL_HDR_SIZE;
            p = (UINT8 *)(p_frag + 1) + p_frag->offset;
            /* TSID, fragment bit and coding of length(in 2 length octets following) */
            *p++ = (p_scb->curr_cfg.mux_tsid_media << 3) | (AVDT_ALH_FRAG_MASK | AVDT_ALH_LCODE_16BIT);

            /* length of all remaining transport packet */
            UINT16_TO_BE_STREAM(p, p_frag->layer_specific );
        }
        /* put fragment into gueue */
        GKI_enqueue(pq, p_frag);
        num_frag--;
    }
}
#endif

#endif /* #if (defined(AVDT_INCLUDED) && AVDT_INCLUDED == TRUE) */

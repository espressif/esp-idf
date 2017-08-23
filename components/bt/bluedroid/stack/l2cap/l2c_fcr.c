/******************************************************************************
 *
 *  Copyright (C) 2004-2012 Broadcom Corporation
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
 *  This file contains the L2CAP 1.2 Flow Control and retransmissions
 *  functions
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bt_trace.h"
#include "bt_types.h"
#include "hcimsgs.h"
#include "l2c_api.h"
#include "l2c_int.h"
#include "l2cdefs.h"
#include "btm_api.h"
#include "btm_int.h"
#include "btu.h"
#include "allocator.h"

#if (CLASSIC_BT_INCLUDED == TRUE)

/* Flag passed to retransmit_i_frames() when all packets should be retransmitted */
#define L2C_FCR_RETX_ALL_PKTS   0xFF

#if BT_TRACE_VERBOSE == TRUE
static char *SAR_types[] = { "Unsegmented", "Start", "End", "Continuation" };
static char *SUP_types[] = { "RR", "REJ", "RNR", "SREJ" };
#endif

/* Look-up table for the CRC calculation */
static const unsigned short crctab[256] = {
    0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
    0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
    0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
    0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
    0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
    0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
    0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
    0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
    0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
    0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
    0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
    0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
    0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
    0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
    0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
    0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
    0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
    0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
    0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
    0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
    0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
    0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
    0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
    0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
    0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
    0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
    0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
    0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
    0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
    0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
    0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
    0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040,
};


/*******************************************************************************
**  Static local functions
*/
static BOOLEAN process_reqseq (tL2C_CCB *p_ccb, UINT16 ctrl_word);
static void    process_s_frame (tL2C_CCB *p_ccb, BT_HDR *p_buf, UINT16 ctrl_word);
static void    process_i_frame (tL2C_CCB *p_ccb, BT_HDR *p_buf, UINT16 ctrl_word, BOOLEAN delay_ack);
static BOOLEAN retransmit_i_frames (tL2C_CCB *p_ccb, UINT8 tx_seq);
static void    prepare_I_frame (tL2C_CCB *p_ccb, BT_HDR *p_buf, BOOLEAN is_retransmission);
static void    process_stream_frame (tL2C_CCB *p_ccb, BT_HDR *p_buf);
static BOOLEAN do_sar_reassembly (tL2C_CCB *p_ccb, BT_HDR *p_buf, UINT16 ctrl_word);

#if (L2CAP_ERTM_STATS == TRUE)
static void l2c_fcr_collect_ack_delay (tL2C_CCB *p_ccb, UINT8 num_bufs_acked);
#endif

/*******************************************************************************
**
** Function         l2c_fcr_updcrc
**
** Description      This function computes the CRC using the look-up table.
**
** Returns          CRC
**
*******************************************************************************/
static unsigned short l2c_fcr_updcrc(unsigned short icrc, unsigned char *icp, int icnt)
{
    register unsigned short crc = icrc;
    register unsigned char  *cp = icp;
    register          int   cnt = icnt;

    while (cnt--) {
        crc = ((crc >> 8) & 0xff) ^ crctab[(crc & 0xff) ^ *cp++];
    }

    return (crc);
}


/*******************************************************************************
**
** Function         l2c_fcr_tx_get_fcs
**
** Description      This function computes the CRC for a frame to be TXed.
**
** Returns          CRC
**
*******************************************************************************/
static UINT16 l2c_fcr_tx_get_fcs (BT_HDR *p_buf)
{
    UINT8   *p = ((UINT8 *) (p_buf + 1)) + p_buf->offset;

    return (l2c_fcr_updcrc (L2CAP_FCR_INIT_CRC, p, p_buf->len));
}

/*******************************************************************************
**
** Function         l2c_fcr_rx_get_fcs
**
** Description      This function computes the CRC for a received frame.
**
** Returns          CRC
**
*******************************************************************************/
static UINT16 l2c_fcr_rx_get_fcs (BT_HDR *p_buf)
{
    UINT8   *p = ((UINT8 *) (p_buf + 1)) + p_buf->offset;

    /* offset points past the L2CAP header, but the CRC check includes it */
    p -= L2CAP_PKT_OVERHEAD;

    return (l2c_fcr_updcrc (L2CAP_FCR_INIT_CRC, p, p_buf->len + L2CAP_PKT_OVERHEAD));
}

/*******************************************************************************
**
** Function         l2c_fcr_start_timer
**
** Description      This function starts the (monitor or retransmission) timer.
**
** Returns          -
**
*******************************************************************************/
void l2c_fcr_start_timer (tL2C_CCB *p_ccb)
{
    assert(p_ccb != NULL);
    UINT32  tout;

    /* The timers which are in milliseconds */
    if (p_ccb->fcrb.wait_ack) {
        tout = (UINT32)p_ccb->our_cfg.fcr.mon_tout;
    } else {
        tout = (UINT32)p_ccb->our_cfg.fcr.rtrans_tout;
    }

    /* Only start a timer that was not started */
    if (p_ccb->fcrb.mon_retrans_timer.in_use == 0) {
        btu_start_quick_timer (&p_ccb->fcrb.mon_retrans_timer, BTU_TTYPE_L2CAP_CHNL, tout * QUICK_TIMER_TICKS_PER_SEC / 1000);
    }
}

/*******************************************************************************
**
** Function         l2c_fcr_stop_timer
**
** Description      This function stops the (monitor or transmission) timer.
**
** Returns          -
**
*******************************************************************************/
void l2c_fcr_stop_timer (tL2C_CCB *p_ccb)
{
    assert(p_ccb != NULL);
    if (p_ccb->fcrb.mon_retrans_timer.in_use) {
        btu_stop_quick_timer (&p_ccb->fcrb.mon_retrans_timer);
    }
}

/*******************************************************************************
**
** Function         l2c_fcr_cleanup
**
** Description      This function cleans up the variable used for flow-control/retrans.
**
** Returns          -
**
*******************************************************************************/
void l2c_fcr_cleanup (tL2C_CCB *p_ccb)
{
    assert(p_ccb != NULL);
    tL2C_FCRB *p_fcrb = &p_ccb->fcrb;

    l2c_fcr_stop_timer (p_ccb);

    if (p_fcrb->p_rx_sdu) {
        osi_free(p_fcrb->p_rx_sdu);
        p_fcrb->p_rx_sdu = NULL;
    }


    fixed_queue_free(p_fcrb->waiting_for_ack_q, osi_free_func);
    p_fcrb->waiting_for_ack_q = NULL;

    fixed_queue_free(p_fcrb->srej_rcv_hold_q, osi_free_func);
    p_fcrb->srej_rcv_hold_q = NULL;

    fixed_queue_free(p_fcrb->retrans_q, osi_free_func);
    p_fcrb->retrans_q = NULL;
	
    btu_stop_quick_timer (&p_fcrb->ack_timer);
    btu_stop_quick_timer (&p_ccb->fcrb.mon_retrans_timer);

#if (L2CAP_ERTM_STATS == TRUE)
    if ( (p_ccb->local_cid >= L2CAP_BASE_APPL_CID) && (p_ccb->peer_cfg.fcr.mode == L2CAP_FCR_ERTM_MODE) ) {
        UINT32  dur = osi_time_get_os_boottime_ms() - p_ccb->fcrb.connect_tick_count;
        char    *p_str = (char *)osi_malloc(120);
        UINT16  i;
        UINT32  throughput_avg, ack_delay_avg, ack_q_count_avg;

        BT_TRACE(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI , TRACE_TYPE_GENERIC,
                 "---  L2CAP ERTM  Stats for CID: 0x%04x   Duration: %08ums", p_ccb->local_cid, dur);
        BT_TRACE(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI , TRACE_TYPE_GENERIC,
                 "Retransmissions:%08u Times Flow Controlled:%08u Retrans Touts:%08u Ack Touts:%08u",
                 p_ccb->fcrb.pkts_retransmitted, p_ccb->fcrb.xmit_window_closed, p_ccb->fcrb.retrans_touts, p_ccb->fcrb.xmit_ack_touts);
        BT_TRACE(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI , TRACE_TYPE_GENERIC,
                 "Times there is less than 2 packets in controller when flow controlled:%08u", p_ccb->fcrb.controller_idle);
        BT_TRACE(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI , TRACE_TYPE_GENERIC,
                 "max_held_acks:%08u, in_cfg.fcr.tx_win_sz:%08u", p_ccb->fcrb.max_held_acks, p_ccb->peer_cfg.fcr.tx_win_sz );
        if (p_str) {
            sprintf(p_str, "Sent Pkts:%08u Bytes:%10u(%06u/sec) RR:%08u REJ:%08u RNR:%08u SREJ:%08u",
                    p_ccb->fcrb.ertm_pkt_counts[0], p_ccb->fcrb.ertm_byte_counts[0],
                    (dur >= 10 ? (p_ccb->fcrb.ertm_byte_counts[0] * 100) / (dur / 10) : 0),
                    p_ccb->fcrb.s_frames_sent[0], p_ccb->fcrb.s_frames_sent[1], p_ccb->fcrb.s_frames_sent[2], p_ccb->fcrb.s_frames_sent[3]);

            BT_TRACE(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI , TRACE_TYPE_GENERIC, "%s", p_str);

            sprintf(p_str, "Rcvd Pkts:%08u Bytes:%10u(%06u/sec) RR:%08u REJ:%08u RNR:%08u SREJ:%08u",
                    p_ccb->fcrb.ertm_pkt_counts[1], p_ccb->fcrb.ertm_byte_counts[1],
                    (dur >= 10 ? (p_ccb->fcrb.ertm_byte_counts[1] * 100) / (dur / 10) : 0),
                    p_ccb->fcrb.s_frames_rcvd[0], p_ccb->fcrb.s_frames_rcvd[1], p_ccb->fcrb.s_frames_rcvd[2], p_ccb->fcrb.s_frames_rcvd[3]);

            BT_TRACE(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI , TRACE_TYPE_GENERIC, "%s", p_str);

            throughput_avg  = 0;
            ack_delay_avg   = 0;
            ack_q_count_avg = 0;

            for (i = 0; i < L2CAP_ERTM_STATS_NUM_AVG; i++ ) {
                if (i == p_ccb->fcrb.ack_delay_avg_index ) {
                    BT_TRACE(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI , TRACE_TYPE_GENERIC,
                             "[%02u] collecting data ...", i );
                    continue;
                }

                sprintf(p_str, "[%02u] throughput: %5u, ack_delay avg:%3u, min:%3u, max:%3u, ack_q_count avg:%3u, min:%3u, max:%3u",
                        i, p_ccb->fcrb.throughput[i],
                        p_ccb->fcrb.ack_delay_avg[i], p_ccb->fcrb.ack_delay_min[i], p_ccb->fcrb.ack_delay_max[i],
                        p_ccb->fcrb.ack_q_count_avg[i], p_ccb->fcrb.ack_q_count_min[i], p_ccb->fcrb.ack_q_count_max[i] );

                BT_TRACE(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI , TRACE_TYPE_GENERIC, "%s", p_str);

                throughput_avg  += p_ccb->fcrb.throughput[i];
                ack_delay_avg   += p_ccb->fcrb.ack_delay_avg[i];
                ack_q_count_avg += p_ccb->fcrb.ack_q_count_avg[i];
            }

            throughput_avg  /= (L2CAP_ERTM_STATS_NUM_AVG - 1);
            ack_delay_avg   /= (L2CAP_ERTM_STATS_NUM_AVG - 1);
            ack_q_count_avg /= (L2CAP_ERTM_STATS_NUM_AVG - 1);

            BT_TRACE(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI , TRACE_TYPE_GENERIC,
                     "throughput_avg: %8u (kbytes/sec), ack_delay_avg: %8u ms, ack_q_count_avg: %8u",
                     throughput_avg, ack_delay_avg, ack_q_count_avg );

            osi_free(p_str);
        }

        BT_TRACE(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI , TRACE_TYPE_GENERIC,
                 "---");
    }
#endif

    memset (p_fcrb, 0, sizeof (tL2C_FCRB));
}

/*******************************************************************************
**
** Function         l2c_fcr_clone_buf
**
** Description      This function allocates and copies requested part of a buffer
**                  at a new-offset.
**
** Returns          pointer to new buffer
**
*******************************************************************************/
BT_HDR *l2c_fcr_clone_buf (BT_HDR *p_buf, UINT16 new_offset, UINT16 no_of_bytes)
{
    assert(p_buf != NULL);
    /*
     * NOTE: We allocate extra L2CAP_FCS_LEN octets, in case we need to put
     * the FCS (Frame Check Sequence) at the end of the buffer.
     */
    uint16_t buf_size = no_of_bytes + sizeof(BT_HDR) + new_offset + L2CAP_FCS_LEN;
#if (L2CAP_ERTM_STATS == TRUE)
    /*
     * NOTE: If L2CAP_ERTM_STATS is enabled, we need 4 extra octets at the
     * end for a timestamp at the end of an I-frame.
     */
    buf_size += sizeof(uint32_t);
#endif
    BT_HDR *p_buf2 = (BT_HDR *)osi_malloc(buf_size);

    p_buf2->offset = new_offset;
    p_buf2->len = no_of_bytes;
    memcpy(((UINT8 *)(p_buf2 + 1)) + p_buf2->offset,
           ((UINT8 *)(p_buf + 1))  + p_buf->offset,
           no_of_bytes);

    return (p_buf2);
}

/*******************************************************************************
**
** Function         l2c_fcr_is_flow_controlled
**
** Description      This function checks if the CCB is flow controlled by peer.
**
** Returns          The control word
**
*******************************************************************************/
BOOLEAN l2c_fcr_is_flow_controlled (tL2C_CCB *p_ccb)
{
    assert(p_ccb != NULL);
    if (p_ccb->peer_cfg.fcr.mode == L2CAP_FCR_ERTM_MODE) {
        /* Check if remote side flowed us off or the transmit window is full */
        if ( (p_ccb->fcrb.remote_busy == TRUE)
         ||  (fixed_queue_length(p_ccb->fcrb.waiting_for_ack_q) >= p_ccb->peer_cfg.fcr.tx_win_sz) ) {
#if (L2CAP_ERTM_STATS == TRUE)
            if (!fixed_queue_is_empty(p_ccb->xmit_hold_q)) {
                p_ccb->fcrb.xmit_window_closed++;

                if ((p_ccb->p_lcb->sent_not_acked < 2) && (l2cb.controller_xmit_window > 0)) {
                    p_ccb->fcrb.controller_idle++;
                }
            }
#endif
            return (TRUE);
        }
    }
    return (FALSE);
}

/*******************************************************************************
**
** Function         prepare_I_frame
**
** Description      This function sets the FCR variables in an I-frame that is
**                  about to be sent to HCI for transmission. This may be the
**                  first time the I-frame is sent, or a retransmission
**
** Returns          -
**
*******************************************************************************/
static void prepare_I_frame (tL2C_CCB *p_ccb, BT_HDR *p_buf, BOOLEAN is_retransmission)
{
    assert(p_ccb != NULL);
    assert(p_buf != NULL);
    tL2C_FCRB   *p_fcrb = &p_ccb->fcrb;
    UINT8       *p;
    UINT16      fcs;
    UINT16      ctrl_word;
    BOOLEAN     set_f_bit = p_fcrb->send_f_rsp;

    p_fcrb->send_f_rsp = FALSE;

    if (is_retransmission) {
        /* Get the old control word and clear out the old req_seq and F bits */
        p = ((UINT8 *) (p_buf + 1)) + p_buf->offset + L2CAP_PKT_OVERHEAD;

        STREAM_TO_UINT16 (ctrl_word, p);

        ctrl_word  &= ~(L2CAP_FCR_REQ_SEQ_BITS + L2CAP_FCR_F_BIT);
    } else {
        ctrl_word  = p_buf->layer_specific & L2CAP_FCR_SEG_BITS;                /* SAR bits */
        ctrl_word |= (p_fcrb->next_tx_seq << L2CAP_FCR_TX_SEQ_BITS_SHIFT);      /* Tx Seq */

        p_fcrb->next_tx_seq = (p_fcrb->next_tx_seq + 1) & L2CAP_FCR_SEQ_MODULO;
    }

    /* Set the F-bit and reqseq only if using re-transmission mode */
    if (p_ccb->peer_cfg.fcr.mode == L2CAP_FCR_ERTM_MODE) {
        if (set_f_bit) {
            ctrl_word |= L2CAP_FCR_F_BIT;
        }

        ctrl_word |= (p_fcrb->next_seq_expected) << L2CAP_FCR_REQ_SEQ_BITS_SHIFT;

        p_fcrb->last_ack_sent = p_ccb->fcrb.next_seq_expected;

        if (p_ccb->fcrb.ack_timer.in_use) {
            btu_stop_quick_timer (&p_ccb->fcrb.ack_timer);
        }
    }

    /* Set the control word */
    p = ((UINT8 *) (p_buf + 1)) + p_buf->offset + L2CAP_PKT_OVERHEAD;

    UINT16_TO_STREAM (p, ctrl_word);

    /* Compute the FCS and add to the end of the buffer if not bypassed */
    if (p_ccb->bypass_fcs != L2CAP_BYPASS_FCS) {
        /* length field in l2cap header has to include FCS length */
        p = ((UINT8 *) (p_buf + 1)) + p_buf->offset;
        UINT16_TO_STREAM (p, p_buf->len + L2CAP_FCS_LEN - L2CAP_PKT_OVERHEAD);

        /* Calculate the FCS */
        fcs = l2c_fcr_tx_get_fcs(p_buf);

        /* Point to the end of the buffer and put the FCS there */
        p = ((UINT8 *) (p_buf + 1)) + p_buf->offset + p_buf->len;

        UINT16_TO_STREAM (p, fcs);

        p_buf->len += L2CAP_FCS_LEN;
    }

#if BT_TRACE_VERBOSE == TRUE
    if (is_retransmission) {
        L2CAP_TRACE_EVENT ("L2CAP eRTM ReTx I-frame  CID: 0x%04x  Len: %u  SAR: %s  TxSeq: %u  ReqSeq: %u  F: %u",
                           p_ccb->local_cid, p_buf->len,
                           SAR_types[(ctrl_word & L2CAP_FCR_SAR_BITS) >> L2CAP_FCR_SAR_BITS_SHIFT],
                           (ctrl_word & L2CAP_FCR_TX_SEQ_BITS) >> L2CAP_FCR_TX_SEQ_BITS_SHIFT,
                           (ctrl_word & L2CAP_FCR_REQ_SEQ_BITS) >> L2CAP_FCR_REQ_SEQ_BITS_SHIFT,
                           (ctrl_word & L2CAP_FCR_F_BIT) >> L2CAP_FCR_F_BIT_SHIFT);
    } else {
        L2CAP_TRACE_EVENT ("L2CAP eRTM Tx I-frame CID: 0x%04x  Len: %u  SAR: %-12s  TxSeq: %u  ReqSeq: %u  F: %u",
                           p_ccb->local_cid, p_buf->len,
                           SAR_types[(ctrl_word & L2CAP_FCR_SAR_BITS) >> L2CAP_FCR_SAR_BITS_SHIFT],
                           (ctrl_word & L2CAP_FCR_TX_SEQ_BITS) >> L2CAP_FCR_TX_SEQ_BITS_SHIFT,
                           (ctrl_word & L2CAP_FCR_REQ_SEQ_BITS) >> L2CAP_FCR_REQ_SEQ_BITS_SHIFT,
                           (ctrl_word & L2CAP_FCR_F_BIT) >> L2CAP_FCR_F_BIT_SHIFT);
    }
#endif

    /* Start the retransmission timer if not already running */
    if (p_ccb->peer_cfg.fcr.mode == L2CAP_FCR_ERTM_MODE) {
        l2c_fcr_start_timer (p_ccb);
    }
}

/*******************************************************************************
**
** Function         l2c_fcr_send_S_frame
**
** Description      This function formats and sends an S-frame for transmission.
**
** Returns          -
**
*******************************************************************************/
void l2c_fcr_send_S_frame (tL2C_CCB *p_ccb, UINT16 function_code, UINT16 pf_bit)
{
    assert(p_ccb != NULL);
    BT_HDR      *p_buf;
    UINT8       *p;
    UINT16      ctrl_word;
    UINT16      fcs;

    if ((!p_ccb->in_use) || (p_ccb->chnl_state != CST_OPEN)) {
        return;
    }

#if (L2CAP_ERTM_STATS == TRUE)
    p_ccb->fcrb.s_frames_sent[function_code]++;
#endif

    if (pf_bit == L2CAP_FCR_P_BIT) {
        p_ccb->fcrb.wait_ack = TRUE;

        l2c_fcr_stop_timer (p_ccb);         /* Restart the monitor timer */
        l2c_fcr_start_timer (p_ccb);
    }

    /* Create the control word to use */
    ctrl_word = (function_code << L2CAP_FCR_SUP_SHIFT) | L2CAP_FCR_S_FRAME_BIT;
    ctrl_word |= (p_ccb->fcrb.next_seq_expected << L2CAP_FCR_REQ_SEQ_BITS_SHIFT);
    ctrl_word |= pf_bit;

    if ((p_buf = (BT_HDR *)osi_malloc(L2CAP_CMD_BUF_SIZE)) != NULL) {
        p_buf->offset = HCI_DATA_PREAMBLE_SIZE;
        p_buf->len    = L2CAP_PKT_OVERHEAD + L2CAP_FCR_OVERHEAD;

        /* Set the pointer to the beginning of the data */
        p = (UINT8 *)(p_buf + 1) + p_buf->offset;

        /* Put in the L2CAP header */
        UINT16_TO_STREAM (p, L2CAP_FCR_OVERHEAD + L2CAP_FCS_LEN);
        UINT16_TO_STREAM (p, p_ccb->remote_cid);
        UINT16_TO_STREAM (p, ctrl_word);

        /* Compute the FCS and add to the end of the buffer if not bypassed */
        if (p_ccb->bypass_fcs != L2CAP_BYPASS_FCS) {
            fcs = l2c_fcr_tx_get_fcs (p_buf);

            UINT16_TO_STREAM (p, fcs);
            p_buf->len += L2CAP_FCS_LEN;
        } else { /* rewrite the length without FCS length */
            p -= 6;
            UINT16_TO_STREAM (p, L2CAP_FCR_OVERHEAD);
        }

        /* Now, the HCI transport header */
        p_buf->layer_specific = L2CAP_NON_FLUSHABLE_PKT;
        l2cu_set_acl_hci_header (p_buf, p_ccb);

#if BT_TRACE_VERBOSE == TRUE
        if ((((ctrl_word & L2CAP_FCR_SUP_BITS) >> L2CAP_FCR_SUP_SHIFT) == 1)
                || (((ctrl_word & L2CAP_FCR_SUP_BITS) >> L2CAP_FCR_SUP_SHIFT) == 3)) {
            L2CAP_TRACE_WARNING ("L2CAP eRTM Tx S-frame  CID: 0x%04x  ctrlword: 0x%04x  Type: %s  ReqSeq: %u  P: %u  F: %u",
                                 p_ccb->local_cid, ctrl_word,
                                 SUP_types[(ctrl_word & L2CAP_FCR_SUP_BITS) >> L2CAP_FCR_SUP_SHIFT],
                                 (ctrl_word & L2CAP_FCR_REQ_SEQ_BITS) >> L2CAP_FCR_REQ_SEQ_BITS_SHIFT,
                                 (ctrl_word & L2CAP_FCR_P_BIT) >> L2CAP_FCR_P_BIT_SHIFT,
                                 (ctrl_word & L2CAP_FCR_F_BIT) >> L2CAP_FCR_F_BIT_SHIFT);
            L2CAP_TRACE_WARNING ("                  Buf Len: %u", p_buf->len);
        } else {
            L2CAP_TRACE_EVENT ("L2CAP eRTM Tx S-frame  CID: 0x%04x  ctrlword: 0x%04x  Type: %s  ReqSeq: %u  P: %u  F: %u",
                               p_ccb->local_cid, ctrl_word,
                               SUP_types[(ctrl_word & L2CAP_FCR_SUP_BITS) >> L2CAP_FCR_SUP_SHIFT],
                               (ctrl_word & L2CAP_FCR_REQ_SEQ_BITS) >> L2CAP_FCR_REQ_SEQ_BITS_SHIFT,
                               (ctrl_word & L2CAP_FCR_P_BIT) >> L2CAP_FCR_P_BIT_SHIFT,
                               (ctrl_word & L2CAP_FCR_F_BIT) >> L2CAP_FCR_F_BIT_SHIFT);
            L2CAP_TRACE_EVENT ("                  Buf Len: %u", p_buf->len);
        }
#endif  /* BT_TRACE_VERBOSE */

        l2c_link_check_send_pkts (p_ccb->p_lcb, NULL, p_buf);

        p_ccb->fcrb.last_ack_sent = p_ccb->fcrb.next_seq_expected;

        if (p_ccb->fcrb.ack_timer.in_use) {
            btu_stop_quick_timer (&p_ccb->fcrb.ack_timer);
        }
    } else {
        L2CAP_TRACE_ERROR ("l2c_fcr_send_S_frame(No Resources) cid 0x%04x, Type: 0x%4x",
                           p_ccb->local_cid, function_code);
    }
}


/*******************************************************************************
**
** Function         l2c_fcr_proc_pdu
**
** Description      This function is the entry point for processing of a
**                  received PDU when in flow control and/or retransmission modes.
**
** Returns          -
**
*******************************************************************************/
void l2c_fcr_proc_pdu (tL2C_CCB *p_ccb, BT_HDR *p_buf)
{
    assert(p_ccb != NULL);
    assert(p_buf != NULL);
    UINT8       *p;
    UINT16      fcs;
    UINT16      min_pdu_len;
    UINT16      ctrl_word;

    /* Check the length */
    min_pdu_len = (p_ccb->bypass_fcs == L2CAP_BYPASS_FCS) ?
                  (UINT16)L2CAP_FCR_OVERHEAD : (UINT16)(L2CAP_FCS_LEN + L2CAP_FCR_OVERHEAD);

    if (p_buf->len < min_pdu_len) {
        L2CAP_TRACE_WARNING ("Rx L2CAP PDU: CID: 0x%04x  Len too short: %u", p_ccb->local_cid, p_buf->len);
        osi_free (p_buf);
        return;
    }

    if (p_ccb->peer_cfg.fcr.mode == L2CAP_FCR_STREAM_MODE) {
        process_stream_frame (p_ccb, p_buf);
        return;
    }

#if BT_TRACE_VERBOSE == TRUE
    /* Get the control word */
    p = ((UINT8 *)(p_buf + 1)) + p_buf->offset;
    STREAM_TO_UINT16 (ctrl_word, p);

    if (ctrl_word & L2CAP_FCR_S_FRAME_BIT) {
        if ((((ctrl_word & L2CAP_FCR_SUP_BITS) >> L2CAP_FCR_SUP_SHIFT) == 1)
                || (((ctrl_word & L2CAP_FCR_SUP_BITS) >> L2CAP_FCR_SUP_SHIFT) == 3)) {
            /* REJ or SREJ */
            L2CAP_TRACE_WARNING ("L2CAP eRTM Rx S-frame: cid: 0x%04x  Len: %u  Type: %s  ReqSeq: %u  P: %u  F: %u",
                                 p_ccb->local_cid, p_buf->len,
                                 SUP_types[(ctrl_word & L2CAP_FCR_SUP_BITS) >> L2CAP_FCR_SUP_SHIFT],
                                 (ctrl_word & L2CAP_FCR_REQ_SEQ_BITS) >> L2CAP_FCR_REQ_SEQ_BITS_SHIFT,
                                 (ctrl_word & L2CAP_FCR_P_BIT) >> L2CAP_FCR_P_BIT_SHIFT,
                                 (ctrl_word & L2CAP_FCR_F_BIT) >> L2CAP_FCR_F_BIT_SHIFT);
        } else {
            L2CAP_TRACE_EVENT ("L2CAP eRTM Rx S-frame: cid: 0x%04x  Len: %u  Type: %s  ReqSeq: %u  P: %u  F: %u",
                               p_ccb->local_cid, p_buf->len,
                               SUP_types[(ctrl_word & L2CAP_FCR_SUP_BITS) >> L2CAP_FCR_SUP_SHIFT],
                               (ctrl_word & L2CAP_FCR_REQ_SEQ_BITS) >> L2CAP_FCR_REQ_SEQ_BITS_SHIFT,
                               (ctrl_word & L2CAP_FCR_P_BIT) >> L2CAP_FCR_P_BIT_SHIFT,
                               (ctrl_word & L2CAP_FCR_F_BIT) >> L2CAP_FCR_F_BIT_SHIFT);
        }
    } else {
        L2CAP_TRACE_EVENT ("L2CAP eRTM Rx I-frame: cid: 0x%04x  Len: %u  SAR: %-12s  TxSeq: %u  ReqSeq: %u  F: %u",
                           p_ccb->local_cid, p_buf->len,
                           SAR_types[(ctrl_word & L2CAP_FCR_SAR_BITS) >> L2CAP_FCR_SAR_BITS_SHIFT],
                           (ctrl_word & L2CAP_FCR_TX_SEQ_BITS) >> L2CAP_FCR_TX_SEQ_BITS_SHIFT,
                           (ctrl_word & L2CAP_FCR_REQ_SEQ_BITS) >> L2CAP_FCR_REQ_SEQ_BITS_SHIFT,
                           (ctrl_word & L2CAP_FCR_F_BIT) >> L2CAP_FCR_F_BIT_SHIFT);
    }

    L2CAP_TRACE_EVENT ("      eRTM Rx Nxt_tx_seq %u, Lst_rx_ack %u, Nxt_seq_exp %u, Lst_ack_snt %u, wt_q.cnt %u, tries %u",
                       p_ccb->fcrb.next_tx_seq, p_ccb->fcrb.last_rx_ack,
                       p_ccb->fcrb.next_seq_expected,
                       p_ccb->fcrb.last_ack_sent,
                       fixed_queue_length(p_ccb->fcrb.waiting_for_ack_q),
                       p_ccb->fcrb.num_tries);

#endif /* BT_TRACE_VERBOSE */

    /* Verify FCS if using */
    if (p_ccb->bypass_fcs != L2CAP_BYPASS_FCS) {
        p = ((UINT8 *)(p_buf + 1)) + p_buf->offset + p_buf->len - L2CAP_FCS_LEN;

        /* Extract and drop the FCS from the packet */
        STREAM_TO_UINT16 (fcs, p);
        p_buf->len -= L2CAP_FCS_LEN;

        if (l2c_fcr_rx_get_fcs(p_buf) != fcs) {
            L2CAP_TRACE_WARNING ("Rx L2CAP PDU: CID: 0x%04x  BAD FCS", p_ccb->local_cid);
            osi_free(p_buf);
            return;
        }
    }

    /* Get the control word */
    p = ((UINT8 *)(p_buf + 1)) + p_buf->offset;

    STREAM_TO_UINT16 (ctrl_word, p);

    p_buf->len    -= L2CAP_FCR_OVERHEAD;
    p_buf->offset += L2CAP_FCR_OVERHEAD;

    /* If we had a poll bit outstanding, check if we got a final response */
    if (p_ccb->fcrb.wait_ack) {
        /* If final bit not set, ignore the frame unless it is a polled S-frame */
        if ( !(ctrl_word & L2CAP_FCR_F_BIT) ) {
            if ( (ctrl_word & L2CAP_FCR_P_BIT) && (ctrl_word & L2CAP_FCR_S_FRAME_BIT) ) {
                if (p_ccb->fcrb.srej_sent) {
                    l2c_fcr_send_S_frame (p_ccb, L2CAP_FCR_SUP_SREJ, L2CAP_FCR_F_BIT);
                } else if (p_ccb->fcrb.local_busy) {
                    l2c_fcr_send_S_frame (p_ccb, L2CAP_FCR_SUP_RNR, L2CAP_FCR_F_BIT);
                } else {
                    l2c_fcr_send_S_frame (p_ccb, L2CAP_FCR_SUP_RR, L2CAP_FCR_F_BIT);
                }

                /* Got a poll while in wait_ack state, so re-start our timer with 1-second */
                /* This is a small optimization... the monitor timer is 12 secs, but we saw */
                /* that if the other side sends us a poll when we are waiting for a final,  */
                /* then it speeds up recovery significantly if we poll him back soon after his poll. */
                btu_start_quick_timer (&p_ccb->fcrb.mon_retrans_timer, BTU_TTYPE_L2CAP_CHNL, QUICK_TIMER_TICKS_PER_SEC);
            }
            osi_free (p_buf);
            return;
        }

        p_ccb->fcrb.wait_ack  = FALSE;

        /* P and F are mutually exclusive */
        if (ctrl_word & L2CAP_FCR_S_FRAME_BIT) {
            ctrl_word &= ~L2CAP_FCR_P_BIT;
        }

        if (fixed_queue_is_empty(p_ccb->fcrb.waiting_for_ack_q)) {
            p_ccb->fcrb.num_tries = 0;
        }

        l2c_fcr_stop_timer (p_ccb);
    } else {
        /* Otherwise, ensure the final bit is ignored */
        ctrl_word &= ~L2CAP_FCR_F_BIT;
    }

    /* Process receive sequence number */
    if (!process_reqseq (p_ccb, ctrl_word)) {
        osi_free (p_buf);
        return;
    }

    /* Process based on whether it is an S-frame or an I-frame */
    if (ctrl_word & L2CAP_FCR_S_FRAME_BIT) {
        process_s_frame (p_ccb, p_buf, ctrl_word);
    } else {
        process_i_frame (p_ccb, p_buf, ctrl_word, FALSE);
    }

    /* Return if the channel got disconnected by a bad packet or max retransmissions */
    if ( (!p_ccb->in_use) || (p_ccb->chnl_state != CST_OPEN) ) {
        return;
    }

    /* If we have some buffers held while doing SREJ, and SREJ has cleared, process them now */
    if ( (!p_ccb->fcrb.local_busy) && (!p_ccb->fcrb.srej_sent) &&
         (!fixed_queue_is_empty(p_ccb->fcrb.srej_rcv_hold_q))) {
        fixed_queue_t *temp_q = p_ccb->fcrb.srej_rcv_hold_q;
        p_ccb->fcrb.srej_rcv_hold_q = fixed_queue_new(SIZE_MAX);

        while ((p_buf = (BT_HDR *)fixed_queue_try_dequeue(temp_q)) != NULL) {
            if (p_ccb->in_use && (p_ccb->chnl_state == CST_OPEN)) {
                /* Get the control word */
                p = ((UINT8 *)(p_buf + 1)) + p_buf->offset - L2CAP_FCR_OVERHEAD;

                STREAM_TO_UINT16 (ctrl_word, p);

                L2CAP_TRACE_DEBUG ("l2c_fcr_proc_pdu() CID: 0x%04x  Process Buffer from SREJ_Hold_Q   TxSeq: %u  Expected_Seq: %u",
                                   p_ccb->local_cid, (ctrl_word & L2CAP_FCR_TX_SEQ_BITS) >> L2CAP_FCR_TX_SEQ_BITS_SHIFT,
                                   p_ccb->fcrb.next_seq_expected);

                /* Process the SREJ held I-frame, but do not send an RR for each individual frame */
                process_i_frame (p_ccb, p_buf, ctrl_word, TRUE);
            } else {
                osi_free (p_buf);
            }

            /* If more frames were lost during SREJ, send a REJ */
            if (p_ccb->fcrb.rej_after_srej) {
                p_ccb->fcrb.rej_after_srej = FALSE;
                p_ccb->fcrb.rej_sent       = TRUE;

                l2c_fcr_send_S_frame (p_ccb, L2CAP_FCR_SUP_REJ, 0);
            }
        }
        fixed_queue_free(temp_q, NULL);

        /* Now, if needed, send one RR for the whole held queue */
        if ( (!p_ccb->fcrb.local_busy) && (!p_ccb->fcrb.rej_sent) && (!p_ccb->fcrb.srej_sent)
                &&  (p_ccb->fcrb.next_seq_expected != p_ccb->fcrb.last_ack_sent) ) {
            l2c_fcr_send_S_frame (p_ccb, L2CAP_FCR_SUP_RR, 0);
        } else {
            L2CAP_TRACE_DEBUG ("l2c_fcr_proc_pdu() not sending RR CID: 0x%04x  local_busy:%d rej_sent:%d srej_sent:%d Expected_Seq:%u Last_Ack:%u",
                               p_ccb->local_cid, p_ccb->fcrb.local_busy, p_ccb->fcrb.rej_sent, p_ccb->fcrb.srej_sent, p_ccb->fcrb.next_seq_expected,
                               p_ccb->fcrb.last_ack_sent);
        }
    }

    /* If a window has opened, check if we can send any more packets */
    if ( (!fixed_queue_is_empty(p_ccb->fcrb.retrans_q) ||
          !fixed_queue_is_empty(p_ccb->xmit_hold_q))
            && (p_ccb->fcrb.wait_ack == FALSE)
            && (l2c_fcr_is_flow_controlled (p_ccb) == FALSE) ) {
        l2c_link_check_send_pkts (p_ccb->p_lcb, NULL, NULL);
    }
}

/*******************************************************************************
**
** Function         l2c_fcr_proc_tout
**
** Description      Handle a timeout. We should be in error recovery state.
**
** Returns          -
**
*******************************************************************************/
void l2c_fcr_proc_tout (tL2C_CCB *p_ccb)
{
    assert(p_ccb != NULL);
    L2CAP_TRACE_DEBUG ("l2c_fcr_proc_tout:  CID: 0x%04x  num_tries: %u (max: %u)  wait_ack: %u  ack_q_count: %u",
                       p_ccb->local_cid, p_ccb->fcrb.num_tries,
                       p_ccb->peer_cfg.fcr.max_transmit,
                       p_ccb->fcrb.wait_ack,
                       fixed_queue_length(p_ccb->fcrb.waiting_for_ack_q));

#if (L2CAP_ERTM_STATS == TRUE)
    p_ccb->fcrb.retrans_touts++;
#endif

    if ( (p_ccb->peer_cfg.fcr.max_transmit != 0) && (++p_ccb->fcrb.num_tries > p_ccb->peer_cfg.fcr.max_transmit) ) {
        l2cu_disconnect_chnl (p_ccb);
    } else {
        if (!p_ccb->fcrb.srej_sent && !p_ccb->fcrb.rej_sent) {
            if (p_ccb->fcrb.local_busy) {
                l2c_fcr_send_S_frame (p_ccb, L2CAP_FCR_SUP_RNR, L2CAP_FCR_P_BIT);
            } else {
                l2c_fcr_send_S_frame (p_ccb, L2CAP_FCR_SUP_RR, L2CAP_FCR_P_BIT);
            }
        }
    }
}


/*******************************************************************************
**
** Function         l2c_fcr_proc_ack_tout
**
** Description      Send RR/RNR if we have not acked I frame
**
** Returns          -
**
*******************************************************************************/
void l2c_fcr_proc_ack_tout (tL2C_CCB *p_ccb)
{
    assert(p_ccb != NULL);
    L2CAP_TRACE_DEBUG ("l2c_fcr_proc_ack_tout:  CID: 0x%04x State: %u  Wack:%u  Rq:%d  Acked:%d", p_ccb->local_cid,
                       p_ccb->chnl_state, p_ccb->fcrb.wait_ack, p_ccb->fcrb.next_seq_expected, p_ccb->fcrb.last_ack_sent);

    if ( (p_ccb->chnl_state == CST_OPEN) && (!p_ccb->fcrb.wait_ack)
            && (p_ccb->fcrb.last_ack_sent != p_ccb->fcrb.next_seq_expected) ) {
#if (L2CAP_ERTM_STATS == TRUE)
        p_ccb->fcrb.xmit_ack_touts++;
#endif
        if (p_ccb->fcrb.local_busy) {
            l2c_fcr_send_S_frame (p_ccb, L2CAP_FCR_SUP_RNR, 0);
        } else {
            l2c_fcr_send_S_frame (p_ccb, L2CAP_FCR_SUP_RR, 0);
        }
    }
}


/*******************************************************************************
**
** Function         process_reqseq
**
** Description      Handle receive sequence number
**
** Returns          -
**
*******************************************************************************/
static BOOLEAN process_reqseq (tL2C_CCB *p_ccb, UINT16 ctrl_word)
{
    assert(p_ccb != NULL);
    tL2C_FCRB   *p_fcrb = &p_ccb->fcrb;
    UINT8       req_seq, num_bufs_acked, xx;
    UINT16      ls;
    UINT16      full_sdus_xmitted;

    /* Receive sequence number does not ack anything for SREJ with P-bit set to zero */
    if ( (ctrl_word & L2CAP_FCR_S_FRAME_BIT)
            &&  ((ctrl_word & L2CAP_FCR_SUP_BITS) == (L2CAP_FCR_SUP_SREJ << L2CAP_FCR_SUP_SHIFT))
            &&  ((ctrl_word & L2CAP_FCR_P_BIT) == 0) ) {
        /* If anything still waiting for ack, restart the timer if it was stopped */
        if (!fixed_queue_is_empty(p_fcrb->waiting_for_ack_q)) {
            l2c_fcr_start_timer(p_ccb);
		}

        return (TRUE);
    }

    /* Extract the receive sequence number from the control word */
    req_seq = (ctrl_word & L2CAP_FCR_REQ_SEQ_BITS) >> L2CAP_FCR_REQ_SEQ_BITS_SHIFT;

    num_bufs_acked = (req_seq - p_fcrb->last_rx_ack) & L2CAP_FCR_SEQ_MODULO;

    /* Verify the request sequence is in range before proceeding */
    if (num_bufs_acked > fixed_queue_length(p_fcrb->waiting_for_ack_q)) {
        /* The channel is closed if ReqSeq is not in range */
        L2CAP_TRACE_WARNING ("L2CAP eRTM Frame BAD Req_Seq - ctrl_word: 0x%04x  req_seq 0x%02x  last_rx_ack: 0x%02x  QCount: %u",
                             ctrl_word, req_seq, p_fcrb->last_rx_ack,
                             fixed_queue_length(p_fcrb->waiting_for_ack_q));

        l2cu_disconnect_chnl (p_ccb);
        return (FALSE);
    }

    p_fcrb->last_rx_ack = req_seq;

    /* Now we can release all acknowledged frames, and restart the retransmission timer if needed */
    if (num_bufs_acked != 0) {
        p_fcrb->num_tries = 0;
        full_sdus_xmitted = 0;

#if (L2CAP_ERTM_STATS == TRUE)
        l2c_fcr_collect_ack_delay (p_ccb, num_bufs_acked);
#endif

        for (xx = 0; xx < num_bufs_acked; xx++) {
            BT_HDR *p_tmp = (BT_HDR *)fixed_queue_try_dequeue(p_fcrb->waiting_for_ack_q);
            ls = p_tmp->layer_specific & L2CAP_FCR_SAR_BITS;

            if ( (ls == L2CAP_FCR_UNSEG_SDU) || (ls == L2CAP_FCR_END_SDU) ) {
                full_sdus_xmitted++;
            }
            osi_free(p_tmp);
        }

        /* If we are still in a wait_ack state, do not mess with the timer */
        if (!p_ccb->fcrb.wait_ack) {
            l2c_fcr_stop_timer (p_ccb);
        }

        /* Check if we need to call the "packet_sent" callback */
        if ( (p_ccb->p_rcb) && (p_ccb->p_rcb->api.pL2CA_TxComplete_Cb) && (full_sdus_xmitted) ) {
            /* Special case for eRTM, if all packets sent, send 0xFFFF */
            if (fixed_queue_is_empty(p_fcrb->waiting_for_ack_q) &&
                fixed_queue_is_empty(p_ccb->xmit_hold_q)) {
                full_sdus_xmitted = 0xFFFF;
            }

            (*p_ccb->p_rcb->api.pL2CA_TxComplete_Cb)(p_ccb->local_cid, full_sdus_xmitted);
        }
    }

    /* If anything still waiting for ack, restart the timer if it was stopped */
    if (!fixed_queue_is_empty(p_fcrb->waiting_for_ack_q)) {
        l2c_fcr_start_timer (p_ccb);
    }

    return (TRUE);
}


/*******************************************************************************
**
** Function         process_s_frame
**
** Description      Process an S frame
**
** Returns          -
**
*******************************************************************************/
static void process_s_frame (tL2C_CCB *p_ccb, BT_HDR *p_buf, UINT16 ctrl_word)
{
    assert(p_ccb != NULL);
    assert(p_buf != NULL);

    tL2C_FCRB   *p_fcrb      = &p_ccb->fcrb;
    UINT16      s_frame_type = (ctrl_word & L2CAP_FCR_SUP_BITS) >> L2CAP_FCR_SUP_SHIFT;
    BOOLEAN     remote_was_busy;
    BOOLEAN     all_ok = TRUE;

    if (p_buf->len != 0) {
        L2CAP_TRACE_WARNING ("Incorrect S-frame Length (%d)", p_buf->len);
    }

    L2CAP_TRACE_DEBUG ("process_s_frame ctrl_word 0x%04x fcrb_remote_busy:%d", ctrl_word, p_fcrb->remote_busy);

#if (L2CAP_ERTM_STATS == TRUE)
    p_ccb->fcrb.s_frames_rcvd[s_frame_type]++;
#endif

    if (ctrl_word & L2CAP_FCR_P_BIT) {
        p_fcrb->rej_sent   = FALSE;             /* After checkpoint, we can send anoher REJ */
        p_fcrb->send_f_rsp = TRUE;              /* Set a flag in case an I-frame is pending */
    }

    switch (s_frame_type) {
    case L2CAP_FCR_SUP_RR:
        remote_was_busy     = p_fcrb->remote_busy;
        p_fcrb->remote_busy = FALSE;

        if ( (ctrl_word & L2CAP_FCR_F_BIT) || (remote_was_busy) ) {
            all_ok = retransmit_i_frames (p_ccb, L2C_FCR_RETX_ALL_PKTS);
        }
        break;

    case L2CAP_FCR_SUP_REJ:
        p_fcrb->remote_busy = FALSE;
        all_ok = retransmit_i_frames (p_ccb, L2C_FCR_RETX_ALL_PKTS);
        break;

    case L2CAP_FCR_SUP_RNR:
        p_fcrb->remote_busy = TRUE;
        l2c_fcr_stop_timer (p_ccb);
        break;

    case L2CAP_FCR_SUP_SREJ:
        p_fcrb->remote_busy = FALSE;
        all_ok = retransmit_i_frames (p_ccb, (UINT8)((ctrl_word & L2CAP_FCR_REQ_SEQ_BITS) >> L2CAP_FCR_REQ_SEQ_BITS_SHIFT));
        break;
    }

    if (all_ok) {
        /* If polled, we need to respond with F-bit. Note, we may have sent a I-frame with the F-bit */
        if (p_fcrb->send_f_rsp) {
            if (p_fcrb->srej_sent) {
                l2c_fcr_send_S_frame (p_ccb, L2CAP_FCR_SUP_SREJ, L2CAP_FCR_F_BIT);
            } else if (p_fcrb->local_busy) {
                l2c_fcr_send_S_frame (p_ccb, L2CAP_FCR_SUP_RNR, L2CAP_FCR_F_BIT);
            } else {
                l2c_fcr_send_S_frame (p_ccb, L2CAP_FCR_SUP_RR, L2CAP_FCR_F_BIT);
            }

            p_fcrb->send_f_rsp = FALSE;
        }
    } else {
        L2CAP_TRACE_DEBUG ("process_s_frame hit_max_retries");
    }

    osi_free (p_buf);
}


/*******************************************************************************
**
** Function         process_i_frame
**
** Description      Process an I frame
**
** Returns          -
**
*******************************************************************************/
static void process_i_frame (tL2C_CCB *p_ccb, BT_HDR *p_buf, UINT16 ctrl_word, BOOLEAN delay_ack)
{
    assert(p_ccb != NULL);
    assert(p_buf != NULL);

    tL2C_FCRB   *p_fcrb = &p_ccb->fcrb;
    UINT8       tx_seq, num_lost, num_to_ack, next_srej;

    /* If we were doing checkpoint recovery, first retransmit all unacked I-frames */
    if (ctrl_word & L2CAP_FCR_F_BIT) {
        if (!retransmit_i_frames (p_ccb, L2C_FCR_RETX_ALL_PKTS)) {
            osi_free(p_buf);
            return;
        }
    }

#if (L2CAP_ERTM_STATS == TRUE)
    p_ccb->fcrb.ertm_pkt_counts[1]++;
    p_ccb->fcrb.ertm_byte_counts[1] += p_buf->len;
#endif

    /* Extract the sequence number */
    tx_seq = (ctrl_word & L2CAP_FCR_TX_SEQ_BITS) >> L2CAP_FCR_TX_SEQ_BITS_SHIFT;

    /* If we have flow controlled the peer, ignore any bad I-frames from him */
    if ( (tx_seq != p_fcrb->next_seq_expected) && (p_fcrb->local_busy) ) {
        L2CAP_TRACE_WARNING ("Dropping bad I-Frame since we flowed off, tx_seq:%u", tx_seq);
        l2c_fcr_send_S_frame (p_ccb, L2CAP_FCR_SUP_RNR, 0);
        osi_free(p_buf);
        return;
    }

    /* Check if tx-sequence is the expected one */
    if (tx_seq != p_fcrb->next_seq_expected) {
        num_lost = (tx_seq - p_fcrb->next_seq_expected) & L2CAP_FCR_SEQ_MODULO;

        /* Is the frame a duplicate ? If so, just drop it */
        if (num_lost >= p_ccb->our_cfg.fcr.tx_win_sz) {
            /* Duplicate - simply drop it */
            L2CAP_TRACE_WARNING ("process_i_frame() Dropping Duplicate Frame tx_seq:%u  ExpectedTxSeq %u", tx_seq, p_fcrb->next_seq_expected);
            osi_free(p_buf);
        } else {
            L2CAP_TRACE_WARNING ("process_i_frame() CID: 0x%04x  Lost: %u  tx_seq:%u  ExpTxSeq %u  Rej: %u  SRej: %u",
                                 p_ccb->local_cid, num_lost, tx_seq, p_fcrb->next_seq_expected, p_fcrb->rej_sent, p_fcrb->srej_sent);

            if (p_fcrb->srej_sent) {
                /* If SREJ sent, save the frame for later processing as long as it is in sequence */
                next_srej = (((BT_HDR *)fixed_queue_try_peek_last(p_fcrb->srej_rcv_hold_q))->layer_specific + 1) & L2CAP_FCR_SEQ_MODULO;

                if ( (tx_seq == next_srej) && (fixed_queue_length(p_fcrb->srej_rcv_hold_q) < p_ccb->our_cfg.fcr.tx_win_sz) ) {
                    /* If user gave us a pool for held rx buffers, use that */
                    /* TODO: Could that happen? Get rid of this code. */
                    if (p_ccb->ertm_info.fcr_rx_buf_size != L2CAP_FCR_RX_BUF_SIZE) {
                        BT_HDR *p_buf2;

                        /* Adjust offset and len so that control word is copied */
                        p_buf->offset -= L2CAP_FCR_OVERHEAD;
                        p_buf->len    += L2CAP_FCR_OVERHEAD;

                        p_buf2 = l2c_fcr_clone_buf (p_buf, p_buf->offset, p_buf->len);

                        if (p_buf2) {
                            osi_free (p_buf);
                            p_buf = p_buf2;
                        }
                        p_buf->offset += L2CAP_FCR_OVERHEAD;
                        p_buf->len -= L2CAP_FCR_OVERHEAD;
                    }
                    L2CAP_TRACE_DEBUG ("process_i_frame() Lost: %u  tx_seq:%u  ExpTxSeq %u  Rej: %u  SRej1",
                                       num_lost, tx_seq, p_fcrb->next_seq_expected, p_fcrb->rej_sent);

                    p_buf->layer_specific = tx_seq;
                    fixed_queue_enqueue(p_fcrb->srej_rcv_hold_q, p_buf);
                } else {
                    L2CAP_TRACE_WARNING ("process_i_frame() CID: 0x%04x  frame dropped in Srej Sent next_srej:%u  hold_q.count:%u  win_sz:%u",
                                         p_ccb->local_cid, next_srej, fixed_queue_length(p_fcrb->srej_rcv_hold_q), p_ccb->our_cfg.fcr.tx_win_sz);

                    p_fcrb->rej_after_srej = TRUE;
                    osi_free (p_buf);
                }
            } else if (p_fcrb->rej_sent) {
                L2CAP_TRACE_WARNING ("process_i_frame() CID: 0x%04x  Lost: %u  tx_seq:%u  ExpTxSeq %u  Rej: 1  SRej: %u",
                                     p_ccb->local_cid, num_lost, tx_seq, p_fcrb->next_seq_expected, p_fcrb->srej_sent);

                /* If REJ sent, just drop the frame */
                osi_free (p_buf);
            } else {
                L2CAP_TRACE_DEBUG ("process_i_frame() CID: 0x%04x  tx_seq:%u  ExpTxSeq %u  Rej: %u",
                                   p_ccb->local_cid, tx_seq, p_fcrb->next_seq_expected, p_fcrb->rej_sent);

                /* If only one lost, we will send SREJ, otherwise we will send REJ */
                if (num_lost > 1) {
                    osi_free (p_buf);
                    p_fcrb->rej_sent = TRUE;
                    l2c_fcr_send_S_frame (p_ccb, L2CAP_FCR_SUP_REJ, 0);
                } else {
                    if (!fixed_queue_is_empty(p_fcrb->srej_rcv_hold_q)) {
                        L2CAP_TRACE_ERROR ("process_i_frame() CID: 0x%04x  sending SREJ tx_seq:%d hold_q.count:%u",
                                             p_ccb->local_cid, tx_seq, fixed_queue_length(p_fcrb->srej_rcv_hold_q));
                    }
                    p_buf->layer_specific = tx_seq;
                    fixed_queue_enqueue(p_fcrb->srej_rcv_hold_q, p_buf);
                    p_fcrb->srej_sent = TRUE;
                    l2c_fcr_send_S_frame (p_ccb, L2CAP_FCR_SUP_SREJ, 0);
                }
                btu_stop_quick_timer (&p_ccb->fcrb.ack_timer);
            }
        }
        return;
    }

    /* Seq number is the next expected. Clear possible reject exception in case it occured */
    p_fcrb->rej_sent = p_fcrb->srej_sent = FALSE;

    /* Adjust the next_seq, so that if the upper layer sends more data in the callback
       context, the received frame is acked by an I-frame. */
    p_fcrb->next_seq_expected = (tx_seq + 1) & L2CAP_FCR_SEQ_MODULO;

    /* If any SAR problem in eRTM mode, spec says disconnect. */
    if (!do_sar_reassembly (p_ccb, p_buf, ctrl_word)) {
        L2CAP_TRACE_WARNING ("process_i_frame() CID: 0x%04x  reassembly failed", p_ccb->local_cid);
        l2cu_disconnect_chnl (p_ccb);
        return;
    }

    /* RR optimization - if peer can still send us more, then start an ACK timer */
    num_to_ack = (p_fcrb->next_seq_expected - p_fcrb->last_ack_sent) & L2CAP_FCR_SEQ_MODULO;

    if ( (num_to_ack < p_ccb->fcrb.max_held_acks) && (!p_fcrb->local_busy) ) {
        delay_ack = TRUE;
    }

    /* We should neve never ack frame if we are not in OPEN state */
    if ((num_to_ack != 0) && p_ccb->in_use && (p_ccb->chnl_state == CST_OPEN)) {
        /* If no frames are awaiting transmission or are held, send an RR or RNR S-frame for ack */
        if (delay_ack) {
            /* If it is the first I frame we did not ack, start ack timer */
            if (!p_ccb->fcrb.ack_timer.in_use) {
                btu_start_quick_timer (&p_ccb->fcrb.ack_timer, BTU_TTYPE_L2CAP_FCR_ACK,
                                       (L2CAP_FCR_ACK_TOUT * QUICK_TIMER_TICKS_PER_SEC) / 1000);
            }
        } else if ((fixed_queue_is_empty(p_ccb->xmit_hold_q) ||
                  l2c_fcr_is_flow_controlled(p_ccb))
                 && fixed_queue_is_empty(p_ccb->fcrb.srej_rcv_hold_q)) {
            if (p_fcrb->local_busy) {
                l2c_fcr_send_S_frame (p_ccb, L2CAP_FCR_SUP_RNR, 0);
            } else {
                l2c_fcr_send_S_frame (p_ccb, L2CAP_FCR_SUP_RR, 0);
            }
        }
    }
}


/*******************************************************************************
**
** Function         process_stream_frame
**
** Description      This function processes frames in streaming mode
**
** Returns          -
**
*******************************************************************************/
static void process_stream_frame (tL2C_CCB *p_ccb, BT_HDR *p_buf)
{
    assert(p_ccb != NULL);
    assert(p_buf != NULL);

    UINT16      ctrl_word;
    UINT16      fcs;
    UINT8       *p;
    UINT8       tx_seq;

    /* Verify FCS if using */
    if (p_ccb->bypass_fcs != L2CAP_BYPASS_FCS) {
        p = ((UINT8 *)(p_buf + 1)) + p_buf->offset + p_buf->len - L2CAP_FCS_LEN;

        /* Extract and drop the FCS from the packet */
        STREAM_TO_UINT16 (fcs, p);
        p_buf->len -= L2CAP_FCS_LEN;

        if (l2c_fcr_rx_get_fcs(p_buf) != fcs) {
            L2CAP_TRACE_WARNING ("Rx L2CAP PDU: CID: 0x%04x  BAD FCS", p_ccb->local_cid);
            osi_free(p_buf);
            return;
        }
    }

    /* Get the control word */
    p = ((UINT8 *)(p_buf + 1)) + p_buf->offset;

    STREAM_TO_UINT16 (ctrl_word, p);

    p_buf->len    -= L2CAP_FCR_OVERHEAD;
    p_buf->offset += L2CAP_FCR_OVERHEAD;

    /* Make sure it is an I-frame */
    if (ctrl_word & L2CAP_FCR_S_FRAME_BIT) {
        L2CAP_TRACE_WARNING ("Rx L2CAP PDU: CID: 0x%04x  BAD S-frame in streaming mode  ctrl_word: 0x%04x", p_ccb->local_cid, ctrl_word);
        osi_free (p_buf);
        return;
    }

#if BT_TRACE_VERBOSE == TRUE
    L2CAP_TRACE_EVENT ("L2CAP eRTM Rx I-frame: cid: 0x%04x  Len: %u  SAR: %-12s  TxSeq: %u  ReqSeq: %u  F: %u",
                       p_ccb->local_cid, p_buf->len,
                       SAR_types[(ctrl_word & L2CAP_FCR_SAR_BITS) >> L2CAP_FCR_SAR_BITS_SHIFT],
                       (ctrl_word & L2CAP_FCR_TX_SEQ_BITS) >> L2CAP_FCR_TX_SEQ_BITS_SHIFT,
                       (ctrl_word & L2CAP_FCR_REQ_SEQ_BITS) >> L2CAP_FCR_REQ_SEQ_BITS_SHIFT,
                       (ctrl_word & L2CAP_FCR_F_BIT) >> L2CAP_FCR_F_BIT_SHIFT);
#endif

    /* Extract the sequence number */
    tx_seq = (ctrl_word & L2CAP_FCR_TX_SEQ_BITS) >> L2CAP_FCR_TX_SEQ_BITS_SHIFT;

    /* Check if tx-sequence is the expected one */
    if (tx_seq != p_ccb->fcrb.next_seq_expected) {
        L2CAP_TRACE_WARNING ("Rx L2CAP PDU: CID: 0x%04x  Lost frames Exp: %u  Got: %u  p_rx_sdu: %p",
                             p_ccb->local_cid, p_ccb->fcrb.next_seq_expected, tx_seq, p_ccb->fcrb.p_rx_sdu);

        /* Lost one or more packets, so flush the SAR queue */
        if (p_ccb->fcrb.p_rx_sdu != NULL) {
            osi_free(p_ccb->fcrb.p_rx_sdu);
            p_ccb->fcrb.p_rx_sdu = NULL;
        }
    }

    p_ccb->fcrb.next_seq_expected = (tx_seq + 1) & L2CAP_FCR_SEQ_MODULO;

    if (!do_sar_reassembly (p_ccb, p_buf, ctrl_word)) {
        /* Some sort of SAR error, so flush the SAR queue */
        if (p_ccb->fcrb.p_rx_sdu != NULL) {
            osi_free(p_ccb->fcrb.p_rx_sdu);
            p_ccb->fcrb.p_rx_sdu = NULL;
        }
    }
}


/*******************************************************************************
**
** Function         do_sar_reassembly
**
** Description      Process SAR bits and re-assemble frame
**
** Returns          TRUE if all OK, else FALSE
**
*******************************************************************************/
static BOOLEAN do_sar_reassembly (tL2C_CCB *p_ccb, BT_HDR *p_buf, UINT16 ctrl_word)
{
    assert(p_ccb != NULL);
    assert(p_buf != NULL);

    tL2C_FCRB   *p_fcrb = &p_ccb->fcrb;
    UINT16      sar_type = ctrl_word & L2CAP_FCR_SEG_BITS;
    BOOLEAN     packet_ok = TRUE;
    UINT8       *p;

    /* Check if the SAR state is correct */
    if ((sar_type == L2CAP_FCR_UNSEG_SDU) || (sar_type == L2CAP_FCR_START_SDU)) {
        if (p_fcrb->p_rx_sdu != NULL) {
            L2CAP_TRACE_WARNING ("SAR - got unexpected unsegmented or start SDU  Expected len: %u  Got so far: %u",
                                 p_fcrb->rx_sdu_len, p_fcrb->p_rx_sdu->len);

            packet_ok = FALSE;
        }
        /* Check the length of the packet */
        if ( (sar_type == L2CAP_FCR_START_SDU) && (p_buf->len < L2CAP_SDU_LEN_OVERHEAD) ) {
            L2CAP_TRACE_WARNING ("SAR start packet too short: %u", p_buf->len);
            packet_ok = FALSE;
        }
    } else {
        if (p_fcrb->p_rx_sdu == NULL) {
            L2CAP_TRACE_WARNING ("SAR - got unexpected cont or end SDU");
            packet_ok = FALSE;
        }
    }

    if ( (packet_ok) && (sar_type != L2CAP_FCR_UNSEG_SDU) ) {
        p = ((UINT8 *)(p_buf + 1)) + p_buf->offset;

        /* For start SDU packet, extract the SDU length */
        if (sar_type == L2CAP_FCR_START_SDU) {
            /* Get the SDU length */
            STREAM_TO_UINT16 (p_fcrb->rx_sdu_len, p);
            p_buf->offset += 2;
            p_buf->len    -= 2;

            if (p_fcrb->rx_sdu_len > p_ccb->max_rx_mtu) {
                L2CAP_TRACE_WARNING ("SAR - SDU len: %u  larger than MTU: %u", p_fcrb->rx_sdu_len, p_fcrb->rx_sdu_len);
                packet_ok = FALSE;
            } else if ((p_fcrb->p_rx_sdu = (BT_HDR *)osi_malloc(L2CAP_MAX_BUF_SIZE)) == NULL) {
                L2CAP_TRACE_ERROR ("SAR - no buffer for SDU start user_rx_buf_size:%d", p_ccb->ertm_info.user_rx_buf_size);
                packet_ok = FALSE;
            } else {
                p_fcrb->p_rx_sdu->offset = 4; /* this is the minimal offset required by OBX to process incoming packets */
                p_fcrb->p_rx_sdu->len    = 0;
            }
        }

        if (packet_ok) {
            if ((p_fcrb->p_rx_sdu->len + p_buf->len) > p_fcrb->rx_sdu_len) {
                L2CAP_TRACE_ERROR ("SAR - SDU len exceeded  Type: %u   Lengths: %u %u %u",
                                   sar_type, p_fcrb->p_rx_sdu->len, p_buf->len, p_fcrb->rx_sdu_len);
                packet_ok = FALSE;
            } else if ( (sar_type == L2CAP_FCR_END_SDU) && ((p_fcrb->p_rx_sdu->len + p_buf->len) != p_fcrb->rx_sdu_len) ) {
                L2CAP_TRACE_WARNING ("SAR - SDU end rcvd but SDU incomplete: %u %u %u",
                                     p_fcrb->p_rx_sdu->len, p_buf->len, p_fcrb->rx_sdu_len);
                packet_ok = FALSE;
            } else {
                memcpy (((UINT8 *) (p_fcrb->p_rx_sdu + 1)) + p_fcrb->p_rx_sdu->offset + p_fcrb->p_rx_sdu->len, p, p_buf->len);

                p_fcrb->p_rx_sdu->len += p_buf->len;

                osi_free (p_buf);
                p_buf = NULL;

                if (sar_type == L2CAP_FCR_END_SDU) {
                    p_buf            = p_fcrb->p_rx_sdu;
                    p_fcrb->p_rx_sdu = NULL;
                }
            }
        }
    }

    if (packet_ok == FALSE) {
        osi_free (p_buf);
    } else if (p_buf != NULL) {
#if (L2CAP_NUM_FIXED_CHNLS > 0)
        if (p_ccb->local_cid < L2CAP_BASE_APPL_CID &&
                (p_ccb->local_cid >= L2CAP_FIRST_FIXED_CHNL && p_ccb->local_cid <= L2CAP_LAST_FIXED_CHNL)) {
            if (l2cb.fixed_reg[p_ccb->local_cid - L2CAP_FIRST_FIXED_CHNL].pL2CA_FixedData_Cb)
                (*l2cb.fixed_reg[p_ccb->local_cid - L2CAP_FIRST_FIXED_CHNL].pL2CA_FixedData_Cb)
                (p_ccb->local_cid, p_ccb->p_lcb->remote_bd_addr, p_buf);
        } else
#endif
            l2c_csm_execute (p_ccb, L2CEVT_L2CAP_DATA, p_buf);
    }

    return (packet_ok);
}


/*******************************************************************************
**
** Function         retransmit_i_frames
**
** Description      This function retransmits i-frames awaiting acks.
**
** Returns          BOOLEAN - TRUE if retransmitted
**
*******************************************************************************/
static BOOLEAN retransmit_i_frames (tL2C_CCB *p_ccb, UINT8 tx_seq)
{
    assert(p_ccb != NULL);

    BT_HDR      *p_buf = NULL;
    UINT8       *p;
    UINT8       buf_seq;
    UINT16      ctrl_word;

    if ( (!fixed_queue_is_empty(p_ccb->fcrb.waiting_for_ack_q))
            &&  (p_ccb->peer_cfg.fcr.max_transmit != 0)
            &&  (p_ccb->fcrb.num_tries >= p_ccb->peer_cfg.fcr.max_transmit) ) {
        L2CAP_TRACE_EVENT ("Max Tries Exceeded:  (last_acq: %d  CID: 0x%04x  num_tries: %u (max: %u) ack_q_count: %u",
                           p_ccb->fcrb.last_rx_ack, p_ccb->local_cid, p_ccb->fcrb.num_tries, p_ccb->peer_cfg.fcr.max_transmit,
                fixed_queue_length(p_ccb->fcrb.waiting_for_ack_q));

        l2cu_disconnect_chnl (p_ccb);
        return (FALSE);
    }

    /* tx_seq indicates whether to retransmit a specific sequence or all (if == L2C_FCR_RETX_ALL_PKTS) */
    list_t *list_ack = NULL;
    const list_node_t *node_ack = NULL;
    if (! fixed_queue_is_empty(p_ccb->fcrb.waiting_for_ack_q)) {
        list_ack = fixed_queue_get_list(p_ccb->fcrb.waiting_for_ack_q);
        node_ack = list_begin(list_ack);
    }
    if (tx_seq != L2C_FCR_RETX_ALL_PKTS) {
        /* If sending only one, the sequence number tells us which one. Look for it.
        */
        if (list_ack != NULL) {
            for ( ; node_ack != list_end(list_ack); node_ack = list_next(node_ack)) {
                p_buf = (BT_HDR *)list_node(node_ack);
                /* Get the old control word */
                p = ((UINT8 *) (p_buf+1)) + p_buf->offset + L2CAP_PKT_OVERHEAD;

                STREAM_TO_UINT16 (ctrl_word, p);

                buf_seq = (ctrl_word & L2CAP_FCR_TX_SEQ_BITS) >> L2CAP_FCR_TX_SEQ_BITS_SHIFT;

                L2CAP_TRACE_DEBUG ("retransmit_i_frames()   cur seq: %u  looking for: %u", buf_seq, tx_seq);

                if (tx_seq == buf_seq) {
                    break;
				}
            }
        }

        if (!p_buf) {
            L2CAP_TRACE_ERROR ("retransmit_i_frames() UNKNOWN seq: %u  q_count: %u",
                               tx_seq,
                               fixed_queue_length(p_ccb->fcrb.waiting_for_ack_q));
            return (TRUE);
        }
    } else {
        // Iterate though list and flush the amount requested from
        // the transmit data queue that satisfy the layer and event conditions.
        for (const list_node_t *node = list_begin(p_ccb->p_lcb->link_xmit_data_q);
                node != list_end(p_ccb->p_lcb->link_xmit_data_q);) {
            BT_HDR *p_buf = (BT_HDR *)list_node(node);
            node = list_next(node);

            /* Do not flush other CIDs or partial segments */
            if ((p_buf->layer_specific == 0) && (p_buf->event == p_ccb->local_cid)) {
                list_remove(p_ccb->p_lcb->link_xmit_data_q, p_buf);
                osi_free(p_buf);
            }
        }

        /* Also flush our retransmission queue */
        while (!fixed_queue_is_empty(p_ccb->fcrb.retrans_q)) {
            osi_free(fixed_queue_try_dequeue(p_ccb->fcrb.retrans_q));
		}

        if (list_ack != NULL) {
            node_ack = list_begin(list_ack);
		}
    }

    if (list_ack != NULL) {
        while (node_ack != list_end(list_ack))
        {
            p_buf = (BT_HDR *)list_node(node_ack);
            node_ack = list_next(node_ack);

            BT_HDR *p_buf2 = l2c_fcr_clone_buf(p_buf, p_buf->offset, p_buf->len);
            if (p_buf2)
            {
                p_buf2->layer_specific = p_buf->layer_specific;

                fixed_queue_enqueue(p_ccb->fcrb.retrans_q, p_buf2);
            }

            if ( (tx_seq != L2C_FCR_RETX_ALL_PKTS) || (p_buf2 == NULL) ) {
                break;
			}
        }
    }

    l2c_link_check_send_pkts (p_ccb->p_lcb, NULL, NULL);

    if (fixed_queue_length(p_ccb->fcrb.waiting_for_ack_q))
    {
        p_ccb->fcrb.num_tries++;
        l2c_fcr_start_timer (p_ccb);
    }

    return (TRUE);
}


/*******************************************************************************
**
** Function         l2c_fcr_get_next_xmit_sdu_seg
**
** Description      Get the next SDU segment to transmit.
**
** Returns          pointer to buffer with segment or NULL
**
*******************************************************************************/
BT_HDR *l2c_fcr_get_next_xmit_sdu_seg (tL2C_CCB *p_ccb, UINT16 max_packet_length)
{
    assert(p_ccb != NULL);

    BOOLEAN     first_seg    = FALSE,       /* The segment is the first part of data  */
                mid_seg      = FALSE,       /* The segment is the middle part of data */
                last_seg     = FALSE;       /* The segment is the last part of data   */
    UINT16      sdu_len = 0;
    BT_HDR      *p_buf, *p_xmit;
    UINT8       *p;
    UINT16      max_pdu = p_ccb->tx_mps /* Needed? - L2CAP_MAX_HEADER_FCS*/;

    /* If there is anything in the retransmit queue, that goes first
    */
    p_buf = (BT_HDR *)fixed_queue_try_dequeue(p_ccb->fcrb.retrans_q);
    if (p_buf != NULL) {
        /* Update Rx Seq and FCS if we acked some packets while this one was queued */
        prepare_I_frame (p_ccb, p_buf, TRUE);

        p_buf->event = p_ccb->local_cid;

#if (L2CAP_ERTM_STATS == TRUE)
        p_ccb->fcrb.pkts_retransmitted++;
        p_ccb->fcrb.ertm_pkt_counts[0]++;
        p_ccb->fcrb.ertm_byte_counts[0] += (p_buf->len - 8);
#endif
        return (p_buf);
    }

    /* For BD/EDR controller, max_packet_length is set to 0             */
    /* For AMP controller, max_packet_length is set by available blocks */
    if ( (max_packet_length > L2CAP_MAX_HEADER_FCS)
            && (max_pdu + L2CAP_MAX_HEADER_FCS > max_packet_length) ) {
        max_pdu = max_packet_length - L2CAP_MAX_HEADER_FCS;
    }

    p_buf = (BT_HDR *)fixed_queue_try_peek_first(p_ccb->xmit_hold_q);

    /* If there is more data than the MPS, it requires segmentation */
    if (p_buf->len > max_pdu) {
        /* We are using the "event" field to tell is if we already started segmentation */
        if (p_buf->event == 0) {
            first_seg = TRUE;
            sdu_len   = p_buf->len;
        } else {
            mid_seg = TRUE;
        }

        /* Get a new buffer and copy the data that can be sent in a PDU */
        p_xmit = l2c_fcr_clone_buf (p_buf, L2CAP_MIN_OFFSET + L2CAP_SDU_LEN_OFFSET,
                                    max_pdu);

        if (p_xmit != NULL) {
            p_buf->event  = p_ccb->local_cid;
            p_xmit->event = p_ccb->local_cid;

            p_buf->len    -= max_pdu;
            p_buf->offset += max_pdu;

            /* copy PBF setting */
            p_xmit->layer_specific = p_buf->layer_specific;
        } else { /* Should never happen if the application has configured buffers correctly */
            L2CAP_TRACE_ERROR ("L2CAP - cannot get buffer for segmentation, max_pdu: %u", max_pdu);
            return (NULL);
        }
    } else { /* Use the original buffer if no segmentation, or the last segment */
        p_xmit = (BT_HDR *)fixed_queue_try_dequeue(p_ccb->xmit_hold_q);

        if (p_xmit->event != 0) {
            last_seg = TRUE;
        }

        p_xmit->event = p_ccb->local_cid;
    }

    /* Step back to add the L2CAP headers */
    p_xmit->offset -= (L2CAP_PKT_OVERHEAD + L2CAP_FCR_OVERHEAD);
    p_xmit->len    += L2CAP_PKT_OVERHEAD + L2CAP_FCR_OVERHEAD;

    if (first_seg) {
        p_xmit->offset -= L2CAP_SDU_LEN_OVERHEAD;
        p_xmit->len    += L2CAP_SDU_LEN_OVERHEAD;
    }

    /* Set the pointer to the beginning of the data */
    p = (UINT8 *)(p_xmit + 1) + p_xmit->offset;

    /* Now the L2CAP header */

    /* Note: if FCS has to be included then the length is recalculated later */
    UINT16_TO_STREAM (p, p_xmit->len - L2CAP_PKT_OVERHEAD);

    UINT16_TO_STREAM (p, p_ccb->remote_cid);

    if (first_seg) {
        /* Skip control word and add SDU length */
        p += 2;
        UINT16_TO_STREAM (p, sdu_len);

        /* We will store the SAR type in layer-specific */
        /* layer_specific is shared with flushable flag(bits 0-1), don't clear it */
        p_xmit->layer_specific |= L2CAP_FCR_START_SDU;

        first_seg = FALSE;
    } else if (mid_seg) {
        p_xmit->layer_specific |= L2CAP_FCR_CONT_SDU;
    } else if (last_seg) {
        p_xmit->layer_specific |= L2CAP_FCR_END_SDU;
    } else {
        p_xmit->layer_specific |= L2CAP_FCR_UNSEG_SDU;
    }

    prepare_I_frame (p_ccb, p_xmit, FALSE);

    if (p_ccb->peer_cfg.fcr.mode == L2CAP_FCR_ERTM_MODE) {
        BT_HDR *p_wack = l2c_fcr_clone_buf (p_xmit, HCI_DATA_PREAMBLE_SIZE, p_xmit->len);

        if (!p_wack) {
            L2CAP_TRACE_ERROR("L2CAP - no buffer for xmit cloning, CID: 0x%04x  Length: %u",
                              p_ccb->local_cid, p_xmit->len);

            /* We will not save the FCS in case we reconfigure and change options */
            if (p_ccb->bypass_fcs != L2CAP_BYPASS_FCS) {
                p_xmit->len -= L2CAP_FCS_LEN;
            }

            /* Pretend we sent it and it got lost */
            fixed_queue_enqueue(p_ccb->fcrb.waiting_for_ack_q, p_xmit);
            return (NULL);
        } else {
#if (L2CAP_ERTM_STATS == TRUE)
            /* set timestamp at the end of tx I-frame to get acking delay */
            p = ((UINT8 *) (p_wack + 1)) + p_wack->offset + p_wack->len;
            UINT32_TO_STREAM (p, osi_time_get_os_boottime_ms());
#endif
            /* We will not save the FCS in case we reconfigure and change options */
            if (p_ccb->bypass_fcs != L2CAP_BYPASS_FCS) {
                p_wack->len -= L2CAP_FCS_LEN;
            }

            p_wack->layer_specific = p_xmit->layer_specific;
            fixed_queue_enqueue(p_ccb->fcrb.waiting_for_ack_q, p_wack);
        }

#if (L2CAP_ERTM_STATS == TRUE)
        p_ccb->fcrb.ertm_pkt_counts[0]++;
        p_ccb->fcrb.ertm_byte_counts[0] += (p_xmit->len - 8);
#endif

    }

    return (p_xmit);
}


/*******************************************************************************
** Configuration negotiation functions
**
** The following functions are used in negotiating channel modes during
** configuration
********************************************************************************/

/*******************************************************************************
**
** Function         l2c_fcr_chk_chan_modes
**
** Description      Validates and adjusts if necessary, the FCR options
**                  based on remote EXT features.
**
**                  Note: This assumes peer EXT Features have been received.
**                      Basic mode is used if FCR Options have not been received
**
** Returns          UINT8 - nonzero if can continue, '0' if no compatible channels
**
*******************************************************************************/
UINT8 l2c_fcr_chk_chan_modes (tL2C_CCB *p_ccb)
{
    assert(p_ccb != NULL);

    /* Remove nonbasic options that the peer does not support */
    if (!(p_ccb->p_lcb->peer_ext_fea & L2CAP_EXTFEA_ENH_RETRANS)) {
        p_ccb->ertm_info.allowed_modes &= ~L2CAP_FCR_CHAN_OPT_ERTM;
    }

    if (!(p_ccb->p_lcb->peer_ext_fea & L2CAP_EXTFEA_STREAM_MODE)) {
        p_ccb->ertm_info.allowed_modes &= ~L2CAP_FCR_CHAN_OPT_STREAM;
    }

    /* At least one type needs to be set (Basic, ERTM, STM) to continue */
    if (!p_ccb->ertm_info.allowed_modes) {
        L2CAP_TRACE_WARNING ("L2CAP - Peer does not support our desired channel types");
    }

    return (p_ccb->ertm_info.allowed_modes);
}

/*******************************************************************************
**
** Function         l2c_fcr_adj_our_req_options
**
** Description      Validates and sets up the FCR options passed in from
**                  L2CA_ConfigReq based on remote device's features.
**
** Returns          TRUE if no errors, Otherwise FALSE
**
*******************************************************************************/
BOOLEAN l2c_fcr_adj_our_req_options (tL2C_CCB *p_ccb, tL2CAP_CFG_INFO *p_cfg)
{
    assert(p_ccb != NULL);
    assert(p_cfg != NULL);

    tL2CAP_FCR_OPTS *p_fcr = &p_cfg->fcr;

    if (p_fcr->mode != p_ccb->ertm_info.preferred_mode) {
        L2CAP_TRACE_WARNING ("l2c_fcr_adj_our_req_options - preferred_mode (%d), does not match mode (%d)",
                             p_ccb->ertm_info.preferred_mode, p_fcr->mode);

        /* The preferred mode is passed in through tL2CAP_ERTM_INFO, so override this one */
        p_fcr->mode = p_ccb->ertm_info.preferred_mode;
    }

    /* If upper layer did not request eRTM mode, BASIC must be used */
    if (p_ccb->ertm_info.allowed_modes == L2CAP_FCR_CHAN_OPT_BASIC) {
        if (p_cfg->fcr_present && p_fcr->mode != L2CAP_FCR_BASIC_MODE) {
            L2CAP_TRACE_WARNING ("l2c_fcr_adj_our_req_options (mode %d): ERROR: No FCR options set using BASIC mode", p_fcr->mode);
        }
        p_fcr->mode = L2CAP_FCR_BASIC_MODE;
    }

    /* Process the FCR options if initial channel bring-up (not a reconfig request)
    ** Determine initial channel mode to try based on our options and remote's features
    */
    if (p_cfg->fcr_present && !(p_ccb->config_done & RECONFIG_FLAG)) {
        /* We need to have at least one mode type common with the peer */
        if (!l2c_fcr_chk_chan_modes(p_ccb)) {
            /* Two channels have incompatible supported types */
            l2cu_disconnect_chnl (p_ccb);
            return (FALSE);
        }

        /* Basic is the only common channel mode between the two devices */
        else if (p_ccb->ertm_info.allowed_modes == L2CAP_FCR_CHAN_OPT_BASIC) {
            /* We only want to try Basic, so bypass sending the FCR options entirely */
            p_cfg->fcr_present = FALSE;
            p_cfg->fcs_present = FALSE;             /* Illegal to use FCS option in basic mode */
            p_cfg->ext_flow_spec_present = FALSE;   /* Illegal to use extended flow spec in basic mode */
        }

        /* We have at least one non-basic mode available
         * Override mode from available mode options based on preference, if needed
         */
        else {
            /* If peer does not support STREAMING, try ERTM */
            if (p_fcr->mode == L2CAP_FCR_STREAM_MODE && !(p_ccb->ertm_info.allowed_modes & L2CAP_FCR_CHAN_OPT_STREAM)) {
                L2CAP_TRACE_DEBUG ("L2C CFG: mode is STREAM, but peer does not support; Try ERTM");
                p_fcr->mode = L2CAP_FCR_ERTM_MODE;
            }

            /* If peer does not support ERTM, try BASIC (will support this if made it here in the code) */
            if (p_fcr->mode == L2CAP_FCR_ERTM_MODE && !(p_ccb->ertm_info.allowed_modes & L2CAP_FCR_CHAN_OPT_ERTM)) {
                L2CAP_TRACE_DEBUG ("L2C CFG: mode is ERTM, but peer does not support; Try BASIC");
                p_fcr->mode = L2CAP_FCR_BASIC_MODE;
            }
        }

        if (p_fcr->mode != L2CAP_FCR_BASIC_MODE) {
            /* MTU must be smaller than buffer size */
            if ( (p_cfg->mtu_present) && (p_cfg->mtu > p_ccb->max_rx_mtu) ) {
                L2CAP_TRACE_WARNING ("L2CAP - MTU: %u  larger than buf size: %u", p_cfg->mtu, p_ccb->max_rx_mtu);
                return (FALSE);
            }

            /* application want to use the default MPS */
            if (p_fcr->mps == L2CAP_DEFAULT_ERM_MPS) {
                p_fcr->mps = L2CAP_MPS_OVER_BR_EDR;
            }
            /* MPS must be less than MTU */
            else if (p_fcr->mps > p_ccb->max_rx_mtu) {
                L2CAP_TRACE_WARNING ("L2CAP - MPS  %u  invalid  MTU: %u", p_fcr->mps, p_ccb->max_rx_mtu);
                return (FALSE);
            }

            /* We always initially read into the HCI buffer pool, so make sure it fits */
            if (p_fcr->mps > (L2CAP_MTU_SIZE - L2CAP_MAX_HEADER_FCS)) {
                p_fcr->mps = L2CAP_MTU_SIZE - L2CAP_MAX_HEADER_FCS;
            }
        } else {
            p_cfg->fcs_present = FALSE;             /* Illegal to use FCS option in basic mode */
            p_cfg->ext_flow_spec_present = FALSE;   /* Illegal to use extended flow spec in basic mode */
        }

        p_ccb->our_cfg.fcr = *p_fcr;
    } else { /* Not sure how to send a reconfiguration(??) should fcr be included? */
        p_ccb->our_cfg.fcr_present = FALSE;
    }

    return (TRUE);
}


/*******************************************************************************
**
** Function         l2c_fcr_adj_monitor_retran_timeout
**
** Description      Overrides monitor/retrans timer value based on controller
**
** Returns          None
**
*******************************************************************************/
void l2c_fcr_adj_monitor_retran_timeout (tL2C_CCB *p_ccb)
{
    assert(p_ccb != NULL);

    /* adjust our monitor/retran timeout */
    if (p_ccb->out_cfg_fcr_present) {
        /*
        ** if we requestd ERTM or accepted ERTM
        ** We may accept ERTM even if we didn't request ERTM, in case of requesting STREAM
        */
        if ((p_ccb->our_cfg.fcr.mode == L2CAP_FCR_ERTM_MODE)
                || (p_ccb->peer_cfg.fcr.mode == L2CAP_FCR_ERTM_MODE)) {
            /* upper layer setting is ignored */
            p_ccb->our_cfg.fcr.mon_tout    = L2CAP_MIN_MONITOR_TOUT;
            p_ccb->our_cfg.fcr.rtrans_tout = L2CAP_MIN_RETRANS_TOUT;
        } else {
            p_ccb->our_cfg.fcr.mon_tout    = 0;
            p_ccb->our_cfg.fcr.rtrans_tout = 0;
        }

        L2CAP_TRACE_DEBUG ("l2c_fcr_adj_monitor_retran_timeout: mon_tout:%d, rtrans_tout:%d",
                           p_ccb->our_cfg.fcr.mon_tout, p_ccb->our_cfg.fcr.rtrans_tout);
    }
}
/*******************************************************************************
**
** Function         l2c_fcr_adj_our_rsp_options
**
** Description      Overrides any neccesary FCR options passed in from
**                  L2CA_ConfigRsp based on our FCR options.
**                  Only makes adjustments if channel is in ERTM mode.
**
** Returns          None
**
*******************************************************************************/
void l2c_fcr_adj_our_rsp_options (tL2C_CCB *p_ccb, tL2CAP_CFG_INFO *p_cfg)
{
    assert(p_ccb != NULL);
    assert(p_cfg != NULL);

    /* adjust our monitor/retran timeout */
    l2c_fcr_adj_monitor_retran_timeout(p_ccb);

    p_cfg->fcr_present = p_ccb->out_cfg_fcr_present;

    if (p_cfg->fcr_present) {
// btla-specific ++
        /* Temporary - until a better algorithm is implemented */
        /* If peer's tx_wnd_sz requires too many buffers for us to support, then adjust it. For now, respond with our own tx_wnd_sz. */
        /* Note: peer is not guaranteed to obey our adjustment */
        if (p_ccb->peer_cfg.fcr.tx_win_sz > p_ccb->our_cfg.fcr.tx_win_sz) {
            L2CAP_TRACE_DEBUG ("%s: adjusting requested tx_win_sz from %i to %i", __FUNCTION__, p_ccb->peer_cfg.fcr.tx_win_sz, p_ccb->our_cfg.fcr.tx_win_sz);
            p_ccb->peer_cfg.fcr.tx_win_sz = p_ccb->our_cfg.fcr.tx_win_sz;
        }
// btla-specific --

        p_cfg->fcr.mode         = p_ccb->peer_cfg.fcr.mode;
        p_cfg->fcr.tx_win_sz    = p_ccb->peer_cfg.fcr.tx_win_sz;
        p_cfg->fcr.max_transmit = p_ccb->peer_cfg.fcr.max_transmit;
        p_cfg->fcr.mps          = p_ccb->peer_cfg.fcr.mps;
        p_cfg->fcr.rtrans_tout  = p_ccb->our_cfg.fcr.rtrans_tout;
        p_cfg->fcr.mon_tout     = p_ccb->our_cfg.fcr.mon_tout;
    }
}

/*******************************************************************************
**
** Function         l2c_fcr_renegotiate_chan
**
** Description      Called upon unsuccessful peer response to config request.
**                  If the error is because of the channel mode, it will try
**                  to resend using another supported optional channel.
**
** Returns          TRUE if resent configuration, False if channel matches or
**                  cannot match.
**
*******************************************************************************/
BOOLEAN l2c_fcr_renegotiate_chan(tL2C_CCB *p_ccb, tL2CAP_CFG_INFO *p_cfg)
{
    assert(p_ccb != NULL);
    assert(p_cfg != NULL);

    UINT8   peer_mode = p_ccb->our_cfg.fcr.mode;
    BOOLEAN can_renegotiate;

    /* Skip if this is a reconfiguration from OPEN STATE or if FCR is not returned */
    if (!p_cfg->fcr_present || (p_ccb->config_done & RECONFIG_FLAG)) {
        return (FALSE);
    }

    /* Only retry if there are more channel options to try */
    if (p_cfg->result == L2CAP_CFG_UNACCEPTABLE_PARAMS) {
        peer_mode = (p_cfg->fcr_present) ? p_cfg->fcr.mode : L2CAP_FCR_BASIC_MODE;

        if (p_ccb->our_cfg.fcr.mode != peer_mode) {

            if ((--p_ccb->fcr_cfg_tries) == 0) {
                p_cfg->result = L2CAP_CFG_FAILED_NO_REASON;
                L2CAP_TRACE_WARNING ("l2c_fcr_renegotiate_chan (Max retries exceeded)");
            }

            can_renegotiate = FALSE;

            /* Try another supported mode if available based on our last attempted channel */
            switch (p_ccb->our_cfg.fcr.mode) {
            /* Our Streaming mode request was unnacceptable; try ERTM or Basic */
            case L2CAP_FCR_STREAM_MODE:
                /* Peer wants ERTM and we support it */
                if ( (peer_mode == L2CAP_FCR_ERTM_MODE) && (p_ccb->ertm_info.allowed_modes & L2CAP_FCR_CHAN_OPT_ERTM) ) {
                    L2CAP_TRACE_DEBUG ("l2c_fcr_renegotiate_chan(Trying ERTM)");
                    p_ccb->our_cfg.fcr.mode = L2CAP_FCR_ERTM_MODE;
                    can_renegotiate = TRUE;
                } else  /* Falls through */

                case L2CAP_FCR_ERTM_MODE: {
                /* We can try basic for any other peer mode if we support it */
                if (p_ccb->ertm_info.allowed_modes & L2CAP_FCR_CHAN_OPT_BASIC) {
                    L2CAP_TRACE_DEBUG ("l2c_fcr_renegotiate_chan(Trying Basic)");
                    can_renegotiate = TRUE;
                    p_ccb->our_cfg.fcr.mode = L2CAP_FCR_BASIC_MODE;
                }
            }
            break;

            default:
                /* All other scenarios cannot be renegotiated */
                break;
            }

            if (can_renegotiate) {
                p_ccb->our_cfg.fcr_present = TRUE;

                if (p_ccb->our_cfg.fcr.mode == L2CAP_FCR_BASIC_MODE) {
                    p_ccb->our_cfg.fcs_present = FALSE;
                    p_ccb->our_cfg.ext_flow_spec_present = FALSE;

                    /* Basic Mode uses ACL Data Pool, make sure the MTU fits */
                    if ( (p_cfg->mtu_present) && (p_cfg->mtu > L2CAP_MTU_SIZE) ) {
                        L2CAP_TRACE_WARNING ("L2CAP - adjust MTU: %u too large", p_cfg->mtu);
                        p_cfg->mtu = L2CAP_MTU_SIZE;
                    }
                }

                l2cu_process_our_cfg_req (p_ccb, &p_ccb->our_cfg);
                l2cu_send_peer_config_req (p_ccb, &p_ccb->our_cfg);
                btu_start_timer (&p_ccb->timer_entry, BTU_TTYPE_L2CAP_CHNL, L2CAP_CHNL_CFG_TIMEOUT);
                return (TRUE);
            }
        }
    }

    /* Disconnect if the channels do not match */
    if (p_ccb->our_cfg.fcr.mode != peer_mode) {
        L2CAP_TRACE_WARNING ("L2C CFG:  Channels incompatible (local %d, peer %d)",
                             p_ccb->our_cfg.fcr.mode, peer_mode);
        l2cu_disconnect_chnl (p_ccb);
    }

    return (FALSE);
}


/*******************************************************************************
**
** Function         l2c_fcr_process_peer_cfg_req
**
** Description      This function is called to process the FCR options passed
**                  in the peer's configuration request.
**
** Returns          UINT8 - L2CAP_PEER_CFG_OK, L2CAP_PEER_CFG_UNACCEPTABLE,
**                          or L2CAP_PEER_CFG_DISCONNECT.
**
*******************************************************************************/
UINT8 l2c_fcr_process_peer_cfg_req(tL2C_CCB *p_ccb, tL2CAP_CFG_INFO *p_cfg)
{
    assert(p_ccb != NULL);
    assert(p_cfg != NULL);

    UINT16 max_retrans_size;
    UINT8  fcr_ok = L2CAP_PEER_CFG_OK;

    p_ccb->p_lcb->w4_info_rsp = FALSE;      /* Handles T61x SonyEricsson Bug in Info Request */

    L2CAP_TRACE_EVENT ("l2c_fcr_process_peer_cfg_req() CFG fcr_present:%d fcr.mode:%d CCB FCR mode:%d preferred: %u allowed:%u",
                       p_cfg->fcr_present, p_cfg->fcr.mode, p_ccb->our_cfg.fcr.mode, p_ccb->ertm_info.preferred_mode,
                       p_ccb->ertm_info.allowed_modes);

    /* If Peer wants basic, we are done (accept it or disconnect) */
    if (p_cfg->fcr.mode == L2CAP_FCR_BASIC_MODE) {
        /* If we do not allow basic, disconnect */
        if ( !(p_ccb->ertm_info.allowed_modes & L2CAP_FCR_CHAN_OPT_BASIC) ) {
            fcr_ok = L2CAP_PEER_CFG_DISCONNECT;
        }
    }

    /* Need to negotiate if our modes are not the same */
    else if (p_cfg->fcr.mode != p_ccb->ertm_info.preferred_mode) {
        /* If peer wants a mode that we don't support then retry our mode (ex. rtx/flc), OR
        ** If we want ERTM and they wanted streaming retry our mode.
        ** Note: If we have already determined they support our mode previously
        **       from their EXF mask.
        */
        if ( (((1 << p_cfg->fcr.mode) & L2CAP_FCR_CHAN_OPT_ALL_MASK) == 0)
                || (p_ccb->ertm_info.preferred_mode == L2CAP_FCR_ERTM_MODE) ) {
            p_cfg->fcr.mode = p_ccb->our_cfg.fcr.mode;
            p_cfg->fcr.tx_win_sz = p_ccb->our_cfg.fcr.tx_win_sz;
            p_cfg->fcr.max_transmit = p_ccb->our_cfg.fcr.max_transmit;
            fcr_ok = L2CAP_PEER_CFG_UNACCEPTABLE;
        }

        /* If we wanted basic, then try to renegotiate it */
        else if (p_ccb->ertm_info.preferred_mode == L2CAP_FCR_BASIC_MODE) {
            p_cfg->fcr.mode = L2CAP_FCR_BASIC_MODE;
            p_cfg->fcr.max_transmit = p_cfg->fcr.tx_win_sz = 0;
            p_cfg->fcr.rtrans_tout = p_cfg->fcr.mon_tout = p_cfg->fcr.mps = 0;
            p_ccb->our_cfg.fcr.rtrans_tout = p_ccb->our_cfg.fcr.mon_tout = p_ccb->our_cfg.fcr.mps = 0;
            fcr_ok = L2CAP_PEER_CFG_UNACCEPTABLE;
        }

        /* Only other valid case is if they want ERTM and we wanted STM which should be
           accepted if we support it; otherwise the channel should be disconnected */
        else if ( (p_cfg->fcr.mode != L2CAP_FCR_ERTM_MODE)
                  || !(p_ccb->ertm_info.allowed_modes & L2CAP_FCR_CHAN_OPT_ERTM) ) {
            fcr_ok = L2CAP_PEER_CFG_DISCONNECT;
        }
    }

    /* Configuration for FCR channels so make any adjustments and fwd to upper layer */
    if (fcr_ok == L2CAP_PEER_CFG_OK) {
        /* by default don't need to send params in the response */
        p_ccb->out_cfg_fcr_present = FALSE;

        /* Make any needed adjustments for the response to the peer */
        if (p_cfg->fcr_present && p_cfg->fcr.mode != L2CAP_FCR_BASIC_MODE) {
            /* Peer desires to bypass FCS check, and streaming or ERTM mode */
            if (p_cfg->fcs_present) {
                p_ccb->peer_cfg.fcs = p_cfg->fcs;
                p_ccb->peer_cfg_bits |= L2CAP_CH_CFG_MASK_FCS;
                if ( p_cfg->fcs == L2CAP_CFG_FCS_BYPASS) {
                    p_ccb->bypass_fcs |= L2CAP_CFG_FCS_PEER;
                }
            }

            max_retrans_size = p_ccb->ertm_info.fcr_tx_buf_size - sizeof(BT_HDR)
                               - L2CAP_MIN_OFFSET - L2CAP_SDU_LEN_OFFSET - L2CAP_FCS_LEN;

            /* Ensure the MPS is not bigger than the MTU */
            if ( (p_cfg->fcr.mps == 0) || (p_cfg->fcr.mps > p_ccb->peer_cfg.mtu) ) {
                p_cfg->fcr.mps = p_ccb->peer_cfg.mtu;
                p_ccb->out_cfg_fcr_present = TRUE;
            }

            /* Ensure the MPS is not bigger than our retransmission buffer */
            if (p_cfg->fcr.mps > max_retrans_size) {
                L2CAP_TRACE_DEBUG("CFG: Overriding MPS to %d (orig %d)", max_retrans_size, p_cfg->fcr.mps);

                p_cfg->fcr.mps = max_retrans_size;
                p_ccb->out_cfg_fcr_present = TRUE;
            }

            if (p_cfg->fcr.mode == L2CAP_FCR_ERTM_MODE || p_cfg->fcr.mode == L2CAP_FCR_STREAM_MODE) {
                /* Always respond with FCR ERTM parameters */
                p_ccb->out_cfg_fcr_present = TRUE;
            }
        }

        /* Everything ok, so save the peer's adjusted fcr options */
        p_ccb->peer_cfg.fcr = p_cfg->fcr;

        if (p_cfg->fcr_present) {
            p_ccb->peer_cfg_bits |= L2CAP_CH_CFG_MASK_FCR;
        }
    } else if (fcr_ok == L2CAP_PEER_CFG_UNACCEPTABLE) {
        /* Allow peer only one retry for mode */
        if (p_ccb->peer_cfg_already_rejected) {
            fcr_ok = L2CAP_PEER_CFG_DISCONNECT;
        } else {
            p_ccb->peer_cfg_already_rejected = TRUE;
        }
    }

    return (fcr_ok);
}

#if (L2CAP_ERTM_STATS == TRUE)
/*******************************************************************************
**
** Function         l2c_fcr_collect_ack_delay
**
** Description      collect throughput, delay, queue size of waiting ack
**
** Parameters
**                  tL2C_CCB
**
** Returns          void
**
*******************************************************************************/
static void l2c_fcr_collect_ack_delay (tL2C_CCB *p_ccb, UINT8 num_bufs_acked)
{
    UINT32  index;
    BT_HDR *p_buf;
    UINT8  *p;
    UINT32  timestamp, delay;
    UINT8   xx;
    UINT8   str[120];

    index = p_ccb->fcrb.ack_delay_avg_index;

    /* update sum, max and min of waiting for ack queue size */
    p_ccb->fcrb.ack_q_count_avg[index] +=
        fixed_queue_length(p_ccb->fcrb.waiting_for_ack_q);

    if (fixed_queue_length(p_ccb->fcrb.waiting_for_ack_q) > p_ccb->fcrb.ack_q_count_max[index]) {
        p_ccb->fcrb.ack_q_count_max[index] = fixed_queue_length(p_ccb->fcrb.waiting_for_ack_q);
	}

    if (fixed_queue_length(p_ccb->fcrb.waiting_for_ack_q) < p_ccb->fcrb.ack_q_count_min[index]) {
        p_ccb->fcrb.ack_q_count_min[index] = fixed_queue_length(p_ccb->fcrb.waiting_for_ack_q);
	}

    /* update sum, max and min of round trip delay of acking */
    list_t *list = NULL;
    if (! fixed_queue_is_empty(p_ccb->fcrb.waiting_for_ack_q))
        list = fixed_queue_get_list(p_ccb->fcrb.waiting_for_ack_q);
    if (list != NULL) {
        for (const list_node_t *node = list_begin(list), xx = 0;
             (node != list_end(list)) && (xx < num_bufs_acked);
             node = list_next(node), xx++) {
            p_buf = list_node(node);
            /* adding up length of acked I-frames to get throughput */
            p_ccb->fcrb.throughput[index] += p_buf->len - 8;

            if ( xx == num_bufs_acked - 1 ) {
                /* get timestamp from tx I-frame that receiver is acking */
                p = ((UINT8 *) (p_buf+1)) + p_buf->offset + p_buf->len;
                if (p_ccb->bypass_fcs != L2CAP_BYPASS_FCS) {
                    p += L2CAP_FCS_LEN;
                }

                STREAM_TO_UINT32(timestamp, p);
                delay = osi_time_get_os_boottime_ms() - timestamp;

                p_ccb->fcrb.ack_delay_avg[index] += delay;
                if ( delay > p_ccb->fcrb.ack_delay_max[index] ) {
                    p_ccb->fcrb.ack_delay_max[index] = delay;
                }
                if ( delay < p_ccb->fcrb.ack_delay_min[index] ) {
                    p_ccb->fcrb.ack_delay_min[index] = delay;
                }
			}
        }
    }

    p_ccb->fcrb.ack_delay_avg_count++;

    /* calculate average and initialize next avg, min and max */
    if (p_ccb->fcrb.ack_delay_avg_count > L2CAP_ERTM_STATS_AVG_NUM_SAMPLES) {
        p_ccb->fcrb.ack_delay_avg_count = 0;

        p_ccb->fcrb.ack_q_count_avg[index] /= L2CAP_ERTM_STATS_AVG_NUM_SAMPLES;
        p_ccb->fcrb.ack_delay_avg[index] /= L2CAP_ERTM_STATS_AVG_NUM_SAMPLES;

        /* calculate throughput */
        timestamp = osi_time_get_os_boottime_ms();
        if (timestamp - p_ccb->fcrb.throughput_start > 0 ) {
            p_ccb->fcrb.throughput[index] /= (timestamp - p_ccb->fcrb.throughput_start);
        }

        p_ccb->fcrb.throughput_start = timestamp;

        sprintf(str, "[%02u] throughput: %5u, ack_delay avg:%3u, min:%3u, max:%3u, ack_q_count avg:%3u, min:%3u, max:%3u",
                index, p_ccb->fcrb.throughput[index],
                p_ccb->fcrb.ack_delay_avg[index], p_ccb->fcrb.ack_delay_min[index], p_ccb->fcrb.ack_delay_max[index],
                p_ccb->fcrb.ack_q_count_avg[index], p_ccb->fcrb.ack_q_count_min[index], p_ccb->fcrb.ack_q_count_max[index] );

        BT_TRACE(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI , TRACE_TYPE_GENERIC, "%s", str);

        index = (index + 1) % L2CAP_ERTM_STATS_NUM_AVG;
        p_ccb->fcrb.ack_delay_avg_index = index;

        p_ccb->fcrb.ack_q_count_max[index] = 0;
        p_ccb->fcrb.ack_q_count_min[index] = 0xFFFFFFFF;
        p_ccb->fcrb.ack_q_count_avg[index] = 0;


        p_ccb->fcrb.ack_delay_max[index] = 0;
        p_ccb->fcrb.ack_delay_min[index] = 0xFFFFFFFF;
        p_ccb->fcrb.ack_delay_avg[index] = 0;

        p_ccb->fcrb.throughput[index] = 0;
    }
}
#endif
#endif ///CLASSIC_BT_INCLUDED == TRUE

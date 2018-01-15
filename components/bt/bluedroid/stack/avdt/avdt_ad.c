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
 *  This module contains the AVDTP adaption layer.
 *
 ******************************************************************************/

// #include <assert.h>
#include "bt_trace.h"
#include <string.h>

#include "bt_types.h"
#include "bt_target.h"
#include "bt_defs.h"
#include "avdt_api.h"
#include "avdtc_api.h"
#include "avdt_int.h"
#include "l2c_api.h"
#include "l2cdefs.h"
#include "allocator.h"

#if (defined(AVDT_INCLUDED) && AVDT_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         avdt_ad_type_to_tcid
**
** Description      Derives the TCID from the channel type and SCB.
**
**
** Returns          TCID value.
**
*******************************************************************************/
UINT8 avdt_ad_type_to_tcid(UINT8 type, tAVDT_SCB *p_scb)
{
    UINT8 scb_idx;

    if (type == AVDT_CHAN_SIG) {
        return 0;
    } else {
        scb_idx = avdt_scb_to_hdl(p_scb) - 1;
        /*
        AVDT_TRACE_DEBUG("type: %d, tcid: %d", type, ((scb_idx * (AVDT_CHAN_NUM_TYPES - 1)) + type));
        */
        return ((scb_idx * (AVDT_CHAN_NUM_TYPES - 1)) + type);
    }
}

/*******************************************************************************
**
** Function         avdt_ad_tcid_to_type
**
** Description      Derives the channel type from the TCID.
**
**
** Returns          Channel type value.
**
*******************************************************************************/
static UINT8 avdt_ad_tcid_to_type(UINT8 tcid)
{
    UINT8 type;

    if (tcid == 0) {
        type = AVDT_CHAN_SIG;
    } else {
        /* tcid translates to type based on number of channels, as follows:
        ** only media channel   :  tcid=1,2,3,4,5,6...  type=1,1,1,1,1,1...
        ** media and report     :  tcid=1,2,3,4,5,6...  type=1,2,1,2,1,2...
        ** media, report, recov :  tcid=1,2,3,4,5,6...  type=1,2,3,1,2,3...
        */
        type = ((tcid + AVDT_CHAN_NUM_TYPES - 2) % (AVDT_CHAN_NUM_TYPES - 1)) + 1;
    }
    AVDT_TRACE_DEBUG("tcid: %d, type: %d\n", tcid, type);
    return type;
}


/*******************************************************************************
**
** Function         avdt_ad_init
**
** Description      Initialize adaption layer.
**
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_ad_init(void)
{
    int             i;
    tAVDT_TC_TBL    *p_tbl = avdt_cb.ad.tc_tbl;
    memset(&avdt_cb.ad, 0, sizeof(tAVDT_AD));

    /* make sure the peer_mtu is a valid value */
    for (i = 0; i < AVDT_NUM_TC_TBL; i++, p_tbl++) {
        p_tbl->peer_mtu = L2CAP_DEFAULT_MTU;
    }
}


/*******************************************************************************
**
** Function         avdt_ad_tc_tbl_by_st
**
** Description      Find adaption layer transport channel table entry matching
**                  the given state.
**
**
** Returns          Pointer to matching entry.  For control channel it returns
**                  the matching entry.  For media or other it returns the
**                  first matching entry (there could be more than one).
**
*******************************************************************************/
tAVDT_TC_TBL *avdt_ad_tc_tbl_by_st(UINT8 type, tAVDT_CCB *p_ccb, UINT8 state)
{
    int             i;
    tAVDT_TC_TBL    *p_tbl = avdt_cb.ad.tc_tbl;
    UINT8           ccb_idx;

    if (p_ccb == NULL) {
        /* resending security req */
        for (i = 0; i < AVDT_NUM_TC_TBL; i++, p_tbl++) {
            /* must be AVDT_CHAN_SIG - tcid always zero */
            if ((p_tbl->tcid == 0) &&
                    (p_tbl->state == state)) {
                break;
            }
        }
    } else {
        ccb_idx = avdt_ccb_to_idx(p_ccb);

        for (i = 0; i < AVDT_NUM_TC_TBL; i++, p_tbl++) {
            if (type == AVDT_CHAN_SIG) {
                /* if control channel, tcid always zero */
                if ((p_tbl->tcid == 0) &&
                        (p_tbl->ccb_idx == ccb_idx) &&
                        (p_tbl->state == state)) {
                    break;
                }
            } else {
                /* if other channel, tcid is always > zero */
                if ((p_tbl->tcid > 0) &&
                        (p_tbl->ccb_idx == ccb_idx) &&
                        (p_tbl->state == state)) {
                    break;
                }
            }
        }
    }

    /* if nothing found return null */
    if (i == AVDT_NUM_TC_TBL) {
        p_tbl = NULL;
    }

    return p_tbl;
}


/*******************************************************************************
**
** Function         avdt_ad_tc_tbl_by_lcid
**
** Description      Find adaption layer transport channel table entry by LCID.
**
**
** Returns          Pointer to entry.
**
*******************************************************************************/
tAVDT_TC_TBL *avdt_ad_tc_tbl_by_lcid(UINT16 lcid)
{
    UINT8 idx;

    idx = avdt_cb.ad.lcid_tbl[lcid - L2CAP_BASE_APPL_CID];

    if (idx < AVDT_NUM_TC_TBL) {
        return &avdt_cb.ad.tc_tbl[idx];
    } else {
        return NULL;
    }
}


/*******************************************************************************
**
** Function         avdt_ad_tc_tbl_by_type
**
** Description      This function retrieves the transport channel table entry
**                  for a particular channel.
**
**
** Returns          Pointer to transport channel table entry.
**
*******************************************************************************/
tAVDT_TC_TBL *avdt_ad_tc_tbl_by_type(UINT8 type, tAVDT_CCB *p_ccb, tAVDT_SCB *p_scb)
{
    UINT8           tcid;
    int             i;
    tAVDT_TC_TBL    *p_tbl = avdt_cb.ad.tc_tbl;
    UINT8           ccb_idx = avdt_ccb_to_idx(p_ccb);

    /* get tcid from type, scb */
    tcid = avdt_ad_type_to_tcid(type, p_scb);

    for (i = 0; i < AVDT_NUM_TC_TBL; i++, p_tbl++) {
        if ((p_tbl->tcid == tcid) && (p_tbl->ccb_idx == ccb_idx)) {
            break;
        }
    }

    assert(i != AVDT_NUM_TC_TBL);

    return p_tbl;
}


/*******************************************************************************
**
** Function         avdt_ad_tc_tbl_alloc
**
** Description      Allocate an entry in the traffic channel table.
**
**
** Returns          Pointer to entry.
**
*******************************************************************************/
tAVDT_TC_TBL *avdt_ad_tc_tbl_alloc(tAVDT_CCB *p_ccb)
{
    int             i;
    tAVDT_TC_TBL    *p_tbl = avdt_cb.ad.tc_tbl;

    /* find next free entry in tc table */
    for (i = 0; i < AVDT_NUM_TC_TBL; i++, p_tbl++) {
        if (p_tbl->state == AVDT_AD_ST_UNUSED) {
            break;
        }
    }

    /* sanity check */
    assert(i != AVDT_NUM_TC_TBL);


    /* initialize entry */
    p_tbl->peer_mtu = L2CAP_DEFAULT_MTU;
    p_tbl->cfg_flags = 0;
    p_tbl->ccb_idx = avdt_ccb_to_idx(p_ccb);
    p_tbl->state = AVDT_AD_ST_IDLE;
    return p_tbl;

}

/*******************************************************************************
**
** Function         avdt_ad_tc_tbl_to_idx
**
** Description      Convert a transport channel table entry to an index.
**
**
** Returns          Index value.
**
*******************************************************************************/
UINT8 avdt_ad_tc_tbl_to_idx(tAVDT_TC_TBL *p_tbl)
{
    AVDT_TRACE_DEBUG("avdt_ad_tc_tbl_to_idx: %d\n", (p_tbl - avdt_cb.ad.tc_tbl));
    /* use array arithmetic to determine index */
    return (UINT8) (p_tbl - avdt_cb.ad.tc_tbl);
}

/*******************************************************************************
**
** Function         avdt_ad_tc_close_ind
**
** Description      This function is called by the L2CAP interface when the
**                  L2CAP channel is closed.  It looks up the CCB or SCB for
**                  the channel and sends it a close event.  The reason
**                  parameter is the same value passed by the L2CAP
**                  callback function.
**
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_ad_tc_close_ind(tAVDT_TC_TBL *p_tbl, UINT16 reason)
{
    tAVDT_CCB   *p_ccb;
    tAVDT_SCB   *p_scb;
    tAVDT_SCB_TC_CLOSE  close;
    // UNUSED(reason);

    close.old_tc_state = p_tbl->state;

    /* clear avdt_ad_tc_tbl entry */
    p_tbl->state = AVDT_AD_ST_UNUSED;
    p_tbl->cfg_flags = 0;
    p_tbl->peer_mtu = L2CAP_DEFAULT_MTU;

    AVDT_TRACE_DEBUG("avdt_ad_tc_close_ind tcid: %d, old: %d\n",
                     p_tbl->tcid, close.old_tc_state);

    /* if signaling channel, notify ccb that channel open */
    if (p_tbl->tcid == 0) {
        p_ccb = avdt_ccb_by_idx(p_tbl->ccb_idx);
        p_ccb->disc_rsn = (reason == AVDT_DISC_RSN_ABNORMAL) ? AVDT_DISC_RSN_ABNORMAL : AVDT_DISC_RSN_NORMAL;
        avdt_ccb_event(p_ccb, AVDT_CCB_LL_CLOSE_EVT, NULL);
    }
    /* if media or other channel, notify scb that channel close */
    else {
        /* look up scb in stream routing table by ccb, tcid */
        p_scb = avdt_scb_by_hdl(avdt_cb.ad.rt_tbl[p_tbl->ccb_idx][p_tbl->tcid].scb_hdl);
        if (p_scb != NULL) {
            close.tcid = p_tbl->tcid;
            close.type = avdt_ad_tcid_to_type(p_tbl->tcid);
            close.disc_rsn = (reason == AVDT_DISC_RSN_ABNORMAL) ? AVDT_DISC_RSN_ABNORMAL : AVDT_DISC_RSN_NORMAL;
            avdt_scb_event(p_scb, AVDT_SCB_TC_CLOSE_EVT, (tAVDT_SCB_EVT *)&close);
        }
    }
}

/*******************************************************************************
**
** Function         avdt_ad_tc_open_ind
**
** Description      This function is called by the L2CAP interface when
**                  the L2CAP channel is opened.  It looks up the CCB or SCB
**                  for the channel and sends it an open event.
**
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_ad_tc_open_ind(tAVDT_TC_TBL *p_tbl)
{
    tAVDT_CCB   *p_ccb;
    tAVDT_SCB   *p_scb;
    tAVDT_OPEN  open;
    tAVDT_EVT_HDR evt;

    p_tbl->state = AVDT_AD_ST_OPEN;

    /* if signaling channel, notify ccb that channel open */
    if (p_tbl->tcid == 0) {
        /* set the signal channel to use high priority within the ACL link */
        L2CA_SetTxPriority(avdt_cb.ad.rt_tbl[p_tbl->ccb_idx][AVDT_CHAN_SIG].lcid, L2CAP_CHNL_PRIORITY_HIGH);

        p_ccb = avdt_ccb_by_idx(p_tbl->ccb_idx);
        /* use err_param to indicate the role of connection.
         * AVDT_ACP, if ACP */
        evt.err_param = AVDT_INT;
        if (p_tbl->cfg_flags & AVDT_L2C_CFG_CONN_ACP) {
            evt.err_param = AVDT_ACP;
        }
        avdt_ccb_event(p_ccb, AVDT_CCB_LL_OPEN_EVT, (tAVDT_CCB_EVT *)&evt);
    }
    /* if media or other channel, notify scb that channel open */
    else {
        /* look up scb in stream routing table by ccb, tcid */
        p_scb = avdt_scb_by_hdl(avdt_cb.ad.rt_tbl[p_tbl->ccb_idx][p_tbl->tcid].scb_hdl);

        /* put lcid in event data */
        if (p_scb != NULL) {
            open.peer_mtu = p_tbl->peer_mtu;
            open.lcid = avdt_cb.ad.rt_tbl[p_tbl->ccb_idx][p_tbl->tcid].lcid;
            open.hdr.err_code = avdt_ad_tcid_to_type(p_tbl->tcid);
            avdt_scb_event(p_scb, AVDT_SCB_TC_OPEN_EVT, (tAVDT_SCB_EVT *) &open);
        }
    }
}


/*******************************************************************************
**
** Function         avdt_ad_tc_cong_ind
**
** Description      This function is called by the L2CAP interface layer when
**                  L2CAP calls the congestion callback.  It looks up the CCB
**                  or SCB for the channel and sends it a congestion event.
**                  The is_congested parameter is the same value passed by
**                  the L2CAP callback function.
**
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_ad_tc_cong_ind(tAVDT_TC_TBL *p_tbl, BOOLEAN is_congested)
{
    tAVDT_CCB   *p_ccb;
    tAVDT_SCB   *p_scb;

    /* if signaling channel, notify ccb of congestion */
    if (p_tbl->tcid == 0) {
        p_ccb = avdt_ccb_by_idx(p_tbl->ccb_idx);
        avdt_ccb_event(p_ccb, AVDT_CCB_LL_CONG_EVT, (tAVDT_CCB_EVT *) &is_congested);
    }
    /* if media or other channel, notify scb that channel open */
    else {
        /* look up scb in stream routing table by ccb, tcid */
        p_scb = avdt_scb_by_hdl(avdt_cb.ad.rt_tbl[p_tbl->ccb_idx][p_tbl->tcid].scb_hdl);
        if (p_scb != NULL) {
            avdt_scb_event(p_scb, AVDT_SCB_TC_CONG_EVT, (tAVDT_SCB_EVT *) &is_congested);
        }
    }
}


/*******************************************************************************
**
** Function         avdt_ad_tc_data_ind
**
** Description      This function is called by the L2CAP interface layer when
**                  incoming data is received from L2CAP.  It looks up the CCB
**                  or SCB for the channel and routes the data accordingly.
**
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_ad_tc_data_ind(tAVDT_TC_TBL *p_tbl, BT_HDR *p_buf)
{
    tAVDT_CCB   *p_ccb;
    tAVDT_SCB   *p_scb;

    /* store type (media, recovery, reporting) */
    p_buf->layer_specific = avdt_ad_tcid_to_type(p_tbl->tcid);


    /* if signaling channel, handle control message */
    if (p_tbl->tcid == 0) {
        p_ccb = avdt_ccb_by_idx(p_tbl->ccb_idx);
        avdt_msg_ind(p_ccb, p_buf);
    }
    /* if media or other channel, send event to scb */
    else {
        p_scb = avdt_scb_by_hdl(avdt_cb.ad.rt_tbl[p_tbl->ccb_idx][p_tbl->tcid].scb_hdl);
        if (p_scb != NULL) {
            avdt_scb_event(p_scb, AVDT_SCB_TC_DATA_EVT, (tAVDT_SCB_EVT *) &p_buf);
        } else {
            osi_free(p_buf);
            AVDT_TRACE_ERROR(" avdt_ad_tc_data_ind buffer freed");
        }
    }
}

/*******************************************************************************
**
** Function         avdt_ad_write_req
**
** Description      This function is called by a CCB or SCB to send data to a
**                  transport channel.  It looks up the LCID of the channel
**                  based on the type, CCB, and SCB (if present).  Then it
**                  passes the data to L2CA_DataWrite().
**
**
** Returns          AVDT_AD_SUCCESS, if data accepted, else FALSE
**                  AVDT_AD_CONGESTED, if data accepted and the channel is congested
**                  AVDT_AD_FAILED, if error
**
*******************************************************************************/
UINT8 avdt_ad_write_req(UINT8 type, tAVDT_CCB *p_ccb, tAVDT_SCB *p_scb, BT_HDR *p_buf)
{
    UINT8   tcid;

    /* get tcid from type, scb */
    tcid = avdt_ad_type_to_tcid(type, p_scb);


    return L2CA_DataWrite(avdt_cb.ad.rt_tbl[avdt_ccb_to_idx(p_ccb)][tcid].lcid, p_buf);
}


/*******************************************************************************
**
** Function         avdt_ad_open_req
**
** Description      This function is called by a CCB or SCB to open a transport
**                  channel.  This function allocates and initializes a
**                  transport channel table entry.  The channel can be opened
**                  in two roles:  as an initiator or acceptor.  When opened
**                  as an initiator the function will start an L2CAP connection.
**                  When opened as an acceptor the function simply configures
**                  the table entry to listen for an incoming channel.
**
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_ad_open_req(UINT8 type, tAVDT_CCB *p_ccb, tAVDT_SCB *p_scb, UINT8 role)
{
    tAVDT_TC_TBL    *p_tbl;
    UINT16          lcid;

    if ((p_tbl = avdt_ad_tc_tbl_alloc(p_ccb)) == NULL) {
        AVDT_TRACE_ERROR("avdt_ad_open_req: Cannot allocate p_tbl");
        return;
    }


    p_tbl->tcid = avdt_ad_type_to_tcid(type, p_scb);
    AVDT_TRACE_DEBUG("avdt_ad_open_req: type: %d, role: %d, tcid:%d\n",
                     type, role, p_tbl->tcid);

    if (type == AVDT_CHAN_SIG) {
        /* if signaling, get mtu from registration control block */
        p_tbl->my_mtu = avdt_cb.rcb.ctrl_mtu;
        p_tbl->my_flush_to = L2CAP_DEFAULT_FLUSH_TO;
    } else {
        /* otherwise get mtu from scb */
        p_tbl->my_mtu = p_scb->cs.mtu;
        p_tbl->my_flush_to = p_scb->cs.flush_to;

        /* also set scb_hdl in rt_tbl */
        avdt_cb.ad.rt_tbl[avdt_ccb_to_idx(p_ccb)][p_tbl->tcid].scb_hdl = avdt_scb_to_hdl(p_scb);
        AVDT_TRACE_DEBUG("avdt_cb.ad.rt_tbl[%d][%d].scb_hdl = %d\n",
                         avdt_ccb_to_idx(p_ccb), p_tbl->tcid,
                         avdt_scb_to_hdl(p_scb));
    }

    /* if we're acceptor, we're done; just sit back and listen */
    if (role == AVDT_ACP) {
        p_tbl->state = AVDT_AD_ST_ACP;
    }
    /* else we're inititator, start the L2CAP connection */
    else {
        p_tbl->state = AVDT_AD_ST_CONN;

        /* call l2cap connect req */
        if ((lcid = L2CA_ConnectReq(AVDT_PSM, p_ccb->peer_addr)) != 0) {
            /* if connect req ok, store tcid in lcid table  */
            avdt_cb.ad.lcid_tbl[lcid - L2CAP_BASE_APPL_CID] = avdt_ad_tc_tbl_to_idx(p_tbl);
            AVDT_TRACE_DEBUG("avdt_cb.ad.lcid_tbl[%d] = %d\n",
                             (lcid - L2CAP_BASE_APPL_CID), avdt_ad_tc_tbl_to_idx(p_tbl));

            avdt_cb.ad.rt_tbl[avdt_ccb_to_idx(p_ccb)][p_tbl->tcid].lcid = lcid;
            AVDT_TRACE_DEBUG("avdt_cb.ad.rt_tbl[%d][%d].lcid = 0x%x\n",
                             avdt_ccb_to_idx(p_ccb), p_tbl->tcid,
                             lcid);
        } else {
            /* if connect req failed, call avdt_ad_tc_close_ind() */
            avdt_ad_tc_close_ind(p_tbl, 0);
        }
    }
}

/*******************************************************************************
**
** Function         avdt_ad_close_req
**
** Description      This function is called by a CCB or SCB to close a
**                  transport channel.  The function looks up the LCID for the
**                  channel and calls L2CA_DisconnectReq().
**
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_ad_close_req(UINT8 type, tAVDT_CCB *p_ccb, tAVDT_SCB *p_scb)
{
    UINT8           tcid;
    tAVDT_TC_TBL    *p_tbl;

    p_tbl = avdt_ad_tc_tbl_by_type(type, p_ccb, p_scb);
    AVDT_TRACE_DEBUG("avdt_ad_close_req state: %d\n", p_tbl->state);

    switch (p_tbl->state) {
    case AVDT_AD_ST_UNUSED:
        /* probably for reporting */
        break;
    case AVDT_AD_ST_ACP:
        /* if we're listening on this channel, send ourselves a close ind */
        avdt_ad_tc_close_ind(p_tbl, 0);
        break;
    default:
        /* get tcid from type, scb */
        tcid = avdt_ad_type_to_tcid(type, p_scb);

        /* call l2cap disconnect req */
        L2CA_DisconnectReq(avdt_cb.ad.rt_tbl[avdt_ccb_to_idx(p_ccb)][tcid].lcid);
    }
}

#endif /* #if (defined(AVDT_INCLUDED) && AVDT_INCLUDED == TRUE) */

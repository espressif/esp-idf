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
 *  This file contains action functions for advanced audio/video main state
 *  machine.
 *
 ******************************************************************************/

#include "common/bt_target.h"
#if defined(BTA_AV_INCLUDED) && (BTA_AV_INCLUDED == TRUE)

#include <string.h>
#include "bta/bta_av_api.h"
#include "bta_av_int.h"
#include "stack/avdt_api.h"
#include "bta/utl.h"
#include "stack/l2c_api.h"
#include "osi/allocator.h"
#include "osi/list.h"
#if( defined BTA_AR_INCLUDED ) && (BTA_AR_INCLUDED == TRUE)
#include "bta/bta_ar_api.h"
#endif

#define LOG_TAG "bt_bta_av"
// #include "osi/include/log.h"
#include "common/bt_trace.h"

/*****************************************************************************
**  Constants
*****************************************************************************/
/* the timer in milliseconds to wait for open req after setconfig for incoming connections */
#ifndef BTA_AV_SIG_TIME_VAL
#define BTA_AV_SIG_TIME_VAL 8000
#endif

/* In millisec to wait for signalling from SNK when it is initiated from SNK.   */
/* If not, we will start signalling from SRC.                                   */
#ifndef BTA_AV_ACP_SIG_TIME_VAL
#define BTA_AV_ACP_SIG_TIME_VAL 2000
#endif

static void bta_av_acp_sig_timer_cback (TIMER_LIST_ENT *p_tle);

/*******************************************************************************
**
** Function         bta_av_get_rcb_by_shdl
**
** Description      find the RCB associated with the given SCB handle.
**
** Returns          tBTA_AV_RCB
**
*******************************************************************************/
tBTA_AV_RCB *bta_av_get_rcb_by_shdl(UINT8 shdl)
{
    tBTA_AV_RCB *p_rcb = NULL;
    int         i;

    for (i = 0; i < BTA_AV_NUM_RCB; i++) {
        if (bta_av_cb.rcb[i].shdl == shdl && bta_av_cb.rcb[i].handle != BTA_AV_RC_HANDLE_NONE) {
            p_rcb = &bta_av_cb.rcb[i];
            break;
        }
    }
    return p_rcb;
}
#define BTA_AV_STS_NO_RSP       0xFF    /* a number not used by tAVRC_STS */

/*******************************************************************************
**
** Function         bta_av_del_rc
**
** Description      delete the given AVRC handle.
**
** Returns          void
**
*******************************************************************************/
void bta_av_del_rc(tBTA_AV_RCB *p_rcb)
{
    tBTA_AV_SCB  *p_scb;
    UINT8        rc_handle;      /* connected AVRCP handle */

    p_scb = NULL;
    if (p_rcb->handle != BTA_AV_RC_HANDLE_NONE) {
        if (p_rcb->shdl) {
            /* Validate array index*/
            if ((p_rcb->shdl - 1) < BTA_AV_NUM_STRS) {
                p_scb = bta_av_cb.p_scb[p_rcb->shdl - 1];
            }
            if (p_scb) {
                APPL_TRACE_DEBUG("bta_av_del_rc shdl:%d, srch:%d rc_handle:%d", p_rcb->shdl,
                                 p_scb->rc_handle, p_rcb->handle);
                if (p_scb->rc_handle == p_rcb->handle) {
                    p_scb->rc_handle = BTA_AV_RC_HANDLE_NONE;
                }
                /* just in case the RC timer is active
                if(bta_av_cb.features & BTA_AV_FEAT_RCCT && p_scb->chnl == BTA_AV_CHNL_AUDIO) */
                bta_sys_stop_timer(&p_scb->timer);
            }
        }

        APPL_TRACE_EVENT("bta_av_del_rc  handle: %d status=0x%x, rc_acp_handle:%d, idx:%d",
                         p_rcb->handle, p_rcb->status, bta_av_cb.rc_acp_handle, bta_av_cb.rc_acp_idx);
        rc_handle = p_rcb->handle;
        if (!(p_rcb->status & BTA_AV_RC_CONN_MASK) ||
                ((p_rcb->status & BTA_AV_RC_ROLE_MASK) == BTA_AV_RC_ROLE_INT) ) {
            p_rcb->status = 0;
            p_rcb->handle = BTA_AV_RC_HANDLE_NONE;
            p_rcb->shdl = 0;
            p_rcb->lidx = 0;
        }
        /* else ACP && connected. do not clear the handle yet */
        AVRC_Close(rc_handle);
        if (rc_handle == bta_av_cb.rc_acp_handle) {
            bta_av_cb.rc_acp_handle = BTA_AV_RC_HANDLE_NONE;
        }
        APPL_TRACE_EVENT("end del_rc handle: %d status=0x%x, rc_acp_handle:%d, lidx:%d",
                         p_rcb->handle, p_rcb->status, bta_av_cb.rc_acp_handle, p_rcb->lidx);
    }
}


/*******************************************************************************
**
** Function         bta_av_close_all_rc
**
** Description      close the all AVRC handle.
**
** Returns          void
**
*******************************************************************************/
static void bta_av_close_all_rc(tBTA_AV_CB *p_cb)
{
    int i;

    for (i = 0; i < BTA_AV_NUM_RCB; i++) {
        if ((p_cb->disabling == TRUE) || (bta_av_cb.rcb[i].shdl != 0)) {
            bta_av_del_rc(&bta_av_cb.rcb[i]);
        }
    }
}

/*******************************************************************************
**
** Function         bta_av_del_sdp_rec
**
** Description      delete the given SDP record handle.
**
** Returns          void
**
*******************************************************************************/
static void bta_av_del_sdp_rec(UINT32 *p_sdp_handle)
{
    if (*p_sdp_handle != 0) {
        SDP_DeleteRecord(*p_sdp_handle);
        *p_sdp_handle = 0;
    }
}

/*******************************************************************************
**
** Function         bta_av_avrc_sdp_cback
**
** Description      AVRCP service discovery callback.
**
** Returns          void
**
*******************************************************************************/
static void bta_av_avrc_sdp_cback(UINT16 status)
{
    BT_HDR *p_msg;
    UNUSED(status);

    if ((p_msg = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_msg->event = BTA_AV_SDP_AVRC_DISC_EVT;
        bta_sys_sendmsg(p_msg);
    }
}

/*******************************************************************************
**
** Function         bta_av_rc_ctrl_cback
**
** Description      AVRCP control callback.
**
** Returns          void
**
*******************************************************************************/
static void bta_av_rc_ctrl_cback(UINT8 handle, UINT8 event, UINT16 result, BD_ADDR peer_addr)
{
    tBTA_AV_RC_CONN_CHG *p_msg;
    UINT16 msg_event = 0;
    UNUSED(result);

    APPL_TRACE_EVENT("%s handle: %d event: 0x%x",__func__, handle, event);

    if (event == AVRC_OPEN_IND_EVT) {
        /* save handle of opened connection
        bta_av_cb.rc_handle = handle;*/

        msg_event = BTA_AV_AVRC_OPEN_EVT;
    } else if (event == AVRC_CLOSE_IND_EVT) {
        msg_event = BTA_AV_AVRC_CLOSE_EVT;
    }

    if (msg_event) {
        if ((p_msg = (tBTA_AV_RC_CONN_CHG *) osi_malloc(sizeof(tBTA_AV_RC_CONN_CHG))) != NULL) {
            p_msg->hdr.event = msg_event;
            p_msg->handle    = handle;
            if (peer_addr) {
                bdcpy(p_msg->peer_addr, peer_addr);
            }
            bta_sys_sendmsg(p_msg);
        }
    }
}

/*******************************************************************************
**
** Function         bta_av_rc_msg_cback
**
** Description      AVRCP message callback.
**
** Returns          void
**
*******************************************************************************/
static void bta_av_rc_msg_cback(UINT8 handle, UINT8 label, UINT8 opcode, tAVRC_MSG *p_msg)
{
    UINT8           *p_data_src = NULL;
    UINT16          data_len = 0;

    APPL_TRACE_DEBUG("%s handle: %u opcode=0x%x", __func__, handle, opcode);

    /* Determine the size of the buffer we need */
    if (opcode == AVRC_OP_VENDOR && p_msg->vendor.p_vendor_data != NULL) {
        p_data_src = p_msg->vendor.p_vendor_data;
        data_len = (UINT16) p_msg->vendor.vendor_len;
    } else if (opcode == AVRC_OP_PASS_THRU && p_msg->pass.p_pass_data != NULL) {
        p_data_src = p_msg->pass.p_pass_data;
        data_len = (UINT16) p_msg->pass.pass_len;
    }

    /* Create a copy of the message */
    tBTA_AV_RC_MSG *p_buf =
        (tBTA_AV_RC_MSG *)osi_malloc((UINT16)(sizeof(tBTA_AV_RC_MSG) + data_len));
    if (p_buf != NULL) {
        p_buf->hdr.event = BTA_AV_AVRC_MSG_EVT;
        p_buf->handle = handle;
        p_buf->label = label;
        p_buf->opcode = opcode;
        memcpy(&p_buf->msg, p_msg, sizeof(tAVRC_MSG));
        /* Copy the data payload, and set the pointer to it */
        if (p_data_src != NULL) {
            UINT8 *p_data_dst = (UINT8 *)(p_buf + 1);
            memcpy(p_data_dst, p_data_src, data_len);
            if (opcode == AVRC_OP_VENDOR) {
                p_buf->msg.vendor.p_vendor_data = p_data_dst;
            } else if (opcode == AVRC_OP_PASS_THRU) {
                p_buf->msg.pass.p_pass_data = p_data_dst;
            }
        }
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         bta_av_rc_create
**
** Description      alloc RCB and call AVRC_Open
**
** Returns          the created rc handle
**
*******************************************************************************/
UINT8 bta_av_rc_create(tBTA_AV_CB *p_cb, UINT8 role, UINT8 shdl, UINT8 lidx)
{
    tAVRC_CONN_CB ccb;
    BD_ADDR_PTR   bda = (BD_ADDR_PTR)bd_addr_any;
    UINT8         status = BTA_AV_RC_ROLE_ACP;
    tBTA_AV_SCB  *p_scb = p_cb->p_scb[shdl - 1];
    int i;
    UINT8   rc_handle;
    tBTA_AV_RCB *p_rcb;

    if (role == AVCT_INT) {
        bda = p_scb->peer_addr;
        status = BTA_AV_RC_ROLE_INT;
    } else {
        if ((p_rcb = bta_av_get_rcb_by_shdl(shdl)) != NULL ) {
            APPL_TRACE_ERROR("bta_av_rc_create ACP handle exist for shdl:%d", shdl);
            return p_rcb->handle;
        }
    }

    ccb.p_ctrl_cback = bta_av_rc_ctrl_cback;
    ccb.p_msg_cback = bta_av_rc_msg_cback;
    ccb.company_id = p_bta_av_cfg->company_id;
    ccb.conn = role;
    /* note: BTA_AV_FEAT_RCTG = AVRC_CT_TARGET, BTA_AV_FEAT_RCCT = AVRC_CT_CONTROL */
    ccb.control = p_cb->features & (BTA_AV_FEAT_RCTG | BTA_AV_FEAT_RCCT | AVRC_CT_PASSIVE);


    if (AVRC_Open(&rc_handle, &ccb, bda) != AVRC_SUCCESS) {
        return BTA_AV_RC_HANDLE_NONE;
    }

    i = rc_handle;
    p_rcb = &p_cb->rcb[i];

    if (p_rcb->handle != BTA_AV_RC_HANDLE_NONE) {
        APPL_TRACE_ERROR("bta_av_rc_create found duplicated handle:%d", rc_handle);
    }

    p_rcb->handle = rc_handle;
    p_rcb->status = status;
    p_rcb->shdl = shdl;
    p_rcb->lidx = lidx;
    p_rcb->peer_features = 0;
    p_rcb->peer_ct_features = 0;
    p_rcb->peer_tg_features = 0;
    if (lidx == (BTA_AV_NUM_LINKS + 1)) {
        /* this LIDX is reserved for the AVRCP ACP connection */
        p_cb->rc_acp_handle = p_rcb->handle;
        p_cb->rc_acp_idx = (i + 1);
        APPL_TRACE_DEBUG("rc_acp_handle:%d idx:%d", p_cb->rc_acp_handle, p_cb->rc_acp_idx);
    }
    APPL_TRACE_DEBUG("create %d, role: %d, shdl:%d, rc_handle:%d, lidx:%d, status:0x%x",
                     i, role, shdl, p_rcb->handle, lidx, p_rcb->status);

    return rc_handle;
}

/*******************************************************************************
**
** Function         bta_av_valid_group_navi_msg
**
** Description      Check if it is Group Navigation Msg for Metadata
**
** Returns          BTA_AV_RSP_ACCEPT or BTA_AV_RSP_NOT_IMPL.
**
*******************************************************************************/
static tBTA_AV_CODE bta_av_group_navi_supported(UINT8 len, UINT8 *p_data, BOOLEAN is_inquiry)
{
    tBTA_AV_CODE ret = BTA_AV_RSP_NOT_IMPL;
    UINT8 *p_ptr = p_data;
    UINT16 u16;
    UINT32 u32;

    if (p_bta_av_cfg->avrc_group && len == BTA_GROUP_NAVI_MSG_OP_DATA_LEN) {
        BTA_AV_BE_STREAM_TO_CO_ID(u32, p_ptr);
        BE_STREAM_TO_UINT16(u16, p_ptr);

        if (u32 == AVRC_CO_METADATA) {
            if (is_inquiry) {
                if (u16 <= AVRC_PDU_PREV_GROUP) {
                    ret = BTA_AV_RSP_IMPL_STBL;
                }
            } else {
                if (u16 <= AVRC_PDU_PREV_GROUP) {
                    ret = BTA_AV_RSP_ACCEPT;
                } else {
                    ret = BTA_AV_RSP_REJ;
                }
            }
        }
    }

    return ret;
}

/*******************************************************************************
**
** Function         bta_av_op_supported
**
** Description      Check if remote control operation is supported.
**
** Returns          BTA_AV_RSP_ACCEPT of supported, BTA_AV_RSP_NOT_IMPL if not.
**
*******************************************************************************/
static tBTA_AV_CODE bta_av_op_supported(tBTA_AV_RC rc_id, BOOLEAN is_inquiry)
{
    tBTA_AV_CODE ret_code = BTA_AV_RSP_NOT_IMPL;
    BOOLEAN rc_id_allowed = FALSE;
    if (bta_av_cb.p_rc_cos && bta_av_cb.p_rc_cos->rc_cmd) {
        rc_id_allowed = bta_av_cb.p_rc_cos->rc_cmd(rc_id);
    }

    if (rc_id_allowed == TRUE) {
        ret_code = is_inquiry ? BTA_AV_RSP_IMPL_STBL : BTA_AV_RSP_ACCEPT;
    }
    return ret_code;
}

/*******************************************************************************
**
** Function         bta_av_find_lcb
**
** Description      Given BD_addr, find the associated LCB.
**
** Returns          NULL, if not found.
**
*******************************************************************************/
tBTA_AV_LCB *bta_av_find_lcb(BD_ADDR addr, UINT8 op)
{
    tBTA_AV_CB   *p_cb = &bta_av_cb;
    int     xx;
    UINT8   mask;
    tBTA_AV_LCB *p_lcb = NULL;

    for (xx = 0; xx < BTA_AV_NUM_LINKS; xx++) {
        mask = 1 << xx; /* the used mask for this lcb */
        if ((mask & p_cb->conn_lcb) && 0 == ( bdcmp(p_cb->lcb[xx].addr, addr))) {
            p_lcb = &p_cb->lcb[xx];
            if (op == BTA_AV_LCB_FREE) {
                p_cb->conn_lcb &= ~mask; /* clear the connect mask */
                APPL_TRACE_DEBUG("conn_lcb: 0x%x", p_cb->conn_lcb);
            }
            break;
        }
    }
    return p_lcb;
}

/*******************************************************************************
**
** Function         bta_av_rc_opened
**
** Description      Set AVRCP state to opened.
**
** Returns          void
**
*******************************************************************************/
void bta_av_rc_opened(tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_RC_OPEN rc_open;
    tBTA_AV_SCB     *p_scb;
    int         i;
    UINT8       shdl = 0;
    tBTA_AV_LCB *p_lcb;
    tBTA_AV_RCB *p_rcb;
    UINT8       tmp;
    UINT8       disc = 0;

    /* find the SCB & stop the timer */
    for (i = 0; i < BTA_AV_NUM_STRS; i++) {
        p_scb = p_cb->p_scb[i];
        if (p_scb && bdcmp(p_scb->peer_addr, p_data->rc_conn_chg.peer_addr) == 0) {
            p_scb->rc_handle = p_data->rc_conn_chg.handle;
            APPL_TRACE_DEBUG("bta_av_rc_opened shdl:%d, srch %d", i + 1, p_scb->rc_handle);
            shdl = i + 1;
            APPL_TRACE_EVENT("%s allow incoming AVRCP connections:%d", __func__, p_scb->use_rc);
            bta_sys_stop_timer(&p_scb->timer);
            disc = p_scb->hndl;
            break;
        }
    }

    i = p_data->rc_conn_chg.handle;
    if (p_cb->rcb[i].handle == BTA_AV_RC_HANDLE_NONE) {
        APPL_TRACE_ERROR("not a valid handle:%d any more", i);
        return;
    }


    if (p_cb->rcb[i].lidx == (BTA_AV_NUM_LINKS + 1) && shdl != 0) {
        /* rc is opened on the RC only ACP channel, but is for a specific
         * SCB -> need to switch RCBs */
        p_rcb = bta_av_get_rcb_by_shdl(shdl);
        if (p_rcb) {
            p_rcb->shdl = p_cb->rcb[i].shdl;
            tmp         = p_rcb->lidx;
            p_rcb->lidx = p_cb->rcb[i].lidx;
            p_cb->rcb[i].lidx = tmp;
            p_cb->rc_acp_handle = p_rcb->handle;
            p_cb->rc_acp_idx = (p_rcb - p_cb->rcb) + 1;
            APPL_TRACE_DEBUG("switching RCB rc_acp_handle:%d idx:%d",
                             p_cb->rc_acp_handle, p_cb->rc_acp_idx);
        }
    }

    p_cb->rcb[i].shdl = shdl;
    rc_open.rc_handle = i;
    APPL_TRACE_EVENT("bta_av_rc_opened rcb[%d] shdl:%d lidx:%d/%d",
                     i, shdl, p_cb->rcb[i].lidx, p_cb->lcb[BTA_AV_NUM_LINKS].lidx);
    p_cb->rcb[i].status |= BTA_AV_RC_CONN_MASK;

    if (!shdl && 0 == p_cb->lcb[BTA_AV_NUM_LINKS].lidx) {
        /* no associated SCB -> connected to an RC only device
         * update the index to the extra LCB */
        p_lcb = &p_cb->lcb[BTA_AV_NUM_LINKS];
        bdcpy(p_lcb->addr, p_data->rc_conn_chg.peer_addr);
        APPL_TRACE_DEBUG("rc_only bd_addr:%02x-%02x-%02x-%02x-%02x-%02x",
                         p_lcb->addr[0], p_lcb->addr[1],
                         p_lcb->addr[2], p_lcb->addr[3],
                         p_lcb->addr[4], p_lcb->addr[5]);
        p_lcb->lidx = BTA_AV_NUM_LINKS + 1;
        p_cb->rcb[i].lidx = p_lcb->lidx;
        p_lcb->conn_msk = 1;
        APPL_TRACE_ERROR("rcb[%d].lidx=%d, lcb.conn_msk=x%x",
                         i, p_cb->rcb[i].lidx, p_lcb->conn_msk);
        disc = p_data->rc_conn_chg.handle | BTA_AV_CHNL_MSK;
    }

    bdcpy(rc_open.peer_addr, p_data->rc_conn_chg.peer_addr);
    rc_open.peer_features = p_cb->rcb[i].peer_features;
    rc_open.peer_ct_features = p_cb->rcb[i].peer_ct_features;
    rc_open.peer_tg_features = p_cb->rcb[i].peer_tg_features;
    rc_open.sdp_disc_done = TRUE;
    rc_open.status = BTA_AV_SUCCESS;
    APPL_TRACE_DEBUG("local features:x%x peer_features:x%x", p_cb->features,
                     rc_open.peer_features);
    if (rc_open.peer_features == 0) {
        /* we have not done SDP on peer RC capabilities.
         * peer must have initiated the RC connection */
        rc_open.peer_features = BTA_AV_FEAT_RCCT;
        rc_open.sdp_disc_done = FALSE;
        bta_av_rc_disc(disc);
    }
    (*p_cb->p_cback)(BTA_AV_RC_OPEN_EVT, (tBTA_AV *) &rc_open);

}


/*******************************************************************************
**
** Function         bta_av_rc_remote_cmd
**
** Description      Send an AVRCP remote control command.
**
** Returns          void
**
*******************************************************************************/
void bta_av_rc_remote_cmd(tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_RCB    *p_rcb;
    if (p_cb->features & BTA_AV_FEAT_RCCT) {
        if (p_data->hdr.layer_specific < BTA_AV_NUM_RCB) {
            p_rcb = &p_cb->rcb[p_data->hdr.layer_specific];
            if (p_rcb->status & BTA_AV_RC_CONN_MASK) {
                AVRC_PassCmd(p_rcb->handle, p_data->api_remote_cmd.label,
                             &p_data->api_remote_cmd.msg);
            }
        }
    }
}

/*******************************************************************************
**
** Function         bta_av_rc_vendor_cmd
**
** Description      Send an AVRCP vendor specific command.
**
** Returns          void
**
*******************************************************************************/
void bta_av_rc_vendor_cmd(tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_RCB    *p_rcb;
    if ( (p_cb->features & (BTA_AV_FEAT_RCCT | BTA_AV_FEAT_VENDOR)) ==
            (BTA_AV_FEAT_RCCT | BTA_AV_FEAT_VENDOR)) {
        if (p_data->hdr.layer_specific < BTA_AV_NUM_RCB) {
            p_rcb = &p_cb->rcb[p_data->hdr.layer_specific];
            AVRC_VendorCmd(p_rcb->handle, p_data->api_vendor.label, &p_data->api_vendor.msg);
        }
    }
}

/*******************************************************************************
**
** Function         bta_av_rc_vendor_rsp
**
** Description      Send an AVRCP vendor specific response.
**
** Returns          void
**
*******************************************************************************/
void bta_av_rc_vendor_rsp(tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_RCB    *p_rcb;
    if ( (p_cb->features & (BTA_AV_FEAT_RCTG | BTA_AV_FEAT_VENDOR)) ==
            (BTA_AV_FEAT_RCTG | BTA_AV_FEAT_VENDOR)) {
        if (p_data->hdr.layer_specific < BTA_AV_NUM_RCB) {
            p_rcb = &p_cb->rcb[p_data->hdr.layer_specific];
            AVRC_VendorRsp(p_rcb->handle, p_data->api_vendor.label, &p_data->api_vendor.msg);
        }
    }
}

/*******************************************************************************
**
** Function         bta_av_rc_meta_rsp
**
** Description      Send an AVRCP metadata/advanced control command/response.
**
** Returns          void
**
*******************************************************************************/
void bta_av_rc_meta_rsp(tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_RCB *p_rcb;
    BOOLEAN         do_free = TRUE;

    if ((p_cb->features & BTA_AV_FEAT_METADATA) && (p_data->hdr.layer_specific < BTA_AV_NUM_RCB)) {
        if ((p_data->api_meta_rsp.is_rsp && (p_cb->features & BTA_AV_FEAT_RCTG)) ||
                (!p_data->api_meta_rsp.is_rsp && (p_cb->features & BTA_AV_FEAT_RCCT)) ) {
            p_rcb = &p_cb->rcb[p_data->hdr.layer_specific];
            if (p_rcb->handle != BTA_AV_RC_HANDLE_NONE) {
                AVRC_MsgReq(p_rcb->handle, p_data->api_meta_rsp.label,
                            p_data->api_meta_rsp.rsp_code,
                            p_data->api_meta_rsp.p_pkt);
                do_free = FALSE;
            }
        }
    }

    if (do_free) {
        osi_free (p_data->api_meta_rsp.p_pkt);
    }
}

/*******************************************************************************
**
** Function         bta_av_rc_free_rsp
**
** Description      free an AVRCP metadata command buffer.
**
** Returns          void
**
*******************************************************************************/
void bta_av_rc_free_rsp (tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data)
{
    UNUSED(p_cb);

    osi_free (p_data->api_meta_rsp.p_pkt);
}

/*******************************************************************************
**
** Function         bta_av_rc_meta_req
**
** Description      Send an AVRCP metadata command.
**
** Returns          void
**
*******************************************************************************/
void bta_av_rc_free_msg (tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data)
{
    UNUSED(p_cb);
    UNUSED(p_data);
}



/*******************************************************************************
**
** Function         bta_av_chk_notif_evt_id
**
** Description      make sure the requested player id is valid.
**
** Returns          BTA_AV_STS_NO_RSP, if no error
**
*******************************************************************************/
static tAVRC_STS bta_av_chk_notif_evt_id(tAVRC_MSG_VENDOR *p_vendor)
{
    tAVRC_STS   status = BTA_AV_STS_NO_RSP;
    UINT16      u16;
    UINT8       *p = p_vendor->p_vendor_data + 2;

    BE_STREAM_TO_UINT16 (u16, p);
    /* double check the fixed length */
    if ((u16 != 5) || (p_vendor->vendor_len != 9)) {
        status = AVRC_STS_INTERNAL_ERR;
    } else {
        /* make sure the event_id is valid */
        status = AVRC_STS_BAD_PARAM;
        if (bta_av_cb.p_rc_cos && bta_av_cb.p_rc_cos->rn_evt_supported) {
            if (bta_av_cb.p_rc_cos->rn_evt_supported(*p) == TRUE) {
                status = BTA_AV_STS_NO_RSP;
            }
        }
    }

    return status;
}

/*******************************************************************************
**
** Function         bta_av_proc_meta_cmd
**
** Description      Process an AVRCP metadata command from the peer.
**
** Returns          TRUE to respond immediately
**
*******************************************************************************/
tBTA_AV_EVT bta_av_proc_meta_cmd(tAVRC_RESPONSE  *p_rc_rsp, tBTA_AV_RC_MSG *p_msg, UINT8 *p_ctype)
{
    tBTA_AV_EVT evt = BTA_AV_META_MSG_EVT;
    UINT8       u8, pdu, *p;
    UINT16      u16;
    tAVRC_MSG_VENDOR    *p_vendor = &p_msg->msg.vendor;

#if (AVRC_METADATA_INCLUDED == TRUE)

    pdu = *(p_vendor->p_vendor_data);
    p_rc_rsp->pdu = pdu;
    *p_ctype = AVRC_RSP_REJ;
    /* Metadata messages only use PANEL sub-unit type */
    if (p_vendor->hdr.subunit_type != AVRC_SUB_PANEL) {
        APPL_TRACE_DEBUG("SUBUNIT must be PANEL");
        /* reject it */
        evt = 0;
        p_vendor->hdr.ctype = BTA_AV_RSP_NOT_IMPL;
        AVRC_VendorRsp(p_msg->handle, p_msg->label, &p_msg->msg.vendor);
    } else if (!AVRC_IsValidAvcType(pdu, p_vendor->hdr.ctype) ) {
        APPL_TRACE_DEBUG("Invalid pdu/ctype: 0x%x, %d", pdu, p_vendor->hdr.ctype);
        /* reject invalid message without reporting to app */
        evt = 0;
        p_rc_rsp->rsp.status = AVRC_STS_BAD_CMD;
    } else {
        switch (pdu) {
        case AVRC_PDU_GET_CAPABILITIES:
            /* process GetCapabilities command without reporting the event to app */
            evt = 0;
            u8 = *(p_vendor->p_vendor_data + 4);
            p = p_vendor->p_vendor_data + 2;
            p_rc_rsp->get_caps.capability_id = u8;
            BE_STREAM_TO_UINT16 (u16, p);
            if ((u16 != 1) || (p_vendor->vendor_len != 5)) {
                p_rc_rsp->get_caps.status = AVRC_STS_INTERNAL_ERR;
            } else {
                p_rc_rsp->get_caps.status = AVRC_STS_NO_ERROR;
                if (u8 == AVRC_CAP_COMPANY_ID) {
                    *p_ctype = AVRC_RSP_IMPL_STBL;
                    p_rc_rsp->get_caps.count = p_bta_av_cfg->num_co_ids;
                    memcpy(p_rc_rsp->get_caps.param.company_id, p_bta_av_cfg->p_meta_co_ids,
                           (p_bta_av_cfg->num_co_ids << 2));
                } else if (u8 == AVRC_CAP_EVENTS_SUPPORTED) {
                    *p_ctype = AVRC_RSP_IMPL_STBL;
                    if (bta_av_cb.p_rc_cos && bta_av_cb.p_rc_cos->rn_evt_cap) {
                        p_rc_rsp->get_caps.count = bta_av_cb.p_rc_cos->rn_evt_cap(
                            p_rc_rsp->get_caps.param.event_id);
                    } else {
                        p_rc_rsp->get_caps.count = 0;
                    }
                } else {
                    APPL_TRACE_DEBUG("Invalid capability ID: 0x%x", u8);
                    /* reject - unknown capability ID */
                    p_rc_rsp->get_caps.status = AVRC_STS_BAD_PARAM;
                }
            }
            break;

        case AVRC_PDU_REGISTER_NOTIFICATION:
            /* make sure the event_id is implemented */
            p_rc_rsp->rsp.status = bta_av_chk_notif_evt_id (p_vendor);
            if (p_rc_rsp->rsp.status != BTA_AV_STS_NO_RSP) {
                evt = 0;
            }
            break;

        case AVRC_PDU_SET_ABSOLUTE_VOLUME:
            p_rc_rsp->rsp.status = BTA_AV_STS_NO_RSP;
            break;
        case AVRC_PDU_SET_PLAYER_APP_VALUE:
            /* Setting of a value by CT does not implicitly mean that the setting will take effect on TG. */
            /* The setting shall take effect after a play command from CT. */
            break;
        default:
            APPL_TRACE_WARNING("%s unhandled RC vendor PDU: 0x%x", __FUNCTION__, pdu);
            break;
        }
    }
#else
    APPL_TRACE_DEBUG("AVRCP 1.3 Metadata not supporteed. Reject command.");
    /* reject invalid message without reporting to app */
    evt = 0;
    p_rc_rsp->rsp.status = AVRC_STS_BAD_CMD;
#endif

    return evt;
}


/*******************************************************************************
**
** Function         bta_av_rc_msg
**
** Description      Process an AVRCP message from the peer.
**
** Returns          void
**
*******************************************************************************/
void bta_av_rc_msg(tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_EVT evt = 0;
    tBTA_AV     av;
    BT_HDR      *p_pkt = NULL;
    tAVRC_MSG_VENDOR    *p_vendor = &p_data->rc_msg.msg.vendor;
    BOOLEAN is_inquiry = ((p_data->rc_msg.msg.hdr.ctype == AVRC_CMD_SPEC_INQ) || p_data->rc_msg.msg.hdr.ctype == AVRC_CMD_GEN_INQ);
#if (AVRC_METADATA_INCLUDED == TRUE)
    UINT8       ctype = 0;
    tAVRC_RESPONSE  rc_rsp;

    rc_rsp.rsp.status = BTA_AV_STS_NO_RSP;
#endif

    if (p_data->rc_msg.opcode == AVRC_OP_PASS_THRU) {
        /* if this is a pass thru command */
        if ((p_data->rc_msg.msg.hdr.ctype == AVRC_CMD_CTRL) ||
                (p_data->rc_msg.msg.hdr.ctype == AVRC_CMD_SPEC_INQ) ||
                (p_data->rc_msg.msg.hdr.ctype == AVRC_CMD_GEN_INQ)
           ) {
            /* check if operation is supported */
            if (p_data->rc_msg.msg.pass.op_id == AVRC_ID_VENDOR) {
                p_data->rc_msg.msg.hdr.ctype = BTA_AV_RSP_NOT_IMPL;
#if (AVRC_METADATA_INCLUDED == TRUE)
                if (p_cb->features & BTA_AV_FEAT_METADATA) {
                    p_data->rc_msg.msg.hdr.ctype =
                        bta_av_group_navi_supported(p_data->rc_msg.msg.pass.pass_len,
                                                    p_data->rc_msg.msg.pass.p_pass_data, is_inquiry);
                }
#endif
            } else {
                p_data->rc_msg.msg.hdr.ctype = bta_av_op_supported(p_data->rc_msg.msg.pass.op_id, is_inquiry);
            }

            APPL_TRACE_DEBUG("ctype %d", p_data->rc_msg.msg.hdr.ctype)

            /* send response */
            if (p_data->rc_msg.msg.hdr.ctype != BTA_AV_RSP_INTERIM) {
                AVRC_PassRsp(p_data->rc_msg.handle, p_data->rc_msg.label, &p_data->rc_msg.msg.pass);
            }

            /* set up for callback if supported */
            if (p_data->rc_msg.msg.hdr.ctype == BTA_AV_RSP_ACCEPT || p_data->rc_msg.msg.hdr.ctype == BTA_AV_RSP_INTERIM) {
                evt = BTA_AV_REMOTE_CMD_EVT;
                av.remote_cmd.rc_id = p_data->rc_msg.msg.pass.op_id;
                av.remote_cmd.key_state = p_data->rc_msg.msg.pass.state;
                av.remote_cmd.p_data = p_data->rc_msg.msg.pass.p_pass_data;
                av.remote_cmd.len = p_data->rc_msg.msg.pass.pass_len;
                memcpy(&av.remote_cmd.hdr, &p_data->rc_msg.msg.hdr, sizeof (tAVRC_HDR));
                av.remote_cmd.label = p_data->rc_msg.label;
            }
        }
        /* else if this is a pass thru respone that TG doesn't implement thie command */
        else if (p_data->rc_msg.msg.hdr.ctype == AVRC_RSP_NOT_IMPL) {
            /* do nothing, no need to setup for callback */
        }
        /* else if this is a pass thru response */
        else if (p_data->rc_msg.msg.hdr.ctype >= AVRC_RSP_ACCEPT) {
            /* set up for callback */
            evt = BTA_AV_REMOTE_RSP_EVT;
            av.remote_rsp.rc_id = p_data->rc_msg.msg.pass.op_id;
            av.remote_rsp.key_state = p_data->rc_msg.msg.pass.state;
            av.remote_rsp.rsp_code = p_data->rc_msg.msg.hdr.ctype;
            av.remote_rsp.label = p_data->rc_msg.label;
        }
        /* must be a bad ctype -> reject*/
        else {
            p_data->rc_msg.msg.hdr.ctype = BTA_AV_RSP_REJ;
            AVRC_PassRsp(p_data->rc_msg.handle, p_data->rc_msg.label, &p_data->rc_msg.msg.pass);
        }
    }
    /* else if this is a vendor specific command or response */
    else if (p_data->rc_msg.opcode == AVRC_OP_VENDOR) {
        /* set up for callback */
        av.vendor_cmd.code = p_data->rc_msg.msg.hdr.ctype;
        av.vendor_cmd.company_id = p_vendor->company_id;
        av.vendor_cmd.label = p_data->rc_msg.label;
        av.vendor_cmd.p_data = p_vendor->p_vendor_data;
        av.vendor_cmd.len = p_vendor->vendor_len;

        /* if configured to support vendor specific and it's a command */
        if ((p_cb->features & BTA_AV_FEAT_VENDOR)  &&
                p_data->rc_msg.msg.hdr.ctype <= AVRC_CMD_GEN_INQ) {
#if (AVRC_METADATA_INCLUDED == TRUE)
            if ((p_cb->features & BTA_AV_FEAT_METADATA) &&
                    (p_vendor->company_id == AVRC_CO_METADATA)) {
                av.meta_msg.p_msg = &p_data->rc_msg.msg;
                evt = bta_av_proc_meta_cmd (&rc_rsp, &p_data->rc_msg, &ctype);
            } else
#endif
            {
                evt = BTA_AV_VENDOR_CMD_EVT;
            }
        }
        /* else if configured to support vendor specific and it's a response */
        else if ((p_cb->features & BTA_AV_FEAT_VENDOR) &&
                 p_data->rc_msg.msg.hdr.ctype >= AVRC_RSP_ACCEPT) {
#if (AVRC_METADATA_INCLUDED == TRUE)
            if ((p_cb->features & BTA_AV_FEAT_METADATA) &&
                    (p_vendor->company_id == AVRC_CO_METADATA)) {
                av.meta_msg.p_msg = &p_data->rc_msg.msg;
                evt = BTA_AV_META_MSG_EVT;
            } else
#endif
            {
                evt = BTA_AV_VENDOR_RSP_EVT;
            }

        }
        /* else if not configured to support vendor specific and it's a command */
        else if (!(p_cb->features & BTA_AV_FEAT_VENDOR)  &&
                 p_data->rc_msg.msg.hdr.ctype <= AVRC_CMD_GEN_INQ) {
            if (p_data->rc_msg.msg.vendor.p_vendor_data[0] == AVRC_PDU_INVALID) {
                /* reject it */
                p_data->rc_msg.msg.hdr.ctype = BTA_AV_RSP_REJ;
                p_data->rc_msg.msg.vendor.p_vendor_data[4] = AVRC_STS_BAD_CMD;
            } else {
                p_data->rc_msg.msg.hdr.ctype = BTA_AV_RSP_NOT_IMPL;
            }
            AVRC_VendorRsp(p_data->rc_msg.handle, p_data->rc_msg.label, &p_data->rc_msg.msg.vendor);
        }
    }
#if (AVRC_METADATA_INCLUDED == TRUE)
    if (evt == 0 && rc_rsp.rsp.status != BTA_AV_STS_NO_RSP) {
        if (!p_pkt) {
            rc_rsp.rsp.opcode = p_data->rc_msg.opcode;
            AVRC_BldResponse (0, &rc_rsp, &p_pkt);
        }
        if (p_pkt) {
            AVRC_MsgReq (p_data->rc_msg.handle, p_data->rc_msg.label, ctype, p_pkt);
        }
    }
#endif

    /* call callback */
    if (evt != 0) {
        av.remote_cmd.rc_handle = p_data->rc_msg.handle;
        (*p_cb->p_cback)(evt, &av);
    }
}

/*******************************************************************************
**
** Function         bta_av_rc_close
**
** Description      close the specified AVRC handle.
**
** Returns          void
**
*******************************************************************************/
void bta_av_rc_close (tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data)
{
    UINT16 handle = p_data->hdr.layer_specific;
    tBTA_AV_SCB  *p_scb;
    tBTA_AV_RCB *p_rcb;

    if (handle < BTA_AV_NUM_RCB) {
        p_rcb = &p_cb->rcb[handle];

        APPL_TRACE_DEBUG("bta_av_rc_close handle: %d, status=0x%x", p_rcb->handle, p_rcb->status);
        if (p_rcb->handle != BTA_AV_RC_HANDLE_NONE) {
            if (p_rcb->shdl) {
                p_scb = bta_av_cb.p_scb[p_rcb->shdl - 1];
                if (p_scb) {
                    /* just in case the RC timer is active
                    if(bta_av_cb.features & BTA_AV_FEAT_RCCT &&
                       p_scb->chnl == BTA_AV_CHNL_AUDIO) */
                    bta_sys_stop_timer(&p_scb->timer);
                }
            }

            AVRC_Close(p_rcb->handle);
        }
    }
}

/*******************************************************************************
**
** Function         bta_av_get_shdl
**
** Returns          The index to p_scb[]
**
*******************************************************************************/
static UINT8 bta_av_get_shdl(tBTA_AV_SCB *p_scb)
{
    int     i;
    UINT8   shdl = 0;
    /* find the SCB & stop the timer */
    for (i = 0; i < BTA_AV_NUM_STRS; i++) {
        if (p_scb == bta_av_cb.p_scb[i]) {
            shdl = i + 1;
            break;
        }
    }
    return shdl;
}

/*******************************************************************************
**
** Function         bta_av_stream_chg
**
** Description      audio streaming status changed.
**
** Returns          void
**
*******************************************************************************/
void bta_av_stream_chg(tBTA_AV_SCB *p_scb, BOOLEAN started)
{
    UINT8   started_msk;
    int     i;
    UINT8   *p_streams;
    BOOLEAN no_streams = FALSE;
    tBTA_AV_SCB *p_scbi;

    started_msk = BTA_AV_HNDL_TO_MSK(p_scb->hdi);
    APPL_TRACE_DEBUG ("bta_av_stream_chg started:%d started_msk:x%x chnl:x%x", started,
                      started_msk, p_scb->chnl);
    if (BTA_AV_CHNL_AUDIO == p_scb->chnl) {
        p_streams = &bta_av_cb.audio_streams;
    } else {
        p_streams = &bta_av_cb.video_streams;
    }

    if (started) {
        /* Let L2CAP know this channel is processed with high priority */
        L2CA_SetAclPriority(p_scb->peer_addr, L2CAP_PRIORITY_HIGH);
        (*p_streams) |= started_msk;
    } else {
        (*p_streams) &= ~started_msk;
    }

    if (!started) {
        i = 0;
        if (BTA_AV_CHNL_AUDIO == p_scb->chnl) {
            if (bta_av_cb.video_streams == 0) {
                no_streams = TRUE;
            }
        } else {
            no_streams = TRUE;
            if ( bta_av_cb.audio_streams ) {
                for (; i < BTA_AV_NUM_STRS; i++) {
                    p_scbi = bta_av_cb.p_scb[i];
                    /* scb is used and started */
                    if ( p_scbi && (bta_av_cb.audio_streams & BTA_AV_HNDL_TO_MSK(i))
                            && bdcmp(p_scbi->peer_addr, p_scb->peer_addr) == 0) {
                        no_streams = FALSE;
                        break;
                    }
                }

            }
        }

        APPL_TRACE_DEBUG ("no_streams:%d i:%d, audio_streams:x%x, video_streams:x%x", no_streams, i,
                          bta_av_cb.audio_streams, bta_av_cb.video_streams);
        if (no_streams) {
            /* Let L2CAP know this channel is processed with low priority */
            L2CA_SetAclPriority(p_scb->peer_addr, L2CAP_PRIORITY_NORMAL);
        }
    }
}


/*******************************************************************************
**
** Function         bta_av_conn_chg
**
** Description      connection status changed.
**                  Open an AVRCP acceptor channel, if new conn.
**
** Returns          void
**
*******************************************************************************/
void bta_av_conn_chg(tBTA_AV_DATA *p_data)
{
    tBTA_AV_CB   *p_cb = &bta_av_cb;
    tBTA_AV_SCB     *p_scb = NULL;
    tBTA_AV_SCB     *p_scbi;
    UINT8   mask;
    UINT8   conn_msk;
    UINT8   old_msk;
    int i;
    int index = (p_data->hdr.layer_specific & BTA_AV_HNDL_MSK) - 1;
    tBTA_AV_LCB *p_lcb;
    tBTA_AV_LCB *p_lcb_rc;
    tBTA_AV_RCB *p_rcb, *p_rcb2;
    BOOLEAN     chk_restore = FALSE;

    /* Validate array index*/
    if (index < BTA_AV_NUM_STRS) {
        p_scb = p_cb->p_scb[index];
    }
    mask = BTA_AV_HNDL_TO_MSK(index);
    p_lcb = bta_av_find_lcb(p_data->conn_chg.peer_addr, BTA_AV_LCB_FIND);
    conn_msk = 1 << (index + 1);
    if (p_data->conn_chg.is_up) {
        /* set the conned mask for this channel */
        if (p_scb) {
            if (p_lcb) {
                p_lcb->conn_msk |= conn_msk;
                for (i = 0; i < BTA_AV_NUM_RCB; i++) {
                    if (bta_av_cb.rcb[i].lidx == p_lcb->lidx) {
                        bta_av_cb.rcb[i].shdl = index + 1;
                        APPL_TRACE_DEBUG("conn_chg up[%d]: %d, status=0x%x, shdl:%d, lidx:%d", i,
                                         bta_av_cb.rcb[i].handle, bta_av_cb.rcb[i].status,
                                         bta_av_cb.rcb[i].shdl, bta_av_cb.rcb[i].lidx);
                        break;
                    }
                }
            }
            if (p_scb->chnl == BTA_AV_CHNL_AUDIO) {
                old_msk = p_cb->conn_audio;
                p_cb->conn_audio |= mask;
            } else {
                old_msk = p_cb->conn_video;
                p_cb->conn_video |= mask;
            }

            if ((old_msk & mask) == 0) {
                /* increase the audio open count, if not set yet */
                bta_av_cb.audio_open_cnt++;
            }


            APPL_TRACE_DEBUG("rc_acp_handle:%d rc_acp_idx:%d", p_cb->rc_acp_handle, p_cb->rc_acp_idx);
            /* check if the AVRCP ACP channel is already connected */
            if (p_lcb && p_cb->rc_acp_handle != BTA_AV_RC_HANDLE_NONE && p_cb->rc_acp_idx) {
                p_lcb_rc = &p_cb->lcb[BTA_AV_NUM_LINKS];
                APPL_TRACE_DEBUG("rc_acp is connected && conn_chg on same addr p_lcb_rc->conn_msk:x%x",
                                 p_lcb_rc->conn_msk);
                /* check if the RC is connected to the scb addr */
                APPL_TRACE_DEBUG ("p_lcb_rc->addr: %02x:%02x:%02x:%02x:%02x:%02x",
                                  p_lcb_rc->addr[0], p_lcb_rc->addr[1], p_lcb_rc->addr[2], p_lcb_rc->addr[3],
                                  p_lcb_rc->addr[4], p_lcb_rc->addr[5]);
                APPL_TRACE_DEBUG ("conn_chg.peer_addr: %02x:%02x:%02x:%02x:%02x:%02x",
                                  p_data->conn_chg.peer_addr[0], p_data->conn_chg.peer_addr[1],
                                  p_data->conn_chg.peer_addr[2],
                                  p_data->conn_chg.peer_addr[3], p_data->conn_chg.peer_addr[4],
                                  p_data->conn_chg.peer_addr[5]);
                if (p_lcb_rc->conn_msk && bdcmp(p_lcb_rc->addr, p_data->conn_chg.peer_addr) == 0) {
                    /* AVRCP is already connected.
                     * need to update the association betwen SCB and RCB */
                    p_lcb_rc->conn_msk = 0; /* indicate RC ONLY is not connected */
                    p_lcb_rc->lidx = 0;
                    p_scb->rc_handle = p_cb->rc_acp_handle;
                    p_rcb = &p_cb->rcb[p_cb->rc_acp_idx - 1];
                    p_rcb->shdl = bta_av_get_shdl(p_scb);
                    APPL_TRACE_DEBUG("update rc_acp shdl:%d/%d srch:%d", index + 1, p_rcb->shdl,
                                     p_scb->rc_handle );

                    p_rcb2 = bta_av_get_rcb_by_shdl(p_rcb->shdl);
                    if (p_rcb2) {
                        /* found the RCB that was created to associated with this SCB */
                        p_cb->rc_acp_handle = p_rcb2->handle;
                        p_cb->rc_acp_idx = (p_rcb2 - p_cb->rcb) + 1;
                        APPL_TRACE_DEBUG("new rc_acp_handle:%d, idx:%d", p_cb->rc_acp_handle,
                                         p_cb->rc_acp_idx);
                        p_rcb2->lidx = (BTA_AV_NUM_LINKS + 1);
                        APPL_TRACE_DEBUG("rc2 handle:%d lidx:%d/%d", p_rcb2->handle, p_rcb2->lidx,
                                         p_cb->lcb[p_rcb2->lidx - 1].lidx);
                    }
                    p_rcb->lidx = p_lcb->lidx;
                    APPL_TRACE_DEBUG("rc handle:%d lidx:%d/%d", p_rcb->handle, p_rcb->lidx,
                                     p_cb->lcb[p_rcb->lidx - 1].lidx);
                }
            }
        }
    } else {
        if ((p_cb->conn_audio & mask) && bta_av_cb.audio_open_cnt) {
            /* this channel is still marked as open. decrease the count */
            bta_av_cb.audio_open_cnt--;
        }

        /* clear the conned mask for this channel */
        p_cb->conn_audio &= ~mask;
        p_cb->conn_video &= ~mask;
        if (p_scb) {
            /* the stream is closed.
             * clear the peer address, so it would not mess up the AVRCP for the next round of operation */
            bdcpy(p_scb->peer_addr, bd_addr_null);
            if (p_scb->chnl == BTA_AV_CHNL_AUDIO) {
                if (p_lcb) {
                    p_lcb->conn_msk &= ~conn_msk;
                }
                /* audio channel is down. make sure the INT channel is down */
                /* just in case the RC timer is active
                if(p_cb->features & BTA_AV_FEAT_RCCT) */
                {
                    bta_sys_stop_timer(&p_scb->timer);
                }
                /* one audio channel goes down. check if we need to restore high priority */
                chk_restore = TRUE;
            }
        }

        APPL_TRACE_DEBUG("bta_av_conn_chg shdl:%d", index + 1);
        for (i = 0; i < BTA_AV_NUM_RCB; i++) {
            APPL_TRACE_DEBUG("conn_chg dn[%d]: %d, status=0x%x, shdl:%d, lidx:%d", i,
                             bta_av_cb.rcb[i].handle, bta_av_cb.rcb[i].status,
                             bta_av_cb.rcb[i].shdl, bta_av_cb.rcb[i].lidx);
            if (bta_av_cb.rcb[i].shdl == index + 1) {
                bta_av_del_rc(&bta_av_cb.rcb[i]);
                break;
            }
        }

        if (p_cb->conn_audio == 0 && p_cb->conn_video == 0) {
            /* if both channels are not connected,
             * close all RC channels */
            bta_av_close_all_rc(p_cb);
        }

        /* if the AVRCP is no longer listening, create the listening channel */
        if (bta_av_cb.rc_acp_handle == BTA_AV_RC_HANDLE_NONE && bta_av_cb.features & BTA_AV_FEAT_RCTG) {
            bta_av_rc_create(&bta_av_cb, AVCT_ACP, 0, BTA_AV_NUM_LINKS + 1);
        }
    }

    APPL_TRACE_DEBUG("bta_av_conn_chg audio:%x video:%x up:%d conn_msk:0x%x chk_restore:%d audio_open_cnt:%d",
                     p_cb->conn_audio, p_cb->conn_video, p_data->conn_chg.is_up, conn_msk, chk_restore, p_cb->audio_open_cnt);

    if (chk_restore) {
        if (p_cb->audio_open_cnt == 1) {
            /* one audio channel goes down and there's one audio channel remains open.
             * restore the switch role in default link policy */
            bta_sys_set_default_policy(BTA_ID_AV, HCI_ENABLE_MASTER_SLAVE_SWITCH);
            /* allow role switch, if this is the last connection */
            bta_av_restore_switch();
        }
        if (p_cb->audio_open_cnt) {
            /* adjust flush timeout settings to longer period */
            for (i = 0; i < BTA_AV_NUM_STRS; i++) {
                p_scbi = bta_av_cb.p_scb[i];
                if (p_scbi && p_scbi->chnl == BTA_AV_CHNL_AUDIO && p_scbi->co_started) {
                    /* may need to update the flush timeout of this already started stream */
                    if (p_scbi->co_started != bta_av_cb.audio_open_cnt) {
                        p_scbi->co_started = bta_av_cb.audio_open_cnt;
                        L2CA_SetFlushTimeout(p_scbi->peer_addr, p_bta_av_cfg->p_audio_flush_to[p_scbi->co_started - 1] );
                    }
                }
            }
        }
    }
}

/*******************************************************************************
**
** Function         bta_av_disable
**
** Description      disable AV.
**
** Returns          void
**
*******************************************************************************/
void bta_av_disable(tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data)
{
    BT_HDR  hdr;
    UINT16  xx;
    UNUSED(p_data);

    p_cb->disabling = TRUE;

    bta_av_close_all_rc(p_cb);

    utl_freebuf((void **) &p_cb->p_disc_db);

    /* disable audio/video - de-register all channels,
     * expect BTA_AV_DEREG_COMP_EVT when deregister is complete */
    for (xx = 0; xx < BTA_AV_NUM_STRS; xx++) {
        hdr.layer_specific = xx + 1;
        bta_av_api_deregister((tBTA_AV_DATA *)&hdr);
    }

    bta_sys_free_timer(&p_cb->sig_tmr);
    memset(&p_cb->sig_tmr, 0, sizeof(TIMER_LIST_ENT));
    bta_sys_free_timer(&p_cb->acp_sig_tmr);
    memset(&p_cb->acp_sig_tmr, 0, sizeof(TIMER_LIST_ENT));
}

/*******************************************************************************
**
** Function         bta_av_api_disconnect
**
** Description      .
**
** Returns          void
**
*******************************************************************************/
void bta_av_api_disconnect(tBTA_AV_DATA *p_data)
{
    AVDT_DisconnectReq(p_data->api_discnt.bd_addr, bta_av_conn_cback);
    bta_sys_stop_timer(&bta_av_cb.sig_tmr);
}

/*******************************************************************************
**
** Function         bta_av_sig_chg
**
** Description      process AVDT signal channel up/down.
**
** Returns          void
**
*******************************************************************************/
void bta_av_sig_chg(tBTA_AV_DATA *p_data)
{
    UINT16 event = p_data->str_msg.hdr.layer_specific;
    tBTA_AV_CB   *p_cb = &bta_av_cb;
    int     xx;
    UINT8   mask;
    tBTA_AV_LCB *p_lcb = NULL;

    APPL_TRACE_DEBUG("bta_av_sig_chg event: %d", event);
    if (event == AVDT_CONNECT_IND_EVT) {
        p_lcb = bta_av_find_lcb(p_data->str_msg.bd_addr, BTA_AV_LCB_FIND);
        if (!p_lcb) {
            /* if the address does not have an LCB yet, alloc one */
            for (xx = 0; xx < BTA_AV_NUM_LINKS; xx++) {
                mask = 1 << xx;
                APPL_TRACE_DEBUG("conn_lcb: 0x%x", p_cb->conn_lcb);

                /* look for a p_lcb with its p_scb registered */
                if ((!(mask & p_cb->conn_lcb)) && (p_cb->p_scb[xx] != NULL)) {
                    p_lcb = &p_cb->lcb[xx];
                    p_lcb->lidx = xx + 1;
                    bdcpy(p_lcb->addr, p_data->str_msg.bd_addr);
                    p_lcb->conn_msk = 0; /* clear the connect mask */
                    /* start listening when the signal channel is open */
                    if (p_cb->features & BTA_AV_FEAT_RCTG) {
                        bta_av_rc_create(p_cb, AVCT_ACP, 0, p_lcb->lidx);
                    }
                    /* this entry is not used yet. */
                    p_cb->conn_lcb |= mask;     /* mark it as used */
                    APPL_TRACE_DEBUG("start sig timer %d", p_data->hdr.offset);
                    if (p_data->hdr.offset == AVDT_ACP) {
                        APPL_TRACE_DEBUG("Incoming L2CAP acquired, set state as incoming");
                        bdcpy(p_cb->p_scb[xx]->peer_addr, p_data->str_msg.bd_addr);
                        p_cb->p_scb[xx]->use_rc = TRUE;     /* allowing RC for incoming connection */
                        bta_av_ssm_execute(p_cb->p_scb[xx], BTA_AV_ACP_CONNECT_EVT, p_data);

                        /* The Pending Event should be sent as soon as the L2CAP signalling channel
                         * is set up, which is NOW. Earlier this was done only after
                         * BTA_AV_SIG_TIME_VAL milliseconds.
                         * The following function shall send the event and start the recurring timer
                         */
                        bta_av_sig_timer(NULL);

                        /* Possible collision : need to avoid outgoing processing while the timer is running */
                        p_cb->p_scb[xx]->coll_mask = BTA_AV_COLL_INC_TMR;

                        p_cb->acp_sig_tmr.param = (UINT32)xx;
                        p_cb->acp_sig_tmr.p_cback = (TIMER_CBACK *)&bta_av_acp_sig_timer_cback;
                        bta_sys_start_timer(&p_cb->acp_sig_tmr, 0, BTA_AV_ACP_SIG_TIME_VAL);
                    }
                    break;
                }
            }

            /* check if we found something */
            if (xx == BTA_AV_NUM_LINKS) {
                /* We do not have scb for this avdt connection.     */
                /* Silently close the connection.                   */
                APPL_TRACE_ERROR("av scb not available for avdt connection");
                AVDT_DisconnectReq (p_data->str_msg.bd_addr, NULL);
                return;
            }
        }
    }
#if( defined BTA_AR_INCLUDED ) && (BTA_AR_INCLUDED == TRUE)
    else if (event == BTA_AR_AVDT_CONN_EVT) {
        bta_sys_stop_timer(&bta_av_cb.sig_tmr);
    }
#endif
    else {
        /* disconnected. */
        p_lcb = bta_av_find_lcb(p_data->str_msg.bd_addr, BTA_AV_LCB_FREE);
        if (p_lcb && p_lcb->conn_msk) {
            APPL_TRACE_DEBUG("conn_msk: 0x%x", p_lcb->conn_msk);
            /* clean up ssm  */
            for (xx = 0; xx < BTA_AV_NUM_STRS; xx++) {
                mask = 1 << (xx + 1);
                if ((mask & p_lcb->conn_msk) && (p_cb->p_scb[xx]) &&
                        (bdcmp(p_cb->p_scb[xx]->peer_addr, p_data->str_msg.bd_addr) == 0)) {
                    p_cb->p_scb[xx]->disc_rsn = p_data->str_msg.hdr.offset;
                    bta_av_ssm_execute(p_cb->p_scb[xx], BTA_AV_AVDT_DISCONNECT_EVT, NULL);
                }
            }
        }
    }
    APPL_TRACE_DEBUG("conn_lcb: 0x%x", p_cb->conn_lcb);
}

/*******************************************************************************
**
** Function         bta_av_sig_timer
**
** Description      process the signal channel timer. This timer is started
**                  when the AVDTP signal channel is connected. If no profile
**                  is connected, the timer goes off every BTA_AV_SIG_TIME_VAL
**
** Returns          void
**
*******************************************************************************/
void bta_av_sig_timer(tBTA_AV_DATA *p_data)
{
    tBTA_AV_CB   *p_cb = &bta_av_cb;
    int     xx;
    UINT8   mask;
    tBTA_AV_LCB *p_lcb = NULL;
    tBTA_AV_PEND pend;
    UNUSED(p_data);

    APPL_TRACE_DEBUG("bta_av_sig_timer");
    for (xx = 0; xx < BTA_AV_NUM_LINKS; xx++) {
        mask = 1 << xx;
        if (mask & p_cb->conn_lcb) {
            /* this entry is used. check if it is connected */
            p_lcb = &p_cb->lcb[xx];
            if (!p_lcb->conn_msk) {
                bta_sys_start_timer(&p_cb->sig_tmr, BTA_AV_SIG_TIMER_EVT, BTA_AV_SIG_TIME_VAL);
                bdcpy(pend.bd_addr, p_lcb->addr);
                (*p_cb->p_cback)(BTA_AV_PENDING_EVT, (tBTA_AV *) &pend);
            }
        }
    }
}

/*******************************************************************************
**
** Function         bta_av_acp_sig_timer_cback
**
** Description      Process the timeout when SRC is accepting connection
**                  and SNK did not start signalling.
**
** Returns          void
**
*******************************************************************************/
static void bta_av_acp_sig_timer_cback (TIMER_LIST_ENT *p_tle)
{
    UINT8   inx = (UINT8)p_tle->param;
    tBTA_AV_CB  *p_cb = &bta_av_cb;
    tBTA_AV_SCB *p_scb = NULL;
    tBTA_AV_API_OPEN  *p_buf;
    if (inx < BTA_AV_NUM_STRS) {
        p_scb = p_cb->p_scb[inx];
    }
    if (p_scb) {
        APPL_TRACE_DEBUG("bta_av_acp_sig_timer_cback, coll_mask = 0x%02X", p_scb->coll_mask);

        if (p_scb->coll_mask & BTA_AV_COLL_INC_TMR) {
            p_scb->coll_mask &= ~BTA_AV_COLL_INC_TMR;

            if (bta_av_is_scb_opening(p_scb)) {
                if (p_scb->p_disc_db) {
                    /* We are still doing SDP. Run the timer again. */
                    p_scb->coll_mask |= BTA_AV_COLL_INC_TMR;

                    p_cb->acp_sig_tmr.param = (UINT32)inx;
                    p_cb->acp_sig_tmr.p_cback = (TIMER_CBACK *)&bta_av_acp_sig_timer_cback;
                    bta_sys_start_timer(&p_cb->acp_sig_tmr, 0, BTA_AV_ACP_SIG_TIME_VAL);
                } else {
                    /* SNK did not start signalling, resume signalling process. */
                    bta_av_discover_req (p_scb, NULL);
                }
            } else if (bta_av_is_scb_incoming(p_scb)) {
                /* Stay in incoming state if SNK does not start signalling */

                /* API open was called right after SNK opened L2C connection. */
                if (p_scb->coll_mask & BTA_AV_COLL_API_CALLED) {
                    p_scb->coll_mask &= ~BTA_AV_COLL_API_CALLED;

                    /* BTA_AV_API_OPEN_EVT */
                    if ((p_buf = (tBTA_AV_API_OPEN *) osi_malloc(sizeof(tBTA_AV_API_OPEN))) != NULL) {
                        memcpy(p_buf, &(p_scb->open_api), sizeof(tBTA_AV_API_OPEN));
                        bta_sys_sendmsg(p_buf);
                    }
                }
            }
        }
    }
}

/*******************************************************************************
**
** Function         bta_av_check_peer_rc_features
**
** Description      check supported AVRC features on the peer device from the SDP
**                  record and return the feature mask
**
** Returns          tBTA_AV_FEAT peer device feature mask
**
*******************************************************************************/
tBTA_AV_FEAT bta_av_check_peer_rc_features (UINT16 service_uuid, UINT16 *rc_features)
{
    tBTA_AV_FEAT peer_features = 0;
    tBTA_AV_CB   *p_cb = &bta_av_cb;
    tSDP_DISC_REC       *p_rec = NULL;
    tSDP_DISC_ATTR      *p_attr;
    UINT16              peer_rc_version = 0;
    UINT16              categories = 0;

    APPL_TRACE_DEBUG("bta_av_check_peer_rc features service_uuid:x%x", service_uuid);
    /* loop through all records we found */
    while (TRUE) {
        /* get next record; if none found, we're done */
        if ((p_rec = SDP_FindServiceInDb(p_cb->p_disc_db, service_uuid, p_rec)) == NULL) {
            break;
        }

        if (( SDP_FindAttributeInRec(p_rec, ATTR_ID_SERVICE_CLASS_ID_LIST)) != NULL) {
            /* find peer features */
            if (SDP_FindServiceInDb(p_cb->p_disc_db, UUID_SERVCLASS_AV_REMOTE_CONTROL, NULL)) {
                peer_features |= BTA_AV_FEAT_RCCT;
            }
            if (SDP_FindServiceInDb(p_cb->p_disc_db, UUID_SERVCLASS_AV_REM_CTRL_TARGET, NULL)) {
                peer_features |= BTA_AV_FEAT_RCTG;
            }
        }

        if (( SDP_FindAttributeInRec(p_rec, ATTR_ID_BT_PROFILE_DESC_LIST)) != NULL) {
            /* get profile version (if failure, version parameter is not updated) */
            SDP_FindProfileVersionInRec(p_rec, UUID_SERVCLASS_AV_REMOTE_CONTROL, &peer_rc_version);
            APPL_TRACE_DEBUG("peer_rc_version 0x%x", peer_rc_version);

            if (peer_rc_version >= AVRC_REV_1_3) {
                peer_features |= (BTA_AV_FEAT_VENDOR | BTA_AV_FEAT_METADATA);
            }

            if (peer_rc_version >= AVRC_REV_1_4) {
                peer_features |= (BTA_AV_FEAT_ADV_CTRL);
                /* get supported categories */
                if ((p_attr = SDP_FindAttributeInRec(p_rec,
                                                     ATTR_ID_SUPPORTED_FEATURES)) != NULL) {
                    categories = p_attr->attr_value.v.u16;
                    if (categories & AVRC_SUPF_CT_BROWSE) {
                        peer_features |= (BTA_AV_FEAT_BROWSE);
                    }
                }
            }
        }
    }

    if (rc_features) {
        *rc_features = categories;
    }

    APPL_TRACE_DEBUG("peer_features:x%x, rc:x%x", peer_features, categories);
    return peer_features;
}

/*******************************************************************************
**
** Function         bta_av_rc_disc_done
**
** Description      Handle AVRCP service discovery results.  If matching
**                  service found, open AVRCP connection.
**
** Returns          void
**
*******************************************************************************/
void bta_av_rc_disc_done(tBTA_AV_DATA *p_data)
{
    tBTA_AV_CB   *p_cb = &bta_av_cb;
    tBTA_AV_SCB  *p_scb = NULL;
    tBTA_AV_LCB  *p_lcb;
    tBTA_AV_RC_OPEN rc_open;
    tBTA_AV_RC_FEAT rc_feat;
    UINT8               rc_handle;
    tBTA_AV_FEAT        peer_features;  /* peer features mask */
    UINT16              peer_ct_features;  /* peer features mask as controller */
    UINT16              peer_tg_features;  /* peer features mask as target */
    UNUSED(p_data);

    APPL_TRACE_DEBUG("bta_av_rc_disc_done disc:x%x", p_cb->disc);
    if (!p_cb->disc) {
        return;
    }

    if ((p_cb->disc & BTA_AV_CHNL_MSK) == BTA_AV_CHNL_MSK) {
        /* this is the rc handle/index to tBTA_AV_RCB */
        rc_handle = p_cb->disc & (~BTA_AV_CHNL_MSK);
    } else {
        /* Validate array index*/
        if (((p_cb->disc & BTA_AV_HNDL_MSK) - 1) < BTA_AV_NUM_STRS) {
            p_scb = p_cb->p_scb[(p_cb->disc & BTA_AV_HNDL_MSK) - 1];
        }
        if (p_scb) {
            rc_handle = p_scb->rc_handle;
        } else {
            p_cb->disc = 0;
            return;
        }
    }

    APPL_TRACE_DEBUG("rc_handle %d", rc_handle);
    /* check peer version and whether support CT and TG role */
    peer_features = bta_av_check_peer_rc_features (UUID_SERVCLASS_AV_REMOTE_CONTROL, &peer_ct_features);
    peer_features |= bta_av_check_peer_rc_features (UUID_SERVCLASS_AV_REM_CTRL_TARGET, &peer_tg_features);

    p_cb->disc = 0;
    utl_freebuf((void **) &p_cb->p_disc_db);

    APPL_TRACE_DEBUG("peer_features 0x%x, local features 0x%x", peer_features, p_cb->features);

    /* if we have no rc connection */
    if (rc_handle == BTA_AV_RC_HANDLE_NONE) {
        if (p_scb) {
            /* if peer remote control service matches ours and USE_RC is TRUE */
            if ((((p_cb->features & BTA_AV_FEAT_RCCT) && (peer_features & BTA_AV_FEAT_RCTG)) ||
                    ((p_cb->features & BTA_AV_FEAT_RCTG) && (peer_features & BTA_AV_FEAT_RCCT))) ) {
                p_lcb = bta_av_find_lcb(p_scb->peer_addr, BTA_AV_LCB_FIND);
                if (p_lcb) {
                    rc_handle = bta_av_rc_create(p_cb, AVCT_INT, (UINT8)(p_scb->hdi + 1), p_lcb->lidx);
                    p_cb->rcb[rc_handle].peer_features = peer_features;
                    p_cb->rcb[rc_handle].peer_ct_features = peer_ct_features;
                    p_cb->rcb[rc_handle].peer_tg_features = peer_tg_features;
                }
#if (BT_USE_TRACES == TRUE || BT_TRACE_APPL == TRUE)
                else {
                    APPL_TRACE_ERROR("can not find LCB!!");
                }
#endif
            } else if (p_scb->use_rc) {
                /* can not find AVRC on peer device. report failure */
                p_scb->use_rc = FALSE;
                bdcpy(rc_open.peer_addr, p_scb->peer_addr);
                rc_open.peer_features = 0;
                rc_open.sdp_disc_done = FALSE;
                rc_open.status = BTA_AV_FAIL_SDP;
                (*p_cb->p_cback)(BTA_AV_RC_OPEN_EVT, (tBTA_AV *) &rc_open);
            }
        }
    } else {
        p_cb->rcb[rc_handle].peer_features = peer_features;
        rc_feat.rc_handle =  rc_handle;
        rc_feat.peer_features = peer_features;
        rc_feat.peer_ct_features = peer_ct_features;
        rc_feat.peer_tg_features = peer_tg_features;
        (*p_cb->p_cback)(BTA_AV_RC_FEAT_EVT, (tBTA_AV *) &rc_feat);
    }
}

/*******************************************************************************
**
** Function         bta_av_rc_closed
**
** Description      Set AVRCP state to closed.
**
** Returns          void
**
*******************************************************************************/
void bta_av_rc_closed(tBTA_AV_DATA *p_data)
{
    tBTA_AV_CB   *p_cb = &bta_av_cb;
    tBTA_AV_RC_CLOSE rc_close;
    tBTA_AV_RC_CONN_CHG *p_msg = (tBTA_AV_RC_CONN_CHG *)p_data;
    tBTA_AV_RCB    *p_rcb;
    tBTA_AV_SCB    *p_scb;
    int i;
    BOOLEAN conn = FALSE;
    tBTA_AV_LCB *p_lcb;

    rc_close.rc_handle = BTA_AV_RC_HANDLE_NONE;
    p_scb = NULL;
    APPL_TRACE_DEBUG("bta_av_rc_closed rc_handle:%d", p_msg->handle);
    for (i = 0; i < BTA_AV_NUM_RCB; i++) {
        p_rcb = &p_cb->rcb[i];
        APPL_TRACE_DEBUG("bta_av_rc_closed rcb[%d] rc_handle:%d, status=0x%x", i, p_rcb->handle, p_rcb->status);
        if (p_rcb->handle == p_msg->handle) {
            rc_close.rc_handle = i;
            p_rcb->status &= ~BTA_AV_RC_CONN_MASK;
            p_rcb->peer_features = 0;
            p_rcb->peer_ct_features = 0;
            p_rcb->peer_tg_features = 0;
            APPL_TRACE_DEBUG("       shdl:%d, lidx:%d", p_rcb->shdl, p_rcb->lidx);
            if (p_rcb->shdl) {
                if ((p_rcb->shdl - 1) < BTA_AV_NUM_STRS) {
                    p_scb = bta_av_cb.p_scb[p_rcb->shdl - 1];
                }
                if (p_scb) {
                    bdcpy(rc_close.peer_addr, p_scb->peer_addr);
                    if (p_scb->rc_handle == p_rcb->handle) {
                        p_scb->rc_handle = BTA_AV_RC_HANDLE_NONE;
                    }
                    APPL_TRACE_DEBUG("shdl:%d, srch:%d", p_rcb->shdl, p_scb->rc_handle);
                }
                p_rcb->shdl = 0;
            } else if (p_rcb->lidx == (BTA_AV_NUM_LINKS + 1) ) {
                /* if the RCB uses the extra LCB, use the addr for event and clean it */
                p_lcb = &p_cb->lcb[BTA_AV_NUM_LINKS];
                bdcpy(rc_close.peer_addr, p_msg->peer_addr);
                APPL_TRACE_DEBUG("rc_only closed bd_addr:%02x-%02x-%02x-%02x-%02x-%02x",
                                 p_msg->peer_addr[0], p_msg->peer_addr[1],
                                 p_msg->peer_addr[2], p_msg->peer_addr[3],
                                 p_msg->peer_addr[4], p_msg->peer_addr[5]);
                p_lcb->conn_msk = 0;
                p_lcb->lidx = 0;
            }
            p_rcb->lidx = 0;

            if ((p_rcb->status & BTA_AV_RC_ROLE_MASK) == BTA_AV_RC_ROLE_INT) {
                /* AVCT CCB is deallocated */
                p_rcb->handle = BTA_AV_RC_HANDLE_NONE;
                p_rcb->status = 0;
            } else {
                /* AVCT CCB is still there. dealloc */
                bta_av_del_rc(p_rcb);

                /* if the AVRCP is no longer listening, create the listening channel */
                if (bta_av_cb.rc_acp_handle == BTA_AV_RC_HANDLE_NONE && bta_av_cb.features & BTA_AV_FEAT_RCTG) {
                    bta_av_rc_create(&bta_av_cb, AVCT_ACP, 0, BTA_AV_NUM_LINKS + 1);
                }
            }
        } else if ((p_rcb->handle != BTA_AV_RC_HANDLE_NONE) && (p_rcb->status & BTA_AV_RC_CONN_MASK)) {
            /* at least one channel is still connected */
            conn = TRUE;
        }
    }

    if (!conn) {
        /* no AVRC channels are connected, go back to INIT state */
        bta_av_sm_execute(p_cb, BTA_AV_AVRC_NONE_EVT, NULL);
    }

    if (rc_close.rc_handle == BTA_AV_RC_HANDLE_NONE) {
        rc_close.rc_handle = p_msg->handle;
        bdcpy(rc_close.peer_addr, p_msg->peer_addr);
    }
    (*p_cb->p_cback)(BTA_AV_RC_CLOSE_EVT, (tBTA_AV *) &rc_close);
}

/*******************************************************************************
**
** Function         bta_av_rc_disc
**
** Description      start AVRC SDP discovery.
**
** Returns          void
**
*******************************************************************************/
void bta_av_rc_disc(UINT8 disc)
{
    tBTA_AV_CB   *p_cb = &bta_av_cb;
    tAVRC_SDP_DB_PARAMS db_params;
    UINT16              attr_list[] = {ATTR_ID_SERVICE_CLASS_ID_LIST,
                                       ATTR_ID_BT_PROFILE_DESC_LIST,
                                       ATTR_ID_SUPPORTED_FEATURES
                                      };
    UINT8       hdi;
    tBTA_AV_SCB *p_scb;
    UINT8       *p_addr = NULL;
    UINT8       rc_handle;

    APPL_TRACE_DEBUG("bta_av_rc_disc 0x%x, %d", disc, bta_av_cb.disc);
    if ((bta_av_cb.disc != 0) || (disc == 0)) {
        return;
    }

    if ((disc & BTA_AV_CHNL_MSK) == BTA_AV_CHNL_MSK) {
        /* this is the rc handle/index to tBTA_AV_RCB */
        rc_handle = disc & (~BTA_AV_CHNL_MSK);
        if (p_cb->rcb[rc_handle].lidx) {
            p_addr = p_cb->lcb[p_cb->rcb[rc_handle].lidx - 1].addr;
        }
    } else {
        hdi = (disc & BTA_AV_HNDL_MSK) - 1;
        p_scb = p_cb->p_scb[hdi];

        if (p_scb) {
            APPL_TRACE_DEBUG("rc_handle %d", p_scb->rc_handle);
            p_addr = p_scb->peer_addr;
        }
    }

    if (p_addr) {
        /* allocate discovery database */
        if (p_cb->p_disc_db == NULL) {
            p_cb->p_disc_db = (tSDP_DISCOVERY_DB *) osi_malloc(BTA_AV_DISC_BUF_SIZE);
        }

        if (p_cb->p_disc_db) {
            /* set up parameters */
            db_params.db_len = BTA_AV_DISC_BUF_SIZE;
            db_params.num_attr = 3;
            db_params.p_db = p_cb->p_disc_db;
            db_params.p_attrs = attr_list;

            /* searching for UUID_SERVCLASS_AV_REMOTE_CONTROL gets both TG and CT */
            if (AVRC_FindService(UUID_SERVCLASS_AV_REMOTE_CONTROL, p_addr, &db_params,
                                 bta_av_avrc_sdp_cback) == AVRC_SUCCESS) {
                p_cb->disc = disc;
                APPL_TRACE_DEBUG("disc %d", p_cb->disc);
            }
        }
    }
}

/*******************************************************************************
**
** Function         bta_av_dereg_comp
**
** Description      deregister complete. free the stream control block.
**
** Returns          void
**
*******************************************************************************/
void bta_av_dereg_comp(tBTA_AV_DATA *p_data)
{
    tBTA_AV_CB   *p_cb = &bta_av_cb;
    tBTA_AV_SCB  *p_scb;
    tBTA_UTL_COD    cod;
    UINT8   mask;
    BT_HDR  *p_buf;

    /* find the stream control block */
    p_scb = bta_av_hndl_to_scb(p_data->hdr.layer_specific);

    if (p_scb) {
        APPL_TRACE_DEBUG("deregistered %d(h%d)", p_scb->chnl, p_scb->hndl);
        mask = BTA_AV_HNDL_TO_MSK(p_scb->hdi);
        if (p_scb->chnl == BTA_AV_CHNL_AUDIO) {
            p_cb->reg_audio  &= ~mask;
            if ((p_cb->conn_audio & mask) && bta_av_cb.audio_open_cnt) {
                /* this channel is still marked as open. decrease the count */
                bta_av_cb.audio_open_cnt--;
            }
            p_cb->conn_audio &= ~mask;

            if (p_scb->q_tag == BTA_AV_Q_TAG_STREAM && p_scb->a2d_list) {
                /* make sure no buffers are in a2d_list */
                while (!list_is_empty(p_scb->a2d_list)) {
                    p_buf = (BT_HDR *)list_front(p_scb->a2d_list);
                    list_remove(p_scb->a2d_list, p_buf);
                    osi_free(p_buf);
                }
            }

            /* remove the A2DP SDP record, if no more audio stream is left */
            if (!p_cb->reg_audio) {
#if( defined BTA_AR_INCLUDED ) && (BTA_AR_INCLUDED == TRUE)
                bta_ar_dereg_avrc (UUID_SERVCLASS_AV_REMOTE_CONTROL, BTA_ID_AV);
#endif
                bta_av_del_sdp_rec(&p_cb->sdp_a2d_handle);
                bta_sys_remove_uuid(UUID_SERVCLASS_AUDIO_SOURCE);

#if (BTA_AV_SINK_INCLUDED == TRUE)
                bta_av_del_sdp_rec(&p_cb->sdp_a2d_snk_handle);
                bta_sys_remove_uuid(UUID_SERVCLASS_AUDIO_SINK);
#endif
            }
        } else {
            p_cb->reg_video  &= ~mask;
            /* make sure that this channel is not connected */
            p_cb->conn_video &= ~mask;
            /* remove the VDP SDP record, (only one video stream at most) */
            bta_av_del_sdp_rec(&p_cb->sdp_vdp_handle);
            bta_sys_remove_uuid(UUID_SERVCLASS_VIDEO_SOURCE);
        }

        /* make sure that the timer is not active */
        bta_sys_stop_timer(&p_scb->timer);
        list_free(p_scb->a2d_list);
        p_scb->a2d_list = NULL;
        utl_freebuf((void **)&p_cb->p_scb[p_scb->hdi]);
    }

    APPL_TRACE_DEBUG("audio 0x%x, video: 0x%x, disable:%d",
                     p_cb->reg_audio, p_cb->reg_video, p_cb->disabling);
    /* if no stream control block is active */
    if ((p_cb->reg_audio + p_cb->reg_video) == 0) {
#if( defined BTA_AR_INCLUDED ) && (BTA_AR_INCLUDED == TRUE)
        /* deregister from AVDT */
        bta_ar_dereg_avdt(BTA_ID_AV);

        /* deregister from AVCT */
        bta_ar_dereg_avrc (UUID_SERVCLASS_AV_REM_CTRL_TARGET, BTA_ID_AV);
        bta_ar_dereg_avct(BTA_ID_AV);
#endif

        if (p_cb->disabling) {
            p_cb->disabling     = FALSE;
            bta_av_cb.features  = 0;
        }

        /* Clear the Capturing/Rendering service class bit */
        if (p_data->api_reg.tsep == AVDT_TSEP_SRC) {
            cod.service = BTM_COD_SERVICE_CAPTURING | BTM_COD_SERVICE_AUDIO;
        } else {
#if (BTA_AV_SINK_INCLUDED == TRUE)
            cod.service = BTM_COD_SERVICE_RENDERING | BTM_COD_SERVICE_AUDIO;
#endif
        }
        utl_set_device_class(&cod, BTA_UTL_CLR_COD_SERVICE_CLASS);
    }
}
#endif /* BTA_AV_INCLUDED */

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
 *  This file contains action functions for advanced audio/video stream
 *  state machine. these functions are shared by both audio and video
 *  streams.
 *
 ******************************************************************************/

#include "common/bt_target.h"
#if defined(BTA_AV_INCLUDED) && (BTA_AV_INCLUDED == TRUE)

#include <assert.h>
#include "common/bt_trace.h"
#include <string.h>

#include "osi/allocator.h"

#include "bta_av_int.h"
#include "stack/avdt_api.h"
#include "bta/utl.h"
#include "stack/l2c_api.h"
#include "stack/l2cdefs.h"
#if( defined BTA_AR_INCLUDED ) && (BTA_AR_INCLUDED == TRUE)
#include "bta/bta_ar_api.h"
#endif
#include "bta/bta_api.h"

/*****************************************************************************
**  Constants
*****************************************************************************/

/* the delay time in milliseconds to start service discovery on AVRCP */
#ifndef BTA_AV_RC_DISC_TIME_VAL
#define BTA_AV_RC_DISC_TIME_VAL     3500
#endif

/* the timer in milliseconds to guard against link busy and AVDT_CloseReq failed to be sent */
#ifndef BTA_AV_CLOSE_REQ_TIME_VAL
#define BTA_AV_CLOSE_REQ_TIME_VAL   4000
#endif

/* number to retry on reconfigure failure - some headsets requires this number to be more than 1 */
#ifndef BTA_AV_RECONFIG_RETRY
#define BTA_AV_RECONFIG_RETRY       6
#endif

/* avdt_handle to send abort command for AVDTP BQB test */
#if A2D_SRC_BQB_INCLUDED
static uint8_t s_avdt_bqb_handle;
#endif /* CONFIG_BT_BQB_ENABLED */

static void bta_av_st_rc_timer(tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);

/* state machine states */
enum {
    BTA_AV_INIT_SST,
    BTA_AV_INCOMING_SST,
    BTA_AV_OPENING_SST,
    BTA_AV_OPEN_SST,
    BTA_AV_RCFG_SST,
    BTA_AV_CLOSING_SST
};

/* ssm action functions for audio stream */
const tBTA_AV_SACT bta_av_a2d_action[] = {
    bta_av_do_disc_a2d,     /* BTA_AV_DO_DISC  */
    bta_av_cleanup,         /* BTA_AV_CLEANUP */
    bta_av_free_sdb,        /* BTA_AV_FREE_SDB */
    bta_av_config_ind,      /* BTA_AV_CONFIG_IND */
    bta_av_disconnect_req,  /* BTA_AV_DISCONNECT_REQ */
    bta_av_security_req,    /* BTA_AV_SECURITY_REQ */
    bta_av_security_rsp,    /* BTA_AV_SECURITY_RSP */
    bta_av_setconfig_rsp,   /* BTA_AV_SETCONFIG_RSP */
    bta_av_st_rc_timer,     /* BTA_AV_ST_RC_TIMER */
    bta_av_str_opened,      /* BTA_AV_STR_OPENED */
    bta_av_security_ind,    /* BTA_AV_SECURITY_IND */
    bta_av_security_cfm,    /* BTA_AV_SECURITY_CFM */
    bta_av_do_close,        /* BTA_AV_DO_CLOSE */
    bta_av_connect_req,     /* BTA_AV_CONNECT_REQ */
    bta_av_sdp_failed,      /* BTA_AV_SDP_FAILED */
    bta_av_disc_results,    /* BTA_AV_DISC_RESULTS */
    bta_av_disc_res_as_acp, /* BTA_AV_DISC_RES_AS_ACP */
    bta_av_open_failed,     /* BTA_AV_OPEN_FAILED */
    bta_av_getcap_results,  /* BTA_AV_GETCAP_RESULTS */
    bta_av_setconfig_rej,   /* BTA_AV_SETCONFIG_REJ */
    bta_av_discover_req,    /* BTA_AV_DISCOVER_REQ */
    bta_av_conn_failed,     /* BTA_AV_CONN_FAILED */
    bta_av_do_start,        /* BTA_AV_DO_START */
    bta_av_str_stopped,     /* BTA_AV_STR_STOPPED */
    bta_av_reconfig,        /* BTA_AV_RECONFIG */
    bta_av_data_path,       /* BTA_AV_DATA_PATH */
    bta_av_start_ok,        /* BTA_AV_START_OK */
    bta_av_start_failed,    /* BTA_AV_START_FAILED */
    bta_av_str_closed,      /* BTA_AV_STR_CLOSED */
    bta_av_clr_cong,        /* BTA_AV_CLR_CONG */
    bta_av_suspend_cfm,     /* BTA_AV_SUSPEND_CFM */
    bta_av_rcfg_str_ok,     /* BTA_AV_RCFG_STR_OK */
    bta_av_rcfg_failed,     /* BTA_AV_RCFG_FAILED */
    bta_av_rcfg_connect,    /* BTA_AV_RCFG_CONNECT */
    bta_av_rcfg_discntd,    /* BTA_AV_RCFG_DISCNTD */
    bta_av_suspend_cont,    /* BTA_AV_SUSPEND_CONT */
    bta_av_rcfg_cfm,        /* BTA_AV_RCFG_CFM */
    bta_av_rcfg_open,       /* BTA_AV_RCFG_OPEN */
    bta_av_security_rej,    /* BTA_AV_SECURITY_REJ */
    bta_av_open_rc,         /* BTA_AV_OPEN_RC */
    bta_av_chk_2nd_start,   /* BTA_AV_CHK_2ND_START */
    bta_av_save_caps,       /* BTA_AV_SAVE_CAPS */
    bta_av_set_use_rc,      /* BTA_AV_SET_USE_RC */
    bta_av_cco_close,       /* BTA_AV_CCO_CLOSE */
    bta_av_switch_role,     /* BTA_AV_SWITCH_ROLE */
    bta_av_role_res,        /* BTA_AV_ROLE_RES */
    bta_av_delay_co,        /* BTA_AV_DELAY_CO */
    bta_av_open_at_inc,     /* BTA_AV_OPEN_AT_INC */
    bta_av_open_fail_sdp,   /* BTA_AV_OPEN_FAIL_SDP */
    bta_av_set_delay_value, /* BTA_AV_SET_DELAY_VALUE */
    NULL
};

/* these tables translate AVDT events to SSM events */
static const UINT16 bta_av_stream_evt_ok[] = {
    BTA_AV_STR_DISC_OK_EVT,         /* AVDT_DISCOVER_CFM_EVT */
    BTA_AV_STR_GETCAP_OK_EVT,       /* AVDT_GETCAP_CFM_EVT */
    BTA_AV_STR_OPEN_OK_EVT,         /* AVDT_OPEN_CFM_EVT */
    BTA_AV_STR_OPEN_OK_EVT,         /* AVDT_OPEN_IND_EVT */
    BTA_AV_STR_CONFIG_IND_EVT,      /* AVDT_CONFIG_IND_EVT */
    BTA_AV_STR_START_OK_EVT,        /* AVDT_START_CFM_EVT */
    BTA_AV_STR_START_OK_EVT,        /* AVDT_START_IND_EVT */
    BTA_AV_STR_SUSPEND_CFM_EVT,     /* AVDT_SUSPEND_CFM_EVT */
    BTA_AV_STR_SUSPEND_CFM_EVT,     /* AVDT_SUSPEND_IND_EVT */
    BTA_AV_STR_CLOSE_EVT,           /* AVDT_CLOSE_CFM_EVT */
    BTA_AV_STR_CLOSE_EVT,           /* AVDT_CLOSE_IND_EVT */
    BTA_AV_STR_RECONFIG_CFM_EVT,    /* AVDT_RECONFIG_CFM_EVT */
    0,                              /* AVDT_RECONFIG_IND_EVT */
    BTA_AV_STR_SECURITY_CFM_EVT,    /* AVDT_SECURITY_CFM_EVT */
    BTA_AV_STR_SECURITY_IND_EVT,    /* AVDT_SECURITY_IND_EVT */
    BTA_AV_STR_WRITE_CFM_EVT,       /* AVDT_WRITE_CFM_EVT */
    BTA_AV_AVDT_CONNECT_EVT,        /* AVDT_CONNECT_IND_EVT */
    BTA_AV_AVDT_DISCONNECT_EVT,     /* AVDT_DISCONNECT_IND_EVT */
#if (AVDT_REPORTING == TRUE)
    BTA_AV_AVDT_RPT_CONN_EVT,       /* AVDT_REPORT_CONN_EVT */
    BTA_AV_AVDT_RPT_CONN_EVT,       /* AVDT_REPORT_DISCONN_EVT */
#endif
    BTA_AV_AVDT_DELAY_RPT_EVT,      /* AVDT_DELAY_REPORT_EVT */
    0                               /* AVDT_DELAY_REPORT_CFM_EVT */
};

static const UINT16 bta_av_stream_evt_fail[] = {
    BTA_AV_STR_DISC_FAIL_EVT,       /* AVDT_DISCOVER_CFM_EVT */
    BTA_AV_STR_GETCAP_FAIL_EVT,     /* AVDT_GETCAP_CFM_EVT */
    BTA_AV_STR_OPEN_FAIL_EVT,       /* AVDT_OPEN_CFM_EVT */
    BTA_AV_STR_OPEN_OK_EVT,         /* AVDT_OPEN_IND_EVT */
    BTA_AV_STR_CONFIG_IND_EVT,      /* AVDT_CONFIG_IND_EVT */
    BTA_AV_STR_START_FAIL_EVT,      /* AVDT_START_CFM_EVT */
    BTA_AV_STR_START_OK_EVT,        /* AVDT_START_IND_EVT */
    BTA_AV_STR_SUSPEND_CFM_EVT,     /* AVDT_SUSPEND_CFM_EVT */
    BTA_AV_STR_SUSPEND_CFM_EVT,     /* AVDT_SUSPEND_IND_EVT */
    BTA_AV_STR_CLOSE_EVT,           /* AVDT_CLOSE_CFM_EVT */
    BTA_AV_STR_CLOSE_EVT,           /* AVDT_CLOSE_IND_EVT */
    BTA_AV_STR_RECONFIG_CFM_EVT,    /* AVDT_RECONFIG_CFM_EVT */
    0,                              /* AVDT_RECONFIG_IND_EVT */
    BTA_AV_STR_SECURITY_CFM_EVT,    /* AVDT_SECURITY_CFM_EVT */
    BTA_AV_STR_SECURITY_IND_EVT,    /* AVDT_SECURITY_IND_EVT */
    BTA_AV_STR_WRITE_CFM_EVT,       /* AVDT_WRITE_CFM_EVT */
    BTA_AV_AVDT_CONNECT_EVT,        /* AVDT_CONNECT_IND_EVT */
    BTA_AV_AVDT_DISCONNECT_EVT,     /* AVDT_DISCONNECT_IND_EVT */
#if (AVDT_REPORTING == TRUE)
    BTA_AV_AVDT_RPT_CONN_EVT,       /* AVDT_REPORT_CONN_EVT */
    BTA_AV_AVDT_RPT_CONN_EVT,       /* AVDT_REPORT_DISCONN_EVT */
#endif
    BTA_AV_AVDT_DELAY_RPT_EVT,      /* AVDT_DELAY_REPORT_EVT */
    0                               /* AVDT_DELAY_REPORT_CFM_EVT */
};

void bta_av_stream_data_cback(UINT8 handle, BT_HDR *p_pkt, UINT32 time_stamp, UINT8 m_pt);
static void bta_av_stream0_cback(UINT8 handle, BD_ADDR bd_addr, UINT8 event, tAVDT_CTRL *p_data);
static void bta_av_stream1_cback(UINT8 handle, BD_ADDR bd_addr, UINT8 event, tAVDT_CTRL *p_data);
#if BTA_AV_NUM_STRS > 2
static void bta_av_stream2_cback(UINT8 handle, BD_ADDR bd_addr, UINT8 event, tAVDT_CTRL *p_data);
#endif
#if BTA_AV_NUM_STRS > 3
static void bta_av_stream3_cback(UINT8 handle, BD_ADDR bd_addr, UINT8 event, tAVDT_CTRL *p_data);
#endif
#if BTA_AV_NUM_STRS > 4
static void bta_av_stream4_cback(UINT8 handle, BD_ADDR bd_addr, UINT8 event, tAVDT_CTRL *p_data);
#endif
#if BTA_AV_NUM_STRS > 5
static void bta_av_stream5_cback(UINT8 handle, BD_ADDR bd_addr, UINT8 event, tAVDT_CTRL *p_data);
#endif
/* the array of callback functions to receive events from AVDT control channel */
tAVDT_CTRL_CBACK *const bta_av_dt_cback[] = {
    bta_av_stream0_cback
    , bta_av_stream1_cback
#if BTA_AV_NUM_STRS > 2
    , bta_av_stream2_cback
#endif
#if BTA_AV_NUM_STRS > 3
    , bta_av_stream3_cback
#endif
#if BTA_AV_NUM_STRS > 4
    , bta_av_stream4_cback
#endif
#if BTA_AV_NUM_STRS > 5
    , bta_av_stream5_cback
#endif
};
/***********************************************
**
** Function         bta_get_scb_handle
**
** Description      gives the registered AVDT handle.by checking with sep_type.
**
**
** Returns          void
***********************************************/
static UINT8 bta_av_get_scb_handle(tBTA_AV_SCB *p_scb, UINT8 local_sep)
{
    UINT8 xx = 0;
    for (xx = 0; xx < BTA_AV_MAX_SEPS; xx++) {
        if ((p_scb->seps[xx].tsep == local_sep) &&
                (p_scb->seps[xx].codec_type == p_scb->codec_type)) {
            return (p_scb->seps[xx].av_handle);
        }
    }
    APPL_TRACE_DEBUG(" bta_av_get_scb_handle appropriate sep_type not found")
    return 0; /* return invalid handle */
}

/***********************************************
**
** Function         bta_av_get_scb_sep_type
**
** Description      gives the sep type by cross-checking with AVDT handle
**
**
** Returns          void
***********************************************/
static UINT8 bta_av_get_scb_sep_type(tBTA_AV_SCB *p_scb, UINT8 tavdt_handle)
{
    UINT8 xx = 0;
    for (xx = 0; xx < BTA_AV_MAX_SEPS; xx++) {
        if (p_scb->seps[xx].av_handle == tavdt_handle) {
            return (p_scb->seps[xx].tsep);
        }
    }
    APPL_TRACE_DEBUG(" bta_av_get_scb_sep_type appropriate handle not found")
    return 3; /* return invalid sep type */
}

/*******************************************************************************
**
** Function         bta_av_save_addr
**
** Description      copy the bd_addr and maybe reset the supported flags
**
**
** Returns          void
**
*******************************************************************************/
static void bta_av_save_addr(tBTA_AV_SCB *p_scb, const BD_ADDR b)
{
    APPL_TRACE_DEBUG("bta_av_save_addr r:%d, s:%d",
                     p_scb->recfg_sup, p_scb->suspend_sup);
    if (bdcmp(p_scb->peer_addr, b) != 0) {
        APPL_TRACE_WARNING("reset flags");
        /* a new addr, reset the supported flags */
        p_scb->recfg_sup    = TRUE;
        p_scb->suspend_sup  = TRUE;
    }

    /* do this copy anyway, just in case the first addr matches
     * the control block one by accident */
    bdcpy(p_scb->peer_addr, b);
}

/*******************************************************************************
**
** Function         notify_start_failed
**
** Description      notify up-layer AV start failed
**
**
** Returns          void
**
*******************************************************************************/
static void notify_start_failed(tBTA_AV_SCB *p_scb)
{
    tBTA_AV_START   start;
    /* if start failed, clear role */
    p_scb->role &= ~BTA_AV_ROLE_START_INT;
    start.chnl   = p_scb->chnl;
    start.status = BTA_AV_FAIL;
    start.initiator = TRUE;
    start.hndl   = p_scb->hndl;
    (*bta_av_cb.p_cback)(BTA_AV_START_EVT, (tBTA_AV *) &start);
}

/*******************************************************************************
**
** Function         bta_av_st_rc_timer
**
** Description      start the AVRC timer if no RC connection & CT is supported &
**                  RC is used or
**                  as ACP (we do not really know if we want AVRC)
**
** Returns          void
**
*******************************************************************************/
static void bta_av_st_rc_timer(tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    UNUSED(p_data);

    APPL_TRACE_DEBUG("bta_av_st_rc_timer rc_handle:%d, use_rc: %d",
                     p_scb->rc_handle, p_scb->use_rc);
    /* for outgoing RC connection as INT/CT */
    if ( (p_scb->rc_handle == BTA_AV_RC_HANDLE_NONE) &&
            /*(bta_av_cb.features & BTA_AV_FEAT_RCCT) &&*/
            (p_scb->use_rc == TRUE || (p_scb->role & BTA_AV_ROLE_AD_ACP)) ) {
        if ((p_scb->wait & BTA_AV_WAIT_ROLE_SW_BITS) == 0) {
            bta_sys_start_timer(&p_scb->timer, BTA_AV_AVRC_TIMER_EVT, BTA_AV_RC_DISC_TIME_VAL);
        } else {
            p_scb->wait |= BTA_AV_WAIT_CHECK_RC;
        }
    }

}

/*******************************************************************************
**
** Function         bta_av_next_getcap
**
** Description      The function gets the capabilities of the next available
**                  stream found in the discovery results.
**
** Returns          TRUE if we sent request to AVDT, FALSE otherwise.
**
*******************************************************************************/
static BOOLEAN bta_av_next_getcap(tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    int     i;
    tAVDT_GETCAP_REQ    *p_req;
    BOOLEAN     sent_cmd = FALSE;
    UINT16 uuid_int = p_scb->uuid_int;
    UINT8 sep_requested = 0;

    if (uuid_int == UUID_SERVCLASS_AUDIO_SOURCE) {
        sep_requested = AVDT_TSEP_SNK;
    } else if (uuid_int == UUID_SERVCLASS_AUDIO_SINK) {
        sep_requested = AVDT_TSEP_SRC;
    }

    for (i = p_scb->sep_info_idx; i < p_scb->num_seps; i++) {
        /* steam not in use, is a sink, and is the right media type (audio/video) */
        if ((p_scb->sep_info[i].in_use == FALSE) &&
                (p_scb->sep_info[i].tsep == sep_requested) &&
                (p_scb->sep_info[i].media_type == p_scb->media_type)) {
            p_scb->sep_info_idx = i;

            /* we got a stream; get its capabilities */
            if (p_scb->p_cap == NULL) {
                p_scb->p_cap = (tAVDT_CFG *) osi_malloc(sizeof(tAVDT_CFG));
            }
            if (p_scb->p_cap == NULL) {
                i = p_scb->num_seps;
                break;
            }
            if (p_scb->avdt_version >= AVDT_VERSION_SYNC) {
                p_req = AVDT_GetAllCapReq;
            } else {
                p_req = AVDT_GetCapReq;
            }
            (*p_req)(p_scb->peer_addr,
                     p_scb->sep_info[i].seid,
                     p_scb->p_cap, bta_av_dt_cback[p_scb->hdi]);
            sent_cmd = TRUE;
            break;
        }
    }

    /* if no streams available then stream open fails */
    if (!sent_cmd) {
        bta_av_ssm_execute(p_scb, BTA_AV_STR_GETCAP_FAIL_EVT, p_data);
    }

    return sent_cmd;

}

/*******************************************************************************
**
** Function         bta_av_proc_stream_evt
**
** Description      Utility function to compose stream events.
**
** Returns          void
**
*******************************************************************************/
static void bta_av_proc_stream_evt(UINT8 handle, BD_ADDR bd_addr, UINT8 event, tAVDT_CTRL *p_data, int index)
{
    tBTA_AV_STR_MSG     *p_msg;
    UINT16              sec_len = 0;
    tBTA_AV_SCB         *p_scb = bta_av_cb.p_scb[index];
    int                 xx;

    if (event == AVDT_DELAY_REPORT_CFM_EVT) {
        APPL_TRACE_DEBUG("%s: AVDT_DELAY_REPORT_CFM_EVT", __func__);
        return;
    }

    if (p_data) {
        if (event == AVDT_SECURITY_IND_EVT) {
            sec_len = (p_data->security_ind.len < BTA_AV_SECURITY_MAX_LEN) ?
                      p_data->security_ind.len : BTA_AV_SECURITY_MAX_LEN;
        } else if (event == AVDT_SECURITY_CFM_EVT && p_data->hdr.err_code == 0) {
            sec_len = (p_data->security_cfm.len < BTA_AV_SECURITY_MAX_LEN) ?
                      p_data->security_cfm.len : BTA_AV_SECURITY_MAX_LEN;
        }
    }

    if (p_scb && (p_msg = (tBTA_AV_STR_MSG *) osi_malloc((UINT16) (sizeof(tBTA_AV_STR_MSG) + sec_len))) != NULL) {

        /* copy event data, bd addr, and handle to event message buffer */
        p_msg->hdr.offset = 0;

        if (bd_addr != NULL) {
            bdcpy(p_msg->bd_addr, bd_addr);
            APPL_TRACE_DEBUG("  bd_addr:%02x-%02x-%02x-%02x-%02x-%02x",
                             bd_addr[0], bd_addr[1],
                             bd_addr[2], bd_addr[3],
                             bd_addr[4], bd_addr[5]);
        }

        if (p_data != NULL) {
            memcpy(&p_msg->msg, p_data, sizeof (tAVDT_CTRL));
            /* copy config params to event message buffer */
            switch (event) {
            case AVDT_RECONFIG_CFM_EVT:
                APPL_TRACE_DEBUG("reconfig cfm event codec info = 0x%06x-%06x-%06x-%02x",
                                 (p_msg->msg.reconfig_cfm.p_cfg->codec_info[0] << 16) + (p_msg->msg.reconfig_cfm.p_cfg->codec_info[1] << 8) + p_msg->msg.reconfig_cfm.p_cfg->codec_info[2],
                                 (p_msg->msg.reconfig_cfm.p_cfg->codec_info[3] << 16) + (p_msg->msg.reconfig_cfm.p_cfg->codec_info[4] << 8) + p_msg->msg.reconfig_cfm.p_cfg->codec_info[5],
                                 (p_msg->msg.reconfig_cfm.p_cfg->codec_info[6] << 16) + (p_msg->msg.reconfig_cfm.p_cfg->codec_info[7] << 8) + p_msg->msg.reconfig_cfm.p_cfg->codec_info[8],
                                 p_msg->msg.reconfig_cfm.p_cfg->codec_info[9]);
                break;



            case AVDT_CONFIG_IND_EVT:
                /* We might have 2 SEP signallings(A2DP + VDP) with one peer device on one L2CAP.
                 * If we already have a signalling connection with the bd_addr and the streaming
                 * SST is at INIT state, change it to INCOMING state to handle the signalling
                 * from the 2nd SEP.                                                                */
                if ((bta_av_find_lcb(bd_addr, BTA_AV_LCB_FIND) != NULL) && (bta_av_is_scb_init(p_scb))) {
                    bta_av_set_scb_sst_incoming (p_scb);

                    /* When ACP_CONNECT_EVT was received, we put first available scb to incoming state.
                     * Later when we receive AVDT_CONFIG_IND_EVT, we use a new p_scb and set its state to
                     * incoming which we do it above.
                     * We also have to set the old p_scb state to init to be used later             */
                    for (xx = 0; xx < BTA_AV_NUM_STRS; xx++) {
                        if ((bta_av_cb.p_scb[xx]) && (xx != index)) {
                            if (bta_av_cb.p_scb[xx]->state == BTA_AV_INCOMING_SST) {
                                bta_av_cb.p_scb[xx]->state = BTA_AV_INIT_SST;
                                bta_av_cb.p_scb[xx]->coll_mask = 0;
                                break;
                            }
                        }
                    }
                }

                memcpy(&p_msg->cfg, p_data->config_ind.p_cfg, sizeof(tAVDT_CFG));
                break;

            case AVDT_SECURITY_IND_EVT:
                p_msg->msg.security_ind.p_data = (UINT8 *) (p_msg + 1);
                memcpy(p_msg->msg.security_ind.p_data, p_data->security_ind.p_data, sec_len);
                break;

            case AVDT_SECURITY_CFM_EVT:
                p_msg->msg.security_cfm.p_data = (UINT8 *) (p_msg + 1);
                if (p_data->hdr.err_code == 0) {
                    memcpy(p_msg->msg.security_cfm.p_data, p_data->security_cfm.p_data, sec_len);
                }
                break;
            case AVDT_SUSPEND_IND_EVT:
                p_msg->msg.hdr.err_code = 0;
                break;
            /*
            case AVDT_CLOSE_CFM_EVT:
            case AVDT_CLOSE_IND_EVT:
            p_msg->disc_rsn = p_data->hdr.err_param;
            break;
            */
            case AVDT_DISCONNECT_IND_EVT:
                p_msg->hdr.offset = p_data->hdr.err_param;
                break;
            default:
                break;
            }
        } else {
            p_msg->msg.hdr.err_code = 0;
        }

        /* look up application event */
        if ((p_data == NULL) || (p_data->hdr.err_code == 0)) {
            p_msg->hdr.event = bta_av_stream_evt_ok[event];
            if (p_msg->hdr.event == BTA_AV_STR_START_OK_EVT) {
                BTA_DmCoexEventTrigger(BTA_COEX_EVT_STREAMING_STARTED);
            } else if (p_msg->hdr.event == BTA_AV_STR_START_FAIL_EVT ||
                p_msg->hdr.event == BTA_AV_STR_SUSPEND_CFM_EVT ||
                p_msg->hdr.event == BTA_AV_STR_CLOSE_EVT) {
                BTA_DmCoexEventTrigger(BTA_COEX_EVT_STREAMING_STOPPED);
            }
        } else {
            p_msg->hdr.event = bta_av_stream_evt_fail[event];
            if (p_msg->hdr.event == BTA_AV_STR_START_FAIL_EVT ||
                p_msg->hdr.event == BTA_AV_STR_START_OK_EVT ||
                p_msg->hdr.event == BTA_AV_STR_SUSPEND_CFM_EVT ||
                p_msg->hdr.event == BTA_AV_STR_CLOSE_EVT) {
                BTA_DmCoexEventTrigger(BTA_COEX_EVT_STREAMING_STOPPED);
            }
        }

        p_msg->initiator = FALSE;
        if (event == AVDT_SUSPEND_CFM_EVT) {
            p_msg->initiator = TRUE;
        }

        APPL_TRACE_VERBOSE("hndl:x%x", p_scb->hndl);
        p_msg->hdr.layer_specific = p_scb->hndl;
        p_msg->handle   = handle;
        p_msg->avdt_event = event;
        bta_sys_sendmsg(p_msg);
    }

    /* coverity[var_deref_model] */
    /* false-positive: bta_av_conn_cback only processes AVDT_CONNECT_IND_EVT and AVDT_DISCONNECT_IND_EVT event
     *                 these 2 events always have associated p_data */
    if (p_data) {
        bta_av_conn_cback(handle, bd_addr, event, p_data);
    } else {
        APPL_TRACE_EVENT("%s: p_data is null", __func__);
    }
}

/*******************************************************************************
**
** Function         bta_av_stream_data_cback
**
** Description      This is the AVDTP callback function for stream events.
**
** Returns          void
**
*******************************************************************************/
void bta_av_stream_data_cback(UINT8 handle, BT_HDR *p_pkt, UINT32 time_stamp, UINT8 m_pt)
{
    int index = 0;
    tBTA_AV_SCB         *p_scb ;
    APPL_TRACE_DEBUG("bta_av_stream_data_cback avdt_handle: %d pkt_len=0x%x  ofst = 0x%x", handle, p_pkt->len, p_pkt->offset);
    APPL_TRACE_DEBUG(" Number of frames 0x%x", *((UINT8 *)(p_pkt + 1) + p_pkt->offset));
    APPL_TRACE_DEBUG("Sequence Number 0x%x", p_pkt->layer_specific);
    /* Get  SCB  and correct sep type*/
    for (index = 0; index < BTA_AV_NUM_STRS; index ++ ) {
        p_scb = bta_av_cb.p_scb[index];
        if ((p_scb->avdt_handle == handle) && (p_scb->seps[p_scb->sep_idx].tsep == AVDT_TSEP_SNK)) {
            break;
        }
    }
    if (index == BTA_AV_NUM_STRS) { /* cannot find correct handler */
        osi_free(p_pkt);
        return;
    }

    if (p_pkt->offset >= 4) {
        /* The offset of avdt packet will always greater than 4, it is safe to add timestamp here */
        *((UINT32 *) (p_pkt + 1)) = time_stamp;
    }
    else {
        APPL_TRACE_WARNING("bta_av_stream_data_cback avdt packet offset small than 4");
    }

    p_pkt->event = BTA_AV_MEDIA_DATA_EVT;
    p_scb->seps[p_scb->sep_idx].p_app_data_cback(BTA_AV_MEDIA_DATA_EVT, (tBTA_AV_MEDIA *)p_pkt);
    /* packet will be free by upper */
}

/*******************************************************************************
**
** Function         bta_av_stream0_cback
**
** Description      This is the AVDTP callback function for stream events.
**
** Returns          void
**
*******************************************************************************/
static void bta_av_stream0_cback(UINT8 handle, BD_ADDR bd_addr, UINT8 event, tAVDT_CTRL *p_data)
{
    APPL_TRACE_VERBOSE("bta_av_stream0_cback avdt_handle: %d event=0x%x", handle, event);
    bta_av_proc_stream_evt(handle, bd_addr, event, p_data, 0);
}

/*******************************************************************************
**
** Function         bta_av_stream1_cback
**
** Description      This is the AVDTP callback function for stream events.
**
** Returns          void
**
*******************************************************************************/
static void bta_av_stream1_cback(UINT8 handle, BD_ADDR bd_addr, UINT8 event, tAVDT_CTRL *p_data)
{
    APPL_TRACE_EVENT("bta_av_stream1_cback avdt_handle: %d event=0x%x", handle, event);
    bta_av_proc_stream_evt(handle, bd_addr, event, p_data, 1);
}

#if BTA_AV_NUM_STRS > 2
/*******************************************************************************
**
** Function         bta_av_stream2_cback
**
** Description      This is the AVDTP callback function for stream events.
**
** Returns          void
**
*******************************************************************************/
static void bta_av_stream2_cback(UINT8 handle, BD_ADDR bd_addr, UINT8 event, tAVDT_CTRL *p_data)
{
    APPL_TRACE_EVENT("bta_av_stream2_cback avdt_handle: %d event=0x%x", handle, event);
    bta_av_proc_stream_evt(handle, bd_addr, event, p_data, 2);
}
#endif

#if BTA_AV_NUM_STRS > 3
/*******************************************************************************
**
** Function         bta_av_stream3_cback
**
** Description      This is the AVDTP callback function for stream events.
**
** Returns          void
**
*******************************************************************************/
static void bta_av_stream3_cback(UINT8 handle, BD_ADDR bd_addr, UINT8 event, tAVDT_CTRL *p_data)
{
    APPL_TRACE_EVENT("bta_av_stream3_cback avdt_handle: %d event=0x%x", handle, event);
    bta_av_proc_stream_evt(handle, bd_addr, event, p_data, 3);
}
#endif

/*******************************************************************************
**
** Function         bta_av_stream4_cback
**
** Description      This is the AVDTP callback function for stream events.
**
** Returns          void
**
*******************************************************************************/
#if BTA_AV_NUM_STRS > 4
static void bta_av_stream4_cback(UINT8 handle, BD_ADDR bd_addr, UINT8 event, tAVDT_CTRL *p_data)
{
    APPL_TRACE_EVENT("bta_av_stream4_cback avdt_handle: %d event=0x%x", handle, event);
    bta_av_proc_stream_evt(handle, bd_addr, event, p_data, 4);
}
#endif

/*******************************************************************************
**
** Function         bta_av_stream5_cback
**
** Description      This is the AVDTP callback function for stream events.
**
** Returns          void
**
*******************************************************************************/
#if BTA_AV_NUM_STRS > 5
static void bta_av_stream5_cback(UINT8 handle, BD_ADDR bd_addr, UINT8 event, tAVDT_CTRL *p_data)
{
    APPL_TRACE_EVENT("bta_av_stream5_cback avdt_handle: %d event=0x%x", handle, event);
    bta_av_proc_stream_evt(handle, bd_addr, event, p_data, 5);
}
#endif

/*******************************************************************************
**
** Function         bta_av_a2d_sdp_cback
**
** Description      A2DP service discovery callback.
**
** Returns          void
**
*******************************************************************************/
static void bta_av_a2d_sdp_cback(BOOLEAN found, tA2D_Service *p_service)
{
    tBTA_AV_SDP_RES *p_msg;
    tBTA_AV_SCB     *p_scb;

    if ((p_msg = (tBTA_AV_SDP_RES *) osi_malloc(sizeof(tBTA_AV_SDP_RES))) != NULL) {
        p_msg->hdr.event = (found) ? BTA_AV_SDP_DISC_OK_EVT : BTA_AV_SDP_DISC_FAIL_EVT;

        p_scb = bta_av_hndl_to_scb(bta_av_cb.handle);
        if (p_scb) {
            if (found && (p_service != NULL)) {
                p_scb->avdt_version = p_service->avdt_version;
            } else {
                p_scb->avdt_version = 0x00;
            }

            p_msg->hdr.layer_specific = bta_av_cb.handle;
            bta_sys_sendmsg(p_msg);
        } else {
            APPL_TRACE_ERROR ("bta_av_a2d_sdp_cback, no scb found for handle(0x%x)", bta_av_cb.handle);
        }
    }
}

/*******************************************************************************
**
** Function         bta_av_adjust_seps_idx
**
** Description      adjust the sep_idx
**
** Returns
**
*******************************************************************************/
static void bta_av_adjust_seps_idx(tBTA_AV_SCB *p_scb, UINT8 avdt_handle)
{
    int xx;
    APPL_TRACE_DEBUG("bta_av_adjust_seps_idx codec_type: %d", p_scb->codec_type);
    for (xx = 0; xx < BTA_AV_MAX_SEPS; xx++) {
        APPL_TRACE_DEBUG("av_handle: %d codec_type: %d",
                         p_scb->seps[xx].av_handle, p_scb->seps[xx].codec_type);
        if ((p_scb->seps[xx].av_handle) && (p_scb->seps[xx].av_handle == avdt_handle)) {
            p_scb->sep_idx      = xx;
            p_scb->avdt_handle  = p_scb->seps[xx].av_handle;
            break;
        }
    }
}

/*******************************************************************************
**
** Function         bta_av_switch_role
**
** Description      Switch role was not started and a timer was started.
**                  another attempt to switch role now - still opening.
**
** Returns          void
**
*******************************************************************************/
void bta_av_switch_role (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_RS_RES      switch_res = BTA_AV_RS_NONE;
    tBTA_AV_API_OPEN  *p_buf = &p_scb->q_info.open;
    UNUSED(p_data);

    APPL_TRACE_DEBUG("bta_av_switch_role wait:x%x", p_scb->wait);
    if (p_scb->wait & BTA_AV_WAIT_ROLE_SW_RES_START) {
        p_scb->wait |= BTA_AV_WAIT_ROLE_SW_RETRY;
    }

    /* clear the masks set when the timer is started */
    p_scb->wait &= ~(BTA_AV_WAIT_ROLE_SW_RES_OPEN | BTA_AV_WAIT_ROLE_SW_RES_START);

    if (p_scb->q_tag == BTA_AV_Q_TAG_OPEN) {
        if (bta_av_switch_if_needed(p_scb) || !bta_av_link_role_ok(p_scb, A2D_SET_MULTL_BIT)) {
            p_scb->wait |= BTA_AV_WAIT_ROLE_SW_RES_OPEN;
        } else {
            /* this should not happen in theory. Just in case...
             * continue to do_disc_a2d */
            switch_res = BTA_AV_RS_DONE;
        }
    } else {
        /* report failure on OPEN */
        switch_res = BTA_AV_RS_FAIL;
    }

    if (switch_res != BTA_AV_RS_NONE) {
        if (bta_av_cb.rs_idx == (p_scb->hdi + 1)) {
            bta_av_cb.rs_idx = 0;
        }
        p_scb->wait &= ~BTA_AV_WAIT_ROLE_SW_RETRY;
        p_scb->q_tag = 0;
        p_buf->switch_res = switch_res;
        bta_av_do_disc_a2d(p_scb, (tBTA_AV_DATA *)p_buf);
    }
}

/*******************************************************************************
**
** Function         bta_av_role_res
**
** Description      Handle the role changed event
**
**
** Returns          void
**
*******************************************************************************/
void bta_av_role_res (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    BOOLEAN         initiator = FALSE;
    tBTA_AV_START   start;
    tBTA_AV_OPEN    av_open;

    APPL_TRACE_DEBUG("bta_av_role_res q_tag:%d, wait:x%x, role:x%x", p_scb->q_tag, p_scb->wait, p_scb->role);
    if (p_scb->role & BTA_AV_ROLE_START_INT) {
        initiator = TRUE;
    }

    if (p_scb->q_tag == BTA_AV_Q_TAG_START) {
        if (p_scb->wait & BTA_AV_WAIT_ROLE_SW_STARTED) {
            p_scb->wait &= ~BTA_AV_WAIT_ROLE_SW_BITS;
            if (p_data->role_res.hci_status != HCI_SUCCESS) {
                p_scb->role &= ~BTA_AV_ROLE_START_INT;
                bta_sys_idle(TSEP_TO_SYS_ID(p_scb->seps[p_scb->sep_idx].tsep), bta_av_cb.audio_open_cnt, p_scb->peer_addr);
                /* start failed because of role switch. */
                start.chnl   = p_scb->chnl;
                start.status = BTA_AV_FAIL_ROLE;
                start.hndl   = p_scb->hndl;
                start.initiator = initiator;
                (*bta_av_cb.p_cback)(BTA_AV_START_EVT, (tBTA_AV *) &start);
            } else {
                bta_av_start_ok(p_scb, p_data);
            }
        } else if (p_scb->wait & BTA_AV_WAIT_ROLE_SW_RES_START) {
            p_scb->wait |= BTA_AV_WAIT_ROLE_SW_FAILED;
        }
    } else if (p_scb->q_tag == BTA_AV_Q_TAG_OPEN) {
        if (p_scb->wait & BTA_AV_WAIT_ROLE_SW_RES_OPEN) {
            p_scb->role &= ~BTA_AV_ROLE_START_INT;
            p_scb->wait &= ~BTA_AV_WAIT_ROLE_SW_BITS;

            if (p_data->role_res.hci_status != HCI_SUCCESS) {
                /* Open failed because of role switch. */
                bdcpy(av_open.bd_addr, p_scb->peer_addr);
                av_open.chnl   = p_scb->chnl;
                av_open.hndl   = p_scb->hndl;
                av_open.status = BTA_AV_FAIL_ROLE;
                if (p_scb->seps[p_scb->sep_idx].tsep == AVDT_TSEP_SRC ) {
                    av_open.sep = AVDT_TSEP_SNK;
                } else if (p_scb->seps[p_scb->sep_idx].tsep == AVDT_TSEP_SNK ) {
                    av_open.sep = AVDT_TSEP_SRC;
                }
                (*bta_av_cb.p_cback)(BTA_AV_OPEN_EVT, (tBTA_AV *)&av_open);
            } else {
                /* Continue av open process */
                p_scb->q_info.open.switch_res = BTA_AV_RS_DONE;
                bta_av_do_disc_a2d (p_scb, (tBTA_AV_DATA *) & (p_scb->q_info.open));
            }
        } else {
            APPL_TRACE_WARNING ("Unexpected role switch event: q_tag = %d wait = %d", p_scb->q_tag, p_scb->wait);
        }
    }

    APPL_TRACE_DEBUG("wait:x%x, role:x%x", p_scb->wait, p_scb->role);
}

/*******************************************************************************
**
** Function         bta_av_delay_co
**
** Description      Call the delay call-out function to report the delay report
**                  from SNK
**
** Returns          void
**
*******************************************************************************/
void bta_av_delay_co (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    p_scb->p_cos->delay(p_scb->hndl, p_data->str_msg.msg.delay_rpt_cmd.delay);
}

/*******************************************************************************
**
** Function         bta_av_do_disc_a2d
**
** Description      Do service discovery for A2DP.
**
** Returns          void
**
*******************************************************************************/
void bta_av_do_disc_a2d (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    BOOLEAN     ok_continue = FALSE;
    tA2D_SDP_DB_PARAMS  db_params;
    UINT16              attr_list[] = {ATTR_ID_SERVICE_CLASS_ID_LIST,
                                       ATTR_ID_PROTOCOL_DESC_LIST,
                                       ATTR_ID_BT_PROFILE_DESC_LIST
                                      };
    UINT16 sdp_uuid = 0; /* UUID for which SDP has to be done */

    APPL_TRACE_DEBUG("bta_av_do_disc_a2d use_rc: %d rs:%d, oc:%d",
                     p_data->api_open.use_rc, p_data->api_open.switch_res, bta_av_cb.audio_open_cnt);

    memcpy (&(p_scb->open_api), &(p_data->api_open), sizeof(tBTA_AV_API_OPEN));

    switch (p_data->api_open.switch_res) {
    case BTA_AV_RS_NONE:
        if (bta_av_switch_if_needed(p_scb) || !bta_av_link_role_ok(p_scb, A2D_SET_MULTL_BIT)) {
            /* waiting for role switch result. save the api to control block */
            memcpy(&p_scb->q_info.open, &p_data->api_open, sizeof(tBTA_AV_API_OPEN));
            p_scb->wait |= BTA_AV_WAIT_ROLE_SW_RES_OPEN;
            p_scb->q_tag = BTA_AV_Q_TAG_OPEN;
        } else {
            ok_continue = TRUE;
        }
        break;

    case BTA_AV_RS_FAIL:
        /* report a new failure event  */
        p_scb->open_status = BTA_AV_FAIL_ROLE;
        bta_av_ssm_execute(p_scb, BTA_AV_SDP_DISC_FAIL_EVT, NULL);
        break;

    case BTA_AV_RS_OK:
        p_data = (tBTA_AV_DATA *)&p_scb->q_info.open;
        /* continue to open if link role is ok */
        if (bta_av_link_role_ok(p_scb, A2D_SET_MULTL_BIT)) {
            ok_continue = TRUE;
        } else {
            p_scb->wait |= BTA_AV_WAIT_ROLE_SW_RES_OPEN;
        }
        break;

    case BTA_AV_RS_DONE:
        ok_continue = TRUE;
        break;
    }

    APPL_TRACE_DEBUG("ok_continue: %d wait:x%x, q_tag: %d", ok_continue, p_scb->wait, p_scb->q_tag);
    if (!ok_continue) {
        return;
    }

    /* clear the role switch bits */
    p_scb->wait &= ~BTA_AV_WAIT_ROLE_SW_BITS;

    if (p_scb->wait & BTA_AV_WAIT_CHECK_RC) {
        p_scb->wait &= ~BTA_AV_WAIT_CHECK_RC;
        bta_sys_start_timer(&p_scb->timer, BTA_AV_AVRC_TIMER_EVT, BTA_AV_RC_DISC_TIME_VAL);
    }

    if (bta_av_cb.features & BTA_AV_FEAT_MASTER) {
        L2CA_SetDesireRole(L2CAP_ROLE_DISALLOW_SWITCH);

        if (bta_av_cb.audio_open_cnt == 1) {
            /* there's already an A2DP connection. do not allow switch */
            bta_sys_clear_default_policy(BTA_ID_AV, HCI_ENABLE_MASTER_SLAVE_SWITCH);
        }
    }
    /* store peer addr other parameters */
    bta_av_save_addr(p_scb, p_data->api_open.bd_addr);
    p_scb->sec_mask = p_data->api_open.sec_mask;
    p_scb->use_rc = p_data->api_open.use_rc;

    bta_sys_app_open(TSEP_TO_SYS_ID(p_scb->seps[p_scb->sep_idx].tsep), p_scb->app_id, p_scb->peer_addr);

    /* allocate discovery database */
    if (p_scb->p_disc_db == NULL) {
        p_scb->p_disc_db = (tSDP_DISCOVERY_DB *) osi_malloc(BTA_AV_DISC_BUF_SIZE);
    }

    /* only one A2D find service is active at a time */
    bta_av_cb.handle = p_scb->hndl;

    if (p_scb->p_disc_db) {
        /* set up parameters */
        db_params.db_len = BTA_AV_DISC_BUF_SIZE;
        db_params.num_attr = 3;
        db_params.p_db = p_scb->p_disc_db;
        db_params.p_attrs = attr_list;
        p_scb->uuid_int = p_data->api_open.uuid;
        if (p_scb->uuid_int == UUID_SERVCLASS_AUDIO_SINK) {
            sdp_uuid = UUID_SERVCLASS_AUDIO_SOURCE;
        } else if (p_scb->uuid_int == UUID_SERVCLASS_AUDIO_SOURCE) {
            sdp_uuid = UUID_SERVCLASS_AUDIO_SINK;
        }

        APPL_TRACE_DEBUG("uuid_int 0x%x, Doing SDP For 0x%x", p_scb->uuid_int, sdp_uuid);
        if (A2D_FindService(sdp_uuid, p_scb->peer_addr, &db_params,
                            bta_av_a2d_sdp_cback) == A2D_SUCCESS) {
            return;
        }
    }

    /* when the code reaches here, either the DB is NULL
     * or A2D_FindService is not successful */
    bta_av_a2d_sdp_cback(FALSE, NULL);
}

/*******************************************************************************
**
** Function         bta_av_cleanup
**
** Description      cleanup AV stream control block.
**
** Returns          void
**
*******************************************************************************/
void bta_av_cleanup(tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_CONN_CHG msg;
    int             xx;
    UINT8           role = BTA_AV_ROLE_AD_INT;
    UNUSED(p_data);

    APPL_TRACE_DEBUG("bta_av_cleanup");

    /* free any buffers */
    utl_freebuf((void **) &p_scb->p_cap);
    utl_freebuf((void **) &p_scb->p_disc_db);
    p_scb->avdt_version = 0;

    /* initialize some control block variables */
    p_scb->open_status = BTA_AV_SUCCESS;

    /* if de-registering shut everything down */
    msg.hdr.layer_specific  = p_scb->hndl;
    p_scb->started  = FALSE;
    p_scb->cong = FALSE;
    p_scb->role = role;
    p_scb->cur_psc_mask = 0;
    p_scb->wait = 0;
    p_scb->num_disc_snks = 0;
    p_scb->disc_rsn = 0;
    p_scb->avdt_handle = 0;
    bta_sys_stop_timer(&p_scb->timer);
    if (p_scb->deregistring) {
        /* remove stream */
        for (xx = 0; xx < BTA_AV_MAX_SEPS; xx++) {
            if (p_scb->seps[xx].av_handle) {
                AVDT_RemoveStream(p_scb->seps[xx].av_handle);
            }
            p_scb->seps[xx].av_handle = 0;
        }

        bta_av_dereg_comp((tBTA_AV_DATA *) &msg);
    } else {
        /* report stream closed to main SM */
        msg.is_up = FALSE;
        bdcpy(msg.peer_addr, p_scb->peer_addr);
        bta_av_conn_chg((tBTA_AV_DATA *) &msg);
    }
}

/*******************************************************************************
**
** Function         bta_av_free_sdb
**
** Description      Free service discovery db buffer.
**
** Returns          void
**
*******************************************************************************/
void bta_av_free_sdb(tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    UNUSED(p_data);
    utl_freebuf((void **) &p_scb->p_disc_db);
}

/*******************************************************************************
**
** Function         bta_av_open_fail_sdp
**
** Description      report BTA_AV_OPEN_EVT with service discovery failed status
**
** Returns          void
**
*******************************************************************************/
void bta_av_open_fail_sdp(tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_OPEN open;

    bdcpy(open.bd_addr, p_scb->peer_addr);
    open.chnl   = p_scb->chnl;
    open.hndl   = p_scb->hndl;
    open.status = BTA_AV_FAIL_SDP;

    if (p_scb->seps[p_scb->sep_idx].tsep == AVDT_TSEP_SRC ) {
        open.sep = AVDT_TSEP_SNK;
    } else if (p_scb->seps[p_scb->sep_idx].tsep == AVDT_TSEP_SNK ) {
        open.sep = AVDT_TSEP_SRC;
    }

    (*bta_av_cb.p_cback)(BTA_AV_OPEN_EVT, (tBTA_AV *) &open);

    UNUSED(p_data);
}

/*******************************************************************************
**
** Function         bta_av_config_ind
**
** Description      Handle a stream configuration indication from the peer.
**
** Returns          void
**
*******************************************************************************/
void bta_av_config_ind (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_CI_SETCONFIG setconfig;
    tAVDT_SEP_INFO       *p_info;
    tAVDT_CFG            *p_evt_cfg = &p_data->str_msg.cfg;
    UINT8   psc_mask = (p_evt_cfg->psc_mask | p_scb->cfg.psc_mask);
    UINT8 local_sep;    /* sep type of local handle on which connection was received */
    tBTA_AV_STR_MSG  *p_msg = (tBTA_AV_STR_MSG *)p_data;
    UNUSED(p_data);

    local_sep = bta_av_get_scb_sep_type(p_scb, p_msg->handle);
    p_scb->avdt_label = p_data->str_msg.msg.hdr.label;
    memcpy(p_scb->cfg.codec_info, p_evt_cfg->codec_info, AVDT_CODEC_SIZE);
    p_scb->codec_type = p_evt_cfg->codec_info[BTA_AV_CODEC_TYPE_IDX];
    bta_av_save_addr(p_scb, p_data->str_msg.bd_addr);

    /* Clear collision mask */
    p_scb->coll_mask = 0;
    bta_sys_stop_timer(&bta_av_cb.acp_sig_tmr);

    /* if no codec parameters in configuration, fail */
    if ((p_evt_cfg->num_codec == 0) ||
            /* or the peer requests for a service we do not support */
            ((psc_mask != p_scb->cfg.psc_mask) &&
             (psc_mask != (p_scb->cfg.psc_mask & ~AVDT_PSC_DELAY_RPT))) ) {
        setconfig.hndl      = p_scb->hndl; /* we may not need this */
        setconfig.err_code  = AVDT_ERR_UNSUP_CFG;
        bta_av_ssm_execute(p_scb, BTA_AV_CI_SETCONFIG_FAIL_EVT, (tBTA_AV_DATA *) &setconfig);
    } else {
        p_info = &p_scb->sep_info[0];
        p_info->in_use = 0;
        p_info->media_type = p_scb->media_type;
        p_info->seid = p_data->str_msg.msg.config_ind.int_seid;

        /* Sep type of Peer will be opposite role to our local sep */
        if (local_sep == AVDT_TSEP_SRC) {
            p_info->tsep = AVDT_TSEP_SNK;
        } else if (local_sep == AVDT_TSEP_SNK) {
            p_info->tsep = AVDT_TSEP_SRC;
        }

        p_scb->role      |= BTA_AV_ROLE_AD_ACP;
        p_scb->cur_psc_mask = p_evt_cfg->psc_mask;
        if (bta_av_cb.features & BTA_AV_FEAT_RCTG) {
            p_scb->use_rc = TRUE;
        } else {
            p_scb->use_rc = FALSE;
        }

        p_scb->num_seps  = 1;
        p_scb->sep_info_idx = 0;
        APPL_TRACE_DEBUG("bta_av_config_ind: SEID: %d use_rc: %d cur_psc_mask:0x%x", p_info->seid, p_scb->use_rc, p_scb->cur_psc_mask);
        /*  in case of A2DP SINK this is the first time peer data is being sent to co functions */
        if (local_sep == AVDT_TSEP_SNK) {
            p_scb->p_cos->setcfg(p_scb->hndl, p_scb->codec_type,
                                 p_evt_cfg->codec_info,
                                 p_info->seid,
                                 p_scb->peer_addr,
                                 p_evt_cfg->num_protect,
                                 p_evt_cfg->protect_info,
                                 AVDT_TSEP_SNK,
                                 p_msg->handle);
        } else {
            p_scb->p_cos->setcfg(p_scb->hndl, p_scb->codec_type,
                                 p_evt_cfg->codec_info,
                                 p_info->seid,
                                 p_scb->peer_addr,
                                 p_evt_cfg->num_protect,
                                 p_evt_cfg->protect_info,
                                 AVDT_TSEP_SRC,
                                 p_msg->handle);
        }
    }
}

/*******************************************************************************
**
** Function         bta_av_disconnect_req
**
** Description      Disconnect AVDTP connection.
**
** Returns          void
**
*******************************************************************************/
void bta_av_disconnect_req (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_RCB *p_rcb;
    UNUSED(p_data);

    APPL_TRACE_DEBUG("bta_av_disconnect_req conn_lcb: 0x%x", bta_av_cb.conn_lcb);

    bta_sys_stop_timer(&bta_av_cb.sig_tmr);
    bta_sys_stop_timer(&p_scb->timer);
    if (bta_av_cb.conn_lcb) {
        p_rcb = bta_av_get_rcb_by_shdl((UINT8)(p_scb->hdi + 1));
        if (p_rcb) {
            bta_av_del_rc(p_rcb);
        }
        AVDT_DisconnectReq(p_scb->peer_addr, bta_av_dt_cback[p_scb->hdi]);
    } else {
        bta_av_ssm_execute(p_scb, BTA_AV_AVDT_DISCONNECT_EVT, NULL);
    }
}

/*******************************************************************************
**
** Function         bta_av_security_req
**
** Description      Send an AVDTP security request.
**
** Returns          void
**
*******************************************************************************/
void bta_av_security_req (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    if (bta_av_cb.features & BTA_AV_FEAT_PROTECT) {
        AVDT_SecurityReq(p_scb->avdt_handle, p_data->api_protect_req.p_data,
                         p_data->api_protect_req.len);
    }
}

/*******************************************************************************
**
** Function         bta_av_security_rsp
**
** Description      Send an AVDTP security response.
**
** Returns          void
**
*******************************************************************************/
void bta_av_security_rsp (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    if (bta_av_cb.features & BTA_AV_FEAT_PROTECT) {
        AVDT_SecurityRsp(p_scb->avdt_handle, p_scb->avdt_label, p_data->api_protect_rsp.error_code,
                         p_data->api_protect_rsp.p_data, p_data->api_protect_rsp.len);
    } else {
        AVDT_SecurityRsp(p_scb->avdt_handle, p_scb->avdt_label, AVDT_ERR_NSC,
                         NULL, 0);
    }
}

/*******************************************************************************
**
** Function         bta_av_setconfig_rsp
**
** Description      setconfig is OK
**
** Returns          void
**
*******************************************************************************/
void bta_av_setconfig_rsp (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    UINT8   num = p_data->ci_setconfig.num_seid + 1;
    UINT8   avdt_handle = p_data->ci_setconfig.avdt_handle;
    UINT8   *p_seid = p_data->ci_setconfig.p_seid;
    int     i;
    UINT8   local_sep;
    tBTA_AV_SNK_PSC_CFG psc_cfg = {0};

    /* we like this codec_type. find the sep_idx */
    local_sep = bta_av_get_scb_sep_type(p_scb, avdt_handle);
    bta_av_adjust_seps_idx(p_scb, avdt_handle);
    APPL_TRACE_DEBUG("bta_av_setconfig_rsp: sep_idx: %d cur_psc_mask:0x%x", p_scb->sep_idx, p_scb->cur_psc_mask);

    if ((p_data->ci_setconfig.err_code == AVDT_SUCCESS) &&
        (p_scb->seps[p_scb->sep_idx].p_app_data_cback != NULL)) {
            p_scb->seps[p_scb->sep_idx].p_app_data_cback(BTA_AV_MEDIA_CFG_EVT,
                    (tBTA_AV_MEDIA *)p_scb->cfg.codec_info);
    }

    if (AVDT_TSEP_SNK == local_sep) {
        if (p_scb->cur_psc_mask & AVDT_PSC_DELAY_RPT) {
            psc_cfg.psc_mask |= BTA_AV_PSC_DEALY_RPT;
        }
        (*bta_av_cb.p_cback)(BTA_AV_SNK_PSC_CFG_EVT, (tBTA_AV *)&psc_cfg);
    }

    AVDT_ConfigRsp(p_scb->avdt_handle, p_scb->avdt_label, p_data->ci_setconfig.err_code,
                   p_data->ci_setconfig.category);

    bta_sys_stop_timer(&bta_av_cb.sig_tmr);

    if (p_data->ci_setconfig.err_code == AVDT_SUCCESS) {
        p_scb->wait = BTA_AV_WAIT_ACP_CAPS_ON;
        if (p_data->ci_setconfig.recfg_needed) {
            p_scb->role |= BTA_AV_ROLE_SUSPEND_OPT;
        }
        APPL_TRACE_DEBUG("bta_av_setconfig_rsp recfg_needed:%d role:x%x num:%d",
                         p_data->ci_setconfig.recfg_needed, p_scb->role, num);
        /* callout module tells BTA the number of "good" SEPs and their SEIDs.
         * getcap on these SEID */
        p_scb->num_seps = num;

        if (p_scb->cur_psc_mask & AVDT_PSC_DELAY_RPT) {
            p_scb->avdt_version = AVDT_VERSION_SYNC;
        }


        if (p_scb->codec_type == BTA_AV_CODEC_SBC || num > 1) {
            /* if SBC is used by the SNK as INT, discover req is not sent in bta_av_config_ind.
                       * call cfg_res now */
            /* this is called in A2DP SRC path only, In case of SINK we don't need it  */
            if (local_sep == AVDT_TSEP_SRC) {
                p_scb->p_cos->cfg_res(p_scb->hndl, num, num, 0, p_scb->peer_addr,
                                      UUID_SERVCLASS_AUDIO_SOURCE);
            }
        } else {
            /* we do not know the peer device and it is using non-SBC codec
             * we need to know all the SEPs on SNK */
            bta_av_discover_req(p_scb, NULL);
            return;
        }

        for (i = 1; i < num; i++) {
            APPL_TRACE_DEBUG("sep_info[%d] SEID: %d", i, p_seid[i - 1]);
            /* initialize the sep_info[] to get capabilities */
            p_scb->sep_info[i].in_use = FALSE;
            p_scb->sep_info[i].tsep = AVDT_TSEP_SNK;
            p_scb->sep_info[i].media_type = p_scb->media_type;
            p_scb->sep_info[i].seid = p_seid[i - 1];
        }

        /* only in case of local sep as SRC we need to look for other SEPs, In case of SINK we don't */
        if (local_sep == AVDT_TSEP_SRC) {
            /* Make sure UUID has been initialized... */
            if (p_scb->uuid_int == 0) {
                p_scb->uuid_int = p_scb->open_api.uuid;
            }
            bta_av_next_getcap(p_scb, p_data);
        }
    }
}

/*******************************************************************************
**
** Function         bta_av_str_opened
**
** Description      Stream opened OK (incoming/outgoing).
**
** Returns          void
**
*******************************************************************************/
void bta_av_str_opened (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_CONN_CHG msg;
    tBTA_AV_OPEN    open;
    UINT8 *p;
    UINT16 mtu;

    msg.hdr.layer_specific = p_scb->hndl;
    msg.is_up = TRUE;
    bdcpy(msg.peer_addr, p_scb->peer_addr);
    p_scb->l2c_cid      = AVDT_GetL2CapChannel(p_scb->avdt_handle);
    bta_av_conn_chg((tBTA_AV_DATA *) &msg);
    /* set the congestion flag, so AV would not send media packets by accident */
    p_scb->cong = TRUE;


    p_scb->stream_mtu = p_data->str_msg.msg.open_ind.peer_mtu - AVDT_MEDIA_HDR_SIZE;
    mtu = bta_av_chk_mtu(p_scb, p_scb->stream_mtu);
    APPL_TRACE_DEBUG("bta_av_str_opened l2c_cid: 0x%x stream_mtu: %d mtu: %d",
                     p_scb->l2c_cid, p_scb->stream_mtu, mtu);
    if (mtu == 0 || mtu > p_scb->stream_mtu) {
        mtu = p_scb->stream_mtu;
    }

    /* Set the media channel as medium priority */
    L2CA_SetTxPriority(p_scb->l2c_cid, L2CAP_CHNL_PRIORITY_MEDIUM);
    L2CA_SetChnlFlushability (p_scb->l2c_cid, TRUE);

    bta_sys_conn_open(TSEP_TO_SYS_ID(p_scb->seps[p_scb->sep_idx].tsep), p_scb->app_id, p_scb->peer_addr);
    memset(&p_scb->q_info, 0, sizeof(tBTA_AV_Q_INFO));

    p_scb->l2c_bufs = 0;
    p_scb->p_cos->open(p_scb->hndl,
                       p_scb->codec_type, p_scb->cfg.codec_info, mtu);

    {
        /* TODO check if other audio channel is open.
         * If yes, check if reconfig is needed
         * Right now we do not do this kind of checking.
         * BTA-AV is INT for 2nd audio connection.
         * The application needs to make sure the current codec_info is proper.
         * If one audio connection is open and another SNK attempts to connect to AV,
         * the connection will be rejected.
         */
        /* check if other audio channel is started. If yes, start */
        bdcpy(open.bd_addr, p_scb->peer_addr);
        open.chnl   = p_scb->chnl;
        open.hndl   = p_scb->hndl;
        open.status = BTA_AV_SUCCESS;
        open.starting = bta_av_chk_start(p_scb);
        open.edr    = 0;
        open.mtu = mtu;
        if ( NULL != (p = BTM_ReadRemoteFeatures(p_scb->peer_addr))) {
            if (HCI_EDR_ACL_2MPS_SUPPORTED(p)) {
                open.edr |= BTA_AV_EDR_2MBPS;
            }
            if (HCI_EDR_ACL_3MPS_SUPPORTED(p)) {
                open.edr |= BTA_AV_EDR_3MBPS;
            }
        }
#if( defined BTA_AR_INCLUDED ) && (BTA_AR_INCLUDED == TRUE)
        bta_ar_avdt_conn(BTA_ID_AV, open.bd_addr);
#endif
        if (p_scb->seps[p_scb->sep_idx].tsep == AVDT_TSEP_SRC ) {
            open.sep = AVDT_TSEP_SNK;
        } else if (p_scb->seps[p_scb->sep_idx].tsep == AVDT_TSEP_SNK ) {
            open.sep = AVDT_TSEP_SRC;
        }

        (*bta_av_cb.p_cback)(BTA_AV_OPEN_EVT, (tBTA_AV *) &open);
        if (open.starting) {
            bta_av_ssm_execute(p_scb, BTA_AV_AP_START_EVT, NULL);
        }
    }

#if A2D_SRC_BQB_INCLUDED
    s_avdt_bqb_handle = p_scb->avdt_handle;
#endif /* A2D_SRC_BQB_INCLUDED */

#if 0 /* TODO: implement the property enable/disable */
    // This code is used to pass PTS TC for AVDTP ABORT
    char value[PROPERTY_VALUE_MAX] = {0};
    if ((property_get("bluetooth.pts.force_a2dp_abort", value, "false"))
            && (!strcmp(value, "true"))) {
        APPL_TRACE_ERROR ("%s: Calling AVDT_AbortReq", __func__);
        AVDT_AbortReq(p_scb->avdt_handle);
    }
#endif /* #if 0*/
}

/*******************************************************************************
**
** Function         avdt_bqb_abort
**
** Description      Send AVDT abort request for BQB test
**
** Returns          void
**
*******************************************************************************/
#if A2D_SRC_BQB_INCLUDED
void avdt_bqb_abort(void)
{
    AVDT_AbortReq(s_avdt_bqb_handle);
}
#endif /* A2D_SRC_BQB_INCLUDED */

/*******************************************************************************
**
** Function         bta_av_security_ind
**
** Description      Handle an AVDTP security indication.
**
** Returns          void
**
*******************************************************************************/
void bta_av_security_ind (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_PROTECT_REQ protect_req;

    p_scb->avdt_label = p_data->str_msg.msg.hdr.label;

    if (bta_av_cb.features & BTA_AV_FEAT_PROTECT) {
        protect_req.chnl    = p_scb->chnl;
        protect_req.hndl    = p_scb->hndl;
        /*
        APPL_TRACE_EVENT("sec ind handle: x%x", protect_req.hndl);
        */
        protect_req.p_data  = p_data->str_msg.msg.security_ind.p_data;
        protect_req.len     = p_data->str_msg.msg.security_ind.len;

        (*bta_av_cb.p_cback)(BTA_AV_PROTECT_REQ_EVT, (tBTA_AV *) &protect_req);
    }
    /* app doesn't support security indication; respond with failure */
    else {
        AVDT_SecurityRsp(p_scb->avdt_handle, p_scb->avdt_label, AVDT_ERR_NSC, NULL, 0);
    }
}

/*******************************************************************************
**
** Function         bta_av_security_cfm
**
** Description      Handle an AVDTP security confirm.
**
** Returns          void
**
*******************************************************************************/
void bta_av_security_cfm (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_PROTECT_RSP protect_rsp;

    if (bta_av_cb.features & BTA_AV_FEAT_PROTECT) {
        protect_rsp.chnl    = p_scb->chnl;
        protect_rsp.hndl    = p_scb->hndl;
        protect_rsp.p_data  = p_data->str_msg.msg.security_cfm.p_data;
        protect_rsp.len     = p_data->str_msg.msg.security_cfm.len;
        protect_rsp.err_code = p_data->str_msg.msg.hdr.err_code;

        (*bta_av_cb.p_cback)(BTA_AV_PROTECT_RSP_EVT, (tBTA_AV *) &protect_rsp);
    }
}

/*******************************************************************************
**
** Function         bta_av_do_close
**
** Description      Close stream.
**
** Returns          void
**
*******************************************************************************/
void bta_av_do_close (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    UNUSED(p_data);

    /* stop stream if started */
    if (p_scb->co_started) {
        bta_av_str_stopped(p_scb, NULL);
    }
    bta_sys_stop_timer(&bta_av_cb.sig_tmr);

    /* close stream */
    p_scb->started = FALSE;

    /* drop the buffers queued in L2CAP */
    L2CA_FlushChannel (p_scb->l2c_cid, L2CAP_FLUSH_CHANS_ALL);

    AVDT_CloseReq(p_scb->avdt_handle);
    /* just in case that the link is congested, link is flow controlled by peer or
     * for whatever reason the the close request can not be sent in time.
     * when this timer expires, AVDT_DisconnectReq will be called to disconnect the link
     */
    bta_sys_start_timer(&p_scb->timer,
                        (UINT16)BTA_AV_API_CLOSE_EVT,
                        BTA_AV_CLOSE_REQ_TIME_VAL);

}

/*******************************************************************************
**
** Function         bta_av_connect_req
**
** Description      Connect AVDTP connection.
**
** Returns          void
**
*******************************************************************************/
void bta_av_connect_req (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    UNUSED(p_data);

    utl_freebuf((void **) &p_scb->p_disc_db);

    if (p_scb->coll_mask & BTA_AV_COLL_INC_TMR) {
        /* SNK initiated L2C connection while SRC was doing SDP.    */
        /* Wait until timeout to check if SNK starts signalling.    */
        APPL_TRACE_EVENT("bta_av_connect_req: coll_mask = 0x%2X", p_scb->coll_mask);
        return;
    }

    AVDT_ConnectReq(p_scb->peer_addr, p_scb->sec_mask, bta_av_dt_cback[p_scb->hdi]);
}

/*******************************************************************************
**
** Function         bta_av_sdp_failed
**
** Description      Service discovery failed.
**
** Returns          void
**
*******************************************************************************/
void bta_av_sdp_failed (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    if (!p_scb->open_status) {
        p_scb->open_status = BTA_AV_FAIL_SDP;
    }

    utl_freebuf((void **) &p_scb->p_disc_db);
    bta_av_str_closed(p_scb, p_data);
}

/*******************************************************************************
**
** Function         bta_av_disc_results
**
** Description      Handle the AVDTP discover results.  Search through the
**                  results and find the first available stream, and get
**                  its capabilities.
**
** Returns          void
**
*******************************************************************************/
void bta_av_disc_results (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    UINT8 num_snks = 0, num_srcs = 0, i;
    /* our uuid in case we initiate connection */
    UINT16 uuid_int = p_scb->uuid_int;

    APPL_TRACE_DEBUG(" initiator UUID 0x%x", uuid_int);
    /* store number of stream endpoints returned */
    p_scb->num_seps = p_data->str_msg.msg.discover_cfm.num_seps;

    UINT8 num_seps = (p_scb->num_seps < BTA_AV_NUM_SEPS) ? p_scb->num_seps : BTA_AV_NUM_SEPS;
    memcpy(p_scb->sep_info, p_data->str_msg.msg.discover_cfm.p_sep_info, sizeof(tAVDT_SEP_INFO) * num_seps);
    for (i = 0; i < p_data->str_msg.msg.discover_cfm.num_seps; i++) {
        APPL_TRACE_DEBUG("peer sep %d, in use %d, seid %d, media type %d,  tsep %d",
                           i,
                           p_data->str_msg.msg.discover_cfm.p_sep_info[i].in_use,
                           p_data->str_msg.msg.discover_cfm.p_sep_info[i].seid,
                           p_data->str_msg.msg.discover_cfm.p_sep_info[i].media_type,
                           p_data->str_msg.msg.discover_cfm.p_sep_info[i].tsep
                           );
    }
    for (i = 0; i < p_scb->num_seps; i++) {
        /* steam not in use, is a sink, and is audio */
        if ((p_scb->sep_info[i].in_use == FALSE) &&
                (p_scb->sep_info[i].media_type == p_scb->media_type)) {
            if ((p_scb->sep_info[i].tsep == AVDT_TSEP_SNK) &&
                    (uuid_int == UUID_SERVCLASS_AUDIO_SOURCE)) {
                num_snks++;
            }

            if ((p_scb->sep_info[i].tsep == AVDT_TSEP_SRC) &&
                    (uuid_int == UUID_SERVCLASS_AUDIO_SINK)) {
                num_srcs++;
            }
            APPL_TRACE_DEBUG("num srcs: %d, num_snks: %d\n", num_snks, num_srcs);
        }
    }

    p_scb->p_cos->disc_res(p_scb->hndl, p_scb->num_seps, num_snks, num_srcs, p_scb->peer_addr,
                           uuid_int);
    p_scb->num_disc_snks = num_snks;
    p_scb->num_disc_srcs = num_srcs;

    /* if we got any */
    if (p_scb->num_seps > 0) {
        /* initialize index into discovery results */
        p_scb->sep_info_idx = 0;

        /* get the capabilities of the first available stream */
        bta_av_next_getcap(p_scb, p_data);
    }
    /* else we got discover response but with no streams; we're done */
    else {
        bta_av_ssm_execute(p_scb, BTA_AV_STR_DISC_FAIL_EVT, p_data);
    }
}

/*******************************************************************************
**
** Function         bta_av_disc_res_as_acp
**
** Description      Handle the AVDTP discover results.  Search through the
**                  results and find the first available stream, and get
**                  its capabilities.
**
** Returns          void
**
*******************************************************************************/
void bta_av_disc_res_as_acp (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    UINT8 num_snks = 0, i;

    /* store number of stream endpoints returned */
    p_scb->num_seps = p_data->str_msg.msg.discover_cfm.num_seps;



    for (i = 0; i < p_scb->num_seps; i++) {
        /* steam is a sink, and is audio */
        if ((p_scb->sep_info[i].tsep == AVDT_TSEP_SNK) &&
                (p_scb->sep_info[i].media_type == p_scb->media_type)) {
            p_scb->sep_info[i].in_use = FALSE;
            num_snks++;
        }
    }
    p_scb->p_cos->disc_res(p_scb->hndl, p_scb->num_seps, num_snks, 0, p_scb->peer_addr,
                           UUID_SERVCLASS_AUDIO_SOURCE);
    p_scb->num_disc_snks = num_snks;
    p_scb->num_disc_srcs = 0;

    /* if we got any */
    if (p_scb->num_seps > 0) {
        /* initialize index into discovery results */
        p_scb->sep_info_idx = 0;

        /* get the capabilities of the first available stream */
        bta_av_next_getcap(p_scb, p_data);
    }
    /* else we got discover response but with no streams; we're done */
    else {
        bta_av_ssm_execute(p_scb, BTA_AV_STR_DISC_FAIL_EVT, p_data);
    }
}

/*******************************************************************************
**
** Function         bta_av_save_caps
**
** Description      report the SNK SEP capabilities to application
**
** Returns          void
**
*******************************************************************************/
void bta_av_save_caps(tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    tAVDT_CFG   cfg;
    tAVDT_SEP_INFO  *p_info = &p_scb->sep_info[p_scb->sep_info_idx];
    UINT8       old_wait = p_scb->wait;
    BOOLEAN     getcap_done = FALSE;

    APPL_TRACE_DEBUG("bta_av_save_caps num_seps:%d sep_info_idx:%d wait:x%x",
                     p_scb->num_seps, p_scb->sep_info_idx, p_scb->wait);
    memcpy(&cfg, p_scb->p_cap, sizeof(tAVDT_CFG));
    /* let application know the capability of the SNK */
    p_scb->p_cos->getcfg(p_scb->hndl, cfg.codec_info[BTA_AV_CODEC_TYPE_IDX],
                         cfg.codec_info, &p_scb->sep_info_idx, p_info->seid,
                         &cfg.num_protect, cfg.protect_info);

    p_scb->sep_info_idx++;
    if (p_scb->num_seps > p_scb->sep_info_idx) {
        /* Some devices have seps at the end of the discover list, which is not */
        /* matching media type(video not audio).                                */
        /* In this case, we are done with getcap without sending another        */
        /* request to AVDT.                                                     */
        if (!bta_av_next_getcap(p_scb, p_data)) {
            getcap_done = TRUE;
        }
    } else {
        getcap_done = TRUE;
    }

    if (getcap_done) {
        /* we are done getting capabilities. restore the p_cb->sep_info_idx */
        p_scb->sep_info_idx = 0;
        p_scb->wait &= ~(BTA_AV_WAIT_ACP_CAPS_ON | BTA_AV_WAIT_ACP_CAPS_STARTED);
        if (old_wait & BTA_AV_WAIT_ACP_CAPS_STARTED) {
            bta_av_start_ok (p_scb, NULL);
        }
    }
}

/*******************************************************************************
**
** Function         bta_av_set_use_rc
**
** Description      set to use AVRC for this stream control block.
**
** Returns          void
**
*******************************************************************************/
void bta_av_set_use_rc (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    UNUSED(p_data);

    p_scb->use_rc = TRUE;
}

/*******************************************************************************
**
** Function         bta_av_cco_close
**
** Description      call close call-out function.
**
** Returns          void
**
*******************************************************************************/
void bta_av_cco_close (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    UINT16 mtu;
    UNUSED(p_data);

    mtu = bta_av_chk_mtu(p_scb, BTA_AV_MAX_A2DP_MTU);

    p_scb->p_cos->close(p_scb->hndl, p_scb->codec_type, mtu);
}

/*******************************************************************************
**
** Function         bta_av_open_failed
**
** Description      Failed to open an AVDT stream
**
** Returns          void
**
*******************************************************************************/
void bta_av_open_failed (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{

    BOOLEAN is_av_opened = FALSE;
    tBTA_AV_SCB *p_opened_scb = NULL;
    UINT8 idx;
    tBTA_AV_OPEN    open;

    APPL_TRACE_DEBUG("bta_av_open_failed");
    p_scb->open_status = BTA_AV_FAIL_STREAM;
    bta_av_cco_close(p_scb, p_data);

    /* check whether there is already an opened audio or video connection with the same device */
    for (idx = 0; (idx < BTA_AV_NUM_STRS) && (is_av_opened == FALSE); idx++ ) {
        p_opened_scb = bta_av_cb.p_scb[idx];
        if (p_opened_scb && (p_opened_scb->state == BTA_AV_OPEN_SST) && (!bdcmp(p_opened_scb->peer_addr, p_scb->peer_addr )) ) {
            is_av_opened = TRUE;
        }

    }

    /* if there is already an active AV connection with the same bd_addr,
       don't send disconnect req, just report the open event with BTA_AV_FAIL_GET_CAP status */
    if (is_av_opened == TRUE) {
        bdcpy(open.bd_addr, p_scb->peer_addr);
        open.chnl   = p_scb->chnl;
        open.hndl   = p_scb->hndl;
        open.status = BTA_AV_FAIL_GET_CAP;
        open.starting = bta_av_chk_start(p_scb);
        open.edr    = 0;
        /* set the state back to initial state */
        bta_av_set_scb_sst_init(p_scb);

        if (p_scb->seps[p_scb->sep_idx].tsep == AVDT_TSEP_SRC ) {
            open.sep = AVDT_TSEP_SNK;
        } else if (p_scb->seps[p_scb->sep_idx].tsep == AVDT_TSEP_SNK ) {
            open.sep = AVDT_TSEP_SRC;
        }

        (*bta_av_cb.p_cback)(BTA_AV_OPEN_EVT, (tBTA_AV *) &open);

    } else {
        AVDT_DisconnectReq(p_scb->peer_addr, bta_av_dt_cback[p_scb->hdi]);
    }
}


/*******************************************************************************
**
** Function         bta_av_getcap_results
**
** Description      Handle the AVDTP get capabilities results.  Check the codec
**                  type and see if it matches ours.  If it does not, get the
**                  capabilities of the next stream, if any.
**
** Returns          void
**
*******************************************************************************/
void bta_av_getcap_results (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    tAVDT_CFG   cfg;
    UINT8       media_type;
    tAVDT_SEP_INFO  *p_info = &p_scb->sep_info[p_scb->sep_info_idx];
    UINT16 uuid_int; /* UUID for which connection was initiated */
    tBTA_AV_SNK_PSC_CFG psc_cfg = {0};

    memcpy(&cfg, &p_scb->cfg, sizeof(tAVDT_CFG));
    cfg.num_codec = 1;
    cfg.num_protect = p_scb->p_cap->num_protect;
    memcpy(cfg.codec_info, p_scb->p_cap->codec_info, AVDT_CODEC_SIZE);
    memcpy(cfg.protect_info, p_scb->p_cap->protect_info, AVDT_PROTECT_SIZE);
    media_type = p_scb->p_cap->codec_info[BTA_AV_MEDIA_TYPE_IDX] >> 4;

    APPL_TRACE_DEBUG("num_codec %d", p_scb->p_cap->num_codec);
    APPL_TRACE_DEBUG("media type x%x, x%x", media_type, p_scb->media_type);
#if AVDT_MULTIPLEXING == TRUE
    APPL_TRACE_DEBUG("mux x%x, x%x", cfg.mux_mask, p_scb->p_cap->mux_mask);
#endif

    /* if codec present and we get a codec configuration */
    if ((p_scb->p_cap->num_codec != 0) &&
            (media_type == p_scb->media_type) &&
            (p_scb->p_cos->getcfg(p_scb->hndl, p_scb->p_cap->codec_info[BTA_AV_CODEC_TYPE_IDX],
                                  cfg.codec_info, &p_scb->sep_info_idx, p_info->seid,
                                  &cfg.num_protect, cfg.protect_info) == 0)) {
#if AVDT_MULTIPLEXING == TRUE
        cfg.mux_mask &= p_scb->p_cap->mux_mask;
        APPL_TRACE_DEBUG("mux_mask used x%x", cfg.mux_mask);
#endif
        /* save copy of codec type and configuration */
        p_scb->codec_type = cfg.codec_info[BTA_AV_CODEC_TYPE_IDX];
        memcpy(&p_scb->cfg, &cfg, sizeof(tAVDT_CFG));

        uuid_int = p_scb->uuid_int;
        APPL_TRACE_DEBUG(" initiator UUID = 0x%x ", uuid_int);
        if (uuid_int == UUID_SERVCLASS_AUDIO_SOURCE) {
            bta_av_adjust_seps_idx(p_scb, bta_av_get_scb_handle(p_scb, AVDT_TSEP_SRC));
        } else if (uuid_int == UUID_SERVCLASS_AUDIO_SINK) {
            bta_av_adjust_seps_idx(p_scb, bta_av_get_scb_handle(p_scb, AVDT_TSEP_SNK));
        }

        /* use only the services peer supports */
        cfg.psc_mask &= p_scb->p_cap->psc_mask;
        p_scb->cur_psc_mask = cfg.psc_mask;

        if (p_scb->seps[p_scb->sep_idx].p_app_data_cback != NULL) {
            APPL_TRACE_DEBUG(" Configure Deoder for A2DP Connection ");
            p_scb->seps[p_scb->sep_idx].p_app_data_cback(BTA_AV_MEDIA_CFG_EVT,
                    (tBTA_AV_MEDIA *)p_scb->cfg.codec_info);
        }

        if (uuid_int == UUID_SERVCLASS_AUDIO_SINK) {
            if (p_scb->cur_psc_mask & AVDT_PSC_DELAY_RPT) {
                psc_cfg.psc_mask |= BTA_AV_PSC_DEALY_RPT;
            }
            (*bta_av_cb.p_cback)(BTA_AV_SNK_PSC_CFG_EVT, (tBTA_AV *)&psc_cfg);
        }

        /* open the stream */
        AVDT_OpenReq(p_scb->seps[p_scb->sep_idx].av_handle, p_scb->peer_addr,
                     p_scb->sep_info[p_scb->sep_info_idx].seid, &cfg);

        if (!bta_av_is_rcfg_sst(p_scb)) {
            /* free capabilities buffer */
            utl_freebuf((void **) &p_scb->p_cap);
        }
    } else {
        /* try the next stream, if any */
        p_scb->sep_info_idx++;
        bta_av_next_getcap(p_scb, p_data);
    }
}

/*******************************************************************************
**
** Function         bta_av_setconfig_rej
**
** Description      Send AVDTP set config reject.
**
** Returns          void
**
*******************************************************************************/
void bta_av_setconfig_rej (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_REJECT reject;
    UINT8 err_code = p_data->ci_setconfig.err_code;

    if (err_code == AVDT_SUCCESS) {
        err_code = AVDT_ERR_UNSUP_CFG;
    }

    bta_av_adjust_seps_idx(p_scb, p_scb->avdt_handle);
    APPL_TRACE_DEBUG("bta_av_setconfig_rej: sep_idx: %d", p_scb->sep_idx);
    AVDT_ConfigRsp(p_scb->avdt_handle, p_scb->avdt_label, err_code, 0);

    bdcpy(reject.bd_addr, p_scb->peer_addr);
    reject.hndl = p_scb->hndl;
    (*bta_av_cb.p_cback)(BTA_AV_REJECT_EVT, (tBTA_AV *) &reject);
}

/*******************************************************************************
**
** Function         bta_av_discover_req
**
** Description      Send an AVDTP discover request to the peer.
**
** Returns          void
**
*******************************************************************************/
void bta_av_discover_req (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    UNUSED(p_data);

    /* send avdtp discover request */

    AVDT_DiscoverReq(p_scb->peer_addr, p_scb->sep_info, BTA_AV_NUM_SEPS, bta_av_dt_cback[p_scb->hdi]);
}

/*******************************************************************************
**
** Function         bta_av_conn_failed
**
** Description      AVDTP connection failed.
**
** Returns          void
**
*******************************************************************************/
void bta_av_conn_failed (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    p_scb->open_status = BTA_AV_FAIL_STREAM;
    bta_av_str_closed(p_scb, p_data);
}

/*******************************************************************************
**
** Function         bta_av_do_start
**
** Description      Start stream.
**
** Returns          void
**
*******************************************************************************/
void bta_av_do_start (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    UINT8 policy = HCI_ENABLE_SNIFF_MODE;
    UINT8       cur_role;

    APPL_TRACE_DEBUG("bta_av_do_start sco_occupied:%d, role:x%x, started:%d", bta_av_cb.sco_occupied, p_scb->role, p_scb->started);
    if (bta_av_cb.sco_occupied) {
        bta_av_start_failed(p_scb, p_data);
        return;
    }

    /* disallow role switch during streaming, only if we are the master role
     * i.e. allow role switch, if we are slave.
     * It would not hurt us, if the peer device wants us to be master */
    if ((BTM_GetRole (p_scb->peer_addr, &cur_role) == BTM_SUCCESS) &&
            (cur_role == BTM_ROLE_MASTER) ) {
        policy |= HCI_ENABLE_MASTER_SLAVE_SWITCH;
    }

    bta_sys_clear_policy(BTA_ID_AV, policy, p_scb->peer_addr);

    if ((p_scb->started == FALSE) && ((p_scb->role & BTA_AV_ROLE_START_INT) == 0)) {
        p_scb->role |= BTA_AV_ROLE_START_INT;
        bta_sys_busy(TSEP_TO_SYS_ID(p_scb->seps[p_scb->sep_idx].tsep), bta_av_cb.audio_open_cnt, p_scb->peer_addr);
        AVDT_StartReq(&p_scb->avdt_handle, 1);
    } else if (p_scb->started) {
        p_scb->role |= BTA_AV_ROLE_START_INT;
        if ( p_scb->wait == 0 ) {
            if (p_scb->role & BTA_AV_ROLE_SUSPEND) {
                notify_start_failed(p_scb);
            } else {
                bta_av_start_ok(p_scb, NULL);
            }
        }
    }
    APPL_TRACE_DEBUG("started %d role:x%x", p_scb->started, p_scb->role);
}

/*******************************************************************************
**
** Function         bta_av_str_stopped
**
** Description      Stream stopped.
**
** Returns          void
**
*******************************************************************************/
void bta_av_str_stopped (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_SUSPEND suspend_rsp;
    UINT8   start = p_scb->started;
    BOOLEAN sus_evt = TRUE;
    BT_HDR  *p_buf;
    UINT8 policy = HCI_ENABLE_SNIFF_MODE;

    APPL_TRACE_DEBUG("bta_av_str_stopped:audio_open_cnt=%d, p_data %p",
                     bta_av_cb.audio_open_cnt, p_data);

    bta_sys_idle(TSEP_TO_SYS_ID(p_scb->seps[p_scb->sep_idx].tsep), bta_av_cb.audio_open_cnt, p_scb->peer_addr);
    if ((bta_av_cb.features & BTA_AV_FEAT_MASTER) == 0 || bta_av_cb.audio_open_cnt == 1) {
        policy |= HCI_ENABLE_MASTER_SLAVE_SWITCH;
    }
    bta_sys_set_policy(BTA_ID_AV, policy, p_scb->peer_addr);

    if (p_scb->co_started) {
        bta_av_stream_chg(p_scb, FALSE);
        p_scb->co_started = FALSE;

        p_scb->p_cos->stop(p_scb->hndl, p_scb->codec_type);
        L2CA_SetFlushTimeout(p_scb->peer_addr, L2CAP_DEFAULT_FLUSH_TO);
    }

    /* if q_info.a2d_list is not empty, drop it now */
    if (BTA_AV_CHNL_AUDIO == p_scb->chnl) {
        while (!list_is_empty(p_scb->a2d_list)) {
            p_buf = (BT_HDR *)list_front(p_scb->a2d_list);
            list_remove(p_scb->a2d_list, p_buf);
            osi_free(p_buf);
        }

        /* drop the audio buffers queued in L2CAP */
        if (p_data && p_data->api_stop.flush) {
            L2CA_FlushChannel (p_scb->l2c_cid, L2CAP_FLUSH_CHANS_ALL);
        }
    }

    suspend_rsp.chnl = p_scb->chnl;
    suspend_rsp.hndl = p_scb->hndl;

    if (p_data && p_data->api_stop.suspend) {
        APPL_TRACE_DEBUG("suspending: %d, sup:%d", start, p_scb->suspend_sup);
        if ((start)  && (p_scb->suspend_sup)) {
            sus_evt = FALSE;
            p_scb->l2c_bufs = 0;
            AVDT_SuspendReq(&p_scb->avdt_handle, 1);
        }

        /* send SUSPEND_EVT event only if not in reconfiguring state and sus_evt is TRUE*/
        if ((sus_evt) && (p_scb->state != BTA_AV_RCFG_SST)) {
            suspend_rsp.status = BTA_AV_SUCCESS;
            suspend_rsp.initiator = TRUE;
            (*bta_av_cb.p_cback)(BTA_AV_SUSPEND_EVT, (tBTA_AV *) &suspend_rsp);
        }
    } else {
        suspend_rsp.status = BTA_AV_SUCCESS;
        suspend_rsp.initiator = TRUE;
        APPL_TRACE_WARNING("bta_av_str_stopped status %d", suspend_rsp.status);

        /* send STOP_EVT event only if not in reconfiguring state */
        if (p_scb->state != BTA_AV_RCFG_SST) {
            (*bta_av_cb.p_cback)(BTA_AV_STOP_EVT, (tBTA_AV *) &suspend_rsp);
        }
    }
}

/*******************************************************************************
**
** Function         bta_av_reconfig
**
** Description      process the reconfigure request.
**                  save the parameter in control block and
**                  suspend, reconfigure or close the stream
**
** Returns          void
**
*******************************************************************************/
void bta_av_reconfig (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    tAVDT_CFG   *p_cfg;
    tBTA_AV_API_STOP    stop;
    tBTA_AV_RECONFIG    evt;
    tBTA_AV_API_RCFG    *p_rcfg = &p_data->api_reconfig;

    APPL_TRACE_DEBUG("bta_av_reconfig r:%d, s:%d idx: %d (o:%d)",
                     p_scb->recfg_sup, p_scb->suspend_sup,
                     p_scb->rcfg_idx, p_scb->sep_info_idx);

    p_scb->num_recfg = 0;
    /* store the new configuration in control block */
    if (p_scb->p_cap == NULL) {
        p_scb->p_cap = (tAVDT_CFG *) osi_malloc(sizeof(tAVDT_CFG));
    }
    if ((p_cfg = p_scb->p_cap) == NULL) {
        /* report failure */
        evt.status = BTA_AV_FAIL_RESOURCES;
        evt.chnl   = p_scb->chnl;
        evt.hndl   = p_scb->hndl;
        (*bta_av_cb.p_cback)(BTA_AV_RECONFIG_EVT, (tBTA_AV *)&evt);

        /* this event is not possible in this state.
         * use it to bring the SSM back to open state */
        bta_av_ssm_execute(p_scb, BTA_AV_SDP_DISC_OK_EVT, NULL);
        return;
    }

    /*if(bta_av_cb.features & BTA_AV_FEAT_RCCT)*/
    bta_sys_stop_timer(&p_scb->timer);

    memcpy(p_cfg, &p_scb->cfg, sizeof(tAVDT_CFG));
    p_cfg->num_protect = p_rcfg->num_protect;
    memcpy(p_cfg->codec_info, p_rcfg->codec_info, AVDT_CODEC_SIZE);
    memcpy(p_cfg->protect_info, p_rcfg->p_protect_info, p_rcfg->num_protect);
    p_scb->rcfg_idx = p_rcfg->sep_info_idx;
    p_scb->p_cap->psc_mask = p_scb->cur_psc_mask;

    /* if the requested index differs from the current one, we can only close/open */
    if ((p_scb->rcfg_idx == p_scb->sep_info_idx) &&
            (p_rcfg->suspend) && (p_scb->recfg_sup) && (p_scb->suspend_sup)) {
        if (p_scb->started) {
            stop.flush   = FALSE;
            stop.suspend = TRUE;
            bta_av_str_stopped(p_scb, (tBTA_AV_DATA *)&stop);
        } else {
            APPL_TRACE_DEBUG("Reconfig");
            AVDT_ReconfigReq(p_scb->avdt_handle, p_scb->p_cap);
            p_scb->p_cap->psc_mask = p_scb->cur_psc_mask;
        }
    } else {
        /* close the stream */
        APPL_TRACE_DEBUG("close/open num_protect: %d", p_cfg->num_protect);
        if (p_scb->started) {
            bta_av_str_stopped(p_scb, NULL);
            p_scb->started = FALSE;

            /* drop the buffers queued in L2CAP */
            L2CA_FlushChannel (p_scb->l2c_cid, L2CAP_FLUSH_CHANS_ALL);

            AVDT_CloseReq(p_scb->avdt_handle);
        }
    }
}

/*******************************************************************************
**
** Function         bta_av_data_path
**
** Description      Handle stream data path.
**
** Returns          void
**
*******************************************************************************/
void bta_av_data_path (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    BT_HDR  *p_buf = NULL;
    UINT32  data_len;
    UINT32  timestamp;
    BOOLEAN new_buf = FALSE;
    UINT8   m_pt = 0x60 | p_scb->codec_type;
    tAVDT_DATA_OPT_MASK     opt;
    UNUSED(p_data);

    if (p_scb->cong) {
        return;
    }

    /*
    APPL_TRACE_ERROR("q: %d", p_scb->l2c_bufs);
    */
    //Always get the current number of bufs que'd up
    p_scb->l2c_bufs = (UINT8)L2CA_FlushChannel (p_scb->l2c_cid, L2CAP_FLUSH_CHANS_GET);

    if (!list_is_empty(p_scb->a2d_list)) {
        p_buf = (BT_HDR *)list_front(p_scb->a2d_list);
        list_remove(p_scb->a2d_list, p_buf);
        /* use q_info.a2d data, read the timestamp */
        timestamp = *(UINT32 *)(p_buf + 1);
    } else {
        new_buf = TRUE;
        /* a2d_list empty, call co_data, dup data to other channels */
        p_buf = (BT_HDR *)p_scb->p_cos->data(p_scb->codec_type, &data_len,
                                             &timestamp);

        if (p_buf) {
            /* use the offset area for the time stamp */
            *(UINT32 *)(p_buf + 1) = timestamp;

            /* dup the data to other channels */
            bta_av_dup_audio_buf(p_scb, p_buf);
        }
    }

    if (p_buf) {
        if (p_scb->l2c_bufs < (BTA_AV_QUEUE_DATA_CHK_NUM)) {
            /* there's a buffer, just queue it to L2CAP */
            /*  There's no need to increment it here, it is always read from L2CAP see above */
            /* p_scb->l2c_bufs++; */
            /*
            APPL_TRACE_ERROR("qw: %d", p_scb->l2c_bufs);
            */

            /* opt is a bit mask, it could have several options set */
            opt = AVDT_DATA_OPT_NONE;
            if (p_scb->no_rtp_hdr) {
                opt |= AVDT_DATA_OPT_NO_RTP;
            }

            AVDT_WriteReqOpt(p_scb->avdt_handle, p_buf, timestamp, m_pt, opt);
            p_scb->cong = TRUE;
        } else {
            /* there's a buffer, but L2CAP does not seem to be moving data */
            if (new_buf) {
                /* just got this buffer from co_data,
                 * put it in queue */
                list_append(p_scb->a2d_list, p_buf);
            } else {
                /* just dequeue it from the a2d_list */
                if (list_length(p_scb->a2d_list) < 3) {
                    /* put it back to the queue */
                    list_prepend(p_scb->a2d_list, p_buf);
                } else {
                    /* too many buffers in a2d_list, drop it. */
                    bta_av_co_audio_drop(p_scb->hndl);
                    osi_free(p_buf);
                }
            }
        }
    }
}

/*******************************************************************************
**
** Function         bta_av_start_ok
**
** Description      Stream started.
**
** Returns          void
**
*******************************************************************************/
void bta_av_start_ok (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_START   start;
    tBTA_AV_API_STOP stop;
    BOOLEAN         initiator = FALSE;
    BOOLEAN         suspend = FALSE;
    UINT16          flush_to;
    UINT8           new_role = p_scb->role;
    BT_HDR          hdr;
    UINT8           policy = HCI_ENABLE_SNIFF_MODE;
    UINT8           cur_role;

    APPL_TRACE_DEBUG("bta_av_start_ok wait:x%x, role:x%x", p_scb->wait, p_scb->role);

    p_scb->started = TRUE;
    if (p_scb->sco_suspend) {
        p_scb->sco_suspend = FALSE;
    }

    if (new_role & BTA_AV_ROLE_START_INT) {
        initiator = TRUE;
    }

    /* for A2DP SINK we do not send get_caps */
    if ((p_scb->avdt_handle == p_scb->seps[p_scb->sep_idx].av_handle)
            && (p_scb->seps[p_scb->sep_idx].tsep == AVDT_TSEP_SNK)) {
        p_scb->wait &= ~(BTA_AV_WAIT_ACP_CAPS_ON);
        APPL_TRACE_DEBUG(" Local SEP type is SNK  new wait is 0x%x", p_scb->wait);
    }
    if (p_scb->wait & BTA_AV_WAIT_ROLE_SW_FAILED) {
        /* role switch has failed */
        p_scb->wait &= ~BTA_AV_WAIT_ROLE_SW_FAILED;
        p_data = (tBTA_AV_DATA *)&hdr;
        hdr.offset = BTA_AV_RS_FAIL;
    }
    APPL_TRACE_DEBUG("wait:x%x", p_scb->wait);

    if (p_data && (p_data->hdr.offset != BTA_AV_RS_NONE)) {
        p_scb->wait &= ~BTA_AV_WAIT_ROLE_SW_BITS;
        if (p_data->hdr.offset == BTA_AV_RS_FAIL) {
            bta_sys_idle(TSEP_TO_SYS_ID(p_scb->seps[p_scb->sep_idx].tsep), bta_av_cb.audio_open_cnt, p_scb->peer_addr);
            start.chnl   = p_scb->chnl;
            start.status = BTA_AV_FAIL_ROLE;
            start.hndl   = p_scb->hndl;
            start.initiator = initiator;
            (*bta_av_cb.p_cback)(BTA_AV_START_EVT, (tBTA_AV *) &start);
            return;
        }
    }

    if (!bta_av_link_role_ok(p_scb, A2D_SET_ONE_BIT)) {
        p_scb->q_tag = BTA_AV_Q_TAG_START;
    } else {
        /* The wait flag may be set here while we are already master on the link */
        /* this could happen if a role switch complete event occurred during reconfig */
        /* if we are now master on the link, there is no need to wait for the role switch, */
        /* complete anymore so we can clear the wait for role switch flag */
        p_scb->wait &= ~BTA_AV_WAIT_ROLE_SW_BITS;
    }

    if (p_scb->wait & (BTA_AV_WAIT_ROLE_SW_RES_OPEN | BTA_AV_WAIT_ROLE_SW_RES_START)) {
        p_scb->wait |= BTA_AV_WAIT_ROLE_SW_STARTED;
        p_scb->q_tag = BTA_AV_Q_TAG_START;
    }

    if (p_scb->wait) {
        APPL_TRACE_ERROR("wait:x%x q_tag:%d- not started", p_scb->wait, p_scb->q_tag);
        /* Clear first bit of p_scb->wait and not to return from this point else
         * HAL layer gets blocked. And if there is delay in Get Capability response as
         * first bit of p_scb->wait is cleared hence it ensures bt_av_start_ok is not called
         * again from bta_av_save_caps.
        */
        p_scb->wait &= ~BTA_AV_WAIT_ACP_CAPS_ON;
    }

    /* tell role manager to check M/S role */
    bta_sys_conn_open(TSEP_TO_SYS_ID(p_scb->seps[p_scb->sep_idx].tsep), p_scb->app_id, p_scb->peer_addr);

    bta_sys_busy(TSEP_TO_SYS_ID(p_scb->seps[p_scb->sep_idx].tsep), bta_av_cb.audio_open_cnt, p_scb->peer_addr);

    if (p_scb->media_type == AVDT_MEDIA_AUDIO) {
        /* in normal logic, conns should be bta_av_cb.audio_count - 1,
         * However, bta_av_stream_chg is not called to increase bta_av_cb.audio_count yet.
         * If the code were to be re-arranged for some reasons, this number may need to be changed
         */
        p_scb->co_started = bta_av_cb.audio_open_cnt;
        flush_to = p_bta_av_cfg->p_audio_flush_to[p_scb->co_started - 1];
    } else {
        flush_to = p_bta_av_cfg->video_flush_to;
    }
    L2CA_SetFlushTimeout(p_scb->peer_addr, flush_to );

    /* clear the congestion flag */
    p_scb->cong = FALSE;

    if (new_role & BTA_AV_ROLE_START_INT) {
        new_role &= ~BTA_AV_ROLE_START_INT;
    } else if ((new_role & BTA_AV_ROLE_AD_ACP) && (new_role & BTA_AV_ROLE_SUSPEND_OPT)) {
        suspend = TRUE;
    }

    if (!suspend) {
        p_scb->q_tag = BTA_AV_Q_TAG_STREAM;
        bta_av_stream_chg(p_scb, TRUE);
    }

    {
        /* If sink starts stream, disable sniff mode here */
        if (!initiator) {
            /* If source is the master role, disable role switch during streaming.
            * Otherwise allow role switch, if source is slave.
            * Because it would not hurt source, if the peer device wants source to be master */
            if ((BTM_GetRole (p_scb->peer_addr, &cur_role) == BTM_SUCCESS) &&
                    (cur_role == BTM_ROLE_MASTER) ) {
                policy |= HCI_ENABLE_MASTER_SLAVE_SWITCH;
            }

            bta_sys_clear_policy(BTA_ID_AV, policy, p_scb->peer_addr);
        }

        p_scb->role = new_role;
        p_scb->role &= ~BTA_AV_ROLE_AD_ACP;
        p_scb->role &= ~BTA_AV_ROLE_SUSPEND_OPT;

        p_scb->no_rtp_hdr = FALSE;
        p_scb->p_cos->start(p_scb->hndl, p_scb->codec_type, p_scb->cfg.codec_info, &p_scb->no_rtp_hdr);
        p_scb->co_started = TRUE;

        APPL_TRACE_DEBUG("bta_av_start_ok suspending: %d, role:x%x, init %d",
                         suspend, p_scb->role, initiator);

        start.suspending = suspend;
        start.initiator = initiator;
        start.chnl   = p_scb->chnl;
        start.status = BTA_AV_SUCCESS;
        start.hndl   = p_scb->hndl;
        (*bta_av_cb.p_cback)(BTA_AV_START_EVT, (tBTA_AV *) &start);

        if (suspend) {
            p_scb->role |= BTA_AV_ROLE_SUSPEND;
            p_scb->cong = TRUE;  /* do not allow the media data to go through */
            /* do not duplicate the media packets to this channel */
            p_scb->p_cos->stop(p_scb->hndl, p_scb->codec_type);
            p_scb->co_started = FALSE;
            stop.flush   = FALSE;
            stop.suspend = TRUE;
            bta_av_ssm_execute(p_scb, BTA_AV_AP_STOP_EVT, (tBTA_AV_DATA *)&stop);
        }
    }
}

/*******************************************************************************
**
** Function         bta_av_start_failed
**
** Description      Stream start failed.
**
** Returns          void
**
*******************************************************************************/
void bta_av_start_failed (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    UNUSED(p_data);

    if (p_scb->started == FALSE && p_scb->co_started == FALSE) {
        bta_sys_idle(TSEP_TO_SYS_ID(p_scb->seps[p_scb->sep_idx].tsep), bta_av_cb.audio_open_cnt, p_scb->peer_addr);
        notify_start_failed(p_scb);
    }

    bta_sys_set_policy(BTA_ID_AV, (HCI_ENABLE_SNIFF_MODE | HCI_ENABLE_MASTER_SLAVE_SWITCH), p_scb->peer_addr);
    p_scb->sco_suspend = FALSE;
}

/*******************************************************************************
**
** Function         bta_av_str_closed
**
** Description      Stream closed.
**
** Returns          void
**
*******************************************************************************/
void bta_av_str_closed (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    tBTA_AV     data;
    tBTA_AV_EVT event;
    UINT16      mtu;
    UINT8 policy = HCI_ENABLE_SNIFF_MODE;

    if ((bta_av_cb.features & BTA_AV_FEAT_MASTER) == 0 || bta_av_cb.audio_open_cnt == 1) {
        policy |= HCI_ENABLE_MASTER_SLAVE_SWITCH;
    }
    bta_sys_set_policy(BTA_ID_AV, policy, p_scb->peer_addr);
    if (bta_av_cb.audio_open_cnt <= 1) {
        /* last connection - restore the allow switch flag */
        L2CA_SetDesireRole(L2CAP_ROLE_ALLOW_SWITCH);
    }

    if (p_scb->open_status) {
        /* must be failure when opening the stream */
        bdcpy(data.open.bd_addr, p_scb->peer_addr);
        data.open.status = p_scb->open_status;
        data.open.chnl   = p_scb->chnl;
        data.open.hndl   = p_scb->hndl;

        if (p_scb->seps[p_scb->sep_idx].tsep == AVDT_TSEP_SRC ) {
            data.open.sep = AVDT_TSEP_SNK;
        } else if (p_scb->seps[p_scb->sep_idx].tsep == AVDT_TSEP_SNK ) {
            data.open.sep = AVDT_TSEP_SRC;
        }

        event = BTA_AV_OPEN_EVT;
        p_scb->open_status = BTA_AV_SUCCESS;

        bta_sys_conn_close(TSEP_TO_SYS_ID(p_scb->seps[p_scb->sep_idx].tsep), p_scb->app_id, p_scb->peer_addr);

        bta_av_cleanup(p_scb, p_data);
        (*bta_av_cb.p_cback)(event, &data);
    } else {
        /* do stop if we were started */
        if (p_scb->co_started) {
            bta_av_str_stopped(p_scb, NULL);
        }

        /* Update common mtu shared by remaining connections */
        mtu = bta_av_chk_mtu(p_scb, BTA_AV_MAX_A2DP_MTU);

        {
            p_scb->p_cos->close(p_scb->hndl, p_scb->codec_type, mtu);
            data.close.chnl = p_scb->chnl;
            data.close.hndl = p_scb->hndl;
            data.close.disc_rsn = p_scb->disc_rsn;
            event = BTA_AV_CLOSE_EVT;

            bta_sys_conn_close(TSEP_TO_SYS_ID(p_scb->seps[p_scb->sep_idx].tsep), p_scb->app_id, p_scb->peer_addr);

            bta_av_cleanup(p_scb, p_data);
            (*bta_av_cb.p_cback)(event, &data);
        }
    }
}

/*******************************************************************************
**
** Function         bta_av_clr_cong
**
** Description      Clear stream congestion flag.
**
** Returns          void
**
*******************************************************************************/
void bta_av_clr_cong (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    UNUSED(p_data);

    if (p_scb->co_started) {
        p_scb->cong = FALSE;
    }
}

/*******************************************************************************
**
** Function         bta_av_suspend_cfm
**
** Description      process the suspend response
**
** Returns          void
**
*******************************************************************************/
void bta_av_suspend_cfm (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_SUSPEND suspend_rsp;
    UINT8           err_code = p_data->str_msg.msg.hdr.err_code;
    UINT8 policy = HCI_ENABLE_SNIFF_MODE;

    APPL_TRACE_DEBUG ("bta_av_suspend_cfm:audio_open_cnt = %d, err_code = %d",
                      bta_av_cb.audio_open_cnt, err_code);

    if (p_scb->started == FALSE) {
        /* handle the condition where there is a collision of SUSPEND req from either side
        ** Second SUSPEND req could be rejected. Do not treat this as a failure
        */
        APPL_TRACE_WARNING("bta_av_suspend_cfm: already suspended, ignore, err_code %d",
                           err_code);
        return;
    }

    suspend_rsp.status = BTA_AV_SUCCESS;
    if (err_code && (err_code != AVDT_ERR_BAD_STATE)) {
        /* Disable suspend feature only with explicit rejection(not with timeout) */
        if (err_code != AVDT_ERR_TIMEOUT) {
            p_scb->suspend_sup = FALSE;
        }
        suspend_rsp.status = BTA_AV_FAIL;

        APPL_TRACE_ERROR ("bta_av_suspend_cfm: suspend failed, closing connection");

        /* SUSPEND failed. Close connection. */
        bta_av_ssm_execute(p_scb, BTA_AV_API_CLOSE_EVT, NULL);
    } else {
        /* only set started to FALSE when suspend is successful */
        p_scb->started = FALSE;
    }

    if (p_scb->role & BTA_AV_ROLE_SUSPEND) {
        p_scb->role &= ~BTA_AV_ROLE_SUSPEND;
        p_scb->cong = FALSE;
    }

    bta_sys_idle(TSEP_TO_SYS_ID(p_scb->seps[p_scb->sep_idx].tsep), bta_av_cb.audio_open_cnt, p_scb->peer_addr);
    if ((bta_av_cb.features & BTA_AV_FEAT_MASTER) == 0 || bta_av_cb.audio_open_cnt == 1) {
        policy |= HCI_ENABLE_MASTER_SLAVE_SWITCH;
    }
    bta_sys_set_policy(BTA_ID_AV, policy, p_scb->peer_addr);

    /* in case that we received suspend_ind, we may need to call co_stop here */
    if (p_scb->co_started) {
        bta_av_stream_chg(p_scb, FALSE);

        {
            p_scb->co_started = FALSE;
            p_scb->p_cos->stop(p_scb->hndl, p_scb->codec_type);
        }
        L2CA_SetFlushTimeout(p_scb->peer_addr, L2CAP_DEFAULT_FLUSH_TO);
    }

    {
        suspend_rsp.chnl = p_scb->chnl;
        suspend_rsp.hndl = p_scb->hndl;
        suspend_rsp.initiator = p_data->str_msg.initiator;
        (*bta_av_cb.p_cback)(BTA_AV_SUSPEND_EVT, (tBTA_AV *) &suspend_rsp);
    }
}

/*******************************************************************************
**
** Function         bta_av_rcfg_str_ok
**
** Description      report reconfigure successful
**
** Returns          void
**
*******************************************************************************/
void bta_av_rcfg_str_ok (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_RECONFIG    evt;
    UNUSED(p_data);

    p_scb->l2c_cid      = AVDT_GetL2CapChannel(p_scb->avdt_handle);
    APPL_TRACE_DEBUG("bta_av_rcfg_str_ok: l2c_cid: %d", p_scb->l2c_cid);

    /* rc listen */
    bta_av_st_rc_timer(p_scb, NULL);
    utl_freebuf((void **)&p_scb->p_cap);

    /* No need to keep the role bits once reconfig is done. */
    p_scb->role &= ~BTA_AV_ROLE_AD_ACP;
    p_scb->role &= ~BTA_AV_ROLE_SUSPEND_OPT;
    p_scb->role &= ~BTA_AV_ROLE_START_INT;

    {
        /* reconfigure success  */
        evt.status = BTA_AV_SUCCESS;
        evt.chnl   = p_scb->chnl;
        evt.hndl   = p_scb->hndl;
        (*bta_av_cb.p_cback)(BTA_AV_RECONFIG_EVT, (tBTA_AV *)&evt);
    }
}

/*******************************************************************************
**
** Function         bta_av_rcfg_failed
**
** Description      process reconfigure failed
**
** Returns          void
**
*******************************************************************************/
void bta_av_rcfg_failed (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_RECONFIG evt;

    APPL_TRACE_DEBUG("bta_av_rcfg_failed num_recfg: %d, conn_lcb:0x%x",
                     p_scb->num_recfg, bta_av_cb.conn_lcb);
    if (p_scb->num_recfg > BTA_AV_RECONFIG_RETRY) {
        bta_av_cco_close(p_scb, p_data);
        /* report failure */
        evt.status = BTA_AV_FAIL_STREAM;
        evt.chnl   = p_scb->chnl;
        evt.hndl   = p_scb->hndl;
        (*bta_av_cb.p_cback)(BTA_AV_RECONFIG_EVT, (tBTA_AV *)&evt);
        /* go to closing state */
        bta_av_ssm_execute(p_scb, BTA_AV_API_CLOSE_EVT, NULL);
    } else {
        /* open failed. try again */
        p_scb->num_recfg++;
        if (bta_av_cb.conn_lcb) {
            AVDT_DisconnectReq(p_scb->peer_addr, bta_av_dt_cback[p_scb->hdi]);
        } else {
            bta_av_connect_req(p_scb, NULL);
        }
    }
}

/*******************************************************************************
**
** Function         bta_av_rcfg_connect
**
** Description      stream closed. reconnect the stream
**
** Returns          void
**
*******************************************************************************/
void bta_av_rcfg_connect (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    UNUSED(p_data);

    p_scb->cong    = FALSE;
    p_scb->num_recfg++;
    APPL_TRACE_DEBUG("bta_av_rcfg_connect num_recfg: %d", p_scb->num_recfg);
    if (p_scb->num_recfg > BTA_AV_RECONFIG_RETRY) {
        /* let bta_av_rcfg_failed report fail */
        bta_av_rcfg_failed(p_scb, NULL);
    } else {
        AVDT_ConnectReq(p_scb->peer_addr, p_scb->sec_mask, bta_av_dt_cback[p_scb->hdi]);
    }
}

/*******************************************************************************
**
** Function         bta_av_rcfg_discntd
**
** Description      AVDT disconnected. reconnect the stream
**
** Returns          void
**
*******************************************************************************/
void bta_av_rcfg_discntd (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_RECONFIG    evt;
    UNUSED(p_data);

    APPL_TRACE_DEBUG("bta_av_rcfg_discntd num_recfg: %d", p_scb->num_recfg);
    p_scb->num_recfg++;
    if (p_scb->num_recfg > BTA_AV_RECONFIG_RETRY) {
        /* report failure */
        evt.status = BTA_AV_FAIL_STREAM;
        evt.chnl   = p_scb->chnl;
        evt.hndl   = p_scb->hndl;
        (*bta_av_cb.p_cback)(BTA_AV_RECONFIG_EVT, (tBTA_AV *)&evt);
        /* report close event & go to init state */
        bta_av_ssm_execute(p_scb, BTA_AV_STR_DISC_FAIL_EVT, NULL);
    } else {
        AVDT_ConnectReq(p_scb->peer_addr, p_scb->sec_mask, bta_av_dt_cback[p_scb->hdi]);
    }
}

/*******************************************************************************
**
** Function         bta_av_suspend_cont
**
** Description      received the suspend response.
**                  continue to reconfigure the stream
**
** Returns          void
**
*******************************************************************************/
void bta_av_suspend_cont (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    UINT8       err_code = p_data->str_msg.msg.hdr.err_code;
    tBTA_AV_RECONFIG    evt;

    p_scb->started = FALSE;
    p_scb->cong    = FALSE;
    if (err_code) {
        if (AVDT_ERR_CONNECT == err_code) {
            /* report failure */
            evt.status = BTA_AV_FAIL;
            (*bta_av_cb.p_cback)(BTA_AV_RECONFIG_EVT, (tBTA_AV *)&evt);
            bta_av_ssm_execute(p_scb, BTA_AV_STR_DISC_FAIL_EVT, NULL);
        } else {
            APPL_TRACE_ERROR("suspend rejected, try close");
            /* Disable suspend feature only with explicit rejection(not with timeout) */
            if (err_code != AVDT_ERR_TIMEOUT) {
                p_scb->suspend_sup = FALSE;
            }
            /* drop the buffers queued in L2CAP */
            L2CA_FlushChannel (p_scb->l2c_cid, L2CAP_FLUSH_CHANS_ALL);

            AVDT_CloseReq(p_scb->avdt_handle);
        }
    } else {
        APPL_TRACE_DEBUG("bta_av_suspend_cont calling AVDT_ReconfigReq");
        /* reconfig the stream */

        AVDT_ReconfigReq(p_scb->avdt_handle, p_scb->p_cap);
        p_scb->p_cap->psc_mask = p_scb->cur_psc_mask;
    }
}

/*******************************************************************************
**
** Function         bta_av_rcfg_cfm
**
** Description      if reconfigure is successful, report the event
**                  otherwise, close the stream.
**
** Returns          void
**
*******************************************************************************/
void bta_av_rcfg_cfm (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    UINT8   err_code = p_data->str_msg.msg.hdr.err_code;

    /*
    APPL_TRACE_DEBUG("bta_av_rcfg_cfm");
    */
    if (err_code) {
        APPL_TRACE_ERROR("reconfig rejected, try close");
        /* Disable reconfiguration feature only with explicit rejection(not with timeout) */
        if (err_code != AVDT_ERR_TIMEOUT) {
            p_scb->recfg_sup = FALSE;
        }
        /* started flag is FALSE when reconfigure command is sent */
        /* drop the buffers queued in L2CAP */
        L2CA_FlushChannel (p_scb->l2c_cid, L2CAP_FLUSH_CHANS_ALL);
        AVDT_CloseReq(p_scb->avdt_handle);
    } else {
        /* update the codec info after rcfg cfm */
        memcpy(p_scb->cfg.codec_info, p_data->str_msg.msg.reconfig_cfm.p_cfg->codec_info, AVDT_CODEC_SIZE);
        /* take the SSM back to OPEN state */
        bta_av_ssm_execute(p_scb, BTA_AV_STR_OPEN_OK_EVT, NULL);
    }
}

/*******************************************************************************
**
** Function         bta_av_rcfg_open
**
** Description      AVDT is connected. open the stream with the new configuration
**
** Returns          void
**
*******************************************************************************/
void bta_av_rcfg_open (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    UNUSED(p_data);

    APPL_TRACE_DEBUG("bta_av_rcfg_open, num_disc_snks = %d", p_scb->num_disc_snks);

    if (p_scb->num_disc_snks == 0) {
        /* Need to update call-out module so that it will be ready for discover */
        p_scb->p_cos->stop(p_scb->hndl, p_scb->codec_type);

        /* send avdtp discover request */
        AVDT_DiscoverReq(p_scb->peer_addr, p_scb->sep_info, BTA_AV_NUM_SEPS, bta_av_dt_cback[p_scb->hdi]);
    } else {
        p_scb->codec_type = p_scb->p_cap->codec_info[BTA_AV_CODEC_TYPE_IDX];
        memcpy(p_scb->cfg.codec_info, p_scb->p_cap->codec_info, AVDT_CODEC_SIZE);
        /* we may choose to use a different SEP at reconfig.
         * adjust the sep_idx now */
        bta_av_adjust_seps_idx(p_scb, bta_av_get_scb_handle(p_scb, AVDT_TSEP_SRC));

        /* open the stream with the new config */
        p_scb->sep_info_idx = p_scb->rcfg_idx;
        AVDT_OpenReq(p_scb->avdt_handle, p_scb->peer_addr,
                     p_scb->sep_info[p_scb->sep_info_idx].seid, p_scb->p_cap);
    }

}

/*******************************************************************************
**
** Function         bta_av_security_rej
**
** Description      Send an AVDTP security reject.
**
** Returns          void
**
*******************************************************************************/
void bta_av_security_rej (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    UNUSED(p_data);

    AVDT_SecurityRsp(p_scb->avdt_handle, p_scb->avdt_label, AVDT_ERR_BAD_STATE,
                     NULL, 0);
}

/*******************************************************************************
**
** Function         bta_av_chk_2nd_start
**
** Description      check if this is 2nd stream and if it needs to be started.
**                  This function needs to be kept very similar to bta_av_chk_start
**
** Returns          void
**
*******************************************************************************/
void bta_av_chk_2nd_start (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_SCB *p_scbi;
    int i;
    BOOLEAN new_started = FALSE;
    UNUSED(p_data);

    if ((p_scb->chnl == BTA_AV_CHNL_AUDIO) && (bta_av_cb.audio_open_cnt >= 2)) {
        /* more than one audio channel is connected */
        if (!(p_scb->role & BTA_AV_ROLE_SUSPEND_OPT)) {
            /* this channel does not need to be reconfigured.
             * if there is other channel streaming, start the stream now */
            for (i = 0; i < BTA_AV_NUM_STRS; i++) {
                p_scbi = bta_av_cb.p_scb[i];
                if (p_scbi && p_scbi->chnl == BTA_AV_CHNL_AUDIO && p_scbi->co_started) {
                    if (!new_started) {
                        /* start the new stream */
                        new_started = TRUE;
                        bta_av_ssm_execute(p_scb, BTA_AV_AP_START_EVT, NULL);
                    }
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
** Function         bta_av_open_rc
**
** Description      Send a message to main SM to open RC channel.
**
** Returns          void
**
*******************************************************************************/
void bta_av_open_rc (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_START   start;

    APPL_TRACE_DEBUG("bta_av_open_rc use_rc: %d, wait: x%x role:x%x", p_scb->use_rc, p_scb->wait, p_scb->role);
    if ((p_scb->wait & BTA_AV_WAIT_ROLE_SW_BITS) && (p_scb->q_tag == BTA_AV_Q_TAG_START)) {
        /* waiting for role switch for some reason & the timer expires */
        if (!bta_av_link_role_ok(p_scb, A2D_SET_ONE_BIT)) {
            APPL_TRACE_ERROR ("failed to start streaming for role management reasons!!");
            bta_sys_stop_timer(&p_scb->timer);
            start.chnl   = p_scb->chnl;
            start.status = BTA_AV_FAIL_ROLE;
            start.initiator = TRUE;
            start.hndl   = p_scb->hndl;
            p_scb->wait &= ~BTA_AV_WAIT_ROLE_SW_BITS;
            bta_av_cb.rs_idx = 0;
            (*bta_av_cb.p_cback)(BTA_AV_START_EVT, (tBTA_AV *) &start);
        } else {
            /* role switch is done. continue to start streaming */
            bta_av_cb.rs_idx = 0;
            p_data->hdr.offset = BTA_AV_RS_OK;
            bta_av_start_ok (p_scb, p_data);
        }
        return;
    }

    if (p_scb->use_rc == TRUE || (p_scb->role & BTA_AV_ROLE_AD_ACP) ) {
        if (bta_av_cb.disc) {
            /* AVRC discover db is in use */
            if (p_scb->rc_handle == BTA_AV_RC_HANDLE_NONE) {
                /* AVRC channel is not connected. delay a little bit */
                if ((p_scb->wait & BTA_AV_WAIT_ROLE_SW_BITS) == 0) {
                    bta_sys_start_timer(&p_scb->timer, BTA_AV_AVRC_TIMER_EVT, BTA_AV_RC_DISC_TIME_VAL);
                } else {
                    p_scb->wait |= BTA_AV_WAIT_CHECK_RC;
                }
            }
        } else {
            /* use main SM for AVRC SDP activities */
            bta_av_rc_disc((UINT8)(p_scb->hdi + 1));
        }
    } else {
        if (BTA_AV_RC_HANDLE_NONE != p_scb->rc_handle) {
            /* the open API said that this handle does not want a RC connection.
             * disconnect it now */
            AVRC_Close(p_scb->rc_handle);
        }
    }
}

/*******************************************************************************
**
** Function         bta_av_open_at_inc
**
** Description      This function is called if API open is called by application
**                  while state-machine is at incoming state.
**
** Returns          void
**
*******************************************************************************/
void bta_av_open_at_inc (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_API_OPEN  *p_buf;

    memcpy (&(p_scb->open_api), &(p_data->api_open), sizeof(tBTA_AV_API_OPEN));

    if (p_scb->coll_mask & BTA_AV_COLL_INC_TMR) {
        p_scb->coll_mask |= BTA_AV_COLL_API_CALLED;

        /* API open will be handled at timeout if SNK did not start signalling. */
        /* API open will be ignored if SNK starts signalling.                   */
    } else {
        /* SNK did not start signalling, API was called N seconds timeout. */
        /* We need to switch to INIT state and start opening connection. */
        p_scb->coll_mask = 0;
        bta_av_set_scb_sst_init (p_scb);

        if ((p_buf = (tBTA_AV_API_OPEN *) osi_malloc(sizeof(tBTA_AV_API_OPEN))) != NULL) {
            memcpy(p_buf, &(p_scb->open_api), sizeof(tBTA_AV_API_OPEN));
            bta_sys_sendmsg(p_buf);
        }
    }
}

/*******************************************************************************
**
** Function         bta_av_set_delay_value
**
** Description      This function is called if application layer
**                  call the API set_delay_value
**
** Returns          void
**
*******************************************************************************/
void bta_av_set_delay_value (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data)
{
    tBTA_AV_DELAY delay = {0};

    AVDT_SetDelayValue(p_data->api_set_delay_vlaue.delay_value);
    delay.delay_value = p_data->api_set_delay_vlaue.delay_value;

    if (p_scb->state == BTA_AV_OPEN_SST) {
        if (AVDT_DelayReport(p_scb->avdt_handle, 0, delay.delay_value) != AVDT_SUCCESS) {
            delay.status = BTA_AV_FAIL;
        }
    }

    /* call callback with set delay value event */
    (*bta_av_cb.p_cback)(BTA_AV_SET_DELAY_VALUE_EVT, (tBTA_AV *)&delay);
}

#endif /* BTA_AV_INCLUDED */

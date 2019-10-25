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
 *  This is the main implementation file for the BTA audio gateway.
 *
 ******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include "bta/bta_api.h"
#include "bta/bta_sys.h"
#include "bta/bta_ag_api.h"
#include "bta/bta_ag_co.h"
#include "bta/utl.h"
#include "common/bt_defs.h"
#include "common/bt_trace.h"
#include "osi/allocator.h"
#include "bta_ag_int.h"

#if (BTA_AG_INCLUDED == TRUE)
/*****************************************************************************
** Constants and types
*****************************************************************************/
#ifndef BTA_AG_DEBUG
#define BTA_AG_DEBUG TRUE
#endif

#if BTA_AG_DEBUG == TRUE
static char *bta_ag_evt_str(UINT16 event, tBTA_AG_RES result);
static char *bta_ag_state_str(UINT8 state);
#endif

/* state machine states */
enum
{
    BTA_AG_INIT_ST,
    BTA_AG_OPENING_ST,
    BTA_AG_OPEN_ST,
    BTA_AG_CLOSING_ST
};

/* state machine action enumeration list */
enum
{
    BTA_AG_REGISTER,
    BTA_AG_DEREGISTER,
    BTA_AG_START_OPEN,
    BTA_AG_RFC_DO_OPEN,
    BTA_AG_RFC_DO_CLOSE,
    BTA_AG_START_DEREG,
    BTA_AG_START_CLOSE,
    BTA_AG_RFC_OPEN,
    BTA_AG_OPEN_FAIL,
    BTA_AG_RFC_ACP_OPEN,
    BTA_AG_RFC_CLOSE,
    BTA_AG_RFC_FAIL,
    BTA_AG_RFC_DATA,
    BTA_AG_DISC_INT_RES,
    BTA_AG_DISC_FAIL,
    BTA_AG_DISC_ACP_RES,
    BTA_AG_FREE_DB,
    BTA_AG_SCO_CONN_OPEN,
    BTA_AG_SCO_CONN_CLOSE,
    BTA_AG_SCO_LISTEN,
    BTA_AG_SCO_OPEN,
    BTA_AG_SCO_CLOSE,
    BTA_AG_SCO_SHUTDOWN,
    BTA_AG_POST_SCO_OPEN,
    BTA_AG_POST_SCO_CLOSE,
    BTA_AG_SVC_CONN_OPEN,
    BTA_AG_RESULT,
    BTA_AG_SETCODEC,
    BTA_AG_SEND_RING,
    BTA_AG_CI_SCO_DATA,
    BTA_AG_CI_RX_DATA,
    BTA_AG_RCVD_SLC_READY,
    BTA_AG_NUM_ACTIONS
};

#define BTA_AG_IGNORE       BTA_AG_NUM_ACTIONS

/* type for action functions */
typedef void (*tBTA_AG_ACTION) (tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data);

/* action functions */
const tBTA_AG_ACTION bta_ag_action[] =
{
    bta_ag_register,
    bta_ag_deregister,
    bta_ag_start_open,
    bta_ag_rfc_do_open,
    bta_ag_rfc_do_close,
    bta_ag_start_dereg,
    bta_ag_start_close,
    bta_ag_rfc_open,
    bta_ag_open_fail,
    bta_ag_rfc_acp_open,
    bta_ag_rfc_close,
    bta_ag_rfc_fail,
    bta_ag_rfc_data,
    bta_ag_disc_int_res,
    bta_ag_disc_fail,
    bta_ag_disc_acp_res,
    bta_ag_free_db,
    bta_ag_sco_conn_open,
    bta_ag_sco_conn_close,
    bta_ag_sco_listen,
    bta_ag_sco_open,
    bta_ag_sco_close,
    bta_ag_sco_shutdown,
    bta_ag_post_sco_open,
    bta_ag_post_sco_close,
    bta_ag_svc_conn_open,
    bta_ag_result,
    bta_ag_setcodec,
    bta_ag_send_ring,
    bta_ag_ci_sco_data,
    bta_ag_ci_rx_data,
    bta_ag_rcvd_slc_ready
};

/* state table information */
#define BTA_AG_ACTIONS              2       /* number of actions */
#define BTA_AG_NEXT_STATE           2       /* position of next state */
#define BTA_AG_NUM_COLS             3       /* number of columns in state tables */

/* state table for init state */
const UINT8 bta_ag_st_init[][BTA_AG_NUM_COLS] =
{
/* Event                    Action 1                Action 2                Next state */
/* API_REGISTER_EVT */      {BTA_AG_REGISTER,       BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* API_DEREGISTER_EVT */    {BTA_AG_DEREGISTER,     BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* API_OPEN_EVT */          {BTA_AG_START_OPEN,     BTA_AG_IGNORE,          BTA_AG_OPENING_ST},
/* API_CLOSE_EVT */         {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* API_AUDIO_OPEN_EVT */    {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* API_AUDIO_CLOSE_EVT */   {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* API_RESULT_EVT */        {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* API_SETCODEC_EVT */      {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* RFC_OPEN_EVT */          {BTA_AG_RFC_ACP_OPEN,   BTA_AG_SCO_LISTEN,      BTA_AG_OPEN_ST},
/* RFC_CLOSE_EVT */         {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* RFC_SRV_CLOSE_EVT */     {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* RFC_DATA_EVT */          {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* SCO_OPEN_EVT */          {BTA_AG_SCO_CONN_OPEN,  BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* SCO_CLOSE_EVT */         {BTA_AG_SCO_CONN_CLOSE, BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* DISC_ACP_RES_EVT */      {BTA_AG_FREE_DB,        BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* DISC_INT_RES_EVT */      {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* DISC_OK_EVT */           {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* DISC_FAIL_EVT */         {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* CI_RX_WRITE_EVT */       {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* RING_TOUT_EVT */         {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* SVC_TOUT_EVT */          {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* CI_SCO_DATA_EVT */       {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* CI_SLC_READY_EVT */      {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_INIT_ST}
};

/* state table for opening state */
const UINT8 bta_ag_st_opening[][BTA_AG_NUM_COLS] =
{
/* Event                    Action 1                Action 2                Next state */
/* API_REGISTER_EVT */      {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_OPENING_ST},
/* API_DEREGISTER_EVT */    {BTA_AG_RFC_DO_CLOSE,   BTA_AG_START_DEREG,     BTA_AG_CLOSING_ST},
/* API_OPEN_EVT */          {BTA_AG_OPEN_FAIL,      BTA_AG_IGNORE,          BTA_AG_OPENING_ST},
/* API_CLOSE_EVT */         {BTA_AG_RFC_DO_CLOSE,   BTA_AG_IGNORE,          BTA_AG_CLOSING_ST},
/* API_AUDIO_OPEN_EVT */    {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_OPENING_ST},
/* API_AUDIO_CLOSE_EVT */   {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_OPENING_ST},
/* API_RESULT_EVT */        {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_OPENING_ST},
/* API_SETCODEC_EVT */      {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_OPENING_ST},
/* RFC_OPEN_EVT */          {BTA_AG_RFC_OPEN,       BTA_AG_SCO_LISTEN,      BTA_AG_OPEN_ST},
/* RFC_CLOSE_EVT */         {BTA_AG_RFC_FAIL,       BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* RFC_SRV_CLOSE_EVT */     {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_OPENING_ST},
/* RFC_DATA_EVT */          {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_OPENING_ST},
/* SCO_OPEN_EVT */          {BTA_AG_SCO_CONN_OPEN,  BTA_AG_IGNORE,          BTA_AG_OPENING_ST},
/* SCO_CLOSE_EVT */         {BTA_AG_SCO_CONN_CLOSE, BTA_AG_IGNORE,          BTA_AG_OPENING_ST},
/* DISC_ACP_RES_EVT */      {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_OPENING_ST},
/* DISC_INT_RES_EVT */      {BTA_AG_DISC_INT_RES,   BTA_AG_IGNORE,          BTA_AG_OPENING_ST},
/* DISC_OK_EVT */           {BTA_AG_RFC_DO_OPEN,    BTA_AG_IGNORE,          BTA_AG_OPENING_ST},
/* DISC_FAIL_EVT */         {BTA_AG_DISC_FAIL,      BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* CI_RX_WRITE_EVT */       {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_OPENING_ST},
/* RING_TOUT_EVT */         {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_OPENING_ST},
/* SVC_TOUT_EVT */          {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_OPENING_ST},
/* CI_SCO_DATA_EVT */       {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_OPENING_ST},
/* CI_SLC_READY_EVT */      {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_OPENING_ST}
};

/* state table for open state */
const UINT8 bta_ag_st_open[][BTA_AG_NUM_COLS] =
{
/* Event                    Action 1                Action 2                Next state */
/* API_REGISTER_EVT */      {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_OPEN_ST},
/* API_DEREGISTER_EVT */    {BTA_AG_START_CLOSE,    BTA_AG_START_DEREG,     BTA_AG_CLOSING_ST},
/* API_OPEN_EVT */          {BTA_AG_OPEN_FAIL,      BTA_AG_IGNORE,          BTA_AG_OPEN_ST},
/* API_CLOSE_EVT */         {BTA_AG_START_CLOSE,    BTA_AG_IGNORE,          BTA_AG_CLOSING_ST},
/* API_AUDIO_OPEN_EVT */    {BTA_AG_SCO_OPEN,       BTA_AG_IGNORE,          BTA_AG_OPEN_ST},
/* API_AUDIO_CLOSE_EVT */   {BTA_AG_SCO_CLOSE,      BTA_AG_IGNORE,          BTA_AG_OPEN_ST},
/* API_RESULT_EVT */        {BTA_AG_RESULT,         BTA_AG_IGNORE,          BTA_AG_OPEN_ST},
/* API_SETCODEC_EVT */      {BTA_AG_SETCODEC,       BTA_AG_IGNORE,          BTA_AG_OPEN_ST},
/* RFC_OPEN_EVT */          {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_OPEN_ST},
/* RFC_CLOSE_EVT */         {BTA_AG_RFC_CLOSE,      BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* RFC_SRV_CLOSE_EVT */     {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_OPEN_ST},
/* RFC_DATA_EVT */          {BTA_AG_RFC_DATA,       BTA_AG_IGNORE,          BTA_AG_OPEN_ST},
/* SCO_OPEN_EVT */          {BTA_AG_SCO_CONN_OPEN,  BTA_AG_POST_SCO_OPEN,   BTA_AG_OPEN_ST},
/* SCO_CLOSE_EVT */         {BTA_AG_SCO_CONN_CLOSE, BTA_AG_POST_SCO_CLOSE,  BTA_AG_OPEN_ST},
/* DISC_ACP_RES_EVT */      {BTA_AG_DISC_ACP_RES,   BTA_AG_IGNORE,          BTA_AG_OPEN_ST},
/* DISC_INT_RES_EVT */      {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_OPEN_ST},
/* DISC_OK_EVT */           {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_OPEN_ST},
/* DISC_FAIL_EVT */         {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_OPEN_ST},
/* CI_RX_WRITE_EVT */       {BTA_AG_CI_RX_DATA,     BTA_AG_IGNORE,          BTA_AG_OPEN_ST},
/* RING_TOUT_EVT */         {BTA_AG_SEND_RING,      BTA_AG_IGNORE,          BTA_AG_OPEN_ST},
/* SVC_TOUT_EVT */          {BTA_AG_START_CLOSE,    BTA_AG_IGNORE,          BTA_AG_CLOSING_ST},
/* CI_SCO_DATA_EVT */       {BTA_AG_CI_SCO_DATA,    BTA_AG_IGNORE,          BTA_AG_OPEN_ST},
/* CI_SLC_READY_EVT */      {BTA_AG_RCVD_SLC_READY, BTA_AG_IGNORE,          BTA_AG_OPEN_ST}
};

/* state table for closing state */
const UINT8 bta_ag_st_closing[][BTA_AG_NUM_COLS] =
{
/* Event                    Action 1                Action 2                Next state */
/* API_REGISTER_EVT */      {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_CLOSING_ST},
/* API_DEREGISTER_EVT */    {BTA_AG_START_DEREG,    BTA_AG_IGNORE,          BTA_AG_CLOSING_ST},
/* API_OPEN_EVT */          {BTA_AG_OPEN_FAIL,      BTA_AG_IGNORE,          BTA_AG_CLOSING_ST},
/* API_CLOSE_EVT */         {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_CLOSING_ST},
/* API_AUDIO_OPEN_EVT */    {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_CLOSING_ST},
/* API_AUDIO_CLOSE_EVT */   {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_CLOSING_ST},
/* API_RESULT_EVT */        {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_CLOSING_ST},
/* API_SETCODEC_EVT */      {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_CLOSING_ST},
/* RFC_OPEN_EVT */          {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_CLOSING_ST},
/* RFC_CLOSE_EVT */         {BTA_AG_RFC_CLOSE,      BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* RFC_SRV_CLOSE_EVT */     {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_CLOSING_ST},
/* RFC_DATA_EVT */          {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_CLOSING_ST},
/* SCO_OPEN_EVT */          {BTA_AG_SCO_CONN_OPEN,  BTA_AG_IGNORE,          BTA_AG_CLOSING_ST},
/* SCO_CLOSE_EVT */         {BTA_AG_SCO_CONN_CLOSE, BTA_AG_POST_SCO_CLOSE,  BTA_AG_CLOSING_ST},
/* DISC_ACP_RES_EVT */      {BTA_AG_FREE_DB,        BTA_AG_IGNORE,          BTA_AG_CLOSING_ST},
/* DISC_INT_RES_EVT */      {BTA_AG_FREE_DB,        BTA_AG_IGNORE,          BTA_AG_INIT_ST},
/* DISC_OK_EVT */           {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_CLOSING_ST},
/* DISC_FAIL_EVT */         {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_CLOSING_ST},
/* CI_RX_WRITE_EVT */       {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_CLOSING_ST},
/* RING_TOUT_EVT */         {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_CLOSING_ST},
/* SVC_TOUT_EVT */          {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_CLOSING_ST},
/* CI_SCO_DATA_EVT */       {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_CLOSING_ST},
/* CI_SLC_READY_EVT */      {BTA_AG_IGNORE,         BTA_AG_IGNORE,          BTA_AG_CLOSING_ST}
};

/* type for state table */
typedef const UINT8 (*tBTA_AG_ST_TBL)[BTA_AG_NUM_COLS];

/* state table */
const tBTA_AG_ST_TBL bta_ag_st_tbl[] =
{
    bta_ag_st_init,
    bta_ag_st_opening,
    bta_ag_st_open,
    bta_ag_st_closing
};

/*****************************************************************************
** Global data
*****************************************************************************/
const char *bta_ag_version = "1.5"; //"1.6"
/* AG control block */
#if BTA_DYNAMIC_MEMORY == FALSE
tBTA_AG_CB  bta_ag_cb;
#else
tBTA_AG_CB  *bta_ag_cb_ptr;
#endif

#if BTA_AG_DEBUG == TRUE
static char *bta_ag_evt_str(UINT16 event, tBTA_AG_RES result)
{
    switch (event)
    {
    case BTA_AG_API_REGISTER_EVT:
        return "Register Request";
    case BTA_AG_API_DEREGISTER_EVT:
        return "Deregister Request";
    case BTA_AG_API_OPEN_EVT:
        return "Open SLC Request";
    case BTA_AG_API_CLOSE_EVT:
        return "Close SLC Request";
    case BTA_AG_API_AUDIO_OPEN_EVT:
        return "Open Audio Request";
    case BTA_AG_API_AUDIO_CLOSE_EVT:
        return "Close Audio Request";
    case BTA_AG_API_RESULT_EVT:
        switch (result) {
            case BTA_AG_SPK_RES:            return ("AT Result  BTA_AG_SPK_RES");
            case BTA_AG_MIC_RES:            return ("AT Result  BTA_AG_MIC_RES");
            case BTA_AG_INBAND_RING_RES:    return ("AT Result  BTA_AG_INBAND_RING_RES");
            case BTA_AG_CIND_RES:           return ("AT Result  BTA_AG_CIND_RES");
            case BTA_AG_BINP_RES:           return ("AT Result  BTA_AG_BINP_RES");
            case BTA_AG_IND_RES:            return ("AT Result  BTA_AG_IND_RES");
            case BTA_AG_BVRA_RES:           return ("AT Result  BTA_AG_BVRA_RES");
            case BTA_AG_CNUM_RES:           return ("AT Result  BTA_AG_CNUM_RES");
            case BTA_AG_BTRH_RES:           return ("AT Result  BTA_AG_BTRH_RES");
            case BTA_AG_CLCC_RES:           return ("AT Result  BTA_AG_CLCC_RES");
            case BTA_AG_COPS_RES:           return ("AT Result  BTA_AG_COPS_RES");
            case BTA_AG_IN_CALL_RES:        return ("AT Result  BTA_AG_IN_CALL_RES");
            case BTA_AG_IN_CALL_CONN_RES:   return ("AT Result  BTA_AG_IN_CALL_CONN_RES");
            case BTA_AG_CALL_WAIT_RES:      return ("AT Result  BTA_AG_CALL_WAIT_RES");
            case BTA_AG_OUT_CALL_ORIG_RES:  return ("AT Result  BTA_AG_OUT_CALL_ORIG_RES");
            case BTA_AG_OUT_CALL_ALERT_RES: return ("AT Result  BTA_AG_OUT_CALL_ALERT_RES");
            case BTA_AG_OUT_CALL_CONN_RES:  return ("AT Result  BTA_AG_OUT_CALL_CONN_RES");
            case BTA_AG_CALL_CANCEL_RES:    return ("AT Result  BTA_AG_CALL_CANCEL_RES");
            case BTA_AG_END_CALL_RES:       return ("AT Result  BTA_AG_END_CALL_RES");
            case BTA_AG_UNAT_RES:           return ("AT Result  BTA_AG_UNAT_RES");
            default:                        return ("Unknown AG Result");
        }
    case BTA_AG_API_SETCODEC_EVT:
        return "Set Codec Request";
    case BTA_AG_RFC_OPEN_EVT:
        return "RFC Opened";
    case BTA_AG_RFC_CLOSE_EVT:
        return "RFC Closed";
    case BTA_AG_RFC_SRV_CLOSE_EVT:
        return "RFC SRV Closed";
    case BTA_AG_RFC_DATA_EVT:
        return "RFC Data";
    case BTA_AG_SCO_OPEN_EVT:
        return "Audio Opened";
    case BTA_AG_SCO_CLOSE_EVT:
        return "Audio Closed";
    case BTA_AG_DISC_ACP_RES_EVT:
        return "Discovery ACP Result";
    case BTA_AG_DISC_INT_RES_EVT:
        return "Discovery INT Result";
    case BTA_AG_DISC_OK_EVT:
        return "Discovery OK";
    case BTA_AG_DISC_FAIL_EVT:
        return "Discovery Failed";
    case BTA_AG_CI_RX_WRITE_EVT:
        return "CI RX Write";
    case BTA_AG_RING_TOUT_EVT:
        return "Ring Timeout";
    case BTA_AG_SVC_TOUT_EVT:
        return "Service Timeout";
    case BTA_AG_API_ENABLE_EVT:
        return "Enable AG";
    case BTA_AG_API_DISABLE_EVT:
        return "Disable AG";
    case BTA_AG_CI_SCO_DATA_EVT:
        return "SCO data Callin";
    case BTA_AG_CI_SLC_READY_EVT:
        return "SLC Ready Callin";
    default:
        return "Unknown AG Event";
    }
}

static char *bta_ag_state_str(UINT8 state)
{
    switch (state) {
    case BTA_AG_INIT_ST:
        return "Initial";
    case BTA_AG_OPENING_ST:
        return "Opening";
    case BTA_AG_OPEN_ST:
        return "Open";
    case BTA_AG_CLOSING_ST:
        return "Closing";
    default:
        return "Unknown AG State";
    }
}

#endif

/*******************************************************************************
**
** Function         bta_ag_timer_cback
**
** Description      AG timer callback.
**
**
** Returns          void
**
*******************************************************************************/
static void bta_ag_timer_cback(void *p)
{
    BT_HDR          *p_buf;
    TIMER_LIST_ENT  *p_tle = (TIMER_LIST_ENT *) p;

    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = p_tle->event;
        p_buf->layer_specific = bta_ag_scb_to_idx((tBTA_AG_SCB *) p_tle->param);
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         bta_ag_scb_alloc
**
** Description      Allocate an AG service control block.
**
**
** Returns          pointer to the scb, or NULL if none could be allocated.
**
*******************************************************************************/
static tBTA_AG_SCB *bta_ag_scb_alloc(void)
{
    tBTA_AG_SCB     *p_scb = &bta_ag_cb.scb[0];
    int             i;

    for (i = 0; i < BTA_AG_NUM_SCB; i++, p_scb++) {
        if (!p_scb->in_use) {
            /* initialize variables */
            p_scb->in_use = TRUE;
            p_scb->sco_idx = BTM_INVALID_SCO_INDEX;
#if (BTM_WBS_INCLUDED == TRUE )
            p_scb->codec_updated = FALSE;
#endif
            /* set up timers */
            p_scb->act_timer.param = (UINT32) p_scb;
            p_scb->act_timer.p_cback = bta_ag_timer_cback;
#if (BTM_WBS_INCLUDED == TRUE)
            /* set eSCO mSBC setting to T2 as the preferred */
            p_scb->codec_msbc_settings = BTA_AG_SCO_MSBC_SETTINGS_T2;
#endif
            APPL_TRACE_DEBUG("bta_ag_scb_alloc %d", bta_ag_scb_to_idx(p_scb));
            break;
        }
    }

    if (i == BTA_AG_NUM_SCB) {
        p_scb = NULL;        /* out of scbs */
        APPL_TRACE_WARNING("Out of ag scbs");
    }
    return p_scb;
}

/*******************************************************************************
**
** Function         bta_ag_scb_dealloc
**
** Description      Deallocate a service control block.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_scb_dealloc(tBTA_AG_SCB *p_scb)
{
    UINT8   idx;
    BOOLEAN allocated = FALSE;

    APPL_TRACE_DEBUG("bta_ag_scb_dealloc %d", bta_ag_scb_to_idx(p_scb));
    /* stop timers */
    bta_sys_stop_timer(&p_scb->act_timer);
#if (BTM_WBS_INCLUDED == TRUE)
    bta_sys_stop_timer(&p_scb->cn_timer);
#endif
    bta_sys_stop_timer(&p_scb->colli_timer);

    /* initialize control block */
    memset(p_scb, 0, sizeof(tBTA_AG_SCB));
    p_scb->sco_idx = BTM_INVALID_SCO_INDEX;
    /* If all scbs are deallocated, callback with disable event */
    if (!bta_sys_is_register (BTA_ID_AG)) {
        for (idx = 0; idx < BTA_AG_NUM_SCB; idx++) {
            if (bta_ag_cb.scb[idx].in_use) {
                allocated = TRUE;
                break;
            }
        }
        if (!allocated) {
            (*bta_ag_cb.p_cback)(BTA_AG_DISABLE_EVT, NULL);
        }
    }
}

/*******************************************************************************
**
** Function         bta_ag_scb_to_idx
**
** Description      Given a pointer to an scb, return its index.
**
**
** Returns          Index of scb.
**
*******************************************************************************/
UINT16 bta_ag_scb_to_idx(tBTA_AG_SCB *p_scb)
{
    /* use array arithmetic to determine index */
    return ((UINT16) (p_scb - bta_ag_cb.scb)) + 1;
}

/*******************************************************************************
**
** Function         bta_ag_scb_by_idx
**
** Description      Given an scb index return pointer to scb.
**
**
** Returns          Pointer to scb or NULL if not allocated.
**
*******************************************************************************/
tBTA_AG_SCB *bta_ag_scb_by_idx(UINT16 idx)
{
    tBTA_AG_SCB     *p_scb;
    /* verify index */
    if (idx > 0 && idx <= BTA_AG_NUM_SCB) {
        p_scb = &bta_ag_cb.scb[idx - 1];
        if (!p_scb->in_use) {
            p_scb = NULL;
            APPL_TRACE_WARNING("ag scb idx %d not allocated", idx);
        }
    } else {
        p_scb = NULL;
        APPL_TRACE_DEBUG("ag scb idx %d out of range", idx);
    }
    return p_scb;
}

/*******************************************************************************
**
** Function         bta_ag_service_to_idx
**
** Description      Given a BTA service mask convert to profile index.
**
**
** Returns          Profile ndex of scb.
**
*******************************************************************************/
UINT8 bta_ag_service_to_idx(tBTA_SERVICE_MASK services)
{
    if (services & BTA_HFP_SERVICE_MASK) {
        return BTA_AG_HFP;
    } else {
        return BTA_AG_HSP;
    }
}

/*******************************************************************************
**
** Function         bta_ag_idx_by_bdaddr
**
** Description      Find SCB associated with peer BD address.
**
**
** Returns          Index of SCB or zero if none found.
**
*******************************************************************************/
UINT16 bta_ag_idx_by_bdaddr(BD_ADDR peer_addr)
{
    tBTA_AG_SCB     *p_scb = &bta_ag_cb.scb[0];
    UINT16          i;

    if (peer_addr != NULL) {
        for (i = 0; i < BTA_AG_NUM_SCB; i++, p_scb++) {
            if (p_scb->in_use && !bdcmp(peer_addr, p_scb->peer_addr)) {
                return (i + 1);
            }
        }
    }
    /* no scb found */
    APPL_TRACE_WARNING("No ag scb for peer addr");
    return 0;
}

/*******************************************************************************
**
** Function         bta_ag_other_scb_open
**
** Description      Check whether any other scb is in open state.
**
**
** Returns          TRUE if another scb is in open state, FALSE otherwise.
**
*******************************************************************************/
BOOLEAN bta_ag_other_scb_open(tBTA_AG_SCB *p_curr_scb)
{
    tBTA_AG_SCB     *p_scb = &bta_ag_cb.scb[0];

    for (int i = 0; i < BTA_AG_NUM_SCB; i++, p_scb++) {
        if (p_scb->in_use && p_scb != p_curr_scb && p_scb->state == BTA_AG_OPEN_ST) {
            return TRUE;
        }
    }
    /* no other scb found */
    APPL_TRACE_DEBUG("No other ag scb open");
    return FALSE;
}

/*******************************************************************************
**
** Function         bta_ag_scb_open
**
** Description      Check whether given scb is in open state.
**
**
** Returns          TRUE if scb is in open state, FALSE otherwise.
**
*******************************************************************************/
BOOLEAN bta_ag_scb_open(tBTA_AG_SCB *p_curr_scb)
{
    if (p_curr_scb && p_curr_scb->in_use && p_curr_scb->state == BTA_AG_OPEN_ST) {
        return TRUE;
    }
    return FALSE;
}

/*******************************************************************************
**
** Function         bta_ag_get_other_idle_scb
**
** Description      Return other scb if it is in INIT st.
**
**
** Returns          Pointer to other scb if INIT st, NULL otherwise.
**
*******************************************************************************/
tBTA_AG_SCB *bta_ag_get_other_idle_scb (tBTA_AG_SCB *p_curr_scb)
{
    tBTA_AG_SCB     *p_scb = &bta_ag_cb.scb[0];
    UINT8   xx;

    for (xx = 0; xx < BTA_AG_NUM_SCB; xx++, p_scb++) {
        if (p_scb->in_use && (p_scb != p_curr_scb) && (p_scb->state == BTA_AG_INIT_ST)) {
            return p_scb;
        }
    }
    /* no other scb found */
    APPL_TRACE_DEBUG("bta_ag_get_other_idle_scb: No idle AG scb");
    return NULL;
}

/*******************************************************************************
**
** Function         bta_ag_colli_timer_cback
**
** Description      AG connection collision timer callback
**
**
** Returns          void
**
*******************************************************************************/
static void bta_ag_colli_timer_cback (TIMER_LIST_ENT *p_tle)
{
    tBTA_AG_SCB *p_scb;

    APPL_TRACE_DEBUG ("bta_ag_colli_timer_cback");
    if (p_tle) {
        p_scb = (tBTA_AG_SCB *)p_tle->param;
        if (p_scb) {
            p_scb->colli_tmr_on = FALSE;
            /* If the peer haven't opened AG connection     */
            /* we will restart opening process.             */
            bta_ag_resume_open (p_scb);
        }
    }
}

/*******************************************************************************
**
** Function         bta_ag_collision_cback
**
** Description      Get notified about collision.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_collision_cback (tBTA_SYS_CONN_STATUS status, UINT8 id, UINT8 app_id, BD_ADDR peer_addr)
{
    UINT16  handle;
    tBTA_AG_SCB *p_scb;
    UNUSED(status);
    UNUSED(app_id);

    /* Check if we have opening scb for the peer device. */
    handle = bta_ag_idx_by_bdaddr (peer_addr);
    p_scb = bta_ag_scb_by_idx (handle);

    if (p_scb && (p_scb->state == BTA_AG_OPENING_ST)) {
        if (id == BTA_ID_SYS) {
            /* ACL collision */
            APPL_TRACE_WARNING ("AG found collision (ACL) ...");
        } else if (id == BTA_ID_AG) {
            /* RFCOMM collision */
            APPL_TRACE_WARNING ("AG found collision (RFCOMM) ...");
        } else {
            APPL_TRACE_WARNING ("AG found collision (\?\?\?) ...");
        }
        p_scb->state = BTA_AG_INIT_ST;
        /* Cancel SDP if it had been started. */
        if(p_scb->p_disc_db) {
            (void)SDP_CancelServiceSearch (p_scb->p_disc_db);
            bta_ag_free_db(p_scb, NULL);
        }
        /* reopen registered servers */
        /* Collision may be detected before or after we close servers. */
        if (bta_ag_is_server_closed (p_scb)) {
            bta_ag_start_servers(p_scb, p_scb->reg_services);
        }
        /* Start timer to han */
        p_scb->colli_timer.p_cback = (TIMER_CBACK*)&bta_ag_colli_timer_cback;
        p_scb->colli_timer.param = (INT32)p_scb;
        bta_sys_start_timer(&p_scb->colli_timer, 0, BTA_AG_COLLISION_TIMER);
        p_scb->colli_tmr_on = TRUE;
    }
}

/*******************************************************************************
**
** Function         bta_ag_resume_open
**
** Description      Resume opening process.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_resume_open (tBTA_AG_SCB *p_scb)
{
    if (p_scb) {
        APPL_TRACE_DEBUG ("bta_ag_resume_open, Handle(%d)", bta_ag_scb_to_idx(p_scb));
        /* resume opening process.  */
        if (p_scb->state == BTA_AG_INIT_ST) {
            p_scb->state = BTA_AG_OPENING_ST;
            bta_ag_start_open (p_scb, NULL);
        }
    } else {
        APPL_TRACE_ERROR ("bta_ag_resume_open, Null p_scb");
    }
}

/*******************************************************************************
**
** Function         bta_ag_api_enable
**
** Description      Handle an API enable event.
**
**
** Returns          void
**
*******************************************************************************/
static void bta_ag_api_enable(tBTA_AG_DATA *p_data)
{
    /* initialize control block */
    memset(&bta_ag_cb, 0, sizeof(tBTA_AG_CB));
    /* store callback function */
    bta_ag_cb.p_cback = p_data->api_enable.p_cback;
    bta_ag_cb.parse_mode = p_data->api_enable.parse_mode;
    /* check if mSBC support enabled */
    if (strcmp(bta_ag_version, "1.6") == 0) {
        bta_ag_cb.msbc_enabled = TRUE;
        bta_ag_cb.scb->negotiated_codec = BTM_SCO_CODEC_MSBC;
    } else{
        bta_ag_cb.msbc_enabled = FALSE;
        bta_ag_cb.scb->negotiated_codec = BTM_SCO_CODEC_CVSD;
    }

    /* set deault setting for eSCO/SCO */
    BTM_WriteVoiceSettings(AG_VOICE_SETTINGS);
    bta_sys_collision_register (BTA_ID_AG, bta_ag_collision_cback);
    /* call callback with enable event */
    (*bta_ag_cb.p_cback)(BTA_AG_ENABLE_EVT, NULL);
}

/*******************************************************************************
**
** Function         bta_ag_api_disable
**
** Description      Handle an API disable event.
**
**
** Returns          void
**
*******************************************************************************/
static void bta_ag_api_disable(tBTA_AG_DATA *p_data)
{
    /* deregister all scbs in use */
    tBTA_AG_SCB     *p_scb = &bta_ag_cb.scb[0];
    BOOLEAN         do_dereg = FALSE;
    int             i;

    if (!bta_sys_is_register (BTA_ID_AG)) {
        APPL_TRACE_ERROR("BTA AG is already disabled, ignoring ...");
        return;
    }
    /* De-register with BTA system manager */
    bta_sys_deregister(BTA_ID_AG);

    for (i = 0; i < BTA_AG_NUM_SCB; i++, p_scb++) {
        if (p_scb->in_use) {
            bta_ag_sm_execute(p_scb, BTA_AG_API_DEREGISTER_EVT, p_data);
            do_dereg = TRUE;
        }
    }

    if (!do_dereg) {
        /* Done, send callback evt to app */
        (*bta_ag_cb.p_cback)(BTA_AG_DISABLE_EVT, NULL);
    }
    bta_sys_collision_register (BTA_ID_AG, NULL);
}

/*******************************************************************************
**
** Function         bta_ag_api_register
**
** Description      Handle an API event registers a new service.
**
**
** Returns          void
**
*******************************************************************************/
static void bta_ag_api_register(tBTA_AG_DATA *p_data)
{
    tBTA_AG_SCB     *p_scb;
    tBTA_AG_REGISTER reg;

    /* allocate an scb */
    if ((p_scb = bta_ag_scb_alloc()) != NULL) {
        APPL_TRACE_DEBUG("bta_ag_api_register: p_scb 0x%08x ", (unsigned int)p_scb);
        bta_ag_sm_execute(p_scb, p_data->hdr.event, p_data);
    } else {
        reg.status = BTA_AG_FAIL_RESOURCES;
        (*bta_ag_cb.p_cback)(BTA_AG_REGISTER_EVT, (tBTA_AG *) &reg);
    }
}

/*******************************************************************************
**
** Function         bta_ag_api_result
**
** Description      Handle an API result event.
**
**
** Returns          void
**
*******************************************************************************/
static void bta_ag_api_result(tBTA_AG_DATA *p_data)
{
    tBTA_AG_SCB     *p_scb;
    int i;
    
    if (p_data->hdr.layer_specific != BTA_AG_HANDLE_ALL) {
        if ((p_scb = bta_ag_scb_by_idx(p_data->hdr.layer_specific)) != NULL) {
            APPL_TRACE_DEBUG("bta_ag_api_result: p_scb 0x%08x ", (unsigned int)p_scb);
            bta_ag_sm_execute(p_scb, BTA_AG_API_RESULT_EVT, p_data);
        }
    } else {
        for (i = 0, p_scb = &bta_ag_cb.scb[0]; i < BTA_AG_NUM_SCB; i++, p_scb++) {
            if (p_scb->in_use && p_scb->svc_conn) {
                APPL_TRACE_DEBUG("bta_ag_api_result p_scb 0x%08x ", (unsigned int)p_scb);
                bta_ag_sm_execute(p_scb, BTA_AG_API_RESULT_EVT, p_data);
            }
        }
    }
}

/*******************************************************************************
**
** Function         bta_ag_sm_execute
**
** Description      State machine event handling function for AG
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_sm_execute(tBTA_AG_SCB *p_scb, UINT16 event, tBTA_AG_DATA *p_data)
{
    tBTA_AG_ST_TBL      state_table;
    UINT8               action;
    
#if BTA_AG_DEBUG == TRUE
    UINT16  in_event = event;
    UINT8   in_state = p_scb->state;
    /* Ignore displaying of AT results when not connected (Ignored in state machine) */
    if (in_event != BTA_AG_API_RESULT_EVT || p_scb->state == BTA_AG_OPEN_ST) {
        APPL_TRACE_EVENT("AG evt (hdl 0x%04x): State %d (%s), Event 0x%04x (%s)",
                           bta_ag_scb_to_idx(p_scb),
                           p_scb->state, bta_ag_state_str(p_scb->state),
                           event, bta_ag_evt_str(event, p_data->api_result.result));
    }
#else
    APPL_TRACE_EVENT("AG evt (hdl 0x%04x): State %d, Event 0x%04x",
                      bta_ag_scb_to_idx(p_scb), p_scb->state, event);
#endif
    event &= 0x00FF;
    if (event >= (BTA_AG_MAX_EVT & 0x00FF)) {
        APPL_TRACE_ERROR("AG evt out of range, ignoring...");
        return;
    }
    /* look up the state table for the current state */
    state_table = bta_ag_st_tbl[p_scb->state];
    /* set next state */
    p_scb->state = state_table[event][BTA_AG_NEXT_STATE];
    /* execute action functions */
    for (int i = 0; i < BTA_AG_ACTIONS; i++) {
        if ((action = state_table[event][i]) != BTA_AG_IGNORE) {
            (*bta_ag_action[action])(p_scb, p_data);
        } else {
            break;
        }
    }
#if BTA_AG_DEBUG == TRUE
    if (p_scb->state != in_state) {
        APPL_TRACE_EVENT("BTA AG State Change: [%s] -> [%s] after Event [%s]",
                      bta_ag_state_str(in_state),
                      bta_ag_state_str(p_scb->state),
                      bta_ag_evt_str(in_event, p_data->api_result.result));
    }
#endif
}

/*******************************************************************************
**
** Function         bta_ag_hdl_event
**
** Description      Data gateway main event handling function.
**
**
** Returns          BOOLEAN
**
*******************************************************************************/
BOOLEAN bta_ag_hdl_event(BT_HDR *p_msg)
{
    tBTA_AG_SCB *p_scb;
    APPL_TRACE_DEBUG("bta_ag_hdl_event: Event 0x%04x ", p_msg->event);

    switch (p_msg->event) {
        /* handle enable event */
        case BTA_AG_API_ENABLE_EVT:
            bta_ag_api_enable((tBTA_AG_DATA *) p_msg);
            break;

        /* handle disable event */
        case BTA_AG_API_DISABLE_EVT:
            bta_ag_api_disable((tBTA_AG_DATA *) p_msg);
            break;

        /* handle register event */
        case BTA_AG_API_REGISTER_EVT:
            bta_ag_api_register((tBTA_AG_DATA *) p_msg);
            break;

        /* handle result event */
        case BTA_AG_API_RESULT_EVT:
            bta_ag_api_result((tBTA_AG_DATA *) p_msg);
            break;

        /* all others reference scb by handle */
        default:
            if ((p_scb = bta_ag_scb_by_idx(p_msg->layer_specific)) != NULL) {
                APPL_TRACE_DEBUG("bta_ag_hdl_event: p_scb 0x%08x ", (unsigned int)p_scb);
                bta_ag_sm_execute(p_scb, p_msg->event, (tBTA_AG_DATA *) p_msg);
            }
            break;
    }
    return TRUE;
}

#endif /* #if (BTA_AG_INCLUDED == TRUE) */
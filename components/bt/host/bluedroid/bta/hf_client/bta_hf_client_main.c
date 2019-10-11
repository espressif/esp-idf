/******************************************************************************
 *
 *  Copyright (c) 2014 The Android Open Source Project
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

#include <string.h>
#include <stdlib.h>
#include "common/bt_defs.h"
#include "bta/bta_api.h"
#include "bta/bta_sys.h"
#include "bta/bta_hf_client_api.h"
#include "bta_hf_client_int.h"

#if (BTA_HF_INCLUDED == TRUE)
/* uncomment to enable extra debug */
/* #define BTA_HF_CLIENT_DEBUG TRUE */

#ifndef BTA_HF_CLIENT_DEBUG
#define BTA_HF_CLIENT_DEBUG FALSE
#endif

#if BTA_HF_CLIENT_DEBUG == TRUE
static char *bta_hf_client_evt_str(UINT16 event);
static char *bta_hf_client_state_str(UINT8 state);
#endif

/* state machine states */
enum {
    BTA_HF_CLIENT_INIT_ST,
    BTA_HF_CLIENT_OPENING_ST,
    BTA_HF_CLIENT_OPEN_ST,
    BTA_HF_CLIENT_CLOSING_ST
};

/* state machine action enumeration list */
enum {
    BTA_HF_CLIENT_REGISTER,
    BTA_HF_CLIENT_DEREGISTER,
    BTA_HF_CLIENT_START_DEREG,
    BTA_HF_CLIENT_RFC_DO_CLOSE,
    BTA_HF_CLIENT_START_CLOSE,
    BTA_HF_CLIENT_START_OPEN,
    BTA_HF_CLIENT_RFC_ACP_OPEN,
    BTA_HF_CLIENT_SCO_LISTEN,
    BTA_HF_CLIENT_SCO_CONN_OPEN,
    BTA_HF_CLIENT_SCO_CONN_CLOSE,
    BTA_HF_CLIENT_SCO_OPEN,
    BTA_HF_CLIENT_SCO_CLOSE,
    BTA_HF_CLIENT_SCO_SHUTDOWN,
    BTA_HF_CLIENT_FREE_DB,
    BTA_HF_CLIENT_OPEN_FAIL,
    BTA_HF_CLIENT_RFC_OPEN,
    BTA_HF_CLIENT_RFC_FAIL,
    BTA_HF_CLIENT_DISC_INT_RES,
    BTA_HF_CLIENT_RFC_DO_OPEN,
    BTA_HF_CLIENT_DISC_FAIL,
    BTA_HF_CLIENT_RFC_CLOSE,
    BTA_HF_CLIENT_RFC_DATA,
    BTA_HF_CLIENT_DISC_ACP_RES,
    BTA_HF_CLIENT_SVC_CONN_OPEN,
    BTA_HF_CLIENT_SEND_AT_CMD,
#if (BTM_SCO_HCI_INCLUDED == TRUE)
    BTA_HF_CLIENT_CI_SCO_DATA,
#endif
    BTA_HF_CLIENT_NUM_ACTIONS,
};

#define BTA_HF_CLIENT_IGNORE BTA_HF_CLIENT_NUM_ACTIONS

/* type for action functions */
typedef void (*tBTA_HF_CLIENT_ACTION)(tBTA_HF_CLIENT_DATA *p_data);

/* action functions table, indexed with action enum */
const tBTA_HF_CLIENT_ACTION bta_hf_client_action[] = {
    /* BTA_HF_CLIENT_REGISTER */      bta_hf_client_register,
    /* BTA_HF_CLIENT_DEREGISTER */    bta_hf_client_deregister,
    /* BTA_HF_CLIENT_START_DEREG */   bta_hf_client_start_dereg,
    /* BTA_HF_CLIENT_RFC_DO_CLOSE */  bta_hf_client_rfc_do_close,
    /* BTA_HF_CLIENT_START_CLOSE */   bta_hf_client_start_close,
    /* BTA_HF_CLIENT_START_OPEN */    bta_hf_client_start_open,
    /* BTA_HF_CLIENT_RFC_ACP_OPEN */  bta_hf_client_rfc_acp_open,
    /* BTA_HF_CLIENT_SCO_LISTEN */    bta_hf_client_sco_listen,
    /* BTA_HF_CLIENT_SCO_CONN_OPEN */ bta_hf_client_sco_conn_open,
    /* BTA_HF_CLIENT_SCO_CONN_CLOSE*/ bta_hf_client_sco_conn_close,
    /* BTA_HF_CLIENT_SCO_OPEN */      bta_hf_client_sco_open,
    /* BTA_HF_CLIENT_SCO_CLOSE */     bta_hf_client_sco_close,
    /* BTA_HF_CLIENT_SCO_SHUTDOWN */  bta_hf_client_sco_shutdown,
    /* BTA_HF_CLIENT_FREE_DB */       bta_hf_client_free_db,
    /* BTA_HF_CLIENT_OPEN_FAIL */     bta_hf_client_open_fail,
    /* BTA_HF_CLIENT_RFC_OPEN */      bta_hf_client_rfc_open,
    /* BTA_HF_CLIENT_RFC_FAIL */      bta_hf_client_rfc_fail,
    /* BTA_HF_CLIENT_DISC_INT_RES */  bta_hf_client_disc_int_res,
    /* BTA_HF_CLIENT_RFC_DO_OPEN */   bta_hf_client_rfc_do_open,
    /* BTA_HF_CLIENT_DISC_FAIL */     bta_hf_client_disc_fail,
    /* BTA_HF_CLIENT_RFC_CLOSE */     bta_hf_client_rfc_close,
    /* BTA_HF_CLIENT_RFC_DATA */      bta_hf_client_rfc_data,
    /* BTA_HF_CLIENT_DISC_ACP_RES */  bta_hf_client_disc_acp_res,
    /* BTA_HF_CLIENT_SVC_CONN_OPEN */ bta_hf_client_svc_conn_open,
    /* BTA_HF_CLIENT_SEND_AT_CMD */   bta_hf_client_send_at_cmd,
#if (BTM_SCO_HCI_INCLUDED == TRUE )
    /* BTA_HF_CLIENT_CI_SCO_DATA */   bta_hf_client_ci_sco_data,
#endif /* (BTM_SCO_HCI_INCLUDED == TRUE ) */
};

/* state table information */
#define BTA_HF_CLIENT_ACTIONS              2       /* number of actions */
#define BTA_HF_CLIENT_NEXT_STATE           2       /* position of next state */
#define BTA_HF_CLIENT_NUM_COLS             3       /* number of columns in state tables */

/* state table for init state */
const UINT8 bta_hf_client_st_init[][BTA_HF_CLIENT_NUM_COLS] = {
    /* Event                    Action 1                       Action 2                       Next state */
    /* API_REGISTER_EVT */      {BTA_HF_CLIENT_REGISTER,       BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_INIT_ST},
    /* API_DEREGISTER_EVT */    {BTA_HF_CLIENT_DEREGISTER,     BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_INIT_ST},
    /* API_OPEN_EVT */          {BTA_HF_CLIENT_START_OPEN,     BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPENING_ST},
    /* API_CLOSE_EVT */         {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_INIT_ST},
    /* API_AUDIO_OPEN_EVT */    {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_INIT_ST},
    /* API_AUDIO_CLOSE_EVT */   {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_INIT_ST},
    /* RFC_OPEN_EVT */          {BTA_HF_CLIENT_RFC_ACP_OPEN,   BTA_HF_CLIENT_SCO_LISTEN,      BTA_HF_CLIENT_OPEN_ST},
    /* RFC_CLOSE_EVT */         {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_INIT_ST},
    /* RFC_SRV_CLOSE_EVT */     {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_INIT_ST},
    /* RFC_DATA_EVT */          {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_INIT_ST},
    /* DISC_ACP_RES_EVT */      {BTA_HF_CLIENT_FREE_DB,        BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_INIT_ST},
    /* DISC_INT_RES_EVT */      {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_INIT_ST},
    /* DISC_OK_EVT */           {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_INIT_ST},
    /* DISC_FAIL_EVT */         {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_INIT_ST},
    /* SCO_OPEN_EVT */          {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_INIT_ST},
    /* SCO_CLOSE_EVT */         {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_INIT_ST},
    /* SEND_AT_CMD_EVT */       {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_INIT_ST},
#if (BTM_SCO_HCI_INCLUDED == TRUE )
    /* CI_SCO_DATA_EVT */       {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_INIT_ST},
#endif /* (BTM_SCO_HCI_INCLUDED == TRUE ) */
};

/* state table for opening state */
const UINT8 bta_hf_client_st_opening[][BTA_HF_CLIENT_NUM_COLS] = {
    /* Event                    Action 1                       Action 2                       Next state */
    /* API_REGISTER_EVT */      {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPENING_ST},
    /* API_DEREGISTER_EVT */    {BTA_HF_CLIENT_RFC_DO_CLOSE,   BTA_HF_CLIENT_START_DEREG,     BTA_HF_CLIENT_CLOSING_ST},
    /* API_OPEN_EVT */          {BTA_HF_CLIENT_OPEN_FAIL,      BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPENING_ST},
    /* API_CLOSE_EVT */         {BTA_HF_CLIENT_RFC_DO_CLOSE,   BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_CLOSING_ST},
    /* API_AUDIO_OPEN_EVT */    {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPENING_ST},
    /* API_AUDIO_CLOSE_EVT */   {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPENING_ST},
    /* RFC_OPEN_EVT */          {BTA_HF_CLIENT_RFC_OPEN,       BTA_HF_CLIENT_SCO_LISTEN,      BTA_HF_CLIENT_OPEN_ST},
    /* RFC_CLOSE_EVT */         {BTA_HF_CLIENT_RFC_FAIL,       BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_INIT_ST},
    /* RFC_SRV_CLOSE_EVT */     {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPENING_ST},
    /* RFC_DATA_EVT */          {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPENING_ST},
    /* DISC_ACP_RES_EVT */      {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPENING_ST},
    /* DISC_INT_RES_EVT */      {BTA_HF_CLIENT_DISC_INT_RES,   BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPENING_ST},
    /* DISC_OK_EVT */           {BTA_HF_CLIENT_RFC_DO_OPEN,    BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPENING_ST},
    /* DISC_FAIL_EVT */         {BTA_HF_CLIENT_DISC_FAIL,      BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_INIT_ST},
    /* SCO_OPEN_EVT */          {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPENING_ST},
    /* SCO_CLOSE_EVT */         {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPENING_ST},
    /* SEND_AT_CMD_EVT */       {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPENING_ST},
#if (BTM_SCO_HCI_INCLUDED == TRUE )
    /* CI_SCO_DATA_EVT */       {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPENING_ST},
#endif /* (BTM_SCO_HCI_INCLUDED == TRUE ) */
};

/* state table for open state */
const UINT8 bta_hf_client_st_open[][BTA_HF_CLIENT_NUM_COLS] = {
    /* Event                    Action 1                       Action 2                       Next state */
    /* API_REGISTER_EVT */      {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPEN_ST},
    /* API_DEREGISTER_EVT */    {BTA_HF_CLIENT_START_CLOSE,    BTA_HF_CLIENT_START_DEREG,     BTA_HF_CLIENT_CLOSING_ST},
    /* API_OPEN_EVT */          {BTA_HF_CLIENT_OPEN_FAIL,      BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPEN_ST},
    /* API_CLOSE_EVT */         {BTA_HF_CLIENT_START_CLOSE,    BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_CLOSING_ST},
    /* API_AUDIO_OPEN_EVT */    {BTA_HF_CLIENT_SCO_OPEN,       BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPEN_ST},
    /* API_AUDIO_CLOSE_EVT */   {BTA_HF_CLIENT_SCO_CLOSE,      BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPEN_ST},
    /* RFC_OPEN_EVT */          {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPEN_ST},
    /* RFC_CLOSE_EVT */         {BTA_HF_CLIENT_RFC_CLOSE,      BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_INIT_ST},
    /* RFC_SRV_CLOSE_EVT */     {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPEN_ST},
    /* RFC_DATA_EVT */          {BTA_HF_CLIENT_RFC_DATA,       BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPEN_ST},
    /* DISC_ACP_RES_EVT */      {BTA_HF_CLIENT_DISC_ACP_RES,   BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPEN_ST},
    /* DISC_INT_RES_EVT */      {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPEN_ST},
    /* DISC_OK_EVT */           {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPEN_ST},
    /* DISC_FAIL_EVT */         {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPEN_ST},
    /* SCO_OPEN_EVT */          {BTA_HF_CLIENT_SCO_CONN_OPEN,  BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPEN_ST},
    /* SCO_CLOSE_EVT */         {BTA_HF_CLIENT_SCO_CONN_CLOSE, BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPEN_ST},
    /* SEND_AT_CMD_EVT */       {BTA_HF_CLIENT_SEND_AT_CMD,    BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPEN_ST},
#if (BTM_SCO_HCI_INCLUDED == TRUE )
    /* CI_SCO_DATA_EVT */       {BTA_HF_CLIENT_CI_SCO_DATA,    BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_OPEN_ST},
#endif /* (BTM_SCO_HCI_INCLUDED == TRUE ) */
};

/* state table for closing state */
const UINT8 bta_hf_client_st_closing[][BTA_HF_CLIENT_NUM_COLS] = {
    /* Event                    Action 1                       Action 2                       Next state */
    /* API_REGISTER_EVT */      {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_CLOSING_ST},
    /* API_DEREGISTER_EVT */    {BTA_HF_CLIENT_START_DEREG,    BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_CLOSING_ST},
    /* API_OPEN_EVT */          {BTA_HF_CLIENT_OPEN_FAIL,      BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_CLOSING_ST},
    /* API_CLOSE_EVT */         {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_CLOSING_ST},
    /* API_AUDIO_OPEN_EVT */    {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_CLOSING_ST},
    /* API_AUDIO_CLOSE_EVT */   {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_CLOSING_ST},
    /* RFC_OPEN_EVT */          {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_CLOSING_ST},
    /* RFC_CLOSE_EVT */         {BTA_HF_CLIENT_RFC_CLOSE,      BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_INIT_ST},
    /* RFC_SRV_CLOSE_EVT */     {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_CLOSING_ST},
    /* RFC_DATA_EVT */          {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_CLOSING_ST},
    /* DISC_ACP_RES_EVT */      {BTA_HF_CLIENT_FREE_DB,        BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_CLOSING_ST},
    /* DISC_INT_RES_EVT */      {BTA_HF_CLIENT_FREE_DB,        BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_INIT_ST},
    /* DISC_OK_EVT */           {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_CLOSING_ST},
    /* DISC_FAIL_EVT */         {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_CLOSING_ST},
    /* SCO_OPEN_EVT */          {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_CLOSING_ST},
    /* SCO_CLOSE_EVT */         {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_CLOSING_ST},
    /* SEND_AT_CMD_EVT */       {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_CLOSING_ST},
#if (BTM_SCO_HCI_INCLUDED == TRUE )
    /* CI_SCO_DATA_EVT */       {BTA_HF_CLIENT_IGNORE,         BTA_HF_CLIENT_IGNORE,          BTA_HF_CLIENT_CLOSING_ST},
#endif /* (BTM_SCO_HCI_INCLUDED == TRUE ) */
};

/* type for state table */
typedef const UINT8 (*tBTA_HF_CLIENT_ST_TBL)[BTA_HF_CLIENT_NUM_COLS];

/* state table */
const tBTA_HF_CLIENT_ST_TBL bta_hf_client_st_tbl[] = {
    bta_hf_client_st_init,
    bta_hf_client_st_opening,
    bta_hf_client_st_open,
    bta_hf_client_st_closing
};

#if BTM_WBS_INCLUDED
const char *bta_hf_client_version = "1.6";
#else
const char *bta_hf_client_version = "1.5";
#endif

/* HF Client control block */
#if BTA_DYNAMIC_MEMORY == FALSE
tBTA_HF_CLIENT_CB  bta_hf_client_cb;
#else
tBTA_HF_CLIENT_CB  *bta_hf_client_cb_ptr;
#endif


/*******************************************************************************
**
** Function         bta_hf_client_scb_init
**
** Description      Initialize an HF_Client service control block.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_scb_init(void)
{
    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    memset(&bta_hf_client_cb.scb, 0, sizeof(tBTA_HF_CLIENT_SCB));
    bta_hf_client_cb.scb.sco_idx = BTM_INVALID_SCO_INDEX;
    bta_hf_client_cb.scb.negotiated_codec = BTM_SCO_CODEC_CVSD;
}

/*******************************************************************************
**
** Function         bta_hf_client_scb_disable
**
** Description      Disable a service control block.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_scb_disable(void)
{
    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    bta_hf_client_scb_init();

    (*bta_hf_client_cb.p_cback)(BTA_HF_CLIENT_DISABLE_EVT, NULL);
}

/*******************************************************************************
**
** Function         bta_hf_client_resume_open
**
** Description      Resume opening process.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_resume_open (void)
{
    APPL_TRACE_DEBUG ("%s", __FUNCTION__);

    /* resume opening process.  */
    if (bta_hf_client_cb.scb.state == BTA_HF_CLIENT_INIT_ST) {
        bta_hf_client_cb.scb.state = BTA_HF_CLIENT_OPENING_ST;
        bta_hf_client_start_open (NULL);
    }
}

/*******************************************************************************
**
** Function         bta_hf_client_colli_timer_cback
**
** Description      HF Client connection collision timer callback
**
**
** Returns          void
**
*******************************************************************************/
static void bta_hf_client_colli_timer_cback (TIMER_LIST_ENT *p_tle)
{
    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    if (p_tle) {
        bta_hf_client_cb.scb.colli_tmr_on = FALSE;

        /* If the peer haven't opened connection, restart opening process */
        bta_hf_client_resume_open ();
    }
}

/*******************************************************************************
**
** Function         bta_hf_client_collision_cback
**
** Description      Get notified about collision.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_collision_cback (tBTA_SYS_CONN_STATUS status, UINT8 id,
                                    UINT8 app_id, BD_ADDR peer_addr)
{
    UNUSED(status);
    UNUSED(app_id);
    UNUSED(peer_addr);

    if (bta_hf_client_cb.scb.state == BTA_HF_CLIENT_OPENING_ST) {
        if (id == BTA_ID_SYS) { /* ACL collision */
            APPL_TRACE_WARNING ("HF Client found collision (ACL) ...");
        } else if (id == BTA_ID_HS) { /* RFCOMM collision */
            APPL_TRACE_WARNING ("HF Client found collision (RFCOMM) ...");
        } else {
            APPL_TRACE_WARNING ("HF Client found collision (\?\?\?) ...");
        }

        bta_hf_client_cb.scb.state = BTA_HF_CLIENT_INIT_ST;

        /* Cancel SDP if it had been started. */
        if (bta_hf_client_cb.scb.p_disc_db) {
            (void)SDP_CancelServiceSearch (bta_hf_client_cb.scb.p_disc_db);
            bta_hf_client_free_db(NULL);
        }

        /* reopen registered server */
        /* Collision may be detected before or after we close servers. */
        bta_hf_client_start_server();

        /* Start timer to handle connection opening restart */
        bta_hf_client_cb.scb.colli_timer.p_cback = (TIMER_CBACK *)&bta_hf_client_colli_timer_cback;
        bta_sys_start_timer(&bta_hf_client_cb.scb.colli_timer, 0, BTA_HF_CLIENT_COLLISION_TIMER);
        bta_hf_client_cb.scb.colli_tmr_on = TRUE;
    }
}

/*******************************************************************************
**
** Function         bta_hf_client_api_enable
**
** Description      Handle an API enable event.
**
**
** Returns          void
**
*******************************************************************************/
static void bta_hf_client_api_enable(tBTA_HF_CLIENT_DATA *p_data)
{
    /* initialize control block */
    memset(&bta_hf_client_cb, 0, sizeof(tBTA_HF_CLIENT_CB));

    /* store callback function */
    bta_hf_client_cb.p_cback = p_data->api_enable.p_cback;

    /* check if mSBC support enabled */
    if (strcmp(bta_hf_client_version, "1.6") == 0) {
        bta_hf_client_cb.msbc_enabled = TRUE;
    } else{
        bta_hf_client_cb.msbc_enabled = FALSE;
    }

    bta_hf_client_cb.scb.negotiated_codec = BTM_SCO_CODEC_CVSD;

    /* set same setting as AG does */
    BTM_WriteVoiceSettings(AG_VOICE_SETTINGS);

    bta_sys_collision_register (BTA_ID_HS, bta_hf_client_collision_cback);

    /* call callback with enable event */
    (*bta_hf_client_cb.p_cback)(BTA_HF_CLIENT_ENABLE_EVT, NULL);
}

/*******************************************************************************
**
** Function         bta_hf_client_api_disable
**
** Description      Handle an API disable event.
**
**
** Returns          void
**
*******************************************************************************/
static void bta_hf_client_api_disable(tBTA_HF_CLIENT_DATA *p_data)
{
    if (!bta_sys_is_register (BTA_ID_HS)) {
        APPL_TRACE_ERROR("BTA HF Client is already disabled, ignoring ...");
        return;
    }

    /* De-register with BTA system manager */
    bta_sys_deregister(BTA_ID_HS);

    bta_hf_client_sm_execute(BTA_HF_CLIENT_API_DEREGISTER_EVT, p_data);

    bta_sys_collision_register (BTA_ID_HS, NULL);
}

/*******************************************************************************
**
** Function         bta_hf_client_hdl_event
**
** Description      Data HF Client main event handling function.
**
**
** Returns          BOOLEAN
**
*******************************************************************************/
BOOLEAN bta_hf_client_hdl_event(BT_HDR *p_msg)
{
#if BTA_HF_CLIENT_DEBUG == TRUE
    APPL_TRACE_DEBUG("bta_hf_client_hdl_event %s (0x%x)", bta_hf_client_evt_str(p_msg->event), p_msg->event);
#endif

    switch (p_msg->event) {
    /* handle enable event */
    case BTA_HF_CLIENT_API_ENABLE_EVT:
        bta_hf_client_api_enable((tBTA_HF_CLIENT_DATA *) p_msg);
        break;

    /* handle disable event */
    case BTA_HF_CLIENT_API_DISABLE_EVT:
        bta_hf_client_api_disable((tBTA_HF_CLIENT_DATA *) p_msg);
        break;

    default:
        bta_hf_client_sm_execute(p_msg->event, (tBTA_HF_CLIENT_DATA *) p_msg);
        break;
    }
    return TRUE;
}

/*******************************************************************************
**
** Function         bta_hf_client_sm_execute
**
** Description      State machine event handling function for HF Client
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_sm_execute(UINT16 event, tBTA_HF_CLIENT_DATA *p_data)
{
    tBTA_HF_CLIENT_ST_TBL      state_table;
    UINT8               action;
    int                 i;

#if BTA_HF_CLIENT_DEBUG == TRUE
    UINT16  in_event = event;
    UINT8 in_state =  bta_hf_client_cb.scb.state;

    /* Ignore displaying of AT results when not connected (Ignored in state machine) */
    if (bta_hf_client_cb.scb.state == BTA_HF_CLIENT_OPEN_ST) {
        APPL_TRACE_EVENT("HF Client evt : State %d (%s), Event 0x%04x (%s)",
                         bta_hf_client_cb.scb.state,
                         bta_hf_client_state_str(bta_hf_client_cb.scb.state),
                         event, bta_hf_client_evt_str(event));
    }
#endif

    event &= 0x00FF;
    if (event >= (BTA_HF_CLIENT_MAX_EVT & 0x00FF)) {
        APPL_TRACE_ERROR("HF Client evt out of range, ignoring...");
        return;
    }

    /* look up the state table for the current state */
    state_table = bta_hf_client_st_tbl[bta_hf_client_cb.scb.state];

    /* set next state */
    bta_hf_client_cb.scb.state = state_table[event][BTA_HF_CLIENT_NEXT_STATE];

    /* execute action functions */
    for (i = 0; i < BTA_HF_CLIENT_ACTIONS; i++) {
        if ((action = state_table[event][i]) != BTA_HF_CLIENT_IGNORE) {
            (*bta_hf_client_action[action])(p_data);
        } else {
            break;
        }
    }

#if BTA_HF_CLIENT_DEBUG == TRUE
    if (bta_hf_client_cb.scb.state != in_state) {
        APPL_TRACE_EVENT("BTA HF Client State Change: [%s] -> [%s] after Event [%s]",
                         bta_hf_client_state_str(in_state),
                         bta_hf_client_state_str(bta_hf_client_cb.scb.state),
                         bta_hf_client_evt_str(in_event));
    }
#endif
}

static void send_post_slc_cmd(void)
{
    bta_hf_client_cb.scb.at_cb.current_cmd = BTA_HF_CLIENT_AT_NONE;

    bta_hf_client_send_at_bia();
    bta_hf_client_send_at_ccwa(TRUE);
    bta_hf_client_send_at_cmee(TRUE);
    bta_hf_client_send_at_cops(FALSE);
    bta_hf_client_send_at_btrh(TRUE, 0);
    bta_hf_client_send_at_clip(TRUE);
}

/*******************************************************************************
**
** Function         bta_hf_client_slc_seq
**
** Description      Handles AT commands sequence required for SLC creation
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_slc_seq(BOOLEAN error)
{
    APPL_TRACE_DEBUG("bta_hf_client_slc_seq cmd: %u", bta_hf_client_cb.scb.at_cb.current_cmd);

    if (error) {
        /* SLC establishment error, sent close rfcomm event */
        APPL_TRACE_ERROR("HFPClient: Failed to create SLC due to AT error, disconnecting (%u)",
                         bta_hf_client_cb.scb.at_cb.current_cmd);

        bta_hf_client_sm_execute(BTA_HF_CLIENT_API_CLOSE_EVT, NULL);
        return;
    }

    if (bta_hf_client_cb.scb.svc_conn) {
        return;
    }

    switch (bta_hf_client_cb.scb.at_cb.current_cmd) {
    case BTA_HF_CLIENT_AT_NONE:
        bta_hf_client_send_at_brsf();
        break;

    case BTA_HF_CLIENT_AT_BRSF:
        if (bta_hf_client_cb.scb.peer_features & BTA_HF_CLIENT_PEER_CODEC) {
            bta_hf_client_send_at_bac();
            break;
        }

        bta_hf_client_send_at_cind(FALSE);
        break;

    case BTA_HF_CLIENT_AT_BAC:
        bta_hf_client_send_at_cind(FALSE);
        break;

    case BTA_HF_CLIENT_AT_CIND:
        bta_hf_client_send_at_cind(TRUE);
        break;

    case BTA_HF_CLIENT_AT_CIND_STATUS:
        bta_hf_client_send_at_cmer(TRUE);
        break;

    case BTA_HF_CLIENT_AT_CMER:
        if (bta_hf_client_cb.scb.peer_features & BTA_HF_CLIENT_PEER_FEAT_3WAY) {
            bta_hf_client_send_at_chld('?', 0);
        } else {
            bta_hf_client_svc_conn_open(NULL);
            send_post_slc_cmd();
        }
        break;

    case BTA_HF_CLIENT_AT_CHLD:
        bta_hf_client_svc_conn_open(NULL);
        send_post_slc_cmd();
        break;

    default:
        /* If happen there is a bug in SLC creation procedure... */
        APPL_TRACE_ERROR("HFPClient: Failed to create SLCdue to unexpected AT command, disconnecting (%u)",
                         bta_hf_client_cb.scb.at_cb.current_cmd);

        bta_hf_client_sm_execute(BTA_HF_CLIENT_API_CLOSE_EVT, NULL);
        break;
    }
}

#if BTA_HF_CLIENT_DEBUG == TRUE

#ifndef CASE_RETURN_STR
#define CASE_RETURN_STR(const) case const: return #const;
#endif

static char *bta_hf_client_evt_str(UINT16 event)
{
    switch (event) {
        CASE_RETURN_STR(BTA_HF_CLIENT_API_REGISTER_EVT)
        CASE_RETURN_STR(BTA_HF_CLIENT_API_DEREGISTER_EVT)
        CASE_RETURN_STR(BTA_HF_CLIENT_API_OPEN_EVT)
        CASE_RETURN_STR(BTA_HF_CLIENT_API_CLOSE_EVT)
        CASE_RETURN_STR(BTA_HF_CLIENT_API_AUDIO_OPEN_EVT)
        CASE_RETURN_STR(BTA_HF_CLIENT_API_AUDIO_CLOSE_EVT)
        CASE_RETURN_STR(BTA_HF_CLIENT_RFC_OPEN_EVT)
        CASE_RETURN_STR(BTA_HF_CLIENT_RFC_CLOSE_EVT)
        CASE_RETURN_STR(BTA_HF_CLIENT_RFC_SRV_CLOSE_EVT)
        CASE_RETURN_STR(BTA_HF_CLIENT_RFC_DATA_EVT)
        CASE_RETURN_STR(BTA_HF_CLIENT_DISC_ACP_RES_EVT)
        CASE_RETURN_STR(BTA_HF_CLIENT_DISC_INT_RES_EVT)
        CASE_RETURN_STR(BTA_HF_CLIENT_DISC_OK_EVT)
        CASE_RETURN_STR(BTA_HF_CLIENT_DISC_FAIL_EVT)
        CASE_RETURN_STR(BTA_HF_CLIENT_API_ENABLE_EVT)
        CASE_RETURN_STR(BTA_HF_CLIENT_API_DISABLE_EVT)
        CASE_RETURN_STR(BTA_HF_CLIENT_SCO_OPEN_EVT)
        CASE_RETURN_STR(BTA_HF_CLIENT_SCO_CLOSE_EVT)
        CASE_RETURN_STR(BTA_HF_CLIENT_SEND_AT_CMD_EVT)
    default:
        return "Unknown HF Client Event";
    }
}

static char *bta_hf_client_state_str(UINT8 state)
{
    switch (state) {
        CASE_RETURN_STR(BTA_HF_CLIENT_INIT_ST)
        CASE_RETURN_STR(BTA_HF_CLIENT_OPENING_ST)
        CASE_RETURN_STR(BTA_HF_CLIENT_OPEN_ST)
        CASE_RETURN_STR(BTA_HF_CLIENT_CLOSING_ST)
    default:
        return "Unknown HF Client State";
    }
}
#endif
#endif /* #if (BTA_HF_INCLUDED == TRUE) */

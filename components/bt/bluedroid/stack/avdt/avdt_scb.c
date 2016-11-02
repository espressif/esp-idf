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
 *  This module contains the stream control block and functions which
 *  operate on the stream control block.
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

/*****************************************************************************
** state machine constants and types
*****************************************************************************/
#if AVDT_DEBUG == TRUE

/* verbose state strings for trace */
const char * const avdt_scb_st_str[] = {
    "SCB_IDLE_ST",
    "SCB_CONF_ST",
    "SCB_OPENING_ST",
    "SCB_OPEN_ST",
    "SCB_STREAM_ST",
    "SCB_CLOSING_ST"
};

/* verbose event strings for trace */
const char * const avdt_scb_evt_str[] = {
    "API_REMOVE_EVT",
    "API_WRITE_REQ_EVT",
    "API_GETCONFIG_REQ_EVT",
    "API_DELAY_RPT_REQ",
    "API_SETCONFIG_REQ_EVT",
    "API_OPEN_REQ_EVT",
    "API_CLOSE_REQ_EVT",
    "API_RECONFIG_REQ_EVT",
    "API_SECURITY_REQ_EVT",
    "API_ABORT_REQ_EVT",
    "API_GETCONFIG_RSP_EVT",
    "API_SETCONFIG_RSP_EVT",
    "API_SETCONFIG_REJ_EVT",
    "API_OPEN_RSP_EVT",
    "API_CLOSE_RSP_EVT",
    "API_RECONFIG_RSP_EVT",
    "API_SECURITY_RSP_EVT",
    "API_ABORT_RSP_EVT",
    "MSG_SETCONFIG_CMD_EVT",
    "MSG_GETCONFIG_CMD_EVT",
    "MSG_OPEN_CMD_EVT",
    "MSG_START_CMD_EVT",
    "MSG_SUSPEND_CMD_EVT",
    "MSG_CLOSE_CMD_EVT",
    "MSG_ABORT_CMD_EVT",
    "MSG_RECONFIG_CMD_EVT",
    "MSG_SECURITY_CMD_EVT",
    "MSG_DELAY_RPT_CMD_EVT",
    "MSG_DELAY_RPT_RSP_EVT",
    "MSG_SETCONFIG_RSP_EVT",
    "MSG_GETCONFIG_RSP_EVT",
    "MSG_OPEN_RSP_EVT",
    "MSG_START_RSP_EVT",
    "MSG_SUSPEND_RSP_EVT",
    "MSG_CLOSE_RSP_EVT",
    "MSG_ABORT_RSP_EVT",
    "MSG_RECONFIG_RSP_EVT",
    "MSG_SECURITY_RSP_EVT",
    "MSG_SETCONFIG_REJ_EVT",
    "MSG_OPEN_REJ_EVT",
    "MSG_START_REJ_EVT",
    "MSG_SUSPEND_REJ_EVT",
    "TC_TOUT_EVT",
    "TC_OPEN_EVT",
    "TC_CLOSE_EVT",
    "TC_CONG_EVT",
    "TC_DATA_EVT",
    "CC_CLOSE_EVT"
};

#endif


/* action function list */
const tAVDT_SCB_ACTION avdt_scb_action[] = {
    avdt_scb_hdl_abort_cmd,
    avdt_scb_hdl_abort_rsp,
    avdt_scb_hdl_close_cmd,
    avdt_scb_hdl_close_rsp,
    avdt_scb_hdl_getconfig_cmd,
    avdt_scb_hdl_getconfig_rsp,
    avdt_scb_hdl_open_cmd,
    avdt_scb_hdl_open_rej,
    avdt_scb_hdl_open_rsp,
    avdt_scb_hdl_pkt,
    avdt_scb_drop_pkt,
    avdt_scb_hdl_reconfig_cmd,
    avdt_scb_hdl_reconfig_rsp,
    avdt_scb_hdl_security_cmd,
    avdt_scb_hdl_security_rsp,
    avdt_scb_hdl_setconfig_cmd,
    avdt_scb_hdl_setconfig_rej,
    avdt_scb_hdl_setconfig_rsp,
    avdt_scb_hdl_start_cmd,
    avdt_scb_hdl_start_rsp,
    avdt_scb_hdl_suspend_cmd,
    avdt_scb_hdl_suspend_rsp,
    avdt_scb_hdl_tc_close,
#if AVDT_REPORTING == TRUE
    avdt_scb_hdl_tc_close_sto,
#endif
    avdt_scb_hdl_tc_open,
#if AVDT_REPORTING == TRUE
    avdt_scb_hdl_tc_open_sto,
#endif
    avdt_scb_snd_delay_rpt_req,
    avdt_scb_hdl_delay_rpt_cmd,
    avdt_scb_hdl_delay_rpt_rsp,
    avdt_scb_hdl_write_req,
    avdt_scb_snd_abort_req,
    avdt_scb_snd_abort_rsp,
    avdt_scb_snd_close_req,
    avdt_scb_snd_stream_close,
    avdt_scb_snd_close_rsp,
    avdt_scb_snd_getconfig_req,
    avdt_scb_snd_getconfig_rsp,
    avdt_scb_snd_open_req,
    avdt_scb_snd_open_rsp,
    avdt_scb_snd_reconfig_req,
    avdt_scb_snd_reconfig_rsp,
    avdt_scb_snd_security_req,
    avdt_scb_snd_security_rsp,
    avdt_scb_snd_setconfig_req,
    avdt_scb_snd_setconfig_rej,
    avdt_scb_snd_setconfig_rsp,
    avdt_scb_snd_tc_close,
    avdt_scb_cb_err,
    avdt_scb_cong_state,
    avdt_scb_rej_state,
    avdt_scb_rej_in_use,
    avdt_scb_rej_not_in_use,
    avdt_scb_set_remove,
    avdt_scb_free_pkt,
    avdt_scb_clr_pkt,
    avdt_scb_chk_snd_pkt,
    avdt_scb_tc_timer,
    avdt_scb_clr_vars,
    avdt_scb_dealloc
};

/* state table information */
#define AVDT_SCB_ACTIONS            2       /* number of actions */
#define AVDT_SCB_NEXT_STATE         2       /* position of next state */
#define AVDT_SCB_NUM_COLS           3       /* number of columns in state tables */

/* state table for idle state */
const UINT8 avdt_scb_st_idle[][AVDT_SCB_NUM_COLS] = {
/* Event                     Action 1                       Action 2                    Next state */
/* API_REMOVE_EVT */        {AVDT_SCB_DEALLOC,              AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* API_WRITE_REQ_EVT */     {AVDT_SCB_FREE_PKT,             AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* API_GETCONFIG_REQ_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* API_DELAY_RPT_REQ_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* API_SETCONFIG_REQ_EVT */ {AVDT_SCB_SND_SETCONFIG_REQ,    AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* API_OPEN_REQ_EVT */      {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* API_CLOSE_REQ_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* API_RECONFIG_REQ_EVT */  {AVDT_SCB_CB_ERR,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* API_SECURITY_REQ_EVT */  {AVDT_SCB_CB_ERR,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* API_ABORT_REQ_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* API_GETCONFIG_RSP_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* API_SETCONFIG_RSP_EVT */ {AVDT_SCB_SND_SETCONFIG_RSP,    AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* API_SETCONFIG_REJ_EVT */ {AVDT_SCB_SND_SETCONFIG_REJ,    AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* API_OPEN_RSP_EVT */      {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* API_CLOSE_RSP_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* API_RECONFIG_RSP_EVT */  {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* API_SECURITY_RSP_EVT */  {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* API_ABORT_RSP_EVT */     {AVDT_SCB_SND_ABORT_RSP,        AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_SETCONFIG_CMD_EVT */ {AVDT_SCB_HDL_SETCONFIG_CMD,    AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_GETCONFIG_CMD_EVT */ {AVDT_SCB_REJ_STATE,            AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_OPEN_CMD_EVT */      {AVDT_SCB_REJ_STATE,            AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_START_CMD_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_SUSPEND_CMD_EVT */   {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_CLOSE_CMD_EVT */     {AVDT_SCB_REJ_STATE,            AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_ABORT_CMD_EVT */     {AVDT_SCB_HDL_ABORT_CMD,        AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_RECONFIG_CMD_EVT */  {AVDT_SCB_REJ_NOT_IN_USE,       AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_SECURITY_CMD_EVT */  {AVDT_SCB_REJ_STATE,            AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_DELAY_RPT_CMD_EVT */ {AVDT_SCB_REJ_STATE,            AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_DELAY_RPT_RSP_EVT */ {AVDT_SCB_HDL_DELAY_RPT_RSP,    AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_SETCONFIG_RSP_EVT */ {AVDT_SCB_HDL_SETCONFIG_RSP,    AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* MSG_GETCONFIG_RSP_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_OPEN_RSP_EVT */      {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_START_RSP_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_SUSPEND_RSP_EVT */   {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_CLOSE_RSP_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_ABORT_RSP_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_RECONFIG_RSP_EVT */  {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_SECURITY_RSP_EVT */  {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_SETCONFIG_REJ_EVT */ {AVDT_SCB_HDL_SETCONFIG_REJ,    AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_OPEN_REJ_EVT */      {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_START_REJ_EVT */     {AVDT_SCB_HDL_START_RSP,        AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_SUSPEND_REJ_EVT */   {AVDT_SCB_HDL_SUSPEND_RSP,      AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* TC_TOUT_EVT */           {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* TC_OPEN_EVT */           {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* TC_CLOSE_EVT */          {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* TC_CONG_EVT */           {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* TC_DATA_EVT */           {AVDT_SCB_DROP_PKT,             AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* CC_CLOSE_EVT */          {AVDT_SCB_CLR_VARS,             AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST}
};

/* state table for configured state */
const UINT8 avdt_scb_st_conf[][AVDT_SCB_NUM_COLS] = {
/* Event                     Action 1                       Action 2                    Next state */
/* API_REMOVE_EVT */        {AVDT_SCB_SND_ABORT_REQ,        AVDT_SCB_SET_REMOVE,        AVDT_SCB_CONF_ST},
/* API_WRITE_REQ_EVT */     {AVDT_SCB_FREE_PKT,             AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* API_GETCONFIG_REQ_EVT */ {AVDT_SCB_SND_GETCONFIG_REQ,    AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* API_DELAY_RPT_REQ_EVT */ {AVDT_SCB_SND_DELAY_RPT_REQ,    AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* API_SETCONFIG_REQ_EVT */ {AVDT_SCB_CB_ERR,               AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* API_OPEN_REQ_EVT */      {AVDT_SCB_SND_OPEN_REQ,         AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* API_CLOSE_REQ_EVT */     {AVDT_SCB_SND_ABORT_REQ,        AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* API_RECONFIG_REQ_EVT */  {AVDT_SCB_CB_ERR,               AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* API_SECURITY_REQ_EVT */  {AVDT_SCB_SND_SECURITY_REQ,     AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* API_ABORT_REQ_EVT */     {AVDT_SCB_SND_ABORT_REQ,        AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* API_GETCONFIG_RSP_EVT */ {AVDT_SCB_SND_GETCONFIG_RSP,    AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* API_SETCONFIG_RSP_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* API_SETCONFIG_REJ_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* API_OPEN_RSP_EVT */      {AVDT_SCB_SND_OPEN_RSP,         AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* API_CLOSE_RSP_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* API_RECONFIG_RSP_EVT */  {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* API_SECURITY_RSP_EVT */  {AVDT_SCB_SND_SECURITY_RSP,     AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* API_ABORT_RSP_EVT */     {AVDT_SCB_SND_ABORT_RSP,        AVDT_SCB_HDL_TC_CLOSE,      AVDT_SCB_IDLE_ST},
/* MSG_SETCONFIG_CMD_EVT */ {AVDT_SCB_REJ_IN_USE,           AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* MSG_GETCONFIG_CMD_EVT */ {AVDT_SCB_HDL_GETCONFIG_CMD,    AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* MSG_OPEN_CMD_EVT */      {AVDT_SCB_HDL_OPEN_CMD,         AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* MSG_START_CMD_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* MSG_SUSPEND_CMD_EVT */   {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* MSG_CLOSE_CMD_EVT */     {AVDT_SCB_REJ_STATE,            AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* MSG_ABORT_CMD_EVT */     {AVDT_SCB_HDL_ABORT_CMD,        AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* MSG_RECONFIG_CMD_EVT */  {AVDT_SCB_REJ_STATE,            AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* MSG_SECURITY_CMD_EVT */  {AVDT_SCB_HDL_SECURITY_CMD,     AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* MSG_DELAY_RPT_CMD_EVT */ {AVDT_SCB_HDL_DELAY_RPT_CMD,    AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* MSG_DELAY_RPT_RSP_EVT */ {AVDT_SCB_HDL_DELAY_RPT_RSP,    AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* MSG_SETCONFIG_RSP_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* MSG_GETCONFIG_RSP_EVT */ {AVDT_SCB_HDL_GETCONFIG_RSP,    AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* MSG_OPEN_RSP_EVT */      {AVDT_SCB_HDL_OPEN_RSP,         AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* MSG_START_RSP_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* MSG_SUSPEND_RSP_EVT */   {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* MSG_CLOSE_RSP_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* MSG_ABORT_RSP_EVT */     {AVDT_SCB_HDL_ABORT_RSP,        AVDT_SCB_HDL_TC_CLOSE,      AVDT_SCB_IDLE_ST},
/* MSG_RECONFIG_RSP_EVT */  {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* MSG_SECURITY_RSP_EVT */  {AVDT_SCB_HDL_SECURITY_RSP,     AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* MSG_SETCONFIG_REJ_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* MSG_OPEN_REJ_EVT */      {AVDT_SCB_HDL_OPEN_REJ,         AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_START_REJ_EVT */     {AVDT_SCB_HDL_START_RSP,        AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* MSG_SUSPEND_REJ_EVT */   {AVDT_SCB_HDL_SUSPEND_RSP,      AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* TC_TOUT_EVT */           {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* TC_OPEN_EVT */           {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* TC_CLOSE_EVT */          {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* TC_CONG_EVT */           {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* TC_DATA_EVT */           {AVDT_SCB_DROP_PKT,             AVDT_SCB_IGNORE,            AVDT_SCB_CONF_ST},
/* CC_CLOSE_EVT */          {AVDT_SCB_HDL_TC_CLOSE,         AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST}
};

/* state table for opening state */
const UINT8 avdt_scb_st_opening[][AVDT_SCB_NUM_COLS] = {
/* Event                     Action 1                       Action 2                    Next state */
/* API_REMOVE_EVT */        {AVDT_SCB_SND_CLOSE_REQ,        AVDT_SCB_SET_REMOVE,        AVDT_SCB_CLOSING_ST},
/* API_WRITE_REQ_EVT */     {AVDT_SCB_FREE_PKT,             AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* API_GETCONFIG_REQ_EVT */ {AVDT_SCB_SND_GETCONFIG_REQ,    AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* API_DELAY_RPT_REQ_EVT */ {AVDT_SCB_SND_DELAY_RPT_REQ,    AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* API_SETCONFIG_REQ_EVT */ {AVDT_SCB_CB_ERR,               AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* API_OPEN_REQ_EVT */      {AVDT_SCB_CB_ERR,               AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* API_CLOSE_REQ_EVT */     {AVDT_SCB_SND_CLOSE_REQ,        AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* API_RECONFIG_REQ_EVT */  {AVDT_SCB_CB_ERR,               AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* API_SECURITY_REQ_EVT */  {AVDT_SCB_SND_SECURITY_REQ,     AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* API_ABORT_REQ_EVT */     {AVDT_SCB_SND_ABORT_REQ,        AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* API_GETCONFIG_RSP_EVT */ {AVDT_SCB_SND_GETCONFIG_RSP,    AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* API_SETCONFIG_RSP_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* API_SETCONFIG_REJ_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* API_OPEN_RSP_EVT */      {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* API_CLOSE_RSP_EVT */     {AVDT_SCB_SND_CLOSE_RSP,        AVDT_SCB_SND_TC_CLOSE,      AVDT_SCB_CLOSING_ST},
/* API_RECONFIG_RSP_EVT */  {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* API_SECURITY_RSP_EVT */  {AVDT_SCB_SND_SECURITY_RSP,     AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* API_ABORT_RSP_EVT */     {AVDT_SCB_SND_ABORT_RSP,        AVDT_SCB_SND_TC_CLOSE,      AVDT_SCB_CLOSING_ST},
/* MSG_SETCONFIG_CMD_EVT */ {AVDT_SCB_REJ_STATE,            AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* MSG_GETCONFIG_CMD_EVT */ {AVDT_SCB_HDL_GETCONFIG_CMD,    AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* MSG_OPEN_CMD_EVT */      {AVDT_SCB_REJ_STATE,            AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* MSG_START_CMD_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* MSG_SUSPEND_CMD_EVT */   {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* MSG_CLOSE_CMD_EVT */     {AVDT_SCB_HDL_CLOSE_CMD,        AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* MSG_ABORT_CMD_EVT */     {AVDT_SCB_HDL_ABORT_CMD,        AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* MSG_RECONFIG_CMD_EVT */  {AVDT_SCB_REJ_STATE,            AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* MSG_SECURITY_CMD_EVT */  {AVDT_SCB_HDL_SECURITY_CMD,     AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* MSG_DELAY_RPT_CMD_EVT */ {AVDT_SCB_HDL_DELAY_RPT_CMD,    AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* MSG_DELAY_RPT_RSP_EVT */ {AVDT_SCB_HDL_DELAY_RPT_RSP,    AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* MSG_SETCONFIG_RSP_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* MSG_GETCONFIG_RSP_EVT */ {AVDT_SCB_HDL_GETCONFIG_RSP,    AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* MSG_OPEN_RSP_EVT */      {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* MSG_START_RSP_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* MSG_SUSPEND_RSP_EVT */   {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* MSG_CLOSE_RSP_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* MSG_ABORT_RSP_EVT */     {AVDT_SCB_SND_TC_CLOSE,         AVDT_SCB_HDL_ABORT_RSP,     AVDT_SCB_CLOSING_ST},
/* MSG_RECONFIG_RSP_EVT */  {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* MSG_SECURITY_RSP_EVT */  {AVDT_SCB_HDL_SECURITY_RSP,     AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* MSG_SETCONFIG_REJ_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* MSG_OPEN_REJ_EVT */      {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* MSG_START_REJ_EVT */     {AVDT_SCB_HDL_START_RSP,        AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* MSG_SUSPEND_REJ_EVT */   {AVDT_SCB_HDL_SUSPEND_RSP,      AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* TC_TOUT_EVT */           {AVDT_SCB_SND_ABORT_REQ,        AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* TC_OPEN_EVT */           {AVDT_SCB_HDL_TC_OPEN,          AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* TC_CLOSE_EVT */          {AVDT_SCB_HDL_TC_CLOSE,         AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* TC_CONG_EVT */           {AVDT_SCB_CONG_STATE,           AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* TC_DATA_EVT */           {AVDT_SCB_DROP_PKT,             AVDT_SCB_IGNORE,            AVDT_SCB_OPENING_ST},
/* CC_CLOSE_EVT */          {AVDT_SCB_SND_TC_CLOSE,         AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST}
};

/* state table for open state */
const UINT8 avdt_scb_st_open[][AVDT_SCB_NUM_COLS] = {
/* Event                     Action 1                       Action 2                    Next state */
/* API_REMOVE_EVT */        {AVDT_SCB_SND_CLOSE_REQ,        AVDT_SCB_SET_REMOVE,        AVDT_SCB_CLOSING_ST},
/* API_WRITE_REQ_EVT */     {AVDT_SCB_FREE_PKT,             AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* API_GETCONFIG_REQ_EVT */ {AVDT_SCB_SND_GETCONFIG_REQ,    AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* API_DELAY_RPT_REQ_EVT */ {AVDT_SCB_SND_DELAY_RPT_REQ,    AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* API_SETCONFIG_REQ_EVT */ {AVDT_SCB_CB_ERR,               AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* API_OPEN_REQ_EVT */      {AVDT_SCB_CB_ERR,               AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* API_CLOSE_REQ_EVT */     {AVDT_SCB_SND_CLOSE_REQ,        AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* API_RECONFIG_REQ_EVT */  {AVDT_SCB_SND_RECONFIG_REQ,     AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* API_SECURITY_REQ_EVT */  {AVDT_SCB_SND_SECURITY_REQ,     AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* API_ABORT_REQ_EVT */     {AVDT_SCB_SND_ABORT_REQ,        AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* API_GETCONFIG_RSP_EVT */ {AVDT_SCB_SND_GETCONFIG_RSP,    AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* API_SETCONFIG_RSP_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* API_SETCONFIG_REJ_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* API_OPEN_RSP_EVT */      {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* API_CLOSE_RSP_EVT */     {AVDT_SCB_SND_CLOSE_RSP,        AVDT_SCB_TC_TIMER,          AVDT_SCB_CLOSING_ST},
/* API_RECONFIG_RSP_EVT */  {AVDT_SCB_SND_RECONFIG_RSP,     AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* API_SECURITY_RSP_EVT */  {AVDT_SCB_SND_SECURITY_RSP,     AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* API_ABORT_RSP_EVT */     {AVDT_SCB_SND_ABORT_RSP,        AVDT_SCB_TC_TIMER,          AVDT_SCB_CLOSING_ST},
/* MSG_SETCONFIG_CMD_EVT */ {AVDT_SCB_REJ_STATE,            AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* MSG_GETCONFIG_CMD_EVT */ {AVDT_SCB_HDL_GETCONFIG_CMD,    AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* MSG_OPEN_CMD_EVT */      {AVDT_SCB_REJ_STATE,            AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* MSG_START_CMD_EVT */     {AVDT_SCB_HDL_START_CMD,        AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* MSG_SUSPEND_CMD_EVT */   {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* MSG_CLOSE_CMD_EVT */     {AVDT_SCB_HDL_CLOSE_CMD,        AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* MSG_ABORT_CMD_EVT */     {AVDT_SCB_HDL_ABORT_CMD,        AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* MSG_RECONFIG_CMD_EVT */  {AVDT_SCB_HDL_RECONFIG_CMD,     AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* MSG_SECURITY_CMD_EVT */  {AVDT_SCB_HDL_SECURITY_CMD,     AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* MSG_DELAY_RPT_CMD_EVT */ {AVDT_SCB_HDL_DELAY_RPT_CMD,    AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* MSG_DELAY_RPT_RSP_EVT */ {AVDT_SCB_HDL_DELAY_RPT_RSP,    AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* MSG_SETCONFIG_RSP_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* MSG_GETCONFIG_RSP_EVT */ {AVDT_SCB_HDL_GETCONFIG_RSP,    AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* MSG_OPEN_RSP_EVT */      {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* MSG_START_RSP_EVT */     {AVDT_SCB_HDL_START_RSP,        AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* MSG_SUSPEND_RSP_EVT */   {AVDT_SCB_HDL_SUSPEND_RSP,      AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* MSG_CLOSE_RSP_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* MSG_ABORT_RSP_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* MSG_RECONFIG_RSP_EVT */  {AVDT_SCB_HDL_RECONFIG_RSP,     AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* MSG_SECURITY_RSP_EVT */  {AVDT_SCB_HDL_SECURITY_RSP,     AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* MSG_SETCONFIG_REJ_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* MSG_OPEN_REJ_EVT */      {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* MSG_START_REJ_EVT */     {AVDT_SCB_HDL_START_RSP,        AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* MSG_SUSPEND_REJ_EVT */   {AVDT_SCB_HDL_SUSPEND_RSP,      AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* TC_TOUT_EVT */           {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
#if AVDT_REPORTING == TRUE
/* TC_OPEN_EVT */           {AVDT_SCB_HDL_TC_OPEN_STO,      AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* TC_CLOSE_EVT */          {AVDT_SCB_HDL_TC_CLOSE_STO,     AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
#else
/* TC_OPEN_EVT */           {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* TC_CLOSE_EVT */          {AVDT_SCB_HDL_TC_CLOSE,         AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
#endif
/* TC_CONG_EVT */           {AVDT_SCB_CONG_STATE,           AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* TC_DATA_EVT */           {AVDT_SCB_DROP_PKT,             AVDT_SCB_IGNORE,            AVDT_SCB_OPEN_ST},
/* CC_CLOSE_EVT */          {AVDT_SCB_SND_TC_CLOSE,         AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST}
};

/* state table for streaming state */
const UINT8 avdt_scb_st_stream[][AVDT_SCB_NUM_COLS] = {
/* Event                     Action 1                       Action 2                    Next state */
/* API_REMOVE_EVT */        {AVDT_SCB_SND_STREAM_CLOSE,     AVDT_SCB_SET_REMOVE,        AVDT_SCB_CLOSING_ST},
/* API_WRITE_REQ_EVT */     {AVDT_SCB_HDL_WRITE_REQ,        AVDT_SCB_CHK_SND_PKT,       AVDT_SCB_STREAM_ST},
/* API_GETCONFIG_REQ_EVT */ {AVDT_SCB_SND_GETCONFIG_REQ,    AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* API_DELAY_RPT_REQ_EVT */ {AVDT_SCB_SND_DELAY_RPT_REQ,    AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* API_SETCONFIG_REQ_EVT */ {AVDT_SCB_CB_ERR,               AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* API_OPEN_REQ_EVT */      {AVDT_SCB_CB_ERR,               AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* API_CLOSE_REQ_EVT */     {AVDT_SCB_SND_STREAM_CLOSE,     AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* API_RECONFIG_REQ_EVT */  {AVDT_SCB_CB_ERR,               AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* API_SECURITY_REQ_EVT */  {AVDT_SCB_SND_SECURITY_REQ,     AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* API_ABORT_REQ_EVT */     {AVDT_SCB_SND_ABORT_REQ,        AVDT_SCB_CLR_PKT,           AVDT_SCB_CLOSING_ST},
/* API_GETCONFIG_RSP_EVT */ {AVDT_SCB_SND_GETCONFIG_RSP,    AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* API_SETCONFIG_RSP_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* API_SETCONFIG_REJ_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* API_OPEN_RSP_EVT */      {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* API_CLOSE_RSP_EVT */     {AVDT_SCB_SND_CLOSE_RSP,        AVDT_SCB_TC_TIMER,          AVDT_SCB_CLOSING_ST},
/* API_RECONFIG_RSP_EVT */  {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* API_SECURITY_RSP_EVT */  {AVDT_SCB_SND_SECURITY_RSP,     AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* API_ABORT_RSP_EVT */     {AVDT_SCB_SND_ABORT_RSP,        AVDT_SCB_TC_TIMER,          AVDT_SCB_CLOSING_ST},
/* MSG_SETCONFIG_CMD_EVT */ {AVDT_SCB_REJ_STATE,            AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* MSG_GETCONFIG_CMD_EVT */ {AVDT_SCB_HDL_GETCONFIG_CMD,    AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* MSG_OPEN_CMD_EVT */      {AVDT_SCB_REJ_STATE,            AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* MSG_START_CMD_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* MSG_SUSPEND_CMD_EVT */   {AVDT_SCB_HDL_SUSPEND_CMD,      AVDT_SCB_CLR_PKT,           AVDT_SCB_OPEN_ST},
/* MSG_CLOSE_CMD_EVT */     {AVDT_SCB_HDL_CLOSE_CMD,        AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* MSG_ABORT_CMD_EVT */     {AVDT_SCB_HDL_ABORT_CMD,        AVDT_SCB_CLR_PKT,           AVDT_SCB_STREAM_ST},
/* MSG_RECONFIG_CMD_EVT */  {AVDT_SCB_REJ_STATE,            AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* MSG_SECURITY_CMD_EVT */  {AVDT_SCB_HDL_SECURITY_CMD,     AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* MSG_DELAY_RPT_CMD_EVT */ {AVDT_SCB_HDL_DELAY_RPT_CMD,    AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* MSG_DELAY_RPT_RSP_EVT */ {AVDT_SCB_HDL_DELAY_RPT_RSP,    AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* MSG_SETCONFIG_RSP_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* MSG_GETCONFIG_RSP_EVT */ {AVDT_SCB_HDL_GETCONFIG_RSP,    AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* MSG_OPEN_RSP_EVT */      {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* MSG_START_RSP_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* MSG_SUSPEND_RSP_EVT */   {AVDT_SCB_HDL_SUSPEND_RSP,      AVDT_SCB_CLR_PKT,           AVDT_SCB_OPEN_ST},
/* MSG_CLOSE_RSP_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* MSG_ABORT_RSP_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* MSG_RECONFIG_RSP_EVT */  {AVDT_SCB_HDL_RECONFIG_RSP,     AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* MSG_SECURITY_RSP_EVT */  {AVDT_SCB_HDL_SECURITY_RSP,     AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* MSG_SETCONFIG_REJ_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* MSG_OPEN_REJ_EVT */      {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* MSG_START_REJ_EVT */     {AVDT_SCB_HDL_START_RSP,        AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* MSG_SUSPEND_REJ_EVT */   {AVDT_SCB_HDL_SUSPEND_RSP,      AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* TC_TOUT_EVT */           {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* TC_OPEN_EVT */           {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* TC_CLOSE_EVT */          {AVDT_SCB_HDL_TC_CLOSE,         AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* TC_CONG_EVT */           {AVDT_SCB_CONG_STATE,           AVDT_SCB_CHK_SND_PKT,       AVDT_SCB_STREAM_ST},
/* TC_DATA_EVT */           {AVDT_SCB_HDL_PKT,              AVDT_SCB_IGNORE,            AVDT_SCB_STREAM_ST},
/* CC_CLOSE_EVT */          {AVDT_SCB_SND_TC_CLOSE,         AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST}
};

/* state table for closing state */
const UINT8 avdt_scb_st_closing[][AVDT_SCB_NUM_COLS] = {
/* Event                     Action 1                       Action 2                    Next state */
/* API_REMOVE_EVT */        {AVDT_SCB_SET_REMOVE,           AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* API_WRITE_REQ_EVT */     {AVDT_SCB_FREE_PKT,             AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* API_GETCONFIG_REQ_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* API_DELAY_RPT_REQ_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* API_SETCONFIG_REQ_EVT */ {AVDT_SCB_CB_ERR,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* API_OPEN_REQ_EVT */      {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* API_CLOSE_REQ_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* API_RECONFIG_REQ_EVT */  {AVDT_SCB_CB_ERR,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* API_SECURITY_REQ_EVT */  {AVDT_SCB_CB_ERR,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* API_ABORT_REQ_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* API_GETCONFIG_RSP_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* API_SETCONFIG_RSP_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* API_SETCONFIG_REJ_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* API_OPEN_RSP_EVT */      {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* API_CLOSE_RSP_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* API_RECONFIG_RSP_EVT */  {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* API_SECURITY_RSP_EVT */  {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* API_ABORT_RSP_EVT */     {AVDT_SCB_SND_ABORT_RSP,        AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* MSG_SETCONFIG_CMD_EVT */ {AVDT_SCB_REJ_STATE,            AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* MSG_GETCONFIG_CMD_EVT */ {AVDT_SCB_REJ_STATE,            AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* MSG_OPEN_CMD_EVT */      {AVDT_SCB_REJ_STATE,            AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* MSG_START_CMD_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* MSG_SUSPEND_CMD_EVT */   {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* MSG_CLOSE_CMD_EVT */     {AVDT_SCB_REJ_STATE,            AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* MSG_ABORT_CMD_EVT */     {AVDT_SCB_HDL_ABORT_CMD,        AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* MSG_RECONFIG_CMD_EVT */  {AVDT_SCB_REJ_STATE,            AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* MSG_SECURITY_CMD_EVT */  {AVDT_SCB_REJ_STATE,            AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* MSG_DELAY_RPT_CMD_EVT */ {AVDT_SCB_REJ_STATE,            AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* MSG_DELAY_RPT_RSP_EVT */ {AVDT_SCB_HDL_DELAY_RPT_RSP,    AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* MSG_SETCONFIG_RSP_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* MSG_GETCONFIG_RSP_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* MSG_OPEN_RSP_EVT */      {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* MSG_START_RSP_EVT */     {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* MSG_SUSPEND_RSP_EVT */   {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* MSG_CLOSE_RSP_EVT */     {AVDT_SCB_SND_TC_CLOSE,         AVDT_SCB_HDL_CLOSE_RSP,     AVDT_SCB_CLOSING_ST},
/* MSG_ABORT_RSP_EVT */     {AVDT_SCB_SND_TC_CLOSE,         AVDT_SCB_HDL_ABORT_RSP,     AVDT_SCB_CLOSING_ST},
/* MSG_RECONFIG_RSP_EVT */  {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* MSG_SECURITY_RSP_EVT */  {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* MSG_SETCONFIG_REJ_EVT */ {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* MSG_OPEN_REJ_EVT */      {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* MSG_START_REJ_EVT */     {AVDT_SCB_HDL_START_RSP,        AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* MSG_SUSPEND_REJ_EVT */   {AVDT_SCB_HDL_SUSPEND_RSP,      AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* TC_TOUT_EVT */           {AVDT_SCB_SND_TC_CLOSE,         AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* TC_OPEN_EVT */           {AVDT_SCB_SND_TC_CLOSE,         AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* TC_CLOSE_EVT */          {AVDT_SCB_HDL_TC_CLOSE,         AVDT_SCB_IGNORE,            AVDT_SCB_IDLE_ST},
/* TC_CONG_EVT */           {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* TC_DATA_EVT */           {AVDT_SCB_DROP_PKT,             AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST},
/* CC_CLOSE_EVT */          {AVDT_SCB_IGNORE,               AVDT_SCB_IGNORE,            AVDT_SCB_CLOSING_ST}
};

/* type for state table */
typedef const UINT8 (*tAVDT_SCB_ST_TBL)[AVDT_SCB_NUM_COLS];

/* state table */
const tAVDT_SCB_ST_TBL avdt_scb_st_tbl[] = {
    avdt_scb_st_idle,
    avdt_scb_st_conf,
    avdt_scb_st_opening,
    avdt_scb_st_open,
    avdt_scb_st_stream,
    avdt_scb_st_closing
};


/*******************************************************************************
**
** Function         avdt_scb_event
**
** Description      State machine event handling function for scb
**
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_event(tAVDT_SCB *p_scb, UINT8 event, tAVDT_SCB_EVT *p_data)
{
    tAVDT_SCB_ST_TBL    state_table;
    UINT8               action;
    int                 i;

#if AVDT_DEBUG == TRUE
    AVDT_TRACE_EVENT("SCB hdl=%d event=%d/%s state=%s", avdt_scb_to_hdl(p_scb), event, avdt_scb_evt_str[event], avdt_scb_st_str[p_scb->state]);
#endif
    /* set current event */
    p_scb->curr_evt = event;

    /* look up the state table for the current state */
    state_table = avdt_scb_st_tbl[p_scb->state];

    /* set next state */
    if (p_scb->state != state_table[event][AVDT_SCB_NEXT_STATE]) {
        p_scb->state = state_table[event][AVDT_SCB_NEXT_STATE];
    }

    /* execute action functions */
    for (i = 0; i < AVDT_SCB_ACTIONS; i++)
    {
        if ((action = state_table[event][i]) != AVDT_SCB_IGNORE)
        {
            (*avdt_cb.p_scb_act[action])(p_scb, p_data);
        }
        else
        {
            break;
        }
    }
}


/*******************************************************************************
**
** Function         avdt_scb_init
**
** Description      Initialize stream control block module.
**
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_init(void)
{
    memset(&avdt_cb.scb[0], 0, sizeof(tAVDT_SCB) * AVDT_NUM_SEPS);
    avdt_cb.p_scb_act = (tAVDT_SCB_ACTION *) avdt_scb_action;
}


/*******************************************************************************
**
** Function         avdt_scb_alloc
**
** Description      Allocate a stream control block.
**
**
** Returns          pointer to the scb, or NULL if none could be allocated.
**
*******************************************************************************/
tAVDT_SCB *avdt_scb_alloc(tAVDT_CS *p_cs)
{
    tAVDT_SCB   *p_scb = &avdt_cb.scb[0];
    int         i;

    /* find available scb */
    for (i = 0; i < AVDT_NUM_SEPS; i++, p_scb++)
    {
        if (!p_scb->allocated)
        {
            memset(p_scb,0,sizeof(tAVDT_SCB));
            p_scb->allocated = TRUE;
            p_scb->p_ccb = NULL;

            /* initialize sink as activated */
            if (p_cs->tsep == AVDT_TSEP_SNK)
            {
                p_scb->sink_activated = TRUE;
            }

            memcpy(&p_scb->cs, p_cs, sizeof(tAVDT_CS));
#if AVDT_MULTIPLEXING == TRUE
            /* initialize fragments gueue */
            GKI_init_q(&p_scb->frag_q);

            if(p_cs->cfg.psc_mask & AVDT_PSC_MUX)
            {
                p_scb->cs.cfg.mux_tcid_media = avdt_ad_type_to_tcid(AVDT_CHAN_MEDIA, p_scb);
#if AVDT_REPORTING == TRUE
                if(p_cs->cfg.psc_mask & AVDT_PSC_REPORT)
                {
                    p_scb->cs.cfg.mux_tcid_report = avdt_ad_type_to_tcid(AVDT_CHAN_REPORT, p_scb);
                }
#endif
            }
#endif
            p_scb->timer_entry.param = (UINT32) p_scb;
            AVDT_TRACE_DEBUG("avdt_scb_alloc hdl=%d, psc_mask:0x%x", i+1, p_cs->cfg.psc_mask);
            break;
        }
    }

    if (i == AVDT_NUM_SEPS)
    {
        /* out of ccbs */
        p_scb = NULL;
        AVDT_TRACE_WARNING("Out of scbs");
    }

    return p_scb;
}

/*******************************************************************************
**
** Function         avdt_scb_dealloc
**
** Description      Deallocate a stream control block.
**
**
** Returns          void.
**
*******************************************************************************/
void avdt_scb_dealloc(tAVDT_SCB *p_scb, tAVDT_SCB_EVT *p_data)
{
#if AVDT_MULTIPLEXING == TRUE
    void *p_buf;
#endif
    UNUSED(p_data);

    AVDT_TRACE_DEBUG("avdt_scb_dealloc hdl=%d", avdt_scb_to_hdl(p_scb));
    btu_stop_timer(&p_scb->timer_entry);

#if AVDT_MULTIPLEXING == TRUE
    /* free fragments we're holding, if any; it shouldn't happen */
    while ((p_buf = GKI_dequeue (&p_scb->frag_q)) != NULL)
        GKI_freebuf(p_buf);
#endif

    memset(p_scb, 0, sizeof(tAVDT_SCB));
}

/*******************************************************************************
**
** Function         avdt_scb_to_hdl
**
** Description      Given a pointer to an scb, return its handle (or seid).
**
**
** Returns          Index of scb.
**
*******************************************************************************/
UINT8 avdt_scb_to_hdl(tAVDT_SCB *p_scb)
{
    return (UINT8) (p_scb - avdt_cb.scb + 1);
}

/*******************************************************************************
**
** Function         avdt_scb_by_hdl
**
** Description      Given an scb handle (or seid), return a pointer to the scb.
**
**
** Returns          Pointer to scb or NULL if index is out of range or scb
**                  is not allocated.
**
*******************************************************************************/
tAVDT_SCB *avdt_scb_by_hdl(UINT8 hdl)
{
    tAVDT_SCB   *p_scb;

    /* verify index */
    if ((hdl > 0) && (hdl <= AVDT_NUM_SEPS))
    {
        p_scb = &avdt_cb.scb[hdl - 1];

        /* verify scb is allocated */
        if (!p_scb->allocated)
        {
            p_scb = NULL;
            AVDT_TRACE_WARNING("scb hdl %d not allocated", hdl);
        }
    }
    else
    {
        p_scb = NULL;
        AVDT_TRACE_WARNING("scb hdl %d out of range", hdl);
    }
    return p_scb;
}

/*******************************************************************************
**
** Function         avdt_scb_verify
**
** Description      Verify the condition of a list of scbs.
**
**
** Returns          SEID that failed, or 0 if success.
**
*******************************************************************************/
UINT8 avdt_scb_verify(tAVDT_CCB *p_ccb, UINT8 state, UINT8 *p_seid, UINT16 num_seid, UINT8 *p_err_code)
{
    int         i;
    tAVDT_SCB   *p_scb;
    UINT8       nsc_mask;
    UINT8       ret = 0;

    AVDT_TRACE_DEBUG("avdt_scb_verify state %d", state);
    /* set nonsupported command mask */
    /* translate public state into private state */
    nsc_mask = 0;
    if (state == AVDT_VERIFY_SUSPEND)
      nsc_mask = AVDT_NSC_SUSPEND;

    /* verify every scb */
    for (i = 0, *p_err_code = 0; (i < num_seid) && (*p_err_code == 0) && (i < AVDT_NUM_SEPS); i++)
    {
        if ((p_scb = avdt_scb_by_hdl(p_seid[i])) == NULL)
            *p_err_code = AVDT_ERR_BAD_STATE;
        else if (p_scb->p_ccb != p_ccb)
            *p_err_code = AVDT_ERR_BAD_STATE;
        else if (p_scb->cs.nsc_mask & nsc_mask)
            *p_err_code = AVDT_ERR_NSC;

        switch (state) {
          case AVDT_VERIFY_OPEN:
          case AVDT_VERIFY_START:
            if (p_scb->state != AVDT_SCB_OPEN_ST && p_scb->state != AVDT_SCB_STREAM_ST)
              *p_err_code = AVDT_ERR_BAD_STATE;
            break;

          case AVDT_VERIFY_SUSPEND:
          case AVDT_VERIFY_STREAMING:
            if (p_scb->state != AVDT_SCB_STREAM_ST)
              *p_err_code = AVDT_ERR_BAD_STATE;
            break;
        }
    }

    if ((i != num_seid) && (i < AVDT_NUM_SEPS))
    {
        ret = p_seid[i];
    }

    return ret;
}

/*******************************************************************************
**
** Function         avdt_scb_peer_seid_list
**
** Description      Given a list of SCB handles, return a list of peer SEIDs
**                  for the handles, copied in place into the struct passed in.
**
**
** Returns          Nothing.
**
*******************************************************************************/
void avdt_scb_peer_seid_list(tAVDT_MULTI *p_multi)
{
    int         i;
    tAVDT_SCB   *p_scb;

    for (i = 0; i < p_multi->num_seps; i++)
    {
        if ((p_scb = avdt_scb_by_hdl(p_multi->seid_list[i])) != NULL)
        {
            p_multi->seid_list[i] = p_scb->peer_seid;
        }
    }
}


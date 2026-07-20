/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "common/bt_target.h"

#include "stack/obex_api.h"
#include "stack/goep_common.h"
#include "stack/goeps_api.h"

#if (GOEPS_INCLUDED == TRUE)

/* GOEPS state machine events */
enum {
    GOEPS_SM_EVENT_CONN_INCOME = 0,
    GOEPS_SM_EVENT_DISCONNECT,
    GOEPS_SM_EVENT_REQ,
    GOEPS_SM_EVENT_REQ_FB,
    GOEPS_SM_EVENT_RSP,
    GOEPS_SM_EVENT_RSP_FB,
};

/* GOEPS state machine states */
enum {
    GOEPS_STATE_IDLE = 0,
    GOEPS_STATE_CONN_PENDING,
    GOEPS_STATE_OPENED_IDLE,
    GOEPS_STATE_OPENED_RSP,
};

/* GOEPS srm state machine states */
enum {
    GOEPS_SRM_STATE_IDLE = 0,
    GOEPS_SRM_STATE_REQ,
    GOEPS_SRM_STATE_ENABLE_WAIT,
    GOEPS_SRM_STATE_ENABLE,
    GOEPS_SRM_STATE_DISABLE,
};

/* GOEPS Server Control block */
typedef struct {
    tGOEPS_EVT_CBACK    *callback;
    UINT16              obex_svr_handle;
    UINT8               allocated;
} tGOEPS_SCB;

/* GOEPS Connection Control block */
typedef struct {
    UINT8               scb_idx;
    UINT16              obex_handle;
    UINT16              peer_mtu;
    UINT16              our_mtu;
    BOOLEAN             congest;

    BT_HDR              *pkt;
    BOOLEAN             pkt_srm_en;
    BOOLEAN             pkt_srm_wait;
    UINT8               curr_rsp_opcode;

    UINT8               last_req_opcode;
    BOOLEAN             srm_wait;
    BOOLEAN             srm_peer_wait;
    UINT8               srm_state;
    UINT8               state;
    UINT8               allocated;
} tGOEPS_CCB;

typedef struct {
    tGOEPS_SCB          scb[GOEPS_MAX_SERVER];
    tGOEPS_CCB          ccb[GOEPS_MAX_CONNECTION];
    UINT8               trace_level;
} tGOEPS_CB;

#if GOEP_DYNAMIC_MEMORY == FALSE
extern tGOEPS_CB goeps_cb;
#else
extern tGOEPS_CB *goeps_cb_ptr;
#define goeps_cb (*goeps_cb_ptr)
#endif

typedef struct {
    UINT16              svr_handle;
    UINT16              peer_mtu;
    UINT16              our_mtu;
    BD_ADDR             addr;
} tGOEPS_CONN_INCOME;

typedef struct {
    UINT16              peer_mtu;
    UINT16              our_mtu;
} tGOEPS_MTU_CHG;

typedef union {
    tGOEPS_CONN_INCOME  conn_income;
    tGOEPS_MTU_CHG      mtu_chg;
    BT_HDR              *pkt;
} tGOEPS_DATA;

tGOEPS_SCB *goeps_allocate_scb(void);
void goeps_free_scb(tGOEPS_SCB *p_scb);
tGOEPS_CCB *goeps_allocate_ccb(UINT8 scb_idx);
void goeps_free_ccb(tGOEPS_CCB *p_ccb);
void goeps_obex_callback(UINT16 handle, UINT8 event, tOBEX_MSG *msg);
BOOLEAN goeps_check_obex_rsp_allow(UINT8 state, BOOLEAN final);
BOOLEAN goeps_check_obex_rsp_param(tOBEX_PARSE_INFO *info);
void goeps_sm_execute(tGOEPS_CCB *p_ccb, UINT8 event, tGOEPS_DATA *p_data);
void goeps_srm_sm_execute(tGOEPS_CCB *p_ccb, BOOLEAN is_rsp, BOOLEAN srm_en, BOOLEAN srm_wait);

#endif /* #if (GOEPS_INCLUDED == TRUE) */

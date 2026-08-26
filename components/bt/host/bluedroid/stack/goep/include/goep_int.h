/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "common/bt_target.h"

#include "stack/obex_api.h"
#include "stack/goep_common.h"
#include "stack/goepc_api.h"

#if (GOEPC_INCLUDED == TRUE)

/* GOEPC state machine events */
enum {
    GOEPC_SM_EVENT_CONNECT = 0,
    GOEPC_SM_EVENT_DISCONNECT,
    GOEPC_SM_EVENT_REQ,
    GOEPC_SM_EVENT_REQ_FB,
    GOEPC_SM_EVENT_RSP,
    GOEPC_SM_EVENT_RSP_FB,
};

/* GOEPC state machine states */
enum {
    GOEPC_STATE_INIT = 0,
    GOEPC_STATE_OPENING,
    GOEPC_STATE_OPENED_IDLE,
    GOEPC_STATE_OPENED_REQ,
    GOEPC_STATE_OPENED_RSP,
};

/* GOEPC srm state machine states */
enum {
    GOEPC_SRM_STATE_IDLE = 0,
    GOEPC_SRM_STATE_REQ,
    GOEPC_SRM_STATE_ENABLE_WAIT,
    GOEPC_SRM_STATE_ENABLE,
    GOEPC_SRM_STATE_DISABLE,
};

/* GOEPC Connection Control block */
typedef struct {
    tGOEPC_EVT_CBACK    *callback;          /* GOEP event callback function */
    UINT16              obex_handle;        /* OBEX connection handle */
    UINT16              peer_mtu;           /* lower layer connection peer MTU */
    UINT16              our_mtu;            /* lower layer connection our MTU */
    BOOLEAN             congest;            /* lower layer connection congestion status */

    BT_HDR              *pkt;               /* packet prepared in this GOEP client */
    BOOLEAN             pkt_srm_en;         /* whether prepared packet had set SRM to enable */
    BOOLEAN             pkt_srm_wait;       /* whether prepared packet had set SRMP to wait */
    UINT8               curr_pkt_opcode;    /* prepared packet opcode */

    UINT8               last_pkt_opcode;    /* last sent packet opcode */
    BOOLEAN             srm_wait;           /* whether we had set SRMP to wait */
    BOOLEAN             srm_peer_wait;      /* whether peer had set SRMP to wait */
    UINT8               srm_state;          /* SRM state machine */
    UINT8               state;              /* main state machine */
    UINT8               allocated;          /* 0, not allocated. index+1, otherwise. equal to api handle */
} tGOEPC_CCB;

/* GOEPC Control block */
typedef struct {
    tGOEPC_CCB          ccb[GOEPC_MAX_CONNECTION];      /* connection control blocks */
    UINT8               trace_level;                    /* trace level */
} tGOEPC_CB;

#if GOEP_DYNAMIC_MEMORY == FALSE
extern tGOEPC_CB goepc_cb;
#else
extern tGOEPC_CB *goepc_cb_ptr;
#define goepc_cb (*goepc_cb_ptr)
#endif

typedef struct {
    UINT16              peer_mtu;
    UINT16              our_mtu;
} tGOEPC_CONNECTED;

typedef struct {
    UINT16              peer_mtu;
    UINT16              our_mtu;
} tGOEPC_MTU_CHG;

typedef union {
    tGOEPC_CONNECTED    connected;
    tGOEPC_MTU_CHG      mtu_chg;
    BT_HDR              *pkt;
} tGOEPC_DATA;

tGOEPC_CCB *goepc_allocate_ccb(void);
void goepc_free_ccb(tGOEPC_CCB *p_ccb);
void goepc_obex_callback(UINT16 handle, UINT8 event, tOBEX_MSG *msg);
BOOLEAN goepc_check_obex_req_allow(UINT8 state, BOOLEAN final);
BOOLEAN goepc_check_obex_req_param(tOBEX_PARSE_INFO *info);
void goepc_sm_execute(tGOEPC_CCB *p_ccb, UINT8 event, tGOEPC_DATA *p_data);
void goepc_srm_sm_execute(tGOEPC_CCB *p_ccb, BOOLEAN is_req, BOOLEAN srm_en, BOOLEAN srm_wait);

#endif /* #if (GOEPC_INCLUDED == TRUE) */

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
 *  This file contains interfaces which are internal to AVCTP.
 *
 ******************************************************************************/
#ifndef AVCT_INT_H
#define AVCT_INT_H

#include "avct_api.h"
#include "avct_defs.h"
#include "l2c_api.h"
#include "fixed_queue.h"

/*****************************************************************************
** constants
*****************************************************************************/

/* lcb state machine events */
enum {
    AVCT_LCB_UL_BIND_EVT,
    AVCT_LCB_UL_UNBIND_EVT,
    AVCT_LCB_UL_MSG_EVT,
    AVCT_LCB_INT_CLOSE_EVT,
    AVCT_LCB_LL_OPEN_EVT,
    AVCT_LCB_LL_CLOSE_EVT,
    AVCT_LCB_LL_MSG_EVT,
    AVCT_LCB_LL_CONG_EVT
};


/* "states" used for L2CAP channel */
#define AVCT_CH_IDLE    0       /* No connection */
#define AVCT_CH_CONN    1       /* Waiting for connection confirm */
#define AVCT_CH_CFG     2       /* Waiting for configuration complete */
#define AVCT_CH_OPEN    3       /* Channel opened */

/* "no event" indicator used by ccb dealloc */
#define AVCT_NO_EVT     0xFF

/*****************************************************************************
** data types
*****************************************************************************/
/* sub control block type - common data members for tAVCT_LCB and tAVCT_BCB */
typedef struct {
    UINT16              peer_mtu;       /* peer l2c mtu */
    UINT16              ch_result;      /* L2CAP connection result value */
    UINT16              ch_lcid;        /* L2CAP channel LCID */
    UINT8               allocated;      /* 0, not allocated. index+1, otherwise. */
    UINT8               state;          /* The state machine state */
    UINT8               ch_state;       /* L2CAP channel state */
    UINT8               ch_flags;       /* L2CAP configuration flags */
} tAVCT_SCB;

/* link control block type */
typedef struct {
    UINT16              peer_mtu;       /* peer l2c mtu */
    UINT16              ch_result;      /* L2CAP connection result value */
    UINT16              ch_lcid;        /* L2CAP channel LCID */
    UINT8               allocated;      /* 0, not allocated. index+1, otherwise. */
    UINT8               state;          /* The state machine state */
    UINT8               ch_state;       /* L2CAP channel state */
    UINT8               ch_flags;       /* L2CAP configuration flags */
    BT_HDR              *p_rx_msg;      /* Message being reassembled */
    UINT16              conflict_lcid;  /* L2CAP channel LCID */
    BD_ADDR             peer_addr;      /* BD address of peer */
    fixed_queue_t       *tx_q;           /* Transmit data buffer queue       */
    BOOLEAN             cong;           /* TRUE, if congested */
} tAVCT_LCB;

/* browse control block type */
typedef struct {
    UINT16              peer_mtu;       /* peer l2c mtu */
    UINT16              ch_result;      /* L2CAP connection result value */
    UINT16              ch_lcid;        /* L2CAP channel LCID */
    UINT8               allocated;      /* 0, not allocated. index+1, otherwise. */
    UINT8               state;          /* The state machine state */
    UINT8               ch_state;       /* L2CAP channel state */
    UINT8               ch_flags;       /* L2CAP configuration flags */
    BT_HDR              *p_tx_msg;      /* Message to be sent - in case the browsing channel is not open when MsgReg is called */
    UINT8               ch_close;       /* CCB index+1, if CCB initiated channel close */
} tAVCT_BCB;

#define AVCT_ALOC_LCB       0x01
#define AVCT_ALOC_BCB       0x02
/* connection control block */
typedef struct {
    tAVCT_CC            cc;                 /* parameters from connection creation */
    tAVCT_LCB           *p_lcb;             /* Associated LCB */
    tAVCT_BCB           *p_bcb;             /* associated BCB */
    BOOLEAN             ch_close;           /* Whether CCB initiated channel close */
    UINT8               allocated;          /* Whether LCB/BCB is allocated */
} tAVCT_CCB;

/* data type associated with UL_MSG_EVT */
typedef struct {
    BT_HDR                  *p_buf;
    tAVCT_CCB               *p_ccb;
    UINT8                   label;
    UINT8                   cr;
} tAVCT_UL_MSG;

/* union associated with lcb state machine events */
typedef union {
    tAVCT_UL_MSG            ul_msg;
    BT_HDR                  *p_buf;
    tAVCT_CCB               *p_ccb;
    UINT16                  result;
    BOOLEAN                 cong;
    UINT8                   err_code;
} tAVCT_LCB_EVT;

/* Control block for AVCT */
typedef struct {
    tAVCT_LCB       lcb[AVCT_NUM_LINKS];    /* link control blocks */
    tAVCT_BCB       bcb[AVCT_NUM_LINKS];    /* browse control blocks */
    tAVCT_CCB       ccb[AVCT_NUM_CONN];     /* connection control blocks */
    UINT16          mtu;                    /* our L2CAP MTU */
    UINT16          mtu_br;                 /* our L2CAP MTU for the Browsing channel */
    UINT8           trace_level;            /* trace level */
} tAVCT_CB;

/*****************************************************************************
** function declarations
*****************************************************************************/

/* LCB function declarations */
extern void avct_lcb_event(tAVCT_LCB *p_lcb, UINT8 event, tAVCT_LCB_EVT *p_data);
#if (AVCT_BROWSE_INCLUDED == TRUE)
extern void avct_bcb_event(tAVCT_BCB *p_bcb, UINT8 event, tAVCT_LCB_EVT *p_data);
extern void avct_close_bcb(tAVCT_LCB *p_lcb, tAVCT_LCB_EVT *p_data);
extern tAVCT_LCB *avct_lcb_by_bcb(tAVCT_BCB *p_bcb);
extern tAVCT_BCB *avct_bcb_by_lcb(tAVCT_LCB *p_lcb);
extern BOOLEAN avct_bcb_last_ccb(tAVCT_BCB *p_bcb, tAVCT_CCB *p_ccb_last);
extern tAVCT_BCB *avct_bcb_by_lcid(UINT16 lcid);
#endif
extern tAVCT_LCB *avct_lcb_by_bd(BD_ADDR bd_addr);
extern tAVCT_LCB *avct_lcb_alloc(BD_ADDR bd_addr);
extern void avct_lcb_dealloc(tAVCT_LCB *p_lcb, tAVCT_LCB_EVT *p_data);
extern tAVCT_LCB *avct_lcb_by_lcid(UINT16 lcid);
extern tAVCT_CCB *avct_lcb_has_pid(tAVCT_LCB *p_lcb, UINT16 pid);
extern BOOLEAN avct_lcb_last_ccb(tAVCT_LCB *p_lcb, tAVCT_CCB *p_ccb_last);

/* LCB action functions */
extern void avct_lcb_chnl_open(tAVCT_LCB *p_lcb, tAVCT_LCB_EVT *p_data);
extern void avct_lcb_unbind_disc(tAVCT_LCB *p_lcb, tAVCT_LCB_EVT *p_data);
extern void avct_lcb_open_ind(tAVCT_LCB *p_lcb, tAVCT_LCB_EVT *p_data);
extern void avct_lcb_open_fail(tAVCT_LCB *p_lcb, tAVCT_LCB_EVT *p_data);
extern void avct_lcb_close_ind(tAVCT_LCB *p_lcb, tAVCT_LCB_EVT *p_data);
extern void avct_lcb_close_cfm(tAVCT_LCB *p_lcb, tAVCT_LCB_EVT *p_data);
extern void avct_lcb_bind_conn(tAVCT_LCB *p_lcb, tAVCT_LCB_EVT *p_data);
extern void avct_lcb_chk_disc(tAVCT_LCB *p_lcb, tAVCT_LCB_EVT *p_data);
extern void avct_lcb_chnl_disc(tAVCT_LCB *p_lcb, tAVCT_LCB_EVT *p_data);
extern void avct_lcb_bind_fail(tAVCT_LCB *p_lcb, tAVCT_LCB_EVT *p_data);
extern void avct_lcb_cong_ind(tAVCT_LCB *p_lcb, tAVCT_LCB_EVT *p_data);
extern void avct_lcb_discard_msg(tAVCT_LCB *p_lcb, tAVCT_LCB_EVT *p_data);
extern void avct_lcb_send_msg(tAVCT_LCB *p_lcb, tAVCT_LCB_EVT *p_data);
extern void avct_lcb_msg_ind(tAVCT_LCB *p_lcb, tAVCT_LCB_EVT *p_data);
extern void avct_lcb_free_msg_ind(tAVCT_LCB *p_lcb, tAVCT_LCB_EVT *p_data);

/* BCB action functions */
#if (AVCT_BROWSE_INCLUDED == TRUE)
typedef void (*tAVCT_BCB_ACTION)(tAVCT_BCB *p_bcb, tAVCT_LCB_EVT *p_data);
extern void avct_bcb_chnl_open(tAVCT_BCB *p_bcb, tAVCT_LCB_EVT *p_data);
extern void avct_bcb_unbind_disc(tAVCT_BCB *p_bcb, tAVCT_LCB_EVT *p_data);
extern void avct_bcb_open_ind(tAVCT_BCB *p_bcb, tAVCT_LCB_EVT *p_data);
extern void avct_bcb_open_fail(tAVCT_BCB *p_bcb, tAVCT_LCB_EVT *p_data);
extern void avct_bcb_close_ind(tAVCT_BCB *p_bcb, tAVCT_LCB_EVT *p_data);
extern void avct_bcb_close_cfm(tAVCT_BCB *p_bcb, tAVCT_LCB_EVT *p_data);
extern void avct_bcb_bind_conn(tAVCT_BCB *p_bcb, tAVCT_LCB_EVT *p_data);
extern void avct_bcb_chk_disc(tAVCT_BCB *p_bcb, tAVCT_LCB_EVT *p_data);
extern void avct_bcb_chnl_disc(tAVCT_BCB *p_bcb, tAVCT_LCB_EVT *p_data);
extern void avct_bcb_bind_fail(tAVCT_BCB *p_bcb, tAVCT_LCB_EVT *p_data);
extern void avct_bcb_cong_ind(tAVCT_BCB *p_bcb, tAVCT_LCB_EVT *p_data);
extern void avct_bcb_discard_msg(tAVCT_BCB *p_bcb, tAVCT_LCB_EVT *p_data);
extern void avct_bcb_send_msg(tAVCT_BCB *p_bcb, tAVCT_LCB_EVT *p_data);
extern void avct_bcb_msg_ind(tAVCT_BCB *p_bcb, tAVCT_LCB_EVT *p_data);
extern void avct_bcb_free_msg_ind(tAVCT_BCB *p_bcb, tAVCT_LCB_EVT *p_data);

extern void avct_bcb_dealloc(tAVCT_BCB *p_bcb, tAVCT_LCB_EVT *p_data);

extern const tAVCT_BCB_ACTION avct_bcb_action[];
extern const UINT8 avct_lcb_pkt_type_len[];
extern const tL2CAP_FCR_OPTS avct_l2c_br_fcr_opts_def;
#endif

/* CCB function declarations */
extern tAVCT_CCB *avct_ccb_alloc(tAVCT_CC *p_cc);
extern void avct_ccb_dealloc(tAVCT_CCB *p_ccb, UINT8 event, UINT16 result, BD_ADDR bd_addr);
extern UINT8 avct_ccb_to_idx(tAVCT_CCB *p_ccb);
extern tAVCT_CCB *avct_ccb_by_idx(UINT8 idx);


/*****************************************************************************
** global data
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/* Main control block */
#if AVCT_DYNAMIC_MEMORY == FALSE
extern tAVCT_CB avct_cb;
#else
extern tAVCT_CB *avct_cb_ptr;
#define avct_cb (*avct_cb_ptr)
#endif

/* L2CAP callback registration structure */
extern const tL2CAP_APPL_INFO avct_l2c_appl;
#if (AVCT_BROWSE_INCLUDED == TRUE)
extern const tL2CAP_APPL_INFO avct_l2c_br_appl;
#endif

#ifdef __cplusplus
}
#endif

#endif /* AVCT_INT_H */

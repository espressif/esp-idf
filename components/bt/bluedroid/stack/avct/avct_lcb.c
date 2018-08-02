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
 *  This module contains the link control state machine and functions which
 *  operate on the link control block.
 *
 ******************************************************************************/

#include <string.h>
#include "stack/bt_types.h"
#include "common/bt_target.h"
#include "common/bt_defs.h"
#include "stack/avct_api.h"
#include "avct_int.h"
#include "osi/allocator.h"

#if (defined(AVCT_INCLUDED) && AVCT_INCLUDED == TRUE)

/*****************************************************************************
** state machine constants and types
*****************************************************************************/

#if BT_TRACE_VERBOSE == TRUE

/* verbose state strings for trace */
const char *const avct_lcb_st_str[] = {
    "LCB_IDLE_ST",
    "LCB_OPENING_ST",
    "LCB_OPEN_ST",
    "LCB_CLOSING_ST"
};

/* verbose event strings for trace */
const char *const avct_lcb_evt_str[] = {
    "UL_BIND_EVT",
    "UL_UNBIND_EVT",
    "UL_MSG_EVT",
    "INT_CLOSE_EVT",
    "LL_OPEN_EVT",
    "LL_CLOSE_EVT",
    "LL_MSG_EVT",
    "LL_CONG_EVT"
};

#endif

/* lcb state machine states */
enum {
    AVCT_LCB_IDLE_ST,
    AVCT_LCB_OPENING_ST,
    AVCT_LCB_OPEN_ST,
    AVCT_LCB_CLOSING_ST
};

/* state machine action enumeration list */
enum {
    AVCT_LCB_CHNL_OPEN,
    AVCT_LCB_CHNL_DISC,
    AVCT_LCB_SEND_MSG,
    AVCT_LCB_OPEN_IND,
    AVCT_LCB_OPEN_FAIL,
    AVCT_LCB_CLOSE_IND,
    AVCT_LCB_CLOSE_CFM,
    AVCT_LCB_MSG_IND,
    AVCT_LCB_CONG_IND,
    AVCT_LCB_BIND_CONN,
    AVCT_LCB_BIND_FAIL,
    AVCT_LCB_UNBIND_DISC,
    AVCT_LCB_CHK_DISC,
    AVCT_LCB_DISCARD_MSG,
    AVCT_LCB_DEALLOC,
    AVCT_LCB_FREE_MSG_IND,
    AVCT_LCB_NUM_ACTIONS
};

#define AVCT_LCB_IGNORE     AVCT_LCB_NUM_ACTIONS

/* type for action functions */
typedef void (*tAVCT_LCB_ACTION)(tAVCT_LCB *p_ccb, tAVCT_LCB_EVT *p_data);

/* action function list */
const tAVCT_LCB_ACTION avct_lcb_action[] = {
    avct_lcb_chnl_open,
    avct_lcb_chnl_disc,
    avct_lcb_send_msg,
    avct_lcb_open_ind,
    avct_lcb_open_fail,
    avct_lcb_close_ind,
    avct_lcb_close_cfm,
    avct_lcb_msg_ind,
    avct_lcb_cong_ind,
    avct_lcb_bind_conn,
    avct_lcb_bind_fail,
    avct_lcb_unbind_disc,
    avct_lcb_chk_disc,
    avct_lcb_discard_msg,
    avct_lcb_dealloc,
    avct_lcb_free_msg_ind
};

/* state table information */
#define AVCT_LCB_ACTIONS            2       /* number of actions */
#define AVCT_LCB_NEXT_STATE         2       /* position of next state */
#define AVCT_LCB_NUM_COLS           3       /* number of columns in state tables */

/* state table for idle state */
const UINT8 avct_lcb_st_idle[][AVCT_LCB_NUM_COLS] = {
    /* Event                Action 1                    Action 2                    Next state */
    /* UL_BIND_EVT */       {AVCT_LCB_CHNL_OPEN,        AVCT_LCB_IGNORE,            AVCT_LCB_OPENING_ST},
    /* UL_UNBIND_EVT */     {AVCT_LCB_UNBIND_DISC,      AVCT_LCB_IGNORE,            AVCT_LCB_IDLE_ST},
    /* UL_MSG_EVT */        {AVCT_LCB_DISCARD_MSG,      AVCT_LCB_IGNORE,            AVCT_LCB_IDLE_ST},
    /* INT_CLOSE_EVT */     {AVCT_LCB_IGNORE,           AVCT_LCB_IGNORE,            AVCT_LCB_IDLE_ST},
    /* LL_OPEN_EVT */       {AVCT_LCB_OPEN_IND,         AVCT_LCB_IGNORE,            AVCT_LCB_OPEN_ST},
    /* LL_CLOSE_EVT */      {AVCT_LCB_CLOSE_IND,        AVCT_LCB_DEALLOC,           AVCT_LCB_IDLE_ST},
    /* LL_MSG_EVT */        {AVCT_LCB_FREE_MSG_IND,     AVCT_LCB_IGNORE,            AVCT_LCB_IDLE_ST},
    /* LL_CONG_EVT */       {AVCT_LCB_IGNORE,           AVCT_LCB_IGNORE,            AVCT_LCB_IDLE_ST}
};

/* state table for opening state */
const UINT8 avct_lcb_st_opening[][AVCT_LCB_NUM_COLS] = {
    /* Event                Action 1                    Action 2                    Next state */
    /* UL_BIND_EVT */       {AVCT_LCB_IGNORE,           AVCT_LCB_IGNORE,            AVCT_LCB_OPENING_ST},
    /* UL_UNBIND_EVT */     {AVCT_LCB_UNBIND_DISC,      AVCT_LCB_IGNORE,            AVCT_LCB_OPENING_ST},
    /* UL_MSG_EVT */        {AVCT_LCB_DISCARD_MSG,      AVCT_LCB_IGNORE,            AVCT_LCB_OPENING_ST},
    /* INT_CLOSE_EVT */     {AVCT_LCB_CHNL_DISC,        AVCT_LCB_IGNORE,            AVCT_LCB_CLOSING_ST},
    /* LL_OPEN_EVT */       {AVCT_LCB_OPEN_IND,         AVCT_LCB_IGNORE,            AVCT_LCB_OPEN_ST},
    /* LL_CLOSE_EVT */      {AVCT_LCB_OPEN_FAIL,        AVCT_LCB_DEALLOC,           AVCT_LCB_IDLE_ST},
    /* LL_MSG_EVT */        {AVCT_LCB_FREE_MSG_IND,     AVCT_LCB_IGNORE,            AVCT_LCB_OPENING_ST},
    /* LL_CONG_EVT */       {AVCT_LCB_CONG_IND,         AVCT_LCB_IGNORE,            AVCT_LCB_OPENING_ST}
};

/* state table for open state */
const UINT8 avct_lcb_st_open[][AVCT_LCB_NUM_COLS] = {
    /* Event                Action 1                    Action 2                    Next state */
    /* UL_BIND_EVT */       {AVCT_LCB_BIND_CONN,        AVCT_LCB_IGNORE,            AVCT_LCB_OPEN_ST},
    /* UL_UNBIND_EVT */     {AVCT_LCB_CHK_DISC,         AVCT_LCB_IGNORE,            AVCT_LCB_OPEN_ST},
    /* UL_MSG_EVT */        {AVCT_LCB_SEND_MSG,         AVCT_LCB_IGNORE,            AVCT_LCB_OPEN_ST},
    /* INT_CLOSE_EVT */     {AVCT_LCB_CHNL_DISC,        AVCT_LCB_IGNORE,            AVCT_LCB_CLOSING_ST},
    /* LL_OPEN_EVT */       {AVCT_LCB_IGNORE,           AVCT_LCB_IGNORE,            AVCT_LCB_OPEN_ST},
    /* LL_CLOSE_EVT */      {AVCT_LCB_CLOSE_IND,        AVCT_LCB_DEALLOC,           AVCT_LCB_IDLE_ST},
    /* LL_MSG_EVT */        {AVCT_LCB_MSG_IND,          AVCT_LCB_IGNORE,            AVCT_LCB_OPEN_ST},
    /* LL_CONG_EVT */       {AVCT_LCB_CONG_IND,         AVCT_LCB_IGNORE,            AVCT_LCB_OPEN_ST}
};

/* state table for closing state */
const UINT8 avct_lcb_st_closing[][AVCT_LCB_NUM_COLS] = {
    /* Event                Action 1                    Action 2                    Next state */
    /* UL_BIND_EVT */       {AVCT_LCB_BIND_FAIL,        AVCT_LCB_IGNORE,            AVCT_LCB_CLOSING_ST},
    /* UL_UNBIND_EVT */     {AVCT_LCB_IGNORE,           AVCT_LCB_IGNORE,            AVCT_LCB_CLOSING_ST},
    /* UL_MSG_EVT */        {AVCT_LCB_DISCARD_MSG,      AVCT_LCB_IGNORE,            AVCT_LCB_CLOSING_ST},
    /* INT_CLOSE_EVT */     {AVCT_LCB_IGNORE,           AVCT_LCB_IGNORE,            AVCT_LCB_CLOSING_ST},
    /* LL_OPEN_EVT */       {AVCT_LCB_IGNORE,           AVCT_LCB_IGNORE,            AVCT_LCB_CLOSING_ST},
    /* LL_CLOSE_EVT */      {AVCT_LCB_CLOSE_CFM,        AVCT_LCB_DEALLOC,           AVCT_LCB_IDLE_ST},
    /* LL_MSG_EVT */        {AVCT_LCB_FREE_MSG_IND,     AVCT_LCB_IGNORE,            AVCT_LCB_CLOSING_ST},
    /* LL_CONG_EVT */       {AVCT_LCB_IGNORE,           AVCT_LCB_IGNORE,            AVCT_LCB_CLOSING_ST}
};

/* type for state table */
typedef const UINT8 (*tAVCT_LCB_ST_TBL)[AVCT_LCB_NUM_COLS];

/* state table */
const tAVCT_LCB_ST_TBL avct_lcb_st_tbl[] = {
    avct_lcb_st_idle,
    avct_lcb_st_opening,
    avct_lcb_st_open,
    avct_lcb_st_closing
};

/*******************************************************************************
**
** Function         avct_lcb_event
**
** Description      State machine event handling function for lcb
**
**
** Returns          Nothing.
**
*******************************************************************************/
void avct_lcb_event(tAVCT_LCB *p_lcb, UINT8 event, tAVCT_LCB_EVT *p_data)
{
    tAVCT_LCB_ST_TBL    state_table;
    UINT8               action;
    int                 i;

#if BT_TRACE_VERBOSE == TRUE
    AVCT_TRACE_EVENT("LCB lcb=%d event=%s state=%s", p_lcb->allocated, avct_lcb_evt_str[event], avct_lcb_st_str[p_lcb->state]);
#else
    AVCT_TRACE_EVENT("LCB lcb=%d event=%d state=%d", p_lcb->allocated, event, p_lcb->state);
#endif

    /* look up the state table for the current state */
    state_table = avct_lcb_st_tbl[p_lcb->state];

    /* set next state */
    p_lcb->state = state_table[event][AVCT_LCB_NEXT_STATE];

    /* execute action functions */
    for (i = 0; i < AVCT_LCB_ACTIONS; i++) {
        if ((action = state_table[event][i]) != AVCT_LCB_IGNORE) {
            (*avct_lcb_action[action])(p_lcb, p_data);
        } else {
            break;
        }
    }
}

/*******************************************************************************
**
** Function         avct_bcb_event
**
** Description      State machine event handling function for lcb
**
**
** Returns          Nothing.
**
*******************************************************************************/
#if (AVCT_BROWSE_INCLUDED == TRUE)
void avct_bcb_event(tAVCT_BCB *p_bcb, UINT8 event, tAVCT_LCB_EVT *p_data)
{
    tAVCT_LCB_ST_TBL    state_table;
    UINT8               action;
    int                 i;

#if BT_TRACE_VERBOSE == TRUE
    AVCT_TRACE_EVENT("BCB lcb=%d event=%s state=%s", p_bcb->allocated, avct_lcb_evt_str[event], avct_lcb_st_str[p_bcb->state]);
#else
    AVCT_TRACE_EVENT("BCB lcb=%d event=%d state=%d", p_bcb->allocated, event, p_bcb->state);
#endif

    /* look up the state table for the current state */
    state_table = avct_lcb_st_tbl[p_bcb->state];

    /* set next state */
    p_bcb->state = state_table[event][AVCT_LCB_NEXT_STATE];

    /* execute action functions */
    for (i = 0; i < AVCT_LCB_ACTIONS; i++) {
        if ((action = state_table[event][i]) != AVCT_LCB_IGNORE) {
            (*avct_bcb_action[action])(p_bcb, p_data);
        } else {
            break;
        }
    }
}
#endif

/*******************************************************************************
**
** Function         avct_lcb_by_bd
**
** Description      This lookup function finds the lcb for a BD address.
**
**
** Returns          pointer to the lcb, or NULL if none found.
**
*******************************************************************************/
tAVCT_LCB *avct_lcb_by_bd(BD_ADDR bd_addr)
{
    tAVCT_LCB   *p_lcb = &avct_cb.lcb[0];
    int         i;

    for (i = 0; i < AVCT_NUM_LINKS; i++, p_lcb++) {
        /* if allocated lcb has matching lcb */
        if (p_lcb->allocated && (!memcmp(p_lcb->peer_addr, bd_addr, BD_ADDR_LEN))) {
            break;
        }
    }

    if (i == AVCT_NUM_LINKS) {
        /* if no lcb found */
        p_lcb = NULL;

        AVCT_TRACE_DEBUG("No lcb for addr %02x-%02x-%02x-%02x-%02x-%02x",
                         bd_addr[0], bd_addr[1], bd_addr[2], bd_addr[3], bd_addr[4], bd_addr[5]);
    }
    return p_lcb;
}

/*******************************************************************************
**
** Function         avct_lcb_alloc
**
** Description      Allocate a link control block.
**
**
** Returns          pointer to the lcb, or NULL if none could be allocated.
**
*******************************************************************************/
tAVCT_LCB *avct_lcb_alloc(BD_ADDR bd_addr)
{
    tAVCT_LCB   *p_lcb = &avct_cb.lcb[0];
    int         i;

    for (i = 0; i < AVCT_NUM_LINKS; i++, p_lcb++) {
        if (!p_lcb->allocated) {
            p_lcb->allocated = (UINT8)(i + 1);
            memcpy(p_lcb->peer_addr, bd_addr, BD_ADDR_LEN);
            AVCT_TRACE_DEBUG("avct_lcb_alloc %d", p_lcb->allocated);
            p_lcb->tx_q = fixed_queue_new(QUEUE_SIZE_MAX);
            break;
        }
    }

    if (i == AVCT_NUM_LINKS) {
        /* out of lcbs */
        p_lcb = NULL;
        AVCT_TRACE_WARNING("Out of lcbs");
    }
    return p_lcb;
}

/*******************************************************************************
**
** Function         avct_lcb_dealloc
**
** Description      Deallocate a link control block.
**
**
** Returns          void.
**
*******************************************************************************/
void avct_lcb_dealloc(tAVCT_LCB *p_lcb, tAVCT_LCB_EVT *p_data)
{
    UNUSED(p_data);

    AVCT_TRACE_DEBUG("%s allocated: %d", __func__, p_lcb->allocated);

    // Check if the LCB is still referenced

    tAVCT_CCB *p_ccb = &avct_cb.ccb[0];
    for (size_t i = 0; i < AVCT_NUM_CONN; i++, p_ccb++)
    {
        if (p_ccb->allocated && p_ccb->p_lcb == p_lcb)
        {
            AVCT_TRACE_DEBUG("%s LCB in use; lcb index: %d", __func__, i);
            return;
        }
    }

    // If not, de-allocate now...

    AVCT_TRACE_DEBUG("%s Freeing LCB", __func__);
    osi_free(p_lcb->p_rx_msg);
    fixed_queue_free(p_lcb->tx_q, NULL);
    memset(p_lcb, 0, sizeof(tAVCT_LCB));
}

/*******************************************************************************
**
** Function         avct_lcb_by_lcid
**
** Description      Find the LCB associated with the L2CAP LCID
**
**
** Returns          pointer to the lcb, or NULL if none found.
**
*******************************************************************************/
tAVCT_LCB *avct_lcb_by_lcid(UINT16 lcid)
{
    tAVCT_LCB   *p_lcb = &avct_cb.lcb[0];
    int         i;

    for (i = 0; i < AVCT_NUM_LINKS; i++, p_lcb++) {
        if (p_lcb->allocated && ((p_lcb->ch_lcid == lcid) || (p_lcb->conflict_lcid == lcid))) {
            break;
        }
    }

    if (i == AVCT_NUM_LINKS) {
        /* out of lcbs */
        p_lcb = NULL;
        AVCT_TRACE_WARNING("No lcb for lcid %x", lcid);
    }

    return p_lcb;
}

/*******************************************************************************
**
** Function         avct_lcb_has_pid
**
** Description      See if any ccbs on this lcb have a particular pid.
**
**
** Returns          Pointer to CCB if PID found, NULL otherwise.
**
*******************************************************************************/
tAVCT_CCB *avct_lcb_has_pid(tAVCT_LCB *p_lcb, UINT16 pid)
{
    tAVCT_CCB   *p_ccb = &avct_cb.ccb[0];
    int         i;

    for (i = 0; i < AVCT_NUM_CONN; i++, p_ccb++) {
        if (p_ccb->allocated && (p_ccb->p_lcb == p_lcb) && (p_ccb->cc.pid == pid)) {
            return p_ccb;
        }
    }
    return NULL;
}

/*******************************************************************************
**
** Function         avct_lcb_last_ccb
**
** Description      See if given ccb is only one on the lcb.
**
**
** Returns          TRUE if ccb is last, FALSE otherwise.
**
*******************************************************************************/
BOOLEAN avct_lcb_last_ccb(tAVCT_LCB *p_lcb, tAVCT_CCB *p_ccb_last)
{
    tAVCT_CCB   *p_ccb = &avct_cb.ccb[0];
    int         i;

    AVCT_TRACE_WARNING("avct_lcb_last_ccb");
    for (i = 0; i < AVCT_NUM_CONN; i++, p_ccb++) {
        AVCT_TRACE_WARNING("%x: aloc:%d, lcb:%p/%p, ccb:%p/%p",
                           i, p_ccb->allocated, p_ccb->p_lcb, p_lcb, p_ccb, p_ccb_last);
        if (p_ccb->allocated && (p_ccb->p_lcb == p_lcb) && (p_ccb != p_ccb_last)) {
            return FALSE;
        }
    }
    return TRUE;
}


#endif /* #if (defined(AVCT_INCLUDED) && AVCT_INCLUDED == TRUE) */

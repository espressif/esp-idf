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
 *  This file contains the PAN main functions and state machine.
 *
 ******************************************************************************/

#include "common/bt_target.h"

#if (BTA_PAN_INCLUDED == TRUE)

#include <string.h>

#include "bt_common.h"
#include "bta/bta_api.h"
#include "bta/bta_pan_api.h"
#include "bta_pan_int.h"
#include "bta/bta_sys.h"
#include "osi/osi.h"
#include "osi/allocator.h"
#include "stack/pan_api.h"
#include "bta/utl.h"

/*****************************************************************************
 * Constants and types
 ****************************************************************************/

/* state machine action enumeration list */
enum {
    BTA_PAN_API_CLOSE,
    BTA_PAN_TX_PATH,
    BTA_PAN_RX_PATH,
    BTA_PAN_TX_FLOW,
    BTA_PAN_WRITE_BUF,
    BTA_PAN_CONN_OPEN,
    BTA_PAN_CONN_CLOSE,
    BTA_PAN_FREE_BUF,
    BTA_PAN_IGNORE
};

/* type for action functions */
typedef void (*tBTA_PAN_ACTION)(tBTA_PAN_SCB *p_scb, tBTA_PAN_DATA *p_data);

/* action function list */
const tBTA_PAN_ACTION bta_pan_action[] = {
    bta_pan_api_close,
    bta_pan_tx_path,
    bta_pan_rx_path,
    bta_pan_tx_flow,
    bta_pan_write_buf,
    bta_pan_conn_open,
    bta_pan_conn_close,
    bta_pan_free_buf,
};

/* state table information */
#define BTA_PAN_ACTIONS 1    /* number of actions */
#define BTA_PAN_NEXT_STATE 1 /* position of next state */
#define BTA_PAN_NUM_COLS 2   /* number of columns in state tables */

/* state table for listen state */
const UINT8 bta_pan_st_idle[][BTA_PAN_NUM_COLS] = {
    /* API_CLOSE */ {BTA_PAN_API_CLOSE, BTA_PAN_IDLE_ST},
    /* CI_TX_READY */ {BTA_PAN_IGNORE, BTA_PAN_IDLE_ST},
    /* CI_RX_READY */ {BTA_PAN_IGNORE, BTA_PAN_IDLE_ST},
    /* CI_TX_FLOW */ {BTA_PAN_IGNORE, BTA_PAN_IDLE_ST},
    /* CI_RX_WRITE */ {BTA_PAN_IGNORE, BTA_PAN_IDLE_ST},
    /* CI_RX_WRITEBUF */ {BTA_PAN_FREE_BUF, BTA_PAN_IDLE_ST},
    /* PAN_CONN_OPEN */ {BTA_PAN_CONN_OPEN, BTA_PAN_OPEN_ST},
    /* PAN_CONN_CLOSE */ {BTA_PAN_CONN_CLOSE, BTA_PAN_IDLE_ST},
    /* FLOW_ENABLE */ {BTA_PAN_IGNORE, BTA_PAN_IDLE_ST},
    /* BNEP_DATA */ {BTA_PAN_IGNORE, BTA_PAN_IDLE_ST}
};

/* state table for open state */
const UINT8 bta_pan_st_open[][BTA_PAN_NUM_COLS] = {
    /* API_CLOSE */ {BTA_PAN_API_CLOSE, BTA_PAN_CLOSING_ST},
    /* CI_TX_READY */ {BTA_PAN_TX_PATH, BTA_PAN_OPEN_ST},
    /* CI_RX_READY */ {BTA_PAN_RX_PATH, BTA_PAN_OPEN_ST},
    /* CI_TX_FLOW */ {BTA_PAN_TX_FLOW, BTA_PAN_OPEN_ST},
    /* CI_RX_WRITE */ {BTA_PAN_IGNORE, BTA_PAN_OPEN_ST},
    /* CI_RX_WRITEBUF */ {BTA_PAN_WRITE_BUF, BTA_PAN_OPEN_ST},
    /* PAN_CONN_OPEN */ {BTA_PAN_IGNORE, BTA_PAN_OPEN_ST},
    /* PAN_CONN_CLOSE */ {BTA_PAN_CONN_CLOSE, BTA_PAN_IDLE_ST},
    /* FLOW_ENABLE */ {BTA_PAN_RX_PATH, BTA_PAN_OPEN_ST},
    /* BNEP_DATA */ {BTA_PAN_TX_PATH, BTA_PAN_OPEN_ST}
};

/* state table for closing state */
const UINT8 bta_pan_st_closing[][BTA_PAN_NUM_COLS] = {
    /* API_CLOSE */ {BTA_PAN_IGNORE, BTA_PAN_CLOSING_ST},
    /* CI_TX_READY */ {BTA_PAN_TX_PATH, BTA_PAN_CLOSING_ST},
    /* CI_RX_READY */ {BTA_PAN_RX_PATH, BTA_PAN_CLOSING_ST},
    /* CI_TX_FLOW */ {BTA_PAN_TX_FLOW, BTA_PAN_CLOSING_ST},
    /* CI_RX_WRITE */ {BTA_PAN_IGNORE, BTA_PAN_CLOSING_ST},
    /* CI_RX_WRITEBUF */ {BTA_PAN_FREE_BUF, BTA_PAN_CLOSING_ST},
    /* PAN_CONN_OPEN */ {BTA_PAN_IGNORE, BTA_PAN_CLOSING_ST},
    /* PAN_CONN_CLOSE */ {BTA_PAN_CONN_CLOSE, BTA_PAN_IDLE_ST},
    /* FLOW_ENABLE */ {BTA_PAN_RX_PATH, BTA_PAN_CLOSING_ST},
    /* BNEP_DATA */ {BTA_PAN_TX_PATH, BTA_PAN_CLOSING_ST}
};

/* type for state table */
typedef const UINT8(*tBTA_PAN_ST_TBL)[BTA_PAN_NUM_COLS];

/* state table */
const tBTA_PAN_ST_TBL bta_pan_st_tbl[] = {bta_pan_st_idle, bta_pan_st_open,
                                          bta_pan_st_closing
                                         };

/*****************************************************************************
 * Global data
 ****************************************************************************/

/* PAN control block */
tBTA_PAN_CB bta_pan_cb;

/*******************************************************************************
 *
 * Function         bta_pan_scb_alloc
 *
 * Description      Allocate a PAN server control block.
 *
 *
 * Returns          pointer to the scb, or NULL if none could be allocated.
 *
 ******************************************************************************/
tBTA_PAN_SCB *bta_pan_scb_alloc(void)
{
    tBTA_PAN_SCB *p_scb = &bta_pan_cb.scb[0];
    int i;

    for (i = 0; i < BTA_PAN_NUM_CONN; i++, p_scb++) {
        if (!p_scb->in_use) {
            p_scb->in_use = TRUE;
            APPL_TRACE_DEBUG("bta_pan_scb_alloc %d", i);
            break;
        }
    }

    if (i == BTA_PAN_NUM_CONN) {
        /* out of scbs */
        p_scb = NULL;
        APPL_TRACE_WARNING("Out of scbs");
    }
    return p_scb;
}

/*******************************************************************************
 *
 * Function         bta_pan_sm_execute
 *
 * Description      State machine event handling function for PAN
 *
 *
 * Returns          void
 *
 ******************************************************************************/
static void bta_pan_sm_execute(tBTA_PAN_SCB *p_scb, UINT16 event,
                               tBTA_PAN_DATA *p_data)
{
    tBTA_PAN_ST_TBL state_table;
    UINT8 action;
    int i;

    APPL_TRACE_EVENT("PAN scb=%d event=0x%x state=%d", bta_pan_scb_to_idx(p_scb),
                     event, p_scb->state);

    /* look up the state table for the current state */
    state_table = bta_pan_st_tbl[p_scb->state];

    event &= 0x00FF;

    /* set next state */
    p_scb->state = state_table[event][BTA_PAN_NEXT_STATE];

    /* execute action functions */
    for (i = 0; i < BTA_PAN_ACTIONS; i++) {
        action = state_table[event][i];
        if (action != BTA_PAN_IGNORE) {
            (*bta_pan_action[action])(p_scb, p_data);
        } else {
            break;
        }
    }
}

/*******************************************************************************
 *
 * Function         bta_pan_api_enable
 *
 * Description      Handle an API enable event.
 *
 *
 * Returns          void
 *
 ******************************************************************************/
static void bta_pan_api_enable(tBTA_PAN_DATA *p_data)
{
    /* initialize control block */
    memset(&bta_pan_cb, 0, sizeof(bta_pan_cb));

    /* store callback function */
    bta_pan_cb.p_cback = p_data->api_enable.p_cback;
    bta_pan_enable(p_data);
}

/*******************************************************************************
 *
 * Function         bta_pan_api_disable
 *
 * Description      Handle an API disable event.
 *
 *
 * Returns          void
 *
 ******************************************************************************/
static void bta_pan_api_disable(UNUSED_ATTR tBTA_PAN_DATA *p_data)
{
    bta_pan_disable();
}

/*******************************************************************************
 *
 * Function         bta_pan_api_open
 *
 * Description      Handle an API listen event.
 *
 *
 * Returns          void
 *
 ******************************************************************************/
static void bta_pan_api_open(tBTA_PAN_DATA *p_data)
{
    tBTA_PAN_SCB *p_scb;
    tBTA_PAN bta_pan;

    /* allocate an scb */
    p_scb = bta_pan_scb_alloc();
    if (p_scb != NULL) {
        bta_pan_open(p_scb, p_data);
    } else {
        bdcpy(bta_pan.open.bd_addr, p_data->api_open.bd_addr);
        bta_pan.open.handle = PAN_INVALID_HANDLE;
        bta_pan.open.status = BTA_PAN_FAIL;
        bta_pan.open.local_role = p_data->api_open.local_role;
        bta_pan.open.peer_role = p_data->api_open.peer_role;
        bta_pan_cb.p_cback(BTA_PAN_OPEN_EVT, &bta_pan);
    }
}
/*******************************************************************************
 *
 * Function         bta_pan_scb_dealloc
 *
 * Description      Deallocate a link control block.
 *
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_pan_scb_dealloc(tBTA_PAN_SCB *p_scb)
{
    APPL_TRACE_DEBUG("bta_pan_scb_dealloc %d", bta_pan_scb_to_idx(p_scb));
    fixed_queue_free(p_scb->data_queue, NULL);
    memset(p_scb, 0, sizeof(tBTA_PAN_SCB));
}

/*******************************************************************************
 *
 * Function         bta_pan_scb_to_idx
 *
 * Description      Given a pointer to an scb, return its index.
 *
 *
 * Returns          Index of scb.
 *
 ******************************************************************************/
UINT8 bta_pan_scb_to_idx(tBTA_PAN_SCB *p_scb)
{
    return ((UINT8)(p_scb - bta_pan_cb.scb)) + 1;
}

/*******************************************************************************
 *
 * Function         bta_pan_scb_by_handle
 *
 * Description      Find scb associated with handle.
 *
 *
 * Returns          Pointer to scb or NULL if not found.
 *
 ******************************************************************************/
tBTA_PAN_SCB *bta_pan_scb_by_handle(UINT16 handle)
{
    tBTA_PAN_SCB *p_scb = &bta_pan_cb.scb[0];
    UINT8 i;

    for (i = 0; i < BTA_PAN_NUM_CONN; i++, p_scb++) {
        if (p_scb->in_use && p_scb->handle == handle) {
            return p_scb;
        }
    }

    return NULL;
}

/*******************************************************************************
 *
 * Function         bta_pan_hdl_event
 *
 * Description      Data gateway main event handling function.
 *
 *
 * Returns          void
 *
 ******************************************************************************/
BOOLEAN bta_pan_hdl_event(BT_HDR *p_msg)
{
    tBTA_PAN_SCB *p_scb;
    BOOLEAN freebuf = TRUE;

    switch (p_msg->event) {
    /* handle enable event */
    case BTA_PAN_API_ENABLE_EVT:
        bta_pan_api_enable((tBTA_PAN_DATA *)p_msg);
        break;

    /* handle disable event */
    case BTA_PAN_API_DISABLE_EVT:
        bta_pan_api_disable((tBTA_PAN_DATA *)p_msg);
        break;

    /* handle set role event */
    case BTA_PAN_API_SET_ROLE_EVT:
        bta_pan_set_role((tBTA_PAN_DATA *)p_msg);
        break;

    /* handle open event */
    case BTA_PAN_API_OPEN_EVT:
        bta_pan_api_open((tBTA_PAN_DATA *)p_msg);
        break;

    /* set protocol filters on peer (must run in BTU context) */
    case BTA_PAN_CI_SET_PFILTER_EVT: {
        tBTA_PAN_CI_SET_PFILTER *p_filt = (tBTA_PAN_CI_SET_PFILTER *)p_msg;
        PAN_SetProtocolFilters(p_filt->hdr.layer_specific, p_filt->num_filters,
                               (p_filt->num_filters > 0) ? p_filt->start_array : NULL,
                               (p_filt->num_filters > 0) ? p_filt->end_array : NULL);
        break;
    }

    /* set multicast filters on peer (must run in BTU context) */
    case BTA_PAN_CI_SET_MFILTER_EVT: {
        tBTA_PAN_CI_SET_MFILTER *p_filt = (tBTA_PAN_CI_SET_MFILTER *)p_msg;
        PAN_SetMulticastFilters(p_filt->hdr.layer_specific, p_filt->num_filters,
                                (p_filt->num_filters > 0) ? p_filt->start_array : NULL,
                                (p_filt->num_filters > 0) ? p_filt->end_array : NULL);
        break;
    }

    /* events that require buffer not be released when an SCB handles them */
    case BTA_PAN_CI_RX_WRITEBUF_EVT:
        p_scb = bta_pan_scb_by_handle(p_msg->layer_specific);
        if (p_scb != NULL) {
            freebuf = FALSE;
            bta_pan_sm_execute(p_scb, p_msg->event, (tBTA_PAN_DATA *)p_msg);
        }
        break;

    /* all other events */
    default:
        p_scb = bta_pan_scb_by_handle(p_msg->layer_specific);
        if (p_scb != NULL) {
            bta_pan_sm_execute(p_scb, p_msg->event, (tBTA_PAN_DATA *)p_msg);
        }
        break;
    }
    return freebuf;
}
#endif /* BTA_PAN_INCLUDED */

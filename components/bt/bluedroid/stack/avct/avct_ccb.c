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
 *  This module contains functions which operate on the AVCTP connection
 *  control block.
 *
 ******************************************************************************/

#include <string.h>
#include "stack/bt_types.h"
#include "common/bt_target.h"
#include "stack/avct_api.h"
#include "avct_int.h"

#if (defined(AVCT_INCLUDED) && AVCT_INCLUDED == TRUE)

/*******************************************************************************
**
** Function         avct_ccb_alloc
**
** Description      Allocate a connection control block; copy parameters to ccb.
**
**
** Returns          pointer to the ccb, or NULL if none could be allocated.
**
*******************************************************************************/
tAVCT_CCB *avct_ccb_alloc(tAVCT_CC *p_cc)
{
    tAVCT_CCB   *p_ccb = &avct_cb.ccb[0];
    int         i;

    for (i = 0; i < AVCT_NUM_CONN; i++, p_ccb++) {
        if (!p_ccb->allocated) {
            p_ccb->allocated = AVCT_ALOC_LCB;
            memcpy(&p_ccb->cc, p_cc, sizeof(tAVCT_CC));
            AVCT_TRACE_DEBUG("avct_ccb_alloc %d", i);
            break;
        }
    }

    if (i == AVCT_NUM_CONN) {
        /* out of ccbs */
        p_ccb = NULL;
        AVCT_TRACE_WARNING("Out of ccbs");
    }
    return p_ccb;
}

/*******************************************************************************
**
** Function         avct_ccb_dealloc
**
** Description      Deallocate a connection control block and call application
**                  callback.
**
**
** Returns          void.
**
*******************************************************************************/
void avct_ccb_dealloc(tAVCT_CCB *p_ccb, UINT8 event, UINT16 result, BD_ADDR bd_addr)
{
    tAVCT_CTRL_CBACK    *p_cback = p_ccb->cc.p_ctrl_cback;

    AVCT_TRACE_DEBUG("avct_ccb_dealloc %d", avct_ccb_to_idx(p_ccb));
#if (AVCT_BROWSE_INCLUDED == TRUE)
    if (p_ccb->p_bcb == NULL) {
        memset(p_ccb, 0, sizeof(tAVCT_CCB));
    } else {
        /* control channel is down, but the browsing channel is still connected 0 disconnect it now */
        avct_bcb_event(p_ccb->p_bcb, AVCT_LCB_UL_UNBIND_EVT, (tAVCT_LCB_EVT *) &p_ccb);
        p_ccb->p_lcb = NULL;
    }
#else
    memset(p_ccb, 0, sizeof(tAVCT_CCB));
#endif

    if (event != AVCT_NO_EVT) {
        (*p_cback)(avct_ccb_to_idx(p_ccb), event, result, bd_addr);
    }
}

/*******************************************************************************
**
** Function         avct_ccb_to_idx
**
** Description      Given a pointer to an ccb, return its index.
**
**
** Returns          Index of ccb.
**
*******************************************************************************/
UINT8 avct_ccb_to_idx(tAVCT_CCB *p_ccb)
{
    /* use array arithmetic to determine index */
    return (UINT8) (p_ccb - avct_cb.ccb);
}

/*******************************************************************************
**
** Function         avct_ccb_by_idx
**
** Description      Return ccb pointer based on ccb index (or handle).
**
**
** Returns          pointer to the ccb, or NULL if none found.
**
*******************************************************************************/
tAVCT_CCB *avct_ccb_by_idx(UINT8 idx)
{
    tAVCT_CCB   *p_ccb;

    /* verify index */
    if (idx < AVCT_NUM_CONN) {
        p_ccb = &avct_cb.ccb[idx];

        /* verify ccb is allocated */
        if (!p_ccb->allocated) {
            p_ccb = NULL;
            AVCT_TRACE_WARNING("ccb %d not allocated", idx);
        }
    } else {
        p_ccb = NULL;
        AVCT_TRACE_WARNING("No ccb for idx %d", idx);
    }
    return p_ccb;
}

#endif /* #if (defined(AVCT_INCLUDED) && AVCT_INCLUDED == TRUE) */

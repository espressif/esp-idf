/******************************************************************************
 *
 *  Copyright (C) 2014 The Android Open Source Project

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
 *  This is the main implementation file for the BTA MCE I/F
 *
 ******************************************************************************/

#include <stdlib.h>
#include "common/bt_target.h"
#include "bta/bta_api.h"
#include "bta/bta_sys.h"
#include "bta/bta_sdp_api.h"
#include "bta_sdp_int.h"

#if defined(BTA_SDP_INCLUDED) && (BTA_SDP_INCLUDED == TRUE)

/*****************************************************************************
** Constants and types
*****************************************************************************/

#if BTA_DYNAMIC_MEMORY == FALSE
tBTA_SDP_CB bta_sdp_cb;
#else
tBTA_SDP_CB *bta_sdp_cb_ptr;
#endif

/* state machine action enumeration list */
#define BTA_SDP_NUM_ACTIONS  (BTA_SDP_MAX_INT_EVT & 0x00ff)

/* type for action functions */
typedef void (*tBTA_SDP_ACTION)(tBTA_SDP_MSG *p_data);

/* action function list */
const tBTA_SDP_ACTION bta_sdp_action[] = {
    bta_sdp_enable,        /* BTA_SDP_API_ENABLE_EVT */
    bta_sdp_search,        /* BTA_SDP_API_SEARCH_EVT */
    bta_sdp_create_record, /* BTA_SDP_API_CREATE_RECORD_USER_EVT */
    bta_sdp_remove_record, /* BTA_SDP_API_REMOVE_RECORD_USER_EVT */
    bta_sdp_disable,       /* BTA_SDP_API_DISABLE_EVT */
};

/*******************************************************************************
** Function         bta_sdp_sm_execute
**
** Description      State machine event handling function for SDP search
**
** Returns          void
*******************************************************************************/
BOOLEAN bta_sdp_sm_execute(BT_HDR *p_msg)
{
    if (p_msg == NULL) {
        return FALSE;
    }

    BOOLEAN ret = FALSE;
    UINT16 action = (p_msg->event & 0x00ff);

    /* execute action functions */
    if (action < BTA_SDP_NUM_ACTIONS) {
        (*bta_sdp_action[action])((tBTA_SDP_MSG *)p_msg);
        ret = TRUE;
    }

    return (ret);
}

#endif /* #if defined(BTA_SDP_INCLUDED) && (BTA_SDP_INCLUDED == TRUE) */

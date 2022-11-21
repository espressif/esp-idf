/******************************************************************************
 *
 *  Copyright (C) 2012 Broadcom Corporation
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
 *  This is the main implementation file for the BTA Java I/F
 *
 ******************************************************************************/

#include "bta/bta_api.h"
#include "bta/bta_sys.h"
#include "bta/bta_jv_api.h"
#include "bta_jv_int.h"
#include "common/bt_target.h"

#if (defined BTA_JV_INCLUDED && BTA_JV_INCLUDED == TRUE)
/*****************************************************************************
** Constants and types
*****************************************************************************/

#if BTA_DYNAMIC_MEMORY == FALSE
tBTA_JV_CB bta_jv_cb;
#else
tBTA_JV_CB *bta_jv_cb_ptr;
#endif

/* state machine action enumeration list */
#define BTA_JV_NUM_ACTIONS  (BTA_JV_MAX_INT_EVT & 0x00ff)

/* type for action functions */
typedef void (*tBTA_JV_ACTION)(tBTA_JV_MSG *p_data);

/* action function list */
const tBTA_JV_ACTION bta_jv_action[] = {
    bta_jv_enable,                  /* BTA_JV_API_ENABLE_EVT */
    bta_jv_disable,                 /* BTA_JV_API_DISABLE_EVT */
    bta_jv_get_channel_id,          /* BTA_JV_API_GET_CHANNEL_EVT */
    bta_jv_free_scn,                /* BTA_JV_API_FREE_SCN_EVT */
    bta_jv_start_discovery,         /* BTA_JV_API_START_DISCOVERY_EVT */
    bta_jv_create_record,           /* BTA_JV_API_CREATE_RECORD_EVT */
    bta_jv_delete_record,           /* BTA_JV_API_DELETE_RECORD_EVT */
#if BTA_JV_L2CAP_INCLUDED
    bta_jv_l2cap_connect,           /* BTA_JV_API_L2CAP_CONNECT_EVT */
    bta_jv_l2cap_close,             /* BTA_JV_API_L2CAP_CLOSE_EVT */
    bta_jv_l2cap_start_server,      /* BTA_JV_API_L2CAP_START_SERVER_EVT */
    bta_jv_l2cap_stop_server,       /* BTA_JV_API_L2CAP_STOP_SERVER_EVT */
    bta_jv_l2cap_read,              /* BTA_JV_API_L2CAP_READ_EVT */
    bta_jv_l2cap_write,             /* BTA_JV_API_L2CAP_WRITE_EVT */
#endif /* BTA_JV_L2CAP_INCLUDED */
#if BTA_JV_RFCOMM_INCLUDED
    bta_jv_rfcomm_config,           /* BTA_JV_API_RFCOMM_CONFIG_EVT */
    bta_jv_rfcomm_connect,          /* BTA_JV_API_RFCOMM_CONNECT_EVT */
    bta_jv_rfcomm_close,            /* BTA_JV_API_RFCOMM_CLOSE_EVT */
    bta_jv_rfcomm_start_server,     /* BTA_JV_API_RFCOMM_START_SERVER_EVT */
    bta_jv_rfcomm_stop_server,      /* BTA_JV_API_RFCOMM_STOP_SERVER_EVT */
    bta_jv_rfcomm_read,             /* BTA_JV_API_RFCOMM_READ_EVT */
    bta_jv_rfcomm_write,            /* BTA_JV_API_RFCOMM_WRITE_EVT */
    bta_jv_rfcomm_flow_control,     /* BTA_JV_API_RFCOMM_FLOW_CONTROL_EVT */
#endif /* BTA_JV_RFCOMM_INCLUDED */
    bta_jv_set_pm_profile,          /* BTA_JV_API_SET_PM_PROFILE_EVT */
    bta_jv_change_pm_state,         /* BTA_JV_API_PM_STATE_CHANGE_EVT */
#if BTA_JV_L2CAP_INCLUDED
    bta_jv_l2cap_connect_le,        /* BTA_JV_API_L2CAP_CONNECT_LE_EVT */
    bta_jv_l2cap_start_server_le,   /* BTA_JV_API_L2CAP_START_SERVER_LE_EVT */
    bta_jv_l2cap_stop_server_le,    /* BTA_JV_API_L2CAP_STOP_SERVER_LE_EVT */
    bta_jv_l2cap_write_fixed,       /* BTA_JV_API_L2CAP_WRITE_FIXED_EVT */
    bta_jv_l2cap_close_fixed,       /*  BTA_JV_API_L2CAP_CLOSE_FIXED_EVT */
#endif /* BTA_JV_L2CAP_INCLUDED */
};

/*******************************************************************************
**
** Function         bta_jv_sm_execute
**
** Description      State machine event handling function for JV
**
**
** Returns          void
**
*******************************************************************************/
BOOLEAN bta_jv_sm_execute(BT_HDR *p_msg)
{
    BOOLEAN ret = FALSE;
    UINT16 action = (p_msg->event & 0x00ff);
    /* execute action functions */

    if (action < BTA_JV_NUM_ACTIONS) {
        (*bta_jv_action[action])((tBTA_JV_MSG *)p_msg);
        ret = TRUE;
    }

    return (ret);
}

#endif  ///defined BTA_JV_INCLUDED && BTA_JV_INCLUDED == TRUE

/******************************************************************************
 *
 *  Copyright (C) 2016 The Android Open Source Project
 *  Copyright (C) 2005-2012 Broadcom Corporation
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
 *  This file contains the HID host main functions and state machine.
 *
 ******************************************************************************/
#include "common/bt_target.h"

#if defined(BTA_HD_INCLUDED) && (BTA_HD_INCLUDED == TRUE)

#include "bta/bta_hd_api.h"
#include "bta_hd_int.h"
#include <string.h>

/*****************************************************************************
 * Constants and types
 ****************************************************************************/
/* state machine states */
enum {
    BTA_HD_INIT_ST,
    BTA_HD_IDLE_ST,              /* not connected, waiting for connection */
    BTA_HD_CONN_ST,              /* host connected */
    BTA_HD_TRANSIENT_TO_INIT_ST, /* transient state: going back from CONN to INIT */
};
typedef uint8_t tBTA_HD_STATE;

/* state machine actions */
enum {
    BTA_HD_REGISTER_ACT,
    BTA_HD_UNREGISTER_ACT,
    BTA_HD_UNREGISTER2_ACT,
    BTA_HD_CONNECT_ACT,
    BTA_HD_DISCONNECT_ACT,
    BTA_HD_ADD_DEVICE_ACT,
    BTA_HD_REMOVE_DEVICE_ACT,
    BTA_HD_SEND_REPORT_ACT,
    BTA_HD_REPORT_ERROR_ACT,
    BTA_HD_VC_UNPLUG_ACT,
    BTA_HD_OPEN_ACT,
    BTA_HD_CLOSE_ACT,
    BTA_HD_INTR_DATA_ACT,
    BTA_HD_GET_REPORT_ACT,
    BTA_HD_SET_REPORT_ACT,
    BTA_HD_SET_PROTOCOL_ACT,
    BTA_HD_VC_UNPLUG_DONE_ACT,
    BTA_HD_SUSPEND_ACT,
    BTA_HD_EXIT_SUSPEND_ACT,
    BTA_HD_NUM_ACTIONS
};

#define BTA_HD_IGNORE BTA_HD_NUM_ACTIONS

typedef void (*tBTA_HD_ACTION)(tBTA_HD_DATA *p_data);
/* action functions */
const tBTA_HD_ACTION bta_hd_action[] = {
    bta_hd_register_act,       bta_hd_unregister_act, bta_hd_unregister2_act,   bta_hd_connect_act,
    bta_hd_disconnect_act,     bta_hd_add_device_act, bta_hd_remove_device_act, bta_hd_send_report_act,
    bta_hd_report_error_act,   bta_hd_vc_unplug_act,  bta_hd_open_act,          bta_hd_close_act,
    bta_hd_intr_data_act,      bta_hd_get_report_act, bta_hd_set_report_act,    bta_hd_set_protocol_act,
    bta_hd_vc_unplug_done_act, bta_hd_suspend_act,    bta_hd_exit_suspend_act,
};

/* state table information */
#define BTA_HD_ACTION 0     /* position of action */
#define BTA_HD_NEXT_STATE 1 /* position of next state */
#define BTA_HD_NUM_COLS 2   /* number of columns */

const uint8_t bta_hd_st_init[][BTA_HD_NUM_COLS] = {
    /* Event                               Action                     Next state
     */
    /* BTA_HD_API_REGISTER_APP_EVT   */ {BTA_HD_REGISTER_ACT, BTA_HD_IDLE_ST},
    /* BTA_HD_API_UNREGISTER_APP_EVT */ {BTA_HD_IGNORE, BTA_HD_INIT_ST},
    /* BTA_HD_API_CONNECT_EVT        */ {BTA_HD_IGNORE, BTA_HD_INIT_ST},
    /* BTA_HD_API_DISCONNECT_EVT     */ {BTA_HD_IGNORE, BTA_HD_INIT_ST},
    /* BTA_HD_API_ADD_DEVICE_EVT     */ {BTA_HD_ADD_DEVICE_ACT, BTA_HD_INIT_ST},
    /* BTA_HD_API_REMOVE_DEVICE_EVT  */ {BTA_HD_REMOVE_DEVICE_ACT, BTA_HD_INIT_ST},
    /* BTA_HD_API_SEND_REPORT_EVT    */ {BTA_HD_IGNORE, BTA_HD_INIT_ST},
    /* BTA_HD_API_REPORT_ERROR_EVT   */ {BTA_HD_IGNORE, BTA_HD_INIT_ST},
    /* BTA_HD_API_VC_UNPLUG_EVT      */ {BTA_HD_IGNORE, BTA_HD_INIT_ST},
    /* BTA_HD_INT_OPEN_EVT           */ {BTA_HD_IGNORE, BTA_HD_INIT_ST},
    /* BTA_HD_INT_CLOSE_EVT          */ {BTA_HD_IGNORE, BTA_HD_INIT_ST},
    /* BTA_HD_INT_INTR_DATA_EVT      */ {BTA_HD_IGNORE, BTA_HD_INIT_ST},
    /* BTA_HD_INT_GET_REPORT_EVT     */ {BTA_HD_IGNORE, BTA_HD_INIT_ST},
    /* BTA_HD_INT_SET_REPORT_EVT     */ {BTA_HD_IGNORE, BTA_HD_INIT_ST},
    /* BTA_HD_INT_SET_PROTOCOL_EVT   */ {BTA_HD_IGNORE, BTA_HD_INIT_ST},
    /* BTA_HD_INT_VC_UNPLUG_EVT      */ {BTA_HD_IGNORE, BTA_HD_INIT_ST},
    /* BTA_HD_INT_SUSPEND_EVT        */ {BTA_HD_IGNORE, BTA_HD_INIT_ST},
    /* BTA_HD_INT_EXIT_SUSPEND_EVT   */ {BTA_HD_IGNORE, BTA_HD_INIT_ST},
};

const uint8_t bta_hd_st_idle[][BTA_HD_NUM_COLS] = {
    /* Event                               Action                     Next state
     */
    /* BTA_HD_API_REGISTER_APP_EVT   */ {BTA_HD_IGNORE, BTA_HD_IDLE_ST},
    /* BTA_HD_API_UNREGISTER_APP_EVT */ {BTA_HD_UNREGISTER_ACT, BTA_HD_INIT_ST},
    /* BTA_HD_API_CONNECT_EVT        */ {BTA_HD_CONNECT_ACT, BTA_HD_IDLE_ST},
    /* BTA_HD_API_DISCONNECT_EVT     */ {BTA_HD_IGNORE, BTA_HD_IDLE_ST},
    /* BTA_HD_API_ADD_DEVICE_EVT     */ {BTA_HD_ADD_DEVICE_ACT, BTA_HD_IDLE_ST},
    /* BTA_HD_API_REMOVE_DEVICE_EVT  */ {BTA_HD_REMOVE_DEVICE_ACT, BTA_HD_IDLE_ST},
    /* BTA_HD_API_SEND_REPORT_EVT    */ {BTA_HD_SEND_REPORT_ACT, BTA_HD_IDLE_ST},
    /* BTA_HD_API_REPORT_ERROR_EVT   */ {BTA_HD_IGNORE, BTA_HD_IDLE_ST},
    /* BTA_HD_API_VC_UNPLUG_EVT      */ {BTA_HD_VC_UNPLUG_ACT, BTA_HD_IDLE_ST},
    /* BTA_HD_INT_OPEN_EVT           */ {BTA_HD_OPEN_ACT, BTA_HD_CONN_ST},
    /* BTA_HD_INT_CLOSE_EVT          */ {BTA_HD_IGNORE, BTA_HD_IDLE_ST},
    /* BTA_HD_INT_INTR_DATA_EVT      */ {BTA_HD_IGNORE, BTA_HD_IDLE_ST},
    /* BTA_HD_INT_GET_REPORT_EVT     */ {BTA_HD_IGNORE, BTA_HD_IDLE_ST},
    /* BTA_HD_INT_SET_REPORT_EVT     */ {BTA_HD_IGNORE, BTA_HD_IDLE_ST},
    /* BTA_HD_INT_SET_PROTOCOL_EVT   */ {BTA_HD_IGNORE, BTA_HD_IDLE_ST},
    /* BTA_HD_INT_VC_UNPLUG_EVT      */ {BTA_HD_IGNORE, BTA_HD_IDLE_ST},
    /* BTA_HD_INT_SUSPEND_EVT        */ {BTA_HD_IGNORE, BTA_HD_IDLE_ST},
    /* BTA_HD_INT_EXIT_SUSPEND_EVT   */ {BTA_HD_IGNORE, BTA_HD_IDLE_ST},
};

const uint8_t bta_hd_st_conn[][BTA_HD_NUM_COLS] = {
    /* Event                               Action Next state */
    /* BTA_HD_API_REGISTER_APP_EVT   */ {BTA_HD_IGNORE, BTA_HD_CONN_ST},
    /* BTA_HD_API_UNREGISTER_APP_EVT */ {BTA_HD_DISCONNECT_ACT, BTA_HD_TRANSIENT_TO_INIT_ST},
    /* BTA_HD_API_CONNECT_EVT        */ {BTA_HD_IGNORE, BTA_HD_CONN_ST},
    /* BTA_HD_API_DISCONNECT_EVT     */ {BTA_HD_DISCONNECT_ACT, BTA_HD_CONN_ST},
    /* BTA_HD_API_ADD_DEVICE_EVT     */ {BTA_HD_ADD_DEVICE_ACT, BTA_HD_CONN_ST},
    /* BTA_HD_API_REMOVE_DEVICE_EVT  */ {BTA_HD_REMOVE_DEVICE_ACT, BTA_HD_CONN_ST},
    /* BTA_HD_API_SEND_REPORT_EVT    */ {BTA_HD_SEND_REPORT_ACT, BTA_HD_CONN_ST},
    /* BTA_HD_API_REPORT_ERROR_EVT   */ {BTA_HD_REPORT_ERROR_ACT, BTA_HD_CONN_ST},
    /* BTA_HD_API_VC_UNPLUG_EVT      */ {BTA_HD_VC_UNPLUG_ACT, BTA_HD_CONN_ST},
    /* BTA_HD_INT_OPEN_EVT           */ {BTA_HD_IGNORE, BTA_HD_CONN_ST},
    /* BTA_HD_INT_CLOSE_EVT          */ {BTA_HD_CLOSE_ACT, BTA_HD_IDLE_ST},
    /* BTA_HD_INT_INTR_DATA_EVT      */ {BTA_HD_INTR_DATA_ACT, BTA_HD_CONN_ST},
    /* BTA_HD_INT_GET_REPORT_EVT     */ {BTA_HD_GET_REPORT_ACT, BTA_HD_CONN_ST},
    /* BTA_HD_INT_SET_REPORT_EVT     */ {BTA_HD_SET_REPORT_ACT, BTA_HD_CONN_ST},
    /* BTA_HD_INT_SET_PROTOCOL_EVT   */ {BTA_HD_SET_PROTOCOL_ACT, BTA_HD_CONN_ST},
    /* BTA_HD_INT_VC_UNPLUG_EVT      */ {BTA_HD_VC_UNPLUG_DONE_ACT, BTA_HD_IDLE_ST},
    /* BTA_HD_INT_SUSPEND_EVT        */ {BTA_HD_SUSPEND_ACT, BTA_HD_CONN_ST},
    /* BTA_HD_INT_EXIT_SUSPEND_EVT   */ {BTA_HD_EXIT_SUSPEND_ACT, BTA_HD_CONN_ST},
};

const uint8_t bta_hd_st_transient_to_init[][BTA_HD_NUM_COLS] = {
    /* Event                               Action Next state */
    /* BTA_HD_API_REGISTER_APP_EVT   */ {BTA_HD_IGNORE, BTA_HD_TRANSIENT_TO_INIT_ST},
    /* BTA_HD_API_UNREGISTER_APP_EVT */ {BTA_HD_IGNORE, BTA_HD_TRANSIENT_TO_INIT_ST},
    /* BTA_HD_API_CONNECT_EVT        */ {BTA_HD_IGNORE, BTA_HD_TRANSIENT_TO_INIT_ST},
    /* BTA_HD_API_DISCONNECT_EVT     */ {BTA_HD_IGNORE, BTA_HD_TRANSIENT_TO_INIT_ST},
    /* BTA_HD_API_ADD_DEVICE_EVT     */ {BTA_HD_IGNORE, BTA_HD_TRANSIENT_TO_INIT_ST},
    /* BTA_HD_API_REMOVE_DEVICE_EVT  */ {BTA_HD_IGNORE, BTA_HD_TRANSIENT_TO_INIT_ST},
    /* BTA_HD_API_SEND_REPORT_EVT    */ {BTA_HD_IGNORE, BTA_HD_TRANSIENT_TO_INIT_ST},
    /* BTA_HD_API_REPORT_ERROR_EVT   */ {BTA_HD_IGNORE, BTA_HD_TRANSIENT_TO_INIT_ST},
    /* BTA_HD_API_VC_UNPLUG_EVT      */ {BTA_HD_IGNORE, BTA_HD_TRANSIENT_TO_INIT_ST},
    /* BTA_HD_INT_OPEN_EVT           */ {BTA_HD_IGNORE, BTA_HD_TRANSIENT_TO_INIT_ST},
    /* BTA_HD_INT_CLOSE_EVT          */ {BTA_HD_UNREGISTER2_ACT, BTA_HD_INIT_ST},
    /* BTA_HD_INT_INTR_DATA_EVT      */ {BTA_HD_IGNORE, BTA_HD_TRANSIENT_TO_INIT_ST},
    /* BTA_HD_INT_GET_REPORT_EVT     */ {BTA_HD_IGNORE, BTA_HD_TRANSIENT_TO_INIT_ST},
    /* BTA_HD_INT_SET_REPORT_EVT     */ {BTA_HD_IGNORE, BTA_HD_TRANSIENT_TO_INIT_ST},
    /* BTA_HD_INT_SET_PROTOCOL_EVT   */ {BTA_HD_IGNORE, BTA_HD_TRANSIENT_TO_INIT_ST},
    /* BTA_HD_INT_VC_UNPLUG_EVT      */ {BTA_HD_UNREGISTER2_ACT, BTA_HD_INIT_ST},
    /* BTA_HD_INT_SUSPEND_EVT        */ {BTA_HD_IGNORE, BTA_HD_TRANSIENT_TO_INIT_ST},
    /* BTA_HD_INT_EXIT_SUSPEND_EVT   */ {BTA_HD_IGNORE, BTA_HD_TRANSIENT_TO_INIT_ST},
};

/* type for state table */
typedef const uint8_t (*tBTA_HD_ST_TBL)[BTA_HD_NUM_COLS];
/* state table */
const tBTA_HD_ST_TBL bta_hd_st_tbl[] = {bta_hd_st_init, bta_hd_st_idle, bta_hd_st_conn, bta_hd_st_transient_to_init};

/*****************************************************************************
 * Global data
 ****************************************************************************/
#if BTA_DYNAMIC_MEMORY == FALSE
tBTA_HD_CB bta_hd_cb;
#else
tBTA_HD_CB *bta_hd_cb_ptr;
#endif

static const char *bta_hd_evt_code(tBTA_HD_INT_EVT evt_code);
static const char *bta_hd_state_code(tBTA_HD_STATE state_code);

/*******************************************************************************
 *
 * Function         bta_hd_sm_execute
 *
 * Description      State machine event handling function for HID Device
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_hd_sm_execute(uint16_t event, tBTA_HD_DATA *p_data)
{
    tBTA_HD_ST_TBL state_table;
    tBTA_HD_STATE prev_state;
    uint8_t action;
    tBTA_HD cback_data;

    APPL_TRACE_EVENT("%s: state=%s (%d) event=%s (%d)", __func__, bta_hd_state_code(bta_hd_cb.state), bta_hd_cb.state,
                     bta_hd_evt_code(event), event);

    prev_state = bta_hd_cb.state;
    memset(&cback_data, 0, sizeof(tBTA_HD));
    state_table = bta_hd_st_tbl[bta_hd_cb.state];
    event &= 0xff;

    if ((action = state_table[event][BTA_HD_ACTION]) < BTA_HD_IGNORE) {
        (*bta_hd_action[action])(p_data);
    }

    bta_hd_cb.state = state_table[event][BTA_HD_NEXT_STATE];

    if (bta_hd_cb.state != prev_state) {
        APPL_TRACE_EVENT("%s: [new] state=%s (%d)", __func__, bta_hd_state_code(bta_hd_cb.state), bta_hd_cb.state);
    }
    return;
}

/*******************************************************************************
 *
 * Function         bta_hd_hdl_event
 *
 * Description      HID device main event handling function.
 *
 * Returns          void
 *
 ******************************************************************************/
bool bta_hd_hdl_event(BT_HDR *p_msg)
{
    APPL_TRACE_API("%s: p_msg->event=%d", __func__, p_msg->event);

    switch (p_msg->event) {
    case BTA_HD_API_ENABLE_EVT:
        bta_hd_api_enable((tBTA_HD_DATA *)p_msg);
        break;
    case BTA_HD_API_DISABLE_EVT:
        if (bta_hd_cb.state == BTA_HD_CONN_ST) {
            APPL_TRACE_WARNING("%s: host connected, disconnect before disabling", __func__);
            // unregister (and disconnect)
            bta_hd_cb.disable_w4_close = TRUE;
            bta_hd_sm_execute(BTA_HD_API_UNREGISTER_APP_EVT, (tBTA_HD_DATA *)p_msg);
        } else {
            bta_hd_api_disable();
        }
        break;
    default:
        bta_hd_sm_execute(p_msg->event, (tBTA_HD_DATA *)p_msg);
    }
    return (TRUE);
}

static const char *bta_hd_evt_code(tBTA_HD_INT_EVT evt_code)
{
    switch (evt_code) {
    case BTA_HD_API_REGISTER_APP_EVT:
        return "BTA_HD_API_REGISTER_APP_EVT";
    case BTA_HD_API_UNREGISTER_APP_EVT:
        return "BTA_HD_API_UNREGISTER_APP_EVT";
    case BTA_HD_API_CONNECT_EVT:
        return "BTA_HD_API_CONNECT_EVT";
    case BTA_HD_API_DISCONNECT_EVT:
        return "BTA_HD_API_DISCONNECT_EVT";
    case BTA_HD_API_ADD_DEVICE_EVT:
        return "BTA_HD_API_ADD_DEVICE_EVT";
    case BTA_HD_API_REMOVE_DEVICE_EVT:
        return "BTA_HD_API_REMOVE_DEVICE_EVT";
    case BTA_HD_API_SEND_REPORT_EVT:
        return "BTA_HD_API_SEND_REPORT_EVT";
    case BTA_HD_API_REPORT_ERROR_EVT:
        return "BTA_HD_API_REPORT_ERROR_EVT";
    case BTA_HD_API_VC_UNPLUG_EVT:
        return "BTA_HD_API_VC_UNPLUG_EVT";
    case BTA_HD_INT_OPEN_EVT:
        return "BTA_HD_INT_OPEN_EVT";
    case BTA_HD_INT_CLOSE_EVT:
        return "BTA_HD_INT_CLOSE_EVT";
    case BTA_HD_INT_INTR_DATA_EVT:
        return "BTA_HD_INT_INTR_DATA_EVT";
    case BTA_HD_INT_GET_REPORT_EVT:
        return "BTA_HD_INT_GET_REPORT_EVT";
    case BTA_HD_INT_SET_REPORT_EVT:
        return "BTA_HD_INT_SET_REPORT_EVT";
    case BTA_HD_INT_SET_PROTOCOL_EVT:
        return "BTA_HD_INT_SET_PROTOCOL_EVT";
    case BTA_HD_INT_VC_UNPLUG_EVT:
        return "BTA_HD_INT_VC_UNPLUG_EVT";
    case BTA_HD_INT_SUSPEND_EVT:
        return "BTA_HD_INT_SUSPEND_EVT";
    case BTA_HD_INT_EXIT_SUSPEND_EVT:
        return "BTA_HD_INT_EXIT_SUSPEND_EVT";
    default:
        return "<unknown>";
    }
}

static const char *bta_hd_state_code(tBTA_HD_STATE state_code)
{
    switch (state_code) {
    case BTA_HD_INIT_ST:
        return "BTA_HD_INIT_ST";
    case BTA_HD_IDLE_ST:
        return "BTA_HD_IDLE_ST";
    case BTA_HD_CONN_ST:
        return "BTA_HD_CONN_ST";
    case BTA_HD_TRANSIENT_TO_INIT_ST:
        return "BTA_HD_TRANSIENT_TO_INIT_ST";
    default:
        return "<unknown>";
    }
}

#if BT_HID_DEVICE_BQB_INCLUDED
tBTA_STATUS bta_hd_bqb_set_local_di_record(void)
{
    tBTA_STATUS status = BTA_FAILURE;

    tBTA_DI_RECORD bqb_device_info;
    bqb_device_info.vendor = 0;
    bqb_device_info.vendor_id_source = 0xff; // BTA_HH_VENDOR_ID_INVALID
    bqb_device_info.product = 1;
    bqb_device_info.version = 0;
    bqb_device_info.primary_record = TRUE;

    return BTA_DmSetLocalDiRecord(&bqb_device_info, &bta_hd_cb.sdp_handle);
}
#endif /* BT_HID_DEVICE_BQB_INCLUDED */

#endif /* BTA_HD_INCLUDED */

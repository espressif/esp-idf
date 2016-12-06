/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
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


/*****************************************************************************
 *
 *  Filename:      btif_av.c
 *
 *  Description:   Bluedroid AV implementation
 *
 *****************************************************************************/

#include "bt_trace.h"
#include <string.h>

#include "bt_defs.h"
#include "esp_bt_defs.h"
#include "esp_a2dp_api.h"
#include "allocator.h"

#define LOG_TAG "bt_btif_av"

#include "btif_av.h"
#include "btif_util.h"
#include "btif_profile_queue.h"
#include "bta_api.h"
#include "btif_media.h"
#include "bta_av_api.h"
#include "gki.h"
#include "btu.h"
#include "bt_utils.h"

/*****************************************************************************
**  Constants & Macros
******************************************************************************/
#define BTIF_AV_SERVICE_NAME "Advanced Audio"

#define BTIF_TIMEOUT_AV_OPEN_ON_RC_SECS  2

typedef enum {
    BTIF_AV_STATE_IDLE = 0x0,
    BTIF_AV_STATE_OPENING,
    BTIF_AV_STATE_OPENED,
    BTIF_AV_STATE_STARTED,
    BTIF_AV_STATE_CLOSING
} btif_av_state_t;

/* Should not need dedicated suspend state as actual actions are no
   different than open state. Suspend flags are needed however to prevent
   media task from trying to restart stream during remote suspend or while
   we are in the process of a local suspend */

#define BTIF_AV_FLAG_LOCAL_SUSPEND_PENDING 0x1
#define BTIF_AV_FLAG_REMOTE_SUSPEND        0x2
#define BTIF_AV_FLAG_PENDING_START         0x4
#define BTIF_AV_FLAG_PENDING_STOP          0x8

/*****************************************************************************
**  Local type definitions
******************************************************************************/

typedef struct {
    tBTA_AV_HNDL bta_handle;
    bt_bdaddr_t peer_bda;
    btif_sm_handle_t sm_handle;
    UINT8 flags;
    tBTA_AV_EDR edr;
    UINT8   peer_sep;  /* sep type of peer device */
} btif_av_cb_t;

typedef struct {
    bt_bdaddr_t *target_bda;
    uint16_t uuid;
} btif_av_connect_req_t;

/*****************************************************************************
**  Static variables
******************************************************************************/

static esp_profile_cb_t bt_av_sink_callback = NULL;

static btif_av_cb_t btif_av_cb = {0};
// static TIMER_LIST_ENT tle_av_open_on_rc;

// TODO: need protection against race
#define BTIF_A2D_CB_TO_APP(_event, _param)    do { \
	if (bt_av_sink_callback) { \
	    bt_av_sink_callback(_event, _param); \
	} \
    } while (0)

/* both interface and media task needs to be ready to alloc incoming request */
#define CHECK_BTAV_INIT() if (btif_av_cb.sm_handle == NULL)\
{\
     BTIF_TRACE_WARNING("%s: BTAV not initialized\n", __FUNCTION__);\
     return ESP_ERR_INVALID_STATE;\
}\
else\
{\
     BTIF_TRACE_EVENT("%s\n", __FUNCTION__);\
}

/* Helper macro to avoid code duplication in the state machine handlers */
#define CHECK_RC_EVENT(e, d) \
    case BTA_AV_RC_OPEN_EVT: \
    case BTA_AV_RC_CLOSE_EVT: \
    case BTA_AV_REMOTE_CMD_EVT: \
    case BTA_AV_VENDOR_CMD_EVT: \
    case BTA_AV_META_MSG_EVT: \
    case BTA_AV_RC_FEAT_EVT: \
    case BTA_AV_REMOTE_RSP_EVT: \
    { \
    }break; \

static BOOLEAN btif_av_state_idle_handler(btif_sm_event_t event, void *data);
static BOOLEAN btif_av_state_opening_handler(btif_sm_event_t event, void *data);
static BOOLEAN btif_av_state_opened_handler(btif_sm_event_t event, void *data);
static BOOLEAN btif_av_state_started_handler(btif_sm_event_t event, void *data);
static BOOLEAN btif_av_state_closing_handler(btif_sm_event_t event, void *data);

static const btif_sm_handler_t btif_av_state_handlers[] = {
    btif_av_state_idle_handler,
    btif_av_state_opening_handler,
    btif_av_state_opened_handler,
    btif_av_state_started_handler,
    btif_av_state_closing_handler
};

static void btif_av_event_free_data(btif_sm_event_t event, void *p_data);

/*************************************************************************
** Extern functions
*************************************************************************/

extern tBTA_AV_CO_FUNCTS bta_av_a2d_cos;
/*****************************************************************************
** Local helper functions
******************************************************************************/

const char *dump_av_sm_state_name(btif_av_state_t state)
{
    switch (state) {
        CASE_RETURN_STR(BTIF_AV_STATE_IDLE)
        CASE_RETURN_STR(BTIF_AV_STATE_OPENING)
        CASE_RETURN_STR(BTIF_AV_STATE_OPENED)
        CASE_RETURN_STR(BTIF_AV_STATE_STARTED)
        CASE_RETURN_STR(BTIF_AV_STATE_CLOSING)
    default: return "UNKNOWN_STATE";
    }
}

const char *dump_av_sm_event_name(btif_av_sm_event_t event)
{
    switch ((int)event) {
        CASE_RETURN_STR(BTA_AV_ENABLE_EVT)
        CASE_RETURN_STR(BTA_AV_REGISTER_EVT)
        CASE_RETURN_STR(BTA_AV_OPEN_EVT)
        CASE_RETURN_STR(BTA_AV_CLOSE_EVT)
        CASE_RETURN_STR(BTA_AV_START_EVT)
        CASE_RETURN_STR(BTA_AV_STOP_EVT)
        CASE_RETURN_STR(BTA_AV_PROTECT_REQ_EVT)
        CASE_RETURN_STR(BTA_AV_PROTECT_RSP_EVT)
        CASE_RETURN_STR(BTA_AV_RC_OPEN_EVT)
        CASE_RETURN_STR(BTA_AV_RC_CLOSE_EVT)
        CASE_RETURN_STR(BTA_AV_REMOTE_CMD_EVT)
        CASE_RETURN_STR(BTA_AV_REMOTE_RSP_EVT)
        CASE_RETURN_STR(BTA_AV_VENDOR_CMD_EVT)
        CASE_RETURN_STR(BTA_AV_VENDOR_RSP_EVT)
        CASE_RETURN_STR(BTA_AV_RECONFIG_EVT)
        CASE_RETURN_STR(BTA_AV_SUSPEND_EVT)
        CASE_RETURN_STR(BTA_AV_PENDING_EVT)
        CASE_RETURN_STR(BTA_AV_META_MSG_EVT)
        CASE_RETURN_STR(BTA_AV_REJECT_EVT)
        CASE_RETURN_STR(BTA_AV_RC_FEAT_EVT)
        CASE_RETURN_STR(BTIF_SM_ENTER_EVT)
        CASE_RETURN_STR(BTIF_SM_EXIT_EVT)
        CASE_RETURN_STR(BTIF_AV_CONNECT_REQ_EVT)
        CASE_RETURN_STR(BTIF_AV_DISCONNECT_REQ_EVT)
        CASE_RETURN_STR(BTIF_AV_START_STREAM_REQ_EVT)
        CASE_RETURN_STR(BTIF_AV_STOP_STREAM_REQ_EVT)
        CASE_RETURN_STR(BTIF_AV_SUSPEND_STREAM_REQ_EVT)
        CASE_RETURN_STR(BTIF_AV_SINK_CONFIG_REQ_EVT)
    default: return "UNKNOWN_EVENT";
    }
}

/****************************************************************************
**  Local helper functions
*****************************************************************************/

/*****************************************************************************
**  Static functions
******************************************************************************/
static void btif_report_connection_state(esp_a2d_connection_state_t state, bt_bdaddr_t *bd_addr)
{
    esp_a2d_cb_param_t param;
    memset(&param, 0, sizeof(esp_a2d_cb_param_t));
    
    param.conn_stat.state = state;
    if (bd_addr) {
        memcpy(param.conn_stat.remote_bda, bd_addr, sizeof(esp_bd_addr_t));
    }
    BTIF_A2D_CB_TO_APP(ESP_A2D_CONNECTION_STATE_EVT, &param);
}

static void btif_report_audio_state(esp_a2d_audio_state_t state, bt_bdaddr_t *bd_addr)
{
    esp_a2d_cb_param_t param;
    memset(&param, 0, sizeof(esp_a2d_cb_param_t));

    param.audio_stat.state = state;
    if (bd_addr) {
        memcpy(param.audio_stat.remote_bda, bd_addr, sizeof(esp_bd_addr_t));
    }
    BTIF_A2D_CB_TO_APP(ESP_A2D_AUDIO_STATE_EVT, &param);
}

/*****************************************************************************
**
** Function     btif_av_state_idle_handler
**
** Description  State managing disconnected AV link
**
** Returns      TRUE if event was processed, FALSE otherwise
**
*******************************************************************************/

static BOOLEAN btif_av_state_idle_handler(btif_sm_event_t event, void *p_data)
{
    BTIF_TRACE_DEBUG("%s event:%s flags %x\n", __FUNCTION__,
                     dump_av_sm_event_name(event), btif_av_cb.flags);

    switch (event) {
    case BTIF_SM_ENTER_EVT:
        /* clear the peer_bda */
        memset(&btif_av_cb.peer_bda, 0, sizeof(bt_bdaddr_t));
        btif_av_cb.flags = 0;
        btif_av_cb.edr = 0;
        btif_a2dp_on_idle();
        break;

    case BTIF_SM_EXIT_EVT:
        break;

    case BTA_AV_ENABLE_EVT:
        break;

    case BTA_AV_REGISTER_EVT:
        btif_av_cb.bta_handle = ((tBTA_AV *)p_data)->registr.hndl;
        break;

    case BTA_AV_PENDING_EVT:
    case BTIF_AV_CONNECT_REQ_EVT: {
        if (event == BTIF_AV_CONNECT_REQ_EVT) {
            memcpy(&btif_av_cb.peer_bda, ((btif_av_connect_req_t *)p_data)->target_bda,
                   sizeof(bt_bdaddr_t));
            BTA_AvOpen(btif_av_cb.peer_bda.address, btif_av_cb.bta_handle,
                       TRUE, BTA_SEC_AUTHENTICATE, ((btif_av_connect_req_t *)p_data)->uuid);
        } else if (event == BTA_AV_PENDING_EVT) {
            bdcpy(btif_av_cb.peer_bda.address, ((tBTA_AV *)p_data)->pend.bd_addr);
            BTA_AvOpen(btif_av_cb.peer_bda.address, btif_av_cb.bta_handle,
                       TRUE, BTA_SEC_AUTHENTICATE, UUID_SERVCLASS_AUDIO_SOURCE);
        }
        btif_sm_change_state(btif_av_cb.sm_handle, BTIF_AV_STATE_OPENING);
    } break;

    case BTA_AV_RC_OPEN_EVT:
    case BTA_AV_REMOTE_CMD_EVT:
    case BTA_AV_VENDOR_CMD_EVT:
    case BTA_AV_META_MSG_EVT:
    case BTA_AV_RC_FEAT_EVT:
    case BTA_AV_REMOTE_RSP_EVT:
    case BTA_AV_RC_CLOSE_EVT:
        BTIF_TRACE_WARNING("%s : unhandled RC event:%s\n", __FUNCTION__,
                           dump_av_sm_event_name(event));
        break;
    default:
        BTIF_TRACE_WARNING("%s : unhandled event:%s\n", __FUNCTION__,
                           dump_av_sm_event_name(event));
        return FALSE;

    }

    return TRUE;
}
/*****************************************************************************
**
** Function        btif_av_state_opening_handler
**
** Description     Intermediate state managing events during establishment
**                 of avdtp channel
**
** Returns         TRUE if event was processed, FALSE otherwise
**
*******************************************************************************/

static BOOLEAN btif_av_state_opening_handler(btif_sm_event_t event, void *p_data)
{
    BTIF_TRACE_DEBUG("%s event:%s flags %x\n", __FUNCTION__,
                     dump_av_sm_event_name(event), btif_av_cb.flags);

    switch (event) {
    case BTIF_SM_ENTER_EVT:
        /* inform the application that we are entering connecting state */
        btif_report_connection_state(ESP_A2D_CONNECTION_STATE_CONNECTING, &(btif_av_cb.peer_bda));
        break;

    case BTIF_SM_EXIT_EVT:
        break;

    case BTA_AV_REJECT_EVT:
        BTIF_TRACE_DEBUG(" Received  BTA_AV_REJECT_EVT \n");
        btif_report_connection_state(ESP_A2D_CONNECTION_STATE_DISCONNECTED, &(btif_av_cb.peer_bda));
        btif_sm_change_state(btif_av_cb.sm_handle, BTIF_AV_STATE_IDLE);
        break;

    case BTA_AV_OPEN_EVT: {
        tBTA_AV *p_bta_data = (tBTA_AV *)p_data;
        esp_a2d_connection_state_t state;
        btif_sm_state_t av_state;
        BTIF_TRACE_DEBUG("status:%d, edr 0x%x\n", p_bta_data->open.status,
                         p_bta_data->open.edr);

        if (p_bta_data->open.status == BTA_AV_SUCCESS) {
            state = ESP_A2D_CONNECTION_STATE_CONNECTED;
            av_state = BTIF_AV_STATE_OPENED;
            btif_av_cb.edr = p_bta_data->open.edr;

            btif_av_cb.peer_sep = p_bta_data->open.sep;
            btif_a2dp_set_peer_sep(p_bta_data->open.sep);
        } else {
            BTIF_TRACE_WARNING("BTA_AV_OPEN_EVT::FAILED status: %d\n",
                               p_bta_data->open.status );
            state = ESP_A2D_CONNECTION_STATE_DISCONNECTED;
            av_state  = BTIF_AV_STATE_IDLE;
        }

        /* inform the application of the event */
        btif_report_connection_state(state, &(btif_av_cb.peer_bda));
        /* change state to open/idle based on the status */
        btif_sm_change_state(btif_av_cb.sm_handle, av_state);
#if (BTIF_AV_SRC_INCLUDED == TRUE)
        if (btif_av_cb.peer_sep == AVDT_TSEP_SNK) {
            /* if queued PLAY command,  send it now */
            btif_rc_check_handle_pending_play(p_bta_data->open.bd_addr,
                                              (p_bta_data->open.status == BTA_AV_SUCCESS));
        } else if (btif_av_cb.peer_sep == AVDT_TSEP_SRC) {
            /* if queued PLAY command,  send it now */
            btif_rc_check_handle_pending_play(p_bta_data->open.bd_addr, FALSE);
            /* Bring up AVRCP connection too */
            BTA_AvOpenRc(btif_av_cb.bta_handle);
        }
#endif
        btif_queue_advance();
    } break;

    case BTIF_AV_SINK_CONFIG_REQ_EVT: {
        if (btif_av_cb.peer_sep == AVDT_TSEP_SRC) {
            esp_a2d_cb_param_t param;
            memcpy(param.audio_cfg.remote_bda, &btif_av_cb.peer_bda, sizeof(esp_bd_addr_t));
            memcpy(&param.audio_cfg.mcc, p_data, sizeof(esp_a2d_mcc_t));
            BTIF_A2D_CB_TO_APP(ESP_A2D_AUDIO_CFG_EVT, &param);
        }
    } break;

    case BTIF_AV_CONNECT_REQ_EVT:
        // Check for device, if same device which moved to opening then ignore callback
        if (memcmp ((bt_bdaddr_t *)p_data, &(btif_av_cb.peer_bda),
                    sizeof(btif_av_cb.peer_bda)) == 0) {
            BTIF_TRACE_DEBUG("%s: Same device moved to Opening state,ignore Connect Req\n", __func__);
            btif_queue_advance();
            break;
        } else {
            BTIF_TRACE_DEBUG("%s: Moved from idle by Incoming Connection request\n", __func__);
            btif_report_connection_state(ESP_A2D_CONNECTION_STATE_DISCONNECTED, (bt_bdaddr_t *)p_data);
            btif_queue_advance();
            break;
        }

    case BTA_AV_PENDING_EVT:
        // Check for device, if same device which moved to opening then ignore callback
        if (memcmp (((tBTA_AV *)p_data)->pend.bd_addr, &(btif_av_cb.peer_bda),
                    sizeof(btif_av_cb.peer_bda)) == 0) {
            BTIF_TRACE_DEBUG("%s: Same device moved to Opening state,ignore Pending Req\n", __func__);
            break;
        } else {
            BTIF_TRACE_DEBUG("%s: Moved from idle by outgoing Connection request\n", __func__);
            BTA_AvDisconnect(((tBTA_AV *)p_data)->pend.bd_addr);
            break;
        }

        CHECK_RC_EVENT(event, p_data);

    default:
        BTIF_TRACE_WARNING("%s : unhandled event:%s\n", __FUNCTION__,
                           dump_av_sm_event_name(event));
        return FALSE;

    }
    return TRUE;
}


/*****************************************************************************
**
** Function        btif_av_state_closing_handler
**
** Description     Intermediate state managing events during closing
**                 of avdtp channel
**
** Returns         TRUE if event was processed, FALSE otherwise
**
*******************************************************************************/

static BOOLEAN btif_av_state_closing_handler(btif_sm_event_t event, void *p_data)
{
    BTIF_TRACE_DEBUG("%s event:%s flags %x\n", __FUNCTION__,
                     dump_av_sm_event_name(event), btif_av_cb.flags);

    switch (event) {
    case BTIF_SM_ENTER_EVT:
        if (btif_av_cb.peer_sep == AVDT_TSEP_SRC) {
            btif_a2dp_set_rx_flush(TRUE);
        }
        break;

    case BTA_AV_STOP_EVT:
    case BTIF_AV_STOP_STREAM_REQ_EVT:
        if (btif_av_cb.peer_sep == AVDT_TSEP_SRC) {
            btif_a2dp_set_rx_flush(TRUE);
        }

        btif_a2dp_on_stopped(NULL);
        break;

    case BTIF_SM_EXIT_EVT:
        break;

    case BTA_AV_CLOSE_EVT:

        /* inform the application that we are disconnecting */
        btif_report_connection_state(ESP_A2D_CONNECTION_STATE_DISCONNECTED, &(btif_av_cb.peer_bda));

        btif_sm_change_state(btif_av_cb.sm_handle, BTIF_AV_STATE_IDLE);
        break;

    /* Handle the RC_CLOSE event for the cleanup */
    case BTA_AV_RC_CLOSE_EVT:
        BTIF_TRACE_WARNING("%s : unhandled RC event:%s\n", __FUNCTION__,
                           dump_av_sm_event_name(event));
        break;

    default:
        BTIF_TRACE_WARNING("%s : unhandled event:%s\n", __FUNCTION__,
                           dump_av_sm_event_name(event));
        return FALSE;
    }
    return TRUE;
}


/*****************************************************************************
**
** Function     btif_av_state_opened_handler
**
** Description  Handles AV events while AVDTP is in OPEN state
**
** Returns      TRUE if event was processed, FALSE otherwise
**
*******************************************************************************/

static BOOLEAN btif_av_state_opened_handler(btif_sm_event_t event, void *p_data)
{
    tBTA_AV *p_av = (tBTA_AV *)p_data;

    BTIF_TRACE_DEBUG("%s event:%s flags %x\n", __FUNCTION__,
                     dump_av_sm_event_name(event), btif_av_cb.flags);

    if ( (event == BTA_AV_REMOTE_CMD_EVT) && (btif_av_cb.flags & BTIF_AV_FLAG_REMOTE_SUSPEND) &&
            (p_av->remote_cmd.rc_id == BTA_AV_RC_PLAY) ) {
        BTIF_TRACE_EVENT("%s: Resetting remote suspend flag on RC PLAY\n", __FUNCTION__);
        btif_av_cb.flags &= ~BTIF_AV_FLAG_REMOTE_SUSPEND;
    }

    switch (event) {
    case BTIF_SM_ENTER_EVT:
        btif_av_cb.flags &= ~BTIF_AV_FLAG_PENDING_STOP;
        btif_av_cb.flags &= ~BTIF_AV_FLAG_PENDING_START;
        break;

    case BTIF_SM_EXIT_EVT:
        btif_av_cb.flags &= ~BTIF_AV_FLAG_PENDING_START;
        break;

    case BTIF_AV_START_STREAM_REQ_EVT:
        BTA_AvStart();
        btif_av_cb.flags |= BTIF_AV_FLAG_PENDING_START;
        break;

    case BTA_AV_START_EVT: {
        BTIF_TRACE_EVENT("BTA_AV_START_EVT status %d, suspending %d, init %d\n",
                         p_av->start.status, p_av->start.suspending, p_av->start.initiator);

        if ((p_av->start.status == BTA_SUCCESS) && (p_av->start.suspending == TRUE)) {
            return TRUE;
        }

        /* remain in open state if status failed */
        if (p_av->start.status != BTA_AV_SUCCESS) {
            return FALSE;
        }

        if (btif_av_cb.peer_sep == AVDT_TSEP_SRC) {
            btif_a2dp_set_rx_flush(FALSE); /*  remove flush state, ready for streaming*/
        }

        /* change state to started, send acknowledgement if start is pending */
        if (btif_av_cb.flags & BTIF_AV_FLAG_PENDING_START) {

            /* pending start flag will be cleared when exit current state */
        }
        btif_sm_change_state(btif_av_cb.sm_handle, BTIF_AV_STATE_STARTED);

    } break;

    case BTIF_AV_DISCONNECT_REQ_EVT:
        BTA_AvClose(btif_av_cb.bta_handle);

        /* inform the application that we are disconnecting */
        btif_report_connection_state(ESP_A2D_CONNECTION_STATE_DISCONNECTING, &(btif_av_cb.peer_bda));
        break;

    case BTA_AV_CLOSE_EVT:
        /* avdtp link is closed */
        btif_a2dp_on_stopped(NULL);

        /* inform the application that we are disconnected */
        btif_report_connection_state(ESP_A2D_CONNECTION_STATE_DISCONNECTED, &(btif_av_cb.peer_bda));

        /* change state to idle, send acknowledgement if start is pending */
        if (btif_av_cb.flags & BTIF_AV_FLAG_PENDING_START) {
            /* pending start flag will be cleared when exit current state */
        }
        btif_sm_change_state(btif_av_cb.sm_handle, BTIF_AV_STATE_IDLE);
        break;

    case BTA_AV_RECONFIG_EVT:
        if ((btif_av_cb.flags & BTIF_AV_FLAG_PENDING_START) &&
                (p_av->reconfig.status == BTA_AV_SUCCESS)) {
            APPL_TRACE_WARNING("reconfig done BTA_AVstart()\n");
            BTA_AvStart();
        } else if (btif_av_cb.flags & BTIF_AV_FLAG_PENDING_START) {
            btif_av_cb.flags &= ~BTIF_AV_FLAG_PENDING_START;
        }
        break;

    case BTIF_AV_CONNECT_REQ_EVT:
        if (memcmp ((bt_bdaddr_t *)p_data, &(btif_av_cb.peer_bda),
                    sizeof(btif_av_cb.peer_bda)) == 0) {
            BTIF_TRACE_DEBUG("%s: Ignore BTIF_AV_CONNECT_REQ_EVT for same device\n", __func__);
        } else {
            BTIF_TRACE_DEBUG("%s: Moved to opened by Other Incoming Conn req\n", __func__);
            btif_report_connection_state(ESP_A2D_CONNECTION_STATE_DISCONNECTED,
                                         (bt_bdaddr_t *)p_data);
        }
        btif_queue_advance();
        break;

        CHECK_RC_EVENT(event, p_data);

    default:
        BTIF_TRACE_WARNING("%s : unhandled event:%s\n", __FUNCTION__,
                           dump_av_sm_event_name(event));
        return FALSE;

    }
    return TRUE;
}

/*****************************************************************************
**
** Function     btif_av_state_started_handler
**
** Description  Handles AV events while A2DP stream is started
**
** Returns      TRUE if event was processed, FALSE otherwise
**
*******************************************************************************/

static BOOLEAN btif_av_state_started_handler(btif_sm_event_t event, void *p_data)
{
    tBTA_AV *p_av = (tBTA_AV *)p_data;

    BTIF_TRACE_DEBUG("%s event:%s flags %x\n", __FUNCTION__,
                     dump_av_sm_event_name(event), btif_av_cb.flags);

    switch (event) {
    case BTIF_SM_ENTER_EVT:

        /* we are again in started state, clear any remote suspend flags */
        btif_av_cb.flags &= ~BTIF_AV_FLAG_REMOTE_SUSPEND;

        btif_report_audio_state(ESP_A2D_AUDIO_STATE_STARTED, &(btif_av_cb.peer_bda));

        /* increase the a2dp consumer task priority temporarily when start
        ** audio playing, to avoid overflow the audio packet queue. */
        adjust_priority_a2dp(TRUE);

        break;

    case BTIF_SM_EXIT_EVT:
        /* restore the a2dp consumer task priority when stop audio playing. */
        adjust_priority_a2dp(FALSE);

        break;

    case BTIF_AV_START_STREAM_REQ_EVT:
        break;

    /* fixme -- use suspend = true always to work around issue with BTA AV */
    case BTIF_AV_STOP_STREAM_REQ_EVT:
    case BTIF_AV_SUSPEND_STREAM_REQ_EVT:

        /* set pending flag to ensure btif task is not trying to restart
           stream while suspend is in progress */
        btif_av_cb.flags |= BTIF_AV_FLAG_LOCAL_SUSPEND_PENDING;

        /* if we were remotely suspended but suspend locally, local suspend
           always overrides */
        btif_av_cb.flags &= ~BTIF_AV_FLAG_REMOTE_SUSPEND;

        if (btif_av_cb.peer_sep == AVDT_TSEP_SRC) {
            btif_a2dp_set_rx_flush(TRUE);
            btif_a2dp_on_stopped(NULL);
        }

        BTA_AvStop(TRUE);
        break;

    case BTIF_AV_DISCONNECT_REQ_EVT:

        /* request avdtp to close */
        BTA_AvClose(btif_av_cb.bta_handle);

        /* inform the application that we are disconnecting */
        btif_report_connection_state(ESP_A2D_CONNECTION_STATE_DISCONNECTING, &(btif_av_cb.peer_bda));

        /* wait in closing state until fully closed */
        btif_sm_change_state(btif_av_cb.sm_handle, BTIF_AV_STATE_CLOSING);
        break;

    case BTA_AV_SUSPEND_EVT:

        BTIF_TRACE_EVENT("BTA_AV_SUSPEND_EVT status %d, init %d\n",
                         p_av->suspend.status, p_av->suspend.initiator);

        /* a2dp suspended, stop media task until resumed */
        btif_a2dp_on_suspended(&p_av->suspend);

        /* if not successful, remain in current state */
        if (p_av->suspend.status != BTA_AV_SUCCESS) {
            btif_av_cb.flags &= ~BTIF_AV_FLAG_LOCAL_SUSPEND_PENDING;

            return FALSE;
        }

        if (p_av->suspend.initiator != TRUE) {
            /* remote suspend, notify HAL and await audioflinger to
               suspend/stop stream */

            /* set remote suspend flag to block media task from restarting
               stream only if we did not already initiate a local suspend */
            if ((btif_av_cb.flags & BTIF_AV_FLAG_LOCAL_SUSPEND_PENDING) == 0) {
                btif_av_cb.flags |= BTIF_AV_FLAG_REMOTE_SUSPEND;
            }

            btif_report_audio_state(ESP_A2D_AUDIO_STATE_REMOTE_SUSPEND, &(btif_av_cb.peer_bda));
        } else {
            btif_report_audio_state(ESP_A2D_AUDIO_STATE_STOPPED, &(btif_av_cb.peer_bda));
        }

        btif_sm_change_state(btif_av_cb.sm_handle, BTIF_AV_STATE_OPENED);

        /* suspend completed and state changed, clear pending status */
        btif_av_cb.flags &= ~BTIF_AV_FLAG_LOCAL_SUSPEND_PENDING;
        break;

    case BTA_AV_STOP_EVT:

        btif_av_cb.flags |= BTIF_AV_FLAG_PENDING_STOP;
        btif_a2dp_on_stopped(&p_av->suspend);

        btif_report_audio_state(ESP_A2D_AUDIO_STATE_STOPPED, &(btif_av_cb.peer_bda));

        /* if stop was successful, change state to open */
        if (p_av->suspend.status == BTA_AV_SUCCESS) {
            btif_sm_change_state(btif_av_cb.sm_handle, BTIF_AV_STATE_OPENED);
        }

        break;

    case BTA_AV_CLOSE_EVT:

        btif_av_cb.flags |= BTIF_AV_FLAG_PENDING_STOP;

        /* avdtp link is closed */
        btif_a2dp_on_stopped(NULL);

        /* inform the application that we are disconnected */
        btif_report_connection_state(ESP_A2D_CONNECTION_STATE_DISCONNECTED, &(btif_av_cb.peer_bda));

        btif_sm_change_state(btif_av_cb.sm_handle, BTIF_AV_STATE_IDLE);
        break;

        CHECK_RC_EVENT(event, p_data);

    default:
        BTIF_TRACE_WARNING("%s : unhandled event:%s\n", __FUNCTION__,
                           dump_av_sm_event_name(event));
        return FALSE;

    }
    return TRUE;
}

/*****************************************************************************
**  Local event handlers
******************************************************************************/

static void btif_av_handle_event(UINT16 event, char *p_param)
{
    btif_sm_dispatch(btif_av_cb.sm_handle, event, (void *)p_param);
    btif_av_event_free_data(event, p_param);
}

void btif_av_event_deep_copy(UINT16 event, char *p_dest, char *p_src)
{
    tBTA_AV *av_src = (tBTA_AV *)p_src;
    tBTA_AV *av_dest = (tBTA_AV *)p_dest;

    // First copy the structure
    memcpy(p_dest, p_src, sizeof(tBTA_AV));

    switch (event) {
    case BTA_AV_META_MSG_EVT:
        if (av_src->meta_msg.p_data && av_src->meta_msg.len) {
            av_dest->meta_msg.p_data = osi_calloc(av_src->meta_msg.len);
            assert(av_dest->meta_msg.p_data);
            memcpy(av_dest->meta_msg.p_data, av_src->meta_msg.p_data, av_src->meta_msg.len);
        }

        if (av_src->meta_msg.p_msg) {
            av_dest->meta_msg.p_msg = osi_calloc(sizeof(tAVRC_MSG));
            assert(av_dest->meta_msg.p_msg);
            memcpy(av_dest->meta_msg.p_msg, av_src->meta_msg.p_msg, sizeof(tAVRC_MSG));

            if (av_src->meta_msg.p_msg->vendor.p_vendor_data &&
                    av_src->meta_msg.p_msg->vendor.vendor_len) {
                av_dest->meta_msg.p_msg->vendor.p_vendor_data = osi_calloc(
                            av_src->meta_msg.p_msg->vendor.vendor_len);
                assert(av_dest->meta_msg.p_msg->vendor.p_vendor_data);
                memcpy(av_dest->meta_msg.p_msg->vendor.p_vendor_data,
                       av_src->meta_msg.p_msg->vendor.p_vendor_data,
                       av_src->meta_msg.p_msg->vendor.vendor_len);
            }
        }
        break;

    default:
        break;
    }
}

static void btif_av_event_free_data(btif_sm_event_t event, void *p_data)
{
    switch (event) {
    case BTA_AV_META_MSG_EVT: {
        tBTA_AV *av = (tBTA_AV *)p_data;
        if (av->meta_msg.p_data) {
            osi_free(av->meta_msg.p_data);
        }

        if (av->meta_msg.p_msg) {
            if (av->meta_msg.p_msg->vendor.p_vendor_data) {
                osi_free(av->meta_msg.p_msg->vendor.p_vendor_data);
            }
            osi_free(av->meta_msg.p_msg);
        }
    }
    break;

    default:
        break;
    }
}

static void bte_av_callback(tBTA_AV_EVT event, tBTA_AV *p_data)
{
    btif_transfer_context(btif_av_handle_event, event,
                          (char *)p_data, sizeof(tBTA_AV), btif_av_event_deep_copy);
}

static void bte_av_media_callback(tBTA_AV_EVT event, tBTA_AV_MEDIA *p_data)
{
    btif_sm_state_t state;
    UINT8 que_len;
    tA2D_STATUS a2d_status;
    tA2D_SBC_CIE sbc_cie;
    esp_a2d_mcc_t mcc;

    if (event == BTA_AV_MEDIA_DATA_EVT) { /* Switch to BTIF_MEDIA context */
        state = btif_sm_get_state(btif_av_cb.sm_handle);
        if ( (state == BTIF_AV_STATE_STARTED) || /* send SBC packets only in Started State */
                (state == BTIF_AV_STATE_OPENED) ) {
            que_len = btif_media_sink_enque_buf((BT_HDR *)p_data);
            BTIF_TRACE_DEBUG(" Packets in Que %d\n", que_len);
        } else {
            return;
        }
    }

    if (event == BTA_AV_MEDIA_SINK_CFG_EVT) {
        /* send a command to BT Media Task */
        btif_reset_decoder((UINT8 *)p_data);

	/* currently only supportes SBC */
        a2d_status = A2D_ParsSbcInfo(&sbc_cie, (UINT8 *)p_data, FALSE);
        if (a2d_status == A2D_SUCCESS) {
            mcc.type = ESP_A2D_MCT_SBC;
            memcpy(&mcc.cie, (uint8_t *)p_data + 3, ESP_A2D_CIE_LEN_SBC);
            /* Switch to BTIF context */
            btif_transfer_context(btif_av_handle_event, BTIF_AV_SINK_CONFIG_REQ_EVT,
                                  (char *)&mcc, sizeof(mcc), NULL);
        } else {
            APPL_TRACE_ERROR("ERROR dump_codec_info A2D_ParsSbcInfo fail:%d\n", a2d_status);
        }
    }
}
/*******************************************************************************
**
** Function         btif_av_init
**
** Description      Initializes btif AV if not already done
**
** Returns          bt_status_t
**
*******************************************************************************/

bt_status_t btif_av_init()
{
    if (btif_av_cb.sm_handle == NULL) {
        if (!btif_a2dp_start_media_task()) {
            return BT_STATUS_FAIL;
        }

        /* Also initialize the AV state machine */
        btif_av_cb.sm_handle =
            btif_sm_init((const btif_sm_handler_t *)btif_av_state_handlers, BTIF_AV_STATE_IDLE);

        btif_enable_service(BTA_A2DP_SOURCE_SERVICE_ID);
#if (BTA_AV_SINK_INCLUDED == TRUE)
        btif_enable_service(BTA_A2DP_SINK_SERVICE_ID);
#endif

        btif_a2dp_on_init();
    }

    return BT_STATUS_SUCCESS;
}

/**
 *
 * Function         register A2DP callback
 *
 * Description      Initializes the AV interface for sink mode
 *
 * Returns          bt_status_t
 *
 */
esp_err_t esp_a2d_register_callback(esp_profile_cb_t callback)
{
    // TODO: need protection against race
    bt_av_sink_callback = callback;
}

/*******************************************************************************
**
** Function         init_sink
**
** Description      Initializes the AV interface for sink mode
**
** Returns          bt_status_t
**
*******************************************************************************/
esp_err_t esp_a2d_sink_init(void)
{
    BTIF_TRACE_EVENT("%s()\n", __func__);

    bt_status_t status = btif_av_init();

    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

/*******************************************************************************
**
** Function         connect
**
** Description      Establishes the AV signalling channel with the remote headset
**
** Returns          bt_status_t
**
*******************************************************************************/

static bt_status_t connect_int(bt_bdaddr_t *bd_addr, uint16_t uuid)
{
    btif_av_connect_req_t connect_req;
    connect_req.target_bda = bd_addr;
    connect_req.uuid = uuid;
    BTIF_TRACE_EVENT("%s\n", __FUNCTION__);

    btif_sm_dispatch(btif_av_cb.sm_handle, BTIF_AV_CONNECT_REQ_EVT, (char *)&connect_req);

    return BT_STATUS_SUCCESS;
}

esp_err_t esp_a2d_sink_connect(esp_bd_addr_t remote_bda)
{
    BTIF_TRACE_EVENT("%s\n", __FUNCTION__);
    CHECK_BTAV_INIT();

    bt_status_t stat;
    bt_bdaddr_t bd_addr;
    memcpy(&bd_addr, remote_bda, sizeof(bt_bdaddr_t));

    stat = btif_queue_connect(UUID_SERVCLASS_AUDIO_SINK, &bd_addr, connect_int);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_a2d_sink_disconnect(esp_bd_addr_t remote_bda)
{
    bt_status_t stat;
    bt_bdaddr_t bd_addr;
    BTIF_TRACE_EVENT("%s\n", __FUNCTION__);
    CHECK_BTAV_INIT();
    memcpy(&bd_addr, remote_bda, sizeof(bt_bdaddr_t));
    /* Switch to BTIF context */
    stat = btif_transfer_context(btif_av_handle_event, BTIF_AV_DISCONNECT_REQ_EVT,
                                 (char *)(&bd_addr), sizeof(bt_bdaddr_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}
/*******************************************************************************
**
** Function         cleanup
**
** Description      Shuts down the AV interface and does the cleanup
**
** Returns          None
**
*******************************************************************************/
static void cleanup(void)
{
    BTIF_TRACE_EVENT("%s\n", __FUNCTION__);

    btif_a2dp_stop_media_task();

    btif_disable_service(BTA_A2DP_SOURCE_SERVICE_ID);
#if (BTA_AV_SINK_INCLUDED == TRUE)
    btif_disable_service(BTA_A2DP_SINK_SERVICE_ID);
#endif

    /* Also shut down the AV state machine */
    btif_sm_shutdown(btif_av_cb.sm_handle);
    btif_av_cb.sm_handle = NULL;
}

void esp_a2d_sink_deinit(void)
{
    BTIF_TRACE_EVENT("%s\n", __FUNCTION__);

    if (bt_av_sink_callback) {
        bt_av_sink_callback = NULL;
        cleanup();
    }
}

/*******************************************************************************
**
** Function         btif_av_get_sm_handle
**
** Description      Fetches current av SM handle
**
** Returns          None
**
*******************************************************************************/

btif_sm_handle_t btif_av_get_sm_handle(void)
{
    return btif_av_cb.sm_handle;
}

/*******************************************************************************
**
** Function         btif_av_stream_ready
**
** Description      Checks whether AV is ready for starting a stream
**
** Returns          None
**
*******************************************************************************/

BOOLEAN btif_av_stream_ready(void)
{
    btif_sm_state_t state = btif_sm_get_state(btif_av_cb.sm_handle);

    BTIF_TRACE_DEBUG("btif_av_stream_ready : sm hdl %d, state %d, flags %x\n",
                     (int)btif_av_cb.sm_handle, state, btif_av_cb.flags);

    /* also make sure main adapter is enabled */
    if (btif_is_enabled() == 0) {
        BTIF_TRACE_EVENT("main adapter not enabled");
        return FALSE;
    }

    /* check if we are remotely suspended or stop is pending */
    if (btif_av_cb.flags & (BTIF_AV_FLAG_REMOTE_SUSPEND | BTIF_AV_FLAG_PENDING_STOP)) {
        return FALSE;
    }

    return (state == BTIF_AV_STATE_OPENED);
}

/*******************************************************************************
**
** Function         btif_av_stream_started_ready
**
** Description      Checks whether AV ready for media start in streaming state
**
** Returns          None
**
*******************************************************************************/

BOOLEAN btif_av_stream_started_ready(void)
{
    btif_sm_state_t state = btif_sm_get_state(btif_av_cb.sm_handle);

    BTIF_TRACE_DEBUG("btif_av_stream_started : sm hdl %d, state %d, flags %x\n",
                     (int)btif_av_cb.sm_handle, state, btif_av_cb.flags);

    /* disallow media task to start if we have pending actions */
    if (btif_av_cb.flags & (BTIF_AV_FLAG_LOCAL_SUSPEND_PENDING | BTIF_AV_FLAG_REMOTE_SUSPEND
                            | BTIF_AV_FLAG_PENDING_STOP)) {
        return FALSE;
    }

    return (state == BTIF_AV_STATE_STARTED);
}

/*******************************************************************************
**
** Function         btif_dispatch_sm_event
**
** Description      Send event to AV statemachine
**
** Returns          None
**
*******************************************************************************/

/* used to pass events to AV statemachine from other tasks */
void btif_dispatch_sm_event(btif_av_sm_event_t event, void *p_data, int len)
{
    /* Switch to BTIF context */
    btif_transfer_context(btif_av_handle_event, event,
                          (char *)p_data, len, NULL);
}

/*******************************************************************************
**
** Function         btif_av_execute_service
**
** Description      Initializes/Shuts down the service
**
** Returns          BT_STATUS_SUCCESS on success, BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btif_av_execute_service(BOOLEAN b_enable)
{
    if (b_enable) {
        /* TODO: Removed BTA_SEC_AUTHORIZE since the Java/App does not
         * handle this request in order to allow incoming connections to succeed.
         * We need to put this back once support for this is added */

        /* Added BTA_AV_FEAT_NO_SCO_SSPD - this ensures that the BTA does not
         * auto-suspend av streaming on AG events(SCO or Call). The suspend shall
         * be initiated by the app/audioflinger layers */
        BTA_AvEnable(BTA_SEC_AUTHENTICATE, (BTA_AV_FEAT_NO_SCO_SSPD),
                     bte_av_callback);
        BTA_AvRegister(BTA_AV_CHNL_AUDIO, BTIF_AV_SERVICE_NAME, 0, bte_av_media_callback, &bta_av_a2d_cos);
    } else {
        BTA_AvDeregister(btif_av_cb.bta_handle);
        BTA_AvDisable();
    }
    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_av_sink_execute_service
**
** Description      Initializes/Shuts down the service
**
** Returns          BT_STATUS_SUCCESS on success, BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btif_av_sink_execute_service(BOOLEAN b_enable)
{
#if (BTA_AV_SINK_INCLUDED == TRUE)
    BTA_AvEnable_Sink(b_enable);
#endif
    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_av_is_connected
**
** Description      Checks if av has a connected sink
**
** Returns          BOOLEAN
**
*******************************************************************************/
BOOLEAN btif_av_is_connected(void)
{
    btif_sm_state_t state = btif_sm_get_state(btif_av_cb.sm_handle);
    return ((state == BTIF_AV_STATE_OPENED) || (state ==  BTIF_AV_STATE_STARTED));
}

/*******************************************************************************
**
** Function         btif_av_is_peer_edr
**
** Description      Check if the connected a2dp device supports
**                  EDR or not. Only when connected this function
**                  will accurately provide a true capability of
**                  remote peer. If not connected it will always be false.
**
** Returns          TRUE if remote device is capable of EDR
**
*******************************************************************************/
BOOLEAN btif_av_is_peer_edr(void)
{
    ASSERTC(btif_av_is_connected(), "No active a2dp connection\n", 0);

    if (btif_av_cb.edr) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/******************************************************************************
**
** Function        btif_av_clear_remote_suspend_flag
**
** Description     Clears btif_av_cd.flags if BTIF_AV_FLAG_REMOTE_SUSPEND is set
**
** Returns          void
******************************************************************************/
void btif_av_clear_remote_suspend_flag(void)
{
    BTIF_TRACE_DEBUG("%s: flag :%x\n", __func__, btif_av_cb.flags);
    btif_av_cb.flags &= ~BTIF_AV_FLAG_REMOTE_SUSPEND;
}

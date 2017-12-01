// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*****************************************************************************
 *
 *  Filename:      btc_avk.c
 *
 *  Description:   AV implementation
 *
 *****************************************************************************/
#include "bt_target.h"
#include <string.h>
#include "bt_trace.h"
#include "bt_defs.h"
#include "esp_bt_defs.h"
#include "esp_a2dp_api.h"
#include "allocator.h"
#include "btc_dm.h"
#include "btc_av.h"
#include "btc_avrc.h"
#include "btc_util.h"
#include "btc_profile_queue.h"
#include "bta_api.h"
#include "btc_media.h"
#include "bta_av_api.h"
#include "btu.h"
#include "bt_utils.h"
#include "btc_common.h"
#include "btc_manage.h"

#if BTC_AV_INCLUDED

/*****************************************************************************
**  Constants & Macros
******************************************************************************/
#define BTC_AV_SERVICE_NAME "Advanced Audio"

#define BTC_TIMEOUT_AV_OPEN_ON_RC_SECS  2

typedef enum {
    BTC_AV_STATE_IDLE = 0x0,
    BTC_AV_STATE_OPENING,
    BTC_AV_STATE_OPENED,
    BTC_AV_STATE_STARTED,
    BTC_AV_STATE_CLOSING
} btc_av_state_t;

/* Should not need dedicated suspend state as actual actions are no
   different than open state. Suspend flags are needed however to prevent
   media task from trying to restart stream during remote suspend or while
   we are in the process of a local suspend */

#define BTC_AV_FLAG_LOCAL_SUSPEND_PENDING 0x1
#define BTC_AV_FLAG_REMOTE_SUSPEND        0x2
#define BTC_AV_FLAG_PENDING_START         0x4
#define BTC_AV_FLAG_PENDING_STOP          0x8

/*****************************************************************************
**  Local type definitions
******************************************************************************/

typedef struct {
    tBTA_AV_HNDL bta_handle;
    bt_bdaddr_t peer_bda;
    btc_sm_handle_t sm_handle;
    UINT8 flags;
    tBTA_AV_EDR edr;
    UINT8   peer_sep;  /* sep type of peer device */
} btc_av_cb_t;

typedef struct {
    bt_bdaddr_t *target_bda;
    uint16_t uuid;
} btc_av_connect_req_t;

/*****************************************************************************
**  Static variables
******************************************************************************/
static btc_av_cb_t btc_av_cb = {0};
static TIMER_LIST_ENT tle_av_open_on_rc;

/* both interface and media task needs to be ready to alloc incoming request */
#define CHECK_BTAV_INIT() do \
{ \
    assert (btc_av_cb.sm_handle != NULL); \
} while (0)


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
         btc_rc_handler(e, d);\
    }break; \

static BOOLEAN btc_av_state_idle_handler(btc_sm_event_t event, void *data);
static BOOLEAN btc_av_state_opening_handler(btc_sm_event_t event, void *data);
static BOOLEAN btc_av_state_opened_handler(btc_sm_event_t event, void *data);
static BOOLEAN btc_av_state_started_handler(btc_sm_event_t event, void *data);
static BOOLEAN btc_av_state_closing_handler(btc_sm_event_t event, void *data);

static const btc_sm_handler_t btc_av_state_handlers[] = {
    btc_av_state_idle_handler,
    btc_av_state_opening_handler,
    btc_av_state_opened_handler,
    btc_av_state_started_handler,
    btc_av_state_closing_handler
};

static void btc_av_event_free_data(btc_sm_event_t event, void *p_data);

/*************************************************************************
** Extern functions
*************************************************************************/

extern tBTA_AV_CO_FUNCTS bta_av_a2d_cos;
/*****************************************************************************
** Local helper functions
******************************************************************************/
static inline void btc_a2d_cb_to_app(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param)
{
    esp_a2d_cb_t btc_a2d_cb = (esp_a2d_cb_t)btc_profile_cb_get(BTC_PID_A2DP);
    if (btc_a2d_cb) {
        btc_a2d_cb(event, param);
    }
}

UNUSED_ATTR static const char *dump_av_sm_state_name(btc_av_state_t state)
{
    switch (state) {
        CASE_RETURN_STR(BTC_AV_STATE_IDLE)
        CASE_RETURN_STR(BTC_AV_STATE_OPENING)
        CASE_RETURN_STR(BTC_AV_STATE_OPENED)
        CASE_RETURN_STR(BTC_AV_STATE_STARTED)
        CASE_RETURN_STR(BTC_AV_STATE_CLOSING)
    default: return "UNKNOWN_STATE";
    }
}

UNUSED_ATTR static const char *dump_av_sm_event_name(btc_av_sm_event_t event)
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
        CASE_RETURN_STR(BTC_SM_ENTER_EVT)
        CASE_RETURN_STR(BTC_SM_EXIT_EVT)
        CASE_RETURN_STR(BTC_AV_CONNECT_REQ_EVT)
        CASE_RETURN_STR(BTC_AV_DISCONNECT_REQ_EVT)
        CASE_RETURN_STR(BTC_AV_START_STREAM_REQ_EVT)
        CASE_RETURN_STR(BTC_AV_STOP_STREAM_REQ_EVT)
        CASE_RETURN_STR(BTC_AV_SUSPEND_STREAM_REQ_EVT)
        CASE_RETURN_STR(BTC_AV_SINK_CONFIG_REQ_EVT)
    default: return "UNKNOWN_EVENT";
    }
}

/****************************************************************************
**  Local helper functions
*****************************************************************************/
/*******************************************************************************
**
** Function         btc_initiate_av_open_tmr_hdlr
**
** Description      Timer to trigger AV open if the remote headset establishes
**                  RC connection w/o AV connection. The timer is needed to IOP
**                  with headsets that do establish AV after RC connection.
**
** Returns          void
**
*******************************************************************************/
static void btc_initiate_av_open_tmr_hdlr(TIMER_LIST_ENT *tle)
{
    BD_ADDR peer_addr;
    UNUSED(tle);
    btc_av_connect_req_t connect_req;
    UNUSED(tle);
    /* is there at least one RC connection - There should be */
    if (btc_rc_get_connected_peer(peer_addr)) {
        LOG_DEBUG("%s Issuing connect to the remote RC peer", __FUNCTION__);
        /* In case of AVRCP connection request, we will initiate SRC connection */
        connect_req.target_bda = (bt_bdaddr_t *)&peer_addr;
        connect_req.uuid = UUID_SERVCLASS_AUDIO_SOURCE;
        btc_sm_dispatch(btc_av_cb.sm_handle, BTC_AV_CONNECT_REQ_EVT, (char *)&connect_req);
    } else {
        LOG_ERROR("%s No connected RC peers", __FUNCTION__);
    }
}

/*****************************************************************************
**  Static functions
******************************************************************************/
static void btc_report_connection_state(esp_a2d_connection_state_t state, bt_bdaddr_t *bd_addr, int disc_rsn)
{
    esp_a2d_cb_param_t param;
    memset(&param, 0, sizeof(esp_a2d_cb_param_t));

    param.conn_stat.state = state;
    if (bd_addr) {
        memcpy(param.conn_stat.remote_bda, bd_addr, sizeof(esp_bd_addr_t));
    }
    if (state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
        param.conn_stat.disc_rsn = (disc_rsn == 0) ? ESP_A2D_DISC_RSN_NORMAL :
                                   ESP_A2D_DISC_RSN_ABNORMAL;
    }
    btc_a2d_cb_to_app(ESP_A2D_CONNECTION_STATE_EVT, &param);
}

static void btc_report_audio_state(esp_a2d_audio_state_t state, bt_bdaddr_t *bd_addr)
{
    esp_a2d_cb_param_t param;
    memset(&param, 0, sizeof(esp_a2d_cb_param_t));

    param.audio_stat.state = state;
    if (bd_addr) {
        memcpy(param.audio_stat.remote_bda, bd_addr, sizeof(esp_bd_addr_t));
    }
    btc_a2d_cb_to_app(ESP_A2D_AUDIO_STATE_EVT, &param);
}

/*****************************************************************************
**
** Function     btc_av_state_idle_handler
**
** Description  State managing disconnected AV link
**
** Returns      TRUE if event was processed, FALSE otherwise
**
*******************************************************************************/

static BOOLEAN btc_av_state_idle_handler(btc_sm_event_t event, void *p_data)
{
    LOG_DEBUG("%s event:%s flags %x\n", __FUNCTION__,
              dump_av_sm_event_name(event), btc_av_cb.flags);

    switch (event) {
    case BTC_SM_ENTER_EVT:
        /* clear the peer_bda */
        memset(&btc_av_cb.peer_bda, 0, sizeof(bt_bdaddr_t));
        btc_av_cb.flags = 0;
        btc_av_cb.edr = 0;
        btc_a2dp_on_idle();
        break;

    case BTC_SM_EXIT_EVT:
        break;

    case BTA_AV_ENABLE_EVT:
        break;

    case BTA_AV_REGISTER_EVT:
        btc_av_cb.bta_handle = ((tBTA_AV *)p_data)->registr.hndl;
        break;

    case BTA_AV_PENDING_EVT:
    case BTC_AV_CONNECT_REQ_EVT: {
        if (event == BTC_AV_CONNECT_REQ_EVT) {
            memcpy(&btc_av_cb.peer_bda, ((btc_av_connect_req_t *)p_data)->target_bda,
                   sizeof(bt_bdaddr_t));
            BTA_AvOpen(btc_av_cb.peer_bda.address, btc_av_cb.bta_handle,
                       TRUE, BTA_SEC_AUTHENTICATE, ((btc_av_connect_req_t *)p_data)->uuid);
        } else if (event == BTA_AV_PENDING_EVT) {
            bdcpy(btc_av_cb.peer_bda.address, ((tBTA_AV *)p_data)->pend.bd_addr);
            BTA_AvOpen(btc_av_cb.peer_bda.address, btc_av_cb.bta_handle,
                       TRUE, BTA_SEC_AUTHENTICATE, UUID_SERVCLASS_AUDIO_SOURCE);
        }
        btc_sm_change_state(btc_av_cb.sm_handle, BTC_AV_STATE_OPENING);
    } break;

    case BTA_AV_RC_OPEN_EVT:
        /* IOP_FIX: Jabra 620 only does RC open without AV open whenever it connects. So
         * as per the AV WP, an AVRC connection cannot exist without an AV connection. Therefore,
         * we initiate an AV connection if an RC_OPEN_EVT is received when we are in AV_CLOSED state.
         * We initiate the AV connection after a small 3s timeout to avoid any collisions from the
         * headsets, as some headsets initiate the AVRC connection first and then
         * immediately initiate the AV connection
         *
         * TODO: We may need to do this only on an AVRCP Play. FixMe
         */

        LOG_DEBUG("BTA_AV_RC_OPEN_EVT received w/o AV");
        memset(&tle_av_open_on_rc, 0, sizeof(tle_av_open_on_rc));
        tle_av_open_on_rc.param = (UINT32)btc_initiate_av_open_tmr_hdlr;
        btu_start_timer(&tle_av_open_on_rc, BTU_TTYPE_USER_FUNC,
                        BTC_TIMEOUT_AV_OPEN_ON_RC_SECS);
        btc_rc_handler(event, p_data);
        break;

    case BTA_AV_REMOTE_CMD_EVT:
    case BTA_AV_VENDOR_CMD_EVT:
    case BTA_AV_META_MSG_EVT:
    case BTA_AV_RC_FEAT_EVT:
    case BTA_AV_REMOTE_RSP_EVT:
        btc_rc_handler(event, (tBTA_AV *)p_data);
        break;

    case BTA_AV_RC_CLOSE_EVT:
        if (tle_av_open_on_rc.in_use) {
            LOG_DEBUG("BTA_AV_RC_CLOSE_EVT: Stopping AV timer.");
            btu_stop_timer(&tle_av_open_on_rc);
        }
        btc_rc_handler(event, p_data);
        break;

    default:
        LOG_WARN("%s : unhandled event:%s\n", __FUNCTION__,
                 dump_av_sm_event_name(event));
        return FALSE;

    }

    return TRUE;
}
/*****************************************************************************
**
** Function        btc_av_state_opening_handler
**
** Description     Intermediate state managing events during establishment
**                 of avdtp channel
**
** Returns         TRUE if event was processed, FALSE otherwise
**
*******************************************************************************/

static BOOLEAN btc_av_state_opening_handler(btc_sm_event_t event, void *p_data)
{
    LOG_DEBUG("%s event:%s flags %x\n", __FUNCTION__,
              dump_av_sm_event_name(event), btc_av_cb.flags);

    switch (event) {
    case BTC_SM_ENTER_EVT:
        /* inform the application that we are entering connecting state */
        btc_report_connection_state(ESP_A2D_CONNECTION_STATE_CONNECTING, &(btc_av_cb.peer_bda), 0);
        break;

    case BTC_SM_EXIT_EVT:
        break;

    case BTA_AV_REJECT_EVT:
        LOG_DEBUG(" Received  BTA_AV_REJECT_EVT \n");
        btc_report_connection_state(ESP_A2D_CONNECTION_STATE_DISCONNECTED, &(btc_av_cb.peer_bda), 0);
        btc_sm_change_state(btc_av_cb.sm_handle, BTC_AV_STATE_IDLE);
        break;

    case BTA_AV_OPEN_EVT: {
        tBTA_AV *p_bta_data = (tBTA_AV *)p_data;
        esp_a2d_connection_state_t state;
        btc_sm_state_t av_state;
        LOG_DEBUG("status:%d, edr 0x%x\n", p_bta_data->open.status,
                  p_bta_data->open.edr);

        if (p_bta_data->open.status == BTA_AV_SUCCESS) {
            state = ESP_A2D_CONNECTION_STATE_CONNECTED;
            av_state = BTC_AV_STATE_OPENED;
            btc_av_cb.edr = p_bta_data->open.edr;

            btc_av_cb.peer_sep = p_bta_data->open.sep;
            btc_a2dp_set_peer_sep(p_bta_data->open.sep);
        } else {
            LOG_WARN("BTA_AV_OPEN_EVT::FAILED status: %d\n",
                     p_bta_data->open.status );
            state = ESP_A2D_CONNECTION_STATE_DISCONNECTED;
            av_state  = BTC_AV_STATE_IDLE;
        }

        /* inform the application of the event */
        btc_report_connection_state(state, &(btc_av_cb.peer_bda), 0);
        /* change state to open/idle based on the status */
        btc_sm_change_state(btc_av_cb.sm_handle, av_state);
        if (btc_av_cb.peer_sep == AVDT_TSEP_SRC) {
            /* Bring up AVRCP connection too */
            BTA_AvOpenRc(btc_av_cb.bta_handle);
        }
        btc_queue_advance();
    } break;

    case BTC_AV_SINK_CONFIG_REQ_EVT: {
        if (btc_av_cb.peer_sep == AVDT_TSEP_SRC) {
            esp_a2d_cb_param_t param;
            memcpy(param.audio_cfg.remote_bda, &btc_av_cb.peer_bda, sizeof(esp_bd_addr_t));
            memcpy(&param.audio_cfg.mcc, p_data, sizeof(esp_a2d_mcc_t));
            btc_a2d_cb_to_app(ESP_A2D_AUDIO_CFG_EVT, &param);
        }
    } break;

    case BTC_AV_CONNECT_REQ_EVT:
        // Check for device, if same device which moved to opening then ignore callback
        if (memcmp ((bt_bdaddr_t *)p_data, &(btc_av_cb.peer_bda),
                    sizeof(btc_av_cb.peer_bda)) == 0) {
            LOG_DEBUG("%s: Same device moved to Opening state,ignore Connect Req\n", __func__);
            btc_queue_advance();
            break;
        } else {
            LOG_DEBUG("%s: Moved from idle by Incoming Connection request\n", __func__);
            btc_report_connection_state(ESP_A2D_CONNECTION_STATE_DISCONNECTED, (bt_bdaddr_t *)p_data, 0);
            btc_queue_advance();
            break;
        }

    case BTA_AV_PENDING_EVT:
        // Check for device, if same device which moved to opening then ignore callback
        if (memcmp (((tBTA_AV *)p_data)->pend.bd_addr, &(btc_av_cb.peer_bda),
                    sizeof(btc_av_cb.peer_bda)) == 0) {
            LOG_DEBUG("%s: Same device moved to Opening state,ignore Pending Req\n", __func__);
            break;
        } else {
            LOG_DEBUG("%s: Moved from idle by outgoing Connection request\n", __func__);
            BTA_AvDisconnect(((tBTA_AV *)p_data)->pend.bd_addr);
            break;
        }

        CHECK_RC_EVENT(event, p_data);

    default:
        LOG_WARN("%s : unhandled event:%s\n", __FUNCTION__,
                 dump_av_sm_event_name(event));
        return FALSE;

    }
    return TRUE;
}


/*****************************************************************************
**
** Function        btc_av_state_closing_handler
**
** Description     Intermediate state managing events during closing
**                 of avdtp channel
**
** Returns         TRUE if event was processed, FALSE otherwise
**
*******************************************************************************/

static BOOLEAN btc_av_state_closing_handler(btc_sm_event_t event, void *p_data)
{
    LOG_DEBUG("%s event:%s flags %x\n", __FUNCTION__,
              dump_av_sm_event_name(event), btc_av_cb.flags);

    switch (event) {
    case BTC_SM_ENTER_EVT:
        if (btc_av_cb.peer_sep == AVDT_TSEP_SRC) {
            btc_a2dp_set_rx_flush(TRUE);
        }
        break;

    case BTA_AV_STOP_EVT:
    case BTC_AV_STOP_STREAM_REQ_EVT:
        if (btc_av_cb.peer_sep == AVDT_TSEP_SRC) {
            btc_a2dp_set_rx_flush(TRUE);
        }

        btc_a2dp_on_stopped(NULL);
        break;

    case BTC_SM_EXIT_EVT:
        break;

    case BTA_AV_CLOSE_EVT: {
        tBTA_AV_CLOSE *close = (tBTA_AV_CLOSE *)p_data;
        /* inform the application that we are disconnecting */
        btc_report_connection_state(ESP_A2D_CONNECTION_STATE_DISCONNECTED, &(btc_av_cb.peer_bda),
                                    close->disc_rsn);

        btc_sm_change_state(btc_av_cb.sm_handle, BTC_AV_STATE_IDLE);
        break;
    }

    /* Handle the RC_CLOSE event for the cleanup */
    case BTA_AV_RC_CLOSE_EVT:
        btc_rc_handler(event, (tBTA_AV *)p_data);
        break;

    default:
        LOG_WARN("%s : unhandled event:%s\n", __FUNCTION__,
                 dump_av_sm_event_name(event));
        return FALSE;
    }
    return TRUE;
}


/*****************************************************************************
**
** Function     btc_av_state_opened_handler
**
** Description  Handles AV events while AVDTP is in OPEN state
**
** Returns      TRUE if event was processed, FALSE otherwise
**
*******************************************************************************/

static BOOLEAN btc_av_state_opened_handler(btc_sm_event_t event, void *p_data)
{
    tBTA_AV *p_av = (tBTA_AV *)p_data;

    LOG_DEBUG("%s event:%s flags %x\n", __FUNCTION__,
              dump_av_sm_event_name(event), btc_av_cb.flags);

    if ( (event == BTA_AV_REMOTE_CMD_EVT) && (btc_av_cb.flags & BTC_AV_FLAG_REMOTE_SUSPEND) &&
            (p_av->remote_cmd.rc_id == BTA_AV_RC_PLAY) ) {
        LOG_INFO("%s: Resetting remote suspend flag on RC PLAY\n", __FUNCTION__);
        btc_av_cb.flags &= ~BTC_AV_FLAG_REMOTE_SUSPEND;
    }

    switch (event) {
    case BTC_SM_ENTER_EVT:
        btc_av_cb.flags &= ~BTC_AV_FLAG_PENDING_STOP;
        btc_av_cb.flags &= ~BTC_AV_FLAG_PENDING_START;
        break;

    case BTC_SM_EXIT_EVT:
        btc_av_cb.flags &= ~BTC_AV_FLAG_PENDING_START;
        break;

    case BTC_AV_START_STREAM_REQ_EVT:
        BTA_AvStart();
        btc_av_cb.flags |= BTC_AV_FLAG_PENDING_START;
        break;

    case BTA_AV_START_EVT: {
        LOG_INFO("BTA_AV_START_EVT status %d, suspending %d, init %d\n",
                 p_av->start.status, p_av->start.suspending, p_av->start.initiator);

        if ((p_av->start.status == BTA_SUCCESS) && (p_av->start.suspending == TRUE)) {
            return TRUE;
        }

        /* remain in open state if status failed */
        if (p_av->start.status != BTA_AV_SUCCESS) {
            return FALSE;
        }

        if (btc_av_cb.peer_sep == AVDT_TSEP_SRC) {
            btc_a2dp_set_rx_flush(FALSE); /*  remove flush state, ready for streaming*/
        }

        /* change state to started, send acknowledgement if start is pending */
        if (btc_av_cb.flags & BTC_AV_FLAG_PENDING_START) {

            /* pending start flag will be cleared when exit current state */
        }
        btc_sm_change_state(btc_av_cb.sm_handle, BTC_AV_STATE_STARTED);

    } break;

    case BTC_AV_DISCONNECT_REQ_EVT:
        BTA_AvClose(btc_av_cb.bta_handle);
        if (btc_av_cb.peer_sep == AVDT_TSEP_SRC) {
            BTA_AvCloseRc(btc_av_cb.bta_handle);
        }

        /* inform the application that we are disconnecting */
        btc_report_connection_state(ESP_A2D_CONNECTION_STATE_DISCONNECTING, &(btc_av_cb.peer_bda), 0);
        break;

    case BTA_AV_CLOSE_EVT: {
        /* avdtp link is closed */
        btc_a2dp_on_stopped(NULL);

        tBTA_AV_CLOSE *close = (tBTA_AV_CLOSE *)p_data;
        /* inform the application that we are disconnected */
        btc_report_connection_state(ESP_A2D_CONNECTION_STATE_DISCONNECTED, &(btc_av_cb.peer_bda),
                                    close->disc_rsn);

        /* change state to idle, send acknowledgement if start is pending */
        if (btc_av_cb.flags & BTC_AV_FLAG_PENDING_START) {
            /* pending start flag will be cleared when exit current state */
        }
        btc_sm_change_state(btc_av_cb.sm_handle, BTC_AV_STATE_IDLE);
        break;
    }

    case BTA_AV_RECONFIG_EVT:
        if ((btc_av_cb.flags & BTC_AV_FLAG_PENDING_START) &&
                (p_av->reconfig.status == BTA_AV_SUCCESS)) {
            LOG_WARN("reconfig done BTA_AVstart()\n");
            BTA_AvStart();
        } else if (btc_av_cb.flags & BTC_AV_FLAG_PENDING_START) {
            btc_av_cb.flags &= ~BTC_AV_FLAG_PENDING_START;
        }
        break;

    case BTC_AV_CONNECT_REQ_EVT:
        if (memcmp ((bt_bdaddr_t *)p_data, &(btc_av_cb.peer_bda),
                    sizeof(btc_av_cb.peer_bda)) == 0) {
            LOG_DEBUG("%s: Ignore BTC_AVCONNECT_REQ_EVT for same device\n", __func__);
        } else {
            LOG_DEBUG("%s: Moved to opened by Other Incoming Conn req\n", __func__);
            btc_report_connection_state(ESP_A2D_CONNECTION_STATE_DISCONNECTED,
                                        (bt_bdaddr_t *)p_data, ESP_A2D_DISC_RSN_NORMAL);
        }
        btc_queue_advance();
        break;

        CHECK_RC_EVENT(event, p_data);

    default:
        LOG_WARN("%s : unhandled event:%s\n", __FUNCTION__,
                 dump_av_sm_event_name(event));
        return FALSE;

    }
    return TRUE;
}

/*****************************************************************************
**
** Function     btc_av_state_started_handler
**
** Description  Handles AV events while A2DP stream is started
**
** Returns      TRUE if event was processed, FALSE otherwise
**
*******************************************************************************/

static BOOLEAN btc_av_state_started_handler(btc_sm_event_t event, void *p_data)
{
    tBTA_AV *p_av = (tBTA_AV *)p_data;

    LOG_DEBUG("%s event:%s flags %x\n", __FUNCTION__,
              dump_av_sm_event_name(event), btc_av_cb.flags);

    switch (event) {
    case BTC_SM_ENTER_EVT:

        /* we are again in started state, clear any remote suspend flags */
        btc_av_cb.flags &= ~BTC_AV_FLAG_REMOTE_SUSPEND;

        btc_report_audio_state(ESP_A2D_AUDIO_STATE_STARTED, &(btc_av_cb.peer_bda));

        /* increase the a2dp consumer task priority temporarily when start
        ** audio playing, to avoid overflow the audio packet queue. */
        adjust_priority_a2dp(TRUE);

        break;

    case BTC_SM_EXIT_EVT:
        /* restore the a2dp consumer task priority when stop audio playing. */
        adjust_priority_a2dp(FALSE);

        break;

    case BTC_AV_START_STREAM_REQ_EVT:
        break;

    /* fixme -- use suspend = true always to work around issue with BTA AV */
    case BTC_AV_STOP_STREAM_REQ_EVT:
    case BTC_AV_SUSPEND_STREAM_REQ_EVT:

        /* set pending flag to ensure btc task is not trying to restart
           stream while suspend is in progress */
        btc_av_cb.flags |= BTC_AV_FLAG_LOCAL_SUSPEND_PENDING;

        /* if we were remotely suspended but suspend locally, local suspend
           always overrides */
        btc_av_cb.flags &= ~BTC_AV_FLAG_REMOTE_SUSPEND;

        if (btc_av_cb.peer_sep == AVDT_TSEP_SRC) {
            btc_a2dp_set_rx_flush(TRUE);
            btc_a2dp_on_stopped(NULL);
        }

        BTA_AvStop(TRUE);
        break;

    case BTC_AV_DISCONNECT_REQ_EVT:

        /* request avdtp to close */
        BTA_AvClose(btc_av_cb.bta_handle);
        if (btc_av_cb.peer_sep == AVDT_TSEP_SRC) {
            BTA_AvCloseRc(btc_av_cb.bta_handle);
        }

        /* inform the application that we are disconnecting */
        btc_report_connection_state(ESP_A2D_CONNECTION_STATE_DISCONNECTING, &(btc_av_cb.peer_bda), 0);

        /* wait in closing state until fully closed */
        btc_sm_change_state(btc_av_cb.sm_handle, BTC_AV_STATE_CLOSING);
        break;

    case BTA_AV_SUSPEND_EVT:

        LOG_INFO("BTA_AV_SUSPEND_EVT status %d, init %d\n",
                 p_av->suspend.status, p_av->suspend.initiator);

        /* a2dp suspended, stop media task until resumed */
        btc_a2dp_on_suspended(&p_av->suspend);

        /* if not successful, remain in current state */
        if (p_av->suspend.status != BTA_AV_SUCCESS) {
            btc_av_cb.flags &= ~BTC_AV_FLAG_LOCAL_SUSPEND_PENDING;

            return FALSE;
        }

        if (p_av->suspend.initiator != TRUE) {
            /* remote suspend, notify HAL and await audioflinger to
               suspend/stop stream */

            /* set remote suspend flag to block media task from restarting
               stream only if we did not already initiate a local suspend */
            if ((btc_av_cb.flags & BTC_AV_FLAG_LOCAL_SUSPEND_PENDING) == 0) {
                btc_av_cb.flags |= BTC_AV_FLAG_REMOTE_SUSPEND;
            }

            btc_report_audio_state(ESP_A2D_AUDIO_STATE_REMOTE_SUSPEND, &(btc_av_cb.peer_bda));
        } else {
            btc_report_audio_state(ESP_A2D_AUDIO_STATE_STOPPED, &(btc_av_cb.peer_bda));
        }

        btc_sm_change_state(btc_av_cb.sm_handle, BTC_AV_STATE_OPENED);

        /* suspend completed and state changed, clear pending status */
        btc_av_cb.flags &= ~BTC_AV_FLAG_LOCAL_SUSPEND_PENDING;
        break;

    case BTA_AV_STOP_EVT:

        btc_av_cb.flags |= BTC_AV_FLAG_PENDING_STOP;
        btc_a2dp_on_stopped(&p_av->suspend);

        btc_report_audio_state(ESP_A2D_AUDIO_STATE_STOPPED, &(btc_av_cb.peer_bda));

        /* if stop was successful, change state to open */
        if (p_av->suspend.status == BTA_AV_SUCCESS) {
            btc_sm_change_state(btc_av_cb.sm_handle, BTC_AV_STATE_OPENED);
        }

        break;

    case BTA_AV_CLOSE_EVT: {

        btc_av_cb.flags |= BTC_AV_FLAG_PENDING_STOP;

        /* avdtp link is closed */
        btc_a2dp_on_stopped(NULL);

        tBTA_AV_CLOSE *close = (tBTA_AV_CLOSE *)p_data;
        /* inform the application that we are disconnected */
        btc_report_connection_state(ESP_A2D_CONNECTION_STATE_DISCONNECTED, &(btc_av_cb.peer_bda),
                                    close->disc_rsn);

        btc_sm_change_state(btc_av_cb.sm_handle, BTC_AV_STATE_IDLE);
        break;
    }

    CHECK_RC_EVENT(event, p_data);

    default:
        LOG_WARN("%s : unhandled event:%s\n", __FUNCTION__,
                 dump_av_sm_event_name(event));
        return FALSE;

    }
    return TRUE;
}

/*****************************************************************************
**  Local event handlers
******************************************************************************/

void btc_av_event_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    tBTA_AV *av_src = (tBTA_AV *)p_src;
    tBTA_AV *av_dest = (tBTA_AV *)p_dest;

    // First copy the structure
    memcpy(p_dest, p_src, sizeof(tBTA_AV));

    switch (msg->act) {
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

static void btc_av_event_free_data(btc_sm_event_t event, void *p_data)
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
    bt_status_t stat;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_A2DP;
    msg.act = (uint8_t) event;
    stat = btc_transfer_context(&msg, p_data, sizeof(tBTA_AV), btc_av_event_deep_copy);

    if (stat) {
        LOG_ERROR("%s transfer failed\n", __func__);
    }
}

static void bte_av_media_callback(tBTA_AV_EVT event, tBTA_AV_MEDIA *p_data)
{
    btc_sm_state_t state;
    UINT8 que_len;
    tA2D_STATUS a2d_status;
    tA2D_SBC_CIE sbc_cie;

    if (event == BTA_AV_MEDIA_DATA_EVT) { /* Switch to BTC_MEDIA context */
        state = btc_sm_get_state(btc_av_cb.sm_handle);
        if ( (state == BTC_AV_STATE_STARTED) || /* send SBC packets only in Started State */
                (state == BTC_AV_STATE_OPENED) ) {
            que_len = btc_media_sink_enque_buf((BT_HDR *)p_data);
            LOG_DEBUG(" Packets in Que %d\n", que_len);
        } else {
            return;
        }
    }

    if (event == BTA_AV_MEDIA_SINK_CFG_EVT) {
        /* send a command to BT Media Task */
        btc_reset_decoder((UINT8 *)p_data);

        /* currently only supportes SBC */
        a2d_status = A2D_ParsSbcInfo(&sbc_cie, (UINT8 *)p_data, FALSE);
        if (a2d_status == A2D_SUCCESS) {
            btc_msg_t msg;
            btc_av_args_t arg;

            msg.sig = BTC_SIG_API_CB;
            msg.pid = BTC_PID_A2DP;
            msg.act = BTC_AV_SINK_CONFIG_REQ_EVT;

            memset(&arg, 0, sizeof(btc_av_args_t));
            arg.mcc.type = ESP_A2D_MCT_SBC;
            memcpy(arg.mcc.cie.sbc, (uint8_t *)p_data + 3, ESP_A2D_CIE_LEN_SBC);
            btc_transfer_context(&msg, &arg, sizeof(btc_av_args_t), NULL);
        } else {
            LOG_ERROR("ERROR dump_codec_info A2D_ParsSbcInfo fail:%d\n", a2d_status);
        }
    }
}
/*******************************************************************************
**
** Function         btc_av_init
**
** Description      Initializes btc AV if not already done
**
** Returns          bt_status_t
**
*******************************************************************************/

bt_status_t btc_av_init(void)
{
    if (btc_av_cb.sm_handle == NULL) {
        if (!btc_a2dp_start_media_task()) {
            return BT_STATUS_FAIL;
        }

        /* Also initialize the AV state machine */
        btc_av_cb.sm_handle =
            btc_sm_init((const btc_sm_handler_t *)btc_av_state_handlers, BTC_AV_STATE_IDLE);

        btc_dm_enable_service(BTA_A2DP_SOURCE_SERVICE_ID);
#if (BTA_AV_SINK_INCLUDED == TRUE)
        btc_dm_enable_service(BTA_A2DP_SINK_SERVICE_ID);
#endif

        btc_a2dp_on_init();
    }

    return BT_STATUS_SUCCESS;
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
bt_status_t btc_a2d_sink_init(void)
{
    LOG_DEBUG("%s()\n", __func__);

    return btc_av_init();
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
    btc_av_connect_req_t connect_req;
    connect_req.target_bda = bd_addr;
    connect_req.uuid = uuid;
    LOG_DEBUG("%s\n", __FUNCTION__);

    btc_sm_dispatch(btc_av_cb.sm_handle, BTC_AV_CONNECT_REQ_EVT, (char *)&connect_req);

    return BT_STATUS_SUCCESS;
}

bt_status_t btc_a2d_sink_connect(bt_bdaddr_t *remote_bda)
{
    LOG_DEBUG("%s\n", __FUNCTION__);
    CHECK_BTAV_INIT();

    return btc_queue_connect(UUID_SERVCLASS_AUDIO_SINK, remote_bda, connect_int);
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
static void btc_a2d_sink_deinit(void)
{
    LOG_DEBUG("%s\n", __FUNCTION__);

    btc_a2dp_stop_media_task();

    btc_dm_disable_service(BTA_A2DP_SOURCE_SERVICE_ID);
#if (BTA_AV_SINK_INCLUDED == TRUE)
    btc_dm_disable_service(BTA_A2DP_SINK_SERVICE_ID);
#endif

    /* Also shut down the AV state machine */
    btc_sm_shutdown(btc_av_cb.sm_handle);
    btc_av_cb.sm_handle = NULL;
}

/*******************************************************************************
**
** Function         btc_av_get_sm_handle
**
** Description      Fetches current av SM handle
**
** Returns          None
**
*******************************************************************************/

btc_sm_handle_t btc_av_get_sm_handle(void)
{
    return btc_av_cb.sm_handle;
}

/*******************************************************************************
**
** Function         btc_av_stream_ready
**
** Description      Checks whether AV is ready for starting a stream
**
** Returns          None
**
*******************************************************************************/

BOOLEAN btc_av_stream_ready(void)
{
    btc_sm_state_t state = btc_sm_get_state(btc_av_cb.sm_handle);

    LOG_DEBUG("btc_av_stream_ready : sm hdl %d, state %d, flags %x\n",
              (int)btc_av_cb.sm_handle, state, btc_av_cb.flags);

    /* check if we are remotely suspended or stop is pending */
    if (btc_av_cb.flags & (BTC_AV_FLAG_REMOTE_SUSPEND | BTC_AV_FLAG_PENDING_STOP)) {
        return FALSE;
    }

    return (state == BTC_AV_STATE_OPENED);
}

/*******************************************************************************
**
** Function         btc_av_stream_started_ready
**
** Description      Checks whether AV ready for media start in streaming state
**
** Returns          None
**
*******************************************************************************/

BOOLEAN btc_av_stream_started_ready(void)
{
    btc_sm_state_t state = btc_sm_get_state(btc_av_cb.sm_handle);

    LOG_DEBUG("btc_av_stream_started : sm hdl %d, state %d, flags %x\n",
              (int)btc_av_cb.sm_handle, state, btc_av_cb.flags);

    /* disallow media task to start if we have pending actions */
    if (btc_av_cb.flags & (BTC_AV_FLAG_LOCAL_SUSPEND_PENDING | BTC_AV_FLAG_REMOTE_SUSPEND
                           | BTC_AV_FLAG_PENDING_STOP)) {
        return FALSE;
    }

    return (state == BTC_AV_STATE_STARTED);
}

/*******************************************************************************
**
** Function         btc_dispatch_sm_event
**
** Description      Send event to AV statemachine
**
** Returns          None
**
*******************************************************************************/

/* used to pass events to AV statemachine from other tasks */
void btc_dispatch_sm_event(btc_av_sm_event_t event, void *p_data, int len)
{
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = event;
    btc_transfer_context(&msg, p_data, len, NULL);
}

/*******************************************************************************
**
** Function         btc_av_execute_service
**
** Description      Initializes/Shuts down the service
**
** Returns          BT_STATUS_SUCCESS on success, BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btc_av_execute_service(BOOLEAN b_enable)
{
    if (b_enable) {
        /* TODO: Removed BTA_SEC_AUTHORIZE since the Java/App does not
         * handle this request in order to allow incoming connections to succeed.
         * We need to put this back once support for this is added */

        /* Added BTA_AV_FEAT_NO_SCO_SSPD - this ensures that the BTA does not
         * auto-suspend av streaming on AG events(SCO or Call). The suspend shall
         * be initiated by the app/audioflinger layers */
        BTA_AvEnable(BTA_SEC_AUTHENTICATE, (BTA_AV_FEAT_NO_SCO_SSPD)
                     | BTA_AV_FEAT_RCTG | BTA_AV_FEAT_METADATA | BTA_AV_FEAT_VENDOR
                     | BTA_AV_FEAT_RCCT | BTA_AV_FEAT_ADV_CTRL,
                     bte_av_callback);
        BTA_AvRegister(BTA_AV_CHNL_AUDIO, BTC_AV_SERVICE_NAME, 0, bte_av_media_callback, &bta_av_a2d_cos);
    } else {
        BTA_AvDeregister(btc_av_cb.bta_handle);
        BTA_AvDisable();
    }
    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btc_av_sink_execute_service
**
** Description      Initializes/Shuts down the service
**
** Returns          BT_STATUS_SUCCESS on success, BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btc_av_sink_execute_service(BOOLEAN b_enable)
{
#if (BTA_AV_SINK_INCLUDED == TRUE)
    BTA_AvEnable_Sink(b_enable);
#endif
    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btc_av_is_connected
**
** Description      Checks if av has a connected sink
**
** Returns          BOOLEAN
**
*******************************************************************************/
BOOLEAN btc_av_is_connected(void)
{
    btc_sm_state_t state = btc_sm_get_state(btc_av_cb.sm_handle);
    return ((state == BTC_AV_STATE_OPENED) || (state ==  BTC_AV_STATE_STARTED));
}

/*******************************************************************************
**
** Function         btc_av_is_peer_edr
**
** Description      Check if the connected a2dp device supports
**                  EDR or not. Only when connected this function
**                  will accurately provide a true capability of
**                  remote peer. If not connected it will always be false.
**
** Returns          TRUE if remote device is capable of EDR
**
*******************************************************************************/
BOOLEAN btc_av_is_peer_edr(void)
{
    BTC_ASSERTC(btc_av_is_connected(), "No active a2dp connection\n", 0);

    if (btc_av_cb.edr) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/******************************************************************************
**
** Function        btc_av_clear_remote_suspend_flag
**
** Description     Clears btc_av_cb.flags if BTC_AV_FLAG_REMOTE_SUSPEND is set
**
** Returns          void
******************************************************************************/
void btc_av_clear_remote_suspend_flag(void)
{
    LOG_DEBUG("%s: flag :%x\n", __func__, btc_av_cb.flags);
    btc_av_cb.flags &= ~BTC_AV_FLAG_REMOTE_SUSPEND;
}

void btc_a2dp_call_handler(btc_msg_t *msg)
{
    btc_av_args_t *arg = (btc_av_args_t *)(msg->arg);
    switch (msg->act) {
    case BTC_AV_SINK_CONFIG_REQ_EVT: {
        btc_sm_dispatch(btc_av_cb.sm_handle, msg->act, (void *)(msg->arg));
        break;
    }
    case BTC_AV_SINK_API_INIT_EVT: {
        btc_a2d_sink_init();
        // todo: callback to application
        break;
    }
    case BTC_AV_SINK_API_DEINIT_EVT: {
        btc_a2d_sink_deinit();
        // todo: callback to application
        break;
    }
    case BTC_AV_SINK_API_CONNECT_EVT: {
        btc_a2d_sink_connect(&arg->connect);
        // todo: callback to application
        break;
    }
    case BTC_AV_SINK_API_DISCONNECT_EVT: {
        CHECK_BTAV_INIT();
        btc_sm_dispatch(btc_av_cb.sm_handle, BTC_AV_DISCONNECT_REQ_EVT, NULL);
        break;
    }
    case BTC_AV_SINK_API_REG_DATA_CB_EVT: {
        btc_a2dp_sink_reg_data_cb(arg->data_cb);
        break;
    }
    default:
        LOG_WARN("%s : unhandled event: %d\n", __FUNCTION__, msg->act);
    }
}

void btc_a2dp_cb_handler(btc_msg_t *msg)
{
    btc_sm_dispatch(btc_av_cb.sm_handle, msg->act, (void *)(msg->arg));
    btc_av_event_free_data(msg->act, msg->arg);
}

#endif /* #if BTC_AV_INCLUDED */

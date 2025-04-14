/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*****************************************************************************
 *
 *  Filename:      btc_avk.c
 *
 *  Description:   AV implementation
 *
 *****************************************************************************/
#include "common/bt_target.h"
#include <string.h>
#include "common/bt_trace.h"
#include "common/bt_defs.h"
#include "osi/allocator.h"
#include "stack/btu.h"
#include "bta/bta_av_api.h"
#include "btc/btc_dm.h"
#include "btc/btc_common.h"
#include "btc/btc_manage.h"
#include "btc_av.h"
#include "btc_avrc.h"
#include "btc/btc_util.h"
#include "btc/btc_profile_queue.h"
#include "btc_a2dp.h"
#include "btc_a2dp_control.h"
#include "btc_a2dp_sink.h"
#include "btc_a2dp_source.h"
#include "esp_a2dp_api.h"
#include "osi/alarm.h"

#if BTC_AV_INCLUDED

// global variable to indicate avrc is initialized with a2dp
bool g_av_with_rc;
// global variable to indicate a2dp is initialized
bool g_a2dp_on_init;
// global variable to indicate a2dp is deinitialized
bool g_a2dp_on_deinit;
// global variable to indicate a2dp source deinitialization is ongoing
bool g_a2dp_source_ongoing_deinit;
// global variable to indicate a2dp sink deinitialization is ongoing
bool g_a2dp_sink_ongoing_deinit;


/*****************************************************************************
**  Constants & Macros
******************************************************************************/
#define BTC_AV_SERVICE_NAME "Advanced Audio"

#define BTC_TIMEOUT_AV_OPEN_ON_RC_SECS  2
/* Max audio per 3-DH5 EDR packet: 23.2ms
  jitter buffer: 5(JITTER_BUFFER_WATER_LEVEL) */
#define BTC_DELAY_REPORT_DFT_VALUE      1200  // 120ms

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
    int service_id;
    tBTA_AV_HNDL bta_handle;
    bt_bdaddr_t peer_bda;
    btc_sm_handle_t sm_handle;
    UINT8 flags;
    tBTA_AV_EDR edr;
    UINT8   peer_sep;  /* sep type of peer device */
#if BTC_AV_SRC_INCLUDED
    osi_alarm_t *tle_av_open_on_rc;
#endif /* BTC_AV_SRC_INCLUDED */
} btc_av_cb_t;

typedef struct {
    bt_bdaddr_t target_bda;
    uint16_t uuid;
} btc_av_connect_req_t;

typedef struct {
    bt_bdaddr_t target_bda;
} btc_av_disconn_req_t;

/*****************************************************************************
**  Static variables
******************************************************************************/

#if A2D_DYNAMIC_MEMORY == FALSE
static btc_av_cb_t btc_av_cb = {0};
#else
static btc_av_cb_t *btc_av_cb_ptr = NULL;
#define btc_av_cb (*btc_av_cb_ptr)
#endif ///A2D_DYNAMIC_MEMORY == FALSE

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
static void clean_up(int service_id);

#if BTC_AV_SRC_INCLUDED
static bt_status_t btc_a2d_src_init(void);
static bt_status_t btc_a2d_src_connect(bt_bdaddr_t *remote_bda);
static void btc_a2d_src_deinit(void);
#endif /* BTC_AV_SRC_INCLUDED */

#if BTC_AV_SINK_INCLUDED
static bt_status_t btc_a2d_sink_init(void);
static bt_status_t btc_a2d_sink_connect(bt_bdaddr_t *remote_bda);
static void btc_a2d_sink_set_delay_value(UINT16 delay_value);
static void btc_a2d_sink_get_delay_value(void);
static void btc_a2d_sink_deinit(void);
#endif /* BTC_AV_SINK_INCLUDED */

static const btc_sm_handler_t btc_av_state_handlers[] = {
    btc_av_state_idle_handler,
    btc_av_state_opening_handler,
    btc_av_state_opened_handler,
    btc_av_state_started_handler,
    btc_av_state_closing_handler
};

static void btc_av_event_free_data(btc_msg_t *msg);

/*************************************************************************
** Extern functions
*************************************************************************/

extern tBTA_AV_CO_FUNCTS bta_av_a2d_cos;
extern tBTA_AVRC_CO_FUNCTS bta_avrc_cos;
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
#if BTC_AV_SRC_INCLUDED
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
static void btc_initiate_av_open_tmr_hdlr(void *arg)
{
    UNUSED(arg);
    BD_ADDR peer_addr;
    btc_av_connect_req_t connect_req;
    /* is there at least one RC connection - There should be */
    if (btc_rc_get_connected_peer(peer_addr)) {
        BTC_TRACE_DEBUG("%s Issuing connect to the remote RC peer", __FUNCTION__);
        /* In case of AVRCP connection request, we will initiate SRC connection */
        memcpy(connect_req.target_bda.address, peer_addr, sizeof(bt_bdaddr_t));
        connect_req.uuid = UUID_SERVCLASS_AUDIO_SOURCE;
        btc_dispatch_sm_event(BTC_AV_CONNECT_REQ_EVT, &connect_req, sizeof(btc_av_connect_req_t));
    } else {
        BTC_TRACE_ERROR("%s No connected RC peers", __FUNCTION__);
    }
}
#endif /* BTC_AV_SRC_INCLUDED */

/*****************************************************************************
**  Static functions
******************************************************************************/
static void btc_report_connection_state(esp_a2d_connection_state_t state, bt_bdaddr_t *bd_addr, int disc_rsn)
{
    // todo: add callback for SRC
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
    // todo: add callback for SRC
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
    esp_a2d_cb_param_t param;

    BTC_TRACE_DEBUG("%s event: %s flags %x\n", __FUNCTION__,
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
            memcpy(&btc_av_cb.peer_bda, &((btc_av_connect_req_t *)p_data)->target_bda,
                   sizeof(bt_bdaddr_t));
            if (g_av_with_rc) {
                BTA_AvOpen(btc_av_cb.peer_bda.address, btc_av_cb.bta_handle,
                        TRUE, BTA_SEC_AUTHENTICATE, ((btc_av_connect_req_t *)p_data)->uuid);
            } else {
                BTA_AvOpen(btc_av_cb.peer_bda.address, btc_av_cb.bta_handle,
                        FALSE, BTA_SEC_AUTHENTICATE, ((btc_av_connect_req_t *)p_data)->uuid);
            }
        } else if (event == BTA_AV_PENDING_EVT) {
            bdcpy(btc_av_cb.peer_bda.address, ((tBTA_AV *)p_data)->pend.bd_addr);
            UINT16 uuid = (btc_av_cb.service_id == BTA_A2DP_SOURCE_SERVICE_ID) ? UUID_SERVCLASS_AUDIO_SOURCE :
                UUID_SERVCLASS_AUDIO_SINK;
            if (g_av_with_rc) {
                BTA_AvOpen(btc_av_cb.peer_bda.address, btc_av_cb.bta_handle,
                        TRUE, BTA_SEC_AUTHENTICATE, uuid);
            } else {
                BTA_AvOpen(btc_av_cb.peer_bda.address, btc_av_cb.bta_handle,
                       FALSE, BTA_SEC_AUTHENTICATE, uuid);
            }
        }
        btc_sm_change_state(btc_av_cb.sm_handle, BTC_AV_STATE_OPENING);
    } break;

    case BTC_AV_DISCONNECT_REQ_EVT:
        BTC_TRACE_WARNING("No Link At All.");
        btc_report_connection_state(ESP_A2D_CONNECTION_STATE_DISCONNECTED, &((btc_av_disconn_req_t *)p_data)->target_bda, 0);
        break;

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
#if BTC_AV_SRC_INCLUDED
        BTC_TRACE_DEBUG("BTA_AV_RC_OPEN_EVT received w/o AV");
        btc_av_cb.tle_av_open_on_rc = osi_alarm_new("AVconn", btc_initiate_av_open_tmr_hdlr, NULL, BTC_TIMEOUT_AV_OPEN_ON_RC_SECS * 1000);
        osi_alarm_set(btc_av_cb.tle_av_open_on_rc, BTC_TIMEOUT_AV_OPEN_ON_RC_SECS * 1000);
#endif /* BTC_AV_SRC_INCLUDED */
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
#if BTC_AV_SRC_INCLUDED
        if (btc_av_cb.tle_av_open_on_rc) {
            osi_alarm_free(btc_av_cb.tle_av_open_on_rc);
            btc_av_cb.tle_av_open_on_rc = NULL;
        }
#endif /* BTC_AV_SRC_INCLUDED */
        btc_rc_handler(event, p_data);
        break;

    case BTA_AV_SNK_PSC_CFG_EVT:
#if BTC_AV_SINK_INCLUDED
        param.a2d_psc_cfg_stat.psc_mask = ((tBTA_AV *)p_data)->psc.psc_mask;
        btc_a2d_cb_to_app(ESP_A2D_SNK_PSC_CFG_EVT, &param);
#endif /* BTC_AV_SINK_INCLUDED */
        break;
    case BTA_AV_SET_DELAY_VALUE_EVT:
#if BTC_AV_SINK_INCLUDED
        param.a2d_set_delay_value_stat.delay_value = ((tBTA_AV *)p_data)->delay.delay_value;
        param.a2d_set_delay_value_stat.set_state = ((tBTA_AV *)p_data)->delay.status;
        btc_a2d_cb_to_app(ESP_A2D_SNK_SET_DELAY_VALUE_EVT, &param);
#endif /* BTC_AV_SINK_INCLUDED */
        break;
    case BTA_AV_GET_DELAY_VALUE_EVT:
#if BTC_AV_SINK_INCLUDED
        param.a2d_get_delay_value_stat.delay_value = ((tBTA_AV *)p_data)->delay.delay_value;
        btc_a2d_cb_to_app(ESP_A2D_SNK_GET_DELAY_VALUE_EVT, &param);
#endif /* BTC_AV_SINK_INCLUDED */
        break;

    default:
        BTC_TRACE_WARNING("%s : unhandled event:%s\n", __FUNCTION__,
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
    esp_a2d_cb_param_t param;

    BTC_TRACE_DEBUG("%s event: %s flags %x\n", __FUNCTION__,
              dump_av_sm_event_name(event), btc_av_cb.flags);

    switch (event) {
    case BTC_SM_ENTER_EVT:
        /* inform the application that we are entering connecting state */
        btc_report_connection_state(ESP_A2D_CONNECTION_STATE_CONNECTING, &(btc_av_cb.peer_bda), 0);
        break;

    case BTC_SM_EXIT_EVT:
        break;

    case BTA_AV_REJECT_EVT:
        BTC_TRACE_WARNING(" Received  BTA_AV_REJECT_EVT \n");
        btc_report_connection_state(ESP_A2D_CONNECTION_STATE_DISCONNECTED, &(btc_av_cb.peer_bda), 0);
        btc_sm_change_state(btc_av_cb.sm_handle, BTC_AV_STATE_IDLE);
        break;

    case BTA_AV_OPEN_EVT: {
        tBTA_AV *p_bta_data = (tBTA_AV *)p_data;
        esp_a2d_connection_state_t conn_stat;
        btc_sm_state_t av_state;
        BTC_TRACE_DEBUG("status:%d, edr 0x%x, peer sep %d\n", p_bta_data->open.status,
                        p_bta_data->open.edr, p_bta_data->open.sep);

        if (p_bta_data->open.status == BTA_AV_SUCCESS) {
            btc_av_cb.edr = p_bta_data->open.edr;
            btc_av_cb.peer_sep = p_bta_data->open.sep;

            conn_stat = ESP_A2D_CONNECTION_STATE_CONNECTED;
            av_state = BTC_AV_STATE_OPENED;
        } else {
            BTC_TRACE_WARNING("BTA_AV_OPEN_EVT::FAILED status: %d\n", p_bta_data->open.status);

            conn_stat = ESP_A2D_CONNECTION_STATE_DISCONNECTED;
            av_state = BTC_AV_STATE_IDLE;
        }
        /* inform the application of the event */
        btc_report_connection_state(conn_stat, &(btc_av_cb.peer_bda), 0);
        /* change state to open/idle based on the status */
        btc_sm_change_state(btc_av_cb.sm_handle, av_state);

        if (btc_av_cb.peer_sep == AVDT_TSEP_SNK) {
            /* if queued PLAY command,  send it now */
            /* necessary to add this?
            btc_rc_check_handle_pending_play(p_bta_data->open.bd_addr,
                                             (p_bta_data->open.status == BTA_AV_SUCCESS));
            */
        } else if (btc_av_cb.peer_sep == AVDT_TSEP_SRC &&
                   (p_bta_data->open.status == BTA_AV_SUCCESS)) {
            /* Bring up AVRCP connection too if AVRC Initialized */
            if(g_av_with_rc) {
                BTA_AvOpenRc(btc_av_cb.bta_handle);
            } else {
                BTC_TRACE_WARNING("AVRC not Init, not using it.");
            }
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
            BTC_TRACE_DEBUG("%s: Same device moved to Opening state,ignore Connect Req\n", __func__);
            btc_queue_advance();
            break;
        } else {
            BTC_TRACE_DEBUG("%s: Moved from idle by Incoming Connection request\n", __func__);
            btc_report_connection_state(ESP_A2D_CONNECTION_STATE_DISCONNECTED, (bt_bdaddr_t *)p_data, 0);
            btc_queue_advance();
            break;
        }

    case BTA_AV_PENDING_EVT:
        // Check for device, if same device which moved to opening then ignore callback
        if (memcmp (((tBTA_AV *)p_data)->pend.bd_addr, &(btc_av_cb.peer_bda),
                    sizeof(btc_av_cb.peer_bda)) == 0) {
            BTC_TRACE_DEBUG("%s: Same device moved to Opening state,ignore Pending Req\n", __func__);
            break;
        } else {
            BTC_TRACE_DEBUG("%s: Moved from idle by outgoing Connection request\n", __func__);
            BTA_AvDisconnect(((tBTA_AV *)p_data)->pend.bd_addr);
            break;
        }

    CHECK_RC_EVENT(event, p_data);

    case BTA_AV_SNK_PSC_CFG_EVT:
#if BTC_AV_SINK_INCLUDED
        param.a2d_psc_cfg_stat.psc_mask = ((tBTA_AV *)p_data)->psc.psc_mask;
        btc_a2d_cb_to_app(ESP_A2D_SNK_PSC_CFG_EVT, &param);
#endif /* BTC_AV_SINK_INCLUDED */
        break;
    case BTA_AV_SET_DELAY_VALUE_EVT:
#if BTC_AV_SINK_INCLUDED
        param.a2d_set_delay_value_stat.delay_value = ((tBTA_AV *)p_data)->delay.delay_value;
        param.a2d_set_delay_value_stat.set_state = ((tBTA_AV *)p_data)->delay.status;
        btc_a2d_cb_to_app(ESP_A2D_SNK_SET_DELAY_VALUE_EVT, &param);
#endif /* BTC_AV_SINK_INCLUDED */
        break;

    case BTA_AV_GET_DELAY_VALUE_EVT:
#if BTC_AV_SINK_INCLUDED
        param.a2d_get_delay_value_stat.delay_value = ((tBTA_AV *)p_data)->delay.delay_value;
        btc_a2d_cb_to_app(ESP_A2D_SNK_GET_DELAY_VALUE_EVT, &param);
#endif /* BTC_AV_SINK_INCLUDED */
        break;

    default:
        BTC_TRACE_WARNING("%s : unhandled event:%s\n", __FUNCTION__, dump_av_sm_event_name(event));
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
    esp_a2d_cb_param_t param;

    BTC_TRACE_DEBUG("%s event: %s flags %x\n", __FUNCTION__,
              dump_av_sm_event_name(event), btc_av_cb.flags);

    switch (event) {
    case BTC_SM_ENTER_EVT:
#if BTC_AV_SRC_INCLUDED
        if (btc_av_cb.peer_sep == AVDT_TSEP_SNK) {
            /* immediately stop transmission of frames */
            btc_a2dp_source_set_tx_flush(TRUE);
            /* wait for audioflinger to stop a2dp */
        }
#endif /* BTC_AV_SRC_INCLUDED */
#if BTC_AV_SINK_INCLUDED
        if (btc_av_cb.peer_sep == AVDT_TSEP_SRC) {
            btc_a2dp_sink_set_rx_flush(TRUE);
        }
#endif /* BTC_AV_SINK_INCLUDED */
        break;

    case BTA_AV_STOP_EVT:
    case BTC_AV_STOP_STREAM_REQ_EVT:
#if BTC_AV_SRC_INCLUDED
        if (btc_av_cb.peer_sep == AVDT_TSEP_SNK) {
            /* immediately flush any pending tx frames while suspend is pending */
            btc_a2dp_source_set_tx_flush(TRUE);
        }
#endif /* BTC_AV_SRC_INCLUDED */
#if BTC_AV_SINK_INCLUDED
        if (btc_av_cb.peer_sep == AVDT_TSEP_SRC) {
            btc_a2dp_sink_set_rx_flush(TRUE);
        }
#endif /* BTC_AV_SINK_INCLUDED */
        btc_a2dp_on_stopped(NULL);
        break;

    case BTC_SM_EXIT_EVT:
        break;

    case BTA_AV_CLOSE_EVT: {
        tBTA_AV_CLOSE *close = (tBTA_AV_CLOSE *)p_data;
        /* inform the application that we are disconnecting */
        btc_report_connection_state(ESP_A2D_CONNECTION_STATE_DISCONNECTED, &(btc_av_cb.peer_bda), close->disc_rsn);
        btc_sm_change_state(btc_av_cb.sm_handle, BTC_AV_STATE_IDLE);
        break;
    }

    /* Handle the RC_CLOSE event for the cleanup */
    case BTA_AV_RC_CLOSE_EVT:
        btc_rc_handler(event, (tBTA_AV *)p_data);
        break;

    case BTA_AV_SNK_PSC_CFG_EVT:
        break;
    case BTA_AV_SET_DELAY_VALUE_EVT:
#if BTC_AV_SINK_INCLUDED
        param.a2d_set_delay_value_stat.delay_value = ((tBTA_AV *)p_data)->delay.delay_value;
        param.a2d_set_delay_value_stat.set_state = ((tBTA_AV *)p_data)->delay.status;
        btc_a2d_cb_to_app(ESP_A2D_SNK_SET_DELAY_VALUE_EVT, &param);
#endif /* BTC_AV_SINK_INCLUDED */
        break;

    case BTA_AV_GET_DELAY_VALUE_EVT:
#if BTC_AV_SINK_INCLUDED
        param.a2d_get_delay_value_stat.delay_value = ((tBTA_AV *)p_data)->delay.delay_value;
        btc_a2d_cb_to_app(ESP_A2D_SNK_GET_DELAY_VALUE_EVT, &param);
#endif /* BTC_AV_SINK_INCLUDED */
        break;

    default:
        BTC_TRACE_WARNING("%s : unhandled event:%s\n", __FUNCTION__, dump_av_sm_event_name(event));
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
    esp_a2d_cb_param_t param;
    tBTA_AV *p_av = (tBTA_AV *)p_data;

    BTC_TRACE_DEBUG("%s event: %s flags %x\n", __FUNCTION__,
              dump_av_sm_event_name(event), btc_av_cb.flags);

    if ( (event == BTA_AV_REMOTE_CMD_EVT) && (btc_av_cb.flags & BTC_AV_FLAG_REMOTE_SUSPEND) &&
            (p_av->remote_cmd.rc_id == BTA_AV_RC_PLAY) ) {
        BTC_TRACE_DEBUG("%s: Resetting remote suspend flag on RC PLAY\n", __FUNCTION__);
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
#if BTC_AV_SRC_INCLUDED
        if (btc_av_cb.peer_sep != AVDT_TSEP_SRC) {
            btc_a2dp_source_setup_codec();
        }
#endif  /* BTC_AV_SRC_INCLUDED */
        BTA_AvStart();
        btc_av_cb.flags |= BTC_AV_FLAG_PENDING_START;
        break;

    case BTA_AV_START_EVT: {
        BTC_TRACE_DEBUG("BTA_AV_START_EVT status %d, suspending %d, init %d\n",
                 p_av->start.status, p_av->start.suspending, p_av->start.initiator);

        if ((p_av->start.status == BTA_SUCCESS) && (p_av->start.suspending == TRUE)) {
            return TRUE;
        }
#if BTC_AV_SRC_INCLUDED
        /* if remote tries to start a2dp when DUT is a2dp source
         * then suspend. In case a2dp is sink and call is active
         * then disconnect the AVDTP channel
         */
        if (!(btc_av_cb.flags & BTC_AV_FLAG_PENDING_START)) {
            if (btc_av_cb.peer_sep == AVDT_TSEP_SNK) {
                BTC_TRACE_DEBUG("%s: trigger suspend as remote initiated!!", __FUNCTION__);
                btc_dispatch_sm_event(BTC_AV_SUSPEND_STREAM_REQ_EVT, NULL, 0);
            }
        }
        /*  In case peer is A2DP SRC we do not want to ack commands on UIPC*/
        if (btc_av_cb.peer_sep == AVDT_TSEP_SNK) {
            if (btc_a2dp_on_started(&p_av->start,
                                    ((btc_av_cb.flags & BTC_AV_FLAG_PENDING_START) != 0))) {
                /* only clear pending flag after acknowledgement */
                btc_av_cb.flags &= ~BTC_AV_FLAG_PENDING_START;
            }
        }
#endif /* BTC_AV_SRC_INCLUDED */
        /* remain in open state if status failed */
        if (p_av->start.status != BTA_AV_SUCCESS) {
            return FALSE;
        }
#if BTC_AV_SINK_INCLUDED
        if (btc_av_cb.peer_sep == AVDT_TSEP_SRC) {
            btc_a2dp_sink_set_rx_flush(FALSE); /*  remove flush state, ready for streaming*/
        }
#endif /* BTC_AV_SINK_INCLUDED */
#if BTC_AV_SRC_INCLUDED
        /* change state to started, send acknowledgement if start is pending */
        if (btc_av_cb.flags & BTC_AV_FLAG_PENDING_START) {
            if (btc_av_cb.peer_sep == AVDT_TSEP_SNK) {
                btc_a2dp_on_started(NULL, TRUE);
            }
            /* pending start flag will be cleared when exit current state */
        }
#endif /* BTC_AV_SRC_INCLUDED */
        /* wait for audio path to open */
        btc_a2dp_control_datapath_ctrl(BTC_AV_DATAPATH_OPEN_EVT);

        btc_sm_change_state(btc_av_cb.sm_handle, BTC_AV_STATE_STARTED);

    } break;

    case BTC_AV_DISCONNECT_REQ_EVT:
        BTA_AvClose(btc_av_cb.bta_handle);
        if (btc_av_cb.peer_sep == AVDT_TSEP_SRC && g_av_with_rc == true) {
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

        if (btc_av_cb.flags & BTC_AV_FLAG_PENDING_START) {
            btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_FAILURE);
            /* pending start flag will be cleared when exit current state */
        }

        /* change state to idle, send acknowledgement if start is pending */
        btc_sm_change_state(btc_av_cb.sm_handle, BTC_AV_STATE_IDLE);

        if (g_a2dp_source_ongoing_deinit) {
            clean_up(BTA_A2DP_SOURCE_SERVICE_ID);
        } else if (g_a2dp_sink_ongoing_deinit) {
            clean_up(BTA_A2DP_SINK_SERVICE_ID);
        }
        break;
    }

    case BTA_AV_RECONFIG_EVT:
        if ((btc_av_cb.flags & BTC_AV_FLAG_PENDING_START) &&
                (p_av->reconfig.status == BTA_AV_SUCCESS)) {
            BTC_TRACE_WARNING("reconfig done BTA_AVstart()\n");
            BTA_AvStart();
        } else if (btc_av_cb.flags & BTC_AV_FLAG_PENDING_START) {
            btc_av_cb.flags &= ~BTC_AV_FLAG_PENDING_START;
            btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_FAILURE);
        }
        break;

    case BTC_AV_CONNECT_REQ_EVT:
        if (memcmp (&((btc_av_connect_req_t *)p_data)->target_bda, &(btc_av_cb.peer_bda),
                    sizeof(btc_av_cb.peer_bda)) == 0) {
            BTC_TRACE_DEBUG("%s: Ignore BTC_AVCONNECT_REQ_EVT for same device\n", __func__);
        } else {
            BTC_TRACE_DEBUG("%s: Moved to opened by Other Incoming Conn req\n", __func__);
            btc_report_connection_state(ESP_A2D_CONNECTION_STATE_DISCONNECTED,
                                        (bt_bdaddr_t *)p_data, ESP_A2D_DISC_RSN_NORMAL);
        }
        btc_queue_advance();
        break;

    CHECK_RC_EVENT(event, p_data);

    case BTA_AV_SNK_PSC_CFG_EVT:
        break;
    case BTA_AV_SET_DELAY_VALUE_EVT:
#if BTC_AV_SINK_INCLUDED
        param.a2d_set_delay_value_stat.delay_value = ((tBTA_AV *)p_data)->delay.delay_value;
        param.a2d_set_delay_value_stat.set_state = ((tBTA_AV *)p_data)->delay.status;
        btc_a2d_cb_to_app(ESP_A2D_SNK_SET_DELAY_VALUE_EVT, &param);
#endif /* BTC_AV_SINK_INCLUDED */
        break;

    case BTA_AV_GET_DELAY_VALUE_EVT:
#if BTC_AV_SINK_INCLUDED
        param.a2d_get_delay_value_stat.delay_value = ((tBTA_AV *)p_data)->delay.delay_value;
        btc_a2d_cb_to_app(ESP_A2D_SNK_GET_DELAY_VALUE_EVT, &param);
#endif /* BTC_AV_SINK_INCLUDED */
        break;

    default:
        BTC_TRACE_WARNING("%s : unhandled event:%s\n", __FUNCTION__,
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
    esp_a2d_cb_param_t param;

    BTC_TRACE_DEBUG("%s event: %s flags %x\n", __FUNCTION__,
              dump_av_sm_event_name(event), btc_av_cb.flags);

    switch (event) {
    case BTC_SM_ENTER_EVT:

        /* we are again in started state, clear any remote suspend flags */
        btc_av_cb.flags &= ~BTC_AV_FLAG_REMOTE_SUSPEND;

        btc_report_audio_state(ESP_A2D_AUDIO_STATE_STARTED, &(btc_av_cb.peer_bda));

        /* increase the a2dp consumer task priority temporarily when start
        ** audio playing, to avoid overflow the audio packet queue. */
        // adjust_priority_a2dp(TRUE);

        break;

    case BTC_SM_EXIT_EVT:
        /* restore the a2dp consumer task priority when stop audio playing. */
        // adjust_priority_a2dp(FALSE);

        break;

    case BTC_AV_START_STREAM_REQ_EVT:
#if BTC_AV_SRC_INCLUDED
        /* we were remotely started, just ack back the local request */
        if (btc_av_cb.peer_sep == AVDT_TSEP_SNK) {
            btc_a2dp_on_started(NULL, TRUE);
        }
#endif /* BTC_AV_SRC_INCLUDED */
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
#if BTC_AV_SRC_INCLUDED
        if (btc_av_cb.peer_sep == AVDT_TSEP_SNK) {
            /* immediately stop transmission of frames while suspend is pending */
            btc_a2dp_source_set_tx_flush(TRUE);
        }
#endif /* BTC_AV_SRC_INCLUDED */
#if BTC_AV_SINK_INCLUDED
        if (btc_av_cb.peer_sep == AVDT_TSEP_SRC) {
            btc_a2dp_sink_set_rx_flush(TRUE);
            btc_a2dp_on_stopped(NULL);
        }
#endif /* BTC_AV_SINK_INCLUDED */
        BTA_AvStop(TRUE);
        break;

    case BTC_AV_DISCONNECT_REQ_EVT:

        /* request avdtp to close */
        BTA_AvClose(btc_av_cb.bta_handle);
        if (btc_av_cb.peer_sep == AVDT_TSEP_SRC && g_av_with_rc == true) {
            BTA_AvCloseRc(btc_av_cb.bta_handle);
        }

        /* inform the application that we are disconnecting */
        btc_report_connection_state(ESP_A2D_CONNECTION_STATE_DISCONNECTING, &(btc_av_cb.peer_bda), 0);

        /* wait in closing state until fully closed */
        btc_sm_change_state(btc_av_cb.sm_handle, BTC_AV_STATE_CLOSING);
        break;

    case BTA_AV_SUSPEND_EVT:

        BTC_TRACE_DEBUG("BTA_AV_SUSPEND_EVT status %d, init %d\n",
                 p_av->suspend.status, p_av->suspend.initiator);

        /* a2dp suspended, stop media task until resumed */
        btc_a2dp_on_suspended(&p_av->suspend);

        /* if not successful, remain in current state */
        if (p_av->suspend.status != BTA_AV_SUCCESS) {
            btc_av_cb.flags &= ~BTC_AV_FLAG_LOCAL_SUSPEND_PENDING;
#if BTC_AV_SRC_INCLUDED
            if (btc_av_cb.peer_sep == AVDT_TSEP_SNK) {
                /* suspend failed, reset back tx flush state */
                btc_a2dp_source_set_tx_flush(FALSE);
            }
#endif /* BTC_AV_SRC_INCLUDED */
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

    case BTA_AV_CLOSE_EVT:
        btc_av_cb.flags |= BTC_AV_FLAG_PENDING_STOP;

        /* avdtp link is closed */
        btc_a2dp_on_stopped(NULL);
        tBTA_AV_CLOSE *close = (tBTA_AV_CLOSE *)p_data;
        /* inform the application that we are disconnected */
        btc_report_connection_state(ESP_A2D_CONNECTION_STATE_DISCONNECTED, &(btc_av_cb.peer_bda),
                                    close->disc_rsn);
        btc_sm_change_state(btc_av_cb.sm_handle, BTC_AV_STATE_IDLE);

        if (g_a2dp_source_ongoing_deinit) {
            clean_up(BTA_A2DP_SOURCE_SERVICE_ID);
        } else if (g_a2dp_sink_ongoing_deinit) {
            clean_up(BTA_A2DP_SINK_SERVICE_ID);
        }
        break;

    CHECK_RC_EVENT(event, p_data);

    case BTA_AV_SNK_PSC_CFG_EVT:
        break;
    case BTA_AV_SET_DELAY_VALUE_EVT:
#if BTC_AV_SINK_INCLUDED
        param.a2d_set_delay_value_stat.delay_value = ((tBTA_AV *)p_data)->delay.delay_value;
        param.a2d_set_delay_value_stat.set_state = ((tBTA_AV *)p_data)->delay.status;
        btc_a2d_cb_to_app(ESP_A2D_SNK_SET_DELAY_VALUE_EVT, &param);
#endif /* BTC_AV_SINK_INCLUDED */
        break;

    case BTA_AV_GET_DELAY_VALUE_EVT:
#if BTC_AV_SINK_INCLUDED
        param.a2d_get_delay_value_stat.delay_value = ((tBTA_AV *)p_data)->delay.delay_value;
        btc_a2d_cb_to_app(ESP_A2D_SNK_GET_DELAY_VALUE_EVT, &param);
#endif /* BTC_AV_SINK_INCLUDED */
        break;

    default:
        BTC_TRACE_WARNING("%s : unhandled event:%s\n", __FUNCTION__,
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

static void btc_av_event_free_data(btc_msg_t *msg)
{
    switch (msg->act) {
    case BTA_AV_META_MSG_EVT: {
        tBTA_AV *av = (tBTA_AV *)msg->arg;
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

/*******************************************************************************
**
** Function         btc_av_init
**
** Description      Initializes btc AV if not already done
**
** Returns          bt_status_t
**
*******************************************************************************/
static bt_status_t btc_av_init(int service_id)
{

#if A2D_DYNAMIC_MEMORY == TRUE
    if (btc_av_cb_ptr != NULL) {
        return BT_STATUS_FAIL;
    }

    if ((btc_av_cb_ptr = (btc_av_cb_t *)osi_malloc(sizeof(btc_av_cb_t))) == NULL) {
        APPL_TRACE_ERROR("%s malloc failed!", __func__);
        return BT_STATUS_NOMEM;
    }
    memset((void *)btc_av_cb_ptr, 0, sizeof(btc_av_cb_t));
#endif

    if (btc_av_cb.sm_handle == NULL) {
        btc_av_cb.service_id = service_id;
        bool stat = false;
        if (service_id == BTA_A2DP_SOURCE_SERVICE_ID) {
#if BTC_AV_SRC_INCLUDED
            stat = btc_a2dp_source_startup();
#endif
        } else if (service_id == BTA_A2DP_SINK_SERVICE_ID) {
#if BTC_AV_SINK_INCLUDED
            stat = btc_a2dp_sink_startup();
#endif
        }

        if (!stat) {
#if A2D_DYNAMIC_MEMORY == TRUE
            osi_free(btc_av_cb_ptr);
            btc_av_cb_ptr = NULL;
#endif
            g_a2dp_on_init = false;
            g_a2dp_on_deinit = true;
            g_a2dp_source_ongoing_deinit = false;
            g_a2dp_sink_ongoing_deinit = false;
            goto av_init_fail;
        }

        /* Also initialize the AV state machine */
        btc_av_cb.sm_handle =
            btc_sm_init((const btc_sm_handler_t *)btc_av_state_handlers, BTC_AV_STATE_IDLE);

        if (service_id == BTA_A2DP_SINK_SERVICE_ID) {
            btc_dm_enable_service(BTA_A2DP_SINK_SERVICE_ID);
        } else {
            btc_dm_enable_service(BTA_A2DP_SOURCE_SERVICE_ID);
        }

        btc_a2dp_on_init();
        g_a2dp_on_init = true;
        g_a2dp_on_deinit = false;
        g_a2dp_source_ongoing_deinit = false;
        g_a2dp_sink_ongoing_deinit = false;

        esp_a2d_cb_param_t param;
        memset(&param, 0, sizeof(esp_a2d_cb_param_t));
        param.a2d_prof_stat.init_state = ESP_A2D_INIT_SUCCESS;
        btc_a2d_cb_to_app(ESP_A2D_PROF_STATE_EVT, &param);
        return BT_STATUS_SUCCESS;
    }

av_init_fail:
    return BT_STATUS_FAIL;
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
    memcpy(&connect_req.target_bda, bd_addr, sizeof(bt_bdaddr_t));
    connect_req.uuid = uuid;
    BTC_TRACE_DEBUG("%s\n", __FUNCTION__);

    btc_sm_dispatch(btc_av_cb.sm_handle, BTC_AV_CONNECT_REQ_EVT, (char *)&connect_req);

    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         clean_up
**
** Description      Shuts down the AV interface and does the cleanup
**
** Returns          None
**
*******************************************************************************/
static void clean_up(int service_id)
{
    BTC_TRACE_DEBUG("%s\n", __FUNCTION__);

    if (service_id == BTA_A2DP_SOURCE_SERVICE_ID) {
#if BTC_AV_SRC_INCLUDED
        btc_a2dp_source_shutdown();
        if (btc_av_cb.tle_av_open_on_rc) {
            osi_alarm_free(btc_av_cb.tle_av_open_on_rc);
            btc_av_cb.tle_av_open_on_rc = NULL;
        }
#endif /* BTC_AV_SRC_INCLUDED */
        btc_dm_disable_service(BTA_A2DP_SOURCE_SERVICE_ID);
    }

    if (service_id == BTA_A2DP_SINK_SERVICE_ID) {
#if BTC_AV_SINK_INCLUDED
        btc_a2dp_sink_shutdown();
#endif /* BTC_AV_SINK_INCLUDED */
        btc_dm_disable_service(BTA_A2DP_SINK_SERVICE_ID);
    }

    /* Also shut down the AV state machine */
    btc_sm_shutdown(btc_av_cb.sm_handle);
    btc_av_cb.sm_handle = NULL;

#if A2D_DYNAMIC_MEMORY == TRUE
    osi_free(btc_av_cb_ptr);
    btc_av_cb_ptr = NULL;
#endif
    g_a2dp_on_init = false;
    g_a2dp_on_deinit = true;
    g_a2dp_source_ongoing_deinit = false;
    g_a2dp_sink_ongoing_deinit = false;

    esp_a2d_cb_param_t param;
    memset(&param, 0, sizeof(esp_a2d_cb_param_t));
    param.a2d_prof_stat.init_state = ESP_A2D_DEINIT_SUCCESS;
    btc_a2d_cb_to_app(ESP_A2D_PROF_STATE_EVT, &param);
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

    BTC_TRACE_DEBUG("btc_av_stream_ready : sm hdl %d, state %d, flags %x\n",
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

    BTC_TRACE_DEBUG("btc_av_stream_started : sm hdl %d, state %d, flags %x\n",
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
    btc_transfer_context(&msg, p_data, len, NULL, NULL);
}

static void bte_av_callback(tBTA_AV_EVT event, tBTA_AV *p_data)
{
    bt_status_t stat;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_A2DP;
    msg.act = (uint8_t) event;
    stat = btc_transfer_context(&msg, p_data, sizeof(tBTA_AV),
                                    btc_av_event_deep_copy, btc_av_event_free_data);

    if (stat) {
        BTC_TRACE_ERROR("%s transfer failed\n", __func__);
    }
}

#if BTC_AV_SINK_INCLUDED
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
            que_len = btc_a2dp_sink_enque_buf((BT_HDR *)p_data);
            BTC_TRACE_DEBUG(" Packets in Que %d\n", que_len);
        } else {
            return;
        }
    }

    if (event == BTA_AV_MEDIA_SINK_CFG_EVT) {
        /* send a command to BT Media Task */
        btc_a2dp_sink_reset_decoder((UINT8 *)p_data);

        /* currently only supports SBC */
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
            btc_transfer_context(&msg, &arg, sizeof(btc_av_args_t), NULL, NULL);
        } else {
            BTC_TRACE_ERROR("ERROR dump_codec_info A2D_ParsSbcInfo fail:%d\n", a2d_status);
        }
    }
    UNUSED(que_len);
}
#else
static void bte_av_media_callback(tBTA_AV_EVT event, tBTA_AV_MEDIA *p_data)
{
    UNUSED(event);
    UNUSED(p_data);
    BTC_TRACE_WARNING("%s : event %u\n", __func__, event);
}
#endif

/*******************************************************************************
**
** Function         btc_av_execute_service
**
** Description      Initializes/Shuts down the service
**
** Returns          BT_STATUS_SUCCESS on success, BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btc_av_execute_service(BOOLEAN b_enable, UINT8 tsep)
{
    if (b_enable) {
        /* TODO: Removed BTA_SEC_AUTHORIZE since the Java/App does not
         * handle this request in order to allow incoming connections to succeed.
         * We need to put this back once support for this is added */

        /* Added BTA_AV_FEAT_NO_SCO_SSPD - this ensures that the BTA does not
         * auto-suspend av streaming on AG events(SCO or Call). The suspend shall
         * be initiated by the app/audioflinger layers */
        if (g_av_with_rc) {
            BTC_TRACE_WARNING("A2DP Enable with AVRC")
            BTA_AvEnable(BTA_SEC_AUTHENTICATE, BTA_AV_FEAT_NO_SCO_SSPD |
                        BTA_AV_FEAT_RCTG | BTA_AV_FEAT_METADATA | BTA_AV_FEAT_VENDOR |
                        BTA_AV_FEAT_RCCT | BTA_AV_FEAT_ADV_CTRL | BTA_AV_FEAT_DELAY_RPT,
                        bte_av_callback);
            BTA_AvRegister(BTA_AV_CHNL_AUDIO, BTC_AV_SERVICE_NAME, 0, bte_av_media_callback, &bta_av_a2d_cos, &bta_avrc_cos, tsep);
        } else {
            BTC_TRACE_WARNING("A2DP Enable without AVRC")
            BTA_AvEnable(BTA_SEC_AUTHENTICATE, BTA_AV_FEAT_NO_SCO_SSPD | BTA_AV_FEAT_DELAY_RPT, bte_av_callback);
            BTA_AvRegister(BTA_AV_CHNL_AUDIO, BTC_AV_SERVICE_NAME, 0, bte_av_media_callback, &bta_av_a2d_cos, NULL, tsep);
        }
    } else {
        BTA_AvDeregister(btc_av_cb.bta_handle);
        BTA_AvDisable();
    }
    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btc_av_source_execute_service
**
** Description      Initializes/Shuts down the A2DP source service
**
** Returns          BT_STATUS_SUCCESS on success, BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btc_av_source_execute_service(BOOLEAN b_enable)
{
    return btc_av_execute_service(b_enable, AVDT_TSEP_SRC);
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
    bt_status_t ret = btc_av_execute_service(b_enable, AVDT_TSEP_SNK);
    if (ret != BT_STATUS_SUCCESS) {
        return ret;
    }
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
 *
 * Function         btc_av_get_service_id
 *
 * Description      Get the current AV service ID.
 *
 * Returns          The stream endpoint type: either BTA_A2DP_SOURCE_SERVICE_ID or
 *                  BTA_A2DP_SINK_SERVICE_ID.
 *
 ******************************************************************************/
uint8_t btc_av_get_service_id(void)
{
    return btc_av_cb.service_id;
}

/*******************************************************************************
 *
 * Function         btc_av_get_peer_sep
 *
 * Description      Get the stream endpoint type.
 *
 * Returns          The stream endpoint type: either AVDT_TSEP_SRC or
 *                  AVDT_TSEP_SNK.
 *
 ******************************************************************************/

uint8_t btc_av_get_peer_sep(void)
{
    return btc_av_cb.peer_sep;
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
    BTC_TRACE_DEBUG("%s: flag :%x\n", __func__, btc_av_cb.flags);
    btc_av_cb.flags &= ~BTC_AV_FLAG_REMOTE_SUSPEND;
}

void btc_a2dp_call_handler(btc_msg_t *msg)
{
    btc_av_args_t *arg = (btc_av_args_t *)(msg->arg);
    switch (msg->act) {
#if BTC_AV_SINK_INCLUDED
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
        btc_av_disconn_req_t disconn_req;
        memcpy(&disconn_req.target_bda, &arg->disconn, sizeof(bt_bdaddr_t));
        btc_sm_dispatch(btc_av_cb.sm_handle, BTC_AV_DISCONNECT_REQ_EVT, &disconn_req);
        break;
    }
    case BTC_AV_SINK_API_REG_DATA_CB_EVT: {
        btc_a2dp_sink_reg_data_cb(arg->data_cb);
        break;
    }
    case BTC_AV_SINK_API_SET_DELAY_VALUE_EVT: {
        btc_a2d_sink_set_delay_value(arg->delay_value);
        break;
    }
    case BTC_AV_SINK_API_GET_DELAY_VALUE_EVT: {
        btc_a2d_sink_get_delay_value();
        break;
    }
#endif /* BTC_AV_SINK_INCLUDED */
#if BTC_AV_SRC_INCLUDED
    case BTC_AV_SRC_API_INIT_EVT: {
        btc_a2d_src_init();
        break;
    }
    case BTC_AV_SRC_API_DEINIT_EVT: {
        btc_a2d_src_deinit();
        break;
    }
    case BTC_AV_SRC_API_CONNECT_EVT: {
        btc_a2d_src_connect(&arg->src_connect);
        break;
    }
    case BTC_AV_SRC_API_DISCONNECT_EVT: {
        CHECK_BTAV_INIT();
        btc_av_disconn_req_t disconn_req;
        memcpy(&disconn_req.target_bda, &arg->src_disconn, sizeof(bt_bdaddr_t));
        btc_sm_dispatch(btc_av_cb.sm_handle, BTC_AV_DISCONNECT_REQ_EVT, &disconn_req);
        break;
    }
    case BTC_AV_SRC_API_REG_DATA_CB_EVT: {
        btc_a2dp_src_reg_data_cb(arg->src_data_cb);
        break;
    }
#endif /* BTC_AV_SRC_INCLUDED */
    case BTC_AV_API_MEDIA_CTRL_EVT: {
        btc_a2dp_control_media_ctrl(arg->ctrl);
        break;
    }
    case BTC_AV_CONNECT_REQ_EVT:
        btc_sm_dispatch(btc_av_cb.sm_handle, msg->act, (char *)msg->arg);
        break;
    // case BTC_AV_DISCONNECT_REQ_EVT:
    case BTC_AV_START_STREAM_REQ_EVT:
    case BTC_AV_STOP_STREAM_REQ_EVT:
    case BTC_AV_SUSPEND_STREAM_REQ_EVT: {
        btc_sm_dispatch(btc_av_cb.sm_handle, msg->act, NULL);
        break;
    }
    default:
        BTC_TRACE_WARNING("%s : unhandled event: %d\n", __FUNCTION__, msg->act);
    }
}

void btc_a2dp_cb_handler(btc_msg_t *msg)
{
    btc_sm_dispatch(btc_av_cb.sm_handle, msg->act, (void *)(msg->arg));
    btc_av_event_free_data(msg);
}

void btc_a2dp_get_profile_status(esp_a2d_profile_status_t *param)
{
    // Not initialized by default
    param->a2d_snk_inited = false;
    param->a2d_src_inited = false;

#if A2D_DYNAMIC_MEMORY == TRUE
    if (btc_av_cb_ptr)
#endif
    {
        if (btc_av_cb.sm_handle) {
            if (btc_av_cb.service_id == BTA_A2DP_SINK_SERVICE_ID) {
                param->a2d_src_inited = false;
                param->a2d_snk_inited = true;
            } else if (btc_av_cb.service_id == BTA_A2DP_SOURCE_SERVICE_ID) {
                param->a2d_src_inited = true;
                param->a2d_snk_inited = false;
            } else {
                param->a2d_snk_inited = false;
                param->a2d_src_inited = false;
                return;
            }
            if (btc_av_is_connected()) {
                param->conn_num++;
            }
        }
    }
}

#if BTC_AV_SINK_INCLUDED

/*******************************************************************************
**
** Function         init_sink
**
** Description      Initializes the AV interface for sink mode
**
** Returns          bt_status_t
**
*******************************************************************************/
static bt_status_t btc_a2d_sink_init(void)
{
    BTC_TRACE_DEBUG("%s()\n", __func__);

    return btc_av_init(BTA_A2DP_SINK_SERVICE_ID);
}

static bt_status_t btc_a2d_sink_connect(bt_bdaddr_t *remote_bda)
{
    BTC_TRACE_DEBUG("%s\n", __FUNCTION__);
    CHECK_BTAV_INIT();

    return btc_queue_connect(UUID_SERVCLASS_AUDIO_SINK, remote_bda, connect_int);
}

static void btc_a2d_sink_set_delay_value(UINT16 delay_value)
{
    esp_a2d_cb_param_t param;

    if (delay_value <= BTC_DELAY_REPORT_DFT_VALUE) {
        param.a2d_set_delay_value_stat.delay_value = 0;
        param.a2d_set_delay_value_stat.set_state = ESP_A2D_SET_INVALID_PARAMS;
        btc_a2d_cb_to_app(ESP_A2D_SNK_SET_DELAY_VALUE_EVT, &param);
    } else {
        BTA_SetDelayValue(delay_value);
    }
}

static void btc_a2d_sink_get_delay_value(void)
{
    BTA_GetDelayValue();
}

static void btc_a2d_sink_deinit(void)
{
    g_a2dp_sink_ongoing_deinit = true;
    if (btc_av_is_connected()) {
        BTA_AvClose(btc_av_cb.bta_handle);
        if (btc_av_cb.peer_sep == AVDT_TSEP_SRC && g_av_with_rc == true) {
            BTA_AvCloseRc(btc_av_cb.bta_handle);
        }
    } else {
        clean_up(BTA_A2DP_SINK_SERVICE_ID);
    }
}

#endif /* BTC_AV_SINK_INCLUDED */

#if BTC_AV_SRC_INCLUDED

/*******************************************************************************
**
** Function         btc_a2d_src_init
**
** Description      Initializes the AV interface for source mode
**
** Returns          bt_status_t
**
*******************************************************************************/
static bt_status_t btc_a2d_src_init(void)
{
    BTC_TRACE_DEBUG("%s()\n", __func__);

    return btc_av_init(BTA_A2DP_SOURCE_SERVICE_ID);
}

static void btc_a2d_src_deinit(void)
{
    g_a2dp_source_ongoing_deinit = true;
    if (btc_av_is_connected()) {
        BTA_AvClose(btc_av_cb.bta_handle);
        if (btc_av_cb.peer_sep == AVDT_TSEP_SNK && g_av_with_rc == true) {
            BTA_AvCloseRc(btc_av_cb.bta_handle);
        }
    } else {
        clean_up(BTA_A2DP_SOURCE_SERVICE_ID);
    }
}

static bt_status_t btc_a2d_src_connect(bt_bdaddr_t *remote_bda)
{
    BTC_TRACE_DEBUG("%s\n", __FUNCTION__);
    CHECK_BTAV_INIT();

    return btc_queue_connect(UUID_SERVCLASS_AUDIO_SOURCE, remote_bda, connect_int);
}

#endif /* BTC_AV_SRC_INCLUDED */

#endif /* #if BTC_AV_INCLUDED */

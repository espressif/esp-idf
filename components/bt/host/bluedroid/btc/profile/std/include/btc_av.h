/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


/*******************************************************************************
 *
 *  Filename:      btc_av.h
 *
 *  Description:   Main API header file for all BTC AV functions accessed
 *                 from internal stack.
 *
 *******************************************************************************/

#ifndef __BTC_AV_H__
#define __BTC_AV_H__

#include <stdbool.h>
#include "common/bt_target.h"
#include "esp_a2dp_api.h"
#include "btc/btc_task.h"
#include "btc/btc_common.h"
#include "btc/btc_sm.h"
#include "btc_av_api.h"
#include "bta/bta_av_api.h"
#include "bta/bta_av_sbc.h"

#if (BTC_AV_INCLUDED == TRUE)

#if (BTA_AV_CO_CP_SCMS_T == TRUE)
#define BTC_MEDIA_AA_OFFSET (AVDT_MEDIA_OFFSET + 1)
#else
#define BTC_MEDIA_AA_OFFSET (AVDT_MEDIA_OFFSET)
#endif

#define BTC_MEDIA_AA_SBC_OFFSET (BTC_MEDIA_AA_OFFSET + BTA_AV_SBC_HDR_SIZE)
#define BTC_AUDIO_BUFF_OFFSET   (BTC_MEDIA_AA_OFFSET)

/* Number of A2DP connections BTC can track at the same time. Only the sink role
 * supports more than one, the source role always uses a single connection. */
#if BTC_AV_SINK_INCLUDED
#define BTC_AV_MAX_CONN         BTA_AV_NUM_SNK_CONN
#else
#define BTC_AV_MAX_CONN         1
#endif

/* Index reported when no connection is currently rendering audio */
#define BTC_AV_IDX_INVALID      (-1)

// global variable to indicate avrc is initialized with a2dp
extern bool g_av_with_rc;
// global variable to indicate a2dp is initialized
extern bool g_a2dp_on_init;
// global variable to indicate a2dp is deinitialized
extern bool g_a2dp_on_deinit;
// global variable to indicate a2dp source deinitialization is ongoing
extern bool g_a2dp_source_ongoing_deinit;
// global variable to indicate a2dp sink deinitialization is ongoing
extern bool g_a2dp_sink_ongoing_deinit;

/*******************************************************************************
**  Type definitions for callback functions
********************************************************************************/

enum {
    BTC_AV_DATAPATH_OPEN_EVT, // original UIPC_OPEN_EVT for data channel in bluedroid
    BTC_AV_DATAPATH_MAX_EVT,
};

typedef enum {
    BTC_AV_CONNECT_REQ_EVT = BTA_AV_MAX_EVT,
    BTC_AV_DISCONNECT_REQ_EVT,
    BTC_AV_START_STREAM_REQ_EVT,
    BTC_AV_SUSPEND_STREAM_REQ_EVT,
    BTC_AV_CONFIG_EVT,
} btc_av_sm_event_t;

typedef enum {
#if BTC_AV_SINK_INCLUDED
    BTC_AV_SINK_API_INIT_EVT = 0,
    BTC_AV_SINK_API_REG_SEP_EVT,
    BTC_AV_SINK_API_DEINIT_EVT,
    BTC_AV_SINK_API_CONNECT_EVT,
    BTC_AV_SINK_API_DISCONNECT_EVT,
    BTC_AV_SINK_API_REG_DATA_CB_EVT,
    BTC_AV_SINK_API_REG_AUDIO_DATA_CB_EVT,
    BTC_AV_SINK_API_SET_DELAY_VALUE_EVT,
    BTC_AV_SINK_API_GET_DELAY_VALUE_EVT,
    BTC_AV_SINK_API_GET_ACTIVE_DEVICE_EVT,
    BTC_AV_SINK_API_SET_ACTIVE_DEVICE_EVT,
#endif  /* BTC_AV_SINK_INCLUDED */
#if BTC_AV_SRC_INCLUDED
    BTC_AV_SRC_API_INIT_EVT,
    BTC_AV_SRC_API_SET_PREF_MCC_EVT,
    BTC_AV_SRC_API_REG_SEP_EVT,
    BTC_AV_SRC_API_DEINIT_EVT,
    BTC_AV_SRC_API_CONNECT_EVT,
    BTC_AV_SRC_API_DISCONNECT_EVT,
    BTC_AV_SRC_API_REG_DATA_CB_EVT,
#endif  /* BTC_AV_SRC_INCLUDED */
    BTC_AV_API_MEDIA_CTRL_EVT,
} btc_av_act_t;

/* btc_av_args_t */
typedef union {
#if BTC_AV_SINK_INCLUDED
    // BTC_AV_SINK_API_CONNECT_EVT
    bt_bdaddr_t connect;
    // BTC_AV_SINK_API_DISCONNECT_EVT
    struct {
        /* TODO: sentinel for deprecated esp_a2d_sink_disconnect(). Remove it
         * when that API is deleted; a disconnect message must carry a real handle.
         */
        esp_a2d_conn_hdl_t conn_hdl;
        bt_bdaddr_t bda;
    } disconn;
    // BTC_AV_SINK_API_REG_DATA_CB_EVT
    esp_a2d_sink_data_cb_t data_cb;
    // BTC_AV_SINK_API_REG_AUDIO_DATA_CB_EVT
    esp_a2d_sink_audio_data_cb_t audio_data_cb;
    // BTC_AV_SINK_API_SET_DELAY_VALUE_EVT
    uint16_t delay_value;
    // BTC_AV_SINK_API_SET_ACTIVE_DEVICE_EVT
    esp_a2d_conn_hdl_t set_active;
#endif  /* BTC_AV_SINK_INCLUDED */
#if BTC_AV_SRC_INCLUDED
    // BTC_AV_SRC_API_REG_DATA_CB_EVT
    esp_a2d_source_data_cb_t src_data_cb;
    // BTC_AV_SRC_API_CONNECT
    bt_bdaddr_t src_connect;
    // BTC_AV_SRC_API_DISCONNECT_EVT
    struct {
        /* TODO: sentinel for deprecated esp_a2d_source_disconnect(). Remove it
         * when that API is deleted; a disconnect message must carry a real handle.
         */
        esp_a2d_conn_hdl_t conn_hdl;
        bt_bdaddr_t bda;
    } src_disconn;
    // BTC_AV_SRC_API_SET_PREF_MCC_EVT
    struct {
        esp_a2d_conn_hdl_t conn_hdl;
        esp_a2d_mcc_t pref_mcc;
    } set_pref_mcc;
#endif /* BTC_AV_SRC_INCLUDED */
    // BTC_AV_CONFIG_EVT
    struct {
        tBTA_AV_HNDL hndl;      /* connection the codec configuration belongs to */
        esp_a2d_mcc_t mcc;
    } config;
    // BTC_AV_SINK_API_REG_SEP_EVT or BTC_AV_SRC_API_REG_SEP_EVT
    struct {
        uint8_t seid;
        esp_a2d_mcc_t mcc;
    } reg_sep;
    // BTC_AV_API_MEDIA_CTRL_EVT
    struct {
        esp_a2d_media_ctrl_t ctrl;
        /* TODO: sentinel for deprecated esp_a2d_media_ctrl(). Remove it when
         * that API is deleted; a media-control message must carry a real handle.
         */
        esp_a2d_conn_hdl_t conn_hdl;
    } media_ctrl;
} btc_av_args_t;

/*******************************************************************************
**  BTC AV API
********************************************************************************/

void btc_a2dp_call_handler(btc_msg_t *msg);

void btc_a2dp_cb_handler(btc_msg_t *msg);

void btc_a2dp_get_profile_status(esp_a2d_profile_status_t *param);

void btc_a2dp_sink_reg_data_cb(esp_a2d_sink_data_cb_t callback);

/*******************************************************************************
**
** Function         btc_a2dp_sink_reg_audio_data_cb
**
** Description      Register a2dp sink audio data callback
**
** Returns          None
**
*******************************************************************************/
void btc_a2dp_sink_reg_audio_data_cb(esp_a2d_sink_audio_data_cb_t callback);

#if (BTC_AV_EXT_CODEC == TRUE)

/*******************************************************************************
**
** Function         btc_av_codec_cap_get
**
** Description      Get external codec capability
**
** Returns          Pointer to codec capability
**
*******************************************************************************/
tBTC_AV_CODEC_INFO *btc_av_codec_cap_get(void);

#endif

void btc_a2dp_src_reg_data_cb(esp_a2d_source_data_cb_t callback);
/*******************************************************************************
**
** Function         btc_av_get_sm_handle
**
** Description      Fetches current av SM handle
**
** Returns          None
**
*******************************************************************************/

btc_sm_handle_t btc_av_get_sm_handle(void);

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
void btc_dispatch_sm_event(btc_av_sm_event_t event, void *p_data, int len);

/*******************************************************************************
**
** Function         btc_av_is_connected
**
** Description      Checks if av has a connected sink
**
** Returns          BOOLEAN
**
*******************************************************************************/

BOOLEAN btc_av_is_connected(void);

/*******************************************************************************
**
** Function         btc_av_is_started
**
** Description      Checks if av is started
**
** Returns          BOOLEAN
**
*******************************************************************************/
BOOLEAN btc_av_is_started(void);

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

uint8_t btc_av_get_peer_sep(void);

/*******************************************************************************
**
** Function         btc_av_get_active_peer
**
** Description      Get the connection handle of the peer selected for rendering.
**                  Same-task helper. The public API reports the result in
**                  ESP_A2D_SNK_ACTIVE_DEVICE_EVT instead of calling this.
**
** Returns          TRUE when a peer is selected
**
*******************************************************************************/
BOOLEAN btc_av_get_active_peer(esp_a2d_conn_hdl_t *conn_hdl);

/*******************************************************************************
**
** Function         btc_av_set_active_peer
**
** Description      Choose which connected peer gets rendered. Accepts OPENED
**                  or STARTED. An idle target is sent AVRCP Play.
**
** Returns          TRUE when the peer became the active one
**
*******************************************************************************/
BOOLEAN btc_av_set_active_peer(esp_a2d_conn_hdl_t conn_hdl);

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

BOOLEAN btc_av_is_peer_edr(void);

/******************************************************************************
**
** Function         btc_av_clear_remote_suspend_flag
**
** Description      Clears remote suspended flag
**
** Returns          Void
********************************************************************************/
void btc_av_clear_remote_suspend_flag(void);

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
uint8_t btc_av_get_service_id(void);

#if (BTC_AV_SRC_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         btc_av_report_all_snk_codec_caps
**
** Description      Report all sink codec capabilities to the application.
**
*******************************************************************************/
void btc_av_report_all_snk_codec_caps(tBTA_AV_HNDL hndl, esp_a2d_sep_mcc_t *sep_mcc, UINT8 sep_num);
#endif /* BTC_AV_SRC_INCLUDED */

#if (BTC_AV_SINK_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         btc_av_get_sink_codec_info
**
** Description      Codec cached on this connection at MEDIA_CFG. That copy is
**                  per-stream and is not overwritten by another phone's
**                  SetConfig (unlike bta_av_co_cb.codec_cfg).
**
*******************************************************************************/
BOOLEAN btc_av_get_sink_codec_info(uint16_t hndl, tBTC_AV_CODEC_INFO *out);
#endif /* BTC_AV_SINK_INCLUDED */

#endif  ///BTC_AV_INCLUDED == TRUE

/*******************************************************************************
**
** Function         btc_a2d_conn_handle_get
**
** Description      Get connection handle of the peer currently rendered, or of
**                  any connected peer when none is selected.
**
** Returns          The connection handle, or ESP_A2D_INVALID_CONN_HDL if the
**                  control block is not allocated
**
*******************************************************************************/
esp_a2d_conn_hdl_t btc_a2d_conn_handle_get(void);

/*******************************************************************************
**
** Function         btc_a2d_conn_hdl_is_valid
**
** Description      TRUE when conn_hdl belongs to a peer that is not IDLE.
**
** Returns          true if the handle is valid
**
*******************************************************************************/
bool btc_a2d_conn_hdl_is_valid(esp_a2d_conn_hdl_t conn_hdl);

/*******************************************************************************
**
** Function         btc_a2d_conn_hdl_get_by_bda
**
** Description      Resolve an A2DP connection handle from a peer address.
**                  Called on the BTC task for the deprecated
**                  esp_a2d_sink_disconnect() and esp_a2d_source_disconnect().
**                  Remove this function together with those two APIs.
**
** Returns          The connection handle, or ESP_A2D_INVALID_CONN_HDL if the
**                  address does not match a connected peer
**
*******************************************************************************/
esp_a2d_conn_hdl_t btc_a2d_conn_hdl_get_by_bda(const esp_bd_addr_t remote_bda);

/*******************************************************************************
**
** Function         btc_a2d_conn_hdl_get_peer
**
** Description      Peer address of the A2DP connection identified by conn_hdl.
**
** Returns          true when the handle belongs to a connected peer
**
*******************************************************************************/
bool btc_a2d_conn_hdl_get_peer(esp_a2d_conn_hdl_t conn_hdl, esp_bd_addr_t remote_bda);

/*******************************************************************************
**
** Function         btc_av_conn_stream_ready
**
** Description      TRUE when this connection is OPENED and may be started.
**
*******************************************************************************/
BOOLEAN btc_av_conn_stream_ready(esp_a2d_conn_hdl_t conn_hdl);

/*******************************************************************************
**
** Function         btc_av_conn_stream_started_ready
**
** Description      TRUE when this connection is STARTED and may accept media.
**
*******************************************************************************/
BOOLEAN btc_av_conn_stream_started_ready(esp_a2d_conn_hdl_t conn_hdl);

/*******************************************************************************
**
** Function         btc_av_conn_is_connected
**
** Description      TRUE when this connection is not IDLE.
**
*******************************************************************************/
BOOLEAN btc_av_conn_is_connected(esp_a2d_conn_hdl_t conn_hdl);

/*******************************************************************************
**
** Function         btc_av_audio_buff_alloc
**
** Description      Allocate audio buffer with specific size
**
** Returns          Void
**
*******************************************************************************/
void btc_av_audio_buff_alloc(uint16_t size, uint8_t **pp_buff, uint8_t **pp_data);

/*******************************************************************************
**
** Function         btc_av_audio_buff_free
**
** Description      Free audio buffer
**
** Returns          Void
**
*******************************************************************************/
void btc_av_audio_buff_free(uint8_t *p_buf);

/*******************************************************************************
**
** Function         btc_a2d_src_audio_mtu_check
**
** Description      Checks if data length is valid, not bigger than mtu
**
** Returns          BOOLEAN
**
*******************************************************************************/
BOOLEAN btc_a2d_src_audio_mtu_check(uint16_t data_len);

/*******************************************************************************
**
** Function         btc_a2d_src_audio_data_send
**
** Description      Send audio data to lower layer, audio buffer is consumed
**                  only when operation is success
**
** Returns          BT_STATUS_SUCCESS if success, otherwise, BT_STATUS_FAIL
**
*******************************************************************************/
bt_status_t btc_a2d_src_audio_data_send(esp_a2d_conn_hdl_t conn_hdl, esp_a2d_audio_buff_t *audio_buf);

#endif /* __BTC_AV_H__ */

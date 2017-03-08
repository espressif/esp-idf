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
 *  Filename:      btc_avrc.c
 *
 *  Description:   Bluetooth AVRC implementation
 *
 *****************************************************************************/
#include <string.h>
#include "bta_api.h"
#include "bta_av_api.h"
#include "avrc_defs.h"
#include "gki.h"

#include "btc_common.h"
#include "btc_util.h"
#include "btc_av.h"
#include "btc_avrc.h"
#include "btc_manage.h"
#include "uinput.h"
#include "esp_avrc_api.h"

/*****************************************************************************
**  Constants & Macros
******************************************************************************/

/* cod value for Headsets */
#define COD_AV_HEADSETS        0x0404
/* for AVRC 1.4 need to change this */
#define MAX_RC_NOTIFICATIONS AVRC_EVT_APP_SETTING_CHANGE

#define IDX_GET_PLAY_STATUS_RSP   0
#define IDX_LIST_APP_ATTR_RSP     1
#define IDX_LIST_APP_VALUE_RSP    2
#define IDX_GET_CURR_APP_VAL_RSP  3
#define IDX_SET_APP_VAL_RSP       4
#define IDX_GET_APP_ATTR_TXT_RSP  5
#define IDX_GET_APP_VAL_TXT_RSP   6
#define IDX_GET_ELEMENT_ATTR_RSP  7
#define MAX_VOLUME 128
#define MAX_LABEL 16
#define MAX_TRANSACTIONS_PER_SESSION 16
#define MAX_CMD_QUEUE_LEN 8
#define PLAY_STATUS_PLAYING 1

#define CHECK_RC_CONNECTED                                                                  \
    LOG_DEBUG("## %s ##", __FUNCTION__);                                            \
    if(btc_rc_cb.rc_connected == FALSE)                                                    \
    {                                                                                       \
        LOG_WARN("Function %s() called when RC is not connected", __FUNCTION__); \
        return BT_STATUS_NOT_READY;                                                         \
    }

#define CHECK_ESP_RC_CONNECTED       do { \
        LOG_DEBUG("## %s ##", __FUNCTION__); \
        if (btc_rc_vb.rc_connected == FALSE) { \
            LOG_WARN("Function %s() called when RC is not connected", __FUNCTION__); \
	    return ESP_ERR_INVALID_STATE; \
        } \
    } while (0)

#define FILL_PDU_QUEUE(index, ctype, label, pending)        \
{                                                           \
    btc_rc_vb.rc_pdu_info[index].ctype = ctype;            \
    btc_rc_vb.rc_pdu_info[index].label = label;            \
    btc_rc_vb.rc_pdu_info[index].is_rsp_pending = pending; \
}

#define SEND_METAMSG_RSP(index, avrc_rsp)                                                      \
{                                                                                              \
    if(btc_rc_vb.rc_pdu_info[index].is_rsp_pending == FALSE)                                  \
    {                                                                                          \
        LOG_WARN("%s Not sending response as no PDU was registered", __FUNCTION__); \
        return BT_STATUS_UNHANDLED;                                                            \
    }                                                                                          \
    send_metamsg_rsp(btc_rc_vb.rc_handle, btc_rc_vb.rc_pdu_info[index].label,                \
        btc_rc_vb.rc_pdu_info[index].ctype, avrc_rsp);                                        \
    btc_rc_vb.rc_pdu_info[index].ctype = 0;                                                   \
    btc_rc_vb.rc_pdu_info[index].label = 0;                                                   \
    btc_rc_vb.rc_pdu_info[index].is_rsp_pending = FALSE;                                      \
}

/*****************************************************************************
**  Local type definitions
******************************************************************************/
typedef struct {
    UINT8 bNotify;
    UINT8 label;
} btc_rc_reg_notifications_t;

typedef struct
{
    UINT8   label;
    UINT8   ctype;
    BOOLEAN is_rsp_pending;
} btc_rc_cmd_ctxt_t;

/* TODO : Merge btc_rc_reg_notifications_t and btc_rc_cmd_ctxt_t to a single struct */
typedef struct {
    BOOLEAN                     rc_connected;
    UINT8                       rc_handle;
    tBTA_AV_FEAT                rc_features;
    BD_ADDR                     rc_addr;
    UINT16                      rc_pending_play;
    btc_rc_cmd_ctxt_t           rc_pdu_info[MAX_CMD_QUEUE_LEN];
    btc_rc_reg_notifications_t  rc_notif[MAX_RC_NOTIFICATIONS];
    unsigned int                rc_volume;
    uint8_t                     rc_vol_label;
} btc_rc_cb_t;

typedef struct {
    BOOLEAN in_use;
    UINT8 lbl;
    UINT8 handle;
} rc_transaction_t;

typedef struct
{
    pthread_mutex_t lbllock;
    rc_transaction_t transaction[MAX_TRANSACTIONS_PER_SESSION];
} rc_device_t;


rc_device_t device;

static int uinput_fd = -1;
static void send_key (int fd, uint16_t key, int pressed);

static const struct {
    const char *name;
    uint8_t avrcp;
    uint16_t mapped_id;
    uint8_t release_quirk;
} key_map[] = {
    { "PLAY",         AVRC_ID_PLAY,     KEY_PLAYCD,       1 },
    { "STOP",         AVRC_ID_STOP,     KEY_STOPCD,       0 },
    { "PAUSE",        AVRC_ID_PAUSE,    KEY_PAUSECD,      1 },
    { "FORWARD",      AVRC_ID_FORWARD,  KEY_NEXTSONG,     0 },
    { "BACKWARD",     AVRC_ID_BACKWARD, KEY_PREVIOUSSONG, 0 },
    { "REWIND",       AVRC_ID_REWIND,   KEY_REWIND,       0 },
    { "FAST FORWARD", AVRC_ID_FAST_FOR, KEY_FAST_FORWARD, 0 },
    { NULL,           0,                0,                0 }
};

static void send_reject_response (UINT8 rc_handle, UINT8 label,
    UINT8 pdu, UINT8 status);
static UINT8 opcode_from_pdu(UINT8 pdu);
static void send_metamsg_rsp (UINT8 rc_handle, UINT8 label,
    tBTA_AV_CODE code, tAVRC_RESPONSE *pmetamsg_resp);
static void register_volumechange(UINT8 label);
static void lbl_init();
static void lbl_destroy();
static void init_all_transactions();
static bt_status_t  get_transaction(rc_transaction_t **ptransaction);
static void release_transaction(UINT8 label);
static rc_transaction_t* get_transaction_by_lbl(UINT8 label);
static void handle_rc_metamsg_rsp(tBTA_AV_META_MSG *pmeta_msg);
static void btc_rc_upstreams_evt(UINT16 event, tAVRC_COMMAND* p_param, UINT8 ctype, UINT8 label);
static void btc_rc_upstreams_rsp_evt(UINT16 event, tAVRC_RESPONSE *pavrc_resp, UINT8 ctype, UINT8 label);

static void handle_rc_connect (tBTA_AV_RC_OPEN *p_rc_open);
static void handle_rc_disconnect (tBTA_AV_RC_CLOSE *p_rc_close);
static void handle_rc_passthrough_rsp ( tBTA_AV_REMOTE_RSP *p_remote_rsp);
static void handle_rc_passthrough_cmd ( tBTA_AV_REMOTE_CMD *p_remote_cmd);
static void handle_uid_changed_notification(tBTA_AV_META_MSG *pmeta_msg, tAVRC_COMMAND *pavrc_command);
static void handle_rc_metamsg_cmd (tBTA_AV_META_MSG *pmeta_msg);
/*****************************************************************************
**  Static variables
******************************************************************************/
static btc_rc_cb_t btc_rc_vb;
static btrc_callbacks_t *bt_rc_callbacks = NULL;

/*****************************************************************************
**  Static functions
******************************************************************************/

/*****************************************************************************
**  Externs
******************************************************************************/
#if BTC_HF_INCLUDED
extern BOOLEAN btc_hf_call_terminated_recently();
#endif

/*****************************************************************************
**   Local uinput helper functions
******************************************************************************/
static inline void btc_avrc_ct_cb_to_app(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *param)
{
    esp_a2d_cb_t btc_avrc_cb = (esp_a2d_cb_t)btc_profile_cb_get(BTC_PID_AVRC);
    if (btc_avrc_cb) {
	btc_avrc_cb(event, param);
    }
}

void send_key (int fd, uint16_t key, int pressed)
{
    LOG_DEBUG("%s fd:%d key:%u pressed:%d, func not implemented", __FUNCTION__,
        fd, key, pressed);

    return;
}

static void handle_rc_features()
{
    btrc_remote_features_t rc_features = BTRC_FEAT_NONE;
    bt_bdaddr_t rc_addr;
    bdcpy(rc_addr.address, btc_rc_vb.rc_addr);

    // TODO(eisenbach): If devices need to be blacklisted for absolute
    // volume, it should be added to device/include/interop_database.h
    // For now, everything goes... If blacklisting is necessary, exclude
    // the following bit here:
    //    btc_rc_vb.rc_features &= ~BTA_AV_FEAT_ADV_CTRL;

    if (btc_rc_vb.rc_features & BTA_AV_FEAT_BROWSE)
    {
        rc_features |= BTRC_FEAT_BROWSE;
    }

    if ( (btc_rc_vb.rc_features & BTA_AV_FEAT_ADV_CTRL) &&
         (btc_rc_vb.rc_features & BTA_AV_FEAT_RCTG))
    {
        rc_features |= BTRC_FEAT_ABSOLUTE_VOLUME;
    }

    if (btc_rc_vb.rc_features & BTA_AV_FEAT_METADATA)
    {
        rc_features |= BTRC_FEAT_METADATA;
    }

    LOG_DEBUG("%s: rc_features=0x%x", __FUNCTION__, rc_features);
    // todo: uncomment the following line when added the AVRC target role
    // BTC_HAL_CBACK(bt_rc_callbacks, remote_features_cb, &rc_addr, rc_features)
    
#if (AVRC_ADV_CTRL_INCLUDED == TRUE)
     LOG_DEBUG("Checking for feature flags in btc_rc_handler with label %d",
                        btc_rc_vb.rc_vol_label);
     // Register for volume change on connect
     if(btc_rc_vb.rc_features & BTA_AV_FEAT_ADV_CTRL &&
	btc_rc_vb.rc_features & BTA_AV_FEAT_RCTG)
     {
         rc_transaction_t *p_transaction=NULL;
         bt_status_t status = BT_STATUS_NOT_READY;
         if(MAX_LABEL==btc_rc_vb.rc_vol_label)
         {
            status=get_transaction(&p_transaction);
         }
         else
         {
            p_transaction=get_transaction_by_lbl(btc_rc_vb.rc_vol_label);
            if(NULL!=p_transaction)
            {
               LOG_DEBUG("register_volumechange already in progress for label %d",
                                  btc_rc_vb.rc_vol_label);
               return;
            }
            else
              status=get_transaction(&p_transaction);
         }

         if(BT_STATUS_SUCCESS == status && NULL!=p_transaction)
         {
            btc_rc_vb.rc_vol_label=p_transaction->lbl;
            register_volumechange(btc_rc_vb.rc_vol_label);
         }
    }
#endif
}


/***************************************************************************
 *  Function       handle_rc_connect
 *
 *  - Argument:    tBTA_AV_RC_OPEN  RC open data structure
 *
 *  - Description: RC connection event handler
 *
 ***************************************************************************/
static void handle_rc_connect (tBTA_AV_RC_OPEN *p_rc_open)
{
    LOG_DEBUG("%s: rc_handle: %d", __FUNCTION__, p_rc_open->rc_handle);
#if (AVRC_CTLR_INCLUDED == TRUE)
    bt_bdaddr_t rc_addr;
#endif

    if(p_rc_open->status == BTA_AV_SUCCESS)
    {
        //check if already some RC is connected
        if (btc_rc_vb.rc_connected)
        {
            LOG_ERROR("Got RC OPEN in connected state, Connected RC: %d \
                and Current RC: %d", btc_rc_vb.rc_handle,p_rc_open->rc_handle );
            if ((btc_rc_vb.rc_handle != p_rc_open->rc_handle)
                && (bdcmp(btc_rc_vb.rc_addr, p_rc_open->peer_addr)))
            {
                LOG_DEBUG("Got RC connected for some other handle");
                BTA_AvCloseRc(p_rc_open->rc_handle);
                return;
            }
        }
        memcpy(btc_rc_vb.rc_addr, p_rc_open->peer_addr, sizeof(BD_ADDR));
        btc_rc_vb.rc_features = p_rc_open->peer_features;
        btc_rc_vb.rc_vol_label=MAX_LABEL;
        btc_rc_vb.rc_volume=MAX_VOLUME;

        btc_rc_vb.rc_connected = TRUE;
        btc_rc_vb.rc_handle = p_rc_open->rc_handle;

        /* on locally initiated connection we will get remote features as part of connect */
        if (btc_rc_vb.rc_features != 0)
            handle_rc_features();
#if UINPUT_INCLUDED
        if(uinput_driver_check() == BT_STATUS_SUCCESS)
        {
            init_uinput();
        }
#endif /* UINPUT_INCLUDED */
#if (AVRC_CTLR_INCLUDED == TRUE)
        bdcpy(rc_addr.address, btc_rc_vb.rc_addr);
        /* report connection state if device is AVRCP target */
        if (btc_rc_vb.rc_features & BTA_AV_FEAT_RCTG) {
	    esp_avrc_ct_cb_param_t param;
	    memset(&param, 0, sizeof(esp_avrc_ct_cb_param_t));
	    param.conn_stat.connected = true;
	    param.conn_stat.feat_mask = btc_rc_vb.rc_features;
	    memcpy(param.conn_stat.remote_bda, &rc_addr, sizeof(esp_bd_addr_t));
	    btc_avrc_ct_cb_to_app(ESP_AVRC_CT_CONNECTION_STATE_EVT, &param);
        }
#endif
    }
    else
    {
        LOG_ERROR("%s Connect failed with error code: %d",
            __FUNCTION__, p_rc_open->status);
        btc_rc_vb.rc_connected = FALSE;
    }
}

/***************************************************************************
 *  Function       handle_rc_disconnect
 *
 *  - Argument:    tBTA_AV_RC_CLOSE     RC close data structure
 *
 *  - Description: RC disconnection event handler
 *
 ***************************************************************************/
static void handle_rc_disconnect (tBTA_AV_RC_CLOSE *p_rc_close)
{
#if (AVRC_CTLR_INCLUDED == TRUE)
    bt_bdaddr_t rc_addr;
    tBTA_AV_FEAT features;
#endif
    LOG_DEBUG("%s: rc_handle: %d", __FUNCTION__, p_rc_close->rc_handle);
    if ((p_rc_close->rc_handle != btc_rc_vb.rc_handle)
        && (bdcmp(btc_rc_vb.rc_addr, p_rc_close->peer_addr)))
    {
        LOG_ERROR("Got disconnect of unknown device");
        return;
    }

    btc_rc_vb.rc_handle = 0;
    btc_rc_vb.rc_connected = FALSE;
    memset(btc_rc_vb.rc_addr, 0, sizeof(BD_ADDR));
    memset(btc_rc_vb.rc_notif, 0, sizeof(btc_rc_vb.rc_notif));
#if (AVRC_CTLR_INCLUDED == TRUE)
    features = btc_rc_vb.rc_features;
#endif
    btc_rc_vb.rc_features = 0;
    btc_rc_vb.rc_vol_label=MAX_LABEL;
    btc_rc_vb.rc_volume=MAX_VOLUME;
    init_all_transactions();
#if UINPUT_INCLUDED
    close_uinput();
#endif /* UINPUT_INCLUDED */
#if (AVRC_CTLR_INCLUDED == TRUE)
    bdcpy(rc_addr.address, btc_rc_vb.rc_addr);
#endif
    memset(btc_rc_vb.rc_addr, 0, sizeof(BD_ADDR));
#if (AVRC_CTLR_INCLUDED == TRUE)
    /* report connection state if device is AVRCP target */
    if (features & BTA_AV_FEAT_RCTG) {
	esp_avrc_ct_cb_param_t param;
	memset(&param, 0, sizeof(esp_avrc_ct_cb_param_t));
	param.conn_stat.connected = false;
	memcpy(param.conn_stat.remote_bda, &rc_addr, sizeof(esp_bd_addr_t));
	btc_avrc_ct_cb_to_app(ESP_AVRC_CT_CONNECTION_STATE_EVT, &param);
    }
#endif
}

/***************************************************************************
 *  Function       handle_rc_passthrough_cmd
 *
 *  - Argument:    tBTA_AV_RC rc_id   remote control command ID
 *                 tBTA_AV_STATE key_state status of key press
 *
 *  - Description: Remote control command handler
 *
 ***************************************************************************/
static void handle_rc_passthrough_cmd ( tBTA_AV_REMOTE_CMD *p_remote_cmd)
{
    const char *status;
    int pressed, i;

    LOG_DEBUG("%s: p_remote_cmd->rc_id=%d", __FUNCTION__, p_remote_cmd->rc_id);

    /* If AVRC is open and peer sends PLAY but there is no AVDT, then we queue-up this PLAY */
    if (p_remote_cmd)
    {
        /* queue AVRC PLAY if GAVDTP Open notification to app is pending (2 second timer) */
        if ((p_remote_cmd->rc_id == BTA_AV_RC_PLAY) && (!btc_av_is_connected()))
        {
            if (p_remote_cmd->key_state == AVRC_STATE_PRESS)
            {
                APPL_TRACE_WARNING("%s: AVDT not open, queuing the PLAY command", __FUNCTION__);
                btc_rc_vb.rc_pending_play = TRUE;
            }
            return;
        }

        if ((p_remote_cmd->rc_id == BTA_AV_RC_PAUSE) && (btc_rc_vb.rc_pending_play))
        {
            APPL_TRACE_WARNING("%s: Clear the pending PLAY on PAUSE received", __FUNCTION__);
            btc_rc_vb.rc_pending_play = FALSE;
            return;
        }
    }

    if ((p_remote_cmd->rc_id == BTA_AV_RC_STOP) && (!btc_av_stream_started_ready()))
    {
        APPL_TRACE_WARNING("%s: Stream suspended, ignore STOP cmd",__FUNCTION__);
        return;
    }

    if (p_remote_cmd->key_state == AVRC_STATE_RELEASE) {
        status = "released";
        pressed = 0;
    } else {
        status = "pressed";
        pressed = 1;
    }

    /* If this is Play/Pause command (press or release)  before processing, check the following
     * a voice call has ended recently
     * the remote device is not of type headset
     * If the above conditions meet, drop the Play/Pause command
     * This fix is to interop with certain carkits which sends an automatic  PLAY  or PAUSE
     * commands right after call ends
     */
    if((p_remote_cmd->rc_id == BTA_AV_RC_PLAY || p_remote_cmd->rc_id == BTA_AV_RC_PAUSE)
#ifdef BTC_HF_INCLUDED
       && (btc_hf_call_terminated_recently() == TRUE)
#endif
#if 0 // temporary hack since no btc_storage module is not ported for now
       && (check_cod( (const bt_bdaddr_t*)&(btc_rc_vb.rc_addr), COD_AV_HEADSETS) != TRUE)
#endif
       )
    {
        LOG_DEBUG("%s:Dropping the play/Pause command received right after call end cmd:%d",
                           __FUNCTION__,p_remote_cmd->rc_id);
        return;
    }

    if (p_remote_cmd->rc_id == BTA_AV_RC_FAST_FOR || p_remote_cmd->rc_id == BTA_AV_RC_REWIND) {
        BTC_HAL_CBACK(bt_rc_callbacks, passthrough_cmd_cb, p_remote_cmd->rc_id, pressed);
        return;
    }

    for (i = 0; key_map[i].name != NULL; i++) {
        if (p_remote_cmd->rc_id == key_map[i].avrcp) {
            LOG_DEBUG("%s: %s %s", __FUNCTION__, key_map[i].name, status);

           /* MusicPlayer uses a long_press_timeout of 1 second for PLAYPAUSE button
            * and maps that to autoshuffle. So if for some reason release for PLAY/PAUSE
            * comes 1 second after the press, the MediaPlayer UI goes into a bad state.
            * The reason for the delay could be sniff mode exit or some AVDTP procedure etc.
            * The fix is to generate a release right after the press and drown the 'actual'
            * release.
            */
            if ((key_map[i].release_quirk == 1) && (pressed == 0))
            {
                LOG_DEBUG("%s: AVRC %s Release Faked earlier, drowned now",
                                  __FUNCTION__, key_map[i].name);
                return;
            }
            send_key(uinput_fd, key_map[i].mapped_id, pressed);
            if ((key_map[i].release_quirk == 1) && (pressed == 1))
            {
                GKI_delay(30); // 30ms
                LOG_DEBUG("%s: AVRC %s Release quirk enabled, send release now",
                                  __FUNCTION__, key_map[i].name);
                send_key(uinput_fd, key_map[i].mapped_id, 0);
            }
            break;
        }
    }

    if (key_map[i].name == NULL)
        LOG_ERROR("%s AVRCP: unknown button 0x%02X %s", __FUNCTION__,
                        p_remote_cmd->rc_id, status);
}

/***************************************************************************
 *  Function       handle_rc_passthrough_rsp
 *
 *  - Argument:    tBTA_AV_REMOTE_RSP passthrough command response
 *
 *  - Description: Remote control passthrough response handler
 *
 ***************************************************************************/
static void handle_rc_passthrough_rsp ( tBTA_AV_REMOTE_RSP *p_remote_rsp)
{
#if (AVRC_CTLR_INCLUDED == TRUE)
    const char *status;
    if (btc_rc_vb.rc_features & BTA_AV_FEAT_RCTG)
    {
        int key_state;
        if (p_remote_rsp->key_state == AVRC_STATE_RELEASE)
        {
            status = "released";
            key_state = 1;
        }
        else
        {
            status = "pressed";
            key_state = 0;
        }

        LOG_DEBUG("%s: rc_id=%d status=%s", __FUNCTION__, p_remote_rsp->rc_id, status);

	do {
	    esp_avrc_ct_cb_param_t param;
	    memset(&param, 0, sizeof(esp_avrc_ct_cb_param_t));
            param.psth_rsp.tl = p_remote_rsp->label;
	    param.psth_rsp.key_code = p_remote_rsp->rc_id;
	    param.psth_rsp.key_state = key_state;
	    btc_avrc_ct_cb_to_app(ESP_AVRC_CT_PASSTHROUGH_RSP_EVT, &param);
	} while (0);
    }
    else
    {
        LOG_ERROR("%s DUT does not support AVRCP controller role", __FUNCTION__);
    }
#else
    LOG_ERROR("%s AVRCP controller role is not enabled", __FUNCTION__);
#endif
}

static void handle_uid_changed_notification(tBTA_AV_META_MSG *pmeta_msg, tAVRC_COMMAND *pavrc_command)
{
    tAVRC_RESPONSE avrc_rsp = {0};
    avrc_rsp.rsp.pdu = pavrc_command->pdu;
    avrc_rsp.rsp.status = AVRC_STS_NO_ERROR;
    avrc_rsp.rsp.opcode = pavrc_command->cmd.opcode;

    avrc_rsp.reg_notif.event_id = pavrc_command->reg_notif.event_id;
    avrc_rsp.reg_notif.param.uid_counter = 0;

    send_metamsg_rsp(pmeta_msg->rc_handle, pmeta_msg->label, AVRC_RSP_INTERIM, &avrc_rsp);
    send_metamsg_rsp(pmeta_msg->rc_handle, pmeta_msg->label, AVRC_RSP_CHANGED, &avrc_rsp);

}


/***************************************************************************
 *  Function       handle_rc_metamsg_cmd
 *
 *  - Argument:    tBTA_AV_VENDOR Structure containing the received
 *                          metamsg command
 *
 *  - Description: Remote control metamsg command handler (AVRCP 1.3)
 *
 ***************************************************************************/
static void handle_rc_metamsg_cmd (tBTA_AV_META_MSG *pmeta_msg)
{
    /* Parse the metamsg command and pass it on to BTL-IFS */
    UINT8             scratch_buf[512] = {0};
    tAVRC_COMMAND    avrc_command = {0};
    tAVRC_STS status;

    LOG_INFO("+ %s", __FUNCTION__);

    if (pmeta_msg->p_msg->hdr.opcode != AVRC_OP_VENDOR)
    {
        LOG_WARN("Invalid opcode: %x", pmeta_msg->p_msg->hdr.opcode);
        return;
    }
    if (pmeta_msg->len < 3)
    {
        LOG_WARN("Invalid length.Opcode: 0x%x, len: 0x%x", pmeta_msg->p_msg->hdr.opcode,
            pmeta_msg->len);
        return;
    }

    if (pmeta_msg->code >= AVRC_RSP_NOT_IMPL)
    {
#if (AVRC_ADV_CTRL_INCLUDED == TRUE)
{
     rc_transaction_t *transaction=NULL;
     transaction=get_transaction_by_lbl(pmeta_msg->label);
     if(NULL!=transaction)
     {
        handle_rc_metamsg_rsp(pmeta_msg);
     }
     else
     {
         LOG_DEBUG("%s:Discard vendor dependent rsp. code: %d label:%d.",
             __FUNCTION__, pmeta_msg->code, pmeta_msg->label);
     }
     return;
}
#else
{
        LOG_DEBUG("%s:Received vendor dependent rsp. code: %d len: %d. Not processing it.",
            __FUNCTION__, pmeta_msg->code, pmeta_msg->len);
        return;
}
#endif
      }

    status=AVRC_ParsCommand(pmeta_msg->p_msg, &avrc_command, scratch_buf, sizeof(scratch_buf));
    LOG_DEBUG("Received vendor command.code,PDU and label: %d, %d,%d",pmeta_msg->code,
                       avrc_command.cmd.pdu, pmeta_msg->label);

    if (status != AVRC_STS_NO_ERROR)
    {
        /* return error */
        LOG_WARN("%s: Error in parsing received metamsg command. status: 0x%02x",
            __FUNCTION__, status);
        send_reject_response(pmeta_msg->rc_handle, pmeta_msg->label, avrc_command.pdu, status);
    }
    else
    {
        /* if RegisterNotification, add it to our registered queue */

        if (avrc_command.cmd.pdu == AVRC_PDU_REGISTER_NOTIFICATION)
        {
            UINT8 event_id = avrc_command.reg_notif.event_id;
            LOG_INFO("%s:New register notification received.event_id:%s,label:0x%x,code:%x",
            __FUNCTION__,dump_rc_notification_event_id(event_id), pmeta_msg->label,pmeta_msg->code);
            btc_rc_vb.rc_notif[event_id-1].bNotify = TRUE;
            btc_rc_vb.rc_notif[event_id-1].label = pmeta_msg->label;

            if(event_id == AVRC_EVT_UIDS_CHANGE)
            {
                handle_uid_changed_notification(pmeta_msg, &avrc_command);
                return;
            }

        }

        LOG_INFO("%s: Passing received metamsg command to app. pdu: %s",
            __FUNCTION__, dump_rc_pdu(avrc_command.cmd.pdu));

        /* Since handle_rc_metamsg_cmd() itself is called from
         * btc context, no context switching is required. Invoke
         * btc_rc_upstreams_evt directly from here. */
        btc_rc_upstreams_evt((uint16_t)avrc_command.cmd.pdu, &avrc_command, pmeta_msg->code,
                               pmeta_msg->label);
    }
}

/***************************************************************************
 **
 ** Function       btc_rc_handler
 **
 ** Description    RC event handler
 **
 ***************************************************************************/
void btc_rc_handler(tBTA_AV_EVT event, tBTA_AV *p_data)
{
    LOG_DEBUG ("%s event:%s", __FUNCTION__, dump_rc_event(event));
    switch (event)
    {
        case BTA_AV_RC_OPEN_EVT:
        {
            LOG_DEBUG("Peer_features:%x", p_data->rc_open.peer_features);
            handle_rc_connect( &(p_data->rc_open) );
        }break;

        case BTA_AV_RC_CLOSE_EVT:
        {
            handle_rc_disconnect( &(p_data->rc_close) );
        }break;

        case BTA_AV_REMOTE_CMD_EVT:
        {
            LOG_DEBUG("rc_id:0x%x key_state:%d", p_data->remote_cmd.rc_id,
                               p_data->remote_cmd.key_state);
            handle_rc_passthrough_cmd( (&p_data->remote_cmd) );
        }
        break;
#if (AVRC_CTLR_INCLUDED == TRUE)
        case BTA_AV_REMOTE_RSP_EVT:
        {
            LOG_DEBUG("RSP: rc_id:0x%x key_state:%d", p_data->remote_rsp.rc_id,
                               p_data->remote_rsp.key_state);
            handle_rc_passthrough_rsp( (&p_data->remote_rsp) );
        }
        break;
#endif
        case BTA_AV_RC_FEAT_EVT:
        {
            LOG_DEBUG("Peer_features:%x", p_data->rc_feat.peer_features);
            btc_rc_vb.rc_features = p_data->rc_feat.peer_features;
            handle_rc_features();
        }
        break;
        case BTA_AV_META_MSG_EVT:
        {
            LOG_DEBUG("BTA_AV_META_MSG_EVT  code:%d label:%d", p_data->meta_msg.code,
                p_data->meta_msg.label);
            LOG_DEBUG("  company_id:0x%x len:%d handle:%d", p_data->meta_msg.company_id,
                p_data->meta_msg.len, p_data->meta_msg.rc_handle);
            /* handle the metamsg command */
            handle_rc_metamsg_cmd(&(p_data->meta_msg));
        }
        break;
        default:
            LOG_DEBUG("Unhandled RC event : 0x%x", event);
    }
}

/***************************************************************************
 **
 ** Function       btc_rc_get_connected_peer
 **
 ** Description    Fetches the connected headset's BD_ADDR if any
 **
 ***************************************************************************/
BOOLEAN btc_rc_get_connected_peer(BD_ADDR peer_addr)
{
    if (btc_rc_vb.rc_connected == TRUE) {
        bdcpy(peer_addr, btc_rc_vb.rc_addr);
        return TRUE;
    }
    return FALSE;
}

/***************************************************************************
 **
 ** Function       btc_rc_check_handle_pending_play
 **
 ** Description    Clears the queued PLAY command. if bSend is TRUE, forwards to app
 **
 ***************************************************************************/

/* clear the queued PLAY command. if bSend is TRUE, forward to app */
void btc_rc_check_handle_pending_play (BD_ADDR peer_addr, BOOLEAN bSendToApp)
{
    UNUSED(peer_addr);

    LOG_DEBUG("%s: bSendToApp=%d", __FUNCTION__, bSendToApp);
    if (btc_rc_vb.rc_pending_play)
    {
        if (bSendToApp)
        {
            tBTA_AV_REMOTE_CMD remote_cmd;
            APPL_TRACE_DEBUG("%s: Sending queued PLAYED event to app", __FUNCTION__);

            memset (&remote_cmd, 0, sizeof(tBTA_AV_REMOTE_CMD));
            remote_cmd.rc_handle  = btc_rc_vb.rc_handle;
            remote_cmd.rc_id      = AVRC_ID_PLAY;
            remote_cmd.hdr.ctype  = AVRC_CMD_CTRL;
            remote_cmd.hdr.opcode = AVRC_OP_PASS_THRU;

            /* delay sending to app, else there is a timing issue in the framework,
             ** which causes the audio to be on th device's speaker. Delay between
             ** OPEN & RC_PLAYs
            */
            GKI_delay (200);
            /* send to app - both PRESSED & RELEASED */
            remote_cmd.key_state  = AVRC_STATE_PRESS;
            handle_rc_passthrough_cmd( &remote_cmd );

            GKI_delay (100);

            remote_cmd.key_state  = AVRC_STATE_RELEASE;
            handle_rc_passthrough_cmd( &remote_cmd );
        }
        btc_rc_vb.rc_pending_play = FALSE;
    }
}

/* Generic reject response */
static void send_reject_response (UINT8 rc_handle, UINT8 label, UINT8 pdu, UINT8 status)
{
    UINT8 ctype = AVRC_RSP_REJ;
    tAVRC_RESPONSE avrc_rsp;
    BT_HDR *p_msg = NULL;
    memset (&avrc_rsp, 0, sizeof(tAVRC_RESPONSE));

    avrc_rsp.rsp.opcode = opcode_from_pdu(pdu);
    avrc_rsp.rsp.pdu    = pdu;
    avrc_rsp.rsp.status = status;

    if (AVRC_STS_NO_ERROR == (status = AVRC_BldResponse(rc_handle, &avrc_rsp, &p_msg)) )
    {
        LOG_DEBUG("%s:Sending error notification to handle:%d. pdu:%s,status:0x%02x",
            __FUNCTION__, rc_handle, dump_rc_pdu(pdu), status);
        BTA_AvMetaRsp(rc_handle, label, ctype, p_msg);
    }
}

/***************************************************************************
 *  Function       send_metamsg_rsp
 *
 *  - Argument:
 *                  rc_handle     RC handle corresponding to the connected RC
 *                  label            Label of the RC response
 *                  code            Response type
 *                  pmetamsg_resp    Vendor response
 *
 *  - Description: Remote control metamsg response handler (AVRCP 1.3)
 *
 ***************************************************************************/
static void send_metamsg_rsp (UINT8 rc_handle, UINT8 label, tBTA_AV_CODE code,
    tAVRC_RESPONSE *pmetamsg_resp)
{
    UINT8 ctype;

    if (!pmetamsg_resp)
    {
        LOG_WARN("%s: Invalid response received from application", __FUNCTION__);
        return;
    }

    LOG_INFO("+%s: rc_handle: %d, label: %d, code: 0x%02x, pdu: %s", __FUNCTION__,
        rc_handle, label, code, dump_rc_pdu(pmetamsg_resp->rsp.pdu));

    if (pmetamsg_resp->rsp.status != AVRC_STS_NO_ERROR)
    {
        ctype = AVRC_RSP_REJ;
    }
    else
    {
        if ( code < AVRC_RSP_NOT_IMPL)
        {
            if (code == AVRC_CMD_NOTIF)
            {
               ctype = AVRC_RSP_INTERIM;
            }
            else if (code == AVRC_CMD_STATUS)
            {
               ctype = AVRC_RSP_IMPL_STBL;
            }
            else
            {
               ctype = AVRC_RSP_ACCEPT;
            }
        }
        else
        {
            ctype = code;
        }
    }
    /* if response is for register_notification, make sure the rc has
    actually registered for this */
    if((pmetamsg_resp->rsp.pdu == AVRC_PDU_REGISTER_NOTIFICATION) && (code == AVRC_RSP_CHANGED))
    {
        BOOLEAN bSent = FALSE;
        UINT8   event_id = pmetamsg_resp->reg_notif.event_id;
        BOOLEAN bNotify = (btc_rc_vb.rc_connected) && (btc_rc_vb.rc_notif[event_id-1].bNotify);

        /* de-register this notification for a CHANGED response */
        btc_rc_vb.rc_notif[event_id-1].bNotify = FALSE;
        LOG_DEBUG("%s rc_handle: %d. event_id: 0x%02d bNotify:%u", __FUNCTION__,
             btc_rc_vb.rc_handle, event_id, bNotify);
        if (bNotify)
        {
            BT_HDR *p_msg = NULL;
            tAVRC_STS status;

            if (AVRC_STS_NO_ERROR == (status = AVRC_BldResponse(btc_rc_vb.rc_handle,
                pmetamsg_resp, &p_msg)) )
            {
                LOG_DEBUG("%s Sending notification to rc_handle: %d. event_id: 0x%02d",
                     __FUNCTION__, btc_rc_vb.rc_handle, event_id);
                bSent = TRUE;
                BTA_AvMetaRsp(btc_rc_vb.rc_handle, btc_rc_vb.rc_notif[event_id-1].label,
                    ctype, p_msg);
            }
            else
            {
                LOG_WARN("%s failed to build metamsg response. status: 0x%02x",
                    __FUNCTION__, status);
            }

        }

        if (!bSent)
        {
            LOG_DEBUG("%s: Notification not sent, as there are no RC connections or the \
                CT has not subscribed for event_id: %s", __FUNCTION__, dump_rc_notification_event_id(event_id));
        }
    }
    else
    {
        /* All other commands go here */

        BT_HDR *p_msg = NULL;
        tAVRC_STS status;

        status = AVRC_BldResponse(rc_handle, pmetamsg_resp, &p_msg);

        if (status == AVRC_STS_NO_ERROR)
        {
            BTA_AvMetaRsp(rc_handle, label, ctype, p_msg);
        }
        else
        {
            LOG_ERROR("%s: failed to build metamsg response. status: 0x%02x",
                __FUNCTION__, status);
        }
    }
}

static UINT8 opcode_from_pdu(UINT8 pdu)
{
    UINT8 opcode = 0;

    switch (pdu)
    {
    case AVRC_PDU_NEXT_GROUP:
    case AVRC_PDU_PREV_GROUP: /* pass thru */
        opcode  = AVRC_OP_PASS_THRU;
        break;

    default: /* vendor */
        opcode  = AVRC_OP_VENDOR;
        break;
    }

    return opcode;
}

/*******************************************************************************
**
** Function         btc_rc_upstreams_evt
**
** Description      Executes AVRC UPSTREAMS events in btc context.
**
** Returns          void
**
*******************************************************************************/
static void btc_rc_upstreams_evt(UINT16 event, tAVRC_COMMAND *pavrc_cmd, UINT8 ctype, UINT8 label)
{
    LOG_INFO("%s pdu: %s handle: 0x%x ctype:%x label:%x", __FUNCTION__,
        dump_rc_pdu(pavrc_cmd->pdu), btc_rc_vb.rc_handle, ctype, label);

    switch (event)
    {
        case AVRC_PDU_GET_PLAY_STATUS:
        {
            FILL_PDU_QUEUE(IDX_GET_PLAY_STATUS_RSP, ctype, label, TRUE)
            BTC_HAL_CBACK(bt_rc_callbacks, get_play_status_cb);
        }
        break;
        case AVRC_PDU_LIST_PLAYER_APP_ATTR:
        case AVRC_PDU_LIST_PLAYER_APP_VALUES:
        case AVRC_PDU_GET_CUR_PLAYER_APP_VALUE:
        case AVRC_PDU_SET_PLAYER_APP_VALUE:
        case AVRC_PDU_GET_PLAYER_APP_ATTR_TEXT:
        case AVRC_PDU_GET_PLAYER_APP_VALUE_TEXT:
        {
            /* TODO: Add support for Application Settings */
            send_reject_response (btc_rc_vb.rc_handle, label, pavrc_cmd->pdu, AVRC_STS_BAD_CMD);
        }
        break;
        case AVRC_PDU_GET_ELEMENT_ATTR:
        {
            btrc_media_attr_t element_attrs[BTRC_MAX_ELEM_ATTR_SIZE];
            UINT8 num_attr;
             memset(&element_attrs, 0, sizeof(element_attrs));
            if (pavrc_cmd->get_elem_attrs.num_attr == 0)
            {
                /* CT requests for all attributes */
                int attr_cnt;
                num_attr = BTRC_MAX_ELEM_ATTR_SIZE;
                for (attr_cnt = 0; attr_cnt < BTRC_MAX_ELEM_ATTR_SIZE; attr_cnt++)
                {
                    element_attrs[attr_cnt] = attr_cnt + 1;
                }
            }
            else if (pavrc_cmd->get_elem_attrs.num_attr == 0xFF)
            {
                /* 0xff indicates, no attributes requested - reject */
                send_reject_response (btc_rc_vb.rc_handle, label, pavrc_cmd->pdu,
                    AVRC_STS_BAD_PARAM);
                return;
            }
            else
            {
                int attr_cnt, filled_attr_count;

                num_attr = 0;
                /* Attribute IDs from 1 to AVRC_MAX_NUM_MEDIA_ATTR_ID are only valid,
                 * hence HAL definition limits the attributes to AVRC_MAX_NUM_MEDIA_ATTR_ID.
                 * Fill only valid entries.
                 */
                for (attr_cnt = 0; (attr_cnt < pavrc_cmd->get_elem_attrs.num_attr) &&
                    (num_attr < AVRC_MAX_NUM_MEDIA_ATTR_ID); attr_cnt++)
                {
                    if ((pavrc_cmd->get_elem_attrs.attrs[attr_cnt] > 0) &&
                        (pavrc_cmd->get_elem_attrs.attrs[attr_cnt] <= AVRC_MAX_NUM_MEDIA_ATTR_ID))
                    {
                        /* Skip the duplicate entries : PTS sends duplicate entries for Fragment cases
                         */
                        for (filled_attr_count = 0; filled_attr_count < num_attr; filled_attr_count++)
                        {
                            if (element_attrs[filled_attr_count] == pavrc_cmd->get_elem_attrs.attrs[attr_cnt])
                                break;
                        }
                        if (filled_attr_count == num_attr)
                        {
                            element_attrs[num_attr] = pavrc_cmd->get_elem_attrs.attrs[attr_cnt];
                            num_attr++;
                        }
                    }
                }
            }
            FILL_PDU_QUEUE(IDX_GET_ELEMENT_ATTR_RSP, ctype, label, TRUE);
            BTC_HAL_CBACK(bt_rc_callbacks, get_element_attr_cb, num_attr, element_attrs);
        }
        break;
        case AVRC_PDU_REGISTER_NOTIFICATION:
        {
            if(pavrc_cmd->reg_notif.event_id == BTRC_EVT_PLAY_POS_CHANGED &&
                pavrc_cmd->reg_notif.param == 0)
            {
                LOG_WARN("%s Device registering position changed with illegal param 0.",
                    __FUNCTION__);
                send_reject_response (btc_rc_vb.rc_handle, label, pavrc_cmd->pdu, AVRC_STS_BAD_PARAM);
                /* de-register this notification for a rejected response */
                btc_rc_vb.rc_notif[BTRC_EVT_PLAY_POS_CHANGED - 1].bNotify = FALSE;
                return;
            }
            BTC_HAL_CBACK(bt_rc_callbacks, register_notification_cb, pavrc_cmd->reg_notif.event_id,
                pavrc_cmd->reg_notif.param);
        }
        break;
        case AVRC_PDU_INFORM_DISPLAY_CHARSET:
        {
            tAVRC_RESPONSE avrc_rsp;
            LOG_INFO("%s() AVRC_PDU_INFORM_DISPLAY_CHARSET", __FUNCTION__);
            if(btc_rc_vb.rc_connected == TRUE)
            {
                memset(&(avrc_rsp.inform_charset), 0, sizeof(tAVRC_RSP));
                avrc_rsp.inform_charset.opcode=opcode_from_pdu(AVRC_PDU_INFORM_DISPLAY_CHARSET);
                avrc_rsp.inform_charset.pdu=AVRC_PDU_INFORM_DISPLAY_CHARSET;
                avrc_rsp.inform_charset.status=AVRC_STS_NO_ERROR;
                send_metamsg_rsp(btc_rc_vb.rc_handle, label, ctype, &avrc_rsp);
            }
        }
        break;
        default:
        {
        send_reject_response (btc_rc_vb.rc_handle, label, pavrc_cmd->pdu,
            (pavrc_cmd->pdu == AVRC_PDU_SEARCH)?AVRC_STS_SEARCH_NOT_SUP:AVRC_STS_BAD_CMD);
        return;
        }
        break;
    }

}


/*******************************************************************************
**
** Function         btc_rc_upstreams_rsp_evt
**
** Description      Executes AVRC UPSTREAMS response events in btc context.
**
** Returns          void
**
*******************************************************************************/
static void btc_rc_upstreams_rsp_evt(UINT16 event, tAVRC_RESPONSE *pavrc_resp, UINT8 ctype, UINT8 label)
{
    LOG_INFO("%s pdu: %s handle: 0x%x ctype:%x label:%x", __FUNCTION__,
        dump_rc_pdu(pavrc_resp->pdu), btc_rc_vb.rc_handle, ctype, label);

#if (AVRC_ADV_CTRL_INCLUDED == TRUE)
    switch (event)
    {
        case AVRC_PDU_REGISTER_NOTIFICATION:
        {
             if(AVRC_RSP_CHANGED==ctype)
                 btc_rc_vb.rc_volume=pavrc_resp->reg_notif.param.volume;
             BTC_HAL_CBACK(bt_rc_callbacks, volume_change_cb, pavrc_resp->reg_notif.param.volume,ctype)
        }
        break;

        case AVRC_PDU_SET_ABSOLUTE_VOLUME:
        {
            LOG_DEBUG("Set absolute volume change event received: volume %d,ctype %d",
                pavrc_resp->volume.volume,ctype);
            if(AVRC_RSP_ACCEPT==ctype)
                btc_rc_vb.rc_volume=pavrc_resp->volume.volume;
            BTC_HAL_CBACK(bt_rc_callbacks,volume_change_cb,pavrc_resp->volume.volume,ctype)
        }
        break;

        default:
            return;
    }
#endif
}

/************************************************************************************
**  AVRCP API Functions
************************************************************************************/

#if 0
/*******************************************************************************
**
** Function         esp_avrc_ct_register_callback
**
** Description      Register AVRCP controller callback function
**
** Returns          esp_err_t
**
*******************************************************************************/
esp_err_t esp_avrc_ct_register_callback(esp_avrc_ct_cb_t callback)
{
    if (bt_rc_ctrl_callback)
	return ESP_FAIL;

    bt_rc_ctrl_callback = callback;
    return ESP_OK;
}
#endif

/*******************************************************************************
**
** Function         btc_avrc_ct_init
**
** Description      Initializes the AVRC interface
**
** Returns          esp_err_t
**
*******************************************************************************/
static void btc_avrc_ct_init(void)
{
    LOG_INFO("## %s ##", __FUNCTION__);

    memset (&btc_rc_vb, 0, sizeof(btc_rc_vb));
    btc_rc_vb.rc_vol_label=MAX_LABEL;
    btc_rc_vb.rc_volume=MAX_VOLUME;
    lbl_init();
}


/***************************************************************************
**
** Function         register_volumechange
**
** Description     Register for volume change notification from remote side.
**
** Returns          void
**
***************************************************************************/

static void register_volumechange (UINT8 lbl)
{
    tAVRC_COMMAND avrc_cmd = {0};
    BT_HDR *p_msg = NULL;
    tAVRC_STS BldResp=AVRC_STS_BAD_CMD;
    rc_transaction_t *p_transaction=NULL;

    LOG_DEBUG("%s called with label:%d",__FUNCTION__,lbl);

    avrc_cmd.cmd.opcode=0x00;
    avrc_cmd.pdu = AVRC_PDU_REGISTER_NOTIFICATION;
    avrc_cmd.reg_notif.event_id = AVRC_EVT_VOLUME_CHANGE;
    avrc_cmd.reg_notif.status = AVRC_STS_NO_ERROR;

    BldResp=AVRC_BldCommand(&avrc_cmd, &p_msg);
    if(AVRC_STS_NO_ERROR==BldResp && p_msg)
    {
         p_transaction=get_transaction_by_lbl(lbl);
         if(NULL!=p_transaction)
         {
             BTA_AvMetaCmd(btc_rc_vb.rc_handle,p_transaction->lbl, AVRC_CMD_NOTIF, p_msg);
             LOG_DEBUG("%s:BTA_AvMetaCmd called",__FUNCTION__);
         }
         else
         {
            if(NULL!=p_msg)
               GKI_freebuf(p_msg);
            LOG_ERROR("%s transaction not obtained with label: %d",__FUNCTION__,lbl);
         }
    }
    else
        LOG_ERROR("%s failed to build command:%d",__FUNCTION__,BldResp);
}


/***************************************************************************
**
** Function         handle_rc_metamsg_rsp
**
** Description      Handle RC metamessage response
**
** Returns          void
**
***************************************************************************/
static void handle_rc_metamsg_rsp(tBTA_AV_META_MSG *pmeta_msg)
{
    tAVRC_RESPONSE    avrc_response = {0};
    UINT8             scratch_buf[512] = {0};
    tAVRC_STS status = BT_STATUS_UNSUPPORTED;

    if(AVRC_OP_VENDOR==pmeta_msg->p_msg->hdr.opcode &&(AVRC_RSP_CHANGED==pmeta_msg->code
      || AVRC_RSP_INTERIM==pmeta_msg->code || AVRC_RSP_ACCEPT==pmeta_msg->code
      || AVRC_RSP_REJ==pmeta_msg->code || AVRC_RSP_NOT_IMPL==pmeta_msg->code))
    {
        status=AVRC_ParsResponse(pmeta_msg->p_msg, &avrc_response, scratch_buf, sizeof(scratch_buf));
        LOG_DEBUG("%s: code %d,event ID %d,PDU %x,parsing status %d, label:%d",
          __FUNCTION__,pmeta_msg->code,avrc_response.reg_notif.event_id,avrc_response.reg_notif.pdu,
          status, pmeta_msg->label);

        if (status != AVRC_STS_NO_ERROR)
        {
            if(AVRC_PDU_REGISTER_NOTIFICATION==avrc_response.rsp.pdu
                && AVRC_EVT_VOLUME_CHANGE==avrc_response.reg_notif.event_id
                && btc_rc_vb.rc_vol_label==pmeta_msg->label)
            {
                btc_rc_vb.rc_vol_label=MAX_LABEL;
                release_transaction(btc_rc_vb.rc_vol_label);
            }
            else if(AVRC_PDU_SET_ABSOLUTE_VOLUME==avrc_response.rsp.pdu)
            {
                release_transaction(pmeta_msg->label);
            }
            return;
        }
        else if(AVRC_PDU_REGISTER_NOTIFICATION==avrc_response.rsp.pdu
            && AVRC_EVT_VOLUME_CHANGE==avrc_response.reg_notif.event_id
            && btc_rc_vb.rc_vol_label!=pmeta_msg->label)
            {
                // Just discard the message, if the device sends back with an incorrect label
                LOG_DEBUG("%s:Discarding register notfn in rsp.code: %d and label %d",
                __FUNCTION__, pmeta_msg->code, pmeta_msg->label);
                return;
            }
    }
    else
    {
        LOG_DEBUG("%s:Received vendor dependent in adv ctrl rsp. code: %d len: %d. Not processing it.",
        __FUNCTION__, pmeta_msg->code, pmeta_msg->len);
        return;
    }

    if(AVRC_PDU_REGISTER_NOTIFICATION==avrc_response.rsp.pdu
        && AVRC_EVT_VOLUME_CHANGE==avrc_response.reg_notif.event_id
        && AVRC_RSP_CHANGED==pmeta_msg->code)
     {
         /* re-register for volume change notification */
         // Do not re-register for rejected case, as it might get into endless loop
         register_volumechange(btc_rc_vb.rc_vol_label);
     }
     else if(AVRC_PDU_SET_ABSOLUTE_VOLUME==avrc_response.rsp.pdu)
     {
          /* free up the label here */
          release_transaction(pmeta_msg->label);
     }

     LOG_INFO("%s: Passing received metamsg response to app. pdu: %s",
             __FUNCTION__, dump_rc_pdu(avrc_response.pdu));
     btc_rc_upstreams_rsp_evt((uint16_t)avrc_response.rsp.pdu, &avrc_response, pmeta_msg->code,
                                pmeta_msg->label);
}


/***************************************************************************
**
** Function         cleanup_ctrl
**
** Description      Closes the AVRC Controller interface
**
** Returns          void
**
***************************************************************************/
static void btc_avrc_ct_deinit(void)
{
    LOG_INFO("## %s ##", __FUNCTION__);

    memset(&btc_rc_vb, 0, sizeof(btc_rc_cb_t));
    lbl_destroy();
    LOG_INFO("## %s ## completed", __FUNCTION__);
}

static bt_status_t btc_avrc_ct_send_passthrough_cmd(uint8_t tl, uint8_t key_code, uint8_t key_state)
{
    tAVRC_STS status = BT_STATUS_UNSUPPORTED;
    if (tl >= 16 ||
        key_state > ESP_AVRC_PT_CMD_STATE_RELEASED) {
        return ESP_ERR_INVALID_ARG;
    }
#if (AVRC_CTLR_INCLUDED == TRUE)
    CHECK_ESP_RC_CONNECTED;
    LOG_DEBUG("%s: key-code: %d, key-state: %d", __FUNCTION__,
                                                    key_code, key_state);
    if (btc_rc_vb.rc_features & BTA_AV_FEAT_RCTG)
    {
        BTA_AvRemoteCmd(btc_rc_vb.rc_handle, tl,
                        (tBTA_AV_RC)key_code, (tBTA_AV_STATE)key_state);
        status =  BT_STATUS_SUCCESS;
        LOG_INFO("%s: succesfully sent passthrough command to BTA", __FUNCTION__);
    }
    else
    {
        status = BT_STATUS_FAIL;
        LOG_DEBUG("%s: feature not supported", __FUNCTION__);
    }
#else
    LOG_DEBUG("%s: feature not enabled", __FUNCTION__);
#endif

    return status;
}

/*******************************************************************************
**      Function         initialize_transaction
**
**      Description    Initializes fields of the transaction structure
**
**      Returns          void
*******************************************************************************/
static void initialize_transaction(int lbl)
{
    pthread_mutex_lock(&device.lbllock);
    if(lbl < MAX_TRANSACTIONS_PER_SESSION)
    {
       device.transaction[lbl].lbl = lbl;
       device.transaction[lbl].in_use=FALSE;
       device.transaction[lbl].handle=0;
    }
    pthread_mutex_unlock(&device.lbllock);
}

/*******************************************************************************
**      Function         lbl_init
**
**      Description    Initializes label structures and mutexes.
**
**      Returns         void
*******************************************************************************/
void lbl_init()
{
    memset(&device,0,sizeof(rc_device_t));
    // pthread_mutexattr_t attr;
    // pthread_mutexattr_init(&attr);
    // pthread_mutex_init(&(device.lbllock), &attr);
    // pthread_mutexattr_destroy(&attr);
    pthread_mutex_init(&(device.lbllock), NULL);
    
    init_all_transactions();
}

/*******************************************************************************
**
** Function         init_all_transactions
**
** Description    Initializes all transactions
**
** Returns          void
*******************************************************************************/
void init_all_transactions()
{
    UINT8 txn_indx=0;
    for(txn_indx=0; txn_indx < MAX_TRANSACTIONS_PER_SESSION; txn_indx++)
    {
        initialize_transaction(txn_indx);
    }
}

/*******************************************************************************
**
** Function         get_transaction_by_lbl
**
** Description    Will return a transaction based on the label. If not inuse
**                     will return an error.
**
** Returns          bt_status_t
*******************************************************************************/
rc_transaction_t *get_transaction_by_lbl(UINT8 lbl)
{
    rc_transaction_t *transaction = NULL;
    pthread_mutex_lock(&device.lbllock);

    /* Determine if this is a valid label */
    if (lbl < MAX_TRANSACTIONS_PER_SESSION)
    {
        if (FALSE==device.transaction[lbl].in_use)
        {
            transaction = NULL;
        }
        else
        {
            transaction = &(device.transaction[lbl]);
            LOG_DEBUG("%s: Got transaction.label: %d",__FUNCTION__,lbl);
        }
    }

    pthread_mutex_unlock(&device.lbllock);
    return transaction;
}

/*******************************************************************************
**
** Function         get_transaction
**
** Description    Obtains the transaction details.
**
** Returns          bt_status_t
*******************************************************************************/

bt_status_t  get_transaction(rc_transaction_t **ptransaction)
{
    bt_status_t result = BT_STATUS_NOMEM;
    UINT8 i=0;
    pthread_mutex_lock(&device.lbllock);

    // Check for unused transactions
    for (i=0; i<MAX_TRANSACTIONS_PER_SESSION; i++)
    {
        if (FALSE==device.transaction[i].in_use)
        {
            LOG_DEBUG("%s:Got transaction.label: %d",__FUNCTION__,device.transaction[i].lbl);
            device.transaction[i].in_use = TRUE;
            *ptransaction = &(device.transaction[i]);
            result = BT_STATUS_SUCCESS;
            break;
        }
    }

    pthread_mutex_unlock(&device.lbllock);
    return result;
}


/*******************************************************************************
**
** Function         release_transaction
**
** Description    Will release a transaction for reuse
**
** Returns          bt_status_t
*******************************************************************************/
void release_transaction(UINT8 lbl)
{
    rc_transaction_t *transaction = get_transaction_by_lbl(lbl);

    /* If the transaction is in use... */
    if (transaction != NULL)
    {
        LOG_DEBUG("%s: lbl: %d", __FUNCTION__, lbl);
        initialize_transaction(lbl);
    }
}

/*******************************************************************************
**
** Function         lbl_destroy
**
** Description    Cleanup of the mutex
**
** Returns          void
*******************************************************************************/
void lbl_destroy()
{
    pthread_mutex_destroy(&(device.lbllock));
}

void btc_avrc_evt_handler(btc_msg_t *msg)
{
    btc_avrc_args_t *arg = (btc_avrc_args_t *)(msg->arg);
    switch (msg->act) {
    case BTC_AVRC_CTRL_API_INIT_EVT: {
        btc_avrc_ct_init();
        // todo: callback to application
        break;
    }
    case BTC_AVRC_CTRL_API_DEINIT_EVT: {
        btc_avrc_ct_deinit();
        // todo: callback to application
        break;
    }
    case BTC_AVRC_CTRL_API_SND_PTCMD_EVT: {
        btc_avrc_ct_send_passthrough_cmd(arg->pt_cmd.tl, arg->pt_cmd.key_code, arg->pt_cmd.key_state);
        // todo: callback to application
        break;
    }
    default:
        LOG_WARN("%s : unhandled event: %d\n", __FUNCTION__, msg->act);
    }
}

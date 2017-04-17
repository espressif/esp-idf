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
#include "bt_target.h"
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
#include "esp_avrc_api.h"
#if BTC_AV_INCLUDED

/*****************************************************************************
**  Constants & Macros
******************************************************************************/
/* for AVRC 1.4 need to change this */
#define MAX_RC_NOTIFICATIONS AVRC_EVT_APP_SETTING_CHANGE

#define MAX_VOLUME 128
#define MAX_LABEL 16
#define MAX_TRANSACTIONS_PER_SESSION 16
#define MAX_CMD_QUEUE_LEN 8

#define CHECK_ESP_RC_CONNECTED       do { \
        LOG_DEBUG("## %s ##", __FUNCTION__); \
        if (btc_rc_vb.rc_connected == FALSE) { \
            LOG_WARN("Function %s() called when RC is not connected", __FUNCTION__); \
        return ESP_ERR_INVALID_STATE; \
        } \
    } while (0)

/*****************************************************************************
**  Local type definitions
******************************************************************************/
typedef struct {
    UINT8 bNotify;
    UINT8 label;
} btc_rc_reg_notifications_t;

typedef struct {
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

typedef struct {
    pthread_mutex_t lbllock;
    rc_transaction_t transaction[MAX_TRANSACTIONS_PER_SESSION];
} rc_device_t;

rc_device_t device;

static void handle_rc_connect (tBTA_AV_RC_OPEN *p_rc_open);
static void handle_rc_disconnect (tBTA_AV_RC_CLOSE *p_rc_close);
static void handle_rc_passthrough_rsp ( tBTA_AV_REMOTE_RSP *p_remote_rsp);

/*****************************************************************************
**  Static variables
******************************************************************************/
static btc_rc_cb_t btc_rc_vb;

/*****************************************************************************
**  Externs
******************************************************************************/
/*****************************************************************************
**  Static functions
******************************************************************************/
static inline void btc_avrc_ct_cb_to_app(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *param)
{
    esp_avrc_ct_cb_t btc_avrc_cb = (esp_avrc_ct_cb_t)btc_profile_cb_get(BTC_PID_AVRC);
    if (btc_avrc_cb) {
        btc_avrc_cb(event, param);
    }
}

static void handle_rc_features(void)
{
    btrc_remote_features_t rc_features = BTRC_FEAT_NONE;
    bt_bdaddr_t rc_addr;
    bdcpy(rc_addr.address, btc_rc_vb.rc_addr);

    // TODO(eisenbach): If devices need to be blacklisted for absolute
    // volume, it should be added to device/include/interop_database.h
    // For now, everything goes... If blacklisting is necessary, exclude
    // the following bit here:
    //    btc_rc_vb.rc_features &= ~BTA_AV_FEAT_ADV_CTRL;

    if (btc_rc_vb.rc_features & BTA_AV_FEAT_BROWSE) {
        rc_features |= BTRC_FEAT_BROWSE;
    }

    if ( (btc_rc_vb.rc_features & BTA_AV_FEAT_ADV_CTRL) &&
            (btc_rc_vb.rc_features & BTA_AV_FEAT_RCTG)) {
        rc_features |= BTRC_FEAT_ABSOLUTE_VOLUME;
    }

    if (btc_rc_vb.rc_features & BTA_AV_FEAT_METADATA) {
        rc_features |= BTRC_FEAT_METADATA;
    }

    LOG_DEBUG("%s: rc_features=0x%x", __FUNCTION__, rc_features);
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

    if (p_rc_open->status == BTA_AV_SUCCESS) {
        //check if already some RC is connected
        if (btc_rc_vb.rc_connected) {
            LOG_ERROR("Got RC OPEN in connected state, Connected RC: %d \
                and Current RC: %d", btc_rc_vb.rc_handle, p_rc_open->rc_handle );
            if ((btc_rc_vb.rc_handle != p_rc_open->rc_handle)
                    && (bdcmp(btc_rc_vb.rc_addr, p_rc_open->peer_addr))) {
                LOG_DEBUG("Got RC connected for some other handle");
                BTA_AvCloseRc(p_rc_open->rc_handle);
                return;
            }
        }
        memcpy(btc_rc_vb.rc_addr, p_rc_open->peer_addr, sizeof(BD_ADDR));
        btc_rc_vb.rc_features = p_rc_open->peer_features;
        btc_rc_vb.rc_vol_label = MAX_LABEL;
        btc_rc_vb.rc_volume = MAX_VOLUME;

        btc_rc_vb.rc_connected = TRUE;
        btc_rc_vb.rc_handle = p_rc_open->rc_handle;

        /* on locally initiated connection we will get remote features as part of connect */
        if (btc_rc_vb.rc_features != 0) {
            handle_rc_features();
        }
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
    } else {
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
            && (bdcmp(btc_rc_vb.rc_addr, p_rc_close->peer_addr))) {
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
    btc_rc_vb.rc_vol_label = MAX_LABEL;
    btc_rc_vb.rc_volume = MAX_VOLUME;
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
    if (btc_rc_vb.rc_features & BTA_AV_FEAT_RCTG) {
        int key_state;
        if (p_remote_rsp->key_state == AVRC_STATE_RELEASE) {
            status = "released";
            key_state = 1;
        } else {
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
    } else {
        LOG_ERROR("%s DUT does not support AVRCP controller role", __FUNCTION__);
    }
#else
    LOG_ERROR("%s AVRCP controller role is not enabled", __FUNCTION__);
#endif
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
    switch (event) {
    case BTA_AV_RC_OPEN_EVT: {
        LOG_DEBUG("Peer_features:%x", p_data->rc_open.peer_features);
        handle_rc_connect( &(p_data->rc_open) );
    } break;

    case BTA_AV_RC_CLOSE_EVT: {
        handle_rc_disconnect( &(p_data->rc_close) );
    } break;

#if (AVRC_CTLR_INCLUDED == TRUE)
    case BTA_AV_REMOTE_RSP_EVT: {
        LOG_DEBUG("RSP: rc_id:0x%x key_state:%d", p_data->remote_rsp.rc_id,
                  p_data->remote_rsp.key_state);
        handle_rc_passthrough_rsp( (&p_data->remote_rsp) );
    }
    break;
#endif
    case BTA_AV_RC_FEAT_EVT: {
        LOG_DEBUG("Peer_features:%x", p_data->rc_feat.peer_features);
        btc_rc_vb.rc_features = p_data->rc_feat.peer_features;
        handle_rc_features();
    }
    break;

    // below events are not handled for now
    case BTA_AV_META_MSG_EVT:
    case BTA_AV_REMOTE_CMD_EVT:
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

/************************************************************************************
**  AVRCP API Functions
************************************************************************************/

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
    LOG_DEBUG("## %s ##", __FUNCTION__);

    memset (&btc_rc_vb, 0, sizeof(btc_rc_vb));
    btc_rc_vb.rc_vol_label = MAX_LABEL;
    btc_rc_vb.rc_volume = MAX_VOLUME;
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
    if (btc_rc_vb.rc_features & BTA_AV_FEAT_RCTG) {
        BTA_AvRemoteCmd(btc_rc_vb.rc_handle, tl,
                        (tBTA_AV_RC)key_code, (tBTA_AV_STATE)key_state);
        status =  BT_STATUS_SUCCESS;
        LOG_INFO("%s: succesfully sent passthrough command to BTA", __FUNCTION__);
    } else {
        status = BT_STATUS_FAIL;
        LOG_DEBUG("%s: feature not supported", __FUNCTION__);
    }
#else
    LOG_DEBUG("%s: feature not enabled", __FUNCTION__);
#endif

    return status;
}

void btc_avrc_call_handler(btc_msg_t *msg)
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

#endif /* #if BTC_AV_INCLUDED */

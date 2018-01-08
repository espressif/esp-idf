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
#include "btc_common.h"
#include "btc_util.h"
#include "btc_av.h"
#include "btc_avrc.h"
#include "btc_manage.h"
#include "esp_avrc_api.h"
#include "mutex.h"
#include "allocator.h"

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
    osi_mutex_t lbllock;
    rc_transaction_t transaction[MAX_TRANSACTIONS_PER_SESSION];
} rc_device_t;

rc_device_t device;

static void handle_rc_connect (tBTA_AV_RC_OPEN *p_rc_open);
static void handle_rc_disconnect (tBTA_AV_RC_CLOSE *p_rc_close);
static void handle_rc_passthrough_rsp ( tBTA_AV_REMOTE_RSP *p_remote_rsp);
static void handle_rc_metadata_rsp ( tBTA_AV_META_MSG *p_remote_rsp);

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
    esp_avrc_ct_cb_param_t param;
    memset(&param, 0, sizeof(esp_avrc_ct_cb_param_t));
    param.rmt_feats.feat_mask = btc_rc_vb.rc_features;
    memcpy(param.rmt_feats.remote_bda, btc_rc_vb.rc_addr, sizeof(esp_bd_addr_t));
    btc_avrc_ct_cb_to_app(ESP_AVRC_CT_REMOTE_FEATURES_EVT, &param);
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
    bt_bdaddr_t rc_addr;

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

        bdcpy(rc_addr.address, btc_rc_vb.rc_addr);

        esp_avrc_ct_cb_param_t param;
        memset(&param, 0, sizeof(esp_avrc_ct_cb_param_t));
        param.conn_stat.connected = true;
        memcpy(param.conn_stat.remote_bda, &rc_addr, sizeof(esp_bd_addr_t));
        btc_avrc_ct_cb_to_app(ESP_AVRC_CT_CONNECTION_STATE_EVT, &param);
        
        /* on locally initiated connection we will get remote features as part of connect */
        if (p_rc_open->sdp_disc_done == TRUE) {
            handle_rc_features();
        }
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
    bt_bdaddr_t rc_addr;

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

    btc_rc_vb.rc_features = 0;
    btc_rc_vb.rc_vol_label = MAX_LABEL;
    btc_rc_vb.rc_volume = MAX_VOLUME;

    bdcpy(rc_addr.address, btc_rc_vb.rc_addr);

    memset(btc_rc_vb.rc_addr, 0, sizeof(BD_ADDR));

    /* report connection state */
    esp_avrc_ct_cb_param_t param;
    memset(&param, 0, sizeof(esp_avrc_ct_cb_param_t));
    param.conn_stat.connected = false;
    memcpy(param.conn_stat.remote_bda, &rc_addr, sizeof(esp_bd_addr_t));
    btc_avrc_ct_cb_to_app(ESP_AVRC_CT_CONNECTION_STATE_EVT, &param);
}

static void handle_rc_attributes_rsp ( tAVRC_MSG_VENDOR *vendor_msg)
{
    uint8_t attr_count = vendor_msg->p_vendor_data[4];
    int attr_index = 5;
    int attr_length = 0;
    uint32_t attr_id = 0;

    //Check if there are any attributes
    if (attr_count < 1) {
        return;
    }

    esp_avrc_ct_cb_param_t param[attr_count];
    memset(&param[0], 0, sizeof(esp_avrc_ct_cb_param_t) * attr_count);

    for (int i = 0; i < attr_count; i++) {
        attr_length = (int) vendor_msg->p_vendor_data[7 + attr_index] | vendor_msg->p_vendor_data[6 + attr_index] << 8;

        //Received attribute text is not null terminated, so it's useful to know it's length
        param[i].meta_rsp.attr_length = attr_length;
        param[i].meta_rsp.attr_text = &vendor_msg->p_vendor_data[8 + attr_index];

        attr_id = vendor_msg->p_vendor_data[3 + attr_index] |
                  vendor_msg->p_vendor_data[2 + attr_index] << 8 | vendor_msg->p_vendor_data[1 + attr_index] << 16 |
                  vendor_msg->p_vendor_data[attr_index] << 24;

        //Convert to mask id
        param[i].meta_rsp.attr_id = (1 << (attr_id - 1));

        btc_avrc_ct_cb_to_app(ESP_AVRC_CT_METADATA_RSP_EVT, &param[i]);

        attr_index += (int) vendor_msg->p_vendor_data[7 + attr_index] + 8;
    }
}

static void handle_rc_notification_rsp ( tAVRC_MSG_VENDOR *vendor_msg)
{
    esp_avrc_ct_cb_param_t param;

    param.change_ntf.event_id = vendor_msg->p_vendor_data[4];

    param.change_ntf.event_parameter = vendor_msg->p_vendor_data[5] << 24 | vendor_msg->p_vendor_data[6] << 16 |
                                       vendor_msg->p_vendor_data[7] << 8 | vendor_msg->p_vendor_data[8];

    btc_avrc_ct_cb_to_app(ESP_AVRC_CT_CHANGE_NOTIFY_EVT, &param);
}

/***************************************************************************
 *  Function       handle_rc_metadata_rsp
 *
 *  - Argument:    tBTA_AV_META_MSG metadata command response
 *
 *  - Description: Vendor metadata response handler
 *
 ***************************************************************************/
static void handle_rc_metadata_rsp ( tBTA_AV_META_MSG *p_remote_rsp)
{
#if (AVRC_METADATA_INCLUDED == TRUE)
    tAVRC_MSG *avrc_msg = p_remote_rsp->p_msg;
    tAVRC_MSG_VENDOR *vendor_msg = &avrc_msg->vendor;

    //Check what type of metadata was received
    switch (vendor_msg->hdr.ctype) {
    case AVRC_RSP_CHANGED:
        if (vendor_msg->p_vendor_data[0] == AVRC_PDU_REGISTER_NOTIFICATION) {
            handle_rc_notification_rsp(vendor_msg);
        }
        break;

    case AVRC_RSP_IMPL_STBL:
        if (vendor_msg->p_vendor_data[0] == AVRC_PDU_GET_ELEMENT_ATTR) {
            handle_rc_attributes_rsp(vendor_msg);
        }
        break;
    }
#else
    LOG_ERROR("%s AVRCP metadata is not enabled", __FUNCTION__);
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

    case BTA_AV_META_MSG_EVT: {
        handle_rc_metadata_rsp(&(p_data->meta_msg));
    }
    break;

    // below events are not handled for now
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

static bt_status_t btc_avrc_ct_send_set_player_value_cmd(uint8_t tl, uint8_t attr_id, uint8_t value_id)
{
    tAVRC_STS status = BT_STATUS_UNSUPPORTED;

#if (AVRC_METADATA_INCLUDED == TRUE)
    CHECK_ESP_RC_CONNECTED;

    tAVRC_COMMAND avrc_cmd = {0};
    BT_HDR *p_msg = NULL;
    tAVRC_APP_SETTING values = {0};

    values.attr_id = attr_id;
    values.attr_val = value_id;

    avrc_cmd.set_app_val.opcode = AVRC_OP_VENDOR;
    avrc_cmd.set_app_val.status = AVRC_STS_NO_ERROR;
    avrc_cmd.set_app_val.num_val = 1;
    avrc_cmd.set_app_val.p_vals = &values;
    avrc_cmd.set_app_val.pdu = AVRC_PDU_SET_PLAYER_APP_VALUE;

    status = AVRC_BldCommand(&avrc_cmd, &p_msg);
    if (status == AVRC_STS_NO_ERROR) {
        if (btc_rc_vb.rc_features & BTA_AV_FEAT_METADATA) {
            BTA_AvMetaCmd(btc_rc_vb.rc_handle, tl, BTA_AV_CMD_CTRL, p_msg);
            status = BT_STATUS_SUCCESS;
        } else {
            status = BT_STATUS_FAIL;
            LOG_DEBUG("%s: feature not supported", __FUNCTION__);
        }
    }

#else
    LOG_DEBUG("%s: feature not enabled", __FUNCTION__);
#endif

    return status;
}

static bt_status_t btc_avrc_ct_send_register_notification_cmd(uint8_t tl, uint8_t event_id, uint32_t event_parameter)
{
    tAVRC_STS status = BT_STATUS_UNSUPPORTED;

#if (AVRC_METADATA_INCLUDED == TRUE)
    CHECK_ESP_RC_CONNECTED;

    tAVRC_COMMAND avrc_cmd = {0};
    BT_HDR *p_msg = NULL;

    avrc_cmd.reg_notif.opcode = AVRC_OP_VENDOR;
    avrc_cmd.reg_notif.status = AVRC_STS_NO_ERROR;
    avrc_cmd.reg_notif.event_id = event_id;
    avrc_cmd.reg_notif.param = event_parameter;
    avrc_cmd.reg_notif.pdu = AVRC_PDU_REGISTER_NOTIFICATION;

    status = AVRC_BldCommand(&avrc_cmd, &p_msg);
    if (status == AVRC_STS_NO_ERROR) {
        if (btc_rc_vb.rc_features & BTA_AV_FEAT_METADATA) {
            BTA_AvMetaCmd(btc_rc_vb.rc_handle, tl, AVRC_CMD_NOTIF, p_msg);
            status = BT_STATUS_SUCCESS;
        } else {
            status = BT_STATUS_FAIL;
            LOG_DEBUG("%s: feature not supported", __FUNCTION__);
        }
    }

#else
    LOG_DEBUG("%s: feature not enabled", __FUNCTION__);
#endif

    return status;
}

static bt_status_t btc_avrc_ct_send_metadata_cmd (uint8_t tl, uint8_t attr_mask)
{
    tAVRC_STS status = BT_STATUS_UNSUPPORTED;

#if (AVRC_METADATA_INCLUDED == TRUE)
    CHECK_ESP_RC_CONNECTED;
    uint32_t index = 0;

    tAVRC_COMMAND avrc_cmd = {0};
    BT_HDR *p_msg = NULL;

    avrc_cmd.get_elem_attrs.opcode = AVRC_OP_VENDOR;
    avrc_cmd.get_elem_attrs.status = AVRC_STS_NO_ERROR;
    avrc_cmd.get_elem_attrs.pdu = AVRC_PDU_GET_ELEMENT_ATTR;

    for (int count = 0; count < AVRC_MAX_ELEM_ATTR_SIZE; count++) {
        if ((attr_mask & (1 << count)) > 0) {
            avrc_cmd.get_elem_attrs.attrs[index] = count + 1;
            index++;
        }
    }

    avrc_cmd.get_elem_attrs.num_attr = index;

    status = AVRC_BldCommand(&avrc_cmd, &p_msg);
    if (status == AVRC_STS_NO_ERROR) {
        if (btc_rc_vb.rc_features & BTA_AV_FEAT_METADATA) {
            BTA_AvMetaCmd(btc_rc_vb.rc_handle, tl, AVRC_CMD_STATUS, p_msg);
            status = BT_STATUS_SUCCESS;
        } else {
            status = BT_STATUS_FAIL;
            LOG_DEBUG("%s: feature not supported", __FUNCTION__);
        }
    }

#else
    LOG_DEBUG("%s: feature not enabled", __FUNCTION__);
#endif

    return status;
}

static bt_status_t btc_avrc_ct_send_passthrough_cmd(uint8_t tl, uint8_t key_code, uint8_t key_state)
{
    tAVRC_STS status = BT_STATUS_UNSUPPORTED;

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
    case BTC_AVRC_STATUS_API_SND_META_EVT: {
        btc_avrc_ct_send_metadata_cmd(arg->md_cmd.tl, arg->md_cmd.attr_mask);
        break;
    }
    case BTC_AVRC_NOTIFY_API_SND_REG_NOTIFY_EVT: {
        btc_avrc_ct_send_register_notification_cmd(arg->rn_cmd.tl, arg->rn_cmd.event_id, arg->rn_cmd.event_parameter);
        break;
    }
    case BTC_AVRC_CTRL_API_SET_PLAYER_SETTING_EVT: {
        btc_avrc_ct_send_set_player_value_cmd(arg->ps_cmd.tl, arg->ps_cmd.attr_id, arg->ps_cmd.value_id);
        break;
    }
    default:
        LOG_WARN("%s : unhandled event: %d\n", __FUNCTION__, msg->act);
    }
}

#endif /* #if BTC_AV_INCLUDED */

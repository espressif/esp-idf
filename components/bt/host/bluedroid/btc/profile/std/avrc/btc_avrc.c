/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*****************************************************************************
 *
 *  Filename:      btc_avrc.c
 *
 *  Description:   Bluetooth AVRC implementation
 *
 *****************************************************************************/
#include "common/bt_target.h"
#include <string.h>
#include "bta/bta_api.h"
#include "bta/bta_av_api.h"
#include "stack/avrc_defs.h"
#include "btc/btc_common.h"
#include "btc/btc_util.h"
#include "btc_av.h"
#include "btc_avrc.h"
#include "btc/btc_manage.h"
#include "esp_avrc_api.h"
#include "osi/mutex.h"
#include "osi/allocator.h"

#if BTC_AV_INCLUDED

static UINT8 opcode_from_pdu(UINT8 pdu);
static void send_reject_response (UINT8 rc_handle, UINT8 label, UINT8 pdu, UINT8 status);
static void handle_rc_connect (tBTA_AV_RC_OPEN *p_rc_open);
static void handle_rc_disconnect (tBTA_AV_RC_CLOSE *p_rc_close);
static void handle_rc_passthrough_rsp ( tBTA_AV_REMOTE_RSP *p_remote_rsp);
static void handle_rc_metamsg_cmd ( tBTA_AV_META_MSG *p_meta_msg);
static void handle_rc_metamsg_rsp ( tBTA_AV_META_MSG *p_remote_rsp);
static void btc_rc_upstreams_evt(UINT16 event, tAVRC_COMMAND *pavrc_cmd, UINT8 ctype, UINT8 label);

/*****************************************************************************
**  Static variables
******************************************************************************/

/* flag indicating whether TG/CT is initialized */
static uint32_t s_rc_ct_init;
static uint32_t s_rc_tg_init;

#if AVRC_DYNAMIC_MEMORY == FALSE
static btc_rc_cb_t btc_rc_cb;
#else
btc_rc_cb_t *btc_rc_cb_ptr;
#endif ///AVRC_DYNAMIC_MEMORY == FALSE

const static uint16_t cs_psth_allowed_cmd[8] = {
    0x0000, /* bit mask: 0=SELECT, 1=UP, 2=DOWN, 3=LEFT,
                         4=RIGHT, 5=RIGHT_UP, 6=RIGHT_DOWN, 7=LEFT_UP,
                         8=LEFT_DOWN, 9=ROOT_MENU, 10=SETUP_MENU, 11=CONT_MENU,
                         12=FAV_MENU, 13=EXIT */
    0x0000, /* not used */
    0x1FFF, /* bit mask: 0=0, 1=1, 2=2, 3=3,
                         4=4, 5=5, 6=6, 7=7,
                         8=8, 9=9, 10=DOT, 11=ENTER,
                         12=CLEAR */
    0x0078, /* bit mask: 0=CHAN_UP, 1=CHAN_DOWN, 2=PREV_CHAN, 3=SOUND_SEL,
                         4=INPUT_SEL, 5=DISP_INFO, 6=HELP, 7=PAGE_UP,
                         8=PAGE_DOWN */
    0x1b7F, /* bit mask: 0=POWER, 1=VOL_UP, 2=VOL_DOWN, 3=MUTE,
                         4=PLAY, 5=STOP, 6=PAUSE, 7=RECORD,
                         8=REWIND, 9=FAST_FOR, 10=EJECT, 11=FORWARD,
                         12=BACKWARD */
    0x0000, /* bit mask: 0=ANGLE, 1=SUBPICT */
    0x0000, /* not used */
    0x003E  /* bit mask: 0=not used, 1=F1, 2=F2, 3=F3,
                         4=F4, 5=F5 */
};

const static uint16_t cs_psth_dft_supported_cmd[8] = {0};
static uint16_t s_psth_supported_cmd[8];

const static uint16_t cs_rn_allowed_evt = \
        0x2000; /* bit mask: 0=rsvd, 1=play_status, 2=track_chg, 3=reached_end,
                             4=reached_start, 5=pos_chg, 6=batt_status, 7=sys_status_chg,
                             8=player_app_setting, 9=now_playing_content, 10=avail_players, 11=addressed_player,
                             12=UID, 13=volume_chg */

const static uint16_t cs_rn_dft_supported_evt = 0;
static uint16_t s_rn_supported_evt;

// event ids supported to register as CT
const static uint16_t cs_ct_rn_supported_evt = \
        0x2026; /* bit mask: 0=rsvd, 1=play_status, 2=track_chg, 3=reached_end,
                             4=reached_start, 5=pos_chg, 6=batt_status, 7=sys_status_chg,
                             8=player_app_setting, 9=now_playing_content, 10=avail_players, 11=addressed_player,
                             12=UID, 13=volume_chg */

/*****************************************************************************
**  Externs
******************************************************************************/
const uint16_t *btc_avrc_tg_get_allowed_command(void)
{
    return cs_psth_allowed_cmd;
}

const uint16_t *btc_avrc_tg_get_supported_command(void)
{
    return s_psth_supported_cmd;
}

bool btc_avrc_tg_check_supported_command(const uint16_t *cmd_set)
{
    if (cmd_set == NULL) {
        return false;
    }

    // check if cmd_set is a subset of allowed command set
    bool ret = true;
    for (int i = 0; i < 8; ++i) {
        if (cs_psth_allowed_cmd[i] != (cmd_set[i] | cs_psth_allowed_cmd[i])) {
            ret = false;
            break;
        }
    }

    return ret;
}

uint16_t btc_avrc_tg_get_rn_allowed_evt(void)
{
    return cs_rn_allowed_evt;
}

uint16_t btc_avrc_tg_get_rn_supported_evt(void)
{
    return s_rn_supported_evt;
}

bool btc_avrc_tg_check_rn_supported_evt(uint16_t evt_set)
{
    uint16_t evt_super_set = evt_set | cs_rn_allowed_evt;
    if (evt_super_set == cs_rn_allowed_evt) {
        return true;
    } else {
        return false;
    }
}

bool btc_avrc_tg_rn_evt_supported(uint8_t event_id)
{
    uint16_t event_bits = s_rn_supported_evt;
    return ((event_bits >> event_id) & 0x0001) ?
           true : false;
}

bool btc_avrc_ct_rn_evt_supported(uint8_t event_id)
{
    uint16_t event_bits = cs_ct_rn_supported_evt;
    return ((event_bits >> event_id) & 0x0001) ?
           true : false;
}

bool btc_avrc_ct_check_cover_art_support(void)
{
    return (btc_rc_cb.rc_features & BTA_AV_FEAT_COVER_ART);
}

bool btc_avrc_tg_init_p(void)
{
    return (s_rc_tg_init == BTC_RC_TG_INIT_MAGIC);
}

bool btc_avrc_ct_init_p(void)
{
    return (s_rc_ct_init == BTC_RC_CT_INIT_MAGIC);
}

bool btc_avrc_tg_connected_p(void)
{
    return (s_rc_tg_init == BTC_RC_TG_INIT_MAGIC) &&
           (btc_rc_cb.rc_connected == TRUE);
}

bool btc_avrc_ct_connected_p(void)
{
    return (s_rc_ct_init == BTC_RC_CT_INIT_MAGIC) &&
           (btc_rc_cb.rc_connected == TRUE) &&
           (btc_rc_cb.rc_features & BTA_AV_FEAT_RCTG);
}

void btc_avrc_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_avrc_args_t *dst = (btc_avrc_args_t *)p_dest;
    btc_avrc_args_t *src = (btc_avrc_args_t *)p_src;
    size_t len;

    switch (msg->act) {
#if BTC_AV_CA_INCLUDED
    case BTC_AVRC_CT_API_COVER_ART_GET_IMAGE_EVT:
        len = src->ca_get_img.image_descriptor_len;
        dst->ca_get_img.image_descriptor = (uint8_t *)osi_malloc(len);
        if (dst->ca_get_img.image_descriptor) {
            memcpy(dst->ca_get_img.image_descriptor, src->ca_get_img.image_descriptor, len);
        } else {
            BTC_TRACE_ERROR("%s %d no mem\n", __FUNCTION__, msg->act);
        }
        break;
#endif
    default:
        BTC_TRACE_DEBUG("%s Unhandled deep copy %d\n", __FUNCTION__, msg->act);
        UNUSED(dst);
        UNUSED(src);
        UNUSED(len);
        break;
    }
}

void btc_avrc_arg_deep_free(btc_msg_t *msg)
{
    btc_avrc_args_t *arg = (btc_avrc_args_t *)msg->arg;

    switch (msg->act) {
#if BTC_AV_CA_INCLUDED
    case BTC_AVRC_CT_API_COVER_ART_GET_IMAGE_EVT:
        if (arg->ca_get_img.image_descriptor) {
            osi_free(arg->ca_get_img.image_descriptor);
        }
        break;
#endif
    default:
        BTC_TRACE_DEBUG("%s Unhandled deep free %d\n", __FUNCTION__, msg->act);
        UNUSED(arg);
        break;
    }
}

void btc_avrc_tg_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_avrc_tg_args_t *dst = (btc_avrc_tg_args_t *) p_dest;
    btc_avrc_tg_args_t *src = (btc_avrc_tg_args_t *)p_src;
    size_t len;

    switch (msg->act) {
    case BTC_AVRC_TG_API_SET_PSTH_SUPPORTED_CMD_EVT:
        len = 8 * sizeof(uint16_t);
        dst->set_psth_cmd = (uint16_t *)osi_malloc(len);
        if (dst->set_psth_cmd) {
            memcpy(dst->set_psth_cmd, src->set_psth_cmd, len);
        } else {
            BTC_TRACE_ERROR("%s %d no mem\n", __FUNCTION__, msg->act);
        }
        break;
    default:
        BTC_TRACE_DEBUG("%s Unhandled deep copy %d\n", __FUNCTION__, msg->act);
        break;
    }
}

void btc_avrc_tg_arg_deep_free(btc_msg_t *msg)
{
    btc_avrc_tg_args_t *arg = (btc_avrc_tg_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_AVRC_TG_API_SET_PSTH_SUPPORTED_CMD_EVT:
        if (arg->set_psth_cmd) {
            osi_free(arg->set_psth_cmd);
        }
        break;
    default:
        BTC_TRACE_DEBUG("%s Unhandled deep free %d\n", __FUNCTION__, msg->act);
        break;
    }
}

/*****************************************************************************
**  Static functions
******************************************************************************/
static bool btc_avrc_tg_set_supported_command(const uint16_t *cmd_set)
{
    if (!btc_avrc_tg_init_p()) {
        BTC_TRACE_WARNING("%s failed: AVRC TG not yet initialized\n", __FUNCTION__);
        return false;
    }
    if (!btc_avrc_tg_check_supported_command(cmd_set)) {
        return false;
    } else {
        memcpy(s_psth_supported_cmd, cmd_set, sizeof(s_psth_supported_cmd));
        return true;
    }
}

static bool btc_avrc_tg_set_rn_supported_evt(uint16_t evt_set)
{
    if (!btc_avrc_tg_init_p()) {
        BTC_TRACE_WARNING("%s failed: AVRC TG not yet initialized\n", __FUNCTION__);
        return false;
    }

    if (btc_avrc_tg_check_rn_supported_evt(evt_set)) {
        s_rn_supported_evt = evt_set;
        return true;
    } else {
        return false;
    }
}

static inline void btc_avrc_ct_cb_to_app(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *param)
{
    if (s_rc_ct_init != BTC_RC_CT_INIT_MAGIC) {
        return;
    }

    esp_avrc_ct_cb_t btc_avrc_ct_cb = (esp_avrc_ct_cb_t)btc_profile_cb_get(BTC_PID_AVRC_CT);
    if (btc_avrc_ct_cb) {
        btc_avrc_ct_cb(event, param);
    }
}

static inline void btc_avrc_tg_cb_to_app(esp_avrc_tg_cb_event_t event, esp_avrc_tg_cb_param_t *param)
{
    if (s_rc_tg_init != BTC_RC_TG_INIT_MAGIC) {
        return;
    }

    esp_avrc_tg_cb_t btc_avrc_tg_cb = (esp_avrc_tg_cb_t)btc_profile_cb_get(BTC_PID_AVRC_TG);
    if (btc_avrc_tg_cb) {
        btc_avrc_tg_cb(event, param);
    }
}

/***************************************************************************
 *  Function       send_metamsg_rsp
 *
 *  - Argument:
 *                  rc_handle     RC handle corresponding to the connected RC
 *                  label            Label of the RC response
 *                  code            Response type
 *                  p_meta_rsp      Vendor response
 *
 *  - Description: Remote control metamsg response handler (AVRCP 1.3)
 *
 ***************************************************************************/
static void send_metamsg_rsp (UINT8 rc_handle, UINT8 label, tBTA_AV_CODE code,
                              tAVRC_RESPONSE *p_meta_rsp)
{
    UINT8 ctype;

    if (!p_meta_rsp) {
        BTC_TRACE_WARNING("%s: Invalid response received from application", __FUNCTION__);
        return;
    }

    BTC_TRACE_EVENT("%s: rc_handle: %d, label: %d, code: 0x%02x, pdu: %d", __FUNCTION__,
                    rc_handle, label, code, p_meta_rsp->rsp.pdu);

    if (p_meta_rsp->rsp.status != AVRC_STS_NO_ERROR) {
        ctype = AVRC_RSP_REJ;
    } else {
        if (code < AVRC_RSP_NOT_IMPL) {
            if (code == AVRC_CMD_NOTIF) {
                ctype = AVRC_RSP_INTERIM;
            } else if (code == AVRC_CMD_STATUS) {
                ctype = AVRC_RSP_IMPL_STBL;
            } else {
                ctype = AVRC_RSP_ACCEPT;
            }
        } else {
            ctype = code;
        }
    }

    /* if response is for register_notification, make sure the rc has actually registered for this */
    if ((p_meta_rsp->rsp.pdu == AVRC_PDU_REGISTER_NOTIFICATION) && (code == AVRC_RSP_CHANGED)) {
        UINT8   event_id = p_meta_rsp->reg_notif.event_id;
        BOOLEAN notify = (btc_rc_cb.rc_connected) && (btc_rc_cb.rc_ntf[event_id - 1].registered);

        /* de-register this notification for a CHANGED response */
        btc_rc_cb.rc_ntf[event_id - 1].registered = FALSE;
        BTC_TRACE_DEBUG("%s rc_handle: %d. event_id: 0x%02d deregistered", __FUNCTION__,
                        btc_rc_cb.rc_handle, event_id);
        if (notify) {
            BT_HDR *p_msg = NULL;
            tAVRC_STS status = AVRC_BldResponse(btc_rc_cb.rc_handle, p_meta_rsp, &p_msg);

            if (status == AVRC_STS_NO_ERROR) {
                BTC_TRACE_DEBUG("%s Sending notification to rc_handle: %d. event_id: 0x%02d",
                                __FUNCTION__, btc_rc_cb.rc_handle, event_id);
                BTA_AvMetaRsp(btc_rc_cb.rc_handle, btc_rc_cb.rc_ntf[event_id - 1].label, ctype, p_msg);
            } else {
                BTC_TRACE_WARNING("%s failed to build metamsg response. status: 0x%02x",
                                  __FUNCTION__, status);
            }
        }
    } else {
        // send response
        BT_HDR *p_msg = NULL;
        tAVRC_STS status;

        status = AVRC_BldResponse(rc_handle, p_meta_rsp, &p_msg);

        if (status == AVRC_STS_NO_ERROR) {
            BTA_AvMetaRsp(rc_handle, label, ctype, p_msg);
        } else {
            BTC_TRACE_ERROR("%s: failed to build metamsg response. status: 0x%02x",
                            __FUNCTION__, status);
        }
    }
}


static UINT8 opcode_from_pdu(UINT8 pdu)
{
    UINT8 opcode = 0;

    switch (pdu) {
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

    if (AVRC_STS_NO_ERROR == (status = AVRC_BldResponse(rc_handle, &avrc_rsp, &p_msg)) ) {
        BTC_TRACE_DEBUG("%s: Sending error notification to handle:%d. pdu:%s,status:0x%02x",
                        __FUNCTION__, rc_handle, dump_rc_pdu(pdu), status);
        BTA_AvMetaRsp(rc_handle, label, ctype, p_msg);
    }
}

static void handle_rc_features(void)
{
    if ((btc_rc_cb.rc_features & BTA_AV_FEAT_RCTG) &&
        (btc_rc_cb.rc_tg_features != 0)) {
        esp_avrc_ct_cb_param_t param;
        memset(&param, 0, sizeof(esp_avrc_ct_cb_param_t));
        param.rmt_feats.feat_mask = btc_rc_cb.rc_features;
        param.rmt_feats.tg_feat_flag = btc_rc_cb.rc_tg_features;
        memcpy(param.rmt_feats.remote_bda, btc_rc_cb.rc_addr, sizeof(esp_bd_addr_t));
        btc_avrc_ct_cb_to_app(ESP_AVRC_CT_REMOTE_FEATURES_EVT, &param);
    }

    if ((btc_rc_cb.rc_features & BTA_AV_FEAT_RCCT) &&
        (btc_rc_cb.rc_ct_features != 0)) {
        esp_avrc_tg_cb_param_t param;
        memset(&param, 0, sizeof(esp_avrc_tg_cb_param_t));
        param.rmt_feats.feat_mask = btc_rc_cb.rc_features;
        param.rmt_feats.ct_feat_flag = btc_rc_cb.rc_ct_features;
        memcpy(param.rmt_feats.remote_bda, btc_rc_cb.rc_addr, sizeof(esp_bd_addr_t));
        btc_avrc_tg_cb_to_app(ESP_AVRC_TG_REMOTE_FEATURES_EVT, &param);
    }
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
    BTC_TRACE_DEBUG("%s: rc_handle: %d", __FUNCTION__, p_rc_open->rc_handle);
    bt_bdaddr_t rc_addr;

    if (p_rc_open->status == BTA_AV_SUCCESS) {
        //check if already some RC is connected
        if (btc_rc_cb.rc_connected) {
            BTC_TRACE_ERROR("Got RC OPEN in connected state, Connected RC: %d \
                and Current RC: %d", btc_rc_cb.rc_handle, p_rc_open->rc_handle );
            if ((btc_rc_cb.rc_handle != p_rc_open->rc_handle)
                    && (bdcmp(btc_rc_cb.rc_addr, p_rc_open->peer_addr))) {
                BTC_TRACE_DEBUG("Got RC connected for some other handle");
                BTA_AvCloseRc(p_rc_open->rc_handle);
                return;
            }
        }
        memcpy(btc_rc_cb.rc_addr, p_rc_open->peer_addr, sizeof(BD_ADDR));
        btc_rc_cb.rc_features = p_rc_open->peer_features;
        btc_rc_cb.rc_connected = TRUE;
        btc_rc_cb.rc_handle = p_rc_open->rc_handle;

        bdcpy(rc_addr.address, btc_rc_cb.rc_addr);
        // callback to application
        if (p_rc_open->peer_features & BTA_AV_FEAT_RCTG) {
            esp_avrc_ct_cb_param_t param;
            memset(&param, 0, sizeof(esp_avrc_ct_cb_param_t));
            param.conn_stat.connected = true;
            memcpy(param.conn_stat.remote_bda, &rc_addr, sizeof(esp_bd_addr_t));
            btc_avrc_ct_cb_to_app(ESP_AVRC_CT_CONNECTION_STATE_EVT, &param);
        }

        if (btc_avrc_tg_init_p()) {
            esp_avrc_tg_cb_param_t param;
            memset(&param, 0, sizeof(esp_avrc_tg_cb_param_t));
            param.conn_stat.connected = true;
            memcpy(param.conn_stat.remote_bda, &rc_addr, sizeof(esp_bd_addr_t));
            btc_avrc_tg_cb_to_app(ESP_AVRC_TG_CONNECTION_STATE_EVT, &param);
        }

        /* on locally initiated connection we will get remote features as part of connect */
        if (p_rc_open->sdp_disc_done == TRUE) {
            btc_rc_cb.rc_ct_features = p_rc_open->peer_ct_features;
            btc_rc_cb.rc_tg_features = p_rc_open->peer_tg_features;
            handle_rc_features();
        }
    } else {
        BTC_TRACE_ERROR("%s Connect failed with error code: %d",
                        __FUNCTION__, p_rc_open->status);
        btc_rc_cb.rc_connected = FALSE;
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
    BTC_TRACE_DEBUG("%s: rc_handle: %d", __FUNCTION__, p_rc_close->rc_handle);
    if ((p_rc_close->rc_handle != btc_rc_cb.rc_handle)
            && (bdcmp(btc_rc_cb.rc_addr, p_rc_close->peer_addr))) {
        BTC_TRACE_ERROR("Got disconnect of unknown device");
        return;
    }

    tBTA_AV_FEAT rc_features = btc_rc_cb.rc_features;

    // clean up the state
    btc_rc_cb.rc_handle = 0;
    btc_rc_cb.rc_connected = FALSE;
#if BTC_AV_CA_INCLUDED
    bool cover_art_connected = btc_rc_cb.rc_cover_art_connected;
    btc_rc_cb.rc_cover_art_connected = FALSE;
#endif
    btc_rc_cb.rc_features = 0;
    btc_rc_cb.rc_ct_features = 0;
    btc_rc_cb.rc_tg_features = 0;
    memset(btc_rc_cb.rc_addr, 0, sizeof(BD_ADDR));
    memset(btc_rc_cb.rc_ntf, 0, sizeof(btc_rc_cb.rc_ntf));

#if BTC_AV_CA_INCLUDED
    /* report connection state */
    if (cover_art_connected) {
        /* if rc disconnect, cover art disconnect too */
        esp_avrc_ct_cb_param_t param;
        memset(&param, 0, sizeof(esp_avrc_ct_cb_param_t));
        param.cover_art_state.state = ESP_AVRC_COVER_ART_DISCONNECTED;
        param.cover_art_state.reason = BT_STATUS_FAIL;
        btc_avrc_ct_cb_to_app(ESP_AVRC_CT_COVER_ART_STATE_EVT, &param);
    }
#endif

    if (rc_features & BTA_AV_FEAT_RCTG) {
        esp_avrc_ct_cb_param_t param;
        memset(&param, 0, sizeof(esp_avrc_ct_cb_param_t));
        param.conn_stat.connected = false;
        memcpy(param.conn_stat.remote_bda, btc_rc_cb.rc_addr, sizeof(esp_bd_addr_t));
        btc_avrc_ct_cb_to_app(ESP_AVRC_CT_CONNECTION_STATE_EVT, &param);
    }

    if (btc_avrc_tg_init_p()) {
        esp_avrc_tg_cb_param_t param;
        memset(&param, 0, sizeof(esp_avrc_ct_cb_param_t));
        param.conn_stat.connected = false;
        memcpy(param.conn_stat.remote_bda, btc_rc_cb.rc_addr, sizeof(esp_bd_addr_t));
        btc_avrc_tg_cb_to_app(ESP_AVRC_TG_CONNECTION_STATE_EVT, &param);
    }
}

static void handle_rc_attributes_rsp (tAVRC_MSG_VENDOR *vendor_msg)
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

        attr_index += attr_length + 8;
    }
}

static void handle_rc_notification_rsp (tAVRC_MSG_VENDOR *vendor_msg)
{
    esp_avrc_ct_cb_param_t param;
    memset(&param, 0, sizeof(esp_avrc_ct_cb_param_t));

    param.change_ntf.event_id = vendor_msg->p_vendor_data[4];

    uint8_t *data = &vendor_msg->p_vendor_data[5];
    if (!btc_avrc_ct_rn_evt_supported(param.change_ntf.event_id)) {
        BTC_TRACE_WARNING("%s unsupported notification on CT, event id 0x%x", __FUNCTION__,
                          param.change_ntf.event_id);
        return;
    }

    switch (param.change_ntf.event_id) {
    case ESP_AVRC_RN_PLAY_STATUS_CHANGE:
        BE_STREAM_TO_UINT8(param.change_ntf.event_parameter.playback, data);
        break;
    case ESP_AVRC_RN_TRACK_CHANGE:
        memcpy(param.change_ntf.event_parameter.elm_id, data, 8);
        break;
    case ESP_AVRC_RN_PLAY_POS_CHANGED:
        BE_STREAM_TO_UINT32(param.change_ntf.event_parameter.play_pos, data);
        break;
    case ESP_AVRC_RN_BATTERY_STATUS_CHANGE:
        BE_STREAM_TO_UINT8(param.change_ntf.event_parameter.batt, data);
        break;
    case ESP_AVRC_RN_VOLUME_CHANGE:
        BE_STREAM_TO_UINT8(param.change_ntf.event_parameter.volume, data);
        break;
    // for non-parameter event response
    case ESP_AVRC_RN_TRACK_REACHED_END:
    case ESP_AVRC_RN_TRACK_REACHED_START:
        break;
    // for other unsupported event:
    case ESP_AVRC_RN_SYSTEM_STATUS_CHANGE:
    case ESP_AVRC_RN_APP_SETTING_CHANGE:
    case ESP_AVRC_RN_NOW_PLAYING_CHANGE:
    case ESP_AVRC_RN_AVAILABLE_PLAYERS_CHANGE:
    case ESP_AVRC_RN_ADDRESSED_PLAYER_CHANGE:
    case ESP_AVRC_RN_UIDS_CHANGE:
    default:
        BTC_TRACE_WARNING("%s RC unhandled notification response, event id 0x%x", __FUNCTION__,
                          param.change_ntf.event_id);
        break;
    }
    btc_avrc_ct_cb_to_app(ESP_AVRC_CT_CHANGE_NOTIFY_EVT, &param);
}

static void handle_rc_get_caps_rsp (tAVRC_GET_CAPS_RSP *rsp)
{
    if (rsp->capability_id == AVRC_CAP_EVENTS_SUPPORTED) {
        esp_avrc_ct_cb_param_t param;
        memset(&param, 0, sizeof(esp_avrc_ct_cb_param_t));
        param.get_rn_caps_rsp.cap_count = rsp->count;
        for (int i = 0; i < rsp->count; ++i) {
            esp_avrc_rn_evt_bit_mask_operation(ESP_AVRC_BIT_MASK_OP_SET, &param.get_rn_caps_rsp.evt_set,
                                               rsp->param.event_id[i]);
        }
        btc_avrc_ct_cb_to_app(ESP_AVRC_CT_GET_RN_CAPABILITIES_RSP_EVT, &param);
    } else {
        // ignore other capability_id
        BTC_TRACE_WARNING("AVRC unhandled event, CapabilityID: 0x%x", rsp->capability_id);
        return;
    }
}

static void handle_rc_set_absolute_volume_rsp(tAVRC_SET_VOLUME_RSP *rsp)
{
    esp_avrc_ct_cb_param_t param;
    memset(&param, 0, sizeof(esp_avrc_ct_cb_param_t));

    param.set_volume_rsp.volume = rsp->volume;

    btc_avrc_ct_cb_to_app(ESP_AVRC_CT_SET_ABSOLUTE_VOLUME_RSP_EVT, &param);
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
static void handle_rc_metamsg_cmd (tBTA_AV_META_MSG *p_meta_msg)
{
    BTC_TRACE_DEBUG("%s, opcode 0x%x, len %d, code %d", __FUNCTION__, p_meta_msg->p_msg->hdr.opcode, p_meta_msg->len, p_meta_msg->code);

    if (p_meta_msg->p_msg->hdr.opcode != AVRC_OP_VENDOR) {
        BTC_TRACE_WARNING("Invalid opcode: %x", p_meta_msg->p_msg->hdr.opcode);
        return;
    }
    if (p_meta_msg->len < 3) {
        BTC_TRACE_WARNING("Invalid length.Opcode: 0x%x, len: 0x%x", p_meta_msg->p_msg->hdr.opcode,
                          p_meta_msg->len);
        return;
    }

    if (p_meta_msg->code >= AVRC_RSP_NOT_IMPL) {
#if (AVRC_ADV_CTRL_INCLUDED == TRUE)
        handle_rc_metamsg_rsp(p_meta_msg);
#else
        BTC_TRACE_WARNING("%s:Received vendor dependent rsp. code: %d len: %d. Not processing it.",
                          __FUNCTION__, p_meta_msg->code, p_meta_msg->len);
#endif
        return;
    }

#define RC_CMD_PRS_BUF_LEN  (512)
    uint8_t *buf = (uint8_t *)osi_calloc(RC_CMD_PRS_BUF_LEN);
    tAVRC_COMMAND avrc_command = {0};
    tAVRC_STS status;

    status = AVRC_ParsCommand(p_meta_msg->p_msg, &avrc_command, buf, RC_CMD_PRS_BUF_LEN);
    BTC_TRACE_DEBUG("Rcv vendor cmd: code %d, PDU 0x%x, label %d", p_meta_msg->code,
                    avrc_command.cmd.pdu, p_meta_msg->label);

    if (status != AVRC_STS_NO_ERROR) {
        /* return error */
        BTC_TRACE_WARNING("%s: Error in parsing vendor command. status: 0x%02x",
                          __FUNCTION__, status);
        send_reject_response(p_meta_msg->rc_handle, p_meta_msg->label, avrc_command.pdu, status);
    } else {
        btc_rc_upstreams_evt(avrc_command.cmd.pdu, &avrc_command, p_meta_msg->code, p_meta_msg->label);
    }
    osi_free(buf);
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
    BTC_TRACE_EVENT("%s pdu: 0x%x handle: 0x%x ctype:%x label:%x", __FUNCTION__,
                    pavrc_cmd->pdu, btc_rc_cb.rc_handle, ctype, label);

    switch (event) {
    case AVRC_PDU_SET_ABSOLUTE_VOLUME: {
        // set up response
        tAVRC_RESPONSE avrc_rsp;
        BTC_TRACE_EVENT("%s() AVRC_PDU_SET_ABSOLUTE_VOLUME", __FUNCTION__);
        memset(&(avrc_rsp.volume), 0, sizeof(tAVRC_RSP));
        avrc_rsp.volume.opcode = opcode_from_pdu(AVRC_PDU_SET_ABSOLUTE_VOLUME);
        avrc_rsp.volume.pdu = AVRC_PDU_SET_ABSOLUTE_VOLUME;
        avrc_rsp.volume.status = AVRC_STS_NO_ERROR;
        avrc_rsp.volume.volume = pavrc_cmd->volume.volume;
        send_metamsg_rsp(btc_rc_cb.rc_handle, label, ctype, &avrc_rsp);

        // set up callback
        esp_avrc_tg_cb_param_t param;
        memset(&param, 0, sizeof(esp_avrc_tg_cb_param_t));
        param.set_abs_vol.volume = pavrc_cmd->volume.volume;
        btc_avrc_tg_cb_to_app(ESP_AVRC_TG_SET_ABSOLUTE_VOLUME_CMD_EVT, &param);
    }
    break;
    case AVRC_PDU_SET_PLAYER_APP_VALUE: {
        // set up callback
        esp_avrc_tg_cb_param_t param;
        param.set_app_value.num_val = pavrc_cmd->set_app_val.num_val;
        param.set_app_value.p_vals = (esp_avrc_set_app_value_param_t *)pavrc_cmd->set_app_val.p_vals;
        btc_avrc_tg_cb_to_app(ESP_AVRC_TG_SET_PLAYER_APP_VALUE_EVT, &param);
    }
    break;
    case AVRC_PDU_GET_PLAY_STATUS:
    case AVRC_PDU_GET_ELEMENT_ATTR:
    case AVRC_PDU_INFORM_DISPLAY_CHARSET:
    //todo: check the valid response for these PDUs
    case AVRC_PDU_LIST_PLAYER_APP_ATTR:
    case AVRC_PDU_LIST_PLAYER_APP_VALUES:
    case AVRC_PDU_GET_CUR_PLAYER_APP_VALUE:
    case AVRC_PDU_GET_PLAYER_APP_ATTR_TEXT:
    case AVRC_PDU_GET_PLAYER_APP_VALUE_TEXT: {
        send_reject_response (btc_rc_cb.rc_handle, label, pavrc_cmd->pdu, AVRC_STS_BAD_CMD);
    }
    break;
    case AVRC_PDU_REGISTER_NOTIFICATION: {
        UINT8 event_id = pavrc_cmd->reg_notif.event_id;
        if (event_id > MAX_RC_NOTIFICATIONS) {
            BTC_TRACE_WARNING("Invalid event_id: 0x%x", event_id);
            break;
        }

        btc_rc_cb.rc_ntf[event_id - 1].registered = TRUE;
        btc_rc_cb.rc_ntf[event_id - 1].label = label;
        BTC_TRACE_EVENT("%s: New registered notification: event_id:0x%x, label:0x%x",
                        __FUNCTION__, event_id, label);

        // set up callback
        esp_avrc_tg_cb_param_t param;
        memset(&param, 0, sizeof(esp_avrc_tg_cb_param_t));
        param.reg_ntf.event_id = pavrc_cmd->reg_notif.event_id;
        param.reg_ntf.event_parameter = pavrc_cmd->reg_notif.param;
        btc_avrc_tg_cb_to_app(ESP_AVRC_TG_REGISTER_NOTIFICATION_EVT, &param);
    }
    break;
    default: {
        send_reject_response (btc_rc_cb.rc_handle, label, pavrc_cmd->pdu,
                              (pavrc_cmd->pdu == AVRC_PDU_SEARCH) ? AVRC_STS_SEARCH_NOT_SUP : AVRC_STS_BAD_CMD);
    }
    break;
    }
    return;
}

/***************************************************************************
 *  Function       handle_rc_metamsg_rsp
 *
 *  - Argument:    tBTA_AV_META_MSG metadata command response
 *
 *  - Description: Vendor metadata response handler
 *
 ***************************************************************************/
static void handle_rc_metamsg_rsp (tBTA_AV_META_MSG *p_meta_msg)
{
    tAVRC_RESPONSE avrc_response = {0};
    tAVRC_STS status;
    tAVRC_MSG_VENDOR *vendor_msg = &p_meta_msg->p_msg->vendor;
    BTC_TRACE_DEBUG("%s: opcode %d, pdu 0x%x, code %d", __FUNCTION__, p_meta_msg->p_msg->hdr.opcode, vendor_msg->p_vendor_data[0],
                    p_meta_msg->code);
    if ( p_meta_msg->p_msg->hdr.opcode != AVRC_OP_VENDOR) {
        return;
    }
    if (p_meta_msg->code != AVRC_RSP_CHANGED &&
            p_meta_msg->code != AVRC_RSP_INTERIM &&
            p_meta_msg->code != AVRC_RSP_ACCEPT &&
            p_meta_msg->code != AVRC_RSP_REJ &&
            p_meta_msg->code != AVRC_RSP_NOT_IMPL &&
            p_meta_msg->code != AVRC_RSP_IMPL_STBL) {
        return;
    }

    // handle GET_ELEMENT_ATTR response
    if (p_meta_msg->code == AVRC_RSP_IMPL_STBL &&
            vendor_msg->p_vendor_data[0] == AVRC_PDU_GET_ELEMENT_ATTR) {
        handle_rc_attributes_rsp(vendor_msg);
        return;
    }

    status = AVRC_ParsResponse(p_meta_msg->p_msg, &avrc_response);
    if (status != AVRC_STS_NO_ERROR) {
        BTC_TRACE_WARNING("%s: code %d error 0x%x", __FUNCTION__, p_meta_msg->code, status);
        return;
    }

    tAVRC_MSG *avrc_msg = p_meta_msg->p_msg;
    vendor_msg = &avrc_msg->vendor;
    switch (avrc_response.rsp.pdu) {
    case AVRC_PDU_REGISTER_NOTIFICATION:
        if (vendor_msg->hdr.ctype == AVRC_RSP_CHANGED) {
            handle_rc_notification_rsp(vendor_msg);
        } else if (vendor_msg->hdr.ctype == AVRC_RSP_INTERIM) {
            // ignore this response
        }
        break;
    case AVRC_PDU_GET_ELEMENT_ATTR:
        // todo: handle this PDU here
        break;
    case AVRC_PDU_GET_CAPABILITIES:
        if (vendor_msg->hdr.ctype == AVRC_RSP_IMPL_STBL) {
            handle_rc_get_caps_rsp(&avrc_response.get_caps);
        }
        break;
    case AVRC_PDU_SET_ABSOLUTE_VOLUME:
        if (vendor_msg->hdr.ctype == AVRC_RSP_ACCEPT) {
            handle_rc_set_absolute_volume_rsp(&avrc_response.volume);
        }
        break;
    default:
        BTC_TRACE_WARNING("%s: unhandled meta rsp: pdu 0x%x", __FUNCTION__, avrc_response.rsp.pdu);
    }
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
    if (btc_rc_cb.rc_features & BTA_AV_FEAT_RCTG) {
        int key_state;
        if (p_remote_rsp->key_state == AVRC_STATE_RELEASE) {
            status = "released";
            key_state = 1;
        } else {
            status = "pressed";
            key_state = 0;
        }

        BTC_TRACE_DEBUG("%s: rc_id=%d status=%s", __FUNCTION__, p_remote_rsp->rc_id, status);

        do {
            esp_avrc_ct_cb_param_t param;
            memset(&param, 0, sizeof(esp_avrc_ct_cb_param_t));
            param.psth_rsp.tl = p_remote_rsp->label;
            param.psth_rsp.key_code = p_remote_rsp->rc_id;
            param.psth_rsp.key_state = key_state;
            param.psth_rsp.rsp_code = p_remote_rsp->rsp_code;
            btc_avrc_ct_cb_to_app(ESP_AVRC_CT_PASSTHROUGH_RSP_EVT, &param);
        } while (0);
    } else {
        BTC_TRACE_ERROR("%s DUT does not support AVRCP controller role", __FUNCTION__);
    }

    UNUSED(status);
#else
    BTC_TRACE_ERROR("%s AVRCP controller role is not enabled", __FUNCTION__);
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
void handle_rc_passthrough_cmd ( tBTA_AV_REMOTE_CMD *p_remote_cmd)
{
    esp_avrc_tg_cb_param_t param;
    memset(&param, 0, sizeof(esp_avrc_tg_cb_param_t));
    param.psth_cmd.key_code = p_remote_cmd->rc_id;
    param.psth_cmd.key_state = p_remote_cmd->key_state;
    btc_avrc_tg_cb_to_app(ESP_AVRC_TG_PASSTHROUGH_CMD_EVT, &param);
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
    BTC_TRACE_DEBUG ("%s event:%s", __FUNCTION__, dump_rc_event(event));
    switch (event) {
    case BTA_AV_RC_OPEN_EVT: {
        BTC_TRACE_DEBUG("RC open, peer_features:%x", p_data->rc_open.peer_features);
        handle_rc_connect( &(p_data->rc_open) );
    } break;

    case BTA_AV_RC_CLOSE_EVT: {
        handle_rc_disconnect( &(p_data->rc_close) );
    } break;

#if (AVRC_CTLR_INCLUDED == TRUE)
    case BTA_AV_REMOTE_RSP_EVT: {
        BTC_TRACE_DEBUG("RSP: rc_id:0x%x key_state:%d", p_data->remote_rsp.rc_id,
                        p_data->remote_rsp.key_state);
        handle_rc_passthrough_rsp( (&p_data->remote_rsp) );
    }
    break;
#endif
    case BTA_AV_RC_FEAT_EVT: {
        BTC_TRACE_DEBUG("Peer_features:%x", p_data->rc_feat.peer_features);
        do {
            // report connection state if connection state wasn't reported on BTA_AV_RC_OPEN_EVT
            tBTA_AV_FEAT old_feats = btc_rc_cb.rc_features;
            if ((p_data->rc_feat.peer_features & BTA_AV_FEAT_RCTG) &&
                !(old_feats & BTA_AV_FEAT_RCTG)) {
                esp_avrc_ct_cb_param_t param;
                memset(&param, 0, sizeof(esp_avrc_ct_cb_param_t));
                param.conn_stat.connected = true;
                memcpy(param.conn_stat.remote_bda, btc_rc_cb.rc_addr, sizeof(esp_bd_addr_t));
                btc_avrc_ct_cb_to_app(ESP_AVRC_CT_CONNECTION_STATE_EVT, &param);
            }
            /**
             * @note ESP_AVRC_TG_CONNECTION_STATE_EVT has been reported on rc connect/disconnect event,
             * it doesn't rely on the SDP results.
             */
        } while (0);
        btc_rc_cb.rc_features = p_data->rc_feat.peer_features;
        btc_rc_cb.rc_ct_features = p_data->rc_feat.peer_ct_features;
        btc_rc_cb.rc_tg_features = p_data->rc_feat.peer_tg_features;
        handle_rc_features();
    }
    break;

    case BTA_AV_META_MSG_EVT: {
        handle_rc_metamsg_cmd(&(p_data->meta_msg));
    }
    break;
    case BTA_AV_REMOTE_CMD_EVT: {
        BTC_TRACE_DEBUG("rc_id:0x%x key_state:%d", p_data->remote_cmd.rc_id,
                        p_data->remote_cmd.key_state);
        handle_rc_passthrough_cmd(&p_data->remote_cmd);
    }
    break;
#if BTC_AV_CA_INCLUDED
    case BTA_AV_CA_STATUS_EVT: {
        btc_rc_cb.rc_cover_art_connected = p_data->ca_status.connected;
        esp_avrc_ct_cb_param_t param;
        memset(&param, 0, sizeof(esp_avrc_ct_cb_param_t));
        if (p_data->ca_status.connected) {
            param.cover_art_state.state = ESP_AVRC_COVER_ART_CONNECTED;
        }
        else {
            param.cover_art_state.state = ESP_AVRC_COVER_ART_DISCONNECTED;
        }
        param.cover_art_state.reason = p_data->ca_status.reason;
        btc_avrc_ct_cb_to_app(ESP_AVRC_CT_COVER_ART_STATE_EVT, &param);
    }
    break;
    case BTA_AV_CA_DATA_EVT: {
        esp_avrc_ct_cb_param_t param;
        memset(&param, 0, sizeof(esp_avrc_ct_cb_param_t));
        param.cover_art_data.status = p_data->ca_data.status;
        param.cover_art_data.final = p_data->ca_data.final;
        param.cover_art_data.data_len = p_data->ca_data.data_len;
        param.cover_art_data.p_data = p_data->ca_data.p_data;
        btc_avrc_ct_cb_to_app(ESP_AVRC_CT_COVER_ART_DATA_EVT, &param);
        /* free the data packet now */
        if (p_data->ca_data.p_hdr != NULL) {
            osi_free(p_data->ca_data.p_hdr);
        }
    }
    break;
#endif /* BTC_AV_CA_INCLUDED */
    default:
        BTC_TRACE_DEBUG("Unhandled RC event : 0x%x", event);
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
    if (btc_rc_cb.rc_connected == TRUE) {
        bdcpy(peer_addr, btc_rc_cb.rc_addr);
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
** Description      Initializes the AVRC Controller interface
**
** Returns          esp_err_t
**
*******************************************************************************/
static void btc_avrc_ct_init(void)
{
    BTC_TRACE_DEBUG("## %s ##", __FUNCTION__);
    if (s_rc_ct_init == BTC_RC_CT_INIT_MAGIC) {
        BTC_TRACE_WARNING("%s already initialized", __FUNCTION__);
        return;
    }

    /// initialize CT-specific resources
    s_rc_ct_init = BTC_RC_CT_INIT_MAGIC;

    /// initialize CT-TG shared resources
    if (s_rc_tg_init != BTC_RC_TG_INIT_MAGIC) {
        memset (&btc_rc_cb, 0, sizeof(btc_rc_cb_t));

        if (!g_av_with_rc) {
            g_av_with_rc = true;
        }

        if (g_a2dp_on_init) {
            BTC_TRACE_WARNING("AVRC Controller is expected to be initialized in advance of A2DP !!!");
        }
    }
}


/***************************************************************************
**
** Function         btc_avrc_ct_deinit
**
** Description      Closes the AVRC Controller interface
**
** Returns          void
**
***************************************************************************/
static void btc_avrc_ct_deinit(void)
{
    BTC_TRACE_API("## %s ##", __FUNCTION__);

    if (g_a2dp_on_deinit) {
        BTC_TRACE_WARNING("A2DP already deinit, AVRC CT should deinit in advance of A2DP !!!");
    }

    if (s_rc_ct_init != BTC_RC_CT_INIT_MAGIC) {
        BTC_TRACE_WARNING("%s not initialized", __FUNCTION__);
        return;
    }

    /// deinit CT-specific resources
    s_rc_ct_init = 0;

    /// deinit CT-TG shared resources
    if (s_rc_tg_init != BTC_RC_TG_INIT_MAGIC) {
        memset (&btc_rc_cb, 0, sizeof(btc_rc_cb_t));
        if (g_av_with_rc) {
            g_av_with_rc = false;
        }
    }

    BTC_TRACE_API("## %s ## completed", __FUNCTION__);
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

    if (btc_rc_cb.rc_features & BTA_AV_FEAT_METADATA) {
        status = AVRC_BldCommand(&avrc_cmd, &p_msg);
        if (status == AVRC_STS_NO_ERROR) {
            BTA_AvMetaCmd(btc_rc_cb.rc_handle, tl, BTA_AV_CMD_CTRL, p_msg);
            status = BT_STATUS_SUCCESS;
        }
    } else {
        status = BT_STATUS_FAIL;
        BTC_TRACE_DEBUG("%s: feature not supported", __FUNCTION__);
    }

#else
    BTC_TRACE_DEBUG("%s: feature not enabled", __FUNCTION__);
#endif

    return status;
}

static bt_status_t btc_avrc_ct_send_get_rn_caps_cmd(uint8_t tl)
{
    tAVRC_STS status = BT_STATUS_UNSUPPORTED;

#if (AVRC_METADATA_INCLUDED == TRUE)
    CHECK_ESP_RC_CONNECTED;

    tAVRC_COMMAND avrc_cmd = {0};
    BT_HDR *p_msg = NULL;

    avrc_cmd.get_caps.opcode = AVRC_OP_VENDOR;
    avrc_cmd.get_caps.status = AVRC_STS_NO_ERROR;
    avrc_cmd.get_caps.pdu = AVRC_PDU_GET_CAPABILITIES;
    avrc_cmd.get_caps.capability_id = AVRC_CAP_EVENTS_SUPPORTED;

    if (btc_rc_cb.rc_features & BTA_AV_FEAT_METADATA) {
        status = AVRC_BldCommand(&avrc_cmd, &p_msg);
        if (status == AVRC_STS_NO_ERROR) {
            BTA_AvMetaCmd(btc_rc_cb.rc_handle, tl, AVRC_CMD_STATUS, p_msg);
            status = BT_STATUS_SUCCESS;
        }
    } else {
        status = BT_STATUS_FAIL;
        BTC_TRACE_DEBUG("%s: feature not supported", __FUNCTION__);
    }

#else
    BTC_TRACE_DEBUG("%s: feature not enabled", __FUNCTION__);
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

    if (btc_rc_cb.rc_features & BTA_AV_FEAT_METADATA) {
        status = AVRC_BldCommand(&avrc_cmd, &p_msg);
        if (status == AVRC_STS_NO_ERROR) {
            BTA_AvMetaCmd(btc_rc_cb.rc_handle, tl, AVRC_CMD_NOTIF, p_msg);
            status = BT_STATUS_SUCCESS;
        }
    } else {
        status = BT_STATUS_FAIL;
        BTC_TRACE_DEBUG("%s: feature not supported", __FUNCTION__);
    }

#else
    BTC_TRACE_DEBUG("%s: feature not enabled", __FUNCTION__);
#endif

    return status;
}

static bt_status_t btc_avrc_ct_send_set_absolute_volume_cmd(uint8_t tl, uint8_t volume)
{
    tAVRC_STS status = BT_STATUS_UNSUPPORTED;

#if (AVRC_METADATA_INCLUDED == TRUE)
    CHECK_ESP_RC_CONNECTED;

    tAVRC_COMMAND avrc_cmd = {0};
    BT_HDR *p_msg = NULL;

    avrc_cmd.volume.opcode = AVRC_OP_VENDOR;
    avrc_cmd.volume.status = AVRC_STS_NO_ERROR;
    avrc_cmd.volume.volume = volume;
    avrc_cmd.volume.pdu = AVRC_PDU_SET_ABSOLUTE_VOLUME;

    if (btc_rc_cb.rc_features & BTA_AV_FEAT_METADATA) {
        status = AVRC_BldCommand(&avrc_cmd, &p_msg);
        if (status == AVRC_STS_NO_ERROR) {
            BTA_AvMetaCmd(btc_rc_cb.rc_handle, tl, AVRC_CMD_CTRL, p_msg);
            status = BT_STATUS_SUCCESS;
        }
    } else {
        status = BT_STATUS_FAIL;
        BTC_TRACE_DEBUG("%s: feature not supported", __FUNCTION__);
    }

#else
    BTC_TRACE_DEBUG("%s: feature not enabled", __FUNCTION__);
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

    if (btc_rc_cb.rc_features & BTA_AV_FEAT_METADATA) {
        status = AVRC_BldCommand(&avrc_cmd, &p_msg);
        if (status == AVRC_STS_NO_ERROR) {
            BTA_AvMetaCmd(btc_rc_cb.rc_handle, tl, AVRC_CMD_STATUS, p_msg);
            status = BT_STATUS_SUCCESS;
        }
    } else {
        status = BT_STATUS_FAIL;
        BTC_TRACE_DEBUG("%s: feature not supported", __FUNCTION__);
    }

#else
    BTC_TRACE_DEBUG("%s: feature not enabled", __FUNCTION__);
#endif

    return status;
}

static bt_status_t btc_avrc_ct_send_passthrough_cmd(uint8_t tl, uint8_t key_code, uint8_t key_state)
{
    tAVRC_STS status = BT_STATUS_UNSUPPORTED;

#if (AVRC_CTLR_INCLUDED == TRUE)
    CHECK_ESP_RC_CONNECTED;
    BTC_TRACE_DEBUG("%s: key-code: %d, key-state: %d", __FUNCTION__,
                    key_code, key_state);
    if (btc_rc_cb.rc_features & BTA_AV_FEAT_RCTG) {
        BTA_AvRemoteCmd(btc_rc_cb.rc_handle, tl,
                        (tBTA_AV_RC)key_code, (tBTA_AV_STATE)key_state);
        status =  BT_STATUS_SUCCESS;
        BTC_TRACE_API("%s: successfully sent passthrough command to BTA", __FUNCTION__);
    } else {
        status = BT_STATUS_FAIL;
        BTC_TRACE_DEBUG("%s: feature not supported", __FUNCTION__);
    }
#else
    BTC_TRACE_DEBUG("%s: feature not enabled", __FUNCTION__);
#endif

    return status;
}

#if BTC_AV_CA_INCLUDED

static void btc_avrc_ct_cover_art_connect(UINT16 mtu)
{
    if (!btc_rc_cb.rc_cover_art_connected) {
        BTA_AvCaOpen(btc_rc_cb.rc_handle, mtu);
    }
    else {
        BTC_TRACE_WARNING("%s: cover art already connected", __FUNCTION__);
    }
    return;
}

static void btc_avrc_ct_cover_art_disconnect(void)
{
    if (btc_rc_cb.rc_cover_art_connected) {
        BTA_AvCaClose(btc_rc_cb.rc_handle);
    }
    else {
        BTC_TRACE_WARNING("%s: cover art not connected", __FUNCTION__);
    }
    return;
}

static void btc_avrc_ct_cover_art_get_image_properties(UINT8 *image_handle)
{
    if (btc_rc_cb.rc_cover_art_connected) {
        BTA_AvCaGet(btc_rc_cb.rc_handle, BTA_AV_CA_GET_IMAGE_PROPERTIES, image_handle, NULL, 0);
    }
    else {
        BTC_TRACE_WARNING("%s: cover art not connected", __FUNCTION__);
    }
    return;
}

static void btc_avrc_ct_cover_art_get_image(UINT8 *image_handle, UINT8 *image_descriptor, UINT16 image_descriptor_len)
{
    if (btc_rc_cb.rc_cover_art_connected) {
        BTA_AvCaGet(btc_rc_cb.rc_handle, BTA_AV_CA_GET_IMAGE, image_handle, image_descriptor, image_descriptor_len);
    }
    else {
        BTC_TRACE_WARNING("%s: cover art not connected", __FUNCTION__);
    }
    return;
}

static void btc_avrc_ct_cover_art_get_linked_thumbnail(UINT8 *image_handle)
{
    if (btc_rc_cb.rc_cover_art_connected) {
        BTA_AvCaGet(btc_rc_cb.rc_handle, BTA_AV_CA_GET_LINKED_THUMBNAIL, image_handle, NULL, 0);
    }
    else {
        BTC_TRACE_WARNING("%s: cover art not connected", __FUNCTION__);
    }
    return;
}

#endif /* BTC_AV_CA_INCLUDED */

/*******************************************************************************
**
** Function         btc_avrc_tg_init
**
** Description      Initializes the AVRC Target interface
**
** Returns          esp_err_t
**
*******************************************************************************/
static void btc_avrc_tg_init(void)
{
    BTC_TRACE_DEBUG("## %s ##", __FUNCTION__);
    if (s_rc_tg_init == BTC_RC_TG_INIT_MAGIC) {
        BTC_TRACE_WARNING("%s already initialized", __FUNCTION__);
        return;
    }

    /// initialize TG-specific resources
    memcpy(s_psth_supported_cmd, cs_psth_dft_supported_cmd, sizeof(s_psth_supported_cmd));
    s_rn_supported_evt = cs_rn_dft_supported_evt;

    /// initialize CT-TG shared resources
    if (s_rc_ct_init != BTC_RC_CT_INIT_MAGIC) {
        memset (&btc_rc_cb, 0, sizeof(btc_rc_cb));

        if (!g_av_with_rc) {
            g_av_with_rc = true;
        }

        if (g_a2dp_on_init) {
            BTC_TRACE_WARNING("AVRC Target is expected to be initialized in advance of A2DP !!!");
        }
    }

    s_rc_tg_init = BTC_RC_TG_INIT_MAGIC;
}


/***************************************************************************
**
** Function         btc_avrc_tg_deinit
**
** Description      Closes the AVRC Target interface
**
** Returns          void
**
***************************************************************************/
static void btc_avrc_tg_deinit(void)
{
    BTC_TRACE_API("## %s ##", __FUNCTION__);

    if (g_a2dp_on_deinit) {
        BTC_TRACE_WARNING("A2DP already deinit, AVRC TG should deinit in advance of A2DP !!!");
    }

    if (s_rc_tg_init != BTC_RC_TG_INIT_MAGIC) {
        BTC_TRACE_WARNING("%s not initialized", __FUNCTION__);
        return;
    }

    /// deinit TG-specific resources
    memset(s_psth_supported_cmd, 0, sizeof(s_psth_supported_cmd));
    s_rn_supported_evt = 0;
    s_rc_tg_init = 0;

    /// deinit CT-TG shared resources
    if (s_rc_ct_init != BTC_RC_CT_INIT_MAGIC) {
        memset (&btc_rc_cb, 0, sizeof(btc_rc_cb));
        if (g_av_with_rc) {
            g_av_with_rc = false;
        }
    }

    BTC_TRACE_API("## %s ## completed", __FUNCTION__);
}

static void btc_avrc_tg_send_rn_rsp(esp_avrc_rn_event_ids_t event_id, esp_avrc_rn_rsp_t rsp, const esp_avrc_rn_param_t *param)
{
    tAVRC_RESPONSE avrc_rsp;
    if (! btc_avrc_tg_connected_p()) {
        BTC_TRACE_WARNING("%s, RC unconnected, operation fail, event_id 0x%x", __FUNCTION__, event_id);
        return;
    }

    if (btc_rc_cb.rc_ntf[event_id - 1].registered == FALSE) {
        BTC_TRACE_ERROR("Event id not registered: event_id = %x", event_id);
        return;
    }
    memset(&(avrc_rsp.reg_notif), 0, sizeof(tAVRC_REG_NOTIF_RSP));
    avrc_rsp.reg_notif.event_id = event_id;

    switch (event_id) {
    case ESP_AVRC_RN_VOLUME_CHANGE:
        avrc_rsp.reg_notif.param.volume = param->volume;
        break;
    // todo: implement other event notifications
    default:
        BTC_TRACE_WARNING("%s : Unhandled event ID : 0x%x", __FUNCTION__, event_id);
        return;
    }

    avrc_rsp.reg_notif.pdu = AVRC_PDU_REGISTER_NOTIFICATION;
    avrc_rsp.reg_notif.opcode = opcode_from_pdu(AVRC_PDU_REGISTER_NOTIFICATION);
    avrc_rsp.get_play_status.status = AVRC_STS_NO_ERROR;

    /* Send the response. */
    send_metamsg_rsp(btc_rc_cb.rc_handle, btc_rc_cb.rc_ntf[event_id - 1].label,
                     ((rsp == ESP_AVRC_RN_RSP_INTERIM) ? AVRC_CMD_NOTIF : AVRC_RSP_CHANGED), &avrc_rsp);
    return;
}

void btc_avrc_ct_call_handler(btc_msg_t *msg)
{
    btc_avrc_args_t *arg = (btc_avrc_args_t *)(msg->arg);
    switch (msg->act) {
    case BTC_AVRC_CT_API_INIT_EVT: {
        btc_avrc_ct_init();
        // todo: callback to application
        break;
    }
    case BTC_AVRC_CT_API_DEINIT_EVT: {
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
    case BTC_AVRC_STATUS_API_SND_GET_RN_CAPS_EVT: {
        btc_avrc_ct_send_get_rn_caps_cmd(arg->get_caps_cmd.tl);
        break;
    }
    case BTC_AVRC_NOTIFY_API_SND_REG_NOTIFY_EVT: {
        btc_avrc_ct_send_register_notification_cmd(arg->rn_cmd.tl, arg->rn_cmd.event_id, arg->rn_cmd.event_parameter);
        break;
    }
    case BTC_AVRC_CTRL_API_SND_SET_PLAYER_SETTING_EVT: {
        btc_avrc_ct_send_set_player_value_cmd(arg->ps_cmd.tl, arg->ps_cmd.attr_id, arg->ps_cmd.value_id);
        break;
    }
    case BTC_AVRC_CTRL_API_SND_SET_ABSOLUTE_VOLUME_EVT: {
        btc_avrc_ct_send_set_absolute_volume_cmd(arg->set_abs_vol_cmd.tl, arg->set_abs_vol_cmd.volume);
        break;
    }
#if BTC_AV_CA_INCLUDED
    case BTC_AVRC_CT_API_COVER_ART_CONNECT_EVT: {
        btc_avrc_ct_cover_art_connect(arg->ca_conn.mtu);
        break;
    }
    case BTC_AVRC_CT_API_COVER_ART_DISCONNECT_EVT: {
        btc_avrc_ct_cover_art_disconnect();
        break;
    }
    case BTC_AVRC_CT_API_COVER_ART_GET_IMAGE_PROPERTIES_EVT: {
        btc_avrc_ct_cover_art_get_image_properties(arg->ca_get_img_prop.image_handle);
        break;
    }
    case BTC_AVRC_CT_API_COVER_ART_GET_IMAGE_EVT: {
        btc_avrc_ct_cover_art_get_image(arg->ca_get_img.image_handle, arg->ca_get_img.image_descriptor, arg->ca_get_img.image_descriptor_len);
        break;
    }
    case BTC_AVRC_CT_API_COVER_ART_GET_LINKED_THUMBNAIL_EVT: {
        btc_avrc_ct_cover_art_get_linked_thumbnail(arg->ca_get_lk_thn.image_handle);
        break;
    }
#endif
    default:
        BTC_TRACE_WARNING("%s : unhandled event: %d\n", __FUNCTION__, msg->act);
    }
}

void btc_avrc_tg_call_handler(btc_msg_t *msg)
{
    btc_avrc_tg_args_t *arg = (btc_avrc_tg_args_t *)(msg->arg);
    switch (msg->act) {
    case BTC_AVRC_TG_API_INIT_EVT: {
        btc_avrc_tg_init();
        break;
    }
    case BTC_AVRC_TG_API_DEINIT_EVT: {
        btc_avrc_tg_deinit();
        break;
    }
    case BTC_AVRC_TG_API_SET_RN_SUPPORTED_EVT: {
        btc_avrc_tg_set_rn_supported_evt(arg->set_rn_evt);
        break;
    }
    case BTC_AVRC_TG_API_SET_PSTH_SUPPORTED_CMD_EVT: {
        btc_avrc_tg_set_supported_command(arg->set_psth_cmd);
        break;
    }
    case BTC_AVRC_TG_API_SEND_RN_RSP_EVT: {
        btc_avrc_tg_send_rn_rsp(arg->rn_rsp.event_id, arg->rn_rsp.rsp,
                                &arg->rn_rsp.param);
        break;
    }
    default:
        BTC_TRACE_WARNING("%s : unhandled event: %d\n", __FUNCTION__, msg->act);
    }

    btc_avrc_tg_arg_deep_free(msg);
}

#endif /* #if BTC_AV_INCLUDED */

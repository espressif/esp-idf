/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "esp_a2dp_api.h"
#include "esp_avrc_api.h"

#include "bt_app_core_utils.h"
#include "bredr_app_common_utils.h"
#include "a2dp_sink_common_utils.h"
#include "a2dp_utils_tags.h"
#include "avrcp_utils_tags.h"
#include "avrcp_common_utils.h"
#if CONFIG_EXAMPLE_A2DP_SINK_USE_EXTERNAL_CODEC == FALSE
#include "a2dp_sink_int_codec_utils.h"
#else
#include "a2dp_sink_ext_codec_utils.h"
#endif

/* device name */
static const char local_device_name[] = CONFIG_EXAMPLE_LOCAL_DEVICE_NAME;

/* event for stack up */
enum {
    BT_APP_EVT_STACK_UP = 0,
};

#ifdef CONFIG_BT_A2DP_SINK_MAX_CONN
#define BT_APP_A2D_MAX_PEERS    CONFIG_BT_A2DP_SINK_MAX_CONN
#else
#define BT_APP_A2D_MAX_PEERS    1
#endif

#if BT_APP_A2D_MAX_PEERS > 1
/* Addresses of the source devices that are connected right now. Only one of them
 * is rendered at a time, the stack reports which one with
 * ESP_A2D_SNK_ACTIVE_DEVICE_EVT and discards the media packets of the others. */
static esp_bd_addr_t s_a2d_peers[BT_APP_A2D_MAX_PEERS];
static int s_a2d_peer_num;
#endif

/********************************
 * STATIC FUNCTION DECLARATIONS
 *******************************/

/* Device callback function */
static void bt_app_dev_cb(esp_bt_dev_cb_event_t event, esp_bt_dev_cb_param_t *param);

/* GAP callback function */
static void bt_app_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);

/* callback function for A2DP sink */
static void bt_app_a2d_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param);

#if CONFIG_EXAMPLE_A2DP_SINK_USE_EXTERNAL_CODEC == FALSE
/* callback function for A2DP sink audio data stream */
static void bt_app_a2d_data_cb(const uint8_t *data, uint32_t len);
#else
/* callback function for A2DP sink undecoded audio data */
static void bt_app_a2d_audio_data_cb(esp_a2d_conn_hdl_t conn_hdl, esp_a2d_audio_buff_t *audio_buf);
#endif

/* handler for AVRCP controller events */
static void bt_app_avrc_ct_evt_hdl(uint16_t event, void *param);

/* callback function for AVRCP controller */
static void bt_app_rc_ct_cb(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *param);

/* callback function for AVRCP target */
static void bt_app_rc_tg_cb(esp_avrc_tg_cb_event_t event, esp_avrc_tg_cb_param_t *param);

/* handler for bluetooth stack enabled events */
static void bt_av_hdl_stack_evt(uint16_t event, void *p_param);

/*******************************
 * STATIC FUNCTION DEFINITIONS
 ******************************/

static void bt_app_dev_cb(esp_bt_dev_cb_event_t event, esp_bt_dev_cb_param_t *param)
{
    bredr_app_dev_evt_def_hdl(event, param);
}

static void bt_app_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    bredr_app_gap_evt_def_hdl(event, param);
}

#if BT_APP_A2D_MAX_PEERS > 1
static int bt_app_a2d_peer_find(const esp_bd_addr_t bda)
{
    for (int i = 0; i < s_a2d_peer_num; i++) {
        if (memcmp(s_a2d_peers[i], bda, sizeof(esp_bd_addr_t)) == 0) {
            return i;
        }
    }
    return -1;
}

static void bt_app_a2d_peer_add(const esp_bd_addr_t bda)
{
    if (bt_app_a2d_peer_find(bda) >= 0 || s_a2d_peer_num >= BT_APP_A2D_MAX_PEERS) {
        return;
    }
    memcpy(s_a2d_peers[s_a2d_peer_num++], bda, sizeof(esp_bd_addr_t));
}

static void bt_app_a2d_peer_remove(const esp_bd_addr_t bda)
{
    int idx = bt_app_a2d_peer_find(bda);

    if (idx < 0) {
        return;
    }
    s_a2d_peer_num--;
    if (idx != s_a2d_peer_num) {
        memcpy(s_a2d_peers[idx], s_a2d_peers[s_a2d_peer_num], sizeof(esp_bd_addr_t));
    }
}

/* Track how many sources are connected so that the device stays visible until
 * every slot is taken, and so that the audio output is only torn down when the
 * last source goes away. */
static void bt_app_a2d_multi_evt_hdl(uint16_t event, void *param)
{
    esp_a2d_cb_param_t *a2d = (esp_a2d_cb_param_t *)(param);

    if (event == ESP_A2D_SNK_ACTIVE_DEVICE_EVT) {
        uint8_t *bda = a2d->a2d_snk_active_device.remote_bda;

        if (a2d->a2d_snk_active_device.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGW(BT_AV_TAG, "A2DP set active failed, status %d, [%02x:%02x:%02x:%02x:%02x:%02x]",
                     a2d->a2d_snk_active_device.status,
                     bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
            return;
        }
        if (a2d->a2d_snk_active_device.active) {
            ESP_LOGI(BT_AV_TAG, "A2DP rendering [%02x:%02x:%02x:%02x:%02x:%02x], other sources suspended",
                     bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
        } else {
            ESP_LOGI(BT_AV_TAG, "A2DP no source is being rendered");
        }
        return;
    }

    if (event == ESP_A2D_CONNECTION_STATE_EVT) {
        if (a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
            bt_app_a2d_peer_add(a2d->conn_stat.remote_bda);
        } else if (a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
            bt_app_a2d_peer_remove(a2d->conn_stat.remote_bda);
            if (s_a2d_peer_num > 0) {
                /* another source is still connected, leave its audio path alone */
                esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
                return;
            }
        }
    }

#if CONFIG_EXAMPLE_A2DP_SINK_USE_EXTERNAL_CODEC == FALSE
    bt_a2d_evt_int_codec_hdl(event, param);
#else
    bt_a2d_evt_ext_codec_hdl(event, param);
#endif

    /* the codec handler turns the device invisible on every connection, stay
     * visible as long as a slot is free */
    if (event == ESP_A2D_CONNECTION_STATE_EVT && s_a2d_peer_num < BT_APP_A2D_MAX_PEERS) {
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
    }
}
#endif /* BT_APP_A2D_MAX_PEERS > 1 */

static void bt_app_a2d_evt_hdl(uint16_t event, void *param)
{
    esp_a2d_cb_param_t *a2d = (esp_a2d_cb_param_t *)(param);

    switch (event) {
    /* when a2dp init or deinit completed, this event comes */
    case ESP_A2D_PROF_STATE_EVT: {
        if (ESP_A2D_INIT_SUCCESS == a2d->a2d_prof_stat.init_state) {
            ESP_LOGI(BT_AV_TAG, "A2DP PROF STATE: Init Complete");
#if CONFIG_EXAMPLE_A2DP_SINK_USE_EXTERNAL_CODEC == TRUE
            esp_a2d_mcc_t mcc = {0};
            mcc.type = ESP_A2D_MCT_SBC;
            mcc.cie.sbc_info.samp_freq = ESP_A2D_SBC_CIE_SF_16K |
                                         ESP_A2D_SBC_CIE_SF_32K |
                                         ESP_A2D_SBC_CIE_SF_44K |
                                         ESP_A2D_SBC_CIE_SF_48K;
            mcc.cie.sbc_info.ch_mode = ESP_A2D_SBC_CIE_CH_MODE_MONO |
                                       ESP_A2D_SBC_CIE_CH_MODE_DUAL_CHANNEL |
                                       ESP_A2D_SBC_CIE_CH_MODE_STEREO |
                                       ESP_A2D_SBC_CIE_CH_MODE_JOINT_STEREO;
            mcc.cie.sbc_info.block_len = ESP_A2D_SBC_CIE_BLOCK_LEN_4 |
                                         ESP_A2D_SBC_CIE_BLOCK_LEN_8 |
                                         ESP_A2D_SBC_CIE_BLOCK_LEN_12 |
                                         ESP_A2D_SBC_CIE_BLOCK_LEN_16;
            mcc.cie.sbc_info.num_subbands = ESP_A2D_SBC_CIE_NUM_SUBBANDS_4 | ESP_A2D_SBC_CIE_NUM_SUBBANDS_8;
            mcc.cie.sbc_info.alloc_mthd = ESP_A2D_SBC_CIE_ALLOC_MTHD_SNR | ESP_A2D_SBC_CIE_ALLOC_MTHD_LOUDNESS;
            mcc.cie.sbc_info.max_bitpool = 250;
            mcc.cie.sbc_info.min_bitpool = 2;
            ESP_ERROR_CHECK(esp_a2d_sink_register_stream_endpoint(0, &mcc));
#endif
            /* Get the default value of the delay value */
            esp_a2d_sink_get_delay_value();
        } else {
            ESP_LOGI(BT_AV_TAG, "A2DP PROF STATE: Deinit Complete");
        }
        break;
    }
    /* others */
    default:
        ESP_LOGE(BT_AV_TAG, "%s unhandled event: %d", __func__, event);
        break;
    }
}

static void bt_app_a2d_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param)
{
    switch (event) {
    case ESP_A2D_PROF_STATE_EVT: {
        bt_app_work_dispatch(bt_app_a2d_evt_hdl, event, param, sizeof(esp_a2d_cb_param_t), NULL, NULL);
        break;
    }
    case ESP_A2D_SNK_PSC_CFG_EVT:
    case ESP_A2D_SNK_SET_DELAY_VALUE_EVT:
    case ESP_A2D_SNK_GET_DELAY_VALUE_EVT: {
        bt_app_work_dispatch(bt_a2d_evt_def_hdl, event, param, sizeof(esp_a2d_cb_param_t), NULL, NULL);
        break;
    }
    case ESP_A2D_CONNECTION_STATE_EVT:
    case ESP_A2D_AUDIO_STATE_EVT:
    case ESP_A2D_AUDIO_CFG_EVT:
    case ESP_A2D_SEP_REG_STATE_EVT:
    case ESP_A2D_SNK_ACTIVE_DEVICE_EVT:
#if BT_APP_A2D_MAX_PEERS > 1
    {
        bt_app_work_dispatch(bt_app_a2d_multi_evt_hdl, event, param, sizeof(esp_a2d_cb_param_t), NULL, NULL);
        break;
    }
#else
    {
        if (event == ESP_A2D_SNK_ACTIVE_DEVICE_EVT) {
            break;
        }
#if CONFIG_EXAMPLE_A2DP_SINK_USE_EXTERNAL_CODEC == FALSE
        bt_app_work_dispatch(bt_a2d_evt_int_codec_hdl, event, param, sizeof(esp_a2d_cb_param_t), NULL, NULL);
#else
        bt_app_work_dispatch(bt_a2d_evt_ext_codec_hdl, event, param, sizeof(esp_a2d_cb_param_t), NULL, NULL);
#endif
        break;
    }
#endif
    default:
        ESP_LOGE(BT_AV_TAG, "Invalid A2DP event: %d", event);
        break;
    }
}

#if CONFIG_EXAMPLE_A2DP_SINK_USE_EXTERNAL_CODEC == FALSE
static void bt_app_a2d_data_cb(const uint8_t *data, uint32_t len)
{
    bt_a2d_data_hdl(data, len);
}
#else
static void bt_app_a2d_audio_data_cb(esp_a2d_conn_hdl_t conn_hdl, esp_a2d_audio_buff_t *audio_buf)
{
    bt_a2d_audio_data_hdl(conn_hdl, audio_buf);
}
#endif

static void bt_app_avrc_ct_evt_hdl(uint16_t event, void *param)
{
    esp_avrc_ct_cb_param_t *rc = (esp_avrc_ct_cb_param_t *)(param);

    switch (event) {
    case ESP_AVRC_CT_CONNECTION_STATE_EVT: {
        bt_avrc_common_ct_evt_def_hdl(event, param);
        if (rc->conn_stat.connected) {
            bt_avrc_common_ct_get_peer_rn_cap();
        } else {
            bt_avrc_common_ct_set_peer_rn_cap(0);
        }
        break;
    }
    case ESP_AVRC_CT_GET_RN_CAPABILITIES_RSP_EVT: {
        bt_avrc_common_ct_evt_def_hdl(event, param);
        bt_avrc_common_ct_set_peer_rn_cap(rc->get_rn_caps_rsp.evt_set.bits);
        bt_avrc_common_ct_rn_track_changed();
        bt_avrc_common_ct_rn_play_status_changed();
        bt_avrc_common_ct_rn_play_pos_changed();
        break;
    }
    case ESP_AVRC_CT_CHANGE_NOTIFY_EVT: {
        bt_avrc_common_ct_evt_def_hdl(event, param);
        bt_avrc_common_ct_notify_evt_handler(rc->change_ntf.event_id, &rc->change_ntf.event_parameter);
        break;
    }
    default:
        bt_avrc_common_ct_evt_def_hdl(event, param);
        break;
    }
}

static void bt_app_rc_ct_cb(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *param)
{
    switch (event) {
    case ESP_AVRC_CT_CONNECTION_STATE_EVT:
    case ESP_AVRC_CT_PASSTHROUGH_RSP_EVT:
    case ESP_AVRC_CT_CHANGE_NOTIFY_EVT:
    case ESP_AVRC_CT_REMOTE_FEATURES_EVT:
    case ESP_AVRC_CT_GET_RN_CAPABILITIES_RSP_EVT:
    case ESP_AVRC_CT_PROF_STATE_EVT: {
        if (!bt_app_work_dispatch(bt_app_avrc_ct_evt_hdl, event, param, sizeof(esp_avrc_ct_cb_param_t), NULL, NULL)) {
            ESP_LOGE(BT_RC_CT_TAG, "CT event %d dispatch failed", event);
        }
        break;
    }
    default:
        ESP_LOGE(BT_RC_CT_TAG, "Invalid AVRC event: %d", event);
        break;
    }
}

static void bt_app_rc_tg_cb(esp_avrc_tg_cb_event_t event, esp_avrc_tg_cb_param_t *param)
{
    switch (event) {
    case ESP_AVRC_TG_CONNECTION_STATE_EVT:
    case ESP_AVRC_TG_REMOTE_FEATURES_EVT:
    case ESP_AVRC_TG_PASSTHROUGH_CMD_EVT:
    case ESP_AVRC_TG_SET_ABSOLUTE_VOLUME_CMD_EVT:
    case ESP_AVRC_TG_REGISTER_NOTIFICATION_EVT:
    case ESP_AVRC_TG_SET_PLAYER_APP_VALUE_EVT:
    case ESP_AVRC_TG_PROF_STATE_EVT: {
        if (!bt_app_work_dispatch(bt_avrc_common_tg_evt_def_hdl, event, param, sizeof(esp_avrc_tg_cb_param_t), NULL, NULL)) {
            ESP_LOGE(BT_RC_TG_TAG, "TG event %d dispatch failed", event);
        }
        break;
    }
    default:
        ESP_LOGE(BT_RC_TG_TAG, "Invalid AVRC event: %d", event);
        break;
    }
}

static void bt_av_hdl_stack_evt(uint16_t event, void *p_param)
{
    ESP_LOGD(BT_AV_TAG, "%s event: %d", __func__, event);

    switch (event) {
    /* when do the stack up, this event comes */
    case BT_APP_EVT_STACK_UP: {
        esp_bt_gap_set_device_name(local_device_name);
        esp_bt_dev_register_callback(bt_app_dev_cb);
        esp_bt_gap_register_callback(bt_app_gap_cb);

        /* AVRCP must be initialized before A2DP, otherwise the stack
         * enables A2DP without AVRCP and rejects the peer's AVCTP (PSM 23). */
        esp_avrc_ct_register_callback(bt_app_rc_ct_cb);
        assert(esp_avrc_ct_init() == ESP_OK);
        esp_avrc_tg_register_callback(bt_app_rc_tg_cb);
        assert(esp_avrc_tg_init() == ESP_OK);

        esp_avrc_rn_evt_cap_mask_t evt_set = {0};
        esp_avrc_rn_evt_bit_mask_operation(ESP_AVRC_BIT_MASK_OP_SET, &evt_set, ESP_AVRC_RN_VOLUME_CHANGE);
        assert(esp_avrc_tg_set_rn_evt_cap(&evt_set) == ESP_OK);

        esp_a2d_register_callback(&bt_app_a2d_cb);
        assert(esp_a2d_sink_init() == ESP_OK);

#if CONFIG_EXAMPLE_A2DP_SINK_USE_EXTERNAL_CODEC == FALSE
        esp_a2d_sink_register_data_callback(bt_app_a2d_data_cb);
#else
        esp_a2d_sink_register_audio_data_callback(bt_app_a2d_audio_data_cb);
#endif

        /* Get local device name */
        esp_bt_gap_get_device_name();

        /* set discoverable and connectable mode, wait to be connected */
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        break;
    }
    /* others */
    default:
        ESP_LOGE(BT_AV_TAG, "%s unhandled event: %d", __func__, event);
        break;
    }
}

/*******************************
 * MAIN ENTRY POINT
 ******************************/

void app_main(void)
{
    ESP_ERROR_CHECK(bredr_app_common_init());

    bt_app_task_start_up();
    /* bluetooth device name, connection mode and profile set up */
    bt_app_work_dispatch(bt_av_hdl_stack_evt, BT_APP_EVT_STACK_UP, NULL, 0, NULL, NULL);
}

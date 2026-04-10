/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "esp_log.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "esp_avrc_api.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "bt_app_core_utils.h"
#include "bt_app_av.h"
#include "avrcp_utils_tags.h"
#include "avrcp_common_utils.h"
#include "avrcp_metadata_service.h"
#include "avrcp_metadata_utils.h"
#include "a2dp_utils_tags.h"
#include "a2dp_sink_common_utils.h"
#if CONFIG_EXAMPLE_A2DP_SINK_STREAM_ENABLE
#if CONFIG_EXAMPLE_A2DP_SINK_USE_EXTERNAL_CODEC == FALSE
#include "a2dp_sink_int_codec_utils.h"
#else
#include "a2dp_sink_ext_codec_utils.h"
#endif
#endif
#include "avrcp_cover_art_service.h"
#include "avrcp_cover_art_utils.h"

/********************************
 * STATIC FUNCTION DEFINITIONS
 *******************************/

static void bt_app_avrc_ct_evt_hdl(uint16_t event, void *param)
{
    ESP_LOGD(BT_RC_CT_TAG, "%s event: %d", __func__, event);

    esp_avrc_ct_cb_param_t *rc = (esp_avrc_ct_cb_param_t *)(param);

    switch (event) {
    case ESP_AVRC_CT_PASSTHROUGH_RSP_EVT:
    case ESP_AVRC_CT_PROF_STATE_EVT: {
        bt_avrc_common_ct_evt_def_hdl(event, param);
        break;
    }
    case ESP_AVRC_CT_CONNECTION_STATE_EVT: {
        bt_avrc_md_ct_evt_hdl(event, param);
        bt_avrc_ca_ct_evt_hdl(event, param);
        if (rc->conn_stat.connected) {
            /* get remote supported event_ids of peer AVRCP Target */
            bt_avrc_common_ct_get_peer_rn_cap();
        } else {
            /* clear peer notification capability record */
            bt_avrc_common_ct_set_peer_rn_cap(0);
        }
        break;
    }
    case ESP_AVRC_CT_CHANGE_NOTIFY_EVT: {
        bt_avrc_md_ct_evt_hdl(event, param);
        bt_avrc_ca_ct_evt_hdl(event, param);
        bt_avrc_common_ct_notify_evt_handler(rc->change_ntf.event_id, &rc->change_ntf.event_parameter);
        break;
    }
    case ESP_AVRC_CT_GET_RN_CAPABILITIES_RSP_EVT: {
        /* set peer notification capability record */
        bt_avrc_common_ct_set_peer_rn_cap(rc->get_rn_caps_rsp.evt_set.bits);
        bt_avrc_common_ct_rn_track_changed();
        bt_avrc_common_ct_rn_play_status_changed();
        bt_avrc_common_ct_rn_play_pos_changed();

        bt_avrc_md_ct_evt_hdl(event, param);
        bt_avrc_ca_ct_evt_hdl(event, param);
        break;
    }
    case ESP_AVRC_CT_METADATA_RSP_EVT: {
        if (rc->meta_rsp.attr_id == ESP_AVRC_MD_ATTR_COVER_ART) {
            bt_avrc_ca_ct_evt_hdl(event, param);
        } else {
            bt_avrc_md_ct_evt_hdl(event, param);
        }
        break;
    }
    case ESP_AVRC_CT_REMOTE_FEATURES_EVT: {
        bt_avrc_ca_ct_evt_hdl(event, param);
        break;
    }
    case ESP_AVRC_CT_COVER_ART_STATE_EVT:
    case ESP_AVRC_CT_COVER_ART_DATA_EVT: {
        bt_avrc_ca_ct_evt_hdl(event, param);
        break;
    }
    default:
        ESP_LOGE(BT_RC_CT_TAG, "Invalid AVRC event: %d", event);
        break;
    }
}

/********************************
 * EXTERNAL FUNCTION DEFINITIONS
 *******************************/

void bt_app_a2d_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param)
{
    switch (event) {
    case ESP_A2D_PROF_STATE_EVT:
    case ESP_A2D_SNK_PSC_CFG_EVT:
    case ESP_A2D_SNK_SET_DELAY_VALUE_EVT:
    case ESP_A2D_SNK_GET_DELAY_VALUE_EVT: {
        bt_app_work_dispatch(bt_a2d_evt_def_hdl, event, param, sizeof(esp_a2d_cb_param_t), NULL);
        break;
    }
    case ESP_A2D_CONNECTION_STATE_EVT:
    case ESP_A2D_AUDIO_STATE_EVT:
    case ESP_A2D_AUDIO_CFG_EVT:
    case ESP_A2D_SEP_REG_STATE_EVT: {
#if CONFIG_EXAMPLE_A2DP_SINK_STREAM_ENABLE
#if CONFIG_EXAMPLE_A2DP_SINK_USE_EXTERNAL_CODEC == FALSE
        bt_app_work_dispatch(bt_a2d_evt_int_codec_hdl, event, param, sizeof(esp_a2d_cb_param_t), NULL);
#else
        bt_app_work_dispatch(bt_a2d_evt_ext_codec_hdl, event, param, sizeof(esp_a2d_cb_param_t), NULL);
#endif
#else
        bt_app_work_dispatch(bt_a2d_evt_def_hdl, event, param, sizeof(esp_a2d_cb_param_t), NULL);
#endif
        break;
    }
    default:
        ESP_LOGE(BT_AV_TAG, "Invalid A2DP event: %d", event);
        break;
    }
}

#if CONFIG_EXAMPLE_A2DP_SINK_STREAM_ENABLE
#if CONFIG_EXAMPLE_A2DP_SINK_USE_EXTERNAL_CODEC == FALSE
void bt_app_a2d_data_cb(const uint8_t *data, uint32_t len)
{
    bt_a2d_data_hdl(data, len);
}
#else
void bt_app_a2d_audio_data_cb(esp_a2d_conn_hdl_t conn_hdl, esp_a2d_audio_buff_t *audio_buf)
{
    bt_a2d_audio_data_hdl(conn_hdl, audio_buf);
}
#endif
#endif

void bt_app_rc_ct_cb(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *param)
{
    switch (event) {
    case ESP_AVRC_CT_METADATA_RSP_EVT: {
        bt_app_work_dispatch(bt_app_avrc_ct_evt_hdl, event, param, sizeof(esp_avrc_ct_cb_param_t), bt_avrc_common_copy_metadata);
        break;
    }
    case ESP_AVRC_CT_COVER_ART_DATA_EVT: {
        /* we must handle ESP_AVRC_CT_COVER_ART_DATA_EVT in avrcp controller callback, */
        /* copy image data to other buff before return if need */
        if (param->cover_art_data.status == ESP_BT_STATUS_SUCCESS) {
            avrc_cover_art_srv_save_image_data(param->cover_art_data.p_data, param->cover_art_data.data_len);
        } else {
            ESP_LOGE(BT_RC_CT_TAG, "Cover Art Client get operation failed");
            break;
        }
        bt_app_work_dispatch(bt_app_avrc_ct_evt_hdl, event, param, sizeof(esp_avrc_ct_cb_param_t), NULL);
        break;
    }
    case ESP_AVRC_CT_CONNECTION_STATE_EVT:
    case ESP_AVRC_CT_PASSTHROUGH_RSP_EVT:
    case ESP_AVRC_CT_CHANGE_NOTIFY_EVT:
    case ESP_AVRC_CT_REMOTE_FEATURES_EVT:
    case ESP_AVRC_CT_GET_RN_CAPABILITIES_RSP_EVT:
    case ESP_AVRC_CT_COVER_ART_STATE_EVT:
    case ESP_AVRC_CT_PROF_STATE_EVT:
        bt_app_work_dispatch(bt_app_avrc_ct_evt_hdl, event, param, sizeof(esp_avrc_ct_cb_param_t), NULL);
        break;
    default:
        ESP_LOGE(BT_RC_CT_TAG, "Invalid AVRC event: %d", event);
        break;
    }
}

void bt_app_rc_tg_cb(esp_avrc_tg_cb_event_t event, esp_avrc_tg_cb_param_t *param)
{
    switch (event) {
    case ESP_AVRC_TG_CONNECTION_STATE_EVT:
    case ESP_AVRC_TG_REMOTE_FEATURES_EVT:
    case ESP_AVRC_TG_PASSTHROUGH_CMD_EVT:
    case ESP_AVRC_TG_SET_ABSOLUTE_VOLUME_CMD_EVT:
    case ESP_AVRC_TG_REGISTER_NOTIFICATION_EVT:
    case ESP_AVRC_TG_SET_PLAYER_APP_VALUE_EVT:
    case ESP_AVRC_TG_PROF_STATE_EVT:
        bt_app_work_dispatch(bt_avrc_common_tg_evt_def_hdl, event, param, sizeof(esp_avrc_tg_cb_param_t), NULL);
        break;
    default:
        ESP_LOGE(BT_RC_TG_TAG, "Invalid AVRC event: %d", event);
        break;
    }
}

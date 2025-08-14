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
#include "esp_bt_main.h"
#include "esp_avrc_api.h"

#include "avrcp_common_utils.h"
#include "avrcp_utils_tags.h"

typedef struct {
    uint8_t tl;                             /* AVRCP transaction labels */
    esp_avrc_rn_evt_cap_mask_t peer_rn_cap; /* AVRCP target notification capability bit mask */
} avrc_common_cb_t;

/*******************************
 * STATIC VARIABLE DEFINITIONS
 ******************************/

static avrc_common_cb_t s_avrc_common_cb;

/********************************
 * EXTERNAL FUNCTION DEFINITIONS
 *******************************/

void bt_avrc_common_ct_evt_def_hdl(uint16_t event, void *param)
{
    ESP_LOGD(BT_RC_CT_TAG, "%s event: %d", __func__, event);

    esp_avrc_ct_cb_param_t *rc = (esp_avrc_ct_cb_param_t *)(param);

    switch (event) {
    /* when connection state changed, this event comes */
    case ESP_AVRC_CT_CONNECTION_STATE_EVT: {
        uint8_t *bda = rc->conn_stat.remote_bda;
        ESP_LOGI(BT_RC_CT_TAG, "AVRC conn_state event: state %d, [%02x:%02x:%02x:%02x:%02x:%02x]",
                 rc->conn_stat.connected, bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
        break;
    }
    /* when passthrough response, this event comes */
    case ESP_AVRC_CT_PASSTHROUGH_RSP_EVT: {
        ESP_LOGI(BT_RC_CT_TAG, "AVRC passthrough rsp: key_code 0x%x, key_state %d, rsp_code %d", rc->psth_rsp.key_code,
                 rc->psth_rsp.key_state, rc->psth_rsp.rsp_code);
        break;
    }
    /* when metadata response, this event comes */
    case ESP_AVRC_CT_METADATA_RSP_EVT: {
        ESP_LOGI(BT_RC_CT_TAG, "AVRC metadata rsp: attribute id 0x%x, %s", rc->meta_rsp.attr_id, rc->meta_rsp.attr_text);
        break;
    }
    /* when notified, this event comes */
    case ESP_AVRC_CT_CHANGE_NOTIFY_EVT: {
        ESP_LOGI(BT_RC_CT_TAG, "AVRC event notification: %d", rc->change_ntf.event_id);
        break;
    }
    /* when feature of remote device indicated, this event comes */
    case ESP_AVRC_CT_REMOTE_FEATURES_EVT: {
        ESP_LOGI(BT_RC_CT_TAG, "AVRC remote features %"PRIx32", TG features %x", rc->rmt_feats.feat_mask, rc->rmt_feats.tg_feat_flag);
        break;
    }
    /* when notification capability of peer device got, this event comes */
    case ESP_AVRC_CT_GET_RN_CAPABILITIES_RSP_EVT: {
        ESP_LOGI(BT_RC_CT_TAG, "remote rn_cap: count %d, bitmask 0x%x", rc->get_rn_caps_rsp.cap_count,
                 rc->get_rn_caps_rsp.evt_set.bits);
        break;
    }
    /* when avrcp controller init or deinit completed, this event comes */
    case ESP_AVRC_CT_PROF_STATE_EVT: {
        if (ESP_AVRC_INIT_SUCCESS == rc->avrc_ct_init_stat.state) {
            ESP_LOGI(BT_RC_CT_TAG, "AVRCP CT STATE: Init Complete");
        } else if (ESP_AVRC_DEINIT_SUCCESS == rc->avrc_ct_init_stat.state) {
            ESP_LOGI(BT_RC_CT_TAG, "AVRCP CT STATE: Deinit Complete");
        } else {
            ESP_LOGE(BT_RC_CT_TAG, "AVRCP CT STATE error: %d", rc->avrc_ct_init_stat.state);
        }
        break;
    }
    /* others */
    default:
        ESP_LOGE(BT_RC_CT_TAG, "%s unhandled event: %d", __func__, event);
        break;
    }
}

void bt_avrc_common_tg_evt_def_hdl(uint16_t event, void *param)
{
    ESP_LOGD(BT_RC_TG_TAG, "%s event: %d", __func__, event);

    esp_avrc_tg_cb_param_t *rc = (esp_avrc_tg_cb_param_t *)(param);

    switch (event) {
    /* when connection state changed, this event comes */
    case ESP_AVRC_TG_CONNECTION_STATE_EVT: {
        uint8_t *bda = rc->conn_stat.remote_bda;
        ESP_LOGI(BT_RC_TG_TAG, "AVRC conn_state evt: state %d, [%02x:%02x:%02x:%02x:%02x:%02x]",
                 rc->conn_stat.connected, bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
        break;
    }
    /* when passthrough commanded, this event comes */
    case ESP_AVRC_TG_PASSTHROUGH_CMD_EVT: {
        ESP_LOGI(BT_RC_TG_TAG, "AVRC passthrough cmd: key_code 0x%x, key_state %d", rc->psth_cmd.key_code, rc->psth_cmd.key_state);
        break;
    }
    /* when absolute volume command from remote device set, this event comes */
    case ESP_AVRC_TG_SET_ABSOLUTE_VOLUME_CMD_EVT: {
        ESP_LOGI(BT_RC_TG_TAG, "AVRC set absolute volume: %d%%", (int)rc->set_abs_vol.volume * 100 / 0x7f);
        break;
    }
    /* when notification registered, this event comes */
    case ESP_AVRC_TG_REGISTER_NOTIFICATION_EVT: {
        ESP_LOGI(BT_RC_TG_TAG, "AVRC register event notification: %d, param: 0x%"PRIx32, rc->reg_ntf.event_id, rc->reg_ntf.event_parameter);
        break;
    }
    /* when feature of remote device indicated, this event comes */
    case ESP_AVRC_TG_REMOTE_FEATURES_EVT: {
        ESP_LOGI(BT_RC_TG_TAG, "AVRC remote features: %"PRIx32", CT features: %x", rc->rmt_feats.feat_mask, rc->rmt_feats.ct_feat_flag);
        break;
    }
    /* when avrcp target init or deinit completed, this event comes */
    case ESP_AVRC_TG_PROF_STATE_EVT: {
        if (ESP_AVRC_INIT_SUCCESS == rc->avrc_tg_init_stat.state) {
            ESP_LOGI(BT_RC_TG_TAG, "AVRCP TG STATE: Init Complete");
        } else if (ESP_AVRC_DEINIT_SUCCESS == rc->avrc_tg_init_stat.state) {
            ESP_LOGI(BT_RC_TG_TAG, "AVRCP TG STATE: Deinit Complete");
        } else {
            ESP_LOGE(BT_RC_TG_TAG, "AVRCP TG STATE error: %d", rc->avrc_tg_init_stat.state);
        }
        break;
    }
    /* others */
    default:
        ESP_LOGE(BT_RC_TG_TAG, "%s unhandled event: %d", __func__, event);
        break;
    }
}

uint8_t bt_avrc_common_alloc_tl(void)
{
    if (s_avrc_common_cb.tl > ESP_AVRC_TRANS_LABEL_MAX) {
        s_avrc_common_cb.tl = 0;
    }

    return s_avrc_common_cb.tl++;
}

void bt_avrc_common_copy_metadata(void *p_dest, void *p_src, int len)
{
    esp_avrc_ct_cb_param_t *p_dest_rc = (esp_avrc_ct_cb_param_t *)(p_dest);
    esp_avrc_ct_cb_param_t *p_src_rc = (esp_avrc_ct_cb_param_t *)(p_src);

    p_dest_rc->meta_rsp.attr_id = p_src_rc->meta_rsp.attr_id;
    p_dest_rc->meta_rsp.attr_length = p_src_rc->meta_rsp.attr_length;

    p_dest_rc->meta_rsp.attr_text = (uint8_t *) malloc(p_dest_rc->meta_rsp.attr_length + 1);
    memcpy(p_dest_rc->meta_rsp.attr_text, p_src_rc->meta_rsp.attr_text, p_dest_rc->meta_rsp.attr_length);
    p_dest_rc->meta_rsp.attr_text[p_dest_rc->meta_rsp.attr_length] = 0;
}

void bt_avrc_common_ct_get_peer_rn_cap(void)
{
    esp_avrc_ct_send_get_rn_capabilities_cmd(bt_avrc_common_alloc_tl());
}

void bt_avrc_common_ct_set_peer_rn_cap(uint16_t peer_rn_cap_t)
{
    s_avrc_common_cb.peer_rn_cap.bits = peer_rn_cap_t;
}

void bt_avrc_common_ct_rn_track_changed(void)
{
    /* register notification if peer support the event_id */
    if (esp_avrc_rn_evt_bit_mask_operation(ESP_AVRC_BIT_MASK_OP_TEST, &s_avrc_common_cb.peer_rn_cap,
                                           ESP_AVRC_RN_TRACK_CHANGE)) {
        esp_avrc_ct_send_register_notification_cmd(bt_avrc_common_alloc_tl(), ESP_AVRC_RN_TRACK_CHANGE, 0);
    }
}

void bt_avrc_common_ct_rn_play_status_changed(void)
{
    /* register notification if peer support the event_id */
    if (esp_avrc_rn_evt_bit_mask_operation(ESP_AVRC_BIT_MASK_OP_TEST, &s_avrc_common_cb.peer_rn_cap,
                                           ESP_AVRC_RN_PLAY_STATUS_CHANGE)) {
        esp_avrc_ct_send_register_notification_cmd(bt_avrc_common_alloc_tl(), ESP_AVRC_RN_PLAY_STATUS_CHANGE, 0);
    }
}

void bt_avrc_common_ct_rn_play_pos_changed(void)
{
    /* register notification if peer support the event_id */
    if (esp_avrc_rn_evt_bit_mask_operation(ESP_AVRC_BIT_MASK_OP_TEST, &s_avrc_common_cb.peer_rn_cap,
                                           ESP_AVRC_RN_PLAY_POS_CHANGED)) {
        esp_avrc_ct_send_register_notification_cmd(bt_avrc_common_alloc_tl(), ESP_AVRC_RN_PLAY_POS_CHANGED, 10);
    }
}

void bt_avrc_common_ct_notify_evt_handler(uint8_t event_id, esp_avrc_rn_param_t *event_parameter)
{
    switch (event_id) {
    /* when new track is loaded, this event comes */
    case ESP_AVRC_RN_TRACK_CHANGE:
        ESP_LOGI(BT_RC_CT_TAG, "New track loaded");
        bt_avrc_common_ct_rn_track_changed();
        break;
    /* when track status changed, this event comes */
    case ESP_AVRC_RN_PLAY_STATUS_CHANGE:
        ESP_LOGI(BT_RC_CT_TAG, "Playback status changed: 0x%x", event_parameter->playback);
        bt_avrc_common_ct_rn_play_status_changed();
        break;
    /* when track playing position changed, this event comes */
    case ESP_AVRC_RN_PLAY_POS_CHANGED:
        ESP_LOGI(BT_RC_CT_TAG, "Play position changed: %"PRIu32"-ms", event_parameter->play_pos);
        bt_avrc_common_ct_rn_play_pos_changed();
        break;
    /* others */
    default:
        ESP_LOGI(BT_RC_CT_TAG, "unhandled event: %d", event_id);
        break;
    }
}

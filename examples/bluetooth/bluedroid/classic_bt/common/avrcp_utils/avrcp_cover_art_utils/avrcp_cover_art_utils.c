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
#include "esp_avrc_api.h"
#include "avrcp_utils_tags.h"
#include "avrcp_cover_art_service.h"

void bt_avrc_ca_ct_evt_hdl(uint16_t event, void *param)
{
    ESP_LOGD(BT_RC_CT_TAG, "%s event: %d", __func__, event);

    esp_avrc_ct_cb_param_t *rc = (esp_avrc_ct_cb_param_t *)(param);

    switch (event) {
    /* when connection state changed, this event comes */
    case ESP_AVRC_CT_CONNECTION_STATE_EVT: {
        if (rc->conn_stat.connected) {
            /* open the cover art service */
            avrc_cover_art_srv_open();
        } else {
            /* close the cover art service */
            avrc_cover_art_srv_close();
        }
        break;
    }
    /* when metadata response, this event comes */
    case ESP_AVRC_CT_METADATA_RSP_EVT: {
        ESP_LOGI(BT_RC_CT_TAG, "AVRC metadata rsp: attribute id 0x%x, %s", rc->meta_rsp.attr_id, rc->meta_rsp.attr_text);
        if (rc->meta_rsp.attr_id == ESP_AVRC_MD_ATTR_COVER_ART) {
            avrc_cover_art_srv_ct_metadata_update(rc->meta_rsp.attr_text, rc->meta_rsp.attr_length);
        }
        break;
    }
    /* when notified, this event comes */
    case ESP_AVRC_CT_CHANGE_NOTIFY_EVT: {
        if (rc->change_ntf.event_id == ESP_AVRC_RN_TRACK_CHANGE) {
            /* request the cover art */
            avrc_cover_art_srv_ca_req();
        }
        break;
    }
    /* when feature of remote device indicated, this event comes */
    case ESP_AVRC_CT_REMOTE_FEATURES_EVT: {
        ESP_LOGI(BT_RC_CT_TAG, "AVRC remote features %"PRIx32", TG features %x", rc->rmt_feats.feat_mask, rc->rmt_feats.tg_feat_flag);
        if ((rc->rmt_feats.tg_feat_flag & ESP_AVRC_FEAT_FLAG_TG_COVER_ART)) {
            ESP_LOGW(BT_RC_CT_TAG, "Peer support Cover Art feature");
            /* start the cover art connection */
            avrc_cover_art_srv_connect(ESP_AVRC_CA_MTU_MAX);
        }
        break;
    }
    /* when notification capability of peer device got, this event comes */
    case ESP_AVRC_CT_GET_RN_CAPABILITIES_RSP_EVT: {
        /* request the cover art */
        avrc_cover_art_srv_ca_req();
        break;
    }
    /* when the state of cover art changes, this event comes */
    case ESP_AVRC_CT_COVER_ART_STATE_EVT: {
        if (rc->cover_art_state.state == ESP_AVRC_COVER_ART_CONNECTED) {
            avrc_cover_art_srv_set_connected(true);
            ESP_LOGW(BT_RC_CT_TAG, "Cover Art Client connected");
            /* request the cover art */
            avrc_cover_art_srv_ca_req();
        } else {
            avrc_cover_art_srv_set_connected(false);
            ESP_LOGW(BT_RC_CT_TAG, "Cover Art Client disconnected, reason:%d", rc->cover_art_state.reason);
        }
        break;
    }
    /* when obtaining the cover art image data, this event comes */
    case ESP_AVRC_CT_COVER_ART_DATA_EVT: {
        /* when rc->cover_art_data.final is true, it means we have received the entire image */
        avrc_cover_art_srv_set_image_final(rc->cover_art_data.final);
        break;
    }
    /* others */
    default:
        ESP_LOGE(BT_RC_CT_TAG, "%s unhandled event: %d", __func__, event);
        break;
    }
}

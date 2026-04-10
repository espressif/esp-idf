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
#include "esp_gap_bt_api.h"
#include "esp_a2dp_api.h"

#include "a2dp_sink_ext_codec_utils.h"
#include "a2dp_utils_tags.h"

/*******************************
 * STATIC VARIABLE DEFINITIONS
 ******************************/

/* count for audio packet */
static uint32_t s_pkt_cnt = 0;
/* connection state in string */
static const char *s_a2d_conn_state_str[] = {"Disconnected", "Connecting", "Connected", "Disconnecting"};
/* audio stream datapath state in string */
static const char *s_a2d_audio_state_str[] = {"Suspended", "Started"};

/********************************
 * EXTERNAL FUNCTION DEFINITIONS
 *******************************/

void bt_a2d_evt_ext_codec_hdl(uint16_t event, void *param)
{
    ESP_LOGD(BT_AV_TAG, "%s event: %d", __func__, event);

    esp_a2d_cb_param_t *a2d = (esp_a2d_cb_param_t *)(param);

    switch (event) {
    /* when connection state changed, this event comes */
    case ESP_A2D_CONNECTION_STATE_EVT: {
        uint8_t *bda = a2d->conn_stat.remote_bda;
        ESP_LOGI(BT_AV_TAG, "A2DP connection state: %s, [%02x:%02x:%02x:%02x:%02x:%02x]",
                 s_a2d_conn_state_str[a2d->conn_stat.state], bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
        if (a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
            esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        } else if (a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
            esp_bt_gap_set_scan_mode(ESP_BT_NON_CONNECTABLE, ESP_BT_NON_DISCOVERABLE);
        }
        break;
    }
    /* when audio stream transmission state changed, this event comes */
    case ESP_A2D_AUDIO_STATE_EVT: {
        ESP_LOGI(BT_AV_TAG, "A2DP audio state: %s", s_a2d_audio_state_str[a2d->audio_stat.state]);
        if (ESP_A2D_AUDIO_STATE_STARTED == a2d->audio_stat.state) {
            s_pkt_cnt = 0;
        }
        break;
    }
    /* when audio codec is configured, this event comes */
    case ESP_A2D_AUDIO_CFG_EVT: {
        esp_a2d_mcc_t *p_mcc = &a2d->audio_cfg.mcc;
        ESP_LOGI(BT_AV_TAG, "A2DP audio stream configuration, codec type: %d", p_mcc->type);
        /* for now only SBC stream is supported */
        if (p_mcc->type == ESP_A2D_MCT_SBC) {
            int sample_rate = 16000;
            if (p_mcc->cie.sbc_info.samp_freq & ESP_A2D_SBC_CIE_SF_32K) {
                sample_rate = 32000;
            } else if (p_mcc->cie.sbc_info.samp_freq & ESP_A2D_SBC_CIE_SF_44K) {
                sample_rate = 44100;
            } else if (p_mcc->cie.sbc_info.samp_freq & ESP_A2D_SBC_CIE_SF_48K) {
                sample_rate = 48000;
            }
            ESP_LOGI(BT_AV_TAG, "Configure audio player: 0x%x-0x%x-0x%x-0x%x-0x%x-%d-%d",
                     p_mcc->cie.sbc_info.samp_freq,
                     p_mcc->cie.sbc_info.ch_mode,
                     p_mcc->cie.sbc_info.block_len,
                     p_mcc->cie.sbc_info.num_subbands,
                     p_mcc->cie.sbc_info.alloc_mthd,
                     p_mcc->cie.sbc_info.min_bitpool,
                     p_mcc->cie.sbc_info.max_bitpool);
            ESP_LOGI(BT_AV_TAG, "Audio player configured, sample rate: %d", sample_rate);
        }
        break;
    }
    /* when using external codec, after sep registration done, this event comes */
    case ESP_A2D_SEP_REG_STATE_EVT: {
        if (a2d->a2d_sep_reg_stat.reg_state == ESP_A2D_SEP_REG_SUCCESS) {
            ESP_LOGI(BT_AV_TAG, "A2DP register SEP success, seid: %d", a2d->a2d_sep_reg_stat.seid);
        } else {
            ESP_LOGI(BT_AV_TAG, "A2DP register SEP fail, seid: %d, state: %d", a2d->a2d_sep_reg_stat.seid, a2d->a2d_sep_reg_stat.reg_state);
        }
        break;
    }
    /* others */
    default:
        ESP_LOGE(BT_AV_TAG, "%s unhandled event: %d", __func__, event);
        break;
    }
}

void bt_a2d_audio_data_hdl(esp_a2d_conn_hdl_t conn_hdl, esp_a2d_audio_buff_t *audio_buf)
{
    /* log the number every 100 packets */
    if (++s_pkt_cnt % 100 == 0) {
        ESP_LOGI(BT_AV_TAG, "Undecoded audio package count: %"PRIu32, s_pkt_cnt);
    }

    /*
     * Normally, user should send the audio_buf to other task, decode and free audio buff,
     * But the codec component is not merge into IDF now, so we just free audio data here
     */
    esp_a2d_audio_buff_free(audio_buf);
}

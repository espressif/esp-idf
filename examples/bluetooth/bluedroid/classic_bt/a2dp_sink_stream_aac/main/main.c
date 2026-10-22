/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "esp_a2dp_api.h"
#include "esp_audio_dec.h"
#include "esp_aac_dec.h"
#include "esp_sbc_dec.h"
#include "esp_audio_dec_reg.h"
#include "bt_app_core_utils.h"
#include "bredr_app_common_utils.h"
#include "a2dp_sink_common_utils.h"
#include "a2dp_utils_tags.h"
#include "a2dp_sink_int_codec_utils.h"
#include "a2dp_sink_ext_codec_utils.h"
#include "audio_sink_service.h"

#define BT_A2DP_RAW_Q_DEPTH           24
#define BT_A2DP_DECODE_TASK_STACK     4096
#ifndef BT_A2DP_DECODE_TASK_PRIO
#define BT_A2DP_DECODE_TASK_PRIO      (tskIDLE_PRIORITY + 5)
#endif

#ifdef CONFIG_BT_A2DP_SINK_MAX_CONN
#define BT_APP_A2D_MAX_PEERS          CONFIG_BT_A2DP_SINK_MAX_CONN
#else
#define BT_APP_A2D_MAX_PEERS          1
#endif

typedef struct {
    esp_a2d_conn_hdl_t conn_hdl;
    esp_bd_addr_t bda;
    esp_a2d_mcc_t mcc;
    bool mcc_valid;
} bt_app_a2d_peer_t;

static QueueHandle_t s_a2dp_raw_q;
static TaskHandle_t s_a2dp_dec_task;
static SemaphoreHandle_t s_a2dp_raw_mux;
static bool s_a2dp_raw_started;
static uint32_t s_a2dp_raw_pkt_cnt;
static esp_audio_dec_handle_t s_dec_hdl;
static bool s_dec_opened;

/* Connected sources and the one currently being rendered. The stack drops
 * media of every other connection; this example keeps one decoder/I2S path. */
static bt_app_a2d_peer_t s_a2d_peers[BT_APP_A2D_MAX_PEERS];
static int s_a2d_peer_num;
static esp_a2d_conn_hdl_t s_a2d_active_hdl;

/* device name */
static const char local_device_name[] = CONFIG_EXAMPLE_LOCAL_DEVICE_NAME;

/* event for stack up */
enum {
    BT_APP_EVT_STACK_UP = 0,
};

/********************************
 * STATIC FUNCTION DECLARATIONS
 *******************************/

/* Device callback function */
static void bt_app_dev_cb(esp_bt_dev_cb_event_t event, esp_bt_dev_cb_param_t *param);

/* GAP callback function */
static void bt_app_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);

/* callback function for A2DP sink */
static void bt_app_a2d_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param);

/* callback function for A2DP sink undecoded audio data */
static void bt_app_a2d_audio_data_cb(esp_a2d_conn_hdl_t conn_hdl, esp_a2d_audio_buff_t *audio_buf);

/* handler for bluetooth stack enabled events */
static void bt_av_hdl_stack_evt(uint16_t event, void *p_param);

/*******************************
 * STATIC FUNCTION DEFINITIONS
 ******************************/

static int bt_app_mcc_get_sample_rate(const esp_a2d_mcc_t *mcc)
{
    if (mcc->type == ESP_A2D_MCT_SBC) {
        const esp_a2d_cie_sbc_t *sbc = &mcc->cie.sbc_info;
        if (sbc->samp_freq & ESP_A2D_SBC_CIE_SF_48K) {
            return 48000;
        }
        if (sbc->samp_freq & ESP_A2D_SBC_CIE_SF_44K) {
            return 44100;
        }
        if (sbc->samp_freq & ESP_A2D_SBC_CIE_SF_32K) {
            return 32000;
        }
        if (sbc->samp_freq & ESP_A2D_SBC_CIE_SF_16K) {
            return 16000;
        }
        return 44100;
    }
    if (mcc->type == ESP_A2D_MCT_M24) {
        const esp_a2d_cie_m24_t *m24 = &mcc->cie.m24_info;
        if (m24->samp_freq2 & ESP_A2D_M24_CIE_SF2_96K) {
            return 96000;
        }
        if (m24->samp_freq2 & ESP_A2D_M24_CIE_SF2_88K) {
            return 88200;
        }
        if (m24->samp_freq2 & ESP_A2D_M24_CIE_SF2_64K) {
            return 64000;
        }
        if (m24->samp_freq2 & ESP_A2D_M24_CIE_SF2_48K) {
            return 48000;
        }
        if (m24->samp_freq1 & ESP_A2D_M24_CIE_SF1_44K) {
            return 44100;
        }
        if (m24->samp_freq1 & ESP_A2D_M24_CIE_SF1_32K) {
            return 32000;
        }
        if (m24->samp_freq1 & ESP_A2D_M24_CIE_SF1_24K) {
            return 24000;
        }
        if (m24->samp_freq1 & ESP_A2D_M24_CIE_SF1_22K) {
            return 22050;
        }
        if (m24->samp_freq1 & ESP_A2D_M24_CIE_SF1_16K) {
            return 16000;
        }
        if (m24->samp_freq1 & ESP_A2D_M24_CIE_SF1_12K) {
            return 12000;
        }
        if (m24->samp_freq1 & ESP_A2D_M24_CIE_SF1_11K) {
            return 11025;
        }
        if (m24->samp_freq1 & ESP_A2D_M24_CIE_SF1_8K) {
            return 8000;
        }
        return 44100;
    }
    return 44100;
}

static int bt_app_mcc_get_channel_count(const esp_a2d_mcc_t *mcc)
{
    if (mcc->type == ESP_A2D_MCT_SBC) {
        if (mcc->cie.sbc_info.ch_mode & ESP_A2D_SBC_CIE_CH_MODE_MONO) {
            return 1;
        }
        return 2;
    }
    if (mcc->type == ESP_A2D_MCT_M24) {
        if (mcc->cie.m24_info.ch & ESP_A2D_M24_CIE_CH_1) {
            return 1;
        }
        return 2;
    }
    return 2;
}

static void bt_app_audio_decoder_close(void)
{
    if (!s_dec_opened) {
        return;
    }
    esp_audio_dec_close(s_dec_hdl);
    s_dec_hdl = NULL;
    s_dec_opened = false;
}

static esp_audio_err_t bt_app_audio_decoder_apply_mcc(const esp_a2d_mcc_t *mcc)
{
    esp_audio_err_t ret;

    if (mcc == NULL) {
        return ESP_AUDIO_ERR_INVALID_PARAMETER;
    }
    if (mcc->type != ESP_A2D_MCT_SBC && mcc->type != ESP_A2D_MCT_M24) {
        ESP_LOGW(BT_AV_TAG, "Unsupported A2DP codec type: %d", mcc->type);
        return ESP_AUDIO_ERR_FAIL;
    }

    bt_app_audio_decoder_close();

    if (mcc->type == ESP_A2D_MCT_SBC) {
        esp_sbc_dec_cfg_t sbc_cfg = ESP_SBC_DEC_CONFIG_DEFAULT();
        sbc_cfg.sbc_mode = ESP_SBC_MODE_STD;
        sbc_cfg.ch_num = bt_app_mcc_get_channel_count(mcc);
        sbc_cfg.enable_plc = true;

        esp_audio_dec_cfg_t dec_cfg = {
            .type = ESP_AUDIO_TYPE_SBC,
            .cfg = &sbc_cfg,
            .cfg_sz = sizeof(esp_sbc_dec_cfg_t),
        };

        esp_sbc_dec_register();
        ret = esp_audio_dec_open(&dec_cfg, &s_dec_hdl);
        if (ret != ESP_AUDIO_ERR_OK) {
            ESP_LOGE(BT_AV_TAG, "esp_audio_dec_open (SBC) failed: %d", ret);
            return ret;
        }
        ESP_LOGI(BT_AV_TAG, "SBC decoder open: %" PRIu32 " Hz, %d ch",
                 (uint32_t)bt_app_mcc_get_sample_rate(mcc), sbc_cfg.ch_num);
    } else {
        const esp_a2d_cie_m24_t *m24 = &mcc->cie.m24_info;
        esp_aac_dec_cfg_t aac_cfg = ESP_AAC_DEC_CONFIG_DEFAULT();
        aac_cfg.sample_rate = bt_app_mcc_get_sample_rate(mcc);
        aac_cfg.channel = bt_app_mcc_get_channel_count(mcc);
        aac_cfg.bits_per_sample = ESP_AUDIO_BIT16;
        aac_cfg.no_adts_header = true;
        aac_cfg.aac_plus_enable = (m24->obj_type & (ESP_A2D_M24_CIE_OBJ_TYPE_4_HE_AAC |
                                                    ESP_A2D_M24_CIE_OBJ_TYPE_4_HE_AAC_V2)) != 0;

        esp_audio_dec_cfg_t dec_cfg = {
            .type = ESP_AUDIO_TYPE_AAC,
            .cfg = &aac_cfg,
            .cfg_sz = sizeof(esp_aac_dec_cfg_t),
        };

        esp_aac_dec_register();
        ret = esp_audio_dec_open(&dec_cfg, &s_dec_hdl);
        if (ret != ESP_AUDIO_ERR_OK) {
            ESP_LOGE(BT_AV_TAG, "esp_audio_dec_open (AAC) failed: %d", ret);
            return ret;
        }
        ESP_LOGI(BT_AV_TAG, "AAC decoder open: %" PRIu32 " Hz, %d ch, aac_plus=%d",
                 (uint32_t)aac_cfg.sample_rate, aac_cfg.channel, aac_cfg.aac_plus_enable);
    }

    s_dec_opened = true;
    return ESP_AUDIO_ERR_OK;
}

static int bt_app_a2d_peer_find_by_bda(const esp_bd_addr_t bda)
{
    for (int i = 0; i < s_a2d_peer_num; i++) {
        if (memcmp(s_a2d_peers[i].bda, bda, sizeof(esp_bd_addr_t)) == 0) {
            return i;
        }
    }
    return -1;
}

static int bt_app_a2d_peer_find_by_hdl(esp_a2d_conn_hdl_t conn_hdl)
{
    if (conn_hdl == 0) {
        return -1;
    }
    for (int i = 0; i < s_a2d_peer_num; i++) {
        if (s_a2d_peers[i].conn_hdl == conn_hdl) {
            return i;
        }
    }
    return -1;
}

static bt_app_a2d_peer_t *bt_app_a2d_peer_get_or_add(esp_a2d_conn_hdl_t conn_hdl, const esp_bd_addr_t bda)
{
    int idx = bt_app_a2d_peer_find_by_hdl(conn_hdl);

    if (idx < 0) {
        idx = bt_app_a2d_peer_find_by_bda(bda);
    }
    if (idx < 0) {
        if (s_a2d_peer_num >= BT_APP_A2D_MAX_PEERS) {
            return NULL;
        }
        idx = s_a2d_peer_num++;
        memset(&s_a2d_peers[idx], 0, sizeof(bt_app_a2d_peer_t));
    }
    s_a2d_peers[idx].conn_hdl = conn_hdl;
    memcpy(s_a2d_peers[idx].bda, bda, sizeof(esp_bd_addr_t));
    return &s_a2d_peers[idx];
}

static void bt_app_a2d_peer_remove(const esp_bd_addr_t bda)
{
    int idx = bt_app_a2d_peer_find_by_bda(bda);

    if (idx < 0) {
        return;
    }
    if (s_a2d_peers[idx].conn_hdl == s_a2d_active_hdl) {
        s_a2d_active_hdl = 0;
    }
    s_a2d_peer_num--;
    if (idx != s_a2d_peer_num) {
        s_a2d_peers[idx] = s_a2d_peers[s_a2d_peer_num];
    }
    memset(&s_a2d_peers[s_a2d_peer_num], 0, sizeof(bt_app_a2d_peer_t));
}

static void bt_app_a2d_raw_audio_task(void *arg)
{
    esp_a2d_audio_buff_t *audio_buf;
    uint32_t max_out_size = 4096;
    uint8_t *out_buf = (uint8_t *)malloc((size_t)max_out_size);
    if (out_buf == NULL) {
        ESP_LOGE(BT_AV_TAG, "raw dec: out buffer alloc failed");
        vTaskDelete(NULL);
        return;
    }

    for (;;) {
        xQueueReceive(s_a2dp_raw_q, &audio_buf, portMAX_DELAY);

        if (!s_dec_opened) {
            esp_a2d_audio_buff_free(audio_buf);
            continue;
        }

        esp_audio_dec_in_raw_t in_raw = {
            .buffer = audio_buf->data,
            .len = audio_buf->data_len,
        };
        esp_audio_dec_out_frame_t out_frame = {
            .buffer = out_buf,
            .len = max_out_size,
        };

        while (in_raw.len > 0) {
            esp_audio_err_t dec_ret = esp_audio_dec_process(s_dec_hdl, &in_raw, &out_frame);
            if (dec_ret != ESP_AUDIO_ERR_OK && dec_ret != ESP_AUDIO_ERR_BUFF_NOT_ENOUGH) {
                ESP_LOGW(BT_AV_TAG, "esp_audio_dec_process failed: %d", dec_ret);
                break;
            }

            if (dec_ret == ESP_AUDIO_ERR_BUFF_NOT_ENOUGH) {
                uint8_t *new_dec_buff = (uint8_t *)realloc(out_frame.buffer, (size_t)max_out_size * 2u);
                if (new_dec_buff == NULL) {
                    break;
                }
                max_out_size *= 2;
                out_buf = new_dec_buff;
                out_frame.buffer = new_dec_buff;
                out_frame.len = max_out_size;
                continue;
            }

            audio_sink_srv_data_output(out_frame.buffer, out_frame.decoded_size);
            in_raw.buffer += in_raw.consumed;
            in_raw.len -= in_raw.consumed;
        }

        esp_a2d_audio_buff_free(audio_buf);
    }
}

static void bt_a2d_sink_ext_codec_postproc_start(void)
{
    if (s_a2dp_raw_mux == NULL) {
        s_a2dp_raw_mux = xSemaphoreCreateMutex();
        if (s_a2dp_raw_mux == NULL) {
            return;
        }
    }
    if (xSemaphoreTake(s_a2dp_raw_mux, portMAX_DELAY) != pdTRUE) {
        return;
    }
    if (s_a2dp_raw_started) {
        xSemaphoreGive(s_a2dp_raw_mux);
        return;
    }

    s_a2dp_raw_q = xQueueCreate(BT_A2DP_RAW_Q_DEPTH, sizeof(esp_a2d_audio_buff_t *));
    if (s_a2dp_raw_q == NULL) {
        xSemaphoreGive(s_a2dp_raw_mux);
        ESP_LOGE(BT_AV_TAG, "A2DP raw queue create failed");
        return;
    }
    if (xTaskCreate(bt_app_a2d_raw_audio_task, "a2dp_raw_dec", BT_A2DP_DECODE_TASK_STACK,
                    NULL, BT_A2DP_DECODE_TASK_PRIO, &s_a2dp_dec_task) != pdPASS) {
        vQueueDelete(s_a2dp_raw_q);
        s_a2dp_raw_q = NULL;
        s_a2dp_dec_task = NULL;
        xSemaphoreGive(s_a2dp_raw_mux);
        ESP_LOGE(BT_AV_TAG, "A2DP raw decoder task create failed");
        return;
    }
    s_a2dp_raw_started = true;
    xSemaphoreGive(s_a2dp_raw_mux);
    ESP_LOGI(BT_AV_TAG, "A2DP raw postproc (queue %d) started", BT_A2DP_RAW_Q_DEPTH);
}

/* Caller must hold s_a2dp_raw_mux. Leaves the decode task suspended. */
static void bt_a2d_sink_ext_codec_flush_queue_locked(void)
{
    esp_a2d_audio_buff_t *audio_buf;

    if (s_a2dp_dec_task != NULL) {
        vTaskSuspend(s_a2dp_dec_task);
    }
    if (s_a2dp_raw_q != NULL) {
        while (xQueueReceive(s_a2dp_raw_q, &audio_buf, 0) == pdTRUE) {
            esp_a2d_audio_buff_free(audio_buf);
        }
    }
}

static void bt_a2d_sink_ext_codec_postproc_flush_ex(bool close_decoder)
{
    if (s_a2dp_raw_mux == NULL) {
        if (close_decoder) {
            bt_app_audio_decoder_close();
        }
        return;
    }
    if (xSemaphoreTake(s_a2dp_raw_mux, portMAX_DELAY) != pdTRUE) {
        return;
    }
    bt_a2d_sink_ext_codec_flush_queue_locked();
    if (close_decoder) {
        bt_app_audio_decoder_close();
    }
    if (s_a2dp_dec_task != NULL) {
        vTaskResume(s_a2dp_dec_task);
    }
    xSemaphoreGive(s_a2dp_raw_mux);
}

static void bt_a2d_sink_ext_codec_postproc_flush(void)
{
    bt_a2d_sink_ext_codec_postproc_flush_ex(true);
}

/* Rebuild the shared decoder/I2S for the stream being rendered. The decode
 * task stays suspended across flush and decoder replace so it cannot consume
 * old frames with the new decoder (or the reverse).
 * codec_info_update() stops I2S; pass restart_i2s=true when the output should
 * keep running (active switch / reconfig). Leave it false on the first
 * AUDIO_CFG so CONNECTED can start I2S as usual. */
static void bt_app_render_apply(const esp_a2d_mcc_t *mcc, bool restart_i2s)
{
    if (mcc == NULL) {
        return;
    }
    if (s_a2dp_raw_mux != NULL && xSemaphoreTake(s_a2dp_raw_mux, portMAX_DELAY) == pdTRUE) {
        bt_a2d_sink_ext_codec_flush_queue_locked();
        bt_app_audio_decoder_apply_mcc(mcc);
        audio_sink_srv_codec_info_update((esp_a2d_mcc_t *)mcc);
        if (restart_i2s) {
            audio_sink_srv_start();
        }
        if (s_a2dp_dec_task != NULL) {
            vTaskResume(s_a2dp_dec_task);
        }
        xSemaphoreGive(s_a2dp_raw_mux);
    } else {
        bt_app_audio_decoder_apply_mcc(mcc);
        audio_sink_srv_codec_info_update((esp_a2d_mcc_t *)mcc);
        if (restart_i2s) {
            audio_sink_srv_start();
        }
    }
}

static void bt_app_register_a2dp_sink_seps(void)
{
    esp_a2d_mcc_t mcc_aac = {0};
    mcc_aac.type = ESP_A2D_MCT_M24;
    mcc_aac.cie.m24_info.drc = ESP_A2D_M24_CIE_DRC_NS;
    mcc_aac.cie.m24_info.obj_type = ESP_A2D_M24_CIE_OBJ_TYPE_2_AAC_LC |
                                    ESP_A2D_M24_CIE_OBJ_TYPE_4_AAC_LC |
                                    ESP_A2D_M24_CIE_OBJ_TYPE_4_HE_AAC |
                                    ESP_A2D_M24_CIE_OBJ_TYPE_4_HE_AAC_V2;
    mcc_aac.cie.m24_info.samp_freq1 = ESP_A2D_M24_CIE_SF1_8K |
                                      ESP_A2D_M24_CIE_SF1_11K |
                                      ESP_A2D_M24_CIE_SF1_12K |
                                      ESP_A2D_M24_CIE_SF1_16K |
                                      ESP_A2D_M24_CIE_SF1_22K |
                                      ESP_A2D_M24_CIE_SF1_24K |
                                      ESP_A2D_M24_CIE_SF1_32K |
                                      ESP_A2D_M24_CIE_SF1_44K;
    mcc_aac.cie.m24_info.samp_freq2 = ESP_A2D_M24_CIE_SF2_48K |
                                      ESP_A2D_M24_CIE_SF2_64K |
                                      ESP_A2D_M24_CIE_SF2_88K |
                                      ESP_A2D_M24_CIE_SF2_96K;
    mcc_aac.cie.m24_info.ch = ESP_A2D_M24_CIE_CH_1 |
                              ESP_A2D_M24_CIE_CH_2;
    mcc_aac.cie.m24_info.vbr = ESP_A2D_M24_CIE_VBR_SUPPORT;
    mcc_aac.cie.m24_info.br1 = 0x7F & ESP_A2D_M24_CIE_BR1_MSK;
    mcc_aac.cie.m24_info.br2 = 0xFF & ESP_A2D_M24_CIE_BR2_MSK;
    mcc_aac.cie.m24_info.br3 = 0xFF & ESP_A2D_M24_CIE_BR3_MSK;
    ESP_ERROR_CHECK(esp_a2d_sink_register_stream_endpoint(0, &mcc_aac));

    esp_a2d_mcc_t mcc_sbc = {0};
    mcc_sbc.type = ESP_A2D_MCT_SBC;
    mcc_sbc.cie.sbc_info.samp_freq = ESP_A2D_SBC_CIE_SF_16K |
                                     ESP_A2D_SBC_CIE_SF_32K |
                                     ESP_A2D_SBC_CIE_SF_44K |
                                     ESP_A2D_SBC_CIE_SF_48K;
    mcc_sbc.cie.sbc_info.ch_mode = ESP_A2D_SBC_CIE_CH_MODE_MONO |
                                   ESP_A2D_SBC_CIE_CH_MODE_DUAL_CHANNEL |
                                   ESP_A2D_SBC_CIE_CH_MODE_STEREO |
                                   ESP_A2D_SBC_CIE_CH_MODE_JOINT_STEREO;
    mcc_sbc.cie.sbc_info.block_len = ESP_A2D_SBC_CIE_BLOCK_LEN_4 |
                                     ESP_A2D_SBC_CIE_BLOCK_LEN_8 |
                                     ESP_A2D_SBC_CIE_BLOCK_LEN_12 |
                                     ESP_A2D_SBC_CIE_BLOCK_LEN_16;
    mcc_sbc.cie.sbc_info.num_subbands = ESP_A2D_SBC_CIE_NUM_SUBBANDS_4 | ESP_A2D_SBC_CIE_NUM_SUBBANDS_8;
    mcc_sbc.cie.sbc_info.alloc_mthd = ESP_A2D_SBC_CIE_ALLOC_MTHD_SNR | ESP_A2D_SBC_CIE_ALLOC_MTHD_LOUDNESS;
    mcc_sbc.cie.sbc_info.min_bitpool = 2;
    mcc_sbc.cie.sbc_info.max_bitpool = 250;
    ESP_ERROR_CHECK(esp_a2d_sink_register_stream_endpoint(1, &mcc_sbc));
}

static void bt_app_dev_cb(esp_bt_dev_cb_event_t event, esp_bt_dev_cb_param_t *param)
{
    bredr_app_dev_evt_def_hdl(event, param);
}

static void bt_app_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    bredr_app_gap_evt_def_hdl(event, param);
}

static void bt_app_a2d_restore_discoverable_if_slot_free(void)
{
    if (s_a2d_peer_num < BT_APP_A2D_MAX_PEERS) {
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
    }
}

static void bt_app_a2d_hdl(uint16_t event, void *param)
{
    esp_a2d_cb_param_t *a2d = (esp_a2d_cb_param_t *)param;

    switch (event) {
    case ESP_A2D_PROF_STATE_EVT: {
        if (ESP_A2D_INIT_SUCCESS == a2d->a2d_prof_stat.init_state) {
            ESP_LOGI(BT_AV_TAG, "A2DP PROF STATE: Init Complete");
            bt_app_register_a2dp_sink_seps();

            /* Get the default value of the delay value */
            esp_a2d_sink_get_delay_value();
        } else {
            ESP_LOGI(BT_AV_TAG, "A2DP PROF STATE: Deinit Complete");
        }
        break;
    }
    case ESP_A2D_SNK_PSC_CFG_EVT:
    case ESP_A2D_SNK_SET_DELAY_VALUE_EVT:
    case ESP_A2D_SNK_GET_DELAY_VALUE_EVT: {
        bt_a2d_evt_def_hdl(event, param);
        break;
    }
    case ESP_A2D_SNK_ACTIVE_DEVICE_EVT: {
        uint8_t *bda = a2d->a2d_snk_active_device.remote_bda;

        if (a2d->a2d_snk_active_device.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGW(BT_AV_TAG, "A2DP set active failed, status %d, [%02x:%02x:%02x:%02x:%02x:%02x]",
                     a2d->a2d_snk_active_device.status,
                     bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
            break;
        }
        if (a2d->a2d_snk_active_device.active) {
            int idx;

            s_a2d_active_hdl = a2d->a2d_snk_active_device.conn_hdl;
            ESP_LOGI(BT_AV_TAG, "A2DP rendering [%02x:%02x:%02x:%02x:%02x:%02x], other sources suspended",
                     bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
            idx = bt_app_a2d_peer_find_by_hdl(s_a2d_active_hdl);
            if (idx < 0) {
                idx = bt_app_a2d_peer_find_by_bda(bda);
            }
            if (idx >= 0 && s_a2d_peers[idx].mcc_valid) {
                bt_app_render_apply(&s_a2d_peers[idx].mcc, true);
            }
        } else {
            ESP_LOGI(BT_AV_TAG, "A2DP no source is being rendered");
            s_a2d_active_hdl = 0;
            /* Another source may still be connected. Do not close the decoder
             * so a later AUDIO_CFG from that peer cannot steal I2S; SNK_ACTIVE
             * will apply the right MCC when playback resumes. */
            bt_a2d_sink_ext_codec_postproc_flush_ex(s_a2d_peer_num == 0);
        }
        break;
    }
    case ESP_A2D_CONNECTION_STATE_EVT: {
        if (a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
            bt_app_a2d_peer_get_or_add(a2d->conn_stat.conn_hdl, a2d->conn_stat.remote_bda);
            bt_a2d_evt_int_codec_hdl(event, param);
            bt_app_a2d_restore_discoverable_if_slot_free();
        } else if (a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
            int idx = bt_app_a2d_peer_find_by_bda(a2d->conn_stat.remote_bda);
            bool was_active = (a2d->conn_stat.conn_hdl != 0 &&
                               a2d->conn_stat.conn_hdl == s_a2d_active_hdl);
            uint8_t *bda = a2d->conn_stat.remote_bda;

            if (!was_active && idx >= 0 && s_a2d_peers[idx].conn_hdl == s_a2d_active_hdl) {
                was_active = true;
            }
            bt_app_a2d_peer_remove(a2d->conn_stat.remote_bda);
            if (s_a2d_peer_num > 0) {
                /* another source is still connected, leave its audio path alone */
                if (was_active) {
                    bt_a2d_sink_ext_codec_postproc_flush_ex(true);
                }
                ESP_LOGI(BT_AV_TAG, "A2DP connection state: Disconnected, [%02x:%02x:%02x:%02x:%02x:%02x]",
                         bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
                esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
                break;
            }
            bt_a2d_sink_ext_codec_postproc_flush();
            bt_a2d_evt_int_codec_hdl(event, param);
        } else {
            bt_a2d_evt_int_codec_hdl(event, param);
        }
        break;
    }
    case ESP_A2D_AUDIO_STATE_EVT: {
        if (a2d->audio_stat.state == ESP_A2D_AUDIO_STATE_STARTED) {
            bt_a2d_sink_ext_codec_postproc_start();
            /* Pause/resume does not send AUDIO_CFG again; reopen the decoder
             * if a previous flush closed it. */
            if (!s_dec_opened) {
                int idx = bt_app_a2d_peer_find_by_hdl(a2d->audio_stat.conn_hdl);
                if (idx < 0) {
                    idx = bt_app_a2d_peer_find_by_bda(a2d->audio_stat.remote_bda);
                }
                if (idx >= 0 && s_a2d_peers[idx].mcc_valid &&
                    (s_a2d_active_hdl == 0 || s_a2d_peers[idx].conn_hdl == s_a2d_active_hdl)) {
                    bt_app_render_apply(&s_a2d_peers[idx].mcc, true);
                }
            }
        } else if (a2d->audio_stat.state == ESP_A2D_AUDIO_STATE_SUSPEND) {
            /* A displaced peer's suspend must not wipe the stream now playing. */
            if (s_a2d_active_hdl == 0 || a2d->audio_stat.conn_hdl == s_a2d_active_hdl) {
                bt_a2d_sink_ext_codec_postproc_flush_ex(false);
            }
        }
        bt_a2d_evt_int_codec_hdl(event, param);
        break;
    }
    case ESP_A2D_AUDIO_CFG_EVT: {
        bt_app_a2d_peer_t *peer = bt_app_a2d_peer_get_or_add(a2d->audio_cfg.conn_hdl,
                                                            a2d->audio_cfg.remote_bda);
        bool apply = (a2d->audio_cfg.conn_hdl == s_a2d_active_hdl) ||
                     (s_a2d_active_hdl == 0 && s_a2d_peer_num <= 1);

        if (peer != NULL) {
            peer->mcc = a2d->audio_cfg.mcc;
            peer->mcc_valid = true;
        }
        /* Second phone's AUDIO_CFG must not tear down the playing decoder/I2S. */
        if (apply) {
            bt_app_render_apply(&a2d->audio_cfg.mcc, s_a2d_active_hdl != 0);
        } else {
            ESP_LOGI(BT_AV_TAG, "A2DP audio cfg cached for conn %u, codec %d (not rendered)",
                     (unsigned)a2d->audio_cfg.conn_hdl, a2d->audio_cfg.mcc.type);
        }
        break;
    }
    case ESP_A2D_SEP_REG_STATE_EVT: {
        bt_a2d_evt_ext_codec_hdl(event, param);
        break;
    }
    default:
        ESP_LOGE(BT_AV_TAG, "Invalid A2DP event: %d", event);
        break;
    }
}

static void bt_app_a2d_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param)
{
    switch (event) {
    case ESP_A2D_PROF_STATE_EVT:
    case ESP_A2D_SNK_PSC_CFG_EVT:
    case ESP_A2D_SNK_SET_DELAY_VALUE_EVT:
    case ESP_A2D_SNK_GET_DELAY_VALUE_EVT:
    case ESP_A2D_CONNECTION_STATE_EVT:
    case ESP_A2D_AUDIO_STATE_EVT:
    case ESP_A2D_AUDIO_CFG_EVT:
    case ESP_A2D_SEP_REG_STATE_EVT:
    case ESP_A2D_SNK_ACTIVE_DEVICE_EVT: {
        bt_app_work_dispatch(bt_app_a2d_hdl, event, param, sizeof(esp_a2d_cb_param_t), NULL, NULL);
        break;
    }
    default:
        ESP_LOGE(BT_AV_TAG, "Invalid A2DP event: %d", event);
        break;
    }
}

static void bt_app_a2d_audio_data_cb(esp_a2d_conn_hdl_t conn_hdl, esp_a2d_audio_buff_t *audio_buf)
{
    if (audio_buf == NULL || audio_buf->data == NULL || audio_buf->data_len == 0) {
        if (audio_buf != NULL) {
            esp_a2d_audio_buff_free(audio_buf);
        }
        return;
    }

    if (s_a2d_active_hdl != 0 && conn_hdl != s_a2d_active_hdl) {
        esp_a2d_audio_buff_free(audio_buf);
        return;
    }

    if (!s_a2dp_raw_started) {
        esp_a2d_audio_buff_free(audio_buf);
        return;
    }

    if (++s_a2dp_raw_pkt_cnt % 100 == 0) {
        ESP_LOGI(BT_AV_TAG, "Undecoded audio package count: %" PRIu32, s_a2dp_raw_pkt_cnt);
    }

    if (xQueueSend(s_a2dp_raw_q, &audio_buf, 0) != pdTRUE) {
        ESP_LOGW(BT_AV_TAG, "raw queue full, drop packet");
        esp_a2d_audio_buff_free(audio_buf);
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

        esp_a2d_register_callback(&bt_app_a2d_cb);
        assert(esp_a2d_sink_init() == ESP_OK);

        esp_a2d_sink_register_audio_data_callback(bt_app_a2d_audio_data_cb);

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

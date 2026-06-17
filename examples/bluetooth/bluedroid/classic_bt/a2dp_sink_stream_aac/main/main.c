/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

static QueueHandle_t s_a2dp_raw_q;
static TaskHandle_t s_a2dp_dec_task;
static SemaphoreHandle_t s_a2dp_raw_mux;
static bool s_a2dp_raw_started;
static uint32_t s_a2dp_raw_pkt_cnt;
static esp_audio_dec_handle_t s_dec_hdl;
static bool s_dec_opened;

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

void bt_a2d_sink_ext_codec_postproc_start(void)
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

void bt_a2d_sink_ext_codec_postproc_flush(void)
{
    esp_a2d_audio_buff_t *audio_buf;

    if (s_a2dp_raw_mux == NULL) {
        return;
    }
    if (xSemaphoreTake(s_a2dp_raw_mux, portMAX_DELAY) != pdTRUE) {
        return;
    }
    if (s_a2dp_dec_task != NULL) {
        vTaskSuspend(s_a2dp_dec_task);
    }
    if (s_a2dp_raw_q != NULL) {
        while (xQueueReceive(s_a2dp_raw_q, &audio_buf, 0) == pdTRUE) {
            esp_a2d_audio_buff_free(audio_buf);
        }
    }
    if (s_a2dp_dec_task != NULL) {
        vTaskResume(s_a2dp_dec_task);
    }
    bt_app_audio_decoder_close();
    xSemaphoreGive(s_a2dp_raw_mux);
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
    esp_a2d_sink_register_stream_endpoint(0, &mcc_aac);

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
    esp_a2d_sink_register_stream_endpoint(1, &mcc_sbc);
}

static void bt_app_dev_cb(esp_bt_dev_cb_event_t event, esp_bt_dev_cb_param_t *param)
{
    bredr_app_dev_evt_def_hdl(event, param);
}

static void bt_app_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    bredr_app_gap_evt_def_hdl(event, param);
}

static void bt_app_a2d_hdl(uint16_t event, void *param)
{
    esp_a2d_cb_param_t *a2d = (esp_a2d_cb_param_t *)param;

    switch (event) {
    case ESP_A2D_PROF_STATE_EVT:
    case ESP_A2D_SNK_PSC_CFG_EVT:
    case ESP_A2D_SNK_SET_DELAY_VALUE_EVT:
    case ESP_A2D_SNK_GET_DELAY_VALUE_EVT: {
        bt_a2d_evt_def_hdl(event, param);
        break;
    }
    case ESP_A2D_CONNECTION_STATE_EVT: {
        if (a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
            bt_a2d_sink_ext_codec_postproc_flush();
        }
        bt_a2d_evt_int_codec_hdl(event, param);
        break;
    }
    case ESP_A2D_AUDIO_STATE_EVT: {
        if (a2d->audio_stat.state == ESP_A2D_AUDIO_STATE_STARTED) {
            bt_a2d_sink_ext_codec_postproc_start();
        } else if (a2d->audio_stat.state == ESP_A2D_AUDIO_STATE_SUSPEND) {
            bt_a2d_sink_ext_codec_postproc_flush();
        }
        bt_a2d_evt_int_codec_hdl(event, param);
        break;
    }
    case ESP_A2D_AUDIO_CFG_EVT:
        bt_app_audio_decoder_apply_mcc(&a2d->audio_cfg.mcc);
        bt_a2d_evt_int_codec_hdl(event, param);
        break;
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
    case ESP_A2D_SEP_REG_STATE_EVT: {
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
    (void)conn_hdl;

    if (audio_buf == NULL || audio_buf->data == NULL || audio_buf->data_len == 0) {
        if (audio_buf != NULL) {
            esp_a2d_audio_buff_free(audio_buf);
        }
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

        bt_app_register_a2dp_sink_seps();
        esp_a2d_sink_register_audio_data_callback(bt_app_a2d_audio_data_cb);

        /* Get the default value of the delay value */
        esp_a2d_sink_get_delay_value();
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

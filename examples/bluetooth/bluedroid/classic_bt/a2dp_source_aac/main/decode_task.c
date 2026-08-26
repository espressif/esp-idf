/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_a2dp_api.h"
#include "esp_audio_enc.h"
#include "esp_aac_enc.h"
#include "decode_task.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define ENCODE_TASK_TAG                 "BT_AV_AAC"

#define A2DP_SRC_SEND_RETRY_DELAY_MS    (2U)
#define A2DP_SRC_SEND_RETRY_MAX         (10U)
#define A2DP_SRC_AAC_MTU_OVERHEAD_BYTES (20U)
#define A2DP_SRC_STREAM_TASK_STACK      (4 * 1024)
#define A2DP_SRC_STREAM_TASK_PRIO       (10)
#define A2DP_SRC_STREAM_STOP_WAIT_MS    (3000U)

#define A2DP_SRC_TONE_FREQ_HZ           (440.0f)
#define A2DP_SRC_TONE_AMPLITUDE         (12000.0f)

#define ENCODE_DEFAULT_SAMPLE_RATE_HZ   (44100)
#define ENCODE_DEFAULT_CHANNEL          (ESP_AUDIO_DUAL)
#define ENCODE_DEFAULT_AAC_BITRATE       (128000)

typedef struct {
    esp_audio_enc_handle_t enc_hdl;
    uint8_t *pcm;
    uint8_t *aac;
    int inbuf_sz;
    int outbuf_sz;
    int sample_rate_hz;
    int channel_count;
    float tone_phase;
    uint32_t samples_per_frame;
    uint64_t frame_us;
    int64_t next_frame_us;
    uint32_t rtp_ts;
} aac_stream_ctx_t;

static esp_a2d_conn_hdl_t s_conn_hdl;
static uint16_t s_audio_mtu;
static esp_a2d_mcc_t s_stream_mcc;
static bool s_stream_mcc_valid;
static TaskHandle_t s_stream_task_hdl;
static volatile bool s_stream_run;
static bool s_enc_registered;

void encode_task_set_a2dp_link(esp_a2d_conn_hdl_t conn_hdl, uint16_t audio_mtu)
{
    s_conn_hdl = conn_hdl;
    s_audio_mtu = audio_mtu;
}

void encode_task_set_stream_config(const esp_a2d_mcc_t *mcc)
{
    if (mcc == NULL || mcc->type != ESP_A2D_MCT_M24) {
        s_stream_mcc_valid = false;
        return;
    }

    s_stream_mcc = *mcc;
    s_stream_mcc_valid = true;
}

bool encode_task_has_stream_config(void)
{
    return s_stream_mcc_valid;
}

static unsigned encode_task_popcount_u8(uint8_t v)
{
    unsigned n = 0;

    while (v != 0) {
        n += (unsigned)(v & 1U);
        v >>= 1;
    }
    return n;
}

static bool encode_task_m24_sample_rate_hz(const esp_a2d_cie_m24_t *m24, int *hz_out)
{
    uint8_t sf1 = m24->samp_freq1;
    uint8_t sf2 = m24->samp_freq2;

    if (encode_task_popcount_u8(sf1) + encode_task_popcount_u8(sf2) != 1U) {
        return false;
    }

    if (sf1 != 0) {
        if (sf1 & ESP_A2D_M24_CIE_SF1_8K) {
            *hz_out = ESP_AUDIO_SAMPLE_RATE_8K;
        } else if (sf1 & ESP_A2D_M24_CIE_SF1_11K) {
            *hz_out = ESP_AUDIO_SAMPLE_RATE_11K;
        } else if (sf1 & ESP_A2D_M24_CIE_SF1_12K) {
            *hz_out = ESP_AUDIO_SAMPLE_RATE_12K;
        } else if (sf1 & ESP_A2D_M24_CIE_SF1_16K) {
            *hz_out = ESP_AUDIO_SAMPLE_RATE_16K;
        } else if (sf1 & ESP_A2D_M24_CIE_SF1_22K) {
            *hz_out = ESP_AUDIO_SAMPLE_RATE_22K;
        } else if (sf1 & ESP_A2D_M24_CIE_SF1_24K) {
            *hz_out = ESP_AUDIO_SAMPLE_RATE_24K;
        } else if (sf1 & ESP_A2D_M24_CIE_SF1_32K) {
            *hz_out = ESP_AUDIO_SAMPLE_RATE_32K;
        } else if (sf1 & ESP_A2D_M24_CIE_SF1_44K) {
            *hz_out = ESP_AUDIO_SAMPLE_RATE_44K;
        } else {
            return false;
        }
        return true;
    }

    if (sf2 & ESP_A2D_M24_CIE_SF2_48K) {
        *hz_out = ESP_AUDIO_SAMPLE_RATE_48K;
    } else if (sf2 & ESP_A2D_M24_CIE_SF2_64K) {
        *hz_out = ESP_AUDIO_SAMPLE_RATE_64K;
    } else if (sf2 & ESP_A2D_M24_CIE_SF2_88K) {
        *hz_out = ESP_AUDIO_SAMPLE_RATE_88K;
    } else if (sf2 & ESP_A2D_M24_CIE_SF2_96K) {
        *hz_out = ESP_AUDIO_SAMPLE_RATE_96K;
    } else {
        return false;
    }

    return true;
}

static bool encode_task_m24_channel(const esp_a2d_cie_m24_t *m24, int *channel_out)
{
    uint8_t ch = m24->ch;

    if (encode_task_popcount_u8(ch) != 1U) {
        return false;
    }

    if (ch & ESP_A2D_M24_CIE_CH_1) {
        *channel_out = ESP_AUDIO_MONO;
        return true;
    }
    if (ch & ESP_A2D_M24_CIE_CH_2) {
        *channel_out = ESP_AUDIO_DUAL;
        return true;
    }

    ESP_LOGW(ENCODE_TASK_TAG, "M24 channel 0x%02x not supported, use stereo", ch);
    *channel_out = ESP_AUDIO_DUAL;
    return true;
}

static uint32_t encode_task_m24_bitrate_bps(const esp_a2d_cie_m24_t *m24)
{
    return ((uint32_t)(m24->br1 & ESP_A2D_M24_CIE_BR1_MSK) << 16) |
           ((uint32_t)(m24->br2 & ESP_A2D_M24_CIE_BR2_MSK) << 8) |
           (uint32_t)(m24->br3 & ESP_A2D_M24_CIE_BR3_MSK);
}

static void encode_task_build_aac_config(esp_aac_enc_config_t *aac_cfg)
{
    *aac_cfg = (esp_aac_enc_config_t) {
        .sample_rate = ENCODE_DEFAULT_SAMPLE_RATE_HZ,
        .channel = ENCODE_DEFAULT_CHANNEL,
        .bits_per_sample = ESP_AUDIO_BIT16,
        .bitrate = ENCODE_DEFAULT_AAC_BITRATE,
        .adts_used = false,
    };

    if (!s_stream_mcc_valid) {
        ESP_LOGW(ENCODE_TASK_TAG, "No M24 config, use AAC defaults");
        return;
    }

    const esp_a2d_cie_m24_t *m24 = &s_stream_mcc.cie.m24_info;
    int sample_rate_hz = 0;
    int channel = 0;

    if (!encode_task_m24_sample_rate_hz(m24, &sample_rate_hz)) {
        ESP_LOGW(ENCODE_TASK_TAG, "Invalid M24 sample rate, use %d Hz", aac_cfg->sample_rate);
    } else {
        aac_cfg->sample_rate = sample_rate_hz;
    }

    if (!encode_task_m24_channel(m24, &channel)) {
        ESP_LOGW(ENCODE_TASK_TAG, "Invalid M24 channels, use ch %d", aac_cfg->channel);
    } else {
        aac_cfg->channel = channel;
    }

    uint32_t br = encode_task_m24_bitrate_bps(m24);
    bool vbr = (m24->vbr & ESP_A2D_M24_CIE_VBR_SUPPORT) != 0;

    if (br > 0) {
        aac_cfg->bitrate = (int)br;
    } else if (vbr) {
        aac_cfg->bitrate = 0;
    }

    ESP_LOGI(ENCODE_TASK_TAG, "AAC enc: %d Hz, ch %d, bitrate %d (vbr=%d)",
             aac_cfg->sample_rate, aac_cfg->channel, aac_cfg->bitrate, vbr);
}

static uint64_t encode_task_frame_us(int sample_rate_hz, uint32_t samples_per_frame)
{
    if (sample_rate_hz <= 0 || samples_per_frame == 0) {
        return 23000ULL;
    }

    return ((uint64_t)samples_per_frame * 1000000ULL) / (uint64_t)sample_rate_hz;
}

static void encode_task_wait_ms(uint32_t wait_ms)
{
    if (wait_ms == 0) {
        return;
    }

    (void)ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(wait_ms));
}

static void encode_task_pace_frame(aac_stream_ctx_t *ctx)
{
    int64_t now_us = esp_timer_get_time();

    if (ctx->next_frame_us <= 0) {
        ctx->next_frame_us = now_us + (int64_t)ctx->frame_us;
        return;
    }

    if (now_us < ctx->next_frame_us) {
        uint32_t wait_ms = (uint32_t)((ctx->next_frame_us - now_us + 999) / 1000);
        encode_task_wait_ms(wait_ms);
    } else if ((uint64_t)(now_us - ctx->next_frame_us) > ctx->frame_us) {
        ctx->next_frame_us = esp_timer_get_time();
    }

    ctx->next_frame_us += (int64_t)ctx->frame_us;
}

static uint16_t encode_task_max_encoded_bytes(uint16_t mtu)
{
    if (mtu <= A2DP_SRC_AAC_MTU_OVERHEAD_BYTES) {
        return 0;
    }
    return (uint16_t)(mtu - A2DP_SRC_AAC_MTU_OVERHEAD_BYTES);
}

static bool encode_task_payload_fits_mtu(uint16_t encoded_bytes, uint16_t mtu)
{
    if (encoded_bytes == 0 || mtu == 0) {
        return false;
    }

    return encoded_bytes <= encode_task_max_encoded_bytes(mtu);
}

static esp_err_t encode_task_send_audio_frame(esp_a2d_conn_hdl_t conn_hdl, esp_a2d_audio_buff_t *audio_buff)
{
    for (unsigned retry = 0; retry < A2DP_SRC_SEND_RETRY_MAX && s_stream_run; retry++) {
        esp_err_t err = esp_a2d_source_audio_data_send(conn_hdl, audio_buff);
        if (err == ESP_OK) {
            return ESP_OK;
        }
        if (err == ESP_ERR_INVALID_ARG || err == ESP_ERR_INVALID_SIZE || err == ESP_ERR_INVALID_STATE) {
            return err;
        }
        vTaskDelay(pdMS_TO_TICKS(A2DP_SRC_SEND_RETRY_DELAY_MS));
    }

    return ESP_FAIL;
}

static void encode_task_fill_pcm_sine(aac_stream_ctx_t *ctx, uint8_t *pcm, int pcm_len)
{
    int16_t *samples = (int16_t *)pcm;
    int ch = ctx->channel_count > 0 ? ctx->channel_count : 2;
    int total = pcm_len / (int)sizeof(int16_t);
    int frames = total / ch;
    float phase_inc = (2.0f * (float)M_PI * A2DP_SRC_TONE_FREQ_HZ) / (float)ctx->sample_rate_hz;

    for (int i = 0; i < frames; i++) {
        int16_t v = (int16_t)(A2DP_SRC_TONE_AMPLITUDE * sinf(ctx->tone_phase));

        for (int c = 0; c < ch; c++) {
            samples[i * ch + c] = v;
        }

        ctx->tone_phase += phase_inc;
        if (ctx->tone_phase >= 2.0f * (float)M_PI) {
            ctx->tone_phase -= 2.0f * (float)M_PI;
        }
    }
}

static bool encode_task_register_encoder(void)
{
    if (s_enc_registered) {
        return true;
    }

    if (esp_aac_enc_register() != ESP_AUDIO_ERR_OK) {
        ESP_LOGE(ENCODE_TASK_TAG, "esp_aac_enc_register failed");
        return false;
    }

    s_enc_registered = true;
    return true;
}

static bool encode_task_open_encoder(aac_stream_ctx_t *ctx)
{
    static esp_aac_enc_config_t aac_cfg;
    esp_audio_enc_config_t enc_cfg;
    esp_audio_enc_handle_t enc_hdl = NULL;
    esp_audio_enc_info_t enc_info = {0};

    encode_task_build_aac_config(&aac_cfg);

    enc_cfg.type = ESP_AUDIO_TYPE_AAC;
    enc_cfg.cfg = &aac_cfg;
    enc_cfg.cfg_sz = sizeof(aac_cfg);

    if (!encode_task_register_encoder()) {
        return false;
    }

    if (esp_audio_enc_open(&enc_cfg, &enc_hdl) != ESP_AUDIO_ERR_OK) {
        ESP_LOGE(ENCODE_TASK_TAG, "esp_audio_enc_open failed");
        return false;
    }

    esp_audio_enc_get_frame_size(enc_hdl, &ctx->inbuf_sz, &ctx->outbuf_sz);
    esp_audio_enc_get_info(enc_hdl, &enc_info);

    ctx->enc_hdl = enc_hdl;
    ctx->channel_count = (int)enc_info.channel > 0 ? (int)enc_info.channel : 2;
    ctx->sample_rate_hz = (int)enc_info.sample_rate > 0 ? (int)enc_info.sample_rate : aac_cfg.sample_rate;
    ctx->samples_per_frame = (uint32_t)(ctx->inbuf_sz / ((int)sizeof(int16_t) * ctx->channel_count));
    if (ctx->samples_per_frame == 0) {
        ctx->samples_per_frame = 1024;
    }

    ESP_LOGI(ENCODE_TASK_TAG, "AAC in=%d out=%d bitrate=%" PRId32,
             ctx->inbuf_sz, ctx->outbuf_sz, enc_info.bitrate);

    return true;
}

static void encode_task_stream_task(void *arg)
{
    aac_stream_ctx_t ctx = {0};

    (void)arg;

    if (!encode_task_open_encoder(&ctx)) {
        vTaskDelete(NULL);
        return;
    }

    ctx.frame_us = encode_task_frame_us(ctx.sample_rate_hz, ctx.samples_per_frame);
    ctx.next_frame_us = 0;

    ESP_LOGI(ENCODE_TASK_TAG, "stream: %" PRIu64 " us/frame, %d Hz, %" PRIu32 " samples",
             ctx.frame_us, ctx.sample_rate_hz, ctx.samples_per_frame);
    ESP_LOGI(ENCODE_TASK_TAG, "A2DP MTU %u, max AAC payload %u", s_audio_mtu,
             encode_task_max_encoded_bytes(s_audio_mtu));

    ctx.pcm = malloc(ctx.inbuf_sz);
    ctx.aac = malloc(ctx.outbuf_sz);
    if (ctx.pcm == NULL || ctx.aac == NULL) {
        ESP_LOGE(ENCODE_TASK_TAG, "stream buffer alloc failed");
        goto cleanup;
    }

    while (s_stream_run) {
        encode_task_pace_frame(&ctx);
        if (!s_stream_run) {
            break;
        }

        encode_task_fill_pcm_sine(&ctx, ctx.pcm, ctx.inbuf_sz);

        esp_audio_enc_in_frame_t in_frame = {
            .buffer = ctx.pcm,
            .len = ctx.inbuf_sz,
        };
        esp_audio_enc_out_frame_t out_frame = {
            .buffer = ctx.aac,
            .len = ctx.outbuf_sz,
        };

        esp_audio_err_t enc_ret = esp_audio_enc_process(ctx.enc_hdl, &in_frame, &out_frame);
        if (enc_ret != ESP_AUDIO_ERR_OK || out_frame.encoded_bytes == 0) {
            ESP_LOGE(ENCODE_TASK_TAG, "AAC encode failed: 0x%x, bytes %d",
                     enc_ret, out_frame.encoded_bytes);
            continue;
        }

        uint16_t encoded_bytes = (uint16_t)out_frame.encoded_bytes;
        if (!encode_task_payload_fits_mtu(encoded_bytes, s_audio_mtu)) {
            ESP_LOGW(ENCODE_TASK_TAG, "encoded %u bytes exceeds MTU %u (max %u), drop",
                     (unsigned)encoded_bytes, s_audio_mtu, encode_task_max_encoded_bytes(s_audio_mtu));
            continue;
        }

        esp_a2d_audio_buff_t *audio_buff = esp_a2d_audio_buff_alloc(encoded_bytes);
        if (audio_buff == NULL) {
            ESP_LOGE(ENCODE_TASK_TAG, "esp_a2d_audio_buff_alloc failed");
            continue;
        }

        audio_buff->data_len = encoded_bytes;
        audio_buff->number_frame = 1;
        audio_buff->timestamp = ctx.rtp_ts;
        memcpy(audio_buff->data, ctx.aac, encoded_bytes);

        esp_err_t err = encode_task_send_audio_frame(s_conn_hdl, audio_buff);
        if (err != ESP_OK) {
            ESP_LOGE(ENCODE_TASK_TAG, "audio send failed: %s", esp_err_to_name(err));
            esp_a2d_audio_buff_free(audio_buff);
            if (err == ESP_ERR_INVALID_STATE) {
                break;
            }
        } else {
            ctx.rtp_ts += ctx.samples_per_frame;
        }
    }

    ESP_LOGI(ENCODE_TASK_TAG, "stream task exiting");

cleanup:
    free(ctx.pcm);
    free(ctx.aac);
    if (ctx.enc_hdl != NULL) {
        esp_audio_enc_close(ctx.enc_hdl);
    }
    s_stream_task_hdl = NULL;
    s_stream_run = false;
    vTaskDelete(NULL);
}

bool encode_task_is_running(void)
{
    return s_stream_task_hdl != NULL;
}

void encode_task_stop(void)
{
    TaskHandle_t task = s_stream_task_hdl;

    if (task == NULL) {
        return;
    }

    s_stream_run = false;
    xTaskNotifyGive(task);

    for (unsigned waited_ms = 0; waited_ms < A2DP_SRC_STREAM_STOP_WAIT_MS; waited_ms += 10) {
        if (s_stream_task_hdl == NULL) {
            ESP_LOGI(ENCODE_TASK_TAG, "stream task stopped");
            s_conn_hdl = 0;
            s_audio_mtu = 0;
            return;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    if (s_stream_task_hdl != NULL) {
        ESP_LOGW(ENCODE_TASK_TAG, "stream task stop timeout, delete task");
        vTaskDelete(s_stream_task_hdl);
        s_stream_task_hdl = NULL;
        s_stream_run = false;
    }

    s_conn_hdl = 0;
    s_audio_mtu = 0;
}

esp_err_t encode_task_launch(void)
{
    if (s_stream_task_hdl != NULL) {
        ESP_LOGW(ENCODE_TASK_TAG, "stream task already running");
        return ESP_ERR_INVALID_STATE;
    }

    if (!s_stream_mcc_valid) {
        ESP_LOGE(ENCODE_TASK_TAG, "No M24 stream config");
        return ESP_ERR_INVALID_STATE;
    }

    s_stream_run = true;
    if (xTaskCreate(encode_task_stream_task, "a2dp_aac", A2DP_SRC_STREAM_TASK_STACK, NULL,
                    A2DP_SRC_STREAM_TASK_PRIO, &s_stream_task_hdl) != pdPASS) {
        ESP_LOGE(ENCODE_TASK_TAG, "stream task create failed");
        s_stream_run = false;
        s_stream_task_hdl = NULL;
        return ESP_FAIL;
    }

    return ESP_OK;
}

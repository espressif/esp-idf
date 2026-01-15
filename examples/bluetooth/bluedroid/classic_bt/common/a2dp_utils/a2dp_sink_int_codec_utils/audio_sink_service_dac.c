/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "esp_log.h"
#include "esp_a2dp_api.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "driver/dac_continuous.h"
#include "audio_sink_service.h"

#if defined(CONFIG_EXAMPLE_A2DP_SINK_OUTPUT_INTERNAL_DAC)

/* log tag */
#define AUDIO_SNK_SRV_DAC_TAG    "SNK_SRV_DAC"

typedef struct {
    dac_continuous_handle_t tx_chan;        /* handle of dac continuous channel */
    audio_sink_chan_st_t chan_st;           /* dac channel status */
    TaskHandle_t write_task_handle;         /* handle of writing task */
    RingbufHandle_t ringbuf;                /* handle of ringbuffer */
    SemaphoreHandle_t write_semaphore;      /* handle of write semaphore */
    uint16_t ringbuffer_mode;               /* ringbuffer mode */
} audio_sink_srv_dac_cb_t;

/*******************************
 * STATIC FUNCTION DECLARATIONS
 ******************************/

/* task handler for writing data to dac */
static void audio_sink_srv_dac_task_handler(void *arg);

/*******************************
 * STATIC VARIABLE DEFINITIONS
 ******************************/

/* audio sink service for dac control block */
static audio_sink_srv_dac_cb_t s_dac_cb;

/*******************************
 * STATIC FUNCTION DEFINITIONS
 ******************************/

static void audio_sink_srv_dac_task_handler(void *arg)
{
    uint8_t *data = NULL;
    size_t item_size = 0;
    const size_t item_size_upto = 240 * 6;
    size_t bytes_written = 0;

    for (;;) {
        if (pdTRUE == xSemaphoreTake(s_dac_cb.write_semaphore, portMAX_DELAY)) {
            for (;;) {
                item_size = 0;
                /* receive data from ringbuffer and write it to DAC DMA transmit buffer */
                data = (uint8_t *)xRingbufferReceiveUpTo(s_dac_cb.ringbuf, &item_size, (TickType_t)pdMS_TO_TICKS(20), item_size_upto);
                if (item_size == 0) {
                    ESP_LOGI(AUDIO_SNK_SRV_DAC_TAG, "ringbuffer underflowed! mode changed: RINGBUFFER_MODE_PREFETCHING");
                    s_dac_cb.ringbuffer_mode = RINGBUFFER_MODE_PREFETCHING;
                    break;
                }

                dac_continuous_write(s_dac_cb.tx_chan, data, item_size, &bytes_written, -1);
                vRingbufferReturnItem(s_dac_cb.ringbuf, (void *)data);
            }
        }
    }
}

/*******************************
 * EXTERNAL FUNCTION DEFINITIONS
 ******************************/

void audio_sink_srv_open(void)
{
    memset(&s_dac_cb, 0, sizeof(audio_sink_srv_dac_cb_t));
    dac_continuous_config_t cont_cfg = {
        .chan_mask = DAC_CHANNEL_MASK_ALL,
        .desc_num = 8,
        .buf_size = 2048,
        .freq_hz = 44100,
        .offset = 127,
        .clk_src = DAC_DIGI_CLK_SRC_DEFAULT,   // Using APLL as clock source to get a wider frequency range
        .chan_mode = DAC_CHANNEL_MODE_ALTER,
    };
    /* Allocate continuous channels */
    ESP_ERROR_CHECK(dac_continuous_new_channels(&cont_cfg, &s_dac_cb.tx_chan));
    s_dac_cb.chan_st = CHANNEL_STATUS_OPENED;
}

void audio_sink_srv_close(void)
{
    audio_sink_srv_stop();

    if (s_dac_cb.chan_st == CHANNEL_STATUS_OPENED) {
        ESP_ERROR_CHECK(dac_continuous_del_channels(s_dac_cb.tx_chan));
        s_dac_cb.chan_st = CHANNEL_STATUS_IDLE;
    }
    if (s_dac_cb.write_task_handle) {
        vTaskDelete(s_dac_cb.write_task_handle);
        s_dac_cb.write_task_handle = NULL;
    }
    if (s_dac_cb.ringbuf) {
        vRingbufferDelete(s_dac_cb.ringbuf);
        s_dac_cb.ringbuf = NULL;
    }
    if (s_dac_cb.write_semaphore) {
        vSemaphoreDelete(s_dac_cb.write_semaphore);
        s_dac_cb.write_semaphore = NULL;
    }
    memset(&s_dac_cb, 0, sizeof(audio_sink_srv_dac_cb_t));
}

void audio_sink_srv_start(void)
{
    if (s_dac_cb.chan_st != CHANNEL_STATUS_OPENED) {
        ESP_LOGE(AUDIO_SNK_SRV_DAC_TAG, "%s, TX channel wrong state: %d", __func__, s_dac_cb.chan_st);
        return;
    }
    ESP_ERROR_CHECK(dac_continuous_enable(s_dac_cb.tx_chan));
    s_dac_cb.chan_st = CHANNEL_STATUS_ENABLED;

    ESP_LOGI(AUDIO_SNK_SRV_DAC_TAG, "ringbuffer data empty! mode changed: RINGBUFFER_MODE_PREFETCHING");
    s_dac_cb.ringbuffer_mode = RINGBUFFER_MODE_PREFETCHING;
    if ((s_dac_cb.write_semaphore = xSemaphoreCreateBinary()) == NULL) {
        ESP_LOGE(AUDIO_SNK_SRV_DAC_TAG, "%s, Semaphore create failed", __func__);
        return;
    }
    if ((s_dac_cb.ringbuf = xRingbufferCreate(RINGBUF_HIGHEST_WATER_LEVEL, RINGBUF_TYPE_BYTEBUF)) == NULL) {
        ESP_LOGE(AUDIO_SNK_SRV_DAC_TAG, "%s, ringbuffer create failed", __func__);
        return;
    }
    xTaskCreate(audio_sink_srv_dac_task_handler, "BtDACTask", 4 * 1024, NULL, configMAX_PRIORITIES - 3, &s_dac_cb.write_task_handle);
}

void audio_sink_srv_stop(void)
{
    if (s_dac_cb.chan_st == CHANNEL_STATUS_ENABLED) {
        ESP_ERROR_CHECK(dac_continuous_disable(s_dac_cb.tx_chan));
        s_dac_cb.chan_st = CHANNEL_STATUS_OPENED;
    }
}

void audio_sink_srv_codec_info_update(esp_a2d_mcc_t *mcc)
{
    ESP_LOGI(AUDIO_SNK_SRV_DAC_TAG, "A2DP audio stream configuration, codec type: %d", mcc->type);
    /* for now only SBC stream is supported */
    if (mcc->type == ESP_A2D_MCT_SBC) {
        int sample_rate = 16000;
        int ch_count = 2;
        if (mcc->cie.sbc_info.samp_freq & ESP_A2D_SBC_CIE_SF_32K) {
            sample_rate = 32000;
        } else if (mcc->cie.sbc_info.samp_freq & ESP_A2D_SBC_CIE_SF_44K) {
            sample_rate = 44100;
        } else if (mcc->cie.sbc_info.samp_freq & ESP_A2D_SBC_CIE_SF_48K) {
            sample_rate = 48000;
        }

        if (mcc->cie.sbc_info.ch_mode & ESP_A2D_SBC_CIE_CH_MODE_MONO) {
            ch_count = 1;
        }
        dac_continuous_del_channels(s_dac_cb.tx_chan);
        dac_continuous_config_t cont_cfg = {
            .chan_mask = DAC_CHANNEL_MASK_ALL,
            .desc_num = 8,
            .buf_size = 2048,
            .freq_hz = sample_rate,
            .offset = 127,
            .clk_src = DAC_DIGI_CLK_SRC_DEFAULT,   // Using APLL as clock source to get a wider frequency range
            .chan_mode = (ch_count == 1) ? DAC_CHANNEL_MODE_SIMUL : DAC_CHANNEL_MODE_ALTER,
        };
        /* Allocate continuous channels */
        dac_continuous_new_channels(&cont_cfg, &s_dac_cb.tx_chan);
        ESP_LOGI(AUDIO_SNK_SRV_DAC_TAG, "Configure audio player: 0x%x-0x%x-0x%x-0x%x-0x%x-%d-%d",
                 mcc->cie.sbc_info.samp_freq,
                 mcc->cie.sbc_info.ch_mode,
                 mcc->cie.sbc_info.block_len,
                 mcc->cie.sbc_info.num_subbands,
                 mcc->cie.sbc_info.alloc_mthd,
                 mcc->cie.sbc_info.min_bitpool,
                 mcc->cie.sbc_info.max_bitpool);
        ESP_LOGI(AUDIO_SNK_SRV_DAC_TAG, "Audio player configured, sample rate: %d", sample_rate);
    }
}

size_t audio_sink_srv_data_output(const uint8_t *data, size_t size)
{
    size_t item_size = 0;
    BaseType_t done = pdFALSE;

    if (s_dac_cb.ringbuffer_mode == RINGBUFFER_MODE_DROPPING) {
        ESP_LOGW(AUDIO_SNK_SRV_DAC_TAG, "ringbuffer is full, drop this packet!");
        vRingbufferGetInfo(s_dac_cb.ringbuf, NULL, NULL, NULL, NULL, &item_size);
        if (item_size <= RINGBUF_PREFETCH_WATER_LEVEL) {
            ESP_LOGI(AUDIO_SNK_SRV_DAC_TAG, "ringbuffer data decreased! mode changed: RINGBUFFER_MODE_PROCESSING");
            s_dac_cb.ringbuffer_mode = RINGBUFFER_MODE_PROCESSING;
        }
        return 0;
    }

    done = xRingbufferSend(s_dac_cb.ringbuf, (void *)data, size, (TickType_t)0);

    if (!done) {
        ESP_LOGW(AUDIO_SNK_SRV_DAC_TAG, "ringbuffer overflowed, ready to decrease data! mode changed: RINGBUFFER_MODE_DROPPING");
        s_dac_cb.ringbuffer_mode = RINGBUFFER_MODE_DROPPING;
    }

    if (s_dac_cb.ringbuffer_mode == RINGBUFFER_MODE_PREFETCHING) {
        vRingbufferGetInfo(s_dac_cb.ringbuf, NULL, NULL, NULL, NULL, &item_size);
        if (item_size >= RINGBUF_PREFETCH_WATER_LEVEL) {
            ESP_LOGI(AUDIO_SNK_SRV_DAC_TAG, "ringbuffer data increased! mode changed: RINGBUFFER_MODE_PROCESSING");
            s_dac_cb.ringbuffer_mode = RINGBUFFER_MODE_PROCESSING;
            if (pdFALSE == xSemaphoreGive(s_dac_cb.write_semaphore)) {
                ESP_LOGE(AUDIO_SNK_SRV_DAC_TAG, "semaphore give failed");
            }
        }
    }

    return done ? size : 0;
}

#endif /* defined(CONFIG_EXAMPLE_A2DP_SINK_OUTPUT_INTERNAL_DAC) */

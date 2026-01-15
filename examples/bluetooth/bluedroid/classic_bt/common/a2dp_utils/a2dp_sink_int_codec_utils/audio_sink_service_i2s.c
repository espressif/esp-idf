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
#include "driver/i2s_std.h"
#include "audio_sink_service.h"

#if defined(CONFIG_EXAMPLE_A2DP_SINK_OUTPUT_EXTERNAL_I2S)

/* log tag */
#define AUDIO_SNK_SRV_I2S_TAG    "SNK_SRV_I2S"

typedef struct {
    i2s_chan_handle_t tx_chan;        /* handle of i2s channel */
    audio_sink_chan_st_t chan_st;     /* i2s channel status */
    TaskHandle_t write_task_handle;   /* handle of writing task */
    RingbufHandle_t ringbuf;          /* handle of ringbuffer */
    SemaphoreHandle_t write_semaphore;/* handle of write semaphore */
    uint16_t ringbuffer_mode;         /* ringbuffer mode */
} audio_sink_srv_i2s_cb_t;

/*******************************
 * STATIC FUNCTION DECLARATIONS
 ******************************/

/* task handler for writing data to i2s */
static void audio_sink_srv_i2s_task_handler(void *arg);

/*******************************
 * STATIC VARIABLE DEFINITIONS
 ******************************/

/* audio sink service for i2s control block */
static audio_sink_srv_i2s_cb_t s_i2s_cb;

/*******************************
 * STATIC FUNCTION DEFINITIONS
 ******************************/

static void audio_sink_srv_i2s_task_handler(void *arg)
{
    uint8_t *data = NULL;
    size_t item_size = 0;
    /**
     * The total length of DMA buffer of I2S is:
     * `dma_frame_num * dma_desc_num * i2s_channel_num * i2s_data_bit_width / 8`.
     * Transmit `dma_frame_num * dma_desc_num` bytes to DMA is trade-off.
     */
    const size_t item_size_upto = 240 * 6;
    size_t bytes_written = 0;

    for (;;) {
        if (pdTRUE == xSemaphoreTake(s_i2s_cb.write_semaphore, portMAX_DELAY)) {
            for (;;) {
                item_size = 0;
                /* receive data from ringbuffer and write it to I2S DMA transmit buffer */
                data = (uint8_t *)xRingbufferReceiveUpTo(s_i2s_cb.ringbuf, &item_size, (TickType_t)pdMS_TO_TICKS(20), item_size_upto);
                if (item_size == 0) {
                    ESP_LOGI(AUDIO_SNK_SRV_I2S_TAG, "ringbuffer underflowed! mode changed: RINGBUFFER_MODE_PREFETCHING");
                    s_i2s_cb.ringbuffer_mode = RINGBUFFER_MODE_PREFETCHING;
                    break;
                }

                i2s_channel_write(s_i2s_cb.tx_chan, data, item_size, &bytes_written, portMAX_DELAY);
                vRingbufferReturnItem(s_i2s_cb.ringbuf, (void *)data);
            }
        }
    }
}

/*******************************
 * EXTERNAL FUNCTION DEFINITIONS
 ******************************/

void audio_sink_srv_open(void)
{
    memset(&s_i2s_cb, 0, sizeof(audio_sink_srv_i2s_cb_t));
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    chan_cfg.auto_clear = true;
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(44100),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = CONFIG_EXAMPLE_I2S_BCK_PIN,
            .ws = CONFIG_EXAMPLE_I2S_LRCK_PIN,
            .dout = CONFIG_EXAMPLE_I2S_DATA_PIN,
            .din = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    /* initialize I2S channel */
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, &s_i2s_cb.tx_chan, NULL));
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(s_i2s_cb.tx_chan, &std_cfg));
    s_i2s_cb.chan_st = CHANNEL_STATUS_OPENED;
}

void audio_sink_srv_close(void)
{
    audio_sink_srv_stop();

    if (s_i2s_cb.chan_st == CHANNEL_STATUS_OPENED) {
        ESP_ERROR_CHECK(i2s_del_channel(s_i2s_cb.tx_chan));
        s_i2s_cb.chan_st = CHANNEL_STATUS_IDLE;
    }
    if (s_i2s_cb.write_task_handle) {
        vTaskDelete(s_i2s_cb.write_task_handle);
        s_i2s_cb.write_task_handle = NULL;
    }
    if (s_i2s_cb.ringbuf) {
        vRingbufferDelete(s_i2s_cb.ringbuf);
        s_i2s_cb.ringbuf = NULL;
    }
    if (s_i2s_cb.write_semaphore) {
        vSemaphoreDelete(s_i2s_cb.write_semaphore);
        s_i2s_cb.write_semaphore = NULL;
    }
    memset(&s_i2s_cb, 0, sizeof(audio_sink_srv_i2s_cb_t));
}

void audio_sink_srv_start(void)
{
    if (s_i2s_cb.chan_st != CHANNEL_STATUS_OPENED) {
        ESP_LOGE(AUDIO_SNK_SRV_I2S_TAG, "%s, TX channel wrong state: %d", __func__, s_i2s_cb.chan_st);
        return;
    }
    ESP_ERROR_CHECK(i2s_channel_enable(s_i2s_cb.tx_chan));
    s_i2s_cb.chan_st = CHANNEL_STATUS_ENABLED;

    ESP_LOGI(AUDIO_SNK_SRV_I2S_TAG, "ringbuffer data empty! mode changed: RINGBUFFER_MODE_PREFETCHING");
    s_i2s_cb.ringbuffer_mode = RINGBUFFER_MODE_PREFETCHING;
    if ((s_i2s_cb.write_semaphore = xSemaphoreCreateBinary()) == NULL) {
        ESP_LOGE(AUDIO_SNK_SRV_I2S_TAG, "%s, Semaphore create failed", __func__);
        return;
    }
    if ((s_i2s_cb.ringbuf = xRingbufferCreate(RINGBUF_HIGHEST_WATER_LEVEL, RINGBUF_TYPE_BYTEBUF)) == NULL) {
        ESP_LOGE(AUDIO_SNK_SRV_I2S_TAG, "%s, ringbuffer create failed", __func__);
        return;
    }
    xTaskCreate(audio_sink_srv_i2s_task_handler, "BtI2STask", 4 * 1024, NULL, configMAX_PRIORITIES - 3, &s_i2s_cb.write_task_handle);
}

void audio_sink_srv_stop(void)
{
    if (s_i2s_cb.chan_st == CHANNEL_STATUS_ENABLED) {
        ESP_ERROR_CHECK(i2s_channel_disable(s_i2s_cb.tx_chan));
        s_i2s_cb.chan_st = CHANNEL_STATUS_OPENED;
    }
}

void audio_sink_srv_codec_info_update(esp_a2d_mcc_t *mcc)
{
    ESP_LOGI(AUDIO_SNK_SRV_I2S_TAG, "A2DP audio stream configuration, codec type: %d", mcc->type);
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
        i2s_std_clk_config_t clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(sample_rate);
        i2s_std_slot_config_t slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, ch_count);
        i2s_channel_reconfig_std_clock(s_i2s_cb.tx_chan, &clk_cfg);
        i2s_channel_reconfig_std_slot(s_i2s_cb.tx_chan, &slot_cfg);
        ESP_LOGI(AUDIO_SNK_SRV_I2S_TAG, "Configure audio player: 0x%x-0x%x-0x%x-0x%x-0x%x-%d-%d",
                 mcc->cie.sbc_info.samp_freq,
                 mcc->cie.sbc_info.ch_mode,
                 mcc->cie.sbc_info.block_len,
                 mcc->cie.sbc_info.num_subbands,
                 mcc->cie.sbc_info.alloc_mthd,
                 mcc->cie.sbc_info.min_bitpool,
                 mcc->cie.sbc_info.max_bitpool);
        ESP_LOGI(AUDIO_SNK_SRV_I2S_TAG, "Audio player configured, sample rate: %d", sample_rate);
    }
}

size_t audio_sink_srv_data_output(const uint8_t *data, size_t size)
{
    size_t item_size = 0;
    BaseType_t done = pdFALSE;

    if (s_i2s_cb.ringbuffer_mode == RINGBUFFER_MODE_DROPPING) {
        ESP_LOGW(AUDIO_SNK_SRV_I2S_TAG, "ringbuffer is full, drop this packet!");
        vRingbufferGetInfo(s_i2s_cb.ringbuf, NULL, NULL, NULL, NULL, &item_size);
        if (item_size <= RINGBUF_PREFETCH_WATER_LEVEL) {
            ESP_LOGI(AUDIO_SNK_SRV_I2S_TAG, "ringbuffer data decreased! mode changed: RINGBUFFER_MODE_PROCESSING");
            s_i2s_cb.ringbuffer_mode = RINGBUFFER_MODE_PROCESSING;
        }
        return 0;
    }

    done = xRingbufferSend(s_i2s_cb.ringbuf, (void *)data, size, (TickType_t)0);

    if (!done) {
        ESP_LOGW(AUDIO_SNK_SRV_I2S_TAG, "ringbuffer overflowed, ready to decrease data! mode changed: RINGBUFFER_MODE_DROPPING");
        s_i2s_cb.ringbuffer_mode = RINGBUFFER_MODE_DROPPING;
    }

    if (s_i2s_cb.ringbuffer_mode == RINGBUFFER_MODE_PREFETCHING) {
        vRingbufferGetInfo(s_i2s_cb.ringbuf, NULL, NULL, NULL, NULL, &item_size);
        if (item_size >= RINGBUF_PREFETCH_WATER_LEVEL) {
            ESP_LOGI(AUDIO_SNK_SRV_I2S_TAG, "ringbuffer data increased! mode changed: RINGBUFFER_MODE_PROCESSING");
            s_i2s_cb.ringbuffer_mode = RINGBUFFER_MODE_PROCESSING;
            if (pdFALSE == xSemaphoreGive(s_i2s_cb.write_semaphore)) {
                ESP_LOGE(AUDIO_SNK_SRV_I2S_TAG, "semaphore give failed");
            }
        }
    }

    return done ? size : 0;
}

#endif /* defined(CONFIG_EXAMPLE_A2DP_SINK_OUTPUT_EXTERNAL_I2S) */

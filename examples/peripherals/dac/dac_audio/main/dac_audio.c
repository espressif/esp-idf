/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/dac_driver.h"
#include "esp_check.h"
#include "audio_example_file.h"

#define EXAMPLE_CONVERT_FREQ_HZ     16000   // DAC conversion frequency, it determines how fast to play the audio

static const char *TAG = "dac audio";

void app_main(void)
{
    ESP_LOGI(TAG, "DAC audio example start");
    ESP_LOGI(TAG, "--------------------------------------");

    dac_channels_handle_t dac_handle;

    dac_channels_config_t cfg = {
        .chan_sel = DAC_CHANNEL_MASK_BOTH,
    };
    dac_conti_config_t dma_cfg = {
        .freq_hz = EXAMPLE_CONVERT_FREQ_HZ,
        /* Assume the data in buffer is 'A B C D E F'
         * DAC_CHANNEL_MODE_SIMUL:
         *      - channel 0: A B C D E F
         *      - channel 1: A B C D E F
         * DAC_CHANNEL_MODE_ALTER:
         *      - channel 0: A C E
         *      - channel 1: B D F
         */
        .chan_mode = DAC_CHANNEL_MODE_SIMUL,
        .clk_src = DAC_DIGI_CLK_SRC_APLL,    // Using APLL as clock source to get a wider frequency range
        .desc_num = 5,                  // At least 2 descriptions
    };
    /* Allocate the channel group */
    ESP_ERROR_CHECK(dac_new_channels(&cfg, &dac_handle));
    /* Enable the channels in the group */
    ESP_ERROR_CHECK(dac_channels_enable(dac_handle));
    /* Initialize DAC DMA peripheral */
    ESP_ERROR_CHECK(dac_channels_init_continuous_mode(dac_handle, &dma_cfg));
    /* Start the DAC DMA peripheral */
    ESP_ERROR_CHECK(dac_channels_enable_continuous_mode(dac_handle));
    ESP_LOGI(TAG, "DAC initialized success, DAC DMA is ready");

    uint8_t *audio = NULL;
    size_t audio_size = sizeof(audio_table);
#if CONFIG_IDF_TARGET_ESP32
    audio = (uint8_t *)calloc(1, audio_size * 2);
    assert(audio);
    /* On ESP32, the data have to align with 16 bits, and only the high 8 bit will be converted by DAC */
    for (int i = 0; i < audio_size; i++) {
        audio[2 * i + 1] = audio_table[i];
    }
#else
    audio = (uint8_t *)calloc(1, audio_size);
    assert(audio);
    /* 'audio_table' is a const buffer which can't be sent by DMA directly, copy it into a new buffer */
    memcpy(audio, audio_table, audio_size);
#endif

    while (1) {
        ESP_LOGI(TAG, "Audio size %d bytes, played at frequency %d Hz", audio_size, EXAMPLE_CONVERT_FREQ_HZ);
        ESP_ERROR_CHECK(dac_channels_write_continuously(dac_handle, audio, audio_size, NULL, portMAX_DELAY));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

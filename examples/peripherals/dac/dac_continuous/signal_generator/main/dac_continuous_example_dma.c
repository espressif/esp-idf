/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/dac_continuous.h"
#include "esp_check.h"
#include "dac_continuous_example.h"

#define EXAMPLE_WAVE_FREQ_HZ       2000                      // Default wave frequency 2000 Hz, it can't be too low
#define EXAMPLE_UPDATE_RATE_HZ     (EXAMPLE_ARRAY_LEN * EXAMPLE_WAVE_FREQ_HZ) // The frequency at which the DAC samples (codes) are updated

static void dac_dma_write_task(void *args)
{
    dac_continuous_handle_t handle = (dac_continuous_handle_t)args;
    dac_example_wave_type_t wave_sel = DAC_SINE_WAVE; // Start from sine wave

    while (1) {
        ESP_LOGI(TAG, "%s wave start", wave_name[wave_sel]);
        /* The wave in the buffer will be converted cyclically */
        ESP_ERROR_CHECK(dac_continuous_write_cyclically(handle, wave_data[wave_sel], EXAMPLE_ARRAY_LEN, NULL));
        /* Switch wave every CONFIG_EXAMPLE_WAVE_PERIOD_SEC seconds */
        vTaskDelay(pdMS_TO_TICKS(CONFIG_EXAMPLE_WAVE_PERIOD_SEC * 1000));
        ESP_ERROR_CHECK(dac_continuous_stop_cyclically(handle));
        wave_sel = (wave_sel + 1) % DAC_WAVE_MAX;
    }
}

void example_dac_continuous_by_dma(void)
{
    dac_continuous_handle_t cont_handle;
    dac_continuous_config_t cont_cfg = {
        .chan_mask = DAC_CHANNEL_MASK_CH0,  // This example outputs on one channel only
        .desc_num = 8,
        .buf_size = 2048,
        .freq_hz = EXAMPLE_UPDATE_RATE_HZ,
    };
    /* Allocate continuous channel */
    ESP_ERROR_CHECK(dac_continuous_new_channels(&cont_cfg, &cont_handle));
    /* Generate wave data */
    example_generate_wave(dac_continuous_get_bitwidth(cont_handle));
    /* Enable the channels in the group */
    ESP_ERROR_CHECK(dac_continuous_enable(cont_handle));

    example_log_info(EXAMPLE_UPDATE_RATE_HZ, EXAMPLE_WAVE_FREQ_HZ);

    /* Start to convert wave */
    xTaskCreate(dac_dma_write_task, "dac_dma_write_task", 4096, cont_handle, 5, NULL);
}

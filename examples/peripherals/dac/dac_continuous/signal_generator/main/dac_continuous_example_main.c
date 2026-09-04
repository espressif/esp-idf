/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include "esp_log.h"
#include "hal/dac_types.h"
#include "hal/dac_periph.h"
#include "dac_continuous_example.h"

#define EXAMPLE_DAC_CHAN_IO  dac_periph_signal.dac_channel_io_num[DAC_CHAN_0]

const char* wave_name[DAC_WAVE_MAX] = {"sine", "triangle", "sawtooth", "square"};
dac_example_sample_t wave_data[DAC_WAVE_MAX][EXAMPLE_ARRAY_LEN];

void example_generate_wave(uint8_t resolution_bits)
{
    const size_t N = EXAMPLE_ARRAY_LEN;
    const uint16_t max_code = (1U << resolution_bits) - 1;

    for (size_t i = 0; i < N; i++) {
        wave_data[DAC_SINE_WAVE][i] = (sin(2 * M_PI * i / N) + 1) / 2 * max_code + 0.5;
        wave_data[DAC_TRIANGLE_WAVE][i] = (i < N / 2) ? (2 * max_code * i / N) : (2 * max_code * (N - i) / N);
        wave_data[DAC_SAWTOOTH_WAVE][i] = max_code * i / N;
        wave_data[DAC_SQUARE_WAVE][i] = (i < N / 2) ? max_code : 0;
    }
}

void example_log_info(uint32_t update_rate, uint32_t wave_freq)
{
    ESP_LOGI(TAG, "--------------------------------------------------");
#if CONFIG_EXAMPLE_DAC_CONTINUOUS_BY_DMA
    ESP_LOGI(TAG, "DAC continuous output by DMA");
#else
    ESP_LOGI(TAG, "DAC continuous output by timer");
#endif
    ESP_LOGI(TAG, "DAC channel 0 io: GPIO_NUM_%d", EXAMPLE_DAC_CHAN_IO);
    ESP_LOGI(TAG, "Waveform: SINE -> TRIANGLE -> SAWTOOTH -> SQUARE");
    ESP_LOGI(TAG, "DAC sample update rate (Hz): %"PRIu32, update_rate);
    ESP_LOGI(TAG, "Waveform frequency (Hz): %"PRIu32, wave_freq);
    ESP_LOGI(TAG, "--------------------------------------------------");
}

void app_main(void)
{
#if CONFIG_EXAMPLE_DAC_CONTINUOUS_BY_DMA
    /* Output 2 kHz waves using DMA */
    example_dac_continuous_by_dma();
#else
    /* Output 50 Hz waves using timer interrupt */
    example_dac_continuous_by_timer();
#endif
}

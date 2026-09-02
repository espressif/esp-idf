/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "driver/dac_cosine.h"
#include "esp_check.h"

void app_main(void)
{
    dac_cosine_handle_t chan0_handle;
    dac_cosine_handle_t chan1_handle;
    /* Normally two channels can only be configured to one frequency
     * But we can set force_set_freq bit to force update the frequency
     * The example here will produce cosine wave at 8 KHz on both channels */
    dac_cosine_config_t cos0_cfg = {
        .chan_id = DAC_CHAN_0,
        .freq_hz = 1000, // It will be covered by 8000 in the latter configuration
        .phase = DAC_COSINE_PHASE_0,
        .atten = DAC_COSINE_ATTEN_DEFAULT,
        .flags.force_set_freq = false,
    };
    dac_cosine_config_t cos1_cfg = {
        .chan_id = DAC_CHAN_1,
        .freq_hz = 8000,
        .phase = DAC_COSINE_PHASE_180,
        .atten = DAC_COSINE_ATTEN_DB_6,
        .flags.force_set_freq = true, // set true will allow to overwrite the frequency that set before
    };
    ESP_ERROR_CHECK(dac_cosine_new_channel(&cos0_cfg, &chan0_handle));
    ESP_ERROR_CHECK(dac_cosine_new_channel(&cos1_cfg, &chan1_handle));
    ESP_ERROR_CHECK(dac_cosine_start(chan0_handle));
    ESP_ERROR_CHECK(dac_cosine_start(chan1_handle));

    printf("DAC cosine wave started\n");
}

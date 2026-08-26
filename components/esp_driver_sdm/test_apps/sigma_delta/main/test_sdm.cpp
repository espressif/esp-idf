/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "driver/sdm.h"
#include "hal/sdm_periph.h"
#include "hal/sdm_caps.h"
#include "esp_attr.h"

TEST_CASE("sdm_channel_install_uninstall", "[sdm]")
{
    printf("install sdm channels exhaustively\r\n");
    sdm_config_t config = {
        .gpio_num = GPIO_NUM_0,
        .clk_src = SDM_CLK_SRC_DEFAULT,
        .sample_rate_hz = 1000000,
        .flags = {
            .invert_out = false,
            .allow_pd = false,
        },
    };
    sdm_channel_handle_t chans[SDM_CAPS_GET(INST_NUM)][SDM_CAPS_GET(CHANS_PER_INST)] = {};
    for (int i = 0; i < SDM_CAPS_GET(INST_NUM); i++) {
        for (int j = 0; j < SDM_CAPS_GET(CHANS_PER_INST); j++) {
            TEST_ESP_OK(sdm_new_channel(&config, &chans[i][j]));
        }
        TEST_ESP_ERR(ESP_ERR_NOT_FOUND, sdm_new_channel(&config, &chans[0][0]));
    }

    printf("delete sdm channels\r\n");
    for (int i = 0; i < SDM_CAPS_GET(INST_NUM); i++) {
        for (int j = 0; j < SDM_CAPS_GET(CHANS_PER_INST); j++) {
            TEST_ESP_OK(sdm_del_channel(chans[i][j]));
        }
    }
}

TEST_CASE("sdm_channel_set_pulse_density", "[sdm]")
{
    const gpio_num_t sdm_chan_gpios[2] = {GPIO_NUM_0, GPIO_NUM_2};
    sdm_config_t config = {
        .gpio_num = GPIO_NUM_NC, // will be set later
        .clk_src = SDM_CLK_SRC_DEFAULT,
        .sample_rate_hz = 1000000,
        .flags = {
            .invert_out = false,
            .allow_pd = false,
        },
    };
    sdm_channel_handle_t chans[2] = {};
    for (size_t i = 0; i < 2; i++) {
        config.gpio_num = sdm_chan_gpios[i];
        TEST_ESP_OK(sdm_new_channel(&config, &chans[i]));
        // should see a ~250KHz (sample_rate/4) square wave
        TEST_ESP_OK(sdm_channel_set_pulse_density(chans[i], 0));
        TEST_ESP_OK(sdm_channel_enable(chans[i]));
    }
    vTaskDelay(pdMS_TO_TICKS(500));

    // can't delete the channel if the channel is in the enable state
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, sdm_del_channel(chans[0]));

    TEST_ESP_OK(sdm_channel_set_pulse_density(chans[0], 127));
    TEST_ESP_OK(sdm_channel_set_pulse_density(chans[1], -128));
    vTaskDelay(pdMS_TO_TICKS(500));

    for (size_t i = 0; i < 2; i++) {
        TEST_ESP_OK(sdm_channel_disable(chans[i]));
        TEST_ESP_OK(sdm_del_channel(chans[i]));
    }
}

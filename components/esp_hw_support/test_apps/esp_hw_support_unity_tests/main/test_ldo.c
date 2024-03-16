/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "esp_ldo_regulator.h"

TEST_CASE("LDO channel acquire and release (no adjustable)", "[LDO]")
{
    esp_ldo_channel_handle_t success_ldo_chans[3] = {};
    esp_ldo_channel_handle_t fail_ldo_chan = NULL;
    esp_ldo_channel_config_t ldo_chan_config = {
        .chan_id = 4,
        .voltage_mv = 1800,
    };
    for (int i = 0; i < 3; i++) {
        TEST_ESP_OK(esp_ldo_acquire_channel(&ldo_chan_config, &success_ldo_chans[i]));
    }
    TEST_ASSERT_EQUAL(success_ldo_chans[0], success_ldo_chans[1]);
    TEST_ASSERT_EQUAL(success_ldo_chans[0], success_ldo_chans[2]);
    // can't acquire with a different voltage
    ldo_chan_config.voltage_mv = 3300;
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_ldo_acquire_channel(&ldo_chan_config, &fail_ldo_chan));
    // the channel has been acquired as "not adjustable" before, so we can't acquire it as "adjustable" again
    ldo_chan_config = (esp_ldo_channel_config_t) {
        .chan_id = 4,
        .voltage_mv = 1800,
        .flags.adjustable = true,
    };
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_ldo_acquire_channel(&ldo_chan_config, &fail_ldo_chan));

    // can't change the voltage for a non-adjustable channel
    TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, esp_ldo_channel_adjust_voltage(success_ldo_chans[0], 3300));

    for (int i = 0; i < 3; i++) {
        TEST_ESP_OK(esp_ldo_release_channel(success_ldo_chans[i]));
    }
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_ldo_release_channel(success_ldo_chans[0]));
}

TEST_CASE("LDO channel acquire and release (adjustable)", "[LDO]")
{
    esp_ldo_channel_handle_t success_ldo_chan = NULL;
    esp_ldo_channel_handle_t fail_ldo_chan = NULL;
    esp_ldo_channel_config_t ldo_chan_config = {
        .chan_id = 4,
        .voltage_mv = 1800,
        .flags.adjustable = true,
    };
    TEST_ESP_OK(esp_ldo_acquire_channel(&ldo_chan_config, &success_ldo_chan));

    // can't acquire multiple handles for the same adjustable channel
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_ldo_acquire_channel(&ldo_chan_config, &fail_ldo_chan));
    // even we acquire another copy as non-adjustable, it's still not allowed
    ldo_chan_config.flags.adjustable = false;
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_ldo_acquire_channel(&ldo_chan_config, &fail_ldo_chan));

    // can change voltage for an adjustable channel
    TEST_ESP_OK(esp_ldo_channel_adjust_voltage(success_ldo_chan, 3300));
    TEST_ESP_OK(esp_ldo_release_channel(success_ldo_chan));
}

TEST_CASE("LDO channel state dump", "[LDO][manual][ignore]")
{
    esp_ldo_channel_handle_t success_ldo_chans[3] = {};
    esp_ldo_channel_config_t ldo_chan_config = {
        .chan_id = 2,
        .voltage_mv = 1800,
    };
    TEST_ESP_OK(esp_ldo_acquire_channel(&ldo_chan_config, &success_ldo_chans[0]));

    ldo_chan_config.chan_id = 3;
    ldo_chan_config.voltage_mv = 2500;
    TEST_ESP_OK(esp_ldo_acquire_channel(&ldo_chan_config, &success_ldo_chans[1]));

    ldo_chan_config.chan_id  = 4;
    ldo_chan_config.voltage_mv = 1100;
    TEST_ESP_OK(esp_ldo_acquire_channel(&ldo_chan_config, &success_ldo_chans[2]));

    esp_ldo_dump(stdout);

    while (1) {
        vTaskDelay(1);
    }
}

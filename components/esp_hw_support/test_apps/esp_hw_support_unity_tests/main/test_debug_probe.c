/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "soc/soc_caps.h"
#include "hal/debug_probe_ll.h"
#include "esp_private/debug_probe.h"

TEST_CASE("debug probe install & uninstall", "[debug_probe]")
{
    debug_probe_unit_config_t unit_config = {
        .probe_out_gpio_nums = {0},
    };
    debug_probe_unit_handle_t probe_units[SOC_DEBUG_PROBE_NUM_UNIT] = {0};
    printf("install debug probe exhaustively\r\n");
    for (int i = 0; i < SOC_DEBUG_PROBE_NUM_UNIT; i++) {
        TEST_ESP_OK(debug_probe_new_unit(&unit_config, &probe_units[i]));
    }
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, debug_probe_new_unit(&unit_config, &probe_units[0]));

    debug_probe_channel_config_t chan_config = {
        .target_module = 1,
    };
    debug_probe_channel_handle_t probe_chans[SOC_DEBUG_PROBE_NUM_UNIT][DEBUG_PROBE_LL_CHANNELS_PER_UNIT] = {0};
    for (int i = 0; i < SOC_DEBUG_PROBE_NUM_UNIT; i++) {
        for (int j = 0; j < DEBUG_PROBE_LL_CHANNELS_PER_UNIT; j++) {
            TEST_ESP_OK(debug_probe_new_channel(probe_units[i], &chan_config, &probe_chans[i][j]));
        }
        TEST_ESP_ERR(ESP_ERR_NOT_FOUND, debug_probe_new_channel(probe_units[i], &chan_config, &probe_chans[i][0]));
    }

    for (int i = 0; i < SOC_DEBUG_PROBE_NUM_UNIT; i++) {
        for (int j = 0; j < DEBUG_PROBE_LL_CHANNELS_PER_UNIT; j++) {
            TEST_ESP_OK(debug_probe_del_channel(probe_chans[i][j]));
        }
        TEST_ESP_OK(debug_probe_del_unit(probe_units[i]));
    }
}

TEST_CASE("debug probe read", "[debug_probe]")
{
    debug_probe_unit_config_t unit_config = {
        .probe_out_gpio_nums = {
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
        }
    };
    debug_probe_unit_handle_t probe_unit = NULL;
    TEST_ESP_OK(debug_probe_new_unit(&unit_config, &probe_unit));

    // allocate two channels from the unit, each channel monitor different target module
    debug_probe_channel_handle_t probe_chans[2] = {0};
    for (int i = 0; i < 2; i++) {
        debug_probe_channel_config_t chan_config = {
            .target_module = i + 1,
        };
        TEST_ESP_OK(debug_probe_new_channel(probe_unit, &chan_config, &probe_chans[i]));

        // group15[7:0] contains the debug target module ID
        TEST_ESP_OK(debug_probe_chan_add_signal_by_byte(probe_chans[i], 0, 15));
        TEST_ESP_OK(debug_probe_chan_add_signal_by_byte(probe_chans[i], 1, 15));
        TEST_ESP_OK(debug_probe_chan_add_signal_by_byte(probe_chans[i], 2, 15));
        TEST_ESP_OK(debug_probe_chan_add_signal_by_byte(probe_chans[i], 3, 15));
    }

    TEST_ESP_OK(debug_probe_unit_merge16(probe_unit,
                                         probe_chans[0], DEBUG_PROBE_SPLIT_LOWER16,
                                         probe_chans[1], DEBUG_PROBE_SPLIT_LOWER16));
    uint32_t probe_result = 0;
    TEST_ESP_OK(debug_probe_unit_read(probe_unit, &probe_result));
    printf("probe result: 0x%"PRIx32"\r\n", probe_result);
    TEST_ASSERT_EQUAL_HEX32(0x20001, probe_result);

    // free resources
    for (int i = 0; i < 2; i++) {
        TEST_ESP_OK(debug_probe_del_channel(probe_chans[i]));
    }
    TEST_ESP_OK(debug_probe_del_unit(probe_unit));
}

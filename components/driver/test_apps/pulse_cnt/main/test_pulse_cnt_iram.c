/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "driver/pulse_cnt.h"
#include "driver/gpio.h"
#include "esp_spi_flash.h"
#include "esp_attr.h"
#include "soc/soc_caps.h"
#include "test_pulse_cnt_board.h"

#if CONFIG_PCNT_ISR_IRAM_SAFE

static bool IRAM_ATTR test_pcnt_iram_safe_callback(pcnt_unit_handle_t unit, pcnt_watch_event_data_t *event_data, void *user_data)
{
    uint32_t *data = (uint32_t *)user_data;
    (*data)++;
    // PCNT control function can still work in ISR
    pcnt_unit_stop(unit);
    return false;
}

static void IRAM_ATTR test_pcnt_iram_simulation(int gpio_sig)
{
    // disable flash cache
    spi_flash_guard_get()->start();
    test_gpio_simulate_rising_edge(gpio_sig, 2);
    // enable flash cache
    spi_flash_guard_get()->end();
}

TEST_CASE("pcnt_iram_interrupt_safe", "[pcnt]")
{
    pcnt_unit_config_t unit_config = {
        .low_limit = -100,
        .high_limit = 100
    };

    printf("install pcnt unit\r\n");
    pcnt_unit_handle_t unit = NULL;
    TEST_ESP_OK(pcnt_new_unit(&unit_config, &unit));

    printf("add watch point and event callback\r\n");
    TEST_ESP_OK(pcnt_unit_add_watch_point(unit, 2));
    pcnt_event_callbacks_t cbs = {
        .on_reach = test_pcnt_iram_safe_callback,
    };
    uint32_t num_of_event_triggered = 0;
    TEST_ESP_OK(pcnt_unit_register_event_callbacks(unit, &cbs, &num_of_event_triggered));

    printf("install pcnt channels\r\n");
    pcnt_chan_config_t channel_config = {
        .edge_gpio_num = TEST_PCNT_GPIO_A,
        .level_gpio_num = -1,
        .flags.io_loop_back = true,
    };
    pcnt_channel_handle_t channelA = NULL;
    pcnt_channel_handle_t channelB = NULL;
    TEST_ESP_OK(pcnt_new_channel(unit, &channel_config, &channelA));
    TEST_ESP_OK(pcnt_new_channel(unit, &channel_config, &channelB));

    printf("enable unit\r\n");
    TEST_ESP_OK(pcnt_unit_enable(unit));

    printf("Set pcnt actions for channels\r\n");
    // both channels increase on pulse edge
    TEST_ESP_OK(pcnt_channel_set_edge_action(channelA, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    TEST_ESP_OK(pcnt_channel_set_level_action(channelA, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP));
    TEST_ESP_OK(pcnt_channel_set_edge_action(channelA, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    TEST_ESP_OK(pcnt_channel_set_level_action(channelB, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP));

    printf("start unit\r\n");
    TEST_ESP_OK(pcnt_unit_start(unit));

    printf("disable cache and check interrupt triggered\r\n");
    TEST_ESP_OK(pcnt_unit_clear_count(unit));
    // the function that will disable the flash must be placed in the IRAM
    test_pcnt_iram_simulation(TEST_PCNT_GPIO_A);
    // check if the interrupt has fired up
    TEST_ASSERT_EQUAL(1, num_of_event_triggered);

    printf("check current count value\r\n");
    int cur_count = 0;
    TEST_ESP_OK(pcnt_unit_get_count(unit, &cur_count));
    // when the watch point got triggered, we disabled the PCNT unit in thr `test_pcnt_iram_safe_callback()`
    // so the finally count value should equal to the watch point value
    TEST_ASSERT_EQUAL(2, cur_count);

    printf("delete channels and unit\r\n");
    TEST_ESP_OK(pcnt_unit_disable(unit));
    TEST_ESP_OK(pcnt_del_channel(channelA));
    TEST_ESP_OK(pcnt_del_channel(channelB));
    TEST_ESP_OK(pcnt_del_unit(unit));
}

#endif // CONFIG_PCNT_ISR_IRAM_SAFE

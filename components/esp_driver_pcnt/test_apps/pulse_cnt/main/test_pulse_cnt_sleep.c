/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "unity_test_utils.h"
#include "driver/pulse_cnt.h"
#include "driver/gpio.h"
#include "spi_flash_mmap.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "soc/soc_caps.h"
#include "soc/pcnt_struct.h"
#include "hal/pcnt_ll.h"
#include "test_pulse_cnt_board.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/esp_pmu.h"

/**
 * @brief Test the PCNT driver can still work after light sleep
 */
static void test_pcnt_sleep_retention(void)
{
    test_gpio_init_for_simulation(TEST_PCNT_GPIO_A);
    test_gpio_init_for_simulation(TEST_PCNT_GPIO_B);

    printf("install pcnt units\r\n");
    pcnt_unit_config_t unit_config = {
        .low_limit = -100,
        .high_limit = 100,
    };
    pcnt_unit_handle_t units[2];
    for (int i = 0; i < 2; i++) {
        TEST_ESP_OK(pcnt_new_unit(&unit_config, &units[i]));
    }

    printf("install pcnt channels\r\n");
    const int channel_gpios[] = {TEST_PCNT_GPIO_A, TEST_PCNT_GPIO_B};
    pcnt_chan_config_t chan_config = {
        .level_gpio_num = -1,
    };
    pcnt_channel_handle_t chans[2];
    for (int i = 0; i < 2; i++) {
        chan_config.edge_gpio_num = channel_gpios[i];
        TEST_ESP_OK(pcnt_new_channel(units[i], &chan_config, &chans[i]));
        TEST_ESP_OK(pcnt_channel_set_edge_action(chans[i], PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
        TEST_ESP_OK(pcnt_channel_set_level_action(chans[i], PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP));
    }

    printf("enable and start unit\r\n");
    for (int i = 0; i < 2; i++) {
        TEST_ESP_OK(pcnt_unit_enable(units[i]));
        TEST_ESP_OK(pcnt_unit_start(units[i]));
    }

    // trigger 10 rising edge on GPIO
    test_gpio_simulate_rising_edge(TEST_PCNT_GPIO_A, 10);
    test_gpio_simulate_rising_edge(TEST_PCNT_GPIO_B, 10);

    int count_value = 0;
    for (int i = 0; i < 2; i++) {
        TEST_ESP_OK(pcnt_unit_get_count(units[i], &count_value));
        TEST_ASSERT_EQUAL(10, count_value);
    }

    // hold GPIO to avoid trigger PCNT counter during GPIO power down
    gpio_hold_en(TEST_PCNT_GPIO_A);
    gpio_hold_en(TEST_PCNT_GPIO_B);

    printf("stop and disable unit before sleep\r\n");
    for (int i = 0; i < 2; i++) {
        TEST_ESP_OK(pcnt_unit_stop(units[i]));
        TEST_ESP_OK(pcnt_unit_disable(units[i]));
    }

    // go to sleep
    esp_sleep_context_t sleep_ctx;
    esp_sleep_set_sleep_context(&sleep_ctx);
    printf("go to light sleep for 1 seconds\r\n");
#if ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(true));
#endif
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(1 * 1000 * 1000));
    TEST_ESP_OK(esp_light_sleep_start());

    printf("Waked up! Let's see if PCNT driver can still work...\r\n");
#if ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(false));
#endif

    printf("check if the sleep happened as expected\r\n");
    TEST_ASSERT_EQUAL(0, sleep_ctx.sleep_request_result);
#if SOC_PCNT_SUPPORT_SLEEP_RETENTION
    // check if the power domain also is powered down
    TEST_ASSERT_EQUAL(0, (sleep_ctx.sleep_flags) & PMU_SLEEP_PD_TOP);
#endif
    esp_sleep_set_sleep_context(NULL);

    gpio_hold_dis(TEST_PCNT_GPIO_A);
    gpio_hold_dis(TEST_PCNT_GPIO_B);

    printf("enable and start unit after sleep\r\n");
    for (int i = 0; i < 2; i++) {
        TEST_ESP_OK(pcnt_unit_enable(units[i]));
        TEST_ESP_OK(pcnt_unit_start(units[i]));
    }

    // Verify the counter still holds the value
    test_gpio_simulate_rising_edge(TEST_PCNT_GPIO_A, 10);
    test_gpio_simulate_rising_edge(TEST_PCNT_GPIO_B, 10);

    int reg_value = 0;
    for (int i = 0; i < 2; i++) {
        // check the counter value (include accum value and register value)
        TEST_ESP_OK(pcnt_unit_get_count(units[i], &count_value));
        TEST_ASSERT_EQUAL(20, count_value);
        // check the register value
        reg_value = pcnt_ll_get_count(&PCNT, i);
        TEST_ASSERT_EQUAL(20, reg_value);
    }

    for (int i = 0; i < 2; i++) {
        TEST_ESP_OK(pcnt_unit_stop(units[i]));
        TEST_ESP_OK(pcnt_unit_disable(units[i]));
        TEST_ESP_OK(pcnt_del_channel(chans[i]));
        TEST_ESP_OK(pcnt_del_unit(units[i]));
    }
}

TEST_CASE("pcnt light sleep", "[pcnt]")
{
    test_pcnt_sleep_retention();
}

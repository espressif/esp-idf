/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "driver/sdm.h"
#include "soc/soc_caps.h"
#include "esp_sleep.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/esp_pmu.h"

/**
 * @brief Test the SDM driver can still work after light sleep
 *
 * @param allow_pd Whether to allow power down the peripheral in light sleep
 */
static void test_sdm_sleep_retention(bool allow_pd)
{
    const gpio_num_t sdm_chan_gpios[2] = {GPIO_NUM_0, GPIO_NUM_2};
    sdm_config_t sdm_config = {
        .gpio_num = GPIO_NUM_NC, // will be set later
        .clk_src = SDM_CLK_SRC_DEFAULT,
        .sample_rate_hz = 1000000,
        .flags = {
            .invert_out = false,
            .allow_pd = allow_pd,
        },
    };
    sdm_channel_handle_t chans[2] = {0};
    for (int i = 0; i < 2; i++) {
        sdm_config.gpio_num = sdm_chan_gpios[i];
        TEST_ESP_OK(sdm_new_channel(&sdm_config, &chans[i]));
        // should see a ~250KHz (sample_rate/4) square wave
        TEST_ESP_OK(sdm_channel_set_pulse_density(chans[i], 0));
        TEST_ESP_OK(sdm_channel_enable(chans[i]));
    }
    vTaskDelay(pdMS_TO_TICKS(500));

    // Note: disable the sdm before going to sleep, ensure no power management lock is acquired by it
    for (int i = 0; i < 2; i++) {
        TEST_ESP_OK(sdm_channel_disable(chans[i]));
    }

    esp_sleep_context_t sleep_ctx;
    esp_sleep_set_sleep_context(&sleep_ctx);
    printf("go to light sleep for 2 seconds\r\n");
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(true));
#endif
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(2 * 1000 * 1000));
    TEST_ESP_OK(esp_light_sleep_start());

    printf("Waked up! Let's see if SDM driver can still work...\r\n");
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(false));
#endif

    printf("check if the sleep happened as expected\r\n");
    TEST_ASSERT_EQUAL(0, sleep_ctx.sleep_request_result);
#if SOC_SDM_SUPPORT_SLEEP_RETENTION
    // check if the power domain also is powered down
    TEST_ASSERT_EQUAL(allow_pd ? PMU_SLEEP_PD_TOP : 0, (sleep_ctx.sleep_flags) & PMU_SLEEP_PD_TOP);
#endif
    esp_sleep_set_sleep_context(NULL);

    // re-enable the SDM channel
    for (int i = 0; i < 2; i++) {
        TEST_ESP_OK(sdm_channel_enable(chans[i]));
    }
    // should see a ~250KHz (sample_rate/4) square wave again
    vTaskDelay(pdMS_TO_TICKS(500));

    for (int i = 0; i < 2; i++) {
        TEST_ESP_OK(sdm_channel_disable(chans[i]));
        TEST_ESP_OK(sdm_del_channel(chans[i]));
    }
}

TEST_CASE("sdm can work after light sleep", "[sdm]")
{
    test_sdm_sleep_retention(false);
#if SOC_SDM_SUPPORT_SLEEP_RETENTION
    test_sdm_sleep_retention(true);
#endif
}

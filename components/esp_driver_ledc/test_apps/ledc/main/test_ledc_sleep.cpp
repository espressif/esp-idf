/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "test_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "test_ledc_utils.h"
#include "esp_sleep.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/esp_pmu.h"
#include "hal/ledc_periph.h"
#include "esp_private/sleep_retention.h"
#include "esp_rom_serial_output.h"

// Note. Test cases in this file cannot run one after another without reset

/**
 * @brief Test LEDC can still output PWM signal after light sleep
 *
 * @param allow_pd Whether to allow powering down the peripheral in light sleep
 */
static void test_ledc_sleep_retention(bool allow_pd)
{
    int pulse_count __attribute__((unused)) = 0;

    ledc_timer_config_t ledc_time_config = create_default_timer_config();
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));

    ledc_channel_config_t ledc_ch_config = initialize_channel_config();
    ledc_ch_config.sleep_mode = (allow_pd ? LEDC_SLEEP_MODE_NO_ALIVE_ALLOW_PD : LEDC_SLEEP_MODE_NO_ALIVE_NO_PD);
    TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));

    vTaskDelay(50 / portTICK_PERIOD_MS);

    pulse_count = wave_count(200);
    TEST_ASSERT_UINT32_WITHIN(5, TEST_PWM_FREQ * 200 / 1000, pulse_count);

    esp_sleep_context_t sleep_ctx;
    esp_sleep_set_sleep_context(&sleep_ctx);

#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(true));
#endif
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(2 * 1000 * 1000));

    printf("Go to light sleep for 2 seconds\n");
    TEST_ESP_OK(esp_light_sleep_start());
    printf("Waked up! Let's see if LEDC peripheral can still work...\n");

#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(false));
#endif

    printf("Check if the sleep happened as expected\r\n");
    TEST_ASSERT_EQUAL(0, sleep_ctx.sleep_request_result);
#if SOC_PMU_SUPPORTED
    // check if the TOP power domain on/off as desired
    TEST_ASSERT_EQUAL(allow_pd ? PMU_SLEEP_PD_TOP : 0, (sleep_ctx.sleep_flags) & PMU_SLEEP_PD_TOP);
#endif
    esp_sleep_set_sleep_context(NULL);

    if (allow_pd) {
        // check if the RO duty_r register field get synced back
        TEST_ASSERT_EQUAL(4000, ledc_get_duty(TEST_SPEED_MODE, LEDC_CHANNEL_0));
    }

    pulse_count = wave_count(200);
    TEST_ASSERT_UINT32_WITHIN(5, TEST_PWM_FREQ * 200 / 1000, pulse_count);
}

TEST_CASE("ledc can output after light sleep (LEDC power domain xpd)", "[ledc]")
{
    test_ledc_sleep_retention(false);
}

#if SOC_LEDC_SUPPORT_SLEEP_RETENTION && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
TEST_CASE("ledc can output after light sleep (LEDC power domain pd)", "[ledc]")
{
    // test retention feature
    test_ledc_sleep_retention(true);

    // ledc driver does not have channel release, we will do retention release here to avoid memory leak
    sleep_retention_module_t module = ledc_reg_retention_info.module_id;
    sleep_retention_module_free(module);
    sleep_retention_module_deinit(module);
}
#endif

static const ledc_clk_cfg_t test_ledc_clk_in_slp[] = {
    LEDC_USE_RC_FAST_CLK,
#if SOC_LEDC_SUPPORT_XTAL_CLOCK
    LEDC_USE_XTAL_CLK,
#endif
};

static const int test_clks_num = sizeof(test_ledc_clk_in_slp) / sizeof(test_ledc_clk_in_slp[0]);

static void ledc_output_monitor(void)
{
    for (int i = 0; i < test_clks_num; i++) {
        unity_wait_for_signal("Go to light sleep for 3 seconds");
        vTaskDelay(500 / portTICK_PERIOD_MS);
        int pulse_count = wave_count(200);
        uint32_t acceptable_delta = (test_ledc_clk_in_slp[i] == LEDC_USE_RC_FAST_CLK) ? 20 : 5; // RC_FAST as the clk src has a bigger error range is reasonable
        TEST_ASSERT_UINT32_WITHIN(acceptable_delta, TEST_PWM_LOW_FREQ * 200 / 1000, pulse_count);
        unity_wait_for_signal("Waked up!");
    }
}

static void ledc_output_in_sleep(void)
{
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(3 * 1000 * 1000));

    ledc_channel_config_t ledc_ch_config = initialize_channel_config();
    ledc_ch_config.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_ch_config.sleep_mode = LEDC_SLEEP_MODE_KEEP_ALIVE;
    TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));

    for (int i = 0; i < test_clks_num; i++) {
        ledc_timer_config_t ledc_time_config = create_default_timer_config();
        ledc_time_config.speed_mode = LEDC_LOW_SPEED_MODE;
        ledc_time_config.clk_cfg = test_ledc_clk_in_slp[i];
        ledc_time_config.freq_hz = TEST_PWM_LOW_FREQ;
        TEST_ESP_OK(ledc_timer_config(&ledc_time_config));

        TEST_ESP_OK(ledc_update_duty(ledc_ch_config.speed_mode, ledc_ch_config.channel));

        unity_send_signal("Go to light sleep for 3 seconds");
        esp_rom_output_tx_wait_idle(CONFIG_ESP_CONSOLE_ROM_SERIAL_PORT_NUM); // wait until the signal sent
        TEST_ESP_OK(esp_light_sleep_start());
        unity_send_signal("Waked up!");

        TEST_ESP_OK(ledc_timer_pause(ledc_time_config.speed_mode, ledc_time_config.timer_num));
        ledc_time_config.deconfigure = 1;
        TEST_ESP_OK(ledc_timer_config(&ledc_time_config));
    }
}

TEST_CASE_MULTIPLE_DEVICES("ledc can output during light sleep", "[ledc][test_env=generic_multi_device]", ledc_output_in_sleep, ledc_output_monitor);

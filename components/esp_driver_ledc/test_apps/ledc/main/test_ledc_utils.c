/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "test_ledc_utils.h"
#include "soc/soc_caps.h"

ledc_channel_config_t initialize_channel_config(void)
{
    ledc_channel_config_t config;
    memset(&config, 0, sizeof(ledc_channel_config_t));
    config.gpio_num = PULSE_IO;
    config.speed_mode = TEST_SPEED_MODE;
    config.channel  = LEDC_CHANNEL_0;
    config.intr_type = LEDC_INTR_DISABLE;
    config.timer_sel = LEDC_TIMER_0;
    config.duty = 4000;
    config.hpoint = 0;
    return config;
}

ledc_timer_config_t create_default_timer_config(void)
{
    ledc_timer_config_t ledc_time_config;
    memset(&ledc_time_config, 0, sizeof(ledc_timer_config_t));
    ledc_time_config.speed_mode = TEST_SPEED_MODE;
    ledc_time_config.duty_resolution = LEDC_TIMER_13_BIT;
    ledc_time_config.timer_num = LEDC_TIMER_0;
    ledc_time_config.freq_hz = TEST_PWM_FREQ;
    ledc_time_config.clk_cfg = TEST_DEFAULT_CLK_CFG;
    return ledc_time_config;
}

// use PCNT to test the waveform of LEDC
#if SOC_PCNT_SUPPORTED
#include "driver/pulse_cnt.h"

#define HIGHEST_LIMIT 10000
#define LOWEST_LIMIT -10000

static pcnt_unit_handle_t pcnt_unit;
static pcnt_channel_handle_t pcnt_chan;

void setup_testbench(void)
{
    pcnt_unit_config_t unit_config = {
        .high_limit = HIGHEST_LIMIT,
        .low_limit = LOWEST_LIMIT,
    };
    TEST_ESP_OK(pcnt_new_unit(&unit_config, &pcnt_unit));
    pcnt_chan_config_t chan_config = {
        .edge_gpio_num = PULSE_IO,
        .level_gpio_num = -1,
    };
    TEST_ESP_OK(pcnt_new_channel(pcnt_unit, &chan_config, &pcnt_chan));
    TEST_ESP_OK(pcnt_channel_set_level_action(pcnt_chan, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP));
    TEST_ESP_OK(pcnt_channel_set_edge_action(pcnt_chan, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    TEST_ESP_OK(pcnt_unit_enable(pcnt_unit));
}

void tear_testbench(void)
{
    TEST_ESP_OK(pcnt_unit_disable(pcnt_unit));
    TEST_ESP_OK(pcnt_del_channel(pcnt_chan));
    TEST_ESP_OK(pcnt_del_unit(pcnt_unit));
}

int wave_count(int last_time)
{
    int test_counter = 0;
    TEST_ESP_OK(pcnt_unit_clear_count(pcnt_unit));
    TEST_ESP_OK(pcnt_unit_start(pcnt_unit));
    vTaskDelay(pdMS_TO_TICKS(last_time));
    TEST_ESP_OK(pcnt_unit_stop(pcnt_unit));
    TEST_ESP_OK(pcnt_unit_get_count(pcnt_unit, &test_counter));
    return test_counter;
}
#endif

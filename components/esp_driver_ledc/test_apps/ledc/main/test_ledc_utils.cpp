/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "test_ledc_utils.h"
#include "driver/uart.h"

ledc_channel_config_t initialize_channel_config(void)
{
    ledc_channel_config_t config = {};
    config.gpio_num = PULSE_IO;
    config.speed_mode = TEST_SPEED_MODE;
    config.channel  = LEDC_CHANNEL_0;
    config.timer_sel = LEDC_TIMER_0;
    config.duty = 4000;
    config.hpoint = 0;
    return config;
}

ledc_timer_config_t create_default_timer_config(void)
{
    ledc_timer_config_t ledc_time_config = {};
    ledc_time_config.speed_mode = TEST_SPEED_MODE;
    ledc_time_config.duty_resolution = LEDC_TIMER_13_BIT;
    ledc_time_config.timer_num = LEDC_TIMER_0;
    ledc_time_config.freq_hz = TEST_PWM_FREQ;
    ledc_time_config.clk_cfg = TEST_DEFAULT_CLK_CFG;
    return ledc_time_config;
}

// use UART auto baud rate detection feature to test the waveform of LEDC
int wave_count(int last_time)
{
    uart_bitrate_detect_config_t conf = {};
    conf.rx_io_num = PULSE_IO;
    conf.source_clk = UART_SCLK_DEFAULT;
    uart_bitrate_res_t res = {};
    uart_detect_bitrate_start(UART_NUM_1, &conf);
    vTaskDelay(pdMS_TO_TICKS(last_time));
    uart_detect_bitrate_stop(UART_NUM_1, true, &res);
    return (res.edge_cnt + 1) / 2; // edge count -> pulse count, round up
}

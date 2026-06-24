/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "test_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "test_ledc_utils.h"
#include "esp_etm.h"
#include "esp_attr.h"
#include "driver/uart.h"

TEST_CASE("ledc etm event and task", "[ledc]")
{
    // Generate a PWM signal with certain number of pulses

    ledc_timer_config_t ledc_time_config = create_default_timer_config();
    TEST_ESP_OK(ledc_timer_config(&ledc_time_config));

    ledc_channel_config_t ledc_ch_config = initialize_channel_config();
    TEST_ESP_OK(ledc_channel_config(&ledc_ch_config));

    vTaskDelay(pdMS_TO_TICKS(50));

    ledc_timer_pause(TEST_SPEED_MODE, LEDC_TIMER_0);
    TEST_ESP_OK(ledc_channel_configure_maximum_timer_ovf_cnt(TEST_SPEED_MODE, LEDC_CHANNEL_0, 500));

    esp_etm_channel_config_t etm_config = {};
    esp_etm_channel_handle_t etm_channel_a = NULL;
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &etm_channel_a));

    esp_etm_event_handle_t event_handle = NULL;
    ledc_channel_etm_event_config_t event_config = {
        .event_type = LEDC_CHANNEL_ETM_EVENT_REACH_MAX_OVF_CNT,
    };
    TEST_ESP_OK(ledc_channel_new_etm_event(TEST_SPEED_MODE, LEDC_CHANNEL_0, &event_config, &event_handle));

    esp_etm_task_handle_t task_handle_a = NULL;
    ledc_timer_etm_task_config_t task_config_a = {
        .task_type = LEDC_TIMER_ETM_TASK_PAUSE,
    };
    TEST_ESP_OK(ledc_timer_new_etm_task(TEST_SPEED_MODE, LEDC_TIMER_0, &task_config_a, &task_handle_a));

    TEST_ESP_OK(esp_etm_channel_connect(etm_channel_a, event_handle, task_handle_a));

    TEST_ESP_OK(esp_etm_channel_enable(etm_channel_a));

    // use UART auto baud rate detection feature to count the PWM pulse count
    uart_bitrate_detect_config_t conf = {};
    conf.rx_io_num = PULSE_IO;
    conf.source_clk = UART_SCLK_DEFAULT;
    uart_bitrate_res_t res = {};

    uart_detect_bitrate_start(UART_NUM_1, &conf);
    TEST_ESP_OK(ledc_timer_rst(TEST_SPEED_MODE, LEDC_TIMER_0));
    TEST_ESP_OK(ledc_timer_resume(TEST_SPEED_MODE, LEDC_TIMER_0));
    vTaskDelay(pdMS_TO_TICKS(500));
    uart_detect_bitrate_stop(UART_NUM_1, true, &res);
    uint32_t pulse_count = (res.edge_cnt + 1) / 2;
    TEST_ASSERT_INT32_WITHIN(1, 500, pulse_count);

    TEST_ESP_OK(esp_etm_channel_disable(etm_channel_a));

    TEST_ESP_OK(esp_etm_del_event(event_handle));
    TEST_ESP_OK(esp_etm_del_task(task_handle_a));
    TEST_ESP_OK(esp_etm_del_channel(etm_channel_a));
}

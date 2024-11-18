/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "unity.h"
#include "unity_test_utils.h"
#include "freertos/FreeRTOS.h"
#include "esp_attr.h"
#include "esp_etm.h"
#include "esp_systick_etm.h"
#include "driver/gpio_etm.h"
#include "driver/gpio.h"

TEST_CASE("rtos_systick_etm_event", "[etm]")
{
    // systimer alarm  ---> EMT channel ---> GPIO toggle
    const uint32_t output_gpio = 1;
    printf("allocate etm channels\r\n");
    esp_etm_channel_config_t etm_config = {};
    esp_etm_channel_handle_t etm_channel_a = NULL;
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &etm_channel_a));

    printf("allocate GPIO etm task\r\n");
    esp_etm_task_handle_t gpio_task = NULL;
    gpio_etm_task_config_t gpio_task_config = {
        .action = GPIO_ETM_TASK_ACTION_TOG,
    };
    TEST_ESP_OK(gpio_new_etm_task(&gpio_task_config, &gpio_task));

    // bind GPIO to the task
    TEST_ESP_OK(gpio_etm_task_add_gpio(gpio_task, output_gpio));

    printf("initialize gpio\r\n");
    gpio_config_t task_gpio_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << output_gpio,
    };
    TEST_ESP_OK(gpio_config(&task_gpio_config));

    printf("acquire systick etm event\r\n");
    esp_etm_event_handle_t systick_event = NULL;
    TEST_ESP_OK(esp_systick_new_etm_alarm_event(0, &systick_event));

    printf("connect event and task to the channel\r\n");
    TEST_ESP_OK(esp_etm_channel_connect(etm_channel_a, systick_event, gpio_task));

    TEST_ESP_OK(esp_etm_channel_enable(etm_channel_a));

    // should see a 50Hz square wave on the GPIO (if RTOS systick is set to 100Hz)
    vTaskDelay(pdMS_TO_TICKS(1000));

    // delete etm primitives
    TEST_ESP_OK(gpio_etm_task_rm_gpio(gpio_task, output_gpio));
    TEST_ESP_OK(esp_etm_del_task(gpio_task));
    TEST_ESP_OK(esp_etm_del_event(systick_event));
    TEST_ESP_OK(esp_etm_channel_disable(etm_channel_a));
    TEST_ESP_OK(esp_etm_del_channel(etm_channel_a));
}

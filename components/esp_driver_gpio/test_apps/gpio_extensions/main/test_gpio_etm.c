/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "unity.h"
#include "unity_test_utils.h"
#include "esp_attr.h"
#include "driver/gpio_etm.h"
#include "driver/gpio.h"

TEST_CASE("gpio_etm_self_trigger", "[etm]")
{
    // GPIO any edge  ---> EMT channel ---> GPIO toggle
    const uint32_t input_gpio = 0;
    const uint32_t output_gpio = 1;
    printf("allocate etm channels\r\n");
    esp_etm_channel_config_t etm_config = {};
    esp_etm_channel_handle_t etm_channel_a = NULL;
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &etm_channel_a));

    printf("allocate GPIO etm event and task\r\n");
    esp_etm_task_handle_t gpio_task = NULL;
    esp_etm_event_handle_t gpio_event = NULL;
    gpio_etm_event_config_t gpio_event_config = {
        .edge = GPIO_ETM_EVENT_EDGE_ANY,
    };
    TEST_ESP_OK(gpio_new_etm_event(&gpio_event_config, &gpio_event));
    gpio_etm_task_config_t gpio_task_config = {
        .action = GPIO_ETM_TASK_ACTION_TOG,
    };
    TEST_ESP_OK(gpio_new_etm_task(&gpio_task_config, &gpio_task));

    // bind GPIO to the event and task
    TEST_ESP_OK(gpio_etm_event_bind_gpio(gpio_event, input_gpio));
    TEST_ESP_OK(gpio_etm_task_add_gpio(gpio_task, output_gpio));

    printf("initialize gpio\r\n");
    gpio_config_t task_gpio_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT, // we want to read the GPIO value, so it should be input and output
        .pin_bit_mask = 1ULL << output_gpio,
    };
    TEST_ESP_OK(gpio_config(&task_gpio_config));
    // set the initial level
    TEST_ESP_OK(gpio_set_level(output_gpio, 0));

    gpio_config_t event_gpio_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT, // we want to simulate the edge signal by software, so it should be input and output
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pin_bit_mask = 1ULL << input_gpio,
    };
    TEST_ESP_OK(gpio_config(&event_gpio_config));

    printf("connect event and task to the channel\r\n");
    TEST_ESP_OK(esp_etm_channel_connect(etm_channel_a, gpio_event, gpio_task));

    TEST_ESP_OK(esp_etm_channel_enable(etm_channel_a));

    for (int i = 0; i < 10; i++) {
        TEST_ESP_OK(gpio_set_level(input_gpio, i & 0x01));
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    // check the final level
    TEST_ASSERT_EQUAL(1, gpio_get_level(output_gpio));

    vTaskDelay(pdMS_TO_TICKS(100));

    for (int i = 0; i < 5; i++) {
        TEST_ESP_OK(gpio_set_level(input_gpio, i & 0x01));
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    // check the final level
    TEST_ASSERT_EQUAL(0, gpio_get_level(output_gpio));

    // delete gpio etm task without remove all bounded GPIOs should fail
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_etm_del_task(gpio_task));
    // remove unrelated GPIO from the task should fail
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, gpio_etm_task_rm_gpio(gpio_task, 10));

    // delete etm primitives
    TEST_ESP_OK(gpio_etm_task_rm_gpio(gpio_task, output_gpio));
    TEST_ESP_OK(esp_etm_channel_disable(etm_channel_a));
    TEST_ESP_OK(esp_etm_del_task(gpio_task));
    TEST_ESP_OK(esp_etm_del_event(gpio_event));
    TEST_ESP_OK(esp_etm_del_channel(etm_channel_a));
}

TEST_CASE("gpio_etm_self_trigger_multi_action", "[etm]")
{
    // GPIO 0 pos edge event ---> GPIO 1 set level task
    // GPIO 2 pos edge event ---> GPIO 1 clear level task

    const uint32_t input_gpio1 = 0;
    const uint32_t input_gpio2 = 2;
    const uint32_t output_gpio = 1;
    printf("allocate etm channels\r\n");
    esp_etm_channel_config_t etm_config = {};
    esp_etm_channel_handle_t etm_channel_a = NULL;
    esp_etm_channel_handle_t etm_channel_b = NULL;
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &etm_channel_a));
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &etm_channel_b));

    printf("allocate GPIO etm event and task\r\n");
    esp_etm_task_handle_t gpio_task_a = NULL;
    esp_etm_event_handle_t gpio_event_a = NULL;
    esp_etm_task_handle_t gpio_task_b = NULL;
    esp_etm_event_handle_t gpio_event_b = NULL;
    gpio_etm_event_config_t gpio_event_config = {};
    gpio_event_config.edges[0] = GPIO_ETM_EVENT_EDGE_POS;
    TEST_ESP_OK(gpio_new_etm_event(&gpio_event_config, &gpio_event_a));
    esp_etm_event_handle_t gpio_event_c = NULL; // an extra event only used for testing binding
    gpio_event_config.edges[1] = GPIO_ETM_EVENT_EDGE_ANY;
    TEST_ESP_OK(gpio_new_etm_event(&gpio_event_config, &gpio_event_b, &gpio_event_c));
    gpio_etm_task_config_t gpio_task_config = {};
    gpio_task_config.actions[0] = GPIO_ETM_TASK_ACTION_CLR;
    gpio_task_config.actions[1] = GPIO_ETM_TASK_ACTION_SET;
    TEST_ESP_OK(gpio_new_etm_task(&gpio_task_config, &gpio_task_b, &gpio_task_a));

    // bind GPIO to the event and task
    TEST_ESP_OK(gpio_etm_event_bind_gpio(gpio_event_a, input_gpio1));
    TEST_ESP_OK(gpio_etm_event_bind_gpio(gpio_event_b, input_gpio2));
    TEST_ESP_OK(gpio_etm_task_add_gpio(gpio_task_a, output_gpio));
    TEST_ESP_OK(gpio_etm_task_add_gpio(gpio_task_b, output_gpio));

    // try an infeasible bind of second event to a GPIO
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, gpio_etm_event_bind_gpio(gpio_event_c, input_gpio1));
    // try a feasible bind of second event to a GPIO
    TEST_ESP_OK(gpio_etm_event_bind_gpio(gpio_event_c, input_gpio2));
    // delete the event to unbind it from the GPIO
    TEST_ESP_OK(esp_etm_del_event(gpio_event_c));

    printf("initialize gpio\r\n");
    gpio_config_t task_gpio_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT, // we want to read the GPIO value, so it should be input and output
        .pin_bit_mask = 1ULL << output_gpio,
    };
    TEST_ESP_OK(gpio_config(&task_gpio_config));
    // set the initial level
    TEST_ESP_OK(gpio_set_level(output_gpio, 0));

    gpio_config_t event_gpio_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT, // we want to simulate the edge signal by software, so it should be input and output
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pin_bit_mask = (1ULL << input_gpio1) | (1ULL << input_gpio2),
    };
    TEST_ESP_OK(gpio_config(&event_gpio_config));
    // set the initial level
    TEST_ESP_OK(gpio_set_level(input_gpio1, 0));
    TEST_ESP_OK(gpio_set_level(input_gpio2, 0));

    printf("connect event and task to the channel\r\n");
    TEST_ESP_OK(esp_etm_channel_connect(etm_channel_a, gpio_event_a, gpio_task_a));
    TEST_ESP_OK(esp_etm_channel_connect(etm_channel_b, gpio_event_b, gpio_task_b));

    TEST_ESP_OK(esp_etm_channel_enable(etm_channel_a));
    TEST_ESP_OK(esp_etm_channel_enable(etm_channel_b));

    // input_gpio1 pos edge ---> output_gpio level being set
    TEST_ESP_OK(gpio_set_level(input_gpio1, 1));
    vTaskDelay(pdMS_TO_TICKS(100));
    TEST_ASSERT_EQUAL(1, gpio_get_level(output_gpio));

    // input_gpio1 neg edge does not affect output_gpio level
    TEST_ESP_OK(gpio_set_level(input_gpio1, 0));
    vTaskDelay(pdMS_TO_TICKS(100));
    TEST_ASSERT_EQUAL(1, gpio_get_level(output_gpio));

    // input_gpio2 pos edge ---> output_gpio level being cleared
    TEST_ESP_OK(gpio_set_level(input_gpio2, 1));
    vTaskDelay(pdMS_TO_TICKS(100));
    TEST_ASSERT_EQUAL(0, gpio_get_level(output_gpio));

    // input_gpio2 neg edge does not affect output_gpio level
    TEST_ESP_OK(gpio_set_level(input_gpio2, 0));
    vTaskDelay(pdMS_TO_TICKS(100));
    TEST_ASSERT_EQUAL(0, gpio_get_level(output_gpio));

    // Create a new gpio etm task separately, and add it to the output_gpio should fail (the task does not belong to the same GPIO ETM task channel as gpio_task_a and gpio_task_b)
    esp_etm_task_handle_t gpio_task_c = NULL;
    gpio_etm_task_config_t gpio_task_config_2 = {
        .action = GPIO_ETM_TASK_ACTION_TOG,
    };
    TEST_ESP_OK(gpio_new_etm_task(&gpio_task_config_2, &gpio_task_c));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, gpio_etm_task_add_gpio(gpio_task_c, output_gpio));

    // delete etm primitives
    TEST_ESP_OK(gpio_etm_task_rm_gpio(gpio_task_a, output_gpio));
    TEST_ESP_OK(gpio_etm_task_rm_gpio(gpio_task_b, output_gpio));
    TEST_ESP_OK(esp_etm_channel_disable(etm_channel_a));
    TEST_ESP_OK(esp_etm_channel_disable(etm_channel_b));
    TEST_ESP_OK(esp_etm_del_task(gpio_task_a));
    TEST_ESP_OK(esp_etm_del_task(gpio_task_b));
    TEST_ESP_OK(esp_etm_del_task(gpio_task_c));
    TEST_ESP_OK(esp_etm_del_event(gpio_event_a));
    TEST_ESP_OK(esp_etm_del_event(gpio_event_b));
    TEST_ESP_OK(esp_etm_del_channel(etm_channel_a));
    TEST_ESP_OK(esp_etm_del_channel(etm_channel_b));
}

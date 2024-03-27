/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
#include "driver/temperature_sensor_etm.h"
#include "driver/temperature_sensor.h"
#include "soc/soc_etm_struct.h"

// To run this example, you need a facility that can make the temperature change
// on board. Like a heat gun.
// Then after the temperature meet the threshold, you can see the gpio level changes
// from 0 to 1 on logic analyzer or oscilloscope.
TEST_CASE("temperature sensor alarm cause gpio pull up", "[etm]")
{
    const uint32_t output_gpio = 5;
    // temperature sensor alarm ---> ETM channel A ---> GPIO level to high
    printf("allocate etm channel\r\n");
    esp_etm_channel_config_t etm_config = {};
    esp_etm_channel_handle_t etm_channel_a;
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &etm_channel_a));

    printf("allocate GPIO etm task\r\n");
    esp_etm_task_handle_t gpio_task = NULL;
    gpio_etm_task_config_t gpio_task_config = {
        .action = GPIO_ETM_TASK_ACTION_SET,
    };
    TEST_ESP_OK(gpio_new_etm_task(&gpio_task_config, &gpio_task));
    // set gpio number for the gpio etm primitives
    TEST_ESP_OK(gpio_etm_task_add_gpio(gpio_task, output_gpio));

    printf("initialize gpio\r\n");
    gpio_set_level(output_gpio, 0);
    gpio_config_t task_gpio_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << output_gpio,
    };
    TEST_ESP_OK(gpio_config(&task_gpio_config));

    float tsens_out;
    temperature_sensor_config_t temp_sensor = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);
    temperature_sensor_handle_t temp_handle = NULL;
    TEST_ESP_OK(temperature_sensor_install(&temp_sensor, &temp_handle));

    temperature_sensor_abs_threshold_config_t threshold_cfg = {
        .high_threshold = 50,
        .low_threshold = -10,
    };
    TEST_ESP_OK(temperature_sensor_set_absolute_threshold(temp_handle, &threshold_cfg));
    TEST_ESP_OK(temperature_sensor_enable(temp_handle));
    printf("Temperature sensor started\n");

    temperature_sensor_etm_event_config_t tsens_etm_event = {
        .event_type = TEMPERATURE_SENSOR_EVENT_OVER_LIMIT,
    };

    esp_etm_event_handle_t tsens_evt;

    TEST_ESP_OK(temperature_sensor_new_etm_event(temp_handle, &tsens_etm_event, &tsens_evt));

    printf("connect event and task to the channel\r\n");
    TEST_ESP_OK(esp_etm_channel_connect(etm_channel_a, tsens_evt, gpio_task));

    printf("enable etm channel\r\n");
    TEST_ESP_OK(esp_etm_channel_enable(etm_channel_a));

    uint32_t cnt = 20;
    while (cnt--) {
        TEST_ESP_OK(temperature_sensor_get_celsius(temp_handle, &tsens_out));
        printf("Temperature out celsius %f°C\n", tsens_out);
        vTaskDelay(100);
    }

    TEST_ESP_OK(temperature_sensor_disable(temp_handle));
    TEST_ESP_OK(temperature_sensor_uninstall(temp_handle));
    // delete etm primitives
    TEST_ESP_OK(gpio_etm_task_rm_gpio(gpio_task, output_gpio));
    TEST_ESP_OK(esp_etm_del_task(gpio_task));
    TEST_ESP_OK(esp_etm_del_event(tsens_evt));
    TEST_ESP_OK(esp_etm_channel_disable(etm_channel_a));
    TEST_ESP_OK(esp_etm_del_channel(etm_channel_a));
}

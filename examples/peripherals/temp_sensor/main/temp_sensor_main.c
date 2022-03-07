/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/temperature_sensor.h"

static const char *TAG = "example";

void tempsensor_example(void)
{
    // Initialize touch pad peripheral, it will start a timer to run a filter
    ESP_LOGI(TAG, "Initializing Temperature sensor");
    float tsens_out;
    temperature_sensor_config_t temp_sensor = TEMPERAUTRE_SENSOR_CONFIG_DEFAULT(10, 50);
    temperature_sensor_handle_t temp_handle = NULL;
    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor, &temp_handle));
    ESP_ERROR_CHECK(temperature_sensor_start(temp_handle));
    ESP_LOGI(TAG, "Temperature sensor started");
    int cnt = 20; //read value for 20 times
    while (cnt) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP_ERROR_CHECK(temperature_sensor_get_celsius(temp_handle, &tsens_out));
        ESP_LOGI(TAG, "Temperature out celsius %.02f", tsens_out);
        cnt--;
    }
}

void app_main(void)
{
    tempsensor_example();
}

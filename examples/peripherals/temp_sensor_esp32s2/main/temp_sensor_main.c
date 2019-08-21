/* Temperature Sensor Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Note: ESP32 don't support temperature sensor */

#if CONFIG_IDF_TARGET_ESP32S2BETA
#include "temp_sensor.h"

static const char *TAG = "TempSensor";

void tempsensor_example(void *arg)
{
    // Initialize touch pad peripheral, it will start a timer to run a filter
    ESP_LOGI(TAG, "Initializing Temperature sensor");
    float tsens_out;
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
    temp_sensor_get_config(&temp_sensor);
    ESP_LOGI(TAG, "default dac %d, clk_div %d", temp_sensor.dac_offset, temp_sensor.clk_div);
    temp_sensor.dac_offset = TSENS_DAC_DEFAULT; // DEFAULT: range:-10℃ ~  80℃, error < 1℃.
    temp_sensor_set_config(temp_sensor);
    temp_sensor_start();
    ESP_LOGI(TAG, "Temperature sensor started");
    while (1) {
        vTaskDelay(1000 / portTICK_RATE_MS);
        temp_sensor_read_celsius(&tsens_out);
        ESP_LOGI(TAG, "Temperature out celsius %f°C", tsens_out);
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    xTaskCreate(tempsensor_example, "temp", 2048, NULL, 5, NULL);
}

#elif CONFIG_IDF_TARGET_ESP32

void app_main(void)
{
    printf("ESP32 don't support temperature sensor\n");
}

#endif

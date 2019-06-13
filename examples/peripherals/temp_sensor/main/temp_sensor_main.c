/* ADC1 Example

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
#include "driver/temp_sensor.h"

#if CONFIG_IDF_TARGET_ESP32S2BETA

static const char* TAG = "TempSensor";

void tempsensor_example(void *arg)
{
    // Initialize touch pad peripheral, it will start a timer to run a filter
    ESP_LOGI(TAG, "Initializing temp sensor");
    uint8_t temp_out;
    temp_sensor_t temp_sensor;
    temp_sensor_get_config(&temp_sensor);
    ESP_LOGI(TAG, "default dac %d, clk_div %d", temp_sensor.dac_offset, temp_sensor.clk_div);
    temp_sensor.dac_offset = TEMP_SENSOR_DAC_DEFAULT; // DEFAULT: range:-10℃ ~  80℃, error < 1℃.
    temp_sensor_set_config(temp_sensor);
    temp_sensor_start();
    ESP_LOGI(TAG, "temp sensor started");
    while(1) {
        vTaskDelay(1000 / portTICK_RATE_MS);
        temp_sensor_read(&temp_out);
        ESP_LOGI(TAG, "temp out %d", temp_out);
    }
    ESP_LOGI(TAG, "test over");
    vTaskDelete(NULL);
}

void app_main()
{
    xTaskCreate(tempsensor_example, "temp", 2048, NULL, 5, NULL);
}
#endif


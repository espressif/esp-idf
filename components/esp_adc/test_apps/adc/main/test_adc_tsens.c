/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "soc/adc_periph.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "driver/temperature_sensor.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "test_common_adc.h"
#include "test_utils.h"

#if SOC_TEMP_SENSOR_SUPPORTED && SOC_ADC_SUPPORTED

static const char *TAG = "adc_tsens";

#define EXAMPLE_ADC1_CHAN0          ADC_CHANNEL_2
#define EXAMPLE_ADC1_CHAN1          ADC_CHANNEL_4
#define EXAMPLE_ADC_ATTEN           ADC_ATTEN_DB_12

static int adc_raw[2][10];

TEST_CASE("Test temperature sensor cannot be influenced by ADC", "[adc]")
{
    ESP_LOGI(TAG, "Install temperature sensor, expected temp ranger range: 10~50 ℃");
    temperature_sensor_handle_t temp_sensor = NULL;
    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-10, 80);
    TEST_ESP_OK(temperature_sensor_install(&temp_sensor_config, &temp_sensor));
    int cnt = 2;
    float tsens_value;
    while (cnt--) {
        temperature_sensor_enable(temp_sensor);
        TEST_ESP_OK(temperature_sensor_get_celsius(temp_sensor, &tsens_value));
        ESP_LOGI(TAG, "Temperature value %.02f ℃", tsens_value);
        vTaskDelay(pdMS_TO_TICKS(100));
        TEST_ESP_OK(temperature_sensor_disable(temp_sensor));
    }

    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    TEST_ESP_OK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = EXAMPLE_ADC_ATTEN,
    };
    TEST_ESP_OK(adc_oneshot_config_channel(adc1_handle, EXAMPLE_ADC1_CHAN0, &config));
    TEST_ESP_OK(adc_oneshot_config_channel(adc1_handle, EXAMPLE_ADC1_CHAN1, &config));

    cnt = 2;
    while (cnt--) {
        TEST_ESP_OK(adc_oneshot_read(adc1_handle, EXAMPLE_ADC1_CHAN0, &adc_raw[0][0]));
        ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, EXAMPLE_ADC1_CHAN0, adc_raw[0][0]);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    TEST_ESP_OK(adc_oneshot_del_unit(adc1_handle));

    cnt = 2;
    while (cnt--) {
        temperature_sensor_enable(temp_sensor);
        TEST_ESP_OK(temperature_sensor_get_celsius(temp_sensor, &tsens_value));
        ESP_LOGI(TAG, "Temperature value %.02f ℃", tsens_value);
        vTaskDelay(pdMS_TO_TICKS(100));
        TEST_ESP_OK(temperature_sensor_disable(temp_sensor));
    }

    TEST_ESP_OK(temperature_sensor_uninstall(temp_sensor));
}

#endif

/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "hal/adc_periph.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_continuous.h"
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

#define TEST_ADC1_CHAN0          ADC_CHANNEL_2
#define TEST_ADC_ATTEN           ADC_ATTEN_DB_12

static int adc_raw;
static float tsens_value;

static void temp_sensor_test(temperature_sensor_handle_t temp_sensor)
{
    int cnt = 2;
    while (cnt--) {
        TEST_ESP_OK(temperature_sensor_get_celsius(temp_sensor, &tsens_value));
        ESP_LOGI(TAG, "Temperature value %.02f ℃", tsens_value);
        TEST_ASSERT_GREATER_THAN(0, tsens_value);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

static void adc_oneshot_test(adc_oneshot_unit_handle_t adc_handle)
{
    test_adc_set_io_level(ADC_UNIT_1, TEST_ADC1_CHAN0, 0);
    TEST_ESP_OK(adc_oneshot_read(adc_handle, TEST_ADC1_CHAN0, &adc_raw));
    ESP_LOGI(TAG, "low  raw  data: %d", adc_raw);
    TEST_ASSERT_INT_WITHIN(ADC_TEST_LOW_THRESH, ADC_TEST_LOW_VAL, adc_raw);

    test_adc_set_io_level(ADC_UNIT_1, TEST_ADC1_CHAN0, 1);
    TEST_ESP_OK(adc_oneshot_read(adc_handle, TEST_ADC1_CHAN0, &adc_raw));
    ESP_LOGI(TAG, "high raw  data: %d", adc_raw);
    TEST_ASSERT_INT_WITHIN(ADC_TEST_HIGH_THRESH, ADC_TEST_HIGH_VAL, adc_raw);
}

TEST_CASE("Test temperature sensor work with ADC oneshot", "[adc]")
{
    ESP_LOGI(TAG, "Install temperature sensor, expected temp ranger range: 10~50 ℃");
    temperature_sensor_handle_t temp_sensor = NULL;
    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);
    TEST_ESP_OK(temperature_sensor_install(&temp_sensor_config, &temp_sensor));
    TEST_ESP_OK(temperature_sensor_enable(temp_sensor));

    temp_sensor_test(temp_sensor);

    //ADC oneshot work after temperature sensor
    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    TEST_ESP_OK(adc_oneshot_new_unit(&init_config1, &adc_handle));
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = TEST_ADC_ATTEN,
    };
    TEST_ESP_OK(adc_oneshot_config_channel(adc_handle, TEST_ADC1_CHAN0, &config));
    adc_oneshot_test(adc_handle);
    TEST_ESP_OK(adc_oneshot_del_unit(adc_handle));

    //Temperature sensor work after ADC oneshot
    temp_sensor_test(temp_sensor);
    TEST_ESP_OK(temperature_sensor_disable(temp_sensor));

    TEST_ESP_OK(temperature_sensor_uninstall(temp_sensor));
}

#if SOC_ADC_DMA_SUPPORTED
#if (SOC_ADC_DIGI_RESULT_BYTES == 2)
#define ADC_DRIVER_TEST_OUTPUT_TYPE             ADC_DIGI_OUTPUT_FORMAT_TYPE1
#define ADC_DRIVER_TEST_GET_CHANNEL(p_data)     ((p_data)->type1.channel)
#else
#define ADC_DRIVER_TEST_OUTPUT_TYPE             ADC_DIGI_OUTPUT_FORMAT_TYPE2
#define ADC_DRIVER_TEST_GET_CHANNEL(p_data)     ((p_data)->type2.channel)
#endif

#define TEST_ADC_BUF_SIZE   256

TEST_CASE("Test temperature sensor work with ADC continuous", "[adc]")
{
    ESP_LOGI(TAG, "Install temperature sensor, expected temp ranger range: 10~50 ℃");
    temperature_sensor_handle_t temp_sensor = NULL;
    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);
    TEST_ESP_OK(temperature_sensor_install(&temp_sensor_config, &temp_sensor));
    TEST_ESP_OK(temperature_sensor_enable(temp_sensor));

    temp_sensor_test(temp_sensor);

    adc_continuous_handle_t handle = NULL;
    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = TEST_ADC_BUF_SIZE,
        .conv_frame_size = TEST_ADC_BUF_SIZE,
    };
    TEST_ESP_OK(adc_continuous_new_handle(&adc_config, &handle));

    adc_continuous_config_t dig_cfg = {
        .sample_freq_hz = 50 * 1000,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
    };
    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
    adc_pattern[0].atten = ADC_ATTEN_DB_12;
    adc_pattern[0].channel = TEST_ADC1_CHAN0;
    adc_pattern[0].unit = ADC_UNIT_1;
    adc_pattern[0].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;
    dig_cfg.adc_pattern = adc_pattern;
    dig_cfg.pattern_num = 1;
    TEST_ESP_OK(adc_continuous_config(handle, &dig_cfg));

    uint8_t* result = malloc(TEST_ADC_BUF_SIZE);
    TEST_ASSERT(result);

    uint32_t ret_num = 0;
    TEST_ESP_OK(adc_continuous_start(handle));

    temp_sensor_test(temp_sensor);

    //ADC continuous work after temperature sensor
    TEST_ESP_OK(adc_continuous_read(handle, result, TEST_ADC_BUF_SIZE, &ret_num, ADC_MAX_DELAY));
    TEST_ASSERT_EQUAL(TEST_ADC_BUF_SIZE, ret_num);
    for (int i = 0; i < ret_num; i += SOC_ADC_DIGI_RESULT_BYTES) {
        adc_digi_output_data_t *p = (void*)&result[i];
        uint32_t chan_num = ADC_DRIVER_TEST_GET_CHANNEL(p);
        TEST_ASSERT(chan_num < SOC_ADC_CHANNEL_NUM(ADC_UNIT_1));
    }

    //Temperature sensor work after ADC continuous read
    temp_sensor_test(temp_sensor);

    TEST_ESP_OK(adc_continuous_stop(handle));

    //Temperature sensor work after ADC continuous stop
    temp_sensor_test(temp_sensor);

    //Temperature sensor work after ADC continuous deinit
    temp_sensor_test(temp_sensor);

    TEST_ESP_OK(temperature_sensor_disable(temp_sensor));
    TEST_ESP_OK(temperature_sensor_uninstall(temp_sensor));
    TEST_ESP_OK(adc_continuous_deinit(handle));
    free(result);
}

#endif // SOC_ADC_DMA_SUPPORTED
#endif // SOC_TEMP_SENSOR_SUPPORTED && SOC_ADC_SUPPORTED

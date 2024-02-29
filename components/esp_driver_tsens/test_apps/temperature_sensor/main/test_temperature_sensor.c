/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "unity.h"
#include "driver/temperature_sensor.h"
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "unity_test_utils_cache.h"

TEST_CASE("Temperature_sensor_driver_workflow_test", "[temperature_sensor]")
{
    printf("Initializing Temperature sensor\n");
    float tsens_out;
    temperature_sensor_config_t temp_sensor = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);
    temperature_sensor_handle_t temp_handle = NULL;
    TEST_ESP_OK(temperature_sensor_install(&temp_sensor, &temp_handle));
    // read sensor before enable it should fail
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, temperature_sensor_get_celsius(temp_handle, &tsens_out));
    TEST_ESP_OK(temperature_sensor_enable(temp_handle));
    printf("Temperature sensor started\n");
    TEST_ESP_OK(temperature_sensor_get_celsius(temp_handle, &tsens_out));
    printf("Temperature out celsius %f°C\n", tsens_out);
    // uninstall driver before disable it should fail
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, temperature_sensor_uninstall(temp_handle));
    TEST_ESP_OK(temperature_sensor_disable(temp_handle));
    TEST_ESP_OK(temperature_sensor_uninstall(temp_handle));
    // Reconfig the temperature sensor.
    temp_sensor.range_min = -20;
    temp_sensor.range_max = 45;
    TEST_ESP_OK(temperature_sensor_install(&temp_sensor, &temp_handle));
    TEST_ESP_OK(temperature_sensor_enable(temp_handle));
    printf("Temperature sensor started again\n");
    TEST_ESP_OK(temperature_sensor_get_celsius(temp_handle, &tsens_out));
    printf("Temperature out celsius %f°C\n", tsens_out);
    TEST_ESP_OK(temperature_sensor_disable(temp_handle));
    TEST_ESP_OK(temperature_sensor_uninstall(temp_handle));
}

TEST_CASE("Double install error cause test", "[temperature_sensor]")
{
    printf("Initializing Temperature sensor\n");
    temperature_sensor_config_t temp_sensor = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);
    temperature_sensor_handle_t temp_handle = NULL;
    TEST_ESP_OK(temperature_sensor_install(&temp_sensor, &temp_handle));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, temperature_sensor_install(&temp_sensor, &temp_handle));
    TEST_ESP_OK(temperature_sensor_uninstall(temp_handle));
}

TEST_CASE("Double start error cause test", "[temperature_sensor]")
{
    printf("Initializing Temperature sensor\n");
    temperature_sensor_config_t temp_sensor = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);
    temperature_sensor_handle_t temp_handle = NULL;
    TEST_ESP_OK(temperature_sensor_install(&temp_sensor, &temp_handle));
    TEST_ESP_OK(temperature_sensor_enable(temp_handle));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, temperature_sensor_enable(temp_handle));
    TEST_ESP_OK(temperature_sensor_disable(temp_handle));
    TEST_ESP_OK(temperature_sensor_uninstall(temp_handle));
}

TEST_CASE("Double Start-Stop test", "[temperature_sensor]")
{
    printf("Initializing Temperature sensor\n");
    float tsens_result0;
    float tsens_result1;
    temperature_sensor_config_t temp_sensor = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);
    temperature_sensor_handle_t temp_handle = NULL;
    TEST_ESP_OK(temperature_sensor_install(&temp_sensor, &temp_handle));
    TEST_ESP_OK(temperature_sensor_enable(temp_handle));
    printf("Temperature sensor started\n");
    TEST_ESP_OK(temperature_sensor_get_celsius(temp_handle, &tsens_result0));
    printf("Temperature out celsius %f°C\n", tsens_result0);
    TEST_ESP_OK(temperature_sensor_disable(temp_handle));
    TEST_ESP_OK(temperature_sensor_enable(temp_handle));
    TEST_ESP_OK(temperature_sensor_get_celsius(temp_handle, &tsens_result1));
    printf("Temperature out celsius %f°C\n", tsens_result1);
    TEST_ASSERT_FLOAT_WITHIN(4.0, tsens_result0, tsens_result1);
    TEST_ESP_OK(temperature_sensor_disable(temp_handle));
    TEST_ESP_OK(temperature_sensor_uninstall(temp_handle));
}

#if SOC_TEMPERATURE_SENSOR_INTR_SUPPORT

IRAM_ATTR static bool temp_sensor_cbs_test(temperature_sensor_handle_t tsens, const temperature_sensor_threshold_event_data_t *edata, void *user_data)
{
    uint8_t *times = (uint8_t*)user_data;
    ESP_DRAM_LOGI("tsens", "Temperature value is higher or lower than threshold, restart...\n\n");
    (*times)++;
    return false;
}

#if CONFIG_TEMP_SENSOR_ISR_IRAM_SAFE
static void IRAM_ATTR test_delay_post_cache_disable(void *args)
{
    esp_rom_delay_us(1000);
}
#endif

TEST_CASE("Temperature sensor callback test", "[temperature_sensor]")
{
    printf("Initializing Temperature sensor\n");
    float tsens_out;
    temperature_sensor_config_t temp_sensor = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);
    temperature_sensor_handle_t temp_handle = NULL;
    TEST_ESP_OK(temperature_sensor_install(&temp_sensor, &temp_handle));

    temperature_sensor_event_callbacks_t cbs = {
        .on_threshold = temp_sensor_cbs_test,
    };

    temperature_sensor_abs_threshold_config_t threshold_cfg = {
        .high_threshold = 50,
        .low_threshold = -10,
    };
    uint8_t temperature_alarm = 0;
    uint8_t cnt = 10;
    TEST_ESP_OK(temperature_sensor_set_absolute_threshold(temp_handle, &threshold_cfg));
    temperature_sensor_register_callbacks(temp_handle, &cbs, &temperature_alarm);

    TEST_ESP_OK(temperature_sensor_enable(temp_handle));
#if CONFIG_TEMP_SENSOR_ISR_IRAM_SAFE
    printf("disable flash cache and check if we can still get temperature intr\r\n");
    for (int i = 0; i < 100; i++) {
        unity_utils_run_cache_disable_stub(test_delay_post_cache_disable, NULL);
    }
#endif
    while (cnt--) {
        ESP_ERROR_CHECK(temperature_sensor_get_celsius(temp_handle, &tsens_out));
        printf("Temperature out celsius %f°C\n", tsens_out);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    TEST_ESP_OK(temperature_sensor_disable(temp_handle));
    TEST_ESP_OK(temperature_sensor_uninstall(temp_handle));
    printf("temperature alarm is %d\n", temperature_alarm);
    // Note that on CI runner there is no way to heat the board to trigger such an interrupt.
    // But locally test should be notice that alarm must be larger than 0.
    TEST_ASSERT_GREATER_OR_EQUAL(0, temperature_alarm);
}

#endif // SOC_TEMPERATURE_SENSOR_INTR_SUPPORT

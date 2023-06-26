/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "unity.h"
#include "driver/temp_sensor.h"

TEST_CASE("Temperature_legacy_workflow_test", "[hw_timer]")
{
    printf("Initializing Temperature sensor\n");
    float tsens_out;
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
    TEST_ESP_OK(temp_sensor_get_config(&temp_sensor));
    printf("default dac %d, clk_div %d\n", temp_sensor.dac_offset, temp_sensor.clk_div);
    temp_sensor.dac_offset = TSENS_DAC_DEFAULT; // DEFAULT: range:-10℃ ~  80℃, error < 1℃.
    TEST_ESP_OK(temp_sensor_set_config(temp_sensor));
    TEST_ESP_OK(temp_sensor_start());
    printf("Temperature sensor started\n");
    TEST_ESP_OK(temp_sensor_read_celsius(&tsens_out));
    printf("Temperature out celsius %f°C\n", tsens_out);
    TEST_ESP_OK(temp_sensor_stop());
#if !CONFIG_IDF_TARGET_ESP32H2 // disable on eps32h2, seems have some issues on esp32h2
    temp_sensor.dac_offset = TSENS_DAC_L3;
    TEST_ESP_OK(temp_sensor_set_config(temp_sensor));
    TEST_ESP_OK(temp_sensor_start());
    printf("Temperature sensor started again\n");
    TEST_ESP_OK(temp_sensor_read_celsius(&tsens_out));
    printf("Temperature out celsius %f°C\n", tsens_out);
    TEST_ESP_OK(temp_sensor_stop());
#endif
}

TEST_CASE("Temperature legacy double start error cause test", "[temperature_sensor]")
{
    printf("Initializing Temperature sensor\n");
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
    TEST_ESP_OK(temp_sensor_set_config(temp_sensor));
    TEST_ESP_OK(temp_sensor_start());
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, temp_sensor_start());
    TEST_ESP_OK(temp_sensor_stop());
}

TEST_CASE("Temperature legacy double Start-Stop test", "[temperature_sensor]")
{
    printf("Initializing Temperature sensor\n");
    float tsens_out;
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
    TEST_ESP_OK(temp_sensor_get_config(&temp_sensor));
    printf("default dac %d, clk_div %d\n", temp_sensor.dac_offset, temp_sensor.clk_div);
    temp_sensor.dac_offset = TSENS_DAC_DEFAULT; // DEFAULT: range:-10℃ ~  80℃, error < 1℃.
    TEST_ESP_OK(temp_sensor_set_config(temp_sensor));
    TEST_ESP_OK(temp_sensor_start());
    printf("Temperature sensor started\n");
    TEST_ESP_OK(temp_sensor_read_celsius(&tsens_out));
    printf("Temperature out celsius %f°C\n", tsens_out);
    TEST_ESP_OK(temp_sensor_stop());
    TEST_ESP_OK(temp_sensor_start());
    printf("Temperature sensor started again\n");
    TEST_ESP_OK(temp_sensor_read_celsius(&tsens_out));
    printf("Temperature out celsius %f°C\n", tsens_out);
    TEST_ESP_OK(temp_sensor_stop());
}

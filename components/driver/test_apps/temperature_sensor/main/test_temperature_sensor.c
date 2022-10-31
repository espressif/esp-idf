/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "unity.h"
#include "driver/temperature_sensor.h"

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
    float tsens_out;
    temperature_sensor_config_t temp_sensor = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);
    temperature_sensor_handle_t temp_handle = NULL;
    TEST_ESP_OK(temperature_sensor_install(&temp_sensor, &temp_handle));
    TEST_ESP_OK(temperature_sensor_enable(temp_handle));
    printf("Temperature sensor started\n");
    TEST_ESP_OK(temperature_sensor_get_celsius(temp_handle, &tsens_out));
    printf("Temperature out celsius %f°C\n", tsens_out);
    TEST_ESP_OK(temperature_sensor_disable(temp_handle));
    TEST_ESP_OK(temperature_sensor_enable(temp_handle));
    printf("Temperature sensor started again\n");
    TEST_ESP_OK(temperature_sensor_get_celsius(temp_handle, &tsens_out));
    printf("Temperature out celsius %f°C\n", tsens_out);
    TEST_ESP_OK(temperature_sensor_disable(temp_handle));
    TEST_ESP_OK(temperature_sensor_uninstall(temp_handle));
}

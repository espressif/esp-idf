/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "hal/temperature_sensor_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of temperature sensor driver handle
 */
typedef struct temperature_sensor_obj_t *temperature_sensor_handle_t;

/**
 * @brief Configuration of measurement range for the temperature sensor.
 *
 * @note If you see the log `the boundary you gave cannot meet the range of internal temperature sensor`. You may need to refer to
 *       predefined range listed doc ``api-reference/peripherals/Temperature sensor``.
 */
typedef struct {
    int range_min;   /**< the minimum value of the temperature you want to test */
    int range_max;   /**< the maximum value of the temperature you want to test */
    temperature_sensor_clk_src_t clk_src;  /**< the clock source of the temperature sensor. */
} temperature_sensor_config_t;

/**
 * @brief temperature_sensor_config_t default constructure
 */
#define TEMPERATURE_SENSOR_CONFIG_DEFAULT(min, max)    \
    {                                                  \
        .range_min = min,                              \
        .range_max = max,                              \
        .clk_src = TEMPERATURE_SENSOR_CLK_SRC_DEFAULT, \
    }

/**
 * @brief Install temperature sensor driver
 *
 * @param tsens_config Pointer to config structure.
 * @param ret_tsens Return the pointer of temperature sensor handle.
 * @return
 *      - ESP_OK if succeed
 */
esp_err_t temperature_sensor_install(const temperature_sensor_config_t *tsens_config, temperature_sensor_handle_t *ret_tsens);

/**
 * @brief Uninstall the temperature sensor driver
 *
 * @param tsens The handle created by `temperature_sensor_install()`.
 * @return
 *      - ESP_OK if succeed.
 */
esp_err_t temperature_sensor_uninstall(temperature_sensor_handle_t tsens);

/**
 * @brief Enable the temperature sensor
 *
 * @param tsens The handle created by `temperature_sensor_install()`.
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_STATE if temperature sensor is enabled already.
 */
esp_err_t temperature_sensor_enable(temperature_sensor_handle_t tsens);

/**
 * @brief Disable temperature sensor
 *
 * @param tsens The handle created by `temperature_sensor_install()`.
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_STATE if temperature sensor is not enabled yet.
 */
esp_err_t temperature_sensor_disable(temperature_sensor_handle_t tsens);

/**
 * @brief Read temperature sensor data that is converted to degrees Celsius.
 * @note  Should not be called from interrupt.
 *
 * @param tsens The handle created by `temperature_sensor_install()`.
 * @param out_celsius The measure output value.
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG invalid arguments
 *     - ESP_ERR_INVALID_STATE Temperature sensor is not enabled yet.
 *     - ESP_FAIL Parse the sensor data into ambient temperature failed (e.g. out of the range).
 */
esp_err_t temperature_sensor_get_celsius(temperature_sensor_handle_t tsens, float *out_celsius);

#ifdef __cplusplus
}
#endif

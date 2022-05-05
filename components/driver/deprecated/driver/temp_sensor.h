/*
 * SPDX-FileCopyrightText: 2010-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "driver/temp_sensor_types_legacy.h"

#if !CONFIG_TEMP_SENSOR_SUPPRESS_DEPRECATE_WARN
#warning "legacy temperature sensor driver is deprecated, please migrate to driver/temperature_sensor.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set parameter of temperature sensor.
 * @param tsens
 * @return
 *     - ESP_OK Success
 */
esp_err_t temp_sensor_set_config(temp_sensor_config_t tsens);

/**
 * @brief Get parameter of temperature sensor.
 * @param tsens
 * @return
 *     - ESP_OK Success
 */
esp_err_t temp_sensor_get_config(temp_sensor_config_t *tsens);

/**
 * @brief Start temperature sensor measure.
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_STATE if temperature sensor is started already.
 */
esp_err_t temp_sensor_start(void);

/**
 * @brief Stop temperature sensor measure.
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_STATE if temperature sensor is stopped already.
 */
esp_err_t temp_sensor_stop(void);

/**
 * @brief Read temperature sensor raw data.
 * @param tsens_out Pointer to raw data, Range: 0 ~ 255
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG `tsens_out` is NULL
 *     - ESP_ERR_INVALID_STATE temperature sensor dont start
 */
esp_err_t temp_sensor_read_raw(uint32_t *tsens_out);

/**
 * @brief Read temperature sensor data that is converted to degrees Celsius.
 * @note  Should not be called from interrupt.
 * @param celsius The measure output value.
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG ARG is NULL.
 *     - ESP_ERR_INVALID_STATE The ambient temperature is out of range.
 */
esp_err_t temp_sensor_read_celsius(float *celsius);

#ifdef __cplusplus
}
#endif

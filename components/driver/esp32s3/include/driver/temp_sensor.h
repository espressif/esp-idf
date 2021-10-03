/*
 * SPDX-FileCopyrightText: 2010-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief temperature sensor range option.
 */
typedef enum {
    TSENS_DAC_L0 = 0, /*!< offset = -2, measure range: 50℃ ~ 125℃, error < 3℃. */
    TSENS_DAC_L1,     /*!< offset = -1, measure range: 20℃ ~ 100℃, error < 2℃. */
    TSENS_DAC_L2,     /*!< offset =  0, measure range:-10℃ ~  80℃, error < 1℃. */
    TSENS_DAC_L3,     /*!< offset =  1, measure range:-30℃ ~  50℃, error < 2℃. */
    TSENS_DAC_L4,     /*!< offset =  2, measure range:-40℃ ~  20℃, error < 3℃. */
    TSENS_DAC_MAX,
    TSENS_DAC_DEFAULT = TSENS_DAC_L2,
} temp_sensor_dac_offset_t;

/**
 * @brief Configuration for temperature sensor reading
 */
typedef struct {
    temp_sensor_dac_offset_t dac_offset;    /*!< The temperature measurement range is configured with a built-in temperature offset DAC. */
    uint8_t clk_div;                        /*!< Default: 6 */
} temp_sensor_config_t;

/**
 * @brief temperature sensor default setting.
 */
#define TSENS_CONFIG_DEFAULT() {.dac_offset = TSENS_DAC_L2, \
                                .clk_div = 6}

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
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t temp_sensor_start(void);

/**
 * @brief Stop temperature sensor measure.
 * @return
 *     - ESP_OK Success
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

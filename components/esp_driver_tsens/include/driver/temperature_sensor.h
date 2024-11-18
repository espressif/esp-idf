/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
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
    struct {
        uint32_t allow_pd;  /**< If set, the driver will backup/restore the temperature sensor registers before/after entering/exist sleep mode.
                                 By this approach, the system can power off temperature sensor's power domain.
                                 This can save power, but at the expense of more RAM being consumed */
    } flags;                /**< Temperature sensor config flags */

} temperature_sensor_config_t;

/**
 * @brief temperature_sensor_config_t default constructor
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

#if SOC_TEMPERATURE_SENSOR_INTR_SUPPORT

/**
 * @brief Enum for temperature sensor interrupt condition.
 */
typedef enum {
    TEMPERATURE_VAL_HIGHER_THAN_HIGH_THRESHOLD = 0,  /*!< temperature sensor value is higher than high threshold*/
    TEMPERATURE_VAL_LOWER_THAN_LOW_THRESHOLD = 1,  /*!< temperature sensor value is lower than low threshold*/
} temperature_val_intr_condition_t;

/**
 * @brief Temperature sensor event data
 */
typedef struct {
    int celsius_value; /**< Celsius value in interrupt callback. */
    temperature_val_intr_condition_t intr_condition;  /*!< Can be used to judge temperature sensor interrupts in which reason*/
} temperature_sensor_threshold_event_data_t;

/**
 * @brief Callback for temperature sensor threshold interrupt.
 *
 * @param[in] tsens The handle created by `temperature_sensor_install()`.
 * @param[in] edata temperature sensor event data, fed by driver.
 * @param[in] user_data User data, set in `temperature_sensor_register_callbacks()`.
 * @return Whether a high priority task has been waken up by this function.
 */
typedef bool (*temperature_thres_cb_t)(temperature_sensor_handle_t tsens, const temperature_sensor_threshold_event_data_t *edata, void *user_data);

/**
 * @brief Group of temperature sensor callback functions, all of them will be run in ISR.
 */
typedef struct {
    temperature_thres_cb_t on_threshold;  /**< Temperature value interrupt callback */
} temperature_sensor_event_callbacks_t;

/**
 * @brief Config options for temperature value absolute interrupt.
 */
typedef struct {
    float high_threshold;  /**< High threshold value(Celsius). Interrupt will be triggered if temperature value is higher than this value */
    float low_threshold;   /**< Low threshold value(Celsius). Interrupt will be triggered if temperature value is lower than this value */
} temperature_sensor_abs_threshold_config_t;

/**
 * @brief Set temperature sensor absolute mode automatic monitor.
 *
 * @param tsens The handle created by `temperature_sensor_install()`.
 * @param abs_cfg Configuration of temperature sensor absolute mode interrupt, see `temperature_sensor_abs_threshold_config_t`.
 * @note This function should not be called with `temperature_sensor_set_delta_threshold`.
 *
 * @return
 *      - ESP_OK: Set absolute threshold successfully.
 *      - ESP_ERR_INVALID_STATE: Set absolute threshold failed because of wrong state.
 *      - ESP_ERR_INVALID_ARG: Set absolute threshold failed because of invalid argument.
 */
esp_err_t temperature_sensor_set_absolute_threshold(temperature_sensor_handle_t tsens, const temperature_sensor_abs_threshold_config_t *abs_cfg);

/**
 * @brief Config options for temperature value delta interrupt.
 */
typedef struct {
    float increase_delta;   /**< Interrupt will be triggered if the temperature increment of two consecutive samplings if larger than `increase_delta` */
    float decrease_delta;   /**< Interrupt will be triggered if the temperature decrement of two consecutive samplings if smaller than `decrease_delta` */
} temperature_sensor_delta_threshold_config_t;

/**
 * @brief Set temperature sensor differential mode automatic monitor.
 *
 * @param tsens The handle created by `temperature_sensor_install()`.
 * @param delta_cfg Configuration of temperature sensor delta mode interrupt, see `temperature_sensor_delta_threshold_config_t`.
 * @note This function should not be called with `temperature_sensor_set_absolute_threshold`
 *
 * @return
 *      - ESP_OK: Set differential value threshold successfully.
 *      - ESP_ERR_INVALID_STATE: Set absolute threshold failed because of wrong state.
 *      - ESP_ERR_INVALID_ARG: Set differential value threshold failed because of invalid argument.
 */
esp_err_t temperature_sensor_set_delta_threshold(temperature_sensor_handle_t tsens, const temperature_sensor_delta_threshold_config_t *delta_cfg);

/**
 * @brief Install temperature sensor interrupt callback. Temperature sensor interrupt will be enabled at same time
 *
 * @param tsens The handle created by `temperature_sensor_install()`.
 * @param cbs Pointer to the group of temperature sensor interrupt callbacks.
 * @param user_arg Callback argument.
 *
 * @return
 *      - ESP_OK: Set event callbacks successfully
 *      - ESP_ERR_INVALID_ARG: Set event callbacks failed because of invalid argument
 *      - ESP_FAIL: Set event callbacks failed because of other error
 */
esp_err_t temperature_sensor_register_callbacks(temperature_sensor_handle_t tsens, const temperature_sensor_event_callbacks_t *cbs, void *user_arg);

#endif // SOC_TEMPERATURE_SENSOR_INTR_SUPPORT

#ifdef __cplusplus
}
#endif

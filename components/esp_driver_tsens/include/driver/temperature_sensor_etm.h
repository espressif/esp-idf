/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_etm.h"
#include "driver/temperature_sensor.h"
#include "hal/temperature_sensor_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Temperature Sensor ETM event configuration
 */
typedef struct {
    temperature_sensor_etm_event_type_t event_type;     /*!< Temperature Sensor ETM event type */
} temperature_sensor_etm_event_config_t;

/**
 * @brief Get the ETM event for Temperature Sensor
 *
 * @note The created ETM event object can be deleted later by calling `esp_etm_del_event`
 *
 * @param[in] tsens Temperature Sensor handle, allocated by `temperature_sensor_install()`
 * @param[in] config Temperature Sensor ETM event configuration
 * @param[out] out_event Returned ETM event handle
 * @return
 *      - ESP_OK: Get ETM event successfully
 *      - ESP_ERR_INVALID_ARG: Get ETM event failed because of invalid argument
 *      - ESP_FAIL: Get ETM event failed because of other error
 */
esp_err_t temperature_sensor_new_etm_event(temperature_sensor_handle_t tsens, const temperature_sensor_etm_event_config_t *config, esp_etm_event_handle_t *out_event);

/**
 * @brief Temperature Sensor ETM task configuration
 */
typedef struct {
    temperature_sensor_etm_task_type_t task_type;    /*!< Temperature Sensor ETM task type */
} temperature_sensor_etm_task_config_t;

/**
 * @brief Get the ETM task for Temperature Sensor
 *
 * @note The created ETM task object can be deleted later by calling `esp_etm_del_task`
 *
 * @param[in] tsens Temperature Sensor, allocated by `temperature_sensor_install()`
 * @param[in] config Temperature Sensor ETM task configuration
 * @param[out] out_task Returned ETM task handle
 * @return
 *      - ESP_OK: Get ETM task successfully
 *      - ESP_ERR_INVALID_ARG: Get ETM task failed because of invalid argument
 *      - ESP_FAIL: Get ETM task failed because of other error
 */
esp_err_t temperature_sensor_new_etm_task(temperature_sensor_handle_t tsens, const temperature_sensor_etm_task_config_t *config, esp_etm_task_handle_t *out_task);

#ifdef __cplusplus
}
#endif

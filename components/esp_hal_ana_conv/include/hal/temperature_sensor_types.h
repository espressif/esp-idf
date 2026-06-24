/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/clk_tree_defs.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief temperature sensor clock source
 */
#if SOC_TEMP_SENSOR_SUPPORTED
typedef soc_periph_temperature_sensor_clk_src_t temperature_sensor_clk_src_t;
#else
typedef int temperature_sensor_clk_src_t;
#endif // SOC_TEMP_SENSOR_SUPPORTED

/**
 * @brief temperature sensor event types enum
 */
typedef enum {
    TEMPERATURE_SENSOR_EVENT_OVER_LIMIT,  /*!< Temperature sensor over limit event */
    TEMPERATURE_SENSOR_EVENT_MAX,         /*!< Maximum number of temperature sensor events */
} temperature_sensor_etm_event_type_t;

/**
 * @brief temperature sensor task types enum
 */
typedef enum {
    TEMPERATURE_SENSOR_TASK_START,         /*!< Temperature sensor start task */
    TEMPERATURE_SENSOR_TASK_STOP,          /*!< Temperature sensor stop task */
    TEMPERATURE_SENSOR_TASK_MAX,           /*!< Maximum number of temperature sensor tasks */
} temperature_sensor_etm_task_type_t;

#ifdef __cplusplus
}
#endif

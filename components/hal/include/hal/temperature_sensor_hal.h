/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_TEMP_SENSOR_SUPPORTED

/**
 * @brief Get raw temperature value with range management
 *
 * @param range_changed Pointer to store whether range was changed (can be NULL)
 *
 * @return Temperature sensor raw value
 */
int16_t temperature_sensor_hal_get_degree(bool *range_changed);

/**
 * @brief Initialize temperature sensor HAL
 */
void temperature_sensor_hal_init(void);

/**
 * @brief Sync temperature sensor index from main layer to HAL layer
 *
 * @param tsens_idx Temperature sensor index to sync
 */
void temperature_sensor_hal_sync_tsens_idx(uint8_t tsens_idx);

#endif // SOC_TEMP_SENSOR_SUPPORTED

#ifdef __cplusplus
}
#endif

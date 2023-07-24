/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * SAR related peripherals are interdependent. This file
 * provides a united control to these registers, as multiple
 * components require these controls.
 *
 * See target/sar_periph_ctrl.c to know involved peripherals
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Acquire the temperature sensor power
 */
void temperature_sensor_power_acquire(void);

/**
 * @brief Release the temperature sensor power
 */
void temperature_sensor_power_release(void);

/**
 * @brief Get the temperature value and choose the temperature sensor range. Will be both used in phy and peripheral.
 *
 * @param range_changed Pointer to whether range has been changed here. If you don't need this param, you can
 *        set NULL directly.
 *
 * @return temperature sensor value.
 */
int16_t temp_sensor_get_raw_value(bool *range_changed);

/**
 * @brief Synchronize the tsens_idx between sar_periph and driver
 *
 * @param tsens_idx index value of temperature sensor attribute
 */
void temp_sensor_sync_tsens_idx(int tsens_idx);


#ifdef __cplusplus
}
#endif

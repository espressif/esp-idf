/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
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
 * Initialise SAR related peripheral register settings
 * Should only be used when running into app stage
 */
void sar_periph_ctrl_init(void);


/*------------------------------------------------------------------------------
* ADC Power
*----------------------------------------------------------------------------*/
/**
 * @brief Acquire the ADC oneshot mode power
 */
void sar_periph_ctrl_adc_oneshot_power_acquire(void);

/**
 * @brief Release the ADC oneshot mode power
 */
void sar_periph_ctrl_adc_oneshot_power_release(void);

/**
 * @brief Acquire the ADC continuous mode power
 */
void sar_periph_ctrl_adc_continuous_power_acquire(void);

/**
 * @brief Release the ADC ADC continuous mode power
 */
void sar_periph_ctrl_adc_continuous_power_release(void);


/*------------------------------------------------------------------------------
* PWDET Power
*----------------------------------------------------------------------------*/
/**
 * @brief Acquire the PWDET Power
 */
void sar_periph_ctrl_pwdet_power_acquire(void);

/**
 * @brief Release the PWDET Power
 */
void sar_periph_ctrl_pwdet_power_release(void);

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
 * @brief Enable SAR power when system wakes up
 */
void sar_periph_ctrl_power_enable(void);

/**
 * @brief Disable SAR power when system goes to sleep
 */
void sar_periph_ctrl_power_disable(void);

/*------------------------------------------------------------------------------
* ADC Reset
*----------------------------------------------------------------------------*/

/**
* @note For chips that temperature sensor uses part of ADC registers,
*       ADC reset will reset these temperature sensor registers.
*       So we need to backup and restore these temperature sensor registers when ADC reset.
*       And in case temperature sensor result error during ADC reset,
*       we need to acquire a lock to prevent temperature sensor readings during ADC reset.
*/

/**
 * @brief Acquire ADC reset lock
 */
void adc_reset_lock_acquire(void);

/**
 * @brief Release ADC reset lock
 */
void adc_reset_lock_release(void);

/**
 * @brief Reset ADC module
 *
 */
void sar_periph_ctrl_adc_reset(void);

#ifdef __cplusplus
}
#endif

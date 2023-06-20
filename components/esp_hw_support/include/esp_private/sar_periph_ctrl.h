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
 * @brief Enable SAR power when system wakes up
 */
void sar_periph_ctrl_power_enable(void);

/**
 * @brief Disable SAR power when system goes to sleep
 */
void sar_periph_ctrl_power_disable(void);

#ifdef __cplusplus
}
#endif

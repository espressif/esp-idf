/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hal/adc_types.h"
#include "hal/adc_types_private.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * This header file is only for hardware abstract concepts and APIs
 * used by both ADC RTC controller and Digital controller
 */

/**
 * ADC work mode
 */
typedef enum adc_hal_work_mode_t {
    ADC_HAL_SINGLE_READ_MODE,
    ADC_HAL_CONTINUOUS_READ_MODE,
    ADC_HAL_PWDET_MODE,
    ADC_HAL_LP_MODE,
} adc_hal_work_mode_t;

/**
 * Set ADC work mode
 *
 * @param unit       ADC unit
 * @param work_mode  see `adc_hal_work_mode_t`
 */
void adc_hal_set_controller(adc_unit_t unit, adc_hal_work_mode_t work_mode);

#if SOC_ADC_ARBITER_SUPPORTED
//No ADC2 controller arbiter on ESP32
/**
 * Config ADC2 module arbiter.
 * The arbiter is to improve the use efficiency of ADC2. After the control right is robbed by the high priority,
 * the low priority controller will read the invalid ADC2 data, and the validity of the data can be judged by the flag bit in the data.
 *
 * @note Only ADC2 support arbiter.
 * @note The arbiter's working clock is APB_CLK. When the APB_CLK clock drops below 8 MHz, the arbiter must be in shield mode.
 * @note Default priority: Wi-Fi > RTC > Digital;
 *
 * @param config Refer to ``adc_arbiter_t``.
 */
void adc_hal_arbiter_config(adc_arbiter_t *config);
#endif  //#if SOC_ADC_ARBITER_SUPPORTED

/*---------------------------------------------------------------
                    ADC calibration setting
---------------------------------------------------------------*/
#if SOC_ADC_CALIBRATION_V1_SUPPORTED

/**
 * @brief Initialize default parameter for the calibration block.
 *
 * @param adc_n ADC index number
 */
void adc_hal_calibration_init(adc_unit_t adc_n);

/**
 * Set the calibration result (initial data) to ADC.
 *
 * @note  Different ADC units and different attenuation options use different calibration data (initial data).
 *
 * @param adc_n ADC index number.
 * @param param the calibration parameter to configure
 */
void adc_hal_set_calibration_param(adc_unit_t adc_n, uint32_t param);

/**
 * Calibrate the ADC using internal connections.
 *
 * @note  Different ADC units and different attenuation options use different calibration data (initial data).
 *
 * @param adc_n ADC index number.
 * @param atten ADC attenuation
 * @param internal_gnd true:  Disconnect from the IO port and use the internal GND as the calibration voltage.
 *                     false: Use IO external voltage as calibration voltage.
 *
 * @return
 *      - The calibration result (initial data) to ADC, use `adc_hal_set_calibration_param` to set.
 */
uint32_t adc_hal_self_calibration(adc_unit_t adc_n, adc_atten_t atten, bool internal_gnd);

#endif //SOC_ADC_CALIBRATION_V1_SUPPORTED


#ifdef __cplusplus
}
#endif

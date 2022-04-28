/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

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
    ADC_HAL_ULP_MODE,
    ADC_HAL_SINGLE_READ_MODE,
    ADC_HAL_CONTINUOUS_READ_MODE,
    ADC_HAL_PWDET_MODE
} adc_hal_work_mode_t;

/**
 * Set ADC work mode
 *
 * @param unit       ADC unit
 * @param work_mode  see `adc_hal_work_mode_t`
 */
void adc_hal_set_controller(adc_unit_t unit, adc_hal_work_mode_t work_mode);

#ifdef __cplusplus
}
#endif

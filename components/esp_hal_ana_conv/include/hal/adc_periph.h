/*
 * SPDX-FileCopyrightText: 2019-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc.h"
#include "soc/soc_caps.h"
#if SOC_ADC_SUPPORTED
#include "soc/adc_channel.h"
#include "hal/adc_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Store IO number corresponding to the ADC channel number.
 *
 * @value
 *      - >=0 : GPIO number index.
 *      - -1  : Not support.
 */
extern const int adc_channel_io_map[SOC_ADC_PERIPH_NUM][ADC_LL_MAX_CHANNEL_NUM];

#if ADC_LL_SW_WEIGHT_CALIBRATION_SUPPORTED
#define ADC_HAL_SW_WEIGHT_BIT_NUM      17
#define ADC_HAL_SW_WEIGHT_SCALE        10000
#define ADC_HAL_SW_WEIGHT_CONFIG_NUM   7
#define ADC_HAL_SW_WEIGHT_SAMPLES      2048

typedef struct {
    uint8_t bit;
    uint8_t cal_sel;
    uint8_t cal_sel_1;
} adc_hal_sw_weight_cal_config_t;

/* Calibration configurations for raw bit10..bit16. */
extern const adc_hal_sw_weight_cal_config_t adc_hal_sw_weight_cal_configs[ADC_HAL_SW_WEIGHT_CONFIG_NUM];

/* Ideal weights indexed from bit16 down to bit0, stored as fixed-point x10000. */
extern const int32_t adc_hal_sw_weight_ideal_weights[ADC_HAL_SW_WEIGHT_BIT_NUM];
#endif // ADC_LL_SW_WEIGHT_CALIBRATION_SUPPORTED

#ifdef __cplusplus
}
#endif

/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/adc_periph.h"

/* Store the GPIO number for each public ADC channel. */
const int adc_channel_io_map[SOC_ADC_PERIPH_NUM][ADC_LL_MAX_CHANNEL_NUM] = {
    /* ADC1 */
    {
        ADC1_CHANNEL_0_GPIO_NUM, ADC1_CHANNEL_1_GPIO_NUM, ADC1_CHANNEL_2_GPIO_NUM, ADC1_CHANNEL_3_GPIO_NUM,
        ADC1_CHANNEL_4_GPIO_NUM, ADC1_CHANNEL_5_GPIO_NUM, ADC1_CHANNEL_6_GPIO_NUM, ADC1_CHANNEL_7_GPIO_NUM
    },
    /* ADC2 */
    {
        ADC2_CHANNEL_0_GPIO_NUM, ADC2_CHANNEL_1_GPIO_NUM, ADC2_CHANNEL_2_GPIO_NUM, ADC2_CHANNEL_3_GPIO_NUM,
        ADC2_CHANNEL_4_GPIO_NUM, ADC2_CHANNEL_5_GPIO_NUM, ADC2_CHANNEL_6_GPIO_NUM, ADC2_CHANNEL_7_GPIO_NUM
    }
};

#if ADC_LL_SW_WEIGHT_CALIBRATION_SUPPORTED
/* Calibration configurations for raw bit10..bit16. */
const adc_hal_sw_weight_cal_config_t adc_hal_sw_weight_cal_configs[ADC_HAL_SW_WEIGHT_CONFIG_NUM] = {
    {10, 0b00000010, 0b00000001},
    {11, 0b00000100, 0b00000011},
    {12, 0b00001000, 0b00000111},
    {13, 0b00010000, 0b00001111},
    {14, 0b00100000, 0b00011111},
    {15, 0b01000000, 0b00111111},
    {16, 0b10000000, 0b01111111},
};

/* Ideal weights indexed from bit16 down to bit0, stored as fixed-point x10000. */
#define W(v) ((int32_t)((v) * ADC_HAL_SW_WEIGHT_SCALE))
const int32_t adc_hal_sw_weight_ideal_weights[ADC_HAL_SW_WEIGHT_BIT_NUM] = {
    W(2048), W(1024), W(512), W(256), W(256), W(128), W(64),
    W(32),   W(32),   W(16),  W(8),   W(8),   W(4),   W(2),
    W(2),    W(0),    W(1),
};
#undef W
#endif // ADC_LL_SW_WEIGHT_CALIBRATION_SUPPORTED

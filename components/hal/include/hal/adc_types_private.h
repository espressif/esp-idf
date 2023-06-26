/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#if SOC_ADC_ARBITER_SUPPORTED
/*---------------------------------------------------------------
                    Arbiter
---------------------------------------------------------------*/
/**
 * @brief ADC arbiter work mode option.
 */
typedef enum {
    ADC_ARB_MODE_SHIELD,    ///< Force shield arbiter, Select the highest priority controller to work
    ADC_ARB_MODE_FIX,       ///< Fixed priority switch controller mode
    ADC_ARB_MODE_LOOP,      ///< Loop priority switch controller mode. Each controller has the same priority, and the arbiter will switch to the next controller after the measurement is completed
} adc_arbiter_mode_t;

/**
 * @brief ADC arbiter work mode and priority setting.
 *
 * @note Only ADC2 support arbiter.
 */
typedef struct {
    adc_arbiter_mode_t mode;    ///< Refer to ``adc_arbiter_mode_t``
    uint8_t rtc_pri;            ///< RTC controller priority. Range: 0 ~ 2
    uint8_t dig_pri;            ///< Digital controller priority. Range: 0 ~ 2
    uint8_t pwdet_pri;          ///< Wi-Fi controller priority. Range: 0 ~ 2
} adc_arbiter_t;

/**
 * @brief ADC arbiter default configuration.
 */
#define ADC_ARBITER_CONFIG_DEFAULT() { \
    .mode = ADC_ARB_MODE_FIX, \
    .rtc_pri = 1, \
    .dig_pri = 0, \
    .pwdet_pri = 2, \
}
#endif  //#if SOC_ADC_ARBITER_SUPPORTED

#ifdef __cplusplus
}
#endif

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

#if SOC_ADC_MONITOR_SUPPORTED
/*---------------------------------------------------------------
                    Monitor
---------------------------------------------------------------*/
/**
 * @brief ADC digital controller (DMA mode) monitor index options.
 *
 * @note  For ESP32-S2, The monitor object of the ADC is fixed.
 */
typedef enum {
    ADC_DIGI_MONITOR_IDX0 = 0, /*!<The monitor index 0.
                                  For ESP32-S2, It can only be used to monitor all enabled channels of ADC1 unit at the same time. */
    ADC_DIGI_MONITOR_IDX1,     /*!<The monitor index 1.
                                  For ESP32-S2, It can only be used to monitor all enabled channels of ADC2 unit at the same time. */
    ADC_DIGI_MONITOR_IDX_MAX
} adc_digi_monitor_idx_t;

/**
 * @brief Set monitor mode of adc digital controller.
 *        MONITOR_HIGH:If ADC_OUT >  threshold, Generates monitor interrupt.
 *        MONITOR_LOW: If ADC_OUT <  threshold, Generates monitor interrupt.
 */
typedef enum {
#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C2
    ADC_DIGI_MONITOR_DIS = 0,  /*!<Disable monitor. */
    ADC_DIGI_MONITOR_EN,       /*!<If ADC_OUT <  threshold, Generates monitor interrupt. */
                               /*!<If ADC_OUT >  threshold, Generates monitor interrupt. */
#else
    ADC_DIGI_MONITOR_HIGH = 0,  /*!<If ADC_OUT >  threshold, Generates monitor interrupt. */
    ADC_DIGI_MONITOR_LOW,       /*!<If ADC_OUT <  threshold, Generates monitor interrupt. */
#endif
    ADC_DIGI_MONITOR_MAX
} adc_digi_monitor_mode_t;

/**
 * @brief ADC digital controller (DMA mode) monitor configuration.
 *
 */
typedef struct {
    adc_unit_t adc_unit;            /*!<Set adc unit number for monitor.
                                        For ESP32-S2, monitor IDX0/IDX1 can only be used to monitor all enabled channels of ADC1/ADC2 unit at the same time. */
    adc_channel_t channel;          /*!<Set adc channel number for monitor.
                                        For ESP32-S2, it's always `ADC_CHANNEL_MAX` */
    adc_digi_monitor_mode_t mode;   /*!<Set adc monitor mode. See ``adc_digi_monitor_mode_t``. */
    uint32_t h_threshold;           /*!<Set monitor threshold of adc digital controller. */
    uint32_t l_threshold;           /*!<Set monitor threshold of adc digital controller. */
} adc_digi_monitor_t;
#endif  //#if SOC_ADC_MONITOR_SUPPORTED

#ifdef __cplusplus
}
#endif

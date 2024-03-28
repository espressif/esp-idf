/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "hal/adc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ADC resolution setting option.
 * @note  Only used in single read mode
 */
typedef enum {
#if CONFIG_IDF_TARGET_ESP32
    ADC_WIDTH_BIT_9  = 9, /*!< ADC capture width is 9Bit. */
    ADC_WIDTH_BIT_10 = 10, /*!< ADC capture width is 10Bit. */
    ADC_WIDTH_BIT_11 = 11, /*!< ADC capture width is 11Bit. */
    ADC_WIDTH_BIT_12 = 12, /*!< ADC capture width is 12Bit. */
#elif SOC_ADC_RTC_MAX_BITWIDTH == 12
    ADC_WIDTH_BIT_12 = 12, /*!< ADC capture width is 12Bit. */
#elif SOC_ADC_RTC_MAX_BITWIDTH == 13
    ADC_WIDTH_BIT_13 = 13, /*!< ADC capture width is 13Bit. */
#endif
    ADC_WIDTH_MAX,
} adc_bits_width_t;

/**
 * The default (max) bit width of the ADC of current version. You can also get the maximum bitwidth
 * by `SOC_ADC_RTC_MAX_BITWIDTH` defined in soc_caps.h.
 */
#define ADC_WIDTH_BIT_DEFAULT    (ADC_WIDTH_MAX-1)

#if CONFIG_IDF_TARGET_ESP32
typedef enum {
    ADC1_CHANNEL_0 = 0, /*!< ADC1 channel 0 is GPIO36 */
    ADC1_CHANNEL_1,     /*!< ADC1 channel 1 is GPIO37 */
    ADC1_CHANNEL_2,     /*!< ADC1 channel 2 is GPIO38 */
    ADC1_CHANNEL_3,     /*!< ADC1 channel 3 is GPIO39 */
    ADC1_CHANNEL_4,     /*!< ADC1 channel 4 is GPIO32 */
    ADC1_CHANNEL_5,     /*!< ADC1 channel 5 is GPIO33 */
    ADC1_CHANNEL_6,     /*!< ADC1 channel 6 is GPIO34 */
    ADC1_CHANNEL_7,     /*!< ADC1 channel 7 is GPIO35 */
    ADC1_CHANNEL_MAX,
} adc1_channel_t;
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
typedef enum {
    ADC1_CHANNEL_0 = 0, /*!< ADC1 channel 0 is GPIO1  */
    ADC1_CHANNEL_1,     /*!< ADC1 channel 1 is GPIO2  */
    ADC1_CHANNEL_2,     /*!< ADC1 channel 2 is GPIO3  */
    ADC1_CHANNEL_3,     /*!< ADC1 channel 3 is GPIO4  */
    ADC1_CHANNEL_4,     /*!< ADC1 channel 4 is GPIO5  */
    ADC1_CHANNEL_5,     /*!< ADC1 channel 5 is GPIO6  */
    ADC1_CHANNEL_6,     /*!< ADC1 channel 6 is GPIO7  */
    ADC1_CHANNEL_7,     /*!< ADC1 channel 7 is GPIO8  */
    ADC1_CHANNEL_8,     /*!< ADC1 channel 8 is GPIO9  */
    ADC1_CHANNEL_9,     /*!< ADC1 channel 9 is GPIO10 */
    ADC1_CHANNEL_MAX,
} adc1_channel_t;
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C2
typedef enum {
    ADC1_CHANNEL_0 = 0, /*!< ADC1 channel 0 is GPIO0 */
    ADC1_CHANNEL_1,     /*!< ADC1 channel 1 is GPIO1 */
    ADC1_CHANNEL_2,     /*!< ADC1 channel 2 is GPIO2 */
    ADC1_CHANNEL_3,     /*!< ADC1 channel 3 is GPIO3 */
    ADC1_CHANNEL_4,     /*!< ADC1 channel 4 is GPIO4 */
    ADC1_CHANNEL_MAX,
} adc1_channel_t;
#elif CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP32P4 || CONFIG_IDF_TARGET_ESP32C5 || CONFIG_IDF_TARGET_ESP32C61
typedef enum {
    ADC1_CHANNEL_0 = 0, /*!< ADC1 channel 0 is GPIO0 */
    ADC1_CHANNEL_1,     /*!< ADC1 channel 1 is GPIO1 */
    ADC1_CHANNEL_2,     /*!< ADC1 channel 2 is GPIO2 */
    ADC1_CHANNEL_3,     /*!< ADC1 channel 3 is GPIO3 */
    ADC1_CHANNEL_4,     /*!< ADC1 channel 4 is GPIO4 */
    ADC1_CHANNEL_5,     /*!< ADC1 channel 5 is GPIO5 */
    ADC1_CHANNEL_6,     /*!< ADC1 channel 6 is GPIO6 */
    ADC1_CHANNEL_MAX,
} adc1_channel_t;
#endif // CONFIG_IDF_TARGET_*

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
typedef enum {
    ADC2_CHANNEL_0 = 0, /*!< ADC2 channel 0 is GPIO4  (ESP32), GPIO11 (ESP32-S2) */
    ADC2_CHANNEL_1,     /*!< ADC2 channel 1 is GPIO0  (ESP32), GPIO12 (ESP32-S2) */
    ADC2_CHANNEL_2,     /*!< ADC2 channel 2 is GPIO2  (ESP32), GPIO13 (ESP32-S2) */
    ADC2_CHANNEL_3,     /*!< ADC2 channel 3 is GPIO15 (ESP32), GPIO14 (ESP32-S2) */
    ADC2_CHANNEL_4,     /*!< ADC2 channel 4 is GPIO13 (ESP32), GPIO15 (ESP32-S2) */
    ADC2_CHANNEL_5,     /*!< ADC2 channel 5 is GPIO12 (ESP32), GPIO16 (ESP32-S2) */
    ADC2_CHANNEL_6,     /*!< ADC2 channel 6 is GPIO14 (ESP32), GPIO17 (ESP32-S2) */
    ADC2_CHANNEL_7,     /*!< ADC2 channel 7 is GPIO27 (ESP32), GPIO18 (ESP32-S2) */
    ADC2_CHANNEL_8,     /*!< ADC2 channel 8 is GPIO25 (ESP32), GPIO19 (ESP32-S2) */
    ADC2_CHANNEL_9,     /*!< ADC2 channel 9 is GPIO26 (ESP32), GPIO20 (ESP32-S2) */
    ADC2_CHANNEL_MAX,
} adc2_channel_t;
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C2
// ESP32C6 has no ADC2
typedef enum {
    ADC2_CHANNEL_0 = 0, /*!< ADC2 channel 0 is GPIO5 */
    ADC2_CHANNEL_MAX,
} adc2_channel_t;
#endif

#if SOC_ADC_DMA_SUPPORTED
/**
 * @brief Digital ADC DMA read max timeout value, it may make the ``adc_digi_read_bytes`` block forever if the OS supports
 */
#define ADC_MAX_DELAY UINT32_MAX

/**
 * @brief ADC DMA driver configuration
 */
typedef struct adc_digi_init_config_s {
    uint32_t max_store_buf_size;    ///< Max length of the converted data that driver can store before they are processed.
    uint32_t conv_num_each_intr;    ///< Bytes of data that can be converted in 1 interrupt. This should be in multiples of `SOC_ADC_DIGI_DATA_BYTES_PER_CONV`.
    uint32_t adc1_chan_mask;        ///< Channel list of ADC1 to be initialized.
    uint32_t adc2_chan_mask;        ///< Channel list of ADC2 to be initialized.
} adc_digi_init_config_t;

/**
 * @brief ADC digital controller settings
 */
typedef struct {
    bool conv_limit_en;                     ///< Suggest leaving it empty, this parameter has been deprecated
    uint32_t conv_limit_num;                ///< suggest leaving it empty, this parameter has been deprecated
    uint32_t pattern_num;                   ///< Number of ADC channels that will be used
    adc_digi_pattern_config_t *adc_pattern; ///< List of configs for each ADC channel that will be used
    uint32_t sample_freq_hz;                /*!< Please refer to `soc/soc_caps.h` to know the ADC sampling frequency range*/
    adc_digi_convert_mode_t conv_mode;      ///< ADC DMA conversion mode, see `adc_digi_convert_mode_t`.
    adc_digi_output_format_t format;        ///< ADC DMA conversion output format, see `adc_digi_output_format_t`.
} adc_digi_configuration_t;
#endif // #if SOC_ADC_DMA_SUPPORTED

#ifdef __cplusplus
}
#endif

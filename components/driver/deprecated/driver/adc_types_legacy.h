/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
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

typedef enum {
    ADC1_CHANNEL_0 = 0,
    ADC1_CHANNEL_1,
    ADC1_CHANNEL_2,
    ADC1_CHANNEL_3,
#if SOC_ADC_CHANNEL_NUM(0) > 4
    ADC1_CHANNEL_4,
#endif
#if SOC_ADC_CHANNEL_NUM(0) > 5
    ADC1_CHANNEL_5,
#endif
#if SOC_ADC_CHANNEL_NUM(0) > 6
    ADC1_CHANNEL_6,
#endif
#if SOC_ADC_CHANNEL_NUM(0) > 7
    ADC1_CHANNEL_7,
#endif
#if SOC_ADC_CHANNEL_NUM(0) > 8
    ADC1_CHANNEL_8,
#endif
#if SOC_ADC_CHANNEL_NUM(0) > 9
    ADC1_CHANNEL_9,
#endif
    ADC1_CHANNEL_MAX = SOC_ADC_CHANNEL_NUM(0),
} adc1_channel_t;

#if SOC_ADC_PERIPH_NUM > 1
typedef enum {
    ADC2_CHANNEL_0 = 0,
#if SOC_ADC_CHANNEL_NUM(1) > 1
    ADC2_CHANNEL_1,
#endif
#if SOC_ADC_CHANNEL_NUM(1) > 2
    ADC2_CHANNEL_2,
#endif
#if SOC_ADC_CHANNEL_NUM(1) > 3
    ADC2_CHANNEL_3,
#endif
#if SOC_ADC_CHANNEL_NUM(1) > 4
    ADC2_CHANNEL_4,
#endif
#if SOC_ADC_CHANNEL_NUM(1) > 5
    ADC2_CHANNEL_5,
#endif
#if SOC_ADC_CHANNEL_NUM(1) > 6
    ADC2_CHANNEL_6,
#endif
#if SOC_ADC_CHANNEL_NUM(1) > 7
    ADC2_CHANNEL_7,
#endif
#if SOC_ADC_CHANNEL_NUM(1) > 8
    ADC2_CHANNEL_8,
#endif
#if SOC_ADC_CHANNEL_NUM(1) > 9
    ADC2_CHANNEL_9,
#endif
    ADC2_CHANNEL_MAX = SOC_ADC_CHANNEL_NUM(1),
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

/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ADC unit
 */
typedef enum {
    ADC_UNIT_1,        ///< SAR ADC 1
    ADC_UNIT_2,        ///< SAR ADC 2
} adc_unit_t;

/**
 * @brief ADC channels
 */
typedef enum {
    ADC_CHANNEL_0,     ///< ADC channel
    ADC_CHANNEL_1,     ///< ADC channel
    ADC_CHANNEL_2,     ///< ADC channel
    ADC_CHANNEL_3,     ///< ADC channel
    ADC_CHANNEL_4,     ///< ADC channel
    ADC_CHANNEL_5,     ///< ADC channel
    ADC_CHANNEL_6,     ///< ADC channel
    ADC_CHANNEL_7,     ///< ADC channel
    ADC_CHANNEL_8,     ///< ADC channel
    ADC_CHANNEL_9,     ///< ADC channel
} adc_channel_t;

/**
 * @brief ADC attenuation parameter. Different parameters determine the range of the ADC.
 */
typedef enum {
    ADC_ATTEN_DB_0   = 0,  ///<No input attenuation, ADC can measure up to approx.
    ADC_ATTEN_DB_2_5 = 1,  ///<The input voltage of ADC will be attenuated extending the range of measurement by about 2.5 dB
    ADC_ATTEN_DB_6   = 2,  ///<The input voltage of ADC will be attenuated extending the range of measurement by about 6 dB
    ADC_ATTEN_DB_12  = 3,  ///<The input voltage of ADC will be attenuated extending the range of measurement by about 12 dB
    ADC_ATTEN_DB_11 __attribute__((deprecated)) = ADC_ATTEN_DB_12,  ///<This is deprecated, it behaves the same as `ADC_ATTEN_DB_12`
} adc_atten_t;

typedef enum {
    ADC_BITWIDTH_DEFAULT = 0, ///< Default ADC output bits, max supported width will be selected
    ADC_BITWIDTH_9  = 9,      ///< ADC output width is 9Bit
    ADC_BITWIDTH_10 = 10,     ///< ADC output width is 10Bit
    ADC_BITWIDTH_11 = 11,     ///< ADC output width is 11Bit
    ADC_BITWIDTH_12 = 12,     ///< ADC output width is 12Bit
    ADC_BITWIDTH_13 = 13,     ///< ADC output width is 13Bit
} adc_bitwidth_t;

typedef enum {
    ADC_ULP_MODE_DISABLE = 0, ///< ADC ULP mode is disabled
    ADC_ULP_MODE_FSM     = 1, ///< ADC is controlled by ULP FSM
    ADC_ULP_MODE_RISCV   = 2, ///< ADC is controlled by ULP RISCV
#if SOC_LP_ADC_SUPPORTED
    ADC_ULP_MODE_LP_CORE = 3, ///< ADC is controlled by LP Core
#endif // SOC_LP_ADC_SUPPORTED
} adc_ulp_mode_t;

/**
 * @brief ADC digital controller (DMA mode) work mode.
 */
typedef enum {
    ADC_CONV_SINGLE_UNIT_1 = 1,  ///< Only use ADC1 for conversion
    ADC_CONV_SINGLE_UNIT_2 = 2,  ///< Only use ADC2 for conversion
    ADC_CONV_BOTH_UNIT     = 3,  ///< Use Both ADC1 and ADC2 for conversion simultaneously
    ADC_CONV_ALTER_UNIT    = 7,  ///< Use both ADC1 and ADC2 for conversion by turn. e.g. ADC1 -> ADC2 -> ADC1 -> ADC2 .....
} adc_digi_convert_mode_t;

/**
 * @brief ADC digital controller (DMA mode) output data format option.
 */
typedef enum {
    ADC_DIGI_OUTPUT_FORMAT_TYPE1,   ///< See `adc_digi_output_data_t.type1`
    ADC_DIGI_OUTPUT_FORMAT_TYPE2,   ///< See `adc_digi_output_data_t.type2`
} adc_digi_output_format_t;

#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
typedef soc_periph_adc_digi_clk_src_t    adc_oneshot_clk_src_t;     ///< Clock source type of oneshot mode which uses digital controller
typedef soc_periph_adc_digi_clk_src_t    adc_continuous_clk_src_t;  ///< Clock source type of continuous mode which uses digital controller
#elif SOC_ADC_RTC_CTRL_SUPPORTED
typedef soc_periph_adc_rtc_clk_src_t     adc_oneshot_clk_src_t;     ///< Clock source type of oneshot mode which uses RTC controller
typedef soc_periph_adc_digi_clk_src_t    adc_continuous_clk_src_t;  ///< Clock source type of continuous mode which uses digital controller
#else
typedef int                              adc_oneshot_clk_src_t;     ///< Default type
typedef int                              adc_continuous_clk_src_t;  ///< Default type
#endif

/**
 * @brief ADC digital controller pattern configuration
 */
typedef struct {
    uint8_t atten;      ///< Attenuation of this ADC channel
    uint8_t channel;    ///< ADC channel
    uint8_t unit;       ///< ADC unit
    uint8_t bit_width;  ///< ADC output bit width
} adc_digi_pattern_config_t;

/**
 * @brief ADC IIR Filter ID
 */
typedef enum {
    ADC_DIGI_IIR_FILTER_0,  ///< Filter 0
    ADC_DIGI_IIR_FILTER_1,  ///< Filter 1
} adc_digi_iir_filter_t;

/**
 * @brief IIR Filter Coefficient
 */
typedef enum {
    ADC_DIGI_IIR_FILTER_COEFF_2,     ///< The filter coefficient is 2
    ADC_DIGI_IIR_FILTER_COEFF_4,     ///< The filter coefficient is 4
    ADC_DIGI_IIR_FILTER_COEFF_8,     ///< The filter coefficient is 8
    ADC_DIGI_IIR_FILTER_COEFF_16,    ///< The filter coefficient is 16
    ADC_DIGI_IIR_FILTER_COEFF_32,    ///< The filter coefficient is 32
    ADC_DIGI_IIR_FILTER_COEFF_64,    ///< The filter coefficient is 64
} adc_digi_iir_filter_coeff_t;

/*---------------------------------------------------------------
                        ADC Monitor
---------------------------------------------------------------*/
/**
 * @brief ADC monitor (continuous mode) ID
 */
typedef enum {
    ADC_MONITOR_0,          ///< The monitor index 0.
    ADC_MONITOR_1,          ///< The monitor index 1.
} adc_monitor_id_t;

/**
 * @brief Monitor config/event mode type
 */
typedef enum {
    ADC_MONITOR_MODE_HIGH = 0,      ///< ADC raw_result > threshold value, monitor interrupt will be generated.
    ADC_MONITOR_MODE_LOW,           ///< ADC raw_result < threshold value, monitor interrupt will be generated.
} adc_monitor_mode_t;

/*---------------------------------------------------------------
                    Output Format
---------------------------------------------------------------*/
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
/**
 * @brief ADC digital controller (DMA mode) output data format.
 *        Used to analyze the acquired ADC (DMA) data.
 * @note  ESP32: Only `type1` is valid. ADC2 does not support DMA mode.
 * @note  ESP32-S2: Member `channel` can be used to judge the validity of the ADC data,
 *                  because the role of the arbiter may get invalid ADC data.
 */
typedef struct {
    union {
        struct {
            uint16_t data:     12;  /*!<ADC real output data info. Resolution: 12 bit. */
            uint16_t channel:   4;  /*!<ADC channel index info. */
        } type1;                    /*!<ADC type1 */
        struct {
            uint16_t data:     11;  /*!<ADC real output data info. Resolution: 11 bit. */
            uint16_t channel:   4;  /*!<ADC channel index info. For ESP32-S2:
                                        If (channel < ADC_CHANNEL_MAX), The data is valid.
                                        If (channel > ADC_CHANNEL_MAX), The data is invalid. */
            uint16_t unit:      1;  /*!<ADC unit index info. 0: ADC1; 1: ADC2.  */
        } type2;                    /*!<When the configured output format is 11bit.*/
        uint16_t val;               /*!<Raw data value */
    };
} adc_digi_output_data_t;

#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C2
/**
 * @brief ADC digital controller (DMA mode) output data format.
 *        Used to analyze the acquired ADC (DMA) data.
 */
typedef struct {
    union {
        struct {
            uint32_t data:          12; /*!<ADC real output data info. Resolution: 12 bit. */
            uint32_t reserved12:    1;  /*!<Reserved12. */
            uint32_t channel:       3;  /*!<ADC channel index info.
                                            If (channel < ADC_CHANNEL_MAX), The data is valid.
                                            If (channel > ADC_CHANNEL_MAX), The data is invalid. */
            uint32_t unit:          1;  /*!<ADC unit index info. 0: ADC1; 1: ADC2.  */
            uint32_t reserved17_31: 15; /*!<Reserved17. */
        } type2;                        /*!<When the configured output format is 12bit. */
        uint32_t val;                   /*!<Raw data value */
    };
} adc_digi_output_data_t;

#elif CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32P4
/**
 * @brief ADC digital controller (DMA mode) output data format.
 *        Used to analyze the acquired ADC (DMA) data.
 */
typedef struct {
    union {
        struct {
            uint32_t data:          12; /*!<ADC real output data info. Resolution: 12 bit. */
            uint32_t reserved12:    1;  /*!<Reserved12. */
            uint32_t channel:       4;  /*!<ADC channel index info.
                                            If (channel < ADC_CHANNEL_MAX), The data is valid.
                                            If (channel > ADC_CHANNEL_MAX), The data is invalid. */
            uint32_t unit:          1;  /*!<ADC unit index info. 0: ADC1; 1: ADC2.  */
            uint32_t reserved17_31: 14; /*!<Reserved17. */
        } type2;                        /*!<When the configured output format is 12bit. */
        uint32_t val;                   /*!<Raw data value */
    };
} adc_digi_output_data_t;

#elif CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP32C5
/**
 * @brief ADC digital controller (DMA mode) output data format.
 *        Used to analyze the acquired ADC (DMA) data.
 */
typedef struct {
    union {
        struct {
            uint32_t data:          12; /*!<ADC real output data info. Resolution: 12 bit. */
            uint32_t reserved12:    1;  /*!<Reserved12. */
            uint32_t channel:       4;  /*!<ADC channel index info.
                                            If (channel < ADC_CHANNEL_MAX), The data is valid.
                                            If (channel > ADC_CHANNEL_MAX), The data is invalid. */
            uint32_t reserved17_31: 15; /*!<Reserved 17-31. */
        } type2;                        /*!<When the configured output format is 12bit. */
        uint32_t val;                   /*!<Raw data value */
    };
} adc_digi_output_data_t;

#endif


#if CONFIG_IDF_TARGET_ESP32S2
/**
 * @brief ADC digital controller (DMA mode) clock system setting.
 *        Calculation formula: controller_clk = (`APLL` or `APB`) / (div_num + div_a / div_b + 1).
 *
 * @note: The clocks of the DAC digital controller use the ADC digital controller clock divider.
 */
typedef struct {
    bool use_apll;      /*!<true: use APLL clock; false: use APB clock. */
    uint32_t div_num;   /*!<Division factor. Range: 0 ~ 255.
                            Note: When a higher frequency clock is used (the division factor is less than 9),
                            the ADC reading value will be slightly offset. */
    uint32_t div_b;     /*!<Division factor. Range: 1 ~ 63. */
    uint32_t div_a;     /*!<Division factor. Range: 0 ~ 63. */
} adc_digi_clk_t;
#endif

#ifdef __cplusplus
}
#endif

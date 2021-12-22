/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "esp_attr.h"

/**
 * @brief ADC unit enumeration.
 *
 * @note  For ADC digital controller (DMA mode), ESP32 doesn't support `ADC_UNIT_2`, `ADC_UNIT_BOTH`, `ADC_UNIT_ALTER`.
 */
typedef enum {
    ADC_UNIT_1 = 1,          /*!< SAR ADC 1. */
    ADC_UNIT_2 = 2,          /*!< SAR ADC 2. */
    ADC_UNIT_BOTH = 3,       /*!< SAR ADC 1 and 2. */
    ADC_UNIT_ALTER = 7,      /*!< SAR ADC 1 and 2 alternative mode. */
    ADC_UNIT_MAX,
} adc_unit_t;

/**
 * @brief ADC channels handle. See ``adc1_channel_t``, ``adc2_channel_t``.
 *
 * @note  For ESP32 ADC1, don't use `ADC_CHANNEL_8`, `ADC_CHANNEL_9`. See ``adc1_channel_t``.
 */
typedef enum {
    ADC_CHANNEL_0 = 0, /*!< ADC channel */
    ADC_CHANNEL_1,     /*!< ADC channel */
    ADC_CHANNEL_2,     /*!< ADC channel */
    ADC_CHANNEL_3,     /*!< ADC channel */
    ADC_CHANNEL_4,     /*!< ADC channel */
    ADC_CHANNEL_5,     /*!< ADC channel */
    ADC_CHANNEL_6,     /*!< ADC channel */
    ADC_CHANNEL_7,     /*!< ADC channel */
    ADC_CHANNEL_8,     /*!< ADC channel */
    ADC_CHANNEL_9,     /*!< ADC channel */
    ADC_CHANNEL_MAX,
} adc_channel_t;

/**
 * @brief ADC attenuation parameter. Different parameters determine the range of the ADC. See ``adc1_config_channel_atten``.
 */
typedef enum {
    ADC_ATTEN_DB_0   = 0,  /*!<No input attenumation, ADC can measure up to approx. 800 mV. */
    ADC_ATTEN_DB_2_5 = 1,  /*!<The input voltage of ADC will be attenuated extending the range of measurement by about 2.5 dB (1.33 x) */
    ADC_ATTEN_DB_6   = 2,  /*!<The input voltage of ADC will be attenuated extending the range of measurement by about 6 dB (2 x) */
    ADC_ATTEN_DB_11  = 3,  /*!<The input voltage of ADC will be attenuated extending the range of measurement by about 11 dB (3.55 x) */
    ADC_ATTEN_MAX,
} adc_atten_t;

/**
 * @brief ADC resolution setting option.
 * @note  Only used in single read mode
 */
typedef enum {
#if CONFIG_IDF_TARGET_ESP32
    ADC_WIDTH_BIT_9  = 0, /*!< ADC capture width is 9Bit. */
    ADC_WIDTH_BIT_10 = 1, /*!< ADC capture width is 10Bit. */
    ADC_WIDTH_BIT_11 = 2, /*!< ADC capture width is 11Bit. */
    ADC_WIDTH_BIT_12 = 3, /*!< ADC capture width is 12Bit. */
#elif SOC_ADC_MAX_BITWIDTH == 12
    ADC_WIDTH_BIT_12 = 3, /*!< ADC capture width is 12Bit. */
#elif SOC_ADC_MAX_BITWIDTH == 13
    ADC_WIDTH_BIT_13 = 4, /*!< ADC capture width is 13Bit. */
#endif
    ADC_WIDTH_MAX,
} adc_bits_width_t;

/**
 * @brief ADC digital controller (DMA mode) work mode.
 */
typedef enum {
    ADC_CONV_SINGLE_UNIT_1 = 1,  ///< Only use ADC1 for conversion
    ADC_CONV_SINGLE_UNIT_2 = 2,  ///< Only use ADC2 for conversion
    ADC_CONV_BOTH_UNIT     = 3,  ///< Use Both ADC1 and ADC2 for conversion simultaneously
    ADC_CONV_ALTER_UNIT    = 7,  ///< Use both ADC1 and ADC2 for conversion by turn. e.g. ADC1 -> ADC2 -> ADC1 -> ADC2 .....
    ADC_CONV_UNIT_MAX,
} adc_digi_convert_mode_t;

/**
 * @brief ADC digital controller (DMA mode) output data format option.
 */
typedef enum {
    ADC_DIGI_FORMAT_12BIT __attribute__((deprecated)),  /*!<ADC to DMA data format,                [15:12]-channel, [11: 0]-12 bits ADC data (`adc_digi_output_data_t`). Note: For single convert mode. */
    ADC_DIGI_FORMAT_11BIT __attribute__((deprecated)),  /*!<ADC to DMA data format, [15]-adc unit, [14:11]-channel, [10: 0]-11 bits ADC data (`adc_digi_output_data_t`). Note: For multi or alter convert mode. */
    ADC_DIGI_FORMAT_MAX   __attribute__((deprecated)),

    ADC_DIGI_OUTPUT_FORMAT_TYPE1,   ///< See `adc_digi_output_data_t.type1`
    ADC_DIGI_OUTPUT_FORMAT_TYPE2,   ///< See `adc_digi_output_data_t.type2`
} adc_digi_output_format_t;

/**
 * @brief ADC digital controller pattern configuration
 */
typedef struct {
    uint8_t atten;      ///< Attenuation of this ADC channel
    uint8_t channel;    ///< ADC channel
    uint8_t unit;       ///< ADC unit
    uint8_t bit_width;  ///< ADC output bit width
} adc_digi_pattern_config_t;

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
        } type1;
        struct {
            uint16_t data:     11;  /*!<ADC real output data info. Resolution: 11 bit. */
            uint16_t channel:   4;  /*!<ADC channel index info. For ESP32-S2:
                                        If (channel < ADC_CHANNEL_MAX), The data is valid.
                                        If (channel > ADC_CHANNEL_MAX), The data is invalid. */
            uint16_t unit:      1;  /*!<ADC unit index info. 0: ADC1; 1: ADC2.  */
        } type2;                    /*!<When the configured output format is 11bit. `ADC_DIGI_FORMAT_11BIT` */
        uint16_t val;               /*!<Raw data value */
    };
} adc_digi_output_data_t;

#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
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
        } type2;                         /*!<When the configured output format is 12bit. `ADC_DIGI_FORMAT_11BIT` */
        uint32_t val;                   /*!<Raw data value */
    };
} adc_digi_output_data_t;

#elif CONFIG_IDF_TARGET_ESP32S3
/**
 * @brief ADC digital controller (DMA mode) output data format.
 *        Used to analyze the acquired ADC (DMA) data.
 */
typedef struct {
    union {
        struct {
            uint32_t data:          13; /*!<ADC real output data info. Resolution: 13 bit. */
            uint32_t channel:       4;  /*!<ADC channel index info.
                                            If (channel < ADC_CHANNEL_MAX), The data is valid.
                                            If (channel > ADC_CHANNEL_MAX), The data is invalid. */
            uint32_t unit:          1;  /*!<ADC unit index info. 0: ADC1; 1: ADC2.  */
            uint32_t reserved17_31: 14; /*!<Reserved17. */
        } type2;                         /*!<When the configured output format is 12bit. `ADC_DIGI_FORMAT_11BIT` */
        uint32_t val;                   /*!<Raw data value */
    };
} adc_digi_output_data_t;
#endif


#if SOC_ADC_ARBITER_SUPPORTED
/*---------------------------------------------------------------
                    Arbiter
---------------------------------------------------------------*/
/**
 * @brief ADC arbiter work mode option.
 *
 * @note ESP32-S2: Only ADC2 support arbiter.
 */
typedef enum {
    ADC_ARB_MODE_SHIELD,/*!<Force shield arbiter, Select the highest priority controller to work. */
    ADC_ARB_MODE_FIX,   /*!<Fixed priority switch controller mode. */
    ADC_ARB_MODE_LOOP,  /*!<Loop priority switch controller mode. Each controller has the same priority,
                            and the arbiter will switch to the next controller after the measurement is completed. */
} adc_arbiter_mode_t;

/**
 * @brief ADC arbiter work mode and priority setting.
 *
 * @note ESP32-S2: Only ADC2 support arbiter.
 */
typedef struct {
    adc_arbiter_mode_t mode; /*!<Refer to ``adc_arbiter_mode_t``. Note: only support ADC2. */
    uint8_t rtc_pri;        /*!<RTC controller priority. Range: 0 ~ 2. */
    uint8_t dig_pri;        /*!<Digital controller priority. Range: 0 ~ 2. */
    uint8_t pwdet_pri;      /*!<Wi-Fi controller priority. Range: 0 ~ 2. */
} adc_arbiter_t;

/**
 * @brief ADC arbiter default configuration.
 *
 * @note ESP32S2: Only ADC2 supports (needs) an arbiter.
 */
#define ADC_ARBITER_CONFIG_DEFAULT() { \
    .mode = ADC_ARB_MODE_FIX, \
    .rtc_pri = 1, \
    .dig_pri = 0, \
    .pwdet_pri = 2, \
}
#endif  //#if SOC_ADC_ARBITER_SUPPORTED

#if SOC_ADC_FILTER_SUPPORTED
/*---------------------------------------------------------------
                    Filter
---------------------------------------------------------------*/
/**
 * @brief ADC digital controller (DMA mode) filter index options.
 *
 * @note  For ESP32-S2, The filter object of the ADC is fixed.
 */
typedef enum {
    ADC_DIGI_FILTER_IDX0 = 0, /*!<The filter index 0.
                                  For ESP32-S2, It can only be used to filter all enabled channels of ADC1 unit at the same time. */
    ADC_DIGI_FILTER_IDX1,     /*!<The filter index 1.
                                  For ESP32-S2, It can only be used to filter all enabled channels of ADC2 unit at the same time. */
    ADC_DIGI_FILTER_IDX_MAX
} adc_digi_filter_idx_t;

/**
 * @brief ADC digital controller (DMA mode) filter type options.
 *        Expression: filter_data = (k-1)/k * last_data + new_data / k.
 */
typedef enum {
#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
    ADC_DIGI_FILTER_DIS = -1,  /*!< Disable filter */
#endif
    ADC_DIGI_FILTER_IIR_2 = 0, /*!<The filter mode is first-order IIR filter. The coefficient is 2. */
    ADC_DIGI_FILTER_IIR_4,     /*!<The filter mode is first-order IIR filter. The coefficient is 4. */
    ADC_DIGI_FILTER_IIR_8,     /*!<The filter mode is first-order IIR filter. The coefficient is 8. */
    ADC_DIGI_FILTER_IIR_16,    /*!<The filter mode is first-order IIR filter. The coefficient is 16. */
    ADC_DIGI_FILTER_IIR_64,    /*!<The filter mode is first-order IIR filter. The coefficient is 64. */
    ADC_DIGI_FILTER_IIR_MAX
} adc_digi_filter_mode_t;

/**
 * @brief ADC digital controller (DMA mode) filter configuration.
 *
 * @note  For ESP32-S2, The filter object of the ADC is fixed.
 * @note  For ESP32-S2, The filter object is always all enabled channels.
 */
typedef struct {
    adc_unit_t adc_unit;        /*!<Set adc unit number for filter.
                                    For ESP32-S2, Filter IDX0/IDX1 can only be used to filter all enabled channels of ADC1/ADC2 unit at the same time. */
    adc_channel_t channel;      /*!<Set adc channel number for filter.
                                    For ESP32-S2, it's always `ADC_CHANNEL_MAX` */
    adc_digi_filter_mode_t mode;/*!<Set adc filter mode for filter. See ``adc_digi_filter_mode_t``. */
} adc_digi_filter_t;
#endif  // #if SOC_ADC_FILTER_SUPPORTED

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
#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
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
 * @note  For ESP32-S2, The monitor object of the ADC is fixed.
 * @note  For ESP32-S2, The monitor object is always all enabled channels.
 */
typedef struct {
    adc_unit_t adc_unit;            /*!<Set adc unit number for monitor.
                                        For ESP32-S2, monitor IDX0/IDX1 can only be used to monitor all enabled channels of ADC1/ADC2 unit at the same time. */
    adc_channel_t channel;          /*!<Set adc channel number for monitor.
                                        For ESP32-S2, it's always `ADC_CHANNEL_MAX` */
    adc_digi_monitor_mode_t mode;   /*!<Set adc monitor mode. See ``adc_digi_monitor_mode_t``. */
#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
    uint32_t h_threshold;             /*!<Set monitor threshold of adc digital controller. */
    uint32_t l_threshold;             /*!<Set monitor threshold of adc digital controller. */
#else
    uint32_t threshold;             /*!<Set monitor threshold of adc digital controller. */
#endif
} adc_digi_monitor_t;
#endif  //#if SOC_ADC_MONITOR_SUPPORTED

/*---------------------------------------------------------------
            To Be Deprecated TODO: IDF-3610
---------------------------------------------------------------*/
#ifdef CONFIG_IDF_TARGET_ESP32
/**
 * @brief ESP32 ADC DMA source selection.
 */
#else
/**
 * @brief ESP32 ADC DMA source selection.
 *
 * @deprecated  Not applicable on ESP32-S2 because ESP32-S2 doesn't use I2S DMA.
 */
#endif
typedef enum {
    ADC_I2S_DATA_SRC_IO_SIG = 0, /*!< I2S data from GPIO matrix signal  */
    ADC_I2S_DATA_SRC_ADC = 1,    /*!< I2S data from ADC */
    ADC_I2S_DATA_SRC_MAX,
} adc_i2s_source_t;

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

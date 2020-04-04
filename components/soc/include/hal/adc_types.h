#pragma once

#include "soc/adc_caps.h"
#include "sdkconfig.h"
#include <stdbool.h>

/**
 * @brief ADC units selected handle.
 *
 * @note  For ADC digital controller(DMA mode), ESP32 don't support `ADC_UNIT_2`, `ADC_UNIT_BOTH`, `ADC_UNIT_ALTER`.
 */
typedef enum {
    ADC_UNIT_1 = 1,          /*!< SAR ADC 1. */
    ADC_UNIT_2 = 2,          /*!< SAR ADC 2. */
    ADC_UNIT_BOTH = 3,       /*!< SAR ADC 1 and 2. */
    ADC_UNIT_ALTER = 7,      /*!< SAR ADC 1 and 2 alternative mode, not supported yet */
    ADC_UNIT_MAX,
} adc_unit_t;

/**
 * @brief ADC channels handle. See ``adc1_channel_t``, ``adc2_channel_t``.
 *
 * @note  For ESP32 ADC1, don't support `ADC_CHANNEL_8`, `ADC_CHANNEL_9`. See ``adc1_channel_t``.
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
    ADC_ATTEN_DB_0   = 0,  /*!<The input voltage of ADC will be reduced to about 1/1 */
    ADC_ATTEN_DB_2_5 = 1,  /*!<The input voltage of ADC will be reduced to about 1/1.34 */
    ADC_ATTEN_DB_6   = 2,  /*!<The input voltage of ADC will be reduced to about 1/2 */
    ADC_ATTEN_DB_11  = 3,  /*!<The input voltage of ADC will be reduced to about 1/3.6*/
    ADC_ATTEN_MAX,
} adc_atten_t;

/**
 * @brief ESP32 ADC DMA source selection.
 *
 * @note  It's be deprecated in ESP32S2. Beacause ESP32S2 don't use I2S DMA.
 */
typedef enum {
    ADC_I2S_DATA_SRC_IO_SIG = 0, /*!< I2S data from GPIO matrix signal  */
    ADC_I2S_DATA_SRC_ADC = 1,    /*!< I2S data from ADC */
    ADC_I2S_DATA_SRC_MAX,
} adc_i2s_source_t;

/**
 * @brief ADC resolution setting option.
 *
 * @note  For ESP32S2. Only support 13 bit resolution.
 *        For ESP32.   Don't support 13 bit resolution.
 */
typedef enum {
    ADC_WIDTH_BIT_9  = 0, /*!< ADC capture width is 9Bit. Only ESP32 is supported. */
    ADC_WIDTH_BIT_10 = 1, /*!< ADC capture width is 10Bit. Only ESP32 is supported. */
    ADC_WIDTH_BIT_11 = 2, /*!< ADC capture width is 11Bit. Only ESP32 is supported. */
    ADC_WIDTH_BIT_12 = 3, /*!< ADC capture width is 12Bit. Only ESP32 is supported. */
#ifdef CONFIG_IDF_TARGET_ESP32S2
    ADC_WIDTH_BIT_13 = 4, /*!< ADC capture width is 13Bit. Only ESP32S2 is supported. */
#endif
    ADC_WIDTH_MAX,
} adc_bits_width_t;

#ifdef CONFIG_IDF_TARGET_ESP32S2

/**
 * @brief ADC digital controller (DMA mode) clock system setting.
 *        Expression: controller_clk = (`APLL` or `APB`) * (div_num  + div_b / div_a).
 */
typedef struct {
    bool use_apll;      /*!<true: use APLL clock; false: use APB clock. */
    uint32_t div_num;   /*!<Division factor. Range: 1 ~ 255. */
    uint32_t div_b;     /*!<Division factor. Range: 1 ~ 63. */
    uint32_t div_a;     /*!<Division factor. Range: 1 ~ 63. */
} adc_digi_clk_t;

/**
 * @brief ADC digital controller (DMA mode) clock system default setting.
 */
#define ADC_DIGITAL_CLK_DEFAULT() { \
    .use_apll = 0, \
    .div_num = 40, \
    .div_b = 1, \
    .div_a = 1, \
}

/**
 * @brief ADC arbiter work mode option.
 *
 * @note ESP32S2: Only ADC2 support arbiter.
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
 * @note ESP32S2: Only ADC2 support arbiter.
 */
typedef struct {
    adc_arbiter_mode_t mode; /*!<Refer to `adc_arbiter_mode_t`. Note: only support ADC2. */
    uint8_t rtc_pri;        /*!<RTC controller priority. Range: 0 ~ 2. */
    uint8_t dig_pri;        /*!<Digital controller priority. Range: 0 ~ 2. */
    uint8_t pwdet_pri;      /*!<Wi-Fi controller priority. Range: 0 ~ 2. */
} adc_arbiter_t;

/**
 * @brief ADC arbiter default configuration.
 *
 * @note ESP32S2: Only ADC2 support arbiter.
 */
#define ADC_ARBITER_CONFIG_DEFAULT() { \
    .mode = ADC_ARB_MODE_FIX, \
    .rtc_pri = 1, \
    .dig_pri = 0, \
    .pwdet_pri = 2, \
}

/**
 * @brief ADC digital controller (DMA mode) work mode.
 *
 * @note  Member `channel` can be used to judge the validity of the ADC data, because the role of the arbiter may get invalid ADC data.
 */
typedef enum {
    ADC_CONV_SINGLE_UNIT_1 = 1, /*!< SAR ADC 1. */
    ADC_CONV_SINGLE_UNIT_2 = 2, /*!< SAR ADC 2. */
    ADC_CONV_BOTH_UNIT     = 3, /*!< SAR ADC 1 and 2. */
    ADC_CONV_ALTER_UNIT    = 7, /*!< SAR ADC 1 and 2 alternative mode. */
    ADC_CONV_UNIT_MAX,
} adc_digi_convert_mode_t;

/**
 * @brief ADC digital controller (DMA mode) output data format option.
 */
typedef enum {
    ADC_DIGI_FORMAT_12BIT,   /*!<ADC to DMA data format,                [15:12]-channel, [11: 0]-12 bits ADC data (`adc_digi_output_data_t`).
                                 Note: In single convert mode. */
    ADC_DIGI_FORMAT_11BIT,   /*!<ADC to DMA data format, [15]-adc unit, [14:11]-channel, [10: 0]-11 bits ADC data (`adc_digi_output_data_t`).
                                 Note: In multi or alter convert mode. */
    ADC_DIGI_FORMAT_MAX,
} adc_digi_output_format_t;

/**
 * @brief ADC digital controller (DMA mode) output data format.
 *        Used to analyze the acquired ADC (DMA) data.
 *
 * @note  Member `channel` can be used to judge the validity of the ADC data, because the role of the arbiter may get invalid ADC data.
 */
typedef struct {
    union {
        struct {
            uint16_t data:     12;  /*!<ADC real output data info. Resolution: 12 bit. */
            uint16_t channel:   4;  /*!<ADC channel index info.
                                        If (channel < ADC_CHANNEL_MAX), The data is valid.
                                        If (channel > ADC_CHANNEL_MAX), The data is invalid. */
        } type1;                    /*!<When the configured output format is 12bit. `ADC_DIGI_FORMAT_12BIT` */
        struct {
            uint16_t data:     11;  /*!<ADC real output data info. Resolution: 11 bit. */
            uint16_t channel:   4;  /*!<ADC channel index info.
                                        If (channel < ADC_CHANNEL_MAX), The data is valid.
                                        If (channel > ADC_CHANNEL_MAX), The data is invalid. */
            uint16_t unit:      1;  /*!<ADC unit index info. 0: ADC1; 1: ADC2.  */
        } type2;                    /*!<When the configured output format is 11bit. `ADC_DIGI_FORMAT_11BIT` */
        uint16_t val;
    };
} adc_digi_output_data_t;

/**
 * @brief ADC digital controller (DMA mode) conversion rules setting.
 */
typedef struct {
    union {
        struct {
            uint8_t atten:     2;   /*!< ADC sampling voltage attenuation configuration.
                                         0: input voltage * 1;
                                         1: input voltage * 1/1.34;
                                         2: input voltage * 1/2;
                                         3: input voltage * 1/3.6. */
            uint8_t reserved:  2;    /*!< reserved0 */
            uint8_t channel:   4;   /*!< ADC channel index. */
        };
        uint8_t val;
    };
} adc_digi_pattern_table_t;

/**
 * @brief ADC digital controller (DMA mode) interrupt type options.
 */
typedef enum {
    ADC_DIGI_INTR_MASK_MONITOR = 0x1,
    ADC_DIGI_INTR_MASK_MEAS_DONE = 0x2,
    ADC_DIGI_INTR_MASK_ALL = 0x3,
} adc_digi_intr_t;

/**
 * @brief ADC digital controller (DMA mode) configuration parameters.
 */
typedef struct {
    bool conv_limit_en;         /*!<Enable max conversion number detection for digital controller.
                                    If the number of ADC conversion is equal to the `limit_num`, the conversion is stopped. */
    uint32_t conv_limit_num;    /*!<ADC max conversion number for digital controller. */
    uint32_t adc1_pattern_len;  /*!<Pattern table length for digital controller. Range: 0 ~ 16.
                                    The pattern table that defines the conversion rules for each SAR ADC. Each table has 16 items, in which channel selection,
                                    resolution and attenuation are stored. When the conversion is started, the controller reads conversion rules from the
                                    pattern table one by one. For each controller the scan sequence has at most 16 different rules before repeating itself. */
    uint32_t adc2_pattern_len;  /*!<Refer to `adc1_pattern_len` */
    adc_digi_pattern_table_t *adc1_pattern;  /*!<Pointer to pattern table for digital controller. The table size defined by `adc1_pattern_len`. */
    adc_digi_pattern_table_t *adc2_pattern;  /*!<Refer to `adc1_pattern` */
    adc_digi_convert_mode_t conv_mode;       /*!<ADC conversion mode for digital controller. */
    adc_digi_output_format_t format;         /*!<ADC output data format for digital controller. */
    uint32_t interval;          /*!<The number of interval clock cycles for the digital controller to trigger the measurement.
                                    The unit is the divided clock. Range: 40 ~ 4095. */
    adc_digi_clk_t dig_clk;     /*!<Refer to `adc_digi_clk_t` */
    uint32_t dma_eof_num;       /*!<DMA eof num of adc digital controller.
                                    If the number of measurements reaches `dma_eof_num`,
                                    then `dma_in_suc_eof` signal is generated. */
} adc_digi_config_t;

/**
 * @brief ADC digital controller (DMA mode) filter index options.
 *
 * @note  For ESP32S2, The filter object of the ADC is fixed.
 */
typedef enum {
    ADC_DIGI_FILTER_IDX0 = 0, /*!<The filter index 0.
                                  For ESP32S2, It can only be used to filter all enabled channels of ADC1 unit at the same time. */
    ADC_DIGI_FILTER_IDX1,     /*!<The filter index 1.
                                  For ESP32S2, It can only be used to filter all enabled channels of ADC2 unit at the same time. */
    ADC_DIGI_FILTER_IDX_MAX
} adc_digi_filter_idx_t;

/**
 * @brief ADC digital controller (DMA mode) filter type options.
 *        Expression: filter_data = (k-1)/k * last_data + new_data / k.
 */
typedef enum {
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
 * @note  For ESP32S2, The filter object of the ADC is fixed.
 * @note  For ESP32S2, The filter object is always all enabled channels.
 */
typedef struct {
    adc_unit_t adc_unit;        /*!<Set adc unit number for filter.
                                    For ESP32S2, Filter IDX0/IDX1 can only be used to filter all enabled channels of ADC1/ADC2 unit at the same time. */
    adc_channel_t channel;      /*!<Set adc channel number for filter.
                                    For ESP32S2, it's always `ADC_CHANNEL_MAX` */
    adc_digi_filter_mode_t mode;/*!<Set adc filter mode for filter. See ``adc_digi_filter_mode_t``. */
} adc_digi_filter_t;

/**
 * @brief ADC digital controller (DMA mode) monitor index options.
 *
 * @note  For ESP32S2, The monitor object of the ADC is fixed.
 */
typedef enum {
    ADC_DIGI_MONITOR_IDX0 = 0, /*!<The monitor index 0.
                                  For ESP32S2, It can only be used to monitor all enabled channels of ADC1 unit at the same time. */
    ADC_DIGI_MONITOR_IDX1,     /*!<The monitor index 1.
                                  For ESP32S2, It can only be used to monitor all enabled channels of ADC2 unit at the same time. */
    ADC_DIGI_MONITOR_IDX_MAX
} adc_digi_monitor_idx_t;

/**
 * @brief Set monitor mode of adc digital controller.
 *        MONITOR_HIGH:If ADC_OUT >  threshold, Generates monitor interrupt.
 *        MONITOR_LOW: If ADC_OUT <  threshold, Generates monitor interrupt.
 */
typedef enum {
    ADC_DIGI_MONITOR_HIGH = 0,  /*!<If ADC_OUT >  threshold, Generates monitor interrupt. */
    ADC_DIGI_MONITOR_LOW,       /*!<If ADC_OUT <  threshold, Generates monitor interrupt. */
    ADC_DIGI_MONITOR_MAX
} adc_digi_monitor_mode_t;

/**
 * @brief ADC digital controller (DMA mode) monitor configuration.
 *
 * @note  For ESP32S2, The monitor object of the ADC is fixed.
 * @note  For ESP32S2, The monitor object is always all enabled channels.
 */
typedef struct {
    adc_unit_t adc_unit;            /*!<Set adc unit number for monitor.
                                        For ESP32S2, monitor IDX0/IDX1 can only be used to monitor all enabled channels of ADC1/ADC2 unit at the same time. */
    adc_channel_t channel;          /*!<Set adc channel number for monitor.
                                        For ESP32S2, it's always `ADC_CHANNEL_MAX` */
    adc_digi_monitor_mode_t mode;   /*!<Set adc monitor mode. See ``adc_digi_monitor_mode_t``. */
    uint32_t threshold;             /*!<Set monitor threshold of adc digital controller. */
} adc_digi_monitor_t;

#endif // CONFIG_IDF_TARGET_ESP32S2
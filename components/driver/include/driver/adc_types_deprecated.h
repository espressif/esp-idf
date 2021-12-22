/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "esp_err.h"
#include "hal/adc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_IDF_TARGET_ESP32
/*---------------------------------------------------------------
                    ESP32 Deprecated Types
---------------------------------------------------------------*/
/**
 * @brief ADC digital controller (DMA mode) conversion rules setting.
 */
typedef struct {
    union {
        struct {
            uint8_t atten:     2;   /*!< ADC sampling voltage attenuation configuration. Modification of attenuation affects the range of measurements.
                                         0: measurement range 0 - 800mV,
                                         1: measurement range 0 - 1100mV,
                                         2: measurement range 0 - 1350mV,
                                         3: measurement range 0 - 2600mV. */
            uint8_t bit_width: 2;   /*!< ADC resolution.
-                                         0: 9 bit;
-                                         1: 10 bit;
-                                         2: 11 bit;
-                                         3: 12 bit. */
            int8_t channel:   4;   /*!< ADC channel index. */
        };
        uint8_t val;                /*!<Raw data value */
    };
} adc_digi_pattern_table_t __attribute__((deprecated));

/**
  * Explanation of the relationship between `conv_limit_num`, `dma_eof_num` and the number of DMA outputs:
  *
  *     +---------------------+--------+--------+--------+
  *     | conv_mode           | single |  both  |  alter |
  *     +---------------------+--------+--------+--------+
  *     | trigger meas times  |    1   |    1   |    1   |
  *     +---------------------+--------+--------+--------+
  *     | conv_limit_num      |   +1   |   +1   |   +1   |
  *     | dma_eof_num         |   +1   |   +2   |   +1   |
  *     | dma output (byte)   |   +2   |   +4   |   +2   |
  *     +---------------------+--------+--------+--------+
  */
typedef struct {
    bool conv_limit_en;                      /*!<Enable the function of limiting ADC conversion times.
                                                 If the number of ADC conversion trigger count is equal to the `limit_num`, the conversion is stopped. */
    uint32_t conv_limit_num;                 /*!<Set the upper limit of the number of ADC conversion triggers. Range: 1 ~ 255. */
    uint32_t adc1_pattern_len;               /*!<Pattern table length for digital controller. Range: 0 ~ 16 (0: Don't change the pattern table setting).
                                                 The pattern table that defines the conversion rules for each SAR ADC. Each table has 16 items, in which channel selection,
                                                 resolution and attenuation are stored. When the conversion is started, the controller reads conversion rules from the
                                                 pattern table one by one. For each controller the scan sequence has at most 16 different rules before repeating itself. */
    uint32_t adc2_pattern_len;               /*!<Refer to ``adc1_pattern_len`` */
    adc_digi_pattern_table_t *adc1_pattern;  /*!<Pointer to pattern table for digital controller. The table size defined by `adc1_pattern_len`. */
    adc_digi_pattern_table_t *adc2_pattern;  /*!<Refer to `adc1_pattern` */
    adc_digi_convert_mode_t conv_mode;       /*!<ADC conversion mode for digital controller. See ``adc_digi_convert_mode_t``. */
    adc_digi_output_format_t format;         /*!<ADC output data format for digital controller. See ``adc_digi_output_format_t``. */
} adc_digi_config_t __attribute__((deprecated));
#endif  //#if CONFIG_IDF_TARGET_ESP32


#if CONFIG_IDF_TARGET_ESP32S2
/*---------------------------------------------------------------
                    ESP32S2 Deprecated Types
---------------------------------------------------------------*/
/**
 * @brief ADC digital controller (DMA mode) conversion rules setting.
 */
typedef struct {
    union {
        struct {
            uint8_t atten:     2;   /*!< ADC sampling voltage attenuation configuration. Modification of attenuation affects the range of measurements.
                                         0: measurement range 0 - 800mV,
                                         1: measurement range 0 - 1100mV,
                                         2: measurement range 0 - 1350mV,
                                         3: measurement range 0 - 2600mV. */
            uint8_t reserved:  2;   /*!< reserved0 */
            uint8_t channel:   4;   /*!< ADC channel index. */
        };
        uint8_t val;                /*!<Raw data value */
    };
} adc_digi_pattern_table_t __attribute__((deprecated));

/**
  * @brief ADC digital controller (DMA mode) configuration parameters.
  *
  * Example setting: When using ADC1 channel0 to measure voltage, the sampling rate is required to be 1 kHz:
  *
  *     +---------------------+--------+--------+--------+
  *     | sample rate         | 1 kHz  | 1 kHz  | 1 kHz  |
  *     +---------------------+--------+--------+--------+
  *     | conv_mode           | single |  both  |  alter |
  *     | adc1_pattern_len    |    1   |    1   |    1   |
  *     | dig_clk.use_apll    |    0   |    0   |    0   |
  *     | dig_clk.div_num     |   99   |   99   |   99   |
  *     | dig_clk.div_b       |    0   |    0   |    0   |
  *     | dig_clk.div_a       |    0   |    0   |    0   |
  *     | interval            |  400   |  400   |  200   |
  *     +---------------------+--------+--------+--------+
  *     | `trigger_meas_freq` | 1 kHz  | 1 kHz  | 2 kHz  |
  *     +---------------------+--------+--------+--------+
  *
  * Explanation of the relationship between `conv_limit_num`, `dma_eof_num` and the number of DMA outputs:
  *
  *     +---------------------+--------+--------+--------+
  *     | conv_mode           | single |  both  |  alter |
  *     +---------------------+--------+--------+--------+
  *     | trigger meas times  |    1   |    1   |    1   |
  *     +---------------------+--------+--------+--------+
  *     | conv_limit_num      |   +1   |   +1   |   +1   |
  *     | dma_eof_num         |   +1   |   +2   |   +1   |
  *     | dma output (byte)   |   +2   |   +4   |   +2   |
  *     +---------------------+--------+--------+--------+
  */
typedef struct {
    bool conv_limit_en;                      /*!<Enable the function of limiting ADC conversion times.
                                                 If the number of ADC conversion trigger count is equal to the `limit_num`, the conversion is stopped. */
    uint32_t conv_limit_num;                 /*!<Set the upper limit of the number of ADC conversion triggers. Range: 1 ~ 255. */
    uint32_t adc1_pattern_len;               /*!<Pattern table length for digital controller. Range: 0 ~ 16 (0: Don't change the pattern table setting).
                                                 The pattern table that defines the conversion rules for each SAR ADC. Each table has 16 items, in which channel selection,
                                                 resolution and attenuation are stored. When the conversion is started, the controller reads conversion rules from the
                                                 pattern table one by one. For each controller the scan sequence has at most 16 different rules before repeating itself. */
    uint32_t adc2_pattern_len;               /*!<Refer to ``adc1_pattern_len`` */
    adc_digi_pattern_table_t *adc1_pattern;  /*!<Pointer to pattern table for digital controller. The table size defined by `adc1_pattern_len`. */
    adc_digi_pattern_table_t *adc2_pattern;  /*!<Refer to `adc1_pattern` */
    adc_digi_convert_mode_t conv_mode;       /*!<ADC conversion mode for digital controller. See ``adc_digi_convert_mode_t``. */
    adc_digi_output_format_t format;         /*!<ADC output data format for digital controller. See ``adc_digi_output_format_t``. */
    uint32_t interval;                       /*!<The number of interval clock cycles for the digital controller to trigger the measurement.
                                                 The unit is the divided clock. Range: 40 ~ 4095.
                                                 Expression: `trigger_meas_freq` = `controller_clk` / 2 / interval. Refer to ``adc_digi_clk_t``.
                                                 Note: The sampling rate of each channel is also related to the conversion mode (See ``adc_digi_convert_mode_t``) and pattern table settings. */
    adc_digi_clk_t dig_clk;                  /*!<ADC digital controller clock divider settings. Refer to ``adc_digi_clk_t``.
                                                 Note: The clocks of the DAC digital controller use the ADC digital controller clock divider. */
    uint32_t dma_eof_num;                    /*!<DMA eof num of adc digital controller.
                                                 If the number of measurements reaches `dma_eof_num`, then `dma_in_suc_eof` signal is generated in DMA.
                                                 Note: The converted data in the DMA in link buffer will be multiple of two bytes. */
} adc_digi_config_t __attribute__((deprecated));

/**
 * @brief ADC digital controller (DMA mode) interrupt type options.
 */
typedef enum {
    ADC_DIGI_INTR_MASK_MONITOR = 0x1,
    ADC_DIGI_INTR_MASK_MEAS_DONE = 0x2,
    ADC_DIGI_INTR_MASK_ALL = 0x3,
} adc_digi_intr_t __attribute__((deprecated));
FLAG_ATTR(adc_digi_intr_t)
#endif  //#if CONFIG_IDF_TARGET_ESP32S2


#if CONFIG_IDF_TARGET_ESP32C3
/*---------------------------------------------------------------
                    ESP32C3 Deprecated Types
---------------------------------------------------------------*/
/**
 * @brief ADC digital controller (DMA mode) conversion rules setting.
 */
typedef struct {
    union {
        struct {
            uint8_t atten:     2;   /*!< ADC sampling voltage attenuation configuration. Modification of attenuation affects the range of measurements.
                                         0: measurement range 0 - 800mV,
                                         1: measurement range 0 - 1100mV,
                                         2: measurement range 0 - 1350mV,
                                         3: measurement range 0 - 2600mV. */
            uint8_t channel:   3;   /*!< ADC channel index. */
            uint8_t unit:      1;   /*!< ADC unit index. */
            uint8_t reserved:  2;   /*!< reserved0 */
        };
        uint8_t val;                /*!<Raw data value */
    };
} adc_digi_pattern_table_t __attribute__((deprecated));

typedef struct {
    bool conv_limit_en;                      /*!<Enable the function of limiting ADC conversion times.
                                                 If the number of ADC conversion trigger count is equal to the `limit_num`, the conversion is stopped. */
    uint32_t conv_limit_num;                 /*!<Set the upper limit of the number of ADC conversion triggers. Range: 1 ~ 255. */
    uint32_t adc_pattern_len;                /*!<Pattern table length for digital controller. Range: 0 ~ 7 (0: Don't change the pattern table setting).
                                                 The pattern table that defines the conversion rules for each SAR ADC. Each table has 7 items, in which channel selection,
                                                 resolution and attenuation are stored. When the conversion is started, the controller reads conversion rules from the
                                                 pattern table one by one. For each controller the scan sequence has at most 16 different rules before repeating itself. */
    adc_digi_pattern_table_t *adc_pattern;   /*!<Pointer to pattern table for digital controller. The table size defined by `adc_pattern_len`. */
    uint32_t sample_freq_hz;  /*!< The expected ADC sampling frequency in Hz. Range: 611Hz ~ 83333Hz
                                   Fs = Fd / interval / 2
                                   Fs: sampling frequency;
                                   Fd: digital controller frequency, no larger than 5M for better performance
                                   interval: interval between 2 measurement trigger signal, the smallest interval should not be smaller than the ADC measurement period, the largest interval should not be larger than 4095 */
} adc_digi_config_t __attribute__((deprecated));
#endif  //#if CONFIG_IDF_TARGET_ESP32C3


#ifdef __cplusplus
}
#endif

/*
 * SPDX-FileCopyrightText: 2016-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*----------------------------------------------------------------------------------
        This file contains ESP32 and ESP32S2 Depricated ADC APIs and functions
-----------------------------------------------------------------------------------*/

#include "sdkconfig.h"
#include "esp_types.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "driver/rtc_io.h"
#include "hal/adc_hal.h"
#include "hal/adc_ll.h"
#include "hal/adc_types.h"
#ifdef CONFIG_PM_ENABLE
#include "esp_pm.h"
#endif
#include "freertos/FreeRTOS.h"

#include "driver/adc_i2s_legacy.h"
#include "driver/adc_types_legacy.h"

static __attribute__((unused)) const char *ADC_TAG = "ADC";

#define ADC_CHECK_RET(fun_ret) ({                  \
    if (fun_ret != ESP_OK) {                                \
        ESP_LOGE(ADC_TAG,"%s:%d\n",__FUNCTION__,__LINE__);  \
        return ESP_FAIL;                                    \
    }                                                       \
})

#define ADC_CHECK(a, str, ret_val) ({                                               \
    if (!(a)) {                                                                     \
        ESP_LOGE(ADC_TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str);                \
        return (ret_val);                                                           \
    }                                                                               \
})

#define ADC_CHANNEL_CHECK(periph, channel) ADC_CHECK(channel < SOC_ADC_CHANNEL_NUM(periph), "ADC"#periph" channel error", ESP_ERR_INVALID_ARG)
#define ADC_GET_IO_NUM(periph, channel) (adc_channel_io_map[periph][channel])

extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.
#define ADC_ENTER_CRITICAL()  portENTER_CRITICAL(&rtc_spinlock)
#define ADC_EXIT_CRITICAL()  portEXIT_CRITICAL(&rtc_spinlock)

#ifdef CONFIG_PM_ENABLE
esp_pm_lock_handle_t adc_digi_arbiter_lock = NULL;
#endif  //CONFIG_PM_ENABLE


#if CONFIG_IDF_TARGET_ESP32
/*---------------------------------------------------------------
        ESP32 Depricated ADC APIs and functions
---------------------------------------------------------------*/
#define DIG_ADC_OUTPUT_FORMAT_DEFUALT (ADC_DIGI_FORMAT_12BIT)
#define DIG_ADC_ATTEN_DEFUALT         (ADC_ATTEN_DB_11)
#define DIG_ADC_BIT_WIDTH_DEFUALT     (3)   //3 for ADC_WIDTH_BIT_12

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
} adc_digi_pattern_table_t;

/**
 * @brief ADC digital controller (DMA mode) output data format option.
 */
typedef enum {
    ADC_DIGI_FORMAT_12BIT,  /*!<ADC to DMA data format,                [15:12]-channel, [11: 0]-12 bits ADC data (`adc_digi_output_data_t`). Note: For single convert mode. */
    ADC_DIGI_FORMAT_11BIT,  /*!<ADC to DMA data format, [15]-adc unit, [14:11]-channel, [10: 0]-11 bits ADC data (`adc_digi_output_data_t`). Note: For multi or alter convert mode. */
    ADC_DIGI_FORMAT_MAX,
} adc_digi_format_t;

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
    uint32_t adc1_pattern_len;               /*!<Pattern table length for digital controller. Range: 0 ~ 16 (0: Don't change the pattern table setting).
                                                 The pattern table that defines the conversion rules for each SAR ADC. Each table has 16 items, in which channel selection,
                                                 resolution and attenuation are stored. When the conversion is started, the controller reads conversion rules from the
                                                 pattern table one by one. For each controller the scan sequence has at most 16 different rules before repeating itself. */
    uint32_t adc2_pattern_len;               /*!<Refer to ``adc1_pattern_len`` */
    adc_digi_pattern_table_t *adc1_pattern;  /*!<Pointer to pattern table for digital controller. The table size defined by `adc1_pattern_len`. */
    adc_digi_pattern_table_t *adc2_pattern;  /*!<Refer to `adc1_pattern` */
    adc_digi_convert_mode_t conv_mode;       /*!<ADC conversion mode for digital controller. See ``adc_digi_convert_mode_t``. */
    adc_digi_format_t format;                /*!<ADC output data format for digital controller. See ``adc_digi_format_t``. */
} adc_digi_config_t;

/**
 * Set adc output 16-bit-data format from digital controller.
 *
 * @param data_sel 1: [15] unit, [14:11] channel, [10:0] data, 11-bit-width at most. Only work under `ADC_LL_DIGI_CONV_BOTH_UNIT` or `ADC_LL_DIGI_CONV_ALTER_UNIT` mode.
 *                 0: [15:12] channel, [11:0] data, 12-bit-width at most. Only work under `ADC_LL_DIGI_CONV_ONLY_ADC1` or `ADC_LL_DIGI_CONV_ONLY_ADC2` mode
 * @note see `adc_ll_digi_pattern_table_t` for more detail of data bit width
 */
static inline void adc_ll_digi_set_output_format(bool data_sel)
{
    SYSCON.saradc_ctrl.data_sar_sel = data_sel;
}

static inline void adc_ll_digi_prepare_pattern_table(adc_unit_t adc_n, uint32_t pattern_index, adc_digi_pattern_table_t pattern)
{
    uint32_t tab;
    uint8_t index = pattern_index / 4;
    uint8_t offset = (pattern_index % 4) * 8;
    if (adc_n == ADC_UNIT_1) {
        tab = SYSCON.saradc_sar1_patt_tab[index];   // Read old register value
        tab &= (~(0xFF000000 >> offset));           // clear old data
        tab |= ((uint32_t)pattern.val << 24) >> offset; // Fill in the new data
        SYSCON.saradc_sar1_patt_tab[index] = tab;   // Write back
    } else { // adc_n == ADC_UNIT_2
        tab = SYSCON.saradc_sar2_patt_tab[index];   // Read old register value
        tab &= (~(0xFF000000 >> offset));           // clear old data
        tab |= ((uint32_t)pattern.val << 24) >> offset; // Fill in the new data
        SYSCON.saradc_sar2_patt_tab[index] = tab;   // Write back
    }
}

static void adc_digi_controller_reg_set(const adc_digi_config_t *cfg)
{
    /* On ESP32, only support ADC1 */
    switch (cfg->conv_mode) {
        case ADC_CONV_SINGLE_UNIT_1:
            adc_ll_digi_set_convert_mode(ADC_LL_DIGI_CONV_ONLY_ADC1);
            break;
        case ADC_CONV_SINGLE_UNIT_2:
            adc_ll_digi_set_convert_mode(ADC_LL_DIGI_CONV_ONLY_ADC2);
            break;
        case ADC_CONV_BOTH_UNIT:
            adc_ll_digi_set_convert_mode(ADC_LL_DIGI_CONV_BOTH_UNIT);
            break;
        case ADC_CONV_ALTER_UNIT:
            adc_ll_digi_set_convert_mode(ADC_LL_DIGI_CONV_ALTER_UNIT);
            break;
        default:
            abort();
    }

    if (cfg->conv_mode & ADC_CONV_SINGLE_UNIT_1) {
        adc_ll_set_controller(ADC_UNIT_1, ADC_LL_CTRL_DIG);
        if (cfg->adc1_pattern_len) {
            adc_ll_digi_clear_pattern_table(ADC_UNIT_1);
            adc_ll_digi_set_pattern_table_len(ADC_UNIT_1, cfg->adc1_pattern_len);
            for (uint32_t i = 0; i < cfg->adc1_pattern_len; i++) {
                adc_ll_digi_prepare_pattern_table(ADC_UNIT_1, i, cfg->adc1_pattern[i]);
            }
        }
    }
    if (cfg->conv_mode & ADC_CONV_SINGLE_UNIT_2) {
        adc_ll_set_controller(ADC_UNIT_2, ADC_LL_CTRL_DIG);
        if (cfg->adc2_pattern_len) {
            adc_ll_digi_clear_pattern_table(ADC_UNIT_2);
            adc_ll_digi_set_pattern_table_len(ADC_UNIT_2, cfg->adc2_pattern_len);
            for (uint32_t i = 0; i < cfg->adc2_pattern_len; i++) {
                adc_ll_digi_prepare_pattern_table(ADC_UNIT_2, i, cfg->adc2_pattern[i]);
            }
        }
    }
    adc_ll_digi_set_output_format(cfg->format);
    adc_ll_digi_convert_limit_enable(ADC_LL_DEFAULT_CONV_LIMIT_EN);
    adc_ll_digi_set_convert_limit_num(ADC_LL_DEFAULT_CONV_LIMIT_NUM);
    adc_ll_digi_set_data_source(ADC_I2S_DATA_SRC_ADC);
}

esp_err_t adc_set_i2s_data_source(adc_i2s_source_t src)
{
    ADC_CHECK((src == ADC_I2S_DATA_SRC_IO_SIG || src == ADC_I2S_DATA_SRC_ADC), "ADC i2s data source error", ESP_ERR_INVALID_ARG);
    ADC_ENTER_CRITICAL();
    adc_ll_digi_set_data_source(src);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

extern esp_err_t adc_common_gpio_init(adc_unit_t adc_unit, adc_channel_t channel);
esp_err_t adc_i2s_mode_init(adc_unit_t adc_unit, adc_channel_t channel)
{
    if (adc_unit == ADC_UNIT_1) {
        ADC_CHANNEL_CHECK(ADC_UNIT_1, channel);
    } else if (adc_unit == ADC_UNIT_2) {
        //ADC2 does not support DMA mode
        ADC_CHECK(false, "ADC2 not support DMA for now.", ESP_ERR_INVALID_ARG);
        ADC_CHANNEL_CHECK(ADC_UNIT_2, channel);
    }

    adc_digi_pattern_table_t adc1_pattern[1];
    adc_digi_pattern_table_t adc2_pattern[1];
    adc_digi_config_t dig_cfg = {
        .format = DIG_ADC_OUTPUT_FORMAT_DEFUALT,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
    };

    if (adc_unit == ADC_UNIT_1) {
        adc1_pattern[0].atten = DIG_ADC_ATTEN_DEFUALT;
        adc1_pattern[0].bit_width = DIG_ADC_BIT_WIDTH_DEFUALT;
        adc1_pattern[0].channel = channel;
        dig_cfg.adc1_pattern_len = 1;
        dig_cfg.adc1_pattern = adc1_pattern;
    } else if (adc_unit == ADC_UNIT_2) {
        adc2_pattern[0].atten = DIG_ADC_ATTEN_DEFUALT;
        adc2_pattern[0].bit_width = DIG_ADC_BIT_WIDTH_DEFUALT;
        adc2_pattern[0].channel = channel;
        dig_cfg.adc2_pattern_len = 1;
        dig_cfg.adc2_pattern = adc2_pattern;
    }
    adc_common_gpio_init(adc_unit, channel);
    ADC_ENTER_CRITICAL();
    adc_ll_digi_set_fsm_time(ADC_LL_FSM_RSTB_WAIT_DEFAULT, ADC_LL_FSM_START_WAIT_DEFAULT,
                             ADC_LL_FSM_STANDBY_WAIT_DEFAULT);
    adc_ll_set_sample_cycle(ADC_LL_SAMPLE_CYCLE_DEFAULT);
    adc_hal_pwdet_set_cct(ADC_LL_PWDET_CCT_DEFAULT);
    adc_ll_digi_output_invert(ADC_UNIT_1, ADC_LL_DIGI_DATA_INVERT_DEFAULT(ADC_UNIT_1));
    adc_ll_digi_output_invert(ADC_UNIT_2, ADC_LL_DIGI_DATA_INVERT_DEFAULT(ADC_UNIT_2));
    adc_ll_digi_set_clk_div(ADC_LL_DIGI_SAR_CLK_DIV_DEFAULT);
    adc_digi_controller_reg_set(&dig_cfg);
    ADC_EXIT_CRITICAL();

    return ESP_OK;
}

#endif  //#if CONFIG_IDF_TARGET_ESP32

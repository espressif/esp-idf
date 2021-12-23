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
#include "hal/adc_ll.h"
#include "hal/adc_types.h"
#ifdef CONFIG_PM_ENABLE
#include "esp_pm.h"
#endif
#include "adc.h"
#include "esp_private/adc_cali.h"
#include "freertos/FreeRTOS.h"

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include "driver/adc_types_deprecated.h"

static const char *ADC_TAG = "ADC";

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
#define ADC_MEAS_NUM_LIM_DEFAULT      (1)
#define ADC_MAX_MEAS_NUM_DEFAULT      (255)
#define DIG_ADC_OUTPUT_FORMAT_DEFUALT (ADC_DIGI_FORMAT_12BIT)
#define DIG_ADC_ATTEN_DEFUALT         (ADC_ATTEN_DB_11)
#define DIG_ADC_BIT_WIDTH_DEFUALT     (ADC_WIDTH_BIT_12)

esp_err_t adc_digi_init(void)
{
    ADC_ENTER_CRITICAL();
    adc_hal_init();
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_digi_deinit(void)
{
    adc_power_release();
    ADC_ENTER_CRITICAL();
    adc_hal_digi_deinit(NULL);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

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

static inline void adc_ll_digi_prepare_pattern_table(adc_ll_num_t adc_n, uint32_t pattern_index, adc_digi_pattern_table_t pattern)
{
    uint32_t tab;
    uint8_t index = pattern_index / 4;
    uint8_t offset = (pattern_index % 4) * 8;
    if (adc_n == ADC_NUM_1) {
        tab = SYSCON.saradc_sar1_patt_tab[index];   // Read old register value
        tab &= (~(0xFF000000 >> offset));           // clear old data
        tab |= ((uint32_t)pattern.val << 24) >> offset; // Fill in the new data
        SYSCON.saradc_sar1_patt_tab[index] = tab;   // Write back
    } else { // adc_n == ADC_NUM_2
        tab = SYSCON.saradc_sar2_patt_tab[index];   // Read old register value
        tab &= (~(0xFF000000 >> offset));           // clear old data
        tab |= ((uint32_t)pattern.val << 24) >> offset; // Fill in the new data
        SYSCON.saradc_sar2_patt_tab[index] = tab;   // Write back
    }
}

void adc_digi_controller_reg_set(const adc_digi_config_t *cfg)
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
        adc_ll_set_controller(ADC_NUM_1, ADC_LL_CTRL_DIG);
        if (cfg->adc1_pattern_len) {
            adc_ll_digi_clear_pattern_table(ADC_NUM_1);
            adc_ll_digi_set_pattern_table_len(ADC_NUM_1, cfg->adc1_pattern_len);
            for (uint32_t i = 0; i < cfg->adc1_pattern_len; i++) {
                adc_ll_digi_prepare_pattern_table(ADC_NUM_1, i, cfg->adc1_pattern[i]);
            }
        }
    }
    if (cfg->conv_mode & ADC_CONV_SINGLE_UNIT_2) {
        adc_ll_set_controller(ADC_NUM_2, ADC_LL_CTRL_DIG);
        if (cfg->adc2_pattern_len) {
            adc_ll_digi_clear_pattern_table(ADC_NUM_2);
            adc_ll_digi_set_pattern_table_len(ADC_NUM_2, cfg->adc2_pattern_len);
            for (uint32_t i = 0; i < cfg->adc2_pattern_len; i++) {
                adc_ll_digi_prepare_pattern_table(ADC_NUM_2, i, cfg->adc2_pattern[i]);
            }
        }
    }
    adc_ll_digi_set_output_format(cfg->format);
    if (cfg->conv_limit_en) {
        adc_ll_digi_set_convert_limit_num(cfg->conv_limit_num);
        adc_ll_digi_convert_limit_enable();
    } else {
        adc_ll_digi_convert_limit_disable();
    }
    adc_ll_digi_set_data_source(ADC_I2S_DATA_SRC_ADC);
}

esp_err_t adc_digi_controller_config(const adc_digi_config_t *config)
{
    adc_power_acquire();
    ADC_ENTER_CRITICAL();
    adc_digi_controller_reg_set(config);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_set_i2s_data_source(adc_i2s_source_t src)
{
    ADC_CHECK(src < ADC_I2S_DATA_SRC_MAX, "ADC i2s data source error", ESP_ERR_INVALID_ARG);
    ADC_ENTER_CRITICAL();
    adc_ll_digi_set_data_source(src);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

extern esp_err_t adc_common_gpio_init(adc_unit_t adc_unit, adc_channel_t channel);
esp_err_t adc_i2s_mode_init(adc_unit_t adc_unit, adc_channel_t channel)
{
    if (adc_unit & ADC_UNIT_1) {
        ADC_CHECK((SOC_ADC_SUPPORT_DMA_MODE(ADC_NUM_1)), "ADC1 not support DMA for now.", ESP_ERR_INVALID_ARG);
        ADC_CHANNEL_CHECK(ADC_NUM_1, channel);
    }
    if (adc_unit & ADC_UNIT_2) {
        ADC_CHECK((SOC_ADC_SUPPORT_DMA_MODE(ADC_NUM_2)), "ADC2 not support DMA for now.", ESP_ERR_INVALID_ARG);
        ADC_CHANNEL_CHECK(ADC_NUM_2, channel);
    }

    adc_digi_pattern_table_t adc1_pattern[1];
    adc_digi_pattern_table_t adc2_pattern[1];
    adc_digi_config_t dig_cfg = {
        .conv_limit_en = ADC_MEAS_NUM_LIM_DEFAULT,
        .conv_limit_num = ADC_MAX_MEAS_NUM_DEFAULT,
        .format = DIG_ADC_OUTPUT_FORMAT_DEFUALT,
        .conv_mode = (adc_digi_convert_mode_t)adc_unit,
    };

    if (adc_unit & ADC_UNIT_1) {
        adc1_pattern[0].atten = DIG_ADC_ATTEN_DEFUALT;
        adc1_pattern[0].bit_width = DIG_ADC_BIT_WIDTH_DEFUALT;
        adc1_pattern[0].channel = channel;
        dig_cfg.adc1_pattern_len = 1;
        dig_cfg.adc1_pattern = adc1_pattern;
    }
    if (adc_unit & ADC_UNIT_2) {
        adc2_pattern[0].atten = DIG_ADC_ATTEN_DEFUALT;
        adc2_pattern[0].bit_width = DIG_ADC_BIT_WIDTH_DEFUALT;
        adc2_pattern[0].channel = channel;
        dig_cfg.adc2_pattern_len = 1;
        dig_cfg.adc2_pattern = adc2_pattern;
    }
    adc_common_gpio_init(adc_unit, channel);
    ADC_ENTER_CRITICAL();
    adc_hal_init();
    adc_digi_controller_reg_set(&dig_cfg);
    ADC_EXIT_CRITICAL();

    return ESP_OK;
}

#endif  //#if CONFIG_IDF_TARGET_ESP32

#if CONFIG_IDF_TARGET_ESP32S2
/*---------------------------------------------------------------
            ESP32S2 Depricated ADC functions and APIs
---------------------------------------------------------------*/
esp_err_t adc_arbiter_config(adc_unit_t adc_unit, adc_arbiter_t *config)
{
    if (adc_unit & ADC_UNIT_1) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    ADC_ENTER_CRITICAL();
    adc_hal_arbiter_config(config);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

/**
 * Enable interrupt of adc digital controller by bitmask.
 *
 * @param adc_n ADC unit.
 * @param intr Interrupt bitmask.
 */
static inline void adc_ll_digi_intr_enable(adc_ll_num_t adc_n, adc_digi_intr_t intr)
{
    if (adc_n == ADC_NUM_1) {
        if (intr & ADC_DIGI_INTR_MASK_MONITOR) {
            APB_SARADC.int_ena.adc1_thres = 1;
        }
        if (intr & ADC_DIGI_INTR_MASK_MEAS_DONE) {
            APB_SARADC.int_ena.adc1_done = 1;
        }
    } else { // adc_n == ADC_NUM_2
        if (intr & ADC_DIGI_INTR_MASK_MONITOR) {
            APB_SARADC.int_ena.adc2_thres = 1;
        }
        if (intr & ADC_DIGI_INTR_MASK_MEAS_DONE) {
            APB_SARADC.int_ena.adc2_done = 1;
        }
    }
}

esp_err_t adc_digi_intr_enable(adc_unit_t adc_unit, adc_digi_intr_t intr_mask)
{
    ADC_ENTER_CRITICAL();
    if (adc_unit & ADC_UNIT_1) {
        adc_ll_digi_intr_enable(ADC_NUM_1, intr_mask);
    }
    if (adc_unit & ADC_UNIT_2) {
        adc_ll_digi_intr_enable(ADC_NUM_2, intr_mask);
    }
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

/**
 * Disable interrupt of adc digital controller by bitmask.
 *
 * @param adc_n ADC unit.
 * @param intr Interrupt bitmask.
 */
static inline void adc_ll_digi_intr_disable(adc_ll_num_t adc_n, adc_digi_intr_t intr)
{
    if (adc_n == ADC_NUM_1) {
        if (intr & ADC_DIGI_INTR_MASK_MONITOR) {
            APB_SARADC.int_ena.adc1_thres = 0;
        }
        if (intr & ADC_DIGI_INTR_MASK_MEAS_DONE) {
            APB_SARADC.int_ena.adc1_done = 0;
        }
    } else { // adc_n == ADC_NUM_2
        if (intr & ADC_DIGI_INTR_MASK_MONITOR) {
            APB_SARADC.int_ena.adc2_thres = 0;
        }
        if (intr & ADC_DIGI_INTR_MASK_MEAS_DONE) {
            APB_SARADC.int_ena.adc2_done = 0;
        }
    }
}

esp_err_t adc_digi_intr_disable(adc_unit_t adc_unit, adc_digi_intr_t intr_mask)
{
    ADC_ENTER_CRITICAL();
    if (adc_unit & ADC_UNIT_1) {
        adc_ll_digi_intr_disable(ADC_NUM_1, intr_mask);
    }
    if (adc_unit & ADC_UNIT_2) {
        adc_ll_digi_intr_disable(ADC_NUM_2, intr_mask);
    }
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

/**
 * Clear interrupt of adc digital controller by bitmask.
 *
 * @param adc_n ADC unit.
 * @param intr Interrupt bitmask.
 */
static inline void adc_ll_digi_intr_clear(adc_ll_num_t adc_n, adc_digi_intr_t intr)
{
    if (adc_n == ADC_NUM_1) {
        if (intr & ADC_DIGI_INTR_MASK_MONITOR) {
            APB_SARADC.int_clr.adc1_thres = 1;
        }
        if (intr & ADC_DIGI_INTR_MASK_MEAS_DONE) {
            APB_SARADC.int_clr.adc1_done = 1;
        }
    } else { // adc_n == ADC_NUM_2
        if (intr & ADC_DIGI_INTR_MASK_MONITOR) {
            APB_SARADC.int_clr.adc2_thres = 1;
        }
        if (intr & ADC_DIGI_INTR_MASK_MEAS_DONE) {
            APB_SARADC.int_clr.adc2_done = 1;
        }
    }
}

esp_err_t adc_digi_intr_clear(adc_unit_t adc_unit, adc_digi_intr_t intr_mask)
{
    ADC_ENTER_CRITICAL();
    if (adc_unit & ADC_UNIT_1) {
        adc_ll_digi_intr_clear(ADC_NUM_1, intr_mask);
    }
    if (adc_unit & ADC_UNIT_2) {
        adc_ll_digi_intr_clear(ADC_NUM_2, intr_mask);
    }
    ADC_EXIT_CRITICAL();

    return ESP_OK;
}

/**
 * Get interrupt status mask of adc digital controller.
 *
 * @param adc_n ADC unit.
 * @return
 *     - intr Interrupt bitmask.
 */
static inline uint32_t adc_ll_digi_get_intr_status(adc_ll_num_t adc_n)
{
    uint32_t int_st = APB_SARADC.int_st.val;
    uint32_t ret_msk = 0;

    if (adc_n == ADC_NUM_1) {
        if (int_st & APB_SARADC_ADC1_DONE_INT_ST_M) {
            ret_msk |= ADC_DIGI_INTR_MASK_MEAS_DONE;
        }
        if (int_st & APB_SARADC_ADC1_THRES_INT_ST) {
            ret_msk |= ADC_DIGI_INTR_MASK_MONITOR;
        }
    } else { // adc_n == ADC_NUM_2
        if (int_st & APB_SARADC_ADC2_DONE_INT_ST_M) {
            ret_msk |= ADC_DIGI_INTR_MASK_MEAS_DONE;
        }
        if (int_st & APB_SARADC_ADC2_THRES_INT_ST_M) {
            ret_msk |= ADC_DIGI_INTR_MASK_MONITOR;
        }
    }

    return ret_msk;
}

uint32_t adc_digi_intr_get_status(adc_unit_t adc_unit)
{
    uint32_t ret = 0;
    ADC_ENTER_CRITICAL();
    if (adc_unit & ADC_UNIT_1) {
        ret = adc_ll_digi_get_intr_status(ADC_NUM_1);
    }
    if (adc_unit & ADC_UNIT_2) {
        ret = adc_ll_digi_get_intr_status(ADC_NUM_2);
    }
    ADC_EXIT_CRITICAL();
    return ret;
}

static uint8_t s_isr_registered = 0;
static intr_handle_t s_adc_isr_handle = NULL;

esp_err_t adc_digi_isr_register(void (*fn)(void *), void *arg, int intr_alloc_flags)
{
    ADC_CHECK((fn != NULL), "Parameter error", ESP_ERR_INVALID_ARG);
    ADC_CHECK(s_isr_registered == 0, "ADC ISR have installed, can not install again", ESP_FAIL);

    esp_err_t ret = esp_intr_alloc(ETS_APB_ADC_INTR_SOURCE, intr_alloc_flags, fn, arg, &s_adc_isr_handle);
    if (ret == ESP_OK) {
        s_isr_registered = 1;
    }
    return ret;
}

esp_err_t adc_digi_isr_deregister(void)
{
    esp_err_t ret = ESP_FAIL;
    if (s_isr_registered) {
        ret = esp_intr_free(s_adc_isr_handle);
        if (ret == ESP_OK) {
            s_isr_registered = 0;
        }
    }
    return ret;
}

esp_err_t adc_digi_init(void)
{
    adc_arbiter_t config = ADC_ARBITER_CONFIG_DEFAULT();
    ADC_ENTER_CRITICAL();
    adc_hal_init();
    adc_hal_arbiter_config(&config);
    ADC_EXIT_CRITICAL();

    adc_hal_calibration_init(ADC_NUM_1);
    adc_hal_calibration_init(ADC_NUM_2);

    return ESP_OK;
}

esp_err_t adc_digi_deinit(void)
{
#ifdef CONFIG_PM_ENABLE
    if (adc_digi_arbiter_lock) {
        esp_pm_lock_delete(adc_digi_arbiter_lock);
        adc_digi_arbiter_lock = NULL;
    }
#endif
    adc_power_release();
    ADC_ENTER_CRITICAL();
    adc_hal_digi_deinit(NULL);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

/**
 * @brief Reset FSM of adc digital controller.
 *
 * @return
 *      - ESP_OK Success
 */
esp_err_t adc_digi_reset(void)
{
    ADC_ENTER_CRITICAL();
    adc_ll_digi_reset();
    adc_ll_digi_clear_pattern_table(ADC_NUM_1);
    adc_ll_digi_clear_pattern_table(ADC_NUM_2);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

/**
 * Set adc output data format for digital controller.
 *
 * @param format Output data format.
 */
static inline void adc_ll_digi_set_output_format(adc_digi_output_format_t format)
{
    APB_SARADC.ctrl.data_sar_sel = format;
}

static inline void adc_ll_digi_prepare_pattern_table(adc_ll_num_t adc_n, uint32_t pattern_index, adc_digi_pattern_table_t pattern)
{
    uint32_t tab;
    uint8_t index = pattern_index / 4;
    uint8_t offset = (pattern_index % 4) * 8;
    if (adc_n == ADC_NUM_1) {
        tab = APB_SARADC.sar1_patt_tab[index];  // Read old register value
        tab &= (~(0xFF000000 >> offset));       // clear old data
        tab |= ((uint32_t)pattern.val << 24) >> offset; // Fill in the new data
        APB_SARADC.sar1_patt_tab[index] = tab;  // Write back
    } else { // adc_n == ADC_NUM_2
        tab = APB_SARADC.sar2_patt_tab[index];  // Read old register value
        tab &= (~(0xFF000000 >> offset));       // clear old data
        tab |= ((uint32_t)pattern.val << 24) >> offset; // Fill in the new data
        APB_SARADC.sar2_patt_tab[index] = tab;  // Write back
    }
}

static void adc_digi_controller_reg_set(const adc_digi_config_t *cfg)
{
    /* Single channel mode or multi channel mode. */
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
        if (cfg->adc1_pattern_len) {
            adc_ll_digi_clear_pattern_table(ADC_NUM_1);
            adc_ll_digi_set_pattern_table_len(ADC_NUM_1, cfg->adc1_pattern_len);
            for (uint32_t i = 0; i < cfg->adc1_pattern_len; i++) {
                adc_ll_digi_prepare_pattern_table(ADC_NUM_1, i, cfg->adc1_pattern[i]);
            }
        }
    }
    if (cfg->conv_mode & ADC_CONV_SINGLE_UNIT_2) {
        if (cfg->adc2_pattern_len) {
            adc_ll_digi_clear_pattern_table(ADC_NUM_2);
            adc_ll_digi_set_pattern_table_len(ADC_NUM_2, cfg->adc2_pattern_len);
            for (uint32_t i = 0; i < cfg->adc2_pattern_len; i++) {
                adc_ll_digi_prepare_pattern_table(ADC_NUM_2, i, cfg->adc2_pattern[i]);
            }
        }
    }
    if (cfg->conv_mode & ADC_CONV_SINGLE_UNIT_1) {
        adc_ll_set_controller(ADC_NUM_1, ADC_LL_CTRL_DIG);
    }
    if (cfg->conv_mode & ADC_CONV_SINGLE_UNIT_2) {
        adc_ll_set_controller(ADC_NUM_2, ADC_LL_CTRL_ARB);
    }
    adc_ll_digi_set_output_format(cfg->format);
    if (cfg->conv_limit_en) {
        adc_ll_digi_set_convert_limit_num(cfg->conv_limit_num);
        adc_ll_digi_convert_limit_enable();
    } else {
        adc_ll_digi_convert_limit_disable();
    }

    adc_ll_digi_set_trigger_interval(cfg->interval);
    adc_ll_digi_controller_clk_div(cfg->dig_clk.div_num, cfg->dig_clk.div_b, cfg->dig_clk.div_a);
    adc_ll_digi_clk_sel(cfg->dig_clk.use_apll);
    adc_ll_digi_dma_set_eof_num(cfg->dma_eof_num);
}

esp_err_t adc_digi_controller_config(const adc_digi_config_t *config)
{
#ifdef CONFIG_PM_ENABLE
    esp_err_t err;
    if (adc_digi_arbiter_lock == NULL) {
        if (config->dig_clk.use_apll) {
            err = esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "adc_dma", &adc_digi_arbiter_lock);
        } else {
            err = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "adc_dma", &adc_digi_arbiter_lock);
        }
        if (err != ESP_OK) {
            adc_digi_arbiter_lock = NULL;
            ESP_LOGE(ADC_TAG, "ADC-DMA pm lock error");
            return err;
        }
    }
#endif //CONFIG_PM_ENABLE

    if (config->conv_mode & ADC_CONV_SINGLE_UNIT_1) {
        for (int i = 0; i < config->adc1_pattern_len; i++) {
            adc_cal_offset(ADC_NUM_1, config->adc1_pattern[i].channel, config->adc1_pattern[i].atten);
        }
    }
    if (config->conv_mode & ADC_CONV_SINGLE_UNIT_2) {
        for (int i = 0; i < config->adc2_pattern_len; i++) {
            adc_cal_offset(ADC_NUM_2, config->adc2_pattern[i].channel, config->adc2_pattern[i].atten);
        }
    }

    /* If enable digtal controller, adc xpd should always on. */
    adc_power_acquire();
    ADC_ENTER_CRITICAL();
    adc_digi_controller_reg_set(config);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}
#endif // #if CONFIG_IDF_TARGET_ESP32S2


#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3
/*---------------------------------------------------------------
            ESP32S2 Depricated ADC functions and APIs
---------------------------------------------------------------*/
esp_err_t adc_gpio_init(adc_unit_t adc_unit, adc_channel_t channel)
{
    gpio_num_t gpio_num = 0;
    //If called with `ADC_UNIT_BOTH (ADC_UNIT_1 | ADC_UNIT_2)`, both if blocks will be run
    if (adc_unit & ADC_UNIT_1) {
        ADC_CHANNEL_CHECK(ADC_NUM_1, channel);
        gpio_num = ADC_GET_IO_NUM(ADC_NUM_1, channel);
        ADC_CHECK_RET(rtc_gpio_init(gpio_num));
        ADC_CHECK_RET(rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_DISABLED));
        ADC_CHECK_RET(rtc_gpio_pulldown_dis(gpio_num));
        ADC_CHECK_RET(rtc_gpio_pullup_dis(gpio_num));
    }
    if (adc_unit & ADC_UNIT_2) {
        ADC_CHANNEL_CHECK(ADC_NUM_2, channel);
        gpio_num = ADC_GET_IO_NUM(ADC_NUM_2, channel);
        ADC_CHECK_RET(rtc_gpio_init(gpio_num));
        ADC_CHECK_RET(rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_DISABLED));
        ADC_CHECK_RET(rtc_gpio_pulldown_dis(gpio_num));
        ADC_CHECK_RET(rtc_gpio_pullup_dis(gpio_num));
    }

    return ESP_OK;
}
#endif //#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3

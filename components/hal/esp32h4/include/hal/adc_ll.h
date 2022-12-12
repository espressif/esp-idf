/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include "esp_attr.h"

#include "soc/adc_periph.h"
#include "hal/adc_types.h"
#include "hal/adc_types_private.h"
#include "soc/apb_saradc_struct.h"
#include "soc/apb_saradc_reg.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/rtc_cntl_reg.h"
#include "hal/misc.h"
#include "hal/regi2c_ctrl.h"

#include "soc/regi2c_saradc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADC_LL_CLKM_DIV_NUM_DEFAULT       15
#define ADC_LL_CLKM_DIV_B_DEFAULT         1
#define ADC_LL_CLKM_DIV_A_DEFAULT         0
#define ADC_LL_DEFAULT_CONV_LIMIT_EN      0
#define ADC_LL_DEFAULT_CONV_LIMIT_NUM     10

#define ADC_LL_EVENT_ADC1_ONESHOT_DONE    BIT(31)
#define ADC_LL_EVENT_ADC2_ONESHOT_DONE    BIT(30)

typedef enum {
    ADC_POWER_BY_FSM,   /*!< ADC XPD controled by FSM. Used for polling mode */
    ADC_POWER_SW_ON,    /*!< ADC XPD controled by SW. power on. Used for DMA mode */
    ADC_POWER_SW_OFF,   /*!< ADC XPD controled by SW. power off. */
    ADC_POWER_MAX,      /*!< For parameter check. */
} adc_ll_power_t;

typedef enum {
    ADC_RTC_DATA_OK = 0,
    ADC_RTC_CTRL_UNSELECTED = 1,
    ADC_RTC_CTRL_BREAK = 2,
    ADC_RTC_DATA_FAIL = -1,
} adc_ll_rtc_raw_data_t;

typedef enum {
    ADC_LL_CTRL_DIG = 0,    ///< For ADC1. Select DIG controller.
    ADC_LL_CTRL_ARB = 1,    ///< For ADC2. The controller is selected by the arbiter.
} adc_ll_controller_t;

/**
 * @brief ADC digital controller (DMA mode) work mode.
 *
 * @note  The conversion mode affects the sampling frequency:
 *        ESP32H4 only support ALTER_UNIT mode
 *        ALTER_UNIT   : When the measurement is triggered, ADC1 or ADC2 samples alternately.
 */
typedef enum {
    ADC_LL_DIGI_CONV_ALTER_UNIT = 0,     // Use both ADC1 and ADC2 for conversion by turn. e.g. ADC1 -> ADC2 -> ADC1 -> ADC2 .....
} adc_ll_digi_convert_mode_t;

//These values should be set according to the HW
typedef enum {
    ADC_LL_INTR_THRES1_LOW  = BIT(26),
    ADC_LL_INTR_THRES0_LOW  = BIT(27),
    ADC_LL_INTR_THRES1_HIGH = BIT(28),
    ADC_LL_INTR_THRES0_HIGH = BIT(29),
    ADC_LL_INTR_ADC2_DONE   = BIT(30),
    ADC_LL_INTR_ADC1_DONE   = BIT(31),
} adc_ll_intr_t;
FLAG_ATTR(adc_ll_intr_t)

typedef struct  {
    union {
        struct {
            uint8_t atten:      2;
            uint8_t channel:    3;
            uint8_t unit:       1;
            uint8_t reserved:   2;
        };
        uint8_t val;
    };
} __attribute__((packed)) adc_ll_digi_pattern_table_t;

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/

/**
 * Set adc fsm interval parameter for digital controller. These values are fixed for same platforms.
 *
 * @param rst_wait cycles between DIG ADC controller reset ADC sensor and start ADC sensor.
 * @param start_wait Delay time after open xpd.
 * @param standby_wait Delay time to close xpd.
 */
static inline void adc_ll_digi_set_fsm_time(uint32_t rst_wait, uint32_t start_wait, uint32_t standby_wait)
{
    // Internal FSM reset wait time
    HAL_FORCE_MODIFY_U32_REG_FIELD(APB_SARADC.fsm_wait, rstb_wait, rst_wait);
    // Internal FSM start wait time
    HAL_FORCE_MODIFY_U32_REG_FIELD(APB_SARADC.fsm_wait, xpd_wait, start_wait);
    // Internal FSM standby wait time
    HAL_FORCE_MODIFY_U32_REG_FIELD(APB_SARADC.fsm_wait, standby_wait, standby_wait);
}

/**
 * Set adc sample cycle for digital controller.
 *
 * @note Normally, please use default value.
 * @param sample_cycle Cycles between DIG ADC controller start ADC sensor and beginning to receive data from sensor.
 *                     Range: 2 ~ 0xFF.
 */
static inline void adc_ll_set_sample_cycle(uint32_t sample_cycle)
{
    /* Should be called before writing I2C registers. */
    SET_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_SAR_I2C_PU);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_SAMPLE_CYCLE_ADDR, sample_cycle);
}

/**
 * Set SAR ADC module clock division factor.
 * SAR ADC clock divided from digital controller clock.
 *
 * @param div Division factor.
 */
static inline void adc_ll_digi_set_clk_div(uint32_t div)
{
    /* ADC clock devided from digital controller clock clk */
    HAL_FORCE_MODIFY_U32_REG_FIELD(APB_SARADC.ctrl, sar_clk_div, div);
}

/**
 * Set adc max conversion number for digital controller.
 * If the number of ADC conversion is equal to the maximum, the conversion is stopped.
 *
 * @param meas_num Max conversion number. Range: 0 ~ 255.
 */
static inline void adc_ll_digi_set_convert_limit_num(uint32_t meas_num)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(APB_SARADC.ctrl2, max_meas_num, meas_num);
}

/**
 * Enable max conversion number detection for digital controller.
 * If the number of ADC conversion is equal to the maximum, the conversion is stopped.
 *
 * @param enable  true: enable; false: disable
 */
static inline void adc_ll_digi_convert_limit_enable(bool enable)
{
    APB_SARADC.ctrl2.meas_num_limit = enable;
}

/**
 * Set adc conversion mode for digital controller.
 *
 * @note ESP32H4 only support ADC1 single mode.
 *
 * @param mode Conversion mode select.
 */
static inline void adc_ll_digi_set_convert_mode(adc_ll_digi_convert_mode_t mode)
{
    //ESP32H4 only supports ADC_CONV_ALTER_UNIT mode
}

/**
 * Set pattern table length for digital controller.
 * The pattern table that defines the conversion rules for each SAR ADC. Each table has 8 items, in which channel selection,
 * and attenuation are stored. When the conversion is started, the controller reads conversion rules from the
 * pattern table one by one. For each controller the scan sequence has at most 8 different rules before repeating itself.
 *
 * @param adc_n ADC unit.
 * @param patt_len Items range: 1 ~ 8.
 */
static inline void adc_ll_digi_set_pattern_table_len(adc_unit_t adc_n, uint32_t patt_len)
{
    APB_SARADC.ctrl.sar_patt_len = patt_len - 1;
}

/**
 * Set pattern table for digital controller.
 * The pattern table that defines the conversion rules for each SAR ADC. Each table has 8 items, in which channel selection,
 * resolution and attenuation are stored. When the conversion is started, the controller reads conversion rules from the
 * pattern table one by one. For each controller the scan sequence has at most 8 different rules before repeating itself.
 *
 * @param adc_n ADC unit.
 * @param pattern_index Items index. Range: 0 ~ 7.
 * @param pattern Stored conversion rules.
 */
static inline void adc_ll_digi_set_pattern_table(adc_unit_t adc_n, uint32_t pattern_index, adc_digi_pattern_config_t table)
{
    uint32_t tab;
    uint8_t index = pattern_index / 4;
    uint8_t offset = (pattern_index % 4) * 6;
    adc_ll_digi_pattern_table_t pattern = {0};

    pattern.val = (table.atten & 0x3) | ((table.channel & 0x7) << 2) | ((table.unit & 0x1) << 5);
    tab = APB_SARADC.sar_patt_tab[index].sar_patt_tab1;         // Read old register value
    tab &= (~(0xFC0000 >> offset));                             // Clear old data
    tab |= ((uint32_t)(pattern.val & 0x3F) << 18) >> offset;    // Fill in the new data
    APB_SARADC.sar_patt_tab[index].sar_patt_tab1 = tab;         // Write back
}

/**
 * Reset the pattern table pointer, then take the measurement rule from table header in next measurement.
 *
 * @param adc_n ADC unit.
 */
static inline void adc_ll_digi_clear_pattern_table(adc_unit_t adc_n)
{
    APB_SARADC.ctrl.sar_patt_p_clear = 1;
    APB_SARADC.ctrl.sar_patt_p_clear = 0;
}

/**
 * Sets the number of cycles required for the conversion to complete and wait for the arbiter to stabilize.
 *
 * @note Only ADC2 have arbiter function.
 * @param cycle range: 0 ~ 4.
 */
static inline void adc_ll_digi_set_arbiter_stable_cycle(uint32_t cycle)
{
    APB_SARADC.ctrl.wait_arb_cycle = cycle;
}

/**
 * ADC Digital controller output data invert or not.
 *
 * @param adc_n ADC unit.
 * @param inv_en data invert or not.
 */
static inline void adc_ll_digi_output_invert(adc_unit_t adc_n, bool inv_en)
{
    if (adc_n == ADC_UNIT_1) {
        APB_SARADC.ctrl2.sar1_inv = inv_en;   // Enable / Disable ADC data invert
    } else { // adc_n == ADC_UNIT_2
        APB_SARADC.ctrl2.sar2_inv = inv_en;   // Enable / Disable ADC data invert
    }
}

/**
 * Set the interval clock cycle for the digital controller to trigger the measurement.
 * Expression: `trigger_meas_freq` = `controller_clk` / 2 / interval.
 *
 * @note The trigger interval should not be smaller than the sampling time of the SAR ADC.
 * @param cycle The clock cycle (trigger interval) of the measurement. Range: 30 ~ 4095.
 */
static inline void adc_ll_digi_set_trigger_interval(uint32_t cycle)
{
    APB_SARADC.ctrl2.timer_target = cycle;
}

/**
 * Enable digital controller timer to trigger the measurement.
 */
static inline void adc_ll_digi_trigger_enable(void)
{
    APB_SARADC.ctrl2.timer_en = 1;
}

/**
 * Disable digital controller timer to trigger the measurement.
 */
static inline void adc_ll_digi_trigger_disable(void)
{
    APB_SARADC.ctrl2.timer_en = 0;
}

/**
 * Set ADC digital controller clock division factor. The clock divided from `APLL` or `APB` clock.
 * Expression: controller_clk = (APLL or APB) / (div_num + div_a / div_b + 1).
 *
 * @param div_num Division factor. Range: 0 ~ 255.
 * @param div_b Division factor. Range: 1 ~ 63.
 * @param div_a Division factor. Range: 0 ~ 63.
 */
static inline void adc_ll_digi_controller_clk_div(uint32_t div_num, uint32_t div_b, uint32_t div_a)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(APB_SARADC.apb_adc_clkm_conf, clkm_div_num, div_num);
    APB_SARADC.apb_adc_clkm_conf.clkm_div_b = div_b;
    APB_SARADC.apb_adc_clkm_conf.clkm_div_a = div_a;
}

/**
 * Enable clock and select clock source for ADC digital controller.
 *
 * @param use_apll true: use APLL clock; false: use APB clock.
 */
static inline void adc_ll_digi_clk_sel(bool use_apll)
{
    if (use_apll) {
        APB_SARADC.apb_adc_clkm_conf.clk_sel = 1;   // APLL clock
    } else {
        APB_SARADC.apb_adc_clkm_conf.clk_sel = 2;   // APB clock
    }
    APB_SARADC.ctrl.sar_clk_gated = 1;
}

/**
 * Disable clock for ADC digital controller.
 */
static inline void adc_ll_digi_controller_clk_disable(void)
{
    APB_SARADC.ctrl.sar_clk_gated = 0;
}

/**
 * Reset adc digital controller filter.
 *
 * @param adc_n ADC unit.
 */
static inline void adc_ll_digi_filter_reset(adc_unit_t adc_n)
{
    APB_SARADC.filter_ctrl0.filter_reset = 1;
}

/**
 * Set adc digital controller filter factor.
 *
 * @note If the channel info is not supported, the filter function will not be enabled.
 * @param idx ADC filter unit.
 * @param filter Filter config. Expression: filter_data = (k-1)/k * last_data + new_data / k. Set values: (2, 4, 8, 16, 64).
 */
static inline void adc_ll_digi_filter_set_factor(adc_digi_filter_idx_t idx, adc_digi_filter_t *filter)
{
    if (idx == ADC_DIGI_FILTER_IDX0) {
        APB_SARADC.filter_ctrl0.filter_channel0 = (filter->adc_unit << 3) | (filter->channel & 0x7);
        APB_SARADC.filter_ctrl1.filter_factor0 = filter->mode;
    } else if (idx == ADC_DIGI_FILTER_IDX1) {
        APB_SARADC.filter_ctrl0.filter_channel1 = (filter->adc_unit << 3) | (filter->channel & 0x7);
        APB_SARADC.filter_ctrl1.filter_factor1 = filter->mode;
    }
}

/**
 * Get adc digital controller filter factor.
 *
 * @param adc_n ADC unit.
 * @param factor Expression: filter_data = (k-1)/k * last_data + new_data / k. Set values: (2, 4, 8, 16, 64).
 */
static inline void adc_ll_digi_filter_get_factor(adc_digi_filter_idx_t idx, adc_digi_filter_t *filter)
{
    if (idx == ADC_DIGI_FILTER_IDX0) {
        filter->adc_unit = (APB_SARADC.filter_ctrl0.filter_channel0 >> 3) & 0x1;
        filter->channel = APB_SARADC.filter_ctrl0.filter_channel0 & 0x7;
        filter->mode = APB_SARADC.filter_ctrl1.filter_factor0;
    } else if (idx == ADC_DIGI_FILTER_IDX1) {
        filter->adc_unit = (APB_SARADC.filter_ctrl0.filter_channel1 >> 3) & 0x1;
        filter->channel = APB_SARADC.filter_ctrl0.filter_channel1 & 0x7;
        filter->mode = APB_SARADC.filter_ctrl1.filter_factor1;
    }
}

/**
 * Disable adc digital controller filter.
 * Filtering the ADC data to obtain smooth data at higher sampling rates.
 *
 * @note If the channel info is not supported, the filter function will not be enabled.
 * @param adc_n ADC unit.
 */
static inline void adc_ll_digi_filter_disable(adc_digi_filter_idx_t idx)
{
    if (idx == ADC_DIGI_FILTER_IDX0) {
        APB_SARADC.filter_ctrl0.filter_channel0 = 0xF;
        APB_SARADC.filter_ctrl1.filter_factor0 = 0;
    } else if (idx == ADC_DIGI_FILTER_IDX1) {
        APB_SARADC.filter_ctrl0.filter_channel1 = 0xF;
        APB_SARADC.filter_ctrl1.filter_factor1 = 0;
    }
}

/**
 * Set monitor mode of adc digital controller.
 *
 * @note If the channel info is not supported, the monitor function will not be enabled.
 * @param adc_n ADC unit.
 * @param is_larger true:  If ADC_OUT >  threshold, Generates monitor interrupt.
 *                  false: If ADC_OUT <  threshold, Generates monitor interrupt.
 */
static inline void adc_ll_digi_monitor_set_mode(adc_digi_monitor_idx_t idx, adc_digi_monitor_t *cfg)
{
    if (idx == ADC_DIGI_MONITOR_IDX0) {
        APB_SARADC.thres0_ctrl.thres0_channel = (cfg->adc_unit << 3) | (cfg->channel & 0x7);
        APB_SARADC.thres0_ctrl.thres0_high = cfg->h_threshold;
        APB_SARADC.thres0_ctrl.thres0_low = cfg->l_threshold;
    } else { // ADC_DIGI_MONITOR_IDX1
        APB_SARADC.thres1_ctrl.thres1_channel = (cfg->adc_unit << 3) | (cfg->channel & 0x7);
        APB_SARADC.thres1_ctrl.thres1_high = cfg->h_threshold;
        APB_SARADC.thres1_ctrl.thres1_low = cfg->l_threshold;
    }
}

/**
 * Enable/disable monitor of adc digital controller.
 *
 * @note If the channel info is not supported, the monitor function will not be enabled.
 * @param adc_n ADC unit.
 */
static inline void adc_ll_digi_monitor_disable(adc_digi_monitor_idx_t idx)
{
    if (idx == ADC_DIGI_MONITOR_IDX0) {
        APB_SARADC.thres0_ctrl.thres0_channel = 0xF;
    } else { // ADC_DIGI_MONITOR_IDX1
        APB_SARADC.thres1_ctrl.thres1_channel = 0xF;
    }
}

/**
 * Set DMA eof num of adc digital controller.
 * If the number of measurements reaches `dma_eof_num`, then `dma_in_suc_eof` signal is generated.
 *
 * @param num eof num of DMA.
 */
static inline void adc_ll_digi_dma_set_eof_num(uint32_t num)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(APB_SARADC.dma_conf, apb_adc_eof_num, num);
}

/**
 * Enable output data to DMA from adc digital controller.
 */
static inline void adc_ll_digi_dma_enable(void)
{
    APB_SARADC.dma_conf.apb_adc_trans = 1;
}

/**
 * Disable output data to DMA from adc digital controller.
 */
static inline void adc_ll_digi_dma_disable(void)
{
    APB_SARADC.dma_conf.apb_adc_trans = 0;
}

/**
 * Reset adc digital controller.
 */
static inline void adc_ll_digi_reset(void)
{
    APB_SARADC.dma_conf.apb_adc_reset_fsm = 1;
    APB_SARADC.dma_conf.apb_adc_reset_fsm = 0;
}

/*---------------------------------------------------------------
                    PWDET(Power detect) controller setting
---------------------------------------------------------------*/
/**
 * Set adc cct for PWDET controller.
 *
 * @note Capacitor tuning of the PA power monitor. cct set to the same value with PHY.
 * @param cct Range: 0 ~ 7.
 */
static inline void adc_ll_pwdet_set_cct(uint32_t cct)
{
    /* Capacitor tuning of the PA power monitor. cct set to the same value with PHY. */
    abort();
}

/**
 * Get adc cct for PWDET controller.
 *
 * @note Capacitor tuning of the PA power monitor. cct set to the same value with PHY.
 * @return cct Range: 0 ~ 7.
 */
static inline uint32_t adc_ll_pwdet_get_cct(void)
{
    /* Capacitor tuning of the PA power monitor. cct set to the same value with PHY. */
    abort();
}

/**
 * Analyze whether the obtained raw data is correct.
 * ADC2 can use arbiter. The arbitration result is stored in the channel information of the returned data.
 *
 * @param adc_n ADC unit.
 * @param raw_data ADC raw data input (convert value).
 * @return
 *        -  0: The data is correct to use.
 *        - -1: The data is invalid.
 */
static inline adc_ll_rtc_raw_data_t adc_ll_analysis_raw_data(adc_unit_t adc_n, int raw_data)
{
    if (adc_n == ADC_UNIT_1) {
        return ADC_RTC_DATA_OK;
    }

    //The raw data API returns value without channel information. Read value directly from the register
    if (((APB_SARADC.apb_saradc2_data_status.adc2_data >> 13) & 0xF) > 9) {
        return ADC_RTC_DATA_FAIL;
    }

    return ADC_RTC_DATA_OK;
}

/*---------------------------------------------------------------
                    Common setting
---------------------------------------------------------------*/
/**
 * Set ADC module power management.
 *
 * @param manage Set ADC power status.
 */
static inline void adc_ll_set_power_manage(adc_ll_power_t manage)
{
    /* Bit1  0:Fsm  1: SW mode
       Bit0  0:SW mode power down  1: SW mode power on */
    if (manage == ADC_POWER_SW_ON) {
        APB_SARADC.ctrl.sar_clk_gated = 1;
        APB_SARADC.ctrl.xpd_sar_force = 3;
    } else if (manage == ADC_POWER_BY_FSM) {
        APB_SARADC.ctrl.sar_clk_gated = 1;
        APB_SARADC.ctrl.xpd_sar_force = 0;
    } else if (manage == ADC_POWER_SW_OFF) {
        APB_SARADC.ctrl.sar_clk_gated = 0;
        APB_SARADC.ctrl.xpd_sar_force = 2;
    }
}

__attribute__((always_inline))
static inline void adc_ll_set_controller(adc_unit_t adc_n, adc_ll_controller_t ctrl)
{
    //Not used on ESP32H4
}

/**
 * Set ADC2 module arbiter work mode.
 * The arbiter is to improve the use efficiency of ADC2. After the control right is robbed by the high priority,
 * the low priority controller will read the invalid ADC data, and the validity of the data can be judged by the flag bit in the data.
 *
 * @note Only ADC2 support arbiter.
 * @note The arbiter's working clock is APB_CLK. When the APB_CLK clock drops below 8 MHz, the arbiter must be in shield mode.
 *
 * @param mode Refer to `adc_arbiter_mode_t`.
 */
__attribute__((always_inline))
static inline void adc_ll_set_arbiter_work_mode(adc_arbiter_mode_t mode)
{
    if (mode == ADC_ARB_MODE_FIX) {
        APB_SARADC.apb_adc_arb_ctrl.adc_arb_grant_force = 0;
        APB_SARADC.apb_adc_arb_ctrl.adc_arb_fix_priority = 1;
    } else if (mode == ADC_ARB_MODE_LOOP) {
        APB_SARADC.apb_adc_arb_ctrl.adc_arb_grant_force = 0;
        APB_SARADC.apb_adc_arb_ctrl.adc_arb_fix_priority = 0;
    } else {
        APB_SARADC.apb_adc_arb_ctrl.adc_arb_grant_force = 1;    // Shield arbiter.
    }
}

/**
 * Set ADC2 module controller priority in arbiter.
 * The arbiter is to improve the use efficiency of ADC2. After the control right is robbed by the high priority,
 * the low priority controller will read the invalid ADC data, and the validity of the data can be judged by the flag bit in the data.
 *
 * @note Only ADC2 support arbiter.
 * @note The arbiter's working clock is APB_CLK. When the APB_CLK clock drops below 8 MHz, the arbiter must be in shield mode.
 * @note Default priority: Wi-Fi(2) > RTC(1) > Digital(0);
 *
 * @param pri_rtc RTC controller priority. Range: 0 ~ 2.
 * @param pri_dig Digital controller priority. Range: 0 ~ 2.
 * @param pri_pwdet Wi-Fi controller priority. Range: 0 ~ 2.
 */
__attribute__((always_inline))
static inline void adc_ll_set_arbiter_priority(uint8_t pri_rtc, uint8_t pri_dig, uint8_t pri_pwdet)
{
    if (pri_rtc != pri_dig && pri_rtc != pri_pwdet && pri_dig != pri_pwdet) {
        APB_SARADC.apb_adc_arb_ctrl.adc_arb_rtc_priority = pri_rtc;
        APB_SARADC.apb_adc_arb_ctrl.adc_arb_apb_priority = pri_dig;
        APB_SARADC.apb_adc_arb_ctrl.adc_arb_wifi_priority = pri_pwdet;
    }
    /* Should select highest priority controller. */
    if (pri_rtc > pri_dig) {
        if (pri_rtc > pri_pwdet) {
            APB_SARADC.apb_adc_arb_ctrl.adc_arb_apb_force = 0;
            APB_SARADC.apb_adc_arb_ctrl.adc_arb_rtc_force = 1;
            APB_SARADC.apb_adc_arb_ctrl.adc_arb_wifi_force = 0;
        } else {
            APB_SARADC.apb_adc_arb_ctrl.adc_arb_apb_force = 0;
            APB_SARADC.apb_adc_arb_ctrl.adc_arb_rtc_force = 0;
            APB_SARADC.apb_adc_arb_ctrl.adc_arb_wifi_force = 1;
        }
    } else {
        if (pri_dig > pri_pwdet) {
            APB_SARADC.apb_adc_arb_ctrl.adc_arb_apb_force = 1;
            APB_SARADC.apb_adc_arb_ctrl.adc_arb_rtc_force = 0;
            APB_SARADC.apb_adc_arb_ctrl.adc_arb_wifi_force = 0;
        } else {
            APB_SARADC.apb_adc_arb_ctrl.adc_arb_apb_force = 0;
            APB_SARADC.apb_adc_arb_ctrl.adc_arb_rtc_force = 0;
            APB_SARADC.apb_adc_arb_ctrl.adc_arb_wifi_force = 1;
        }
    }
}

/* ADC calibration code. */
/**
 * @brief Set common calibration configuration. Should be shared with other parts (PWDET).
 */
__attribute__((always_inline))
static inline void adc_ll_calibration_init(adc_unit_t adc_n)
{
    if (adc_n == ADC_UNIT_1) {
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_DREF_ADDR, 1);
    } else {
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR2_DREF_ADDR, 1);
    }
}

/**
 * Configure the registers for ADC calibration. You need to call the ``adc_ll_calibration_finish`` interface to resume after calibration.
 *
 * @note  Different ADC units and different attenuation options use different calibration data (initial data).
 *
 * @param adc_n ADC index number.
 * @param channel adc channel number.
 * @param internal_gnd true:  Disconnect from the IO port and use the internal GND as the calibration voltage.
 *                     false: Use IO external voltage as calibration voltage.
 */
static inline void adc_ll_calibration_prepare(adc_unit_t adc_n, adc_channel_t channel, bool internal_gnd)
{
    /* Enable/disable internal connect GND (for calibration). */
    if (adc_n == ADC_UNIT_1) {
        if (internal_gnd) {
            REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_ENCAL_GND_ADDR, 1);
        } else {
            REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_ENCAL_GND_ADDR, 0);
        }
    } else {
        if (internal_gnd) {
            REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR2_ENCAL_GND_ADDR, 1);
        } else {
            REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR2_ENCAL_GND_ADDR, 0);
        }
    }
}

/**
 * Resume register status after calibration.
 *
 * @param adc_n ADC index number.
 */
static inline void adc_ll_calibration_finish(adc_unit_t adc_n)
{
    if (adc_n == ADC_UNIT_1) {
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_ENCAL_GND_ADDR, 0);
    } else {
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR2_ENCAL_GND_ADDR, 0);
    }
}

/**
 * Set the calibration result to ADC.
 *
 * @note  Different ADC units and different attenuation options use different calibration data (initial data).
 *
 * @param adc_n ADC index number.
 */
__attribute__((always_inline))
static inline void adc_ll_set_calibration_param(adc_unit_t adc_n, uint32_t param)
{
    uint8_t msb = param >> 8;
    uint8_t lsb = param & 0xFF;
    if (adc_n == ADC_UNIT_1) {
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_INITIAL_CODE_HIGH_ADDR, msb);
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_INITIAL_CODE_LOW_ADDR, lsb);
    } else {
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR2_INITIAL_CODE_HIGH_ADDR, msb);
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR2_INITIAL_CODE_LOW_ADDR, lsb);
    }
}
/* Temp code end. */

/**
 *  Output ADCn inter reference voltage to ADC2 channels.
 *
 *  This function routes the internal reference voltage of ADCn to one of
 *  ADC1's channels. This reference voltage can then be manually measured
 *  for calibration purposes.
 *
 *  @param[in]  adc ADC unit select
 *  @param[in]  channel ADC1 channel number
 *  @param[in]  en Enable/disable the reference voltage output
 */
static inline void adc_ll_vref_output(adc_unit_t adc, adc_channel_t channel, bool en)
{
    abort();
}

/*---------------------------------------------------------------
                    Single Read
---------------------------------------------------------------*/
/**
 * Set adc output data format for oneshot mode
 *
 * @note ESP32C3 Oneshot mode only supports 12bit.
 * @param adc_n ADC unit.
 * @param bits  Output data bits width option.
 */
static inline void adc_oneshot_ll_set_output_bits(adc_unit_t adc_n, adc_bitwidth_t bits)
{
    abort();  //TODO IDF-3908
    // //ESP32C3 only supports 12bit, leave here for compatibility
    // HAL_ASSERT(bits == ADC_BITWIDTH_12);
}

/**
 * Enable adc channel to start convert.
 *
 * @note Only one channel can be selected for measurement.
 *
 * @param adc_n   ADC unit.
 * @param channel ADC channel number for each ADCn.
 */
static inline void adc_oneshot_ll_set_channel(adc_unit_t adc_n, adc_channel_t channel)
{
    abort();  //TODO IDF-3908
    // APB_SARADC.onetime_sample.onetime_channel = ((adc_n << 3) | channel);
}

/**
 * Disable adc channel to start convert.
 *
 * @note Only one channel can be selected in once measurement.
 *
 * @param adc_n ADC unit.
 */
static inline void adc_oneshot_ll_disable_channel(adc_unit_t adc_n)
{
    abort();  //TODO IDF-3908
    // if (adc_n == ADC_UNIT_1) {
    //     APB_SARADC.onetime_sample.onetime_channel = ((adc_n << 3) | 0xF);
    // } else { // adc_n == ADC_UNIT_2
    //     APB_SARADC.onetime_sample.onetime_channel = ((adc_n << 3) | 0x1);
    // }
}

/**
 * Start oneshot conversion by software
 *
 * @param val Usage: set to 1 to start the ADC conversion. The step signal should at least keep 3 ADC digital controller clock cycle,
 *            otherwise the step signal may not be captured by the ADC digital controller when its frequency is slow.
 *            This hardware limitation will be removed in future versions.
 */
static inline void adc_oneshot_ll_start(bool val)
{
    abort();  //TODO IDF-3908
    // APB_SARADC.onetime_sample.onetime_start = val;
}

/**
 * Clear the event for each ADCn for Oneshot mode
 *
 * @param event ADC event
 */
static inline void adc_oneshot_ll_clear_event(uint32_t event_mask)
{
    abort();  //TODO IDF-3908
    // APB_SARADC.int_clr.val |= event_mask;
}

/**
 * Check the event for each ADCn for Oneshot mode
 *
 * @param event ADC event
 *
 * @return
 *      -true  : The conversion process is finish.
 *      -false : The conversion process is not finish.
 */
static inline bool adc_oneshot_ll_get_event(uint32_t event_mask)
{
    abort();  //TODO IDF-3908
    // return (APB_SARADC.int_raw.val & event_mask);
}

/**
 * Get the converted value for each ADCn for RTC controller.
 *
 * @param adc_n ADC unit.
 * @return
 *      - Converted value.
 */
static inline uint32_t adc_oneshot_ll_get_raw_result(adc_unit_t adc_n)
{
    abort();  //TODO IDF-3908
    // uint32_t ret_val = 0;
    // if (adc_n == ADC_UNIT_1) {
    //     ret_val = APB_SARADC.apb_saradc1_data_status.adc1_data & 0xfff;
    // } else { // adc_n == ADC_UNIT_2
    //     ret_val = APB_SARADC.apb_saradc2_data_status.adc2_data & 0xfff;
    // }
    // return ret_val;
}

/**
 * Analyze whether the obtained raw data is correct.
 * ADC2 can use arbiter. The arbitration result is stored in the channel information of the returned data.
 *
 * @param adc_n    ADC unit.
 * @param raw_data ADC raw data input (convert value).
 * @return
 *        - 1: The data is correct to use.
 *        - 0: The data is invalid.
 */
static inline bool adc_oneshot_ll_raw_check_valid(adc_unit_t adc_n, uint32_t raw_data)
{
    abort();  //TODO IDF-3908
    // if (adc_n == ADC_UNIT_1) {
    //     return true;
    // }

    // //The raw data API returns value without channel information. Read value directly from the register
    // if (((APB_SARADC.apb_saradc2_data_status.adc2_data >> 13) & 0xF) > 9) {
    //     return false;
    // }

    // return true;
}

/**
 * ADC module RTC output data invert or not.
 *
 * @param adc_n ADC unit.
 * @param inv_en data invert or not.
 */
static inline void adc_oneshot_ll_output_invert(adc_unit_t adc_n, bool inv_en)
{
    abort();  //TODO IDF-3908
    // (void)adc_n;
    // (void)inv_en;
    // //For compatibility
}

/**
 * Enable oneshot conversion trigger
 *
 * @param adc_n  ADC unit
 */
static inline void adc_oneshot_ll_enable(adc_unit_t adc_n)
{
    abort();  //TODO IDF-3908
    // if (adc_n == ADC_UNIT_1) {
    //     APB_SARADC.onetime_sample.adc1_onetime_sample = 1;
    // } else {
    //     APB_SARADC.onetime_sample.adc2_onetime_sample = 1;
    // }
}

/**
 * Disable oneshot conversion trigger for all the ADC units
 */
static inline void adc_oneshot_ll_disable_all_unit(void)
{
    abort();  //TODO IDF-3908
    // APB_SARADC.onetime_sample.adc1_onetime_sample = 0;
    // APB_SARADC.onetime_sample.adc2_onetime_sample = 0;
}

/**
 * Set attenuation
 *
 * @note Attenuation is for all channels
 *
 * @param adc_n   ADC unit
 * @param channel ADC channel
 * @param atten   ADC attenuation
 */
static inline void adc_oneshot_ll_set_atten(adc_unit_t adc_n, adc_channel_t channel, adc_atten_t atten)
{
    abort();  //TODO IDF-3908
    // (void)adc_n;
    // (void)channel;
    // // Attenuation is for all channels, unit and channel are for compatibility
    // APB_SARADC.onetime_sample.onetime_atten = atten;
}
#ifdef __cplusplus
}
#endif

// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include "regi2c_ctrl.h"
#include "esp_attr.h"

#include "soc/adc_periph.h"
#include "hal/adc_types.h"
#include "soc/apb_saradc_struct.h"
#include "soc/apb_saradc_reg.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/rtc_cntl_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADC_LL_CLKM_DIV_NUM_DEFAULT 15
#define ADC_LL_CLKM_DIV_B_DEFAULT   1
#define ADC_LL_CLKM_DIV_A_DEFAULT   0

typedef enum {
    ADC_NUM_1 = 0,          /*!< SAR ADC 1 */
    ADC_NUM_2 = 1,          /*!< SAR ADC 2 */
    ADC_NUM_MAX,
} adc_ll_num_t;

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

/**
 * @brief ADC controller type selection.
 *
 * @note For ADC2, use the force option with care. The system power consumption detection will use ADC2.
 *       If it is forced to switch to another controller, it may cause the system to obtain incorrect values.
 * @note Normally, there is no need to switch the controller manually.
 */
typedef enum {
    ADC_CTRL_RTC = 0,   /*!<For ADC1. Select RTC controller. For ADC2. The controller is selected by the arbiter. Arbiter in default mode. */
    ADC_CTRL_DIG = 2,   /*!<For ADC1. Select DIG controller. For ADC2. The controller is selected by the arbiter. Arbiter in default mode. */
    ADC2_CTRL_PWDET = 3,/*!<For ADC2. The controller is selected by the arbiter. Arbiter in default mode. */
    ADC2_CTRL_FORCE_PWDET = 3,  /*!<For ADC2. Arbiter in shield mode. Force select Wi-Fi controller work. */
    ADC2_CTRL_FORCE_RTC = 4,    /*!<For ADC2. Arbiter in shield mode. Force select RTC controller work. */
    ADC2_CTRL_FORCE_DIG = 6,    /*!<For ADC2. Arbiter in shield mode. Force select digital controller work. */
} adc_ll_controller_t;

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
    APB_SARADC.fsm_wait.rstb_wait = rst_wait;
    // Internal FSM start wait time
    APB_SARADC.fsm_wait.xpd_wait = start_wait;
    // Internal FSM standby wait time
    APB_SARADC.fsm_wait.standby_wait = standby_wait;
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
    APB_SARADC.ctrl.sar_clk_div = div;
}

/**
 * Set adc max conversion number for digital controller.
 * If the number of ADC conversion is equal to the maximum, the conversion is stopped.
 *
 * @param meas_num Max conversion number. Range: 0 ~ 255.
 */
static inline void adc_ll_digi_set_convert_limit_num(uint32_t meas_num)
{
    APB_SARADC.ctrl2.max_meas_num = meas_num;
}

/**
 * Enable max conversion number detection for digital controller.
 * If the number of ADC conversion is equal to the maximum, the conversion is stopped.
 */
static inline void adc_ll_digi_convert_limit_enable(void)
{
    APB_SARADC.ctrl2.meas_num_limit = 1;
}

/**
 * Disable max conversion number detection for digital controller.
 * If the number of ADC conversion is equal to the maximum, the conversion is stopped.
 */
static inline void adc_ll_digi_convert_limit_disable(void)
{
    APB_SARADC.ctrl2.meas_num_limit = 0;
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
static inline void adc_ll_digi_set_pattern_table_len(adc_ll_num_t adc_n, uint32_t patt_len)
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
static inline void adc_ll_digi_set_pattern_table(adc_ll_num_t adc_n, uint32_t pattern_index, adc_digi_pattern_table_t pattern)
{
    uint32_t tab;
    uint8_t index = pattern_index / 4;
    uint8_t offset = (pattern_index % 4) * 6;

    tab = APB_SARADC.sar_patt_tab[index].sar_patt_tab1;  // Read old register value
    tab &= (~(0xFC0000 >> offset));                      // Clear old data
    tab |= ((uint32_t)(pattern.val & 0x3F) << 18) >> offset;       // Fill in the new data
    APB_SARADC.sar_patt_tab[index].sar_patt_tab1 = tab;  // Write back
}

/**
 * Reset the pattern table pointer, then take the measurement rule from table header in next measurement.
 *
 * @param adc_n ADC unit.
 */
static inline void adc_ll_digi_clear_pattern_table(adc_ll_num_t adc_n)
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
static inline void adc_ll_digi_output_invert(adc_ll_num_t adc_n, bool inv_en)
{
    if (adc_n == ADC_NUM_1) {
        APB_SARADC.ctrl2.sar1_inv = inv_en;   // Enable / Disable ADC data invert
    } else { // adc_n == ADC_NUM_2
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
 * Expression: controller_clk = APLL/APB * (div_num  + div_b / div_a).
 *
 * @param div_num Division factor. Range: 1 ~ 255.
 * @param div_b Division factor. Range: 1 ~ 63.
 * @param div_a Division factor. Range: 0 ~ 63.
 */
static inline void adc_ll_digi_controller_clk_div(uint32_t div_num, uint32_t div_b, uint32_t div_a)
{
    APB_SARADC.apb_adc_clkm_conf.clkm_div_num = div_num;
    APB_SARADC.apb_adc_clkm_conf.clkm_div_b = div_b;
    APB_SARADC.apb_adc_clkm_conf.clkm_div_a = div_a;
}

/**
 * Enable clock and select clock source for ADC digital controller.
 *
 * @param use_apll true: use APLL clock; false: use APB clock.
 */
static inline void adc_ll_digi_controller_clk_enable(bool use_apll)
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
static inline void adc_ll_digi_filter_reset(adc_ll_num_t adc_n)
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
    APB_SARADC.dma_conf.apb_adc_eof_num = num;
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
    RTCCNTL.sensor_ctrl.sar2_pwdet_cct = cct;
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
    return RTCCNTL.sensor_ctrl.sar2_pwdet_cct;
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
static inline adc_ll_rtc_raw_data_t adc_ll_analysis_raw_data(adc_ll_num_t adc_n, int raw_data)
{
    if (adc_n == ADC_NUM_1) {
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
    // /* Bit1  0:Fsm  1: SW mode
    //    Bit0  0:SW mode power down  1: SW mode power on */
    if (manage == ADC_POWER_SW_ON) {
        APB_SARADC.ctrl.sar_clk_gated = 1;
        APB_SARADC.ctrl.xpd_sar_force = 3;
    } else if (manage == ADC_POWER_BY_FSM) {
        APB_SARADC.ctrl.sar_clk_gated = 1;
        APB_SARADC.ctrl.xpd_sar_force = 0;
    } else if (manage == ADC_POWER_SW_OFF) {
        APB_SARADC.ctrl.xpd_sar_force = 2;
        APB_SARADC.ctrl.sar_clk_gated = 0;
    }
}

/**
 * Get ADC module power management.
 *
 * @return
 *      - ADC power status.
 */
static inline adc_ll_power_t adc_ll_get_power_manage(void)
{
    /* Bit1  0:Fsm  1: SW mode
       Bit0  0:SW mode power down  1: SW mode power on */
    adc_ll_power_t manage;
    if (APB_SARADC.ctrl.xpd_sar_force == 3) {
        manage = ADC_POWER_SW_ON;
    } else if (APB_SARADC.ctrl.xpd_sar_force == 2) {
        manage = ADC_POWER_SW_OFF;
    } else {
        manage = ADC_POWER_BY_FSM;
    }
    return manage;
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
static inline void adc_ll_calibration_init(adc_ll_num_t adc_n)
{
    if (adc_n == ADC_NUM_1) {
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
static inline void adc_ll_calibration_prepare(adc_ll_num_t adc_n, adc_channel_t channel, bool internal_gnd)
{
    /* Enable/disable internal connect GND (for calibration). */
    if (adc_n == ADC_NUM_1) {
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
static inline void adc_ll_calibration_finish(adc_ll_num_t adc_n)
{
    if (adc_n == ADC_NUM_1) {
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
static inline void adc_ll_set_calibration_param(adc_ll_num_t adc_n, uint32_t param)
{
    uint8_t msb = param >> 8;
    uint8_t lsb = param & 0xFF;
    if (adc_n == ADC_NUM_1) {
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
static inline void adc_ll_vref_output(adc_ll_num_t adc, adc_channel_t channel, bool en)
{
    if (en) {
        REG_SET_FIELD(RTC_CNTL_SENSOR_CTRL_REG, RTC_CNTL_FORCE_XPD_SAR, 3);
        SET_PERI_REG_MASK(RTC_CNTL_REG, RTC_CNTL_REGULATOR_FORCE_PU);

        REG_SET_FIELD(APB_SARADC_APB_ADC_CLKM_CONF_REG, APB_SARADC_CLK_SEL, 2);
        SET_PERI_REG_MASK(APB_SARADC_APB_ADC_CLKM_CONF_REG, APB_SARADC_CLK_EN);
        SET_PERI_REG_MASK(APB_SARADC_APB_ADC_ARB_CTRL_REG, APB_SARADC_ADC_ARB_GRANT_FORCE);
        SET_PERI_REG_MASK(APB_SARADC_APB_ADC_ARB_CTRL_REG, APB_SARADC_ADC_ARB_APB_FORCE);
        APB_SARADC.sar_patt_tab[0].sar_patt_tab1 = 0xFFFFFF;
        APB_SARADC.sar_patt_tab[1].sar_patt_tab1 = 0xFFFFFF;
        APB_SARADC.onetime_sample.adc1_onetime_sample = 1;
        APB_SARADC.onetime_sample.onetime_channel = channel;
        SET_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_SAR_I2C_PU);
        if (adc == ADC_NUM_1) {
            /* Config test mux to route v_ref to ADC1 Channels */
            REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC1_ENCAL_REF_ADDR, 1);
            REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_DTEST_RTC_ADDR, 1);
            REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENT_TSENS_ADDR, 0);
            REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENT_RTC_ADDR, 1);
        } else {
            /* Config test mux to route v_ref to ADC2 Channels */
            REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC2_ENCAL_REF_ADDR, 1);
            REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_DTEST_RTC_ADDR, 0);
            REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENT_TSENS_ADDR, 0);
            REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENT_RTC_ADDR, 0);
        }
    } else {
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC2_ENCAL_REF_ADDR, 0);
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC1_ENCAL_REF_ADDR, 0);
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_DTEST_RTC_ADDR, 0);
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENT_RTC_ADDR, 0);
        APB_SARADC.onetime_sample.adc1_onetime_sample = 0;
        APB_SARADC.onetime_sample.onetime_channel = 0xf;
        REG_SET_FIELD(RTC_CNTL_SENSOR_CTRL_REG, RTC_CNTL_FORCE_XPD_SAR, 0);
        REG_SET_FIELD(APB_SARADC_APB_ADC_CLKM_CONF_REG, APB_SARADC_CLK_SEL, 0);
        CLEAR_PERI_REG_MASK(APB_SARADC_APB_ADC_CLKM_CONF_REG, APB_SARADC_CLK_EN);
        CLEAR_PERI_REG_MASK(APB_SARADC_APB_ADC_ARB_CTRL_REG, APB_SARADC_ADC_ARB_GRANT_FORCE);
        CLEAR_PERI_REG_MASK(APB_SARADC_APB_ADC_ARB_CTRL_REG, APB_SARADC_ADC_ARB_APB_FORCE);
    }
}

/*---------------------------------------------------------------
                    Single Read
---------------------------------------------------------------*/
/**
 * Trigger single read
 *
 * @param val Usage: set to 1 to start the ADC conversion. The step signal should at least keep 3 ADC digital controller clock cycle,
 *            otherwise the step signal may not be captured by the ADC digital controller when its frequency is slow.
 *            This hardware limitation will be removed in future versions.
 */
static inline void adc_ll_onetime_start(bool val)
{
    APB_SARADC.onetime_sample.onetime_start = val;
}

static inline void adc_ll_onetime_set_channel(adc_ll_num_t unit, adc_channel_t channel)
{
    APB_SARADC.onetime_sample.onetime_channel = ((unit << 3) | channel);
}

static inline void adc_ll_onetime_set_atten(adc_atten_t atten)
{
    APB_SARADC.onetime_sample.onetime_atten = atten;
}

static inline void adc_ll_intr_enable(adc_ll_intr_t mask)
{
    APB_SARADC.int_ena.val |= mask;
}

static inline void adc_ll_intr_disable(adc_ll_intr_t mask)
{
    APB_SARADC.int_ena.val &= ~mask;
}

static inline void adc_ll_intr_clear(adc_ll_intr_t mask)
{
    APB_SARADC.int_clr.val |= mask;
}

static inline bool adc_ll_intr_get_raw(adc_ll_intr_t mask)
{
    return (APB_SARADC.int_raw.val & mask);
}

static inline bool adc_ll_intr_get_status(adc_ll_intr_t mask)
{
    return (APB_SARADC.int_st.val & mask);
}

static inline void adc_ll_onetime_sample_enable(adc_ll_num_t adc_n, bool enable)
{
    if (adc_n == ADC_NUM_1) {
        APB_SARADC.onetime_sample.adc1_onetime_sample = enable;
    } else {
        APB_SARADC.onetime_sample.adc2_onetime_sample = enable;
    }
}

static inline uint32_t adc_ll_adc1_read(void)
{
    //On ESP32C3, valid data width is 12-bit
    return (APB_SARADC.apb_saradc1_data_status.adc1_data & 0xfff);
}

static inline uint32_t adc_ll_adc2_read(void)
{
    //On ESP32C3, valid data width is 12-bit
    return (APB_SARADC.apb_saradc2_data_status.adc2_data & 0xfff);
}

#ifdef __cplusplus
}
#endif

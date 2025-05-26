/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include "esp_attr.h"

#include "soc/adc_periph.h"
#include "soc/apb_saradc_struct.h"
#include "soc/apb_saradc_reg.h"
#include "soc/pmu_reg.h"
#include "soc/clk_tree_defs.h"
#include "soc/pcr_struct.h"
#include "hal/misc.h"
#include "hal/assert.h"
#include "hal/adc_types.h"
#include "hal/adc_types_private.h"
#include "hal/regi2c_ctrl.h"
#include "hal/sar_ctrl_ll.h"

#include "soc/regi2c_saradc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADC_LL_EVENT_ADC1_ONESHOT_DONE    BIT(31)
#define ADC_LL_EVENT_ADC2_ONESHOT_DONE    BIT(30)

#define ADC_LL_THRES_ALL_INTR_ST_M  (SARADC_THRES0_HIGH_INT_ST_M | \
                                     SARADC_THRES1_HIGH_INT_ST_M | \
                                     SARADC_THRES0_LOW_INT_ST_M  | \
                                     SARADC_THRES1_LOW_INT_ST_M)

#define ADC_LL_GET_HIGH_THRES_MASK(monitor_id)    ((monitor_id == 0) ? SARADC_THRES0_HIGH_INT_ST_M : SARADC_THRES1_HIGH_INT_ST_M)
#define ADC_LL_GET_LOW_THRES_MASK(monitor_id)     ((monitor_id == 0) ? SARADC_THRES0_LOW_INT_ST_M : SARADC_THRES1_LOW_INT_ST_M)

/*---------------------------------------------------------------
                    Oneshot
---------------------------------------------------------------*/
#define ADC_LL_DATA_INVERT_DEFAULT(PERIPH_NUM)         (0)
#define ADC_LL_DELAY_CYCLE_AFTER_DONE_SIGNAL           (0)

/*---------------------------------------------------------------
                    DMA
---------------------------------------------------------------*/
#define ADC_LL_DIGI_DATA_INVERT_DEFAULT(PERIPH_NUM)    (0)
#define ADC_LL_FSM_RSTB_WAIT_DEFAULT                   (8)
#define ADC_LL_FSM_START_WAIT_DEFAULT                  (5)
#define ADC_LL_FSM_STANDBY_WAIT_DEFAULT                (100)
#define ADC_LL_SAMPLE_CYCLE_DEFAULT                    (2)
#define ADC_LL_DIGI_SAR_CLK_DIV_DEFAULT                (1)
#define ADC_LL_CLKM_DIV_NUM_DEFAULT       15
#define ADC_LL_CLKM_DIV_B_DEFAULT         1
#define ADC_LL_CLKM_DIV_A_DEFAULT         0
#define ADC_LL_DEFAULT_CONV_LIMIT_EN      0
#define ADC_LL_DEFAULT_CONV_LIMIT_NUM     10

#define ADC_LL_POWER_MANAGE_SUPPORTED     1 //ESP32C61 supported to manage power mode
/*---------------------------------------------------------------
                    PWDET (Power Detect)
---------------------------------------------------------------*/
#define ADC_LL_PWDET_CCT_DEFAULT                       (4)

typedef enum {
    ADC_LL_POWER_BY_FSM = SAR_CTRL_LL_POWER_FSM,   /*!< ADC XPD controlled by FSM. Used for polling mode */
    ADC_LL_POWER_SW_ON = SAR_CTRL_LL_POWER_ON,    /*!< ADC XPD controlled by SW. power on. Used for DMA mode */
    ADC_LL_POWER_SW_OFF = SAR_CTRL_LL_POWER_OFF,   /*!< ADC XPD controlled by SW. power off. */
} adc_ll_power_t;

typedef enum {
    ADC_LL_CTRL_DIG = 0,    ///< ADC digital controller
} adc_ll_controller_t;

/**
 * @brief ADC digital controller (DMA mode) work mode.
 *
 * @note  The conversion mode affects the sampling frequency:
 *        ESP32C61 only support ONLY_ADC1 mode
 *        SINGLE_UNIT_1: When the measurement is triggered, only ADC1 is sampled once.
 */
typedef enum {
    ADC_LL_DIGI_CONV_ONLY_ADC1 = 0,     // Only use ADC1 for conversion
} adc_ll_digi_convert_mode_t;

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
    HAL_FORCE_MODIFY_U32_REG_FIELD(ADC.saradc_fsm_wait, saradc_rstb_wait, rst_wait);
    // Internal FSM start wait time
    HAL_FORCE_MODIFY_U32_REG_FIELD(ADC.saradc_fsm_wait, saradc_xpd_wait, start_wait);
    // Internal FSM standby wait time
    HAL_FORCE_MODIFY_U32_REG_FIELD(ADC.saradc_fsm_wait, saradc_standby_wait, standby_wait);
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
    /* Peripheral reg i2c has powered up in rtc_init, write directly */
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_SAMPLE_CYCLE_ADDR, sample_cycle);
}

/**
 * Set SAR ADC module clock division factor.
 * SAR ADC clock divided from digital controller clock.
 *
 * @param div Division factor.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_set_clk_div(uint32_t div)
{
    /* ADC clock divided from digital controller clock clk */
    HAL_FORCE_MODIFY_U32_REG_FIELD(ADC.saradc_ctrl, saradc_sar_clk_div, div);
}

/**
 * Set adc max conversion number for digital controller.
 * If the number of ADC conversion is equal to the maximum, the conversion is stopped.
 *
 * @param meas_num Max conversion number. Range: 0 ~ 255.
 */
static inline void adc_ll_digi_set_convert_limit_num(uint32_t meas_num)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(ADC.saradc_ctrl2, saradc_max_meas_num, meas_num);
}

/**
 * Enable max conversion number detection for digital controller.
 * If the number of ADC conversion is equal to the maximum, the conversion is stopped.
 *
 * @param enable  true: enable; false: disable
 */
static inline void adc_ll_digi_convert_limit_enable(bool enable)
{
    ADC.saradc_ctrl2.saradc_meas_num_limit = enable;
}

/**
 * Set adc conversion mode for digital controller.
 *
 * @note ESP32C61 only support ADC1 single mode.
 *
 * @param mode Conversion mode select.
 */
static inline void adc_ll_digi_set_convert_mode(adc_ll_digi_convert_mode_t mode)
{
    //ESP32C61 only supports ADC_LL_DIGI_CONV_ONLY_ADC1 mode
}

/**
 * Set pattern table length for digital controller.
 * The pattern table that defines the conversion rules for each SAR ADC. Each table has 4 items, in which channel selection,
 * and attenuation are stored. When the conversion is started, the controller reads conversion rules from the
 * pattern table one by one. For each controller the scan sequence has at most 8 different rules before repeating itself.
 *
 * @param adc_n ADC unit.
 * @param patt_len Items range: 1 ~ 8.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_set_pattern_table_len(adc_unit_t adc_n, uint32_t patt_len)
{
    ADC.saradc_ctrl.saradc_sar_patt_len = patt_len - 1;
}

/**
 * Set pattern table for digital controller.
 * The pattern table that defines the conversion rules for each SAR ADC. Each table has 4 items, in which channel selection,
 * resolution and attenuation are stored. When the conversion is started, the controller reads conversion rules from the
 * pattern table one by one. For each controller the scan sequence has at most 8 different rules before repeating itself.
 *
 * @param adc_n ADC unit.
 * @param pattern_index Items index. Range: 0 ~ 7.
 * @param pattern Stored conversion rules.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_set_pattern_table(adc_unit_t adc_n, uint32_t pattern_index, adc_digi_pattern_config_t table)
{
    uint32_t tab;
    uint8_t index = pattern_index / 4;
    uint8_t offset = (pattern_index % 4) * 6;
    adc_ll_digi_pattern_table_t pattern = {0};

    pattern.val = (table.atten & 0x3) | ((table.channel & 0x7) << 2) | ((table.unit & 0x1) << 5);
    if (index == 0) {
        tab = ADC.saradc_sar_patt_tab1.saradc_sar_patt_tab1;         // Read old register value
        tab &= (~(0xFC0000 >> offset));                             // Clear old data
        tab |= ((uint32_t)(pattern.val & 0x3F) << 18) >> offset;    // Fill in the new data
        ADC.saradc_sar_patt_tab1.saradc_sar_patt_tab1 = tab;         // Write back
    } else {
        tab = ADC.saradc_sar_patt_tab2.saradc_sar_patt_tab2;         // Read old register value
        tab &= (~(0xFC0000 >> offset));                             // Clear old data
        tab |= ((uint32_t)(pattern.val & 0x3F) << 18) >> offset;    // Fill in the new data
        ADC.saradc_sar_patt_tab2.saradc_sar_patt_tab2 = tab;         // Write back
    }
}

/**
 * Rest pattern table to default value
 */
static inline void adc_ll_digi_reset_pattern_table(void)
{
    ADC.saradc_sar_patt_tab1.saradc_sar_patt_tab1 = 0xffffff;
    ADC.saradc_sar_patt_tab2.saradc_sar_patt_tab2 = 0xffffff;
}

/**
 * Reset the pattern table pointer, then take the measurement rule from table header in next measurement.
 *
 * @param adc_n ADC unit.
 */
static inline void adc_ll_digi_clear_pattern_table(adc_unit_t adc_n)
{
    ADC.saradc_ctrl.saradc_sar_patt_p_clear = 1;
    ADC.saradc_ctrl.saradc_sar_patt_p_clear = 0;
}

/**
 * Sets the number of cycles required for the conversion to complete and wait for the arbiter to stabilize.
 *
 * @note Only ADC2 have arbiter function.
 * @param cycle range: 0 ~ 4.
 */
static inline void adc_ll_digi_set_arbiter_stable_cycle(uint32_t cycle)
{
    ADC.saradc_ctrl.saradc_wait_arb_cycle = cycle;
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
        ADC.saradc_ctrl2.saradc_sar1_inv = inv_en;   // Enable / Disable ADC data invert
    } else { // adc_n == ADC_UNIT_2
        ADC.saradc_ctrl2.saradc_sar2_inv = inv_en;   // Enable / Disable ADC data invert
    }
}

/**
 * Set the interval clock cycle for the digital controller to trigger the measurement.
 * Expression: `trigger_meas_freq` = `controller_clk` / 2 / interval.
 *
 * @note The trigger interval should not be smaller than the sampling time of the SAR ADC.
 * @param cycle The clock cycle (trigger interval) of the measurement. Range: 30 ~ 4095.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_set_trigger_interval(uint32_t cycle)
{
    ADC.saradc_ctrl2.saradc_timer_target = cycle;
}

/**
 * Enable digital controller timer to trigger the measurement.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_trigger_enable(void)
{
    ADC.saradc_ctrl2.saradc_timer_en = 1;
}

/**
 * Disable digital controller timer to trigger the measurement.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_trigger_disable(void)
{
    ADC.saradc_ctrl2.saradc_timer_en = 0;
}

/**
 * Set ADC digital controller clock division factor. The clock divided from `APLL` or `APB` clock.
 * Expression: controller_clk = (APLL or APB) / (div_num + div_a / div_b + 1).
 *
 * @param div_num Division factor. Range: 0 ~ 255.
 * @param div_b Division factor. Range: 1 ~ 63.
 * @param div_a Division factor. Range: 0 ~ 63.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_controller_clk_div(uint32_t div_num, uint32_t div_b, uint32_t div_a)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(PCR.saradc_clkm_conf, saradc_clkm_div_num, div_num);
    PCR.saradc_clkm_conf.saradc_clkm_div_b = div_b;
    PCR.saradc_clkm_conf.saradc_clkm_div_a = div_a;
}

/**
 * Enable clock and select clock source for ADC digital controller.
 *
 * @param clk_src clock source for ADC digital controller.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_clk_sel(adc_continuous_clk_src_t clk_src)
{
    switch (clk_src) {
        case ADC_DIGI_CLK_SRC_XTAL:
            PCR.saradc_clkm_conf.saradc_clkm_sel = 0;
            break;
        case ADC_DIGI_CLK_SRC_RC_FAST:
            PCR.saradc_clkm_conf.saradc_clkm_sel = 1;
            break;
        case ADC_DIGI_CLK_SRC_PLL_F80M:
            PCR.saradc_clkm_conf.saradc_clkm_sel = 2;
            break;
        default:
            HAL_ASSERT(false && "unsupported clock");
    }
    // Enable ADC_CTRL_CLK (i.e. digital domain clock)
    ADC.saradc_ctrl.saradc_sar_clk_gated = 1;
}

/**
 * Disable clock for ADC digital controller.
 */
static inline void adc_ll_digi_controller_clk_disable(void)
{
    ADC.saradc_ctrl.saradc_sar_clk_gated = 0;
}

/**
 * Reset adc digital controller filter.
 *
 * @param idx   Filter index
 * @param adc_n ADC unit.
 */
static inline void adc_ll_digi_filter_reset(adc_digi_iir_filter_t idx, adc_unit_t adc_n)
{
    (void)adc_n;
    ADC.saradc_filter_ctrl0.saradc_adc_filter_reset = 1;
    ADC.saradc_filter_ctrl0.saradc_adc_filter_reset = 0;
}

/**
 * Set adc digital controller filter coeff.
 *
 * @param idx      filter index
 * @param adc_n    adc unit
 * @param channel  adc channel
 * @param coeff    filter coeff
 */
static inline void adc_ll_digi_filter_set_factor(adc_digi_iir_filter_t idx, adc_unit_t adc_n, adc_channel_t channel, adc_digi_iir_filter_coeff_t coeff)
{
    uint32_t factor_reg_val = 0;
    switch (coeff) {
        case ADC_DIGI_IIR_FILTER_COEFF_2:
            factor_reg_val = 1;
            break;
        case ADC_DIGI_IIR_FILTER_COEFF_4:
            factor_reg_val = 2;
            break;
        case ADC_DIGI_IIR_FILTER_COEFF_8:
            factor_reg_val = 3;
            break;
        case ADC_DIGI_IIR_FILTER_COEFF_16:
            factor_reg_val = 4;
            break;
        case ADC_DIGI_IIR_FILTER_COEFF_64:
            factor_reg_val = 6;
            break;
        default:
            HAL_ASSERT(false);
    }

    if (idx == ADC_DIGI_IIR_FILTER_0) {
        ADC.saradc_filter_ctrl0.saradc_adc_filter_channel0 = ((adc_n + 1) << 3) | (channel & 0x7);
        ADC.saradc_filter_ctrl1.saradc_adc_filter_factor0 = factor_reg_val;
    } else if (idx == ADC_DIGI_IIR_FILTER_1) {
        ADC.saradc_filter_ctrl0.saradc_adc_filter_channel1 = ((adc_n + 1) << 3) | (channel & 0x7);
        ADC.saradc_filter_ctrl1.saradc_adc_filter_factor1 = factor_reg_val;
    }
}

/**
 * Enable adc digital controller filter.
 * Filtering the ADC data to obtain smooth data at higher sampling rates.
 *
 * @param idx      filter index
 * @param adc_n    ADC unit
 * @param enable   Enable / Disable
 */
static inline void adc_ll_digi_filter_enable(adc_digi_iir_filter_t idx, adc_unit_t adc_n, bool enable)
{
    (void)adc_n;
    if (!enable) {
        if (idx == ADC_DIGI_IIR_FILTER_0) {
            ADC.saradc_filter_ctrl0.saradc_adc_filter_channel0 = 0xF;
            ADC.saradc_filter_ctrl1.saradc_adc_filter_factor0 = 0;
        } else if (idx == ADC_DIGI_IIR_FILTER_1) {
            ADC.saradc_filter_ctrl0.saradc_adc_filter_channel1 = 0xF;
            ADC.saradc_filter_ctrl1.saradc_adc_filter_factor1 = 0;
        }
    }
    //nothing to do to enable, after adc_ll_digi_filter_set_factor, it's enabled.
}

/**
 * Set monitor threshold of adc digital controller on specific channel.
 *
 * @param monitor_id ADC digi monitor unit index.
 * @param adc_n      Which adc unit the channel belong to.
 * @param channel    Which channel of adc want to be monitored.
 * @param h_thresh   High threshold of this monitor.
 * @param l_thresh   Low threshold of this monitor.
 */
static inline void adc_ll_digi_monitor_set_thres(adc_monitor_id_t monitor_id, adc_unit_t adc_n, uint8_t channel, int32_t h_thresh, int32_t l_thresh)
{
    if (monitor_id == ADC_MONITOR_0) {
        ADC.saradc_thres0_ctrl.saradc_adc_thres0_channel = (adc_n << 3) | (channel & 0x7);
        ADC.saradc_thres0_ctrl.saradc_adc_thres0_high = h_thresh;
        ADC.saradc_thres0_ctrl.saradc_adc_thres0_low = l_thresh;
    } else { // ADC_MONITOR_1
        ADC.saradc_thres1_ctrl.saradc_adc_thres1_channel = (adc_n << 3) | (channel & 0x7);
        ADC.saradc_thres1_ctrl.saradc_adc_thres1_high = h_thresh;
        ADC.saradc_thres1_ctrl.saradc_adc_thres1_low = l_thresh;
    }
}

/**
 * Start/Stop monitor of adc digital controller.
 *
 * @param monitor_id ADC digi monitor unit index.
 * @param start 1 for start, 0 for stop
 */
static inline void adc_ll_digi_monitor_user_start(adc_monitor_id_t monitor_id, bool start)
{
    if (monitor_id == ADC_MONITOR_0) {
        ADC.saradc_thres_ctrl.saradc_adc_thres0_en = start;
    } else {
        ADC.saradc_thres_ctrl.saradc_adc_thres1_en = start;
    }
}

/**
 * Enable/disable a intr of adc digital monitor.
 *
 * @param monitor_id ADC digi monitor unit index.
 * @param mode monit mode to enable/disable intr.
 * @param enable enable or disable.
 */
static inline void adc_ll_digi_monitor_enable_intr(adc_monitor_id_t monitor_id, adc_monitor_mode_t mode, bool enable)
{
    if (monitor_id == ADC_MONITOR_0) {
        if (mode == ADC_MONITOR_MODE_HIGH) {
            ADC.saradc_int_ena.saradc_adc_thres0_high_int_ena = enable;
        } else {
            ADC.saradc_int_ena.saradc_adc_thres0_low_int_ena = enable;
        }
    }
    if (monitor_id == ADC_MONITOR_1) {
        if (mode == ADC_MONITOR_MODE_HIGH) {
            ADC.saradc_int_ena.saradc_adc_thres1_high_int_ena = enable;
        } else {
            ADC.saradc_int_ena.saradc_adc_thres1_low_int_ena = enable;
        }
    }
}

/**
 * Clear intr raw for adc digi monitors.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_monitor_clear_intr(void)
{
    ADC.saradc_int_clr.val |= ADC_LL_THRES_ALL_INTR_ST_M;
}

/**
 * Get the address of digi monitor intr statue register.
 *
 * @return address of register.
 */
__attribute__((always_inline))
static inline volatile const void *adc_ll_digi_monitor_get_intr_status_addr(void)
{
    return &ADC.saradc_int_st.val;
}

/**
 * Set DMA eof num of adc digital controller.
 * If the number of measurements reaches `dma_eof_num`, then `dma_in_suc_eof` signal is generated.
 *
 * @param num eof num of DMA.
 */
static inline void adc_ll_digi_dma_set_eof_num(uint32_t num)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(ADC.saradc_dma_conf, saradc_apb_adc_eof_num, num);
}

/**
 * Enable output data to DMA from adc digital controller.
 */
static inline void adc_ll_digi_dma_enable(void)
{
    ADC.saradc_dma_conf.saradc_apb_adc_trans = 1;
}

/**
 * Disable output data to DMA from adc digital controller.
 */
static inline void adc_ll_digi_dma_disable(void)
{
    ADC.saradc_dma_conf.saradc_apb_adc_trans = 0;
}

/**
 * Reset adc digital controller.
 */
static inline void adc_ll_digi_reset(void)
{
    ADC.saradc_dma_conf.saradc_apb_adc_reset_fsm = 1;
    ADC.saradc_dma_conf.saradc_apb_adc_reset_fsm = 0;
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
    (void)cct;
}

/**
 * Get adc cct for PWDET controller.
 *
 * @note Capacitor tuning of the PA power monitor. cct set to the same value with PHY.
 * @return cct Range: 0 ~ 7.
 */
static inline uint32_t adc_ll_pwdet_get_cct(void)
{
    return 0;
}

/*---------------------------------------------------------------
                    Common setting
---------------------------------------------------------------*/

/**
 * @brief Enable the ADC APB clock
 * @param enable true to enable, false to disable
 */
__attribute__((always_inline))
static inline void adc_ll_enable_bus_clock(bool enable)
{
    PCR.saradc_conf.saradc_reg_clk_en = enable;
}

/**
 * @brief Enable the ADC function clock
 * @param enable true to enable, false to disable
 */
__attribute__((always_inline))
static inline void adc_ll_enable_func_clock(bool enable)
{
    PCR.saradc_clkm_conf.saradc_clkm_en = enable;
}

/**
 * @brief Reset ADC module
 */
__attribute__((always_inline))
static inline void adc_ll_reset_register(void)
{
    PCR.saradc_conf.saradc_rst_en = 1;
    PCR.saradc_conf.saradc_rst_en = 0;
    PCR.saradc_conf.saradc_reg_rst_en = 1;
    PCR.saradc_conf.saradc_reg_rst_en = 0;
}

/**
 * Set ADC module power management.
 *
 * @param manage Set ADC power status.
 */
static inline void adc_ll_set_power_manage(adc_unit_t adc_n, adc_ll_power_t manage)
{
    (void) adc_n;
    /* Bit1  0:Fsm  1: SW mode
       Bit0  0:SW mode power down  1: SW mode power on */
    if (manage == ADC_LL_POWER_SW_ON) {
        ADC.saradc_ctrl.saradc_sar_clk_gated = 1;
        ADC.saradc_ctrl.saradc_xpd_sar_force = 3;
    } else if (manage == ADC_LL_POWER_BY_FSM) {
        ADC.saradc_ctrl.saradc_sar_clk_gated = 1;
        ADC.saradc_ctrl.saradc_xpd_sar_force = 0;
    } else if (manage == ADC_LL_POWER_SW_OFF) {
        ADC.saradc_ctrl.saradc_sar_clk_gated = 0;
        ADC.saradc_ctrl.saradc_xpd_sar_force = 2;
    }
}

__attribute__((always_inline))
static inline void adc_ll_set_controller(adc_unit_t adc_n, adc_ll_controller_t ctrl)
{
    //Not used on ESP32C61
}

/*---------------------------------------------------------------
                    Calibration
---------------------------------------------------------------*/
/**
 * Set the calibration result to ADC.
 *
 * @note  Different ADC units and different attenuation options use different calibration data (initial data).
 *
 * @param adc_n ADC index number.
 * @param param calibration param
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
        //C61 doesn't support ADC2, here is for backward compatibility for RNG
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR2_INITIAL_CODE_HIGH_ADDR, msb);
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR2_INITIAL_CODE_LOW_ADDR, lsb);
    }
}

/**
 * Set the SAR DTEST param
 *
 * @param param DTEST value
 */
__attribute__((always_inline))
static inline void adc_ll_set_dtest_param(uint32_t param)
{
    REGI2C_WRITE_MASK(I2C_SAR_ADC, I2C_SARADC_DTEST, param);
}

/**
 * Set the SAR ENT param
 *
 * @param param ENT value
 */
__attribute__((always_inline))
static inline void adc_ll_set_ent_param(uint32_t param)
{
    REGI2C_WRITE_MASK(I2C_SAR_ADC, I2C_SARADC_ENT_SAR, param);
}

/**
 * Enable/disable the calibration voltage reference for ADC unit.
 *
 * @param adc_n ADC index number.
 * @param en true to enable, false to disable
 */
__attribute__((always_inline))
static inline void adc_ll_enable_calibration_ref(adc_unit_t adc_n, bool en)
{
    //C61 doesn't support ADC2, here is for backward compatibility for RNG
    if (adc_n == ADC_UNIT_1) {
        REGI2C_WRITE_MASK(I2C_SAR_ADC, I2C_SARADC1_ENCAL_REF, en);
    } else {
        REGI2C_WRITE_MASK(I2C_SAR_ADC, I2C_SARADC2_ENCAL_REF, en);
    }
}

__attribute__((always_inline))
/**
 * Init regi2c SARADC registers
 */
static inline void adc_ll_regi2c_init(void)
{
    adc_ll_set_dtest_param(0);
    adc_ll_set_ent_param(1);
    // Config ADC circuit (Analog part) with I2C(HOST ID 0x69) and chose internal voltage as sampling source
    adc_ll_enable_calibration_ref(ADC_UNIT_1, true);
    adc_ll_enable_calibration_ref(ADC_UNIT_2, true);
}

/**
 * Deinit regi2c SARADC registers
 */
__attribute__((always_inline))
static inline void adc_ll_regi2c_adc_deinit(void)
{
    adc_ll_set_dtest_param(0);
    adc_ll_set_ent_param(0);
    adc_ll_enable_calibration_ref(ADC_UNIT_1, false);
    adc_ll_enable_calibration_ref(ADC_UNIT_2, false);
}

/*---------------------------------------------------------------
                    Oneshot Read
---------------------------------------------------------------*/
/**
 * Set adc output data format for oneshot mode
 *
 * @note ESP32C61 Oneshot mode only supports 12bit.
 * @param adc_n ADC unit.
 * @param bits  Output data bits width option.
 */
static inline void adc_oneshot_ll_set_output_bits(adc_unit_t adc_n, adc_bitwidth_t bits)
{
    (void) adc_n;
    //ESP32C61 only supports 12bit, leave here for compatibility
    HAL_ASSERT(bits == ADC_BITWIDTH_12 || bits == ADC_BITWIDTH_DEFAULT);
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
    ADC.saradc_onetime_sample.saradc_onetime_channel = ((adc_n << 3) | channel);
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
    ADC.saradc_onetime_sample.saradc_onetime_channel = ((adc_n << 3) | 0xF);
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
    ADC.saradc_onetime_sample.saradc_onetime_start = val;
}

/**
 * Clear the event for each ADCn for Oneshot mode
 *
 * @param event ADC event
 */
static inline void adc_oneshot_ll_clear_event(uint32_t event_mask)
{
    ADC.saradc_int_clr.val |= event_mask;
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
    return (ADC.saradc_int_raw.val & event_mask);
}

/**
 * Get the converted value for each ADCn for controller.
 *
 * @param adc_n ADC unit.
 * @return
 *      - Converted value.
 */
static inline uint32_t adc_oneshot_ll_get_raw_result(adc_unit_t adc_n)
{
    uint32_t ret_val = 0;
    if (adc_n == ADC_UNIT_1) {
        ret_val = ADC.saradc_sar1data_status.saradc_adc1_data & 0xfff;
    } else { // adc_n == ADC_UNIT_2
        ret_val = ADC.saradc_sar2data_status.saradc_adc2_data & 0xfff;
    }
    return ret_val;
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
    (void)adc_n;
    return true;
}

/**
 * ADC module RTC output data invert or not.
 *
 * @param adc_n ADC unit.
 * @param inv_en data invert or not.
 */
static inline void adc_oneshot_ll_output_invert(adc_unit_t adc_n, bool inv_en)
{
    (void)adc_n;
    (void)inv_en;
    //For compatibility
}

/**
 * Enable oneshot conversion trigger
 *
 * @param adc_n  ADC unit
 */
static inline void adc_oneshot_ll_enable(adc_unit_t adc_n)
{
    if (adc_n == ADC_UNIT_1) {
        ADC.saradc_onetime_sample.saradc_saradc1_onetime_sample = 1;
    } else {
        ADC.saradc_onetime_sample.saradc_saradc2_onetime_sample = 1;
    }
}

/**
 * Disable oneshot conversion trigger for all the ADC units
 */
static inline void adc_oneshot_ll_disable_all_unit(void)
{
    ADC.saradc_onetime_sample.saradc_saradc1_onetime_sample = 0;
    ADC.saradc_onetime_sample.saradc_saradc2_onetime_sample = 0;
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
    (void)adc_n;
    (void)channel;
    // Attenuation is for all channels, unit and channel are for compatibility
    ADC.saradc_onetime_sample.saradc_onetime_atten = atten;
}

/**
 * Get the attenuation of a particular channel on ADCn.
 *
 * @param adc_n ADC unit.
 * @param channel ADCn channel number.
 * @return atten The attenuation option.
 */
__attribute__((always_inline))
static inline adc_atten_t adc_ll_get_atten(adc_unit_t adc_n, adc_channel_t channel)
{
    (void)adc_n;
    (void)channel;
    return (adc_atten_t)ADC.saradc_onetime_sample.saradc_onetime_atten;
}

#ifdef __cplusplus
}
#endif

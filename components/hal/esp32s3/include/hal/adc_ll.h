/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdio.h>
#include <stdbool.h>

#include "soc/adc_periph.h"
#include "hal/adc_types.h"
#include "hal/adc_types_private.h"
#include "hal/assert.h"
#include "hal/misc.h"
#include "soc/apb_saradc_struct.h"
#include "soc/sens_struct.h"
#include "soc/apb_saradc_reg.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/regi2c_defs.h"
#include "soc/clk_tree_defs.h"
#include "soc/system_struct.h"
#include "hal/regi2c_ctrl.h"
#include "soc/regi2c_saradc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADC_LL_EVENT_ADC1_ONESHOT_DONE    (1 << 0)
#define ADC_LL_EVENT_ADC2_ONESHOT_DONE    (1 << 1)

#define ADC_LL_THRES_ALL_INTR_ST_M  (APB_SARADC_THRES0_HIGH_INT_ST_M | \
                                     APB_SARADC_THRES1_HIGH_INT_ST_M | \
                                     APB_SARADC_THRES0_LOW_INT_ST_M  | \
                                     APB_SARADC_THRES1_LOW_INT_ST_M)
#define ADC_LL_GET_HIGH_THRES_MASK(monitor_id)    ((monitor_id == 0) ? APB_SARADC_THRES0_HIGH_INT_ST_M : APB_SARADC_THRES1_HIGH_INT_ST_M)
#define ADC_LL_GET_LOW_THRES_MASK(monitor_id)     ((monitor_id == 0) ? APB_SARADC_THRES0_LOW_INT_ST_M : APB_SARADC_THRES1_LOW_INT_ST_M)

/*---------------------------------------------------------------
                    Oneshot
---------------------------------------------------------------*/
#define ADC_LL_DATA_INVERT_DEFAULT(PERIPH_NUM)         (0)
#define ADC_LL_SAR_CLK_DIV_DEFAULT(PERIPH_NUM)         (1)
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

/*---------------------------------------------------------------
                    PWDET (Power Detect)
---------------------------------------------------------------*/
#define ADC_LL_PWDET_CCT_DEFAULT                       (4)

typedef enum {
    ADC_LL_POWER_BY_FSM,   /*!< ADC XPD controlled by FSM. Used for polling mode */
    ADC_LL_POWER_SW_ON,    /*!< ADC XPD controlled by SW. power on. Used for DMA mode */
    ADC_LL_POWER_SW_OFF,   /*!< ADC XPD controlled by SW. power off. */
} adc_ll_power_t;

typedef enum {
    ADC_LL_RTC_DATA_OK = 0,
    ADC_LL_RTC_CTRL_UNSELECTED = 1,     ///< The current controller is not enabled by the arbiter.
    ADC_LL_RTC_CTRL_BREAK = 2,          ///< The current controller process was interrupted by a higher priority controller.
    ADC_LL_RTC_DATA_FAIL = -1,          ///< The data is wrong
} adc_ll_rtc_raw_data_t;

typedef enum {
    ADC_LL_CTRL_RTC = 0,    ///< For ADC1. Select RTC controller.
    ADC_LL_CTRL_ULP = 1,    ///< For ADC1 and ADC2. Select ULP controller.
    ADC_LL_CTRL_DIG = 2,    ///< For ADC1. Select DIG controller.
    ADC_LL_CTRL_ARB = 3,    ///< For ADC2. The controller is selected by the arbiter.
} adc_ll_controller_t;

/**
 * @brief ADC digital controller (DMA mode) work mode.
 *
 * @note  The conversion mode affects the sampling frequency:
 *        SINGLE_UNIT_1: When the measurement is triggered, only ADC1 is sampled once.
 *        SINGLE_UNIT_2: When the measurement is triggered, only ADC2 is sampled once.
 *        BOTH_UNIT    : When the measurement is triggered, ADC1 and ADC2 are sampled at the same time.
 *        ALTER_UNIT   : When the measurement is triggered, ADC1 or ADC2 samples alternately.
 */
typedef enum {
    ADC_LL_DIGI_CONV_ONLY_ADC1  = 0,    // Only use ADC1 for conversion
    ADC_LL_DIGI_CONV_ONLY_ADC2  = 1,    // Only use ADC2 for conversion
    ADC_LL_DIGI_CONV_BOTH_UNIT  = 2,    // Use Both ADC1 and ADC2 for conversion simultaneously
    ADC_LL_DIGI_CONV_ALTER_UNIT = 3     // Use both ADC1 and ADC2 for conversion by turn. e.g. ADC1 -> ADC2 -> ADC1 -> ADC2 .....
} adc_ll_digi_convert_mode_t;

typedef struct {
    union {
        struct {
            uint8_t atten:       2;
            uint8_t channel:     4;
            uint8_t reserved:    2;
        };
        uint8_t val;
    };
} __attribute__((packed)) adc_ll_digi_pattern_table_t;

#ifdef _MSC_VER
#pragma pack(push, 1)
#endif /* _MSC_VER */

/**
 * @brief Analyze whether the obtained raw data is correct.
 *        ADC2 use arbiter by default. The arbitration result can be judged by the flag bit in the original data.
 *
 */
typedef struct {
    union {
        struct {
            uint16_t data:     13;  /*!<ADC real output data info. Resolution: 13 bit. */
            uint16_t reserved:  1;  /*!<reserved */
            uint16_t flag:      2;  /*!<ADC data flag info.
                                        If (flag == 0), The data is valid.
                                        If (flag > 0), The data is invalid. */
        };
        uint16_t val;
    };
} adc_ll_rtc_output_data_t;

#ifdef _MSC_VER
#pragma pack(pop)
#endif /* _MSC_VER */


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
    SET_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_SAR_I2C_PU_M);
    CLEAR_PERI_REG_MASK(ANA_CONFIG_REG, I2C_SAR_M);
    SET_PERI_REG_MASK(ANA_CONFIG2_REG, ANA_SAR_CFG2_M);

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
    /* ADC clock divided from digital controller clock clk */
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
 * @param mode Conversion mode select.
 */
static inline void adc_ll_digi_set_convert_mode(adc_ll_digi_convert_mode_t mode)
{
    if (mode == ADC_LL_DIGI_CONV_ONLY_ADC1) {
        APB_SARADC.ctrl.work_mode = 0;
        APB_SARADC.ctrl.sar_sel = 0;
    } else if (mode == ADC_LL_DIGI_CONV_ONLY_ADC2) {
        APB_SARADC.ctrl.work_mode = 0;
        APB_SARADC.ctrl.sar_sel = 1;
    } else if (mode == ADC_LL_DIGI_CONV_BOTH_UNIT) {
        APB_SARADC.ctrl.work_mode = 1;
    } else if (mode == ADC_LL_DIGI_CONV_ALTER_UNIT) {
        APB_SARADC.ctrl.work_mode = 2;
    }
    APB_SARADC.ctrl.data_sar_sel = 1;
}

/**
 * Set pattern table length for digital controller.
 * The pattern table that defines the conversion rules for each SAR ADC. Each table has 16 items, in which channel selection,
 * resolution and attenuation are stored. When the conversion is started, the controller reads conversion rules from the
 * pattern table one by one. For each controller the scan sequence has at most 16 different rules before repeating itself.
 *
 * @param adc_n ADC unit.
 * @param patt_len Items range: 1 ~ 16.
 */
static inline void adc_ll_digi_set_pattern_table_len(adc_unit_t adc_n, uint32_t patt_len)
{
    if (adc_n == ADC_UNIT_1) {
        APB_SARADC.ctrl.sar1_patt_len = patt_len - 1;
    } else { // adc_n == ADC_UNIT_2
        APB_SARADC.ctrl.sar2_patt_len = patt_len - 1;
    }
}

/**
 * Set pattern table for digital controller.
 * The pattern table that defines the conversion rules for each SAR ADC. Each table has 12 items, in which channel selection,
 * resolution and attenuation are stored. When the conversion is started, the controller reads conversion rules from the
 * pattern table one by one. For each controller the scan sequence has at most 12 different rules before repeating itself.
 *
 * @param adc_n ADC unit.
 * @param pattern_index Items index. Range: 0 ~ 11.
 * @param pattern Stored conversion rules.
 */
static inline void adc_ll_digi_set_pattern_table(adc_unit_t adc_n, uint32_t pattern_index, adc_digi_pattern_config_t table)
{
    uint32_t tab;
    uint8_t index = pattern_index / 4;
    uint8_t offset = (pattern_index % 4) * 6;
    adc_ll_digi_pattern_table_t pattern = {0};

    pattern.val = (table.atten & 0x3) | ((table.channel & 0xF) << 2);
    if (table.unit == ADC_UNIT_1){
        tab = APB_SARADC.sar1_patt_tab[index].sar1_patt_tab;            //Read old register value
        tab &= (~(0xFC0000 >> offset));                 //Clear old data
        tab |= ((uint32_t)(pattern.val & 0x3F) << 18) >> offset;   //Fill in the new data
        APB_SARADC.sar1_patt_tab[index].sar1_patt_tab = tab;            //Write back
    } else {
        tab = APB_SARADC.sar2_patt_tab[index].sar2_patt_tab;        //Read old register value
        tab &= (~(0xFC0000 >> offset));                             //clear old data
        tab |= ((uint32_t)(pattern.val & 0x3F) << 18) >> offset;    //Fill in the new data
        APB_SARADC.sar2_patt_tab[index].sar2_patt_tab = tab;        //Write back
    }
}

/**
 * Reset the pattern table pointer, then take the measurement rule from table header in next measurement.
 *
 * @param adc_n ADC unit.
 */
static inline void adc_ll_digi_clear_pattern_table(adc_unit_t adc_n)
{
    if (adc_n == ADC_UNIT_1) {
        APB_SARADC.ctrl.sar1_patt_p_clear = 1;
        APB_SARADC.ctrl.sar1_patt_p_clear = 0;
    } else { // adc_n == ADC_UNIT_2
        APB_SARADC.ctrl.sar2_patt_p_clear = 1;
        APB_SARADC.ctrl.sar2_patt_p_clear = 0;
    }
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
    APB_SARADC.ctrl2.timer_sel = 1;
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
 * @param clk_src clock source for ADC digital controller.
 */
static inline void adc_ll_digi_clk_sel(adc_continuous_clk_src_t clk_src)
{
    APB_SARADC.apb_adc_clkm_conf.clk_sel = (clk_src == ADC_DIGI_CLK_SRC_APB) ? 2 : 1;
    APB_SARADC.ctrl.sar_clk_gated = 1;
}

/**
 * Disable clock for ADC digital controller.
 */
static inline void adc_ll_digi_controller_clk_disable(void)
{
    APB_SARADC.ctrl.sar_clk_gated = 0;
    APB_SARADC.apb_adc_clkm_conf.clk_sel = 0;
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
    APB_SARADC.filter_ctrl0.filter_reset = 1;
    APB_SARADC.filter_ctrl0.filter_reset = 0;
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
        APB_SARADC.filter_ctrl0.filter_channel0 = ((adc_n + 1) << 3) | (channel & 0x7);
        APB_SARADC.filter_ctrl1.filter_factor0 = factor_reg_val;
    } else if (idx == ADC_DIGI_IIR_FILTER_1) {
        APB_SARADC.filter_ctrl0.filter_channel1 = ((adc_n + 1) << 3) | (channel & 0x7);
        APB_SARADC.filter_ctrl1.filter_factor1 = factor_reg_val;
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
            APB_SARADC.filter_ctrl0.filter_channel0 = 0xF;
            APB_SARADC.filter_ctrl1.filter_factor0 = 0;
        } else if (idx == ADC_DIGI_IIR_FILTER_1) {
            APB_SARADC.filter_ctrl0.filter_channel1 = 0xF;
            APB_SARADC.filter_ctrl1.filter_factor1 = 0;
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
        APB_SARADC.thres0_ctrl.thres0_channel = (adc_n << 3) | (channel & 0x7);
        APB_SARADC.thres0_ctrl.thres0_high = h_thresh;
        APB_SARADC.thres0_ctrl.thres0_low = l_thresh;
    } else { // ADC_MONITOR_1
        APB_SARADC.thres1_ctrl.thres1_channel = (adc_n << 3) | (channel & 0x7);
        APB_SARADC.thres1_ctrl.thres1_high = h_thresh;
        APB_SARADC.thres1_ctrl.thres1_low = l_thresh;
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
        APB_SARADC.thres_ctrl.thres0_en = start;
    } else {
        APB_SARADC.thres_ctrl.thres1_en = start;
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
            APB_SARADC.int_ena.thres0_high = enable;
        } else {
            APB_SARADC.int_ena.thres0_low = enable;
        }
    }
    if (monitor_id == ADC_MONITOR_1) {
        if (mode == ADC_MONITOR_MODE_HIGH) {
            APB_SARADC.int_ena.thres1_high = enable;
        } else {
            APB_SARADC.int_ena.thres1_low = enable;
        }
    }
}

/**
 * Clear intr raw for adc digi monitors.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_monitor_clear_intr(void)
{
    APB_SARADC.int_clr.val |= ADC_LL_THRES_ALL_INTR_ST_M;
}

/**
 * Get the address of digi monitor intr statue register.
 *
 * @return address of register.
 */
__attribute__((always_inline))
static inline volatile const void *adc_ll_digi_monitor_get_intr_status_addr(void)
{
    return &APB_SARADC.int_st.val;
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
    SENS.sar_meas2_mux.sar2_pwdet_cct = cct;
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
    return SENS.sar_meas2_mux.sar2_pwdet_cct;
}

/*---------------------------------------------------------------
                    Common setting
---------------------------------------------------------------*/

/**
 * @brief Enable the ADC clock
 * @param enable true to enable, false to disable
 */
static inline void adc_ll_enable_bus_clock(bool enable)
{
    SYSTEM.perip_clk_en0.apb_saradc_clk_en = enable;
}
// SYSTEM.perip_clk_en0 is a shared register, so this function must be used in an atomic way
#define adc_ll_enable_bus_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        adc_ll_enable_bus_clock(__VA_ARGS__); \
    } while(0)

/**
 * @brief Reset ADC module
 */
static inline void adc_ll_reset_register(void)
{
    SYSTEM.perip_rst_en0.apb_saradc_rst = 1;
    SYSTEM.perip_rst_en0.apb_saradc_rst = 0;
}
//  SYSTEM.perip_rst_en0 is a shared register, so this function must be used in an atomic way
#define adc_ll_reset_register(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        adc_ll_reset_register(__VA_ARGS__); \
    } while(0)

/**
 * Set ADC module power management.
 *
 * @param manage Set ADC power status.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_set_power_manage(adc_ll_power_t manage)
{
    if (manage == ADC_LL_POWER_SW_ON) {
        APB_SARADC.ctrl.sar_clk_gated = 1;
        APB_SARADC.ctrl.xpd_sar_force = 0x3;
    } else if (manage == ADC_LL_POWER_BY_FSM) {
        APB_SARADC.ctrl.sar_clk_gated = 1;
        APB_SARADC.ctrl.xpd_sar_force = 0x0;
    } else if (manage == ADC_LL_POWER_SW_OFF) {
        APB_SARADC.ctrl.sar_clk_gated = 0;
        APB_SARADC.ctrl.xpd_sar_force = 0x2;
    }
}

/**
 * Set ADC module controller.
 * There are five SAR ADC controllers:
 * Two digital controller: Continuous conversion mode (DMA). High performance with multiple channel scan modes;
 * Two RTC controller: Single conversion modes (Polling). For low power purpose working during deep sleep;
 * the other is dedicated for Power detect (PWDET / PKDET), Only support ADC2.
 *
 * @param adc_n ADC unit.
 * @param ctrl ADC controller.
 */
__attribute__((always_inline))
static inline void adc_ll_set_controller(adc_unit_t adc_n, adc_ll_controller_t ctrl)
{
    if (adc_n == ADC_UNIT_1) {
        switch (ctrl) {
            case ADC_LL_CTRL_RTC:
                SENS.sar_meas1_mux.sar1_dig_force       = 0;    // 1: Select digital control;       0: Select RTC control.
                SENS.sar_meas1_ctrl2.meas1_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
                SENS.sar_meas1_ctrl2.sar1_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
                break;
            case ADC_LL_CTRL_ULP:
                SENS.sar_meas1_mux.sar1_dig_force       = 0;    // 1: Select digital control;       0: Select RTC control.
                SENS.sar_meas1_ctrl2.meas1_start_force  = 0;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
                SENS.sar_meas1_ctrl2.sar1_en_pad_force  = 0;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
                break;
            case ADC_LL_CTRL_DIG:
                SENS.sar_meas1_mux.sar1_dig_force       = 1;    // 1: Select digital control;       0: Select RTC control.
                SENS.sar_meas1_ctrl2.meas1_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
                SENS.sar_meas1_ctrl2.sar1_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
                break;
            default:
                break;
        }
    } else { // adc_n == ADC_UNIT_2
        //If ADC2 is not controlled by ULP, the arbiter will decide which controller to use ADC2.
        switch (ctrl) {
            case ADC_LL_CTRL_ARB:
                SENS.sar_meas2_ctrl2.meas2_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
                SENS.sar_meas2_ctrl2.sar2_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
                break;
            case ADC_LL_CTRL_ULP:
                SENS.sar_meas2_ctrl2.meas2_start_force  = 0;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
                SENS.sar_meas2_ctrl2.sar2_en_pad_force  = 0;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
                break;
            default:
                break;
        }
    }
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
    } else {  // Shield arbiter.
        APB_SARADC.apb_adc_arb_ctrl.adc_arb_grant_force = 1;
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

/**
 * Force switch ADC2 to RTC controller in sleep mode. Shield arbiter.
 * In sleep mode, the arbiter is in power-down mode.
 * Need to switch the controller to RTC to shield the control of the arbiter.
 * After waking up, it needs to switch to arbiter control.
 *
 * @note The hardware will do this automatically. In normal use, there is no need to call this interface to manually switch the controller.
 * @note Only support ADC2.
 */
static inline void adc_ll_enable_sleep_controller(void)
{
    SENS.sar_meas2_mux.sar2_rtc_force = 1;
}

/**
 * Force switch ADC2 to arbiter in wakeup mode.
 * In sleep mode, the arbiter is in power-down mode.
 * Need to switch the controller to RTC to shield the control of the arbiter.
 * After waking up, it needs to switch to arbiter control.
 *
 * @note The hardware will do this automatically. In normal use, there is no need to call this interface to manually switch the controller.
 * @note Only support ADC2.
 */
static inline void adc_ll_disable_sleep_controller(void)
{
    SENS.sar_meas2_mux.sar2_rtc_force = 0;
}

/**
 * @brief Set common calibration configuration. Should be shared with other parts (PWDET).
 */
__attribute__((always_inline))
static inline void adc_ll_calibration_init(adc_unit_t adc_n)
{
    if (adc_n == ADC_UNIT_1) {
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_DREF_ADDR, 4);
    } else {
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR2_DREF_ADDR, 4);
    }
}

/**
 * Configure the registers for ADC calibration. You need to call the ``adc_ll_calibration_finish`` interface to resume after calibration.
 *
 * @param adc_n ADC index number.
 * @param internal_gnd true:  Disconnect from the IO port and use the internal GND as the calibration voltage.
 *                     false: Use IO external voltage as calibration voltage.
 */
static inline void adc_ll_calibration_prepare(adc_unit_t adc_n, bool internal_gnd)
{
    /* Should be called before writing I2C registers. */
    SET_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_SAR_I2C_PU_M);
    CLEAR_PERI_REG_MASK(ANA_CONFIG_REG, I2C_SAR_M);
    SET_PERI_REG_MASK(ANA_CONFIG2_REG, ANA_SAR_CFG2_M);

    /* Enable/disable internal connect GND (for calibration). */
    if (adc_n == ADC_UNIT_1) {
        if (internal_gnd) {
            REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_ENCAL_GND_ADDR, 1);
        } else {
            REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_ENCAL_GND_ADDR, 0);
        }
    } else {    //adc_n == ADC_UNIT_2
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
    } else {    //adc_n == ADC_UNIT_2
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

/**
 *  Output ADC internal reference voltage to channels, only available for ADC2 on ESP32.
 *
 *  This function routes the internal reference voltage of ADCn to one of
 *  ADC2's channels. This reference voltage can then be manually measured
 *  for calibration purposes.
 *
 *  @param[in]  adc ADC unit select
 *  @param[in]  channel ADC2 channel number
 *  @param[in]  en Enable/disable the reference voltage output
 */
static inline void adc_ll_vref_output(adc_unit_t adc, adc_channel_t channel, bool en)
{
    abort();
}

/*---------------------------------------------------------------
                    RTC controller setting
---------------------------------------------------------------*/
/**
 * ADC SAR clock division factor setting. ADC SAR clock divided from `RTC_FAST_CLK`.
 *
 * @param div Division factor.
 */
static inline void adc_ll_set_sar_clk_div(adc_unit_t adc_n, uint32_t div)
{
    if (adc_n == ADC_UNIT_1) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_reader1_ctrl, sar1_clk_div, div);
    } else { // adc_n == ADC_UNIT_2
        HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_reader2_ctrl, sar2_clk_div, div);
    }
}

/**
 * Set adc output data format for RTC controller.
 *
 * @note ESP32S3 RTC controller only support 12bit.
 * @param adc_n ADC unit.
 * @param bits Output data bits width option.
 */
static inline void adc_oneshot_ll_set_output_bits(adc_unit_t adc_n, adc_bitwidth_t bits)
{
    //ESP32S3 only supports 12bit, leave here for compatibility
    HAL_ASSERT(bits == ADC_BITWIDTH_12 || bits == ADC_BITWIDTH_DEFAULT);
}

/**
 * Enable adc channel to start convert.
 *
 * @note Only one channel can be selected for once measurement.
 *
 * @param adc_n ADC unit.
 * @param channel ADC channel number for each ADCn.
 */
static inline void adc_oneshot_ll_set_channel(adc_unit_t adc_n, adc_channel_t channel)
{
    if (adc_n == ADC_UNIT_1) {
        SENS.sar_meas1_ctrl2.sar1_en_pad = (1 << channel); //only one channel is selected.
    } else { // adc_n == ADC_UNIT_2
        SENS.sar_meas2_ctrl2.sar2_en_pad = (1 << channel); //only one channel is selected.
    }
}

/**
 * Disable adc channel to start convert.
 *
 * @note Only one channel can be selected in once measurement.
 *
 * @param adc_n ADC unit.
 * @param channel ADC channel number for each ADCn.
 */
static inline void adc_oneshot_ll_disable_channel(adc_unit_t adc_n)
{
    if (adc_n == ADC_UNIT_1) {
        SENS.sar_meas1_ctrl2.sar1_en_pad = 0; //only one channel is selected.
    } else { // adc_n == ADC_UNIT_2
        SENS.sar_meas2_ctrl2.sar2_en_pad = 0; //only one channel is selected.
    }
}

/**
 * Start conversion once by software for RTC controller.
 *
 * @note It may be block to wait conversion idle for ADC1.
 *
 * @param adc_n ADC unit.
 */
static inline void adc_oneshot_ll_start(adc_unit_t adc_n)
{
    if (adc_n == ADC_UNIT_1) {
        while (HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_slave_addr1, meas_status) != 0) {}
        SENS.sar_meas1_ctrl2.meas1_start_sar = 0;
        SENS.sar_meas1_ctrl2.meas1_start_sar = 1;
    } else { // adc_n == ADC_UNIT_2
        SENS.sar_meas2_ctrl2.meas2_start_sar = 0; //start force 0
        SENS.sar_meas2_ctrl2.meas2_start_sar = 1; //start force 1
    }
}

/**
 * Clear the event for each ADCn for Oneshot mode
 *
 * @param event ADC event
 */
static inline void adc_oneshot_ll_clear_event(uint32_t event)
{
    //For compatibility
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
static inline bool adc_oneshot_ll_get_event(uint32_t event)
{
    bool ret = true;
    if (event == ADC_LL_EVENT_ADC1_ONESHOT_DONE) {
        ret = (bool)SENS.sar_meas1_ctrl2.meas1_done_sar;
    } else if (event == ADC_LL_EVENT_ADC2_ONESHOT_DONE) {
        ret = (bool)SENS.sar_meas2_ctrl2.meas2_done_sar;
    } else {
        HAL_ASSERT(false);
    }
    return ret;
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
    uint32_t ret_val = 0;
    if (adc_n == ADC_UNIT_1) {
        ret_val = HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_meas1_ctrl2, meas1_data_sar);
    } else { // adc_n == ADC_UNIT_2
        ret_val = HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_meas2_ctrl2, meas2_data_sar);
    }
    return ret_val;
}

/**
 * Analyze whether the obtained raw data is correct.
 * ADC2 can use arbiter. The arbitration result can be judged by the flag bit in the original data.
 *
 * @param adc_n ADC unit.
 * @param raw   ADC raw data input (convert value).
 * @return
 *      - true: raw data is valid
 *      - false: raw data is invalid
 */
static inline bool adc_oneshot_ll_raw_check_valid(adc_unit_t adc_n, uint32_t raw)
{
    if (adc_n == ADC_UNIT_1) {
        return true;
    }
    adc_ll_rtc_output_data_t *temp = (adc_ll_rtc_output_data_t *)&raw;
    if (temp->flag == 0) {
        return true;
    } else {
        //Could be ADC_LL_RTC_CTRL_UNSELECTED, ADC_LL_RTC_CTRL_BREAK or ADC_LL_RTC_DATA_FAIL
        return false;
    }
}

/**
 * ADC module RTC output data invert or not.
 *
 * @param adc_n ADC unit.
 * @param inv_en data invert or not.
 */
static inline void adc_oneshot_ll_output_invert(adc_unit_t adc_n, bool inv_en)
{
    if (adc_n == ADC_UNIT_1) {
        SENS.sar_reader1_ctrl.sar1_data_inv = inv_en;   // Enable / Disable ADC data invert
    } else { // adc_n == ADC_UNIT_2
        SENS.sar_reader2_ctrl.sar2_data_inv = inv_en;  // Enable / Disable ADC data invert
    }
}

/**
 * Enable ADCn conversion complete interrupt for RTC controller.
 *
 * @param adc_n ADC unit.
 */
static inline void adc_ll_rtc_intr_enable(adc_unit_t adc_n)
{
    if (adc_n == ADC_UNIT_1) {
        SENS.sar_reader1_ctrl.sar1_int_en = 1;
        RTCCNTL.int_ena.rtc_saradc1 = 1;
    } else { // adc_n == ADC_UNIT_2
        SENS.sar_reader2_ctrl.sar2_int_en = 1;
        RTCCNTL.int_ena.rtc_saradc2 = 1;
    }
}

/**
 * Disable ADCn conversion complete interrupt for RTC controller.
 *
 * @param adc_n ADC unit.
 */
static inline void adc_ll_rtc_intr_disable(adc_unit_t adc_n)
{
    if (adc_n == ADC_UNIT_1) {
        SENS.sar_reader1_ctrl.sar1_int_en = 0;
        RTCCNTL.int_ena.rtc_saradc1 = 0;
    } else { // adc_n == ADC_UNIT_2
        SENS.sar_reader2_ctrl.sar2_int_en = 0;
        RTCCNTL.int_ena.rtc_saradc2 = 0;
    }
}

/**
 * Reset RTC controller FSM.
 */
static inline void adc_ll_rtc_reset(void)
{
    SENS.sar_peri_reset_conf.saradc_reset = 1;
    SENS.sar_peri_reset_conf.saradc_reset = 0;
}

/**
 * Sets the number of cycles required for the conversion to complete and wait for the arbiter to stabilize.
 *
 * @note Only ADC2 have arbiter function.
 * @param cycle range: [0,4].
 */
static inline void adc_ll_rtc_set_arbiter_stable_cycle(uint32_t cycle)
{
    SENS.sar_reader2_ctrl.sar2_wait_arb_cycle = cycle;
}

/**
 * Set the attenuation of a particular channel on ADCn.
 *
 * @note For any given channel, this function must be called before the first time conversion.
 *
 * The default ADC full-scale voltage is 1.1V. To read higher voltages (up to the pin maximum voltage,
 * usually 3.3V) requires setting >0dB signal attenuation for that ADC channel.
 *
 * When VDD_A is 3.3V:
 *
 * - 0dB attenuation (ADC_ATTEN_DB_0) gives full-scale voltage 1.1V
 * - 2.5dB attenuation (ADC_ATTEN_DB_2_5) gives full-scale voltage 1.5V
 * - 6dB attenuation (ADC_ATTEN_DB_6) gives full-scale voltage 2.2V
 * - 11dB attenuation (ADC_ATTEN_DB_12) gives full-scale voltage 3.9V (see note below)
 *
 * @note The full-scale voltage is the voltage corresponding to a maximum reading (depending on ADC1 configured
 * bit width, this value is: 4095 for 12-bits, 2047 for 11-bits, 1023 for 10-bits, 511 for 9 bits.)
 *
 * @note At 11dB attenuation the maximum voltage is limited by VDD_A, not the full scale voltage.
 *
 * Due to ADC characteristics, most accurate results are obtained within the following approximate voltage ranges:
 *
 * - 0dB attenuation (ADC_ATTEN_DB_0) between 100 and 950mV
 * - 2.5dB attenuation (ADC_ATTEN_DB_2_5) between 100 and 1250mV
 * - 6dB attenuation (ADC_ATTEN_DB_6) between 150 to 1750mV
 * - 11dB attenuation (ADC_ATTEN_DB_12) between 150 to 2450mV
 *
 * For maximum accuracy, use the ADC calibration APIs and measure voltages within these recommended ranges.
 *
 * @param adc_n ADC unit.
 * @param channel ADCn channel number.
 * @param atten The attenuation option.
 */
static inline void adc_oneshot_ll_set_atten(adc_unit_t adc_n, adc_channel_t channel, adc_atten_t atten)
{
    if (adc_n == ADC_UNIT_1) {
        SENS.sar_atten1 = ( SENS.sar_atten1 & ~(0x3 << (channel * 2)) ) | ((atten & 0x3) << (channel * 2));
    } else { // adc_n == ADC_UNIT_2
        SENS.sar_atten2 = ( SENS.sar_atten2 & ~(0x3 << (channel * 2)) ) | ((atten & 0x3) << (channel * 2));
    }
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
    if (adc_n == ADC_UNIT_1) {
        return (adc_atten_t)((SENS.sar_atten1 >> (channel * 2)) & 0x3);
    } else {
        return (adc_atten_t)((SENS.sar_atten2 >> (channel * 2)) & 0x3);
    }
}

static inline uint32_t adc_ll_adc1_read(void)
{
    //On ESP32S3, valid data width is 12-bit
    return (APB_SARADC.apb_saradc1_data_status.adc1_data & 0xfff);
}

static inline uint32_t adc_ll_adc2_read(void)
{
    //On ESP32S3, valid data width is 12-bit
    return (APB_SARADC.apb_saradc2_data_status.adc2_data & 0xfff);
}

/**
 * Enable oneshot conversion trigger
 *
 * @param adc_n  Not used, for compatibility
 */
static inline void adc_oneshot_ll_enable(adc_unit_t adc_n)
{
    (void)adc_n;
    //For compatibility
}

/**
 * Disable oneshot conversion trigger for all the ADC units
 */
static inline void adc_oneshot_ll_disable_all_unit(void)
{
    //For compatibility
}

#ifdef __cplusplus
}
#endif

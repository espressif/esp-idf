/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>

#include "hal/adc_periph.h"
#include "soc/adc_struct.h"
#include "soc/clk_tree_defs.h"
#include "soc/lp_peri_clkrst_struct.h"
#include "hal/misc.h"
#include "hal/assert.h"
#include "hal/adc_types.h"
#include "hal/regi2c_ctrl.h"
#include "soc/regi2c_saradc.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------
                    Macros
---------------------------------------------------------------*/
#define ADC_LL_EVENT_ADC1_ONESHOT_DONE    BIT(31)
#define ADC_LL_EVENT_ADC2_ONESHOT_DONE    BIT(30)

#define ADC_LL_FORCE_XPD_SAR_FSM  0  // Use FSM to control power
#define ADC_LL_FORCE_XPD_SAR_PD   2  // Force power down
#define ADC_LL_FORCE_XPD_SAR_PU   3  // Force power up

#define ADC_LL_NEED_APB_PERIPH_CLAIM(ADC_UNIT)      (1)
#define ADC_LL_UNIT2_CHANNEL_SUBSTRATION            (0)

#define ADC_LL_DMA_USE_LP_AHB_GDMA        1

#define ADC_LL_DIGI_SAMPLE_FREQ_DYNAMIC_CLK_DIV    1
#define ADC_LL_RAW_DATA_WEIGHTED_SUM                   (1)

/*---------------------------------------------------------------
                    Differential
---------------------------------------------------------------*/
#define ADC_LL_ZERO_DIFF_CODE                          (2198)

/*---------------------------------------------------------------
                    Oneshot
---------------------------------------------------------------*/
#define ADC_LL_DATA_INVERT_DEFAULT(PERIPH_NUM)         (0)
#define ADC_LL_DELAY_CYCLE_AFTER_DONE_SIGNAL           (0)

#define ADC_LL_DIGI_SAR_CLK_DIV_DEFAULT                (1)

#define ADC_LL_CLKM_DIV_NUM_DEFAULT       4
#define ADC_LL_CLKM_DIV_B_DEFAULT         1
#define ADC_LL_CLKM_DIV_A_DEFAULT         0

#define ADC_LL_POWER_MANAGE_SUPPORTED     1

#define ADC_LL_DIGI_DATA_INVERT_DEFAULT(PERIPH_NUM)    (0)
#define ADC_LL_FSM_RSTB_WAIT_DEFAULT                   (8)
#define ADC_LL_FSM_START_WAIT_DEFAULT                  (5)
#define ADC_LL_FSM_STANDBY_WAIT_DEFAULT                (100)
#define ADC_LL_SAMPLE_CYCLE_DEFAULT                    (2)
#define ADC_LL_DEFAULT_CONV_LIMIT_EN                   (0)
#define ADC_LL_DEFAULT_CONV_LIMIT_NUM                  (255)
#define ADC_LL_WORKAROUND_CLEAR_EOF_COUNTER            (0)
#define ADC_LL_PWDET_CCT_DEFAULT                       (4)

typedef enum {
    ADC_LL_CTRL_DIG   = 0,    ///< S31 uses unified digital controller
} adc_ll_controller_t;

typedef enum {
    ADC_LL_POWER_BY_FSM = ADC_LL_FORCE_XPD_SAR_FSM,   /*!< ADC XPD controlled by FSM. Used for polling mode */
    ADC_LL_POWER_SW_ON  = ADC_LL_FORCE_XPD_SAR_PU,    /*!< ADC XPD controlled by SW. power on. */
    ADC_LL_POWER_SW_OFF = ADC_LL_FORCE_XPD_SAR_PD,    /*!< ADC XPD controlled by SW. power off. */
} adc_ll_power_t;

typedef enum {
    ADC_LL_TRIGGER_MODE_OFF  = 0,    /*!< Trigger disabled */
    ADC_LL_TRIGGER_MODE_SCAN = 1,    /*!< Timer/scan trigger mode */
    ADC_LL_TRIGGER_MODE_SW   = 2,    /*!< Software trigger mode for oneshot */
    ADC_LL_TRIGGER_MODE_ETM  = 3,    /*!< External ETM trigger mode */
} adc_ll_trigger_mode_t;

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/

/**
 * @brief Set SAR ADC module clock division factor.
 *        S31 clock is controlled through LP_PERI_CLKRST divider.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_set_clk_div(uint32_t div)
{
    (void)div;
}

/**
 * @brief Set ADC digital controller clock division factor.
 *        S31 uses LP_PERI_CLKRST.adc_ctrl.lp_adc_div_num for clock division.
 *        Division factor = lp_adc_div_num + 1
 */
__attribute__((always_inline))
static inline void adc_ll_digi_controller_clk_div(uint32_t div_num, uint32_t div_b, uint32_t div_a)
{
    (void)div_b;
    (void)div_a;
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_PERI_CLKRST.adc_ctrl, lp_adc_div_num, div_num);
}

/**
 * @brief Select the digital clock source for ADC.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_clk_sel(adc_continuous_clk_src_t clk_src)
{
    switch (clk_src) {
    case ADC_DIGI_CLK_SRC_XTAL:
        LP_PERI_CLKRST.adc_ctrl.lp_adc_clk_sel = 1;
        break;
    case ADC_DIGI_CLK_SRC_RC_FAST:
        LP_PERI_CLKRST.adc_ctrl.lp_adc_clk_sel = 0;
        break;
    default:
        HAL_ASSERT(false && "unsupported clock");
    }
    ADC.ctrl_date.clk_en = 1;
}

/*---------------------------------------------------------------
                    DMA continuous mode
---------------------------------------------------------------*/

typedef enum {
    ADC_LL_DIGI_CONV_ONLY_ADC1  = 0,
    ADC_LL_DIGI_CONV_ONLY_ADC2  = 1,
    ADC_LL_DIGI_CONV_BOTH_UNIT  = 2,
    ADC_LL_DIGI_CONV_ALTER_UNIT = 3,
} adc_ll_digi_convert_mode_t;

/**
 * Set ADC FSM interval parameters for digital controller.
 *
 * @param rst_wait cycles between DIG ADC controller reset ADC sensor and start ADC sensor.
 * @param start_wait Delay time after opening XPD.
 * @param standby_wait Delay time to close XPD.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_set_fsm_time(uint32_t rst_wait, uint32_t start_wait, uint32_t standby_wait)
{
    (void)rst_wait;
    (void)start_wait;
    (void)standby_wait;
}

/**
 * Set ADC sample cycle for digital controller.
 *
 * @note Normally, please use the default value.
 * @param sample_cycle Cycles between DIG ADC controller start ADC sensor and beginning to receive data from sensor.
 */
__attribute__((always_inline))
static inline void adc_ll_set_sample_cycle(uint32_t sample_cycle)
{
    REGI2C_WRITE_MASK(I2C_SARADC, ADC_SAR1_SAMPLE_CYCLE_ADDR, sample_cycle);
    REGI2C_WRITE_MASK(I2C_SARADC, ADC_SAR2_SAMPLE_CYCLE_ADDR, sample_cycle);
}

/**
 * Set ADC CCT for PWDET controller.
 *
 * @note Capacitor tuning of the PA power monitor. CCT is set to the same value as PHY.
 * @param cct CCT value.
 */
__attribute__((always_inline))
static inline void adc_ll_pwdet_set_cct(uint32_t cct)
{
    (void)cct;
}

/**
 * Get ADC CCT for PWDET controller.
 *
 * @return CCT value.
 */
__attribute__((always_inline))
static inline uint32_t adc_ll_pwdet_get_cct(void)
{
    return ADC_LL_PWDET_CCT_DEFAULT;
}

/**
 * Configure ADC digital controller output data inversion.
 *
 * @param adc_n ADC unit.
 * @param inv_en true to invert output data, false otherwise.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_output_invert(adc_unit_t adc_n, bool inv_en)
{
    (void)adc_n;
    (void)inv_en;
}

/**
 * Enable max conversion number detection for digital controller.
 * If the number of ADC conversions reaches the maximum, the conversion is stopped.
 *
 * @param enable true to enable, false to disable.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_convert_limit_enable(bool enable)
{
    (void)enable;
}

/**
 * Set ADC max conversion number for digital controller.
 * If the number of ADC conversions reaches the maximum, the conversion is stopped.
 *
 * @param meas_num Max conversion number.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_set_convert_limit_num(uint32_t meas_num)
{
    (void)meas_num;
}

/**
 * Disable clock for ADC digital controller.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_controller_clk_disable(void)
{
    // For compatibility
}

/**
 * Set ADC conversion mode for digital controller.
 *
 * @param mode Conversion mode select.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_set_convert_mode(adc_ll_digi_convert_mode_t mode)
{
    switch (mode) {
    case ADC_LL_DIGI_CONV_ONLY_ADC1:
        ADC.ctrl0.sar1_trigger_mode = ADC_LL_TRIGGER_MODE_SCAN;
        ADC.ctrl1.sar2_trigger_mode = ADC_LL_TRIGGER_MODE_OFF;
        ADC.ctrl0.sar1_continue_mode_en = 0;
        ADC.ctrl1.sar2_continue_mode_en = 0;
        break;
    case ADC_LL_DIGI_CONV_ONLY_ADC2:
        ADC.ctrl0.sar1_trigger_mode = ADC_LL_TRIGGER_MODE_OFF;
        ADC.ctrl1.sar2_trigger_mode = ADC_LL_TRIGGER_MODE_SCAN;
        ADC.ctrl0.sar1_continue_mode_en = 0;
        ADC.ctrl1.sar2_continue_mode_en = 0;
        break;
    case ADC_LL_DIGI_CONV_BOTH_UNIT:
    case ADC_LL_DIGI_CONV_ALTER_UNIT:
        ADC.ctrl0.sar1_trigger_mode = ADC_LL_TRIGGER_MODE_SCAN;
        ADC.ctrl1.sar2_trigger_mode = ADC_LL_TRIGGER_MODE_SCAN;
        ADC.ctrl0.sar1_continue_mode_en = 0;
        ADC.ctrl1.sar2_continue_mode_en = 0;
        break;
    default:
        HAL_ASSERT(false && "bad convert mode");
        break;
    }
    /* Update pattern index after each completed conversion; single-ended uses model bit 0 in each entry. */
    ADC.ctrl0.sar1_patt_type = 0;
    ADC.ctrl1.sar2_patt_type = 0;
    ADC.ctrl0.sar1_clk_pos_sel = 1;
    ADC.ctrl1.sar2_clk_pos_sel = 1;
}

/**
 * Set pattern table length for digital controller.
 * The pattern table defines the conversion rules for each SAR ADC.
 *
 * @param adc_n ADC unit.
 * @param patt_len Pattern items. Range: 1 ~ 16.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_set_pattern_table_len(adc_unit_t adc_n, uint32_t patt_len)
{
    if (adc_n == ADC_UNIT_1) {
        ADC.ctrl0.sar1_patt_len = patt_len - 1;
    } else {
        ADC.ctrl1.sar2_patt_len = patt_len - 1;
    }
}

/**
 * Set pattern table for digital controller.
 * The pattern table stores channel selection, attenuation and model bits for scan conversion.
 *
 * @param adc_n ADC unit.
 * @param pattern_index Pattern item index. Range: 0 ~ 15.
 * @param table Stored conversion rule.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_set_pattern_table(adc_unit_t adc_n, uint32_t pattern_index, adc_digi_pattern_config_t table)
{
    HAL_ASSERT(pattern_index < 16);
    uint32_t tab = 0;
    uint8_t index = pattern_index / 4;
    uint8_t offset = (pattern_index % 4) * 6;
    (void)table.atten;
    const uint32_t six_bits = (((uint32_t)table.channel & 0xFU) << 2) | 0U;

    if (adc_n == ADC_UNIT_1) {
        switch (index) {
        case 0: tab = ADC.sar1_patt_tab1.sar1_patt_tab1; break;
        case 1: tab = ADC.sar1_patt_tab2.sar1_patt_tab2; break;
        case 2: tab = ADC.sar1_patt_tab3.sar1_patt_tab3; break;
        default: tab = ADC.sar1_patt_tab4.sar1_patt_tab4; break;
        }
        tab &= ~(0xFC0000U >> offset);
        tab |= (six_bits << 18) >> offset;
        tab &= 0xffffff;
        switch (index) {
        case 0: ADC.sar1_patt_tab1.sar1_patt_tab1 = tab; break;
        case 1: ADC.sar1_patt_tab2.sar1_patt_tab2 = tab; break;
        case 2: ADC.sar1_patt_tab3.sar1_patt_tab3 = tab; break;
        default: ADC.sar1_patt_tab4.sar1_patt_tab4 = tab; break;
        }
    } else {
        switch (index) {
        case 0: tab = ADC.sar2_patt_tab1.sar2_patt_tab1; break;
        case 1: tab = ADC.sar2_patt_tab2.sar2_patt_tab2; break;
        case 2: tab = ADC.sar2_patt_tab3.sar2_patt_tab3; break;
        default: tab = ADC.sar2_patt_tab4.sar2_patt_tab4; break;
        }
        tab &= ~(0xFC0000U >> offset);
        tab |= (six_bits << 18) >> offset;
        tab &= 0xffffff;
        switch (index) {
        case 0: ADC.sar2_patt_tab1.sar2_patt_tab1 = tab; break;
        case 1: ADC.sar2_patt_tab2.sar2_patt_tab2 = tab; break;
        case 2: ADC.sar2_patt_tab3.sar2_patt_tab3 = tab; break;
        default: ADC.sar2_patt_tab4.sar2_patt_tab4 = tab; break;
        }
    }
}

/**
 * Reset the pattern table pointer, then take the measurement rule from table header in next measurement.
 *
 * @param adc_n ADC unit.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_clear_pattern_table(adc_unit_t adc_n)
{
    if (adc_n == ADC_UNIT_1) {
        ADC.ctrl0.sar1_patt_p_clear = 1;
        ADC.ctrl0.sar1_patt_p_clear = 0;
    } else {
        ADC.ctrl1.sar2_patt_p_clear = 1;
        ADC.ctrl1.sar2_patt_p_clear = 0;
    }
}

/**
 * Set the interval clock cycle for the digital controller to trigger the measurement.
 *
 * @note The trigger interval should not be smaller than the sampling time of the SAR ADC.
 * @param cycle The clock cycle of the measurement trigger interval.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_set_trigger_interval(uint32_t cycle)
{
    HAL_ASSERT(cycle <= 4095);
    ADC.ctrl2.timer_target = cycle & 0xFFFU;
}

/**
 * Enable digital controller timer to trigger the measurement.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_trigger_enable(void)
{
    ADC.ctrl2.timer_en = 1;
}

/**
 * Disable digital controller timer to trigger the measurement.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_trigger_disable(void)
{
    ADC.ctrl2.timer_en = 0;
}

/**
 * Set DMA EOF number of ADC digital controller.
 * If the number of measurements reaches this value, then DMA EOF signal is generated.
 *
 * @param num EOF number of DMA.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_dma_set_eof_num(uint32_t num)
{
    ADC.dma_conf.apb_adc_eof_num = num & 0xFFFFU;
}

/**
 * Enable output data to DMA from ADC digital controller.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_dma_enable(void)
{
    ADC.dma_conf.apb_adc_trans = 1;
}

/**
 * Disable output data to DMA from ADC digital controller.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_dma_disable(void)
{
    ADC.dma_conf.apb_adc_trans = 0;
}

/**
 * Reset ADC digital controller.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_reset(void)
{
    ADC.dma_conf.dma_eof_num_clr = 1;
    ADC.dma_conf.dma_eof_num_clr = 0;
}

/*---------------------------------------------------------------
                    Common setting
---------------------------------------------------------------*/

/**
 * @brief Enable the ADC bus clock
 */
__attribute__((always_inline))
static inline void _adc_ll_enable_bus_clock(bool enable)
{
    LP_PERI_CLKRST.adc_ctrl.lp_adc_clk_en = enable;
}
#define adc_ll_enable_bus_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _adc_ll_enable_bus_clock(__VA_ARGS__); \
    } while(0)

/**
 * @brief Enable the ADC function clock (for chips with independent RCC)
 */
__attribute__((always_inline))
static inline void _adc_ll_enable_func_clock(bool enable)
{
    ADC.ctrl_date.clk_en = enable;
}
#define adc_ll_enable_func_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _adc_ll_enable_func_clock(__VA_ARGS__); \
    } while(0)

/**
 * @brief Reset ADC module
 */
__attribute__((always_inline))
static inline void _adc_ll_reset_register(void)
{
    LP_PERI_CLKRST.adc_ctrl.lp_adc_rst_en = 1;
    LP_PERI_CLKRST.adc_ctrl.lp_adc_rst_en = 0;
}
#define adc_ll_reset_register(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _adc_ll_reset_register(__VA_ARGS__); \
    } while(0)

/**
 * @brief Enable ADC reference generator.
 */
__attribute__((always_inline))
static inline void adc_ll_enable_refgen(void)
{
    ADC.ref_control.rtc_xpd_refgen = 1;
    ADC.ref_control.rtc_pre_charge = 1;
    ADC.ref_control.rtc_ref_delay = 1;
}

/**
 * @brief Set ADC power management.
 *        S31 uses ADC_XPD_SAR1_FORCE / ADC_XPD_SAR2_FORCE directly.
 *        0x=auto(FSM), 10=force off, 11=force on
 */
__attribute__((always_inline))
static inline void adc_ll_set_power_manage(adc_unit_t adc_n, adc_ll_power_t manage)
{
    if (manage != ADC_LL_POWER_SW_OFF) {
        adc_ll_enable_refgen();
    }

    if (adc_n == ADC_UNIT_1) {
        ADC.ctrl0.xpd_sar1_force = manage;
    } else {
        ADC.ctrl1.xpd_sar2_force = manage;
    }
}

/**
 * @brief Set ADC module controller.
 *        S31 has a unified controller, this is a no-op.
 */
__attribute__((always_inline))
static inline void adc_ll_set_controller(adc_unit_t adc_n, adc_ll_controller_t ctrl)
{
    (void)adc_n;
    (void)ctrl;
}

/*---------------------------------------------------------------
                    Oneshot Read
---------------------------------------------------------------*/

/**
 * @brief Validate the requested output width.
 */
static inline void adc_oneshot_ll_set_output_bits(adc_unit_t adc_n, adc_bitwidth_t bits)
{
    (void)adc_n;
    HAL_ASSERT(bits == SOC_ADC_RTC_MAX_BITWIDTH || bits == ADC_BITWIDTH_DEFAULT);
}

/**
 * Program the first pattern entry for oneshot conversion.
 *
 * @param adc_n ADC unit.
 * @param channel ADC channel number for each ADCn.
 */
static inline void adc_oneshot_ll_program_pattern(adc_unit_t adc_n, uint32_t channel)
{
    const uint32_t patt_val = ((channel & 0xFU) << 2);
    if (adc_n == ADC_UNIT_1) {
        ADC.ctrl0.sar1_patt_type = 1;
        ADC.ctrl0.sar1_patt_len = 0;
        ADC.sar1_patt_tab1.sar1_patt_tab1 = (patt_val << 18);
        ADC.ctrl0.sar1_patt_p_clear = 1;
        ADC.ctrl0.sar1_patt_p_clear = 0;
        ADC.ctrl0.sar1_trigger_mode = ADC_LL_TRIGGER_MODE_SW;
    } else {
        ADC.ctrl1.sar2_patt_type = 1;
        ADC.ctrl1.sar2_patt_len = 0;
        ADC.sar2_patt_tab1.sar2_patt_tab1 = (patt_val << 18);
        ADC.ctrl1.sar2_patt_p_clear = 1;
        ADC.ctrl1.sar2_patt_p_clear = 0;
        ADC.ctrl1.sar2_trigger_mode = ADC_LL_TRIGGER_MODE_SW;
    }
}

/**
 * Enable ADC channel to start convert.
 *
 * @note Only one channel can be selected for measurement.
 *
 * @param adc_n ADC unit.
 * @param channel ADC channel number for each ADCn.
 */
static inline void adc_oneshot_ll_set_channel(adc_unit_t adc_n, adc_channel_t channel)
{
    adc_oneshot_ll_program_pattern(adc_n, (uint32_t)channel);
}

/**
 * @brief Disable adc channel to start convert.
 *        Return the selected SAR trigger source to OFF.
 */
static inline void adc_oneshot_ll_disable_channel(adc_unit_t adc_n)
{
    if (adc_n == ADC_UNIT_1) {
        ADC.ctrl0.sar1_trigger_mode = ADC_LL_TRIGGER_MODE_OFF;
    } else {
        ADC.ctrl1.sar2_trigger_mode = ADC_LL_TRIGGER_MODE_OFF;
    }
}

/**
 * @brief Start one conversion by software trigger.
 */
static inline void adc_oneshot_ll_start(adc_unit_t adc_n)
{
    if (adc_n == ADC_UNIT_1) {
        ADC.ctrl0.sar1_trigger_start = 1;
    } else {
        ADC.ctrl1.sar2_trigger_start = 1;
    }
}

/**
 * @brief Clear the event for each ADCn for Oneshot mode
 */
static inline void adc_oneshot_ll_clear_event(uint32_t event_mask)
{
    ADC.int_clr.val = event_mask;
}

/**
 * @brief Check the event for each ADCn for Oneshot mode
 *
 * @return
 *      -true  : The conversion process is finished.
 *      -false : The conversion process is not finished.
 */
static inline bool adc_oneshot_ll_get_event(uint32_t event)
{
    if (event == ADC_LL_EVENT_ADC1_ONESHOT_DONE) {
        return (bool)ADC.int_raw.sar1_done_int_raw;
    } else if (event == ADC_LL_EVENT_ADC2_ONESHOT_DONE) {
        return (bool)ADC.int_raw.sar2_done_int_raw;
    }
    HAL_ASSERT(false);
    return false;
}

/**
 * @brief Get the converted value for each ADCn.
 *        S31 output: 22-bit [21:17]=channel, [16:0]=17-bit data.
 *        Returns the 17-bit data portion.
 */
static inline uint32_t adc_oneshot_ll_get_raw_result(adc_unit_t adc_n)
{
    if (adc_n == ADC_UNIT_1) {
        return HAL_FORCE_READ_U32_REG_FIELD(ADC.sar1_data_status, apb_saradc1_data) & 0x1FFFF;
    } else {
        return HAL_FORCE_READ_U32_REG_FIELD(ADC.sar2_data_status, apb_saradc2_data) & 0x1FFFF;
    }
}

/**
 * @brief Check whether the raw result is valid.
 */
static inline bool adc_oneshot_ll_raw_check_valid(adc_unit_t adc_n, uint32_t raw)
{
    (void)adc_n;
    (void)raw;
    return true;
}

/**
 * @brief Configure oneshot output inversion.
 */
static inline void adc_oneshot_ll_output_invert(adc_unit_t adc_n, bool inv_en)
{
    (void)adc_n;
    (void)inv_en;
}

/**
 * @brief Prepare the hardware for oneshot conversion.
 */
static inline void adc_oneshot_ll_enable(adc_unit_t adc_n)
{
    ADC.ctrl2.timer_en = 0;
    adc_ll_enable_refgen();

    if (adc_n == ADC_UNIT_1) {
        ADC.ctrl0.sar1_continue_mode_en = 0;
        ADC.ctrl0.sar1_trigger_stop = 1;
    } else {
        ADC.ctrl1.sar2_continue_mode_en = 0;
        ADC.ctrl1.sar2_trigger_stop = 1;
    }
}

/**
 * @brief Disable oneshot conversion trigger for all the ADC units
 *        Both SAR trigger sources are returned to OFF state.
 */
static inline void adc_oneshot_ll_disable_all_unit(void)
{
    ADC.ctrl0.sar1_trigger_mode = ADC_LL_TRIGGER_MODE_OFF;
    ADC.ctrl1.sar2_trigger_mode = ADC_LL_TRIGGER_MODE_OFF;
}

/**
 * @brief Set attenuation for the first oneshot pattern entry.
 */
static inline void adc_oneshot_ll_set_atten(adc_unit_t adc_n, adc_channel_t channel, adc_atten_t atten)
{
    (void)adc_n;
    (void)channel;
    (void)atten;
}

#ifdef __cplusplus
}
#endif

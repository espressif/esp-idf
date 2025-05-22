/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
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
#include "soc/rtc_cntl_struct.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/clk_tree_defs.h"
#include "soc/system_struct.h"
#include "hal/misc.h"
#include "hal/assert.h"
#include "hal/adc_types.h"
#include "hal/adc_types_private.h"
#include "hal/regi2c_ctrl.h"
#include "soc/regi2c_saradc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADC_LL_EVENT_ADC1_ONESHOT_DONE    BIT(31)
#define ADC_LL_EVENT_ADC2_ONESHOT_DONE    BIT(30)

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
    ADC_LL_CTRL_DIG = 0,    ///< For ADC1. Select DIG controller.
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
    HAL_FORCE_MODIFY_U32_REG_FIELD(APB_SARADC.saradc_fsm_wait, saradc_saradc_rstb_wait, rst_wait);
    // Internal FSM start wait time
    HAL_FORCE_MODIFY_U32_REG_FIELD(APB_SARADC.saradc_fsm_wait, saradc_saradc_xpd_wait, start_wait);
    // Internal FSM standby wait time
    HAL_FORCE_MODIFY_U32_REG_FIELD(APB_SARADC.saradc_fsm_wait, saradc_saradc_standby_wait, standby_wait);
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
    /* ADC clock divided from digital controller clock clk */
    HAL_FORCE_MODIFY_U32_REG_FIELD(APB_SARADC.saradc_ctrl, saradc_saradc_sar_clk_div, div);
}

/**
 * Sets the number of cycles required for the conversion to complete and wait for the arbiter to stabilize.
 *
 * @note Only ADC2 have arbiter function.
 * @param cycle range: 0 ~ 4.
 */
static inline void adc_ll_digi_set_arbiter_stable_cycle(uint32_t cycle)
{
    APB_SARADC.saradc_ctrl.saradc_saradc_wait_arb_cycle = cycle;
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
        APB_SARADC.saradc_ctrl2.saradc_saradc_sar1_inv = inv_en;   // Enable / Disable ADC data invert
    } else { // adc_n == ADC_UNIT_2
        APB_SARADC.saradc_ctrl2.saradc_saradc_sar2_inv = inv_en;   // Enable / Disable ADC data invert
    }
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
    HAL_FORCE_MODIFY_U32_REG_FIELD(APB_SARADC.saradc_apb_adc_clkm_conf, saradc_reg_clkm_div_num, div_num);
    APB_SARADC.saradc_apb_adc_clkm_conf.saradc_reg_clkm_div_b = div_b;
    APB_SARADC.saradc_apb_adc_clkm_conf.saradc_reg_clkm_div_a = div_a;
}

/**
 * Enable clock and select clock source for ADC digital controller.
 *
 * @param clk_src clock source for ADC digital controller.
 */
static inline void adc_ll_digi_clk_sel(adc_continuous_clk_src_t clk_src)
{
    APB_SARADC.saradc_apb_adc_clkm_conf.saradc_reg_clk_sel = 2;
    APB_SARADC.saradc_ctrl.saradc_saradc_sar_clk_gated = 1;
}

/**
 * Disable clock for ADC digital controller.
 */
static inline void adc_ll_digi_controller_clk_disable(void)
{
    APB_SARADC.saradc_ctrl.saradc_saradc_sar_clk_gated = 0;
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
    APB_SARADC.saradc_filter_ctrl0.saradc_filter_reset = 1;
    APB_SARADC.saradc_filter_ctrl0.saradc_filter_reset = 0;
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
        APB_SARADC.saradc_filter_ctrl0.saradc_filter_channel0 = ((adc_n + 1) << 3) | (channel & 0x7);
        APB_SARADC.saradc_filter_ctrl1.saradc_filter_factor0 = factor_reg_val;
    } else if (idx == ADC_DIGI_IIR_FILTER_1) {
        APB_SARADC.saradc_filter_ctrl0.saradc_filter_channel1 = ((adc_n + 1) << 3) | (channel & 0x7);
        APB_SARADC.saradc_filter_ctrl1.saradc_filter_factor1 = factor_reg_val;
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
            APB_SARADC.saradc_filter_ctrl0.saradc_filter_channel0 = 0xF;
            APB_SARADC.saradc_filter_ctrl1.saradc_filter_factor0 = 0;
        } else if (idx == ADC_DIGI_IIR_FILTER_1) {
            APB_SARADC.saradc_filter_ctrl0.saradc_filter_channel1 = 0xF;
            APB_SARADC.saradc_filter_ctrl1.saradc_filter_factor1 = 0;
        }
    }
    //nothing to do to enable, after adc_ll_digi_filter_set_factor, it's enabled.
}

/**
 * Reset adc digital controller.
 */
static inline void adc_ll_digi_reset(void)
{
    APB_SARADC.saradc_dma_conf.saradc_apb_adc_reset_fsm = 1;
    APB_SARADC.saradc_dma_conf.saradc_apb_adc_reset_fsm = 0;
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
    /* Bit1  0:Fsm  1: SW mode
       Bit0  0:SW mode power down  1: SW mode power on */
    if (manage == ADC_LL_POWER_SW_ON) {
        APB_SARADC.saradc_ctrl.saradc_saradc_sar_clk_gated = 1;
        APB_SARADC.saradc_ctrl.saradc_saradc_xpd_sar_force = 3;
    } else if (manage == ADC_LL_POWER_BY_FSM) {
        APB_SARADC.saradc_ctrl.saradc_saradc_sar_clk_gated = 1;
        APB_SARADC.saradc_ctrl.saradc_saradc_xpd_sar_force = 0;
    } else if (manage == ADC_LL_POWER_SW_OFF) {
        APB_SARADC.saradc_ctrl.saradc_saradc_sar_clk_gated = 0;
        APB_SARADC.saradc_ctrl.saradc_saradc_xpd_sar_force = 2;
    }
}

__attribute__((always_inline))
static inline void adc_ll_set_controller(adc_unit_t adc_n, adc_ll_controller_t ctrl)
{
    //Not used on ESP32-C2
}

/* ADC calibration code. */
/**
 * @brief Set common calibration configuration. Should be shared with other parts (PWDET).
 */
__attribute__((always_inline))
static inline void adc_ll_calibration_init(adc_unit_t adc_n)
{
    (void)adc_n;
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_DREF_ADDR, 1);
}

/**
 * Configure the registers for ADC calibration. You need to call the ``adc_ll_calibration_finish`` interface to resume after calibration.
 *
 * @note  Different ADC units and different attenuation options use different calibration data (initial data).
 *
 * @param adc_n ADC index number.
 * @param internal_gnd true:  Disconnect from the IO port and use the internal GND as the calibration voltage.
 *                     false: Use IO external voltage as calibration voltage.
 */
static inline void adc_ll_calibration_prepare(adc_unit_t adc_n, bool internal_gnd)
{
    (void)adc_n;
    /* Enable/disable internal connect GND (for calibration). */
    if (internal_gnd) {
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_ENCAL_GND_ADDR, 1);
    } else {
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_ENCAL_GND_ADDR, 0);
    }
}

/**
 * Resume register status after calibration.
 *
 * @param adc_n ADC index number.
 */
static inline void adc_ll_calibration_finish(adc_unit_t adc_n)
{
    (void)adc_n;
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_ENCAL_GND_ADDR, 0);
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
    (void)adc_n;
    uint8_t msb = param >> 8;
    uint8_t lsb = param & 0xFF;
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_INITIAL_CODE_HIGH_ADDR, msb);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_INITIAL_CODE_LOW_ADDR, lsb);
}

/*---------------------------------------------------------------
                    Oneshot Read
---------------------------------------------------------------*/
/**
 * Set adc output data format for oneshot mode
 *
 * @note ESP32C2 Oneshot mode only supports 12bit.
 * @param adc_n ADC unit.
 * @param bits  Output data bits width option.
 */
static inline void adc_oneshot_ll_set_output_bits(adc_unit_t adc_n, adc_bitwidth_t bits)
{
    //ESP32C2 only supports 12bit, leave here for compatibility
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
    HAL_ASSERT(adc_n == ADC_UNIT_1);
    APB_SARADC.saradc_onetime_sample.saradc_saradc_onetime_channel = ((adc_n << 3) | channel);
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
    HAL_ASSERT(adc_n == ADC_UNIT_1);
    APB_SARADC.saradc_onetime_sample.saradc_saradc_onetime_channel = ((adc_n << 3) | 0xF);
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
    APB_SARADC.saradc_onetime_sample.saradc_saradc_onetime_start = val;
}

/**
 * Clear the event for each ADCn for Oneshot mode
 *
 * @param event ADC event
 */
static inline void adc_oneshot_ll_clear_event(uint32_t event_mask)
{
    APB_SARADC.saradc_int_clr.val |= event_mask;
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
    return (APB_SARADC.saradc_int_raw.val & event_mask);
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
    HAL_ASSERT(adc_n == ADC_UNIT_1);
    uint32_t ret_val = 0;
    ret_val = APB_SARADC.saradc1_data_status.saradc1_data & 0xfff;
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
    HAL_ASSERT(adc_n == ADC_UNIT_1);
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
    HAL_ASSERT(adc_n == ADC_UNIT_1);
    APB_SARADC.saradc_onetime_sample.saradc_saradc1_onetime_sample = 1;
}

/**
 * Disable oneshot conversion trigger for all the ADC units
 */
static inline void adc_oneshot_ll_disable_all_unit(void)
{
    APB_SARADC.saradc_onetime_sample.saradc_saradc1_onetime_sample = 0;
    APB_SARADC.saradc_onetime_sample.saradc_saradc2_onetime_sample = 0;
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
    APB_SARADC.saradc_onetime_sample.saradc_saradc_onetime_atten = atten;
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
    return (adc_atten_t)(APB_SARADC.saradc_onetime_sample.saradc_saradc_onetime_atten);
}

#ifdef __cplusplus
}
#endif

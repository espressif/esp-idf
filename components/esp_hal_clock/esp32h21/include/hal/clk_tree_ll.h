/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc.h"
#include "soc/clk_tree_defs.h"
#include "soc/pcr_struct.h"
#include "soc/lp_clkrst_struct.h"
#include "soc/pmu_reg.h"
#include "hal/regi2c_ctrl.h"
#include "soc/regi2c_bbpll.h"
#include "hal/assert.h"
#include "hal/log.h"
#include "esp32h21/rom/rtc.h"
#include "hal/misc.h"
#include "soc/timer_group_struct.h"

#define MHZ                 (1000000)

#define CLK_LL_PLL_48M_FREQ_MHZ   (48)
// #define CLK_LL_PLL_64M_FREQ_MHZ   (64)
#define CLK_LL_PLL_96M_FREQ_MHZ   (96)

#define CLK_LL_XTAL32K_CONFIG_DEFAULT() { \
    .dac = 3, \
    .dres = 3, \
    .dgm = 3, \
    .dbuf = 1, \
}

// Fix default division factor for the RC_FAST clock for calibration to be 32
#define CLK_LL_RC_FAST_CALIB_TICK_DIV_BITS        5

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief XTAL32K_CLK enable modes
 */
typedef enum {
    CLK_LL_XTAL32K_ENABLE_MODE_CRYSTAL,       //!< Enable the external 32kHz crystal for XTAL32K_CLK
    CLK_LL_XTAL32K_ENABLE_MODE_EXTERNAL,      //!< Enable the external clock signal for XTAL32K_CLK
    CLK_LL_XTAL32K_ENABLE_MODE_BOOTSTRAP,     //!< Bootstrap the crystal oscillator for faster XTAL32K_CLK start up */
} clk_ll_xtal32k_enable_mode_t;

/**
 * @brief XTAL32K_CLK configuration structure
 */
typedef struct {
    uint32_t dac : 6;
    uint32_t dres : 3;
    uint32_t dgm : 3;
    uint32_t dbuf: 1;
} clk_ll_xtal32k_config_t;

/**
 * @brief Power up BBPLL circuit
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_enable(void)
{
    SET_PERI_REG_MASK(PMU_IMM_HP_CK_POWER_REG, PMU_TIE_HIGH_XPD_BB_I2C | PMU_TIE_HIGH_XPD_BBPLL | PMU_TIE_HIGH_XPD_BBPLL_I2C);
    SET_PERI_REG_MASK(PMU_IMM_HP_CK_POWER_REG, PMU_TIE_HIGH_GLOBAL_BBPLL_ICG);
}

/**
 * @brief Power down BBPLL circuit
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_disable(void)
{
    CLEAR_PERI_REG_MASK(PMU_IMM_HP_CK_POWER_REG, PMU_TIE_HIGH_XPD_BB_I2C | PMU_TIE_HIGH_XPD_BBPLL | PMU_TIE_HIGH_XPD_BBPLL_I2C);
    CLEAR_PERI_REG_MASK(PMU_IMM_HP_CK_POWER_REG, PMU_TIE_HIGH_GLOBAL_BBPLL_ICG);
    SET_PERI_REG_MASK(PMU_IMM_HP_CK_POWER_REG, PMU_TIE_LOW_XPD_BBPLL_I2C | PMU_TIE_LOW_XPD_BBPLL | PMU_TIE_LOW_XPD_BBPLL_I2C);
    SET_PERI_REG_MASK(PMU_IMM_HP_CK_POWER_REG, PMU_TIE_LOW_GLOBAL_BBPLL_ICG);
}

// /**
//  * @brief Power up XTAL_X2 circuit
//  */
// static inline __attribute__((always_inline)) void clk_ll_xtal_x2_enable(void)
// {
//     CLEAR_PERI_REG_MASK(PMU_IMM_HP_CK_POWER_REG, PMU_TIE_LOW_XPD_XTALX2);
//     CLEAR_PERI_REG_MASK(PMU_IMM_HP_CK_POWER_REG, PMU_TIE_LOW_GLOBAL_XTALX2_ICG);
//     SET_PERI_REG_MASK(PMU_IMM_HP_CK_POWER_REG, PMU_TIE_HIGH_XTALX2);
//     SET_PERI_REG_MASK(PMU_IMM_HP_CK_POWER_REG, PMU_TIE_HIGH_GLOBAL_XTALX2_ICG);
// }

// /**
//  * @brief Power down XTAL_X2 circuit
//  */
// static inline __attribute__((always_inline)) void clk_ll_xtal_x2_disable(void)
// {
//     CLEAR_PERI_REG_MASK(PMU_IMM_HP_CK_POWER_REG, PMU_TIE_HIGH_XTALX2 | PMU_TIE_HIGH_GLOBAL_XTALX2_ICG);
//     SET_PERI_REG_MASK(PMU_IMM_HP_CK_POWER_REG, PMU_TIE_LOW_XPD_XTALX2);
//     SET_PERI_REG_MASK(PMU_IMM_HP_CK_POWER_REG, PMU_TIE_LOW_GLOBAL_XTALX2_ICG);
// }

/**
 * @brief Enable the 32kHz crystal oscillator
 *
 * @param mode Used to determine the xtal32k configuration parameters
 */
static inline __attribute__((always_inline)) void clk_ll_xtal32k_enable(clk_ll_xtal32k_enable_mode_t mode)
{
    if (mode == CLK_LL_XTAL32K_ENABLE_MODE_EXTERNAL) {
        // No need to configure anything for OSC_SLOW_CLK
        return;
    }
    // Configure xtal32k
    clk_ll_xtal32k_config_t cfg = CLK_LL_XTAL32K_CONFIG_DEFAULT();
    LP_CLKRST.xtal32k.clkrst_dac_xtal32k = cfg.dac;
    LP_CLKRST.xtal32k.clkrst_dres_xtal32k = cfg.dres;
    LP_CLKRST.xtal32k.clkrst_dgm_xtal32k = cfg.dgm;
    LP_CLKRST.xtal32k.clkrst_dbuf_xtal32k = cfg.dbuf;
    // Enable xtal32k xpd
    SET_PERI_REG_MASK(PMU_HP_SLEEP_LP_CK_POWER_REG, PMU_HP_SLEEP_XPD_XTAL32K);
}

/**
 * @brief Disable the 32kHz crystal oscillator
 */
static inline __attribute__((always_inline)) void clk_ll_xtal32k_disable(void)
{
    // Disable xtal32k xpd
    CLEAR_PERI_REG_MASK(PMU_HP_SLEEP_LP_CK_POWER_REG, PMU_HP_SLEEP_XPD_XTAL32K);
}

/**
 * @brief Get the state of the 32kHz crystal clock
 *
 * @return True if the 32kHz XTAL is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_xtal32k_is_enabled(void)
{
    return REG_GET_FIELD(PMU_HP_SLEEP_LP_CK_POWER_REG, PMU_HP_SLEEP_XPD_XTAL32K) == 1;
}

/**
 * @brief Enable the internal oscillator output for RC_FAST_CLK
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_enable(void)
{
    SET_PERI_REG_MASK(PMU_HP_SLEEP_LP_CK_POWER_REG, PMU_HP_SLEEP_XPD_FOSC_CLK);
}

/**
 * @brief Disable the internal oscillator output for RC_FAST_CLK
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_disable(void)
{
    CLEAR_PERI_REG_MASK(PMU_HP_SLEEP_LP_CK_POWER_REG, PMU_HP_SLEEP_XPD_FOSC_CLK);
}

/**
 * @brief Get the state of the internal oscillator for RC_FAST_CLK
 *
 * @return True if the oscillator is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_rc_fast_is_enabled(void)
{
    return REG_GET_FIELD(PMU_HP_SLEEP_LP_CK_POWER_REG, PMU_HP_SLEEP_XPD_FOSC_CLK) == 1;
}

/**
 * @brief Enable the digital RC_FAST_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_digi_enable(void)
{
    LP_CLKRST.clk_to_hp.clkrst_icg_hp_fosc = 1;
}

/**
 * @brief Disable the digital RC_FAST_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_digi_disable(void)
{
    LP_CLKRST.clk_to_hp.clkrst_icg_hp_fosc = 0;
}

/**
 * @brief Get the state of the digital RC_FAST_CLK
 *
 * @return True if the digital RC_FAST_CLK is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_rc_fast_digi_is_enabled(void)
{
    return LP_CLKRST.clk_to_hp.clkrst_icg_hp_fosc;
}

/**
 * @brief Enable the digital XTAL32K_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_xtal32k_digi_enable(void)
{
    LP_CLKRST.clk_to_hp.clkrst_icg_hp_xtal32k = 1;
}

/**
 * @brief Disable the digital XTAL32K_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_xtal32k_digi_disable(void)
{
    LP_CLKRST.clk_to_hp.clkrst_icg_hp_xtal32k = 0;
}

/**
 * @brief Get the state of the digital XTAL32K_CLK
 *
 * @return True if the digital XTAL32K_CLK is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_xtal32k_digi_is_enabled(void)
{
    return LP_CLKRST.clk_to_hp.clkrst_icg_hp_xtal32k;
}

/**
 * @brief Get XTAL_CLK frequency
 *
 * @return Main XTAL clock frequency, in MHz.
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_xtal_get_freq_mhz(void)
{
    return PCR.sysclk_conf.clk_xtal_freq;
}

/**
 * @brief Get PLL_CLK frequency
 *
 * @return PLL clock frequency, in MHz. Returns 0 if register field value is invalid.
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_bbpll_get_freq_mhz(void)
{
    // The target has a fixed 96MHz SPLL
    return CLK_LL_PLL_96M_FREQ_MHZ;
}

/**
 * @brief Set BBPLL frequency from XTAL source (Digital part)
 *
 * @param pll_freq_mhz PLL frequency, in MHz
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_set_freq_mhz(uint32_t pll_freq_mhz)
{
    // The target SPLL is fixed to 96MHz
    // Do nothing
    HAL_ASSERT(pll_freq_mhz == CLK_LL_PLL_96M_FREQ_MHZ);
}

/**
 * @brief Set BBPLL frequency from XTAL source (Analog part)
 *
 * @param pll_freq_mhz PLL frequency, in MHz
 * @param xtal_freq_mhz XTAL frequency, in MHz
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_set_config(uint32_t pll_freq_mhz, uint32_t xtal_freq_mhz)
{
    HAL_ASSERT(xtal_freq_mhz == SOC_XTAL_FREQ_32M);
    HAL_ASSERT(pll_freq_mhz == CLK_LL_PLL_96M_FREQ_MHZ);
    uint8_t oc_ref_div;
    uint8_t oc_div;
    uint8_t oc_dhref_sel;
    uint8_t oc_dlref_sel;

    oc_ref_div = 8;
    oc_div = 24;
    oc_dhref_sel = 3;
    oc_dlref_sel = 1;

    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_OC_REF_DIV, oc_ref_div);
    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_OC_DIV, oc_div);
    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_OC_DHREF_SEL, oc_dhref_sel);
    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_OC_DLREF_SEL, oc_dlref_sel);
}

// /**
//  * @brief Get XTAL_X2_CLK frequency
//  *
//  * @return XTAL_X2 clock frequency, in MHz
//  */
// static inline __attribute__((always_inline)) uint32_t clk_ll_xtal_x2_get_freq_mhz(void)
// {
//     return SOC_XTAL_FREQ_32M * 2;
// }

/**
 * @brief To enable the change of soc_clk_sel, cpu_div_num, and ahb_div_num
 */
static inline __attribute__((always_inline)) void clk_ll_bus_update(void)
{
    PCR.bus_clk_update.bus_clock_update = 1;
    while (PCR.bus_clk_update.bus_clock_update);
}

/**
 * @brief Select the clock source for CPU_CLK (SOC Clock Root)
 *
 * @param in_sel One of the clock sources in soc_cpu_clk_src_t
 */
static inline __attribute__((always_inline)) void clk_ll_cpu_set_src(soc_cpu_clk_src_t in_sel)
{
    switch (in_sel) {
    case SOC_CPU_CLK_SRC_XTAL:
        PCR.sysclk_conf.soc_clk_sel = 0;
        break;
    case SOC_CPU_CLK_SRC_PLL:
        PCR.sysclk_conf.soc_clk_sel = 1;
        break;
    case SOC_CPU_CLK_SRC_RC_FAST:
        PCR.sysclk_conf.soc_clk_sel = 2;
        break;
    // case SOC_CPU_CLK_SRC_XTAL_X2:
    //     PCR.sysclk_conf.soc_clk_sel = 3;
    //     break;
    default:
        // Unsupported CPU_CLK mux input sel
        abort();
    }
}

/**
 * @brief Get the clock source for CPU_CLK (SOC Clock Root)
 *
 * @return Currently selected clock source (one of soc_cpu_clk_src_t values)
 */
static inline __attribute__((always_inline)) soc_cpu_clk_src_t clk_ll_cpu_get_src(void)
{
    uint32_t clk_sel = PCR.sysclk_conf.soc_clk_sel;
    switch (clk_sel) {
    case 0:
        return SOC_CPU_CLK_SRC_XTAL;
    case 1:
        return SOC_CPU_CLK_SRC_PLL;
    case 2:
        return SOC_CPU_CLK_SRC_RC_FAST;
    // case 3:
    //     return SOC_CPU_CLK_SRC_XTAL_X2;
    default:
        // Invalid SOC_CLK_SEL value
        return SOC_CPU_CLK_SRC_INVALID;
    }
}

/**
 * @brief Set CPU_CLK divider
 *
 * @param divider Divider. PRE_DIV_CNT = divider - 1.
 */
static inline __attribute__((always_inline)) void clk_ll_cpu_set_divider(uint32_t divider)
{
    HAL_ASSERT(divider >= 1);
    HAL_FORCE_MODIFY_U32_REG_FIELD(PCR.cpu_freq_conf, cpu_div_num, divider - 1);
}

/**
 * @brief Get CPU_CLK divider
 *
 * @return Divider. Divider = (PRE_DIV_CNT + 1).
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_cpu_get_divider(void)
{
    return HAL_FORCE_READ_U32_REG_FIELD(PCR.cpu_freq_conf, cpu_div_num) + 1;
}

/**
 * @brief Set AHB_CLK divider
 *
 * @param divider Divider. PRE_DIV_CNT = divider - 1.
 */
static inline __attribute__((always_inline)) void clk_ll_ahb_set_divider(uint32_t divider)
{
    HAL_ASSERT(divider >= 1);
    HAL_FORCE_MODIFY_U32_REG_FIELD(PCR.ahb_freq_conf, ahb_div_num, divider - 1);
}

/**
 * @brief Get AHB_CLK divider
 *
 * @return Divider. Divider = (PRE_DIV_CNT + 1).
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_ahb_get_divider(void)
{
    return HAL_FORCE_READ_U32_REG_FIELD(PCR.ahb_freq_conf, ahb_div_num) + 1;
}

/**
 * @brief Set APB_CLK divider. freq of APB_CLK = freq of AHB_CLK / divider
 *
 * @param divider Divider. PCR_APB_DIV_NUM = divider - 1.
 */
static inline __attribute__((always_inline)) void clk_ll_apb_set_divider(uint32_t divider)
{
    // AHB ------> APB
    // Divider option: 1, 2, 4 (PCR_APB_DIV_NUM=0, 1, 3)
    HAL_ASSERT(divider == 1 || divider == 2 || divider == 4);
    HAL_FORCE_MODIFY_U32_REG_FIELD(PCR.apb_freq_conf, apb_div_num, divider - 1);
}

/**
 * @brief Get APB_CLK divider
 *
 * @return Divider. Divider = (PCR_APB_DIV_NUM + 1).
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_apb_get_divider(void)
{
    return HAL_FORCE_READ_U32_REG_FIELD(PCR.apb_freq_conf, apb_div_num) + 1;
}

/**
 * @brief Select the frequency calculation clock source for timergroup0
 *
 * @param clk_sel One of the clock sources in soc_clk_freq_calculation_src_t
 */
static inline __attribute__((always_inline)) void clk_ll_freq_calulation_set_target(soc_clk_freq_calculation_src_t clk_sel)
{
    int timg_cali_clk_sel = -1;
    int clk_32k_sel = -1;

    switch (clk_sel) {
    case CLK_CAL_32K_XTAL:
        timg_cali_clk_sel = 2;
        clk_32k_sel = 1;
        break;
    case CLK_CAL_32K_OSC_SLOW:
        timg_cali_clk_sel = 2;
        clk_32k_sel = 2;
        break;
    case CLK_CAL_RC_SLOW:
        timg_cali_clk_sel = 0;
        break;
    case CLK_CAL_RC_FAST:
        timg_cali_clk_sel = 1;
        break;
    default:
        // Unsupported CLK_CAL mux input
        abort();
    }

    if (timg_cali_clk_sel >= 0) {
        TIMERG0.rtccalicfg.rtc_cali_clk_sel = timg_cali_clk_sel;
    }
    if (clk_32k_sel >= 0) {
        PCR.ctrl_32k_conf.clk_32k_sel = clk_32k_sel;
    }
}

/**
 * @brief Set the frequency division factor of RC_FAST clock
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_tick_conf(void)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(PCR.ctrl_tick_conf, fosc_tick_num, (1 << CLK_LL_RC_FAST_CALIB_TICK_DIV_BITS) - 1); // divider = 1 << CLK_LL_RC_FAST_CALIB_TICK_DIV_BITS
}

/**
 * @brief Select the clock source for RTC_SLOW_CLK
 *
 * @param in_sel One of the clock sources in soc_rtc_slow_clk_src_t
 */
static inline __attribute__((always_inline)) void clk_ll_rtc_slow_set_src(soc_rtc_slow_clk_src_t in_sel)
{
    switch (in_sel) {
    case SOC_RTC_SLOW_CLK_SRC_RC_SLOW_D4:
        LP_CLKRST.lp_clk_conf.clkrst_slow_clk_sel = 0;
        break;
    case SOC_RTC_SLOW_CLK_SRC_XTAL32K:
        LP_CLKRST.lp_clk_conf.clkrst_slow_clk_sel = 1;
        break;
    case SOC_RTC_SLOW_CLK_SRC_OSC_SLOW:
        LP_CLKRST.lp_clk_conf.clkrst_slow_clk_sel = 3;
        break;
    default:
        // Unsupported RTC_SLOW_CLK mux input sel
        abort();
    }
}

/**
 * @brief Get the clock source for RTC_SLOW_CLK
 *
 * @return Currently selected clock source (one of soc_rtc_slow_clk_src_t values)
 */
static inline __attribute__((always_inline)) soc_rtc_slow_clk_src_t clk_ll_rtc_slow_get_src(void)
{
    uint32_t clk_sel = LP_CLKRST.lp_clk_conf.clkrst_slow_clk_sel;
    switch (clk_sel) {
    case 0:
        return SOC_RTC_SLOW_CLK_SRC_RC_SLOW_D4;
    case 1:
        return SOC_RTC_SLOW_CLK_SRC_XTAL32K;
    case 3:
        return SOC_RTC_SLOW_CLK_SRC_OSC_SLOW;
    default:
        return SOC_RTC_SLOW_CLK_SRC_INVALID;
    }
}

/**
 * @brief Select the clock source for RTC_FAST_CLK
 *
 * @param in_sel One of the clock sources in soc_rtc_fast_clk_src_t
 */
static inline __attribute__((always_inline)) void clk_ll_rtc_fast_set_src(soc_rtc_fast_clk_src_t in_sel)
{
    switch (in_sel) {
    case SOC_RTC_FAST_CLK_SRC_RC_FAST:
        LP_CLKRST.lp_clk_conf.clkrst_fast_clk_sel = 0;
        break;
    case SOC_RTC_FAST_CLK_SRC_XTAL_D2:
        LP_CLKRST.lp_clk_conf.clkrst_fast_clk_sel = 1;
        break;
    default:
        // Unsupported RTC_FAST_CLK mux input sel
        abort();
    }
}

/**
 * @brief Get the clock source for RTC_FAST_CLK
 *
 * @return Currently selected clock source (one of soc_rtc_fast_clk_src_t values)
 */
static inline __attribute__((always_inline)) soc_rtc_fast_clk_src_t clk_ll_rtc_fast_get_src(void)
{
    uint32_t clk_sel = LP_CLKRST.lp_clk_conf.clkrst_fast_clk_sel;
    switch (clk_sel) {
    case 0:
        return SOC_RTC_FAST_CLK_SRC_RC_FAST;
    case 1:
        return SOC_RTC_FAST_CLK_SRC_XTAL_D2;
    default:
        return SOC_RTC_FAST_CLK_SRC_INVALID;
    }
}

/**
 * @brief Set RC_FAST_CLK divider. The output from the divider is passed into rtc_fast_clk MUX.
 *
 * @param divider Divider of RC_FAST_CLK. Usually this divider is set to 1 (reg. value is 0) in bootloader stage.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_set_divider(uint32_t divider)
{
    // No divider on the target
    HAL_ASSERT(divider == 1);
}

/**
 * @brief Get RC_FAST_CLK divider
 *
 * @return Divider. Divider = (CK8M_DIV_SEL + 1).
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_rc_fast_get_divider(void)
{
    // No divider on the target, always return divider = 1
    return 1;
}

/**
 * @brief Set RC_SLOW_CLK divider. The output from the divider is passed into rtc_slow_clk MUX.
 *
 * @param divider Divider of RC_SLOW_CLK. Fixed the divider to 4 on the target.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_slow_set_divider(uint32_t divider)
{
    // Register not exposed
    HAL_ASSERT(divider == 4);
}

/************************** LP STORAGE REGISTER STORE/LOAD **************************/
/**
 * @brief Store RTC_SLOW_CLK calibration value in RTC storage register
 *
 * Value of RTC_SLOW_CLK_CAL_REG has to be in the same format as returned by rtc_clk_cal (microseconds,
 * in Q13.19 fixed-point format).
 *
 * @param cal_value The calibration value of slow clock period in microseconds, in Q13.19 fixed point format
 */
static inline __attribute__((always_inline)) void clk_ll_rtc_slow_store_cal(uint32_t cal_value)
{
    REG_WRITE(RTC_SLOW_CLK_CAL_REG, cal_value);
}

/**
 * @brief Load the calibration value of RTC_SLOW_CLK frequency from RTC storage register
 *
 * This value gets updated (i.e. rtc slow clock gets calibrated) every time RTC_SLOW_CLK source switches
 *
 * @return The calibration value of slow clock period in microseconds, in Q13.19 fixed point format
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_rtc_slow_load_cal(void)
{
    return REG_READ(RTC_SLOW_CLK_CAL_REG);
}

#ifdef __cplusplus
}
#endif

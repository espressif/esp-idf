/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
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
#include "soc/pmu_struct.h"
#include "hal/regi2c_ctrl.h"
#include "soc/regi2c_bbpll.h"
#include "hal/assert.h"
#include "hal/log.h"
#include "esp32c5/rom/rtc.h"
#include "hal/misc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MHZ                 (1000000)

#define CLK_LL_PLL_80M_FREQ_MHZ    (80)
#define CLK_LL_PLL_120M_FREQ_MHZ   (120)
#define CLK_LL_PLL_160M_FREQ_MHZ   (160)
#define CLK_LL_PLL_240M_FREQ_MHZ   (240)

#define CLK_LL_PLL_480M_FREQ_MHZ   (480)

#define CLK_LL_XTAL32K_CONFIG_DEFAULT() { \
    .dac = 3, \
    .dres = 3, \
    .dgm = 3, \
    .dbuf = 1, \
}

/*
Set the frequency division factor of ref_tick
The FOSC of rtc calibration uses the 32 frequency division clock,
So the frequency division factor of ref_tick must be greater than or equal to 32
*/
#define REG_FOSC_TICK_NUM  255 // TODO: IDF-8642 No need? Can calibrate on RC_FAST directly?

/**
 * @brief XTAL32K_CLK enable modes
 */
typedef enum {
    CLK_LL_XTAL32K_ENABLE_MODE_CRYSTAL,       //!< Enable the external 32kHz crystal for XTAL32K_CLK
    CLK_LL_XTAL32K_ENABLE_MODE_EXTERNAL,      //!< Enable the external clock signal for OSC_SLOW_CLK
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
    SET_PERI_REG_MASK(PMU_IMM_HP_CK_POWER_REG, PMU_TIE_HIGH_XPD_BB_I2C |
                      PMU_TIE_HIGH_XPD_BBPLL | PMU_TIE_HIGH_XPD_BBPLL_I2C);
    SET_PERI_REG_MASK(PMU_IMM_HP_CK_POWER_REG, PMU_TIE_HIGH_GLOBAL_BBPLL_ICG);
}

/**
 * @brief Power down BBPLL circuit
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_disable(void)
{
    SET_PERI_REG_MASK(PMU_IMM_HP_CK_POWER_REG, PMU_TIE_LOW_GLOBAL_BBPLL_ICG) ;
    SET_PERI_REG_MASK(PMU_IMM_HP_CK_POWER_REG, PMU_TIE_LOW_XPD_BBPLL | PMU_TIE_LOW_XPD_BBPLL_I2C);
}

/**
 * @brief Release the root clock source locked by PMU
 */
static inline __attribute__((always_inline)) void clk_ll_cpu_clk_src_lock_release(void)
{
    SET_PERI_REG_MASK(PMU_IMM_SLEEP_SYSCLK_REG, PMU_UPDATE_DIG_SYS_CLK_SEL);
}

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
    LP_CLKRST.xtal32k.dac_xtal32k = cfg.dac;
    LP_CLKRST.xtal32k.dres_xtal32k = cfg.dres;
    LP_CLKRST.xtal32k.dgm_xtal32k = cfg.dgm;
    LP_CLKRST.xtal32k.dbuf_xtal32k = cfg.dbuf;
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
 * @brief Enable the internal oscillator output for RC32K_CLK
 */
static inline __attribute__((always_inline)) void clk_ll_rc32k_enable(void)
{
    // Enable rc32k xpd status
    SET_PERI_REG_MASK(PMU_HP_SLEEP_LP_CK_POWER_REG, PMU_HP_SLEEP_XPD_RC32K);
}

/**
 * @brief Disable the internal oscillator output for RC32K_CLK
 */
static inline __attribute__((always_inline)) void clk_ll_rc32k_disable(void)
{
    // Disable rc32k xpd status
    CLEAR_PERI_REG_MASK(PMU_HP_SLEEP_LP_CK_POWER_REG, PMU_HP_SLEEP_XPD_RC32K);
}

/**
 * @brief Get the state of the internal oscillator for RC32K_CLK
 *
 * @return True if the oscillator is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_rc32k_is_enabled(void)
{
    return REG_GET_FIELD(PMU_HP_SLEEP_LP_CK_POWER_REG, PMU_HP_SLEEP_XPD_RC32K) == 1;
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
    LP_CLKRST.clk_to_hp.icg_hp_fosc = 1;
}

/**
 * @brief Disable the digital RC_FAST_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_digi_disable(void)
{
    LP_CLKRST.clk_to_hp.icg_hp_fosc = 0;
}

/**
 * @brief Get the state of the digital RC_FAST_CLK
 *
 * @return True if the digital RC_FAST_CLK is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_rc_fast_digi_is_enabled(void)
{
    return LP_CLKRST.clk_to_hp.icg_hp_fosc;
}

/**
 * @brief Enable the digital XTAL32K_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_xtal32k_digi_enable(void)
{
    LP_CLKRST.clk_to_hp.icg_hp_xtal32k = 1;
}

/**
 * @brief Disable the digital XTAL32K_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_xtal32k_digi_disable(void)
{
    LP_CLKRST.clk_to_hp.icg_hp_xtal32k = 0;
}

/**
 * @brief Get the state of the digital XTAL32K_CLK
 *
 * @return True if the digital XTAL32K_CLK is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_xtal32k_digi_is_enabled(void)
{
    return LP_CLKRST.clk_to_hp.icg_hp_xtal32k;
}

/**
 * @brief Enable the digital RC32K_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_rc32k_digi_enable(void)
{
    LP_CLKRST.clk_to_hp.icg_hp_osc32k = 1;
}

/**
 * @brief Disable the digital RC32K_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_rc32k_digi_disable(void)
{
    LP_CLKRST.clk_to_hp.icg_hp_osc32k = 0;
}

/**
 * @brief Get the state of the digital RC32K_CLK
 *
 * @return True if the digital RC32K_CLK is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_rc32k_digi_is_enabled(void)
{
    return LP_CLKRST.clk_to_hp.icg_hp_osc32k;
}

/**
 * @brief Get PLL_CLK frequency
 *
 * @return PLL clock frequency, in MHz. Returns 0 if register field value is invalid.
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_bbpll_get_freq_mhz(void)
{
    // The target has a fixed 480MHz SPLL
    return CLK_LL_PLL_480M_FREQ_MHZ;
}

/**
 * @brief Set BBPLL frequency from XTAL source (Digital part)
 *
 * @param pll_freq_mhz PLL frequency, in MHz
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_set_freq_mhz(uint32_t pll_freq_mhz)
{
    // The target SPLL is fixed to 480MHz
    // Do nothing
    HAL_ASSERT(pll_freq_mhz == CLK_LL_PLL_480M_FREQ_MHZ);
}

/**
 * @brief Set BBPLL frequency from XTAL source (Analog part)
 *
 * @param pll_freq_mhz PLL frequency, in MHz
 * @param xtal_freq_mhz XTAL frequency, in MHz
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_set_config(uint32_t pll_freq_mhz, uint32_t xtal_freq_mhz)
{
    HAL_ASSERT(pll_freq_mhz == CLK_LL_PLL_480M_FREQ_MHZ);
    uint8_t div_ref;
    uint8_t div7_0;
    uint8_t dr1;
    uint8_t dr3;
    uint8_t dchgp;
    uint8_t dbias;

    uint8_t href = 3;
    uint8_t lref = 1;
    /* Configure 480M PLL */
    switch (xtal_freq_mhz) {
    case SOC_XTAL_FREQ_40M:
        div_ref = 1;
        div7_0 = 12;
        dr1 = 0;
        dr3 = 0;
        dchgp = 5;
        dbias = 2;
        break;
    case SOC_XTAL_FREQ_48M:
        div_ref = 1;
        div7_0 = 10;
        dr1 = 1;
        dr3 = 1;
        dchgp = 5;
        dbias = 2;
        break;
    default:
        div_ref = 1;
        div7_0 = 12;
        dr1 = 0;
        dr3 = 0;
        dchgp = 5;
        dbias = 2;
        break;
    }
    uint8_t i2c_bbpll_lref  = (dchgp << I2C_BBPLL_OC_DCHGP_LSB) | (div_ref);
    uint8_t i2c_bbpll_div_7_0 = div7_0;
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_OC_REF_DIV, i2c_bbpll_lref);
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_OC_DIV_7_0, i2c_bbpll_div_7_0);
    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_OC_DR1, dr1);
    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_OC_DR3, dr3);
    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_OC_DLREF_SEL, lref);
    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_OC_DHREF_SEL, href);
    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_OC_VCO_DBIAS, dbias);
}

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
    case SOC_CPU_CLK_SRC_RC_FAST:
        PCR.sysclk_conf.soc_clk_sel = 1;
        break;
    case SOC_CPU_CLK_SRC_PLL_F160M:
        PCR.sysclk_conf.soc_clk_sel = 2;
        break;
    case SOC_CPU_CLK_SRC_PLL_F240M:
        PCR.sysclk_conf.soc_clk_sel = 3;
        break;
    default:
        // Unsupported SOC_CLK mux input sel
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
        return SOC_CPU_CLK_SRC_RC_FAST;
    case 2:
        return SOC_CPU_CLK_SRC_PLL_F160M;
    case 3:
        return SOC_CPU_CLK_SRC_PLL_F240M;
    default:
        // Invalid SOC_CLK_SEL value
        return SOC_CPU_CLK_SRC_INVALID;
    }
}

/**
 * @brief Set CPU_CLK's divider
 *
 * @param divider Divider. (PCR_CPU_DIV_NUM + 1) = divider.
 */
static inline __attribute__((always_inline)) void clk_ll_cpu_set_divider(uint32_t divider)
{
    // SOC_ROOT_CLK ---(1)---> HP_ROOT_CLK ---(2)---> CPU_CLK
    // (2) configurable
    //  divider option: 1, 2, 4 (PCR_CPU_HS_DIV_NUM=0, 1, 3)
    HAL_ASSERT(divider == 1 || divider == 2 || divider == 3 || divider == 4 || divider == 6);
    HAL_FORCE_MODIFY_U32_REG_FIELD(PCR.cpu_freq_conf, cpu_div_num, (divider) - 1);
}

/**
 * @brief Get CPU_CLK's high-speed divider
 *
 * @return Divider. Divider = (PCR_HS_DIV_NUM + 1) * (PCR_CPU_HS_DIV_NUM + 1).
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_cpu_get_divider(void)
{
    uint32_t cpu_div = HAL_FORCE_READ_U32_REG_FIELD(PCR.cpu_freq_conf, cpu_div_num);
    return (cpu_div + 1);
}

/**
 * @brief Set AHB_CLK's low-speed divider (valid when SOC_ROOT clock source is XTAL/RC_FAST)
 *
 * @param divider Divider. (PCR_LS_DIV_NUM + 1) * (PCR_AHB_LS_DIV_NUM + 1) = divider.
 */
static inline __attribute__((always_inline)) void clk_ll_ahb_set_divider(uint32_t divider)
{
    // SOC_ROOT_CLK ---(1)---> HP_ROOT_CLK ---(2)---> AHB_CLK
    HAL_FORCE_MODIFY_U32_REG_FIELD(PCR.ahb_freq_conf, ahb_div_num, divider - 1);
}

/**
 * @brief Get AHB_CLK's low-speed divider
 *
 * @return Divider. Divider = (PCR_LS_DIV_NUM + 1) * (PCR_AHB_LS_DIV_NUM + 1).
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_ahb_get_divider(void)
{
    uint32_t ahb_div = HAL_FORCE_READ_U32_REG_FIELD(PCR.ahb_freq_conf, ahb_div_num);
    uint32_t hp_root_ls_div = HAL_FORCE_READ_U32_REG_FIELD(PCR.sysclk_conf, ls_div_num);
    return (hp_root_ls_div + 1) * (ahb_div + 1);
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
 * @brief Select the clock source for MSPI_FAST_CLK
 *
 * @param in_sel One of the clock sources in soc_periph_mspi_clk_src_t
 */
static inline __attribute__((always_inline)) void clk_ll_mspi_fast_set_src(soc_periph_mspi_clk_src_t in_sel)
{
    switch (in_sel) {
    case MSPI_CLK_SRC_XTAL:
        PCR.mspi_clk_conf.mspi_func_clk_sel = 0;
        break;
    case MSPI_CLK_SRC_RC_FAST:
        PCR.mspi_clk_conf.mspi_func_clk_sel = 1;
        break;
    case MSPI_CLK_SRC_SPLL:
        PCR.mspi_clk_conf.mspi_func_clk_sel = 2;
        break;
    default:
        // Unsupported MSPI_FAST_CLK mux input sel
        abort();
    }
}

/**
 * @brief Set MSPI_FAST_CLK's divider
 *
 * @param divider Divider.
 */
static inline __attribute__((always_inline)) void clk_ll_mspi_fast_set_divider(uint32_t divider)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(PCR.mspi_clk_conf, mspi_fast_div_num, divider - 1);
}

/**
 * @brief Select the calibration 32kHz clock source for timergroup0
 *
 * @param in_sel One of the 32kHz clock sources (RC32K_CLK, XTAL32K_CLK, OSC_SLOW_CLK)
 */
static inline __attribute__((always_inline)) void clk_ll_32k_calibration_set_target(soc_rtc_slow_clk_src_t in_sel)
{
    switch (in_sel) {
    case SOC_RTC_SLOW_CLK_SRC_RC32K:
        PCR.ctrl_32k_conf.clk_32k_sel = 0;
        break;
    case SOC_RTC_SLOW_CLK_SRC_XTAL32K:
        PCR.ctrl_32k_conf.clk_32k_sel = 1;
        break;
    case SOC_RTC_SLOW_CLK_SRC_OSC_SLOW:
        PCR.ctrl_32k_conf.clk_32k_sel = 2;
        break;
    default:
        // Unsupported 32K_SEL mux input
        abort();
    }
}

/**
 * @brief Get the calibration 32kHz clock source for timergroup0
 *
 * @return soc_rtc_slow_clk_src_t Currently selected calibration 32kHz clock (one of the 32kHz clocks)
 */
static inline __attribute__((always_inline)) soc_rtc_slow_clk_src_t clk_ll_32k_calibration_get_target(void)
{
    uint32_t clk_sel = PCR.ctrl_32k_conf.clk_32k_sel;
    switch (clk_sel) {
    case 0:
        return SOC_RTC_SLOW_CLK_SRC_RC32K;
    case 1:
        return SOC_RTC_SLOW_CLK_SRC_XTAL32K;
    case 2:
        return SOC_RTC_SLOW_CLK_SRC_OSC_SLOW;
    default:
        return SOC_RTC_SLOW_CLK_SRC_INVALID;
    }
}

/**
 * @brief Select the clock source for RTC_SLOW_CLK
 *
 * @param in_sel One of the clock sources in soc_rtc_slow_clk_src_t
 */
static inline __attribute__((always_inline)) void clk_ll_rtc_slow_set_src(soc_rtc_slow_clk_src_t in_sel)
{
    switch (in_sel) {
    // case SOC_RTC_SLOW_CLK_SRC_RC_SLOW:
    //     LP_CLKRST.lp_clk_conf.slow_clk_sel = 0;
    //     break;
    case SOC_RTC_SLOW_CLK_SRC_XTAL32K:
        LP_CLKRST.lp_clk_conf.slow_clk_sel = 1;
        break;
    case SOC_RTC_SLOW_CLK_SRC_RC32K:
        LP_CLKRST.lp_clk_conf.slow_clk_sel = 2;
        break;
    case SOC_RTC_SLOW_CLK_SRC_OSC_SLOW:
        LP_CLKRST.lp_clk_conf.slow_clk_sel = 3;
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
    uint32_t clk_sel = LP_CLKRST.lp_clk_conf.slow_clk_sel;
    switch (clk_sel) {
    // case 0:
    //     return SOC_RTC_SLOW_CLK_SRC_RC_SLOW;
    case 1:
        return SOC_RTC_SLOW_CLK_SRC_XTAL32K;
    case 2:
        return SOC_RTC_SLOW_CLK_SRC_RC32K;
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
        LP_CLKRST.lp_clk_conf.fast_clk_sel = 0;
        break;
    case SOC_RTC_FAST_CLK_SRC_XTAL_D2:
        LP_CLKRST.lp_clk_conf.fast_clk_sel = 1;
        break;
    case SOC_RTC_FAST_CLK_SRC_XTAL:
        LP_CLKRST.lp_clk_conf.fast_clk_sel = 2;
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
    uint32_t clk_sel = LP_CLKRST.lp_clk_conf.fast_clk_sel;
    switch (clk_sel) {
    case 0:
        return SOC_RTC_FAST_CLK_SRC_RC_FAST;
    case 1:
        return SOC_RTC_FAST_CLK_SRC_XTAL_D2;
    case 2:
        return SOC_RTC_FAST_CLK_SRC_XTAL;
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
 * @brief Set RC_SLOW_CLK divider
 *
 * @param divider Divider of RC_SLOW_CLK. Usually this divider is set to 1 (reg. value is 0) in bootloader stage.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_slow_set_divider(uint32_t divider)
{
    // No divider on the target
    HAL_ASSERT(divider == 1);
}

/************************** LP STORAGE REGISTER STORE/LOAD **************************/
/**
 * @brief Store XTAL_CLK frequency in RTC storage register
 *
 * Value of RTC_XTAL_FREQ_REG is stored as two copies in lower and upper 16-bit
 * halves. These are the routines to work with that representation.
 *
 * @param xtal_freq_mhz XTAL frequency, in MHz. The frequency must necessarily be even,
 * otherwise there will be a conflict with the low bit, which is used to disable logs
 * in the ROM code.
 */
static inline __attribute__((always_inline)) void clk_ll_xtal_store_freq_mhz(uint32_t xtal_freq_mhz)
{
    // Read the status of whether disabling logging from ROM code
    uint32_t reg = READ_PERI_REG(RTC_XTAL_FREQ_REG) & RTC_DISABLE_ROM_LOG;
    // If so, need to write back this setting
    if (reg == RTC_DISABLE_ROM_LOG) {
        xtal_freq_mhz |= 1;
    }
    WRITE_PERI_REG(RTC_XTAL_FREQ_REG, (xtal_freq_mhz & UINT16_MAX) | ((xtal_freq_mhz & UINT16_MAX) << 16));
}

/**
 * @brief Load XTAL_CLK frequency from RTC storage register
 *
 * Value of RTC_XTAL_FREQ_REG is stored as two copies in lower and upper 16-bit
 * halves. These are the routines to work with that representation.
 *
 * @return XTAL frequency, in MHz. Returns 0 if value in reg is invalid.
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_xtal_load_freq_mhz(void)
{
    // Read from RTC storage register
    uint32_t xtal_freq_reg = READ_PERI_REG(RTC_XTAL_FREQ_REG);
    if ((xtal_freq_reg & 0xFFFF) == ((xtal_freq_reg >> 16) & 0xFFFF) &&
        xtal_freq_reg != 0 && xtal_freq_reg != UINT32_MAX) {
        return xtal_freq_reg & ~RTC_DISABLE_ROM_LOG & UINT16_MAX;
    }
    // If the format in reg is invalid
    return 0;
}

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


/*
Set the frequency division factor of ref_tick
*/
static inline void clk_ll_rc_fast_tick_conf(void)
{
    PCR.ctrl_tick_conf.fosc_tick_num = REG_FOSC_TICK_NUM;
}


#ifdef __cplusplus
}
#endif

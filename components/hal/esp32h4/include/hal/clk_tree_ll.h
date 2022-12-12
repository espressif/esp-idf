/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc.h"
#include "soc/clk_tree_defs.h"
#include "soc/rtc.h"
#include "soc/system_reg.h"
#include "soc/clkrst_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "hal/regi2c_ctrl.h"
#include "soc/regi2c_bbpll.h"
#include "hal/assert.h"
#include "hal/log.h"
#include "esp32h4/rom/rtc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MHZ                 (1000000)

#define CLK_LL_PLL_96M_FREQ_MHZ    (96)

#define CLK_LL_XTAL32K_CONFIG_DEFAULT() { \
    .dac = 3, \
    .dres = 3, \
    .dgm = 3, \
    .dbuf = 1, \
}

#define CLK_LL_RC32K_DFREQ_DEFAULT 707

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
    REG_CLR_BIT(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BB_I2C_FORCE_PD |
                RTC_CNTL_BBPLL_FORCE_PD | RTC_CNTL_BBPLL_I2C_FORCE_PD);
}

/**
 * @brief Power down BBPLL circuit
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_disable(void)
{
    REG_SET_BIT(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BB_I2C_FORCE_PD |
                RTC_CNTL_BBPLL_FORCE_PD | RTC_CNTL_BBPLL_I2C_FORCE_PD);
}

/**
 * @brief Enable the 32kHz crystal oscillator
 *
 * @param mode Used to determine the xtal32k configuration parameters
 */
static inline void clk_ll_xtal32k_enable(clk_ll_xtal32k_enable_mode_t mode)
{
    // Configure xtal32k (or only for mode == CLK_LL_XTAL32K_ENABLE_MODE_CRYSTAL?)
    clk_ll_xtal32k_config_t cfg = CLK_LL_XTAL32K_CONFIG_DEFAULT();
    REG_SET_FIELD(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_DAC_XTAL_32K, cfg.dac);
    REG_SET_FIELD(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_DRES_XTAL_32K, cfg.dres);
    REG_SET_FIELD(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_DGM_XTAL_32K, cfg.dgm);
    REG_SET_FIELD(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_DBUF_XTAL_32K, cfg.dbuf);
    // Enable xtal32k xpd status
    SET_PERI_REG_MASK(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_XPD_XTAL_32K);
    if (mode == CLK_LL_XTAL32K_ENABLE_MODE_EXTERNAL) {
        // Not supported yet?
        ;
    }
}

/**
 * @brief Disable the 32kHz crystal oscillator
 */
static inline void clk_ll_xtal32k_disable(void)
{
    // Set xtal32k xpd to be controlled by software
    SET_PERI_REG_MASK(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_XTAL32K_XPD_FORCE);
    // Disable xtal32k xpd status
    CLEAR_PERI_REG_MASK(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_XPD_XTAL_32K);
}

/**
 * @brief Get the state of the 32kHz crystal clock
 *
 * @return True if the 32kHz XTAL is enabled
 */
static inline bool clk_ll_xtal32k_is_enabled(void)
{
    uint32_t xtal_conf = READ_PERI_REG(RTC_CNTL_EXT_XTL_CONF_REG);
    /* If xtal xpd is controlled by software */
    bool xtal_xpd_sw = (xtal_conf & RTC_CNTL_XTAL32K_XPD_FORCE) >> RTC_CNTL_XTAL32K_XPD_FORCE_S;
    /* If xtal xpd software control is on */
    bool xtal_xpd_st = (xtal_conf & RTC_CNTL_XPD_XTAL_32K) >> RTC_CNTL_XPD_XTAL_32K_S;
    // disabled = xtal_xpd_sw && !xtal_xpd_st; enabled = !disbaled
    bool enabled = !xtal_xpd_sw || xtal_xpd_st;
    return enabled;
}

/**
 * @brief Enable the internal oscillator output for RC32K_CLK
 */
static inline void clk_ll_rc32k_enable(void)
{
    // Configure rc32k
    REG_SET_FIELD(RTC_CNTL_RC32K_CTRL_REG, RTC_CNTL_RC32K_DFREQ, CLK_LL_RC32K_DFREQ_DEFAULT);
    // Enable rc32k xpd status
    SET_PERI_REG_MASK(RTC_CNTL_RC32K_CTRL_REG, RTC_CNTL_RC32K_XPD);
}

/**
 * @brief Disable the internal oscillator output for RC32k_CLK
 */
static inline void clk_ll_rc32k_disable(void)
{
    // Configure rc32k
    REG_SET_FIELD(RTC_CNTL_RC32K_CTRL_REG, RTC_CNTL_RC32K_DFREQ, CLK_LL_RC32K_DFREQ_DEFAULT);
    // Disable rc32k xpd status
    CLEAR_PERI_REG_MASK(RTC_CNTL_RC32K_CTRL_REG, RTC_CNTL_RC32K_XPD);
}

/**
 * @brief Enable the digital RC_FAST_CLK, which is used to support peripherals.
 */
static inline void clk_ll_rc_fast_digi_enable(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_CLK8M_EN_M);
}

/**
 * @brief Disable the digital RC_FAST_CLK, which is used to support peripherals.
 */
static inline void clk_ll_rc_fast_digi_disable(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_CLK8M_EN_M);
}

/**
 * @brief Get the state of the digital RC_FAST_CLK
 *
 * @return True if the digital RC_FAST_CLK is enabled
 */
static inline bool clk_ll_rc_fast_digi_is_enabled(void)
{
    return GET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_CLK8M_EN_M);
}

/**
 * @brief Enable the digital RC32K_CLK, which is used to support peripherals.
 */
static inline void clk_ll_rc32k_digi_enable(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_RC32K_EN_M);
}

/**
 * @brief Disable the digital RC32K_CLK, which is used to support peripherals.
 */
static inline void clk_ll_rc32k_digi_disable(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_RC32K_EN_M);
}

/**
 * @brief Get the state of the digital RC32K_CLK
 *
 * @return True if the digital RC32K_CLK is enabled
 */
static inline bool clk_ll_rc32k_digi_is_enabled(void)
{
    return REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_RC32K_EN);
}

/**
 * @brief Enable the digital XTAL32K_CLK, which is used to support peripherals.
 */
static inline void clk_ll_xtal32k_digi_enable(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_XTAL32K_EN_M);
}

/**
 * @brief Disable the digital XTAL32K_CLK, which is used to support peripherals.
 */
static inline void clk_ll_xtal32k_digi_disable(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_XTAL32K_EN_M);
}

/**
 * @brief Get the state of the digital XTAL32K_CLK
 *
 * @return True if the digital XTAL32K_CLK is enabled
 */
static inline bool clk_ll_xtal32k_digi_is_enabled(void)
{
    return REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_XTAL32K_EN);
}

/**
 * @brief Get PLL_CLK frequency
 *
 * @return PLL clock frequency, in MHz
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_bbpll_get_freq_mhz(void)
{
    uint32_t bbpll_freq = REG_GET_FIELD(SYSTEM_SYSCLK_CONF_REG, SYSTEM_SPLL_FREQ);
    HAL_ASSERT(bbpll_freq == CLK_LL_PLL_96M_FREQ_MHZ);
    return bbpll_freq;
}

/**
 * @brief Set BBPLL frequency from XTAL source (Digital part)
 *
 * @param pll_freq_mhz PLL frequency, in MHz
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_set_freq_mhz(uint32_t pll_freq_mhz)
{
    (void)pll_freq_mhz;
    // ESP32H4 bbpll frequency cannot be changed, fixed to 96MHz
    // Do nothing
}

/**
 * @brief Set BBPLL frequency from XTAL source (Analog part)
 *
 * @param pll_freq_mhz PLL frequency, in MHz
 * @param xtal_freq_mhz XTAL frequency, in MHz
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_set_config(uint32_t pll_freq_mhz, uint32_t xtal_freq_mhz)
{
    (void)xtal_freq_mhz;
    switch (pll_freq_mhz) {
    case CLK_LL_PLL_96M_FREQ_MHZ: // PLL_96M
        /* set up PLL by analog control registers */
        REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_OC_REF_DIV, 0);
        REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_OC_DIV, 1); // I2C_BBPLL_OC_DIV_5_0
        break;
    default:
        break;
    }
    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_OC_DHREF_SEL, 3);
    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_OC_DLREF_SEL, 1);
}

/**
 * @brief Select the clock source for CPU_CLK
 *
 * @param in_sel One of the clock sources in soc_cpu_clk_src_t
 */
static inline __attribute__((always_inline)) void clk_ll_cpu_set_src(soc_cpu_clk_src_t in_sel)
{
    switch (in_sel) {
    case SOC_CPU_CLK_SRC_XTAL:
        REG_SET_FIELD(SYSTEM_SYSCLK_CONF_REG, SYSTEM_SOC_CLK_SEL, 0);
        break;
    case SOC_CPU_CLK_SRC_PLL:
        REG_SET_FIELD(SYSTEM_SYSCLK_CONF_REG, SYSTEM_SOC_CLK_SEL, 1);
        break;
    case SOC_CPU_CLK_SRC_RC_FAST:
        REG_SET_FIELD(SYSTEM_SYSCLK_CONF_REG, SYSTEM_SOC_CLK_SEL, 2);
        break;
    case SOC_CPU_CLK_SRC_XTAL_D2:
        REG_SET_FIELD(SYSTEM_SYSCLK_CONF_REG, SYSTEM_SOC_CLK_SEL, 3);
        break;
    default:
        // Unsupported CPU_CLK mux input sel
        abort();
    }
}

/**
 * @brief Get the clock source for CPU_CLK
 *
 * @return Currently selected clock source (one of soc_cpu_clk_src_t values)
 */
static inline __attribute__((always_inline)) soc_cpu_clk_src_t clk_ll_cpu_get_src(void)
{
    uint32_t clk_sel = REG_GET_FIELD(SYSTEM_SYSCLK_CONF_REG, SYSTEM_SOC_CLK_SEL);
    switch (clk_sel) {
    case 0:
        return SOC_CPU_CLK_SRC_XTAL;
    case 1:
        return SOC_CPU_CLK_SRC_PLL;
    case 2:
        return SOC_CPU_CLK_SRC_RC_FAST;
    case 3:
        return SOC_CPU_CLK_SRC_XTAL_D2;
    default:
        return SOC_CPU_CLK_SRC_INVALID;
    }
}

/**
 * @brief Set CPU_CLK divider. freq of CPU_CLK = freq of CPU clock source / divider
 *
 * @param divider Divider. CPU_DIV_NUM = divider - 1.
 */
static inline __attribute__((always_inline)) void clk_ll_cpu_set_divider(uint32_t divider)
{
    HAL_ASSERT(divider > 0);
    REG_SET_FIELD(SYSTEM_CPUCLK_CONF_REG, SYSTEM_CPU_DIV_NUM, divider - 1);
}

/**
 * @brief Get CPU_CLK divider
 *
 * @return Divider. Divider = (CPU_DIV_NUM + 1).
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_cpu_get_divider(void)
{
    return REG_GET_FIELD(SYSTEM_CPUCLK_CONF_REG, SYSTEM_CPU_DIV_NUM) + 1;
}

/**
 * @brief Set AHB_CLK divider. freq of AHB_CLK = freq of CPU_CLK / divider
 *
 * @param divider Divider. AHB_DIV_NUM = divider - 1.
 */
static inline void clk_ll_ahb_set_divider(uint32_t divider)
{
    HAL_ASSERT(divider > 0);
    REG_SET_FIELD(SYSTEM_BUSCLK_CONF_REG, SYSTEM_AHB_DIV_NUM, divider - 1);
}

/**
 * @brief Get AHB_CLK divider
 *
 * @return Divider. Divider = (AHB_DIV_NUM + 1).
 */
static inline uint32_t clk_ll_ahb_get_divider(void)
{
    return REG_GET_FIELD(SYSTEM_BUSCLK_CONF_REG, SYSTEM_AHB_DIV_NUM) + 1;
}

/**
 * @brief Set APB_CLK divider. freq of APB_CLK = freq of AHB_CLK / divider
 *
 * @param divider Divider. APB_DIV_NUM = divider - 1.
 */
static inline void clk_ll_apb_set_divider(uint32_t divider)
{
    HAL_ASSERT(divider > 0);
    REG_SET_FIELD(SYSTEM_BUSCLK_CONF_REG, SYSTEM_APB_DIV_NUM, divider - 1);
}

/**
 * @brief Get APB_CLK divider
 *
 * @return Divider. Divider = (APB_DIV_NUM + 1).
 */
static inline uint32_t clk_ll_apb_get_divider(void)
{
    return REG_GET_FIELD(SYSTEM_BUSCLK_CONF_REG, SYSTEM_APB_DIV_NUM) + 1;
}

/**
 * @brief Select the clock source for RTC_SLOW_CLK
 *
 * @param in_sel One of the clock sources in soc_rtc_slow_clk_src_t
 */
static inline void clk_ll_rtc_slow_set_src(soc_rtc_slow_clk_src_t in_sel)
{
    switch (in_sel) {
    case SOC_RTC_SLOW_CLK_SRC_RC_SLOW:
        REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ANA_CLK_RTC_SEL, 0);
        break;
    case SOC_RTC_SLOW_CLK_SRC_XTAL32K:
        REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ANA_CLK_RTC_SEL, 1);
        break;
    case SOC_RTC_SLOW_CLK_SRC_RC32K:
        REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ANA_CLK_RTC_SEL, 2);
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
static inline soc_rtc_slow_clk_src_t clk_ll_rtc_slow_get_src(void)
{
    uint32_t clk_sel = REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ANA_CLK_RTC_SEL);
    switch (clk_sel) {
    case 0:
        return SOC_RTC_SLOW_CLK_SRC_RC_SLOW;
    case 1:
        return SOC_RTC_SLOW_CLK_SRC_XTAL32K;
    case 2:
        return SOC_RTC_SLOW_CLK_SRC_RC32K;
    default:
        // Invalid ANA_CLK_RTC_SEL value
        return SOC_RTC_SLOW_CLK_SRC_INVALID;
    }
}

/**
 * @brief Select the clock source for RTC_FAST_CLK
 *
 * @param in_sel One of the clock sources in soc_rtc_fast_clk_src_t
 */
static inline void clk_ll_rtc_fast_set_src(soc_rtc_fast_clk_src_t in_sel)
{
    switch (in_sel) {
    case SOC_RTC_FAST_CLK_SRC_XTAL_D2:
        REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_FAST_CLK_RTC_SEL, 0);
        break;
    case SOC_RTC_FAST_CLK_SRC_RC_FAST:
        REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_FAST_CLK_RTC_SEL, 1);
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
static inline soc_rtc_fast_clk_src_t clk_ll_rtc_fast_get_src(void)
{
    uint32_t clk_sel = REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_FAST_CLK_RTC_SEL);
    switch (clk_sel) {
    case 0:
        return SOC_RTC_FAST_CLK_SRC_XTAL_D2;
    case 1:
        return SOC_RTC_FAST_CLK_SRC_RC_FAST;
    default:
        return SOC_RTC_FAST_CLK_SRC_INVALID;
    }
}

/**
 * @brief Set RC_FAST_CLK divider. The output from the divider is passed into rtc_fast_clk MUX.
 *
 * @param divider Divider of RC_FAST_CLK. Usually this divider is set to 1 (reg. value is 0) in bootloader stage.
 */
static inline void clk_ll_rc_fast_set_divider(uint32_t divider)
{
    HAL_ASSERT(divider > 0);
    CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL_VLD);
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL, divider - 1);
    SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL_VLD);
}

/**
 * @brief Get RC_FAST_CLK divider
 *
 * @return Divider. Divider = (CK8M_DIV_SEL + 1).
 */
static inline uint32_t clk_ll_rc_fast_get_divider(void)
{
    return REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL) + 1;
}

/**
 * @brief Set RC_SLOW_CLK divider
 *
 * @param divider Divider of RC_SLOW_CLK. Usually this divider is set to 1 (reg. value is 0) in bootloader stage.
 */
static inline void clk_ll_rc_slow_set_divider(uint32_t divider)
{
    HAL_ASSERT(divider > 0);
    CLEAR_PERI_REG_MASK(RTC_CNTL_SLOW_CLK_CONF_REG, RTC_CNTL_ANA_CLK_DIV_VLD);
    REG_SET_FIELD(RTC_CNTL_SLOW_CLK_CONF_REG, RTC_CNTL_ANA_CLK_DIV, divider - 1);
    SET_PERI_REG_MASK(RTC_CNTL_SLOW_CLK_CONF_REG, RTC_CNTL_ANA_CLK_DIV_VLD);
}

/************************* RTC STORAGE REGISTER STORE/LOAD **************************/
/**
 * @brief Store XTAL_CLK frequency in RTC storage register
 *
 * Value of RTC_XTAL_FREQ_REG is stored as two copies in lower and upper 16-bit
 * halves. These are the routines to work with that representation.
 *
 * @param xtal_freq_mhz XTAL frequency, in MHz
 */
static inline void clk_ll_xtal_store_freq_mhz(uint32_t xtal_freq_mhz)
{
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
    // ESP32H4 has a fixed crystal frequency (32MHz), but we will still read from the RTC storage register
    uint32_t xtal_freq_reg = READ_PERI_REG(RTC_XTAL_FREQ_REG);
    if ((xtal_freq_reg & UINT16_MAX) != RTC_XTAL_FREQ_32M) {
        return 0;
    }
    return (uint32_t)RTC_XTAL_FREQ_32M;
}

/**
 * @brief Store APB_CLK frequency in RTC storage register
 *
 * Value of RTC_APB_FREQ_REG is stored as two copies in lower and upper 16-bit
 * halves. These are the routines to work with that representation.
 *
 * @param apb_freq_hz APB frequency, in Hz
 */
static inline __attribute__((always_inline)) void clk_ll_apb_store_freq_hz(uint32_t apb_freq_hz)
{
    uint32_t val = apb_freq_hz >> 12;
    WRITE_PERI_REG(RTC_APB_FREQ_REG, (val & UINT16_MAX) | ((val & UINT16_MAX) << 16));
}

/**
 * @brief Store RTC_SLOW_CLK calibration value in RTC storage register
 *
 * Value of RTC_SLOW_CLK_CAL_REG has to be in the same format as returned by rtc_clk_cal (microseconds,
 * in Q13.19 fixed-point format).
 *
 * @param cal_value The calibration value of slow clock period in microseconds, in Q13.19 fixed point format
 */
static inline void clk_ll_rtc_slow_store_cal(uint32_t cal_value)
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
static inline uint32_t clk_ll_rtc_slow_load_cal(void)
{
    return REG_READ(RTC_SLOW_CLK_CAL_REG);
}

#ifdef __cplusplus
}
#endif

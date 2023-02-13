/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc.h"
#include "soc/clk_tree_defs.h"
#include "soc/rtc.h"
#include "soc/system_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "hal/regi2c_ctrl.h"
#include "soc/regi2c_bbpll.h"
#include "hal/assert.h"
#include "hal/log.h"
#include "esp32c2/rom/rtc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MHZ                 (1000000)

#define CLK_LL_PLL_40M_FREQ_MHZ    (40)
#define CLK_LL_PLL_60M_FREQ_MHZ    (60)
#define CLK_LL_PLL_80M_FREQ_MHZ    (80)
#define CLK_LL_PLL_120M_FREQ_MHZ   (120)

#define CLK_LL_PLL_480M_FREQ_MHZ   (480)

#define CLK_LL_AHB_MAX_FREQ_MHZ    CLK_LL_PLL_40M_FREQ_MHZ

/**
 * @brief XTAL32K_CLK enable modes
 */
typedef enum {
    CLK_LL_XTAL32K_ENABLE_MODE_EXTERNAL,      //!< Enable the external clock signal for XTAL32K_CLK (i.e. EXT_OSC_CLK)
} clk_ll_xtal32k_enable_mode_t;


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
 * @brief Enable the internal oscillator output for RC_FAST_CLK
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_enable(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M);
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_CK8M_WAIT, RTC_CK8M_ENABLE_WAIT_DEFAULT);
}

/**
 * @brief Disable the internal oscillator output for RC_FAST_CLK
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_disable(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M);
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_CK8M_WAIT, RTC_CNTL_CK8M_WAIT_DEFAULT);
}

/**
 * @brief Get the state of the internal oscillator for RC_FAST_CLK
 *
 * @return True if the oscillator is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_rc_fast_is_enabled(void)
{
    return GET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M) == 0;
}

/**
 * @brief Enable the output from the internal oscillator to be passed into a configurable divider,
 * which by default divides the input clock frequency by 256. i.e. RC_FAST_D256_CLK = RC_FAST_CLK / 256
 *
 * Divider values other than 256 may be configured, but this facility is not currently needed,
 * so is not exposed in the code.
 * The output of the divider, RC_FAST_D256_CLK, is referred as 8md256 or simply d256 in reg. descriptions.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_d256_enable(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M_DIV);
}

/**
 * @brief Disable the output from the internal oscillator to be passed into a configurable divider.
 * i.e. RC_FAST_D256_CLK = RC_FAST_CLK / 256
 *
 * Disabling this divider could reduce power consumption.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_d256_disable(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M_DIV);
}

/**
 * @brief Get the state of the divider which is applied to the output from the internal oscillator (RC_FAST_CLK)
 *
 * @return True if the divided output is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_rc_fast_d256_is_enabled(void)
{
    return GET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M_DIV) == 0;
}

/**
 * @brief Enable the digital RC_FAST_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_digi_enable(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_CLK8M_EN_M);
}

/**
 * @brief Disable the digital RC_FAST_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_digi_disable(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_CLK8M_EN_M);
}

/**
 * @brief Get the state of the digital RC_FAST_CLK
 *
 * @return True if the digital RC_FAST_CLK is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_rc_fast_digi_is_enabled(void)
{
    return GET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_CLK8M_EN_M);
}

/**
 * @brief Enable the digital RC_FAST_D256_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_d256_digi_enable(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_CLK8M_D256_EN_M);
}

/**
 * @brief Disable the digital RC_FAST_D256_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_d256_digi_disable(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_CLK8M_D256_EN_M);
}

/**
 * @brief Enable the digital XTAL32K_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_xtal32k_digi_enable(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_XTAL32K_EN_M);
}

/**
 * @brief Disable the digital XTAL32K_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_xtal32k_digi_disable(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_XTAL32K_EN_M);
}

/**
 * @brief Get the state of the digital XTAL32K_CLK
 *
 * @return True if the digital XTAL32K_CLK is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_xtal32k_digi_is_enabled(void)
{
    return REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_XTAL32K_EN);
}

/**
 * @brief Get PLL_CLK frequency
 *
 * @return PLL clock frequency, in MHz. Returns 0 if register field value is invalid.
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_bbpll_get_freq_mhz(void)
{
    // ESP32C2 only support 480MHz PLL
    uint32_t pll_freq_sel = REG_GET_FIELD(SYSTEM_CPU_PER_CONF_REG, SYSTEM_PLL_FREQ_SEL);
    switch (pll_freq_sel) {
    case 1: // PLL_480M
        return CLK_LL_PLL_480M_FREQ_MHZ;
    default:
        // Invalid PLL_FREQ_SEL value
        return 0;
    }
}

/**
 * @brief Set BBPLL frequency from XTAL source (Digital part)
 *
 * @param pll_freq_mhz PLL frequency, in MHz
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_set_freq_mhz(uint32_t pll_freq_mhz)
{
    // ESP32C2 only support 480MHz PLL
    HAL_ASSERT(pll_freq_mhz == CLK_LL_PLL_480M_FREQ_MHZ);
    REG_SET_FIELD(SYSTEM_CPU_PER_CONF_REG, SYSTEM_PLL_FREQ_SEL, 1);
}

/**
 * @brief Set BBPLL frequency from XTAL source (Analog part)
 *
 * @param pll_freq_mhz PLL frequency, in MHz
 * @param xtal_freq_mhz XTAL frequency, in MHz
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_set_config(uint32_t pll_freq_mhz, uint32_t xtal_freq_mhz)
{
    (void)pll_freq_mhz;
    uint8_t div_ref;
    uint8_t div7_0;
    uint8_t dr1;
    uint8_t dr3;
    uint8_t dchgp;
    uint8_t dcur;
    uint8_t dbias;

    /* Configure 480M PLL */
    switch (xtal_freq_mhz) {
    case RTC_XTAL_FREQ_26M:
        div_ref = 12;
        div7_0 = 236;
        dr1 = 4;
        dr3 = 4;
        dchgp = 0;
        dcur = 0;
        dbias = 2;
        break;
    case RTC_XTAL_FREQ_32M:
        div_ref = 0;
        div7_0 = 11;
        dr1 = 0;
        dr3 = 0;
        dchgp = 5;
        dcur = 3;
        dbias = 2;
        break;
    case RTC_XTAL_FREQ_40M:
    default:
        div_ref = 0;
        div7_0 = 8;
        dr1 = 0;
        dr3 = 0;
        dchgp = 5;
        dcur = 3;
        dbias = 2;
        break;
    }
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_MODE_HF, 0x6B);
    uint8_t i2c_bbpll_lref  = (dchgp << I2C_BBPLL_OC_DCHGP_LSB) | (div_ref);
    uint8_t i2c_bbpll_div_7_0 = div7_0;
    uint8_t i2c_bbpll_dcur = (1 << I2C_BBPLL_OC_DLREF_SEL_LSB ) | (3 << I2C_BBPLL_OC_DHREF_SEL_LSB) | dcur;
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_OC_REF_DIV, i2c_bbpll_lref);
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_OC_DIV_7_0, i2c_bbpll_div_7_0);
    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_OC_DR1, dr1);
    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_OC_DR3, dr3);
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_OC_DCUR, i2c_bbpll_dcur);
    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_OC_VCO_DBIAS, dbias);
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
    default:
        // Invalid SOC_CLK_SEL value
        return SOC_CPU_CLK_SRC_INVALID;
    }
}

/**
 * @brief Set CPU frequency from PLL clock
 *
 * @param cpu_mhz CPU frequency value, in MHz
 */
static inline __attribute__((always_inline)) void clk_ll_cpu_set_freq_mhz_from_pll(uint32_t cpu_mhz)
{
    switch (cpu_mhz) {
    case CLK_LL_PLL_80M_FREQ_MHZ:
        REG_SET_FIELD(SYSTEM_CPU_PER_CONF_REG, SYSTEM_CPUPERIOD_SEL, 0);
        break;
    case CLK_LL_PLL_120M_FREQ_MHZ:
        REG_SET_FIELD(SYSTEM_CPU_PER_CONF_REG, SYSTEM_CPUPERIOD_SEL, 1);
        break;
    default:
        // Unsupported CPU_CLK freq from PLL
        abort();
    }
}

/**
 * @brief Get CPU_CLK frequency from PLL_CLK source
 *
 * @return CPU clock frequency, in MHz. Returns 0 if register field value is invalid.
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_cpu_get_freq_mhz_from_pll(void)
{
    uint32_t cpu_freq_sel = REG_GET_FIELD(SYSTEM_CPU_PER_CONF_REG, SYSTEM_CPUPERIOD_SEL);
    switch (cpu_freq_sel) {
    case 0:
        return CLK_LL_PLL_80M_FREQ_MHZ;
    case 1:
        return CLK_LL_PLL_120M_FREQ_MHZ;
    default:
        // Invalid CPUPERIOD_SEL value
        return 0;
    }
}

/**
 * @brief Set CPU_CLK's XTAL/FAST_RC clock source path divider
 *
 * @param divider Divider. Usually this divider is set to 1 in bootloader stage. PRE_DIV_CNT = divider - 1.
 */
static inline __attribute__((always_inline)) void clk_ll_cpu_set_divider(uint32_t divider)
{
    HAL_ASSERT(divider > 0);
    REG_SET_FIELD(SYSTEM_SYSCLK_CONF_REG, SYSTEM_PRE_DIV_CNT, divider - 1);
}

/**
 * @brief Get CPU_CLK's XTAL/FAST_RC clock source path divider
 *
 * @return Divider. Divider = (PRE_DIV_CNT + 1).
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_cpu_get_divider(void)
{
    return REG_GET_FIELD(SYSTEM_SYSCLK_CONF_REG, SYSTEM_PRE_DIV_CNT) + 1;
}

/**
 * @brief Select the clock source for RTC_SLOW_CLK
 *
 * @param in_sel One of the clock sources in soc_rtc_slow_clk_src_t
 */
static inline __attribute__((always_inline)) void clk_ll_rtc_slow_set_src(soc_rtc_slow_clk_src_t in_sel)
{
    switch (in_sel) {
    case SOC_RTC_SLOW_CLK_SRC_RC_SLOW:
        REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ANA_CLK_RTC_SEL, 0);
        break;
    case SOC_RTC_SLOW_CLK_SRC_OSC_SLOW:
        REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ANA_CLK_RTC_SEL, 1);
        break;
    case SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256:
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
static inline __attribute__((always_inline)) soc_rtc_slow_clk_src_t clk_ll_rtc_slow_get_src(void)
{
    uint32_t clk_sel = REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ANA_CLK_RTC_SEL);
    switch (clk_sel) {
    case 0:
        return SOC_RTC_SLOW_CLK_SRC_RC_SLOW;
    case 1:
        return SOC_RTC_SLOW_CLK_SRC_OSC_SLOW;
    case 2:
        return SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256;
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
static inline __attribute__((always_inline)) void clk_ll_rtc_fast_set_src(soc_rtc_fast_clk_src_t in_sel)
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
static inline __attribute__((always_inline)) soc_rtc_fast_clk_src_t clk_ll_rtc_fast_get_src(void)
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
static inline __attribute__((always_inline)) void clk_ll_rc_fast_set_divider(uint32_t divider)
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
static inline __attribute__((always_inline)) uint32_t clk_ll_rc_fast_get_divider(void)
{
    return REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL) + 1;
}

/**
 * @brief Set RC_SLOW_CLK divider
 *
 * @param divider Divider of RC_SLOW_CLK. Usually this divider is set to 1 (reg. value is 0) in bootloader stage.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_slow_set_divider(uint32_t divider)
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
 * @brief Load APB_CLK frequency from RTC storage register
 *
 * Value of RTC_APB_FREQ_REG is stored as two copies in lower and upper 16-bit
 * halves. These are the routines to work with that representation.
 *
 * @return The stored APB frequency, in Hz
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_apb_load_freq_hz(void)
{
    // Read from RTC storage register
    uint32_t apb_freq_hz = (READ_PERI_REG(RTC_APB_FREQ_REG) & UINT16_MAX) << 12;
    // Round to the nearest MHz
    apb_freq_hz += MHZ / 2;
    uint32_t remainder = apb_freq_hz % MHZ;
    return apb_freq_hz - remainder;
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

/**
 * @brief Store rtc_fix_us in RTC storage register
 *
 * @param rtc_fix_us The value used to correct the time obtained from the rtc timer when the calibration value changes
 */
static inline __attribute__((always_inline)) void clk_ll_rtc_slow_store_rtc_fix_us(uint64_t rtc_fix_us)
{
    REG_WRITE(RTC_FIX_US_LOW_REG, rtc_fix_us);
    REG_WRITE(RTC_FIX_US_HIGH_REG, rtc_fix_us >> 32);
}

/**
 * @brief Load the rtc_fix_ticks from RTC storage register
 *
 * @return The value used to correct the time obtained from the rtc timer when the calibration value changes
 */
static inline __attribute__((always_inline)) uint64_t clk_ll_rtc_slow_load_rtc_fix_us(void)
{
    return REG_READ(RTC_FIX_US_LOW_REG) | ((uint64_t)REG_READ(RTC_FIX_US_HIGH_REG) << 32);
}

#ifdef __cplusplus
}
#endif

/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc.h"
#include "soc/clk_tree_defs.h"
#include "soc/rtc.h"
#include "soc/pcr_reg.h"
#include "hal/regi2c_ctrl.h"
#include "soc/regi2c_bbpll.h"
#include "hal/assert.h"
#include "hal/log.h"
#include "esp32c6/rom/rtc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MHZ                 (1000000)

#define CLK_LL_PLL_80M_FREQ_MHZ    (80)
#define CLK_LL_PLL_160M_FREQ_MHZ   (160)

#define CLK_LL_PLL_320M_FREQ_MHZ   (320)
#define CLK_LL_PLL_480M_FREQ_MHZ   (480)

#define CLK_LL_XTAL32K_CONFIG_DEFAULT() { \
    .dac = 3, \
    .dres = 3, \
    .dgm = 3, \
    .dbuf = 1, \
}

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
    // TODO: IDF-5645
}

/**
 * @brief Power down BBPLL circuit
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_disable(void)
{
    // TODO: IDF-5645
}

/**
 * @brief Enable the 32kHz crystal oscillator
 *
 * @param mode Used to determine the xtal32k configuration parameters
 */
static inline void clk_ll_xtal32k_enable(clk_ll_xtal32k_enable_mode_t mode)
{
    // TODO: IDF-5645
}

/**
 * @brief Disable the 32kHz crystal oscillator
 */
static inline void clk_ll_xtal32k_disable(void)
{
    // TODO: IDF-5645
}

/**
 * @brief Get the state of the 32kHz crystal clock
 *
 * @return True if the 32kHz XTAL is enabled
 */
static inline bool clk_ll_xtal32k_is_enabled(void)
{
    // TODO: IDF-5645
    return 0;
}

/**
 * @brief Enable the internal oscillator output for RC_FAST_CLK
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_enable(void)
{
    // TODO: IDF-5645
}

/**
 * @brief Disable the internal oscillator output for RC_FAST_CLK
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_disable(void)
{
    // TODO: IDF-5645
}

/**
 * @brief Get the state of the internal oscillator for RC_FAST_CLK
 *
 * @return True if the oscillator is enabled
 */
static inline bool clk_ll_rc_fast_is_enabled(void)
{
    // TODO: IDF-5645
    return 1;
}

/**
 * @brief Enable the output from the internal oscillator to be passed into a configurable divider,
 * which by default divides the input clock frequency by 256. i.e. RC_FAST_D256_CLK = RC_FAST_CLK / 256
 *
 * Divider values other than 256 may be configured, but this facility is not currently needed,
 * so is not exposed in the code.
 * The output of the divider, RC_FAST_D256_CLK, is referred as 8md256 or simply d256 in reg. descriptions.
 */
static inline void clk_ll_rc_fast_d256_enable(void)
{
    // TODO: IDF-5645
}

/**
 * @brief Disable the output from the internal oscillator to be passed into a configurable divider.
 * i.e. RC_FAST_D256_CLK = RC_FAST_CLK / 256
 *
 * Disabling this divider could reduce power consumption.
 */
static inline void clk_ll_rc_fast_d256_disable(void)
{
    // TODO: IDF-5645
}

/**
 * @brief Get the state of the divider which is applied to the output from the internal oscillator (RC_FAST_CLK)
 *
 * @return True if the divided output is enabled
 */
static inline bool clk_ll_rc_fast_d256_is_enabled(void)
{
    // TODO: IDF-5645
    return 1;
}

/**
 * @brief Enable the digital RC_FAST_CLK, which is used to support peripherals.
 */
static inline void clk_ll_rc_fast_digi_enable(void)
{
    // TODO: IDF-5645
}

/**
 * @brief Disable the digital RC_FAST_CLK, which is used to support peripherals.
 */
static inline void clk_ll_rc_fast_digi_disable(void)
{
    // TODO: IDF-5645
}

/**
 * @brief Get the state of the digital RC_FAST_CLK
 *
 * @return True if the digital RC_FAST_CLK is enabled
 */
static inline bool clk_ll_rc_fast_digi_is_enabled(void)
{
    // TODO: IDF-5645
    return 0;
}

/**
 * @brief Enable the digital RC_FAST_D256_CLK, which is used to support peripherals.
 */
static inline void clk_ll_rc_fast_d256_digi_enable(void)
{
    // TODO: IDF-5645
}

/**
 * @brief Disable the digital RC_FAST_D256_CLK, which is used to support peripherals.
 */
static inline void clk_ll_rc_fast_d256_digi_disable(void)
{
    // TODO: IDF-5645
}

/**
 * @brief Enable the digital XTAL32K_CLK, which is used to support peripherals.
 */
static inline void clk_ll_xtal32k_digi_enable(void)
{
    // TODO: IDF-5645
}

/**
 * @brief Disable the digital XTAL32K_CLK, which is used to support peripherals.
 */
static inline void clk_ll_xtal32k_digi_disable(void)
{
    // TODO: IDF-5645
}

/**
 * @brief Get the state of the digital XTAL32K_CLK
 *
 * @return True if the digital XTAL32K_CLK is enabled
 */
static inline bool clk_ll_xtal32k_digi_is_enabled(void)
{
    // TODO: IDF-5645
    return 0;
}

/**
 * @brief Get PLL_CLK frequency
 *
 * @return PLL clock frequency, in MHz. Returns 0 if register field value is invalid.
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_bbpll_get_freq_mhz(void)
{
    // TODO: IDF-5645
    return 0;
}

/**
 * @brief Set BBPLL frequency from XTAL source (Digital part)
 *
 * @param pll_freq_mhz PLL frequency, in MHz
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_set_freq_mhz(uint32_t pll_freq_mhz)
{
    // TODO: IDF-5645
}

/**
 * @brief Set BBPLL frequency from XTAL source (Analog part)
 *
 * @param pll_freq_mhz PLL frequency, in MHz
 * @param xtal_freq_mhz XTAL frequency, in MHz
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_set_config(uint32_t pll_freq_mhz, uint32_t xtal_freq_mhz)
{
    // TODO: IDF-5645
}

/**
 * @brief Select the clock source for CPU_CLK
 *
 * @param in_sel One of the clock sources in soc_cpu_clk_src_t
 */
static inline __attribute__((always_inline)) void clk_ll_cpu_set_src(soc_cpu_clk_src_t in_sel)
{
    // TODO: IDF-5645
    switch (in_sel) {
    case SOC_CPU_CLK_SRC_XTAL:
        REG_SET_FIELD(PCR_SYSCLK_CONF_REG, PCR_SOC_CLK_SEL, 0);
        break;
    case SOC_CPU_CLK_SRC_PLL:
        REG_SET_FIELD(PCR_SYSCLK_CONF_REG, PCR_SOC_CLK_SEL, 1);
        break;
    case SOC_CPU_CLK_SRC_RC_FAST:
        REG_SET_FIELD(PCR_SYSCLK_CONF_REG, PCR_SOC_CLK_SEL, 2);
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
    // TODO: IDF-5645
    uint32_t clk_sel = REG_GET_FIELD(PCR_SYSCLK_CONF_REG, PCR_SOC_CLK_SEL);
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
    // TODO: IDF-5645
}

/**
 * @brief Get CPU_CLK frequency from PLL_CLK source
 *
 * @return CPU clock frequency, in MHz. Returns 0 if register field value is invalid.
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_cpu_get_freq_mhz_from_pll(void)
{
    // TODO: IDF-5645
    return 0;
}

/**
 * @brief Set CPU_CLK's XTAL/FAST_RC clock source path divider
 *
 * @param divider Divider. Usually this divider is set to 1 in bootloader stage. PRE_DIV_CNT = divider - 1.
 */
static inline __attribute__((always_inline)) void clk_ll_cpu_set_divider(uint32_t divider)
{
    // TODO: IDF-5645  not configurable for 761, fixed at 3 for HS, 1 for LS
}

/**
 * @brief Get CPU_CLK's XTAL/FAST_RC clock source path divider
 *
 * @return Divider. Divider = (PRE_DIV_CNT + 1).
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_cpu_get_divider(void)
{
    // TODO: IDF-5645
    return 0;
}

/**
 * @brief Select the clock source for RTC_SLOW_CLK
 *
 * @param in_sel One of the clock sources in soc_rtc_slow_clk_src_t
 */
static inline void clk_ll_rtc_slow_set_src(soc_rtc_slow_clk_src_t in_sel)
{
    // TODO: IDF-5645
}

/**
 * @brief Get the clock source for RTC_SLOW_CLK
 *
 * @return Currently selected clock source (one of soc_rtc_slow_clk_src_t values)
 */
static inline soc_rtc_slow_clk_src_t clk_ll_rtc_slow_get_src(void)
{
    // TODO: IDF-5645
    return 0;
}

/**
 * @brief Select the clock source for RTC_FAST_CLK
 *
 * @param in_sel One of the clock sources in soc_rtc_fast_clk_src_t
 */
static inline void clk_ll_rtc_fast_set_src(soc_rtc_fast_clk_src_t in_sel)
{
    // TODO: IDF-5645
}

/**
 * @brief Get the clock source for RTC_FAST_CLK
 *
 * @return Currently selected clock source (one of soc_rtc_fast_clk_src_t values)
 */
static inline soc_rtc_fast_clk_src_t clk_ll_rtc_fast_get_src(void)
{
    // TODO: IDF-5645
    return 0;
}

/**
 * @brief Set RC_FAST_CLK divider. The output from the divider is passed into rtc_fast_clk MUX.
 *
 * @param divider Divider of RC_FAST_CLK. Usually this divider is set to 1 (reg. value is 0) in bootloader stage.
 */
static inline void clk_ll_rc_fast_set_divider(uint32_t divider)
{
    // TODO: IDF-5645
}

/**
 * @brief Get RC_FAST_CLK divider
 *
 * @return Divider. Divider = (CK8M_DIV_SEL + 1).
 */
static inline uint32_t clk_ll_rc_fast_get_divider(void)
{
    // TODO: IDF-5645
    return 0;
}

/**
 * @brief Set RC_SLOW_CLK divider
 *
 * @param divider Divider of RC_SLOW_CLK. Usually this divider is set to 1 (reg. value is 0) in bootloader stage.
 */
static inline void clk_ll_rc_slow_set_divider(uint32_t divider)
{
    // TODO: IDF-5645
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
    // TODO: IDF-5645
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
    // TODO: IDF-5645
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
    // TODO: IDF-5645
}

/**
 * @brief Load APB_CLK frequency from RTC storage register
 *
 * Value of RTC_APB_FREQ_REG is stored as two copies in lower and upper 16-bit
 * halves. These are the routines to work with that representation.
 *
 * @return The stored APB frequency, in Hz
 */
static inline uint32_t clk_ll_apb_load_freq_hz(void)
{
    // TODO: IDF-5645
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
static inline void clk_ll_rtc_slow_store_cal(uint32_t cal_value)
{
    // TODO: IDF-5645
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
    // TODO: IDF-5645
    return 0;
}

#ifdef __cplusplus
}
#endif

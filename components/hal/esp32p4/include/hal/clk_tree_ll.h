/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc.h"
#include "soc/clk_tree_defs.h"
#include "soc/rtc.h"
#include "hal/regi2c_ctrl.h"
#include "soc/regi2c_bbpll.h"
#include "hal/assert.h"
#include "hal/log.h"
#include "esp32p4/rom/rtc.h"
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

/* APLL multiplier output frequency range */
// TODO: IDF-7526 check if the APLL frequency range is same as before
// apll_multiplier_out = xtal_freq * (4 + sdm2 + sdm1/256 + sdm0/65536)
#define CLK_LL_APLL_MULTIPLIER_MIN_HZ (350000000) // 350 MHz
#define CLK_LL_APLL_MULTIPLIER_MAX_HZ (500000000) // 500 MHz

/* APLL output frequency range */
#define CLK_LL_APLL_MIN_HZ    (5303031)   // 5.303031 MHz, refer to 'periph_rtc_apll_freq_set' for the calculation
#define CLK_LL_APLL_MAX_HZ    (125000000) // 125MHz, refer to 'periph_rtc_apll_freq_set' for the calculation

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
}

/**
 * @brief Power down BBPLL circuit
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_disable(void)
{

}

/**
 * @brief Enable the 32kHz crystal oscillator
 *
 * @param mode Used to determine the xtal32k configuration parameters
 */
static inline __attribute__((always_inline)) void clk_ll_xtal32k_enable(clk_ll_xtal32k_enable_mode_t mode)
{

}

/**
 * @brief Disable the 32kHz crystal oscillator
 */
static inline __attribute__((always_inline)) void clk_ll_xtal32k_disable(void)
{

}

/**
 * @brief Get the state of the 32kHz crystal clock
 *
 * @return True if the 32kHz XTAL is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_xtal32k_is_enabled(void)
{
    return 0;
}

/**
 * @brief Enable the internal oscillator output for RC32K_CLK
 */
static inline __attribute__((always_inline)) void clk_ll_rc32k_enable(void)
{

}

/**
 * @brief Disable the internal oscillator output for RC32K_CLK
 */
static inline __attribute__((always_inline)) void clk_ll_rc32k_disable(void)
{

}

/**
 * @brief Get the state of the internal oscillator for RC32K_CLK
 *
 * @return True if the oscillator is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_rc32k_is_enabled(void)
{
    return 0;
}

/**
 * @brief Enable the internal oscillator output for RC_FAST_CLK
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_enable(void)
{

}

/**
 * @brief Disable the internal oscillator output for RC_FAST_CLK
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_disable(void)
{

}

/**
 * @brief Get the state of the internal oscillator for RC_FAST_CLK
 *
 * @return True if the oscillator is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_rc_fast_is_enabled(void)
{
    return 0;
}

/**
 * @brief Enable the digital RC_FAST_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_digi_enable(void)
{

}

/**
 * @brief Disable the digital RC_FAST_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_digi_disable(void)
{

}

/**
 * @brief Get the state of the digital RC_FAST_CLK
 *
 * @return True if the digital RC_FAST_CLK is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_rc_fast_digi_is_enabled(void)
{
    return 0;
}

/**
 * @brief Enable the digital XTAL32K_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_xtal32k_digi_enable(void)
{

}

/**
 * @brief Disable the digital XTAL32K_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_xtal32k_digi_disable(void)
{

}

/**
 * @brief Get the state of the digital XTAL32K_CLK
 *
 * @return True if the digital XTAL32K_CLK is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_xtal32k_digi_is_enabled(void)
{
    return 0;
}

/**
 * @brief Enable the digital RC32K_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_rc32k_digi_enable(void)
{

}

/**
 * @brief Disable the digital RC32K_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_rc32k_digi_disable(void)
{

}

/**
 * @brief Get the state of the digital RC32K_CLK
 *
 * @return True if the digital RC32K_CLK is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_rc32k_digi_is_enabled(void)
{
    return 0;
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

}

/**
 * @brief Set BBPLL frequency from XTAL source (Analog part)
 *
 * @param pll_freq_mhz PLL frequency, in MHz
 * @param xtal_freq_mhz XTAL frequency, in MHz
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_set_config(uint32_t pll_freq_mhz, uint32_t xtal_freq_mhz)
{

}

/**
 * @brief Select the clock source for CPU_CLK (SOC Clock Root)
 *
 * @param in_sel One of the clock sources in soc_cpu_clk_src_t
 */
static inline __attribute__((always_inline)) void clk_ll_cpu_set_src(soc_cpu_clk_src_t in_sel)
{

}

/**
 * @brief Get the clock source for CPU_CLK (SOC Clock Root)
 *
 * @return Currently selected clock source (one of soc_cpu_clk_src_t values)
 */
static inline __attribute__((always_inline)) soc_cpu_clk_src_t clk_ll_cpu_get_src(void)
{
    return SOC_CPU_CLK_SRC_XTAL;
}

/**
 * @brief Set CPU_CLK's high-speed divider (valid when SOC_ROOT clock source is PLL)
 *
 * @param divider Divider. (PCR_HS_DIV_NUM + 1) * (PCR_CPU_HS_DIV_NUM + 1) = divider.
 */
static inline __attribute__((always_inline)) void clk_ll_cpu_set_hs_divider(uint32_t divider)
{

}

/**
 * @brief Set CPU_CLK's low-speed divider (valid when SOC_ROOT clock source is XTAL/RC_FAST)
 *
 * @param divider Divider. (PCR_LS_DIV_NUM + 1) * (PCR_CPU_LS_DIV_NUM + 1) = divider.
 */
static inline __attribute__((always_inline)) void clk_ll_cpu_set_ls_divider(uint32_t divider)
{

}

/**
 * @brief Get CPU_CLK's high-speed divider
 *
 * @return Divider. Divider = (PCR_HS_DIV_NUM + 1) * (PCR_CPU_HS_DIV_NUM + 1).
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_cpu_get_hs_divider(void)
{
    return 0;
}

/**
 * @brief Get CPU_CLK's low-speed divider
 *
 * @return Divider. Divider = (PCR_LS_DIV_NUM + 1) * (PCR_CPU_LS_DIV_NUM + 1).
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_cpu_get_ls_divider(void)
{
    return 0;
}

/**
 * @brief Set AHB_CLK's high-speed divider (valid when SOC_ROOT clock source is PLL)
 *
 * @param divider Divider. (PCR_HS_DIV_NUM + 1) * (PCR_AHB_HS_DIV_NUM + 1) = divider.
 */
static inline __attribute__((always_inline)) void clk_ll_ahb_set_hs_divider(uint32_t divider)
{

}

/**
 * @brief Set AHB_CLK's low-speed divider (valid when SOC_ROOT clock source is XTAL/RC_FAST)
 *
 * @param divider Divider. (PCR_LS_DIV_NUM + 1) * (PCR_AHB_LS_DIV_NUM + 1) = divider.
 */
static inline __attribute__((always_inline)) void clk_ll_ahb_set_ls_divider(uint32_t divider)
{

}

/**
 * @brief Get AHB_CLK's high-speed divider
 *
 * @return Divider. Divider = (PCR_HS_DIV_NUM + 1) * (PCR_AHB_HS_DIV_NUM + 1).
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_ahb_get_hs_divider(void)
{
    return 0;
}

/**
 * @brief Get AHB_CLK's low-speed divider
 *
 * @return Divider. Divider = (PCR_LS_DIV_NUM + 1) * (PCR_AHB_LS_DIV_NUM + 1).
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_ahb_get_ls_divider(void)
{
    return 1;
}

/**
 * @brief Set APB_CLK divider. freq of APB_CLK = freq of AHB_CLK / divider
 *
 * @param divider Divider. PCR_APB_DIV_NUM = divider - 1.
 */
static inline __attribute__((always_inline)) void clk_ll_apb_set_divider(uint32_t divider)
{

}

/**
 * @brief Get APB_CLK divider
 *
 * @return Divider. Divider = (PCR_APB_DIV_NUM + 1).
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_apb_get_divider(void)
{
    return 1;
}

/**
 * @brief Set MSPI_FAST_CLK's high-speed divider (valid when SOC_ROOT clock source is PLL)
 *
 * @param divider Divider.
 */
static inline __attribute__((always_inline)) void clk_ll_mspi_fast_set_hs_divider(uint32_t divider)
{

}

/**
 * @brief Set MSPI_FAST_CLK's low-speed divider (valid when SOC_ROOT clock source is XTAL/RC_FAST)
 *
 * @param divider Divider.
 */
static inline __attribute__((always_inline)) void clk_ll_mspi_fast_set_ls_divider(uint32_t divider)
{

}

/**
 * @brief Select the calibration 32kHz clock source for timergroup0
 *
 * @param in_sel One of the 32kHz clock sources (RC32K_CLK, XTAL32K_CLK, OSC_SLOW_CLK)
 */
static inline __attribute__((always_inline)) void clk_ll_32k_calibration_set_target(soc_rtc_slow_clk_src_t in_sel)
{

}

/**
 * @brief Get the calibration 32kHz clock source for timergroup0
 *
 * @return soc_rtc_slow_clk_src_t Currently selected calibration 32kHz clock (one of the 32kHz clocks)
 */
static inline __attribute__((always_inline)) soc_rtc_slow_clk_src_t clk_ll_32k_calibration_get_target(void)
{
    return (soc_rtc_slow_clk_src_t)0;
}

/**
 * @brief Select the clock source for RTC_SLOW_CLK
 *
 * @param in_sel One of the clock sources in soc_rtc_slow_clk_src_t
 */
static inline __attribute__((always_inline)) void clk_ll_rtc_slow_set_src(soc_rtc_slow_clk_src_t in_sel)
{

}

/**
 * @brief Get the clock source for RTC_SLOW_CLK
 *
 * @return Currently selected clock source (one of soc_rtc_slow_clk_src_t values)
 */
static inline __attribute__((always_inline)) soc_rtc_slow_clk_src_t clk_ll_rtc_slow_get_src(void)
{
    return (soc_rtc_slow_clk_src_t)0;
}

/**
 * @brief Select the clock source for RTC_FAST_CLK
 *
 * @param in_sel One of the clock sources in soc_rtc_fast_clk_src_t
 */
static inline __attribute__((always_inline)) void clk_ll_rtc_fast_set_src(soc_rtc_fast_clk_src_t in_sel)
{

}

/**
 * @brief Get the clock source for RTC_FAST_CLK
 *
 * @return Currently selected clock source (one of soc_rtc_fast_clk_src_t values)
 */
static inline __attribute__((always_inline)) soc_rtc_fast_clk_src_t clk_ll_rtc_fast_get_src(void)
{
    return (soc_rtc_fast_clk_src_t)0;
}

/**
 * @brief Set RC_FAST_CLK divider. The output from the divider is passed into rtc_fast_clk MUX.
 *
 * @param divider Divider of RC_FAST_CLK. Usually this divider is set to 1 (reg. value is 0) in bootloader stage.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_set_divider(uint32_t divider)
{

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
    return 40;
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
    return 0;
}

#ifdef __cplusplus
}
#endif

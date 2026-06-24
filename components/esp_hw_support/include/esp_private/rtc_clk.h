/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Switch CPU clock source to XTAL, and let cpu frequency equal to main XTAL frequency.
 *
 * This function does not disable CPU's source PLL. If the PLL requires to be disabled to save power, please call
 * `rtc_clk_cpu_freq_set_xtal` instead. It will always disable the corresponding PLL after switching the CPU clock
 * source to XTAL (except for S2).
 *
 * Currently, this function is only called in `esp_restart_noos` and `esp_restart_noos_dig` to switch the CPU
 * clock source back to XTAL (by default) before reset.
 */
void rtc_clk_cpu_set_to_default_config(void);

/**
 * @brief Switch CPU clock source to XTAL, the PLL has different processing methods for different chips.
 *        1. For earlier chips without PMU, there is no PMU module that can turn off the CPU's PLL, so it has to be
 *           disabled at here to save the power consumption. Though ESP32C3/S3 has USB CDC device, it can not function
 *           properly during sleep due to the lack of APB clock (before C6, USJ relies on APB clock to work). Therefore,
 *           we will always disable CPU's PLL (i.e. BBPLL).
 *        2. For PMU supported chips, CPU's PLL power can be turned off by PMU, so no need to disable the PLL at here.
 *           Leaving PLL on at this stage also helps USJ keep connection and retention operation (if they rely on this PLL).
 *           For ESP32P4, if the APB frequency is configured as the hardware default value (10MHz), this will cause the
 *           regdma backup/restore to not achieve optimal performance. The MEM/APB frequency divider needs to be configured
 *           to 40MHz to speed up the retention speed.
 */
void rtc_clk_cpu_freq_set_xtal_for_sleep(void);

/**
 * @brief Notify that the BBPLL has a new in-use consumer
 *
 * Currently, this function is only used for tracking whether USB Serial/JTAG is using the 48MHz PHY clock
 *
 * Note: Calling this function only helps to not disable the BBPLL clock in `rtc_clk_cpu_freq_set_config`.
 */
void rtc_clk_bbpll_add_consumer(void);

/**
 * @brief Notify that the BBPLL has lost a consumer
 */
void rtc_clk_bbpll_remove_consumer(void);

#if SOC_CLK_MPLL_SUPPORTED
//------------------------------------MPLL-------------------------------------//
/**
 * @brief Enable MPLL
 */
void rtc_clk_mpll_enable(void);

/**
 * @brief Disable MPLL
 */
void rtc_clk_mpll_disable(void);

/**
 * @brief Configure MPLL
 *
 * @param[in] xtal_freq  XTAL frequency
 * @param[in] mpll_freq  MPLL frequency
 * @param[in] thread_safe Set true if called from thread safe context, which will save the time of taking spin lock.
 */
void rtc_clk_mpll_configure(uint32_t xtal_freq, uint32_t mpll_freq, bool thread_safe);

/**
 * Get the MPLL frequency
 * @return the value of MPLL frequency in MHz
 */
uint32_t rtc_clk_mpll_get_freq(void);

#endif  //#if SOC_CLK_MPLL_SUPPORTED

#ifdef __cplusplus
}
#endif

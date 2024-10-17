/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
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
 * clock source back to XTAL (by default) before reset, and in `esp_sleep_start` to switch CPU clock source to XTAL
 * before entering sleep for PMU supported chips.
 */
void rtc_clk_cpu_set_to_default_config(void);

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
 */
void rtc_clk_mpll_configure(uint32_t xtal_freq, uint32_t mpll_freq);

/**
 * Get the MPLL frequency
 * @return the value of MPLL frequency in MHz
 */
uint32_t rtc_clk_mpll_get_freq(void);

#endif  //#if SOC_CLK_MPLL_SUPPORTED

#ifdef __cplusplus
}
#endif

/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Switch CPU clock source to XTAL, and let cpu frequency equal to main XTAL frequency.
 *
 * This function does not disable BBPLL. If BBPLL requires to be disabled to save power, please call
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

#ifdef __cplusplus
}
#endif

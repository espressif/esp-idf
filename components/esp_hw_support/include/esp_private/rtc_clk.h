/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
 * `rtc_clk_cpu_freq_set_xtal` instead. It does one extra check to see whether can disable the BBPLL after switching the
 * CPU clock source to XTAL.
 *
 * Currently, this function should only be called in `esp_restart_noos` and `esp_restart_noos_dig` to switch the CPU
 * clock source back to XTAL (by default) before reset.
 */
void rtc_clk_cpu_set_to_default_config(void);

/**
 * @brief Notify that the BBPLL has a new in-use consumer
 *
 * Currently, this function is only used for tracking whether USB Serial/JTAG is using the 48MHz PHY clock
 *
 * Note: Calling this function only helps to not disable the BBPLL clock in `rtc_clk_cpu_freq_set_config`.
 *       For light and deep sleep, whether to disable the BBPLL in the interal call to `rtc_clk_cpu_freq_set_xtal`
 *       varies for targets.
 *       On ESP32C3/S3, USB CDC device can not function properly during sleep due to the lack of APB clock. Therefore.
 *       `rtc_clk_cpu_freq_set_xtal` will always disable BBPLL, no matter whether BBPLL has any consumer.
 *       On ESP32C6/H2, USB CDC device can maintain the minimum connection with the host during sleep, so
 *       `rtc_clk_cpu_freq_set_xtal` will check for BBPLL consumers, and keep BBPLL if USB Serial/JTAG is in use.
 */
void rtc_clk_bbpll_add_consumer(void);

/**
 * @brief Notify that the BBPLL has lost a consumer
 */
void rtc_clk_bbpll_remove_consumer(void);

/**
 * @brief Workaround for C2, S3, C6, H2. Trigger the calibration of PLL. Should be called when the bootloader doesn't provide a good enough PLL accuracy.
*/
void rtc_clk_recalib_bbpll(void);


#ifdef __cplusplus
}
#endif

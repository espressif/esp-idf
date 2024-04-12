/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/clk_tree_defs.h"
#include "soc/clkout_channel.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get SOC_ROOT_CLK frequency
 *
 * @param cpu_clk_src One of the clock sources in soc_cpu_clk_src_t
 *
 * @return SOC ROOT clock frequency, in MHz. Returns 0 if input argument is invalid.
 */
uint32_t clk_hal_soc_root_get_freq_mhz(soc_cpu_clk_src_t cpu_clk_src);

/**
 * @brief Get CPU_CLK frequency
 *
 * @return CPU clock frequency, in Hz. Returns 0 if internal clock configuration is invalid.
 */
uint32_t clk_hal_cpu_get_freq_hz(void);

/**
 * @brief Get APB_CLK frequency
 *
 * @return APB clock frequency, in Hz. Returns 0 if internal clock configuration is invalid.
 */
uint32_t clk_hal_apb_get_freq_hz(void);

/**
 * @brief Get LP_SLOW_CLK (i.e. RTC_SLOW_CLK) approximate frequency
 *
 * @return LP Slow clock frequency, in Hz. Returns 0 if LP_SLOW clock source is invalid.
 */
uint32_t clk_hal_lp_slow_get_freq_hz(void);

/**
 * @brief Get XTAL_CLK frequency
 *
 * @return XTAL clock frequency, in MHz
 */
uint32_t clk_hal_xtal_get_freq_mhz(void);

#if SOC_CLK_APLL_SUPPORTED
/**
 * @brief Get APLL_CLK frequency
 *
 * @return APLL clock frequency, in Hz
 */
uint32_t clk_hal_apll_get_freq_hz(void);
#endif //SOC_CLK_APLL_SUPPORTED

/**
 * @brief Set up clock output channel
 * @param clk_sig    The clock signal source to be mapped to GPIOs
 * @param channel_id The clock output channel to setup
 */
void clk_hal_clock_output_setup(soc_clkout_sig_id_t clk_sig, clock_out_channel_t channel_id);

#if SOC_CLOCKOUT_SUPPORT_CHANNEL_DIVIDER
/**
 * @brief Output the mapped clock after frequency division
 * @param channel_id channel id that need to be configured with frequency division
 * @param div_num  clock frequency division value
 */
void clk_hal_clock_output_set_divider(clock_out_channel_t channel_id, uint32_t div_num);
#endif

/**
 * @brief Teardown clock output channel configuration
 * @param channel_id The clock output channel to teardown
 */
void clk_hal_clock_output_teardown(clock_out_channel_t channel_id);

#ifdef __cplusplus
}
#endif

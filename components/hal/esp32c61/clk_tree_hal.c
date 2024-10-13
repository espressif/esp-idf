/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/clk_tree_hal.h"
#include "hal/clk_tree_ll.h"
#include "hal/assert.h"
#include "hal/log.h"

uint32_t clk_hal_soc_root_get_freq_mhz(soc_cpu_clk_src_t cpu_clk_src)
{
    switch (cpu_clk_src) {
    case SOC_CPU_CLK_SRC_XTAL:
        return clk_hal_xtal_get_freq_mhz();
    case SOC_CPU_CLK_SRC_PLL_F160M:
        return CLK_LL_PLL_160M_FREQ_MHZ;
    case SOC_CPU_CLK_SRC_RC_FAST:
        return SOC_CLK_RC_FAST_FREQ_APPROX / MHZ;
    default:
        // Unknown CPU_CLK mux input
        HAL_ASSERT(false);
        return 0;
    }
}

uint32_t clk_hal_cpu_get_freq_hz(void)
{
    soc_cpu_clk_src_t source = clk_ll_cpu_get_src();
    uint32_t divider = clk_ll_cpu_get_divider();
    return clk_hal_soc_root_get_freq_mhz(source) * MHZ / divider;
}

static uint32_t clk_hal_ahb_get_freq_hz(void)
{
    soc_cpu_clk_src_t source = clk_ll_cpu_get_src();
    uint32_t divider = clk_ll_ahb_get_divider();
    return clk_hal_soc_root_get_freq_mhz(source) * MHZ / divider;
}

uint32_t clk_hal_apb_get_freq_hz(void)
{
    return clk_hal_ahb_get_freq_hz() / clk_ll_apb_get_divider();
}

uint32_t clk_hal_lp_slow_get_freq_hz(void)
{
    switch (clk_ll_rtc_slow_get_src()) {
    case SOC_RTC_SLOW_CLK_SRC_RC_SLOW:
        return SOC_CLK_RC_SLOW_FREQ_APPROX;
    case SOC_RTC_SLOW_CLK_SRC_XTAL32K:
        return SOC_CLK_XTAL32K_FREQ_APPROX;
    case SOC_RTC_SLOW_CLK_SRC_OSC_SLOW:
        return SOC_CLK_OSC_SLOW_FREQ_APPROX;
    default:
        // Unknown RTC_SLOW_CLK mux input
        HAL_ASSERT(false);
        return 0;
    }
}

uint32_t clk_hal_xtal_get_freq_mhz(void)
{
    uint32_t freq = clk_ll_xtal_get_freq_mhz();
    HAL_ASSERT(freq == SOC_XTAL_FREQ_40M);
    return freq;
}

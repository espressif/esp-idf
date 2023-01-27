/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/clk_tree_hal.h"
#include "hal/clk_tree_ll.h"
#include "soc/rtc.h"
#include "hal/assert.h"
#include "hal/log.h"

static const char *CLK_HAL_TAG = "clk_hal";

uint32_t clk_hal_soc_root_get_freq_mhz(soc_cpu_clk_src_t cpu_clk_src)
{
    switch (cpu_clk_src) {
    case SOC_CPU_CLK_SRC_XTAL:
        return clk_hal_xtal_get_freq_mhz();
    case SOC_CPU_CLK_SRC_PLL:
        return clk_ll_bbpll_get_freq_mhz();
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
    switch (source) {
    case SOC_CPU_CLK_SRC_PLL: {
        // PLL 320MHz, CPU 240MHz is an undetermined state
        uint32_t pll_freq_mhz = clk_ll_bbpll_get_freq_mhz();
        uint32_t cpu_freq_mhz = clk_ll_cpu_get_freq_mhz_from_pll();
        if (pll_freq_mhz == CLK_LL_PLL_320M_FREQ_MHZ && cpu_freq_mhz == CLK_LL_PLL_240M_FREQ_MHZ) {
            HAL_LOGE(CLK_HAL_TAG, "Invalid cpu config");
            return 0;
        }
        return cpu_freq_mhz * MHZ;
    }
    default: // SOC_CPU_CLK_SRC_XTAL, SOC_CPU_CLK_SRC_RC_FAST...
        return clk_hal_soc_root_get_freq_mhz(source) * MHZ / clk_ll_cpu_get_divider();
    }
}

uint32_t clk_hal_ahb_get_freq_hz(void)
{
    // AHB_CLK path is highly dependent on CPU_CLK path
    switch (clk_ll_cpu_get_src()) {
    case SOC_CPU_CLK_SRC_PLL:
        // AHB_CLK is a fixed value when CPU_CLK is clocked from PLL
        return CLK_LL_AHB_MAX_FREQ_MHZ * MHZ;
    default: // SOC_CPU_CLK_SRC_XTAL, SOC_CPU_CLK_SRC_RC_FAST...
        return clk_hal_cpu_get_freq_hz();
    }
}

uint32_t clk_hal_apb_get_freq_hz(void)
{
    return clk_hal_ahb_get_freq_hz();
}

uint32_t clk_hal_lp_slow_get_freq_hz(void)
{
    switch (clk_ll_rtc_slow_get_src()) {
    case SOC_RTC_SLOW_CLK_SRC_RC_SLOW:
        return SOC_CLK_RC_SLOW_FREQ_APPROX;
    case SOC_RTC_SLOW_CLK_SRC_XTAL32K:
        return SOC_CLK_XTAL32K_FREQ_APPROX;
    case SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256:
        return SOC_CLK_RC_FAST_D256_FREQ_APPROX;
    default:
        // Unknown RTC_SLOW_CLK mux input
        HAL_ASSERT(false);
        return 0;
    }
}

uint32_t clk_hal_xtal_get_freq_mhz(void)
{
    uint32_t freq = clk_ll_xtal_load_freq_mhz();
    if (freq == 0) {
        HAL_LOGW(CLK_HAL_TAG, "invalid RTC_XTAL_FREQ_REG value, assume 40MHz");
        return (uint32_t)RTC_XTAL_FREQ_40M;
    }
    return freq;
}

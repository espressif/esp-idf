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
    case SOC_CPU_CLK_SRC_APLL:
        return clk_hal_apll_get_freq_hz() / MHZ;
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
    case SOC_CPU_CLK_SRC_PLL:
        return clk_ll_cpu_get_freq_mhz_from_pll() * MHZ;
    case SOC_CPU_CLK_SRC_APLL: {
        uint32_t apll_freq_hz = clk_hal_apll_get_freq_hz();
        uint32_t divider = clk_ll_cpu_get_divider_from_apll();
        if (divider == 0) {
            HAL_LOGE(CLK_HAL_TAG, "Invalid cpu config");
            return 0;
        }
        return apll_freq_hz / divider;
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
    case SOC_CPU_CLK_SRC_APLL:
        return clk_hal_cpu_get_freq_hz() >> 1;
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
        return (uint32_t)RTC_XTAL_FREQ_AUTO;
    }
    return freq;
}

uint32_t clk_hal_apll_get_freq_hz(void)
{
    uint64_t xtal_freq_hz = (uint64_t)clk_hal_xtal_get_freq_mhz() * 1000000ULL;
    uint32_t o_div = 0;
    uint32_t sdm0 = 0;
    uint32_t sdm1 = 0;
    uint32_t sdm2 = 0;
    clk_ll_apll_get_config(&o_div, &sdm0, &sdm1, &sdm2);
    uint32_t numerator = ((4 + sdm2) << 16) | (sdm1 << 8) | sdm0;
    uint32_t denominator = (o_div + 2) << 17;
    uint32_t apll_freq_hz = (uint32_t)((xtal_freq_hz * numerator) / denominator);
    return apll_freq_hz;
}

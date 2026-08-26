/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_attr.h"
#include "hal/clk_tree_hal.h"
#include "hal/clk_tree_ll.h"
#include "hal/assert.h"
#include "hal/log.h"

HAL_LOG_ATTR_TAG(CLK_HAL_TAG, "clk_hal");

uint32_t clk_hal_soc_root_get_freq_mhz(soc_cpu_clk_src_t cpu_clk_src)
{
    switch (cpu_clk_src) {
    case SOC_CPU_CLK_SRC_XTAL:
        return clk_hal_xtal_get_freq_mhz();
    case SOC_CPU_CLK_SRC_CPLL:
        return clk_ll_cpll_get_freq_mhz(clk_hal_xtal_get_freq_mhz());
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
    uint32_t integer, numerator, denominator;
    clk_ll_cpu_get_divider(&integer, &numerator, &denominator);
    if (denominator == 0) {
        denominator = 1;
        numerator = 0;
    }
    return clk_hal_soc_root_get_freq_mhz(source) * MHZ * denominator / (integer * denominator + numerator);
}

static uint32_t clk_hal_mem_get_freq_hz(void)
{
    return clk_hal_cpu_get_freq_hz() / clk_ll_mem_get_divider();
}

uint32_t clk_hal_sys_get_freq_hz(void)
{
    return clk_hal_mem_get_freq_hz() / clk_ll_sys_get_divider();
}

uint32_t clk_hal_apb_get_freq_hz(void)
{
    return clk_hal_sys_get_freq_hz() / clk_ll_apb_get_divider();
}

uint32_t clk_hal_lp_slow_get_freq_hz(void)
{
    switch (clk_ll_rtc_slow_get_src()) {
    case SOC_RTC_SLOW_CLK_SRC_RC_SLOW:
        return SOC_CLK_RC_SLOW_FREQ_APPROX;
    case SOC_RTC_SLOW_CLK_SRC_XTAL32K:
        return SOC_CLK_XTAL32K_FREQ_APPROX;
    case SOC_RTC_SLOW_CLK_SRC_RC32K:
        return SOC_CLK_RC32K_FREQ_APPROX;
    default:
        // Unknown RTC_SLOW_CLK mux input
        HAL_ASSERT(false);
        return 0;
    }
}

IRAM_ATTR uint32_t clk_hal_xtal_get_freq_mhz(void)
{
    uint32_t freq = clk_ll_xtal_load_freq_mhz();
    if (freq == 0) {
        HAL_LOGW(CLK_HAL_TAG, "invalid RTC_XTAL_FREQ_REG value, assume 40MHz");
        return (uint32_t)SOC_XTAL_FREQ_40M;
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

void clk_hal_clock_output_setup(soc_clkout_sig_id_t clk_sig, clock_out_channel_t channel_id)
{
    clk_ll_bind_output_channel(clk_sig, channel_id);
    clk_ll_set_output_channel_divider(channel_id, 1);
    clk_ll_enable_output_channel(channel_id, true);
}

void clk_hal_clock_output_teardown(clock_out_channel_t channel_id)
{
    clk_ll_enable_output_channel(channel_id, false);
}

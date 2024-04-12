/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/clk_tree_hal.h"
#include "hal/clk_tree_ll.h"
#include "hal/assert.h"
#include "hal/log.h"

static const char *CLK_HAL_TAG = "clk_hal";

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

static uint32_t clk_hal_sys_get_freq_hz(void)
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

uint32_t clk_hal_xtal_get_freq_mhz(void)
{
    uint32_t freq = clk_ll_xtal_load_freq_mhz();
    if (freq == 0) {
        HAL_LOGW(CLK_HAL_TAG, "invalid RTC_XTAL_FREQ_REG value, assume 40MHz");
        return (uint32_t)SOC_XTAL_FREQ_40M;
    }
    return freq;
}

void clk_hal_clock_output_setup(soc_clkout_sig_id_t clk_sig, clock_out_channel_t channel_id)
{
    clk_ll_set_dbg_clk_ctrl(clk_sig, channel_id);
    clk_ll_set_dbg_clk_channel_divider(channel_id, 1);
    clk_ll_enable_dbg_clk_channel(channel_id, true);
}

void clk_hal_clock_output_set_divider(clock_out_channel_t channel_id, uint32_t div_num)
{
    clk_ll_set_dbg_clk_channel_divider(channel_id, div_num);
}

void clk_hal_clock_output_teardown(clock_out_channel_t channel_id)
{
    clk_ll_enable_dbg_clk_channel(channel_id, false);
}

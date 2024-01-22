/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "soc/clkout_channel.h"
#include "hal/assert.h"
#include "hal/clk_tree_hal.h"
#include "hal/clk_tree_ll.h"
#include "hal/gpio_ll.h"
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
    case SOC_CPU_CLK_SRC_PLL:
        return clk_ll_cpu_get_freq_mhz_from_pll() * MHZ;
    default: // SOC_CPU_CLK_SRC_XTAL, SOC_CPU_CLK_SRC_RC_FAST...
        return clk_hal_soc_root_get_freq_mhz(source) * MHZ / clk_ll_cpu_get_divider();
    }
}

static uint32_t clk_hal_ahb_get_freq_hz(void)
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
    case SOC_RTC_SLOW_CLK_SRC_OSC_SLOW:
        return SOC_CLK_OSC_SLOW_FREQ_APPROX;
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
        HAL_LOGW(CLK_HAL_TAG, "invalid RTC_XTAL_FREQ_REG value, assume %dMHz", CONFIG_XTAL_FREQ);
        return CONFIG_XTAL_FREQ;
    }
    return freq;
}

void clk_hal_clock_output_setup(soc_clkout_sig_id_t clk_sig, clock_out_channel_t channel_id)
{
    gpio_ll_set_pin_ctrl(clk_sig, CLKOUT_CHANNEL_MASK(channel_id), CLKOUT_CHANNEL_SHIFT(channel_id));
}

void clk_hal_clock_output_teardown(clock_out_channel_t channel_id)
{
    gpio_ll_set_pin_ctrl(0, CLKOUT_CHANNEL_MASK(channel_id), CLKOUT_CHANNEL_SHIFT(channel_id));
}

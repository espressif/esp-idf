/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    case SOC_CPU_CLK_SRC_PLL_F160M:
        return CLK_LL_PLL_160M_FREQ_MHZ;
    case SOC_CPU_CLK_SRC_PLL_F240M:
        return CLK_LL_PLL_240M_FREQ_MHZ;
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    case SOC_CPU_CLK_SRC_PLL:
        return clk_ll_bbpll_get_freq_mhz();
#endif
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
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    uint32_t divider = clk_ll_cpu_get_divider();
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    uint32_t divider = (source == SOC_CPU_CLK_SRC_PLL) ? clk_ll_cpu_get_hs_divider() : clk_ll_cpu_get_ls_divider();
#endif
    return clk_hal_soc_root_get_freq_mhz(source) * MHZ / divider;
}

uint32_t clk_hal_ahb_get_freq_hz(void)
{
    soc_cpu_clk_src_t source = clk_ll_cpu_get_src();
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    uint32_t divider = clk_ll_ahb_get_divider();
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    uint32_t divider = (source == SOC_CPU_CLK_SRC_PLL) ? clk_ll_ahb_get_hs_divider() : clk_ll_ahb_get_ls_divider();
#endif
    return clk_hal_soc_root_get_freq_mhz(source) * MHZ / divider;
}

uint32_t clk_hal_apb_get_freq_hz(void)
{
    return clk_hal_ahb_get_freq_hz() / clk_ll_apb_get_divider();
}

uint32_t clk_hal_lp_slow_get_freq_hz(void)
{
    switch (clk_ll_rtc_slow_get_src()) {
#if CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    case SOC_RTC_SLOW_CLK_SRC_RC_SLOW:
        return SOC_CLK_RC_SLOW_FREQ_APPROX;
#endif
    case SOC_RTC_SLOW_CLK_SRC_XTAL32K:
        return SOC_CLK_XTAL32K_FREQ_APPROX;
    case SOC_RTC_SLOW_CLK_SRC_OSC_SLOW:
        return SOC_CLK_OSC_SLOW_FREQ_APPROX;
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
    gpio_ll_set_pin_ctrl(clk_sig, CLKOUT_CHANNEL_MASK(channel_id), CLKOUT_CHANNEL_SHIFT(channel_id));
}

void clk_hal_clock_output_teardown(clock_out_channel_t channel_id)
{
    gpio_ll_set_pin_ctrl(0, CLKOUT_CHANNEL_MASK(channel_id), CLKOUT_CHANNEL_SHIFT(channel_id));
}

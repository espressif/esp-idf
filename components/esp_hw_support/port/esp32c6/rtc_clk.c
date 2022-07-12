/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "esp32c6/rom/ets_sys.h"
#include "esp32c6/rom/rtc.h"
#include "soc/rtc.h"
#include "esp_hw_log.h"
#include "esp_rom_sys.h"
#include "hal/usb_serial_jtag_ll.h"
#include "hal/clk_tree_ll.h"
#include "hal/regi2c_ctrl_ll.h"
#include "soc/lp_clkrst_reg.h"

static const char *TAG = "rtc_clk";

void rtc_clk_32k_enable(bool enable)
{
    // TODO: IDF-5645
}

void rtc_clk_32k_enable_external(void)
{
    // TODO: IDF-5645
}

void rtc_clk_32k_bootstrap(uint32_t cycle)
{
    // TODO: IDF-5645
}

bool rtc_clk_32k_enabled(void)
{
    // TODO: IDF-5645
    return 0;
}

void rtc_clk_8m_enable(bool clk_8m_en, bool d256_en)
{
    // TODO: IDF-5645
}

bool rtc_clk_8m_enabled(void)
{
    // TODO: IDF-5645
    return 0;
}

bool rtc_clk_8md256_enabled(void)
{
    // TODO: IDF-5645
    return 0;
}

void rtc_clk_slow_src_set(soc_rtc_slow_clk_src_t clk_src)
{
    // TODO: IDF-5645
}

soc_rtc_slow_clk_src_t rtc_clk_slow_src_get(void)
{
    // TODO: IDF-5645
    return REG_GET_FIELD(LP_CLKRST_LP_CLK_CONF_REG, LP_CLKRST_SLOW_CLK_SEL);
}

uint32_t rtc_clk_slow_freq_get_hz(void)
{
    // TODO: IDF-5645
    switch (rtc_clk_slow_freq_get()) {
        case RTC_SLOW_FREQ_RTC: return RTC_SLOW_CLK_FREQ_150K;
        case RTC_SLOW_FREQ_32K_XTAL: return RTC_SLOW_CLK_FREQ_32K;
        case RTC_SLOW_FREQ_8MD256: return RTC_SLOW_CLK_FREQ_8MD256;
        default: return 0;
    }
}

void rtc_clk_fast_src_set(soc_rtc_fast_clk_src_t clk_src)
{
    // TODO: IDF-5645
}

soc_rtc_fast_clk_src_t rtc_clk_fast_src_get(void)
{
    // TODO: IDF-5645
    return 0;
}

#if 0
static void rtc_clk_bbpll_disable(void)
{
    // TODO: IDF-5645
}

static void rtc_clk_bbpll_enable(void)
{
    // TODO: IDF-5645
}

static void rtc_clk_bbpll_configure(rtc_xtal_freq_t xtal_freq, int pll_freq)
{
    // TODO: IDF-5645
}

/**
 * Switch to one of PLL-based frequencies. Current frequency can be XTAL or PLL.
 * PLL must already be enabled.
 * @param cpu_freq new CPU frequency
 */
static void rtc_clk_cpu_freq_to_pll_mhz(int cpu_freq_mhz)
{
    // TODO: IDF-5645
}
#endif

bool rtc_clk_cpu_freq_mhz_to_config(uint32_t freq_mhz, rtc_cpu_freq_config_t *out_config)
{
    // TODO: IDF-5645
    return 0;
}

void rtc_clk_cpu_freq_set_config(const rtc_cpu_freq_config_t *config)
{
    // TODO: IDF-5645
}

void rtc_clk_cpu_freq_get_config(rtc_cpu_freq_config_t *out_config)
{
    // TODO: IDF-5645
}

void rtc_clk_cpu_freq_set_config_fast(const rtc_cpu_freq_config_t *config)
{
    // TODO: IDF-5645
}

void rtc_clk_cpu_freq_set_xtal(void)
{
    ESP_EARLY_LOGW(TAG, "rtc_clk_cpu_freq_set_xtal() has not been implemented yet");
    // TODO: IDF-5645
}

#if 0
/**
 * Switch to XTAL frequency. Does not disable the PLL.
 */
static void rtc_clk_cpu_freq_to_xtal(int freq, int div)
{
    // TODO: IDF-5645
}

static void rtc_clk_cpu_freq_to_8m(void)
{
    // TODO: IDF-5645
}
#endif

rtc_xtal_freq_t rtc_clk_xtal_freq_get(void)
{
    ESP_EARLY_LOGW(TAG, "rtc_clk_xtal_freq_get() has not been implemented yet");
    // TODO: IDF-5645
    return 40;
}

void rtc_clk_xtal_freq_update(rtc_xtal_freq_t xtal_freq)
{
    // TODO: IDF-5645
}

void rtc_clk_apb_freq_update(uint32_t apb_freq)
{
    // TODO: IDF-5645
}

uint32_t rtc_clk_apb_freq_get(void)
{
    ESP_EARLY_LOGW(TAG, "rtc_clk_apb_freq_get() has not been implemented yet");
    // TODO: IDF-5645
    return 0;
}

void rtc_clk_divider_set(uint32_t div)
{
    // TODO: IDF-5645
}

void rtc_clk_8m_divider_set(uint32_t div)
{
    // TODO: IDF-5645
}

void rtc_dig_clk8m_enable(void)
{
    // TODO: IDF-5645
}

void rtc_dig_clk8m_disable(void)
{
    // TODO: IDF-5645
}

bool rtc_dig_8m_enabled(void)
{
    // TODO: IDF-5645
    return 0;
}

#if 0
static bool rtc_clk_set_bbpll_always_on(void)
{
    // TODO: IDF-5645
    return 0;
}
#endif

/* Name used in libphy.a:phy_chip_v7.o
 * TODO: update the library to use rtc_clk_xtal_freq_get
 */
rtc_xtal_freq_t rtc_get_xtal(void) __attribute__((alias("rtc_clk_xtal_freq_get")));

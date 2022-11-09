/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "esp32h4/rom/ets_sys.h"
#include "esp32h4/rom/rtc.h"
#include "esp32h4/rom/uart.h"
#include "esp32h4/rom/gpio.h"
#include "soc/rtc.h"
#include "soc/io_mux_reg.h"
#include "hal/clk_tree_ll.h"
#include "hal/regi2c_ctrl_ll.h"
#include "hal/gpio_ll.h"
#include "esp_hw_log.h"
#include "esp_rom_sys.h"

static const char *TAG = "rtc_clk";

#define RTC_OSC_FREQ_RC8M   8

// Current PLL frequency, in 96MHZ. Zero if PLL is not enabled.
static int s_cur_pll_freq;

static uint32_t rtc_clk_ahb_freq_get(void);
static void rtc_clk_cpu_freq_to_xtal(int freq, int div);
void rtc_clk_cpu_freq_to_8m(void);

static void rtc_gpio_hangup(uint32_t gpio_no)
{
    gpio_ll_pulldown_dis(&GPIO, gpio_no);
    gpio_ll_pullup_dis(&GPIO, gpio_no);
    gpio_ll_output_disable(&GPIO, gpio_no);
    gpio_ll_input_disable(&GPIO, gpio_no);
    gpio_ll_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_no], PIN_FUNC_GPIO);
}

void rtc_clk_32k_enable(bool enable)
{
    if (enable) {
        /* need to hangup 32K_P and 32K_N pins before enable xtal_32k */
        rtc_gpio_hangup(XTAL32K_P_GPIO_NUM);
        rtc_gpio_hangup(XTAL32K_N_GPIO_NUM);
        clk_ll_xtal32k_enable(CLK_LL_XTAL32K_ENABLE_MODE_CRYSTAL);
    } else {
        clk_ll_xtal32k_disable();
    }
}

void rtc_clk_32k_enable_external(void)
{
    clk_ll_xtal32k_enable(CLK_LL_XTAL32K_ENABLE_MODE_EXTERNAL);
}

void rtc_clk_32k_bootstrap(uint32_t cycle)
{
    /* No special bootstrapping needed for ESP32-H4, 'cycle' argument is to keep the signature
     * same as for the ESP32. Just enable the XTAL here.
     */
    (void)cycle;
    rtc_clk_32k_enable(true);
}

bool rtc_clk_32k_enabled(void)
{
    return clk_ll_xtal32k_is_enabled();
}

void rtc_clk_rc32k_enable(bool enable)
{
    if (enable) {
        clk_ll_rc32k_enable();
    } else {
        clk_ll_rc32k_disable();
    }
}

void rtc_clk_slow_src_set(soc_rtc_slow_clk_src_t clk_src)
{
    clk_ll_rtc_slow_set_src(clk_src);
    rtc_clk_32k_enable((clk_src == SOC_RTC_SLOW_CLK_SRC_XTAL32K) ? 1 : 0);
    rtc_clk_rc32k_enable((clk_src == SOC_RTC_SLOW_CLK_SRC_RC32K) ? 1 : 0);
    esp_rom_delay_us(SOC_DELAY_RTC_SLOW_CLK_SWITCH);
}

soc_rtc_slow_clk_src_t rtc_clk_slow_src_get(void)
{
    return clk_ll_rtc_slow_get_src();
}

uint32_t rtc_clk_slow_freq_get_hz(void)
{
    switch (rtc_clk_slow_src_get()) {
    case SOC_RTC_SLOW_CLK_SRC_RC_SLOW: return SOC_CLK_RC_SLOW_FREQ_APPROX;
    case SOC_RTC_SLOW_CLK_SRC_XTAL32K: return SOC_CLK_XTAL32K_FREQ_APPROX;
    case SOC_RTC_SLOW_CLK_SRC_RC32K: return SOC_CLK_RC32K_FREQ_APPROX;
    default: return 0;
    }
}

void rtc_clk_fast_src_set(soc_rtc_fast_clk_src_t clk_src)
{
    clk_ll_rtc_fast_set_src(clk_src);
    esp_rom_delay_us(SOC_DELAY_RTC_FAST_CLK_SWITCH);
}

soc_rtc_fast_clk_src_t rtc_clk_fast_src_get(void)
{
    return clk_ll_rtc_fast_get_src();
}

static void rtc_clk_bbpll_disable(void)
{
    clk_ll_bbpll_disable();
    s_cur_pll_freq = 0;
}

static void rtc_clk_bbpll_enable(void)
{
    clk_ll_bbpll_enable();
}

static void rtc_clk_bbpll_configure(rtc_xtal_freq_t xtal_freq, int pll_freq)
{
    if (!((pll_freq == CLK_LL_PLL_96M_FREQ_MHZ) && (xtal_freq == RTC_XTAL_FREQ_32M))) {
        ESP_HW_LOGE(TAG, "invalid pll or xtal frequency");
    }

    // No digital part needs to be set, pll_freq can only be 96MHz

    // Configure analog part
    regi2c_ctrl_ll_bbpll_calibration_start();
    clk_ll_bbpll_set_config(pll_freq, xtal_freq);
    // Wait until calibration finishes
    while (!regi2c_ctrl_ll_bbpll_calibration_is_done());
    // Prevent BBPLL clock jitter
    regi2c_ctrl_ll_bbpll_calibration_stop();
    s_cur_pll_freq = pll_freq;
}

uint32_t rtc_clk_select_root_clk(soc_cpu_clk_src_t cpu_clk_src)
{
    uint32_t root_clk_freq_mhz;
    switch (cpu_clk_src) {
    case SOC_CPU_CLK_SRC_XTAL:
        root_clk_freq_mhz = RTC_XTAL_FREQ_32M;
        rtc_clk_bbpll_disable();
        break;
    case SOC_CPU_CLK_SRC_PLL:
        // SPLL_ENABLE
        root_clk_freq_mhz = CLK_LL_PLL_96M_FREQ_MHZ;
        rtc_clk_bbpll_enable();
        rtc_clk_bbpll_configure(RTC_XTAL_FREQ_32M, root_clk_freq_mhz);
        break;
    case SOC_CPU_CLK_SRC_RC_FAST:
        root_clk_freq_mhz = RTC_OSC_FREQ_RC8M;
        rtc_dig_clk8m_enable(); // TODO: Do we need to enable digital gating here?
        rtc_clk_8m_divider_set(1);
        rtc_clk_bbpll_disable();
        break;
    case SOC_CPU_CLK_SRC_XTAL_D2:
        root_clk_freq_mhz = RTC_XTAL_FREQ_32M / 2;
        rtc_clk_bbpll_disable();
        break;
    default:
        ESP_HW_LOGE(TAG, "unsupported source clk configuration");
        // fallback to XTAL as root clock source
        root_clk_freq_mhz = RTC_XTAL_FREQ_32M;
        rtc_clk_bbpll_disable();
        cpu_clk_src = SOC_CPU_CLK_SRC_XTAL;
        break;
    }
    clk_ll_cpu_set_src(cpu_clk_src);
    return root_clk_freq_mhz;
}

/**
 * Switch to one of PLL-based frequencies. Current frequency can be XTAL or PLL.
 * PLL must already be enabled.
 * @param cpu_freq new CPU frequency
 */
static void rtc_clk_cpu_freq_to_pll_mhz(int cpu_freq_mhz)
{
    int div = 1;
    if (CLK_LL_PLL_96M_FREQ_MHZ % cpu_freq_mhz == 0) {
        div = CLK_LL_PLL_96M_FREQ_MHZ / cpu_freq_mhz;
    } else {
        ESP_HW_LOGE(TAG, "invalid frequency");
        abort();
    }
    rtc_clk_cpu_freq_set(SOC_CPU_CLK_SRC_PLL, div);
    if (cpu_freq_mhz > RTC_XTAL_FREQ_32M) {
        clk_ll_ahb_set_divider(2);
    } else {
        clk_ll_ahb_set_divider(1);
    }

    ets_update_cpu_frequency(cpu_freq_mhz);
    rtc_clk_apb_freq_update(rtc_clk_apb_freq_get());
}

bool rtc_clk_cpu_freq_mhz_to_config(uint32_t freq_mhz, rtc_cpu_freq_config_t *out_config)
{
    // TODO: This function supposes to only fill a cpu config, but it is writing to registers, needs to check
    uint32_t source_freq_mhz;
    soc_cpu_clk_src_t source;
    uint32_t divider;
    uint32_t xtal_freq = (uint32_t)rtc_clk_xtal_freq_get();
    if (freq_mhz > xtal_freq) {
        source = SOC_CPU_CLK_SRC_PLL;
        source_freq_mhz = CLK_LL_PLL_96M_FREQ_MHZ;
        divider = CLK_LL_PLL_96M_FREQ_MHZ / freq_mhz;
        clk_ll_ahb_set_divider(2);
    } else if (freq_mhz != 0) {
        source = clk_ll_cpu_get_src();
        source_freq_mhz = rtc_clk_select_root_clk(source);
        divider = source_freq_mhz / freq_mhz;
        clk_ll_ahb_set_divider(1);
    } else {
        // unsupported frequency
        return false;
    }
    *out_config = (rtc_cpu_freq_config_t) {
        .source = source,
        .div = divider,
        .source_freq_mhz = source_freq_mhz,
        .freq_mhz = freq_mhz
    };
    return true;
}

void rtc_clk_cpu_freq_set_config(const rtc_cpu_freq_config_t *config)
{
    uint32_t src_freq_mhz = rtc_clk_select_root_clk(config->source);
    uint32_t div = src_freq_mhz / (config->freq_mhz);
    rtc_clk_cpu_freq_set(config->source, div);
    ets_update_cpu_frequency(config->freq_mhz);
}

void rtc_clk_cpu_freq_get_config(rtc_cpu_freq_config_t *out_config)
{
    soc_cpu_clk_src_t source = clk_ll_cpu_get_src();
    uint32_t source_freq_mhz;
    uint32_t div;
    uint32_t freq_mhz;
    switch (source) {
    case SOC_CPU_CLK_SRC_XTAL: {
        div = clk_ll_cpu_get_divider();
        source_freq_mhz = (uint32_t)rtc_clk_xtal_freq_get();
        freq_mhz = source_freq_mhz / div;
        break;
    }
    case SOC_CPU_CLK_SRC_PLL: {
        div = clk_ll_cpu_get_divider();
        source_freq_mhz = CLK_LL_PLL_96M_FREQ_MHZ;
        freq_mhz = source_freq_mhz / div;
        break;
    }
    case SOC_CPU_CLK_SRC_RC_FAST: {
        source_freq_mhz = RTC_OSC_FREQ_RC8M;
        div = clk_ll_cpu_get_divider();
        freq_mhz = source_freq_mhz / div;
        break;
    }
    case SOC_CPU_CLK_SRC_XTAL_D2: {
        div = clk_ll_cpu_get_divider();
        source_freq_mhz = (uint32_t)rtc_clk_xtal_freq_get();
        freq_mhz = source_freq_mhz / div / 2;
        break;
    }
    default: {
        ESP_HW_LOGE(TAG, "unsupported frequency configuration");
        abort();
    }
    }
    *out_config = (rtc_cpu_freq_config_t) {
        .source = source,
        .source_freq_mhz = source_freq_mhz,
        .div = div,
        .freq_mhz = freq_mhz
    };
}

void rtc_clk_cpu_freq_set_config_fast(const rtc_cpu_freq_config_t *config)
{
    if (config->source == SOC_CPU_CLK_SRC_XTAL) {
        rtc_clk_cpu_freq_to_xtal(config->freq_mhz, config->div);
    } else if (config->source == SOC_CPU_CLK_SRC_PLL &&
               s_cur_pll_freq == config->source_freq_mhz) {
        rtc_clk_cpu_freq_to_pll_mhz(config->freq_mhz);
    } else {
        /* fallback */
        rtc_clk_cpu_freq_set_config(config);
    }
}

void rtc_clk_cpu_freq_set_xtal(void)
{
    int freq_mhz = (int)rtc_clk_xtal_freq_get();

    rtc_clk_cpu_freq_to_xtal(freq_mhz, 1);
    rtc_clk_bbpll_disable();
}

/**
 * Switch to XTAL frequency. Does not disable the PLL.
 */
static void rtc_clk_cpu_freq_to_xtal(int freq, int div)
{
    ets_update_cpu_frequency(freq);
    /* Set divider from XTAL to APB clock. Need to set divider to 1 (reg. value 0) first. */
    rtc_clk_cpu_freq_set(SOC_CPU_CLK_SRC_XTAL, div);
    /* no need to adjust the REF_TICK */
    /* switch clock source */
    rtc_clk_apb_freq_update(rtc_clk_apb_freq_get());
}

void rtc_clk_cpu_freq_to_8m(void)
{
    ets_update_cpu_frequency(RTC_OSC_FREQ_RC8M);
    rtc_clk_select_root_clk(SOC_CPU_CLK_SRC_RC_FAST);
    rtc_clk_apb_freq_update(rtc_clk_apb_freq_get());
}

rtc_xtal_freq_t rtc_clk_xtal_freq_get(void)
{
    uint32_t xtal_freq_mhz = clk_ll_xtal_load_freq_mhz();
    if (xtal_freq_mhz == 0) {
        ESP_HW_LOGW(TAG, "invalid RTC_XTAL_FREQ_REG value, assume 32MHz");
        return RTC_XTAL_FREQ_32M;
    }
    return (rtc_xtal_freq_t)xtal_freq_mhz;
}

void rtc_clk_xtal_freq_update(rtc_xtal_freq_t xtal_freq)
{
    clk_ll_xtal_store_freq_mhz(xtal_freq);
}

void rtc_clk_apb_freq_update(uint32_t apb_freq)
{
    clk_ll_apb_store_freq_hz(apb_freq);
}

uint32_t rtc_clk_apb_freq_get(void)
{
    return rtc_clk_ahb_freq_get() / clk_ll_apb_get_divider();
}

static uint32_t rtc_clk_ahb_freq_get(void)
{
    rtc_cpu_freq_config_t cpu_config;
    rtc_clk_cpu_freq_get_config(&cpu_config);
    return cpu_config.freq_mhz * MHZ / clk_ll_ahb_get_divider();
}

void rtc_clk_cpu_freq_set(uint32_t source, uint32_t div)
{
    if ((uint32_t)clk_ll_cpu_get_src() != source) {
        rtc_clk_select_root_clk(source);
    }
    clk_ll_cpu_set_divider(div);
}

void rtc_clk_divider_set(uint32_t div)
{
    clk_ll_rc_slow_set_divider(div);
}

void rtc_clk_8m_divider_set(uint32_t div)
{
    clk_ll_rc_fast_set_divider(div);
}

void rtc_dig_clk8m_enable(void)
{
    clk_ll_rc_fast_digi_enable();
    esp_rom_delay_us(SOC_DELAY_RC_FAST_DIGI_SWITCH);
}

void rtc_dig_clk8m_disable(void)
{
    clk_ll_rc_fast_digi_disable();
    esp_rom_delay_us(SOC_DELAY_RC_FAST_DIGI_SWITCH);
}

bool rtc_dig_8m_enabled(void)
{
    return clk_ll_rc_fast_digi_is_enabled();
}

/* Name used in libphy.a:phy_chip_v7.o
 * TODO: update the library to use rtc_clk_xtal_freq_get
 */
rtc_xtal_freq_t rtc_get_xtal(void) __attribute__((alias("rtc_clk_xtal_freq_get")));

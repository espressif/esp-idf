/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "esp32c61/rom/rtc.h"
#include "soc/rtc.h"
#include "esp_private/rtc_clk.h"
#include "esp_hw_log.h"
#include "esp_rom_sys.h"
#include "hal/clk_tree_ll.h"
#include "hal/regi2c_ctrl_ll.h"
#include "hal/gpio_ll.h"
#include "soc/lp_aon_reg.h"
#include "esp_private/sleep_event.h"

#if SOC_MODEM_CLOCK_SUPPORTED
#include "esp_private/esp_modem_clock.h"
#endif

static const char *TAG = "rtc_clk";

// Current PLL frequency, in 480MHz. Zero if PLL is not enabled.
static int s_cur_pll_freq;

static uint32_t s_bbpll_digi_consumers_ref_count = 0; // Currently, it only tracks whether the 48MHz PHY clock is in-use by USB Serial/JTAG

void rtc_clk_bbpll_add_consumer(void)
{
    s_bbpll_digi_consumers_ref_count += 1;
}

void rtc_clk_bbpll_remove_consumer(void)
{
    s_bbpll_digi_consumers_ref_count -= 1;
}

void rtc_clk_32k_enable(bool enable)
{
    if (enable) {
        clk_ll_xtal32k_enable(CLK_LL_XTAL32K_ENABLE_MODE_CRYSTAL);
    } else {
        clk_ll_xtal32k_disable();
    }
}

void rtc_clk_32k_enable_external(void)
{
    // EXT_OSC_SLOW_GPIO_NUM == GPIO_NUM_0
    gpio_ll_input_enable(&GPIO, EXT_OSC_SLOW_GPIO_NUM);
    REG_SET_BIT(LP_AON_GPIO_HOLD0_REG, BIT(EXT_OSC_SLOW_GPIO_NUM));
    clk_ll_xtal32k_enable(CLK_LL_XTAL32K_ENABLE_MODE_EXTERNAL);
}

void rtc_clk_32k_bootstrap(uint32_t cycle)
{
    /* No special bootstrapping needed for ESP32-C61, 'cycle' argument is to keep the signature
     * same as for the ESP32. Just enable the XTAL here.
     */
    (void)cycle;
    rtc_clk_32k_enable(true);
}

bool rtc_clk_32k_enabled(void)
{
    return clk_ll_xtal32k_is_enabled();
}

void rtc_clk_8m_enable(bool clk_8m_en)
{
    if (clk_8m_en) {
        clk_ll_rc_fast_enable();
        esp_rom_delay_us(SOC_DELAY_RC_FAST_ENABLE);
    } else {
        clk_ll_rc_fast_disable();
    }
}

bool rtc_clk_8m_enabled(void)
{
    return clk_ll_rc_fast_is_enabled();
}

void rtc_clk_slow_src_set(soc_rtc_slow_clk_src_t clk_src)
{
    clk_ll_rtc_slow_set_src(clk_src);
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
    case SOC_RTC_SLOW_CLK_SRC_OSC_SLOW: return SOC_CLK_OSC_SLOW_FREQ_APPROX;
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

static void rtc_clk_enable_i2c_ana_master_clock(bool enable)
{
#if SOC_MODEM_CLOCK_SUPPORTED
#ifdef BOOTLOADER_BUILD
    regi2c_ctrl_ll_master_enable_clock(enable);
#else
    if (enable) {
        modem_clock_module_enable(PERIPH_ANA_I2C_MASTER_MODULE);
    } else {
        modem_clock_module_disable(PERIPH_ANA_I2C_MASTER_MODULE);
    }
#endif
#endif  //SOC_MODEM_CLOCK_SUPPORTED
}

static void rtc_clk_bbpll_configure(soc_xtal_freq_t xtal_freq, int pll_freq)
{
    /* Digital part */
    clk_ll_bbpll_set_freq_mhz(pll_freq);
    /* Analog part */
    rtc_clk_enable_i2c_ana_master_clock(true);
    /* BBPLL CALIBRATION START */
    regi2c_ctrl_ll_bbpll_calibration_start();
    clk_ll_bbpll_set_config(pll_freq, xtal_freq);
    /* WAIT CALIBRATION DONE */
    while(!regi2c_ctrl_ll_bbpll_calibration_is_done());
    esp_rom_delay_us(10); // wait for true stop
    /* BBPLL CALIBRATION STOP */
    regi2c_ctrl_ll_bbpll_calibration_stop();
    rtc_clk_enable_i2c_ana_master_clock(false);
    s_cur_pll_freq = pll_freq;
}

/**
 * Switch to use XTAL as the CPU clock source.
 * Must satisfy: cpu_freq = XTAL_FREQ / div.
 * Does not disable the PLL.
 */
static void rtc_clk_cpu_freq_to_xtal(int cpu_freq, int div)
{
    // let f_cpu = f_ahb
    clk_ll_cpu_set_divider(div);
    clk_ll_ahb_set_divider(div);
    clk_ll_cpu_set_src(SOC_CPU_CLK_SRC_XTAL);
    clk_ll_bus_update();
    esp_rom_set_cpu_ticks_per_us(cpu_freq);
}

static void rtc_clk_cpu_freq_to_8m(void)
{
    clk_ll_cpu_set_divider(1);
    clk_ll_ahb_set_divider(1);
    clk_ll_cpu_set_src(SOC_CPU_CLK_SRC_RC_FAST);
    clk_ll_bus_update();
    esp_rom_set_cpu_ticks_per_us(20);
}

/**
 * Switch to PLL_F160M as cpu clock source.
 * PLL must already be enabled.
 * @param cpu_freq new CPU frequency
 */
static void rtc_clk_cpu_freq_to_pll_160_mhz(int cpu_freq_mhz)
{
    // f_hp_root = 160MHz
    uint32_t cpu_divider = CLK_LL_PLL_160M_FREQ_MHZ / cpu_freq_mhz;
    clk_ll_cpu_set_divider(cpu_divider);
    // Constraint: f_ahb <= 40MHz; f_cpu = N * f_ahb (N = 1, 2, 3...)
    // let f_ahb = 40MHz
    const uint32_t ahb_divider = 4;
    assert((cpu_divider <= ahb_divider) && (ahb_divider % cpu_divider == 0));
    clk_ll_ahb_set_divider(ahb_divider);
    clk_ll_cpu_set_src(SOC_CPU_CLK_SRC_PLL_F160M);
    clk_ll_bus_update();
    esp_rom_set_cpu_ticks_per_us(cpu_freq_mhz);
}

bool rtc_clk_cpu_freq_mhz_to_config(uint32_t freq_mhz, rtc_cpu_freq_config_t *out_config)
{
    uint32_t source_freq_mhz;
    soc_cpu_clk_src_t source;
    uint32_t divider; // divider = freq of SOC_ROOT_CLK / freq of CPU_CLK
    uint32_t real_freq_mhz;

    uint32_t xtal_freq = (uint32_t)rtc_clk_xtal_freq_get();
    if (freq_mhz <= xtal_freq && freq_mhz != 0) {
        divider = xtal_freq / freq_mhz;
        real_freq_mhz = (xtal_freq + divider / 2) / divider; /* round */
        if (real_freq_mhz != freq_mhz) {
            // no suitable divider
            return false;
        }

        source_freq_mhz = xtal_freq;
        source = SOC_CPU_CLK_SRC_XTAL;
    } else if (freq_mhz == 80) {
        real_freq_mhz = freq_mhz;
        source = SOC_CPU_CLK_SRC_PLL_F160M;
        source_freq_mhz = CLK_LL_PLL_160M_FREQ_MHZ;
        divider = 2;
    } else if (freq_mhz == 160) {
        real_freq_mhz = freq_mhz;
        source = SOC_CPU_CLK_SRC_PLL_F160M;
        source_freq_mhz = CLK_LL_PLL_160M_FREQ_MHZ;
        divider = 1;
    } else {
        // unsupported frequency
        return false;
    }
    *out_config = (rtc_cpu_freq_config_t) {
        .source = source,
        .div = divider,
        .source_freq_mhz = source_freq_mhz,
        .freq_mhz = real_freq_mhz
    };
    return true;
}

__attribute__((weak)) void rtc_clk_set_cpu_switch_to_bbpll(int event_id)
{
}

void rtc_clk_cpu_freq_set_config(const rtc_cpu_freq_config_t *config)
{
    soc_cpu_clk_src_t old_cpu_clk_src = clk_ll_cpu_get_src();
    if (config->source == SOC_CPU_CLK_SRC_XTAL) {
        rtc_clk_cpu_freq_to_xtal(config->freq_mhz, config->div);
        if ((old_cpu_clk_src == SOC_CPU_CLK_SRC_PLL_F160M) && !s_bbpll_digi_consumers_ref_count) {
            // We don't turn off the bbpll if some consumers depend on bbpll
            rtc_clk_bbpll_disable();
        }
    } else if (config->source == SOC_CPU_CLK_SRC_PLL_F160M) {
        if (old_cpu_clk_src != SOC_CPU_CLK_SRC_PLL_F160M) {
            rtc_clk_set_cpu_switch_to_bbpll(SLEEP_EVENT_HW_PLL_EN_START);
            rtc_clk_bbpll_enable();
            rtc_clk_bbpll_configure(rtc_clk_xtal_freq_get(), CLK_LL_PLL_480M_FREQ_MHZ);
        }
        rtc_clk_cpu_freq_to_pll_160_mhz(config->freq_mhz);
        rtc_clk_set_cpu_switch_to_bbpll(SLEEP_EVENT_HW_PLL_EN_STOP);
    } else if (config->source == SOC_CPU_CLK_SRC_RC_FAST) {
        rtc_clk_cpu_freq_to_8m();
        if ((old_cpu_clk_src == SOC_CPU_CLK_SRC_PLL_F160M) && !s_bbpll_digi_consumers_ref_count) {
            // We don't turn off the bbpll if some consumers depend on bbpll
            rtc_clk_bbpll_disable();
        }
    }
}

void rtc_clk_cpu_freq_get_config(rtc_cpu_freq_config_t *out_config)
{
    soc_cpu_clk_src_t source = clk_ll_cpu_get_src();
    uint32_t source_freq_mhz;
    switch (source) {
    case SOC_CPU_CLK_SRC_XTAL: {
        source_freq_mhz = (uint32_t)rtc_clk_xtal_freq_get();
        break;
    }
    case SOC_CPU_CLK_SRC_PLL_F160M: {
        source_freq_mhz = CLK_LL_PLL_160M_FREQ_MHZ;
        break;
    }
    case SOC_CPU_CLK_SRC_RC_FAST:
        source_freq_mhz = 20;
        break;
    default:
        ESP_HW_LOGE(TAG, "unsupported frequency configuration");
        abort();
    }
    uint32_t div = clk_ll_cpu_get_divider();
    uint32_t freq_mhz = source_freq_mhz / div; // freq of CPU_CLK = freq of SOC_ROOT_CLK / cpu_div
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
    } else if (config->source == SOC_CPU_CLK_SRC_PLL_F160M &&
               s_cur_pll_freq == CLK_LL_PLL_480M_FREQ_MHZ) {
        rtc_clk_cpu_freq_to_pll_160_mhz(config->freq_mhz);
    } else if (config->source == SOC_CPU_CLK_SRC_RC_FAST) {
        rtc_clk_cpu_freq_to_8m();
    } else {
        /* fallback */
        rtc_clk_cpu_freq_set_config(config);
    }
}

void rtc_clk_cpu_freq_set_xtal(void)
{
    rtc_clk_cpu_set_to_default_config();
    rtc_clk_bbpll_disable();
}

void rtc_clk_cpu_set_to_default_config(void)
{
    int freq_mhz = (int)rtc_clk_xtal_freq_get();

    rtc_clk_cpu_freq_to_xtal(freq_mhz, 1);
    s_cur_pll_freq = 0; // no disable PLL, but set freq to 0 to trigger a PLL calibration after wake-up from sleep
}

void rtc_clk_cpu_freq_to_pll_and_pll_lock_release(int cpu_freq_mhz)
{
    rtc_clk_cpu_freq_to_pll_160_mhz(cpu_freq_mhz);
    clk_ll_cpu_clk_src_lock_release();
}

soc_xtal_freq_t rtc_clk_xtal_freq_get(void)
{
    uint32_t xtal_freq_mhz = clk_ll_xtal_get_freq_mhz();
    assert(xtal_freq_mhz == SOC_XTAL_FREQ_40M);
    return (soc_xtal_freq_t)xtal_freq_mhz;
}

static uint32_t rtc_clk_ahb_freq_get(void)
{
    soc_cpu_clk_src_t source = clk_ll_cpu_get_src();
    uint32_t soc_root_freq_mhz;
    switch (source) {
    case SOC_CPU_CLK_SRC_XTAL:
        soc_root_freq_mhz = rtc_clk_xtal_freq_get();
        break;
    case SOC_CPU_CLK_SRC_PLL_F160M:
        soc_root_freq_mhz = CLK_LL_PLL_160M_FREQ_MHZ;
        break;
    case SOC_CPU_CLK_SRC_RC_FAST:
        soc_root_freq_mhz = 20;
        break;
    default:
        // Unknown SOC_ROOT clock source
        soc_root_freq_mhz = 0;
        ESP_HW_LOGE(TAG, "Invalid SOC_ROOT_CLK");
        break;
    }
    return soc_root_freq_mhz / clk_ll_ahb_get_divider();
}

uint32_t rtc_clk_apb_freq_get(void)
{
    return rtc_clk_ahb_freq_get() / clk_ll_apb_get_divider() * MHZ;
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

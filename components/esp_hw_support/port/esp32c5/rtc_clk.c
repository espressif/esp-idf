/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "esp32c5/rom/rtc.h"
#include "soc/rtc.h"
#include "esp_private/rtc_clk.h"
#include "esp_hw_log.h"
#include "esp_rom_sys.h"
#include "hal/clk_tree_ll.h"
#include "hal/regi2c_ctrl_ll.h"
#include "soc/io_mux_reg.h"
#include "soc/lp_aon_reg.h"
#include "esp_private/sleep_event.h"

#ifdef BOOTLOADER_BUILD
#include "hal/modem_lpcon_ll.h"
#else
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
    PIN_INPUT_ENABLE(IO_MUX_GPIO0_REG);
    REG_SET_BIT(LP_AON_GPIO_HOLD0_REG, BIT(EXT_OSC_SLOW_GPIO_NUM));
    clk_ll_xtal32k_enable(CLK_LL_XTAL32K_ENABLE_MODE_EXTERNAL);
}

void rtc_clk_32k_bootstrap(uint32_t cycle)
{
    /* No special bootstrapping needed for ESP32-C5, 'cycle' argument is to keep the signature
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
        esp_rom_delay_us(SOC_DELAY_RC32K_ENABLE);
    } else {
        clk_ll_rc32k_disable();
    }
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
    // case SOC_RTC_SLOW_CLK_SRC_RC_SLOW: return SOC_CLK_RC_SLOW_FREQ_APPROX;
    case SOC_RTC_SLOW_CLK_SRC_XTAL32K: return SOC_CLK_XTAL32K_FREQ_APPROX;
    case SOC_RTC_SLOW_CLK_SRC_RC32K: return SOC_CLK_RC32K_FREQ_APPROX;
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
#ifdef BOOTLOADER_BUILD
    modem_lpcon_ll_enable_i2c_master_clock(&MODEM_LPCON, enable);
#else
    if (enable) {
        modem_clock_module_enable(PERIPH_ANA_I2C_MASTER_MODULE);
    } else {
        modem_clock_module_disable(PERIPH_ANA_I2C_MASTER_MODULE);
    }
#endif
}

static void rtc_clk_bbpll_configure(soc_xtal_freq_t xtal_freq, int pll_freq)
{
    assert((pll_freq == CLK_LL_PLL_160M_FREQ_MHZ) || \
            (pll_freq == CLK_LL_PLL_240M_FREQ_MHZ));
    /* Digital part */
    clk_ll_bbpll_set_freq_mhz(CLK_LL_PLL_480M_FREQ_MHZ);
    /* Analog part */
    rtc_clk_enable_i2c_ana_master_clock(true);
    /* BBPLL CALIBRATION START */
    regi2c_ctrl_ll_bbpll_calibration_start();
    clk_ll_bbpll_set_config(CLK_LL_PLL_480M_FREQ_MHZ, xtal_freq);
    /* WAIT CALIBRATION DONE */
    while(!regi2c_ctrl_ll_bbpll_calibration_is_done());
    /* BBPLL CALIBRATION STOP */
    regi2c_ctrl_ll_bbpll_calibration_stop();
    rtc_clk_enable_i2c_ana_master_clock(false);
    s_cur_pll_freq = CLK_LL_PLL_480M_FREQ_MHZ;
}

/**
 * Switch to use XTAL as the CPU clock source.
 * Must satisfy: cpu_freq = XTAL_FREQ / div.
 * Does not disable the PLL.
 */
static void rtc_clk_cpu_freq_to_xtal(int cpu_freq, int div)
{
    /* Configure clk mspi fast to XTAL*/
    clk_ll_mspi_fast_sel_clk(SOC_MOD_CLK_XTAL);
    clk_ll_mspi_fast_set_divider(1);

    clk_ll_cpu_set_src(SOC_CPU_CLK_SRC_XTAL);
    clk_ll_ahb_set_divider(div);
    clk_ll_cpu_set_divider(div);
    esp_rom_set_cpu_ticks_per_us(cpu_freq);
}

static void rtc_clk_cpu_freq_to_8m(void)
{
    /* Configure clk mspi fast to XTAL*/
    clk_ll_mspi_fast_sel_clk(SOC_MOD_CLK_XTAL);
    clk_ll_mspi_fast_set_divider(1);

    clk_ll_ahb_set_divider(1);
    clk_ll_cpu_set_divider(1);
    clk_ll_cpu_set_src(SOC_CPU_CLK_SRC_RC_FAST);
    esp_rom_set_cpu_ticks_per_us(20);
}

/**
 * Switch to one of PLL-based frequencies. Current frequency can be XTAL or PLL.
 * PLL must already be enabled.
 * @param cpu_freq new CPU frequency
 */
static void rtc_clk_cpu_freq_to_pll_mhz(int cpu_freq_mhz)
{
    rtc_cpu_freq_config_t cfg;
    rtc_clk_cpu_freq_mhz_to_config(cpu_freq_mhz, &cfg);
    // Set AHB always be 40MHz
    clk_ll_ahb_set_divider(cfg.source_freq_mhz / 40);
    clk_ll_cpu_set_divider(cfg.div);
    clk_ll_cpu_set_src(cfg.source);
    esp_rom_set_cpu_ticks_per_us(cpu_freq_mhz);

    /* Configure clk mspi fast to 80m*/
    clk_ll_mspi_fast_set_divider(6);
    clk_ll_mspi_fast_sel_clk(MSPI_CLK_SRC_SPLL);
}

bool rtc_clk_cpu_freq_mhz_to_config(uint32_t freq_mhz, rtc_cpu_freq_config_t *out_config)
{
    uint32_t source_freq_mhz;
    soc_cpu_clk_src_t source;
    uint32_t divider; // divider = freq of SOC_ROOT_CLK / freq of CPU_CLK
    uint32_t real_freq_mhz;

    uint32_t xtal_freq = (uint32_t)rtc_clk_xtal_freq_get();
    if (freq_mhz == 48 || freq_mhz == 24 || freq_mhz == 16 || freq_mhz == 12 || freq_mhz == 8) {
        divider = xtal_freq / freq_mhz;
        source_freq_mhz = xtal_freq;
        source = SOC_CPU_CLK_SRC_XTAL;
        real_freq_mhz = freq_mhz;
    } else if (freq_mhz == 240 || freq_mhz == 120) {
        real_freq_mhz = freq_mhz;
        source = SOC_CPU_CLK_SRC_PLL_F240;
        source_freq_mhz = CLK_LL_PLL_240M_FREQ_MHZ;
        divider = CLK_LL_PLL_240M_FREQ_MHZ / freq_mhz;
    }else if (freq_mhz == 160 || freq_mhz == 80 || freq_mhz == 40) {
        real_freq_mhz = freq_mhz;
        source = SOC_CPU_CLK_SRC_PLL_F160;
        source_freq_mhz = CLK_LL_PLL_160M_FREQ_MHZ;
        divider = CLK_LL_PLL_160M_FREQ_MHZ / freq_mhz;
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
        /* Configure clk mspi fast to 80m*/
        rtc_clk_cpu_freq_to_xtal(config->freq_mhz, config->div);
        if (((old_cpu_clk_src == SOC_CPU_CLK_SRC_PLL_F160) || (old_cpu_clk_src == SOC_CPU_CLK_SRC_PLL_F240)) && !s_bbpll_digi_consumers_ref_count) {
            // We don't turn off the bbpll if some consumers depend on bbpll
            rtc_clk_bbpll_disable();
        }
    } else if ((config->source == SOC_CPU_CLK_SRC_PLL_F160) || (config->source == SOC_CPU_CLK_SRC_PLL_F240)) {
        if ((old_cpu_clk_src != SOC_CPU_CLK_SRC_PLL_F160) && (old_cpu_clk_src != SOC_CPU_CLK_SRC_PLL_F240)) {
            rtc_clk_set_cpu_switch_to_bbpll(SLEEP_EVENT_HW_BBPLL_EN_START);
            rtc_clk_bbpll_enable();
            rtc_clk_bbpll_configure(rtc_clk_xtal_freq_get(), config->source_freq_mhz);
        }
        rtc_clk_cpu_freq_to_pll_mhz(config->freq_mhz);
        rtc_clk_set_cpu_switch_to_bbpll(SLEEP_EVENT_HW_BBPLL_EN_STOP);
    } else if (config->source == SOC_CPU_CLK_SRC_RC_FAST) {
        rtc_clk_cpu_freq_to_8m();
        if (((old_cpu_clk_src == SOC_CPU_CLK_SRC_PLL_F160) || (old_cpu_clk_src == SOC_CPU_CLK_SRC_PLL_F240)) && !s_bbpll_digi_consumers_ref_count) {
            // We don't turn off the bbpll if some consumers depend on bbpll
            rtc_clk_bbpll_disable();
        }
    }
}

void rtc_clk_cpu_freq_get_config(rtc_cpu_freq_config_t *out_config)
{
    soc_cpu_clk_src_t source = clk_ll_cpu_get_src();
    uint32_t source_freq_mhz;
    uint32_t div; // div = freq of SOC_ROOT_CLK / freq of CPU_CLK
    uint32_t freq_mhz;
    switch (source) {
    case SOC_CPU_CLK_SRC_XTAL: {
        div = clk_ll_cpu_get_divider();
        source_freq_mhz = (uint32_t)rtc_clk_xtal_freq_get();
        freq_mhz = source_freq_mhz / div;
        break;
    }
    case SOC_CPU_CLK_SRC_PLL_F160: {
        div = clk_ll_cpu_get_divider();
        source_freq_mhz = CLK_LL_PLL_160M_FREQ_MHZ;
        freq_mhz = source_freq_mhz / div;
        break;
    }
    case SOC_CPU_CLK_SRC_PLL_F240: {
        div = clk_ll_cpu_get_divider();
        source_freq_mhz = CLK_LL_PLL_240M_FREQ_MHZ;
        freq_mhz = source_freq_mhz / div;
        break;
    }
    case SOC_CPU_CLK_SRC_RC_FAST:
        div = clk_ll_cpu_get_divider();
        source_freq_mhz = 20;
        freq_mhz = source_freq_mhz / div;
        break;
    default:
        ESP_HW_LOGE(TAG, "unsupported frequency configuration");
        abort();
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
    } else if (((config->source == SOC_CPU_CLK_SRC_PLL_F160) || (config->source == SOC_CPU_CLK_SRC_PLL_F240)) &&
               s_cur_pll_freq == config->source_freq_mhz) {
        rtc_clk_cpu_freq_to_pll_mhz(config->freq_mhz);
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
    // We don't turn off the bbpll if some consumers depend on bbpll
    if (!s_bbpll_digi_consumers_ref_count) {
        rtc_clk_bbpll_disable();
    }
}

void rtc_clk_cpu_set_to_default_config(void)
{
    int freq_mhz = (int)rtc_clk_xtal_freq_get();

    rtc_clk_cpu_freq_to_xtal(freq_mhz, 1);
}

void rtc_clk_cpu_freq_to_pll_and_pll_lock_release(int cpu_freq_mhz)
{
    rtc_clk_cpu_freq_to_pll_mhz(cpu_freq_mhz);
    clk_ll_cpu_clk_src_lock_release();
}

soc_xtal_freq_t rtc_clk_xtal_freq_get(void)
{
    return CONFIG_XTAL_FREQ;
}

void rtc_clk_xtal_freq_update(soc_xtal_freq_t xtal_freq)
{
    clk_ll_xtal_store_freq_mhz(xtal_freq);
}

static uint32_t rtc_clk_ahb_freq_get(void)
{
    soc_cpu_clk_src_t source = clk_ll_cpu_get_src();
    uint32_t soc_root_freq_mhz;
    uint32_t divider;
    switch (source) {
    case SOC_CPU_CLK_SRC_XTAL:
        soc_root_freq_mhz = rtc_clk_xtal_freq_get();
        divider = clk_ll_ahb_get_divider();
        break;
    case SOC_CPU_CLK_SRC_PLL_F160:
        soc_root_freq_mhz = CLK_LL_PLL_160M_FREQ_MHZ;
        divider = clk_ll_ahb_get_divider();
        break;
    case SOC_CPU_CLK_SRC_PLL_F240:
        soc_root_freq_mhz = CLK_LL_PLL_240M_FREQ_MHZ;
        divider = clk_ll_ahb_get_divider();
        break;
    case SOC_CPU_CLK_SRC_RC_FAST:
        soc_root_freq_mhz = 20;
        divider = clk_ll_ahb_get_divider();
        break;
    default:
        // Unknown SOC_ROOT clock source
        soc_root_freq_mhz = 0;
        divider = 1;
        ESP_HW_LOGE(TAG, "Invalid SOC_ROOT_CLK");
        break;
    }
    return soc_root_freq_mhz / divider;
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

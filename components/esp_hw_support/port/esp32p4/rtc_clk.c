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
#include "esp32p4/rom/rtc.h"
#include "soc/rtc.h"
#include "esp_private/rtc_clk.h"
#include "esp_hw_log.h"
#include "esp_rom_sys.h"
#include "hal/clk_tree_ll.h"
#include "hal/regi2c_ctrl_ll.h"
#include "hal/gpio_ll.h"
#include "soc/io_mux_reg.h"
#include "esp_private/sleep_event.h" // TODO: IDF-7528

static const char *TAG = "rtc_clk";

// CPLL frequency option, in 360/400MHz. Zero if CPLL is not enabled.
static int s_cur_cpll_freq = 0;

void rtc_clk_32k_enable(bool enable)
{
    if (enable) {
        clk_ll_xtal32k_enable(CLK_LL_XTAL32K_ENABLE_MODE_CRYSTAL);
    } else {
        clk_ll_xtal32k_disable();
    }
}

void rtc_clk_32k_bootstrap(uint32_t cycle)
{
    /* No special bootstrapping needed for ESP32-P4, 'cycle' argument is to keep the signature
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

void rtc_clk_lp_pll_enable(bool enable)
{
    if (enable) {
        clk_ll_lp_pll_enable();
        esp_rom_delay_us(SOC_DELAY_LP_PLL_ENABLE);
    } else {
        clk_ll_lp_pll_disable();
    }
}

void rtc_clk_lp_pll_src_set(soc_lp_pll_clk_src_t clk_src)
{
    clk_ll_lp_pll_set_src(clk_src);
    esp_rom_delay_us(SOC_DELAY_LP_PLL_SWITCH);
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

static void rtc_clk_cpll_disable(void)
{
    clk_ll_cpll_disable();
    s_cur_cpll_freq = 0;
}

static void rtc_clk_cpll_enable(void)
{
    clk_ll_cpll_enable();
}

static void rtc_clk_cpll_configure(soc_xtal_freq_t xtal_freq, int cpll_freq)
{
    /* Digital part */
    clk_ll_cpll_set_freq_mhz(cpll_freq);
    /* Analog part */
    /* CPLL CALIBRATION START */
    regi2c_ctrl_ll_cpll_calibration_start();
    clk_ll_cpll_set_config(cpll_freq, xtal_freq);
    /* WAIT CALIBRATION DONE */
    while(!regi2c_ctrl_ll_cpll_calibration_is_done());
    esp_rom_delay_us(10); // wait for true stop
    /* CPLL CALIBRATION STOP */
    regi2c_ctrl_ll_cpll_calibration_stop();

    s_cur_cpll_freq = cpll_freq;
}

/**
 * Switch to use XTAL as the CPU clock source.
 * Must satisfy: cpu_freq = XTAL_FREQ / div.
 * Does not disable the PLL.
 *
 * If to_default is set, then will configure CPU - MEM - SYS - APB frequencies back to power-on reset configuration (40 - 20 - 20 - 10)
 * If to_default is not set, then will configure to 40 - 40 - 40 - 40
 */
static void rtc_clk_cpu_freq_to_xtal(int cpu_freq, int div, bool to_default)
{
    // let f_cpu = f_mem = f_sys = f_apb
    uint32_t mem_divider = 1;
    uint32_t sys_divider = 1;
    uint32_t apb_divider = 1;
    if (to_default) {
        // f_cpu = 2 * f_mem = 2 * f_sys = 4 * f_apb
        mem_divider = 2;
        apb_divider = 2;
    }
    // Update bit does not control CPU clock sel mux. Therefore, there will be a middle state during the switch (CPU falls)
    // Since before the switch, the clock source is CPLL, there is divider value constraints.
    // Setting the new dividers first is unguaranteed (hardware could automatically modify the real dividers)
    // Therefore, we will switch cpu clock source first, and then set the desired dividers.
    clk_ll_cpu_set_src(SOC_CPU_CLK_SRC_XTAL);
    clk_ll_cpu_set_divider(div, 0, 0);
    clk_ll_mem_set_divider(mem_divider);
    clk_ll_sys_set_divider(sys_divider);
    clk_ll_apb_set_divider(apb_divider);
    clk_ll_bus_update();
    esp_rom_set_cpu_ticks_per_us(cpu_freq);
}

static void rtc_clk_cpu_freq_to_8m(void)
{
    // let f_cpu = f_mem = f_sys = f_apb
    clk_ll_cpu_set_divider(1, 0, 0);
    clk_ll_mem_set_divider(1);
    clk_ll_sys_set_divider(1);
    clk_ll_apb_set_divider(1);
    clk_ll_cpu_set_src(SOC_CPU_CLK_SRC_RC_FAST);
    clk_ll_bus_update();
    esp_rom_set_cpu_ticks_per_us(20);
}

/**
 * Switch to one of CPLL-based frequencies. Current frequency can be XTAL or CPLL.
 * CPLL must already be enabled.
 * @param cpu_freq new CPU frequency
 */
static void rtc_clk_cpu_freq_to_cpll_mhz(int cpu_freq_mhz, hal_utils_clk_div_t *div)
{
    // CPLL -> CPU_CLK -> MEM_CLK -> SYS_CLK -> APB_CLK
    // Constraint: MEM_CLK <= 200MHz, APB_CLK <= 100MHz
    // This implies that when clock source is CPLL,
    //                   If cpu_divider < 2, mem_divider must be larger or equal to 2
    //                   If cpu_divider < 2, mem_divider = 2, sys_divider < 2, apb_divider must be larger or equal to 2
    // Current available configurations:
    // 360  -   360   -    180    -    180   -    90
    // 360  -   180   -    180    -    180   -    90
    // 360  -   90    -    90     -    90    -    90
    uint32_t mem_divider = 1;
    uint32_t sys_divider = 1; // We are not going to change this
    uint32_t apb_divider = 1;
    switch (cpu_freq_mhz) {
    case 360:
        mem_divider = 2;
        apb_divider = 2;
        break;
    case 180:
        mem_divider = 1;
        apb_divider = 2;
        break;
    case 90:
        mem_divider = 1;
        apb_divider = 1;
        break;
    default:
        // Unsupported configuration
        // This is dangerous to modify dividers. Hardware could automatically correct the divider, and it won't be
        // reflected to the registers. Therefore, you won't even be able to calculate out the real mem_clk, apb_clk freq.
        // To avoid such case, we will strictly do abort here.
        abort();
    }
    // Update bit does not control CPU clock sel mux. Therefore, there may be a middle state during the switch (CPU rises)
    // We will switch cpu clock source first, and then set the desired dividers.
    // It is likely that the hardware will automatically adjust dividers to meet mem_clk, apb_clk freq constraints when
    // cpu clock source is set.
    // However, the desired dividers will be written into registers anyways afterwards.
    // This ensures the final confguration is the desired one.
    clk_ll_cpu_set_src(SOC_CPU_CLK_SRC_PLL);
    clk_ll_cpu_set_divider(div->integer, div->numerator, div->denominator);
    clk_ll_mem_set_divider(mem_divider);
    clk_ll_sys_set_divider(sys_divider);
    clk_ll_apb_set_divider(apb_divider);
    clk_ll_bus_update();
    esp_rom_set_cpu_ticks_per_us(cpu_freq_mhz);
}

bool rtc_clk_cpu_freq_mhz_to_config(uint32_t freq_mhz, rtc_cpu_freq_config_t *out_config)
{
    uint32_t source_freq_mhz;
    soc_cpu_clk_src_t source;
    hal_utils_clk_div_t divider = {0}; // divider = freq of HP_ROOT_CLK / freq of CPU_CLK
    uint32_t real_freq_mhz;

    // Keep default CPLL at 360MHz
    uint32_t xtal_freq = (uint32_t)rtc_clk_xtal_freq_get();
    if (freq_mhz <= xtal_freq && freq_mhz != 0) {
        divider.integer = xtal_freq / freq_mhz;
        real_freq_mhz = (xtal_freq + divider.integer / 2) / divider.integer; /* round */
        if (real_freq_mhz != freq_mhz) {
            // no suitable divider
            return false;
        }

        source_freq_mhz = xtal_freq;
        source = SOC_CPU_CLK_SRC_XTAL;
    } else if (freq_mhz == 90) {
        real_freq_mhz = freq_mhz;
        source = SOC_CPU_CLK_SRC_CPLL;
        source_freq_mhz = CLK_LL_PLL_360M_FREQ_MHZ;
        divider.integer = 4;
    } else if (freq_mhz == 180) {
        real_freq_mhz = freq_mhz;
        source = SOC_CPU_CLK_SRC_CPLL;
        source_freq_mhz = CLK_LL_PLL_360M_FREQ_MHZ;
        divider.integer = 2;
    } else if (freq_mhz == 360) {
        real_freq_mhz = freq_mhz;
        source = SOC_CPU_CLK_SRC_CPLL;
        source_freq_mhz = CLK_LL_PLL_360M_FREQ_MHZ;
        divider.integer = 1;
    } else if (freq_mhz == 400) {
        // If CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ selects 400MHz, then at app startup stage will need a CPLL calibration to raise its freq from 360MHz to 400MHz
        real_freq_mhz = freq_mhz;
        source = SOC_CPU_CLK_SRC_CPLL;
        source_freq_mhz = CLK_LL_PLL_400M_FREQ_MHZ;
        divider.integer = 1;
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

void rtc_clk_cpu_freq_set_config(const rtc_cpu_freq_config_t *config)
{
    soc_cpu_clk_src_t old_cpu_clk_src = clk_ll_cpu_get_src();
    if (config->source == SOC_CPU_CLK_SRC_XTAL) {
        rtc_clk_cpu_freq_to_xtal(config->freq_mhz, config->div.integer, false);
        if (old_cpu_clk_src == SOC_CPU_CLK_SRC_CPLL) {
            rtc_clk_cpll_disable();
        }
    } else if (config->source == SOC_CPU_CLK_SRC_CPLL) {
        if (old_cpu_clk_src != SOC_CPU_CLK_SRC_CPLL) {
            rtc_clk_cpll_enable();
        }
        if (config->source_freq_mhz != s_cur_cpll_freq) {
            soc_xtal_freq_t xtal_freq_mhz = rtc_clk_xtal_freq_get();
            // Calibrate CPLL freq to a new value requires to switch CPU clock source to XTAL first
            rtc_clk_cpu_freq_to_xtal((uint32_t)xtal_freq_mhz, 1, false);
            rtc_clk_cpll_configure(xtal_freq_mhz, config->source_freq_mhz);
        }
        rtc_clk_cpu_freq_to_cpll_mhz(config->freq_mhz, (hal_utils_clk_div_t *)&config->div);
    } else if (config->source == SOC_CPU_CLK_SRC_RC_FAST) {
        rtc_clk_cpu_freq_to_8m();
        if (old_cpu_clk_src == SOC_CPU_CLK_SRC_CPLL) {
            rtc_clk_cpll_disable();
        }
    }
}

void rtc_clk_cpu_freq_get_config(rtc_cpu_freq_config_t *out_config)
{
    soc_cpu_clk_src_t source = clk_ll_cpu_get_src();
    uint32_t source_freq_mhz;
    hal_utils_clk_div_t div = {0}; // div = freq of SOC_ROOT_CLK / freq of CPU_CLK
    uint32_t freq_mhz;
    clk_ll_cpu_get_divider(&div.integer, &div.numerator, &div.denominator);
    if (div.denominator == 0) {
        div.denominator = 1;
        div.numerator = 0;
    }
    switch (source) {
    case SOC_CPU_CLK_SRC_XTAL: {
        source_freq_mhz = (uint32_t)rtc_clk_xtal_freq_get();
        break;
    }
    case SOC_CPU_CLK_SRC_CPLL: {
        source_freq_mhz = clk_ll_cpll_get_freq_mhz((uint32_t)rtc_clk_xtal_freq_get());
        break;
    }
    case SOC_CPU_CLK_SRC_RC_FAST:
        source_freq_mhz = 20;
        break;
    default:
        ESP_HW_LOGE(TAG, "unsupported frequency configuration");
        abort();
    }
    freq_mhz = source_freq_mhz * div.denominator / (div.integer * div.denominator + div.numerator);
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
        rtc_clk_cpu_freq_to_xtal(config->freq_mhz, config->div.integer, false);
    } else if (config->source == SOC_CPU_CLK_SRC_CPLL &&
               s_cur_cpll_freq == config->source_freq_mhz) {
        rtc_clk_cpu_freq_to_cpll_mhz(config->freq_mhz, (hal_utils_clk_div_t *)&config->div);
    } else if (config->source == SOC_CPU_CLK_SRC_RC_FAST) {
        rtc_clk_cpu_freq_to_8m();
    } else {
        /* fallback */
        rtc_clk_cpu_freq_set_config(config);
    }
}

void rtc_clk_cpu_freq_set_xtal(void)
{
    int freq_mhz = (int)rtc_clk_xtal_freq_get();

    rtc_clk_cpu_freq_to_xtal(freq_mhz, 1, false);
    rtc_clk_cpll_disable();
}

void rtc_clk_cpu_set_to_default_config(void)
{
    int freq_mhz = (int)rtc_clk_xtal_freq_get();

    rtc_clk_cpu_freq_to_xtal(freq_mhz, 1, true);
}

soc_xtal_freq_t rtc_clk_xtal_freq_get(void)
{
    uint32_t xtal_freq_mhz = clk_ll_xtal_load_freq_mhz();
    if (xtal_freq_mhz == 0) {
        ESP_HW_LOGW(TAG, "invalid RTC_XTAL_FREQ_REG value, assume 40MHz");
        return SOC_XTAL_FREQ_40M;
    }
    return (soc_xtal_freq_t)xtal_freq_mhz;
}

void rtc_clk_xtal_freq_update(soc_xtal_freq_t xtal_freq)
{
    clk_ll_xtal_store_freq_mhz(xtal_freq);
}

uint32_t rtc_clk_apb_freq_get(void)
{
    soc_cpu_clk_src_t source = clk_ll_cpu_get_src();
    uint32_t source_freq_mhz;
    switch (source) {
    case SOC_CPU_CLK_SRC_XTAL:
        source_freq_mhz = (uint32_t)rtc_clk_xtal_freq_get();
        break;
    case SOC_CPU_CLK_SRC_CPLL:
        source_freq_mhz = clk_ll_cpll_get_freq_mhz((uint32_t)rtc_clk_xtal_freq_get());
        break;
    case SOC_CPU_CLK_SRC_RC_FAST:
        source_freq_mhz = 20;
        break;
    default:
        // Unknown HP_ROOT clock source
        source_freq_mhz = 0;
        ESP_HW_LOGE(TAG, "Invalid HP_ROOT_CLK");
        break;
    }
    uint32_t integer, numerator, denominator;
    clk_ll_cpu_get_divider(&integer, &numerator, &denominator);
    if (denominator == 0) {
        denominator = 1;
        numerator = 0;
    }
    uint32_t cpu_freq_hz = source_freq_mhz * MHZ * denominator / (integer * denominator + numerator);
    uint32_t mem_freq_hz = cpu_freq_hz / clk_ll_mem_get_divider();
    uint32_t sys_freq_hz = mem_freq_hz / clk_ll_sys_get_divider();
    return sys_freq_hz / clk_ll_apb_get_divider();
}

void rtc_clk_apll_enable(bool enable)
{
    // TODO: IDF-8884
}

uint32_t rtc_clk_apll_coeff_calc(uint32_t freq, uint32_t *_o_div, uint32_t *_sdm0, uint32_t *_sdm1, uint32_t *_sdm2)
{
    // TODO: IDF-8884
    return 0;
}

void rtc_clk_apll_coeff_set(uint32_t o_div, uint32_t sdm0, uint32_t sdm1, uint32_t sdm2)
{
    // TODO: IDF-8884
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

//------------------------------------MPLL-------------------------------------//
void rtc_clk_mpll_disable(void)
{
    clk_ll_mpll_disable();
}

void rtc_clk_mpll_enable(void)
{
    clk_ll_mpll_enable();
}

void rtc_clk_mpll_configure(uint32_t xtal_freq, uint32_t mpll_freq)
{
    /* Analog part */
    /* MPLL calibration start */
    regi2c_ctrl_ll_mpll_calibration_start();
    clk_ll_mpll_set_config(mpll_freq, xtal_freq);
    /* wait calibration done */
    while(!regi2c_ctrl_ll_mpll_calibration_is_done());
    /* MPLL calibration stop */
    regi2c_ctrl_ll_mpll_calibration_stop();
}

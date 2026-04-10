/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "esp32s31/rom/rtc.h"
#include "soc/rtc.h"
#include "esp_private/rtc_clk.h"
#include "esp_hw_log.h"
#include "esp_rom_sys.h"
#include "hal/clk_tree_ll.h"
#include "esp_private/sleep_event.h"
#include "esp_private/regi2c_ctrl.h"
#include "esp_attr.h"

static const char *TAG = "rtc_clk";

// CPLL frequency option, in 320MHz. Zero if CPLL is not enabled.
static int s_cur_cpll_freq = 0;

// MPLL frequency option, 500MHz. Zero if MPLL is not enabled.
static uint32_t s_cur_mpll_freq = 0;

#if !BOOTLOADER_BUILD
// Indicate whether the specific clock sources are acquired by the hp root clock (i.e. whether ref_cnt in esp_clk_tree.c is incremented by the hp root clock)
static bool s_is_cpll_acquired = (CONFIG_BOOTLOADER_CPU_CLK_FREQ_MHZ == 80 || CONFIG_BOOTLOADER_CPU_CLK_FREQ_MHZ == 160 || CONFIG_BOOTLOADER_CPU_CLK_FREQ_MHZ == 320);
static bool s_is_pll_f240m_acquired = (CONFIG_BOOTLOADER_CPU_CLK_FREQ_MHZ == 240);
#endif


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
    /* No special bootstrapping needed for ESP32-S31, 'cycle' argument is to keep the signature
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
    // TODO: IDF-14645
// #ifndef BOOTLOADER_BUILD
//     if (clk_src == SOC_RTC_SLOW_CLK_SRC_XTAL32K) {
//         esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL32K, ESP_PD_OPTION_ON);
//     } else {
//         esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL32K, ESP_PD_OPTION_AUTO);
//     }
//     if (clk_src == SOC_RTC_SLOW_CLK_SRC_RC32K) {
//         esp_sleep_pd_config(ESP_PD_DOMAIN_RC32K, ESP_PD_OPTION_ON);
//     } else {
//         esp_sleep_pd_config(ESP_PD_DOMAIN_RC32K, ESP_PD_OPTION_AUTO);
//     }
// #endif
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

#if BOOTLOADER_BUILD
static void rtc_clk_cpll_disable(void)
{
    clk_ll_cpll_disable();
    s_cur_cpll_freq = 0;
}

static void rtc_clk_cpll_enable(void)
{
    clk_ll_cpll_enable();
}
#endif

static void rtc_clk_cpll_configure(soc_xtal_freq_t xtal_freq, int cpll_freq)
{
    /* Digital part */
    clk_ll_cpll_set_freq_mhz(cpll_freq, xtal_freq);

    /* Analog part */
    ANALOG_CLOCK_ENABLE();
    /* CPLL CALIBRATION START */
    clk_ll_cpll_calibration_start();
    clk_ll_cpll_set_config(cpll_freq, xtal_freq);
    /* WAIT CALIBRATION DONE */
    while(!clk_ll_cpll_calibration_is_done());
    esp_rom_delay_us(10); // wait for true stop
    /* CPLL CALIBRATION STOP */
    clk_ll_cpll_calibration_stop();
    ANALOG_CLOCK_DISABLE();

    s_cur_cpll_freq = cpll_freq;
}

/**
 * Switch to use XTAL as the CPU clock source.
 * Must satisfy: cpu_freq = XTAL_FREQ / div.
 * Does not disable the PLL.
 *
 * If to_default is set, then will configure CPU - MEM - SYS - APB frequencies back to power-on reset configuration (40 - 20 - 40/3 - 40/6)
 * If to_default is not set, then will configure to 40 - 40 - 40 - 40
 */
static FORCE_IRAM_ATTR void rtc_clk_cpu_freq_to_xtal(int cpu_freq, int div, bool to_default)
{
    // let f_cpu = f_mem = f_sys = f_apb
    uint32_t mem_divider = 1;
    uint32_t sys_divider = 1;
    uint32_t apb_divider = 1;
    if (to_default) {
        // f_cpu = 2 * f_mem = 2 * f_sys = 4 * f_apb
        mem_divider = 2;
        sys_divider = 3;
        apb_divider = 2;
    }
    clk_ll_cpu_set_divider(div, 0, 0);
    clk_ll_mem_set_divider(mem_divider);
    clk_ll_sys_set_divider(sys_divider);
    clk_ll_apb_set_divider(apb_divider);
    clk_ll_cpu_set_src(SOC_CPU_CLK_SRC_XTAL);
    clk_ll_bus_update();
    esp_rom_set_cpu_ticks_per_us(cpu_freq);
}

static void rtc_clk_cpu_freq_to_rc_fast(void)
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
 * Switch to PLL_F240M as cpu clock source.
 * PLL must already be enabled.
 * @param cpu_freq new CPU frequency
 */
static void rtc_clk_cpu_freq_to_pll_240_mhz(int cpu_freq_mhz)
{
    // f_hp_root = 240MHz
    assert(cpu_freq_mhz == 240);
    uint32_t cpu_divider = CLK_LL_PLL_240M_FREQ_MHZ / cpu_freq_mhz;
    clk_ll_cpu_set_divider(cpu_divider, 0, 0);
    clk_ll_mem_set_divider(2); // 120MHz
    clk_ll_sys_set_divider(3); // 80MHz
    clk_ll_apb_set_divider(2); // 40MHz
    clk_ll_cpu_set_src(SOC_CPU_CLK_SRC_PLL_F240M);
    clk_ll_bus_update();
    esp_rom_set_cpu_ticks_per_us(cpu_freq_mhz);
}

/**
 * Switch to one of CPLL-based frequencies. Current frequency can be XTAL or CPLL.
 * CPLL must already be enabled.
 * @param cpu_freq new CPU frequency
 */
static void rtc_clk_cpu_freq_to_cpll_mhz(int cpu_freq_mhz, hal_utils_clk_div_t *div)
{
    /**
     * Constraint: MEM_CLK <= 160MHz, SYS_CLK <= (320/3)MHz, APB_CLK <= (320/6)MHz
     * This implies that when clock source is CPLL,
     *                   If cpu_divider < 2, mem_divider must be larger or equal to 2, sys_divider must be larger or equal to 3
     *                   If cpu_divider < 2, sys_divider = 3, apb_divider must be larger or equal to 2
     *
     * Current available configurations:
     * CPLL    ->     CPU_CLK   ->     MEM_CLK
     *                          ->     SYS_CLK   ->     APB_CLK
     * 320    div1      320    div2      160
     *                         div4      80     div2      40
     * 320    div2      160    div1      160
     *                         div2      80     div2      40
     * 320    div4      80     div1      80
     *                         div1      80     div2      40
     */
    uint32_t mem_divider = 1;
    uint32_t sys_divider = 1;
    uint32_t apb_divider = 1;
    switch (cpu_freq_mhz) {
    case 320:
        mem_divider = 2;
        sys_divider = 4;
        apb_divider = 2;
        break;
    case 160:
        mem_divider = 1;
        sys_divider = 2;
        apb_divider = 2;
        break;
    case 80:
        mem_divider = 1;
        sys_divider = 1;
        apb_divider = 2;
        break;
    default:
        // Unsupported configuration
        // This is dangerous to modify dividers. Hardware could automatically correct the divider, and it won't be
        // reflected to the registers. Therefore, you won't even be able to calculate out the real mem_clk, apb_clk freq.
        // To avoid such case, we will strictly do abort here.
        abort();
    }
    clk_ll_cpu_set_divider(div->integer, div->numerator, div->denominator);
    clk_ll_mem_set_divider(mem_divider);
    clk_ll_sys_set_divider(sys_divider);
    clk_ll_apb_set_divider(apb_divider);
    clk_ll_cpu_set_src(SOC_CPU_CLK_SRC_CPLL);
    clk_ll_bus_update();
    esp_rom_set_cpu_ticks_per_us(cpu_freq_mhz);
}

bool rtc_clk_cpu_freq_mhz_to_config(uint32_t freq_mhz, rtc_cpu_freq_config_t *out_config)
{
    uint32_t source_freq_mhz;
    soc_cpu_clk_src_t source;
    hal_utils_clk_div_t divider = {0}; // divider = freq of HP_ROOT_CLK / freq of CPU_CLK
    uint32_t real_freq_mhz;

    // Keep default CPLL at 320MHz
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
    } else if (freq_mhz == 80) {
        real_freq_mhz = freq_mhz;
        source = SOC_CPU_CLK_SRC_CPLL;
        source_freq_mhz = CLK_LL_PLL_320M_FREQ_MHZ;
        divider.integer = 4;
    } else if (freq_mhz == 160) {
        real_freq_mhz = freq_mhz;
        source = SOC_CPU_CLK_SRC_CPLL;
        source_freq_mhz = CLK_LL_PLL_320M_FREQ_MHZ;
        divider.integer = 2;
    } else if (freq_mhz == 240) {
        real_freq_mhz = freq_mhz;
        source = SOC_CPU_CLK_SRC_PLL_F240M;
        source_freq_mhz = CLK_LL_PLL_240M_FREQ_MHZ;
        divider.integer = 1;
    } else if (freq_mhz == 320) {
        real_freq_mhz = freq_mhz;
        source = SOC_CPU_CLK_SRC_CPLL;
        source_freq_mhz = CLK_LL_PLL_320M_FREQ_MHZ;
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

__attribute__((weak)) void rtc_clk_set_cpu_switch_to_pll(int event_id)
{
}

static void rtc_clk_cpu_src_clk_enable(soc_cpu_clk_src_t new_src, uint32_t new_src_freq_mhz)
{
    if (new_src == SOC_CPU_CLK_SRC_CPLL) {
        bool truly_enabled = false;
#if BOOTLOADER_BUILD
        rtc_clk_cpll_enable();
        truly_enabled = true;
#else
        if (!s_is_cpll_acquired) {
            truly_enabled = esp_clk_tree_enable_power(SOC_ROOT_CIRCUIT_CLK_CPLL, true);
            s_is_cpll_acquired = true;
        }
#endif
        if (truly_enabled || (s_cur_cpll_freq != new_src_freq_mhz)) {
            rtc_clk_cpll_configure(rtc_clk_xtal_freq_get(), new_src_freq_mhz);
        }
    } else if (new_src == SOC_CPU_CLK_SRC_PLL_F240M) {
#if !BOOTLOADER_BUILD
        if (!s_is_pll_f240m_acquired) {
            esp_clk_tree_enable_src(SOC_MOD_CLK_PLL_F240M, true);
            s_is_pll_f240m_acquired = true;
        }
#endif
    }
}

static void rtc_clk_cpu_src_clk_disable(soc_cpu_clk_src_t old_src)
{
    if (old_src == SOC_CPU_CLK_SRC_CPLL) {
#if BOOTLOADER_BUILD
        rtc_clk_cpll_disable();
#else
        assert(s_is_cpll_acquired);
        bool truly_disabled = esp_clk_tree_enable_power(SOC_ROOT_CIRCUIT_CLK_CPLL, false);
        s_is_cpll_acquired = false;
        if (truly_disabled) {
            s_cur_cpll_freq = 0;
        }
#endif
    } else if (old_src == SOC_CPU_CLK_SRC_PLL_F240M) {
#if !BOOTLOADER_BUILD
        assert(s_is_pll_f240m_acquired);
        s_is_pll_f240m_acquired = false;
        esp_clk_tree_enable_src(SOC_MOD_CLK_PLL_F240M, false);
#endif
    }
}

void rtc_clk_cpu_freq_set_config(const rtc_cpu_freq_config_t *config)
{
    soc_cpu_clk_src_t old_cpu_clk_src = clk_ll_cpu_get_src();
    if (old_cpu_clk_src != config->source) {
        rtc_clk_cpu_src_clk_enable(config->source, config->source_freq_mhz);
    }

    if (config->source == SOC_CPU_CLK_SRC_XTAL) {
        rtc_clk_cpu_freq_to_xtal(config->freq_mhz, config->div.integer, false);
    } else if (config->source == SOC_CPU_CLK_SRC_CPLL) {
        rtc_clk_set_cpu_switch_to_pll(SLEEP_EVENT_HW_PLL_EN_START);
        rtc_clk_cpu_freq_to_cpll_mhz(config->freq_mhz, (hal_utils_clk_div_t *)&config->div);
        rtc_clk_set_cpu_switch_to_pll(SLEEP_EVENT_HW_PLL_EN_STOP);
    } else if (config->source == SOC_CPU_CLK_SRC_PLL_F240M) {
        rtc_clk_cpu_freq_to_pll_240_mhz(config->freq_mhz);
    } else if (config->source == SOC_CPU_CLK_SRC_RC_FAST) {
        rtc_clk_cpu_freq_to_rc_fast();
    }

    if (old_cpu_clk_src != config->source) {
        rtc_clk_cpu_src_clk_disable(old_cpu_clk_src);
    }
}

static uint32_t rtc_clk_hp_root_get_freq_mhz(soc_cpu_clk_src_t clk_src)
{
    uint32_t source_freq_mhz = 0;
    switch (clk_src) {
    case SOC_CPU_CLK_SRC_XTAL:
        source_freq_mhz = (uint32_t)rtc_clk_xtal_freq_get();
        break;
    case SOC_CPU_CLK_SRC_CPLL:
        source_freq_mhz = clk_ll_cpll_get_freq_mhz((uint32_t)rtc_clk_xtal_freq_get());
        break;
    case SOC_CPU_CLK_SRC_RC_FAST:
        source_freq_mhz = 20;
        break;
    case SOC_CPU_CLK_SRC_PLL_F240M:
        source_freq_mhz = CLK_LL_PLL_240M_FREQ_MHZ;
        break;
    default:
        // Unknown HP_ROOT clock source
        ESP_HW_LOGE(TAG, "Invalid HP_ROOT_CLK");
        break;
    }
    return source_freq_mhz;
}

void rtc_clk_cpu_freq_get_config(rtc_cpu_freq_config_t *out_config)
{
    soc_cpu_clk_src_t source = clk_ll_cpu_get_src();
    uint32_t source_freq_mhz = rtc_clk_hp_root_get_freq_mhz(source);
    if (source_freq_mhz == 0) {
        // unsupported frequency configuration
        abort();
    }
    hal_utils_clk_div_t div = {0}; // div = freq of SOC_ROOT_CLK / freq of CPU_CLK
    clk_ll_cpu_get_divider(&div.integer, &div.numerator, &div.denominator);
    if (div.denominator == 0) {
        div.denominator = 1;
        div.numerator = 0;
    }
    uint32_t freq_mhz = source_freq_mhz * div.denominator / (div.integer * div.denominator + div.numerator);
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
        rtc_clk_cpu_freq_to_rc_fast();
    } else if (config->source == SOC_CPU_CLK_SRC_PLL_F240M
#if !BOOTLOADER_BUILD
               && s_is_pll_f240m_acquired
#endif
              ) {
        rtc_clk_cpu_freq_to_pll_240_mhz(config->freq_mhz);
    } else {
        /* fallback */
        rtc_clk_cpu_freq_set_config(config);
    }
}

void rtc_clk_cpu_freq_set_xtal(void)
{
    soc_cpu_clk_src_t old_cpu_clk_src = clk_ll_cpu_get_src();
    int freq_mhz = (int)rtc_clk_xtal_freq_get();

    rtc_clk_cpu_freq_to_xtal(freq_mhz, 1, false);
    if (old_cpu_clk_src != SOC_CPU_CLK_SRC_XTAL) {
        rtc_clk_cpu_src_clk_disable(old_cpu_clk_src);
    }
}

FORCE_IRAM_ATTR void rtc_clk_cpu_set_to_default_config(void)
{
    int freq_mhz = (int)rtc_clk_xtal_freq_get();

    rtc_clk_cpu_freq_to_xtal(freq_mhz, 1, true);
}

void rtc_clk_cpu_freq_set_xtal_for_sleep(void)
{
    int freq_mhz = (int)rtc_clk_xtal_freq_get();

    rtc_clk_cpu_freq_to_xtal(freq_mhz, 1, false);
    s_cur_cpll_freq = 0; // no disable PLL, but set freq to 0 to trigger a PLL calibration after wake-up from sleep
}

FORCE_IRAM_ATTR soc_xtal_freq_t rtc_clk_xtal_freq_get(void)
{
    uint32_t xtal_freq_mhz = clk_ll_xtal_get_freq_mhz();
    assert(xtal_freq_mhz == SOC_XTAL_FREQ_40M);
    return (soc_xtal_freq_t)xtal_freq_mhz;
}

uint32_t rtc_clk_apb_freq_get(void)
{
    soc_cpu_clk_src_t source = clk_ll_cpu_get_src();
    uint32_t source_freq_mhz = rtc_clk_hp_root_get_freq_mhz(source);
    uint32_t integer, numerator, denominator;
    clk_ll_cpu_get_divider(&integer, &numerator, &denominator);
    if (denominator == 0) {
        denominator = 1;
        numerator = 0;
    }
    uint32_t cpu_freq_hz = source_freq_mhz * MHZ * denominator / (integer * denominator + numerator);
    uint32_t sys_freq_hz = cpu_freq_hz / clk_ll_sys_get_divider();
    return sys_freq_hz / clk_ll_apb_get_divider();
}

void rtc_clk_apll_enable(bool enable)
{
    if (enable) {
        clk_ll_apll_enable();
    } else {
        clk_ll_apll_disable();
    }
}

uint32_t rtc_clk_apll_coeff_calc(uint32_t freq, uint32_t *_o_div, uint32_t *_sdm0, uint32_t *_sdm1, uint32_t *_sdm2)
{
    uint32_t rtc_xtal_freq = (uint32_t)rtc_clk_xtal_freq_get();
    if (rtc_xtal_freq == 0) {
        // xtal_freq has not set yet
        ESP_HW_LOGE(TAG, "Get xtal clock frequency failed, it has not been set yet");
        abort();
    }
    /* Reference formula: apll_freq = xtal_freq * (4 + sdm2 + sdm1/256 + sdm0/65536) / ((o_div + 2) * 2)
     *                                ----------------------------------------------   -----------------
     *                                     350 MHz <= Numerator <= 500 MHz                Denominator
     */
    int o_div = 0; // range: 0~31
    int sdm0 = 0;  // range: 0~255
    int sdm1 = 0;  // range: 0~255
    int sdm2 = 0;  // range: 0~63
    /* Firstly try to satisfy the condition that the operation frequency of numerator should be greater than 350 MHz,
     * i.e. xtal_freq * (4 + sdm2 + sdm1/256 + sdm0/65536) >= 350 MHz, '+1' in the following code is to get the ceil value.
     * With this condition, as we know the 'o_div' can't be greater than 31, then we can calculate the APLL minimum support frequency is
     * 350 MHz / ((31 + 2) * 2) = 5303031 Hz (for ceil) */
    o_div = (int)(CLK_LL_APLL_MULTIPLIER_MIN_HZ / (float)(freq * 2) + 1) - 2;
    if (o_div > 31) {
        ESP_HW_LOGE(TAG, "Expected frequency is too small");
        return 0;
    }
    if (o_div < 0) {
        /* Try to satisfy the condition that the operation frequency of numerator should be smaller than 500 MHz,
         * i.e. xtal_freq * (4 + sdm2 + sdm1/256 + sdm0/65536) <= 500 MHz, we need to get the floor value in the following code.
         * With this condition, as we know the 'o_div' can't be smaller than 0, then we can calculate the APLL maximum support frequency is
         * 500 MHz / ((0 + 2) * 2) = 125000000 Hz */
        o_div = (int)(CLK_LL_APLL_MULTIPLIER_MAX_HZ / (float)(freq * 2)) - 2;
        if (o_div < 0) {
            ESP_HW_LOGE(TAG, "Expected frequency is too big");
            return 0;
        }
    }
    // sdm2 = (int)(((o_div + 2) * 2) * apll_freq / xtal_freq) - 4
    sdm2 = (int)(((o_div + 2) * 2 * freq) / (rtc_xtal_freq * MHZ)) - 4;
    // numrator = (((o_div + 2) * 2) * apll_freq / xtal_freq) - 4 - sdm2
    float numrator = (((o_div + 2) * 2 * freq) / ((float)rtc_xtal_freq * MHZ)) - 4 - sdm2;
    // If numrator is bigger than 255/256 + 255/65536 + (1/65536)/2 = 1 - (1 / 65536)/2, carry bit to sdm2
    if (numrator > 1.0 - (1.0 / 65536.0) / 2.0) {
        sdm2++;
    }
    // If numrator is smaller than (1/65536)/2, keep sdm0 = sdm1 = 0, otherwise calculate sdm0 and sdm1
    else if (numrator > (1.0 / 65536.0) / 2.0) {
        // Get the closest sdm1
        sdm1 = (int)(numrator * 65536.0 + 0.5) / 256;
        // Get the closest sdm0
        sdm0 = (int)(numrator * 65536.0 + 0.5) % 256;
    }
    uint32_t real_freq = (uint32_t)(rtc_xtal_freq * MHZ * (4 + sdm2 + (float)sdm1/256.0 + (float)sdm0/65536.0) / (((float)o_div + 2) * 2));
    *_o_div = o_div;
    *_sdm0 = sdm0;
    *_sdm1 = sdm1;
    *_sdm2 = sdm2;
    return real_freq;
}

void rtc_clk_apll_coeff_set(uint32_t o_div, uint32_t sdm0, uint32_t sdm1, uint32_t sdm2)
{
    // TODO: IDF-14771, IDF-14750
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
IRAM_ATTR void rtc_clk_mpll_disable(void)
{
    clk_ll_mpll_disable();
    s_cur_mpll_freq = 0;
}

IRAM_ATTR void rtc_clk_mpll_enable(void)
{
    clk_ll_mpll_enable();
}

void rtc_clk_mpll_configure(uint32_t xtal_freq, uint32_t mpll_freq, bool thread_safe)
{
    if (thread_safe) {
        _regi2c_ctrl_ll_master_enable_clock(true);
    } else {
        ANALOG_CLOCK_ENABLE();
    }
    /* MPLL calibration start */
    clk_ll_mpll_calibration_start();
    clk_ll_mpll_set_config(mpll_freq, xtal_freq);
    /* wait calibration done */
    while(!clk_ll_mpll_calibration_is_done());
    /* MPLL calibration stop */
    clk_ll_mpll_calibration_stop();

    if (thread_safe) {
        _regi2c_ctrl_ll_master_enable_clock(false);
    } else {
        ANALOG_CLOCK_DISABLE();
    }
    s_cur_mpll_freq = mpll_freq;
}

IRAM_ATTR uint32_t rtc_clk_mpll_get_freq(void)
{
    return s_cur_mpll_freq;
}

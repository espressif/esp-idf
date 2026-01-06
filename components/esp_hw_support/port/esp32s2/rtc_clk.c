/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "esp32s2/rom/rtc.h"
#include "soc/rtc.h"
#include "esp_private/rtc_clk.h"
#include "esp_private/esp_sleep_internal.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_io_reg.h"
#include "soc/soc_caps.h"
#include "esp_rom_sys.h"
#include "esp_hw_log.h"
#include "sdkconfig.h"
#include "hal/clk_tree_ll.h"
#ifndef BOOTLOADER_BUILD
#include "esp_private/systimer.h"
#include "hal/systimer_ll.h"
#endif
#include "esp_attr.h"

ESP_HW_LOG_ATTR_TAG(TAG, "rtc_clk");

// Current PLL frequency, in MHZ (320 or 480). Zero if PLL is not enabled.
// On the ESP32-S2, 480MHz PLL is enabled at reset.
static uint32_t s_cur_pll_freq = CLK_LL_PLL_480M_FREQ_MHZ;

static void rtc_clk_cpu_freq_to_xtal(int freq, int div);
static void rtc_clk_cpu_freq_to_rc_fast(void);

void rtc_clk_32k_enable(bool enable)
{
    if (enable) {
        SET_PERI_REG_MASK(RTC_IO_XTAL_32P_PAD_REG, RTC_IO_X32P_MUX_SEL);
        SET_PERI_REG_MASK(RTC_IO_XTAL_32N_PAD_REG, RTC_IO_X32N_MUX_SEL);
        clk_ll_xtal32k_enable(CLK_LL_XTAL32K_ENABLE_MODE_CRYSTAL);
    } else {
        clk_ll_xtal32k_disable();
    }
}

void rtc_clk_32k_enable_external(void)
{
    SET_PERI_REG_MASK(RTC_IO_XTAL_32P_PAD_REG, RTC_IO_X32P_MUX_SEL);
    SET_PERI_REG_MASK(RTC_IO_XTAL_32N_PAD_REG, RTC_IO_X32N_MUX_SEL);
    clk_ll_xtal32k_enable(CLK_LL_XTAL32K_ENABLE_MODE_EXTERNAL);
}

void rtc_clk_32k_disable_external(void)
{
    CLEAR_PERI_REG_MASK(RTC_IO_XTAL_32P_PAD_REG, RTC_IO_X32P_MUX_SEL);
    CLEAR_PERI_REG_MASK(RTC_IO_XTAL_32N_PAD_REG, RTC_IO_X32N_MUX_SEL);
    clk_ll_xtal32k_disable();
}

void rtc_clk_32k_bootstrap(uint32_t cycle)
{
    /* No special bootstrapping needed for ESP32-S2, 'cycle' argument is to keep the signature
     * same as for the ESP32. Just enable the XTAL here.
     */
    (void)cycle;
    rtc_clk_32k_enable(true);
}

bool rtc_clk_32k_enabled(void)
{
    return clk_ll_xtal32k_is_enabled();
}

void rtc_clk_8m_enable(bool clk_8m_en, bool d256_en)
{
    if (clk_8m_en) {
        clk_ll_rc_fast_enable();
        esp_rom_delay_us(SOC_DELAY_RC_FAST_ENABLE);
    } else {
        clk_ll_rc_fast_disable();
    }
    /* d256 should be independent configured with 8M
     * Maybe we can split this function into 8m and dmd256
     */
    if (d256_en) {
        clk_ll_rc_fast_d256_enable();
    } else {
        clk_ll_rc_fast_d256_disable();
    }
}

bool rtc_clk_8m_enabled(void)
{
    return clk_ll_rc_fast_is_enabled();
}

bool rtc_clk_8md256_enabled(void)
{
    return clk_ll_rc_fast_d256_is_enabled();
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
    clk_ll_apll_set_config(o_div, sdm0, sdm1, sdm2);

    /* calibration */
    clk_ll_apll_set_calibration();

    /* wait for calibration end */
    while (!clk_ll_apll_calibration_is_done()) {
        /* use esp_rom_delay_us so the RTC bus doesn't get flooded */
        esp_rom_delay_us(1);
    }
}

void rtc_clk_slow_src_set(soc_rtc_slow_clk_src_t clk_src)
{
#ifndef BOOTLOADER_BUILD
    // Keep the RTC8M_CLK on in sleep if RTC clock is rc_fast_d256.
    if ((clk_src == SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256) && (esp_sleep_sub_mode_dump_config(NULL)[ESP_SLEEP_RTC_USE_RC_FAST_MODE] == 0)) { // Switch to RC_FAST_D256
        esp_sleep_sub_mode_config(ESP_SLEEP_RTC_USE_RC_FAST_MODE, true);
    } else if (clk_src != SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256) {
        // This is the only user of ESP_SLEEP_RTC_USE_RC_FAST_MODE submode, so force disable it.
        esp_sleep_sub_mode_force_disable(ESP_SLEEP_RTC_USE_RC_FAST_MODE);
    }
#endif

    clk_ll_rtc_slow_set_src(clk_src);
    /* Why we need to connect this clock to digital?
     * Or maybe this clock should be connected to digital when xtal 32k clock is enabled instead?
     */
    if (clk_src == SOC_RTC_SLOW_CLK_SRC_XTAL32K) {
        clk_ll_xtal32k_digi_enable();
    } else {
        clk_ll_xtal32k_digi_disable();
    }
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
    case SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256: return SOC_CLK_RC_FAST_D256_FREQ_APPROX;
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

static void rtc_clk_bbpll_configure(soc_xtal_freq_t xtal_freq, int pll_freq)
{
    assert(xtal_freq == SOC_XTAL_FREQ_40M);

    /* Digital part */
    clk_ll_bbpll_set_freq_mhz(pll_freq);
    /* Analog part */
    clk_ll_bbpll_set_config(pll_freq, xtal_freq);

    // Enable calibration by software
    clk_ll_bbpll_calibration_enable();
    for (int ext_cap = 0; ext_cap < 16; ext_cap++) {
        if (clk_ll_bbpll_calibration_is_done(ext_cap)) {
            break;
        }
        if (ext_cap == 15) {
            ESP_HW_LOGE(TAG, "BBPLL SOFTWARE CAL FAIL");
            abort();
        }
    }

    s_cur_pll_freq = pll_freq;
}

/**
 * Switch to one of PLL-based frequencies. Current frequency can be XTAL or PLL.
 * PLL must already be enabled.
 * @param cpu_freq new CPU frequency
 */
static void rtc_clk_cpu_freq_to_pll_mhz(int cpu_freq_mhz)
{
    /* To avoid the problem of insufficient voltage when the CPU frequency is switched:
     * When the CPU frequency is switched from low to high, it is necessary to
     * increase the voltage first and then increase the frequency, and the frequency
     * needs to wait for the voltage to fully increase before proceeding.
     * When the frequency of the CPU is switched from high to low, it is necessary
     * to reduce the frequency first and then reduce the voltage.
     */

    rtc_cpu_freq_config_t cur_config;
    rtc_clk_cpu_freq_get_config(&cur_config);
    /* cpu_frequency < 240M: dbias = DIG_DBIAS_XTAL_80M_160M;
     * cpu_frequency = 240M: dbias = DIG_DBIAS_240M;
     */
    if (cpu_freq_mhz > cur_config.freq_mhz) {
        if (cpu_freq_mhz == 240) {
            REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_240M);
            REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DBIAS_WAK, RTC_DBIAS_240M);
            esp_rom_delay_us(40);
        }
    }
    clk_ll_cpu_set_freq_mhz_from_pll(cpu_freq_mhz);
    clk_ll_cpu_set_divider(1);
    clk_ll_cpu_set_src(SOC_CPU_CLK_SRC_PLL);
    rtc_clk_apb_freq_update(80 * MHZ);
    esp_rom_set_cpu_ticks_per_us(cpu_freq_mhz);

    if (cpu_freq_mhz < cur_config.freq_mhz) {
        if (cur_config.freq_mhz == 240) {
            REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_XTAL_80M_160M);
            REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DBIAS_WAK, RTC_DBIAS_XTAL_80M_160M);
            esp_rom_delay_us(40);
        }
    }
}

bool rtc_clk_cpu_freq_mhz_to_config(uint32_t freq_mhz, rtc_cpu_freq_config_t* out_config)
{
    uint32_t source_freq_mhz;
    soc_cpu_clk_src_t source;
    uint32_t divider;
    uint32_t real_freq_mhz;

    uint32_t xtal_freq = CLK_LL_XTAL_FREQ_MHZ;
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
        source = SOC_CPU_CLK_SRC_PLL;
        source_freq_mhz = CLK_LL_PLL_480M_FREQ_MHZ;
        divider = 6;
    } else if (freq_mhz == 160) {
        real_freq_mhz = freq_mhz;
        source = SOC_CPU_CLK_SRC_PLL;
        source_freq_mhz = CLK_LL_PLL_480M_FREQ_MHZ;
        divider = 3;
    } else if (freq_mhz == 240) {
        real_freq_mhz = freq_mhz;
        source = SOC_CPU_CLK_SRC_PLL;
        source_freq_mhz = CLK_LL_PLL_480M_FREQ_MHZ;
        divider = 2;
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

void rtc_clk_cpu_freq_set_config(const rtc_cpu_freq_config_t* config)
{
    soc_cpu_clk_src_t old_cpu_clk_src = clk_ll_cpu_get_src();
    if (old_cpu_clk_src != SOC_CPU_CLK_SRC_XTAL) {
        rtc_clk_cpu_freq_to_xtal(CLK_LL_XTAL_FREQ_MHZ, 1);
    }
    if (old_cpu_clk_src == SOC_CPU_CLK_SRC_PLL && config->source_freq_mhz != s_cur_pll_freq) {
        rtc_clk_bbpll_disable();
    }

    if (config->source == SOC_CPU_CLK_SRC_XTAL) {
        if (config->div > 1) {
            rtc_clk_cpu_freq_to_xtal(config->freq_mhz, config->div);
        }
    } else if (config->source == SOC_CPU_CLK_SRC_PLL) {
        rtc_clk_bbpll_enable();
        rtc_clk_bbpll_configure((soc_xtal_freq_t)CLK_LL_XTAL_FREQ_MHZ, config->source_freq_mhz);
        rtc_clk_cpu_freq_to_pll_mhz(config->freq_mhz);
    } else if (config->source == SOC_CPU_CLK_SRC_RC_FAST) {
        rtc_clk_cpu_freq_to_rc_fast();
    }
}

void rtc_clk_cpu_freq_get_config(rtc_cpu_freq_config_t* out_config)
{
    soc_cpu_clk_src_t source = clk_ll_cpu_get_src();
    uint32_t source_freq_mhz;
    uint32_t div;
    uint32_t freq_mhz;
    switch (source) {
        case SOC_CPU_CLK_SRC_XTAL: {
            div = clk_ll_cpu_get_divider();
            source_freq_mhz = CLK_LL_XTAL_FREQ_MHZ;
            freq_mhz = source_freq_mhz / div;
        }
        break;
        case SOC_CPU_CLK_SRC_PLL: {
            freq_mhz = clk_ll_cpu_get_freq_mhz_from_pll();
            source_freq_mhz = clk_ll_bbpll_get_freq_mhz();
            if (freq_mhz == CLK_LL_PLL_80M_FREQ_MHZ) {
                div = (source_freq_mhz == CLK_LL_PLL_480M_FREQ_MHZ) ? 6 : 4;
            } else if (freq_mhz == CLK_LL_PLL_160M_FREQ_MHZ) {
                div = (source_freq_mhz == CLK_LL_PLL_480M_FREQ_MHZ) ? 3 : 2;
            } else if (freq_mhz == CLK_LL_PLL_240M_FREQ_MHZ && source_freq_mhz == CLK_LL_PLL_480M_FREQ_MHZ) {
                div = 2;
            } else {
                ESP_HW_LOGE(TAG, "unsupported frequency configuration");
                abort();
            }
            break;
        }
        case SOC_CPU_CLK_SRC_RC_FAST:
            source_freq_mhz = 8;
            div = 1;
            freq_mhz = source_freq_mhz;
            break;
        case SOC_CPU_CLK_SRC_APLL:
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

void rtc_clk_cpu_freq_set_config_fast(const rtc_cpu_freq_config_t* config)
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
    rtc_clk_cpu_set_to_default_config();
    /* BBPLL is kept enabled */
}

FORCE_IRAM_ATTR void rtc_clk_cpu_set_to_default_config(void)
{
    rtc_clk_cpu_freq_to_xtal(CLK_LL_XTAL_FREQ_MHZ, 1);
}

void rtc_clk_cpu_freq_set_xtal_for_sleep(void)
{
    rtc_clk_cpu_freq_set_xtal();
}

/**
 * Switch to use XTAL as the CPU clock source.
 * Must satisfy: cpu_freq = XTAL_FREQ / div.
 * Does not disable the PLL.
 */
static FORCE_IRAM_ATTR void rtc_clk_cpu_freq_to_xtal(int cpu_freq, int div)
{
    rtc_cpu_freq_config_t cur_config;
    rtc_clk_cpu_freq_get_config(&cur_config);

    esp_rom_set_cpu_ticks_per_us(cpu_freq);
    /* Set divider from XTAL to APB clock. Need to set divider to 1 (reg. value 0) first. */
    clk_ll_cpu_set_divider(1);
    clk_ll_cpu_set_divider(div);
    /* no need to adjust the REF_TICK, default register value already set it to 1MHz with any cpu clock source */
    /* switch clock source */
    clk_ll_cpu_set_src(SOC_CPU_CLK_SRC_XTAL);
#ifndef BOOTLOADER_BUILD
    systimer_ll_set_step_for_xtal(&SYSTIMER, systimer_us_to_ticks(1) / cpu_freq);
#endif
    rtc_clk_apb_freq_update(cpu_freq * MHZ);

    /* lower the voltage
     * cpu_frequency < 240M: dbias = DIG_DBIAS_XTAL_80M_160M;
     * cpu_frequency = 240M: dbias = DIG_DBIAS_240M;
     */
    if (cur_config.freq_mhz == 240) {
        REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_XTAL_80M_160M);
        REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DBIAS_WAK, RTC_DBIAS_XTAL_80M_160M);
        esp_rom_delay_us(40);
    }
}

static void rtc_clk_cpu_freq_to_rc_fast(void)
{
    assert(0 && "LDO dbias need to modified");
    esp_rom_set_cpu_ticks_per_us(8);
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_XTAL);
    esp_rom_delay_us(40);
    clk_ll_cpu_set_divider(1);
    clk_ll_cpu_set_src(SOC_CPU_CLK_SRC_RC_FAST);
    rtc_clk_apb_freq_update(SOC_CLK_RC_FAST_FREQ_APPROX);
}

soc_xtal_freq_t rtc_clk_xtal_freq_get(void)
{
    // Note, inside esp32s2-only code it's better to use CLK_LL_XTAL_FREQ_MHZ constant
    return (soc_xtal_freq_t)CLK_LL_XTAL_FREQ_MHZ;
}

void rtc_clk_apb_freq_update(uint32_t apb_freq)
{
    clk_ll_apb_store_freq_hz(apb_freq);
}

uint32_t rtc_clk_apb_freq_get(void)
{
    return clk_ll_apb_load_freq_hz();
}

void rtc_clk_divider_set(uint32_t div)
{
    clk_ll_rc_slow_set_divider(div + 1);
}

void rtc_clk_8m_divider_set(uint32_t div)
{
    clk_ll_rc_fast_set_divider(div + 1);
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

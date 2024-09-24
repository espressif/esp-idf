/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "soc/rtc.h"
#include "esp_private/rtc_clk.h"
#include "esp_private/esp_sleep_internal.h"
#include "soc/rtc_periph.h"
#include "soc/sens_reg.h"
#include "soc/soc_caps.h"
#include "soc/chip_revision.h"
#include "hal/efuse_ll.h"
#include "hal/efuse_hal.h"
#include "soc/gpio_struct.h"
#include "hal/gpio_ll.h"
#include "esp_hw_log.h"
#include "sdkconfig.h"
#include "esp_rom_sys.h"
#include "esp_rom_gpio.h"
#include "esp32/rom/rtc.h"
#include "hal/clk_tree_ll.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/io_mux_reg.h"

#define XTAL_32K_BOOTSTRAP_TIME_US      7

static void rtc_clk_cpu_freq_to_8m(void);
static void rtc_clk_cpu_freq_to_pll_mhz(int cpu_freq_mhz);

// Current PLL frequency, in MHZ (320 or 480). Zero if PLL is not enabled.
static uint32_t s_cur_pll_freq;

static const char* TAG = "rtc_clk";

static void rtc_clk_32k_enable_common(clk_ll_xtal32k_enable_mode_t mode)
{
    CLEAR_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG,
                        RTC_IO_X32P_RDE | RTC_IO_X32P_RUE | RTC_IO_X32N_RUE |
                        RTC_IO_X32N_RDE | RTC_IO_X32N_FUN_IE | RTC_IO_X32P_FUN_IE);
    SET_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_X32N_MUX_SEL | RTC_IO_X32P_MUX_SEL);

#ifdef CONFIG_RTC_EXT_CRYST_ADDIT_CURRENT
    // version0 and version1 need provide additional current to external XTAL.
    if (!ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 200)) {
        /* TOUCH sensor can provide additional current to external XTAL.
        In some case, X32N and X32P PAD don't have enough drive capability to start XTAL */
        SET_PERI_REG_MASK(RTC_IO_TOUCH_CFG_REG, RTC_IO_TOUCH_XPD_BIAS_M);
        /* Tie PAD Touch8 to VDD
        NOTE: TOUCH8 and TOUCH9 register settings are reversed except for DAC, so we set RTC_IO_TOUCH_PAD9_REG here instead*/
        SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD9_REG, RTC_IO_TOUCH_PAD9_TIE_OPT_M);
        /* Set the current used to compensate TOUCH PAD8 */
        SET_PERI_REG_BITS(RTC_IO_TOUCH_PAD8_REG, RTC_IO_TOUCH_PAD8_DAC, 4, RTC_IO_TOUCH_PAD8_DAC_S);
        /* Power up TOUCH8
        So the Touch DAC start to drive some current from VDD to TOUCH8(which is also XTAL-N)*/
        SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD9_REG, RTC_IO_TOUCH_PAD9_XPD_M);
    }
#elif defined CONFIG_RTC_EXT_CRYST_ADDIT_CURRENT_V2
    if (!ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 200)) {
        /* TOUCH sensor can provide additional current to external XTAL.
        In some case, X32N and X32P PAD don't have enough drive capability to start XTAL */
        SET_PERI_REG_MASK(RTC_IO_TOUCH_CFG_REG, RTC_IO_TOUCH_XPD_BIAS_M);
        SET_PERI_REG_BITS(RTC_IO_TOUCH_CFG_REG, RTC_IO_TOUCH_DCUR, 3, RTC_IO_TOUCH_DCUR_S);
        CLEAR_PERI_REG_MASK(SENS_SAR_TOUCH_CTRL2_REG, SENS_TOUCH_START_FSM_EN_M);
        /* Tie PAD Touch8 to VDD
        NOTE: TOUCH8 and TOUCH9 register settings are reversed except for DAC, so we set RTC_IO_TOUCH_PAD9_REG here instead
        */
        SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD9_REG, RTC_IO_TOUCH_PAD9_TIE_OPT_M);
        /* Set the current used to compensate TOUCH PAD8 */
        SET_PERI_REG_BITS(RTC_IO_TOUCH_PAD8_REG, RTC_IO_TOUCH_PAD8_DAC, 1, RTC_IO_TOUCH_PAD8_DAC_S);
        /* Power up TOUCH8
        So the Touch DAC start to drive some current from VDD to TOUCH8(which is also XTAL-N)
        */
        SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD9_REG, RTC_IO_TOUCH_PAD9_XPD_M);
        CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_PAD9_REG, RTC_IO_TOUCH_PAD9_START_M);
    }
#endif

    clk_ll_xtal32k_enable(mode);
}

void rtc_clk_32k_enable(bool enable)
{
    if (enable) {
        rtc_clk_32k_enable_common(CLK_LL_XTAL32K_ENABLE_MODE_CRYSTAL);
    } else {
        clk_ll_xtal32k_disable();
        /* Disable X32N and X32P pad drive external xtal */
        CLEAR_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_X32N_MUX_SEL | RTC_IO_X32P_MUX_SEL);

#ifdef CONFIG_RTC_EXT_CRYST_ADDIT_CURRENT
        if (!ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 200)) {
            /* Power down TOUCH */
            CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_PAD9_REG, RTC_IO_TOUCH_PAD9_XPD_M);
        }
#elif defined CONFIG_RTC_EXT_CRYST_ADDIT_CURRENT_V2
        if (!ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 200)) {
            /* Power down TOUCH */
            CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_CFG_REG, RTC_IO_TOUCH_XPD_BIAS_M);
            SET_PERI_REG_BITS(RTC_IO_TOUCH_CFG_REG, RTC_IO_TOUCH_DCUR, 0, RTC_IO_TOUCH_DCUR_S);
            CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_PAD9_REG, RTC_IO_TOUCH_PAD9_XPD_M);
            SET_PERI_REG_MASK(SENS_SAR_TOUCH_CTRL2_REG, SENS_TOUCH_START_FSM_EN_M);
        }
#endif
    }
}

void rtc_clk_32k_enable_external(void)
{
    rtc_clk_32k_enable_common(CLK_LL_XTAL32K_ENABLE_MODE_EXTERNAL);
}

/* Helping external 32kHz crystal to start up.
 * External crystal connected to outputs GPIO32 GPIO33.
 * Forms N pulses with a frequency of about 32KHz on the outputs of the crystal.
 */
void rtc_clk_32k_bootstrap(uint32_t cycle)
{
    if (cycle){
        esp_rom_gpio_pad_select_gpio(XTAL32K_P_GPIO_NUM);
        esp_rom_gpio_pad_select_gpio(XTAL32K_N_GPIO_NUM);
        gpio_ll_output_enable(&GPIO, XTAL32K_P_GPIO_NUM);
        gpio_ll_output_enable(&GPIO, XTAL32K_N_GPIO_NUM);
        gpio_ll_set_level(&GPIO, XTAL32K_P_GPIO_NUM, 1);
        gpio_ll_set_level(&GPIO, XTAL32K_N_GPIO_NUM, 0);

        const uint32_t delay_us = (1000000 / SOC_CLK_XTAL32K_FREQ_APPROX / 2);
        while (cycle) {
            gpio_ll_set_level(&GPIO, XTAL32K_P_GPIO_NUM, 1);
            gpio_ll_set_level(&GPIO, XTAL32K_N_GPIO_NUM, 0);
            esp_rom_delay_us(delay_us);
            gpio_ll_set_level(&GPIO, XTAL32K_N_GPIO_NUM, 1);
            gpio_ll_set_level(&GPIO, XTAL32K_P_GPIO_NUM, 0);
            esp_rom_delay_us(delay_us);
            cycle--;
        }
        // disable pins
        gpio_ll_output_disable(&GPIO, XTAL32K_P_GPIO_NUM);
        gpio_ll_output_disable(&GPIO, XTAL32K_N_GPIO_NUM);
    }

    clk_ll_xtal32k_disable();
    SET_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_X32P_RUE | RTC_IO_X32N_RDE);
    esp_rom_delay_us(XTAL_32K_BOOTSTRAP_TIME_US);

    rtc_clk_32k_enable_common(CLK_LL_XTAL32K_ENABLE_MODE_BOOTSTRAP);
}

bool rtc_clk_32k_enabled(void)
{
    return clk_ll_xtal32k_is_enabled();
}

void rtc_clk_8m_enable(bool clk_8m_en, bool d256_en)
{
    if (clk_8m_en) {
        clk_ll_rc_fast_enable();
        if (d256_en) {
            clk_ll_rc_fast_d256_enable();
        } else {
            clk_ll_rc_fast_d256_disable();
        }
        esp_rom_delay_us(SOC_DELAY_RC_FAST_ENABLE);
    } else {
        clk_ll_rc_fast_disable();
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

    if (!enable && (clk_ll_cpu_get_src() != SOC_CPU_CLK_SRC_PLL)) {
        // if apll and bbpll are both not in use, then can also power down the internal I2C bus
        // this power down affects most of the analog peripherals
        clk_ll_i2c_pd();
    } else {
        clk_ll_i2c_pu();
    }
}

uint32_t rtc_clk_apll_coeff_calc(uint32_t freq, uint32_t *_o_div, uint32_t *_sdm0, uint32_t *_sdm1, uint32_t *_sdm2)
{
    uint32_t xtal_freq_mhz = (uint32_t)rtc_clk_xtal_freq_get();
    if (xtal_freq_mhz == 0) {
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
    sdm2 = (int)(((o_div + 2) * 2 * freq) / (xtal_freq_mhz * MHZ)) - 4;
    // numrator = (((o_div + 2) * 2) * apll_freq / xtal_freq) - 4 - sdm2
    float numrator = (((o_div + 2) * 2 * freq) / ((float)xtal_freq_mhz * MHZ)) - 4 - sdm2;
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
    uint32_t real_freq = (uint32_t)(xtal_freq_mhz * MHZ * (4 + sdm2 + (float)sdm1/256.0 + (float)sdm0/65536.0) / (((float)o_div + 2) * 2));
    *_o_div = o_div;
    *_sdm0 = sdm0;
    *_sdm1 = sdm1;
    *_sdm2 = sdm2;
    return real_freq;
}

void rtc_clk_apll_coeff_set(uint32_t o_div, uint32_t sdm0, uint32_t sdm1, uint32_t sdm2)
{
    bool is_rev0 = (efuse_ll_get_chip_ver_rev1() == 0);
    clk_ll_apll_set_config(is_rev0, o_div, sdm0, sdm1, sdm2);

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
    soc_rtc_slow_clk_src_t clk_src_before_switch = clk_ll_rtc_slow_get_src();
    // Keep the RTC8M_CLK on in sleep if RTC clock is rc_fast_d256.
    if (clk_src == SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256 && clk_src_before_switch != SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256) {       // Switch to RC_FAST_D256
        esp_sleep_sub_mode_config(ESP_SLEEP_RTC_USE_RC_FAST_MODE, true);
    } else if (clk_src != SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256 && clk_src_before_switch == SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256) { // Switch away from RC_FAST_D256
        esp_sleep_sub_mode_config(ESP_SLEEP_RTC_USE_RC_FAST_MODE, false);
    }
#endif

    clk_ll_rtc_slow_set_src(clk_src);
    // The logic should be moved to BT driver
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

    // if apll is under force power down, then can also power down the internal I2C bus
    // this power down affects most of the analog peripherals
    if (clk_ll_apll_is_fpd()) {
        clk_ll_i2c_pd();
    }
}

static void rtc_clk_bbpll_enable(void)
{
    clk_ll_i2c_pu();
    clk_ll_bbpll_enable();
}

static void rtc_clk_bbpll_configure(soc_xtal_freq_t xtal_freq, int pll_freq)
{
    /* Raise the voltage */
    if (pll_freq == CLK_LL_PLL_320M_FREQ_MHZ) {
        REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_80M_160M);
    } else { // CLK_LL_PLL_480M_FREQ_MHZ
        REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_240M);
        esp_rom_delay_us(SOC_DELAY_PLL_DBIAS_RAISE);
    }

    clk_ll_bbpll_set_config(pll_freq, xtal_freq);
    uint32_t delay_pll_en = (clk_ll_rtc_slow_get_src() == SOC_RTC_SLOW_CLK_SRC_RC_SLOW) ?
            SOC_DELAY_PLL_ENABLE_WITH_150K : SOC_DELAY_PLL_ENABLE_WITH_32K;
    esp_rom_delay_us(delay_pll_en);
    s_cur_pll_freq = pll_freq;
}

/**
 * Switch to use XTAL as the CPU clock source.
 * Must satisfy: cpu_freq = XTAL_FREQ / div.
 * Does not disable the PLL.
 */
void rtc_clk_cpu_freq_to_xtal(int cpu_freq, int div)
{
    esp_rom_set_cpu_ticks_per_us(cpu_freq);
    /* set divider from XTAL to APB clock */
    clk_ll_cpu_set_divider(div);
    /* adjust ref_tick */
    clk_ll_ref_tick_set_divider(SOC_CPU_CLK_SRC_XTAL, cpu_freq);
    /* switch clock source */
    clk_ll_cpu_set_src(SOC_CPU_CLK_SRC_XTAL);
    rtc_clk_apb_freq_update(cpu_freq * MHZ);
    /* lower the voltage */
    int dbias = (cpu_freq <= 2) ? DIG_DBIAS_2M : DIG_DBIAS_XTAL;
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, dbias);
}

static void rtc_clk_cpu_freq_to_8m(void)
{
    esp_rom_set_cpu_ticks_per_us(8);
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_XTAL);
    clk_ll_cpu_set_divider(1);
    /* adjust ref_tick */
    clk_ll_ref_tick_set_divider(SOC_CPU_CLK_SRC_RC_FAST, 8);
    /* switch clock source */
    clk_ll_cpu_set_src(SOC_CPU_CLK_SRC_RC_FAST);
    rtc_clk_apb_freq_update(SOC_CLK_RC_FAST_FREQ_APPROX);
}

/**
 * Switch to one of PLL-based frequencies. Current frequency can be XTAL or PLL.
 * PLL must already be enabled.
 * @param cpu_freq new CPU frequency
 */
static void rtc_clk_cpu_freq_to_pll_mhz(int cpu_freq_mhz)
{
    int dbias = (cpu_freq_mhz == 240) ? DIG_DBIAS_240M : DIG_DBIAS_80M_160M;
    clk_ll_cpu_set_freq_mhz_from_pll(cpu_freq_mhz);
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, dbias);
    /* adjust ref_tick */
    clk_ll_ref_tick_set_divider(SOC_CPU_CLK_SRC_PLL, cpu_freq_mhz);
    /* switch clock source */
    clk_ll_cpu_set_src(SOC_CPU_CLK_SRC_PLL);
    rtc_clk_apb_freq_update(80 * MHZ);
    esp_rom_set_cpu_ticks_per_us(cpu_freq_mhz);
    rtc_clk_wait_for_slow_cycle();
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
    rtc_clk_wait_for_slow_cycle();
}

bool rtc_clk_cpu_freq_mhz_to_config(uint32_t freq_mhz, rtc_cpu_freq_config_t* out_config)
{
    uint32_t source_freq_mhz;
    soc_cpu_clk_src_t source;
    uint32_t divider;
    uint32_t real_freq_mhz;

    uint32_t xtal_freq = (uint32_t) rtc_clk_xtal_freq_get();
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
        source_freq_mhz = CLK_LL_PLL_320M_FREQ_MHZ;
        divider = 4;
    } else if (freq_mhz == 160) {
        real_freq_mhz = freq_mhz;
        source = SOC_CPU_CLK_SRC_PLL;
        source_freq_mhz = CLK_LL_PLL_320M_FREQ_MHZ;
        divider = 2;
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
    soc_xtal_freq_t xtal_freq = rtc_clk_xtal_freq_get();
    soc_cpu_clk_src_t old_cpu_clk_src = clk_ll_cpu_get_src();
    if (old_cpu_clk_src != SOC_CPU_CLK_SRC_XTAL) {
        rtc_clk_cpu_freq_to_xtal(xtal_freq, 1);
        rtc_clk_wait_for_slow_cycle();
    }
    if (old_cpu_clk_src == SOC_CPU_CLK_SRC_PLL) {
        rtc_clk_bbpll_disable();
    }

    if (config->source == SOC_CPU_CLK_SRC_XTAL) {
        if (config->div > 1) {
            rtc_clk_cpu_freq_to_xtal(config->freq_mhz, config->div);
        }
    } else if (config->source == SOC_CPU_CLK_SRC_PLL) {
        rtc_clk_bbpll_enable();
        rtc_clk_wait_for_slow_cycle();
        rtc_clk_bbpll_configure(rtc_clk_xtal_freq_get(), config->source_freq_mhz);
        rtc_clk_cpu_freq_to_pll_mhz(config->freq_mhz);
    } else if (config->source == SOC_CPU_CLK_SRC_RC_FAST) {
        rtc_clk_cpu_freq_to_8m();
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
        source_freq_mhz = (uint32_t) rtc_clk_xtal_freq_get();
        freq_mhz = source_freq_mhz / div;
    }
    break;
    case SOC_CPU_CLK_SRC_PLL: {
        freq_mhz = clk_ll_cpu_get_freq_mhz_from_pll();
        if (freq_mhz == CLK_LL_PLL_80M_FREQ_MHZ) {
            source_freq_mhz = CLK_LL_PLL_320M_FREQ_MHZ;
            div = 4;
        } else if (freq_mhz == CLK_LL_PLL_160M_FREQ_MHZ) {
            source_freq_mhz = CLK_LL_PLL_320M_FREQ_MHZ;
            div = 2;
        } else if (freq_mhz == CLK_LL_PLL_240M_FREQ_MHZ) {
            source_freq_mhz = CLK_LL_PLL_480M_FREQ_MHZ;
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

soc_xtal_freq_t rtc_clk_xtal_freq_get(void)
{
    uint32_t xtal_freq_mhz = clk_ll_xtal_load_freq_mhz();
    if (xtal_freq_mhz == 0) {
        return SOC_XTAL_FREQ_AUTO;
    }
    return (soc_xtal_freq_t)xtal_freq_mhz;
}

void rtc_clk_xtal_freq_update(soc_xtal_freq_t xtal_freq)
{
    clk_ll_xtal_store_freq_mhz((uint32_t)xtal_freq);
}

void rtc_clk_apb_freq_update(uint32_t apb_freq)
{
    clk_ll_apb_store_freq_hz(apb_freq);
}

uint32_t rtc_clk_apb_freq_get(void)
{
#if CONFIG_IDF_ENV_FPGA
    return CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ * MHZ;
#endif // CONFIG_IDF_ENV_FPGA
    return clk_ll_apb_load_freq_hz();
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

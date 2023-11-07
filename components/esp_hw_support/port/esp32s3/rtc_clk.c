/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "esp32s3/rom/rtc.h"
#include "soc/rtc.h"
#include "esp_private/rtc_clk.h"
#include "soc/rtc_io_reg.h"
#include "esp_rom_sys.h"
#include "esp_hw_log.h"
#include "hal/clk_tree_ll.h"
#include "hal/regi2c_ctrl_ll.h"
#include "esp_private/regi2c_ctrl.h"
#include "soc/regi2c_dig_reg.h"
#include "sdkconfig.h"

static const char *TAG = "rtc_clk";

// Current PLL frequency, in MHZ (320 or 480). Zero if PLL is not enabled.
static uint32_t s_cur_pll_freq;

static uint32_t s_apb_freq;

void rtc_clk_cpu_freq_to_xtal(int freq, int div);
static void rtc_clk_cpu_freq_to_8m(void);

extern uint32_t g_dig_dbias_pvt_240m;
extern uint32_t g_rtc_dbias_pvt_240m;
extern uint32_t g_dig_dbias_pvt_non_240m;
extern uint32_t g_rtc_dbias_pvt_non_240m;

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

void rtc_clk_32k_bootstrap(uint32_t cycle)
{
    /* No special bootstrapping needed for ESP32-S3, 'cycle' argument is to keep the signature
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

void rtc_clk_slow_src_set(soc_rtc_slow_clk_src_t clk_src)
{
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

static void rtc_clk_bbpll_configure(rtc_xtal_freq_t xtal_freq, int pll_freq)
{
    /* Digital part */
    clk_ll_bbpll_set_freq_mhz(pll_freq);
    /* Analog part */
    /* BBPLL CALIBRATION START */
    regi2c_ctrl_ll_bbpll_calibration_start();
    clk_ll_bbpll_set_config(pll_freq, xtal_freq);
    /* WAIT CALIBRATION DONE */
    while(!regi2c_ctrl_ll_bbpll_calibration_is_done());
    esp_rom_delay_us(10);
    /* BBPLL CALIBRATION STOP */
    regi2c_ctrl_ll_bbpll_calibration_stop();

    s_cur_pll_freq = pll_freq;
}

/**
 * Switch to one of PLL-based frequencies. Current frequency can be XTAL or PLL.
 * PLL must already be enabled.
 * @param cpu_freq new CPU frequency
 */
static void rtc_clk_cpu_freq_to_pll_mhz(int cpu_freq_mhz)
{

    /* There are totally 6 LDO slaves(all on by default). At the moment of swithing LDO slave, LDO voltage will also change instantaneously.
     * LDO slave can reduce the voltage change caused by switching frequency.
     * CPU frequency <= 40M : just open 3 LDO slaves; CPU frequency = 80M : open 4 LDO slaves; CPU frequency = 160M : open 5 LDO slaves; CPU frequency = 240M : open 6 LDO slaves;
     *
     * LDO voltage will decrease at the moment of switching from low frequency to high frequency; otherwise, LDO voltage will increase.
     * In order to reduce LDO voltage drop, LDO voltage should rise first then fall.
     */
    int pd_slave = cpu_freq_mhz / 80;
    rtc_cpu_freq_config_t cur_config;
    rtc_clk_cpu_freq_get_config(&cur_config);
    /* cpu_frequency < 240M: dbias = pvt-dig + 2;
     * cpu_frequency = 240M: dbias = pvt-dig + 3;
     */
    if (cpu_freq_mhz > cur_config.freq_mhz) {
        if (cpu_freq_mhz == 240) {
            REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_EXT_RTC_DREG, g_rtc_dbias_pvt_240m);
            REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_EXT_DIG_DREG, g_dig_dbias_pvt_240m);
            esp_rom_delay_us(40);
        }
        REG_SET_FIELD(RTC_CNTL_DATE_REG, RTC_CNTL_SLAVE_PD,  DEFAULT_LDO_SLAVE >> pd_slave);
    }

    clk_ll_cpu_set_freq_mhz_from_pll(cpu_freq_mhz);
    clk_ll_cpu_set_divider(1);
    /* switch clock source */
    clk_ll_cpu_set_src(SOC_CPU_CLK_SRC_PLL);
    rtc_clk_apb_freq_update(80 * MHZ);
    esp_rom_set_cpu_ticks_per_us(cpu_freq_mhz);

    if (cpu_freq_mhz < cur_config.freq_mhz) {
        if (cur_config.freq_mhz == 240) {
            REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_EXT_RTC_DREG, g_rtc_dbias_pvt_non_240m);
            REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_EXT_DIG_DREG, g_dig_dbias_pvt_non_240m);
            esp_rom_delay_us(40);
        }
        REG_SET_FIELD(RTC_CNTL_DATE_REG, RTC_CNTL_SLAVE_PD,  DEFAULT_LDO_SLAVE >> pd_slave);
    }
}

bool rtc_clk_cpu_freq_mhz_to_config(uint32_t freq_mhz, rtc_cpu_freq_config_t *out_config)
{
    uint32_t source_freq_mhz;
    soc_cpu_clk_src_t source;
    uint32_t divider;
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

void rtc_clk_cpu_freq_set_config(const rtc_cpu_freq_config_t *config)
{
    soc_cpu_clk_src_t old_cpu_clk_src = clk_ll_cpu_get_src();
    if (config->source == SOC_CPU_CLK_SRC_XTAL) {
        rtc_clk_cpu_freq_to_xtal(config->freq_mhz, config->div);
        if ((old_cpu_clk_src == SOC_CPU_CLK_SRC_PLL) && !s_bbpll_digi_consumers_ref_count) {
            // We don't turn off the bbpll if some consumers depend on bbpll
            rtc_clk_bbpll_disable();
        }
    } else if (config->source == SOC_CPU_CLK_SRC_PLL) {
        if (old_cpu_clk_src != SOC_CPU_CLK_SRC_PLL) {
            rtc_clk_bbpll_enable();
            rtc_clk_bbpll_configure(rtc_clk_xtal_freq_get(), config->source_freq_mhz);
        }
        rtc_clk_cpu_freq_to_pll_mhz(config->freq_mhz);
    } else if (config->source == SOC_CPU_CLK_SRC_RC_FAST) {
        rtc_clk_cpu_freq_to_8m();
        if ((old_cpu_clk_src == SOC_CPU_CLK_SRC_PLL) && !s_bbpll_digi_consumers_ref_count) {
            // We don't turn off the bbpll if some consumers depend on bbpll
            rtc_clk_bbpll_disable();
        }
    }
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
    }
    break;
    case SOC_CPU_CLK_SRC_PLL: {
        freq_mhz = clk_ll_cpu_get_freq_mhz_from_pll();
        source_freq_mhz = clk_ll_bbpll_get_freq_mhz();
        if (freq_mhz == CLK_LL_PLL_80M_FREQ_MHZ) {
            div = (source_freq_mhz == CLK_LL_PLL_480M_FREQ_MHZ) ? 6 : 4;
        } else if (freq_mhz == CLK_LL_PLL_160M_FREQ_MHZ) {
            div = (source_freq_mhz == CLK_LL_PLL_480M_FREQ_MHZ) ? 3 : 2;
        } else if (freq_mhz == CLK_LL_PLL_240M_FREQ_MHZ  && source_freq_mhz == CLK_LL_PLL_480M_FREQ_MHZ) {
            div = 2;
        } else {
            ESP_HW_LOGE(TAG, "unsupported frequency configuration");
            return;
        }
        break;
    }
    case SOC_CPU_CLK_SRC_RC_FAST:
        source_freq_mhz = 20;
        div = 1;
        freq_mhz = source_freq_mhz;
        break;
    default:
        ESP_HW_LOGE(TAG, "unsupported frequency configuration");
        return;
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
    rtc_clk_cpu_set_to_default_config();
    rtc_clk_bbpll_disable();
}

void rtc_clk_cpu_set_to_default_config(void)
{
    int freq_mhz = (int)rtc_clk_xtal_freq_get();

    rtc_clk_cpu_freq_to_xtal(freq_mhz, 1);
}

/**
 * Switch to use XTAL as the CPU clock source.
 * Must satisfy: cpu_freq = XTAL_FREQ / div.
 * Does not disable the PLL.
 *
 * Public function for testing only.
 */
void rtc_clk_cpu_freq_to_xtal(int cpu_freq, int div)
{
    rtc_cpu_freq_config_t cur_config;
    rtc_clk_cpu_freq_get_config(&cur_config);

    esp_rom_set_cpu_ticks_per_us(cpu_freq);
    /* Set divider from XTAL to APB clock. Need to set divider to 1 (reg. value 0) first. */
    clk_ll_cpu_set_divider(1);
    clk_ll_cpu_set_divider(div);
    /* switch clock source */
    clk_ll_cpu_set_src(SOC_CPU_CLK_SRC_XTAL);
    rtc_clk_apb_freq_update(cpu_freq * MHZ);

    if (cur_config.freq_mhz == 240) {
        REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_EXT_RTC_DREG, g_rtc_dbias_pvt_non_240m);
        REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_EXT_DIG_DREG, g_dig_dbias_pvt_non_240m);
        esp_rom_delay_us(40);
    }

    REG_SET_FIELD(RTC_CNTL_DATE_REG, RTC_CNTL_SLAVE_PD,  DEFAULT_LDO_SLAVE);
}

static void rtc_clk_cpu_freq_to_8m(void)
{
    assert(0 && "LDO dbias need to modified");
    esp_rom_set_cpu_ticks_per_us(20);
    clk_ll_cpu_set_divider(1);
    clk_ll_cpu_set_src(SOC_CPU_CLK_SRC_RC_FAST);
    rtc_clk_apb_freq_update(SOC_CLK_RC_FAST_FREQ_APPROX);
    REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_EXT_RTC_DREG, g_rtc_dbias_pvt_non_240m);
    REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_EXT_DIG_DREG, g_dig_dbias_pvt_non_240m);
    REG_SET_FIELD(RTC_CNTL_DATE_REG, RTC_CNTL_SLAVE_PD,  DEFAULT_LDO_SLAVE);
}

rtc_xtal_freq_t rtc_clk_xtal_freq_get(void)
{
    uint32_t xtal_freq_mhz = clk_ll_xtal_load_freq_mhz();
    if (xtal_freq_mhz == 0) {
        ESP_HW_LOGW(TAG, "invalid RTC_XTAL_FREQ_REG value, assume 40MHz");
        return RTC_XTAL_FREQ_40M;
    }
    return (rtc_xtal_freq_t)xtal_freq_mhz;
}

void rtc_clk_xtal_freq_update(rtc_xtal_freq_t xtal_freq)
{
    clk_ll_xtal_store_freq_mhz(xtal_freq);
}

void rtc_clk_apb_freq_update(uint32_t apb_freq)
{
    s_apb_freq = apb_freq;
}

uint32_t rtc_clk_apb_freq_get(void)
{
    return s_apb_freq;
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

// Workaround for bootloader not calibrated well issue.
// Placed in IRAM because disabling BBPLL may influence the cache
void rtc_clk_recalib_bbpll(void)
{
    rtc_cpu_freq_config_t old_config;
    rtc_clk_cpu_freq_get_config(&old_config);

    // There are two paths we arrive here: 1. CPU reset. 2. Other reset reasons.
    // - For other reasons, the bootloader will set CPU source to BBPLL and enable it. But there are calibration issues.
    //   Turn off the BBPLL and do calibration again to fix the issue.
    // - For CPU reset, the CPU source will be set to XTAL, while the BBPLL is kept to meet USB Serial JTAG's
    //   requirements. In this case, we don't touch BBPLL to avoid USJ disconnection.
    if (old_config.source == SOC_CPU_CLK_SRC_PLL) {
        rtc_clk_cpu_freq_set_xtal();
        rtc_clk_cpu_freq_set_config(&old_config);
    }
}


/* Name used in libphy.a:phy_chip_v7.o
 * TODO: update the library to use rtc_clk_xtal_freq_get
 */
rtc_xtal_freq_t rtc_get_xtal(void) __attribute__((alias("rtc_clk_xtal_freq_get")));

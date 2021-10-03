/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "esp32h2/rom/ets_sys.h"
#include "esp32h2/rom/rtc.h"
#include "esp32h2/rom/uart.h"
#include "esp32h2/rom/gpio.h"
#include "soc/rtc.h"
#include "i2c_bbpll.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/efuse_reg.h"
#include "soc/syscon_reg.h"
#include "soc/system_reg.h"
#include "regi2c_ctrl.h"
#include "soc_log.h"
#include "rtc_clk_common.h"
#include "esp_rom_sys.h"

static const char *TAG = "rtc_clk";

#define RTC_PLL_FREQ_96M    96
#define RTC_OSC_FREQ_RC8M   18
#define DELAY_RTC_CLK_SWITCH 5
#define RTC_CNTL_ANA_CONF0_CAL_REG 0x6000e040
#define RTC_CNTL_ANA_CONF0_CAL_START BIT(2)
#define RTC_CNTL_ANA_CONF0_CAL_STOP BIT(3)
#define RTC_CNTL_ANA_CONF0_CAL_DONE BIT(24)

// Current PLL frequency, in 96MHZ. Zero if PLL is not enabled.
static int s_cur_pll_freq;

void rtc_clk_cpu_freq_to_8m(void);
static uint32_t rtc_clk_ahb_freq_set(uint32_t div);

void rtc_clk_32k_enable_internal(x32k_config_t cfg)
{
    REG_SET_FIELD(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_DAC_XTAL_32K, cfg.dac);
    REG_SET_FIELD(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_DRES_XTAL_32K, cfg.dres);
    REG_SET_FIELD(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_DGM_XTAL_32K, cfg.dgm);
    REG_SET_FIELD(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_DBUF_XTAL_32K, cfg.dbuf);
    SET_PERI_REG_MASK(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_XPD_XTAL_32K);
}

void rtc_clk_32k_enable(bool enable)
{
    if (enable) {
        /* need to hangup gpio0 & 1 before enable xtal_32k */
        rtc_gpio_hangup(0);
        rtc_gpio_hangup(1);
        x32k_config_t cfg = X32K_CONFIG_DEFAULT();
        rtc_clk_32k_enable_internal(cfg);
    } else {
        SET_PERI_REG_MASK(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_XTAL32K_XPD_FORCE);
        CLEAR_PERI_REG_MASK(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_XPD_XTAL_32K);
    }
}

void rtc_clk_rc32k_dfreq(uint32_t dfreq)
{
    REG_SET_FIELD(RTC_CNTL_RC32K_CTRL_REG, RTC_CNTL_RC32K_DFREQ, dfreq);
}

void rtc_clk_rc32k_enable(bool enable)
{
    rc32k_config_t cfg = RC32K_CONFIG_DEFAULT();
    rtc_clk_rc32k_dfreq(cfg.dfreq);
    REG_SET_FIELD(RTC_CNTL_RC32K_CTRL_REG, RTC_CNTL_RC32K_XPD, enable);
}

void rtc_clk_32k_enable_external(void)
{
    rtc_clk_32k_enable(true);
}

void rtc_clk_32k_bootstrap(uint32_t cycle)
{
    /* No special bootstrapping needed for ESP32-H2, 'cycle' argument is to keep the signature
     * same as for the ESP32. Just enable the XTAL here.
     */
    (void) cycle;
    rtc_clk_32k_enable(true);
}

bool rtc_clk_32k_enabled(void)
{
    uint32_t xtal_conf = READ_PERI_REG(RTC_CNTL_EXT_XTL_CONF_REG);
    /* If xtal xpd is controlled by software */
    bool xtal_xpd_sw = (xtal_conf & RTC_CNTL_XTAL32K_XPD_FORCE) >> RTC_CNTL_XTAL32K_XPD_FORCE_S;
    /* If xtal xpd software control is on */
    bool xtal_xpd_st = (xtal_conf & RTC_CNTL_XPD_XTAL_32K) >> RTC_CNTL_XPD_XTAL_32K_S;
    bool disabled = xtal_xpd_sw && !xtal_xpd_st;
    return !disabled;
}

void rtc_clk_slow_freq_set(rtc_slow_freq_t slow_freq)
{
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ANA_CLK_RTC_SEL, slow_freq);
    rtc_clk_32k_enable((slow_freq == RTC_SLOW_FREQ_32K_XTAL) ? 1 : 0);
    rtc_clk_rc32k_enable((slow_freq == RTC_SLOW_FREQ_RC32K) ? 1 : 0);
    esp_rom_delay_us(DELAY_SLOW_CLK_SWITCH);
}

rtc_slow_freq_t rtc_clk_slow_freq_get(void)
{
    return REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ANA_CLK_RTC_SEL);
}

uint32_t rtc_clk_slow_freq_get_hz(void)
{
    switch (rtc_clk_slow_freq_get()) {
    case RTC_SLOW_FREQ_RTC: return RTC_SLOW_CLK_FREQ_150K;
    case RTC_SLOW_FREQ_32K_XTAL: return RTC_SLOW_CLK_FREQ_32K;
    case RTC_SLOW_FREQ_RC32K: return RTC_SLOW_CLK_FREQ_RC32;
    }
    return 0;
}

void rtc_clk_fast_freq_set(rtc_fast_freq_t fast_freq)
{
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_FAST_CLK_RTC_SEL, fast_freq);
    esp_rom_delay_us(DELAY_FAST_CLK_SWITCH);
}

rtc_fast_freq_t rtc_clk_fast_freq_get(void)
{
    return REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_FAST_CLK_RTC_SEL);
}

static void rtc_clk_bbpll_disable(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BB_I2C_FORCE_PD |
                      RTC_CNTL_BBPLL_FORCE_PD | RTC_CNTL_BBPLL_I2C_FORCE_PD);
    s_cur_pll_freq = 0;
}

static void rtc_clk_bbpll_enable(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BB_I2C_FORCE_PD |
                        RTC_CNTL_BBPLL_FORCE_PD | RTC_CNTL_BBPLL_I2C_FORCE_PD);
}
static void rtc_clk_bbpll_cali_stop(void)
{
    while (!REG_GET_BIT(RTC_CNTL_ANA_CONF0_CAL_REG, RTC_CNTL_ANA_CONF0_CAL_DONE));
    REG_CLR_BIT(RTC_CNTL_ANA_CONF0_CAL_REG, RTC_CNTL_ANA_CONF0_CAL_STOP);
    REG_SET_BIT(RTC_CNTL_ANA_CONF0_CAL_REG, RTC_CNTL_ANA_CONF0_CAL_START);

}

void rtc_clk_bbpll_configure(rtc_xtal_freq_t xtal_freq, int pll_freq)
{
    uint8_t div_ref;
    uint8_t div5_0;
    if ((pll_freq == RTC_PLL_FREQ_96M) && (xtal_freq == RTC_XTAL_FREQ_32M)) {
        /* Configure 96M PLL */
        div_ref = 0;
        div5_0 = 1;
    } else {
        div_ref = 0;
        div5_0 = 1;
        SOC_LOGE(TAG, "invalid pll frequency");
    }

    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_OC_REF_DIV, div_ref);
    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_OC_DIV, div5_0); //I2C_BBPLL_OC_DIV_5_0
    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_OC_DHREF_SEL, 3); // need update to 3 since s2
    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_OC_DLREF_SEL, 1);
    s_cur_pll_freq = pll_freq;
}

/**
 * Switch to one of PLL-based frequencies. Current frequency can be XTAL or PLL.
 * PLL must already be enabled.
 * @param cpu_freq new CPU frequency
 */
static void rtc_clk_cpu_freq_to_pll_mhz(int cpu_freq_mhz)
{
    int div = 1;
    if (RTC_PLL_FREQ_96M % cpu_freq_mhz == 0) {
        div = RTC_PLL_FREQ_96M / cpu_freq_mhz;
    } else {
        SOC_LOGE(TAG, "invalid frequency");
        abort();
    }
    rtc_clk_cpu_freq_set(DPORT_SOC_CLK_SEL_PLL, div - 1);
    if (cpu_freq_mhz > RTC_XTAL_FREQ_32M) {
        rtc_clk_ahb_freq_set(2);
    } else {
        rtc_clk_ahb_freq_set(1);
    }

    rtc_clk_apb_freq_update(rtc_clk_apb_freq_get());
    ets_update_cpu_frequency(cpu_freq_mhz);
}

bool rtc_clk_cpu_freq_mhz_to_config(uint32_t freq_mhz, rtc_cpu_freq_config_t *out_config)
{
    uint32_t source_freq_mhz;
    rtc_cpu_freq_src_t source;
    uint32_t divider;
    uint32_t xtal_freq = (uint32_t) rtc_clk_xtal_freq_get();
    if (freq_mhz > xtal_freq) {
        source = RTC_CPU_FREQ_SRC_PLL;
        source_freq_mhz = RTC_PLL_FREQ_96M;
        divider = RTC_PLL_FREQ_96M / freq_mhz;
        rtc_clk_ahb_freq_set(2);
    } else {
        source = root_clk_get();
        source_freq_mhz = root_clk_slt(source);
        divider = source_freq_mhz / freq_mhz;
        rtc_clk_ahb_freq_set(1);
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
    uint32_t src_freq_mhz = root_clk_slt(config->source);
    uint32_t div = src_freq_mhz / (config->freq_mhz);
    rtc_clk_cpu_freq_set(config->source, div);
}

void rtc_clk_cpu_freq_get_config(rtc_cpu_freq_config_t *out_config)
{
    rtc_cpu_freq_src_t source;
    uint32_t source_freq_mhz;
    uint32_t div;
    uint32_t freq_mhz;
    uint32_t soc_clk_sel = REG_GET_FIELD(SYSTEM_SYSCLK_CONF_REG, SYSTEM_SOC_CLK_SEL);
    switch (soc_clk_sel) {
    case DPORT_SOC_CLK_SEL_XTAL: {
        source = RTC_CPU_FREQ_SRC_XTAL;
        div = REG_GET_FIELD(SYSTEM_CPUCLK_CONF_REG, SYSTEM_PRE_DIV_CNT) + 1;
        source_freq_mhz = (uint32_t) rtc_clk_xtal_freq_get();
        freq_mhz = source_freq_mhz / div;
        break;
    }
    case DPORT_SOC_CLK_SEL_PLL: {
        source = RTC_CPU_FREQ_SRC_PLL;
        div = REG_GET_FIELD(SYSTEM_CPUCLK_CONF_REG, SYSTEM_PRE_DIV_CNT) + 1;
        source_freq_mhz = RTC_PLL_FREQ_96M;
        freq_mhz = source_freq_mhz / div;
        break;
    }
    case DPORT_SOC_CLK_SEL_8M: {
        source = RTC_CPU_FREQ_SRC_8M;
        source_freq_mhz = RTC_OSC_FREQ_RC8M;
        div = REG_GET_FIELD(SYSTEM_CPUCLK_CONF_REG, SYSTEM_PRE_DIV_CNT) + 1;
        freq_mhz = source_freq_mhz / div;
        break;
    }
    case DPORT_SOC_CLK_SEL_XTAL_D2: {
        source = RTC_CPU_FREQ_SRC_XTAL_D2;
        div = REG_GET_FIELD(SYSTEM_CPUCLK_CONF_REG, SYSTEM_PRE_DIV_CNT) + 1;
        source_freq_mhz = (uint32_t) rtc_clk_xtal_freq_get();
        freq_mhz = source_freq_mhz / div / 2;
        break;
    }
    default: {
        SOC_LOGE(TAG, "unsupported frequency configuration");
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
    if (config->source == RTC_CPU_FREQ_SRC_XTAL) {
        rtc_clk_cpu_freq_to_xtal(config->freq_mhz, config->div);
    } else if (config->source == RTC_CPU_FREQ_SRC_PLL &&
               s_cur_pll_freq == config->source_freq_mhz) {
        rtc_clk_cpu_freq_to_pll_mhz(config->freq_mhz);
    } else {
        /* fallback */
        rtc_clk_cpu_freq_set_config(config);
    }
}

void rtc_clk_cpu_freq_set_xtal(void)
{
    int freq_mhz = (int) rtc_clk_xtal_freq_get();
    rtc_clk_cpu_freq_to_xtal(freq_mhz, 1);
    rtc_clk_bbpll_disable();
}

/**
 * Switch to XTAL frequency. Does not disable the PLL.
 */
void rtc_clk_cpu_freq_to_xtal(int freq, int div)
{
    ets_update_cpu_frequency(freq);
    /* Set divider from XTAL to APB clock. Need to set divider to 1 (reg. value 0) first. */
    rtc_clk_cpu_freq_set(DPORT_SOC_CLK_SEL_XTAL, div);
    /* no need to adjust the REF_TICK */
    /* switch clock source */
    rtc_clk_apb_freq_update(rtc_clk_apb_freq_get());
}

void rtc_clk_cpu_freq_to_8m(void)
{
    ets_update_cpu_frequency(RTC_OSC_FREQ_RC8M);
    root_clk_slt(DPORT_SOC_CLK_SEL_8M);
    rtc_clk_apb_freq_update(rtc_clk_apb_freq_get());
}

rtc_xtal_freq_t rtc_clk_xtal_freq_get(void)
{
    uint32_t xtal_freq_reg = READ_PERI_REG(RTC_XTAL_FREQ_REG);
    if (!clk_val_is_valid(xtal_freq_reg)) {
        SOC_LOGW(TAG, "invalid RTC_XTAL_FREQ_REG value: 0x%08x", xtal_freq_reg);
        return RTC_XTAL_FREQ_32M;
    }
    return reg_val_to_clk_val(xtal_freq_reg);
}

void rtc_clk_xtal_freq_update(rtc_xtal_freq_t xtal_freq)
{
    WRITE_PERI_REG(RTC_XTAL_FREQ_REG, clk_val_to_reg_val(xtal_freq));
}

void rtc_clk_apb_freq_update(uint32_t apb_freq)
{
    WRITE_PERI_REG(RTC_APB_FREQ_REG, clk_val_to_reg_val(apb_freq >> 12));
}


uint32_t rtc_clk_apb_freq_get(void)
{
    uint32_t apb_div = REG_GET_FIELD(SYSTEM_BUSCLK_CONF_REG, SYSTEM_APB_DIV_NUM) + 1;
    return rtc_clk_ahb_freq_get() / apb_div;
}

uint32_t rtc_clk_ahb_freq_get()
{
    rtc_cpu_freq_config_t cpu_config;
    uint32_t ahb_div = REG_GET_FIELD(SYSTEM_BUSCLK_CONF_REG, SYSTEM_AHB_DIV_NUM) + 1;
    rtc_clk_cpu_freq_get_config(&cpu_config) ;
    return   cpu_config.freq_mhz / ahb_div;
}

uint32_t rtc_clk_ahb_freq_set(uint32_t div)
{
    REG_SET_FIELD(SYSTEM_BUSCLK_CONF_REG, SYSTEM_AHB_DIV_NUM, div - 1);
    return rtc_clk_ahb_freq_get();
}

uint32_t rtc_clk_apb_freq_set(uint32_t div)
{
    REG_SET_FIELD(SYSTEM_BUSCLK_CONF_REG, SYSTEM_APB_DIV_NUM, div - 1);
    return rtc_clk_apb_freq_get();
}

void rtc_clk_cpu_freq_set(uint32_t source, uint32_t div)
{
    if (root_clk_get() != source) {
        root_clk_slt(source);
    }
    REG_SET_FIELD(SYSTEM_CPUCLK_CONF_REG, SYSTEM_CPU_DIV_NUM, div - 1);
}

void rtc_clk_divider_set(uint32_t div)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_SLOW_CLK_CONF_REG, RTC_CNTL_ANA_CLK_DIV_VLD);
    REG_SET_FIELD(RTC_CNTL_SLOW_CLK_CONF_REG, RTC_CNTL_ANA_CLK_DIV, div - 1);
    SET_PERI_REG_MASK(RTC_CNTL_SLOW_CLK_CONF_REG, RTC_CNTL_ANA_CLK_DIV_VLD);
}

void rtc_clk_8m_divider_set(uint32_t div)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL_VLD);
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL, div - 1);
    SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL_VLD);
}

void rtc_dig_clk8m_enable(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_CLK8M_EN);
    esp_rom_delay_us(DELAY_RTC_CLK_SWITCH);
}

void rtc_dig_clk8m_disable(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_CLK8M_EN);
    esp_rom_delay_us(DELAY_RTC_CLK_SWITCH);
}

uint32_t read_spll_freq(void)
{
    return REG_GET_FIELD(SYSTEM_SYSCLK_CONF_REG, SYSTEM_SPLL_FREQ);
}

uint32_t read_xtal_freq(void)
{
    return REG_GET_FIELD(SYSTEM_SYSCLK_CONF_REG, SYSTEM_CLK_XTAL_FREQ);
}

/* Select clock root source for esp32h2. return source clk freq_mhz
 */
uint32_t root_clk_slt(uint32_t source)
{
    uint32_t root_clk_freq_mhz;
    switch (source) {
    case RTC_CPU_FREQ_SRC_XTAL:
        root_clk_freq_mhz = RTC_XTAL_FREQ_32M;
        rtc_clk_bbpll_disable();
        break;
    case RTC_CPU_FREQ_SRC_PLL:
        // SPLL_ENABLE
        root_clk_freq_mhz = RTC_PLL_FREQ_96M;
        rtc_clk_bbpll_enable();
        rtc_clk_bbpll_configure(RTC_XTAL_FREQ_32M, root_clk_freq_mhz);
        rtc_clk_bbpll_cali_stop();
        break;
    case RTC_CPU_FREQ_SRC_8M:
        root_clk_freq_mhz = RTC_OSC_FREQ_RC8M;
        rtc_dig_clk8m_enable();
        rtc_clk_8m_divider_set(1);
        rtc_clk_bbpll_disable();
        break;
    case RTC_CPU_FREQ_SRC_XTAL_D2:
        root_clk_freq_mhz = RTC_XTAL_FREQ_32M / 2;
        rtc_clk_bbpll_disable();
        break;
    default:
        SOC_LOGE(TAG, "unsupported source clk configuration");
        root_clk_freq_mhz = RTC_XTAL_FREQ_32M;
        rtc_clk_bbpll_disable();
        source = 0;
        break;
    }
    REG_SET_FIELD(SYSTEM_SYSCLK_CONF_REG, SYSTEM_SOC_CLK_SEL, source);
    return root_clk_freq_mhz;
}

uint32_t root_clk_get()
{
    uint32_t src_slt = REG_GET_FIELD(SYSTEM_SYSCLK_CONF_REG, SYSTEM_SOC_CLK_SEL);
    return src_slt;
}

/* Name used in libphy.a:phy_chip_v7.o
 * TODO: update the library to use rtc_clk_xtal_freq_get
 */
rtc_xtal_freq_t rtc_get_xtal(void) __attribute__((alias("rtc_clk_xtal_freq_get")));

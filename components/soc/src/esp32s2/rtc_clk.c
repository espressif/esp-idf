// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "esp32s2/rom/ets_sys.h"
#include "esp32s2/rom/rtc.h"
#include "esp32s2/rom/uart.h"
#include "esp32s2/rom/gpio.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_io_reg.h"
#include "soc/sens_reg.h"
#include "soc/dport_reg.h"
#include "soc/efuse_reg.h"
#include "soc/syscon_reg.h"
#include "regi2c_ctrl.h"
#include "soc_log.h"
#include "rtc_clk_common.h"
#include "sdkconfig.h"
#include "xtensa/core-macros.h"

static const char *TAG = "rtc_clk";

#define RTC_PLL_FREQ_320M   320
#define RTC_PLL_FREQ_480M   480

// Current PLL frequency, in MHZ (320 or 480). Zero if PLL is not enabled.
static int s_cur_pll_freq;

static void rtc_clk_cpu_freq_to_8m(void);

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
        SET_PERI_REG_MASK(RTC_IO_XTAL_32P_PAD_REG, RTC_IO_X32P_MUX_SEL);
        SET_PERI_REG_MASK(RTC_IO_XTAL_32N_PAD_REG, RTC_IO_X32N_MUX_SEL);
        x32k_config_t cfg = X32K_CONFIG_DEFAULT();
        rtc_clk_32k_enable_internal(cfg);
    } else {
        SET_PERI_REG_MASK(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_XTAL32K_XPD_FORCE);
        CLEAR_PERI_REG_MASK(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_XPD_XTAL_32K);
    }
}

void rtc_clk_32k_enable_external(void)
{
    SET_PERI_REG_MASK(RTC_IO_XTAL_32P_PAD_REG, RTC_IO_X32P_MUX_SEL);
    SET_PERI_REG_MASK(RTC_IO_XTAL_32N_PAD_REG, RTC_IO_X32N_MUX_SEL);
    /* TODO: external 32k source may need different settings */
    x32k_config_t cfg = X32K_CONFIG_DEFAULT();
    rtc_clk_32k_enable_internal(cfg);
}

void rtc_clk_32k_bootstrap(uint32_t cycle)
{
    /* No special bootstrapping needed for ESP32-S2, 'cycle' argument is to keep the signature
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

void rtc_clk_8m_enable(bool clk_8m_en, bool d256_en)
{
    if (clk_8m_en) {
        CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M);
        /* no need to wait once enabled by software */
        REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_CK8M_WAIT, RTC_CK8M_ENABLE_WAIT_DEFAULT);
        ets_delay_us(DELAY_8M_ENABLE);
    } else {
        SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M);
        REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_CK8M_WAIT, RTC_CNTL_CK8M_WAIT_DEFAULT);
    }
    /* d256 should be independent configured with 8M
     * Maybe we can split this function into 8m and dmd256
     */
    if (d256_en) {
        CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M_DIV);
    } else {
        SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M_DIV);
    }
}

bool rtc_clk_8m_enabled(void)
{
    return GET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M) == 0;
}

bool rtc_clk_8md256_enabled(void)
{
    return GET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M_DIV) == 0;
}

void rtc_clk_apll_enable(bool enable, uint32_t sdm0, uint32_t sdm1, uint32_t sdm2, uint32_t o_div)
{
    REG_SET_FIELD(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PLLA_FORCE_PD, enable ? 0 : 1);
    REG_SET_FIELD(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PLLA_FORCE_PU, enable ? 1 : 0);

    if (enable) {
        REGI2C_WRITE_MASK(I2C_APLL, I2C_APLL_DSDM2, sdm2);
        REGI2C_WRITE_MASK(I2C_APLL, I2C_APLL_DSDM0, sdm0);
        REGI2C_WRITE_MASK(I2C_APLL, I2C_APLL_DSDM1, sdm1);
        REGI2C_WRITE(I2C_APLL, I2C_APLL_SDM_STOP, APLL_SDM_STOP_VAL_1);
        REGI2C_WRITE(I2C_APLL, I2C_APLL_SDM_STOP, APLL_SDM_STOP_VAL_2_REV1);
        REGI2C_WRITE_MASK(I2C_APLL, I2C_APLL_OR_OUTPUT_DIV, o_div);

        /* calibration */
        REGI2C_WRITE(I2C_APLL, I2C_APLL_IR_CAL_DELAY, APLL_CAL_DELAY_1);
        REGI2C_WRITE(I2C_APLL, I2C_APLL_IR_CAL_DELAY, APLL_CAL_DELAY_2);
        REGI2C_WRITE(I2C_APLL, I2C_APLL_IR_CAL_DELAY, APLL_CAL_DELAY_3);

        /* wait for calibration end */
        while (!(REGI2C_READ_MASK(I2C_APLL, I2C_APLL_OR_CAL_END))) {
            /* use esp_rom_delay_us so the RTC bus doesn't get flooded */
            ets_delay_us(1);
        }
    }
}

void rtc_clk_slow_freq_set(rtc_slow_freq_t slow_freq)
{
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ANA_CLK_RTC_SEL, slow_freq);

    /* Why we need to connect this clock to digital?
     * Or maybe this clock should be connected to digital when xtal 32k clock is enabled instead?
     */
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_XTAL32K_EN,
                  (slow_freq == RTC_SLOW_FREQ_32K_XTAL) ? 1 : 0);

    /* The clk_8m_d256 will be closed when rtc_state in SLEEP,
    so if the slow_clk is 8md256, clk_8m must be force power on
    */
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_FORCE_PU, (slow_freq == RTC_SLOW_FREQ_8MD256) ? 1 : 0);
    ets_delay_us(DELAY_SLOW_CLK_SWITCH);
}

rtc_slow_freq_t rtc_clk_slow_freq_get(void)
{
    return REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ANA_CLK_RTC_SEL);
}

uint32_t rtc_clk_slow_freq_get_hz(void)
{
    switch (rtc_clk_slow_freq_get()) {
    case RTC_SLOW_FREQ_RTC: return RTC_SLOW_CLK_FREQ_90K;
    case RTC_SLOW_FREQ_32K_XTAL: return RTC_SLOW_CLK_FREQ_32K;
    case RTC_SLOW_FREQ_8MD256: return RTC_SLOW_CLK_FREQ_8MD256;
    }
    return 0;
}

void rtc_clk_fast_freq_set(rtc_fast_freq_t fast_freq)
{
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_FAST_CLK_RTC_SEL, fast_freq);
    ets_delay_us(DELAY_FAST_CLK_SWITCH);
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

void rtc_clk_bbpll_configure(rtc_xtal_freq_t xtal_freq, int pll_freq)
{
    uint8_t div_ref;
    uint8_t div7_0;
    uint8_t dr1;
    uint8_t dr3;
    uint8_t dchgp;
    uint8_t dcur;

    assert(xtal_freq == RTC_XTAL_FREQ_40M);

    if (pll_freq == RTC_PLL_FREQ_480M) {
        /* Clear this register to let the digital part know 480M PLL is used */
        SET_PERI_REG_MASK(DPORT_CPU_PER_CONF_REG, DPORT_PLL_FREQ_SEL);
        /* Configure 480M PLL */
        div_ref = 0;
        div7_0 = 8;
        dr1 = 0;
        dr3 = 0;
        dchgp = 5;
        dcur = 4;
        REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_MODE_HF, 0x6B);
    } else {
        /* Clear this register to let the digital part know 320M PLL is used */
        CLEAR_PERI_REG_MASK(DPORT_CPU_PER_CONF_REG, DPORT_PLL_FREQ_SEL);
        /* Configure 320M PLL */
        div_ref = 0;
        div7_0 = 4;
        dr1 = 0;
        dr3 = 0;
        dchgp = 5;
        dcur = 5;
        REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_MODE_HF, 0x69);
    }
    uint8_t i2c_bbpll_lref  = (dchgp << I2C_BBPLL_OC_DCHGP_LSB) | (div_ref);
    uint8_t i2c_bbpll_div_7_0 = div7_0;
    uint8_t i2c_bbpll_dcur = (2 << I2C_BBPLL_OC_DLREF_SEL_LSB ) | (1 << I2C_BBPLL_OC_DHREF_SEL_LSB) | dcur;
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_OC_REF_DIV, i2c_bbpll_lref);
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_OC_DIV_7_0, i2c_bbpll_div_7_0);
    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_OC_DR1, dr1);
    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_OC_DR3, dr3);
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_OC_DCUR, i2c_bbpll_dcur);

    // Enable calibration by software
    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_IR_CAL_ENX_CAP, 1);
    for (int ext_cap = 0; ext_cap < 16; ext_cap++) {
        uint8_t cal_result;
        REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_IR_CAL_EXT_CAP, ext_cap);
        cal_result = REGI2C_READ_MASK(I2C_BBPLL, I2C_BBPLL_OR_CAL_CAP);
        if (cal_result == 0) {
            break;
        }
        if (ext_cap == 15) {
            SOC_LOGE(TAG, "BBPLL SOFTWARE CAL FAIL");
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
    int dbias = DIG_DBIAS_80M_160M;
    int per_conf = DPORT_CPUPERIOD_SEL_80;
    if (cpu_freq_mhz == 80) {
        /* nothing to do */
    } else if (cpu_freq_mhz == 160) {
        per_conf = DPORT_CPUPERIOD_SEL_160;
    } else if (cpu_freq_mhz == 240) {
        dbias = DIG_DBIAS_240M;
        per_conf = DPORT_CPUPERIOD_SEL_240;
    } else {
        SOC_LOGE(TAG, "invalid frequency");
        abort();
    }
    REG_SET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_CPUPERIOD_SEL, per_conf);
    REG_SET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_PRE_DIV_CNT, 0);
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, dbias);
    REG_SET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_SOC_CLK_SEL, DPORT_SOC_CLK_SEL_PLL);
    rtc_clk_apb_freq_update(80 * MHZ);
    ets_update_cpu_frequency(cpu_freq_mhz);
}

bool rtc_clk_cpu_freq_mhz_to_config(uint32_t freq_mhz, rtc_cpu_freq_config_t* out_config)
{
    uint32_t source_freq_mhz;
    rtc_cpu_freq_src_t source;
    uint32_t divider;
    uint32_t real_freq_mhz;

    uint32_t xtal_freq = (uint32_t) rtc_clk_xtal_freq_get();
    if (freq_mhz <= xtal_freq) {
        divider = xtal_freq / freq_mhz;
        real_freq_mhz = (xtal_freq + divider / 2) / divider; /* round */
        if (real_freq_mhz != freq_mhz) {
            // no suitable divider
            return false;
        }

        source_freq_mhz = xtal_freq;
        source = RTC_CPU_FREQ_SRC_XTAL;
    } else if (freq_mhz == 80) {
        real_freq_mhz = freq_mhz;
        source = RTC_CPU_FREQ_SRC_PLL;
        source_freq_mhz = RTC_PLL_FREQ_480M;
        divider = 6;
    } else if (freq_mhz == 160) {
        real_freq_mhz = freq_mhz;
        source = RTC_CPU_FREQ_SRC_PLL;
        source_freq_mhz = RTC_PLL_FREQ_480M;
        divider = 3;
    } else if (freq_mhz == 240) {
        real_freq_mhz = freq_mhz;
        source = RTC_CPU_FREQ_SRC_PLL;
        source_freq_mhz = RTC_PLL_FREQ_480M;
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
    rtc_xtal_freq_t xtal_freq = rtc_clk_xtal_freq_get();
    uint32_t soc_clk_sel = REG_GET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_SOC_CLK_SEL);
    if (soc_clk_sel != DPORT_SOC_CLK_SEL_XTAL) {
        rtc_clk_cpu_freq_to_xtal(xtal_freq, 1);
    }
    if (soc_clk_sel == DPORT_SOC_CLK_SEL_PLL) {
        rtc_clk_bbpll_disable();
    }
    if (config->source == RTC_CPU_FREQ_SRC_XTAL) {
        if (config->div > 1) {
            rtc_clk_cpu_freq_to_xtal(config->freq_mhz, config->div);
        }
    } else if (config->source == RTC_CPU_FREQ_SRC_PLL) {
        rtc_clk_bbpll_enable();
        rtc_clk_bbpll_configure(rtc_clk_xtal_freq_get(), config->source_freq_mhz);
        rtc_clk_cpu_freq_to_pll_mhz(config->freq_mhz);
    } else if (config->source == RTC_CPU_FREQ_SRC_8M) {
        rtc_clk_cpu_freq_to_8m();
    }
}

void rtc_clk_cpu_freq_get_config(rtc_cpu_freq_config_t* out_config)
{
    rtc_cpu_freq_src_t source;
    uint32_t source_freq_mhz;
    uint32_t div;
    uint32_t freq_mhz;
    uint32_t soc_clk_sel = REG_GET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_SOC_CLK_SEL);
    switch (soc_clk_sel) {
        case DPORT_SOC_CLK_SEL_XTAL: {
            source = RTC_CPU_FREQ_SRC_XTAL;
            div = REG_GET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_PRE_DIV_CNT) + 1;
            source_freq_mhz = (uint32_t) rtc_clk_xtal_freq_get();
            freq_mhz = source_freq_mhz / div;
        }
        break;
        case DPORT_SOC_CLK_SEL_PLL: {
            source = RTC_CPU_FREQ_SRC_PLL;
            uint32_t cpuperiod_sel = DPORT_REG_GET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_CPUPERIOD_SEL);
            uint32_t pllfreq_sel = DPORT_REG_GET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_PLL_FREQ_SEL);
            source_freq_mhz = (pllfreq_sel) ? RTC_PLL_FREQ_480M : RTC_PLL_FREQ_320M;
            if (cpuperiod_sel == DPORT_CPUPERIOD_SEL_80) {
                div = (source_freq_mhz == RTC_PLL_FREQ_480M) ? 6 : 4;
                freq_mhz = 80;
            } else if (cpuperiod_sel == DPORT_CPUPERIOD_SEL_160) {
                div = (source_freq_mhz == RTC_PLL_FREQ_480M) ? 3 : 2;
                div = 3;
                freq_mhz = 160;
            } else if (cpuperiod_sel == DPORT_CPUPERIOD_SEL_240) {
                div = 2;
                freq_mhz = 240;
            } else {
                SOC_LOGE(TAG, "unsupported frequency configuration");
                abort();
            }
            break;
        }
        case DPORT_SOC_CLK_SEL_8M:
            source = RTC_CPU_FREQ_SRC_8M;
            source_freq_mhz = 8;
            div = 1;
            freq_mhz = source_freq_mhz;
            break;
        case DPORT_SOC_CLK_SEL_APLL:
        default:
            SOC_LOGE(TAG, "unsupported frequency configuration");
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
    REG_SET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_PRE_DIV_CNT, 0);
    REG_SET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_PRE_DIV_CNT, div - 1);
    /* no need to adjust the REF_TICK */
    /* switch clock source */
    REG_SET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_SOC_CLK_SEL, DPORT_SOC_CLK_SEL_XTAL);
    rtc_clk_apb_freq_update(freq * MHZ);
    /* lower the voltage */
    if (freq <= 2) {
        REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_2M);
    } else {
        REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_XTAL);
    }
}

static void rtc_clk_cpu_freq_to_8m(void)
{
    ets_update_cpu_frequency(8);
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_XTAL);
    REG_SET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_PRE_DIV_CNT, 0);
    REG_SET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_SOC_CLK_SEL, DPORT_SOC_CLK_SEL_8M);
    rtc_clk_apb_freq_update(RTC_FAST_CLK_FREQ_8M);
}

rtc_xtal_freq_t rtc_clk_xtal_freq_get(void)
{
    uint32_t xtal_freq_reg = READ_PERI_REG(RTC_XTAL_FREQ_REG);
    if (!clk_val_is_valid(xtal_freq_reg)) {
        SOC_LOGW(TAG, "invalid RTC_XTAL_FREQ_REG value: 0x%08x", xtal_freq_reg);
        return RTC_XTAL_FREQ_40M;
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
    uint32_t freq_hz = reg_val_to_clk_val(READ_PERI_REG(RTC_APB_FREQ_REG)) << 12;
    // round to the nearest MHz
    freq_hz += MHZ / 2;
    uint32_t remainder = freq_hz % MHZ;
    return freq_hz - remainder;
}

void rtc_clk_divider_set(uint32_t div)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_SLOW_CLK_CONF_REG, RTC_CNTL_ANA_CLK_DIV_VLD);
    REG_SET_FIELD(RTC_CNTL_SLOW_CLK_CONF_REG, RTC_CNTL_ANA_CLK_DIV, div);
    SET_PERI_REG_MASK(RTC_CNTL_SLOW_CLK_CONF_REG, RTC_CNTL_ANA_CLK_DIV_VLD);
}

void rtc_clk_8m_divider_set(uint32_t div)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL_VLD);
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL, div);
    SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL_VLD);
}

/* Name used in libphy.a:phy_chip_v7.o
 * TODO: update the library to use rtc_clk_xtal_freq_get
 */
rtc_xtal_freq_t rtc_get_xtal(void) __attribute__((alias("rtc_clk_xtal_freq_get")));

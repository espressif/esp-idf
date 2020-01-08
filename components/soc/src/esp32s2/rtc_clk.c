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
#include "i2c_rtc_clk.h"
#include "soc_log.h"
#include "sdkconfig.h"
#include "xtensa/core-macros.h"

static const char *TAG = "rtc_clk";

/* PLL currently enabled, if any */
typedef enum {
    RTC_PLL_NONE,
    RTC_PLL_320M,
    RTC_PLL_480M
} rtc_pll_t;
static rtc_pll_t s_cur_pll = RTC_PLL_NONE;

/* Current CPU frequency; saved in a variable for faster freq. switching */
static rtc_cpu_freq_t s_cur_freq = RTC_CPU_FREQ_XTAL;

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
        x32k_config_t cfg = X32K_CONFIG_DEFAULT();
        rtc_clk_32k_enable_internal(cfg);
    } else {
        SET_PERI_REG_MASK(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_XTAL32K_XPD_FORCE);
        CLEAR_PERI_REG_MASK(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_XPD_XTAL_32K);
    }
}

void rtc_clk_32k_bootstrap(uint32_t cycle)
{
}

bool rtc_clk_32k_enabled(void)
{
    uint32_t xtal_conf = READ_PERI_REG(RTC_CNTL_EXT_XTL_CONF_REG);
    /* If xtal xpd is controlled by software */
    bool xtal_xpd_sw = (xtal_conf & RTC_CNTL_XTAL32K_XPD_FORCE) >> RTC_CNTL_XTAL32K_XPD_FORCE_S;
    /* If xtal xpd software control is on */
    bool xtal_xpd_st = (xtal_conf & RTC_CNTL_XPD_XTAL_32K) >> RTC_CNTL_XPD_XTAL_32K_S;
    if (xtal_xpd_sw & !xtal_xpd_st) {
        return false;
    } else {
        return true;
    }
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

    /* BIAS I2C not exist any more, but not sure how to get the same effect yet...
     * if (!enable &&
     *   REG_GET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_SOC_CLK_SEL) != DPORT_SOC_CLK_SEL_PLL) {
     *   REG_SET_BIT(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_I2C_FORCE_PD);
     * } else {
     *   REG_CLR_BIT(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_I2C_FORCE_PD);
     * }
     */

    if (enable) {
        /* no need to differentiate ECO chip any more
        uint8_t sdm_stop_val_2 = APLL_SDM_STOP_VAL_2_REV1;
        uint32_t is_rev0 = (GET_PERI_REG_BITS2(EFUSE_BLK0_RDATA3_REG, 1, 15) == 0);
        if (is_rev0) {
            sdm0 = 0;
            sdm1 = 0;
            sdm_stop_val_2 = APLL_SDM_STOP_VAL_2_REV0;
        }
        */
        I2C_WRITEREG_MASK_RTC(I2C_APLL, I2C_APLL_DSDM2, sdm2);
        I2C_WRITEREG_MASK_RTC(I2C_APLL, I2C_APLL_DSDM0, sdm0);
        I2C_WRITEREG_MASK_RTC(I2C_APLL, I2C_APLL_DSDM1, sdm1);
        I2C_WRITEREG_RTC(I2C_APLL, I2C_APLL_SDM_STOP, APLL_SDM_STOP_VAL_1);
        I2C_WRITEREG_RTC(I2C_APLL, I2C_APLL_SDM_STOP, APLL_SDM_STOP_VAL_2_REV1);
        I2C_WRITEREG_MASK_RTC(I2C_APLL, I2C_APLL_OR_OUTPUT_DIV, o_div);

        /* calibration */
        I2C_WRITEREG_RTC(I2C_APLL, I2C_APLL_IR_CAL_DELAY, APLL_CAL_DELAY_1);
        I2C_WRITEREG_RTC(I2C_APLL, I2C_APLL_IR_CAL_DELAY, APLL_CAL_DELAY_2);
        I2C_WRITEREG_RTC(I2C_APLL, I2C_APLL_IR_CAL_DELAY, APLL_CAL_DELAY_3);

        /* wait for calibration end */
        while (!(I2C_READREG_MASK_RTC(I2C_APLL, I2C_APLL_OR_CAL_END))) {
            /* use ets_delay_us so the RTC bus doesn't get flooded */
            ets_delay_us(1);
        }
    }
}

void rtc_clk_set_xtal_wait(void)
{
    /*
     the `xtal_wait` time need 1ms, so we need calibrate slow clk period,
     and `RTC_CNTL_XTL_BUF_WAIT` depend on it.
    */
    rtc_slow_freq_t slow_clk_freq = rtc_clk_slow_freq_get();
    rtc_slow_freq_t rtc_slow_freq_x32k = RTC_SLOW_FREQ_32K_XTAL;
    rtc_slow_freq_t rtc_slow_freq_8MD256 = RTC_SLOW_FREQ_8MD256;
    rtc_cal_sel_t cal_clk = RTC_CAL_RTC_MUX;
    if (slow_clk_freq == (rtc_slow_freq_x32k)) {
        cal_clk = RTC_CAL_32K_XTAL;
    } else if (slow_clk_freq == rtc_slow_freq_8MD256) {
        cal_clk  = RTC_CAL_8MD256;
    }
    uint32_t slow_clk_period = rtc_clk_cal(cal_clk, 2000);
    uint32_t xtal_wait_1ms = 100;
    if (slow_clk_period) {
        xtal_wait_1ms = (1000 << RTC_CLK_CAL_FRACT) / slow_clk_period;
    }
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_XTL_BUF_WAIT, xtal_wait_1ms);
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
    rtc_clk_set_xtal_wait();
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

/* In 7.2.2, cpu can run at 80M/160M/240M if PLL is 480M
 * pll can run 80M/160M is PLL is 320M
 */
#define DR_REG_I2C_MST_BASE             0x3f40E000
#define I2C_MST_ANA_STATE_REG           (DR_REG_I2C_MST_BASE + 0x040)
#define I2C_MST_BBPLL_CAL_END           (BIT(24))
#define I2C_MST_BBPLL_CAL_END_M         (BIT(24))
#define I2C_MST_BBPLL_CAL_END_V         0x1
#define I2C_MST_BBPLL_CAL_END_S         24

void rtc_clk_bbpll_set(rtc_xtal_freq_t xtal_freq, rtc_pll_t pll_freq)
{
    uint8_t div_ref;
    uint8_t div7_0;
    uint8_t dr1;
    uint8_t dr3;
    uint8_t dchgp;
    uint8_t dcur;

    assert(xtal_freq == RTC_XTAL_FREQ_40M);

    if (pll_freq == RTC_PLL_480M) {
        /* Raise the voltage, if needed */
        /* move to 240M logic */
        //REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_80M_160M);
        /* Set this register to let digital know pll is 480M */
        SET_PERI_REG_MASK(DPORT_CPU_PER_CONF_REG, DPORT_PLL_FREQ_SEL);
        /* Configure 480M PLL */
        div_ref = 0;
        div7_0 = 8;
        dr1 = 0;
        dr3 = 0;
        dchgp = 5;
        dcur = 4;
        I2C_WRITEREG_RTC(I2C_BBPLL, I2C_BBPLL_MODE_HF, 0x6B);
    } else {
        /* Raise the voltage */
        //REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_240M);
        //ets_delay_us(DELAY_PLL_DBIAS_RAISE);
        CLEAR_PERI_REG_MASK(DPORT_CPU_PER_CONF_REG, DPORT_PLL_FREQ_SEL);
        /* Configure 480M PLL */
        div_ref = 0;
        div7_0 = 4;
        dr1 = 0;
        dr3 = 0;
        dchgp = 5;
        dcur = 5;
        I2C_WRITEREG_RTC(I2C_BBPLL, I2C_BBPLL_MODE_HF, 0x69);
    }
    uint8_t i2c_bbpll_lref  = (dchgp << I2C_BBPLL_OC_DCHGP_LSB) | (div_ref);
    uint8_t i2c_bbpll_div_7_0 = div7_0;
    uint8_t i2c_bbpll_dcur = (2 << I2C_BBPLL_OC_DLREF_SEL_LSB ) | (1 << I2C_BBPLL_OC_DHREF_SEL_LSB) | dcur;
    I2C_WRITEREG_RTC(I2C_BBPLL, I2C_BBPLL_OC_REF_DIV, i2c_bbpll_lref);
    I2C_WRITEREG_RTC(I2C_BBPLL, I2C_BBPLL_OC_DIV_7_0, i2c_bbpll_div_7_0);
    I2C_WRITEREG_MASK_RTC(I2C_BBPLL, I2C_BBPLL_OC_DR1, dr1);
    I2C_WRITEREG_MASK_RTC(I2C_BBPLL, I2C_BBPLL_OC_DR3, dr3);
    I2C_WRITEREG_RTC(I2C_BBPLL, I2C_BBPLL_OC_DCUR, i2c_bbpll_dcur);

    // Enable calibration by software
    I2C_WRITEREG_MASK_RTC(I2C_BBPLL, I2C_BBPLL_IR_CAL_ENX_CAP, 1);
    for (int ext_cap = 0; ext_cap < 16; ext_cap++) {
        uint8_t cal_result;
        I2C_WRITEREG_MASK_RTC(I2C_BBPLL, I2C_BBPLL_IR_CAL_EXT_CAP, ext_cap);
        cal_result = I2C_READREG_MASK_RTC(I2C_BBPLL, I2C_BBPLL_OR_CAL_CAP);
        if (cal_result == 0) {
            break;
        }
        if (ext_cap == 15) {
            SOC_LOGE(TAG, "BBPLL SOFTWARE CAL FAIL");
        }
    }

    /* this delay is replaced by polling Pll calibration end flag
     * uint32_t delay_pll_en = (rtc_clk_slow_freq_get() == RTC_SLOW_FREQ_RTC) ?
     *       DELAY_PLL_ENABLE_WITH_150K : DELAY_PLL_ENABLE_WITH_32K;
     * ets_delay_us(delay_pll_en);
     */
    /* this calibration didn't work on 480M
    need to test exact delay according to 320M
    while (!GET_PERI_REG_MASK(I2C_MST_ANA_STATE_REG, I2C_MST_BBPLL_CAL_END)) {
        ets_delay_us(1);
    }
    */
}

/**
 * Switch to XTAL frequency. Does not disable the PLL.
 */
static void rtc_clk_cpu_freq_to_xtal(void)
{
    rtc_xtal_freq_t xtal_freq = rtc_clk_xtal_freq_get();
    ets_update_cpu_frequency(xtal_freq);
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_XTAL);
    REG_SET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_PRE_DIV_CNT, 0);
    REG_SET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_SOC_CLK_SEL, 0);
    /* Why we need to do this ? */
    //DPORT_REG_WRITE(DPORT_CPU_PER_CONF_REG, 0); // clear DPORT_CPUPERIOD_SEL

    rtc_clk_apb_freq_update(xtal_freq * MHZ);
    s_cur_freq = RTC_CPU_FREQ_XTAL;
    s_cur_pll  = RTC_PLL_NONE;
}

/**
 * Switch to one of PLL-based frequencies. Current frequency can be XTAL or PLL.
 * PLL must already be enabled.
 * If switching between frequencies derived from different PLLs (320M and 480M),
 * fall back to rtc_clk_cpu_freq_set.
 * @param cpu_freq new CPU frequency
 */
static void rtc_clk_cpu_freq_to_pll(rtc_cpu_freq_t cpu_freq)
{
    int freq = 0;
    if ((s_cur_pll == RTC_PLL_NONE) || ((s_cur_pll == RTC_PLL_320M) && (cpu_freq == RTC_CPU_FREQ_240M))) {
        /*
         * if switch from non-pll or switch from PLL 320M to 480M
         * need to switch PLLs, fall back to full implementation
         */
        rtc_clk_cpu_freq_set(cpu_freq);
        return;
    }

    if ((cpu_freq == RTC_CPU_FREQ_80M) || (cpu_freq == RTC_CPU_320M_80M)) {
        REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_80M_160M);
        DPORT_REG_WRITE(DPORT_CPU_PER_CONF_REG, 0);
        freq = 80;
    } else if ((cpu_freq == RTC_CPU_FREQ_160M) || (cpu_freq == RTC_CPU_320M_160M)) {
        REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_80M_160M);
        DPORT_REG_WRITE(DPORT_CPU_PER_CONF_REG, 1);
        freq = 160;
    } else if (cpu_freq == RTC_CPU_FREQ_240M) {
        REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_240M);
        DPORT_REG_WRITE(DPORT_CPU_PER_CONF_REG, 2);
        freq = 240;
    }
    // REG_SET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_SOC_CLK_SEL, DPORT_SOC_CLK_SEL_PLL);
    rtc_clk_apb_freq_update(80 * MHZ);
    ets_update_cpu_frequency(freq);
    s_cur_freq = cpu_freq;
}

void rtc_clk_cpu_freq_set_fast(rtc_cpu_freq_t cpu_freq)
{
    if (cpu_freq == s_cur_freq) {
        return;
    } else if (cpu_freq == RTC_CPU_FREQ_2M || s_cur_freq == RTC_CPU_FREQ_2M) {
        /* fall back to full implementation if switch to/from 2M is needed */
        rtc_clk_cpu_freq_set(cpu_freq);
    } else if (cpu_freq == RTC_CPU_FREQ_XTAL) {
        rtc_clk_cpu_freq_to_xtal();
    } else if (cpu_freq > RTC_CPU_FREQ_XTAL) {
        rtc_clk_cpu_freq_to_pll(cpu_freq);
        /* Not neccessary any more */
        //rtc_clk_wait_for_slow_cycle();
    }
}

void rtc_clk_cpu_freq_set(rtc_cpu_freq_t cpu_freq)
{
    rtc_xtal_freq_t xtal_freq = rtc_clk_xtal_freq_get();
    /* Switch CPU to XTAL frequency first */
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_XTAL);
    REG_SET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_SOC_CLK_SEL, 0);
    REG_SET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_PRE_DIV_CNT, 0);
    ets_update_cpu_frequency(xtal_freq);
    /* Frequency switch is synchronized to SLOW_CLK cycle. Wait until the switch
     * is complete before disabling the PLL.
     */
    /* register SOC_CLK_SEL is moved to APB domain, so this delay is not neccessary any more */
    //rtc_clk_wait_for_slow_cycle();

    DPORT_REG_SET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_CPUPERIOD_SEL, 0);

    /* BBPLL force power down won't affect force power up setting */
    SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG,
                      RTC_CNTL_BB_I2C_FORCE_PD | RTC_CNTL_BBPLL_FORCE_PD |
                      RTC_CNTL_BBPLL_I2C_FORCE_PD);
    s_cur_pll = RTC_PLL_NONE;
    rtc_clk_apb_freq_update(xtal_freq * MHZ);

    /* is APLL under force power down? */
    /* may need equivalent function
    uint32_t apll_fpd = REG_GET_FIELD(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PLLA_FORCE_PD);

     * if (apll_fpd) {
     *   SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_I2C_FORCE_PD);
     * }
    */

    /* now switch to the desired frequency */
    if (cpu_freq == RTC_CPU_FREQ_XTAL) {
        /* already at XTAL, nothing to do */
    } else if (cpu_freq == RTC_CPU_FREQ_2M) {
        /* set up divider to produce 2MHz from XTAL */
        REG_SET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_PRE_DIV_CNT, (xtal_freq / 2) - 1);
        ets_update_cpu_frequency(2);
        rtc_clk_apb_freq_update(2 * MHZ);
        /* lower the voltage */
        REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_2M);
    } else {
        /* use PLL as clock source */
        CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG,
                            RTC_CNTL_BB_I2C_FORCE_PD |
                            RTC_CNTL_BBPLL_FORCE_PD | RTC_CNTL_BBPLL_I2C_FORCE_PD);
        if (cpu_freq > RTC_CPU_FREQ_2M) {
            rtc_clk_bbpll_set(xtal_freq, RTC_PLL_320M);
            s_cur_pll = RTC_PLL_320M;
        } else {
            rtc_clk_bbpll_set(xtal_freq, RTC_PLL_480M);
            s_cur_pll = RTC_PLL_480M;
        }

        if ((cpu_freq == RTC_CPU_FREQ_80M) || (cpu_freq == RTC_CPU_320M_80M)) {
            REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_80M_160M);
            DPORT_REG_SET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_CPUPERIOD_SEL, 0);
            ets_update_cpu_frequency(80);
        } else if ((cpu_freq == RTC_CPU_FREQ_160M) || (cpu_freq == RTC_CPU_320M_160M)) {
            REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_80M_160M);
            DPORT_REG_SET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_CPUPERIOD_SEL, 1);
            ets_update_cpu_frequency(160);
        } else if (cpu_freq == RTC_CPU_FREQ_240M) {
            REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_240M);
            DPORT_REG_SET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_CPUPERIOD_SEL, 2);
            ets_update_cpu_frequency(240);
        }
        REG_SET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_SOC_CLK_SEL, 1);
        //rtc_clk_wait_for_slow_cycle();
        rtc_clk_apb_freq_update(80 * MHZ);
    }
    s_cur_freq = cpu_freq;
}

rtc_cpu_freq_t rtc_clk_cpu_freq_get(void)
{
    uint32_t soc_clk_sel = REG_GET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_SOC_CLK_SEL);
    switch (soc_clk_sel) {
    case 0: {
        uint32_t pre_div = REG_GET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_PRE_DIV_CNT);
        if (pre_div == 0) {
            return RTC_CPU_FREQ_XTAL;
        } else if (pre_div == 1) {
            return RTC_CPU_FREQ_XTAL_DIV2;
        } else if (pre_div == rtc_clk_xtal_freq_get() / 2 - 1) {
            return RTC_CPU_FREQ_2M;
        } else {
            assert(false && "unsupported frequency");
        }
        break;
    }
    case 1: {
        uint32_t cpuperiod_sel = DPORT_REG_GET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_CPUPERIOD_SEL);
        uint32_t pllfreq_sel = DPORT_REG_GET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_PLL_FREQ_SEL);
        if (cpuperiod_sel == 0) {
            if (pllfreq_sel == 1) {
                return RTC_CPU_FREQ_80M;
            } else {
                return RTC_CPU_320M_80M;
            }
        } else if (cpuperiod_sel == 1) {
            if (pllfreq_sel == 1) {
                return RTC_CPU_FREQ_160M;
            } else {
                return RTC_CPU_320M_160M;
            }
        } else if (cpuperiod_sel == 2) {
            return RTC_CPU_FREQ_240M;
        } else {
            assert(false && "unsupported frequency");
        }
        break;
    }
    case 2:
    case 3:
    default:
        assert(false && "unsupported frequency");
    }
    return 0;
}

uint32_t rtc_clk_cpu_freq_value(rtc_cpu_freq_t cpu_freq)
{
    switch (cpu_freq) {
    case RTC_CPU_FREQ_XTAL:
        return ((uint32_t) rtc_clk_xtal_freq_get()) * MHZ;
    case RTC_CPU_FREQ_XTAL_DIV2:
        return ((uint32_t) rtc_clk_xtal_freq_get()) / 2 * MHZ;
    case RTC_CPU_FREQ_2M:
        return 2 * MHZ;
    case RTC_CPU_FREQ_80M:
        return 80 * MHZ;
    case RTC_CPU_FREQ_160M:
        return 160 * MHZ;
    case RTC_CPU_FREQ_240M:
        return 240 * MHZ;
    case RTC_CPU_320M_80M:
        return 80 * MHZ;
    case RTC_CPU_320M_160M:
        return 160 * MHZ;
    default:
        assert(false && "invalid rtc_cpu_freq_t value");
        return 0;
    }
}

bool rtc_clk_cpu_freq_from_mhz(int mhz, rtc_cpu_freq_t *out_val)
{
    if (mhz == 240) {
        *out_val = RTC_CPU_FREQ_240M;
    } else if (mhz == 160) {
        *out_val = RTC_CPU_FREQ_160M;
    } else if (mhz == 80) {
        *out_val = RTC_CPU_FREQ_80M;
    } else if (mhz == (int) rtc_clk_xtal_freq_get()) {
        *out_val = RTC_CPU_FREQ_XTAL;
    } else if (mhz == (int) rtc_clk_xtal_freq_get() / 2) {
        *out_val = RTC_CPU_FREQ_XTAL_DIV2;
    } else if (mhz == 2) {
        *out_val = RTC_CPU_FREQ_2M;
    } else {
        return false;
    }
    return true;
}

/* Values of RTC_XTAL_FREQ_REG and RTC_APB_FREQ_REG are stored as two copies in
 * lower and upper 16-bit halves. These are the routines to work with such a
 * representation.
 */
static bool clk_val_is_valid(uint32_t val)
{
    return (val & 0xffff) == ((val >> 16) & 0xffff) &&
           val != 0 &&
           val != UINT32_MAX;
}

static uint32_t reg_val_to_clk_val(uint32_t val)
{
    return val & UINT16_MAX;
}

static uint32_t clk_val_to_reg_val(uint32_t val)
{
    return (val & UINT16_MAX) | ((val & UINT16_MAX) << 16);
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

void rtc_clk_init(rtc_clk_config_t cfg)
{
    rtc_cpu_freq_t cpu_source_before = rtc_clk_cpu_freq_get();
    /* If we get a TG WDT system reset while running at 240MHz,
     * DPORT_CPUPERIOD_SEL register will be reset to 0 resulting in 120MHz
     * APB and CPU frequencies after reset. This will cause issues with XTAL
     * frequency estimation, so we switch to XTAL frequency first.
     *
     * Ideally we would only do this if SYSCON_SOC_CLK_SEL == PLL and
     * PLL is configured for 480M, but it takes less time to switch to 40M and
     * run the following code than querying the PLL does.
     */
    if (REG_GET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_SOC_CLK_SEL) == 1) {
        rtc_clk_cpu_freq_set(RTC_CPU_FREQ_XTAL);
    }

    /* Set tuning parameters for 8M and 150k clocks.
     * Note: this doesn't attempt to set the clocks to precise frequencies.
     * Instead, we calibrate these clocks against XTAL frequency later, when necessary.
     * - SCK_DCAP value controls tuning of 150k clock.
     *   The higher the value of DCAP is, the lower is the frequency.
     * - CK8M_DFREQ value controls tuning of 8M clock.
     *   CLK_8M_DFREQ constant gives the best temperature characteristics.
     */
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_SCK_DCAP, cfg.slow_clk_dcap);
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DFREQ, cfg.clk_8m_dfreq);

    /* Configure 150k clock division */
    rtc_clk_divider_set(cfg.clk_rtc_clk_div);
    /* Configure 8M clock division */
    rtc_clk_8m_divider_set(cfg.clk_8m_clk_div);

    /* Enable the internal bus used to configure PLLs */
    SET_PERI_REG_BITS(ANA_CONFIG_REG, ANA_CONFIG_M, ANA_CONFIG_M, ANA_CONFIG_S);
    CLEAR_PERI_REG_MASK(ANA_CONFIG_REG, I2C_APLL_M | I2C_BBPLL_M);

    rtc_xtal_freq_t xtal_freq = cfg.xtal_freq;
    uart_tx_wait_idle(0);
    rtc_clk_xtal_freq_update(xtal_freq);
    rtc_clk_apb_freq_update(xtal_freq * MHZ);
    /* Set CPU frequency */
    rtc_clk_cpu_freq_set(cfg.cpu_freq);

    /* Re-calculate the ccount to make time calculation correct. */
    uint32_t freq_before = rtc_clk_cpu_freq_value(cpu_source_before) / MHZ;
    uint32_t freq_after = rtc_clk_cpu_freq_value(cfg.cpu_freq) / MHZ;
    XTHAL_SET_CCOUNT( XTHAL_GET_CCOUNT() * freq_after / freq_before );

    /* Slow & fast clocks setup */
    if (cfg.slow_freq == RTC_SLOW_FREQ_32K_XTAL) {
        rtc_clk_32k_enable(true);
    }
    if (cfg.fast_freq == RTC_FAST_FREQ_8M) {
        bool need_8md256 = cfg.slow_freq == RTC_SLOW_FREQ_8MD256;
        rtc_clk_8m_enable(true, need_8md256);
    }
    rtc_clk_fast_freq_set(cfg.fast_freq);
    rtc_clk_slow_freq_set(cfg.slow_freq);
}

/* Name used in libphy.a:phy_chip_v7.o
 * TODO: update the library to use rtc_clk_xtal_freq_get
 */
rtc_xtal_freq_t rtc_get_xtal(void) __attribute__((alias("rtc_clk_xtal_freq_get")));

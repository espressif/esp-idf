// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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
#include "rom/ets_sys.h"
#include "rom/rtc.h"
#include "rom/uart.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_io_reg.h"
#include "soc/sens_reg.h"
#include "soc/dport_reg.h"
#include "soc/efuse_reg.h"
#include "soc/apb_ctrl_reg.h"
#include "i2c_rtc_clk.h"
#include "soc_log.h"
#include "sdkconfig.h"

#define MHZ (1000000)

static const char* TAG = "rtc_clk";

/* Various constants related to the analog internals of the chip.
 * Defined here because they don't have any use outside of this file.
 */

#define BBPLL_ENDIV5_VAL_320M       0x43
#define BBPLL_BBADC_DSMP_VAL_320M   0x84
#define BBPLL_ENDIV5_VAL_480M       0xc3
#define BBPLL_BBADC_DSMP_VAL_480M   0x74

#define APLL_SDM_STOP_VAL_1         0x09
#define APLL_SDM_STOP_VAL_2_REV0    0x69
#define APLL_SDM_STOP_VAL_2_REV1    0x49

#define APLL_CAL_DELAY_1            0x0f
#define APLL_CAL_DELAY_2            0x3f
#define APLL_CAL_DELAY_3            0x1f

#define XTAL_32K_DAC_VAL    1
#define XTAL_32K_DRES_VAL   3
#define XTAL_32K_DBIAS_VAL  0

/* Delays for various clock sources to be enabled/switched.
 * All values are in microseconds.
 * TODO: some of these are excessive, and should be reduced.
 */
#define DELAY_CPU_FREQ_SWITCH_TO_XTAL   80
#define DELAY_CPU_FREQ_SWITCH_TO_PLL    10
#define DELAY_PLL_DBIAS_RAISE           3
#define DELAY_PLL_ENABLE                80
#define DELAY_FAST_CLK_SWITCH           3
#define DELAY_SLOW_CLK_SWITCH           300
#define DELAY_8M_ENABLE                 50


void rtc_clk_32k_enable(bool enable)
{
    if (enable) {
        SET_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_X32N_MUX_SEL | RTC_IO_X32P_MUX_SEL);
        CLEAR_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG,
                RTC_IO_X32P_RDE | RTC_IO_X32P_RUE | RTC_IO_X32N_RUE |
                RTC_IO_X32N_RDE | RTC_IO_X32N_MUX_SEL | RTC_IO_X32P_MUX_SEL);
        REG_SET_FIELD(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_DAC_XTAL_32K, XTAL_32K_DAC_VAL);
        REG_SET_FIELD(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_DRES_XTAL_32K, XTAL_32K_DRES_VAL);
        REG_SET_FIELD(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_DBIAS_XTAL_32K, XTAL_32K_DBIAS_VAL);
        SET_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_XPD_XTAL_32K);
    } else {
        CLEAR_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_XPD_XTAL_32K);
    }
}

bool rtc_clk_32k_enabled()
{
    return GET_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_XPD_XTAL_32K) != 0;
}

void rtc_clk_8m_enable(bool clk_8m_en, bool d256_en)
{
    if (clk_8m_en) {
        CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M);
        /* no need to wait once enabled by software */
        REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_CK8M_WAIT, 1);
        if (d256_en) {
            CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M_DIV);
        } else {
            SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M_DIV);
        }
        ets_delay_us(DELAY_8M_ENABLE);
    } else {
        SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M);
        REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_CK8M_WAIT, RTC_CNTL_CK8M_WAIT_DEFAULT);
    }
}

bool rtc_clk_8m_enabled()
{
    return GET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M) == 0;
}

bool rtc_clk_8md256_enabled()
{
    return GET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M_DIV) == 0;
}

void rtc_clk_apll_enable(bool enable, uint32_t sdm0, uint32_t sdm1, uint32_t sdm2, uint32_t o_div)
{
    REG_SET_FIELD(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PLLA_FORCE_PD, enable ? 0 : 1);
    REG_SET_FIELD(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PLLA_FORCE_PU, enable ? 1 : 0);
    REG_SET_FIELD(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_I2C_FORCE_PD, enable ? 0 : 1);

    if (!enable &&
        REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_SOC_CLK_SEL) != RTC_CNTL_SOC_CLK_SEL_PLL) {
        SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_I2C_FORCE_PD);
    }

    if (enable) {
        uint8_t sdm_stop_val_2 = APLL_SDM_STOP_VAL_2_REV1;
        uint32_t is_rev0 = (GET_PERI_REG_BITS2(EFUSE_BLK0_RDATA3_REG, 1, 15) == 0);
        if (is_rev0) {
            sdm0 = 0;
            sdm1 = 0;
            sdm_stop_val_2 = APLL_SDM_STOP_VAL_2_REV0;
        }
        I2C_WRITEREG_MASK_RTC(I2C_APLL, I2C_APLL_DSDM2, sdm2);
        I2C_WRITEREG_MASK_RTC(I2C_APLL, I2C_APLL_DSDM0, sdm0);
        I2C_WRITEREG_MASK_RTC(I2C_APLL, I2C_APLL_DSDM1, sdm1);
        I2C_WRITEREG_RTC(I2C_APLL, I2C_APLL_SDM_STOP, APLL_SDM_STOP_VAL_1);
        I2C_WRITEREG_RTC(I2C_APLL, I2C_APLL_SDM_STOP, sdm_stop_val_2);
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

void rtc_clk_slow_freq_set(rtc_slow_freq_t slow_freq)
{
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ANA_CLK_RTC_SEL, slow_freq);
    ets_delay_us(DELAY_SLOW_CLK_SWITCH);
}

rtc_slow_freq_t rtc_clk_slow_freq_get()
{
    return REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ANA_CLK_RTC_SEL);
}


void rtc_clk_fast_freq_set(rtc_fast_freq_t fast_freq)
{
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_FAST_CLK_RTC_SEL, fast_freq);
    ets_delay_us(DELAY_FAST_CLK_SWITCH);
}

rtc_fast_freq_t rtc_clk_fast_freq_get()
{
    return REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_FAST_CLK_RTC_SEL);
}

void rtc_clk_bbpll_set(rtc_xtal_freq_t xtal_freq, rtc_cpu_freq_t cpu_freq)
{
    uint8_t div_ref;
    uint8_t div7_0;
    uint8_t div10_8;
    uint8_t lref;
    uint8_t dcur;
    uint8_t bw;

    if (cpu_freq != RTC_CPU_FREQ_240M) {
        /* Configure 320M PLL */
        switch (xtal_freq) {
            case RTC_XTAL_FREQ_40M:
                div_ref = 0;
                div7_0 = 32;
                div10_8 = 0;
                lref = 0;
                dcur = 6;
                bw = 3;
                break;
            case RTC_XTAL_FREQ_26M:
                div_ref = 12;
                div7_0 = 224;
                div10_8 = 4;
                lref = 1;
                dcur = 0;
                bw = 1;
                break;
            case RTC_XTAL_FREQ_24M:
                div_ref = 11;
                div7_0 = 224;
                div10_8 = 4;
                lref = 1;
                dcur = 0;
                bw = 1;
                break;
            default:
                div_ref = 12;
                div7_0 = 224;
                div10_8 = 4;
                lref = 0;
                dcur = 0;
                bw = 0;
                break;
        }
        I2C_WRITEREG_RTC(I2C_BBPLL, I2C_BBPLL_ENDIV5, BBPLL_ENDIV5_VAL_320M);
        I2C_WRITEREG_RTC(I2C_BBPLL, I2C_BBPLL_BBADC_DSMP, BBPLL_BBADC_DSMP_VAL_320M);
    } else {
        /* Raise the voltage */
        REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, RTC_CNTL_DBIAS_1V25);
        ets_delay_us(DELAY_PLL_DBIAS_RAISE);
        /* Configure 480M PLL */
        switch (xtal_freq) {
            case RTC_XTAL_FREQ_40M:
                div_ref = 0;
                div7_0 = 28;
                div10_8 = 0;
                lref = 0;
                dcur = 6;
                bw = 3;
                break;
            case RTC_XTAL_FREQ_26M:
                div_ref = 12;
                div7_0 = 144;
                div10_8 = 4;
                lref = 1;
                dcur = 0;
                bw = 1;
                break;
            case RTC_XTAL_FREQ_24M:
                div_ref = 11;
                div7_0 = 144;
                div10_8 = 4;
                lref = 1;
                dcur = 0;
                bw = 1;
                break;
            default:
                div_ref = 12;
                div7_0 = 224;
                div10_8 = 4;
                lref = 0;
                dcur = 0;
                bw = 0;
                break;
        }
        I2C_WRITEREG_RTC(I2C_BBPLL, I2C_BBPLL_ENDIV5, BBPLL_ENDIV5_VAL_480M);
        I2C_WRITEREG_RTC(I2C_BBPLL, I2C_BBPLL_BBADC_DSMP, BBPLL_BBADC_DSMP_VAL_480M);
    }

    uint8_t i2c_bbpll_lref  = (lref << 7) | (div10_8 << 4) | (div_ref);
    uint8_t i2c_bbpll_div_7_0 = div7_0;
    uint8_t i2c_bbpll_dcur = (bw << 6) | dcur;
    I2C_WRITEREG_RTC(I2C_BBPLL, I2C_BBPLL_OC_LREF, i2c_bbpll_lref);
    I2C_WRITEREG_RTC(I2C_BBPLL, I2C_BBPLL_OC_DIV_7_0, i2c_bbpll_div_7_0);
    I2C_WRITEREG_RTC(I2C_BBPLL, I2C_BBPLL_OC_DCUR, i2c_bbpll_dcur);
    ets_delay_us(DELAY_PLL_ENABLE);
}

void rtc_clk_cpu_freq_set(rtc_cpu_freq_t cpu_freq)
{
    rtc_xtal_freq_t xtal_freq = rtc_clk_xtal_freq_get();
    /* Switch CPU to XTAL frequency first */
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, RTC_CNTL_DBIAS_1V10);
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_SOC_CLK_SEL, RTC_CNTL_SOC_CLK_SEL_XTL);
    REG_SET_FIELD(APB_CTRL_SYSCLK_CONF_REG, APB_CTRL_PRE_DIV_CNT, 0);
    ets_update_cpu_frequency(xtal_freq);
    ets_delay_us(DELAY_CPU_FREQ_SWITCH_TO_XTAL);
    REG_SET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_CPUPERIOD_SEL, 0);
    SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG,
            RTC_CNTL_BB_I2C_FORCE_PD | RTC_CNTL_BBPLL_FORCE_PD |
            RTC_CNTL_BBPLL_I2C_FORCE_PD);
    rtc_clk_apb_freq_update(xtal_freq * MHZ);

    /* is APLL under force power down? */
    uint32_t apll_fpd = REG_GET_FIELD(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PLLA_FORCE_PD);
    if (apll_fpd) {
        /* then also power down the internal I2C bus */
        SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_I2C_FORCE_PD);
    }
    /* now switch to the desired frequency */
    if (cpu_freq == RTC_CPU_FREQ_XTAL) {
        /* already at XTAL, nothing to do */
    } else if (cpu_freq == RTC_CPU_FREQ_2M) {
        /* set up divider to produce 2MHz from XTAL */
        REG_SET_FIELD(APB_CTRL_SYSCLK_CONF_REG, APB_CTRL_PRE_DIV_CNT, (xtal_freq / 2) - 1);
        ets_update_cpu_frequency(2);
        rtc_clk_apb_freq_update(2 * MHZ);
        /* lower the voltage */
        REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, RTC_CNTL_DBIAS_1V00);
    } else {
        /* use PLL as clock source */
        CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG,
                RTC_CNTL_BIAS_I2C_FORCE_PD | RTC_CNTL_BB_I2C_FORCE_PD |
                RTC_CNTL_BBPLL_FORCE_PD | RTC_CNTL_BBPLL_I2C_FORCE_PD);
        rtc_clk_bbpll_set(xtal_freq, cpu_freq);
        if (cpu_freq == RTC_CPU_FREQ_80M) {
            REG_SET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_CPUPERIOD_SEL, 0);
            ets_update_cpu_frequency(80);
        } else if (cpu_freq == RTC_CPU_FREQ_160M) {
            REG_SET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_CPUPERIOD_SEL, 1);
            ets_update_cpu_frequency(160);
        } else if (cpu_freq == RTC_CPU_FREQ_240M) {
            REG_SET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_CPUPERIOD_SEL, 2);
            ets_update_cpu_frequency(240);
        }
        REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_SOC_CLK_SEL, RTC_CNTL_SOC_CLK_SEL_PLL);
        ets_delay_us(DELAY_CPU_FREQ_SWITCH_TO_PLL);
        rtc_clk_apb_freq_update(80 * MHZ);
    }
}

rtc_cpu_freq_t rtc_clk_cpu_freq_get()
{
    uint32_t soc_clk_sel = REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_SOC_CLK_SEL);
    switch (soc_clk_sel) {
        case RTC_CNTL_SOC_CLK_SEL_XTL: {
            uint32_t pre_div = REG_GET_FIELD(APB_CTRL_SYSCLK_CONF_REG, APB_CTRL_PRE_DIV_CNT);
            if (pre_div == 0) {
                return RTC_CPU_FREQ_XTAL;
            } else if (pre_div == rtc_clk_xtal_freq_get() / 2 - 1) {
                return RTC_CPU_FREQ_2M;
            } else {
                assert(false && "unsupported frequency");
            }
            break;
        }
        case RTC_CNTL_SOC_CLK_SEL_PLL: {
            uint32_t cpuperiod_sel = REG_GET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_CPUPERIOD_SEL);
            if (cpuperiod_sel == 0) {
                return RTC_CPU_FREQ_80M;
            } else if (cpuperiod_sel == 1) {
                return RTC_CPU_FREQ_160M;
            } else if (cpuperiod_sel == 2) {
                return RTC_CPU_FREQ_240M;
            } else {
                assert(false && "unsupported frequency");
            }
            break;
        }
        case RTC_CNTL_SOC_CLK_SEL_APLL:
        case RTC_CNTL_SOC_CLK_SEL_8M:
        default:
            assert(false && "unsupported frequency");
    }
    return RTC_CNTL_SOC_CLK_SEL_XTL;
}

uint32_t rtc_clk_cpu_freq_value(rtc_cpu_freq_t cpu_freq)
{
    switch (cpu_freq) {
        case RTC_CPU_FREQ_XTAL:
            return ((uint32_t) rtc_clk_xtal_freq_get()) * MHZ;
        case RTC_CPU_FREQ_2M:
            return 2 * MHZ;
        case RTC_CPU_FREQ_80M:
            return 80 * MHZ;
        case RTC_CPU_FREQ_160M:
            return 160 * MHZ;
        case RTC_CPU_FREQ_240M:
            return 240 * MHZ;
        default:
            assert(false && "invalid rtc_cpu_freq_t value");
            return 0;
    }
}

/* Values of RTC_XTAL_FREQ_REG and RTC_APB_FREQ_REG are stored as two copies in
 * lower and upper 16-bit halves. These are the routines to work with such a
 * representation.
 */
static bool clk_val_is_valid(uint32_t val) {
    return (val & 0xffff) == ((val >> 16) & 0xffff) &&
            val != 0 &&
            val != UINT32_MAX;
}

static uint32_t reg_val_to_clk_val(uint32_t val) {
    return val & UINT16_MAX;
}

static uint32_t clk_val_to_reg_val(uint32_t val) {
    return (val & UINT16_MAX) | ((val & UINT16_MAX) << 16);
}

rtc_xtal_freq_t rtc_clk_xtal_freq_get()
{
    /* We may have already written XTAL value into RTC_XTAL_FREQ_REG */
    uint32_t xtal_freq_reg = READ_PERI_REG(RTC_XTAL_FREQ_REG);
    if (!clk_val_is_valid(xtal_freq_reg)) {
        SOC_LOGW(TAG, "invalid RTC_XTAL_FREQ_REG value: 0x%08x", xtal_freq_reg);
        return RTC_XTAL_FREQ_AUTO;
    }
    return reg_val_to_clk_val(xtal_freq_reg);
}

void rtc_clk_xtal_freq_update(rtc_xtal_freq_t xtal_freq)
{
    WRITE_PERI_REG(RTC_XTAL_FREQ_REG, clk_val_to_reg_val(xtal_freq));
}

static rtc_xtal_freq_t rtc_clk_xtal_freq_estimate()
{
    /* ROM startup code estimates XTAL frequency using an 8MD256 clock and stores
     * the value into RTC_APB_FREQ_REG. The value is in Hz, right shifted by 12.
     * Use this value to guess the real XTAL frequency.
     *
     * TODO: make this more robust by calibrating again after setting
     * RTC_CNTL_CK8M_DFREQ.
     */
    uint32_t apb_freq_reg = READ_PERI_REG(RTC_APB_FREQ_REG);
    if (!clk_val_is_valid(apb_freq_reg)) {
        SOC_LOGW(TAG, "invalid RTC_APB_FREQ_REG value: 0x%08x", apb_freq_reg);
        return RTC_XTAL_FREQ_AUTO;
    }
    uint32_t freq_mhz = (reg_val_to_clk_val(apb_freq_reg) << 12) / MHZ;
    /* Guess the XTAL type. For now, only 40 and 26MHz are supported.
     */
    switch (freq_mhz) {
        case 21 ... 31:
            return RTC_XTAL_FREQ_26M;
        case 32 ... 33:
            SOC_LOGW(TAG, "Potentially bogus XTAL frequency: %d MHz, guessing 26 MHz", freq_mhz);
            return RTC_XTAL_FREQ_26M;
        case 34 ... 35:
            SOC_LOGW(TAG, "Potentially bogus XTAL frequency: %d MHz, guessing 40 MHz", freq_mhz);
            return RTC_XTAL_FREQ_40M;
        case 36 ... 45:
            return RTC_XTAL_FREQ_40M;
        default:
            SOC_LOGW(TAG, "Bogus XTAL frequency: %d MHz", freq_mhz);
            return RTC_XTAL_FREQ_AUTO;
    }
}

void rtc_clk_apb_freq_update(uint32_t apb_freq)
{
    WRITE_PERI_REG(RTC_APB_FREQ_REG, clk_val_to_reg_val(apb_freq >> 12));
}

uint32_t rtc_clk_apb_freq_get()
{
    return reg_val_to_clk_val(READ_PERI_REG(RTC_APB_FREQ_REG)) << 12;
}


void rtc_clk_init(rtc_clk_config_t cfg)
{
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

    /* Configure 8M clock division */
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL, cfg.clk_8m_div);

    /* Enable the internal bus used to configure PLLs */
    SET_PERI_REG_BITS(ANA_CONFIG_REG, ANA_CONFIG_M, ANA_CONFIG_M, ANA_CONFIG_S);
    CLEAR_PERI_REG_MASK(ANA_CONFIG_REG, I2C_APLL_M | I2C_BBPLL_M);

    /* Estimate XTAL frequency if requested */
    rtc_xtal_freq_t xtal_freq = cfg.xtal_freq;
    if (xtal_freq == RTC_XTAL_FREQ_AUTO) {
        xtal_freq = rtc_clk_xtal_freq_estimate();
        if (xtal_freq == RTC_XTAL_FREQ_AUTO) {
            SOC_LOGW(TAG, "Can't estimate XTAL frequency, assuming 26MHz");
            xtal_freq = RTC_XTAL_FREQ_26M;
        }
    }
    rtc_clk_xtal_freq_update(xtal_freq);
    rtc_clk_apb_freq_update(xtal_freq * MHZ);
    /* Set CPU frequency */
    rtc_clk_cpu_freq_set(cfg.cpu_freq);

    /* Slow & fast clocks setup */
    if (cfg.slow_freq == RTC_SLOW_FREQ_32K_XTAL) {
        rtc_clk_32k_enable(false);
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
rtc_xtal_freq_t rtc_get_xtal() __attribute__((alias("rtc_clk_xtal_freq_get")));


/* Referenced in librtc.a:rtc.o.
 * TODO: remove
 */
void rtc_uart_div_modify(int latch)
{

}

/* Referenced in librtc.a:rtc.o.
 * TODO: remove
 */
void rtc_uart_tx_wait_idle(int uart)
{

}

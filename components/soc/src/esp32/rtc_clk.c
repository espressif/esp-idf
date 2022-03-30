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
#include <stdlib.h>
#include "esp32/rom/ets_sys.h"
#include "esp32/rom/rtc.h"
#include "esp32/rom/uart.h"
#include "esp32/rom/gpio.h"
#include "esp_efuse.h"
#include "soc/rtc.h"
#include "soc/rtc_periph.h"
#include "soc/sens_periph.h"
#include "soc/dport_reg.h"
#include "soc/efuse_periph.h"
#include "soc/apb_ctrl_reg.h"
#include "regi2c_ctrl.h"
#include "soc_log.h"
#include "sdkconfig.h"
#include "xtensa/core-macros.h"
#include "rtc_clk_common.h"

/* Frequency of the 8M oscillator is 8.5MHz +/- 5%, at the default DCAP setting */
#define RTC_FAST_CLK_FREQ_8M        8500000
#define RTC_SLOW_CLK_FREQ_150K      150000
#define RTC_SLOW_CLK_FREQ_8MD256    (RTC_FAST_CLK_FREQ_8M / 256)
#define RTC_SLOW_CLK_FREQ_32K       32768

/* BBPLL configuration values */
#define BBPLL_ENDIV5_VAL_320M       0x43
#define BBPLL_BBADC_DSMP_VAL_320M   0x84
#define BBPLL_ENDIV5_VAL_480M       0xc3
#define BBPLL_BBADC_DSMP_VAL_480M   0x74
#define BBPLL_IR_CAL_DELAY_VAL      0x18
#define BBPLL_IR_CAL_EXT_CAP_VAL    0x20
#define BBPLL_OC_ENB_FCAL_VAL       0x9a
#define BBPLL_OC_ENB_VCON_VAL       0x00
#define BBPLL_BBADC_CAL_7_0_VAL     0x00

#define APLL_SDM_STOP_VAL_1         0x09
#define APLL_SDM_STOP_VAL_2_REV0    0x69
#define APLL_SDM_STOP_VAL_2_REV1    0x49

#define APLL_CAL_DELAY_1            0x0f
#define APLL_CAL_DELAY_2            0x3f
#define APLL_CAL_DELAY_3            0x1f

#define XTAL_32K_DAC_VAL    1
#define XTAL_32K_DRES_VAL   3
#define XTAL_32K_DBIAS_VAL  0

#define XTAL_32K_BOOTSTRAP_DAC_VAL      3
#define XTAL_32K_BOOTSTRAP_DRES_VAL     3
#define XTAL_32K_BOOTSTRAP_DBIAS_VAL    0
#define XTAL_32K_BOOTSTRAP_TIME_US      7

#define XTAL_32K_EXT_DAC_VAL    2
#define XTAL_32K_EXT_DRES_VAL   3
#define XTAL_32K_EXT_DBIAS_VAL  1

/* Delays for various clock sources to be enabled/switched.
 * All values are in microseconds.
 * TODO: some of these are excessive, and should be reduced.
 */
#define DELAY_PLL_DBIAS_RAISE           3
#define DELAY_PLL_ENABLE_WITH_150K      80
#define DELAY_PLL_ENABLE_WITH_32K       160
#define DELAY_FAST_CLK_SWITCH           3
#define DELAY_SLOW_CLK_SWITCH           300
#define DELAY_8M_ENABLE                 50

/* Core voltage needs to be increased in two cases:
 * 1. running at 240 MHz
 * 2. running with 80MHz Flash frequency
 *
 * There is a record in efuse which indicates the proper voltage for these two cases.
 */
#define RTC_CNTL_DBIAS_HP_VOLT         (RTC_CNTL_DBIAS_1V25 - (REG_GET_FIELD(EFUSE_BLK0_RDATA5_REG, EFUSE_RD_VOL_LEVEL_HP_INV)))
#ifdef CONFIG_ESPTOOLPY_FLASHFREQ_80M
#define DIG_DBIAS_80M_160M  RTC_CNTL_DBIAS_HP_VOLT
#else
#define DIG_DBIAS_80M_160M  RTC_CNTL_DBIAS_1V10
#endif
#define DIG_DBIAS_240M      RTC_CNTL_DBIAS_HP_VOLT
#define DIG_DBIAS_XTAL      RTC_CNTL_DBIAS_1V10
#define DIG_DBIAS_2M        RTC_CNTL_DBIAS_1V00

#define RTC_PLL_FREQ_320M   320
#define RTC_PLL_FREQ_480M   480

static void rtc_clk_cpu_freq_to_8m(void);
static void rtc_clk_bbpll_disable(void);
static void rtc_clk_bbpll_enable(void);
static void rtc_clk_cpu_freq_to_pll_mhz(int cpu_freq_mhz);

// Current PLL frequency, in MHZ (320 or 480). Zero if PLL is not enabled.
static int s_cur_pll_freq;

static const char* TAG = "rtc_clk";

static void rtc_clk_32k_enable_common(int dac, int dres, int dbias)
{
    CLEAR_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG,
                        RTC_IO_X32P_RDE | RTC_IO_X32P_RUE | RTC_IO_X32N_RUE |
                        RTC_IO_X32N_RDE | RTC_IO_X32N_FUN_IE | RTC_IO_X32P_FUN_IE);
    SET_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_X32N_MUX_SEL | RTC_IO_X32P_MUX_SEL);
    /* Set the parameters of xtal
        dac --> current
        dres --> resistance
        dbias --> bais voltage
    */
    REG_SET_FIELD(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_DAC_XTAL_32K, dac);
    REG_SET_FIELD(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_DRES_XTAL_32K, dres);
    REG_SET_FIELD(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_DBIAS_XTAL_32K, dbias);

#ifdef CONFIG_ESP32_RTC_EXT_CRYST_ADDIT_CURRENT
    uint8_t chip_ver = esp_efuse_get_chip_ver();
    // version0 and version1 need provide additional current to external XTAL.
    if(chip_ver == 0 || chip_ver == 1) {
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
#elif defined CONFIG_ESP32_RTC_EXT_CRYST_ADDIT_CURRENT_V2
    uint8_t chip_ver = esp_efuse_get_chip_ver();
    if(chip_ver == 0 || chip_ver == 1) {
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
    /* Power up external xtal */
    SET_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_XPD_XTAL_32K_M);
}

void rtc_clk_32k_enable(bool enable)
{
    if (enable) {
        rtc_clk_32k_enable_common(XTAL_32K_DAC_VAL, XTAL_32K_DRES_VAL, XTAL_32K_DBIAS_VAL);
    } else {
        /* Disable X32N and X32P pad drive external xtal */
        CLEAR_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_XPD_XTAL_32K_M);
        CLEAR_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_X32N_MUX_SEL | RTC_IO_X32P_MUX_SEL);

#ifdef CONFIG_ESP32_RTC_EXT_CRYST_ADDIT_CURRENT
        uint8_t chip_ver = esp_efuse_get_chip_ver();
        if(chip_ver == 0 || chip_ver == 1) {
            /* Power down TOUCH */
            CLEAR_PERI_REG_MASK(RTC_IO_TOUCH_PAD9_REG, RTC_IO_TOUCH_PAD9_XPD_M);
        }
#elif defined CONFIG_ESP32_RTC_EXT_CRYST_ADDIT_CURRENT_V2
        uint8_t chip_ver = esp_efuse_get_chip_ver();
        if(chip_ver == 0 || chip_ver == 1) {
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
    rtc_clk_32k_enable_common(XTAL_32K_EXT_DAC_VAL, XTAL_32K_EXT_DRES_VAL, XTAL_32K_EXT_DBIAS_VAL);
}

/* Helping external 32kHz crystal to start up.
 * External crystal connected to outputs GPIO32 GPIO33.
 * Forms N pulses with a frequency of about 32KHz on the outputs of the crystal.
 */
void rtc_clk_32k_bootstrap(uint32_t cycle)
{
    if (cycle){
        const uint32_t pin_32 = 32;
        const uint32_t pin_33 = 33;
        const uint32_t mask_32 = (1 << (pin_32 - 32));
        const uint32_t mask_33 = (1 << (pin_33 - 32));

        gpio_pad_select_gpio(pin_32);
        gpio_pad_select_gpio(pin_33);
        gpio_output_set_high(mask_32, mask_33, mask_32 | mask_33, 0);

        const uint32_t delay_us = (1000000 / RTC_SLOW_CLK_FREQ_32K / 2);
        while(cycle){
            gpio_output_set_high(mask_32, mask_33, mask_32 | mask_33, 0);
            ets_delay_us(delay_us);
            gpio_output_set_high(mask_33, mask_32, mask_32 | mask_33, 0);
            ets_delay_us(delay_us);
            cycle--;
        }
        gpio_output_set_high(0, 0, 0, mask_32 | mask_33); // disable pins
    }

    CLEAR_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_XPD_XTAL_32K);
    SET_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_X32P_RUE | RTC_IO_X32N_RDE);
    ets_delay_us(XTAL_32K_BOOTSTRAP_TIME_US);

    rtc_clk_32k_enable_common(XTAL_32K_BOOTSTRAP_DAC_VAL,
            XTAL_32K_BOOTSTRAP_DRES_VAL, XTAL_32K_BOOTSTRAP_DBIAS_VAL);
}

bool rtc_clk_32k_enabled(void)
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

    if (!enable &&
        REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_SOC_CLK_SEL) != RTC_CNTL_SOC_CLK_SEL_PLL) {
        REG_SET_BIT(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_I2C_FORCE_PD);
    } else {
        REG_CLR_BIT(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_I2C_FORCE_PD);
    }

    if (enable) {
        uint8_t sdm_stop_val_2 = APLL_SDM_STOP_VAL_2_REV1;
        uint32_t is_rev0 = (GET_PERI_REG_BITS2(EFUSE_BLK0_RDATA3_REG, 1, 15) == 0);
        if (is_rev0) {
            sdm0 = 0;
            sdm1 = 0;
            sdm_stop_val_2 = APLL_SDM_STOP_VAL_2_REV0;
        }
        REGI2C_WRITE_MASK(I2C_APLL, I2C_APLL_DSDM2, sdm2);
        REGI2C_WRITE_MASK(I2C_APLL, I2C_APLL_DSDM0, sdm0);
        REGI2C_WRITE_MASK(I2C_APLL, I2C_APLL_DSDM1, sdm1);
        REGI2C_WRITE(I2C_APLL, I2C_APLL_SDM_STOP, APLL_SDM_STOP_VAL_1);
        REGI2C_WRITE(I2C_APLL, I2C_APLL_SDM_STOP, sdm_stop_val_2);
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

    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_XTAL32K_EN,
            (slow_freq == RTC_SLOW_FREQ_32K_XTAL) ? 1 : 0);

    ets_delay_us(DELAY_SLOW_CLK_SWITCH);
}

rtc_slow_freq_t rtc_clk_slow_freq_get(void)
{
    return REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ANA_CLK_RTC_SEL);
}

uint32_t rtc_clk_slow_freq_get_hz(void)
{
    switch(rtc_clk_slow_freq_get()) {
        case RTC_SLOW_FREQ_RTC: return RTC_SLOW_CLK_FREQ_150K;
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

void rtc_clk_bbpll_configure(rtc_xtal_freq_t xtal_freq, int pll_freq)
{
    uint8_t div_ref;
    uint8_t div7_0;
    uint8_t div10_8;
    uint8_t lref;
    uint8_t dcur;
    uint8_t bw;

    if (pll_freq == RTC_PLL_FREQ_320M) {
        /* Raise the voltage, if needed */
        REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_80M_160M);
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
        REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_ENDIV5, BBPLL_ENDIV5_VAL_320M);
        REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_BBADC_DSMP, BBPLL_BBADC_DSMP_VAL_320M);
    } else {
        /* Raise the voltage */
        REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_240M);
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
        REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_ENDIV5, BBPLL_ENDIV5_VAL_480M);
        REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_BBADC_DSMP, BBPLL_BBADC_DSMP_VAL_480M);
    }

    uint8_t i2c_bbpll_lref  = (lref << 7) | (div10_8 << 4) | (div_ref);
    uint8_t i2c_bbpll_div_7_0 = div7_0;
    uint8_t i2c_bbpll_dcur = (bw << 6) | dcur;
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_OC_LREF, i2c_bbpll_lref);
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_OC_DIV_7_0, i2c_bbpll_div_7_0);
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_OC_DCUR, i2c_bbpll_dcur);
    uint32_t delay_pll_en = (rtc_clk_slow_freq_get() == RTC_SLOW_FREQ_RTC) ?
            DELAY_PLL_ENABLE_WITH_150K : DELAY_PLL_ENABLE_WITH_32K;
    ets_delay_us(delay_pll_en);
    s_cur_pll_freq = pll_freq;
}

/**
 * Switch to XTAL frequency. Does not disable the PLL.
 */
void rtc_clk_cpu_freq_to_xtal(int freq, int div)
{
    ets_update_cpu_frequency(freq);
    /* set divider from XTAL to APB clock */
    REG_SET_FIELD(APB_CTRL_SYSCLK_CONF_REG, APB_CTRL_PRE_DIV_CNT, div - 1);
    /* adjust ref_tick */
    REG_WRITE(APB_CTRL_XTAL_TICK_CONF_REG, freq * MHZ / REF_CLK_FREQ - 1);
    /* switch clock source */
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_SOC_CLK_SEL, RTC_CNTL_SOC_CLK_SEL_XTL);
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
    REG_SET_FIELD(APB_CTRL_SYSCLK_CONF_REG, APB_CTRL_PRE_DIV_CNT, 0);
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_SOC_CLK_SEL, RTC_CNTL_SOC_CLK_SEL_8M);
    rtc_clk_apb_freq_update(RTC_FAST_CLK_FREQ_8M);
}

static void rtc_clk_bbpll_disable(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG,
            RTC_CNTL_BB_I2C_FORCE_PD | RTC_CNTL_BBPLL_FORCE_PD |
            RTC_CNTL_BBPLL_I2C_FORCE_PD);
    s_cur_pll_freq = 0;

    /* is APLL under force power down? */
    uint32_t apll_fpd = REG_GET_FIELD(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PLLA_FORCE_PD);
    if (apll_fpd) {
        /* then also power down the internal I2C bus */
        SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_I2C_FORCE_PD);
    }
}

static void rtc_clk_bbpll_enable(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG,
             RTC_CNTL_BIAS_I2C_FORCE_PD | RTC_CNTL_BB_I2C_FORCE_PD |
             RTC_CNTL_BBPLL_FORCE_PD | RTC_CNTL_BBPLL_I2C_FORCE_PD);

    /* reset BBPLL configuration */
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_IR_CAL_DELAY, BBPLL_IR_CAL_DELAY_VAL);
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_IR_CAL_EXT_CAP, BBPLL_IR_CAL_EXT_CAP_VAL);
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_OC_ENB_FCAL, BBPLL_OC_ENB_FCAL_VAL);
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_OC_ENB_VCON, BBPLL_OC_ENB_VCON_VAL);
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_BBADC_CAL_7_0, BBPLL_BBADC_CAL_7_0_VAL);
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
    DPORT_REG_WRITE(DPORT_CPU_PER_CONF_REG, per_conf);
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, dbias);
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_SOC_CLK_SEL, RTC_CNTL_SOC_CLK_SEL_PLL);
    rtc_clk_apb_freq_update(80 * MHZ);
    ets_update_cpu_frequency(cpu_freq_mhz);
    rtc_clk_wait_for_slow_cycle();
}

void rtc_clk_cpu_freq_set_xtal(void)
{
    int freq_mhz = (int) rtc_clk_xtal_freq_get();

    rtc_clk_cpu_freq_to_xtal(freq_mhz, 1);
    rtc_clk_wait_for_slow_cycle();
    rtc_clk_bbpll_disable();
}

void rtc_clk_cpu_freq_to_config(rtc_cpu_freq_t cpu_freq, rtc_cpu_freq_config_t* out_config)
{
    uint32_t source_freq_mhz;
    rtc_cpu_freq_src_t source;
    uint32_t freq_mhz;
    uint32_t divider;

    switch (cpu_freq) {
        case RTC_CPU_FREQ_XTAL:
        case RTC_CPU_FREQ_2M:
            source_freq_mhz = rtc_clk_xtal_freq_get();
            source = RTC_CPU_FREQ_SRC_XTAL;
            if (cpu_freq == RTC_CPU_FREQ_2M) {
                freq_mhz = 2;
                divider = source_freq_mhz / 2;
            } else {
                freq_mhz = source_freq_mhz;
                divider = 1;
            }
            break;
        case RTC_CPU_FREQ_80M:
            source = RTC_CPU_FREQ_SRC_PLL;
            source_freq_mhz = RTC_PLL_FREQ_320M;
            divider = 4;
            freq_mhz = 80;
            break;
        case RTC_CPU_FREQ_160M:
            source = RTC_CPU_FREQ_SRC_PLL;
            source_freq_mhz = RTC_PLL_FREQ_320M;
            divider = 2;
            freq_mhz = 160;
            break;
        case RTC_CPU_FREQ_240M:
            source = RTC_CPU_FREQ_SRC_PLL;
            source_freq_mhz = RTC_PLL_FREQ_480M;
            divider = 2;
            freq_mhz = 240;
            break;
        default:
            SOC_LOGE(TAG, "invalid rtc_cpu_freq_t value");
            abort();
    }

    *out_config = (rtc_cpu_freq_config_t) {
        .source = source,
        .source_freq_mhz = source_freq_mhz,
        .div = divider,
        .freq_mhz = freq_mhz
    };
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
        source_freq_mhz = RTC_PLL_FREQ_320M;
        divider = 4;
    } else if (freq_mhz == 160) {
        real_freq_mhz = freq_mhz;
        source = RTC_CPU_FREQ_SRC_PLL;
        source_freq_mhz = RTC_PLL_FREQ_320M;
        divider = 2;
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
    uint32_t soc_clk_sel = REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_SOC_CLK_SEL);
    if (soc_clk_sel != RTC_CNTL_SOC_CLK_SEL_XTL) {
        rtc_clk_cpu_freq_to_xtal(xtal_freq, 1);
        rtc_clk_wait_for_slow_cycle();
    }
    if (soc_clk_sel == RTC_CNTL_SOC_CLK_SEL_PLL) {
        rtc_clk_bbpll_disable();
    }
    if (config->source == RTC_CPU_FREQ_SRC_XTAL) {
        if (config->div > 1) {
            rtc_clk_cpu_freq_to_xtal(config->freq_mhz, config->div);
        }
    } else if (config->source == RTC_CPU_FREQ_SRC_PLL) {
        rtc_clk_bbpll_enable();
        rtc_clk_wait_for_slow_cycle();
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
    uint32_t soc_clk_sel = REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_SOC_CLK_SEL);
    switch (soc_clk_sel) {
        case RTC_CNTL_SOC_CLK_SEL_XTL: {
            source = RTC_CPU_FREQ_SRC_XTAL;
            div = REG_GET_FIELD(APB_CTRL_SYSCLK_CONF_REG, APB_CTRL_PRE_DIV_CNT) + 1;
            source_freq_mhz = (uint32_t) rtc_clk_xtal_freq_get();
            freq_mhz = source_freq_mhz / div;
        }
        break;
        case RTC_CNTL_SOC_CLK_SEL_PLL: {
            source = RTC_CPU_FREQ_SRC_PLL;
            uint32_t cpuperiod_sel = DPORT_REG_GET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_CPUPERIOD_SEL);
            if (cpuperiod_sel == DPORT_CPUPERIOD_SEL_80) {
                source_freq_mhz = RTC_PLL_FREQ_320M;
                div = 4;
                freq_mhz = 80;
            } else if (cpuperiod_sel == DPORT_CPUPERIOD_SEL_160) {
                source_freq_mhz = RTC_PLL_FREQ_320M;
                div = 2;
                freq_mhz = 160;
            } else if (cpuperiod_sel == DPORT_CPUPERIOD_SEL_240) {
                source_freq_mhz = RTC_PLL_FREQ_480M;
                div = 2;
                freq_mhz = 240;
            } else {
                SOC_LOGE(TAG, "unsupported frequency configuration");
                abort();
            }
            break;
        }
        case RTC_CNTL_SOC_CLK_SEL_8M:
            source = RTC_CPU_FREQ_SRC_8M;
            source_freq_mhz = 8;
            div = 1;
            freq_mhz = source_freq_mhz;
            break;
        case RTC_CNTL_SOC_CLK_SEL_APLL:
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

rtc_xtal_freq_t rtc_clk_xtal_freq_get(void)
{
    /* We may have already written XTAL value into RTC_XTAL_FREQ_REG */
    uint32_t xtal_freq_reg = READ_PERI_REG(RTC_XTAL_FREQ_REG);
    if (!clk_val_is_valid(xtal_freq_reg)) {
        return RTC_XTAL_FREQ_AUTO;
    }
    return reg_val_to_clk_val(xtal_freq_reg & ~RTC_DISABLE_ROM_LOG);
}

void rtc_clk_xtal_freq_update(rtc_xtal_freq_t xtal_freq)
{
    uint32_t reg = READ_PERI_REG(RTC_XTAL_FREQ_REG) & RTC_DISABLE_ROM_LOG;
    if (reg == RTC_DISABLE_ROM_LOG) {
        xtal_freq |= 1;
    }
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

/* Name used in libphy.a:phy_chip_v7.o
 * TODO: update the library to use rtc_clk_xtal_freq_get
 */
rtc_xtal_freq_t rtc_get_xtal(void) __attribute__((alias("rtc_clk_xtal_freq_get")));

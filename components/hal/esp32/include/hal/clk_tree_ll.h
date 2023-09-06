/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc.h"
#include "soc/clk_tree_defs.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_io_reg.h"
#include "soc/dport_reg.h"
#include "soc/syscon_reg.h"
#include "hal/regi2c_ctrl.h"
#include "soc/regi2c_bbpll.h"
#include "soc/regi2c_apll.h"
#include "hal/assert.h"
#include "esp32/rom/rtc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MHZ                 (1000000)

#define CLK_LL_PLL_80M_FREQ_MHZ    (80)
#define CLK_LL_PLL_160M_FREQ_MHZ   (160)
#define CLK_LL_PLL_240M_FREQ_MHZ   (240)

#define CLK_LL_PLL_320M_FREQ_MHZ   (320)
#define CLK_LL_PLL_480M_FREQ_MHZ   (480)

#define CLK_LL_AHB_MAX_FREQ_MHZ    CLK_LL_PLL_80M_FREQ_MHZ

/* BBPLL configuration parameters at reset */
#define CLK_LL_BBPLL_IR_CAL_DELAY_VAL          0x18
#define CLK_LL_BBPLL_IR_CAL_EXT_CAP_VAL        0x20
#define CLK_LL_BBPLL_OC_ENB_FCAL_VAL           0x9a
#define CLK_LL_BBPLL_OC_ENB_VCON_VAL           0x00
#define CLK_LL_BBPLL_BBADC_CAL_7_0_VAL         0x00

/* BBPLL configuration parameters */
#define CLK_LL_BBPLL_ENDIV5_VAL_320M           0x43
#define CLK_LL_BBPLL_BBADC_DSMP_VAL_320M       0x84
#define CLK_LL_BBPLL_ENDIV5_VAL_480M           0xc3
#define CLK_LL_BBPLL_BBADC_DSMP_VAL_480M       0x74

/* APLL configuration parameters */
#define CLK_LL_APLL_SDM_STOP_VAL_1             0x09
#define CLK_LL_APLL_SDM_STOP_VAL_2_REV0        0x69
#define CLK_LL_APLL_SDM_STOP_VAL_2_REV1        0x49

/* APLL calibration parameters */
#define CLK_LL_APLL_CAL_DELAY_1                0x0f
#define CLK_LL_APLL_CAL_DELAY_2                0x3f
#define CLK_LL_APLL_CAL_DELAY_3                0x1f

/* XTAL32K configuration parameters for 32kHz crystal */
#define CLK_LL_XTAL_32K_DAC_VAL                1
#define CLK_LL_XTAL_32K_DRES_VAL               3
#define CLK_LL_XTAL_32K_DBIAS_VAL              0

/* XTAL32K configuration parameters for external oscillator clock */
#define CLK_LL_XTAL_32K_EXT_DAC_VAL            2
#define CLK_LL_XTAL_32K_EXT_DRES_VAL           3
#define CLK_LL_XTAL_32K_EXT_DBIAS_VAL          1

/* XTAL32K configuration parameters for fast startup with bootstrap */
#define CLK_LL_XTAL_32K_BOOTSTRAP_DAC_VAL      3
#define CLK_LL_XTAL_32K_BOOTSTRAP_DRES_VAL     3
#define CLK_LL_XTAL_32K_BOOTSTRAP_DBIAS_VAL    0

/* APLL multiplier output frequency range */
// apll_multiplier_out = xtal_freq * (4 + sdm2 + sdm1/256 + sdm0/65536)
#define CLK_LL_APLL_MULTIPLIER_MIN_HZ (350000000) // 350 MHz
#define CLK_LL_APLL_MULTIPLIER_MAX_HZ (500000000) // 500 MHz

/* APLL output frequency range */
#define CLK_LL_APLL_MIN_HZ    (5303031)   // 5.303031 MHz, refer to 'periph_rtc_apll_freq_set' for the calculation
#define CLK_LL_APLL_MAX_HZ    (125000000) // 125MHz, refer to 'periph_rtc_apll_freq_set' for the calculation

/**
 * @brief XTAL32K_CLK enable modes
 */
typedef enum {
    CLK_LL_XTAL32K_ENABLE_MODE_CRYSTAL,       //!< Enable the external 32kHz crystal for XTAL32K_CLK
    CLK_LL_XTAL32K_ENABLE_MODE_EXTERNAL,      //!< Enable the external clock signal for XTAL32K_CLK
    CLK_LL_XTAL32K_ENABLE_MODE_BOOTSTRAP,     //!< Bootstrap the crystal oscillator for faster XTAL32K_CLK start up */
} clk_ll_xtal32k_enable_mode_t;

/**
 * @brief Power up internal I2C bus
 */
static inline __attribute__((always_inline)) void clk_ll_i2c_pu(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_I2C_FORCE_PD);
}

/**
 * @brief Power down internal I2C bus
 */
static inline __attribute__((always_inline)) void clk_ll_i2c_pd(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_I2C_FORCE_PD);
}

/**
 * @brief Power up BBPLL circuit
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_enable(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG,
             RTC_CNTL_BB_I2C_FORCE_PD | RTC_CNTL_BBPLL_FORCE_PD |
             RTC_CNTL_BBPLL_I2C_FORCE_PD);
    // Reset BBPLL configuration
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_IR_CAL_DELAY, CLK_LL_BBPLL_IR_CAL_DELAY_VAL);
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_IR_CAL_EXT_CAP, CLK_LL_BBPLL_IR_CAL_EXT_CAP_VAL);
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_OC_ENB_FCAL, CLK_LL_BBPLL_OC_ENB_FCAL_VAL);
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_OC_ENB_VCON, CLK_LL_BBPLL_OC_ENB_VCON_VAL);
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_BBADC_CAL_7_0, CLK_LL_BBPLL_BBADC_CAL_7_0_VAL);
}

/**
 * @brief Power down BBPLL circuit
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_disable(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG,
            RTC_CNTL_BB_I2C_FORCE_PD | RTC_CNTL_BBPLL_FORCE_PD |
            RTC_CNTL_BBPLL_I2C_FORCE_PD);
}

/**
 * @brief Power up APLL circuit
 */
static inline __attribute__((always_inline)) void clk_ll_apll_enable(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PLLA_FORCE_PD);
    SET_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PLLA_FORCE_PU);
}

/**
 * @brief Power down APLL circuit
 */
static inline __attribute__((always_inline)) void clk_ll_apll_disable(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PLLA_FORCE_PD);
    CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PLLA_FORCE_PU);
}

/**
 * @brief Check whether APLL is under force power down state
 *
 * @return True if APLL is under force power down; otherwise false
 */
static inline __attribute__((always_inline)) bool clk_ll_apll_is_fpd(void)
{
    return REG_GET_FIELD(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PLLA_FORCE_PD);
}

/**
 * @brief Get APLL configuration which can be used to calculate APLL frequency
 *
 * @param[out] o_div  Frequency divider, 0..31
 * @param[out] sdm0  Frequency adjustment parameter, 0..255
 * @param[out] sdm1  Frequency adjustment parameter, 0..255
 * @param[out] sdm2  Frequency adjustment parameter, 0..63
 */
static inline __attribute__((always_inline)) void clk_ll_apll_get_config(uint32_t *o_div, uint32_t *sdm0, uint32_t *sdm1, uint32_t *sdm2)
{
    *o_div = REGI2C_READ_MASK(I2C_APLL, I2C_APLL_OR_OUTPUT_DIV);
    *sdm0 = REGI2C_READ_MASK(I2C_APLL, I2C_APLL_DSDM0);
    *sdm1 = REGI2C_READ_MASK(I2C_APLL, I2C_APLL_DSDM1);
    *sdm2 = REGI2C_READ_MASK(I2C_APLL, I2C_APLL_DSDM2);
}

/**
 * @brief Set APLL configuration
 *
 * @param is_rev0 True if chip version is rev0
 * @param o_div  Frequency divider, 0..31
 * @param sdm0  Frequency adjustment parameter, 0..255
 * @param sdm1  Frequency adjustment parameter, 0..255
 * @param sdm2  Frequency adjustment parameter, 0..63
 */
static inline __attribute__((always_inline)) void clk_ll_apll_set_config(bool is_rev0, uint32_t o_div, uint32_t sdm0, uint32_t sdm1, uint32_t sdm2)
{
    uint8_t sdm_stop_val_2 = CLK_LL_APLL_SDM_STOP_VAL_2_REV1;
    if (is_rev0) {
        sdm0 = 0;
        sdm1 = 0;
        sdm_stop_val_2 = CLK_LL_APLL_SDM_STOP_VAL_2_REV0;
    }
    REGI2C_WRITE_MASK(I2C_APLL, I2C_APLL_DSDM2, sdm2);
    REGI2C_WRITE_MASK(I2C_APLL, I2C_APLL_DSDM0, sdm0);
    REGI2C_WRITE_MASK(I2C_APLL, I2C_APLL_DSDM1, sdm1);
    REGI2C_WRITE(I2C_APLL, I2C_APLL_SDM_STOP, CLK_LL_APLL_SDM_STOP_VAL_1);
    REGI2C_WRITE(I2C_APLL, I2C_APLL_SDM_STOP, sdm_stop_val_2);
    REGI2C_WRITE_MASK(I2C_APLL, I2C_APLL_OR_OUTPUT_DIV, o_div);
}

/**
 * @brief Set APLL calibration parameters
 */
static inline __attribute__((always_inline)) void clk_ll_apll_set_calibration(void)
{
    REGI2C_WRITE(I2C_APLL, I2C_APLL_IR_CAL_DELAY, CLK_LL_APLL_CAL_DELAY_1);
    REGI2C_WRITE(I2C_APLL, I2C_APLL_IR_CAL_DELAY, CLK_LL_APLL_CAL_DELAY_2);
    REGI2C_WRITE(I2C_APLL, I2C_APLL_IR_CAL_DELAY, CLK_LL_APLL_CAL_DELAY_3);
}

/**
 * @brief Check whether APLL calibration is done
 *
 * @return True if calibration is done; otherwise false
 */
static inline __attribute__((always_inline)) bool clk_ll_apll_calibration_is_done(void)
{
    return REGI2C_READ_MASK(I2C_APLL, I2C_APLL_OR_CAL_END);
}

/**
 * @brief Enable the 32kHz crystal oscillator
 *
 * @param mode Used to determine the xtal32k configuration parameters
 */
static inline __attribute__((always_inline)) void clk_ll_xtal32k_enable(clk_ll_xtal32k_enable_mode_t mode)
{
    // Configure xtal32k
    // Default mode as CLK_LL_XTAL32K_ENABLE_MODE_CRYSTAL
    uint32_t dac = CLK_LL_XTAL_32K_DAC_VAL;     // current
    uint32_t dres = CLK_LL_XTAL_32K_DRES_VAL;   // resistance
    uint32_t dbias = CLK_LL_XTAL_32K_DBIAS_VAL; // dbias voltage
    if (mode == CLK_LL_XTAL32K_ENABLE_MODE_EXTERNAL) {
        dac = CLK_LL_XTAL_32K_EXT_DAC_VAL;
        dres = CLK_LL_XTAL_32K_EXT_DRES_VAL;
        dbias = CLK_LL_XTAL_32K_EXT_DBIAS_VAL;
    } else if (mode == CLK_LL_XTAL32K_ENABLE_MODE_BOOTSTRAP) {
        dac = CLK_LL_XTAL_32K_BOOTSTRAP_DAC_VAL;
        dres = CLK_LL_XTAL_32K_BOOTSTRAP_DRES_VAL;
        dbias = CLK_LL_XTAL_32K_BOOTSTRAP_DBIAS_VAL;
    }
    REG_SET_FIELD(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_DAC_XTAL_32K, dac);
    REG_SET_FIELD(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_DRES_XTAL_32K, dres);
    REG_SET_FIELD(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_DBIAS_XTAL_32K, dbias);
    // Enable xtal32k xpd status
    SET_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_XPD_XTAL_32K_M);
}

/**
 * @brief Disable the 32kHz crystal oscillator
 */
static inline __attribute__((always_inline)) void clk_ll_xtal32k_disable(void)
{
    // Disable xtal32k xpd status
    CLEAR_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_XPD_XTAL_32K_M);
}

/**
 * @brief Get the state of the 32kHz crystal clock
 *
 * @return True if the 32kHz XTAL is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_xtal32k_is_enabled(void)
{
    return GET_PERI_REG_MASK(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_XPD_XTAL_32K);
}

/**
 * @brief Enable the internal oscillator output for RC_FAST_CLK
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_enable(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M);
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_CK8M_WAIT, RTC_CK8M_ENABLE_WAIT_DEFAULT);
}

/**
 * @brief Disable the internal oscillator output for RC_FAST_CLK
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_disable(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M);
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_CK8M_WAIT, RTC_CNTL_CK8M_WAIT_DEFAULT);
}

/**
 * @brief Get the state of the internal oscillator for RC_FAST_CLK
 *
 * @return True if the oscillator is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_rc_fast_is_enabled(void)
{
    return GET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M) == 0;
}

/**
 * @brief Enable the output from the internal oscillator to be passed into a configurable divider,
 * which by default divides the input clock frequency by 256. i.e. RC_FAST_D256_CLK = RC_FAST_CLK / 256
 *
 * Divider values other than 256 may be configured, but this facility is not currently needed,
 * so is not exposed in the code.
 * The output of the divider, RC_FAST_D256_CLK, is referred as 8md256 or simply d256 in reg. descriptions.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_d256_enable(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M_DIV);
}

/**
 * @brief Disable the output from the internal oscillator to be passed into a configurable divider.
 * i.e. RC_FAST_D256_CLK = RC_FAST_CLK / 256
 *
 * Disabling this divider could reduce power consumption.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_d256_disable(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M_DIV);
}

/**
 * @brief Get the state of the divider which is applied to the output from the internal oscillator (RC_FAST_CLK)
 *
 * @return True if the divided output is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_rc_fast_d256_is_enabled(void)
{
    return GET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M_DIV) == 0;
}

/**
 * @brief Enable the digital RC_FAST_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_digi_enable(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_CLK8M_EN_M);
}

/**
 * @brief Disable the digital RC_FAST_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_digi_disable(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_CLK8M_EN_M);
}

/**
 * @brief Get the state of the digital RC_FAST_CLK
 *
 * @return True if the digital RC_FAST_CLK is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_rc_fast_digi_is_enabled(void)
{
    return GET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_CLK8M_EN_M);
}

/**
 * @brief Enable the digital RC_FAST_D256_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_d256_digi_enable(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_CLK8M_D256_EN_M);
}

/**
 * @brief Disable the digital RC_FAST_D256_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_d256_digi_disable(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_CLK8M_D256_EN_M);
}

/**
 * @brief Enable the digital XTAL32K_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_xtal32k_digi_enable(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_XTAL32K_EN_M);
}

/**
 * @brief Disable the digital XTAL32K_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_xtal32k_digi_disable(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_XTAL32K_EN_M);
}

/**
 * @brief Get the state of the digital XTAL32K_CLK
 *
 * @return True if the digital XTAL32K_CLK is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_xtal32k_digi_is_enabled(void)
{
    return REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_XTAL32K_EN);
}

/**
 * @brief Get PLL_CLK frequency
 *
 * @return PLL clock frequency, in MHz. Returns 0 if register field value is invalid.
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_bbpll_get_freq_mhz(void)
{
    // ESP32 BBPLL frequency is determined by the cpu freq sel
    uint32_t cpu_freq_sel = DPORT_REG_GET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_CPUPERIOD_SEL);
    switch (cpu_freq_sel) {
    case 0:
    case 1:
        return CLK_LL_PLL_320M_FREQ_MHZ;
    case 2:
        return CLK_LL_PLL_480M_FREQ_MHZ;
    default:
        // Invalid CPUPERIOD_SEL value
        return 0;
    }
}

/**
 * @brief Set BBPLL frequency from XTAL source (digital part)
 *
 * @param pll_freq_mhz PLL frequency, in MHz
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_set_freq_mhz(uint32_t pll_freq_mhz)
{
    (void)pll_freq_mhz;
    // No such operation on ESP32
}

/**
 * @brief Set BBPLL frequency from XTAL source (Analog part)
 *
 * @param pll_freq_mhz PLL frequency, in MHz
 * @param xtal_freq_mhz XTAL frequency, in MHz
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_set_config(uint32_t pll_freq_mhz, uint32_t xtal_freq_mhz)
{
    uint8_t div_ref;
    uint8_t div7_0;
    uint8_t div10_8;
    uint8_t lref;
    uint8_t dcur;
    uint8_t bw;

    if (pll_freq_mhz == CLK_LL_PLL_320M_FREQ_MHZ) {
        /* Configure 320M PLL */
        switch (xtal_freq_mhz) {
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
        REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_ENDIV5, CLK_LL_BBPLL_ENDIV5_VAL_320M);
        REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_BBADC_DSMP, CLK_LL_BBPLL_BBADC_DSMP_VAL_320M);
    } else {
        /* Configure 480M PLL */
        switch (xtal_freq_mhz) {
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
        REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_ENDIV5, CLK_LL_BBPLL_ENDIV5_VAL_480M);
        REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_BBADC_DSMP, CLK_LL_BBPLL_BBADC_DSMP_VAL_480M);
    }

    uint8_t i2c_bbpll_lref  = (lref << 7) | (div10_8 << 4) | (div_ref);
    uint8_t i2c_bbpll_div_7_0 = div7_0;
    uint8_t i2c_bbpll_dcur = (bw << 6) | dcur;
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_OC_LREF, i2c_bbpll_lref);
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_OC_DIV_7_0, i2c_bbpll_div_7_0);
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_OC_DCUR, i2c_bbpll_dcur);
}

/**
 * @brief Select the clock source for CPU_CLK
 *
 * @param in_sel One of the clock sources in soc_cpu_clk_src_t
 */
static inline __attribute__((always_inline)) void clk_ll_cpu_set_src(soc_cpu_clk_src_t in_sel)
{
    switch (in_sel) {
    case SOC_CPU_CLK_SRC_XTAL:
        REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_SOC_CLK_SEL, 0);
        break;
    case SOC_CPU_CLK_SRC_PLL:
        REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_SOC_CLK_SEL, 1);
        break;
    case SOC_CPU_CLK_SRC_RC_FAST:
        REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_SOC_CLK_SEL, 2);
        break;
    case SOC_CPU_CLK_SRC_APLL:
        REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_SOC_CLK_SEL, 3);
        break;
    default:
        // Unsupported CPU_CLK mux input sel
        abort();
    }
}

/**
 * @brief Get the clock source for CPU_CLK
 *
 * @return Currently selected clock source (one of soc_cpu_clk_src_t values)
 */
static inline __attribute__((always_inline)) soc_cpu_clk_src_t clk_ll_cpu_get_src(void)
{
    uint32_t clk_sel = REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_SOC_CLK_SEL);
    switch (clk_sel) {
    case 0:
        return SOC_CPU_CLK_SRC_XTAL;
    case 1:
        return SOC_CPU_CLK_SRC_PLL;
    case 2:
        return SOC_CPU_CLK_SRC_RC_FAST;
    case 3:
        return SOC_CPU_CLK_SRC_APLL;
    default:
        return SOC_CPU_CLK_SRC_INVALID;
    }
}

/**
 * @brief Set CPU frequency from PLL clock
 *
 * @param cpu_mhz CPU frequency value, in MHz
 */
static inline __attribute__((always_inline)) void clk_ll_cpu_set_freq_mhz_from_pll(uint32_t cpu_mhz)
{
    switch (cpu_mhz) {
    case CLK_LL_PLL_80M_FREQ_MHZ:
        DPORT_REG_WRITE(DPORT_CPU_PER_CONF_REG, 0);
        break;
    case CLK_LL_PLL_160M_FREQ_MHZ:
        DPORT_REG_WRITE(DPORT_CPU_PER_CONF_REG, 1);
        break;
    case CLK_LL_PLL_240M_FREQ_MHZ:
        DPORT_REG_WRITE(DPORT_CPU_PER_CONF_REG, 2);
        break;
    default:
        // Unsupported CPU_CLK freq from PLL
        abort();
    }
}

/**
 * @brief Get CPU_CLK frequency from PLL_CLK source
 *
 * @return CPU clock frequency, in MHz. Returns 0 if register field value is invalid.
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_cpu_get_freq_mhz_from_pll(void)
{
    uint32_t cpu_freq_sel = DPORT_REG_GET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_CPUPERIOD_SEL);
    switch (cpu_freq_sel) {
    case 0:
        return CLK_LL_PLL_80M_FREQ_MHZ;
    case 1:
        return CLK_LL_PLL_160M_FREQ_MHZ;
    case 2:
        return CLK_LL_PLL_240M_FREQ_MHZ;
    default:
        // Invalid CPUPERIOD_SEL value
        return 0;
    }
}

/**
 * @brief Set CPU_CLK's XTAL/FAST_RC clock source path divider
 *
 * @param divider Divider. Usually this divider is set to 1 in bootloader stage. PRE_DIV_CNT = divider - 1.
 */
static inline __attribute__((always_inline)) void clk_ll_cpu_set_divider(uint32_t divider)
{
    HAL_ASSERT(divider > 0);
    REG_SET_FIELD(SYSCON_SYSCLK_CONF_REG, SYSCON_PRE_DIV_CNT, divider - 1);
}

/**
 * @brief Get CPU_CLK's XTAL/FAST_RC clock source path divider
 *
 * @return Divider. Divider = (PRE_DIV_CNT + 1).
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_cpu_get_divider(void)
{
    return REG_GET_FIELD(SYSCON_SYSCLK_CONF_REG, SYSCON_PRE_DIV_CNT) + 1;
}

/**
 * @brief Get CPU_CLK's APLL clock source path divider
 *
 * @return Divider. Returns 0 means invalid.
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_cpu_get_divider_from_apll(void)
{
    // APLL path divider choice shares the same register with CPUPERIOD_SEL
    uint32_t cpu_freq_sel = DPORT_REG_GET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_CPUPERIOD_SEL);
    switch (cpu_freq_sel) {
    case 0:
        return 4;
    case 1:
        return 2;
    default:
        // Invalid CPUPERIOD_SEL value if APLL is the clock source
        return 0;
    }
}

/**
 * @brief Set REF_TICK divider to make REF_TICK frequency at 1MHz
 *
 * @param cpu_clk_src Selected CPU clock source (one of soc_cpu_clk_src_t values)
 * @param cpu_freq_mhz CPU frequency value, in MHz
 *
 * Divider = APB_CLK freq in Hz / 1MHz. Value in register = divider - 1.
 */
static inline __attribute__((always_inline)) void clk_ll_ref_tick_set_divider(soc_cpu_clk_src_t cpu_clk_src, uint32_t cpu_freq_mhz)
{
    uint32_t apb_freq_mhz;
    switch (cpu_clk_src) {
    case SOC_CPU_CLK_SRC_XTAL:
        apb_freq_mhz = cpu_freq_mhz;
        REG_WRITE(SYSCON_XTAL_TICK_CONF_REG, apb_freq_mhz - 1);
        break;
    case SOC_CPU_CLK_SRC_PLL:
        apb_freq_mhz = 80;
        REG_WRITE(SYSCON_PLL_TICK_CONF_REG, apb_freq_mhz - 1);
        break;
    case SOC_CPU_CLK_SRC_RC_FAST:
        apb_freq_mhz = cpu_freq_mhz;
        REG_WRITE(SYSCON_CK8M_TICK_CONF_REG, apb_freq_mhz - 1);
        break;
    case SOC_CPU_CLK_SRC_APLL:
        apb_freq_mhz = cpu_freq_mhz >> 1;
        REG_WRITE(SYSCON_APLL_TICK_CONF_REG, apb_freq_mhz - 1);
        break;
    default:
        // Unsupported CPU_CLK mux input sel
        abort();
    }
}

/**
 * @brief Select the clock source for RTC_SLOW_CLK
 *
 * @param in_sel One of the clock sources in soc_rtc_slow_clk_src_t
 */
static inline __attribute__((always_inline)) void clk_ll_rtc_slow_set_src(soc_rtc_slow_clk_src_t in_sel)
{
    switch (in_sel) {
    case SOC_RTC_SLOW_CLK_SRC_RC_SLOW:
        REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ANA_CLK_RTC_SEL, 0);
        break;
    case SOC_RTC_SLOW_CLK_SRC_XTAL32K:
        REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ANA_CLK_RTC_SEL, 1);
        break;
    case SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256:
        REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ANA_CLK_RTC_SEL, 2);
        break;
    default:
        // Unsupported RTC_SLOW_CLK mux input sel
        abort();
    }
}

/**
 * @brief Get the clock source for RTC_SLOW_CLK
 *
 * @return Currently selected clock source (one of soc_rtc_slow_clk_src_t values)
 */
static inline __attribute__((always_inline)) soc_rtc_slow_clk_src_t clk_ll_rtc_slow_get_src(void)
{
    uint32_t clk_sel = REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ANA_CLK_RTC_SEL);
    switch (clk_sel) {
    case 0:
        return SOC_RTC_SLOW_CLK_SRC_RC_SLOW;
    case 1:
        return SOC_RTC_SLOW_CLK_SRC_XTAL32K;
    case 2:
        return SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256;
    default:
        // Invalid ANA_CLK_RTC_SEL value
        return SOC_RTC_SLOW_CLK_SRC_INVALID;
    }
}

/**
 * @brief Select the clock source for RTC_FAST_CLK
 *
 * @param in_sel One of the clock sources in soc_rtc_fast_clk_src_t
 */
static inline __attribute__((always_inline)) void clk_ll_rtc_fast_set_src(soc_rtc_fast_clk_src_t in_sel)
{
    switch (in_sel) {
    case SOC_RTC_FAST_CLK_SRC_XTAL_D4:
        REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_FAST_CLK_RTC_SEL, 0);
        break;
    case SOC_RTC_FAST_CLK_SRC_RC_FAST:
        REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_FAST_CLK_RTC_SEL, 1);
        break;
    default:
        // Unsupported RTC_FAST_CLK mux input sel
        abort();
    }
}

/**
 * @brief Get the clock source for RTC_FAST_CLK
 *
 * @return Currently selected clock source (one of soc_rtc_fast_clk_src_t values)
 */
static inline __attribute__((always_inline)) soc_rtc_fast_clk_src_t clk_ll_rtc_fast_get_src(void)
{
    uint32_t clk_sel = REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_FAST_CLK_RTC_SEL);
    switch (clk_sel) {
    case 0:
        return SOC_RTC_FAST_CLK_SRC_XTAL_D4;
    case 1:
        return SOC_RTC_FAST_CLK_SRC_RC_FAST;
    default:
        return SOC_RTC_FAST_CLK_SRC_INVALID;
    }
}

/**
 * @brief Set RC_FAST_CLK divider. The output from the divider is passed into rtc_fast_clk MUX.
 *
 * @param divider Divider of RC_FAST_CLK. Usually this divider is set to 1 (reg. value is 0) in bootloader stage.
 */
static inline void clk_ll_rc_fast_set_divider(uint32_t divider)
{
    HAL_ASSERT(divider > 0);
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL, divider - 1);
}

/**
 * @brief Get RC_FAST_CLK divider
 *
 * @return Divider. Divider = (CK8M_DIV_SEL + 1).
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_rc_fast_get_divider(void)
{
    return REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL) + 1;
}

/************************* RTC STORAGE REGISTER STORE/LOAD **************************/
/**
 * @brief Store XTAL_CLK frequency in RTC storage register
 *
 * Value of RTC_XTAL_FREQ_REG is stored as two copies in lower and upper 16-bit
 * halves. These are the routines to work with that representation.
 *
 * @param xtal_freq_mhz XTAL frequency, in MHz. The frequency must necessarily be even,
 * otherwise there will be a conflict with the low bit, which is used to disable logs
 * in the ROM code.
 */
static inline __attribute__((always_inline)) void clk_ll_xtal_store_freq_mhz(uint32_t xtal_freq_mhz)
{
    // Read the status of whether disabling logging from ROM code
    uint32_t reg = READ_PERI_REG(RTC_XTAL_FREQ_REG) & RTC_DISABLE_ROM_LOG;
    // If so, need to write back this setting
    if (reg == RTC_DISABLE_ROM_LOG) {
        xtal_freq_mhz |= 1;
    }
    WRITE_PERI_REG(RTC_XTAL_FREQ_REG, (xtal_freq_mhz & UINT16_MAX) | ((xtal_freq_mhz & UINT16_MAX) << 16));
}

/**
 * @brief Load XTAL_CLK frequency from RTC storage register
 *
 * Value of RTC_XTAL_FREQ_REG is stored as two copies in lower and upper 16-bit
 * halves. These are the routines to work with that representation.
 *
 * @return XTAL frequency, in MHz. Returns 0 if format in reg is invalid.
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_xtal_load_freq_mhz(void)
{
    // Read from RTC storage register
    uint32_t xtal_freq_reg = READ_PERI_REG(RTC_XTAL_FREQ_REG);
    if ((xtal_freq_reg & 0xFFFF) == ((xtal_freq_reg >> 16) & 0xFFFF) &&
        xtal_freq_reg != 0 && xtal_freq_reg != UINT32_MAX) {
        return xtal_freq_reg & ~RTC_DISABLE_ROM_LOG & UINT16_MAX;
    }
    // If the format in reg is invalid or haven't written XTAL value into RTC_XTAL_FREQ_REG
    return 0;
}

/**
 * @brief Store APB_CLK frequency in RTC storage register
 *
 * Value of RTC_APB_FREQ_REG is stored as two copies in lower and upper 16-bit
 * halves. These are the routines to work with that representation.
 *
 * @param apb_freq_hz APB frequency, in Hz
 */
static inline __attribute__((always_inline)) void clk_ll_apb_store_freq_hz(uint32_t apb_freq_hz)
{
    uint32_t val = apb_freq_hz >> 12;
    WRITE_PERI_REG(RTC_APB_FREQ_REG, (val & UINT16_MAX) | ((val & UINT16_MAX) << 16));
}

/**
 * @brief Load APB_CLK frequency from RTC storage register
 *
 * Value of RTC_APB_FREQ_REG is stored as two copies in lower and upper 16-bit
 * halves. These are the routines to work with that representation.
 *
 * @return The stored APB frequency, in Hz
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_apb_load_freq_hz(void)
{
    // Read from RTC storage register
    uint32_t apb_freq_hz = (READ_PERI_REG(RTC_APB_FREQ_REG) & UINT16_MAX) << 12;
    // Round to the nearest MHz
    apb_freq_hz += MHZ / 2;
    uint32_t remainder = apb_freq_hz % MHZ;
    return apb_freq_hz - remainder;
}

/**
 * @brief Store RTC_SLOW_CLK calibration value in RTC storage register
 *
 * Value of RTC_SLOW_CLK_CAL_REG has to be in the same format as returned by rtc_clk_cal (microseconds,
 * in Q13.19 fixed-point format).
 *
 * @param cal_value The calibration value of slow clock period in microseconds, in Q13.19 fixed point format
 */
static inline __attribute__((always_inline)) void clk_ll_rtc_slow_store_cal(uint32_t cal_value)
{
    REG_WRITE(RTC_SLOW_CLK_CAL_REG, cal_value);
}

/**
 * @brief Load the calibration value of RTC_SLOW_CLK frequency from RTC storage register
 *
 * This value gets updated (i.e. rtc slow clock gets calibrated) every time RTC_SLOW_CLK source switches
 *
 * @return The calibration value of slow clock period in microseconds, in Q13.19 fixed point format
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_rtc_slow_load_cal(void)
{
    return REG_READ(RTC_SLOW_CLK_CAL_REG);
}

#ifdef __cplusplus
}
#endif

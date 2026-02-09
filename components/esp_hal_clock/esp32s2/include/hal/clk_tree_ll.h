/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc.h"
#include "soc/clk_tree_defs.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/dport_reg.h"
#include "soc/syscon_reg.h"
#include "soc/io_mux_reg.h"
#include "hal/clkout_channel.h"
#include "hal/regi2c_ctrl.h"
#include "soc/regi2c_bbpll.h"
#include "soc/regi2c_apll.h"
#include "soc/timer_group_struct.h"
#include "hal/assert.h"
#include "esp32s2/rom/rtc.h"

#define MHZ                 (1000000)

#define CLK_LL_PLL_80M_FREQ_MHZ    (80)
#define CLK_LL_PLL_160M_FREQ_MHZ   (160)
#define CLK_LL_PLL_240M_FREQ_MHZ   (240)

#define CLK_LL_PLL_320M_FREQ_MHZ   (320)
#define CLK_LL_PLL_480M_FREQ_MHZ   (480)

#define CLK_LL_AHB_MAX_FREQ_MHZ    CLK_LL_PLL_80M_FREQ_MHZ

// ESP32S2 only supports 40MHz crystal
#define CLK_LL_XTAL_FREQ_MHZ       (SOC_XTAL_FREQ_40M)

/* RC_FAST clock enable/disable wait time */
#define CLK_LL_RC_FAST_WAIT_DEFAULT            20
#define CLK_LL_RC_FAST_ENABLE_WAIT_DEFAULT     5

/* APLL configuration parameters */
#define CLK_LL_APLL_SDM_STOP_VAL_1         0x09
#define CLK_LL_APLL_SDM_STOP_VAL_2_REV0    0x69
#define CLK_LL_APLL_SDM_STOP_VAL_2_REV1    0x49

/* APLL calibration parameters */
#define CLK_LL_APLL_CAL_DELAY_1            0x0f
#define CLK_LL_APLL_CAL_DELAY_2            0x3f
#define CLK_LL_APLL_CAL_DELAY_3            0x1f

/* APLL multiplier output frequency range */
// apll_multiplier_out = xtal_freq * (4 + sdm2 + sdm1/256 + sdm0/65536)
#define CLK_LL_APLL_MULTIPLIER_MIN_HZ (350000000) // 350 MHz
#define CLK_LL_APLL_MULTIPLIER_MAX_HZ (500000000) // 500 MHz

/* APLL output frequency range */
#define CLK_LL_APLL_MIN_HZ    (5303031)   // 5.303031 MHz, refer to 'periph_rtc_apll_freq_set' for the calculation
#define CLK_LL_APLL_MAX_HZ    (125000000) // 125MHz, refer to 'periph_rtc_apll_freq_set' for the calculation

#define CLK_LL_XTAL32K_CONFIG_DEFAULT() { \
    .dac = 3, \
    .dres = 3, \
    .dgm = 3, \
    .dbuf = 1, \
}

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief XTAL32K_CLK enable modes
 */
typedef enum {
    CLK_LL_XTAL32K_ENABLE_MODE_CRYSTAL,       //!< Enable the external 32kHz crystal for XTAL32K_CLK
    CLK_LL_XTAL32K_ENABLE_MODE_EXTERNAL,      //!< Enable the external clock signal for XTAL32K_CLK
    CLK_LL_XTAL32K_ENABLE_MODE_BOOTSTRAP,     //!< Bootstrap the crystal oscillator for faster XTAL32K_CLK start up */
} clk_ll_xtal32k_enable_mode_t;

/**
 * @brief XTAL32K_CLK configuration structure
 */
typedef struct {
    uint32_t dac : 6;
    uint32_t dres : 3;
    uint32_t dgm : 3;
    uint32_t dbuf: 1;
} clk_ll_xtal32k_config_t;

/**
 * @brief Power up BBPLL circuit
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_enable(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BB_I2C_FORCE_PD |
                        RTC_CNTL_BBPLL_FORCE_PD | RTC_CNTL_BBPLL_I2C_FORCE_PD);
}

/**
 * @brief Power down BBPLL circuit
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_disable(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BB_I2C_FORCE_PD |
                      RTC_CNTL_BBPLL_FORCE_PD | RTC_CNTL_BBPLL_I2C_FORCE_PD);
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
 * @param o_div  Frequency divider, 0..31
 * @param sdm0  Frequency adjustment parameter, 0..255
 * @param sdm1  Frequency adjustment parameter, 0..255
 * @param sdm2  Frequency adjustment parameter, 0..63
 */
static inline __attribute__((always_inline)) void clk_ll_apll_set_config(uint32_t o_div, uint32_t sdm0, uint32_t sdm1, uint32_t sdm2)
{
    REGI2C_WRITE_MASK(I2C_APLL, I2C_APLL_DSDM2, sdm2);
    REGI2C_WRITE_MASK(I2C_APLL, I2C_APLL_DSDM0, sdm0);
    REGI2C_WRITE_MASK(I2C_APLL, I2C_APLL_DSDM1, sdm1);
    REGI2C_WRITE(I2C_APLL, I2C_APLL_SDM_STOP, CLK_LL_APLL_SDM_STOP_VAL_1);
    REGI2C_WRITE(I2C_APLL, I2C_APLL_SDM_STOP, CLK_LL_APLL_SDM_STOP_VAL_2_REV1);
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
    clk_ll_xtal32k_config_t cfg = CLK_LL_XTAL32K_CONFIG_DEFAULT();
    REG_SET_FIELD(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_DAC_XTAL_32K, cfg.dac);
    REG_SET_FIELD(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_DRES_XTAL_32K, cfg.dres);
    REG_SET_FIELD(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_DGM_XTAL_32K, cfg.dgm);
    REG_SET_FIELD(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_DBUF_XTAL_32K, cfg.dbuf);
    // Enable xtal32k xpd status
    SET_PERI_REG_MASK(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_XPD_XTAL_32K);
    if (mode == CLK_LL_XTAL32K_ENABLE_MODE_EXTERNAL) {
        /* TODO: external 32k source may need different settings */
        ;
    }
}

/**
 * @brief Disable the 32kHz crystal oscillator
 */
static inline __attribute__((always_inline)) void clk_ll_xtal32k_disable(void)
{
    // Set xtal32k xpd to be controlled by software
    SET_PERI_REG_MASK(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_XTAL32K_XPD_FORCE);
    // Disable xtal32k xpd status
    CLEAR_PERI_REG_MASK(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_XPD_XTAL_32K);
}

/**
 * @brief Get the state of the 32kHz crystal clock
 *
 * @return True if the 32kHz XTAL is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_xtal32k_is_enabled(void)
{
    uint32_t xtal_conf = READ_PERI_REG(RTC_CNTL_EXT_XTL_CONF_REG);
    /* If xtal xpd is controlled by software */
    bool xtal_xpd_sw = (xtal_conf & RTC_CNTL_XTAL32K_XPD_FORCE) >> RTC_CNTL_XTAL32K_XPD_FORCE_S;
    /* If xtal xpd software control is on */
    bool xtal_xpd_st = (xtal_conf & RTC_CNTL_XPD_XTAL_32K) >> RTC_CNTL_XPD_XTAL_32K_S;
    // disabled = xtal_xpd_sw && !xtal_xpd_st; enabled = !disabled
    bool enabled = !xtal_xpd_sw || xtal_xpd_st;
    return enabled;
}

/**
 * @brief Enable the internal oscillator output for RC_FAST_CLK
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_enable(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M);
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_CK8M_WAIT, CLK_LL_RC_FAST_ENABLE_WAIT_DEFAULT);
}

/**
 * @brief Disable the internal oscillator output for RC_FAST_CLK
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_disable(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ENB_CK8M);
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_CK8M_WAIT, CLK_LL_RC_FAST_WAIT_DEFAULT);
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
    uint32_t pll_freq_sel = DPORT_REG_GET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_PLL_FREQ_SEL);
    switch (pll_freq_sel) {
    case 0: // PLL_320M
        return CLK_LL_PLL_320M_FREQ_MHZ;
    case 1: // PLL_480M
        return CLK_LL_PLL_480M_FREQ_MHZ;
    default:
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
    switch (pll_freq_mhz) {
    case CLK_LL_PLL_320M_FREQ_MHZ: // PLL_320M
        CLEAR_PERI_REG_MASK(DPORT_CPU_PER_CONF_REG, DPORT_PLL_FREQ_SEL);
        break;
    case CLK_LL_PLL_480M_FREQ_MHZ: // PLL_480M
        SET_PERI_REG_MASK(DPORT_CPU_PER_CONF_REG, DPORT_PLL_FREQ_SEL);
        break;
    default:
        abort();
    }
}

/**
 * @brief Set BBPLL frequency from XTAL source (Analog part)
 *
 * @param pll_freq_mhz PLL frequency, in MHz
 * @param xtal_freq_mhz XTAL frequency, in MHz
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_set_config(uint32_t pll_freq_mhz, uint32_t xtal_freq_mhz)
{
    (void)xtal_freq_mhz;
    uint8_t div_ref;
    uint8_t div7_0;
    uint8_t dr1;
    uint8_t dr3;
    uint8_t dchgp;
    uint8_t dcur;

    if (pll_freq_mhz == CLK_LL_PLL_480M_FREQ_MHZ) {
        /* Configure 480M PLL */
        div_ref = 0;
        div7_0 = 8;
        dr1 = 0;
        dr3 = 0;
        dchgp = 5;
        dcur = 4;
        REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_MODE_HF, 0x6B);
    } else {
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
    uint8_t i2c_bbpll_dcur = (2 << I2C_BBPLL_OC_DLREF_SEL_LSB) | (1 << I2C_BBPLL_OC_DHREF_SEL_LSB) | dcur;
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_OC_REF_DIV, i2c_bbpll_lref);
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_OC_DIV_7_0, i2c_bbpll_div_7_0);
    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_OC_DR1, dr1);
    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_OC_DR3, dr3);
    REGI2C_WRITE(I2C_BBPLL, I2C_BBPLL_OC_DCUR, i2c_bbpll_dcur);
}

/**
 * @brief Enable BBPLL self-calibration
 */
static inline __attribute__((always_inline)) void clk_ll_bbpll_calibration_enable(void)
{
    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_IR_CAL_ENX_CAP, 1);
}

/**
 * @brief Check whether BBPLL calibration is done
 *
 * @param ext_cap Steps write to I2C_BBPLL_IR_CAL_EXT_CAP
 *
 * @return True if calibration is done; otherwise false
 */
static inline __attribute__((always_inline)) bool clk_ll_bbpll_calibration_is_done(uint32_t ext_cap)
{
    REGI2C_WRITE_MASK(I2C_BBPLL, I2C_BBPLL_IR_CAL_EXT_CAP, ext_cap);
    return REGI2C_READ_MASK(I2C_BBPLL, I2C_BBPLL_OR_CAL_CAP) == 0;
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
        REG_SET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_SOC_CLK_SEL, 0);
        break;
    case SOC_CPU_CLK_SRC_PLL:
        REG_SET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_SOC_CLK_SEL, 1);
        break;
    case SOC_CPU_CLK_SRC_RC_FAST:
        REG_SET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_SOC_CLK_SEL, 2);
        break;
    case SOC_CPU_CLK_SRC_APLL:
        REG_SET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_SOC_CLK_SEL, 3);
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
    uint32_t clk_sel = REG_GET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_SOC_CLK_SEL);
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
        REG_SET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_CPUPERIOD_SEL, 0);
        break;
    case CLK_LL_PLL_160M_FREQ_MHZ:
        REG_SET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_CPUPERIOD_SEL, 1);
        break;
    case CLK_LL_PLL_240M_FREQ_MHZ:
        REG_SET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_CPUPERIOD_SEL, 2);
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
        // When PLL frequency selection is 320MHz but CPU frequency selection is 240MHz, it is an undetermined state.
        // It is checked in the upper layer.
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
    REG_SET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_PRE_DIV_CNT, divider - 1);
}

/**
 * @brief Get CPU_CLK's XTAL/FAST_RC clock source path divider
 *
 * @return Divider. Divider = (PRE_DIV_CNT + 1).
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_cpu_get_divider(void)
{
    return REG_GET_FIELD(DPORT_SYSCLK_CONF_REG, DPORT_PRE_DIV_CNT) + 1;
}

/**
 * @brief Get CPU_CLK's APLL clock source path divider
 *
 * @return Divider. Returns 0 means invalid.
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_cpu_get_divider_from_apll(void)
{
    // APLL path divider choice depends on PLL_FREQ_SEL and CPUPERIOD_SEL
    uint32_t pll_freq_sel = DPORT_REG_GET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_PLL_FREQ_SEL);
    uint32_t cpu_freq_sel = DPORT_REG_GET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_CPUPERIOD_SEL);
    if (pll_freq_sel == 0 && cpu_freq_sel == 0) {
        return 4;
    } else if (pll_freq_sel == 0 && cpu_freq_sel == 1) {
        return 2;
    } else {
        // Invalid configuration if APLL is the clock source
        return 0;
    }
}

/**
 * @brief Set REF_TICK divider to make REF_TICK frequency at 1MHz
 *
 * @param cpu_clk_src Selected CPU clock source (one of soc_cpu_clk_src_t values)
 *
 * When PLL, APLL, or XTAL is set as CPU clock source, divider = XTAL_CLK freq in Hz / 1MHz.
 * When RC_FAST is set as CPU clock source, divider = RC_FAST_CLK freq in Hz / 1MHz.
 * Value in register = divider - 1.
 */
static inline __attribute__((always_inline)) void clk_ll_ref_tick_set_divider(soc_cpu_clk_src_t cpu_clk_src)
{
    switch (cpu_clk_src) {
    case SOC_CPU_CLK_SRC_XTAL:
    case SOC_CPU_CLK_SRC_PLL:
    case SOC_CPU_CLK_SRC_APLL:
        REG_SET_FIELD(SYSCON_TICK_CONF_REG, SYSCON_XTAL_TICK_NUM, CLK_LL_XTAL_FREQ_MHZ - 1);
        break;
    case SOC_CPU_CLK_SRC_RC_FAST:
        REG_SET_FIELD(SYSCON_TICK_CONF_REG, SYSCON_CK8M_TICK_NUM, 8 - 1);
        break;
    default:
        // Unsupported CPU_CLK mux input sel
        abort();
    }
}

/**
 * @brief Select the frequency calculation clock source for timergroup0
 *
 * @param clk_sel One of the clock sources in soc_clk_freq_calculation_src_t
 */
static inline __attribute__((always_inline)) void clk_ll_freq_calulation_set_target(soc_clk_freq_calculation_src_t clk_sel)
{
    switch (clk_sel) {
    case CLK_CAL_RC_SLOW:
        TIMERG0.rtccalicfg.rtc_cali_clk_sel = 0;
        break;
    case CLK_CAL_RC_FAST_D256:
        TIMERG0.rtccalicfg.rtc_cali_clk_sel = 1;
        break;
    case CLK_CAL_32K_XTAL:
        TIMERG0.rtccalicfg.rtc_cali_clk_sel = 2;
        break;
    default:
        // Unsupported CLK_CAL mux input
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
static inline __attribute__((always_inline)) void clk_ll_rc_fast_set_divider(uint32_t divider)
{
    HAL_ASSERT(divider > 0);
    CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL_VLD);
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL, divider - 1);
    SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL_VLD);
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

/**
 * @brief Set RC_SLOW_CLK divider
 *
 * @param divider Divider of RC_SLOW_CLK. Usually this divider is set to 1 (reg. value is 0) in bootloader stage.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_slow_set_divider(uint32_t divider)
{
    HAL_ASSERT(divider > 0);
    CLEAR_PERI_REG_MASK(RTC_CNTL_SLOW_CLK_CONF_REG, RTC_CNTL_ANA_CLK_DIV_VLD);
    REG_SET_FIELD(RTC_CNTL_SLOW_CLK_CONF_REG, RTC_CNTL_ANA_CLK_DIV, divider - 1);
    SET_PERI_REG_MASK(RTC_CNTL_SLOW_CLK_CONF_REG, RTC_CNTL_ANA_CLK_DIV_VLD);
}

/************************** CLOCK OUTPUT **************************/
/**
 * @brief Clock output channel configuration
 *
 * @param clk_sig The clock signal source to be mapped to GPIOs
 * @param channel_id The clock output channel ID
 */
static inline __attribute__((always_inline)) void clk_ll_bind_output_channel(soc_clkout_sig_id_t clk_sig, clock_out_channel_t channel_id)
{
    SET_PERI_REG_BITS(PIN_CTRL, CLKOUT_CHANNEL_MASK(channel_id), clk_sig, CLKOUT_CHANNEL_SHIFT(channel_id));
}

/**
 * @brief Enable the clock output channel
 *
 * @param channel_id The clock output channel ID
 * @param enable Enable or disable the clock output channel
 */
static inline __attribute__((always_inline)) void clk_ll_enable_output_channel(clock_out_channel_t channel_id, bool enable)
{
    // No such gating on the target
    (void)channel_id;
    (void)enable;
}

/**
 * @brief Output the mapped clock after frequency division
 *
 * @param channel_id The clock output channel ID
 * @param divider Clock frequency division value
 */
static inline __attribute__((always_inline)) void clk_ll_set_output_channel_divider(clock_out_channel_t channel_id, uint32_t divider)
{
    // No divider on the target
    HAL_ASSERT(divider == 1);
}

/************************* RTC STORAGE REGISTER STORE/LOAD **************************/
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

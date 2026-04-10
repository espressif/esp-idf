/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc.h"
#include "soc/chip_revision.h"
#include "soc/clk_tree_defs.h"
#include "soc/hp_sys_clkrst_reg.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/lp_clkrst_reg.h"
#include "soc/lp_clkrst_struct.h"
#include "soc/hp_alive_sys_reg.h"
#include "soc/hp_alive_sys_struct.h"
#include "soc/pmu_reg.h"
#include "hal/assert.h"
#include "hal/log.h"
#include "esp32s31/rom/rtc.h"
#include "hal/misc.h"

#define MHZ                 (1000000)

#define CLK_LL_PLL_8M_FREQ_MHZ     (8)

#define CLK_LL_PLL_80M_FREQ_MHZ    (80)
#define CLK_LL_PLL_160M_FREQ_MHZ   (160)
#define CLK_LL_PLL_240M_FREQ_MHZ   (240)

#define CLK_LL_PLL_320M_FREQ_MHZ   (320)

#define CLK_LL_PLL_480M_FREQ_MHZ   (480)
#define CLK_LL_PLL_500M_FREQ_MHZ   (500)

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
    .dac = 7, \
    .dres = 7, \
    .dgm = 7, \
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
    CLK_LL_XTAL32K_ENABLE_MODE_BOOTSTRAP,     //!< Bootstrap the crystal oscillator for faster XTAL32K_CLK start up */
} clk_ll_xtal32k_enable_mode_t;

/**
 * @brief XTAL32K_CLK configuration structure
 */
typedef struct {
    uint32_t dac : 3;
    uint32_t dres : 3;
    uint32_t dgm : 3;
    uint32_t dbuf: 1;
} clk_ll_xtal32k_config_t;

/**
 * @brief Power up CPLL circuit
 */
static inline __attribute__((always_inline)) void clk_ll_cpll_enable(void)
{
    SET_PERI_REG_MASK(PMU_HP_ACTIVE_HP_CK_POWER_REG, PMU_HP_ACTIVE_XPD_CPLL);
    SET_PERI_REG_MASK(HP_ALIVE_SYS_HP_CLK_CTRL_REG, HP_ALIVE_SYS_HP_CPLL_300M_CLK_EN);
}

/**
 * @brief Power down CPLL circuit
 */
static inline __attribute__((always_inline)) void clk_ll_cpll_disable(void)
{
    CLEAR_PERI_REG_MASK(HP_ALIVE_SYS_HP_CLK_CTRL_REG, HP_ALIVE_SYS_HP_CPLL_300M_CLK_EN);
    CLEAR_PERI_REG_MASK(PMU_HP_ACTIVE_HP_CK_POWER_REG, PMU_HP_ACTIVE_XPD_CPLL);
}

/**
 * @brief Power up APLL circuit
 */
static inline __attribute__((always_inline)) void clk_ll_apll_enable(void)
{
    SET_PERI_REG_MASK(PMU_HP_ACTIVE_HP_CK_POWER_REG, PMU_HP_ACTIVE_XPD_APLL);
    SET_PERI_REG_MASK(HP_ALIVE_SYS_HP_CLK_CTRL_REG, HP_ALIVE_SYS_HP_AUDIO_PLL_CLK_EN);
}

/**
 * @brief Power down APLL circuit
 */
static inline __attribute__((always_inline)) void clk_ll_apll_disable(void)
{
    CLEAR_PERI_REG_MASK(HP_ALIVE_SYS_HP_CLK_CTRL_REG, HP_ALIVE_SYS_HP_AUDIO_PLL_CLK_EN);
    CLEAR_PERI_REG_MASK(PMU_HP_ACTIVE_HP_CK_POWER_REG, PMU_HP_ACTIVE_XPD_APLL);
}

/**
 * @brief Power up MPLL circuit
 */
static inline __attribute__((always_inline)) void clk_ll_mpll_enable(void)
{
    REG_SET_BIT(PMU_PSRAM_CFG_REG, PMU_PSRAM_XPD);
    SET_PERI_REG_MASK(PMU_HP_ACTIVE_HP_CK_POWER_REG, PMU_HP_ACTIVE_XPD_MPLL);
    SET_PERI_REG_MASK(HP_ALIVE_SYS_HP_CLK_CTRL_REG, HP_ALIVE_SYS_HP_MPLL_500M_CLK_EN);
}

/**
 * @brief Power down MPLL circuit
 */
static inline __attribute__((always_inline)) void clk_ll_mpll_disable(void)
{
    REG_CLR_BIT(PMU_PSRAM_CFG_REG, PMU_PSRAM_XPD);
    CLEAR_PERI_REG_MASK(HP_ALIVE_SYS_HP_CLK_CTRL_REG, HP_ALIVE_SYS_HP_MPLL_500M_CLK_EN);
    CLEAR_PERI_REG_MASK(PMU_HP_ACTIVE_HP_CK_POWER_REG, PMU_HP_ACTIVE_XPD_MPLL);
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
    LP_CLKRST.xtal32k.dac_xtal32k = cfg.dac;
    LP_CLKRST.xtal32k.dres_xtal32k = cfg.dres;
    LP_CLKRST.xtal32k.dgm_xtal32k = cfg.dgm;
    LP_CLKRST.xtal32k.dbuf_xtal32k = cfg.dbuf;
    // Enable xtal32k xpd
    SET_PERI_REG_MASK(PMU_HP_SLEEP_LP_CK_POWER_REG, PMU_HP_SLEEP_XPD_XTAL32K);
    REG_SET_BIT(HP_ALIVE_SYS_HP_CLK_CTRL_REG, HP_ALIVE_SYS_HP_XTAL_32K_CLK_EN);
}

/**
 * @brief Disable the 32kHz crystal oscillator
 */
static inline __attribute__((always_inline)) void clk_ll_xtal32k_disable(void)
{
    REG_CLR_BIT(HP_ALIVE_SYS_HP_CLK_CTRL_REG, HP_ALIVE_SYS_HP_XTAL_32K_CLK_EN);
    // Disable xtal32k xpd
    CLEAR_PERI_REG_MASK(PMU_HP_SLEEP_LP_CK_POWER_REG, PMU_HP_SLEEP_XPD_XTAL32K);
}

/**
 * @brief Get the state of the 32kHz crystal clock
 *
 * @return True if the 32kHz XTAL is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_xtal32k_is_enabled(void)
{
    return REG_GET_FIELD(PMU_HP_SLEEP_LP_CK_POWER_REG, PMU_HP_SLEEP_XPD_XTAL32K) == 1;
}

/**
 * @brief Enable the internal oscillator output for RC32K_CLK
 */
static inline __attribute__((always_inline)) void clk_ll_rc32k_enable(void)
{
    REG_SET_BIT(HP_ALIVE_SYS_HP_CLK_CTRL_REG, HP_ALIVE_SYS_HP_RC_32K_CLK_EN);
    // Enable rc32k xpd status
    SET_PERI_REG_MASK(PMU_HP_SLEEP_LP_CK_POWER_REG, PMU_HP_SLEEP_XPD_RC32K);
}

/**
 * @brief Disable the internal oscillator output for RC32K_CLK
 */
static inline __attribute__((always_inline)) void clk_ll_rc32k_disable(void)
{
    // Disable rc32k xpd status
    CLEAR_PERI_REG_MASK(PMU_HP_SLEEP_LP_CK_POWER_REG, PMU_HP_SLEEP_XPD_RC32K);
    REG_CLR_BIT(HP_ALIVE_SYS_HP_CLK_CTRL_REG, HP_ALIVE_SYS_HP_RC_32K_CLK_EN);
}

/**
 * @brief Get the state of the internal oscillator for RC32K_CLK
 *
 * @return True if the oscillator is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_rc32k_is_enabled(void)
{
    return REG_GET_FIELD(PMU_HP_SLEEP_LP_CK_POWER_REG, PMU_HP_SLEEP_XPD_RC32K) == 1;
}

/**
 * @brief Enable the internal oscillator output for RC_FAST_CLK
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_enable(void)
{
    SET_PERI_REG_MASK(PMU_HP_SLEEP_LP_CK_POWER_REG, PMU_HP_SLEEP_XPD_FOSC_CLK);
}

/**
 * @brief Disable the internal oscillator output for RC_FAST_CLK
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_disable(void)
{
    CLEAR_PERI_REG_MASK(PMU_HP_SLEEP_LP_CK_POWER_REG, PMU_HP_SLEEP_XPD_FOSC_CLK);
}

/**
 * @brief Get the state of the internal oscillator for RC_FAST_CLK
 *
 * @return True if the oscillator is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_rc_fast_is_enabled(void)
{
    return REG_GET_FIELD(PMU_HP_SLEEP_LP_CK_POWER_REG, PMU_HP_SLEEP_XPD_FOSC_CLK) == 1;
}

/**
 * @brief Enable the digital RC_FAST_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_digi_enable(void)
{
    HP_ALIVE_SYS.hp_clk_ctrl.hp_fosc_20m_clk_en = 1;
}

/**
 * @brief Disable the digital RC_FAST_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_digi_disable(void)
{
    HP_ALIVE_SYS.hp_clk_ctrl.hp_fosc_20m_clk_en = 0;
}

/**
 * @brief Get the state of the digital RC_FAST_CLK
 *
 * @return True if the digital RC_FAST_CLK is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_rc_fast_digi_is_enabled(void)
{
    return HP_ALIVE_SYS.hp_clk_ctrl.hp_fosc_20m_clk_en;
}

/**
 * @brief Enable the digital XTAL32K_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_xtal32k_digi_enable(void)
{
    HP_ALIVE_SYS.hp_clk_ctrl.hp_xtal_32k_clk_en = 1;
}

/**
 * @brief Disable the digital XTAL32K_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_xtal32k_digi_disable(void)
{
    HP_ALIVE_SYS.hp_clk_ctrl.hp_xtal_32k_clk_en = 0;
}

/**
 * @brief Get the state of the digital XTAL32K_CLK
 *
 * @return True if the digital XTAL32K_CLK is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_xtal32k_digi_is_enabled(void)
{
    return HP_ALIVE_SYS.hp_clk_ctrl.hp_xtal_32k_clk_en;
}

/**
 * @brief Enable the digital RC32K_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_rc32k_digi_enable(void)
{
    HP_ALIVE_SYS.hp_clk_ctrl.hp_rc_32k_clk_en = 1;
}

/**
 * @brief Disable the digital RC32K_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_rc32k_digi_disable(void)
{
    HP_ALIVE_SYS.hp_clk_ctrl.hp_rc_32k_clk_en = 0;
}

/**
 * @brief Get the state of the digital RC32K_CLK
 *
 * @return True if the digital RC32K_CLK is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_rc32k_digi_is_enabled(void)
{
    return HP_ALIVE_SYS.hp_clk_ctrl.hp_rc_32k_clk_en;
}

/**
 * @brief Get XTAL_CLK frequency
 *
 * @return Main XTAL clock frequency, in MHz.
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_xtal_get_freq_mhz(void)
{
    return LP_CLKRST.hp_clk_ctrl.clk_xtal_freq;
}

/**
 * @brief Get CPLL_CLK frequency (only reliable when CPLL power is on)
 *
 * @param xtal_freq_mhz XTAL frequency, in MHz
 *
 * @return CPLL clock frequency, in MHz
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_cpll_get_freq_mhz(uint32_t xtal_freq_mhz)
{
    uint8_t fb_div = HAL_FORCE_READ_U32_REG_FIELD(LP_CLKRST.cpll_div, cpll_fb_div);
    uint8_t ref_div = LP_CLKRST.cpll_div.cpll_ref_div;
    return xtal_freq_mhz * fb_div / ref_div;
}

/**
 * @brief Set CPLL frequency from XTAL source (Digital part)
 *
 * @param cpll_freq_mhz CPLL frequency, in MHz
 */
static inline __attribute__((always_inline)) void clk_ll_cpll_set_freq_mhz(uint32_t cpll_freq_mhz, uint32_t xtal_freq_mhz)
{
    HAL_ASSERT(xtal_freq_mhz == 40);
    // cpll_freq = freq_xtal * fb_div / ref_div
    uint8_t fb_div = 8;
    uint8_t ref_div = 1;

    switch (cpll_freq_mhz) {
    case CLK_LL_PLL_320M_FREQ_MHZ:
    default:
        /* Configure 320M CPLL */
        fb_div = 8;
        ref_div = 1;
        break;
    }

    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_CLKRST.cpll_div, cpll_fb_div, fb_div);
    LP_CLKRST.cpll_div.cpll_ref_div = ref_div;
}

/**
 * @brief Set CPLL frequency from XTAL source (Analog part - through regi2c)
 *
 * @param cpll_freq_mhz CPLL frequency, in MHz
 * @param xtal_freq_mhz XTAL frequency, in MHz
 */
static inline __attribute__((always_inline)) void clk_ll_cpll_set_config(uint32_t cpll_freq_mhz, uint32_t xtal_freq_mhz)
{
    // Do nothing. CPLL frequency controlled by digital only on the target.
    HAL_ASSERT(xtal_freq_mhz == 40);
    HAL_ASSERT(cpll_freq_mhz == CLK_LL_PLL_320M_FREQ_MHZ);
}

/**
 * @brief Start CPLL self-calibration
 */
static inline __attribute__((always_inline)) void clk_ll_cpll_calibration_start(void)
{
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_ANA_PLL_CTRL0_REG, HP_SYS_CLKRST_REG_CPU_PLL_CAL_STOP);
}

/**
 * @brief Stop CPLL self-calibration
 */
static inline __attribute__((always_inline)) void clk_ll_cpll_calibration_stop(void)
{
    SET_PERI_REG_MASK(HP_SYS_CLKRST_ANA_PLL_CTRL0_REG, HP_SYS_CLKRST_REG_CPU_PLL_CAL_STOP);
}

/**
 * @brief Check whether CPLL calibration is done
 *
 * @return True if calibration is done; otherwise false
 */
static inline __attribute__((always_inline)) bool clk_ll_cpll_calibration_is_done(void)
{
    return REG_GET_BIT(HP_SYS_CLKRST_ANA_PLL_CTRL0_REG, HP_SYS_CLKRST_REG_CPU_PLL_CAL_END);
}

/**
 * @brief Get MPLL_CLK frequency (only reliable when MPLL power is on)
 *
 * @param xtal_freq_mhz XTAL frequency, in MHz
 *
 * @return MPLL clock frequency, in MHz
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_mpll_get_freq_mhz(uint32_t xtal_freq_mhz)
{
    uint8_t fb_div = REG_GET_FIELD(LP_AONCLKRST_MSPI_DIV_REG, LP_AONCLKRST_MSPI_FB_DIV);
    return xtal_freq_mhz * (fb_div + 1) / 2;
}

/**
 * @brief Set MPLL frequency from XTAL source
 *
 * @param mpll_freq_mhz MPLL frequency, in MHz
 * @param xtal_freq_mhz XTAL frequency, in MHz
 */
static inline __attribute__((always_inline)) void clk_ll_mpll_set_config(uint32_t mpll_freq_mhz, uint32_t xtal_freq_mhz)
{
    HAL_ASSERT(xtal_freq_mhz == SOC_XTAL_FREQ_40M);

    uint8_t ref_div = 1;
    uint8_t fb_div = mpll_freq_mhz * (ref_div + 1) / xtal_freq_mhz - 1;
    REG_SET_FIELD(LP_AONCLKRST_MSPI_DIV_REG, LP_AONCLKRST_MSPI_FB_DIV, fb_div);
}

/**
 * @brief Start MPLL self-calibration
 */
static inline __attribute__((always_inline)) void clk_ll_mpll_calibration_start(void)
{
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_ANA_PLL_CTRL0_REG, HP_SYS_CLKRST_REG_MSPI_CAL_STOP);
}

/**
 * @brief Stop MPLL self-calibration
 */
static inline __attribute__((always_inline)) void clk_ll_mpll_calibration_stop(void)
{
    SET_PERI_REG_MASK(HP_SYS_CLKRST_ANA_PLL_CTRL0_REG, HP_SYS_CLKRST_REG_MSPI_CAL_STOP);
}

/**
 * @brief Check whether MPLL calibration is done
 *
 * @return True if calibration is done; otherwise false
 */
static inline __attribute__((always_inline)) bool clk_ll_mpll_calibration_is_done(void)
{
    return REG_GET_BIT(HP_SYS_CLKRST_ANA_PLL_CTRL0_REG, HP_SYS_CLKRST_REG_MSPI_CAL_END);
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
    // TODO: IDF-14771, IDF-14750
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
    // TODO: IDF-14771, IDF-14750
}

/**
 * @brief Set APLL calibration parameters
 */
static inline __attribute__((always_inline)) void clk_ll_apll_set_calibration(void)
{
    // TODO: IDF-14771, IDF-14750
}

/**
 * @brief Check whether APLL calibration is done
 *
 * @return True if calibration is done; otherwise false
 */
static inline __attribute__((always_inline)) bool clk_ll_apll_calibration_is_done(void)
{
    // TODO: IDF-14771, IDF-14750
    return 0;
}

/**
 * @brief To enable the change of soc_clk_sel, cpu_div_num, mem_div_num, sys_div_num, and apb_div_num
 */
static inline __attribute__((always_inline)) void clk_ll_bus_update(void)
{
    HP_SYS_CLKRST.root_clk_ctrl0.reg_soc_clk_update = 1;
    while (HP_SYS_CLKRST.root_clk_ctrl0.reg_soc_clk_update);
}

/**
 * @brief Select the clock source for CPU_CLK (SOC Clock Root)
 *
 * @param in_sel One of the clock sources in soc_cpu_clk_src_t
 */
static inline __attribute__((always_inline)) void clk_ll_cpu_set_src(soc_cpu_clk_src_t in_sel)
{
    switch (in_sel) {
    case SOC_CPU_CLK_SRC_XTAL:
        HP_SYS_CLKRST.soc_clk_sel.reg_soc_clk_sel = 0;
        break;
    case SOC_CPU_CLK_SRC_CPLL:
        HP_SYS_CLKRST.soc_clk_sel.reg_soc_clk_sel = 1;
        break;
    case SOC_CPU_CLK_SRC_RC_FAST:
        HP_SYS_CLKRST.soc_clk_sel.reg_soc_clk_sel = 2;
        break;
    case SOC_CPU_CLK_SRC_PLL_F240M:
        HP_SYS_CLKRST.soc_clk_sel.reg_soc_clk_sel = 3;
        break;
    default:
        // Unsupported CPU_CLK mux input sel
        abort();
    }
}

/**
 * @brief Get the clock source for CPU_CLK (SOC Clock Root)
 *
 * @return Currently selected clock source (one of soc_cpu_clk_src_t values)
 */
static inline __attribute__((always_inline)) soc_cpu_clk_src_t clk_ll_cpu_get_src(void)
{
    uint32_t clk_sel = HP_SYS_CLKRST.soc_clk_sel.reg_soc_clk_sel;
    switch (clk_sel) {
    case 0:
        return SOC_CPU_CLK_SRC_XTAL;
    case 1:
        return SOC_CPU_CLK_SRC_CPLL;
    case 2:
        return SOC_CPU_CLK_SRC_RC_FAST;
    case 3:
        return SOC_CPU_CLK_SRC_PLL_F240M;
    default:
        // Invalid HP_ROOT_CLK_SRC_SEL value
        return SOC_CPU_CLK_SRC_INVALID;
    }
}

/**
 * @brief Set CPU_CLK divider. freq of CPU_CLK = freq of HP_ROOT_CLK / divider
 *
 * @param integer Integer part of the divider
 * @param numerator Numerator part of the divider
 * @param denominator Denominator part of the divider
 */
static inline __attribute__((always_inline)) void clk_ll_cpu_set_divider(uint32_t integer, uint32_t numerator, uint32_t denominator)
{
    HAL_ASSERT(integer >= 1 && integer <= HP_SYS_CLKRST_REG_CPU_CLK_DIV_NUM_V);
    HAL_ASSERT(numerator <= HP_SYS_CLKRST_REG_CPU_CLK_DIV_NUMERATOR_V);
    HAL_ASSERT(denominator <= HP_SYS_CLKRST_REG_CPU_CLK_DIV_DENOMINATOR_V);
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.cpu_freq_ctrl0, reg_cpu_clk_div_num, integer - 1);
    HP_SYS_CLKRST.cpu_freq_ctrl0.reg_cpu_clk_div_numerator = numerator;
    HP_SYS_CLKRST.cpu_freq_ctrl0.reg_cpu_clk_div_denominator = denominator;
}

/**
 * @brief Get CPU_CLK divider
 *
 * @param integer Integer part of the divider
 * @param numerator Numerator part of the divider
 * @param denominator Denominator part of the divider
 */
static inline __attribute__((always_inline)) void clk_ll_cpu_get_divider(uint32_t *integer, uint32_t *numerator, uint32_t *denominator)
{
    *integer = HAL_FORCE_READ_U32_REG_FIELD(HP_SYS_CLKRST.cpu_freq_ctrl0, reg_cpu_clk_div_num) + 1;
    *numerator = HP_SYS_CLKRST.cpu_freq_ctrl0.reg_cpu_clk_div_numerator;
    *denominator = HP_SYS_CLKRST.cpu_freq_ctrl0.reg_cpu_clk_div_denominator;
}

/**
 * @brief Set MEM_CLK (ROM_CLK) divider. freq of MEM_CLK = freq of CPU_CLK / divider
 *
 * @note There is constraint on the mem divider. Hardware could change the actual divider if the configured value is
 *       unachievable. Be careful on this. Check TRM or upper layer.
 *
 * @param divider Divider. CLK_DIV_NUM = divider - 1.
 */
static inline __attribute__((always_inline)) void clk_ll_mem_set_divider(uint32_t divider)
{
    HAL_ASSERT(divider == 1 || divider == 2); // 1-bit field
    HP_SYS_CLKRST.mem_freq_ctrl0.reg_mem_clk_div_num = divider - 1;
}

/**
 * @brief Get MEM_CLK (ROM_CLK) divider
 *
 * @return Divider. Divider = (CLK_DIV_NUM + 1).
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_mem_get_divider(void)
{
    return HP_SYS_CLKRST.mem_freq_ctrl0.reg_mem_clk_div_num + 1;
}

/**
 * @brief Set SYS_CLK divider. freq of SYS_CLK = freq of CPU_CLK / divider
 *
 * ESP32S31 SYS_CLK supports fractional divnum (not supported in software yet)
 *
 * @param divider Divider. CLK_DIV_NUM = divider - 1.
 */
static inline __attribute__((always_inline)) void clk_ll_sys_set_divider(uint32_t divider)
{
    HAL_ASSERT(divider >= 1);
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.sys_freq_ctrl0, reg_sys_clk_div_num, divider - 1);
}

/**
 * @brief Get SYS_CLK divider
 *
 * Fractional divnum not used now.
 *
 * @return Divider. Divider = (CLK_DIV_NUM + 1).
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_sys_get_divider(void)
{
    return HAL_FORCE_READ_U32_REG_FIELD(HP_SYS_CLKRST.sys_freq_ctrl0, reg_sys_clk_div_num) + 1;
}

/**
 * @brief Set APB_CLK divider. freq of APB_CLK = freq of SYS_CLK / divider
 *
 * ESP32S31 APB_CLK supports fractional divnum (not supported in software yet)
 *
 * @note There is constraint on the apb divider. Hardware could change the actual divider if the configured value is
 *       unachievable. Be careful on this. Check TRM or upper layer.
 *
 * @param divider Divider. CLK_DIV_NUM = divider - 1.
 */
static inline __attribute__((always_inline)) void clk_ll_apb_set_divider(uint32_t divider)
{
    HAL_ASSERT(divider >= 1);
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.apb_freq_ctrl0, reg_apb_clk_div_num, divider - 1);
}

/**
 * @brief Get APB_CLK divider
 *
 * Fractional divnum not used now.
 *
 * @return Divider. Divider = (CLK_DIV_NUM + 1).
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_apb_get_divider(void)
{
    return HAL_FORCE_READ_U32_REG_FIELD(HP_SYS_CLKRST.apb_freq_ctrl0, reg_apb_clk_div_num) + 1;
}

// TODO: IDF-14730
// static inline __attribute__((always_inline)) void clk_ll_ref_500m_set_src(soc_ref_500m_clk_src_t in_sel)
// {
//     // 0: cpll (320MHz)
//     // 1: mpll (500MHz)
//     HP_SYS_CLKRST.ref_500m_ctrl0.reg_ref_500m_sel = in_sel;
// }

/**
 * @brief Set PLL_F50M_CLK divider. freq of PLL_F50M_CLK = freq of MPLL_CLK / divider
 *
 * @param divider Divider. CLK_DIV_NUM = divider - 1.
 */
static inline __attribute__((always_inline)) void clk_ll_pll_f50m_set_divider(uint32_t divider)
{
    HAL_ASSERT(divider >= 1);
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.ref_50m_ctrl0, reg_ref_50m_clk_div_num, divider - 1);
}

/**
 * @brief Set PLL_F25M_CLK divider. freq of PLL_F25M_CLK = freq of MPLL_CLK / divider
 *
 * @param divider Divider. CLK_DIV_NUM = divider - 1.
 */
static inline __attribute__((always_inline)) void clk_ll_pll_f25m_set_divider(uint32_t divider)
{
    HAL_ASSERT(divider >= 1);
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.ref_25m_ctrl0, reg_ref_25m_clk_div_num, divider - 1);
}

/**
 * @brief Set PLL_F20M_CLK divider. freq of PLL_F20M_CLK = freq of SPLL_CLK / divider
 *
 * @param divider Divider. CLK_DIV_NUM = divider - 1.
 */
static inline __attribute__((always_inline)) void clk_ll_pll_f20m_set_divider(uint32_t divider)
{
    HAL_ASSERT(divider >= 1);
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.ref_20m_ctrl0, reg_ref_20m_clk_div_num, divider - 1);
}

/**
 * @brief Get PLL_F20M_CLK divider
 *
 * @return Divider. Divider = (CLK_DIV_NUM + 1).
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_pll_f20m_get_divider(void)
{
    return HAL_FORCE_READ_U32_REG_FIELD(HP_SYS_CLKRST.ref_20m_ctrl0, reg_ref_20m_clk_div_num) + 1;
}

/**
 * @brief Select the frequency calculation clock source for timergroup0
 *
 * @param clk_sel One of the clock sources in soc_clk_freq_calculation_src_t
 */
static inline __attribute__((always_inline)) void clk_ll_freq_calulation_set_target(soc_clk_freq_calculation_src_t clk_sel)
{
    int timg_cali_clk_sel = -1;

    switch (clk_sel) {
    case CLK_CAL_MPLL:
        timg_cali_clk_sel = 0;
        break;
    case CLK_CAL_BBPLL:
        timg_cali_clk_sel = 1;
        break;
    case CLK_CAL_CPLL:
        timg_cali_clk_sel = 2;
        break;
    case CLK_CAL_APLL:
        timg_cali_clk_sel = 3;
        break;
    case CLK_CAL_AHB:
        timg_cali_clk_sel = 4;
        break;
    case CLK_CAL_APB:
        timg_cali_clk_sel = 5;
        break;
    case CLK_CAL_ROM:
        timg_cali_clk_sel = 6;
        break;
    case CLK_CAL_RC_FAST:
        timg_cali_clk_sel = 7;
        break;
    case CLK_CAL_RC_SLOW:
        timg_cali_clk_sel = 8;
        break;
    case CLK_CAL_32K_XTAL:
        timg_cali_clk_sel = 10;
        break;
    case CLK_CAL_XTAL:
        timg_cali_clk_sel = 12;
        break;
    case CLK_CAL_UART0_PAD:
        timg_cali_clk_sel = 13;
        break;
    case CLK_CAL_CORE0:
        timg_cali_clk_sel = 14;
        break;
    case CLK_CAL_CORE1:
        timg_cali_clk_sel = 15;
        break;
    default:
        // Unsupported CLK_CAL mux input
        abort();
    }

    if (timg_cali_clk_sel >= 0) {
        HP_SYS_CLKRST.timergrp0_tgrt_ctrl0.reg_timergrp0_tgrt_clk_src_sel = timg_cali_clk_sel;
    }
}

/**
 * @brief Set a divider for the clock to be frequency calculated by timergroup0
 *
 * @param divider Divider. PRE_DIV_CNT = divider - 1.
 */
static inline __attribute__((always_inline)) void clk_ll_freq_calculation_set_divider(uint32_t divider)
{
    HAL_ASSERT(divider >= 1);
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.timergrp0_tgrt_ctrl0, reg_timergrp0_tgrt_clk_div_num, divider - 1);
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
        LP_CLKRST.root_clk_conf.slow_clk_sel = 0;
        break;
    case SOC_RTC_SLOW_CLK_SRC_XTAL32K:
        LP_CLKRST.root_clk_conf.slow_clk_sel = 1;
        break;
    // LP_CLKRST.root_clk_conf.slow_clk_sel = 3 is for SOC_RTC_SLOW_CLK_SRC_OSC_SLOW (a 32kHz clock signal generated
    // by an external circuit connecting to XTAL_32K_N (i.e. GPIO0)), but we don't use it on ESP32S31, since osc_slow
    // clock can not be calibrated to get its actual frequency
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
    uint32_t clk_sel = LP_CLKRST.root_clk_conf.slow_clk_sel;
    switch (clk_sel) {
    case 0:
        return SOC_RTC_SLOW_CLK_SRC_RC_SLOW;
    case 1:
        return SOC_RTC_SLOW_CLK_SRC_XTAL32K;
    default:
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
    case SOC_RTC_FAST_CLK_SRC_RC_FAST:
        LP_CLKRST.root_clk_conf.fast_clk_sel = 0;
        break;
    case SOC_RTC_FAST_CLK_SRC_XTAL:
        LP_CLKRST.root_clk_conf.fast_clk_sel = 1;
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
    uint32_t clk_sel = LP_CLKRST.root_clk_conf.fast_clk_sel;
    switch (clk_sel) {
    case 0:
        return SOC_RTC_FAST_CLK_SRC_RC_FAST;
    case 1:
        return SOC_RTC_FAST_CLK_SRC_XTAL;
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
    // No divider on the target
    HAL_ASSERT(divider == 1);
}

/**
 * @brief Get RC_FAST_CLK divider
 *
 * @return Divider
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_rc_fast_get_divider(void)
{
    // No divider on the target, always return divider = 1
    return 1;
}

/**
 * @brief Set RC_SLOW_CLK divider
 *
 * @param divider Divider of RC_SLOW_CLK. Usually this divider is set to 1 (reg. value is 0) in bootloader stage.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_slow_set_divider(uint32_t divider)
{
    // No divider on the target
    HAL_ASSERT(divider == 1);
}

/************************** LP STORAGE REGISTER STORE/LOAD **************************/
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

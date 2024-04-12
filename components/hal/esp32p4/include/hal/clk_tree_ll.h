/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/clkout_channel.h"
#include "soc/soc.h"
#include "soc/clk_tree_defs.h"
#include "soc/hp_sys_clkrst_reg.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/lp_clkrst_struct.h"
#include "soc/pmu_reg.h"
#include "hal/regi2c_ctrl.h"
#include "soc/regi2c_cpll.h"
#include "soc/regi2c_mpll.h"
#include "soc/regi2c_bias.h"
#include "hal/assert.h"
#include "hal/log.h"
#include "esp32p4/rom/rtc.h"
#include "hal/misc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MHZ                 (1000000)

#define CLK_LL_PLL_8M_FREQ_MHZ     (8)

#define CLK_LL_PLL_20M_FREQ_MHZ    (20)
#define CLK_LL_PLL_80M_FREQ_MHZ    (80)
#define CLK_LL_PLL_160M_FREQ_MHZ   (160)
#define CLK_LL_PLL_240M_FREQ_MHZ   (240)

#define CLK_LL_PLL_360M_FREQ_MHZ   (360)
#define CLK_LL_PLL_400M_FREQ_MHZ   (400)

#define CLK_LL_PLL_480M_FREQ_MHZ   (480)
#define CLK_LL_PLL_500M_FREQ_MHZ   (500)

/* APLL multiplier output frequency range */
// TODO: IDF-8884 check if the APLL frequency range is same as before
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
    uint32_t dac : 6;
    uint32_t dres : 3;
    uint32_t dgm : 3;
    uint32_t dbuf: 1;
} clk_ll_xtal32k_config_t;


/**
 * @brief Power up CPLL circuit
 */
static inline __attribute__((always_inline)) void clk_ll_cpll_enable(void)
{
    SET_PERI_REG_MASK(PMU_IMM_HP_CK_POWER_REG, PMU_TIE_HIGH_XPD_CPLL | PMU_TIE_HIGH_XPD_CPLL_I2C);
    SET_PERI_REG_MASK(PMU_IMM_HP_CK_POWER_REG, PMU_TIE_HIGH_GLOBAL_CPLL_ICG);
}

/**
 * @brief Power down CPLL circuit
 */
static inline __attribute__((always_inline)) void clk_ll_cpll_disable(void)
{
    SET_PERI_REG_MASK(PMU_IMM_HP_CK_POWER_REG, PMU_TIE_LOW_GLOBAL_CPLL_ICG) ;
    SET_PERI_REG_MASK(PMU_IMM_HP_CK_POWER_REG, PMU_TIE_LOW_XPD_CPLL | PMU_TIE_LOW_XPD_CPLL_I2C);
}

/**
 * @brief Enable the internal oscillator output for LP_PLL_CLK
 */
static inline __attribute__((always_inline)) void clk_ll_lp_pll_enable(void)
{
    // Enable lp_pll xpd status
    SET_PERI_REG_MASK(PMU_HP_SLEEP_LP_CK_POWER_REG, PMU_HP_SLEEP_XPD_LPPLL);
}

/**
 * @brief Disable the internal oscillator output for LP_PLL_CLK
 */
static inline __attribute__((always_inline)) void clk_ll_lp_pll_disable(void)
{
    // Disable lp_pll xpd status
    CLEAR_PERI_REG_MASK(PMU_HP_SLEEP_LP_CK_POWER_REG, PMU_HP_SLEEP_XPD_LPPLL);
}

/**
 * @brief Power up MPLL circuit
 */
static inline __attribute__((always_inline)) void clk_ll_mpll_enable(void)
{
    REG_SET_BIT(PMU_RF_PWC_REG, PMU_MSPI_PHY_XPD);
}

/**
 * @brief Power down MPLL circuit
 */
static inline __attribute__((always_inline)) void clk_ll_mpll_disable(void)
{
    REG_CLR_BIT(PMU_RF_PWC_REG, PMU_MSPI_PHY_XPD);
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
    LP_AON_CLKRST.xtal32k.dac_xtal32k = cfg.dac;
    LP_AON_CLKRST.xtal32k.dres_xtal32k = cfg.dres;
    LP_AON_CLKRST.xtal32k.dgm_xtal32k = cfg.dgm;
    LP_AON_CLKRST.xtal32k.dbuf_xtal32k = cfg.dbuf;
    // Enable xtal32k xpd
    SET_PERI_REG_MASK(PMU_HP_SLEEP_LP_CK_POWER_REG, PMU_HP_SLEEP_XPD_XTAL32K);
}

/**
 * @brief Disable the 32kHz crystal oscillator
 */
static inline __attribute__((always_inline)) void clk_ll_xtal32k_disable(void)
{
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
    LP_AON_CLKRST.clk_to_hp.icg_hp_fosc = 1;
}

/**
 * @brief Disable the digital RC_FAST_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_rc_fast_digi_disable(void)
{
    LP_AON_CLKRST.clk_to_hp.icg_hp_fosc = 0;
}

/**
 * @brief Get the state of the digital RC_FAST_CLK
 *
 * @return True if the digital RC_FAST_CLK is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_rc_fast_digi_is_enabled(void)
{
    return LP_AON_CLKRST.clk_to_hp.icg_hp_fosc;
}

/**
 * @brief Enable the digital XTAL32K_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_xtal32k_digi_enable(void)
{
    LP_AON_CLKRST.clk_to_hp.icg_hp_xtal32k = 1;
}

/**
 * @brief Disable the digital XTAL32K_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_xtal32k_digi_disable(void)
{
    LP_AON_CLKRST.clk_to_hp.icg_hp_xtal32k = 0;
}

/**
 * @brief Get the state of the digital XTAL32K_CLK
 *
 * @return True if the digital XTAL32K_CLK is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_xtal32k_digi_is_enabled(void)
{
    return LP_AON_CLKRST.clk_to_hp.icg_hp_xtal32k;
}

/**
 * @brief Enable the digital RC32K_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_rc32k_digi_enable(void)
{
    LP_AON_CLKRST.clk_to_hp.icg_hp_osc32k = 1;
}

/**
 * @brief Disable the digital RC32K_CLK, which is used to support peripherals.
 */
static inline __attribute__((always_inline)) void clk_ll_rc32k_digi_disable(void)
{
    LP_AON_CLKRST.clk_to_hp.icg_hp_osc32k = 0;
}

/**
 * @brief Get the state of the digital RC32K_CLK
 *
 * @return True if the digital RC32K_CLK is enabled
 */
static inline __attribute__((always_inline)) bool clk_ll_rc32k_digi_is_enabled(void)
{
    return LP_AON_CLKRST.clk_to_hp.icg_hp_osc32k;
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
    uint8_t div = REGI2C_READ_MASK(I2C_CPLL, I2C_CPLL_OC_DIV_7_0);
    uint8_t ref_div = REGI2C_READ_MASK(I2C_CPLL, I2C_CPLL_OC_REF_DIV);
    return xtal_freq_mhz * (div + 4) / (ref_div + 1);
}

/**
 * @brief Set CPLL frequency from XTAL source (Digital part)
 *
 * @param cpll_freq_mhz CPLL frequency, in MHz
 */
static inline __attribute__((always_inline)) void clk_ll_cpll_set_freq_mhz(uint32_t cpll_freq_mhz)
{
    // Do nothing. CPLL frequency controlled by analog only on the target.
    (void)cpll_freq_mhz;
}

/**
 * @brief Set CPLL frequency from XTAL source (Analog part - through regi2c)
 *
 * @param cpll_freq_mhz CPLL frequency, in MHz
 * @param xtal_freq_mhz XTAL frequency, in MHz
 */
static inline __attribute__((always_inline)) void clk_ll_cpll_set_config(uint32_t cpll_freq_mhz, uint32_t xtal_freq_mhz)
{
    uint8_t div_ref;
    uint8_t div7_0;
    uint8_t dchgp = 5;
    uint8_t dcur = 3;
    uint8_t oc_enb_fcal = 0;

    // Currently, only supporting 40MHz XTAL
    HAL_ASSERT(xtal_freq_mhz == SOC_XTAL_FREQ_40M);
    switch (cpll_freq_mhz) {
    case CLK_LL_PLL_400M_FREQ_MHZ:
        /* Configure 400M CPLL */
        div7_0 = 6;
        div_ref = 0;
        break;
    case CLK_LL_PLL_360M_FREQ_MHZ:
    default:
        /* Configure 360M CPLL */
        div7_0 = 5;
        div_ref = 0;
        break;
    }
    uint8_t i2c_cpll_lref  = (oc_enb_fcal << I2C_CPLL_OC_ENB_FCAL_LSB) | (dchgp << I2C_CPLL_OC_DCHGP_LSB) | (div_ref);
    uint8_t i2c_cpll_div_7_0 = div7_0;
    uint8_t i2c_cpll_dcur = (1 << I2C_CPLL_OC_DLREF_SEL_LSB ) | (3 << I2C_CPLL_OC_DHREF_SEL_LSB) | dcur;
    REGI2C_WRITE(I2C_CPLL, I2C_CPLL_OC_REF_DIV, i2c_cpll_lref);
    REGI2C_WRITE(I2C_CPLL, I2C_CPLL_OC_DIV_7_0, i2c_cpll_div_7_0);
    REGI2C_WRITE(I2C_CPLL, I2C_CPLL_OC_DCUR, i2c_cpll_dcur);
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
    uint8_t div = REGI2C_READ_MASK(I2C_MPLL, I2C_MPLL_DIV_ADDR);
    uint8_t ref_div = REGI2C_READ_MASK(I2C_MPLL, I2C_MPLL_REF_DIV_ADDR);
    return xtal_freq_mhz * (div + 1) / (ref_div + 1);
}

/**
 * @brief Set MPLL frequency from XTAL source (Analog part - through regi2c)
 *
 * @param mpll_freq_mhz MPLL frequency, in MHz
 * @param xtal_freq_mhz XTAL frequency, in MHz
 */
static inline __attribute__((always_inline)) void clk_ll_mpll_set_config(uint32_t mpll_freq_mhz, uint32_t xtal_freq_mhz)
{
    HAL_ASSERT(xtal_freq_mhz == SOC_XTAL_FREQ_40M);

    uint8_t mpll_dhref_val = REGI2C_READ(I2C_MPLL, I2C_MPLL_DHREF);
    REGI2C_WRITE(I2C_MPLL, I2C_MPLL_DHREF,  mpll_dhref_val | (3 << I2C_MPLL_DHREF_LSB));
    uint8_t mpll_rstb_val = REGI2C_READ(I2C_MPLL, I2C_MPLL_IR_CAL_RSTB);
    REGI2C_WRITE(I2C_MPLL, I2C_MPLL_IR_CAL_RSTB, mpll_rstb_val & 0xdf);
    REGI2C_WRITE(I2C_MPLL, I2C_MPLL_IR_CAL_RSTB, mpll_rstb_val | (1 << I2C_MPLL_IR_CAL_RSTB_lSB));

    // MPLL_Freq = XTAL_Freq * (div + 1) / (ref_div + 1)
    uint8_t ref_div = 1;
    uint8_t div = mpll_freq_mhz / 20 - 1;
    uint8_t val = ((div << 3) | ref_div);
    REGI2C_WRITE(I2C_MPLL, I2C_MPLL_DIV_REG_ADDR, val);
}

/**
 * @brief To enable the change of cpu_div_num, mem_div_num, sys_div_num, and apb_div_num
 */
static inline __attribute__((always_inline)) void clk_ll_bus_update(void)
{
    HP_SYS_CLKRST.root_clk_ctrl0.reg_soc_clk_div_update = 1;
    while (HP_SYS_CLKRST.root_clk_ctrl0.reg_soc_clk_div_update);
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
        LP_AON_CLKRST.hp_clk_ctrl.hp_root_clk_src_sel = 0;
        break;
    case SOC_CPU_CLK_SRC_CPLL:
        LP_AON_CLKRST.hp_clk_ctrl.hp_root_clk_src_sel = 1;
        break;
    case SOC_CPU_CLK_SRC_RC_FAST:
        LP_AON_CLKRST.hp_clk_ctrl.hp_root_clk_src_sel = 2;
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
    uint32_t clk_sel = LP_AON_CLKRST.hp_clk_ctrl.hp_root_clk_src_sel;
    switch (clk_sel) {
    case 0:
        return SOC_CPU_CLK_SRC_XTAL;
    case 1:
        return SOC_CPU_CLK_SRC_CPLL;
    case 2:
        return SOC_CPU_CLK_SRC_RC_FAST;
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
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.root_clk_ctrl0, reg_cpu_clk_div_num, integer - 1);
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.root_clk_ctrl0, reg_cpu_clk_div_numerator, numerator);
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.root_clk_ctrl0, reg_cpu_clk_div_denominator, denominator);
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
    *integer = HAL_FORCE_READ_U32_REG_FIELD(HP_SYS_CLKRST.root_clk_ctrl0, reg_cpu_clk_div_num) + 1;
    *numerator = HAL_FORCE_READ_U32_REG_FIELD(HP_SYS_CLKRST.root_clk_ctrl0, reg_cpu_clk_div_numerator);
    *denominator = HAL_FORCE_READ_U32_REG_FIELD(HP_SYS_CLKRST.root_clk_ctrl0, reg_cpu_clk_div_denominator);
}

/**
 * @brief Set MEM_CLK divider. freq of MEM_CLK = freq of CPU_CLK / divider
 *
 * ESP32P4 MEM_CLK supports fractional divnum (not supported in software yet)
 *
 * @note There is constraint on the mem divider. Hardware could change the actual divider if the configured value is
 *       unachievable. Be careful on this. Check TRM or upper layer.
 *
 * @param divider Divider. CLK_DIV_NUM = divider - 1.
 */
static inline __attribute__((always_inline)) void clk_ll_mem_set_divider(uint32_t divider)
{
    HAL_ASSERT(divider >= 1 && divider <= 2); // We haven't confirmed the reliable functionality of cache when cpu_clk freq is more than 2 times faster than the cache clk freq. Need to verify before removing the constraint of divider <= 2.
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.root_clk_ctrl1, reg_mem_clk_div_num, divider - 1);
}

/**
 * @brief Get MEM_CLK divider
 *
 * Fractional divnum not used now.
 *
 * @return Divider. Divider = (CLK_DIV_NUM + 1).
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_mem_get_divider(void)
{
    return HAL_FORCE_READ_U32_REG_FIELD(HP_SYS_CLKRST.root_clk_ctrl1, reg_mem_clk_div_num) + 1;
}

/**
 * @brief Set SYS_CLK divider. freq of SYS_CLK = freq of MEM_CLK / divider
 *
 * ESP32P4 SYS_CLK supports fractional divnum (not supported in software yet)
 *
 * @param divider Divider. CLK_DIV_NUM = divider - 1.
 */
static inline __attribute__((always_inline)) void clk_ll_sys_set_divider(uint32_t divider)
{
    HAL_ASSERT(divider >= 1);
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.root_clk_ctrl1, reg_sys_clk_div_num, divider - 1);
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
    return HAL_FORCE_READ_U32_REG_FIELD(HP_SYS_CLKRST.root_clk_ctrl1, reg_sys_clk_div_num) + 1;
}

/**
 * @brief Set APB_CLK divider. freq of APB_CLK = freq of SYS_CLK / divider
 *
 * ESP32P4 APB_CLK supports fractional divnum (not supported in software yet)
 *
 * @note There is constraint on the apb divider. Hardware could change the actual divider if the configured value is
 *       unachievable. Be careful on this. Check TRM or upper layer.
 *
 * @param divider Divider. CLK_DIV_NUM = divider - 1.
 */
static inline __attribute__((always_inline)) void clk_ll_apb_set_divider(uint32_t divider)
{
    HAL_ASSERT(divider >= 1);
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.root_clk_ctrl2, reg_apb_clk_div_num, divider - 1);
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
    return HAL_FORCE_READ_U32_REG_FIELD(HP_SYS_CLKRST.root_clk_ctrl2, reg_apb_clk_div_num) + 1;
}

/**
 * @brief Set PLL_F50M_CLK divider. freq of PLL_F50M_CLK = freq of MPLL_CLK / divider
 *
 * @param divider Divider. CLK_DIV_NUM = divider - 1.
 */
static inline __attribute__((always_inline)) void clk_ll_pll_f50m_set_divider(uint32_t divider)
{
    HAL_ASSERT(divider >= 1);
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.ref_clk_ctrl0, reg_ref_50m_clk_div_num, divider - 1);
}

/**
 * @brief Set PLL_F25M_CLK divider. freq of PLL_F25M_CLK = freq of MPLL_CLK / divider
 *
 * @param divider Divider. CLK_DIV_NUM = divider - 1.
 */
static inline __attribute__((always_inline)) void clk_ll_pll_f25m_set_divider(uint32_t divider)
{
    HAL_ASSERT(divider >= 1);
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.ref_clk_ctrl0, reg_ref_25m_clk_div_num, divider - 1);
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
        LP_AON_CLKRST.lp_clk_conf.slow_clk_sel = 0;
        break;
    case SOC_RTC_SLOW_CLK_SRC_XTAL32K:
        LP_AON_CLKRST.lp_clk_conf.slow_clk_sel = 1;
        break;
    case SOC_RTC_SLOW_CLK_SRC_RC32K:
        LP_AON_CLKRST.lp_clk_conf.slow_clk_sel = 2;
        break;
    // LP_AON_CLKRST.lp_clk_conf.slow_clk_sel = 3 is for SOC_RTC_SLOW_CLK_SRC_OSC_SLOW (a 32kHz clock signal generated
    // by an external circuit connecting to XTAL_32K_N (i.e. GPIO0)), but we don't use it on ESP32P4, since osc_slow
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
    uint32_t clk_sel = LP_AON_CLKRST.lp_clk_conf.slow_clk_sel;
    switch (clk_sel) {
    case 0:
        return SOC_RTC_SLOW_CLK_SRC_RC_SLOW;
    case 1:
        return SOC_RTC_SLOW_CLK_SRC_XTAL32K;
    case 2:
        return SOC_RTC_SLOW_CLK_SRC_RC32K;
    default:
        return SOC_RTC_SLOW_CLK_SRC_INVALID;
    }
}

/**
 * @brief Select the clock source for LP_PLL_CLK
 *
 * @param in_sel One of the clock sources in soc_lp_pll_clk_src_t
 */
static inline __attribute__((always_inline)) void clk_ll_lp_pll_set_src(soc_lp_pll_clk_src_t in_sel)
{
    uint32_t field_value;
    switch (in_sel) {
    case SOC_LP_PLL_CLK_SRC_RC32K:
        field_value = 0;
        break;
    case SOC_LP_PLL_CLK_SRC_XTAL32K:
        field_value = 1;
        break;
    default:
        // Unsupported LP_PLL_CLK mux input sel
        abort();
    }
    LP_AON_CLKRST.lp_clk_conf.ana_sel_ref_pll8m = field_value;
}

/**
 * @brief Get the clock source for LP_PLL_CLK
 *
 * @return Currently selected clock source (one of soc_lp_pll_clk_src_t values)
 */
static inline __attribute__((always_inline)) soc_lp_pll_clk_src_t clk_ll_lp_pll_get_src(void)
{
    uint32_t clk_sel = LP_AON_CLKRST.lp_clk_conf.ana_sel_ref_pll8m;
    switch (clk_sel) {
    case 0:
        return SOC_LP_PLL_CLK_SRC_RC32K;
    case 1:
        return SOC_LP_PLL_CLK_SRC_XTAL32K;
    default:
        return SOC_LP_PLL_CLK_SRC_INVALID;
    }
}

/**
 * @brief Get LP_PLL_CLK frequency
 *
 * @return LP_PLL clock frequency, in MHz
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_lp_pll_get_freq_mhz(void)
{
    // The target has a fixed 8MHz LP_PLL
    return CLK_LL_PLL_8M_FREQ_MHZ;
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
        LP_AON_CLKRST.lp_clk_conf.fast_clk_sel = 0;
        break;
    case SOC_RTC_FAST_CLK_SRC_XTAL:
        LP_AON_CLKRST.lp_clk_conf.fast_clk_sel = 1;
        break;
    case SOC_RTC_FAST_CLK_SRC_LP_PLL:
        LP_AON_CLKRST.lp_clk_conf.fast_clk_sel = 2;
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
    uint32_t clk_sel = LP_AON_CLKRST.lp_clk_conf.fast_clk_sel;
    switch (clk_sel) {
    case 0:
        return SOC_RTC_FAST_CLK_SRC_RC_FAST;
    case 1:
        return SOC_RTC_FAST_CLK_SRC_XTAL;
    case 2:
        return SOC_RTC_FAST_CLK_SRC_LP_PLL;
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
 * @return Divider. Divider = (CK8M_DIV_SEL + 1).
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
 * @return XTAL frequency, in MHz. Returns 0 if value in reg is invalid.
 */
static inline __attribute__((always_inline)) uint32_t clk_ll_xtal_load_freq_mhz(void)
{
    // Read from RTC storage register
    uint32_t xtal_freq_reg = READ_PERI_REG(RTC_XTAL_FREQ_REG);
    if ((xtal_freq_reg & 0xFFFF) == ((xtal_freq_reg >> 16) & 0xFFFF) &&
        xtal_freq_reg != 0 && xtal_freq_reg != UINT32_MAX) {
        return xtal_freq_reg & ~RTC_DISABLE_ROM_LOG & UINT16_MAX;
    }
    // If the format in reg is invalid
    return 0;
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

/**
 * @brief Clock output channel configuration
 * @param clk_sig    The clock signal source to be mapped to GPIOs
 * @param channel_id The clock output channel to setup
 */
static inline __attribute__((always_inline)) void clk_ll_set_dbg_clk_ctrl(soc_clkout_sig_id_t clk_sig, clock_out_channel_t channel_id)
{
    if (channel_id == CLKOUT_CHANNEL_1) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.dbg_clk_ctrl0, reg_dbg_ch0_sel, clk_sig);
    } else if (channel_id == CLKOUT_CHANNEL_2) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.dbg_clk_ctrl0, reg_dbg_ch1_sel, clk_sig);
    } else {
        abort();
    }
}

/**
 * @brief Enable the clock output channel
 * @param  enable Enable or disable the clock output channel
 */
static inline __attribute__((always_inline)) void clk_ll_enable_dbg_clk_channel(clock_out_channel_t channel_id, bool enable)
{
    if (channel_id == CLKOUT_CHANNEL_1) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.dbg_clk_ctrl1, reg_dbg_ch0_en, enable);
    } else if (channel_id == CLKOUT_CHANNEL_2) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.dbg_clk_ctrl1, reg_dbg_ch1_en, enable);
    } else {
        abort();
    }
}

/**
 * @brief Output the mapped clock after frequency division
 * @param channel_id channel id that need to be configured with frequency division
 * @param div_num  clock frequency division value
 */
static inline __attribute__((always_inline)) void clk_ll_set_dbg_clk_channel_divider(clock_out_channel_t channel_id, uint32_t div_num)
{
    if (channel_id == CLKOUT_CHANNEL_1) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.dbg_clk_ctrl0, reg_dbg_ch0_div_num, div_num - 1);
    } else if (channel_id == CLKOUT_CHANNEL_2) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.dbg_clk_ctrl1, reg_dbg_ch1_div_num, div_num - 1);
    } else {
        abort();
    }
}

#ifdef __cplusplus
}
#endif

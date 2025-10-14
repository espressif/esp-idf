/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "soc/soc.h"
#include "soc/hp_sys_clkrst_reg.h"
#include "soc/lpperi_struct.h"
#include "soc/i2c_ana_mst_struct.h"
#include "soc/pmu_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ANA_I2C_MST_CLK_HAS_ROOT_GATING 1   /*!< Any regi2c operation needs enable the analog i2c master clock first */

/**
 * @brief Enable analog I2C master clock
 */
static inline __attribute__((always_inline)) void _regi2c_ctrl_ll_master_enable_clock(bool en)
{
    LPPERI.clk_en.ck_en_lp_i2cmst = en;
}

// LPPERI.clk_en is a shared register, so this function must be used in an atomic way
#define regi2c_ctrl_ll_master_enable_clock(...) do { \
        (void)__DECLARE_RCC_RC_ATOMIC_ENV; \
        _regi2c_ctrl_ll_master_enable_clock(__VA_ARGS__); \
    } while(0)

/**
 * @brief Check whether analog I2C master clock is enabled
 */
static inline __attribute__((always_inline)) bool regi2c_ctrl_ll_master_is_clock_enabled(void)
{
    return LPPERI.clk_en.ck_en_lp_i2cmst;
}

/**
 * @brief Reset analog I2C master
 */
static inline __attribute__((always_inline)) void _regi2c_ctrl_ll_master_reset(void)
{
    LPPERI.reset_en.rst_en_lp_i2cmst = 1;
    LPPERI.reset_en.rst_en_lp_i2cmst = 0;
}

// LPPERI.reset_en is a shared register, so this function must be used in an atomic way
#define regi2c_ctrl_ll_master_reset(...) do { \
        (void)__DECLARE_RCC_RC_ATOMIC_ENV; \
        _regi2c_ctrl_ll_master_reset(__VA_ARGS__); \
    } while(0)

/**
 * @brief Configure analog I2C master clock
 */
static inline __attribute__((always_inline)) void regi2c_ctrl_ll_master_configure_clock(void)
{
    I2C_ANA_MST.clk160m.clk_i2c_mst_sel_160m = 1;
}

/**
 * @brief Start CPLL self-calibration
 */
static inline __attribute__((always_inline)) void regi2c_ctrl_ll_cpll_calibration_start(void)
{
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_ANA_PLL_CTRL0_REG, HP_SYS_CLKRST_REG_CPU_PLL_CAL_STOP);
}

/**
 * @brief Stop CPLL self-calibration
 */
static inline __attribute__((always_inline)) void regi2c_ctrl_ll_cpll_calibration_stop(void)
{
    SET_PERI_REG_MASK(HP_SYS_CLKRST_ANA_PLL_CTRL0_REG, HP_SYS_CLKRST_REG_CPU_PLL_CAL_STOP);
}

/**
 * @brief Check whether CPLL calibration is done
 *
 * @return True if calibration is done; otherwise false
 */
static inline __attribute__((always_inline)) bool regi2c_ctrl_ll_cpll_calibration_is_done(void)
{
    return REG_GET_BIT(HP_SYS_CLKRST_ANA_PLL_CTRL0_REG, HP_SYS_CLKRST_REG_CPU_PLL_CAL_END);
}

/**
 * @brief Start MPLL self-calibration
 */
static inline __attribute__((always_inline)) void regi2c_ctrl_ll_mpll_calibration_start(void)
{
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_ANA_PLL_CTRL0_REG, HP_SYS_CLKRST_REG_MSPI_CAL_STOP);
}

/**
 * @brief Stop MPLL self-calibration
 */
static inline __attribute__((always_inline)) void regi2c_ctrl_ll_mpll_calibration_stop(void)
{
    SET_PERI_REG_MASK(HP_SYS_CLKRST_ANA_PLL_CTRL0_REG, HP_SYS_CLKRST_REG_MSPI_CAL_STOP);
}

/**
 * @brief Check whether MPLL calibration is done
 *
 * @return True if calibration is done; otherwise false
 */
static inline __attribute__((always_inline)) bool regi2c_ctrl_ll_mpll_calibration_is_done(void)
{
    return REG_GET_BIT(HP_SYS_CLKRST_ANA_PLL_CTRL0_REG, HP_SYS_CLKRST_REG_MSPI_CAL_END);
}

/**
 * @brief Enable the I2C internal bus to do I2C read/write operation to the SAR_ADC and TSENS registers
 */
static inline void regi2c_ctrl_ll_i2c_sar_periph_enable(void)
{
    //Enter regi2c reset mode
    CLEAR_PERI_REG_MASK(PMU_RF_PWC_REG, PMU_PERIF_I2C_RSTB);
    //Enable REGI2C for SAR_ADC and TSENS
    SET_PERI_REG_MASK(PMU_RF_PWC_REG, PMU_XPD_PERIF_I2C);
    //Release regi2c reset mode, enter work mode
    SET_PERI_REG_MASK(PMU_RF_PWC_REG, PMU_PERIF_I2C_RSTB);
}

/**
 * @brief Disable the I2C internal bus to do I2C read/write operation to the SAR_ADC and TSENS registers
 */
static inline void regi2c_ctrl_ll_i2c_sar_periph_disable(void)
{
    CLEAR_PERI_REG_MASK(PMU_RF_PWC_REG, PMU_XPD_PERIF_I2C);
}

#ifdef __cplusplus
}
#endif

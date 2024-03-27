/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "soc/soc.h"
#include "soc/regi2c_defs.h"
#include "soc/hp_sys_clkrst_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

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
 * @brief Enable the I2C internal bus to do I2C read/write operation to the SAR_ADC register
 */
static inline void regi2c_ctrl_ll_i2c_saradc_enable(void)
{
    CLEAR_PERI_REG_MASK(ANA_CONFIG_REG, ANA_I2C_SAR_FORCE_PD);
    SET_PERI_REG_MASK(ANA_CONFIG2_REG, ANA_I2C_SAR_FORCE_PU);
}

/**
 * @brief Disable the I2C internal bus to do I2C read/write operation to the SAR_ADC register
 */
static inline void regi2c_ctrl_ll_i2c_saradc_disable(void)
{
    CLEAR_PERI_REG_MASK(ANA_CONFIG_REG, ANA_I2C_SAR_FORCE_PU);
    SET_PERI_REG_MASK(ANA_CONFIG2_REG, ANA_I2C_SAR_FORCE_PD);
}

#ifdef __cplusplus
}
#endif

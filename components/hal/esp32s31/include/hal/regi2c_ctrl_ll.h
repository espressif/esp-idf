/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "soc/soc.h"
#include "soc/hp_sys_clkrst_reg.h"
#include "soc/pmu_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable analog I2C master clock
 */
static inline __attribute__((always_inline)) void _regi2c_ctrl_ll_master_enable_clock(bool en)
{
    // TODO: [ESP32S31] IDF-14680
}

// LPPERI.clk_en is a shared register, so this function must be used in an atomic way
#define regi2c_ctrl_ll_master_enable_clock(...) (void)__DECLARE_RCC_RC_ATOMIC_ENV; _regi2c_ctrl_ll_master_enable_clock(__VA_ARGS__)

/**
 * @brief Check whether analog I2C master clock is enabled
 */
static inline __attribute__((always_inline)) bool regi2c_ctrl_ll_master_is_clock_enabled(void)
{
    return 0;// TODO: [ESP32S31] IDF-14680
}

/**
 * @brief Reset analog I2C master
 */
static inline __attribute__((always_inline)) void _regi2c_ctrl_ll_master_reset(void)
{
    // TODO: [ESP32S31] IDF-14680
}

// LPPERI.reset_en is a shared register, so this function must be used in an atomic way
#define regi2c_ctrl_ll_master_reset(...) (void)__DECLARE_RCC_RC_ATOMIC_ENV; _regi2c_ctrl_ll_master_reset(__VA_ARGS__)

/**
 * @brief Configure analog I2C master clock
 */
static inline __attribute__((always_inline)) void regi2c_ctrl_ll_master_configure_clock(void)
{
    // TODO: [ESP32S31] IDF-14680
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
    // TODO: [ESP32S31] IDF-14680
}

/**
 * @brief Disable the I2C internal bus to do I2C read/write operation to the SAR_ADC register
 */
static inline void regi2c_ctrl_ll_i2c_sar_periph_disable(void)
{
    // TODO: [ESP32S31] IDF-14680
}

#ifdef __cplusplus
}
#endif

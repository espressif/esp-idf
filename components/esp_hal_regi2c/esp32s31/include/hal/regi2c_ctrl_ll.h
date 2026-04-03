/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "soc/soc.h"
#include "soc/pmu_reg.h"
#include "modem/modem_lpcon_struct.h"
#include "modem/modem_syscon_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ANA_I2C_MST_CLK_HAS_ROOT_GATING 1   /*!< Any regi2c operation needs enable the analog i2c master clock first */

/**
 * @brief Enable analog I2C master clock
 */
static inline __attribute__((always_inline)) void _regi2c_ctrl_ll_master_enable_clock(bool en)
{
    MODEM_LPCON.clk_conf.clk_i2c_mst_en = en;
}

// use a macro to wrap the function, force the caller to use it in a critical section
// the critical section needs to declare the __DECLARE_RCC_RC_ATOMIC_ENV variable in advance
#define regi2c_ctrl_ll_master_enable_clock(...) (void)__DECLARE_RCC_RC_ATOMIC_ENV; _regi2c_ctrl_ll_master_enable_clock(__VA_ARGS__)

/**
 * @brief Check whether analog I2C master clock is enabled
 */
static inline __attribute__((always_inline)) bool regi2c_ctrl_ll_master_is_clock_enabled(void)
{
    return MODEM_LPCON.clk_conf.clk_i2c_mst_en;
}

/**
 * @brief Reset analog I2C master
 */
static inline __attribute__((always_inline)) void _regi2c_ctrl_ll_master_reset(void)
{
    MODEM_LPCON.rst_conf.rst_i2c_mst = 1;
    MODEM_LPCON.rst_conf.rst_i2c_mst = 0;
}

// LPPERI.reset_en is a shared register, so this function must be used in an atomic way
#define regi2c_ctrl_ll_master_reset(...) (void)__DECLARE_RCC_RC_ATOMIC_ENV; _regi2c_ctrl_ll_master_reset(__VA_ARGS__)

/**
 * @brief Force enable analog I2C master clock
 */
static inline __attribute__((always_inline)) void regi2c_ctrl_ll_master_force_enable_clock(bool en)
{
    MODEM_LPCON.clk_conf_force_on.clk_i2c_mst_fo = en;
}

/**
 * @brief Configure analog I2C master clock
 */
static inline __attribute__((always_inline)) void regi2c_ctrl_ll_master_configure_clock(void)
{
    MODEM_SYSCON.clk_conf.clk_i2c_mst_sel_160m = 1;
}

/**
 * @brief Enable the I2C internal bus to do I2C read/write operation to the SAR_ADC and TSENS registers
 */
static inline void regi2c_ctrl_ll_i2c_sar_periph_enable(void)
{
    // TODO: IDF-14632, IDF-14744
    SET_PERI_REG_MASK(PMU_ANA_PERI_PWR_CTRL_REG, PMU_XPD_PERIF_I2C);
    SET_PERI_REG_MASK(PMU_ANA_PERI_PWR_CTRL_REG, PMU_RSTB_PERIF_I2C);
}

/**
 * @brief Disable the I2C internal bus to do I2C read/write operation to the SAR_ADC register
 */
static inline void regi2c_ctrl_ll_i2c_sar_periph_disable(void)
{
    // TODO: IDF-14632, IDF-14744
    CLEAR_PERI_REG_MASK(PMU_ANA_PERI_PWR_CTRL_REG, PMU_XPD_PERIF_I2C);
}

#ifdef __cplusplus
}
#endif

/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "soc/soc.h"
#include "soc/i2c_ana_mst_reg.h"
#include "soc/pmu_reg.h"
#include "modem/modem_lpcon_struct.h"

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

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_RC_ATOMIC_ENV variable in advance
#define regi2c_ctrl_ll_master_enable_clock(...) do { \
        (void)__DECLARE_RCC_RC_ATOMIC_ENV; \
        _regi2c_ctrl_ll_master_enable_clock(__VA_ARGS__); \
    } while(0)

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

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_RC_ATOMIC_ENV variable in advance
#define regi2c_ctrl_ll_master_reset(...) do { \
        (void)__DECLARE_RCC_RC_ATOMIC_ENV; \
        _regi2c_ctrl_ll_master_reset(__VA_ARGS__); \
    } while(0)

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
    MODEM_LPCON.i2c_mst_clk_conf.clk_i2c_mst_sel_160m = 1;
}

/**
 * @brief Start BBPLL self-calibration
 */
static inline __attribute__((always_inline)) void regi2c_ctrl_ll_bbpll_calibration_start(void)
{
    REG_CLR_BIT(I2C_ANA_MST_ANA_CONF0_REG, I2C_MST_BBPLL_STOP_FORCE_HIGH);
    REG_SET_BIT(I2C_ANA_MST_ANA_CONF0_REG, I2C_MST_BBPLL_STOP_FORCE_LOW);
}

/**
 * @brief Stop BBPLL self-calibration
 */
static inline __attribute__((always_inline)) void regi2c_ctrl_ll_bbpll_calibration_stop(void)
{
    REG_CLR_BIT(I2C_ANA_MST_ANA_CONF0_REG, I2C_MST_BBPLL_STOP_FORCE_LOW);
    REG_SET_BIT(I2C_ANA_MST_ANA_CONF0_REG, I2C_MST_BBPLL_STOP_FORCE_HIGH);
}

/**
 * @brief Check whether BBPLL calibration is done
 *
 * @return True if calibration is done; otherwise false
 */
static inline __attribute__((always_inline)) bool regi2c_ctrl_ll_bbpll_calibration_is_done(void)
{
    return REG_GET_BIT(I2C_ANA_MST_ANA_CONF0_REG, I2C_MST_BBPLL_CAL_DONE);
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

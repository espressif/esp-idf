/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "soc/soc.h"
#include "soc/regi2c_defs.h"
#include "soc/i2c_ana_mst_reg.h"
#include "soc/pmu_reg.h"

//TODO: [ESP32H21] IDF-11550, inherit from h2

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable analog I2C master clock
 */
static inline __attribute__((always_inline)) void regi2c_ctrl_ll_master_enable_clock(bool en)
{
    //TODO: [ESP32H21] IDF-11550
}

/**
 * @brief Reset analog I2C master
 */
static inline __attribute__((always_inline)) void regi2c_ctrl_ll_master_reset(void)
{
    //TODO: [ESP32H21] IDF-11550
}

/**
 * @brief Force enable analog I2C master clock
 */
static inline __attribute__((always_inline)) void regi2c_ctrl_ll_master_force_enable_clock(bool en)
{
    //TODO: [ESP32H21] IDF-11550
}

/**
 * @brief Configure analog I2C master clock
 */
static inline __attribute__((always_inline)) void regi2c_ctrl_ll_master_configure_clock(void)
{
    // Nothing to configure
}

/**
 * @brief Start BBPLL self-calibration
 */
static inline __attribute__((always_inline)) void regi2c_ctrl_ll_bbpll_calibration_start(void)
{
    REG_CLR_BIT(I2C_MST_ANA_CONF0_REG, I2C_MST_BBPLL_STOP_FORCE_HIGH);
    REG_SET_BIT(I2C_MST_ANA_CONF0_REG, I2C_MST_BBPLL_STOP_FORCE_LOW);
}

/**
 * @brief Stop BBPLL self-calibration
 */
static inline __attribute__((always_inline)) void regi2c_ctrl_ll_bbpll_calibration_stop(void)
{
    REG_CLR_BIT(I2C_MST_ANA_CONF0_REG, I2C_MST_BBPLL_STOP_FORCE_LOW);
    REG_SET_BIT(I2C_MST_ANA_CONF0_REG, I2C_MST_BBPLL_STOP_FORCE_HIGH);
}

/**
 * @brief Check whether BBPLL calibration is done
 *
 * @return True if calibration is done; otherwise false
 */
static inline __attribute__((always_inline)) bool regi2c_ctrl_ll_bbpll_calibration_is_done(void)
{
    return REG_GET_BIT(I2C_MST_ANA_CONF0_REG, I2C_MST_BBPLL_CAL_DONE);
}

/**
 * @brief Enable the I2C internal bus to do I2C read/write operation to the SAR_ADC and TSENS registers
 */
static inline void regi2c_ctrl_ll_i2c_sar_periph_enable(void)
{
    SET_PERI_REG_MASK(PMU_RF_PWC_REG, PMU_XPD_PERIF_I2C);
}

/**
 * @brief Disable the I2C internal bus to do I2C read/write operation to the SAR_ADC register
 */
static inline void regi2c_ctrl_ll_i2c_sar_periph_disable(void)
{
    CLEAR_PERI_REG_MASK(PMU_RF_PWC_REG, PMU_XPD_PERIF_I2C);
}

#ifdef __cplusplus
}
#endif

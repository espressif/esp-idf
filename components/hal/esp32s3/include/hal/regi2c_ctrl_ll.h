/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc.h"
#include "soc/regi2c_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reset (Disable) the I2C internal bus for all regi2c registers
 */
static inline void regi2c_ctrl_ll_i2c_reset(void)
{
    SET_PERI_REG_BITS(ANA_CONFIG_REG, ANA_CONFIG_M, ANA_CONFIG_M, ANA_CONFIG_S);
}

/**
 * @brief Enable the I2C internal bus to do I2C read/write operation to the BBPLL configuration register
 */
static inline void regi2c_ctrl_ll_i2c_bbpll_enable(void)
{
    CLEAR_PERI_REG_MASK(ANA_CONFIG_REG, I2C_BBPLL_M);
}

/**
 * @brief Enable the I2C internal bus to do I2C read/write operation to the SAR_ADC register
 */
static inline void regi2c_ctrl_ll_i2c_saradc_enable(void)
{
    CLEAR_PERI_REG_MASK(ANA_CONFIG_REG, I2C_SAR_M);
    SET_PERI_REG_MASK(ANA_CONFIG2_REG, ANA_SAR_CFG2_M);
}

/**
 * @brief Disable the I2C internal bus to do I2C read/write operation to the SAR_ADC register
 */
static inline void regi2c_ctrl_ll_i2c_saradc_disable(void)
{
    CLEAR_PERI_REG_MASK(ANA_CONFIG2_REG, ANA_SAR_CFG2_M);
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

#ifdef __cplusplus
}
#endif

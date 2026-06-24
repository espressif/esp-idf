/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
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
 * @brief Enable the I2C internal bus to do I2C read/write operation to the APLL configuration register
 */
static inline void regi2c_ctrl_ll_i2c_apll_enable(void)
{
    CLEAR_PERI_REG_MASK(ANA_CONFIG_REG, I2C_APLL_M);
}

/**
 * @brief Enable the I2C internal bus to do I2C read/write operation to the SAR_ADC and TSENS registers
 */
static inline void regi2c_ctrl_ll_i2c_sar_periph_enable(void)
{
    // Not used on ESP32, but leave a blank function here.
    // I2C_SARADC is only used for enabling some analog features. However,
    // ESP32 does not use it to support those features.
    // In order to make it convenient for compiling other targets, left a blank function here.
}

/**
 * @brief Disable the I2C internal bus to do I2C read/write operation to the SAR_ADC and TSENS registers
 */
static inline void regi2c_ctrl_ll_i2c_sar_periph_disable(void)
{
    // Not used on ESP32, but leave a blank function here.
    // I2C_SARADC is only used for enabling some analog features. However,
    // ESP32 does not use it to support those features.
    // In order to make it convenient for compiling other targets, left a blank function here.
}

#ifdef __cplusplus
}
#endif

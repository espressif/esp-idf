/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "soc/soc.h"
#include "soc/regi2c_defs.h"
#include "modem/modem_lpcon_struct.h"
#include "modem/modem_syscon_struct.h"
#include "soc/i2c_ana_mst_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable analog I2C master clock
 */
static inline __attribute__((always_inline)) void regi2c_ctrl_ll_master_enable_clock(bool en)
{
    MODEM_LPCON.clk_conf.clk_i2c_mst_en = en;
}

/**
 * @brief Reset analog I2C master
 */
static inline __attribute__((always_inline)) void regi2c_ctrl_ll_master_reset(void)
{
    MODEM_LPCON.rst_conf.rst_i2c_mst = 1;
    MODEM_LPCON.rst_conf.rst_i2c_mst = 0;
}

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
 * @brief Enable the I2C internal bus to do I2C read/write operation to the SAR_ADC register
 */
static inline void regi2c_ctrl_ll_i2c_saradc_enable(void)
{
    // TODO: IDF-8727
}

/**
 * @brief Disable the I2C internal bus to do I2C read/write operation to the SAR_ADC register
 */
static inline void regi2c_ctrl_ll_i2c_saradc_disable(void)
{
    // TODO: IDF-8727
}

#ifdef __cplusplus
}
#endif

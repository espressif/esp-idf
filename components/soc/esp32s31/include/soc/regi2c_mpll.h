/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once

/**
 * @file regi2c_mpll.h
 * @brief Register definitions for MSPI_PLL (MPLL)
 *
 * This file lists register fields of MPLL, located on an internal configuration
 * bus. These definitions are used via macros defined in regi2c_ctrl.h
 */

#define I2C_MPLL                     0xB
#define I2C_MPLL_HOSTID              0

#define I2C_MPLL_IR_CAL_EXT_CAP        1
#define I2C_MPLL_IR_CAL_EXT_CAP_MSB    1
#define I2C_MPLL_IR_CAL_EXT_CAP_LSB    0

#define I2C_MPLL_IR_CAL_ENX_CAP        1
#define I2C_MPLL_IR_CAL_ENX_CAP_MSB    2
#define I2C_MPLL_IR_CAL_ENX_CAP_LSB    2

#define I2C_MPLL_IR_CAL_RSTB         1
#define I2C_MPLL_IR_CAL_RSTB_MSB     5
#define I2C_MPLL_IR_CAL_RSTB_lSB     5

#define I2C_MPLL_DHREF               3
#define I2C_MPLL_DHREF_MSB           5
#define I2C_MPLL_DHREF_LSB           4

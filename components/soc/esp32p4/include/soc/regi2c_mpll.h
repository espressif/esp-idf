/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @file regi2c_mpll.h
 * @brief Register definitions for MSPI_PLL (MPLL)
 *
 * This file lists register fields of MPLL, located on an internal configuration
 * bus. These definitions are used via macros defined in regi2c_ctrl.h
 */

#define I2C_MPLL                     0x63
#define I2C_MPLL_HOSTID              0

#define I2C_MPLL_IR_CAL_RSTB         1
#define I2C_MPLL_IR_CAL_RSTB_MSB     5
#define I2C_MPLL_IR_CAL_RSTB_lSB     5

#define I2C_MPLL_DIV_REG_ADDR        2
#define I2C_MPLL_REF_DIV_ADDR        I2C_MPLL_DIV_REG_ADDR
#define I2C_MPLL_REF_DIV_ADDR_MSB    2
#define I2C_MPLL_REF_DIV_ADDR_LSB    0

#define I2C_MPLL_DIV_ADDR            I2C_MPLL_DIV_REG_ADDR
#define I2C_MPLL_DIV_ADDR_MSB        7
#define I2C_MPLL_DIV_ADDR_LSB        3

#define I2C_MPLL_DHREF               3
#define I2C_MPLL_DHREF_MSB           5
#define I2C_MPLL_DHREF_LSB           4

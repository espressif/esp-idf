/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @file regi2c_bbpll.h
 * @brief Register definitions for digital PLL (BBPLL)
 *
 * This file lists register fields of BBPLL, located on an internal configuration
 * bus. These definitions are used via macros defined in regi2c_ctrl.h, by
 * rtc_clk_cpu_freq_set function in rtc_clk.c.
 */

#define I2C_BBPLL           0x66
#define I2C_BBPLL_HOSTID    0

#define I2C_BBPLL_OC_REF_DIV        2
#define I2C_BBPLL_OC_REF_DIV_MSB    3
#define I2C_BBPLL_OC_REF_DIV_LSB    0

#define I2C_BBPLL_OC_DIV        3
#define I2C_BBPLL_OC_DIV_MSB    5
#define I2C_BBPLL_OC_DIV_LSB    0

#define I2C_BBPLL_OC_DHREF_SEL        5
#define I2C_BBPLL_OC_DHREF_SEL_MSB    5
#define I2C_BBPLL_OC_DHREF_SEL_LSB    4

#define I2C_BBPLL_OC_DLREF_SEL        5
#define I2C_BBPLL_OC_DLREF_SEL_MSB    7
#define I2C_BBPLL_OC_DLREF_SEL_LSB    6

/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
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


#define I2C_BBPLL_IR_CAL_EXT_CAP        1
#define I2C_BBPLL_IR_CAL_EXT_CAP_MSB    4
#define I2C_BBPLL_IR_CAL_EXT_CAP_LSB    0

#define I2C_BBPLL_IR_CAL_ENX_CAP        1
#define I2C_BBPLL_IR_CAL_ENX_CAP_MSB    5
#define I2C_BBPLL_IR_CAL_ENX_CAP_LSB    5

#define I2C_BBPLL_IR_CAL_RSTB        1
#define I2C_BBPLL_IR_CAL_RSTB_MSB    6
#define I2C_BBPLL_IR_CAL_RSTB_LSB    6

#define I2C_BBPLL_IR_CAL_START        1
#define I2C_BBPLL_IR_CAL_START_MSB    7
#define I2C_BBPLL_IR_CAL_START_LSB    7



#define I2C_BBPLL_OC_REF_DIV        2
#define I2C_BBPLL_OC_REF_DIV_MSB    3
#define I2C_BBPLL_OC_REF_DIV_LSB    0

#define I2C_BBPLL_OC_DCHGP        2
#define I2C_BBPLL_OC_DCHGP_MSB    6
#define I2C_BBPLL_OC_DCHGP_LSB    4

#define I2C_BBPLL_IR_CAL_UNSTOP        2
#define I2C_BBPLL_IR_CAL_UNSTOP_MSB    7
#define I2C_BBPLL_IR_CAL_UNSTOP_LSB    7

#define I2C_BBPLL_OC_DIV_7_0        3
#define I2C_BBPLL_OC_DIV_7_0_MSB    7
#define I2C_BBPLL_OC_DIV_7_0_LSB    0

#define I2C_BBPLL_OC_DR1        5
#define I2C_BBPLL_OC_DR1_MSB    2
#define I2C_BBPLL_OC_DR1_LSB    0

#define I2C_BBPLL_OC_DR3        5
#define I2C_BBPLL_OC_DR3_MSB    6
#define I2C_BBPLL_OC_DR3_LSB    4

#define I2C_BBPLL_OC_DHREF_SEL        6
#define I2C_BBPLL_OC_DHREF_SEL_MSB    5
#define I2C_BBPLL_OC_DHREF_SEL_LSB    4

#define I2C_BBPLL_OC_DLREF_SEL        6
#define I2C_BBPLL_OC_DLREF_SEL_MSB    7
#define I2C_BBPLL_OC_DLREF_SEL_LSB    6

#define I2C_BBPLL_OR_LOCK        8
#define I2C_BBPLL_OR_LOCK_MSB    3
#define I2C_BBPLL_OR_LOCK_LSB    3

#define I2C_BBPLL_OC_VCO_DBIAS        9
#define I2C_BBPLL_OC_VCO_DBIAS_MSB    2
#define I2C_BBPLL_OC_VCO_DBIAS_LSB    0

#define I2C_BBPLL_ENT_PLL        10
#define I2C_BBPLL_ENT_PLL_MSB    3
#define I2C_BBPLL_ENT_PLL_LSB    3

#define I2C_BBPLL_DTEST        10
#define I2C_BBPLL_DTEST_MSB    5
#define I2C_BBPLL_DTEST_LSB    4

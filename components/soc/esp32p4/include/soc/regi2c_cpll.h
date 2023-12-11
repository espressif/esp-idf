/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @file regi2c_cpll.h
 * @brief Register definitions for CPU_PLL (CPLL)
 *
 * This file lists register fields of CPLL, located on an internal configuration
 * bus. These definitions are used via macros defined in regi2c_ctrl.h, by
 * rtc_clk_cpu_freq_set function in rtc_clk.c.
 */

#define I2C_CPLL           0x67
#define I2C_CPLL_HOSTID    0

#define I2C_CPLL_IR_CAL_DELAY        0
#define I2C_CPLL_IR_CAL_DELAY_MSB    3
#define I2C_CPLL_IR_CAL_DELAY_LSB    0

#define I2C_CPLL_IR_CAL_CK_DIV        0
#define I2C_CPLL_IR_CAL_CK_DIV_MSB    7
#define I2C_CPLL_IR_CAL_CK_DIV_LSB    4

#define I2C_CPLL_IR_CAL_EXT_CAP        1
#define I2C_CPLL_IR_CAL_EXT_CAP_MSB    3
#define I2C_CPLL_IR_CAL_EXT_CAP_LSB    0

#define I2C_CPLL_IR_CAL_ENX_CAP        1
#define I2C_CPLL_IR_CAL_ENX_CAP_MSB    4
#define I2C_CPLL_IR_CAL_ENX_CAP_LSB    4

#define I2C_CPLL_IR_CAL_RSTB        1
#define I2C_CPLL_IR_CAL_RSTB_MSB    5
#define I2C_CPLL_IR_CAL_RSTB_LSB    5

#define I2C_CPLL_IR_CAL_START        1
#define I2C_CPLL_IR_CAL_START_MSB    6
#define I2C_CPLL_IR_CAL_START_LSB    6

#define I2C_CPLL_IR_CAL_UNSTOP        1
#define I2C_CPLL_IR_CAL_UNSTOP_MSB    7
#define I2C_CPLL_IR_CAL_UNSTOP_LSB    7

#define I2C_CPLL_OC_REF_DIV        2
#define I2C_CPLL_OC_REF_DIV_MSB    3
#define I2C_CPLL_OC_REF_DIV_LSB    0

#define I2C_CPLL_OC_DCHGP        2
#define I2C_CPLL_OC_DCHGP_MSB    6
#define I2C_CPLL_OC_DCHGP_LSB    4

#define I2C_CPLL_OC_ENB_FCAL        2
#define I2C_CPLL_OC_ENB_FCAL_MSB    7
#define I2C_CPLL_OC_ENB_FCAL_LSB    7

#define I2C_CPLL_OC_DIV_7_0        3
#define I2C_CPLL_OC_DIV_7_0_MSB    7
#define I2C_CPLL_OC_DIV_7_0_LSB    0

#define I2C_CPLL_MODE_HF        4
#define I2C_CPLL_MODE_HF_MSB    1
#define I2C_CPLL_MODE_HF_LSB    1

#define I2C_CPLL_DIV_CPU        4
#define I2C_CPLL_DIV_CPU_MSB    5
#define I2C_CPLL_DIV_CPU_LSB    5

#define I2C_CPLL_OC_ENB_VCON        4
#define I2C_CPLL_OC_ENB_VCON_MSB    6
#define I2C_CPLL_OC_ENB_VCON_LSB    6

#define I2C_CPLL_OC_TSCHGP        4
#define I2C_CPLL_OC_TSCHGP_MSB    7
#define I2C_CPLL_OC_TSCHGP_LSB    7

#define I2C_CPLL_OC_DR1        5
#define I2C_CPLL_OC_DR1_MSB    2
#define I2C_CPLL_OC_DR1_LSB    0

#define I2C_CPLL_OC_DR3        5
#define I2C_CPLL_OC_DR3_MSB    6
#define I2C_CPLL_OC_DR3_LSB    4

#define I2C_CPLL_EN_USB        5
#define I2C_CPLL_EN_USB_MSB    7
#define I2C_CPLL_EN_USB_LSB    7

#define I2C_CPLL_OC_DCUR        6
#define I2C_CPLL_OC_DCUR_MSB    2
#define I2C_CPLL_OC_DCUR_LSB    0

#define I2C_CPLL_INC_CUR        6
#define I2C_CPLL_INC_CUR_MSB    3
#define I2C_CPLL_INC_CUR_LSB    3

#define I2C_CPLL_OC_DHREF_SEL        6
#define I2C_CPLL_OC_DHREF_SEL_MSB    5
#define I2C_CPLL_OC_DHREF_SEL_LSB    4

#define I2C_CPLL_OC_DLREF_SEL        6
#define I2C_CPLL_OC_DLREF_SEL_MSB    7
#define I2C_CPLL_OC_DLREF_SEL_LSB    6

#define I2C_CPLL_OR_CAL_CAP        8
#define I2C_CPLL_OR_CAL_CAP_MSB    3
#define I2C_CPLL_OR_CAL_CAP_LSB    0

#define I2C_CPLL_OR_CAL_UDF        8
#define I2C_CPLL_OR_CAL_UDF_MSB    4
#define I2C_CPLL_OR_CAL_UDF_LSB    4

#define I2C_CPLL_OR_CAL_OVF        8
#define I2C_CPLL_OR_CAL_OVF_MSB    5
#define I2C_CPLL_OR_CAL_OVF_LSB    5

#define I2C_CPLL_OR_CAL_END        8
#define I2C_CPLL_OR_CAL_END_MSB    6
#define I2C_CPLL_OR_CAL_END_LSB    6

#define I2C_CPLL_OR_LOCK        8
#define I2C_CPLL_OR_LOCK_MSB    7
#define I2C_CPLL_OR_LOCK_LSB    7

#define I2C_CPLL_OC_VCO_DBIAS        9
#define I2C_CPLL_OC_VCO_DBIAS_MSB    1
#define I2C_CPLL_OC_VCO_DBIAS_LSB    0

#define I2C_CPLL_ENT_PLL        10
#define I2C_CPLL_ENT_PLL_MSB    3
#define I2C_CPLL_ENT_PLL_LSB    3

#define I2C_CPLL_DTEST        10
#define I2C_CPLL_DTEST_MSB    5
#define I2C_CPLL_DTEST_LSB    4

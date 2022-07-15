/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @file regi2c_apll.h
 * @brief Register definitions for audio PLL (APLL)
 *
 * This file lists register fields of APLL, located on an internal configuration
 * bus. These definitions are used via macros defined in regi2c_ctrl.h, by
 * rtc_clk_apll_freq_set and rtc_clk_apll_enable function in rtc_clk.c.
 */

#define I2C_APLL            0X6D
#define I2C_APLL_HOSTID     3

#define I2C_APLL_IR_CAL_DELAY        0
#define I2C_APLL_IR_CAL_DELAY_MSB    3
#define I2C_APLL_IR_CAL_DELAY_LSB    0

#define I2C_APLL_IR_CAL_RSTB        0
#define I2C_APLL_IR_CAL_RSTB_MSB    4
#define I2C_APLL_IR_CAL_RSTB_LSB    4

#define I2C_APLL_IR_CAL_START        0
#define I2C_APLL_IR_CAL_START_MSB    5
#define I2C_APLL_IR_CAL_START_LSB    5

#define I2C_APLL_IR_CAL_UNSTOP        0
#define I2C_APLL_IR_CAL_UNSTOP_MSB    6
#define I2C_APLL_IR_CAL_UNSTOP_LSB    6

#define I2C_APLL_OC_ENB_FCAL        0
#define I2C_APLL_OC_ENB_FCAL_MSB    7
#define I2C_APLL_OC_ENB_FCAL_LSB    7

#define I2C_APLL_IR_CAL_EXT_CAP        1
#define I2C_APLL_IR_CAL_EXT_CAP_MSB    4
#define I2C_APLL_IR_CAL_EXT_CAP_LSB    0

#define I2C_APLL_IR_CAL_ENX_CAP        1
#define I2C_APLL_IR_CAL_ENX_CAP_MSB    5
#define I2C_APLL_IR_CAL_ENX_CAP_LSB    5

#define I2C_APLL_OC_LBW        1
#define I2C_APLL_OC_LBW_MSB    6
#define I2C_APLL_OC_LBW_LSB    6

#define I2C_APLL_IR_CAL_CK_DIV        2
#define I2C_APLL_IR_CAL_CK_DIV_MSB    3
#define I2C_APLL_IR_CAL_CK_DIV_LSB    0

#define I2C_APLL_OC_DCHGP        2
#define I2C_APLL_OC_DCHGP_MSB    6
#define I2C_APLL_OC_DCHGP_LSB    4

#define I2C_APLL_OC_ENB_VCON        2
#define I2C_APLL_OC_ENB_VCON_MSB    7
#define I2C_APLL_OC_ENB_VCON_LSB    7

#define I2C_APLL_OR_CAL_CAP        3
#define I2C_APLL_OR_CAL_CAP_MSB    4
#define I2C_APLL_OR_CAL_CAP_LSB    0

#define I2C_APLL_OR_CAL_UDF        3
#define I2C_APLL_OR_CAL_UDF_MSB    5
#define I2C_APLL_OR_CAL_UDF_LSB    5

#define I2C_APLL_OR_CAL_OVF        3
#define I2C_APLL_OR_CAL_OVF_MSB    6
#define I2C_APLL_OR_CAL_OVF_LSB    6

#define I2C_APLL_OR_CAL_END        3
#define I2C_APLL_OR_CAL_END_MSB    7
#define I2C_APLL_OR_CAL_END_LSB    7

#define I2C_APLL_OR_OUTPUT_DIV        4
#define I2C_APLL_OR_OUTPUT_DIV_MSB    4
#define I2C_APLL_OR_OUTPUT_DIV_LSB    0

#define I2C_APLL_OC_TSCHGP        4
#define I2C_APLL_OC_TSCHGP_MSB    6
#define I2C_APLL_OC_TSCHGP_LSB    6

#define I2C_APLL_EN_FAST_CAL        4
#define I2C_APLL_EN_FAST_CAL_MSB    7
#define I2C_APLL_EN_FAST_CAL_LSB    7

#define I2C_APLL_OC_DHREF_SEL        5
#define I2C_APLL_OC_DHREF_SEL_MSB    1
#define I2C_APLL_OC_DHREF_SEL_LSB    0

#define I2C_APLL_OC_DLREF_SEL        5
#define I2C_APLL_OC_DLREF_SEL_MSB    3
#define I2C_APLL_OC_DLREF_SEL_LSB    2

#define I2C_APLL_SDM_DITHER        5
#define I2C_APLL_SDM_DITHER_MSB    4
#define I2C_APLL_SDM_DITHER_LSB    4

#define I2C_APLL_SDM_STOP        5
#define I2C_APLL_SDM_STOP_MSB    5
#define I2C_APLL_SDM_STOP_LSB    5

#define I2C_APLL_SDM_RSTB        5
#define I2C_APLL_SDM_RSTB_MSB    6
#define I2C_APLL_SDM_RSTB_LSB    6

#define I2C_APLL_OC_DVDD        6
#define I2C_APLL_OC_DVDD_MSB    4
#define I2C_APLL_OC_DVDD_LSB    0

#define I2C_APLL_DSDM2        7
#define I2C_APLL_DSDM2_MSB    5
#define I2C_APLL_DSDM2_LSB    0

#define I2C_APLL_DSDM1        8
#define I2C_APLL_DSDM1_MSB    7
#define I2C_APLL_DSDM1_LSB    0

#define I2C_APLL_DSDM0        9
#define I2C_APLL_DSDM0_MSB    7
#define I2C_APLL_DSDM0_LSB    0

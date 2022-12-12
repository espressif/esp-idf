/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @file regi2c_bias.h
 * @brief Register definitions for bias
 *
 * This file lists register fields of BIAS. These definitions are used via macros defined in regi2c_ctrl.h, by
 * bootloader_hardware_init function in bootloader_esp32c3.c.
 */

#define I2C_BIAS           0x6a
#define I2C_BIAS_HOSTID    0

#define I2C_BIAS_DREG_1P6        0
#define I2C_BIAS_DREG_1P6_MSB    3
#define I2C_BIAS_DREG_1P6_LSB    0

#define I2C_BIAS_DREG_0P8        0
#define I2C_BIAS_DREG_0P8_MSB    7
#define I2C_BIAS_DREG_0P8_LSB    4

#define I2C_BIAS_DREG_1P1_PVT        1
#define I2C_BIAS_DREG_1P1_PVT_MSB    3
#define I2C_BIAS_DREG_1P1_PVT_LSB    0

#define I2C_BIAS_DREG_1P2        1
#define I2C_BIAS_DREG_1P2_MSB    7
#define I2C_BIAS_DREG_1P2_LSB    4

#define I2C_BIAS_ENT_CPREG        2
#define I2C_BIAS_ENT_CPREG_MSB    0
#define I2C_BIAS_ENT_CPREG_LSB    0

#define I2C_BIAS_ENT_CGM        2
#define I2C_BIAS_ENT_CGM_MSB    1
#define I2C_BIAS_ENT_CGM_LSB    1

#define I2C_BIAS_CGM_BIAS        2
#define I2C_BIAS_CGM_BIAS_MSB    3
#define I2C_BIAS_CGM_BIAS_LSB    2

#define I2C_BIAS_DREF_IGM        2
#define I2C_BIAS_DREF_IGM_MSB    4
#define I2C_BIAS_DREF_IGM_LSB    4

#define I2C_BIAS_RC_DVREF        2
#define I2C_BIAS_RC_DVREF_MSB    6
#define I2C_BIAS_RC_DVREF_LSB    5

#define I2C_BIAS_FORCE_DISABLE_BIAS_SLEEP        2
#define I2C_BIAS_FORCE_DISABLE_BIAS_SLEEP_MSB    7
#define I2C_BIAS_FORCE_DISABLE_BIAS_SLEEP_LSB    7

#define I2C_BIAS_RC_ENX        3
#define I2C_BIAS_RC_ENX_MSB    0
#define I2C_BIAS_RC_ENX_LSB    0

#define I2C_BIAS_RC_START        3
#define I2C_BIAS_RC_START_MSB    1
#define I2C_BIAS_RC_START_LSB    1

#define I2C_BIAS_RC_DCAP_EXT        3
#define I2C_BIAS_RC_DCAP_EXT_MSB    7
#define I2C_BIAS_RC_DCAP_EXT_LSB    2

#define I2C_BIAS_XPD_RC        4
#define I2C_BIAS_XPD_RC_MSB    0
#define I2C_BIAS_XPD_RC_LSB    0

#define I2C_BIAS_ENT_CONSTI        4
#define I2C_BIAS_ENT_CONSTI_MSB    1
#define I2C_BIAS_ENT_CONSTI_LSB    1

#define I2C_BIAS_XPD_ICX        4
#define I2C_BIAS_XPD_ICX_MSB    2
#define I2C_BIAS_XPD_ICX_LSB    2

#define I2C_BIAS_RC_RSTB        4
#define I2C_BIAS_RC_RSTB_MSB    3
#define I2C_BIAS_RC_RSTB_LSB    3

#define I2C_BIAS_RC_DIV        4
#define I2C_BIAS_RC_DIV_MSB    7
#define I2C_BIAS_RC_DIV_LSB    4

#define I2C_BIAS_RC_CAP        5
#define I2C_BIAS_RC_CAP_MSB    5
#define I2C_BIAS_RC_CAP_LSB    0

#define I2C_BIAS_RC_UD        5
#define I2C_BIAS_RC_UD_MSB    6
#define I2C_BIAS_RC_UD_LSB    6

#define I2C_BIAS_RC_LOCKB        5
#define I2C_BIAS_RC_LOCKB_MSB    7
#define I2C_BIAS_RC_LOCKB_LSB    7

#define I2C_BIAS_RC_CHG_COUNT        6
#define I2C_BIAS_RC_CHG_COUNT_MSB    4
#define I2C_BIAS_RC_CHG_COUNT_LSB    0

#define I2C_BIAS_XPD_CPREG        7
#define I2C_BIAS_XPD_CPREG_MSB    0
#define I2C_BIAS_XPD_CPREG_LSB    0

#define I2C_BIAS_XPD_CGM        7
#define I2C_BIAS_XPD_CGM_MSB    1
#define I2C_BIAS_XPD_CGM_LSB    1

#define I2C_BIAS_DTEST        7
#define I2C_BIAS_DTEST_MSB    3
#define I2C_BIAS_DTEST_LSB    2

#define I2C_BIAS_DRES12K        7
#define I2C_BIAS_DRES12K_MSB    7
#define I2C_BIAS_DRES12K_LSB    4

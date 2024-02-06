/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @file regi2c_bias.h
 * @brief Register definitions for bias
 *
 * This file lists register fields of BIAS. These definitions are used via macros defined in regi2c_ctrl.h, by
 * rtc_clk_init function in esp32p4/rtc_clk_init.c.
 */

#define I2C_BIAS        0x6A
#define I2C_BIAS_HOSTID 0

#define I2C_BIAS_DREG_1P6       0
#define I2C_BIAS_DREG_1P6_MSB   3
#define I2C_BIAS_DREG_1P6_LSB   0

#define I2C_BIAS_DREG_1P1       0
#define I2C_BIAS_DREG_1P1_MSB   7
#define I2C_BIAS_DREG_1P1_LSB   4

#define I2C_BIAS_DREG_1P1_PVT       1
#define I2C_BIAS_DREG_1P1_PVT_MSB   3
#define I2C_BIAS_DREG_1P1_PVT_LSB   0

#define I2C_BIAS_DREG_2P2_PVT       1
#define I2C_BIAS_DREG_2P2_PVT_MSB   7
#define I2C_BIAS_DREG_2P2_PVT_LSB   4

#define I2C_BIAS_OR_FORCE_XPD_CK        4
#define I2C_BIAS_OR_FORCE_XPD_CK_MSB    0
#define I2C_BIAS_OR_FORCE_XPD_CK_LSB    0

#define I2C_BIAS_OR_FORCE_XPD_REF_OUT_BUF        4
#define I2C_BIAS_OR_FORCE_XPD_REF_OUT_BUF_MSB    1
#define I2C_BIAS_OR_FORCE_XPD_REF_OUT_BUF_LSB    1

#define I2C_BIAS_OR_FORCE_XPD_IPH        4
#define I2C_BIAS_OR_FORCE_XPD_IPH_MSB    2
#define I2C_BIAS_OR_FORCE_XPD_IPH_LSB    2

#define I2C_BIAS_OR_FORCE_XPD_VGATE_BUF        4
#define I2C_BIAS_OR_FORCE_XPD_VGATE_BUF_MSB    3
#define I2C_BIAS_OR_FORCE_XPD_VGATE_BUF_LSB    3

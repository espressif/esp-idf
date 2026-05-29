/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once

/**
 * @file regi2c_bias.h
 * @brief Register definitions for bias
 *
 * This file lists register fields of BIAS. These definitions are used via macros defined in regi2c_ctrl.h, by
 * rtc_clk_init function in esp32s31/rtc_clk_init.c.
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

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

#define I2C_BIAS            0X6A
#define I2C_BIAS_HOSTID     0

#define I2C_BIAS_DREG_1P1_PVT 1
#define I2C_BIAS_DREG_1P1_PVT_MSB 3
#define I2C_BIAS_DREG_1P1_PVT_LSB 0

/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @file i2c_brownout.h
 * @brief Register definitions for brownout detector
 *
 * This file lists register fields of the brownout detector, located on an internal configuration
 * bus. These definitions are used via macros defined in i2c_rtc_clk.h.
 */

#define I2C_BOD            0x61
#define I2C_BOD_HOSTID     1

#define I2C_BOD_THRESHOLD           0x5
#define I2C_BOD_THRESHOLD_MSB       2
#define I2C_BOD_THRESHOLD_LSB       0

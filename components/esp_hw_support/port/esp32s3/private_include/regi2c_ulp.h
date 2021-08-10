/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @file regi2c_ulp.h
 * @brief Register definitions for analog to calibrate o_code for getting a more precise voltage.
 *
 * This file lists register fields of ULP, located on an internal configuration
 * bus. These definitions are used via macros defined in regi2c_ctrl.h, by
 * rtc_init function in rtc_init.c.
 */

#define I2C_ULP 0x61
#define I2C_ULP_HOSTID 1

#define I2C_ULP_IR_RESETB 0
#define I2C_ULP_IR_RESETB_MSB 0
#define I2C_ULP_IR_RESETB_LSB 0

#define I2C_ULP_O_DONE_FLAG 3
#define I2C_ULP_O_DONE_FLAG_MSB 0
#define I2C_ULP_O_DONE_FLAG_LSB 0

#define I2C_ULP_BG_O_DONE_FLAG 3
#define I2C_ULP_BG_O_DONE_FLAG_MSB 3
#define I2C_ULP_BG_O_DONE_FLAG_LSB 3

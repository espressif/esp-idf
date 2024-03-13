/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @file regi2c_lp_bias.h
 * @brief Register definitions for analog to calibrate o_code for getting a more precise voltage.
 *
 * This file lists register fields of low power dbais, located on an internal configuration
 * bus. These definitions are used via macros defined in regi2c_ctrl.h, by
 * rtc_init function in rtc_init.c.
 */

#define I2C_ULP 0x61
#define I2C_ULP_HOSTID 0

#define I2C_ULP_IR_RESETB 0
#define I2C_ULP_IR_RESETB_MSB 0
#define I2C_ULP_IR_RESETB_LSB 0

#define I2C_ULP_IR_FORCE_XPD_CK 0
#define I2C_ULP_IR_FORCE_XPD_CK_MSB 2
#define I2C_ULP_IR_FORCE_XPD_CK_LSB 2

#define I2C_ULP_IR_FORCE_XPD_IPH 0
#define I2C_ULP_IR_FORCE_XPD_IPH_MSB 4
#define I2C_ULP_IR_FORCE_XPD_IPH_LSB 4

#define I2C_ULP_IR_DISABLE_WATCHDOG_CK 0
#define I2C_ULP_IR_DISABLE_WATCHDOG_CK_MSB 6
#define I2C_ULP_IR_DISABLE_WATCHDOG_CK_LSB 6

#define I2C_ULP_O_DONE_FLAG 3
#define I2C_ULP_O_DONE_FLAG_MSB 0
#define I2C_ULP_O_DONE_FLAG_LSB 0

#define I2C_ULP_BG_O_DONE_FLAG 3
#define I2C_ULP_BG_O_DONE_FLAG_MSB 3
#define I2C_ULP_BG_O_DONE_FLAG_LSB 3

#define I2C_ULP_OCODE 4
#define I2C_ULP_OCODE_MSB 7
#define I2C_ULP_OCODE_LSB 0

#define I2C_ULP_IR_FORCE_CODE 5
#define I2C_ULP_IR_FORCE_CODE_MSB 6
#define I2C_ULP_IR_FORCE_CODE_LSB 6

#define I2C_ULP_EXT_CODE 6
#define I2C_ULP_EXT_CODE_MSB 7
#define I2C_ULP_EXT_CODE_LSB 0

/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @file regi2c_lp_bias.h
 * @brief Register definitions for analog to calibrate o_code for getting a more precise voltage.
 */

#define I2C_ULP_IR_FORCE_CODE 5
#define I2C_ULP_IR_FORCE_CODE_MSB 6
#define I2C_ULP_IR_FORCE_CODE_LSB 6

//register for OCode
#define I2C_ULP_EXT_CODE 6
#define I2C_ULP_EXT_CODE_MSB 7
#define I2C_ULP_EXT_CODE_LSB 0

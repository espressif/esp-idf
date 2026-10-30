/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once

/**
 * @file regi2c_lp_bias.h
 * @brief ULP bandgap fields used to force the external OCode.
 *
 * Slave address 0x61 is REGI2C_ULP in the S31 analog I2C master.
 * These definitions are used via REGI2C_WRITE_MASK().
 */

#define I2C_ULP 0x61
#define I2C_ULP_HOSTID 0

#define I2C_ULP_IR_FORCE_CODE 5
#define I2C_ULP_IR_FORCE_CODE_MSB 6
#define I2C_ULP_IR_FORCE_CODE_LSB 6

#define I2C_ULP_EXT_CODE 6
#define I2C_ULP_EXT_CODE_MSB 7
#define I2C_ULP_EXT_CODE_LSB 0

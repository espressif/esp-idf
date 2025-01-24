/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @file regi2c_brownout.h
 * @brief Register definitions for brownout detector
 *
 * This file lists register fields of the brownout detector, located on an internal configuration
 * bus. These definitions are used via macros defined in regi2c_ctrl.h.
 */

#define I2C_BOD            0x6a
#define I2C_BOD_HOSTID     0

#define I2C_BOD_THRESHOLD_L           0x9
#define I2C_BOD_THRESHOLD_L_MSB       4
#define I2C_BOD_THRESHOLD_L_LSB       2
#define I2C_BOD_THRESHOLD_H           0x9
#define I2C_BOD_THRESHOLD_H_MSB       7
#define I2C_BOD_THRESHOLD_H_LSB       5

#define I2C_BIAS_OR_DREF_VBAT_L       8
#define I2C_BIAS_OR_DREF_VBAT_L_MSB   4
#define I2C_BIAS_OR_DREF_VBAT_L_LSB   2
#define I2C_BIAS_OR_DREF_VBAT_H       8
#define I2C_BIAS_OR_DREF_VBAT_H_MSB   7
#define I2C_BIAS_OR_DREF_VBAT_H_LSB   5

#define I2C_BIAS_OR_DREF_VBAT_CHARGER_L       10
#define I2C_BIAS_OR_DREF_VBAT_CHARGER_L_MSB   4
#define I2C_BIAS_OR_DREF_VBAT_CHARGER_L_LSB   2
#define I2C_BIAS_OR_DREF_VBAT_CHARGER_H       10
#define I2C_BIAS_OR_DREF_VBAT_CHARGER_H_MSB   7
#define I2C_BIAS_OR_DREF_VBAT_CHARGER_H_LSB   5

#define I2C_BIAS_OR_FORCE_PU_VBAT_CHARGER 10
#define I2C_BIAS_OR_FORCE_PU_VBAT_CHARGER_MSB 0
#define I2C_BIAS_OR_FORCE_PU_VBAT_CHARGER_LSB 0

/** Change the charging current by adjusting the value of the series resistor. 1kΩ + N * 0.5kΩ */
#define I2C_BIAS_OR_DRES_CHARGER     3
#define I2C_BIAS_OR_DRES_CHARGER_MSB 7
#define I2C_BIAS_OR_DRES_CHARGER_LSB 4

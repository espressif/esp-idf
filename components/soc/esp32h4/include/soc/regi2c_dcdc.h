/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @file regi2c_dcdc.h
 * @brief Register definitions for digital to get rtc voltage & digital voltage
 *        by setting rtc_dbias_Wak & dig_dbias_wak or by analog self-calibration.
 */

#define I2C_DCDC    0x6D
#define I2C_DCDC_HOSTID 0

#define I2C_DCDC_CCM_DREG0               7
#define I2C_DCDC_CCM_DREG0_MSB           4
#define I2C_DCDC_CCM_DREG0_LSB           0

#define I2C_DCDC_CCM_PCUR_LIMIT0        7
#define I2C_DCDC_CCM_PCUR_LIMIT0_MSB    7
#define I2C_DCDC_CCM_PCUR_LIMIT0_LSB    5

#define I2C_DCDC_VCM_DREG0              10
#define I2C_DCDC_VCM_DREG0_MSB          4
#define I2C_DCDC_VCM_DREG0_LSB          0

#define I2C_DCDC_VCM_PCUR_LIMIT0        10
#define I2C_DCDC_VCM_PCUR_LIMIT0_MSB    7
#define I2C_DCDC_VCM_PCUR_LIMIT0_LSB    5

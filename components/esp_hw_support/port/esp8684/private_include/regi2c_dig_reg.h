/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @file regi2c_dig_reg.h
 * @brief Register definitions for digital to get rtc voltage & digital voltage
 *        by setting rtc_dbias_Wak & dig_dbias_wak or by analog self-calibration.
 */

#define I2C_DIG_REG 0x6D
#define I2C_DIG_REG_HOSTID 0

#define I2C_DIG_REG_EXT_RTC_DREG    4
#define I2C_DIG_REG_EXT_RTC_DREG_MSB    4
#define I2C_DIG_REG_EXT_RTC_DREG_LSB    0

#define I2C_DIG_REG_ENX_RTC_DREG    4
#define I2C_DIG_REG_ENX_RTC_DREG_MSB    7
#define I2C_DIG_REG_ENX_RTC_DREG_LSB    7

#define I2C_DIG_REG_EXT_RTC_DREG_SLEEP    5
#define I2C_DIG_REG_EXT_RTC_DREG_SLEEP_MSB    4
#define I2C_DIG_REG_EXT_RTC_DREG_SLEEP_LSB    0

#define I2C_DIG_REG_ENX_RTC_DREG_SLEEP    5
#define I2C_DIG_REG_ENX_RTC_DREG_SLEEP_MSB    7
#define I2C_DIG_REG_ENX_RTC_DREG_SLEEP_LSB    7

#define I2C_DIG_REG_EXT_DIG_DREG    6
#define I2C_DIG_REG_EXT_DIG_DREG_MSB    4
#define I2C_DIG_REG_EXT_DIG_DREG_LSB    0

#define I2C_DIG_REG_ENX_DIG_DREG    6
#define I2C_DIG_REG_ENX_DIG_DREG_MSB    7
#define I2C_DIG_REG_ENX_DIG_DREG_LSB    7

#define I2C_DIG_REG_EXT_DIG_DREG_SLEEP    7
#define I2C_DIG_REG_EXT_DIG_DREG_SLEEP_MSB    4
#define I2C_DIG_REG_EXT_DIG_DREG_SLEEP_LSB    0

#define I2C_DIG_REG_ENX_DIG_DREG_SLEEP    7
#define I2C_DIG_REG_ENX_DIG_DREG_SLEEP_MSB    7
#define I2C_DIG_REG_ENX_DIG_DREG_SLEEP_LSB    7

#define I2C_DIG_REG_OR_EN_CONT_CAL 9
#define I2C_DIG_REG_OR_EN_CONT_CAL_MSB 7
#define I2C_DIG_REG_OR_EN_CONT_CAL_LSB 7

#define I2C_DIG_REG_XPD_RTC_REG 13
#define I2C_DIG_REG_XPD_RTC_REG_MSB 2
#define I2C_DIG_REG_XPD_RTC_REG_LSB 2

#define I2C_DIG_REG_XPD_DIG_REG 13
#define I2C_DIG_REG_XPD_DIG_REG_MSB 3
#define I2C_DIG_REG_XPD_DIG_REG_LSB 3

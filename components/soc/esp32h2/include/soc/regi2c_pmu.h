/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @file regi2c_pmu.h
 * @brief Register definitions for digital to get rtc voltage & digital voltage
 *        by setting rtc_dbias_Wak & dig_dbias_wak or by analog self-calibration.
 */

#define I2C_PMU           0x6d
#define I2C_PMU_HOSTID    0

#define I2C_PMU_EN_I2C_RTC_DREG        8
#define I2C_PMU_EN_I2C_RTC_DREG_MSB    0
#define I2C_PMU_EN_I2C_RTC_DREG_LSB    0

#define I2C_PMU_EN_I2C_DIG_DREG        8
#define I2C_PMU_EN_I2C_DIG_DREG_MSB    1
#define I2C_PMU_EN_I2C_DIG_DREG_LSB    1

#define I2C_PMU_EN_I2C_RTC_DREG_SLP        8
#define I2C_PMU_EN_I2C_RTC_DREG_SLP_MSB    2
#define I2C_PMU_EN_I2C_RTC_DREG_SLP_LSB    2

#define I2C_PMU_EN_I2C_DIG_DREG_SLP        8
#define I2C_PMU_EN_I2C_DIG_DREG_SLP_MSB    3
#define I2C_PMU_EN_I2C_DIG_DREG_SLP_LSB    3

#define I2C_PMU_OR_XPD_RTC_REG            9
#define I2C_PMU_OR_XPD_RTC_REG_MSB        4
#define I2C_PMU_OR_XPD_RTC_REG_LSB        4

#define I2C_PMU_OR_XPD_DIG_REG            9
#define I2C_PMU_OR_XPD_DIG_REG_MSB        5
#define I2C_PMU_OR_XPD_DIG_REG_LSB        5

#define I2C_PMU_OC_SCK_DCAP        14
#define I2C_PMU_OC_SCK_DCAP_MSB    7
#define I2C_PMU_OC_SCK_DCAP_LSB    0

#define I2C_PMU_OR_XPD_TRX        15
#define I2C_PMU_OR_XPD_TRX_MSB    2
#define I2C_PMU_OR_XPD_TRX_LSB    2

#define I2C_PMU_SEL_PLL8M_REF        21
#define I2C_PMU_SEL_PLL8M_REF_MSB    6
#define I2C_PMU_SEL_PLL8M_REF_LSB    6

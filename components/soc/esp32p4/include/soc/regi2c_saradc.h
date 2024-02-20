/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @file regi2c_saradc.h
 * @brief Register definitions for analog to calibrate initial code for getting a more precise voltage of SAR ADC.
 *
 * This file lists register fields of SAR, located on an internal configuration
 * bus. These definitions are used via macros defined in regi2c_ctrl.h, by
 * function in adc_ll.h.
 */

#define I2C_SAR_ADC            0X69
#define I2C_SAR_ADC_HOSTID     0

#define I2C_SARADC_TSENS_DAC 0x6
#define I2C_SARADC_TSENS_DAC_MSB 3
#define I2C_SARADC_TSENS_DAC_LSB 0

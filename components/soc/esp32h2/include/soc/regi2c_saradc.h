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

#define ADC_SAR1_SAMPLE_CYCLE_ADDR 0x2
#define ADC_SAR1_SAMPLE_CYCLE_ADDR_MSB 0x2
#define ADC_SAR1_SAMPLE_CYCLE_ADDR_LSB 0x0

#define ADC_SAR1_DREF_ADDR  0x2
#define ADC_SAR1_DREF_ADDR_MSB  0x6
#define ADC_SAR1_DREF_ADDR_LSB  0x4

#define ADC_SAR2_DREF_ADDR  0x5
#define ADC_SAR2_DREF_ADDR_MSB  0x6
#define ADC_SAR2_DREF_ADDR_LSB  0x4

#define ADC_SAR1_INITIAL_CODE_HIGH_ADDR 0x1
#define ADC_SAR1_INITIAL_CODE_HIGH_ADDR_MSB 0x3
#define ADC_SAR1_INITIAL_CODE_HIGH_ADDR_LSB 0x0

#define ADC_SAR1_INITIAL_CODE_LOW_ADDR  0x0
#define ADC_SAR1_INITIAL_CODE_LOW_ADDR_MSB  0x7
#define ADC_SAR1_INITIAL_CODE_LOW_ADDR_LSB  0x0

#define I2C_SARADC_TSENS_DAC 0x6
#define I2C_SARADC_TSENS_DAC_MSB 3
#define I2C_SARADC_TSENS_DAC_LSB 0

#define I2C_SARADC_DTEST        7
#define I2C_SARADC_DTEST_MSB    1
#define I2C_SARADC_DTEST_LSB    0

#define I2C_SARADC_ENT_SAR      7
#define I2C_SARADC_ENT_SAR_MSB  3
#define I2C_SARADC_ENT_SAR_LSB  1

#define I2C_SARADC_EN_TOUT_SAR1_BUS      7
#define I2C_SARADC_EN_TOUT_SAR1_BUS_MSB  5
#define I2C_SARADC_EN_TOUT_SAR1_BUS_LSB  5

#define I2C_SARADC_SAR1_INIT_CODE_LSB       0
#define I2C_SARADC_SAR1_INIT_CODE_LSB_MSB   7
#define I2C_SARADC_SAR1_INIT_CODE_LSB_LSB   0

#define I2C_SARADC_SAR1_INIT_CODE_MSB       1
#define I2C_SARADC_SAR1_INIT_CODE_MSB_MSB   3
#define I2C_SARADC_SAR1_INIT_CODE_MSB_LSB   0

#define I2C_SARADC_SAR2_INIT_CODE_LSB       3
#define I2C_SARADC_SAR2_INIT_CODE_LSB_MSB   7
#define I2C_SARADC_SAR2_INIT_CODE_LSB_LSB   0

#define I2C_SARADC_SAR2_INIT_CODE_MSB       4
#define I2C_SARADC_SAR2_INIT_CODE_MSB_MSB   3
#define I2C_SARADC_SAR2_INIT_CODE_MSB_LSB   0

#define ADC_SAR1_ENCAL_GND_ADDR             0x8
#define ADC_SAR1_ENCAL_GND_ADDR_MSB         0x1
#define ADC_SAR1_ENCAL_GND_ADDR_LSB         0x1

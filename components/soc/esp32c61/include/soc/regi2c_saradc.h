/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
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

#define I2C_SAR_ADC                             0x69
#define I2C_SAR_ADC_HOSTID                      0

#define ADC_SAR1_INITIAL_CODE_LOW_ADDR          0x0
#define ADC_SAR1_INITIAL_CODE_LOW_ADDR_MSB      0x7
#define ADC_SAR1_INITIAL_CODE_LOW_ADDR_LSB      0x0

#define ADC_SAR1_INITIAL_CODE_HIGH_ADDR         0x1
#define ADC_SAR1_INITIAL_CODE_HIGH_ADDR_MSB     0x3
#define ADC_SAR1_INITIAL_CODE_HIGH_ADDR_LSB     0x0

#define ADC_SAR1_SAMPLE_CYCLE_ADDR              0x2
#define ADC_SAR1_SAMPLE_CYCLE_ADDR_MSB          0x2
#define ADC_SAR1_SAMPLE_CYCLE_ADDR_LSB          0x0

#define ADC_SAR1_DREF_ADDR                      0x2
#define ADC_SAR1_DREF_ADDR_MSB                  0x6
#define ADC_SAR1_DREF_ADDR_LSB                  0x4

#define ADC_SAR2_INITIAL_CODE_LOW_ADDR          0x3
#define ADC_SAR2_INITIAL_CODE_LOW_ADDR_MSB      0x7
#define ADC_SAR2_INITIAL_CODE_LOW_ADDR_LSB      0x0

#define ADC_SAR2_INITIAL_CODE_HIGH_ADDR         0x4
#define ADC_SAR2_INITIAL_CODE_HIGH_ADDR_MSB     0x3
#define ADC_SAR2_INITIAL_CODE_HIGH_ADDR_LSB     0x0

#define ADC_SAR2_DREF_ADDR                      0x5
#define ADC_SAR2_DREF_ADDR_MSB                  0x6
#define ADC_SAR2_DREF_ADDR_LSB                  0x4

#define I2C_SARADC_TSENS_DAC                    0x6
#define I2C_SARADC_TSENS_DAC_MSB                0x3
#define I2C_SARADC_TSENS_DAC_LSB                0x0

#define I2C_SARADC_DTEST                        0x7
#define I2C_SARADC_DTEST_MSB                    0x1
#define I2C_SARADC_DTEST_LSB                    0x0

#define I2C_SARADC_ENT_TSENS                    0x7
#define I2C_SARADC_ENT_TSENS_MSB                0x2
#define I2C_SARADC_ENT_TSENS_LSB                0x2

#define I2C_SARADC_ENT_SAR                      0x7
#define I2C_SARADC_ENT_SAR_MSB                  0x3
#define I2C_SARADC_ENT_SAR_LSB                  0x3

#define I2C_SARADC1_ENCAL_REF                   0x7
#define I2C_SARADC1_ENCAL_REF_MSB               0x4
#define I2C_SARADC1_ENCAL_REF_LSB               0x4

#define ADC_SAR1_ENCAL_GND_ADDR                 0x7
#define ADC_SAR1_ENCAL_GND_ADDR_MSB             0x5
#define ADC_SAR1_ENCAL_GND_ADDR_LSB             0x5

#define I2C_SARADC2_ENCAL_REF                   0x7
#define I2C_SARADC2_ENCAL_REF_MSB               0x6
#define I2C_SARADC2_ENCAL_REF_LSB               0x6

#define I2C_SAR2_ENCAL_GND                      0x7
#define I2C_SAR2_ENCAL_GND_MSB                  0x7
#define I2C_SAR2_ENCAL_GND_LSB                  0x7

#define POWER_GLITCH_XPD_VDET_PERIF             0x10
#define POWER_GLITCH_XPD_VDET_PERIF_MSB         0x0
#define POWER_GLITCH_XPD_VDET_PERIF_LSB         0x0

#define POWER_GLITCH_XPD_VDET_VDDPST            0x10
#define POWER_GLITCH_XPD_VDET_VDDPST_MSB        0x1
#define POWER_GLITCH_XPD_VDET_VDDPST_LSB        0x1

#define POWER_GLITCH_XPD_VDET_PLLBB             0x10
#define POWER_GLITCH_XPD_VDET_PLLBB_MSB         0x2
#define POWER_GLITCH_XPD_VDET_PLLBB_LSB         0x2

#define POWER_GLITCH_XPD_VDET_PLL               0x10
#define POWER_GLITCH_XPD_VDET_PLL_MSB           0x3
#define POWER_GLITCH_XPD_VDET_PLL_LSB           0x3

#define POWER_GLITCH_DREF_VDET_PERIF            0x11
#define POWER_GLITCH_DREF_VDET_PERIF_MSB        0x2
#define POWER_GLITCH_DREF_VDET_PERIF_LSB        0x0

#define POWER_GLITCH_DREF_VDET_VDDPST           0x11
#define POWER_GLITCH_DREF_VDET_VDDPST_MSB       0x6
#define POWER_GLITCH_DREF_VDET_VDDPST_LSB       0x4

#define POWER_GLITCH_DREF_VDET_PLLBB            0x12
#define POWER_GLITCH_DREF_VDET_PLLBB_MSB        0x2
#define POWER_GLITCH_DREF_VDET_PLLBB_LSB        0x0

#define POWER_GLITCH_DREF_VDET_PLL              0x12
#define POWER_GLITCH_DREF_VDET_PLL_MSB          0x6
#define POWER_GLITCH_DREF_VDET_PLL_LSB          0x4

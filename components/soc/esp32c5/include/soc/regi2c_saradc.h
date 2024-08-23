/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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

#define I2C_SAR_ADC_SAR1_INIT_CODE_LSB      0x0
#define I2C_SAR_ADC_SAR1_INIT_CODE_LSB_MSB  0x7
#define I2C_SAR_ADC_SAR1_INIT_CODE_LSB_LSB  0x0

#define I2C_SAR_ADC_SAR1_INIT_CODE_MSB      0x1
#define I2C_SAR_ADC_SAR1_INIT_CODE_MSB_MSB  0x3
#define I2C_SAR_ADC_SAR1_INIT_CODE_MSB_LSB  0x0

#define ADC_SAR1_SAMPLE_CYCLE_ADDR          0x2
#define ADC_SAR1_SAMPLE_CYCLE_ADDR_MSB      0x2
#define ADC_SAR1_SAMPLE_CYCLE_ADDR_LSB      0x0

#define ADC_SAR1_DREF_ADDR                  0x2
#define ADC_SAR1_DREF_ADDR_MSB              0x6
#define ADC_SAR1_DREF_ADDR_LSB              0x4

#define ADC_SAR2_INITIAL_CODE_LOW_ADDR      0x3
#define ADC_SAR2_INITIAL_CODE_LOW_ADDR_MSB  0x7
#define ADC_SAR2_INITIAL_CODE_LOW_ADDR_LSB  0x0

#define ADC_SAR2_INITIAL_CODE_HIGH_ADDR     0x4
#define ADC_SAR2_INITIAL_CODE_HIGH_ADDR_MSB 0x3
#define ADC_SAR2_INITIAL_CODE_HIGH_ADDR_LSB 0x0

#define ADC_SAR2_SAMPLE_CYCLE_ADDR          0x5
#define ADC_SAR2_SAMPLE_CYCLE_ADDR_MSB      0x2
#define ADC_SAR2_SAMPLE_CYCLE_ADDR_LSB      0x0

#define ADC_SAR2_DREF_ADDR                  0x5
#define ADC_SAR2_DREF_ADDR_MSB              0x6
#define ADC_SAR2_DREF_ADDR_LSB              0x4

#define I2C_SARADC_TSENS_DAC                0x6
#define I2C_SARADC_TSENS_DAC_MSB            0x3
#define I2C_SARADC_TSENS_DAC_LSB            0x0

#define ADC_SARADC_DTEST_RTC_ADDR           0x7
#define ADC_SARADC_DTEST_RTC_ADDR_MSB       0x1
#define ADC_SARADC_DTEST_RTC_ADDR_LSB       0x0

#define ADC_SARADC_ENT_TSENS_ADDR           0x7
#define ADC_SARADC_ENT_TSENS_ADDR_MSB       0x2
#define ADC_SARADC_ENT_TSENS_ADDR_LSB       0x2

#define ADC_SARADC1_EN_TOUT_SAR1_BUS_ADDR   0x7
#define ADC_SARADC1_EN_TOUT_SAR1_BUS_ADDR_MSB 0x4
#define ADC_SARADC1_EN_TOUT_SAR1_BUS_ADDR_LSB 0x4

#define ADC_SARADC2_EN_TOUT_SAR2_BUS_ADDR   0x7
#define ADC_SARADC2_EN_TOUT_SAR2_BUS_ADDR_MSB 0x5
#define ADC_SARADC2_EN_TOUT_SAR2_BUS_ADDR_LSB 0x5

#define ADC_SARADC_ENT_PERIF_ADDR           0x7
#define ADC_SARADC_ENT_PERIF_ADDR_MSB       0x6
#define ADC_SARADC_ENT_PERIF_ADDR_LSB       0x6

#define ADC_SARADC1_EN_TOUT_ADDR            0x8
#define ADC_SARADC1_EN_TOUT_ADDR_MSB        0x0
#define ADC_SARADC1_EN_TOUT_ADDR_LSB        0x0

#define ADC_SARADC2_EN_TOUT_ADDR            0x8
#define ADC_SARADC2_EN_TOUT_ADDR_MSB        0x2
#define ADC_SARADC2_EN_TOUT_ADDR_LSB        0x2

#define POWER_GLITCH_DREF_VDET_PERIF         11
#define POWER_GLITCH_DREF_VDET_PERIF_MSB     2
#define POWER_GLITCH_DREF_VDET_PERIF_LSB     0

#define POWER_GLITCH_DREF_VDET_VDDPST        11
#define POWER_GLITCH_DREF_VDET_VDDPST_MSB    6
#define POWER_GLITCH_DREF_VDET_VDDPST_LSB    4

#define POWER_GLITCH_DREF_VDET_XTAL          12
#define POWER_GLITCH_DREF_VDET_XTAL_MSB      2
#define POWER_GLITCH_DREF_VDET_XTAL_LSB      0

#define POWER_GLITCH_DREF_VDET_PLL           12
#define POWER_GLITCH_DREF_VDET_PLL_MSB       6
#define POWER_GLITCH_DREF_VDET_PLL_LSB       4

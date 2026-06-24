/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once

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

#define I2C_SARADC_DTEST                        0x7
#define I2C_SARADC_DTEST_MSB                    0x1
#define I2C_SARADC_DTEST_LSB                    0x0

#define I2C_SARADC_ENT_SAR                      0x7
#define I2C_SARADC_ENT_SAR_MSB                  0x3
#define I2C_SARADC_ENT_SAR_LSB                  0x2

#define I2C_SARADC_EN_TOUT_SAR1_BUS             0x7
#define I2C_SARADC_EN_TOUT_SAR1_BUS_MSB         0x5
#define I2C_SARADC_EN_TOUT_SAR1_BUS_LSB         0x5

#define I2C_SARADC_EN_TOUT_SAR2_BUS             0x7
#define I2C_SARADC_EN_TOUT_SAR2_BUS_MSB         0x6
#define I2C_SARADC_EN_TOUT_SAR2_BUS_LSB         0x6

#define ADC_SAR1_ENCAL_GND_ADDR                 0x8
#define ADC_SAR1_ENCAL_GND_ADDR_MSB             0x1
#define ADC_SAR1_ENCAL_GND_ADDR_LSB             0x1

#define I2C_SARADC_TSENS_DAC                    0x6
#define I2C_SARADC_TSENS_DAC_MSB                0x3
#define I2C_SARADC_TSENS_DAC_LSB                0x0

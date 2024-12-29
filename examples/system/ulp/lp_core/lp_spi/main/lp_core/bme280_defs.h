/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

// Contains only the subset of registers used in this example

#define BME280_CHIP_ID_REG              0xD0   // Chip ID Register

#define BME280_RESET_REG                0xE0   // Soft Reset Register

#define BME280_CTRL_HUM_REG             0xF2   // Humidity control
#define BME280_HUM_OVERSAMPLING_BIT     0      // Bit position for humidity oversampling
#define BME280_HUM_OVERSAMPLING_1X      0x1    // Value for 1x oversampling

#define BME280_STATUS_REG               0xF3   // Status Register
#define BME280_MEASURING_BIT            3      // Bit position for measuring in progress status

#define BME280_CTRL_MEAS_REG            0xF4   // Measurement Control Register
#define BME280_MODE_BIT                 0      // Bit position for mode
#define BME280_MODE_FORCED              0x1    // Value for setting forced mode
#define BME280_TEMP_OVERSAMPLING_BIT    5      // Bit position for temperature oversampling
#define BME280_TEMP_OVERSAMPLING_1X     0x1    // Value for 1x oversampling

#define BME280_CONFIG_REG               0xF5   // Configuration Register

#define BME280_TEMPERATURE_MSB_REG      0xFA   // Temperature data MSB
#define BME280_TEMPERATURE_LSB_REG      0xFB   // Temperature data LSB
#define BME280_TEMPERATURE_XLSB_REG     0xFC   // Temperature data XLSB
#define BME280_HUMIDITY_MSB_REG         0xFD   // Humidity data MSB
#define BME280_HUMIDITY_LSB_REG         0xFE   // Humidity data LSB

#define BME280_TRIM_PARAM_TEMP_1_REG    0x88    // Trimming Parameter T1
#define BME280_TRIM_PARAM_HUM_1_REG     0xA1    // Trimming Parameter H1
#define BME280_TRIM_PARAM_HUM_2_REG     0xE1    // Trimming Parameter H2

#define BME280_RESET_VAL                0xB6   // Write value to trigger a reset
#define BME280_CHIP_ID_VAL              0x60   // Chip ID

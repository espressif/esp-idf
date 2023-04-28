/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

/***************************************************
 * BMP180 Register Addresses
 ***************************************************/
#define BMP180_SENSOR_I2C_ADDR                  0x77
#define BMP180_SENSOR_REG_ADDR_WHO_AM_I         0xD0
#define BMP180_SENSOR_REG_ADDR_SOFT_RESET       0xE0
#define BMP180_SENSOR_REG_ADDR_AC1_MSB          0xAA
#define BMP180_SENSOR_REG_ADDR_AC1_LSB          0xAB
#define BMP180_SENSOR_REG_ADDR_AC2_MSB          0xAC
#define BMP180_SENSOR_REG_ADDR_AC2_LSB          0xAD
#define BMP180_SENSOR_REG_ADDR_AC3_MSB          0xAE
#define BMP180_SENSOR_REG_ADDR_AC3_LSB          0xAF
#define BMP180_SENSOR_REG_ADDR_AC4_MSB          0xB0
#define BMP180_SENSOR_REG_ADDR_AC4_LSB          0xB1
#define BMP180_SENSOR_REG_ADDR_AC5_MSB          0xB2
#define BMP180_SENSOR_REG_ADDR_AC5_LSB          0xB3
#define BMP180_SENSOR_REG_ADDR_AC6_MSB          0xB4
#define BMP180_SENSOR_REG_ADDR_AC6_LSB          0xB5
#define BMP180_SENSOR_REG_ADDR_B1_MSB           0xB6
#define BMP180_SENSOR_REG_ADDR_B1_LSB           0xB7
#define BMP180_SENSOR_REG_ADDR_B2_MSB           0xB8
#define BMP180_SENSOR_REG_ADDR_B2_LSB           0xB9
#define BMP180_SENSOR_REG_ADDR_MB_MSB           0xBA
#define BMP180_SENSOR_REG_ADDR_MB_LSB           0xBB
#define BMP180_SENSOR_REG_ADDR_MC_MSB           0xBC
#define BMP180_SENSOR_REG_ADDR_MC_LSB           0xBD
#define BMP180_SENSOR_REG_ADDR_MD_MSB           0xBE
#define BMP180_SENSOR_REG_ADDR_MD_LSB           0xBF
#define BMP180_SENSOR_REG_ADDR_CTRL_REG         0xF4
#define BMP180_SENSOR_REG_ADDR_SENSOR_DATA_MSB  0xF6
#define BMP180_SENSOR_REG_ADDR_SENSOR_DATA_LSB  0xF7
#define BMP180_SENSOR_REG_ADDR_SENSOR_DATA_XLSB 0xF8

/***************************************************
 * BMP180 Control Commands
 ***************************************************/
#define BMP180_SENSOR_CMD_READ_TEMPERATURE      0x2E
#define BMP180_SENSOR_CMD_READ_PRESSURE_OSS_0   0x34
#define BMP180_SENSOR_CMD_READ_PRESSURE_OSS_1   0x74
#define BMP180_SENSOR_CMD_READ_PRESSURE_OSS_2   0xB4
#define BMP180_SENSOR_CMD_READ_PRESSURE_OSS_3   0xF4
#define BMP180_SENSOR_CMD_SOFT_RESET            0xB6

/***************************************************
 * BMP180 Chip ID
 ***************************************************/
#define BMP180_SENSOR_CHIP_ID                   0x55

/***************************************************
 * BMP180 Calibration Data
 ***************************************************/
typedef struct {
    int16_t  ac1;
    int16_t  ac2;
    int16_t  ac3;
    uint16_t ac4;
    uint16_t ac5;
    uint16_t ac6;
    int16_t  b1;
    int16_t  b2;
    int16_t  mb;
    int16_t  mc;
    int16_t  md;
} bmp180_cal_data_t;

bmp180_cal_data_t bmp180_cal_data;

/***************************************************
 * BMP180 Oversampling setting to measure pressure
 ***************************************************/
typedef enum {
    OSS_0 = 0,  // Ultra low power
    OSS_1 = 1,  // Standard
    OSS_2 = 2,  // High resolution
    OSS_3 = 3   // Ultra high resolution
} oss_mode_t;

/***************************************************
 * BMP180 Interaction APIs
 ***************************************************/
static void bmp180_read_cal_data(void);                                 // Read cal data
static void bmp180_read_ut_data(int16_t *ut_data);                      // Read uncompensated temperature
static void bmp180_read_up_data(int32_t *up_data, oss_mode_t oss_mode); // Read uncompensated pressure

/************************************************
 * BMP180 Utility APIs
 ************************************************/
static void bmp180_read16(uint16_t *data_out, uint32_t reg_msb, uint32_t reg_lsb);
static int32_t bmp180_calculate_real_temp(int32_t ut_data);
static int32_t bmp180_calculate_real_pressure(int32_t up_data, int32_t ut_data, oss_mode_t oss_mode);

/************************************************
 * Pressure measurement mode
 ************************************************/
#define EXAMPLE_OSS_MODE        OSS_0

/************************************************
 * Temperature and Pressure thresholds (uncompensated) to wake up Main CPU
 * The threshold values have been selected for demo purposes and may not
 * represent real world use case.
 ************************************************/
#define EXAMPLE_UT_THRESHOLD    25000
#define EXAMPLE_UP_THRESHOLD    50000

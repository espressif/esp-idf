/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

/* Device Information */
#define LSM6DSOX_STATIC_ADDRESS     0x6A
#define LSM6DSOX_WHO_AM_I_VALUE     0x6C

/* Register Addresses */
#define LSM6DSOX_FUNC_CFG_ACCESS    0X01
#define LSM6DSOX_INT1               0X0D
#define LSM6DSOX_INT2               0X0E
#define LSM6DSOX_STATUS_REG         0X1E
#define LSM6DSOX_WHO_AM_I_REG       0X0F
#define LSM6DSOX_CTRL1_XL           0X10
#define LSM6DSOX_CTRL2_G            0X11
#define LSM6DSOX_CTRL3_C            0X12
#define LSM6DSOX_CTRL4_C            0X13
#define LSM6DSOX_CTRL5_C            0X14
#define LSM6DSOX_CTRL6_C            0X15
#define LSM6DSOX_CTRL7_G            0X16
#define LSM6DSOX_CTRL8_XL           0X17
#define LSM6DSOX_CTRL9_XL           0X18
#define LSM6DSOX_CTRL10_C           0X19
#define LSM6DSOX_INT_SRC            0X1A
#define LSM6DSOX_TAP_SRC            0X1C
#define LSM6DSOX_OUT_TEMP_L         0X20
#define LSM6DSOX_OUT_TEMP_H         0X21
#define LSM6DSOX_OUTX_L_G           0X22
#define LSM6DSOX_OUTX_H_G           0X23
#define LSM6DSOX_OUTY_L_G           0X24
#define LSM6DSOX_OUTY_H_G           0X25
#define LSM6DSOX_OUTZ_L_G           0X26
#define LSM6DSOX_OUTZ_H_G           0X27
#define LSM6DSOX_OUTX_L_XL          0X28
#define LSM6DSOX_OUTX_H_XL          0X29
#define LSM6DSOX_OUTY_L_XL          0X2A
#define LSM6DSOX_OUTY_H_XL          0X2B
#define LSM6DSOX_OUTZ_L_XL          0X2C
#define LSM6DSOX_OUTZ_H_XL          0X2D
#define LSM6DSOX_TAP_CFG0           0X56
#define LSM6DSOX_TAP_CFG1           0X57
#define LSM6DSOX_TAP_CFG2           0X58
#define LSM6DSOX_TAP_THS_6D         0X59
#define LSM6DSOX_INT_DUR2           0X5A
#define LSM6DSOX_WAKE_UP_THS        0X5B
#define LSM6DSOX_WAKE_UP_DUR        0X5C
#define LSM6DSOX_FREE_FALL          0X5D
#define LSM6DSOX_MD1_CFG            0X5E
#define LSM6DSOX_MD2_CFG            0X5F
#define LSM6DSOX_I3C_BUS_AVB        0X62

/* Register Configuration Values */
#define LSM6DSOX_CTRL2_G_ODR_104HZ_2000DPS  0x7C  // Gyroscope: 104Hz, 2000dps
#define LSM6DSOX_CTRL1_XL_ODR_104HZ_4G      0x3C  // Accelerometer: 104Hz, 4g
#define LSM6DSOX_CTRL3_C_DEFAULT            0x04  // Default control register 3
#define LSM6DSOX_CTRL9_XL_DEFAULT           0x70  // Default control register 9
#define LSM6DSOX_TAP_CFG0_XYZ_EN            0x0E  // Enable X,Y,Z axes for tap detection
#define LSM6DSOX_TAP_CFG1_XYZ_EN            0x0F  // Enable X,Y,Z axes for tap detection
#define LSM6DSOX_TAP_CFG2_XYZ_EN            0xFF  // Enable X,Y,Z axes for tap detection
#define LSM6DSOX_TAP_THS_DEFAULT            0x02  // Tap threshold (~750mg @ 2g FS)
#define LSM6DSOX_INT_DUR2_DEFAULT           0x1F  // Tap time window
#define LSM6DSOX_MD1_CFG_TAP_EN             0x48  // Enable single tap interrupt on INT1
#define LSM6DSOX_I3C_BUS_AVB_DEFAULT        0x18  // Default I3C bus available configuration
#define ENEC_IBI                            0x0B  // Enable IBI
#define SETMRL_DATA                         0x0A1000    // Maximum read length configuration
#define LSM6DSOX_BCR_VALUE                  0x00  // Bus Characteristics Register
#define LSM6DSOX_DCR_VALUE                  0x00  // Device Characteristics Register
#define LSM6DSOX_REG_DEFAULT_VALUE          0x00  // Default register value
#define LSM6DSOX_DYNAMIC_ADDR               0x09  // Dynamic address assigned to LSM6DSOX

/* TAP_SRC Register Bit Definitions */
#define LSM6DSOX_TAP_SRC_X_TAP              (1 << 0)  // X-axis tap detected
#define LSM6DSOX_TAP_SRC_Y_TAP              (1 << 1)  // Y-axis tap detected
#define LSM6DSOX_TAP_SRC_Z_TAP              (1 << 2)  // Z-axis tap detected
#define LSM6DSOX_TAP_SRC_TAP_SIGN           (1 << 3)  // Tap sign (direction)
#define LSM6DSOX_TAP_SRC_DOUBLE_TAP         (1 << 4)  // Double tap detected
#define LSM6DSOX_TAP_SRC_SINGLE_TAP         (1 << 5)  // Single tap detected
#define LSM6DSOX_TAP_SRC_TAP_IA             (1 << 6)  // Tap interrupt active

/* Application Configuration */
#define LSM6DSOX_TEMP_CONVERSION_FACTOR     256.0f
#define LSM6DSOX_TEMP_OFFSET_CELSIUS        25.0f

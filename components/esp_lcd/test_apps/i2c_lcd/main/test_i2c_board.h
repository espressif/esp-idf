/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define TEST_LCD_H_RES          128
#define TEST_LCD_V_RES          64

#define TEST_I2C_SDA_GPIO       0
#define TEST_I2C_SCL_GPIO       2

#define TEST_I2C_HOST_ID        0

#define TEST_I2C_DEV_ADDR       0x3C

#define TEST_LCD_PIXEL_CLOCK_HZ (400 * 1000)

#ifdef __cplusplus
}
#endif

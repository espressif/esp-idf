/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_IDF_TARGET_ESP32
#define EXAMPLE_I2S_BCLK_IO1        GPIO_NUM_4      // I2S bit clock io number
#define EXAMPLE_I2S_WS_IO1          GPIO_NUM_5      // I2S word select io number
#define EXAMPLE_I2S_DOUT_IO1        GPIO_NUM_18     // I2S data out io number
#define EXAMPLE_I2S_DIN_IO1         GPIO_NUM_19     // I2S data in io number
// STD simplex pins
#define EXAMPLE_I2S_BCLK_IO2        GPIO_NUM_22     // I2S bit clock io number
#define EXAMPLE_I2S_WS_IO2          GPIO_NUM_23     // I2S word select io number
#define EXAMPLE_I2S_DOUT_IO2        GPIO_NUM_25     // I2S data out io number
#define EXAMPLE_I2S_DIN_IO2         GPIO_NUM_26     // I2S data in io number

#elif CONFIG_IDF_TARGET_ESP32P4
#define EXAMPLE_I2S_BCLK_IO1        GPIO_NUM_45     // I2S bit clock io number
#define EXAMPLE_I2S_WS_IO1          GPIO_NUM_46     // I2S word select io number
#define EXAMPLE_I2S_DOUT_IO1        GPIO_NUM_47     // I2S data out io number
#define EXAMPLE_I2S_DIN_IO1         GPIO_NUM_48     // I2S data in io number
// PDM RX 4 line IO
#define EXAMPLE_I2S_DIN1_IO1        GPIO_NUM_20     // I2S data in io number
#define EXAMPLE_I2S_DIN2_IO1        GPIO_NUM_21     // I2S data in io number
#define EXAMPLE_I2S_DIN3_IO1        GPIO_NUM_22     // I2S data in io number
// STD/TDM simplex pins
#define EXAMPLE_I2S_BCLK_IO2        GPIO_NUM_20     // I2S bit clock io number
#define EXAMPLE_I2S_WS_IO2          GPIO_NUM_21     // I2S word select io number
#define EXAMPLE_I2S_DOUT_IO2        GPIO_NUM_22     // I2S data out io number
#define EXAMPLE_I2S_DIN_IO2         GPIO_NUM_23     // I2S data in io number

#elif CONFIG_IDF_TARGET_ESP32S3
#define EXAMPLE_I2S_BCLK_IO1        GPIO_NUM_2      // I2S bit clock io number
#define EXAMPLE_I2S_WS_IO1          GPIO_NUM_3      // I2S word select io number
#define EXAMPLE_I2S_DOUT_IO1        GPIO_NUM_4      // I2S data out io number
#define EXAMPLE_I2S_DIN_IO1         GPIO_NUM_5      // I2S data in io number
// PDM RX 4 line IO
#define EXAMPLE_I2S_DIN1_IO1        GPIO_NUM_6     // I2S data in io number
#define EXAMPLE_I2S_DIN2_IO1        GPIO_NUM_7     // I2S data in io number
#define EXAMPLE_I2S_DIN3_IO1        GPIO_NUM_8     // I2S data in io number
// STD/TDM simplex pins
#define EXAMPLE_I2S_BCLK_IO2        GPIO_NUM_6      // I2S bit clock io number
#define EXAMPLE_I2S_WS_IO2          GPIO_NUM_7      // I2S word select io number
#define EXAMPLE_I2S_DOUT_IO2        GPIO_NUM_8      // I2S data out io number
#define EXAMPLE_I2S_DIN_IO2         GPIO_NUM_9      // I2S data in io number

#else
#define EXAMPLE_I2S_BCLK_IO1        GPIO_NUM_2      // I2S bit clock io number
#define EXAMPLE_I2S_WS_IO1          GPIO_NUM_3      // I2S word select io number
#define EXAMPLE_I2S_DOUT_IO1        GPIO_NUM_4      // I2S data out io number
#define EXAMPLE_I2S_DIN_IO1         GPIO_NUM_5      // I2S data in io number

#define EXAMPLE_I2S_BCLK_IO2        GPIO_NUM_6      // I2S bit clock io number
#define EXAMPLE_I2S_WS_IO2          GPIO_NUM_7      // I2S word select io number
#define EXAMPLE_I2S_DOUT_IO2        GPIO_NUM_8      // I2S data out io number
#define EXAMPLE_I2S_DIN_IO2         GPIO_NUM_9      // I2S data in io number

#endif

#ifdef __cplusplus
}
#endif
